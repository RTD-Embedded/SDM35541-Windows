/**
    @file

    @brief
        Defines for the SDM35541 (Device-specific values)

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

    $Id: SDM35541.h 151973 2026-02-05 14:08:03Z bkorpacz $
*/

#ifndef __SDM35541_H__
#define __SDM35541_H__

/**
 * @brief
 * SDM35541 PCI vendor ID
 */

#define SDM35541_PCI_VENDOR_ID	0x1435

/**
 * @brief
 *     SDM35541 PCI device ID
 */
#define SDM35541_PCI_DEVICE_ID	0x5441

/**
 * @brief
 *     Number of buffers per DMA channel
 */
#define SDM35541_NUM_DMA_BUFFERS		7

/**
 * @brief
 *      Maximum number of boards allowed per stack
 */
#define SDM35541_MAX_BOARDS             8

/**
 * @brief
 *     Sample size of the FIFO
 */
#define SDM35541_FIFO_SAMPLE_SIZE		8192

#endif
