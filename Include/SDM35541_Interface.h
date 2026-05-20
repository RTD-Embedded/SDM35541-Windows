/**
    @file

    @brief
        This file contains the IOCTL codes, structures and enumerations for
        direct driver access.

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

    $Id: SDM35541_Interface.h 121161 2019-06-25 12:25:58Z kdally $
*/


// Define the Setup Class GUID for the SDM35541. This is same
// as the "RTD dataModules" guid in the INF files.
DEFINE_GUID (GUID_DEVCLASS_SDM35541,
            0xA2CB91F8,0xD73C,0x406D,0x89,0xE1,0x41,0x0F,0x90,0x5F,0x97,0xBB);
//{A2CB91F8-D73C-406D-89E1-410F905F97BB}



#ifndef __SDM35541_INTERFACE_H__
#define __SDM35541_INTERFACE_H__


#include "RTD_Types.h"
//#include "SDM35541_Interrupt_Types.h"
#include "SDM35541_types.h"

/**
    Defines the maximum number of devices (boards) that this driver will
    simultaneously support.
*/
#define SDM35541_INTRFC_MAX_DEVICE_COUNT  8


/**
    Represents the availability of SDM35541 devices. Used in registry enumeration
    entries.
*/
typedef enum
{
    /**
        Represents an invalid device.
    */
    SDM35541_INTRFC_AVAIL_INVALID = 0,

    /**
        Represents a device that is not functioning correctly.
    */
    SDM35541_INTRFC_AVAIL_MALFUNC,

    /**
        Represents a device that is working but is already being used.
    */
    SDM35541_INTRFC_AVAIL_IN_USE,

    /**
        Represents a device that is completely ready for use.
    */
    SDM35541_INTRFC_AVAIL_READY,

} SDM35541_Intrfc_Avail;




// Defines for registry enumeration key names (ANSI and Unicode)
/**
    Defines the ANSI name of the 'device number' enumeration value.
*/
#define SDM35541_INTRFC_REGNAME_ANSI_DEV_NUM      "Device_Number"

/**
    Defines the UNICODE name of the 'device number' enumeration value.
*/
#define SDM35541_INTRFC_REGNAME_WIDE_DEV_NUM      L"Device_Number"

/**
    Defines the ANSI name of the 'availability' enumeration value.
*/
#define SDM35541_INTRFC_REGNAME_ANSI_AVAIL        "Availability"

/**
    Defines the UNICODE name of the 'availability' enumeration value.
*/
#define SDM35541_INTRFC_REGNAME_WIDE_AVAIL        L"Availability"

/**
    Defines the ANSI name of the 'pci device id' enumeration value.
*/
#define SDM35541_INTRFC_REGNAME_ANSI_DEVICE_ID    "Device_Id"

/**
    Defines the UNICODE name of the 'pci device id' enumeration value.
*/
#define SDM35541_INTRFC_REGNAME_WIDE_DEVICE_ID    L"Device_Id"

/**
    Defines the ANSI name of the Common Buffer size value.
*/
#define SDM35541_INTRFC_REGNAME_ANSI_COMM_BUFF_SIZE    "Common_Buffer_Size"

/**
    Defines the UNICODE name of the Common Buffer size value.
*/
#define SDM35541_INTRFC_REGNAME_WIDE_COMM_BUFF_SIZE    L"Common_Buffer_Size"

/**
    Defines the ANSI name of the 'irq number' enumeration value.
*/
#define SDM35541_INTRFC_REGNAME_ANSI_IRQ_NUM      "IRQ"

/**
    Defines the UNICODE name of the 'irq number' enumeration value.
*/
#define SDM35541_INTRFC_REGNAME_WIDE_IRQ_NUM      L"IRQ"

/**
    Defines the maximum number of requests (per channel) that the driver can
    queue up at one time.
*/
#define SDM35541_INTRFC_MAX_DMA_REQUESTS  16

/**
    Defines the maximum size (in bytes) for a single DMA request.
*/
#define SDM35541_INTRFC_MAX_DMA_SIZE      0x40000 

/**
    Defines the maximum number of register blocks usable by one device.
*/
#define SDM35541_INTRFC_MAX_REG_BLK_COUNT 4

