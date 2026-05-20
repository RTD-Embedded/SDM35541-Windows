/**
    @file

    @brief
        This file implements the Windows-specific functions for the SDM35541Lib
        dll.

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

    $Id: librtd-SDM35541_os.c 141010 2024-01-29 17:02:26Z lfrankenfield $
*/


#include "SDM35541_board_access.h"
#include "SDM35541_os.h"
#include "SDM35541_error.h"
#include "RTD_Types.h"
#include <stdio.h>


/**
    @brief
        Returns the error code specific to the given IOCTL.

    @param
        ioctl

        The IOCTL to get an error code for.

    @internal

    @warning
        This is an internal function and should not be called directly.

*/
SDM35541_Error get_error_from_ioctl(DWORD ioctl)
{
    SDM35541_Error result;

    switch (ioctl)
    {
    case IOCTL_SDM35541_QUERY_BOARD:
        result = SDM35541_ERROR_QUERY_FAILURE;
        break;
    case IOCTL_SDM35541_BOARD_RESET:
        result = SDM35541_ERROR_RESET_FAILURE;
        break;
    case IOCTL_SDM35541_REGISTER_OP:
        result = SDM35541_ERROR_REG_OP_FAILURE;
        break;
    case IOCTL_SDM35541_INT_CONTROL:
        result = SDM35541_ERROR_INT_CONTROL_FAILURE;
        break;
    case IOCTL_SDM35541_INT_STATUS:
        result = SDM35541_ERROR_INT_STATUS_FAILURE;
        break;
    case IOCTL_SDM35541_DMA_CONTROL:
        result = SDM35541_ERROR_DMA_CONTROL_FAILURE;
        break;
    case IOCTL_SDM35541_DMA_STATUS:
        result = SDM35541_ERROR_DMA_STATUS_FAILURE;
        break;
    default:
        result = SDM35541_ERROR_UNEXPECTED;
        break;
    }

    return result;
}

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
        pointer to a SDM35541_Intrfc_* structure containing the parameters for
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
                        LPOVERLAPPED overlapped)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    DWORD num, res;
    OVERLAPPED ovrlp;
    HANDLE temp_handle;

    // If an overlapped structure was not provided, create one (and an event)
    if(overlapped == NULL)
    {
        // Set the overlap to zeros
        memset(&ovrlp, 0, sizeof(ovrlp));
        ovrlp.hEvent = CreateEvent(NULL, FALSE, FALSE, "");
        if(!ovrlp.hEvent)
        {
            return SDM35541_ERROR_CREATE_EVENT_FAILURE;
        }
        
        overlapped = &ovrlp;
    }
    else
    {
        temp_handle = overlapped->hEvent;
        memset(&overlapped, 0, sizeof(overlapped));
        overlapped->hEvent = temp_handle;
    }

    do
    {
        // Perform the IOCTL call
        if(!DeviceIoControl(handle->file_handle,
                            code,
                            buffer,
                            buff_size,
                            buffer,
                            buff_size,
                            &num,
                            overlapped))
        {
            
            res = GetLastError();
            if(res != ERROR_IO_PENDING)
            {
                SetLastError(res);
                result = get_error_from_ioctl(code);
                break;
            }
        }

        // If a local event was created, wait for it to be signaled
        if(overlapped == &ovrlp)
        {
            res = WaitForSingleObject(overlapped->hEvent, INFINITE);
            if(res != WAIT_OBJECT_0)
            {
                result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
                break;
            }

            if(!GetOverlappedResult(handle->file_handle, &ovrlp, &num, FALSE))
            {
                result = GetLastError();
                printf("OVERLAP ERROR: %d\r\n",result);
                result = get_error_from_ioctl(code);
                break;
            }
        }

    } while(0);

    // If a local event was created earlier, release it
    if(overlapped == &ovrlp)
    {
        if(!CloseHandle(ovrlp.hEvent))
        {
            result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
        }
    }

    return result;
}

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
                                SDM35541_EnumInfo * info)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    LONG err_val = ERROR_SUCCESS;
    TCHAR sub_key_name[255];
    DWORD max_key_len = 255;
    DWORD value_type, value_data, value_size = sizeof(DWORD);
    FILETIME last_write_time;
    HKEY sub_key;

    err_val = RegEnumKeyEx( enum_key,
                            index,
                            sub_key_name,
                            &max_key_len,
                            NULL,
                            NULL,
                            NULL,
                            &last_write_time);
    if(err_val == ERROR_NO_MORE_ITEMS)
    {
        return SDM35541_ERROR_NO_ERROR;
    }
    else if(err_val != ERROR_SUCCESS)
    {
        SetLastError(err_val);
        return SDM35541_ERROR_ENUM_FAILURE;
    }

    err_val = RegOpenKeyEx( enum_key,
                            sub_key_name,
                            0,
                            KEY_READ,
                            &sub_key);
    if(err_val != ERROR_SUCCESS)
    {
        SetLastError(err_val);
        return SDM35541_ERROR_ENUM_FAILURE;
    }

    // Read the key's values
    do
    {
        // Get the device number
        err_val = RegQueryValueEx(  sub_key,
                                    SDM35541_INTRFC_REGNAME_ANSI_DEV_NUM,
                                    NULL,
                                    &value_type,
                                    (LPBYTE)&value_data,
                                    &value_size);
        if(err_val != ERROR_SUCCESS)
        {
            SetLastError(err_val);
            result = SDM35541_ERROR_ENUM_FAILURE;
            break;
        }
        else if((value_type != REG_DWORD)   ||
                (value_size != sizeof(DWORD)))
        {
            result = SDM35541_ERROR_UNEXPECTED;
            break;
        }
        else
        {
            info->dev_num = (uint32)value_data;
        }

        // Get the availability
        err_val = RegQueryValueEx(  sub_key,
                                    SDM35541_INTRFC_REGNAME_ANSI_AVAIL,
                                    NULL,
                                    &value_type,
                                    (LPBYTE)&value_data,
                                    &value_size);
        if(err_val != ERROR_SUCCESS)
        {
            SetLastError(err_val);
            result = SDM35541_ERROR_ENUM_FAILURE;
            break;
        }
        else if((value_type != REG_DWORD)   ||
                (value_size != sizeof(DWORD)))
        {
            result = SDM35541_ERROR_UNEXPECTED;
            break;
        }
        else
        {
            info->avail = (uint32)value_data;
        }

        // Get the pci device id
        err_val = RegQueryValueEx(  sub_key,
                                    SDM35541_INTRFC_REGNAME_ANSI_DEVICE_ID,
                                    NULL,
                                    &value_type,
                                    (LPBYTE)&value_data,
                                    &value_size);
        if(err_val != ERROR_SUCCESS)
        {
            SetLastError(err_val);
            result = SDM35541_ERROR_ENUM_FAILURE;
            break;
        }
        else if((value_type != REG_DWORD)   ||
                (value_size != sizeof(DWORD)))
        {
            result = SDM35541_ERROR_UNEXPECTED;
            break;
        }
        else
        {
            info->pci_device_id = (uint32)value_data;
        }

        // Get the IRQ
        err_val = RegQueryValueEx(  sub_key,
                                    SDM35541_INTRFC_REGNAME_ANSI_IRQ_NUM,
                                    NULL,
                                    &value_type,
                                    (LPBYTE)&value_data,
                                    &value_size);
        if(err_val != ERROR_SUCCESS)
        {
            SetLastError(err_val);
            result = SDM35541_ERROR_ENUM_FAILURE;
            break;
        }
        else if((value_type != REG_DWORD)   ||
                (value_size != sizeof(DWORD)))
        {
            result = SDM35541_ERROR_UNEXPECTED;
            break;
        }
        else
        {
            info->irq = value_data;
        }

    } while(0);

    // Close the key
    err_val = RegCloseKey(sub_key);
    if(err_val != ERROR_SUCCESS)
    {
        SetLastError(err_val);
        result = SDM35541_ERROR_ENUM_FAILURE;
    }

    return result;
}

