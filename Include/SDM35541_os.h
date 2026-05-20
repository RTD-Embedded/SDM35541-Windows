/**
    @file

    @brief
        SDM35541 user library definitions

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

    $Id: SDM35541_os.h 151612 2026-01-20 20:53:48Z bkorpacz $
*/

#ifndef __SDM35541LIB_H__
#define __SDM35541LIB_H__

#include <windows.h>
#include "SDM35541_error.h"
#include "SDM35541_board_access.h"
#include "SDM35541_board_access_structs.h"
#include "SDM35541_Interface.h"
#include "RTD_Types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
    @brief
    Defines the maximum number of notification requests that can be queued at
    one time within callback threads.
*/
#define SDM35541_NOTIFY_QUEUE_DEPTH   16


/**
  @brief
  SDM35541 board descriptor.  This structure holds information about
  the board as a whole.  It holds the file descriptor and ISR callback
  function, if applicable.
 */

struct SDM35541_Board_Descriptor
{
    /**
        A device-file handle used to communicate with the device via IOCTLs.
    */
    HANDLE file_handle;

    /**
        Holds information about the board
    */
    SDM35541_Intrfc_Board_Info board_info;

    /**
        Holds handles for all the DMA callback threads.
    */
    HANDLE dma_thread_handles[SDM35541_MAX_FB][MAX_DMA_CHANNELS][MAX_DMA_BUFFERS];

    /**
        The termination events for the DMA callback threads.
    */
    HANDLE dma_terminate_events[SDM35541_MAX_FB][MAX_DMA_CHANNELS][MAX_DMA_BUFFERS];

    /**
        Holds handles for all the A/D interrupt callback threads.
    */
    HANDLE int_thread_handles[SDM35541_MAX_FB];
    
    /**
        The termination events for the A/D interrupt callback threads.
    */
    HANDLE int_terminate_events[SDM35541_MAX_FB];

    /**
     * Stores the board-level interrupts that are enabled
     * in the register (SDM35541_INT).
     */
    uint16_t ints_enabled;

} ;

/**
    @brief
        This structure holds a board's enumeration information gathered from
        the Registry.
*/
typedef struct
{
    /**
        Board Device Number
    */
    uint32 dev_num;

    /**
        Availability Status
    */
    SDM35541_Intrfc_Avail avail;

    /**
        PCI Device ID
    */
    uint32 pci_device_id;

    /**
        Board IRQ
    */
    uint32 irq;

} SDM35541_EnumInfo;

typedef struct
{
    /**
        This value describes why the callback was called. If the value is
        SDM35541_ERROR_NO_ERROR, this is a normal interrupt callback. Otherwise,
        this value describes a fatal error that has occurred within the thread.
    */
    SDM35541_Error result;

    /**
        Number of driver interrupts that occurred since the last time the
        user's callback was called.
    */
    uint32 interrupts;

    /**
        Number of interrupts missed by the driver's ISR since the last time the
        user's callback was called.
    */
    uint32 overrun;

} SDM35541_Interrupt_Callback_Info;

/**
    This describes the format for interrupt callback routines.\n Callbacks
    should be declared as:
    @code
    void my_callback(SDM35541_Interrupt_Callback_Info);
    @endcode

    @note
        This callback function will be called in its own thread context.
        Appropriate synchronization to code and data is required.

    @note
        If an unexpected error should occur within the notification thread, the
        user's callback will be called before the thread exits. The 'result'
        value in the SDM35541_Interrupt_Callback_Info structure will contain the
        appropriate error code.

*/
typedef void (*SDM35541_Interrupt_Callback)(SDM35541_Interrupt_Callback_Info info);