/**
    A sub-structure used in the IOCTL_SDM35541_QUERY_BOARD IOCTL.
*/
typedef struct
{
    /**
        This is the register block's starting (physical) address.
    */
    UINT64 start_address;

    /**
        This is the register block's length in bytes. If a block's length is
        zero, then it is unconfigured/unusable.
    */
    uint32 length;

    /**
        If this value is TRUE, then this register block is memory-mapped. If
        this value is FALSE, then this register block is IO-mapped.
    */
    BOOLEAN is_memory;

} SDM35541_Intrfc_Reg_Block_Info;

/**
    A structure used in the IOCTL_SDM35541_QUERY_BOARD IOCTL.
*/
typedef struct
{
    /**
        This index is a value unique to this board, out of all boards
        controlled by this driver.
    */
    uint32 index;

    /**
        This value is the total number of boards being controlled by this
        driver.
    */
    uint32 board_total;

    /**
        This value is the PCI vendor id of this board (from PCI configuration
        space).
    */
    uint16 pci_vendor_id;

    /**
        This value is the PCI device id of this board (from PCI configuration
        space).
    */
    uint16 pci_device_id;

    /**
        This is an array describing the register blocks being used by this
        board.
    */
    SDM35541_Intrfc_Reg_Block_Info reg_blocks[SDM35541_INTRFC_MAX_REG_BLK_COUNT];

    /**
        This value is the number of the interrupt that the board is using.
    */
    uint32 irq_number;

    /**
        If this value is TRUE, the board being described is a bus master and is
        capable of DMA. If this value is FALSE, the board being described cannot
        perform DMAs.
    */
    BOOLEAN is_bus_master;
    
    /**
        This is the size of the Common Buffer objects being used by the driver to
        provide DMA access to the board. This value is the same across all hardware
        DMA buffers for the board, but can vary from board to board. This value
        also determines the maximum size of any individual DMA transfer.
        
        This value is set when the driver is started for each board. 
    */
    uint32 common_buffer_size;
} SDM35541_Intrfc_Board_Info;

/**
    Describes the possible register operation types.
*/
typedef enum
{
    /**
        Reads the register at 'offset' and returns it in 'data'.
    */
    SDM35541_INTRFC_REG_OP_READ = 0,

    /**
        Writes the value in 'data' to the register at 'offset'.
    */
    SDM35541_INTRFC_REG_OP_WRITE,

    /**
        Writes the value in 'data' to the register at 'offset', only changing
        the bits denoted by 'mask'.

        @note
            newValue = (oldValue & ~mask) | (data & mask)
    */
    SDM35541_INTRFC_REG_OP_SAFEWRITE,

} SDM35541_Intrfc_Reg_Op_Type;

/**
    Describes the possible register operation sizes.
*/
typedef enum
{
    /**
        Represents an 8-bit operation.
    */
    SDM35541_INTRFC_REG_SIZE_8 = 1,

    /**
        Represents a 16-bit operation.
    */
    SDM35541_INTRFC_REG_SIZE_16 = 2,

    /**
        Represents a 32-bit operation.
    */
    SDM35541_INTRFC_REG_SIZE_32 = 4,

    /**
        Represents a 64-bit operation.
    */
    SDM35541_INTRFC_REG_SIZE_64 = 8,

} SDM35541_Intrfc_Reg_Op_Size;

/**
    A structure used in the IOCTL_SDM35541_REGISTER_OP IOCTL.
*/
typedef struct
{
    /**
        This is the particular operation to perform.
    */
    SDM35541_Intrfc_Reg_Op_Type op;

    /**
        This is the size of the register operation to perform.
    */
    SDM35541_Intrfc_Reg_Op_Size size;

    /**
        This value indicates which register block that this operation is to
        occur in. The value is zero-based.
    */
    uint8 block;

    /**
        This is an offset into the register block, indicating which register to
        perform the operation on.
    */
    uint32 offset;

    /**
        This holds an incoming or outgoing register value. (On an 8-bit
        operation only the first byte is used. On a 16-bit operation only the
        first two bytes are used.)
    */
    uint32 data;

    /**
        This is the mask value to use in a SDM35541_INTRFC_REG_OP_SAFEWRITE
        operation. (On an 8-bit operation only the first byte is used. On a
        16-bit operation only the first two bytes are used.)

        A one in this value means that the corresponding bit in the register CAN
        be altered. (mask = 0xFF -> same as a WRITE; mask = 0x00 -> no change to
        the register)
    */
    uint32 mask;

} SDM35541_Intrfc_Reg_Op;