/******************************************************************************
    Enumerates the supported boards in the system.
******************************************************************************/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_General_Enum_Boards( SDM35541_EnumInfo * info,
                                                ULONG size)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    LONG err_val = ERROR_SUCCESS;
    HKEY enum_key;
    DWORD i;

    for(i = 0; i < size; ++i)
    {
        info[i].dev_num = 0;
        info[i].avail = 0;
        info[i].irq = 0;
    }

    err_val = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                            "SYSTEM\\CurrentControlSet\\Services\\SDM35541\\Enum",
                            0,
                            KEY_READ,
                            &enum_key);
    if(err_val != ERROR_SUCCESS)
    {
        SetLastError(err_val);
        return SDM35541_ERROR_ENUM_FAILURE;
    }

    for(i = 0; i < size; ++i)
    {
        result = enum_registry_key(enum_key, i, &(info[i]));
        if(result != SDM35541_ERROR_NO_ERROR)
        {
            break;
        }
    }

    err_val = RegCloseKey(enum_key);
    if(err_val != ERROR_SUCCESS)
    {
        SetLastError(err_val);
        return SDM35541_ERROR_ENUM_FAILURE;
    }

    return result;
}

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
SDM35541_Error pci_region_num_to_reg_op_block(enum SDM35541_pci_region_num pci_region, uint8 * block){
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;

    switch(pci_region){
        case SDM35541_PCI_REGION_GBC:
        case SDM35541_PCI_REGION_GBC2:
            *block = 0;
            break;
        case SDM35541_PCI_REGION_FB:
            *block = 1;
            break;
        default:
            result = SDM35541_ERROR_BAD_PARAMETER;
            *block = 0;
            break;
    }
    return result;
}

/**
    @brief
        Implements the main loop of the callback thread, waiting for
        notification and calling the user's function.

    @param
        thread_data

        This is a pointer to the data that was passed to
        interrupt_callback_thread().

    @param
        int_stat

        This is an array of structs used for calling the interrupt status IOCTL.

    @param
        overlapped

        This array of structures hold the events which are signaled when a
        notification occurs.

    @param
        result

        This is a pointer to a SDM35541_Error which is filled with the result of
        the function.

    @param
        last_error

        This is a pointer to a DWORD value which, in the case of an error, is
        filled with the value from GetLastError().

    @internal

    @warning
        This is an internal function and should not be called directly.

*/
BOOL WINAPI interrupt_callback_thread_core(
                SDM35541_Interrupt_Thread_Data* thread_data,
                SDM35541_Intrfc_Int_Status int_stat[SDM35541_NOTIFY_QUEUE_DEPTH],
                OVERLAPPED overlapped[SDM35541_NOTIFY_QUEUE_DEPTH],
                SDM35541_Error * result,
                DWORD * last_error)
{
    SDM35541_Interrupt_Callback_Info info;
    HANDLE wait_events[2];
    DWORD num, res, waitret;
    uint32 event_count = 0;

    wait_events[0] = overlapped[0].hEvent;
    wait_events[1] = thread_data->terminate_event;

    // Enter the main thread loop
    do
    {
        // Wait for the next notification event
        waitret = WaitForMultipleObjects(2, wait_events, FALSE, INFINITE);
        if(waitret == WAIT_OBJECT_0)
        {
            // Get the complete notification result
            if(!GetOverlappedResult(thread_data->device_file,
                                    &overlapped[event_count],
                                    &num,
                                    FALSE))
            {
                *result = SDM35541_ERROR_INT_STATUS_FAILURE;
                *last_error = GetLastError();

                // Try to signal the current event so the thread doesn't hang
                //  waiting for all events to be signalled on cleanup
                SetEvent(wait_events[0]);

                return FALSE;
            }

            // Fill in the data to be sent to the callback function
            info.result = SDM35541_ERROR_NO_ERROR;
            info.interrupts = int_stat[event_count].count;

            // Call the user's callback
            (thread_data->callback)(info);

            // Queue this notification event again
            if(!DeviceIoControl(thread_data->device_file,
                                IOCTL_SDM35541_INT_STATUS,
                                &int_stat[event_count],
                                sizeof(SDM35541_Intrfc_Int_Status),
                                &int_stat[event_count],
                                sizeof(SDM35541_Intrfc_Int_Status),
                                &num,
                                &overlapped[event_count]))
            {
                res = GetLastError();
                if(res != ERROR_IO_PENDING)
                {
                    *result = SDM35541_ERROR_INT_STATUS_FAILURE;
                    *last_error = res;
                    return FALSE;
                }
            }

            // Increment the event count in order to wait on the next event in
            //  the queue
            event_count++;
            if(event_count >= SDM35541_NOTIFY_QUEUE_DEPTH)
            {
                event_count = 0;
            }

            wait_events[0] = overlapped[event_count].hEvent;
        }
        else if(waitret == WAIT_OBJECT_0 + 1)
        {
            // The termination event was signaled, exit the thread
            break;
        }
        else
        {
            // Some type of error occurred, exit the thread
            *result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
            *last_error = GetLastError();
            break;
        }

    } while(TRUE);

    return TRUE;
}