typedef struct
{
    /**
        Holds the function block number associated with this interrupt callback.
    */
    UINT8 fb_num;

    /**
        A handle to the board's device file, so that IOCTL's can be called in
        the thread.
    */
    HANDLE device_file;

    /**
        A handle to the event which kills the thread when it is set.
    */
    HANDLE terminate_event;

    /**
        This value is set by the thread to denote its status. The value is
        initialized to zero. If this value is set to one then the thread is
        ready to process callbacks. If this value is set to two then the thread
        encountered an error while preparing. In either case, the thread will
        clean up any resources associated with the thread data.
    */
    LONG status;

    /**
        A function pointer to the interrupt's callback.
    */
    SDM35541_Interrupt_Callback callback;

} SDM35541_Interrupt_Thread_Data;


   /**
    @brief
        This structure holds the information passed to the user's DMA callback.
*/
typedef struct
{
    /**
        This value describes why the callback was called. If the value is
        SDM35541_ERROR_NO_ERROR, this is a normal DMA callback. Otherwise, this
        value describes a fatal error that has occurred within the thread.
    */
    SDM35541_Error result;

    /**
        This value describes how the DMA transfer completed and if it was
        successful.
    */
    SDM35541_Intrfc_DMA_Result request_result;
    
    /**
        The function block number the thread is associated with.
    */ 
    UINT8 fb_num;

    /**
        Gives the DMA channel within the function block the thread is associated with.
    */
    UINT8 channel;

    /**
        Gives the DMA buffer within the channel that the thread is associated with.
    */
    UINT8 buffer_num;

    /**
        This is the id value given to SDM35541_DMA_Request_Transfer() when this
        transfer request was made.
    */
    uint32 request_id;

    /**
        This value gives the number of transfer requests that were in the queue
        behind this one.
    */
    uint32 pending;
    
    /**
        This is the actual number of bytes transferred for this DMA.
    */
    uint32 bytes_transferred;

	/**
		This is a void pointer for use as a context pointer
	*/
	LPVOID context_ptr;

} SDM35541_DMA_Callback_Info;

/**
    This describes the format for DMA callback routines.\n Callbacks should be
    declared as:
    @code
    void my_callback(SDM35541_DMA_Callback_Info);
    @endcode

    @note
        This callback function will be called in its own thread context.
        Appropriate synchronization to code and data is required.

    @note
        If an unexpected error should occur within the notification thread, the
        user's callback will be called before the thread exits. The 'result'
        value in the SDM35541_DMA_Callback_Info structure will contain the
        appropriate error code.

*/
typedef void (*SDM35541_DMA_Callback)(SDM35541_DMA_Callback_Info info);


typedef struct
{
    /**
        The function block number the thread is associated with.
    */ 
    UINT8 fb_num;

    /**
        Gives the DMA channel within the function block the thread is associated with.
    */
    UINT8 channel;

    /**
        Gives the DMA buffer within the channel that the thread is associated with.
    */
    UINT8 buffer_num;
    
    /**
        A handle to the board's device file, so that IOCTL's can be called in
        the thread.
    */
    HANDLE device_file;

    /**
        A handle to the event which kills the thread when it is set.
    */
    HANDLE terminate_event;

    /**
        This value is set by the thread to denote its status. The value is
        initialized to zero. If this value is set to one then the thread is
        ready to process callbacks. If this value is set to two then the thread
        encountered an error while preparing. In either case, the thread will
        clean up any resources associated with the thread data.
    */
    LONG status;

    /**
        A function pointer to the DMA channel's callback.
    */
    SDM35541_DMA_Callback callback;

	/**
		A void pointer for use as a context pointer
	*/
	LPVOID context_ptr;

} SDM35541_DMA_Thread_Data;

/**
    @brief
        DMA Operation
*/
typedef enum
{
    /**
        DMA transfer from the board to a buffer
    */
    SDM35541_DMA_OP_BOARD_TO_BUFFER = 0,

    /**
        DMA transfer from a buffer to the board
    */
    SDM35541_DMA_OP_BUFFER_TO_BOARD,

    /**
        DMA transfer from the board to an arbitrary address
    */
    SDM35541_DMA_OP_BOARD_TO_ADDRESS,

    /**
        DMA transfer from an arbitrary address to the board
    */
    SDM35541_DMA_OP_ADDRESS_TO_BOARD,

} SDM35541_dma_operation;    
    
/**
    @brief
        Calls a board IOCTL

    @param
        handle

        A pointer to the board's descriptor.

    @param
        code

        The code representing the IOCTL to be called.

    @param
        buffer

        A pointer to a buffer to be passed to the IOCTL. This is usually a
        pointer to a DM35425_Intrfc_* structure containing the parameters for
        the IOCTL.

    @param
        buff_size

        Size of the buffer pointed to by 'buffer'.

    @param
        overlapped

        A pointer to an overlapped structure to use. This value can be NULL

    @internal

    @warning
        This is an internal function and should not be called directly.

*/
SDM35541_Error call_ioctl(struct SDM35541_Board_Descriptor * handle,
                        DWORD code,
                        LPVOID buffer,
                        DWORD buff_size,
                        LPOVERLAPPED overlapped);

/**
    @brief
        Reads the board enumeration registry keys.

    @param
        enum_key

        Registry key to read.

    @param
        index

        Board index.

    @param
        info

        Address of structure to store the enumeration information.

    @internal

    @warning
        This is an internal function and should not be called directly.

*/                        
SDM35541_Error enum_registry_key( HKEY enum_key,
                                DWORD index,
                                SDM35541_EnumInfo * info);

