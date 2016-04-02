
#include "Header.h"

#ifndef ____MINIWHEEL_BUILD_MESSAGES_SHOW
# message "miniwheel build by:"
#if defined( MINIWHEEL_COMPILER_MSVC )
# message "compiler MSVC..."
#if defined( MINIWHEEL_TARGET_64BIT )
# message "target 64 bit..."
#if defined( MINIWHEEL_PLATFORM_WINDOWS )
# message "platform windows..."
#message ""
#define ____MINIWHEEL_BUILD_MESSAGES_SHOW
#endif