/**
    @brief
        The function that is run in another thread to execute a user's interrupt
        callback.

    @param
        data

        A pointer to a SDM35541_Interrupt_Thread_Data structure holding the
        information needed by the thread. The space pointed to by this parameter
        was allocated on the heap by another function but it is free'd by this
        function.

    @internal

    @warning
        This is an internal function and should not be called directly.

*/
DWORD WINAPI interrupt_callback_thread(PVOID data)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    SDM35541_Interrupt_Thread_Data* thread_data =
                                        (SDM35541_Interrupt_Thread_Data*)data;
    SDM35541_Intrfc_Int_Status int_stat[SDM35541_NOTIFY_QUEUE_DEPTH];
    SDM35541_Intrfc_Int_Status int_stat_flush;
    SDM35541_Interrupt_Callback_Info info;
    OVERLAPPED overlapped[SDM35541_NOTIFY_QUEUE_DEPTH];
    OVERLAPPED flush_overlapped;
    HANDLE events[SDM35541_NOTIFY_QUEUE_DEPTH];
    HANDLE flush_event;
    DWORD num, res, last_error = 0;
    uint32 i = 0, event_count = 0, num_queued;

    do
    {
        // Prepare needed thread resources
        flush_event = NULL;
        memset(events, 0, sizeof(HANDLE) * SDM35541_NOTIFY_QUEUE_DEPTH);

        flush_event = CreateEvent(NULL, FALSE, FALSE, "");
        if(!flush_event)
        {
            last_error = GetLastError();
            result = SDM35541_ERROR_CREATE_EVENT_FAILURE;
            break;
        }

        for(i = 0; i < SDM35541_NOTIFY_QUEUE_DEPTH; i++)
        {
            events[i] = CreateEvent(NULL, FALSE, FALSE, "");
            if(!events[i])
            {
                last_error = GetLastError();
                result = SDM35541_ERROR_CREATE_EVENT_FAILURE;
                break;
            }
        }
        if(result != SDM35541_ERROR_NO_ERROR)
            break;

        // Fill in static values
        int_stat_flush.function_block_number = thread_data->fb_num;
        int_stat_flush.op = SDM35541_INTRFC_INT_STATUS_OP_CLEAR_QUEUE;

        memset(&flush_overlapped, 0, sizeof(OVERLAPPED));
        flush_overlapped.hEvent = flush_event;

        for(i = 0; i < SDM35541_NOTIFY_QUEUE_DEPTH; i++)
        {
            int_stat[i].function_block_number = thread_data->fb_num;
            int_stat[i].op = SDM35541_INTRFC_INT_STATUS_OP_NOTIFY;

            memset(&overlapped[i], 0, sizeof(OVERLAPPED));
            overlapped[i].hEvent = events[i];
        }

        do
        {
            // Queue up the notification requests
            for(num_queued = 0;
                num_queued < SDM35541_NOTIFY_QUEUE_DEPTH;
                num_queued++)
            {
                if(!DeviceIoControl(thread_data->device_file,
                                    IOCTL_SDM35541_INT_STATUS,
                                    &int_stat[num_queued],
                                    sizeof(SDM35541_Intrfc_Int_Status),
                                    &int_stat[num_queued],
                                    sizeof(SDM35541_Intrfc_Int_Status),
                                    &num,
                                    &overlapped[num_queued]))
                {
                    res = GetLastError();
                    if(res != ERROR_IO_PENDING)
                    {
                        result = SDM35541_ERROR_INT_STATUS_FAILURE;
                        last_error = res;
                        break;
                    }
                }
            }
            if(result != SDM35541_ERROR_NO_ERROR)
                break;


            // Mark that the thread is ready
            InterlockedExchange(&thread_data->status, 1);

            // Enter the main thread loop
            if(!interrupt_callback_thread_core( thread_data,
                                                int_stat,
                                                overlapped,
                                                &result,
                                                &last_error))
            {
                break;
            }

        } while(0);

        do
        {
            // Flush all queued notifications
            if(!DeviceIoControl(thread_data->device_file,
                                IOCTL_SDM35541_INT_STATUS,
                                &int_stat_flush,
                                sizeof(SDM35541_Intrfc_Int_Status),
                                &int_stat_flush,
                                sizeof(SDM35541_Intrfc_Int_Status),
                                &num,
                                &flush_overlapped))
            {
                res = GetLastError();
                if(res != ERROR_IO_PENDING)
                {
                    if(result == SDM35541_ERROR_NO_ERROR)
                    {
                        result = SDM35541_ERROR_DMA_STATUS_FAILURE;
                        last_error = res;
                    }
                    break;
                }
            }

            // Wait for the flush operation to complete
            res = WaitForSingleObject(flush_event, INFINITE);
            if(res != WAIT_OBJECT_0)
            {
                if(result == SDM35541_ERROR_NO_ERROR)
                {
                    result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
                    last_error = GetLastError();
                }
                break;
            }

            // Wait for the queued notification events to be signaled
            for(; num_queued > 0; num_queued--)
            {
                res = WaitForSingleObject(events[num_queued - 1], INFINITE);
                if(res != WAIT_OBJECT_0)
                {
                    if(result == SDM35541_ERROR_NO_ERROR)
                    {
                        result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
                        last_error = GetLastError();
                    }
                }
            }

        } while(0);

    } while(0);

    // If there was an error while preparing, set the status value to notify
    //  the install callback
    if(result != SDM35541_ERROR_NO_ERROR)
    {
        InterlockedExchange(&thread_data->status, 2);
    }

    // Clean-up the thread resources
    for(i = 0; i < SDM35541_NOTIFY_QUEUE_DEPTH; i++)
    {
        if(events[i] != NULL)
        {
            if(!CloseHandle(events[i]))
            {
                if(result == SDM35541_ERROR_NO_ERROR)
                {
                    result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
                    last_error = GetLastError();
                }
            }
        }
    }

    if(flush_event != NULL)
    {
        if(!CloseHandle(flush_event))
        {
            if(result == SDM35541_ERROR_NO_ERROR)
            {
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
                last_error = GetLastError();
            }
        }
    }

    // Close the handle opened in SDM35541_Interrupt_Install_Callback
    if(!CloseHandle(thread_data->terminate_event))
    {
        if(result == SDM35541_ERROR_NO_ERROR)
        {
            result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
            last_error = GetLastError();
        }
    }

    // If there was an error, call the user's callback function to report it
    if(result != SDM35541_ERROR_NO_ERROR)
    {
        if(last_error)
            SetLastError(last_error);

        info.result = result;

        (thread_data->callback)(info);
    }

    // Free the memory allocated in SDM35541_Interrupt_Install_Callback
    free(data);

    return 0;
}

