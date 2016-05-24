/*
   ==============================================================================

   Utility to turn a bunch of binary files into a .cpp file and .h file full of
   data so they can be built directly into an executable.

   Use this code at your own risk! It carries no warranty!

   ==============================================================================
 */

#include "treecore/File.h"
#include "treecore/FileOutputStream.h"
#include "treecore/MemoryBlock.h"
#include "treecore/OptionParser.h"
#include "treecore/OutputStream.h"
#include "treecore/RefCountHolder.h"
#include "treecore/ScopedPointer.h"
#include "treecore/StringPool.h"
#include "treecore/StringRef.h"
#include "treecore/Time.h"

#include <iostream>

using namespace treecore;

String name_dir_in;
String name_dir_out;
String out_class_name;
String wild_card;
bool dry_run;
bool update_only;
bool quiet;
bool show_help;

Option opt_dir_in( "in", 'i', "Input/Output",
                   &name_dir_in, Option::FLAG_NONE,
                   "All files in this directory will be wrapped.", "PATH" );

Option opt_dir_out( "out", 'o', "Input/Output",
                    &name_dir_out, Option::FLAG_NONE,
                    "Source code will be generated in this directory.", "PATH" );

Option opt_out_class_name( "name", 'n', "Input/Output",
                           &out_class_name, Option::FLAG_NONE,
                           "Output class name and file prefix", "STRING" );

Option opt_wild_card( "wildcard", 'w', "Input/Output",
                      &wild_card, Option::FLAG_NONE,
                      "Only files matching this pattern will be used. Leave empty for all files", "STRING" );

Option opt_dry_run( "dry-run", 'D', "Input/Output",
                    &dry_run, Option::FLAG_NONE,
                    "Do not acually execute, only write a list of output files" );

Option opt_update_only( "update", 'u', "Input/Output",
                        &update_only, Option::FLAG_NONE,
                        "Generate output only when output is outdate." );

Option opt_quiet( "quiet", 'q', "Misc",
                  &quiet, Option::FLAG_NONE,
                  "Do not show information." );

Option opt_show_help( "help", 'h', "Misc",
                      &show_help, Option::FLAG_NONE,
                      "Show help." );

String output_name_from_file( const File& input_file )
{
    return input_file.getFileName().toLowerCase()
           .replaceCharacter( ' ', '_' )
           .replaceCharacter( '.', '_' )
           .retainCharacters( "abcdefghijklmnopqrstuvwxyz_0123456789" );
}

static bool isHiddenFile( const File& f, const File& root )
{
    return f.getFileName().endsWithIgnoreCase( ".scc" )
           || f.getFileName() == ".svn"
           || f.getFileName().startsWithChar( '.' )
           || ( f.getSize() == 0 && !f.isDirectory() )
           || ( f.getParentDirectory() != root && isHiddenFile( f.getParentDirectory(), root ) );
}

void collect_input_files( const File& dir_in, Array<File>& files_in )
{
    Array<File> tmp_files_in;
    if (wild_card.length() > 0)
        dir_in.findChildFiles( tmp_files_in, File::findFiles, true, wild_card );
    else
        dir_in.findChildFiles( tmp_files_in, File::findFiles, true, "*" );

    for (const File& f : tmp_files_in)
    {
        if ( !isHiddenFile( f, dir_in ) ) files_in.add( f );
    }
}

static size_t addFile( const File&   file,
                       const String& classname,
                       OutputStream& headerStream,
                       OutputStream& cppStream )
{
    MemoryBlock mb;
    file.loadFileAsData( mb );

    const String name = output_name_from_file( file );

    std::cout << "Adding " << name << ": "
              << (int) mb.getSize() << " bytes" << std::endl;

    headerStream << "    extern const char*  " << name << ";\r\n"
        "    const int           " << name << "Size = "
                 << (int) mb.getSize() << ";\r\n\r\n";

    static int tempNum = 0;

    cppStream << "static const unsigned char temp" << ++tempNum << "[] = {";

    size_t i = 0;
    const uint8* const data = (const uint8*) mb.getData();

    while (i < mb.getSize() - 1)
    {
        if ( (i % 40) != 39 )
            cppStream << (int) data[i] << ",";
        else
            cppStream << (int) data[i] << ",\r\n  ";

        ++i;
    }

    cppStream << (int) data[i] << ",0,0};\r\n";

    cppStream << "const char* " << classname << "::" << name
              << " = (const char*) temp" << tempNum << ";\r\n\r\n";

    return mb.getSize();
}

File output_file_from_input( const File& input_file, const File& dir_out )
{
    return dir_out.getChildFile(  input_file.getFileName() + ".cpp" );
}

OutputStream* open_output_src_file_handle( const File& f_out )
{
    if (!quiet && !dry_run)
        Logger::writeToLog( "create output file " + f_out.getFullPathName() );

    FileOutputStream* fh = f_out.createOutputStream();

    if (fh == nullptr)
    {
        std::cerr << "Couldn't open " << f_out.getFullPathName() << " for writing" << std::endl << std::endl;
        exit( EXIT_FAILURE );
    }

    *fh << "/* (Auto-generated binary data file). */\r\n\r\n"
        "#include \"" << out_class_name << ".h\"\r\n\r\n";

    return fh;
}

