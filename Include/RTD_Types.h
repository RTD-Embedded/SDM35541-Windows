/**
    @file

    @brief
        A set of macros and typedefs to abstract away pre-defined compiler
        types.

    @verbatim
    --------------------------------------------------------------------------
    This file and its contents are copyright (C) RTD Embedded Technologies,
    Inc.  All Rights Reserved.

    This software is licensed as described in the RTD End-User Software License
    Agreement.  For a copy of this agreement, refer to the file LICENSE.TXT
    (which should be included with this software) or contact RTD Embedded
    Technologies, Inc.
    --------------------------------------------------------------------------
    @endverbatim

    $Id: RTD_Types.h 66335 2013-01-18 21:50:03Z jstrouse $
*/

#ifndef __RTDTYPES_H__
#define __RTDTYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

// Windows header already defines these vars, so if we are in windows
// we do NOT want to redefine them. Note that this file must be included
// AFTER windows.h for this to work.
#ifndef LOBYTE

typedef int BOOL;

#define FALSE               0
#define TRUE                1

#define LOBYTE(w)           ((byte)(w))
#define HIBYTE(w)           ((byte)((word)(w) >> 8))

#define LOWORD(l)           ((word)(l))
#define HIWORD(l)           ((word)((dword)(l) >> 16))

#define MAKELONG(low, high) \
                    ((dword)(((word)(low)) | (((dword)((word)(high))) << 16)))

#endif // LOBYTE

#include <limits.h>

//==========================================================================
//
// Common names
//
//==========================================================================

typedef unsigned char  uchar;
typedef signed char    schar;
typedef unsigned int   uint;
typedef void *         pvoid;


//==========================================================================
//
// Platform-independent types:  int8, uint8, int16, uint16, int32, uint32
//    and their ~_MIN, ~_MAX values
//
//==========================================================================

#if CHAR_BIT != 8
   #error The char is not 8 bits long
#endif

#if SCHAR_MAX == 0x7f
   typedef signed char   int8;           // 8-bit int
   #define int8_MIN SCHAR_MIN
   #define int8_MAX SCHAR_MAX
#else
   #error No proper char type found for int8
#endif

#if UCHAR_MAX == 0xff
   typedef unsigned char uint8;         // 8-bit uint
   #define uint8_MAX UCHAR_MAX
#else
   #error No proper char type found for uint8
#endif


#if SHRT_MAX == 0x7fff
   typedef short int16;                  // 16-bit int
   #define int16_MIN SHRT_MIN
   #define int16_MAX SHRT_MAX
#else
   #error No proper int type found for int16
#endif

#if USHRT_MAX == 0xffff
   typedef unsigned short uint16;        // 16-bit uint
   #define uint16_MAX USHRT_MAX
#else
   #error No proper unsigned int type found for uint16
#endif

#if SHRT_MAX == 0x7fffffff
   typedef short int32;                  // 32-bit int
   #define int32_MIN SHRT_MIN
   #define int32_MAX SHRT_MAX
#elif INT_MAX == 0x7fffffff
   typedef int int32;
   #define int32_MIN INT_MIN
   #define int32_MAX INT_MAX
#elif LONG_MAX == 0x7fffffff
   typedef long int32;
   #define int32_MIN LONG_MIN
   #define int32_MAX LONG_MAX
#else
   #error No proper int type found for int32
#endif

#if USHRT_MAX == 0xffffffff
   typedef unsigned short uint32;        // 32-bit uint
   #define uint32_MAX USHRT_MAX
#elif UINT_MAX == 0xffffffff
   typedef unsigned int uint32;
   #define uint32_MAX UINT_MAX
#elif ULONG_MAX == 0xffffffff
   typedef unsigned long uint32;
   #define uint32_MAX ULONG_MAX
#else
   #error No proper unsigned int type found for uint32
#endif

#if LONG_MAX == 0x7fffffffffffffff
    typedef long int64;                 // 64-bit int
    #define int64_MIN LONG_MIN
    #define int64_MAX LONG_MAX
#elif LLONG_MAX == 0x7fffffffffffffff
    typedef long long int64;
    #define int64_MIN LLONG_MIN
    #define int64_MAX LLONG_MAX
#else
    #error No proper int type found for int64
#endif

#if ULONG_MAX == 0xffffffffffffffff
    typedef unsigned long uint64;                 // 64-bit int
    #define uint64_MIN ULONG_MIN
    #define uint64_MAX ULONG_MAX
#elif ULLONG_MAX == 0xffffffffffffffff
    typedef unsigned long long uint64;
    #define uint64_MIN ULLONG_MIN
    #define uint64_MAX ULLONG_MAX
#else
    #error No proper unsigned int type found for uint64
#endif

//             -------     // Warning / OK / Error

//================== End of platform-independent types =====================

#ifdef __cplusplus
}
#endif

#endif  // __RTDTYPES_H__