/**
    A structure used in the IOCTL_SDM35541_INT_CONTROL IOCTL.
*/
typedef struct
{
    /**
        This specifies which function block the interrupt belongs to.
    */
    uint32 function_block_number;
    
    /**
        Pass in TRUE to set the interrupt to the state given by 'enable'. Pass
        in FALSE to simply fill 'enable' with the interrupt's current
        enable/disable state.
    */
    BOOLEAN change_state;

    /**
        When this value is TRUE it represents an enabled interrupt. When this
        value is FALSE it represents a disabled interrupt. See the description
        of 'change_state' for details.

        @note
            Enabling an interrupt will clear it's 'count' and 'overrun count'.
    */
    BOOLEAN enable;

} SDM35541_Intrfc_Int_Control;

/**
    Describes the possible interrupt status operation types.
*/
typedef enum
{
    /**
        Represents a simple polling of interrupt status.

        @note
            When using this operation, DeviceIoControl completes immediately and
            the structure given as output is valid at that time.
    */
    SDM35541_INTRFC_INT_STATUS_OP_POLL,

    /**
        Represents a request for a notification the next time a certain
        interrupt occurs.

        @note
            To use this operation, the file used to access the device must have
            been opened with 'overlapped' capabilities. Also the DeviceIoControl
            call must have been given an OVERLAPPED structure, containing a
            valid, unique event handle.

        @note
            When using this operation, DeviceIoControl completes immediately but
            the structure given as output is not yet valid. The event in the
            OVERLAPPED structure will be signaled (to notify the user) the next
            time the relevant event occurs (an interrupt of a specific type).
            The structure given as output will be valid at this time.
    */
    SDM35541_INTRFC_INT_STATUS_OP_NOTIFY,

    /**
        Represents clearing the notification request queue for a certain
        interrupt type by canceling all of the interrupt type's notification
        requests.

        The notification requests cleared from the queue will NOT have a valid
        SDM35541_Intrfc_Int_Status structure upon cancellation. Also, if
        GetOverlappedResult is called for a canceled request it will return an
        error and GetLastError will equal ERROR_OPERATION_ABORTED.

        @note
            When using this operation, DeviceIoControl completes immediately and
            the structure given as output is valid at that time.
    */
    SDM35541_INTRFC_INT_STATUS_OP_CLEAR_QUEUE

} SDM35541_Intrfc_Int_Status_Op;

/**
    A structure used in the IOCTL_SDM35541_INT_STATUS IOCTL.
*/
typedef struct
{
    /**
        This specifies which function block the interrupt belongs to.
    */
    uint32 function_block_number;
    
    /**
        This is the status operation to be performed.
    */
    SDM35541_Intrfc_Int_Status_Op op;

    /**
        When the output structure is valid (see SDM35541_Intrfc_Int_Status_Op)
        this value contains the interrupt count for the specified interrupt.

        @note
            An interrupt's count is reset every time you: check its status using
            SDM35541_INTRFC_INT_STATUS_OP_POLL, get notification of it using
            SDM35541_INTRFC_INT_STATUS_OP_NOTIFY, or enable it using
            IOCTL_SDM35541_INT_CONTROL.

        @note
            If 'op' is set to SDM35541_INTRFC_INT_STATUS_OP_CLEAR_QUEUE, this
            value is not used.
    */
    uint32 count;

} SDM35541_Intrfc_Int_Status;

