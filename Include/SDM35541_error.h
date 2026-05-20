/**
    @file

    @brief
        This file contains a list of error codes used in the SDM35541Lib API.

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

    $Id: SDM35541_error.h 84613 2014-12-17 22:29:25Z mmcintire $
*/

#ifndef __SDM35541LIB_ERROR_H__
#define __SDM35541LIB_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif


/**
    Enumerates possible error/return codes.
*/
typedef enum
{
    /**
        Success.
    */
    SDM35541_ERROR_NO_ERROR = 0,

    /**
        An unexpected error has occurred.
    */
    SDM35541_ERROR_UNEXPECTED,

    /**
        An invalid parameter was passed into the function.
    */
    SDM35541_ERROR_BAD_PARAMETER,

    /**
        Configuration must be performed before this function can be used.
    */
    SDM35541_ERROR_NOT_CONFIGURED,

    /**
        The module must be enabled before this function can be used.
    */
    SDM35541_ERROR_NOT_ENABLED,

    /**
        This function cannot be used while the module is in use/enabled.
    */
    SDM35541_ERROR_ALREADY_IN_USE,

    /**
        The device file for the board could not be opened.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_OPEN_FAILURE,

    /**
        The device file for the board could not be closed.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_CLOSE_FAILURE,

    /**
        A necessary event object could not be created.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_CREATE_EVENT_FAILURE,

    /**
        There was a failure while waiting for an event.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_WAIT_EVENT_FAILURE,

    /**
        The callback thread could not be created.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_CREATE_THREAD_FAILURE,

    /**
        The callback thread had a problem. The callback function will be called
        with further error info.
    */
    SDM35541_ERROR_INTERNAL_THREAD_FAILURE,

    /**
        An error occurred while setting the callback thread termination event.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_STOP_THREAD_FAILURE,

    /**
        The callback thread did not finish when expected.
    */
    SDM35541_ERROR_THREAD_STUCK,

    /**
        A Window's object handle could not be closed.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_CLOSE_HANDLE_FAILURE,

    /**
        The board could not be queried for its configuration.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_QUERY_FAILURE,

    /**
        The board could not be reset.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_RESET_FAILURE,

    /**
        There was a failure in performing a register access operation.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_REG_OP_FAILURE,

    /**
        The status of the interrupts could not be retrieved.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_INT_STATUS_FAILURE,

    /**
        The setup/configuration of the interrupts could not be performed.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_INT_CONTROL_FAILURE,

    /**
        The DMA status could not be retrieved.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_DMA_STATUS_FAILURE,

    /**
        The DMA operation could not be performed.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_DMA_CONTROL_FAILURE,

    /**
        The boards could not be properly enumerated.

        @note To get more information about the cause of this error call the
            GetLastError() Windows API function.
    */
    SDM35541_ERROR_ENUM_FAILURE,

    /**
        There is not enough memory currently available.
    */
    SDM35541_ERROR_INSUFFICIENT_MEMORY,

    /**
        The board is not bus mastering so DMAs cannot be performed.
    */
    SDM35541_ERROR_NOT_BUS_MASTER,

    /**
        Used to count the number of enumerated error codes.

        @warning This value must always be the last one in the enumeration.
    */
    SDM35541_ERROR_COUNT

} SDM35541_Error;

/**
    Defines the maximum number of characters in an error message string.
*/
#define SDM35541_ERROR_MAX_MSG_LENGTH  100

/**
    Defines a list of error code message strings.

    @warning Must match the SDM35541_Error enumeration.
*/
#define SDM35541_ERROR_STRINGS  { \
/* SDM35541_ERROR_NO_ERROR */ \
"Success",\
/* SDM35541_ERROR_UNEXPECTED */ \
"An unexpected error has occurred.",\
/* SDM35541_ERROR_BAD_PARAMETER */ \
"An invalid parameter was passed to this function.",\
/* SDM35541_ERROR_NOT_CONFIGURED */ \
"The board must be configured before this function can be used.",\
/* SDM35541_ERROR_NOT_ENABLED */ \
"The module must be enabled before this function can be used.",\
/* SDM35541_ERROR_ALREADY_IN_USE */ \
"This function cannot be used while the module is in use/enabled.",\
/* SDM35541_ERROR_OPEN_FAILURE */ \
"The device file for the board could not be opened.",\
/* SDM35541_ERROR_CLOSE_FAILURE */ \
"The device file for the board could not be closed.",\
/* SDM35541_ERROR_CREATE_EVENT_FAILURE */ \
"A necessary event object could not be created.",\
/* SDM35541_ERROR_WAIT_EVENT_FAILURE */ \
"There was a failure while waiting for an event.",\
/* SDM35541_ERROR_CREATE_THREAD_FAILURE */ \
"The callback thread could not be created.",\
/* SDM35541_ERROR_INTERNAL_THREAD_FAILURE */ \
"The callback thread had a problem. The callback function will be called with"\
    " further error info.",\