/**
    @brief
        Implements the main loop of the callback thread, waiting for
        notification and calling the user's function.

    @param
        thread_data

        This is a pointer to the data that was passed to dma_callback_thread().

    @param
        dma_stat

        This is an array of structs used for calling the DMA status IOCTL.

    @param
        overlapped

        This array of structures hold the events which are signaled when a
        notification occurs.

    @param
        result

        This is a pointer to a SDM35541_Error which is filled with the result of
        the function.

    @param
        last_error

        This is a pointer to a DWORD value which, in the case of an error, is
        filled with the value from GetLastError().

    @internal

    @warning
        This is an internal function and should not be called directly.

*/
BOOL WINAPI dma_callback_thread_core(
                SDM35541_DMA_Thread_Data* thread_data,
                SDM35541_Intrfc_DMA_Status dma_stat[SDM35541_NOTIFY_QUEUE_DEPTH],
                OVERLAPPED overlapped[SDM35541_NOTIFY_QUEUE_DEPTH],
                SDM35541_Error * result,
                DWORD * last_error)
{
    SDM35541_DMA_Callback_Info info;
    HANDLE wait_events[2];
    DWORD num, res, waitret;
    uint32 event_count = 0;

    wait_events[0] = overlapped[0].hEvent;
    wait_events[1] = thread_data->terminate_event;

    // Enter the main thread loop
    do
    {
        // Wait for the next notification event
        waitret = WaitForMultipleObjects(2, wait_events, FALSE, INFINITE);
        if(waitret == WAIT_OBJECT_0)
        {
            // Get the complete notification result
            if(!GetOverlappedResult(thread_data->device_file,
                                    &overlapped[event_count],
                                    &num,
                                    FALSE))
            {
                *result = SDM35541_ERROR_DMA_STATUS_FAILURE;
                *last_error = GetLastError();

                // Try to signal the current event so the thread doesn't hang
                //  waiting for all events to be signalled on cleanup
                SetEvent(wait_events[0]);

                return FALSE;
            }

            // Fill in the data to be sent to the callback function
            info.result = SDM35541_ERROR_NO_ERROR;
            info.request_result = dma_stat[event_count].request_result;
            info.request_id = dma_stat[event_count].request_id;
            info.fb_num = dma_stat[event_count].function_block_number;
            info.channel = dma_stat[event_count].channel;
            info.buffer_num = dma_stat[event_count].buffer_num;
            info.bytes_transferred = dma_stat[event_count].bytes_transferred;
			info.context_ptr = thread_data->context_ptr;

            if(dma_stat[event_count].is_working)
                info.pending = dma_stat[event_count].pending + 1;
            else
                info.pending = 0;

            // Call the user's callback
            (thread_data->callback)(info);

            // Queue this notification event again
            if(!DeviceIoControl(thread_data->device_file,
                                IOCTL_SDM35541_DMA_STATUS,
                                &dma_stat[event_count],
                                sizeof(SDM35541_Intrfc_DMA_Status),
                                &dma_stat[event_count],
                                sizeof(SDM35541_Intrfc_DMA_Status),
                                &num,
                                &overlapped[event_count]))
            {
                res = GetLastError();
                if(res != ERROR_IO_PENDING)
                {
                    *result = SDM35541_ERROR_DMA_STATUS_FAILURE;
                    *last_error = res;
                    return FALSE;
                }
            }

            // Increment the event count in order to wait on the next event in
            //  the queue
            event_count++;
            if(event_count >= SDM35541_NOTIFY_QUEUE_DEPTH)
            {
                event_count = 0;
            }

            wait_events[0] = overlapped[event_count].hEvent;
        }
        else if(waitret == WAIT_OBJECT_0 + 1)
        {
            // The termination event was signaled, exit the thread
            break;
        }
        else
        {
            // Some type of error occurred, exit the thread
            *result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
            *last_error = GetLastError();
            break;
        }

    } while(TRUE);

    return TRUE;
}