/**
    @brief
        Performs translation from the common library SDM35541_pci_region_num enum
        to the SDM35541_Intrfc_Reg_Op struct block number that the Windows driver
        uses.

    @param
        pci_region

        The SDM35541_pci_region_num to be translated.

    @param
        block

        A pointer to the translated block number.

    @internal

    @warning
        This is an internal function and should not be called directly.

*/                                
SDM35541_Error pci_region_num_to_reg_op_block(enum SDM35541_pci_region_num pci_region, uint8 * block);
                                
/**
    @defgroup SDM35541Lib_API_General SDM35541 Library API - General
    @{
*/

/**
    @brief
        Enumerates the supported boards in the system.

    @param
        info

        Address of board enumeration structure.

    @param
        size

        Number of boards to try to enumerate.

*/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_General_Enum_Boards( SDM35541_EnumInfo * info,
                                                ULONG size);

 /**
    @brief
        Installs an interrupt callback function for the function block
        specified.

    @param
        handle

        A pointer to the board's descriptor.

    @param
        fb

        Pointer to the function block descriptor.
        
    @param
        callback
        
        A function that will be called each time an interrupt in this function
        block occurs.

*/                                               
SDM35541LIB_API
int WINAPI SDM35541_Interrupt_Install_Callback(struct SDM35541_Board_Descriptor * handle,
                                              struct SDM35541_Function_Block * fb,
                                              SDM35541_Interrupt_Callback callback);

/**
    @brief
        Removes the interrupt callback associated with this function block.

    @param
        handle

        A pointer to the board's descriptor.

    @param
        fb

        Pointer to the function block descriptor.

*/
SDM35541LIB_API
int WINAPI SDM35541_Interrupt_Remove_Callback(struct SDM35541_Board_Descriptor * handle,
                                             struct SDM35541_Function_Block * fb);  


/**
    @brief
        Requests a DMA transfer to or from one of the board's fifos.

    @param
        handle

        A pointer to the board's descriptor.

    @param
        fb_num
        
        The function block number to use.
    
    @param
        channel

        The DMA channel within the function block that will perform this transfer.

	@param
        dma_buffer_number

        This is the fifo that will be have data taken from it or put into it.

	@param
        operation

        The type of DMA transfer to perform. This parameter describes whether
        data is going to or from the board and whether the other end of the
        transfer is a user buffer or an arbitrary physical address.



    @param
        address

        This is a pointer to the off-board end of the transfer. This will
        either be a pointer to a user's buffer or an arbitrary physical address.

    @param
        size

        This is the number of samples to be transferred.

    @param
        demand

        If TRUE, this will be a demand-mode DMA and data transfer will be
        controlled by the demand source set with DM75XX_DMA_Set_Demand_Source().

    @param
        request_id

        This can be any value. This value will stay with the transfer request
        and will be reported back to the user if the user has installed a DMA
        callback function.

    @param
        timeout

        If zero, the timeout is ignored and this transfer will only finish when
        successful or when aborted by the user. If this value is not zero, it
        defines a timeout value in milliseconds. This timeout value begins
        counting when this DMA transfer request reaches the front of the queue.
        If the request's entire transfer does not complete within the allotted
        time, it is aborted and processing continues with the next request in
        the queue.

    @param
        wait

        If FALSE, this function will return as soon as the DMA request is added
        to the queue of requests. If TRUE, this function will not return until
        this DMA request has been processed and completed.

    @param
        dma_result

        If 'wait' is set to TRUE, the pointed-to value will be filled with the
        result of this DMA request (success, timeout, etc). Otherwise, this
        value is not used.

*/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_DMA_Request_Transfer(
                                        struct SDM35541_Board_Descriptor * handle,
                                        UINT8 fb_num,
                                        UINT8 channel,
                                        UINT8 dma_buffer_number,
                                        SDM35541_dma_operation operation,
                                        void * address,
                                        uint32 size,
                                        BOOL demand,
                                        uint32 request_id,
                                        uint32 timeout,
                                        BOOL wait,
                                        SDM35541_Intrfc_DMA_Result * dma_result);


/**
    @brief
        Aborts the current DMA request on the selected function block
        and DMA channel.

    @param
        handle

        A pointer to the board's descriptor.

    @param
        fb_num
        
        The function block number within which the DMA will be aborted

    @param
        channel

        The DMA channel within the function block whose current DMA will
        be aborted.

    @param
        buffer_num

        The DMA buffer within the channel whose current DMA will
        be aborted.    
        
    @note
        Once the current DMA request is aborted, processing will continue
        normally for any other queued DMA requests.

*/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_DMA_Abort( struct SDM35541_Board_Descriptor * handle,
                                        UINT8 fb_num,
                                        UINT8 channel,
                                        UINT8 buffer_num);


