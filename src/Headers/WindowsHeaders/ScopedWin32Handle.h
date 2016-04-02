//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    ScopedWin32Handle.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/19
//! @brief   
//!********************************************************************************


#ifndef ____SCOPEDWIN32HANDLE__A2529323_2F71_4B85_AA04_9120C47B1700
#define ____SCOPEDWIN32HANDLE__A2529323_2F71_4B85_AA04_9120C47B1700

#include "Headers/WindowsHeaders/minimal_windows_header.h"
#include "../Unils/Asserts.h"

#include "../Unils/OS.h"

#if TREECORE_OS_WINDOWS

namespace treecore {


/* 智能释放的windows句柄,
   在生命期结束时自动安全调用CloseHandle
   */
class ScopedWin32Handle
{
public:

    //* 初始化,自动附空(符合windows api规范)
    ScopedWin32Handle() = default;

    //* 初始化为一个句柄,自动转换INVALID_HANDLE_VALUE到NULL
    //  这样的目的是windows使用两种返回值代表错误: INVALID_HANDLE_VALUE(-1)和NULL(0)
    //  而不查api文档你根本无从知道哪个错误返回-1哪个返回0,我也是醉了!
    //  所以这里统一了,你不用再担心了.
    ScopedWin32Handle( HANDLE initHandle );

    //* 当句柄为合法值时,析构时会自动释放之.若不是
    //  合法值,即为NULL时,句柄不需要释放(windows规定的),所以也就什么都不做.
    ~ScopedWin32Handle();

    //* 赋值为一个新句柄,老的句柄将被释放.
    //  自动转换INVALID_HANDLE_VALUE到NULL
    //  这样的目的是windows使用两种返回值代表错误: INVALID_HANDLE_VALUE(-1)和NULL(0)
    //  而不查api文档你根本无从知道哪个错误返回-1哪个返回0,我也是醉了!
    //  所以这里统一了,你不用再担心了.
    ScopedWin32Handle& operator=( HANDLE newHandle );

    //* 检查当前所持有的句柄是否为合法句柄.
    bool isValid() const;

    //* 若检查发现当前句柄不是合法句柄,则会抛出一个ZTD
    //  特有的异常,里面包含文件名,行号和windows返回的错误代码.
    //  你应该在赋值或使用赋值拷贝初始化本类实例后立刻调用这个函数
    //  以便得到正确的windows错误信息,否则错误信息会被后面的错误信息冲掉.
    //  * 若当前句柄是合法句柄,本函数什么都不干.
    void tryThrow() const;

    //* 将handle暴露到外部.
    operator HANDLE() const { tassert( mHandle != NULL ); return mHandle; }

    //* 关闭当前句柄并将当前句柄置空,多次调用是安全的.
    void closeHandle();

    //* 将当前句柄置空,但不调用关闭句柄方法!
    void release();

private:
    HANDLE mHandle = NULL;
    JUCE_DECLARE_NON_COPYABLE( ScopedWin32Handle );
};


} //namespace treecore


#endif //TREECORE_OS_WINDOWS


#endif // ____SCOPEDWIN32HANDLE__A2529323_2F71_4B85_AA04_9120C47B1700