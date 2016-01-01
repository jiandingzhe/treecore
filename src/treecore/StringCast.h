#ifndef TREECORE_STRING_CAST_H
#define TREECORE_STRING_CAST_H

#include "treecore/String.h"

#include <cstdlib>

namespace treecore
{

template<typename T>
bool fromString( const String& str, T& result_value );

template<typename T>
String toString( T value );

template<>
inline bool fromString<short>( const String& str, short& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtol( data, &end, 0 );
    return data != end;
}

template<>
inline bool fromString<unsigned short>( const String& str, unsigned short& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtoul( data, &end, 0 );
    return data != end;
}

template<>
inline bool fromString<int>( const String& str, int& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtol( data, &end, 0 );
    return data != end;
}

template<>
inline bool fromString<unsigned int>( const String& str, unsigned int& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtoul( data, &end, 0 );
    return data != end;
}

template<>
inline bool fromString<long>( const String& str, long& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtol( data, &end, 0 );
    return data != end;
}

template<>
inline bool fromString<unsigned long>( const String& str, unsigned long& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtoul( data, &end, 0 );
    return data != end;
}

template<>
inline bool fromString<long long>( const String& str, long long& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtoll( data, &end, 0 );
    return data != end;
}

template<>
inline bool fromString<unsigned long long>( const String& str, unsigned long long& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtoull( data, &end, 0 );
    return data != end;
}

template<>
inline bool fromString<float>( const String& str, float& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtof( data, &end );
    return data != end;
}

template<>
inline bool fromString<double>( const String& str, double& result )
{
    char* data = str.getCharPointer().getAddress();
    char* end  = data;
    result = std::strtod( data, &end );
    return data != end;
}

template<>
inline String toString<int16>( int16 value ) { return String( value ); }

template<>
inline String toString<uint16>( uint16 value ) { return String( value ); }

template<>
inline String toString<int32>( int32 value ) { return String( value ); }

template<>
inline String toString<uint32>( uint32 value ) { return String( value ); }

template<>
inline String toString<int64>( int64 value ) { return String( value ); }

template<>
inline String toString<uint64>( uint64 value ) { return String( value ); }

template<>
inline String toString<float>( float value ) { return String( value ); }

template<>
inline String toString<double>( double value ) { return String( value ); }

} // namespace treecore

#endif // TREECORE_STRING_CAST_H