/**
    Describes the possible DMA operation types.
*/
typedef enum
{
    /**
        Represents a request for data to be DMA'd from an 'external' host/cpu
        buffer to 'local' board memory.
    */
    SDM35541_INTRFC_DMA_OP_FROM_BUFFER,

    /**
        Represents a request for data to be DMA'd from 'local' board memory to
        an 'external' host/cpu buffer.
    */
    SDM35541_INTRFC_DMA_OP_TO_BUFFER,

    /**
        Represents a request for data to be DMA'd from 'external' PCI memory to
        'local' board memory.
    */
    SDM35541_INTRFC_DMA_OP_FROM_ADDRESS,

    /**
        Represents a request for data to be DMA'd from 'local' board memory to
        'external' PCI memory.
    */
    SDM35541_INTRFC_DMA_OP_TO_ADDRESS,

    /**
        Represents an abort/cancellation of the DMA request in progress. DMA
        request processing will continue, as normal, with the next request.
    */
    SDM35541_INTRFC_DMA_OP_ABORT,

    /**
        Represents an abort/cancellation of the DMA request in progress plus a
        flushing/removal of all requests still waiting in the queue. DMA request
        processing will continue normally when new requests are added to the
        queue.
        
        This operation is specific to the function block, channel, and buffer 
        specified in the DMA control structure.
    */
    SDM35541_INTRFC_DMA_OP_FLUSH,
    
    /**
        Represents an abort/cancellation of ALL DMA requests in progress plus a
        flushing/removal of all requests still waiting in the queue. 
        
        This operation will perform the flush on every DMA buffer in every DMA
        channel on every function block.
    */
    SDM35541_INTRFC_DMA_OP_FLUSH_ALL

} SDM35541_Intrfc_DMA_Op;

/**
    Describes the possible outcomes for a DMA request.
*/
typedef enum
{
    /**
        Represents a complete, successful, DMA request. DMA request processing
        continues as normal.
    */
    SDM35541_INTRFC_DMA_RESULT_SUCCESS,

    /**
        Represents a request that has timed out and has been aborted. DMA
        request processing continues, as normal, with the next request.
    */
    SDM35541_INTRFC_DMA_RESULT_TIMEOUT,

    /**
        Represents a request which was manually aborted by the user.
    */
    SDM35541_INTRFC_DMA_RESULT_ABORTED,

    /**
        Represents a DMA request which was aborted (or simply removed from the
        DMA request queue) when the queue was manually flushed by the user.
    */
    SDM35541_INTRFC_DMA_RESULT_FLUSHED,
    
    /**
        Represents a request in which the FV_DMAm_STAT_USED error interrupt 
        fired.
    */
    SDM35541_INTRFC_DMA_RESULT_USED,
    
    /**
        Represents a request in which the FV_DMAm_STAT_INVALID error interrupt 
        fired.
    */
    SDM35541_INTRFC_DMA_RESULT_INVALID,
    
    /**
        Represents a request in which the FV_DMAm_STAT_OVERFLOW error interrupt 
        fired.
    */
    SDM35541_INTRFC_DMA_RESULT_OVERFLOW,
    
    /**
        RRepresents a request in which the FV_DMAm_STAT_UNDERFLOW error interrupt 
        fired.
    */
    SDM35541_INTRFC_DMA_RESULT_UNDERFLOW

} SDM35541_Intrfc_DMA_Result;