/**
    @brief
        The function that is run in another thread to execute a user's DMA
        callback.

    @param
        data

        A pointer to a SDM35541_DMA_Thread_Data structure holding the
        information needed by the thread. The space pointed to by this parameter
        was allocated on the heap by another function but it is free'd by this
        function.

    @internal

    @warning
        This is an internal function and should not be called directly.

*/
DWORD WINAPI dma_callback_thread(PVOID data)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    SDM35541_DMA_Thread_Data* thread_data = (SDM35541_DMA_Thread_Data*)data;
    SDM35541_Intrfc_DMA_Status dma_stat[SDM35541_NOTIFY_QUEUE_DEPTH];
    SDM35541_Intrfc_DMA_Status dma_stat_flush;
    SDM35541_DMA_Callback_Info info;
    OVERLAPPED overlapped[SDM35541_NOTIFY_QUEUE_DEPTH];
    OVERLAPPED flush_overlapped;
    HANDLE events[SDM35541_NOTIFY_QUEUE_DEPTH];
    HANDLE flush_event;
    DWORD num, res, last_error = 0;
    uint32 i = 0, event_count = 0, num_queued;

    do
    {
        // Prepare needed thread resources
        flush_event = NULL;
        memset(events, 0, sizeof(HANDLE) * SDM35541_NOTIFY_QUEUE_DEPTH);

        flush_event = CreateEvent(NULL, FALSE, FALSE, "");
        if(!flush_event)
        {
            last_error = GetLastError();
            result = SDM35541_ERROR_CREATE_EVENT_FAILURE;
            break;
        }

        for(i = 0; i < SDM35541_NOTIFY_QUEUE_DEPTH; i++)
        {
            events[i] = CreateEvent(NULL, FALSE, FALSE, "");
            if(!events[i])
            {
                last_error = GetLastError();
                result = SDM35541_ERROR_CREATE_EVENT_FAILURE;
                break;
            }
        }
        if(result != SDM35541_ERROR_NO_ERROR)
            break;

        // Fill in static values
        dma_stat_flush.channel = thread_data->channel;
        dma_stat_flush.buffer_num = thread_data->buffer_num;
        dma_stat_flush.function_block_number = thread_data->fb_num;
        dma_stat_flush.op = SDM35541_INTRFC_DMA_STATUS_OP_CLEAR_QUEUE;

        memset(&flush_overlapped, 0, sizeof(OVERLAPPED));
        flush_overlapped.hEvent = flush_event;

        for(i = 0; i < SDM35541_NOTIFY_QUEUE_DEPTH; i++)
        {
            dma_stat[i].function_block_number = thread_data->fb_num;
            dma_stat[i].channel = thread_data->channel;
            dma_stat[i].buffer_num = thread_data->buffer_num;
            dma_stat[i].op = SDM35541_INTRFC_DMA_STATUS_OP_NOTIFY;

            memset(&overlapped[i], 0, sizeof(OVERLAPPED));
            overlapped[i].hEvent = events[i];
        }

        do
        {
            // Queue up the notification requests
            for(num_queued = 0;
                num_queued < SDM35541_NOTIFY_QUEUE_DEPTH;
                num_queued++)
            {
                if(!DeviceIoControl(thread_data->device_file,
                                    IOCTL_SDM35541_DMA_STATUS,
                                    &dma_stat[num_queued],
                                    sizeof(SDM35541_Intrfc_DMA_Status),
                                    &dma_stat[num_queued],
                                    sizeof(SDM35541_Intrfc_DMA_Status),
                                    &num,
                                    &overlapped[num_queued]))
                {
                    res = GetLastError();
                    if(res != ERROR_IO_PENDING)
                    {
                        result = SDM35541_ERROR_DMA_STATUS_FAILURE;
                        last_error = res;
                        break;
                    }
                }
            }
            if(result != SDM35541_ERROR_NO_ERROR)
                break;


            // Mark that the thread is ready
            InterlockedExchange(&thread_data->status, 1);

            // Enter the main thread loop
            if(!dma_callback_thread_core(   thread_data,
                                            dma_stat,
                                            overlapped,
                                            &result,
                                            &last_error))
            {
                break;
            }

        } while(0);

        do
        {
            // Flush all queued notifications
            if(!DeviceIoControl(thread_data->device_file,
                                IOCTL_SDM35541_DMA_STATUS,
                                &dma_stat_flush,
                                sizeof(SDM35541_Intrfc_DMA_Status),
                                &dma_stat_flush,
                                sizeof(SDM35541_Intrfc_DMA_Status),
                                &num,
                                &flush_overlapped))
            {
                res = GetLastError();
                if(res != ERROR_IO_PENDING)
                {
                    if(result == SDM35541_ERROR_NO_ERROR)
                    {
                        result = SDM35541_ERROR_DMA_STATUS_FAILURE;
                        last_error = res;
                    }
                    break;
                }
            }

            // Wait for the flush operation to complete
            res = WaitForSingleObject(flush_event, INFINITE);
            if(res != WAIT_OBJECT_0)
            {
                if(result == SDM35541_ERROR_NO_ERROR)
                {
                    result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
                    last_error = GetLastError();
                }
                break;
            }

            // Wait for the queued notification events to be signaled
            for(; num_queued > 0; num_queued--)
            {
                res = WaitForSingleObject(events[num_queued - 1], INFINITE);
                if(res != WAIT_OBJECT_0)
                {
                    if(result == SDM35541_ERROR_NO_ERROR)
                    {
                        result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
                        last_error = GetLastError();
                    }
                }
            }

        } while(0);

    } while(0);

    // If there was an error set the status value to notify the install callback
    //  if needed
    if(result != SDM35541_ERROR_NO_ERROR)
    {
        InterlockedExchange(&thread_data->status, 2);
    }

    // Clean-up the thread resources
    for(i = 0; i < SDM35541_NOTIFY_QUEUE_DEPTH; i++)
    {
        if(events[i] != NULL)
        {
            if(!CloseHandle(events[i]))
            {
                if(result == SDM35541_ERROR_NO_ERROR)
                {
                    result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
                    last_error = GetLastError();
                }
            }
        }
    }

    if(flush_event != NULL)
    {
        if(!CloseHandle(flush_event))
        {
            if(result == SDM35541_ERROR_NO_ERROR)
            {
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
                last_error = GetLastError();
            }
        }
    }

    // Close the handle opened in SDM35541_DMA_Install_Callback
    if(!CloseHandle(thread_data->terminate_event))
    {
        if(result == SDM35541_ERROR_NO_ERROR)
        {
            result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
            last_error = GetLastError();
        }
    }

    // If there was an error, call the user's callback function to report it
    if(result != SDM35541_ERROR_NO_ERROR)
    {
        if(last_error)
            SetLastError(last_error);

        info.result = result;

        (thread_data->callback)(info);
    }

    // Free the memory allocated in SDM35541_DMA_Install_Callback
    free(data);

    return 0;
}


/**
    @brief
        Validates an Interrupt Type value.

    @param
        type

        The value to validate.

    @retval
        0

        The value passed in is valid for this type.

    @retval
        -1

        The value passed in is NOT valid for this type.

    @internal

    @warning
        This is an internal function and should not be called directly.

*/
UINT8 validate_fb_num(UINT8 fb_num)
{
    if(fb_num < SDM35541_MAX_FB)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/******************************************************************************
    Installs a callback routine for a specific interrupt type.
******************************************************************************/
SDM35541LIB_API
int WINAPI SDM35541_Interrupt_Install_Callback(struct SDM35541_Board_Descriptor * handle,
                                              struct SDM35541_Function_Block * fb,
                                              SDM35541_Interrupt_Callback callback)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    HANDLE term_event, thread;
    ULONG thread_id;
    SDM35541_Interrupt_Thread_Data* thread_data;
    LONG thread_status = 0;
    DWORD res;

    if(!handle)
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }
    
    if(validate_fb_num(fb->fb_num))
    {
        return SDM35541_ERROR_BAD_PARAMETER;    
    }

    thread_data = (SDM35541_Interrupt_Thread_Data*)malloc(
                                        sizeof(SDM35541_Interrupt_Thread_Data));
    if(!thread_data)
    {
        return SDM35541_ERROR_INSUFFICIENT_MEMORY;
    }

    do
    {

        term_event = CreateEvent(NULL, TRUE, FALSE, "");
        if(term_event == NULL)
        {
            result = SDM35541_ERROR_CREATE_EVENT_FAILURE;
            break;
        }

        do
        {
            // Fill thread data
            thread_data->fb_num = fb->fb_num;
            thread_data->device_file = handle->file_handle;
            thread_data->terminate_event = term_event;
            thread_data->callback = callback;

            InterlockedExchange(&thread_data->status, 0);

            handle->int_terminate_events[fb->fb_num] = term_event;

            // Create thread
            thread = CreateThread(  0,
                                    0,
                                    interrupt_callback_thread,
                                    (void *) thread_data,
                                    0,
                                    &thread_id);
            if(thread == NULL)
            {
                handle->int_terminate_events[fb->fb_num] = NULL;
                result = SDM35541_ERROR_CREATE_THREAD_FAILURE;
                break;
            }

        } while(0);

        if(result != SDM35541_ERROR_NO_ERROR)
        {
            if(!CloseHandle(term_event))
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
        }

    } while(0);

    if(result != SDM35541_ERROR_NO_ERROR)
        free(thread_data);

    // If the thread was created, wait for it to be ready
    if(result == SDM35541_ERROR_NO_ERROR)
    {
        while(thread_status == 0)
        {
            // Give the thread a chance to initialize
            Sleep(0);

            // Check the thread status value atomically
            thread_status = InterlockedExchangeAdd(&thread_data->status, 0);
        }

        // Check the thread status
        if(thread_status == 1)
        {
            // Success, store the thread handle
            handle->int_thread_handles[fb->fb_num] = thread;
        }
        else if(thread_status == 2)
        {
            result = SDM35541_ERROR_INTERNAL_THREAD_FAILURE;

            // The thread has errored and is closing so wait for it
            res = WaitForSingleObject(thread, 2000);
            if(res == WAIT_OBJECT_0)
            {
                // The thread already finished, just close the thread handle
                if(!CloseHandle(handle->int_thread_handles[fb->fb_num]))
                    result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
            }
            else if(res != WAIT_TIMEOUT)
            {
                result = SDM35541_ERROR_WAIT_EVENT_FAILURE;

                // There was an error checking for the thread so try and clean
                //  up before returning the error
                if(!CloseHandle(handle->int_thread_handles[fb->fb_num]))
                    result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
            }
            else
            {
                result = SDM35541_ERROR_THREAD_STUCK;
            }

            handle->int_terminate_events[fb->fb_num] = NULL;
            handle->int_thread_handles[fb->fb_num] = NULL;
        }
        else
        {
            // Unknown thread status
            result = SDM35541_ERROR_UNEXPECTED;
        }
    }

    return result;
}

