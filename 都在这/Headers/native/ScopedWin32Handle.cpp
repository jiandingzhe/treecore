#include "ScopedWin32Handle.h"
#include "ThrowLastError.h"


namespace ztd {

ScopedWin32Handle::ScopedWin32Handle( HANDLE initHandle )
    : ScopedWin32Handle()
{
    this->operator=( initHandle );
}

ScopedWin32Handle::~ScopedWin32Handle()
{
    closeHandle();
}



bool ScopedWin32Handle::isValid() const
{
    return mHandle != NULL;
}


void ScopedWin32Handle::tryThrow() const
{
    if( !isValid() )
    {
        tryThrowWindowsLastError();
    }
}


ScopedWin32Handle& ScopedWin32Handle::operator=( HANDLE newHandle )
{
    closeHandle();
    mHandle = newHandle;
    if( mHandle == INVALID_HANDLE_VALUE )
    {
        mHandle = NULL;
    }
    return *this;
}


void ScopedWin32Handle::closeHandle()
{
    if( isValid() )
    {
        BOOL result = CloseHandle( mHandle );
        mHandle = NULL;
        if( result == 0 )
        {
            tryThrowWindowsLastError();
        }
    }
}

void ScopedWin32Handle::release()
{
    mHandle = NULL;
}


} // ztd