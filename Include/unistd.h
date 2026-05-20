/**
    @file

    @brief
        unistd.h is not available in Windows so this file serves as a holding
        place for any types that would be defined there and are not defined
        elsewhere. The common RTD library makes use of unistd.h and this file
        is therefore required to compile the library in Windows without 
        modification.
        

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
    
    $Id: unistd.h 89110 2015-06-16 13:39:33Z mmcintire $
*/

#ifndef __UNISTD_H__
#define __UNISTD_H__

#ifdef __cplusplus
extern "C" {
#endif

// place any types here that are not otherwise defined in other files.

#ifdef __cplusplus
}
#endif

#endif  // __RTDTYPES_H__