/******************************************************************************
    Removes the callback routine of a specific interrupt type.
******************************************************************************/
SDM35541LIB_API
int WINAPI SDM35541_Interrupt_Remove_Callback(struct SDM35541_Board_Descriptor * handle,
                                             struct SDM35541_Function_Block * fb)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    DWORD res;

    if(!handle)
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(validate_fb_num(fb->fb_num))
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(handle->int_terminate_events[fb->fb_num] != NULL)
    {
        // Check if the thread has already exited from an error
        res = WaitForSingleObject(handle->int_thread_handles[fb->fb_num], 0);
        if(res == WAIT_OBJECT_0)
        {
            result = SDM35541_ERROR_INTERNAL_THREAD_FAILURE;

            // The thread already finished, just close the thread handle
            if(!CloseHandle(handle->int_thread_handles[fb->fb_num]))
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
        }
        else if(res != WAIT_TIMEOUT)
        {
            result = SDM35541_ERROR_WAIT_EVENT_FAILURE;

            // There was an error checking for the thread so try and clean up
            //  before returning the error
            if(SetEvent(handle->int_terminate_events[fb->fb_num]) == 0)
                result = SDM35541_ERROR_STOP_THREAD_FAILURE;

            if(!CloseHandle(handle->int_thread_handles[fb->fb_num]))
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
        }
        else
        {
            // Tell the thread to clean up and exit
            if(SetEvent(handle->int_terminate_events[fb->fb_num]) == 0)
            {
                result = SDM35541_ERROR_STOP_THREAD_FAILURE;
            }
            else
            {
                // Wait for the thread to finish
                res = WaitForSingleObject(
                                        handle->int_thread_handles[fb->fb_num],
                                        1000);
                if(res == WAIT_TIMEOUT)
                {
                    result = SDM35541_ERROR_THREAD_STUCK;
                }
                else if(res != WAIT_OBJECT_0)
                {
                    result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
                }

                if(!CloseHandle(handle->int_thread_handles[fb->fb_num]))
                    result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
            }
        }

        handle->int_terminate_events[fb->fb_num] = NULL;
        handle->int_thread_handles[fb->fb_num] = NULL;
    }

    return result;
}

/**
    Helper function to bounds check the DMA channel number
    specified when setting up a user-mode DMA callback.
*/
BOOL validate_dma_channel(UINT8 dma_channel_number)
{
    if(dma_channel_number > MAX_DMA_CHANNELS)
        return FALSE;
    
    return TRUE;    
}

/******************************************************************************
    Requests a DMA transfer to or from one of the board's fifos.
******************************************************************************/
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
                                        SDM35541_Intrfc_DMA_Result * dma_result)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    SDM35541_Intrfc_DMA_Control dma_control;
    OVERLAPPED ovrlp;
    DWORD num, res;

    // Don't bother to validate the operation or source, they are validated here
    if(!validate_dma_channel(channel))
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(!handle->board_info.is_bus_master)
    {
        return SDM35541_ERROR_NOT_BUS_MASTER;
    }


    // Fill out the control struct
    dma_control.channel = channel;
    dma_control.function_block_number = fb_num;
    dma_control.buffer_num = dma_buffer_number;
    dma_control.request_id = request_id;
    dma_control.incr_local = FALSE;
    dma_control.external = address;
    dma_control.do_demand = demand;
    dma_control.timeout = timeout;
    dma_control.access_size = SDM35541_INTRFC_REG_SIZE_32;
    dma_control.request_size = size;

    switch (operation)
    {
    case SDM35541_DMA_OP_BOARD_TO_BUFFER:
        dma_control.op = SDM35541_INTRFC_DMA_OP_TO_BUFFER;
        break;
    case SDM35541_DMA_OP_BUFFER_TO_BOARD:
        dma_control.op = SDM35541_INTRFC_DMA_OP_FROM_BUFFER;
        break;
    case SDM35541_DMA_OP_BOARD_TO_ADDRESS:
        dma_control.op = SDM35541_INTRFC_DMA_OP_TO_ADDRESS;
        break;
    case SDM35541_DMA_OP_ADDRESS_TO_BOARD:
        dma_control.op = SDM35541_INTRFC_DMA_OP_FROM_ADDRESS;
        break;
    default:
        return SDM35541_ERROR_BAD_PARAMETER;
        break;
    }

    if(wait)
        dma_control.return_immediate = FALSE;
    else
        dma_control.return_immediate = TRUE;


    if(!wait)
    {
        // If the user is not waiting for this DMA, just call the ioctl
        result = call_ioctl(handle,
                            IOCTL_SDM35541_DMA_CONTROL,
                            &dma_control,
                            sizeof(SDM35541_Intrfc_DMA_Control),
                            NULL);
    }
    else
    {
        // If the user is waiting for this DMA, first create the needed event
        memset(&ovrlp, 0, sizeof(OVERLAPPED));

        ovrlp.hEvent = CreateEvent(NULL, FALSE, FALSE, "");
        if(!ovrlp.hEvent)
        {
            result = SDM35541_ERROR_CREATE_EVENT_FAILURE;
        }
        else
        {
            // Call the IOCTL to make the request
            result = call_ioctl(handle,
                                IOCTL_SDM35541_DMA_CONTROL,
                                &dma_control,
                                sizeof(SDM35541_Intrfc_DMA_Control),
                                &ovrlp);

            // Wait for the request to complete
            res = WaitForSingleObject(ovrlp.hEvent, INFINITE);
            if(res != WAIT_OBJECT_0)
            {
                result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
            }

            // Get the ioctl result, close the event handle and return the dma
            //  request result
            if(!GetOverlappedResult(handle->file_handle, &ovrlp, &num, FALSE))
            {
                result = SDM35541_ERROR_DMA_CONTROL_FAILURE;
            }

            if(!CloseHandle(ovrlp.hEvent))
            {
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
            }

            if( result == SDM35541_ERROR_NO_ERROR &&
                dma_result != NULL)
            {
                *dma_result = dma_control.request_result;
            }
        }
    }

    return result;
}

