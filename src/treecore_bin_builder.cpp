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
#include "treecore/ScopedPointer.h"
#include "treecore/StringRef.h"
#include "treecore/Time.h"

#include <iostream>

using namespace treecore;

String name_dir_in;
String name_dir_out;
String out_class_name;
String wild_card;
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

Option opt_update_only( "update", 'u', "Input/Output",
                        &update_only, Option::FLAG_NONE,
                        "Generate output only when output is outdate." );

Option opt_quiet( "quiet", 'q', "Misc",
                  &quiet, Option::FLAG_NONE,
                  "Do not show information." );

Option opt_show_help( "help", 'h', "Misc",
                      &show_help, Option::FLAG_NONE,
                      "Show help." );

//==============================================================================
static size_t addFile( const File&   file,
                       const String& classname,
                       OutputStream& headerStream,
                       OutputStream& cppStream )
{
    MemoryBlock mb;
    file.loadFileAsData( mb );

    const String name( file.getFileName().toLowerCase()
                       .replaceCharacter( ' ', '_' )
                       .replaceCharacter( '.', '_' )
                       .retainCharacters( "abcdefghijklmnopqrstuvwxyz_0123456789" ) );

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

static bool isHiddenFile( const File& f, const File& root )
{
    return f.getFileName().endsWithIgnoreCase( ".scc" )
           || f.getFileName() == ".svn"
           || f.getFileName().startsWithChar( '.' )
           || ( f.getSize() == 0 && !f.isDirectory() )
           || ( f.getParentDirectory() != root && isHiddenFile( f.getParentDirectory(), root ) );
}

//==============================================================================
int main( int argc, char const** argv )
{
    OptionParser parser;
    parser.add_option( opt_dir_in );
    parser.add_option( opt_dir_out );
    parser.add_option( opt_out_class_name );
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

    const File sourceDirectory = File::getCurrentWorkingDirectory().getChildFile( name_dir_in );

    if ( !sourceDirectory.isDirectory() )
    {
        std::cerr << "Source directory doesn't exist: "
                  << sourceDirectory.getFullPathName()
                  << std::endl << std::endl;

        exit( EXIT_FAILURE );
    }

    const File destDirectory = File::getCurrentWorkingDirectory().getChildFile( name_dir_out );

    if ( !destDirectory.isDirectory() )
    {
        std::cerr << "Destination directory doesn't exist: "
                  << destDirectory.getFullPathName() << std::endl << std::endl;

        exit( EXIT_FAILURE );
    }

    String className = out_class_name.trim();

    const File headerFile( destDirectory.getChildFile( className ).withFileExtension( ".h" ) );
    const File cppFile( destDirectory.getChildFile( className ).withFileExtension( ".cpp" ) );

    if (!quiet)
        Logger::writeToLog( "Creating " + headerFile.getFullPathName() + " and " + cppFile.getFullPathName() + " from files in " + sourceDirectory.getFullPathName() + "..." );

    Array<File> input_files;
    if (wild_card.length() > 0)
        sourceDirectory.findChildFiles( input_files, File::findFiles, true, wild_card );
    else
        sourceDirectory.findChildFiles( input_files, File::findFiles, true, "*" );

    if (input_files.size() == 0)
    {
        std::cerr << "Didn't find any source files in: "
                  << sourceDirectory.getFullPathName() << std::endl << std::endl;
        exit( EXIT_FAILURE );
    }

    //
    // validate time stamp
    // skip if output is already up-to-date
    //
    if ( update_only && headerFile.existsAsFile() && cppFile.existsAsFile() )
    {
        // collect time of the oldest output file
        int64 output_oldest = std::min( headerFile.getCreationTime().toMilliseconds(), cppFile.getCreationTime().toMilliseconds() );

        // collect time of the newest input file
        int64 input_newest = 0;
        for (const File& input_file : input_files)
        {
            int64 curr_file_time = input_file.getLastModificationTime().toMilliseconds();
            if (curr_file_time > input_newest) input_newest = curr_file_time;
        }

        if (input_newest < output_oldest)
        {
            if (!quiet)
                Logger::writeToLog( "newest input time " + String( input_newest ) + " is older than oldest output time " + String( output_oldest ) + ", do nothing..." );
            exit( 0 );
        }
    }

    //
    // generate output
    //
    headerFile.deleteFile();
    cppFile.deleteFile();

    ScopedPointer<OutputStream> header( headerFile.createOutputStream() );

    if (header == 0)
    {
        std::cerr << "Couldn't open " << headerFile.getFullPathName() << " for writing" << std::endl << std::endl;
        exit( EXIT_FAILURE );
    }

    ScopedPointer<OutputStream> cpp( cppFile.createOutputStream() );

    if (cpp == 0)
    {
        std::cerr << "Couldn't open " << cppFile.getFullPathName() << " for writing" << std::endl << std::endl;
        exit( EXIT_FAILURE );
    }

    *header << "/* (Auto-generated binary data file). */\r\n\r\n"
        "#ifndef BINARY_" << className.toUpperCase() << "_H\r\n"
        "#define BINARY_" << className.toUpperCase() << "_H\r\n\r\n"
        "namespace " << className << "\r\n"
        "{\r\n";

    *cpp << "/* (Auto-generated binary data file). */\r\n\r\n"
        "#include \"" << className << ".h\"\r\n\r\n";

    size_t totalBytes = 0;

    for (int i = 0; i < input_files.size(); ++i)
    {
        const File file( input_files[i] );

        // (avoid source control files and hidden files..)
        if ( !isHiddenFile( file, sourceDirectory ) )
        {
            if (file.getParentDirectory() != sourceDirectory)
            {
                *header << "  #ifdef " << file.getParentDirectory().getFileName().toUpperCase() << "\r\n";
                *cpp << "#ifdef " << file.getParentDirectory().getFileName().toUpperCase() << "\r\n";

                totalBytes += addFile( file, className, *header, *cpp );

                *header << "  #endif\r\n";
                *cpp << "#endif\r\n";
            }
            else
            {
                totalBytes += addFile( file, className, *header, *cpp );
            }
        }
    }

    *header << "}\r\n\r\n"
        "#endif\r\n";

    header = 0;
    cpp    = 0;

    if (!quiet)
        Logger::writeToLog( "Total size of binary data: " + String( totalBytes ) + " bytes" );
}