/**
    A structure used in the IOCTL_SDM35541_DMA_CONTROL IOCTL.
*/
typedef struct
{
    /**
        This specifies which function block the DMA will be associated with.
    */
    uint32 function_block_number;

    /**
        This is the DMA channel within the function block to perform an
        operation on (channels are 0-based).
    */
    uint8 channel;
    
    /**
        This is the buffer number within the DMA channel to perform the
        operation on.
    */
    uint8 buffer_num;
    
    /**
        This is the DMA operation to be performed.
    */
    SDM35541_Intrfc_DMA_Op op;

    /**
        This is a user-defined value that will be associated with this
        particular DMA request. The user can use this value to differentiate
        between DMA requests.

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH.

    */
    uint32 request_id;

    /**
        This is an offset into the 'local' address space of the board. This
        offset is the starting point for local data being copied to/from the
        board. See 'incr_local' for details.

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH.

    */
    uint32 local_offset;

    /**
        If TRUE, the DMA operation will write/read data from consecutive
        local offsets starting at 'local_offset'. If FALSE, all local
        writes/reads will be done at 'local_offset'.

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH.

    */
    BOOLEAN incr_local;

    /**
        This address is the starting address for external data being copied
        to/from a board.

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH.

    */
    void* external;

    /**
        This gives the number of bytes used in each write/read access made,
        both on the board (locally) and off the board (externally). This value
        gives the number of bytes used to increment the external address after
        each access. If 'incr_local' is TRUE, then this value also gives the
        number of bytes used in incrementing the local offset.

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH.

    */
    SDM35541_Intrfc_Reg_Op_Size access_size;

    /**
        This gives the total number of bytes to be copied for this request. This
        value must be evenly divisible by the access size give by 'access_size'.

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH.

    */
    uint32 request_size;

    /**
        If TRUE, the DMA operation will be configured for demand-mode. See the
        hardware documentation for details.

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH.

    */
    BOOLEAN do_demand;

    /**
        This value is the number of milliseconds that should be allowed for the
        DMA request to complete (starting when the DMA request becomes active,
        not when it is queued). If the request is not finished in the allowed
        time, it will be aborted. However, if a timeout value of zero is
        specified, the request will never timeout and can only be aborted
        manually.

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH.

    */
    uint32 timeout;

    /**
        If TRUE, the IOCTL call will complete as soon as the request has been
        queued. If FALSE, the IOCTL call does not complete until the request is
        complete (i.e. the request has finished or has timed-out).

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH. The IOCTL call will not complete
            until the abort/flush operation is finished.
    */
    BOOLEAN return_immediate;

    /**
        If return_immediate was set to FALSE, this value gives the final result
        of this DMA request (when it completes). If return_immediate was set to
        TRUE, this value is not used.

        @note
            This value is not used if 'op' is set to SDM35541_INTRFC_DMA_OP_ABORT
            or SDM35541_INTRFC_DMA_OP_FLUSH. The IOCTL call will not complete
            until the abort/flush operation is finished.
    */
    SDM35541_Intrfc_DMA_Result request_result;
    
    /**
        The count in bytes of the actual amount transferred. This is taken from
        the board FB_DMAm_COUNT register.
    */
    UINT32 bytes_transferred;

} SDM35541_Intrfc_DMA_Control;

/**
   32-bit version of the SDM35541_Intrfc_DMA_Control structure defined above for 
   exclusive use by the 64-bit driver when handling requests from 32-bit applications.
   
   Note: This structure is identical to the SDM35541_Intrfc_DMA_Control declared above 
   with the exception of the POINTER_32 macro used with the "external" field. This
   forces Windows to use a 32-bit pointer for that field, even on a 64-bit system,
   allowing the 64-bit driver to work correctly with instances of this structure
   sent from 32-bit user-mode applications.
*/
typedef struct
{
    uint32 function_block_number;
    uint8 channel;
    uint8 buffer_num;
    SDM35541_Intrfc_DMA_Op op;
    uint32 request_id;
    uint32 local_offset;
    BOOLEAN incr_local;
    void* POINTER_32 external;
    SDM35541_Intrfc_Reg_Op_Size access_size;
    uint32 request_size;
    BOOLEAN do_demand;
    uint32 timeout;
    BOOLEAN return_immediate;
    SDM35541_Intrfc_DMA_Result request_result;
    UINT32 bytes_transferred;
} SDM35541_Intrfc_DMA_Control_32;

/**
    Describes the possible DMA status operation types.
*/
typedef enum
{
    /**
        Represents a simple polling of DMA status.

        @note
            When using this operation, DeviceIoControl completes immediately and
            the structure given as output is valid at that time.
    */
    SDM35541_INTRFC_DMA_STATUS_OP_POLL,

    /**
        Represents a request for a notification the next time a DMA finishes on
        a certain channel.

        @note
            To use this operation, the file used to access the device must have
            been opened with 'overlapped' capabilities. Also the DeviceIoControl
            call must have been given an OVERLAPPED structure, containing a
            valid, unique event handle.

        @note
            When using this operation, DeviceIoControl completes immediately but
            the structure given as output is not yet valid. The event in the
            OVERLAPPED structure will be signaled (to notify the user) the next
            time the relevant event occurs (a DMA channel finishing a request).
            The structure given as output will be valid at that time.
    */
    SDM35541_INTRFC_DMA_STATUS_OP_NOTIFY,

    /**
        Represents clearing the DMA notification request queue for a certain
        channel by canceling all of the channel's DMA notification requests.

        The notification requests cleared from the queue will NOT have a valid
        SDM35541_Intrfc_DMA_Status structure upon cancellation. Also, if
        GetOverlappedResult is called for a canceled notification request it
        will return an error and GetLastError will equal
        ERROR_OPERATION_ABORTED.

        @note
            When using this operation, DeviceIoControl completes immediately and
            the structure given as output is valid at that time.
    */
    SDM35541_INTRFC_DMA_STATUS_OP_CLEAR_QUEUE

} SDM35541_Intrfc_DMA_Status_Op;