int main( int argc, char const** argv )
{
    OptionParser parser;
    parser.add_option( opt_dir_in );
    parser.add_option( opt_dir_out );
    parser.add_option( opt_out_class_name );
    parser.add_option( opt_wild_card );
    parser.add_option( opt_dry_run  );
    parser.add_option( opt_update_only );
    parser.add_option( opt_quiet );
    parser.add_option( opt_show_help );

    parser.parse_options( argc, argv );

    if (show_help)
    {
        Logger::writeToLog( "" );
        Logger::writeToLog( String( argv[0] ) + " - make binary resources to C++ source code" );
        Logger::writeToLog( "" );
        Logger::writeToLog( parser.format_document() );
        exit( 0 );
    }

    const File dir_in = File::isAbsolutePath( name_dir_in )
                        ? File( name_dir_in )
                        : File::getCurrentWorkingDirectory().getChildFile( name_dir_in );

    if ( !dir_in.isDirectory() )
    {
        std::cerr << "Source directory doesn't exist: "
                  << dir_in.getFullPathName()
                  << std::endl << std::endl;

        exit( EXIT_FAILURE );
    }

    const File dir_out = File::isAbsolutePath( name_dir_out )
                         ? File( name_dir_out )
                         : File::getCurrentWorkingDirectory().getChildFile( name_dir_out );

    if ( !dir_out.isDirectory() )
    {
        std::cerr << "Destination directory doesn't exist: "
                  << dir_out.getFullPathName() << std::endl << std::endl;

        exit( EXIT_FAILURE );
    }

    out_class_name = out_class_name.trim();
    if (out_class_name.length() == 0)
    {
        std::cerr << "Output class name not specified!" << std::endl;
        exit( EXIT_FAILURE );
    }

    const File headerFile( dir_out.getChildFile( out_class_name ).withFileExtension( ".h" ) );

    if (!quiet && !dry_run)
        Logger::writeToLog( "Creating " + headerFile.getFullPathName() + " from files in " + dir_in.getFullPathName() + "..." );

    //
    // collect input files
    //
    Array<File> files_in;
    collect_input_files( dir_in, files_in );

    if (files_in.size() == 0)
    {
        std::cerr << "Didn't find any source files in: "
                  << dir_in.getFullPathName() << std::endl << std::endl;
        exit( EXIT_FAILURE );
    }

    //
    // generate output file list from input files
    //
    bool all_output_exist = true;
    Array<File> files_out;
    for (const File& file_in : files_in)
    {
        File file_out = output_file_from_input( file_in, dir_out );
        if ( files_out.contains( file_out ) )
        {
            std::cerr << "input file " << file_in.getFullPathName() << " would generate non-unique output file" << std::endl;
            exit( EXIT_FAILURE );
        }

        files_out.add( file_out );
        if ( !file_out.existsAsFile() ) all_output_exist = false;
    }

    //
    // validate time stamp
    // skip if output is already up-to-date
    //
    if (update_only && headerFile.existsAsFile() && dir_out.exists() && all_output_exist)
    {
        // collect time of the oldest output file
        int64 output_oldest = headerFile.getCreationTime().toMilliseconds();
        for (const File& f : files_out)
            output_oldest = std::min( output_oldest, f.getCreationTime().toMilliseconds() );

        // collect time of the newest input file
        int64 input_newest = 0;
        for (const File& input_file : files_in)
        {
            int64 curr_file_time = input_file.getLastModificationTime().toMilliseconds();
            if (curr_file_time > input_newest) input_newest = curr_file_time;
        }

        if (input_newest < output_oldest)
        {
            if (!quiet && !dry_run)
                Logger::writeToLog( "newest input time " + String( input_newest ) + " is older than oldest output time " + String( output_oldest ) + ", do nothing..." );
            exit( 0 );
        }
    }

    //
    // generate output
    //
    headerFile.deleteFile();
    for (const File& f : files_out)
        f.deleteFile();

    // open header
    ScopedPointer<OutputStream> fh_header( headerFile.createOutputStream() );

    if (fh_header == 0)
    {
        std::cerr << "Couldn't open " << headerFile.getFullPathName() << " for writing" << std::endl << std::endl;
        exit( EXIT_FAILURE );
    }

    *fh_header << "/* (Auto-generated binary data file). */\r\n\r\n"
        "#ifndef BINARY_" << out_class_name.toUpperCase() << "_H\r\n"
        "#define BINARY_" << out_class_name.toUpperCase() << "_H\r\n\r\n"
        "namespace " << out_class_name << "\r\n"
        "{\r\n";

    // traverse input files
    size_t totalBytes = 0;

    for (int i = 0; i < files_in.size(); ++i)
    {
        const File& file_in  = files_in[i];
        const File& file_out = files_out[i];

        if (dry_run)
        {
            std::cout << file_out.getFullPathName();
            if (i != files_in.size() - 1)
                std::cout << ";";
            else
                std::cout << std::endl;
        }
        else
        {
            RefCountHolder<OutputStream> fh_out = open_output_src_file_handle( file_out );
            totalBytes += addFile( file_in, out_class_name, *fh_header, *fh_out );
        }
    }

    *fh_header << "}\r\n\r\n"
        "#endif\r\n";

    if (!quiet && !dry_run)
        Logger::writeToLog( "Total size of binary data: " + String( totalBytes ) + " bytes" );

    // clear stuffs to avoid some messages
    StringPool::releaseInstance();
}

