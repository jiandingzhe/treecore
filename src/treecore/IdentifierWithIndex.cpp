#include "treecore/IdentifierWithIndex.h"

namespace treecore
{

IdentifierWithIndex::IdentifierWithIndex() noexcept
{
}

IdentifierWithIndex::IdentifierWithIndex( const char* name , uint index)
    : m_content(String(name) + String(index))
{
}

IdentifierWithIndex::IdentifierWithIndex( const String& name , uint index)
    : m_content(name + String(index))
{
}

IdentifierWithIndex::IdentifierWithIndex( String::CharPointerType nameStart , String::CharPointerType nameEnd )
    : m_content(nameStart, nameEnd)
{
}

IdentifierWithIndex::IdentifierWithIndex( const Identifier& other , uint index ) noexcept
    : m_content(other.toString() + String(index))
{
}

IdentifierWithIndex::IdentifierWithIndex (const IdentifierWithIndex& other) noexcept
    : m_content(other.m_content)
{
}

IdentifierWithIndex& IdentifierWithIndex::operator = (const IdentifierWithIndex& other) noexcept
{
    m_content = other.m_content;
    return *this;
}

IdentifierWithIndex::~IdentifierWithIndex() noexcept
{
}

} // namespace treecore