/**
    A structure used in the IOCTL_SDM35541_DMA_STATUS IOCTL.
*/
typedef struct
{
    /**
        This specifies which function block the DMA will be associated with.
    */
    uint32 function_block_number;
    
    /**
        This is the DMA channel within the function block to get the status of
        (channels are 0-based).
    */
    uint8 channel;
    
    /**
        This is the buffer number within the channel to get the status of 
        (buffers are 0-based).
    */
    uint8 buffer_num;
    
    /**
        This is the status operation to be performed.
    */
    SDM35541_Intrfc_DMA_Status_Op op;

    /**
        When the output structure is valid (see SDM35541_Intrfc_DMA_Status_Op)
        this value will contain the state of the DMA channel. If TRUE this
        channel is fulfilling a request. If FALSE this channel is currently
        inactive.

        @note
            If 'op' is set to SDM35541_INTRFC_DMA_STATUS_OP_CLEAR_QUEUE, this
            value is not used.
    */
    BOOLEAN is_working;

    /**
        When the output structure is valid (see SDM35541_Intrfc_DMA_Status_Op)
        this value will contain the number of DMA requests waiting to be
        processed.

        @note
            If 'op' is set to SDM35541_INTRFC_DMA_STATUS_OP_CLEAR_QUEUE, this
            value is not used.
    */
    uint32 pending;

    /**
        When the output structure is valid (see SDM35541_Intrfc_DMA_Status_Op)
        this value will contain a DMA request's id (as passed in to the
        IOCTL_SDM35541_DMA_CONTROL IOCTL when making the DMA request). If this
        IOCTL is returning DMA notification, this value will be the id of the
        DMA that just finished and is sending notification. If this IOCTL is
        simply polling, this value will be the id of the DMA currently in
        progress (or zero if no DMA is in progress).

        @note
            If 'op' is set to SDM35541_INTRFC_DMA_STATUS_OP_CLEAR_QUEUE, this
            value is not used.
    */
    uint32 request_id;

    /**
        If this IOCTL is returning DMA notification, then when the output
        structure is valid (see SDM35541_Intrfc_DMA_Status_Op) this value will
        contain the result of the DMA request that is completing and sending its
        notification.

        @note
            If 'op' is set to either SDM35541_INTRFC_DMA_STATUS_OP_CLEAR_QUEUE or
            SDM35541_INTRFC_DMA_STATUS_OP_POLL, this value is not used.
    */
    SDM35541_Intrfc_DMA_Result request_result;
    
    /**
        If this IOCTL is returning DMA notification, then when the output
        structure is valid (see SDM35541_Intrfc_DMA_Status_Op) this value will
        contain the number of bytes transferred in this DMA.

        @note
            If 'op' is set to either SDM35541_INTRFC_DMA_STATUS_OP_CLEAR_QUEUE or
            SDM35541_INTRFC_DMA_STATUS_OP_POLL, this value is not used.
    */
    UINT32 bytes_transferred;

} SDM35541_Intrfc_DMA_Status;


/**
    Defines a macro for creating new IOCTLs.
*/
#define SDM35541_NEW_IOCTL(v) CTL_CODE(   0x8000,\
                                        v,\
                                        METHOD_BUFFERED,\
                                        FILE_READ_ACCESS | FILE_WRITE_ACCESS)

/**
    @brief
        Defines a IOCTL to query the board about its configuration.

    The output buffer must be a SDM35541_Intrfc_Board_Info structure. The input
    buffer is not used.

    @note
        If this device's file was opened with the FILE_FLAG_OVERLAPPED flag,
        then any calls to DeviceIoControl must have a valid OVERLAPPED structure
        (with valid hEvent parameter) passed in via the lpOverlapped parameter.

*/
#define IOCTL_SDM35541_QUERY_BOARD        SDM35541_NEW_IOCTL(0x800)

