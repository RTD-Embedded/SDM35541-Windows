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

    $Id: librtd-SDM35541_board_access.c 138769 2023-08-10 14:06:34Z lfrankenfield $
*/

#include <windows.h>
#include "SDM35541_board_access.h"
#include "SDM35541_dma_library.h"
#include "SDM35541_error.h"
#include "SDM35541_os.h"
#include "RTD_Types.h"
#include <stdio.h>



/**
    @brief
        Performs a read-modify-write on a register on the board.

    @param
        handle

        A pointer to the board's descriptor.

    @param
        region

        Standard PCI Region Number.

    @param
        offset

        Register Offset.

    @param
        data

        Value to write to the register.

    @param
        mask

        A '1' in a bit position indicates it is to be modified. A '0' indicates
        that that bit value should not be changed. A mask of all 1's is the same
        as a write.

    @internal

    @warning
        This is an internal function and should not be called directly.

*/
int SDM35541_Modify(struct SDM35541_Board_Descriptor *handle,
	               union SDM35541_ioctl_argument *ioctl_request)
{
    int status = SDM35541_ERROR_NO_ERROR;
    SDM35541_Intrfc_Reg_Op reg_op;

    // make sure we have a valid handle
    if(!handle)
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }
    
    reg_op.op = SDM35541_INTRFC_REG_OP_SAFEWRITE;
    reg_op.offset = ioctl_request->modify.access.offset;
    status = pci_region_num_to_reg_op_block(ioctl_request->modify.access.region,&reg_op.block);
    if(status != SDM35541_ERROR_NO_ERROR)
    {
        return status;
    }
    
    
    // determine what size write this is and get the appropriate field in the union
    switch(ioctl_request->modify.access.size)
    {
        case SDM35541_PCI_REGION_ACCESS_8:
            reg_op.size = SDM35541_INTRFC_REG_SIZE_8;
            reg_op.data = ioctl_request->modify.access.data.data8;
            reg_op.mask = ioctl_request->modify.mask.mask8;
            break;
        case SDM35541_PCI_REGION_ACCESS_16:
            reg_op.size = SDM35541_INTRFC_REG_SIZE_16;
            reg_op.data = ioctl_request->modify.access.data.data16;
            reg_op.mask = ioctl_request->modify.mask.mask16;
            break;
        case SDM35541_PCI_REGION_ACCESS_32:
        default:
            reg_op.size = SDM35541_INTRFC_REG_SIZE_32;
            reg_op.data = ioctl_request->modify.access.data.data32;
            reg_op.mask = ioctl_request->modify.mask.mask32;
            break;          
    }
    
    // Send the IOCTL
    return call_ioctl(  handle,
                        IOCTL_SDM35541_REGISTER_OP,
                        &reg_op,
                        sizeof(SDM35541_Intrfc_Reg_Op),
                        NULL);
}
   
   
/**
    @brief
        Performs a write to a register on the board.

    @param
        handle

        A pointer to the board's descriptor.

    @param
        region

        Standard PCI Region Number.

    @param
        offset

        Register Offset.

    @param
        data

        Value to write to the register.

    @internal

    @warning
        This is an internal function and should not be called directly.

*/   
int SDM35541_Write(struct SDM35541_Board_Descriptor *handle,
	              union SDM35541_ioctl_argument *ioctl_request)
{
    int status = SDM35541_ERROR_NO_ERROR;
    SDM35541_Intrfc_Reg_Op reg_op;
    
    // make sure we have a valid handle
    if(!handle)
    {
        return SDM35541_ERROR_BAD_PARAMETER;
    }
    
    // set up the SDM35541_Intrfc_Reg_Op used by the Windows driver
    reg_op.op = SDM35541_INTRFC_REG_OP_WRITE;
    reg_op.offset = ioctl_request->readwrite.access.offset;
    status = pci_region_num_to_reg_op_block(ioctl_request->modify.access.region,&reg_op.block);
    if(status != SDM35541_ERROR_NO_ERROR)
    {
        return status;
    }
    
    // determine what size write this is and get the appropriate field in the union
    switch(ioctl_request->readwrite.access.size)
    {
        case SDM35541_PCI_REGION_ACCESS_8:
            reg_op.size = SDM35541_INTRFC_REG_SIZE_8;
            reg_op.data = ioctl_request->readwrite.access.data.data8;
            break;
        case SDM35541_PCI_REGION_ACCESS_16:
            reg_op.size = SDM35541_INTRFC_REG_SIZE_16;
            reg_op.data = ioctl_request->readwrite.access.data.data16;
            break;
        case SDM35541_PCI_REGION_ACCESS_32:
        default:
            reg_op.size = SDM35541_INTRFC_REG_SIZE_32;
            reg_op.data = ioctl_request->readwrite.access.data.data32;
            break;          
    }
    
    // Send the IOCTL
    return call_ioctl(handle,
                      IOCTL_SDM35541_REGISTER_OP,
                      &reg_op,
                      sizeof(SDM35541_Intrfc_Reg_Op),
                      NULL);
}  

/**
    @brief
        Performs a read from a register on the board.

    @param
        handle

        A pointer to the board's descriptor.

    @param
        region

        Standard PCI Region Number.

    @param
        offset

        Register Offset.

    @param
        data

        Address of a variable to store the data read from the register.

    @internal

    @warning
        This is an internal function and should not be called directly.

*/   
int SDM35541_Read(struct SDM35541_Board_Descriptor *handle,
	             union SDM35541_ioctl_argument *ioctl_request)
{
    int status = SDM35541_ERROR_NO_ERROR;
    SDM35541_Intrfc_Reg_Op reg_op;
    
