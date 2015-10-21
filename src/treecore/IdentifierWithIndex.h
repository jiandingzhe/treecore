#ifndef TREECORE_IDENTIFIER_WITH_INDEX
#define TREECORE_IDENTIFIER_WITH_INDEX

#include "treecore/Identifier.h"

namespace treecore
{

class IdentifierWithIndex
{
public:
    IdentifierWithIndex() noexcept;

    IdentifierWithIndex( const char* name , uint index = 0 );

    IdentifierWithIndex( const String& name , uint index = 0 );

    IdentifierWithIndex( String::CharPointerType nameStart , String::CharPointerType nameEnd );

    IdentifierWithIndex( const Identifier& other , uint index ) noexcept;

    IdentifierWithIndex (const IdentifierWithIndex& other) noexcept;

    IdentifierWithIndex& operator= (const IdentifierWithIndex& other) noexcept;

    ~IdentifierWithIndex() noexcept;

    String toRawString() const noexcept{ return m_content.toString(); }

    String::CharPointerType getRawCharPointer() const noexcept{ return m_content.getCharPointer(); }

    bool isValid() const noexcept{ return m_content.isValid(); }

    bool isNull() const noexcept{ return m_content.isNull(); }

    forcedinline Identifier operator[]( const uint i ) const { return m_content.toString() + String( i ); }

private:
    Identifier m_content;
};

} // namespace treecore

#endif // TREECORE_IDENTIFIER_WITH_INDEX