/**
    @brief
        Aborts/cancels the current DMA request, and all queued requests, on the
        selected DMA channel.

    @param
        handle

        A pointer to the board's descriptor.
        
    @param
        fb_num
        
        The function block number to perform the flush on.

    @param
        channel

        The DMA channel within the function block to flush.
        
    @param
        buffer_num

        The DMA buffer within the channel whose current DMA will
        be aborted.

    @note
        After this function, the specified DMA channel will be idle, waiting
        for new DMA requests.

*/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_DMA_Flush( struct SDM35541_Board_Descriptor * handle,
                                        UINT8 fb_num,
                                        UINT8 channel,
                                        UINT8 buffer_num);

                                        
/**
    @brief
        Aborts/cancels all DMA requests and all queued requests.
        
    @param
        handle

        A pointer to the board's descriptor.

    @note
        After this function, all DMA will be idle, waiting
        for new DMA requests.

*/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_DMA_Flush_All( struct SDM35541_Board_Descriptor * handle);                                        
/**
    @brief
        Retrieves the current state of a DMA channel.

    @param
        handle

        A pointer to the board's descriptor.
        
    @param
        fb_num
        
        The function block number of interest for this status request.

    @param
        channel

        The DMA channel within the function block to get the status of.

    @param
        buffer_num

        The DMA buffer within the channel whose current DMA will
        be aborted.
        
    @param
        is_working

        This a pointer to a BOOL value which will be filled with the state of
        the selected DMA channel. If set to TRUE, the DMA channel is currently
        processing a DMA request. If set to FALSE, the DMA channel has finished
        all requests and is waiting for further requests to be made.

    @param
        pending

        This a pointer to a uint32 value which is filled with the number of DMA
        requests queued-up and waiting to be processed. This value does NOT
        include the DMA request currently being processed.

    @param
        request_id

        This is the request id of the DMA request currently in progress, or
        zero if no DMA request is currently being processed.

*/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_DMA_Get_Status( struct SDM35541_Board_Descriptor * handle,
                                            UINT8 fb_num,
                                            UINT8 channel,
                                            UINT8 buffer_num,
                                            BOOL * is_working,
                                            uint32 * pending,
                                            uint32 * request_id);

 
/**
    @brief
        Installs a user callback function which is called whenever a DMA request
        is finished on the specified function block, channel, and buffer.

    @param
        handle

        A pointer to the board's descriptor.
        
    @param
        fb_num
        
        The function block number to install the callback function for.

    @param
        channel

        The DMA channel to install a callback function for.

    @param
        buffer_num
        
        The DMA buffer within the channel to install the callback function for.
        
    @param
        callback

        A function that will be called each time a DMA finishes on the given
        channel.

    @note
        The given callback function will be called in its own thread context.
        Appropriate synchronization to code and data is required.

    @param
        context_ptr

        A pointer that will be be available to the callback function.

*/              
SDM35541LIB_API
int WINAPI SDM35541_DMA_Install_Callback(struct SDM35541_Board_Descriptor * handle,
                                               UINT8 fb_num,
                                               UINT8  channel,
                                               UINT8 buffer_num,
                                               SDM35541_DMA_Callback callback,
											   LPVOID context_ptr);
 
/**
    @brief
        Removes the callback function associated with the given DMA channel.

    @param
        handle

        A pointer to the board's descriptor.
        
    @param
        fb_num
        
        The function block number to remove the DMA callback from.

    @param
        channel

        The DMA channel to remove the callback function from.
        
    @param
        buffer_num
        
        The DMA buffer number within the channel to remove the callback from.

*/ 
SDM35541LIB_API
int WINAPI SDM35541_DMA_Remove_Callback(struct SDM35541_Board_Descriptor * handle,
                                       UINT8 fb_num,
                                       UINT8  channel,
                                       UINT8 buffer_num);

/**
    @brief
        Retrieves the error message associated with a particular error code.

    @param
        Error

        The error code to get the message for.

    @param
        String

        A pointer to a buffer (with a minimum size of
        SDM35541_ERROR_MAX_MSG_LENGTH) which will be filled with the
        NULL-terminated error message.

    @param
        String_Lgth

        The length of the character string pointed to by String.

*/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_Get_Error_Msg(SDM35541_Error Error,
                                         char * String,
                                         unsigned String_Lgth);
                                                                                    
#ifdef __cplusplus
}
#endif

#endif