    do
    {
        // make sure we have a valid handle
        if(!handle)
        {
            status = SDM35541_ERROR_BAD_PARAMETER;
            break;
        }
        
        // set up the SDM35541_Intrfc_Reg_Op used by the Windows driver
        reg_op.op = SDM35541_INTRFC_REG_OP_READ;
        reg_op.offset = ioctl_request->readwrite.access.offset;
        status = pci_region_num_to_reg_op_block(ioctl_request->modify.access.region,&reg_op.block);
        if(status != SDM35541_ERROR_NO_ERROR)
        {
            return status;
        }
        
        // determine what size read this is
        switch(ioctl_request->readwrite.access.size)
        {
            case SDM35541_PCI_REGION_ACCESS_8:
                reg_op.size = SDM35541_INTRFC_REG_SIZE_8;
                break;
            case SDM35541_PCI_REGION_ACCESS_16:
                reg_op.size = SDM35541_INTRFC_REG_SIZE_16;
                break;
            case SDM35541_PCI_REGION_ACCESS_32:
            default:
                reg_op.size = SDM35541_INTRFC_REG_SIZE_32;
                break;          
        }
    
        status = call_ioctl(handle,
                            IOCTL_SDM35541_REGISTER_OP,
                            &reg_op,
                            sizeof(SDM35541_Intrfc_Reg_Op),
                            NULL);
    
        if(status == SDM35541_ERROR_NO_ERROR)
        {
            // copy the data from the SDM35541_Intrfc_Reg_Op to the SDM35541_ioctl_argument
            switch(ioctl_request->readwrite.access.size)
            {
                case SDM35541_PCI_REGION_ACCESS_8:
                    ioctl_request->readwrite.access.data.data8 = reg_op.data;
                    break;
                case SDM35541_PCI_REGION_ACCESS_16:
                    ioctl_request->readwrite.access.data.data16 = reg_op.data;
                    break;
                case SDM35541_PCI_REGION_ACCESS_32:
                default:
                    ioctl_request->readwrite.access.data.data32 = reg_op.data;
                    break;          
            }
        }
    }while(0);
    return status;
}

SDM35541LIB_API
int SDM35541_Board_Open(uint8_t dev_num,
                       struct SDM35541_Board_Descriptor **handle)
{
    SDM35541_Error result = SDM35541_ERROR_NO_ERROR;
    char dev_name[30];
    SDM35541_EnumInfo devinfo[50];
    struct SDM35541_Board_Descriptor * new_descriptor;

    *handle = NULL;
    memset(devinfo, '\0', 50 * sizeof(SDM35541_EnumInfo));

    // Enum boards
    result = SDM35541_General_Enum_Boards(devinfo, 50);
    if(result != SDM35541_ERROR_NO_ERROR)
        return result;

    if(devinfo[dev_num].avail != 3)
    {
        return SDM35541_ERROR_OPEN_FAILURE;
    }

    // Allocate space for the board descriptor structure
    new_descriptor = (struct SDM35541_Board_Descriptor*)malloc(
                                            sizeof(struct SDM35541_Board_Descriptor));
    if(new_descriptor == NULL)
    {
        return SDM35541_ERROR_INSUFFICIENT_MEMORY;
    }

    do
    {
        // Set entire descriptor to NULL (zero)
        memset(new_descriptor, '\0', sizeof(struct SDM35541_Board_Descriptor));

        // Open the board
        if(sprintf_s(   dev_name,
                        30,
                        "\\\\.\\RTD_SDM35541_%d",
                        devinfo[dev_num].dev_num) <= 0)
        {
            result = SDM35541_ERROR_UNEXPECTED;
            break;
        }

        new_descriptor->file_handle = CreateFile(
                                                dev_name,
                                                GENERIC_READ | GENERIC_WRITE,
                                                0,
                                                NULL,
                                                OPEN_EXISTING,
                                                FILE_FLAG_OVERLAPPED,
                                                0);
        if(new_descriptor->file_handle == INVALID_HANDLE_VALUE)
        {
            result = SDM35541_ERROR_OPEN_FAILURE;
            break;
        }

        do
        {
            
            if(result != SDM35541_ERROR_NO_ERROR)
            {
                break;
            }
        
            // Query the board's configuration
            result = call_ioctl(new_descriptor,
                                IOCTL_SDM35541_QUERY_BOARD,
                                &(new_descriptor->board_info),
                                sizeof(SDM35541_Intrfc_Board_Info),
                                NULL);
            if(result != SDM35541_ERROR_NO_ERROR)
            {
                break;
            }

            *handle = new_descriptor;

        } while(0);

        if(result != SDM35541_ERROR_NO_ERROR)
        {
            if(!CloseHandle(new_descriptor->file_handle))
                result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
        }

    } while(0);

    if(result != SDM35541_ERROR_NO_ERROR)
        free(new_descriptor);

    return result;
}

SDM35541LIB_API
int SDM35541_Board_Close(struct SDM35541_Board_Descriptor *handle)
{
    int result = SDM35541_ERROR_NO_ERROR;
    int temp_result = SDM35541_ERROR_NO_ERROR;

    // Cleanup DMA
    if( (handle->board_info.irq_number != 0) &&
        handle->board_info.is_bus_master)
    {
        // perform a flush-all on the DMA requests
        SDM35541_DMA_Flush_All(handle);
    }

    if(temp_result != SDM35541_ERROR_NO_ERROR)
        result = temp_result;   
    
    if(!CloseHandle(handle->file_handle))
    {
        result = SDM35541_ERROR_CLOSE_HANDLE_FAILURE;
    }

    free(handle);

    return result;
}