/* SDM35541_ERROR_STOP_THREAD_FAILURE */ \
"An error occurred while setting the callback thread termination event.",\
/* SDM35541_ERROR_THREAD_STUCK */ \
"The callback thread did not finish when expected.",\
/* SDM35541_ERROR_CLOSE_HANDLE_FAILURE */ \
"A Window's object handle could not be closed.",\
/* SDM35541_ERROR_QUERY_FAILURE */ \
"The board could not be queried for its configuration.",\
/* SDM35541_ERROR_RESET_FAILURE */ \
"The board could not be reset.",\
/* SDM35541_ERROR_REG_OP_FAILURE */ \
"There was a failure in performing a register access operation.",\
/* SDM35541_ERROR_INT_STATUS_FAILURE */ \
"The interrupt status could not be retrieved.",\
/* SDM35541_ERROR_INT_CONTROL_FAILURE */ \
"The setup/configuration of interrupts could not be performed.",\
/* SDM35541_ERROR_DMA_STATUS_FAILURE */ \
"The DMA status could not be retrieved.",\
/* SDM35541_ERROR_DMA_CONTROL_FAILURE */ \
"The DMA operation could not be performed.",\
/* SDM35541_ERROR_ENUM_FAILURE */ \
"There was an error while reading enumeration information from the Registry.",\
/* SDM35541_ERROR_INSUFFICIENT_MEMORY */ \
"There is not enough memory available to perform this function.",\
/* SDM35541_ERROR_NOT_BUS_MASTER */ \
"The board is not bus master so DMAs cannot be performed."\
}

/**
    Defines a list of bool values that indicate whether or not GetLastError()
    can be used to get more error information.

    @warning Must match the SDM35541_Error enumeration.
*/
#define SDM35541_ERROR_CAN_GETLASTERR { \
false,  /* SDM35541_ERROR_NO_ERROR */ \
false,  /* SDM35541_ERROR_UNEXPECTED */ \
false,  /* SDM35541_ERROR_BAD_PARAMETER */ \
false,  /* SDM35541_ERROR_NOT_CONFIGURED */ \
false,  /* SDM35541_ERROR_NOT_ENABLED */ \
false,  /* SDM35541_ERROR_ALREADY_IN_USE */ \
true,   /* SDM35541_ERROR_OPEN_FAILURE */ \
true,   /* SDM35541_ERROR_CLOSE_FAILURE */ \
true,   /* SDM35541_ERROR_CREATE_EVENT_FAILURE */ \
true,   /* SDM35541_ERROR_WAIT_EVENT_FAILURE */ \
true,   /* SDM35541_ERROR_CREATE_THREAD_FAILURE */ \
false,  /* SDM35541_ERROR_INTERNAL_THREAD_FAILURE */ \
true,   /* SDM35541_ERROR_STOP_THREAD_FAILURE */ \
false,  /* SDM35541_ERROR_THREAD_STUCK */ \
true,   /* SDM35541_ERROR_CLOSE_HANDLE_FAILURE */ \
true,   /* SDM35541_ERROR_QUERY_FAILURE */ \
true,   /* SDM35541_ERROR_RESET_FAILURE */ \
true,   /* SDM35541_ERROR_REG_OP_FAILURE */ \
true,   /* SDM35541_ERROR_INT_STATUS_FAILURE */ \
true,   /* SDM35541_ERROR_INT_CONTROL_FAILURE */ \
true,   /* SDM35541_ERROR_DMA_STATUS_FAILURE */ \
true,   /* SDM35541_ERROR_DMA_CONTROL_FAILURE */ \
true,   /* SDM35541_ERROR_ENUM_FAILURE */ \
false,  /* SDM35541_ERROR_INSUFFICIENT_MEMORY */ \
false,  /* SDM35541_ERROR_NOT_BUS_MASTER */ \
}

#ifdef __cplusplus
}
#endif

#endif