/**
    @brief
        Defines a IOCTL to 'reset' the board and any associated state inside
        the driver.

    The input and output buffers are not used.

    @note
        If this device's file was opened with the FILE_FLAG_OVERLAPPED flag,
        then any calls to DeviceIoControl must have a valid OVERLAPPED structure
        (with valid hEvent parameter) passed in via the lpOverlapped parameter.

*/
#define IOCTL_SDM35541_BOARD_RESET        SDM35541_NEW_IOCTL(0x801)

/**
    @brief
        Defines a IOCTL to read, write or safe-write an SDM35541 register.

    The input and output buffers must both be a SDM35541_Intrfc_Reg_Op structure.

    @note
        If this device's file was opened with the FILE_FLAG_OVERLAPPED flag,
        then any calls to DeviceIoControl must have a valid OVERLAPPED structure
        (with valid hEvent parameter) passed in via the lpOverlapped parameter.

*/
#define IOCTL_SDM35541_REGISTER_OP        SDM35541_NEW_IOCTL(0x810)

/**
    @brief
        Defines a IOCTL to disable/enable interrupts and retrieve their
        disable/enable state.

    The input and output buffers must both be a SDM35541_Intrfc_Int_Control
    structure.

    @note
        If this device's file was opened with the FILE_FLAG_OVERLAPPED flag,
        then any calls to DeviceIoControl must have a valid OVERLAPPED structure
        (with valid hEvent parameter) passed in via the lpOverlapped parameter.

*/
#define IOCTL_SDM35541_INT_CONTROL        SDM35541_NEW_IOCTL(0x820)

/**
    @brief
        Defines a IOCTL which can retrieve the status of an interrupt or can
        ask the driver to signal an event the next time the interrupt happens.

    The input and output buffers must both be a SDM35541_Intrfc_Int_Status
    structure.

    @note
        This IOCTL can be used on a single interrupt type more than once
        provided that the user's notification events (the events passed in to
        DeviceIoControl via the OVERLAPPED structure) are unique. If this is
        done, the events will be queued in the order of their arrival.

    @note
        If this device's file was opened with the FILE_FLAG_OVERLAPPED flag,
        then any calls to DeviceIoControl must have a valid OVERLAPPED structure
        (with valid hEvent parameter) passed in via the lpOverlapped parameter.

*/
#define IOCTL_SDM35541_INT_STATUS         SDM35541_NEW_IOCTL(0x821)

/**
    @brief
        Defines a IOCTL for requesting (and controlling) DMA operations.

    The input and output buffers must both be a SDM35541_Intrfc_DMA_Control
    structure.

    @note
        If this device's file was opened with the FILE_FLAG_OVERLAPPED flag,
        then any calls to DeviceIoControl must have a valid OVERLAPPED structure
        (with valid hEvent parameter) passed in via the lpOverlapped parameter.

    @note
        For any calls made to this IOCTL from a thread context other than the
        one where the device file was created: these IOCTLs will be cancelled
        when the thread exits. This will cause any locked user buffers to be
        leaked and a bugcheck will occur. Make sure all DMA requests made from
        a different thread context are flushed before the thread exits.

*/
#define IOCTL_SDM35541_DMA_CONTROL        SDM35541_NEW_IOCTL(0x830)

/**
    @brief
        Defines a IOCTL which can retrieve the status of a DMA request or can
        ask the driver to signal an event whenever a DMA request finishes.

    The input and output buffers must both be a SDM35541_Intrfc_DMA_Status
    structure.

    @note
        This IOCTL can be used on a single DMA channel more than once provided
        that the user's notification events (the events passed in to
        DeviceIoControl via the OVERLAPPED structure) are unique. If this is
        done, the events will be queued in the order of their arrival.

    @note
        If this device's file was opened with the FILE_FLAG_OVERLAPPED flag,
        then any calls to DeviceIoControl must have a valid OVERLAPPED structure
        (with valid hEvent parameter) passed in via the lpOverlapped parameter.

*/
#define IOCTL_SDM35541_DMA_STATUS         SDM35541_NEW_IOCTL(0x831)


#endif //__SDM35541_INTERFACE_H__