/******************************************************************************
    Aborts the current DMA request on the selected DMA channel.
******************************************************************************/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_DMA_Abort( struct SDM35541_Board_Descriptor * handle,
                                        UINT8 fb_num,
                                        UINT8 channel,
                                        UINT8 buffer_num)
{
    SDM35541_Intrfc_DMA_Control dma_control;

    if(!validate_dma_channel(channel))
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(!handle->board_info.is_bus_master)
    {
        return SDM35541_ERROR_NOT_BUS_MASTER;
    }

    memset(&dma_control, '\0', sizeof(SDM35541_Intrfc_DMA_Control));

    dma_control.function_block_number = fb_num;
    dma_control.channel = channel;
    dma_control.buffer_num = buffer_num;
    dma_control.op = SDM35541_INTRFC_DMA_OP_ABORT;


    return call_ioctl(  handle,
                        IOCTL_SDM35541_DMA_CONTROL,
                        &dma_control,
                        sizeof(SDM35541_Intrfc_DMA_Control),
                        NULL);
}

/******************************************************************************
    Aborts/cancels the current DMA request, and all queued requests, on the
    selected DMA channel.
******************************************************************************/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_DMA_Flush( struct SDM35541_Board_Descriptor * handle,
                                        UINT8 fb_num,
                                        UINT8 channel,
                                        UINT8 buffer_num)
{
    SDM35541_Intrfc_DMA_Control dma_control;

    if(!validate_dma_channel(channel))
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(!handle->board_info.is_bus_master)
    {
        return SDM35541_ERROR_NOT_BUS_MASTER;
    }

    memset(&dma_control, '\0', sizeof(SDM35541_Intrfc_DMA_Control));

    dma_control.function_block_number = fb_num;
    dma_control.channel = channel;
    dma_control.buffer_num = buffer_num;
    dma_control.op = SDM35541_INTRFC_DMA_OP_FLUSH;

    return call_ioctl(  handle,
                        IOCTL_SDM35541_DMA_CONTROL,
                        &dma_control,
                        sizeof(SDM35541_Intrfc_DMA_Control),
                        NULL);
}

/******************************************************************************
    Aborts/cancels all DMA requests and all queued requests.
******************************************************************************/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_DMA_Flush_All( struct SDM35541_Board_Descriptor * handle)
{
    SDM35541_Intrfc_DMA_Control dma_control;
    
    if(!handle->board_info.is_bus_master)
    {
        return SDM35541_ERROR_NOT_BUS_MASTER;
    }

    memset(&dma_control, '\0', sizeof(SDM35541_Intrfc_DMA_Control));

    dma_control.function_block_number = 0;
    dma_control.channel = 0;
    dma_control.buffer_num = 0;
    dma_control.op = SDM35541_INTRFC_DMA_OP_FLUSH_ALL;

    return call_ioctl(  handle,
                        IOCTL_SDM35541_DMA_CONTROL,
                        &dma_control,
                        sizeof(SDM35541_Intrfc_DMA_Control),
                        NULL);

}

/******************************************************************************
    Retrieves the current state of a DMA channel.
******************************************************************************/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_DMA_Get_Status( struct SDM35541_Board_Descriptor * handle,
                                            UINT8 fb_num,
                                            UINT8 channel,
                                            UINT8 buffer_num,
                                            BOOL * is_working,
                                            uint32 * pending,
                                            uint32 * request_id)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    SDM35541_Intrfc_DMA_Status dma_status;

    if(!validate_dma_channel(channel))
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(!handle->board_info.is_bus_master)
    {
        return SDM35541_ERROR_NOT_BUS_MASTER;
    }

    memset(&dma_status, '\0', sizeof(SDM35541_Intrfc_DMA_Status));

    dma_status.function_block_number = fb_num;
    dma_status.channel = channel;
    dma_status.buffer_num = buffer_num;
    dma_status.op = SDM35541_INTRFC_DMA_STATUS_OP_POLL;

    result = call_ioctl(handle,
                        IOCTL_SDM35541_DMA_STATUS,
                        &dma_status,
                        sizeof(SDM35541_Intrfc_DMA_Status),
                        NULL);
    if(result != SDM35541_ERROR_NO_ERROR)
    {
        return result;
    }

    if(is_working)
    {
        *is_working = dma_status.is_working;
    }

    if(pending)
    {
        *pending = dma_status.pending;
    }

    if(request_id)
    {
        *request_id = dma_status.request_id;
    }

    return SDM35541_ERROR_NO_ERROR;
}


