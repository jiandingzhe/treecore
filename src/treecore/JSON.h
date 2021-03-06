/*
  ==============================================================================

   This file is part of the juce_core module of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission to use, copy, modify, and/or distribute this software for any purpose with
   or without fee is hereby granted, provided that the above copyright notice and this
   permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ------------------------------------------------------------------------------

   NOTE! This permissive ISC license applies ONLY to files within the juce_core module!
   All other JUCE modules are covered by a dual GPL/commercial license, so if you are
   using any other modules, be sure to check that you also comply with their license.

   For more details, visit www.juce.com

  ==============================================================================
*/

#ifndef TREECORE_JSON_H
#define TREECORE_JSON_H

#include "treecore/Array.h"
#include "treecore/String.h"

namespace treecore {

class File;
class InputStream;
class OutputStream;
class Result;
class StringRef;
class var;

//==============================================================================
/**
    Contains static methods for converting JSON-formatted text to and from var objects.

    The var class is structurally compatible with JSON-formatted data, so these
    functions allow you to parse JSON into a var object, and to convert a var
    object to JSON-formatted text.

    @see var
*/
class TREECORE_SHARED_API  JSON
{
public:
    //==============================================================================
    /** Parses a string of JSON-formatted text, and returns a result code containing
        any parse errors.

        This will return the parsed structure in the parsedResult parameter, and will
        return a Result object to indicate whether parsing was successful, and if not,
        it will contain an error message.

        If you're not interested in the error message, you can use one of the other
        shortcut parse methods, which simply return a var::null if the parsing fails.

        Note that this will only parse valid JSON, which means that the item given must
        be either an object or an array definition. If you want to also be able to parse
        any kind of primitive JSON object, use the fromString() method.
    */
    static Result parse (const String& text, var& parsedResult);

    /** Attempts to parse some JSON-formatted text, and returns the result as a var object.

        If the parsing fails, this simply returns var::null - if you need to find out more
        detail about the parse error, use the alternative parse() method which returns a Result.

        Note that this will only parse valid JSON, which means that the item given must
        be either an object or an array definition. If you want to also be able to parse
        any kind of primitive JSON object, use the fromString() method.
    */
    static var parse (const String& text);

    /** Attempts to parse some JSON-formatted text from a file, and returns the result
        as a var object.

        Note that this is just a short-cut for reading the entire file into a string and
        parsing the result.

        If the parsing fails, this simply returns var::null - if you need to find out more
        detail about the parse error, use the alternative parse() method which returns a Result.
    */
    static var parse (const File& file);

    /** Attempts to parse some JSON-formatted text from a stream, and returns the result
        as a var object.

        Note that this is just a short-cut for reading the entire stream into a string and
        parsing the result.

        If the parsing fails, this simply returns var::null - if you need to find out more
        detail about the parse error, use the alternative parse() method which returns a Result.
    */
    static var parse (InputStream& input);

    //==============================================================================
    /** Returns a string which contains a JSON-formatted representation of the var object.
        If allOnOneLine is true, the result will be compacted into a single line of text
        with no carriage-returns. If false, it will be laid-out in a more human-readable format.
        @see writeToStream
    */
    static String toString (const var& objectToFormat,
                            bool allOnOneLine = false);

    /** Parses a string that was created with the toString() method.
        This is slightly different to the parse() methods because they will reject primitive
        values and only accept array or object definitions, whereas this method will handle
        either.
    */
    static var fromString (StringRef);

    /** Writes a JSON-formatted representation of the var object to the given stream.
        If allOnOneLine is true, the result will be compacted into a single line of text
        with no carriage-returns. If false, it will be laid-out in a more human-readable format.
        @see toString
    */
    static void writeToStream (OutputStream& output,
                               const var& objectToFormat,
                               bool allOnOneLine = false);

    /** Returns a version of a string with any extended characters escaped. */
    static String escapeString (StringRef);

    /** Parses a quoted string-literal in JSON format, returning the un-escaped result in the
        result parameter, and an error message in case the content was illegal.
        This advances the text parameter, leaving it positioned after the closing quote.
    */
    static Result parseQuotedString (String::CharPointerType& text, var& result);

private:
    //==============================================================================
    JSON() = delete; // This class can't be instantiated - just use its static methods.
};

class JSONParser
{
public:
    static Result parseObjectOrArray (String::CharPointerType t, var& result);

    static Result parseString (const treecore_wchar quoteChar, String::CharPointerType& t, var& result);

    static Result parseAny (String::CharPointerType& t, var& result);

private:
    static Result createFail (const char* const message, const String::CharPointerType* location = nullptr);

    static Result parseNumber (String::CharPointerType& t, var& result, const bool isNegative);

    static Result parseObject (String::CharPointerType& t, var& result);

    static Result parseArray (String::CharPointerType& t, var& result);
};

//==============================================================================
class JSONFormatter
{
public:
    static void write (OutputStream& out, const var& v,
                       const int indentLevel, const bool allOnOneLine);

    static void writeEscapedChar (OutputStream& out, const unsigned short value);

    static void writeString (OutputStream& out, String::CharPointerType t);

    static void writeSpaces (OutputStream& out, int numSpaces);

    static void writeArray (OutputStream& out, const Array<var>& array,
                            const int indentLevel, const bool allOnOneLine);

    enum { indentSize = 2 };
};

}

#endif   // TREECORE_JSON_H