/******************************************************************************
    Installs a user callback function which is called whenever a DMA request
    is finished on the specified channel.
******************************************************************************/
SDM35541LIB_API
int WINAPI SDM35541_DMA_Install_Callback(struct SDM35541_Board_Descriptor * handle,
                                               UINT8 fb_num,
                                               UINT8 channel,
                                               UINT8 buffer_num,
                                               SDM35541_DMA_Callback callback,
											   LPVOID context_ptr)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    HANDLE term_event, thread;
    ULONG thread_id;
    SDM35541_DMA_Thread_Data* thread_data;
    LONG thread_status = 0;
    DWORD res;

    if(!handle)
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(!validate_dma_channel(channel))
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(!handle->board_info.is_bus_master)
    {
        return SDM35541_ERROR_NOT_BUS_MASTER;
    }

    thread_data = (SDM35541_DMA_Thread_Data*)malloc(
                                                sizeof(SDM35541_DMA_Thread_Data));
    if(!thread_data)
    {
        return SDM35541_ERROR_INSUFFICIENT_MEMORY;
    }

    do
    {
        term_event = CreateEvent(NULL, TRUE, FALSE, "");
        if(term_event == NULL)
        {
            result = SDM35541_ERROR_CREATE_EVENT_FAILURE;
            break;
        }

        do
        {
            // Fill thread data
            thread_data->fb_num = fb_num;
            thread_data->channel = channel;
            thread_data->buffer_num = buffer_num;
            thread_data->device_file = handle->file_handle;
            thread_data->terminate_event = term_event;
            thread_data->callback = callback;
			thread_data->context_ptr = context_ptr;

            InterlockedExchange(&thread_data->status, 0);

            handle->dma_terminate_events[fb_num][channel][buffer_num] = term_event;

            // Create thread
            thread = CreateThread(  0,
                                    0,
                                    dma_callback_thread,
                                    (void *) thread_data,
                                    0,
                                    &thread_id);
            if(thread == NULL)
            {
                handle->dma_terminate_events[fb_num][channel][buffer_num] = NULL;
                result = SDM35541_ERROR_CREATE_THREAD_FAILURE;
                break;
            }

        } while(0);

        if(result != SDM35541_ERROR_NO_ERROR)
        {
            if(!CloseHandle(term_event))
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
        }

    } while(0);

    if(result != SDM35541_ERROR_NO_ERROR)
        free(thread_data);


    // If the thread was created, wait for it to be ready
    if(result == SDM35541_ERROR_NO_ERROR)
    {
        while(thread_status == 0)
        {
            // Give the thread a chance to initialize
            Sleep(0);

            // Check the thread status value atomically
            thread_status = InterlockedExchangeAdd(&thread_data->status, 0);
        }

        // Check the thread status
        if(thread_status == 1)
        {
            // Success, store the thread handle
            handle->dma_thread_handles[fb_num][channel][buffer_num] = thread;
        }
        else if(thread_status == 2)
        {
            result = SDM35541_ERROR_INTERNAL_THREAD_FAILURE;

            // The thread has errored and is closing so wait for it
            res = WaitForSingleObject(thread, 2000);
            if(res == WAIT_OBJECT_0)
            {
                // The thread already finished, just close the thread handle
                if(!CloseHandle(handle->dma_thread_handles[fb_num][channel][buffer_num]))
                    result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
            }
            else if(res != WAIT_TIMEOUT)
            {
                result = SDM35541_ERROR_WAIT_EVENT_FAILURE;

                // There was an error checking for the thread so try and clean
                //  up before returning the error
                if(!CloseHandle(handle->dma_thread_handles[fb_num][channel][buffer_num]))
                    result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
            }
            else
            {
                result = SDM35541_ERROR_THREAD_STUCK;
            }

            handle->dma_terminate_events[fb_num][channel][buffer_num] = NULL;
            handle->dma_thread_handles[fb_num][channel][buffer_num] = NULL;
        }
        else
        {
            // Unknown thread status
            result = SDM35541_ERROR_UNEXPECTED;
        }
    }

    return result;
}

/******************************************************************************
    Removes the callback function associated with the given DMA channel.
******************************************************************************/
SDM35541LIB_API
int WINAPI SDM35541_DMA_Remove_Callback(struct SDM35541_Board_Descriptor * handle,
                                       UINT8 fb_num,
                                       UINT8  channel,
                                       UINT8 buffer_num)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    DWORD res;

    if(!handle)
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(!validate_dma_channel(channel))
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(!handle->board_info.is_bus_master)
    {
        return SDM35541_ERROR_NOT_BUS_MASTER;
    }

    if(handle->dma_terminate_events[fb_num][channel][buffer_num] != NULL)
    {
        // Check if the thread has already exited from an error
        res = WaitForSingleObject(handle->dma_thread_handles[fb_num][channel][buffer_num], 0);
        if(res == WAIT_OBJECT_0)
        {
            result = SDM35541_ERROR_INTERNAL_THREAD_FAILURE;

            // The thread already finished, just close the thread handle
            if(!CloseHandle(handle->dma_thread_handles[fb_num][channel][buffer_num]))
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
        }
        else if(res != WAIT_TIMEOUT)
        {
            result = SDM35541_ERROR_WAIT_EVENT_FAILURE;

            // There was an error checking for the thread so try and clean up
            //  before returning the error
            if(SetEvent(handle->dma_terminate_events[fb_num][channel][buffer_num]) == 0)
                result = SDM35541_ERROR_STOP_THREAD_FAILURE;

            if(!CloseHandle(handle->dma_thread_handles[fb_num][channel][buffer_num]))
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
        }
        else
        {
            // Tell the thread to clean up and exit
            if(SetEvent(handle->dma_terminate_events[fb_num][channel][buffer_num]) == 0)
            {
                result = SDM35541_ERROR_STOP_THREAD_FAILURE;
            }
            else
            {
                // Wait for the thread to finish
                res = WaitForSingleObject(  handle->dma_thread_handles[fb_num][channel][buffer_num],
                                            1000);
                if(res == WAIT_TIMEOUT)
                {
                    result = SDM35541_ERROR_THREAD_STUCK;
                }
                else if(res != WAIT_OBJECT_0)
                {
                    result = SDM35541_ERROR_WAIT_EVENT_FAILURE;
                }

                if(!CloseHandle(handle->dma_thread_handles[fb_num][channel][buffer_num]))
                    result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
            }
        }

        handle->dma_terminate_events[fb_num][channel][buffer_num] = NULL;
        handle->dma_thread_handles[fb_num][channel][buffer_num] = NULL;
    }

    return result;
}

/******************************************************************************
    Retrieves the error message associated with a particular error code.
*******************************************************************************/
SDM35541LIB_API
SDM35541_Error WINAPI SDM35541_Get_Error_Msg(SDM35541_Error Error,
                                         char * String,
                                         unsigned String_Lgth)
{
    char messages[SDM35541_ERROR_COUNT][SDM35541_ERROR_MAX_MSG_LENGTH] =
                                                        SDM35541_ERROR_STRINGS;

    if(!String || Error >= SDM35541_ERROR_COUNT)
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }

    if(strcpy_s(String, String_Lgth, messages[Error]) != 0)
        return SDM35541_ERROR_UNEXPECTED;

    return SDM35541_ERROR_NO_ERROR;
}

   