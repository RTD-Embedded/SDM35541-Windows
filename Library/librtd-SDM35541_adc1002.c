/**
    @file

    @brief
        SDM35541 user library source code

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

    $Id: librtd-SDM35541_adc1002.c 152296 2026-02-18 14:41:33Z bkorpacz $
*/

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>

#include "SDM35541_registers.h"
#include "SDM35541_adc1002_library.h"
#include "SDM35541_board_access_structs.h"
#include "SDM35541_board_access.h"
#include "SDM35541_dma_library.h"

#include "SDM35541_os.h"
#include "SDM35541_util_library.h"

/**
 * @defgroup SDM35541_Library_Source SDM35541 user library source code
 * @{
 */
/*=============================================================================
Private functions
 =============================================================================*/
/**
 * @brief
 * Dummy Value used for some registers
 */
#define NO_ARG 					0x00

/**
 * @brief
 * 		Maximum number of SDM35541 boards allowed
 * 		in a stack
 */
#define MAX_NUM_DEVICES			8

/**
 * @defgroup SDM35541_Library_Private_Functions SDM35541 user library source code private functions
 * @{
 */

 /**
  * @internal
  */

/**
 * @brief
 * 		Each index represents a DM75xx DMA channel number
 * 		and each value is a MURAL DMA channel
 */
static int dma_channel_remap[2] = {-1, -1};

/**
 * Converts a DM75xx DMA channel number to a mural DMA channel
 * 
 * @param
 * 		channel
 * 
 * 		DM75xx DMA channel number (0 or 1)
 * 
 * @return
 * 		MURAL DMA channel number [0-3]
 * 
 * 		-1 if MURAL DMA channel was not initialized
 */
static inline SDM35541_dma_source_t dma_old_to_new(unsigned int channel) {
	return dma_channel_remap[channel];
}


/**
*******************************************************************************
@brief
    Validate a DSP command

@param
    command

    The DSP Command to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      request is not valid

@note
    This function will consider DMA channel 0 and 1 interrupt sources as invalid
    as these sources are enabled by default.
 *******************************************************************************
 */
static int validate_dsp_command(SDM35541_dsp_command_t command)
{
	switch (command) {
	case SDM35541_DSP_CAL_AUTO:
	case SDM35541_DSP_FLASH_DOWNLOAD:
	case SDM35541_DSP_USER_RUN:
	case SDM35541_DSP_USER_UPGRADE:
	case SDM35541_DSP_INT_FLASH_ERASE:
	case SDM35541_DSP_EXT_FLASH_ERASE:
	case SDM35541_DSP_ATTENTION:
	case SDM35541_DSP_CAL_DEFAULT:
	case SDM35541_DSP_CAL_SET:
	case SDM35541_DSP_CAL_VERSION:
	case SDM35541_DSP_BOOT_VERSION:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}



/**
*******************************************************************************
@brief
    Validate the DMA demand mode request source.

@param
    request

    The request source to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      request is not valid
 *******************************************************************************
 */
static int validate_dma_request(SDM35541_dma_request_t request)
{
	switch (request) {
	case SDM35541_DMA_DEMAND_DISABLE:
	case SDM35541_DMA_DEMAND_SCNT_ADC:
	case SDM35541_DMA_DEMAND_SCNT_DAC1:
	case SDM35541_DMA_DEMAND_SCNT_DAC2:
	case SDM35541_DMA_DEMAND_UTC1:
	case SDM35541_DMA_DEMAND_FIFO_ADC:
	case SDM35541_DMA_DEMAND_FIFO_DAC1:
	case SDM35541_DMA_DEMAND_FIFO_DAC2:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the DMA local source.

@param
    source

    The source to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      source is not valid
 *******************************************************************************
 */
static int validate_dma_source(SDM35541_dma_source_t source)
{
	switch (source) {
	case SDM35541_DMA_FIFO_ADC:
	case SDM35541_DMA_FIFO_DAC1:
	case SDM35541_DMA_FIFO_DAC2:
	case SDM35541_DMA_FIFO_HSDIN:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the DMA channel.

@param
    channel

    The channel to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      channel is not valid
 *******************************************************************************
 */
static int validate_dma_channel(SDM35541_dma_channel_t channel)
{
	switch (channel) {
	case 0:
	case 1:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the user output selection.

@param
    select

    The user output selection to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      selection is not valid
 *******************************************************************************
 */
static int validate_uio_select(SDM35541_uio_source_t source)
{
	switch (source) {
	case SDM35541_UIO_ADC:
	case SDM35541_UIO_DAC1:
	case SDM35541_UIO_DAC2:
	case SDM35541_UIO_PRG:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the external trigger polarity.

@param
    polarity

    The polarity to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      polarity is not valid
 *******************************************************************************
 */
static int validate_ext_polarity(SDM35541_ext_polarity_t polarity)
{
	switch (polarity) {
	case SDM35541_EXT_POLARITY_POS:
	case SDM35541_EXT_POLARITY_NEG:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the syncbus source.

@param
    src

    The syncbus source to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      source is not valid
 *******************************************************************************
 */
static int validate_sbus_source(SDM35541_sbus_src_t src)
{
	switch (src) {
	case SDM35541_SBUS_SRC_SOFT_ADC:
	case SDM35541_SBUS_SRC_PCLK:
	case SDM35541_SBUS_SRC_BCLK:
	case SDM35541_SBUS_SRC_DIG_IT:
	case SDM35541_SBUS_SRC_ETRIG:
	case SDM35541_SBUS_SRC_DAC_UPDATE:
	case SDM35541_SBUS_SRC_DAC_CLK:
	case SDM35541_SBUS_SRC_UTC2:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the highspeed digital input signal.

@param
    signal

    The signal to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      signal is not valid
 *******************************************************************************
 */
static int validate_hsdin_signal(SDM35541_hsdin_signal_t signal)
{
	switch (signal) {
	case SDM35541_HSDIN_SIGNAL_SOFTWARE:
	case SDM35541_HSDIN_SIGNAL_ADC:
	case SDM35541_HSDIN_SIGNAL_UTC0:
	case SDM35541_HSDIN_SIGNAL_UTC1:
	case SDM35541_HSDIN_SIGNAL_UTC2:
	case SDM35541_HSDIN_SIGNAL_EPCLK:
	case SDM35541_HSDIN_SIGNAL_ETRIG:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the burst clock primary frequency.

@param
    freq

    The frequency to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      frequency is not valid
 *******************************************************************************
 */
static int validate_bclk_freq(SDM35541_bclk_freq_t freq)
{
	switch (freq) {
	case SDM35541_BCLK_FREQ_8_MHZ:
	case SDM35541_BCLK_FREQ_20_MHZ:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the burst clock start trigger.

@param
    start

    The start trigger to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      start trigger is not valid
 *******************************************************************************
 */
static int validate_bclk_start(SDM35541_bclk_start_t start)
{
	switch (start) {
	case SDM35541_BCLK_START_SOFTWARE:
	case SDM35541_BCLK_START_PACER:
	case SDM35541_BCLK_START_ETRIG:
	case SDM35541_BCLK_START_DIG_IT:
	case SDM35541_BCLK_START_SBUS0:
	case SDM35541_BCLK_START_SBUS1:
	case SDM35541_BCLK_START_SBUS2:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the digital to analog primary frequency.

@param
    freq

    The frequency to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      frequency is not valid
 *******************************************************************************
 */
static int validate_dac_freq(SDM35541_dac_freq_t freq)
{
	switch (freq) {
	case SDM35541_DAC_FREQ_8_MHZ:
	case SDM35541_DAC_FREQ_20_MHZ:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the digital to analog channel.

@param
    dac

    The dac to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      dac is not valid
 *******************************************************************************
 */
static int validate_dac_channel(SDM35541_dac_channel_t dac)
{
	switch (dac) {
	case SDM35541_DAC1:
	case SDM35541_DAC2:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the digital to analog clock mode.

@param
    clk_mode

    The mode to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      clk_mode is not valid
 *******************************************************************************
 */
static int validate_dac_clk_mode(SDM35541_dac_clk_mode_t clk_mode)
{
	switch (clk_mode) {
	case SDM35541_DAC_CLK_FREE_RUN:
	case SDM35541_DAC_CLK_START_STOP:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the digital to analog clock stop trigger.

@param
    dac_clk_stop

    The digital to analog clock stop trigger to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      dac_clk_stop is not valid
 *******************************************************************************
 */
static int validate_dac_clk_stop(SDM35541_dac_clk_stop_t dac_clk_stop)
{
	switch (dac_clk_stop) {
	case SDM35541_DAC_CLK_STOP_SOFTWARE_PACER:
	case SDM35541_DAC_CLK_STOP_ETRIG:
	case SDM35541_DAC_CLK_STOP_DIG_IT:
	case SDM35541_DAC_CLK_STOP_UTC2:
	case SDM35541_DAC_CLK_STOP_SBUS0:
	case SDM35541_DAC_CLK_STOP_SBUS1:
	case SDM35541_DAC_CLK_STOP_SBUS2:
	case SDM35541_DAC_CLK_STOP_SOFTWARE:
	case SDM35541_DAC_CLK_STOP_DAC1_UCNT:
	case SDM35541_DAC_CLK_STOP_DAC2_UCNT:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the digital to analog clock start trigger.

@param
    dac_clk_start

    The digital to analog clock start trigger to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      dac_clk_start is not valid
 *******************************************************************************
 */
static int validate_dac_clk_start(SDM35541_dac_clk_start_t dac_clk_start)
{
	switch (dac_clk_start) {
	case SDM35541_DAC_CLK_START_SOFTWARE_PACER:
	case SDM35541_DAC_CLK_START_ETRIG:
	case SDM35541_DAC_CLK_START_DIG_IT:
	case SDM35541_DAC_CLK_START_UTC2:
	case SDM35541_DAC_CLK_START_SBUS0:
	case SDM35541_DAC_CLK_START_SBUS1:
	case SDM35541_DAC_CLK_START_SBUS2:
	case SDM35541_DAC_CLK_START_SOFTWARE:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the digital to analog mode.

@param
    dac_mode

    The mode to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      dac_mode is not valid
 *******************************************************************************
 */
static int validate_dac_mode(SDM35541_dac_channel_t dac, SDM35541_dac_mode_t dac_mode)
{
	if (validate_dac_channel(dac)) {
		return -1;
	}
	switch (dac_mode) {
	case SDM35541_DAC_MODE_NOT_CYCLE:
	case SDM35541_DAC_MODE_CYCLE:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the digital to analog update source.

@param
    dac_update_src

    The digital to analog update source to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      dac_update_src is not valid
 *******************************************************************************
 */
static int validate_dac_update_source(SDM35541_dac_update_src_t dac_update_src)
{

	switch (dac_update_src) {
	case SDM35541_DAC_UPDATE_SOFTWARE:
	case SDM35541_DAC_UPDATE_CGT:
	case SDM35541_DAC_UPDATE_CLOCK:
	case SDM35541_DAC_UPDATE_EPCLK:
	case SDM35541_DAC_UPDATE_SBUS0:
	case SDM35541_DAC_UPDATE_SBUS1:
	case SDM35541_DAC_UPDATE_SBUS2:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the digital to analog range.

@param
    dac_range

    The digital to analog range to validate.

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      dac_range is not valid
 *******************************************************************************
 */
static int validate_dac_range(SDM35541_dac_range_t dac_range)
{
	switch (dac_range) {
	case SDM35541_DAC_RANGE_UNIPOLAR_5:
	case SDM35541_DAC_RANGE_UNIPOLAR_10:
	case SDM35541_DAC_RANGE_BIPOLAR_5:
	case SDM35541_DAC_RANGE_BIPOLAR_10:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the analog to digital sample counter source.

@param
    src

    The analog to digital sample counter source to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      src is not valid
 *******************************************************************************
 */
static int validate_adc_scnt_src(SDM35541_adc_scnt_src_t src)
{
	switch (src) {
	case SDM35541_ADC_SCNT_SRC_CGT:
	case SDM35541_ADC_SCNT_SRC_FIFO:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the analog to digital conversion signal.

@param
    adc_conv_signal

    The analog to digital conversion signal to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      adc_conv_signal is not valid
 *******************************************************************************
 */
static int validate_adc_conv_signal(SDM35541_adc_conv_signal_t adc_conv_signal)
{
	switch (adc_conv_signal) {
	case SDM35541_ADC_CONV_SIGNAL_SOFTWARE:
	case SDM35541_ADC_CONV_SIGNAL_PCLK:
	case SDM35541_ADC_CONV_SIGNAL_BCLK:
	case SDM35541_ADC_CONV_SIGNAL_DIG_IT:
	case SDM35541_ADC_CONV_SIGNAL_DAC1_MRKR1:
	case SDM35541_ADC_CONV_SIGNAL_DAC2_MRKR1:
	case SDM35541_ADC_CONV_SIGNAL_SBUS0:
	case SDM35541_ADC_CONV_SIGNAL_SBUS1:
	case SDM35541_ADC_CONV_SIGNAL_SBUS2:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the pacer clock mode.

@param
    pclk_mode

    The pacer clock mode to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      pclk_mode is not valid
 *******************************************************************************
 */
static int validate_pclk_mode(SDM35541_pclk_mode_t pclk_mode)
{
	switch (pclk_mode) {
	case SDM35541_PCLK_NO_REPEAT:
	case SDM35541_PCLK_REPEAT:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the pacer clock stop trigger.

@param
    pclk_stop

    The pacer clock stop trigger to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      pclk_stop is not valid
 *******************************************************************************
 */
static int validate_pclk_stop(SDM35541_pclk_stop_t pclk_stop)
{
	switch (pclk_stop) {
	case SDM35541_PCLK_STOP_SOFTWARE:
	case SDM35541_PCLK_STOP_ETRIG:
	case SDM35541_PCLK_STOP_DIGITAL_IT:
	case SDM35541_PCLK_STOP_ACNT:
	case SDM35541_PCLK_STOP_UTC2:
	case SDM35541_PCLK_STOP_SBUS0:
	case SDM35541_PCLK_STOP_SBUS1:
	case SDM35541_PCLK_STOP_SBUS2:
	case SDM35541_PCLK_STOP_ASOFTWARE:
	case SDM35541_PCLK_STOP_AETRIG:
	case SDM35541_PCLK_STOP_ADIGITAL_IT:
	case SDM35541_PCLK_STOP_RES:
	case SDM35541_PCLK_STOP_AUTC2:
	case SDM35541_PCLK_STOP_ASBUS0:
	case SDM35541_PCLK_STOP_ASBUS1:
	case SDM35541_PCLK_STOP_ASBUS2:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the pacer clock start trigger.

@param
    pclk_start

    The pacer clock start trigger to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      pclk_start is not valid
 *******************************************************************************
 */
static int validate_pclk_start(SDM35541_pclk_start_t pclk_start)
{
	switch (pclk_start) {
	case SDM35541_PCLK_START_SOFTWARE:
	case SDM35541_PCLK_START_ETRIG:
	case SDM35541_PCLK_START_DIGITAL_IT:
	case SDM35541_PCLK_START_UTC2:
	case SDM35541_PCLK_START_SBUS0:
	case SDM35541_PCLK_START_SBUS1:
	case SDM35541_PCLK_START_SBUS2:
	case SDM35541_PCLK_START_RES:
	case SDM35541_PCLK_START_DSOFTWARE:
	case SDM35541_PCLK_START_DETRIG:
	case SDM35541_PCLK_START_DDIGITAL_IT:
	case SDM35541_PCLK_START_DUTC2:
	case SDM35541_PCLK_START_DSBUS0:
	case SDM35541_PCLK_START_DSBUS1:
	case SDM35541_PCLK_START_DSBUS2:
	case SDM35541_PCLK_START_ETRIG_GATE:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the pacer clock select.

@param
    pclk_select

    The pacer clock selection to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      pclk_select is not valid
 *******************************************************************************
 */
static int validate_pclk_select(SDM35541_pclk_select_t pclk_select)
{
	switch (pclk_select) {
	case SDM35541_PCLK_EXTERNAL:
	case SDM35541_PCLK_INTERNAL:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the pacer clock primary frequency.

@param
    freq_select

    The pacer clock primary frequency to validate

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      freq_select is not valid
 *******************************************************************************
 */
static int validate_pclk_freq(SDM35541_pclk_freq_t freq_select)
{
	switch (freq_select) {
	case SDM35541_PCLK_FREQ_8_MHZ:
	case SDM35541_PCLK_FREQ_20_MHZ:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the user timer/counter clock source.

@param
    utc_select

    The user timer/counter selected.

@param
    source

    The user timer/counter clock source to validate.

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      utc_select is not valid

        @arg \c
            EINVAL      source is not valid
 *******************************************************************************
 */
static int validate_utc_clock_source(SDM35541_utc_timer_t utc_select,
			      SDM35541_utc_clk_t source)
{
	switch (utc_select) {
	case SDM35541_UTC_0:
		switch (source) {
		case SDM35541_CUTC_8_MHZ:
		case SDM35541_CUTC_EXT_TC_CLOCK_1:
		case SDM35541_CUTC_EXT_TC_CLOCK_2:
		case SDM35541_CUTC_EXT_PCLK:
			break;
		default:
			errno = EINVAL;
			return -1;
			break;
		}
		break;
	case SDM35541_UTC_1:
		switch (source) {
		case SDM35541_CUTC_8_MHZ:
		case SDM35541_CUTC_EXT_TC_CLOCK_1:
		case SDM35541_CUTC_EXT_TC_CLOCK_2:
		case SDM35541_CUTC_EXT_PCLK:
		case SDM35541_CUTC_UTC_0_OUT:
		case SDM35541_CUTC_HSDIN_SIGNAL:
			break;
		default:
			errno = EINVAL;
			return -1;
			break;
		}
		break;
	case SDM35541_UTC_2:

		switch (source) {
		case SDM35541_CUTC_8_MHZ:
		case SDM35541_CUTC_EXT_TC_CLOCK_1:
		case SDM35541_CUTC_EXT_TC_CLOCK_2:
		case SDM35541_CUTC_EXT_PCLK:
		case SDM35541_CUTC_UTC_1_OUT:
			break;
		default:
			errno = EINVAL;
			return -1;
			break;
		}
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the user timer/counter gate value.

@param
    utc_select

    The user timer/counter selected.

@param
    gate

    The gate value to validate.

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      utc_select is not valid

        @arg \c
            EINVAL      gate is not valid
 *******************************************************************************
 */
static int validate_utc_gate(SDM35541_utc_timer_t utc_select, SDM35541_utc_gate gate)
{
	switch (utc_select) {
	case SDM35541_UTC_0:
		switch (gate) {
		case SDM35541_GUTC_NOT_GATED:
		case SDM35541_GUTC_GATED:
		case SDM35541_GUTC_EXT_TC_CLK_1:
		case SDM35541_GUTC_EXT_TC_CLK_2:
			break;
		default:
			errno = EINVAL;
			return -1;
			break;
		}
		break;
	case SDM35541_UTC_1:
		switch (gate) {
		case SDM35541_GUTC_NOT_GATED:
		case SDM35541_GUTC_GATED:
		case SDM35541_GUTC_EXT_TC_CLK_1:
		case SDM35541_GUTC_EXT_TC_CLK_2:
		case SDM35541_GUTC_UTC_0_OUT:
			break;
		default:
			errno = EINVAL;
			return -1;
			break;
		}
		break;
	case SDM35541_UTC_2:
		switch (gate) {
		case SDM35541_GUTC_NOT_GATED:
		case SDM35541_GUTC_GATED:
		case SDM35541_GUTC_EXT_TC_CLK_1:
		case SDM35541_GUTC_EXT_TC_CLK_2:
		case SDM35541_GUTC_UTC_0_OUT:
			break;
		default:
			errno = EINVAL;
			return -1;
			break;
		}
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    Validate the user timer/counter mode.

@param
    utc_select

    The user timer/counter selected.

@param
    mode

    The user timer/counter mode to validate.

@return
    0

    Success

@return
    -1

    Failure.@n@n

    errno may be set as follows:
        @arg \c
            EINVAL      utc_select is not valid

        @arg \c
            EINVAL      mode is not valid
 *******************************************************************************
 */
static int validate_utc_mode(SDM35541_utc_timer_t utc_select, SDM35541_utc_mode mode)
{
	switch (utc_select) {
	case SDM35541_UTC_0:
	case SDM35541_UTC_1:
	case SDM35541_UTC_2:
		break;
	default:
		errno = EINVAL;
		return -1;
	}
	switch (mode) {
	case SDM35541_UTC_MODE_EVENT_COUNTER:
	case SDM35541_UTC_MODE_PROG_ONE_SHOT:
	case SDM35541_UTC_MODE_RATE_GENERATOR:
	case SDM35541_UTC_MODE_SQUARE_WAVE:
	case SDM35541_UTC_MODE_SOFTWARE_STROBE:
	case SDM35541_UTC_MODE_HARDWARE_STROBE:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return 0;
}

/**
*******************************************************************************
@brief
    32 bit write access to a SDM35541 register.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    offset

    Register to access.

 @param
    data

    Data to write to the register

 @return
    0

    Success.

 @return
    -1

    Failure.
 *******************************************************************************
 */
static int
SDM35541_Write_Register_32(struct SDM35541_Board_Descriptor * handle,
			 const struct SDM35541_Function_Block *func_block,
			 uint16_t offset, uint32_t data)
{
	union SDM35541_ioctl_argument ioctl_request;

	ioctl_request.readwrite.access.offset = func_block->control_offset + offset;
	ioctl_request.readwrite.access.region = SDM35541_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = SDM35541_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = data;

	return SDM35541_Write(handle, &ioctl_request);
}

/**
*******************************************************************************
 @brief
    32 bit read access to a SDM35541 register.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    offset

    Register to access.

 @param
    data

    Address where to store the read data.

 @return
    0

    Success.

 @return
    -1

    Failure.
 *******************************************************************************
 */
static int
SDM35541_Read_Register_32(struct SDM35541_Board_Descriptor * handle,
			 const struct SDM35541_Function_Block *func_block,
             uint16_t offset, uint32_t * data)
{
	union SDM35541_ioctl_argument ioctl_request;
    int result;

	ioctl_request.readwrite.access.offset = func_block->control_offset + offset;
	ioctl_request.readwrite.access.region = SDM35541_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = SDM35541_PCI_REGION_ACCESS_32;
    
	result = SDM35541_Read(handle, &ioctl_request);

	*data = ioctl_request.readwrite.access.data.data32;

	return result;
}

/**
*******************************************************************************
 @brief
    16 bit write access to a SDM35541 register.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    offset

    Register to access.

 @param
    data

    Data to write to the register

 @return
    0

    Success.

 @return
    -1

    Failure.
 *******************************************************************************
 */
static int
SDM35541_Write_Register_16(struct SDM35541_Board_Descriptor * handle,
			 const struct SDM35541_Function_Block *func_block,
			 uint16_t offset, uint16_t data)
{
	union SDM35541_ioctl_argument ioctl_request;

	ioctl_request.readwrite.access.region = SDM35541_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->control_offset + offset;
	ioctl_request.readwrite.access.size = SDM35541_PCI_REGION_ACCESS_16;
	ioctl_request.readwrite.access.data.data16 = data;
	
	return SDM35541_Write(handle, &ioctl_request);
}

/**
*******************************************************************************
 @brief
    16 bit read access to a SDM35541 register.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    offset

    Register to access.

 @param
    data

    Address where to store the read data.

 @return
    0

    Success.

 @return
    -1

    Failure.
 *******************************************************************************
 */
static int
SDM35541_Read_Register_16(struct SDM35541_Board_Descriptor * handle,
			 const struct SDM35541_Function_Block *func_block,
			uint16_t offset, uint16_t * data)
{
	union SDM35541_ioctl_argument ioctl_request;
    int result;

	ioctl_request.readwrite.access.region = SDM35541_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->control_offset + offset;
	ioctl_request.readwrite.access.size = SDM35541_PCI_REGION_ACCESS_16;
	
	result = SDM35541_Read(handle, &ioctl_request);

	*data = ioctl_request.readwrite.access.data.data16;
	return result;
}

/**
*******************************************************************************
 @brief
    8 bit write access to a SDM35541 register.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    offset

    Register to access.

 @param
    data

    Data to write to the register

 @return
    0

    Success.

 @return
    -1

    Failure.
 *******************************************************************************
 */
static int
SDM35541_Write_Register_8(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
			uint16_t offset, uint8_t data)
{
	union SDM35541_ioctl_argument ioctl_request;
	ioctl_request.readwrite.access.region = SDM35541_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->control_offset + offset;
	ioctl_request.readwrite.access.size = SDM35541_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = data;
	
    return SDM35541_Write(handle, &ioctl_request);
}

/**
*******************************************************************************
 @brief
    8 bit read access to a SDM35541 register.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    offset

    Register to access.

 @param
    data

    Address where to store the read value.

 @return
    0

    Success.

 @return
    -1

    Failure.
 *******************************************************************************
 */
static int
SDM35541_Read_Register_8(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t offset, uint8_t * data)
{
	union SDM35541_ioctl_argument ioctl_request;
    int result;

	ioctl_request.readwrite.access.region = SDM35541_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->control_offset + offset;
	ioctl_request.readwrite.access.size = SDM35541_PCI_REGION_ACCESS_8;
	
    result = SDM35541_Read(handle, &ioctl_request);

	*data = ioctl_request.readwrite.access.data.data8;

	return result;
}

/**
*******************************************************************************
 @brief
    8 bit read-modify-write access to a SDM35541 register.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    offset

    Register to access.

 @param
    data

    Data to write to the register

 @param
    mask

    Mask of the bits to be modified.

 @return
    0

    Success.

 @return
    -1

    Failure.
 *******************************************************************************
 */
static int
SDM35541_Modify_Register_8(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
			 uint16_t offset, uint8_t data, uint8_t mask)
{
	union SDM35541_ioctl_argument ioctl_request;
	ioctl_request.modify.access.region = SDM35541_PCI_REGION_FB;
	ioctl_request.modify.access.offset = func_block->control_offset + offset;
	ioctl_request.modify.access.size = SDM35541_PCI_REGION_ACCESS_8;
	ioctl_request.modify.access.data.data8 = data;
	ioctl_request.modify.mask.mask8 = mask;
	
	return SDM35541_Modify(handle, &ioctl_request);
}

/**
*******************************************************************************
 @brief
    16 bit read-modify-write access to a SDM35541 register.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    offset

    Register to access.

 @param
    data

    Data to write to the register

 @param
    mask

    Mask of the bits not to be modified.

 @return
    0

    Success.

 @return
    -1

    Failure.
 *******************************************************************************
 */
static int
SDM35541_Modify_Register_16(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t offset, uint16_t data, uint16_t mask)
{
	union SDM35541_ioctl_argument ioctl_request;
	ioctl_request.modify.access.region = SDM35541_PCI_REGION_FB;
	ioctl_request.modify.access.offset = func_block->control_offset + offset;
	ioctl_request.modify.access.size = SDM35541_PCI_REGION_ACCESS_16;
	ioctl_request.modify.access.data.data16 = data;
	ioctl_request.modify.mask.mask16 = mask;
	
    return SDM35541_Modify(handle, &ioctl_request);
}

/**
*******************************************************************************
@brief
    32 bit read-modify-write access to a SDM35541 register.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    offset

    Register to access.

 @param
    data

    Data to write to the register

 @param
    mask

    Mask of the bits not to be modified.

 @return
    0

    Success.

 @return
    -1

    Failure.
 *******************************************************************************
 */
static int
SDM35541_Modify_Register_32(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t offset, uint32_t data, uint32_t mask)
{
	union SDM35541_ioctl_argument ioctl_request;
	ioctl_request.modify.access.region = SDM35541_PCI_REGION_FB;
	ioctl_request.modify.access.offset = func_block->control_offset + offset;
	ioctl_request.modify.access.size = SDM35541_PCI_REGION_ACCESS_32;
	ioctl_request.modify.access.data.data32 = data;
	ioctl_request.modify.mask.mask32 = mask;
	
    return SDM35541_Modify(handle, &ioctl_request);
}

/**
 * @} SDM35541_Library_Private_Functions
 */
/**
 * @} SDM35541_Library_Source
 */

/*=============================================================================
Public functions
 =============================================================================*/

/******************************************************************************
 * Gets the SDM35541_IT_OVERRUN register
 ******************************************************************************/
SDM35541LIB_API
 int SDM35541_Get_IT_Overrun(struct SDM35541_Board_Descriptor *handle,
			const struct SDM35541_Function_Block *func_block,
			uint16_t *reg)
{
	return SDM35541_Read_Register_16(handle, func_block, SDM35541_IT_OVERRUN, reg);
}


/******************************************************************************
 * Clears the SDM35541_IT_OVERRUN register
 ******************************************************************************/
int SDM35541_Clear_IT_Mask(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_int_source_t mask)
{
    int result;
	uint16_t dummy;
	
	result = SDM35541_Write_Register_16(handle, func_block, SDM35541_CLEAR_IT, mask);

	result |= SDM35541_Read_Register_16(handle, func_block, SDM35541_CLEAR_IT, &dummy);

	return result;
}


/******************************************************************************
 * Clears the SDM35541_INT register
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Set_IT_Mask(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_int_source_t mask)
{
	/*
	 * Enumeration out of range
	 */
	if (mask > 0xffff) {
		return -1;
	}

    return SDM35541_Write_Register_16(handle, func_block, SDM35541_INT, mask);
}


/******************************************************************************
 * Enables SDM35541's DMA/DAC/ADC/DIO interrupts
 * This function must be called after SDM35541_DMA_Set_Source
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Interrupt_Enable(struct SDM35541_Board_Descriptor *handle,
			const struct SDM35541_Function_Block *func_block,
			SDM35541_int_source_t mask)
{
	uint8_t algdio_ints;
	uint32_t dma_ints;
	int result = 0;

	/*
	 * Grab Analog DIO Mask Register
	 */
	SDM35541_ALGDIO_Get_IRQ_Status(handle, func_block, &algdio_ints);

	algdio_ints = (mask >> 24) & 0x3c;

	/**
	 * Update ALGDIO INT register
	 */
	SDM35541_ALGDIO_Set_IRQ_Status(handle, func_block, algdio_ints);

	/*
	 * Enable DMA channel 0 and/or 1
	 */
	dma_ints = (mask & 0x00600000);

	if (dma_ints & SDM35541_INT_DMA_0) {
		result |= SDM35541_Dma_Configure_Interrupts(handle, func_block,
					dma_old_to_new(0), 1, 1);
	}

	if (dma_ints & SDM35541_INT_DMA_1) {
		result |= SDM35541_Dma_Configure_Interrupts(handle, func_block,
					dma_old_to_new(1), 1, 1);
	}

	/*
	 * Set the board-level interrupts
	 */
	handle->ints_enabled |= mask & 0xffff;
	result |= SDM35541_Set_IT_Mask(handle, func_block, handle->ints_enabled);
	
	return result;
}


/******************************************************************************
 * Disables SDM35541's DMA/DAC/ADC/DIO interrupts
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Interrupt_Disable(struct SDM35541_Board_Descriptor *handle,
			const struct SDM35541_Function_Block *func_block,
			SDM35541_int_source_t mask)
{
	uint8_t algdio_ints;
	uint32_t dma_ints;
	int result = 0;

	/*
	 * Grab Analog DIO Mask Register
	 */
	SDM35541_ALGDIO_Get_IRQ_Status(handle, func_block, &algdio_ints);

	/**
	 * Clear ALGDIO interrupts
	 */
	algdio_ints &= ~((mask >> 24) & 0x3c);

	/**
	 * Update ALGDIO INT register
	 */
	SDM35541_ALGDIO_Set_IRQ_Status(handle, func_block, algdio_ints);

	/*
	 * Get DMA channel 0/1 interrupts
	 */
	dma_ints = (mask & 0x00600000);

	if (dma_ints & SDM35541_INT_DMA_0) {
		result |= SDM35541_Dma_Configure_Interrupts(handle, func_block,
					dma_old_to_new(0), 0, 0);
	}

	if (dma_ints & SDM35541_INT_DMA_1) {
		result |= SDM35541_Dma_Configure_Interrupts(handle, func_block,
					dma_old_to_new(1), 0, 0);
	}

	/*
	 * Set the board-level interrupts
	 */
	handle->ints_enabled &= ~(mask & 0xffff);
	result |= SDM35541_Set_IT_Mask(handle, func_block, handle->ints_enabled);
	
	return result;
}


/******************************************************************************
 * Gets the SDM35541_INT register
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Get_IT_Mask(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t *reg)
{
    return SDM35541_Read_Register_16(handle, func_block, SDM35541_INT, reg);
}


/******************************************************************************
 * Clears the SDM35541_IT_OVERRUN register
 * @param handle pointer to the board handler
 * @param func_block pointer to the adc_1002 function block
 * @return
    0
    Success.

 * @return
    -1
    Failure.
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Clear_IT_Overrun(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block)
{
    return SDM35541_Write_Register_32(handle, func_block, SDM35541_IT_OVERRUN, NO_ARG);
}


/******************************************************************************
 Set Demand Mode DMA Request Source
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DMA_Request_Source(struct SDM35541_Board_Descriptor *handle,
			  const struct SDM35541_Function_Block *func_block,
			  SDM35541_dma_channel_t channel,
			  SDM35541_dma_request_t request)
{
	uint16_t dma_source_register;

	if (validate_dma_request(request) || validate_dma_channel(channel)) {
		return -1;
	}

	switch (channel) {
	case 0:
		dma_source_register = SDM35541_DMA0_SRC;
		break;
	case 1:
		dma_source_register = SDM35541_DMA1_SRC;
		break;
	default:
		return -1;
	}
	
	return SDM35541_Write_Register_32(handle, func_block, dma_source_register, request);
}


/******************************************************************************
 * Choose which demand type a given DMA channel should use
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DMA_Set_Source(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block,
            unsigned int channel, SDM35541_dma_source_t dma_source,
            SDM35541_dma_request_t request)
{
    uint16_t dma_source_register;
	uint16_t fifo_source_register;
	uint32_t fifo_source;
	int result;

	if (validate_dma_request(request)
                || validate_dma_source(dma_source)
                || validate_dma_channel(channel))
    {
		return -1;
	}

	switch (channel) {
	/*
	 * DM75xx_DMA_CHANNEL_0
	 */
	case 0:
		dma_source_register = SDM35541_DMA0_SRC;
		fifo_source_register = SDM35541_DMA_CH0_FIFO;
		break;
	/*
	 * DM75xx_DMA_CHANNEL_1
	 */
	case 1:
		dma_source_register = SDM35541_DMA1_SRC;
		fifo_source_register = SDM35541_DMA_CH1_FIFO;
		break;
	default:
		return -1;
	}

	switch (dma_source) {
	case SDM35541_DMA_FIFO_ADC:
		fifo_source = SDM35541_DMA_CHANNEL_ADC;
		break;
    case SDM35541_DMA_FIFO_DAC1:
        fifo_source = SDM35541_DMA_CHANNEL_DAC1;
		break;
	case SDM35541_DMA_FIFO_DAC2:
        fifo_source = SDM35541_DMA_CHANNEL_DAC2;
		break;
	case SDM35541_DMA_FIFO_HSDIN:
        fifo_source = SDM35541_DMA_CHANNEL_HSDIN;
		break;
	default:
		return -2;
	}
	
	dma_channel_remap[channel] = fifo_source;

	result = SDM35541_Write_Register_32(handle, func_block, dma_source_register, request);

	result |= SDM35541_Write_Register_8(handle, func_block,
                fifo_source_register, fifo_source + 1);

    return result;
}


/******************************************************************************
Get Device FIFO Size
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_FIFO_Size(struct SDM35541_Board_Descriptor * handle, unsigned int *data)
{
	*data = 8192;
    return 0;
}


/*##############################################################################
    User Timer/Counter 8254 Functions
##############################################################################*/
/******************************************************************************
Set User Timer/Counter Clock Source
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Set_Clock_Source(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_utc_timer_t utc, SDM35541_utc_clk_t source)
{
	uint16_t utc_offset = 0;
	if (validate_utc_clock_source(utc, source)) {
		return -1;
	}
	/*
	 * Determine the offset
	 */
	switch (utc) {
	case SDM35541_UTC_0:
		utc_offset = SDM35541_UTC0_CLOCK;
		break;
	case SDM35541_UTC_1:
		utc_offset = SDM35541_UTC1_CLOCK;
		break;
	case SDM35541_UTC_2:
		utc_offset = SDM35541_UTC2_CLOCK;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return SDM35541_Write_Register_32(handle, func_block, utc_offset,
					source);
}

/******************************************************************************
Set User Timer/Counter Gate
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Set_Gate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_utc_timer_t utc, SDM35541_utc_gate gate)
{
	uint16_t utc_offset = 0x0000;

	if (validate_utc_gate(utc, gate)) {
		return -1;
	}
	/*
	 * Determine the offset
	 */
	switch (utc) {
	case SDM35541_UTC_0:
		utc_offset = SDM35541_UTC0_GATE;
		break;
	case SDM35541_UTC_1:
		utc_offset = SDM35541_UTC1_GATE;
		break;
	case SDM35541_UTC_2:
		utc_offset = SDM35541_UTC2_GATE;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return SDM35541_Write_Register_32(handle, func_block, utc_offset, gate);
}

/******************************************************************************
Set User Timer/Counter Mode
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Set_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_utc_timer_t utc, SDM35541_utc_mode mode)
{
	if (validate_utc_mode(utc, mode)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_UTC_CTRL,
					((utc << 6) | (mode << 1) | 0x30));
}

/******************************************************************************
Get the User/Timer Counter Mode
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Get_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_utc_timer_t utc, uint16_t * mode)
{
	uint16_t offset, cntr;
	uint32_t data;

	switch (utc) {
	case SDM35541_UTC_0:
		offset = SDM35541_UTC0;
		cntr = 0x02;
		break;
	case SDM35541_UTC_1:
		offset = SDM35541_UTC1;
		cntr = 0x04;
		break;
	case SDM35541_UTC_2:
		offset = SDM35541_UTC2;
		cntr = 0x08;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	/*
	 * Write the control word
	 * This is using a Read-Back command to get the mode.
	 */
	if (SDM35541_Write_Register_32(handle, func_block, SDM35541_UTC_CTRL,
				     (0xE0 | cntr))
	    != 0) {
		return -1;
	}

	/*
	 * We need to read a random register to delay 165ns to allow
	 * multiple reads from this register.
	 */
	if (SDM35541_Read_Register_32(handle, func_block, SDM35541_MT_MODE,
				    &data)
	    != 0) {
		return -1;
	}

	/*
	 * Read LSB
	 */
	if (SDM35541_Read_Register_32(handle, func_block, offset, &data)
	    != 0) {
		return -1;
	}

	/*
	 * Mask off everything but the 3 mode bits which are
	 * bits 1, 2, and 3
	 */
	*mode = (((data & 0x00FF) >> 1) & 0x7);

	return 0;
}

/******************************************************************************
Set User Timer/Counter Divisor
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Set_Divisor(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_utc_timer_t utc, uint16_t divisor)
{
	uint16_t offset, lsb, msb;

	lsb = (divisor & 0x00FF);
	msb = (divisor & 0xFF00) >> 8;

	switch (utc) {
	case SDM35541_UTC_0:
		offset = SDM35541_UTC0;
		break;
	case SDM35541_UTC_1:
		offset = SDM35541_UTC1;
		break;
	case SDM35541_UTC_2:
		offset = SDM35541_UTC2;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	/*
	 * Write least significant byte.
	 */
	if (SDM35541_Write_Register_32(handle, func_block, offset, lsb)
	    != 0) {
		return -1;
	}

	/*
	 * Write most significant byte.
	 */
	return SDM35541_Write_Register_32(handle, func_block, offset, msb);
}

/******************************************************************************
Return current User Timer/Counter value
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Get_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		     SDM35541_utc_timer_t utc, uint16_t * count)
{
	uint16_t offset;
	uint32_t lsb, msb;

	switch (utc) {
	case SDM35541_UTC_0:
		offset = SDM35541_UTC0;
		break;
	case SDM35541_UTC_1:
		offset = SDM35541_UTC1;
		break;
	case SDM35541_UTC_2:
		offset = SDM35541_UTC2;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	/*
	 * Latch the counter
	 */
	if (SDM35541_Write_Register_32(handle, func_block, SDM35541_UTC_CTRL,
				     (utc << 6))
	    != 0) {
		return -1;
	}

	/*
	 * We need to read a random register to delay 165ns to allow
	 * multiple reads from this register.
	 */
	if (SDM35541_Read_Register_32
	    (handle, func_block, SDM35541_MT_MODE, &lsb)
	    != 0) {
		return -1;
	}

	/*
	 * Read least significant byte
	 */
	if (SDM35541_Read_Register_32(handle, func_block, offset, &lsb)
	    != 0) {
		return -1;
	}

	/*
	 * Read most significant byte
	 */
	if (SDM35541_Read_Register_32(handle, func_block, offset, &msb)
	    != 0) {
		return -1;
	}

	lsb &= 0x000000FF;
	msb &= 0x000000FF;

	*count = ((msb << 8) | lsb);

	return 0;
}

/******************************************************************************
Return current User Timer/Counter status
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Get_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_utc_timer_t utc_select, uint8_t * utc_status)
{
	uint16_t offset;
	uint32_t data;

	switch (utc_select) {
	case SDM35541_UTC_0:
		offset = SDM35541_UTC0;
		break;
	case SDM35541_UTC_1:
		offset = SDM35541_UTC1;
		break;
	case SDM35541_UTC_2:
		offset = SDM35541_UTC2;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	/*
	 * Write the control word for the readback command.
	 */
	if (SDM35541_Write_Register_32(handle, func_block, SDM35541_UTC_CTRL,
				     0xE0 | (1 << (utc_select + 1)))
	    != 0) {
		return -1;
	}

	/*
	 * We need to read a random register to delay 165ns to allow
	 * multiple reads from this register.
	 */
	if (SDM35541_Read_Register_32
	    (handle, func_block, SDM35541_MT_MODE, &data)
	    != 0) {
		return -1;
	}

	/*
	 * Now perform the actual register read.
	 */
	if (SDM35541_Read_Register_32(handle, func_block, offset, &data)
	    != 0) {
		return -1;
	}

	/*
	 * Mask off unecessary bits.
	 */
	*utc_status = data & 0xFF;
	return 0;
}

/******************************************************************************
Setup a User Timer/Counter
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Setup(struct SDM35541_Board_Descriptor * handle,
        const struct SDM35541_Function_Block *func_block,
        SDM35541_utc_timer_t utc,
        SDM35541_utc_clk_t source,
        SDM35541_utc_gate gate, SDM35541_utc_mode mode, uint16_t divisor)
{
	int SDM35541_status = 0x0000;
	SDM35541_status |= SDM35541_UTC_Set_Clock_Source(handle, func_block, utc, source);
	SDM35541_status |= SDM35541_UTC_Set_Gate(handle, func_block, utc, gate);
	SDM35541_status |= SDM35541_UTC_Set_Mode(handle, func_block, utc, mode);
	SDM35541_status |= SDM35541_UTC_Set_Divisor(handle, func_block, utc, divisor);
	if (SDM35541_status != 0) {
		return -1;
	}
	return 0;
}

/*##############################################################################
 Generic Status Functions
##############################################################################*/
/*******************************************************************************
 Get FIFO Status
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_FIFO_Get_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * fifo_status)
{
	uint32_t data;

	if (SDM35541_Read_Register_32
	    (handle, func_block, SDM35541_FIFO_STATUS, &data) != 0) {
		return -1;
	}

	/*
	 * Mask off unecessary bits.
	 */
	*fifo_status = data & 0x7777;
	return 0;
}

/*******************************************************************************
 Get Pacer/Burst Status
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_CLK_Get_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * status)
{
	return
	    SDM35541_Read_Register_16(handle, func_block, SDM35541_TIMER,
				    status);
}

/*##############################################################################
 Burst Clock Functions
##############################################################################*/
/*******************************************************************************
 Get Burst Clock counter value
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Get_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * data)
{

	return SDM35541_Read_Register_16(handle, func_block,
				       SDM35541_BURST_CNT, data);
}

/*******************************************************************************
 Set Burst Clock counter value
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t data)
{
	return SDM35541_Write_Register_16(handle, func_block,
					SDM35541_BURST_CNT, data);
}

/*******************************************************************************
 Set Burst Clock counter value
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Set_Rate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_bclk_freq_t freq, float rate, float *actualRate)
{
	uint16_t divisor;
	uint32_t primary_clock;
	if (SDM35541_BCLK_Set_Frequency(handle, func_block, freq)) {
		return -1;
	}

	switch (freq) {
	case SDM35541_BCLK_FREQ_8_MHZ:
		primary_clock = 8000000L;
		break;
	case SDM35541_BCLK_FREQ_20_MHZ:
		primary_clock = 20000000L;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	if (rate > 1250000L) {
		errno = EINVAL;
		return -1;
	}

	if (rate <= (float)primary_clock / 0xFFFF) {
		return -1;
	}

	divisor = primary_clock / (uint32_t)rate;
	*actualRate = (float)primary_clock / divisor;
	return SDM35541_BCLK_Set_Count(handle, func_block, divisor - 1);
}

/*******************************************************************************
 Set Burst Clock start trigger
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Set_Start(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		      SDM35541_bclk_start_t start)
{
	if (validate_bclk_start(start)) {
		return -1;
	}
	return SDM35541_Write_Register_16(handle, func_block,
					SDM35541_BURST_START, start);
}

/*******************************************************************************
 Set Burst Clock primary frequency
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Set_Frequency(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
			  SDM35541_bclk_freq_t freq)
{
	if (validate_bclk_freq(freq)) {
		return -1;
	}
	return SDM35541_Write_Register_16(handle, func_block,
					SDM35541_BCLK_FREQ, freq);
}

/*******************************************************************************
 Get Burst Clock primary frequency
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Setup(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_bclk_start_t start,
            SDM35541_bclk_freq_t freq, float rate, float *actualRate)
{
	int SDM35541_status = 0x0000;

	SDM35541_status = SDM35541_BCLK_Set_Start(handle, func_block, start);
	SDM35541_status = SDM35541_BCLK_Set_Rate(handle, func_block, freq, rate, actualRate);
	if (SDM35541_status != 0) {
		return -1;
	}
	return SDM35541_status;
}

/*##############################################################################
 Pacer Clock Functions
##############################################################################*/
/*******************************************************************************
 Set the Pacer Clock frequency
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Frequency(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_freq_t pclk_freq)
{
	if (validate_pclk_freq(pclk_freq)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_PCLK_FREQ, pclk_freq);
}

/*******************************************************************************
 Set the Pacer Clock source
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Source(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_select_t pclk_select)
{
	if (validate_pclk_select(pclk_select)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_PACER_SELECT, pclk_select);
}

/*******************************************************************************
 Set the Pacer Clock Start Trigger
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Start(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_start_t pclk_start)
{
	if (validate_pclk_start(pclk_start)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_PACER_START, pclk_start);
}

/*******************************************************************************
 Set the Pacer Clock Stop Trigger
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Stop(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_stop_t pclk_stop)
{
	if (validate_pclk_stop(pclk_stop)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_PACER_STOP, pclk_stop);
}

/*******************************************************************************
 Read Pacer Clock
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint32_t * pacer_value)
{
	uint32_t data = 0x00000000;

	SDM35541_Read_Register_32(handle, func_block, SDM35541_PCLK_CNT, &data);

	*pacer_value = data & 0xFFFFFF;
	return 0;
}

/*******************************************************************************
 Set Pacer Start Trigger Mode
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Trigger_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_mode_t pclk_mode)
{
	if (validate_pclk_mode(pclk_mode)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_PACER_MODE, pclk_mode);
}

/*******************************************************************************
 Set Pacer Clock Count
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint32_t count)
{
	return SDM35541_Write_Register_32(handle, func_block, SDM35541_PCLK_CNT,
					count);
}

/*******************************************************************************
 Set Pacer Clock Rate
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Rate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_freq_t freq, float rate, float *actualRate)
{
	uint32_t divisor;
	uint32_t primary_clock;

	if (SDM35541_PCLK_Set_Frequency(handle, func_block, freq)) {
		return -1;
	}

	switch (freq) {
	case SDM35541_PCLK_FREQ_8_MHZ:
		primary_clock = 8000000L;
		break;
	case SDM35541_PCLK_FREQ_20_MHZ:
		primary_clock = 20000000L;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	if (rate > 1250000L) {
		errno = EINVAL;
		return -1;
	}

	if (rate <= (float)primary_clock / 0x00FFFFFF) {
		return -1;
	}

	divisor = primary_clock / (uint32_t)rate;
	*actualRate = (float)primary_clock / divisor;
	return SDM35541_PCLK_Set_Count(handle, func_block, divisor - 1);
}

/*******************************************************************************
 Setup Pacer Clock
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Setup(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_select_t pclk_select,
            SDM35541_pclk_freq_t pclk_freq,
            SDM35541_pclk_mode_t pclk_mode,
            SDM35541_pclk_start_t pclk_start,
            SDM35541_pclk_stop_t pclk_stop, float rate, float *actualRate)
{
	int status = 0;

	status |= SDM35541_PCLK_Set_Source(handle, func_block, pclk_select);
	status |= SDM35541_PCLK_Set_Frequency(handle, func_block, pclk_freq);
	status |= SDM35541_PCLK_Set_Trigger_Mode(handle, func_block, pclk_mode);
	status |= SDM35541_PCLK_Set_Start(handle, func_block, pclk_start);
	status |= SDM35541_PCLK_Set_Stop(handle, func_block, pclk_stop);
	status |= SDM35541_PCLK_Set_Rate(handle, func_block, pclk_freq, rate, actualRate);

	if (status != 0) {
		return -1;
	}

	return 0;
}

/*******************************************************************************
 Pacer Start - Software
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Start(struct SDM35541_Board_Descriptor * handle, 
            const struct SDM35541_Function_Block *func_block)
{
	uint32_t data;

	return SDM35541_Read_Register_32(handle, func_block, SDM35541_PACER,
				       &data);
}

/*******************************************************************************
 Pacer Stop - Software
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Stop(struct SDM35541_Board_Descriptor * handle, 
            const struct SDM35541_Function_Block *func_block)
{
	return SDM35541_Write_Register_32(handle, func_block, SDM35541_PACER,
					0x00);
}

/*##############################################################################
  Channel Gain Table Functions
 #############################################################################*/
/*******************************************************************************
 Enable Channel Gain Table
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Enable(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t enable)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_CGT_ENABLE, enable);
}

/*******************************************************************************
 Enable Digital Table
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DT_Enable(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t enable)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_DT_ENABLE, enable);
}

/*******************************************************************************
 CGT Latch (needed for single-channel operation)
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Latch(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_cgt_entry_t cgt)
{
	uint16_t cgt_entry;
	SDM35541_CGT_Create_Entry(&cgt, &cgt_entry);
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_CGT_LATCH, cgt_entry);
}

/*******************************************************************************
 Write CGT entry
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Write(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_cgt_entry_t cgt)
{
	uint16_t cgt_entry;
	SDM35541_CGT_Create_Entry(&cgt, &cgt_entry);
	return SDM35541_Write_Register_16(handle, func_block,
					SDM35541_CGT_WRITE, cgt_entry);
}

/******************************************************************************
Reset CGT
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Reset(struct SDM35541_Board_Descriptor * handle, 
            const struct SDM35541_Function_Block *func_block)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_CGT_RESET, NO_ARG);
}

/******************************************************************************
Clear CGT
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Clear(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_CGT_CLEAR, NO_ARG);
}

/*******************************************************************************
 Create CGT Entry
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Create_Entry(SDM35541_cgt_entry_t * cgt, uint16_t * cgt_entry)
{
	/*
	 * Here we have to build our 16 bit CGT entry from the struct
	 */
	*cgt_entry = 0x0000;
	*cgt_entry |= (cgt->channel & 0x0F);
	*cgt_entry |= (cgt->gain & 0x07) << 4;
	*cgt_entry |= (cgt->nrse & 0x01) << 7;
	*cgt_entry |= (cgt->range & 0x03) << 8;
	*cgt_entry |= (cgt->ground & 0x01) << 10;
	*cgt_entry |= (cgt->pause & 0x01) << 11;
	*cgt_entry |= (cgt->dac1 & 0x01) << 12;
	*cgt_entry |= (cgt->dac2 & 0x01) << 13;
	*cgt_entry |= (cgt->skip & 0x01) << 14;

	return 0;
}

/*******************************************************************************
 Write Digital Table Entry
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DT_Write_Entry(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t data)
{
	return SDM35541_Write_Register_16(handle, func_block,
					SDM35541_DT_WRITE, data);
}

/*******************************************************************************
 Pause channel gain table.
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Pause(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t pause)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_PAUSE_TABLE, pause);
}

/*##############################################################################
 Analog to Digital Functions.
 #############################################################################*/
/*******************************************************************************
 Read Analog to Digital FIFO
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_FIFO_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * value)
{
	return SDM35541_Read_Register_16(handle, func_block,
				       SDM35541_ADC_FIFO, value);
}

/*******************************************************************************
 A/D Software Sample
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_Software_Sample(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block)
{
	return SDM35541_Write_Register_16(handle, func_block,
					SDM35541_FIFO_STATUS, NO_ARG);
}

/*******************************************************************************
 Set Analog to Digital Conversion Signal
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_Conv_Signal(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_adc_conv_signal_t adc_conv_signal)
{
	if (validate_adc_conv_signal(adc_conv_signal)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_ADC_CONV, adc_conv_signal);
}

/*******************************************************************************
 A/D Sample Counter source select
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_SCNT_Source(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_adc_scnt_src_t src)
{
	if (validate_adc_scnt_src(src)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_ADC_SCNT_SRC, src);
}

/*******************************************************************************
 About Counter Stop Enable
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_About_Enable(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t enable)
{
	uint16_t data;
	if (enable > 0) {
		data = 0x0000;
	} else if (enable == 0x00) {
		data = 0xFFFF;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_ACNT_ENABLE, data);
}

/******************************************************************************
 Clear A/D Fifo
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_Clear(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_ADC_FIFO_CLR, NO_ARG);
}

/******************************************************************************
 Read A/D Sample Counter
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_SCNT_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t * data)
{
	return SDM35541_Read_Register_16(handle, func_block,
				       SDM35541_ADC_SAMPLE, data);
}

/******************************************************************************
 Write A/D Sample Counter
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_SCNT_Load(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t data)
{
	return SDM35541_Write_Register_16(handle, func_block,
					SDM35541_ADC_SAMPLE, data);
}

/*##############################################################################
 Digital to Analog Functions
 #############################################################################*/
/*******************************************************************************
 Cause a DAC Software update
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Soft_Update(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t dac)
{
    int result;
	if (dac == SDM35541_DAC1) {
		result = SDM35541_Write_Register_32(handle, func_block, SDM35541_DAC1_UPDATE,
					 NO_ARG);
	} else if (dac == SDM35541_DAC2) {
		result = SDM35541_Write_Register_32(handle, func_block, SDM35541_DAC2_UPDATE,
					 NO_ARG);
	} else if (dac == (SDM35541_DAC1 & SDM35541_DAC2)) {
		result = SDM35541_Write_Register_32(handle, func_block, SDM35541_DAC_UPDATE,
					 NO_ARG);
	} else {
		errno = EINVAL;
		result = -1;
	}


	return result;
}

/*******************************************************************************
 Get Digital to Analog Update Counter
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Get_Update_Counter(struct SDM35541_Board_Descriptor * handle,
                const struct SDM35541_Function_Block *func_block,
                SDM35541_dac_channel_t dac, uint16_t * data)
{
	uint16_t offset;
	switch (dac) {
	case SDM35541_DAC1:
		offset = SDM35541_DAC1_SAMPLE;
		break;
	case SDM35541_DAC2:
		offset = SDM35541_DAC2_SAMPLE;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return SDM35541_Read_Register_16(handle, func_block, offset, data);
}

/*******************************************************************************
 Set Digital to Analog Update Counter
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Update_Counter(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac, uint16_t data)
{
	uint16_t offset;

	switch (dac) {
	case SDM35541_DAC1:
		offset = SDM35541_DAC1_SAMPLE;
		break;
	case SDM35541_DAC2:
		offset = SDM35541_DAC2_SAMPLE;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return SDM35541_Write_Register_16(handle, func_block, offset, data);
}

/*******************************************************************************
 Set Digital to Analog Range
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Range(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac, SDM35541_dac_range_t range)
{
	uint16_t offset;

	if (validate_dac_channel(dac)) {
		return -1;
	}
	if (validate_dac_range(range)) {
		return -1;
	}
	switch (dac) {
	case SDM35541_DAC1:
		offset = SDM35541_DAC1_RANGE;
		break;
	case SDM35541_DAC2:
		offset = SDM35541_DAC2_RANGE;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return SDM35541_Write_Register_16(handle, func_block, offset, range);
}

/*******************************************************************************
 Set Digital to Analog Update Source
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Update_Source(struct SDM35541_Board_Descriptor * handle,
                const struct SDM35541_Function_Block *func_block,
                SDM35541_dac_channel_t dac,
                SDM35541_dac_update_src_t src)
{
	uint16_t offset;

	if (validate_dac_channel(dac)) {
		return -1;
	}
	if (validate_dac_update_source(src)) {
		return -1;
	}
	switch (dac) {
	case SDM35541_DAC1:
		offset = SDM35541_DAC1_SRC;
		break;
	case SDM35541_DAC2:
		offset = SDM35541_DAC2_SRC;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return SDM35541_Write_Register_16(handle, func_block, offset, src);
}

/*******************************************************************************
 Set Digital to Analog Cycle Mode
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_dac_channel_t dac, SDM35541_dac_mode_t mode)
{
	uint16_t offset;

	if (validate_dac_mode(dac, mode)) {
		return -1;
	}

	switch (dac) {
	case SDM35541_DAC1:
		offset = SDM35541_DAC1_CYCLE;
		break;
	case SDM35541_DAC2:
		offset = SDM35541_DAC2_CYCLE;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return SDM35541_Write_Register_16(handle, func_block, offset, mode);
}

/*******************************************************************************
 Write a value to the DAC FIFO
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_FIFO_Write(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac, uint16_t data)
{
	uint16_t offset;
    
	switch (dac) {
	case SDM35541_DAC1:
		offset = SDM35541_DAC1_FIFO;
		break;
	case SDM35541_DAC2:
		offset = SDM35541_DAC2_FIFO;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return SDM35541_Write_Register_16(handle, func_block, offset, data);
}

/*******************************************************************************
 Set Digital to Analog Clock
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Frequency(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_freq_t freq)
{
	if (validate_dac_freq(freq)) {
		return -1;
	}
	return SDM35541_Write_Register_16(handle,
					func_block,
					SDM35541_DAC_CLK_SOURCE, freq);
}

/*******************************************************************************
 Set Digital to Analog Clock Count
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint32_t count)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_DAC_CLK, count);
}

/*******************************************************************************
 Get Digital to Analog Clock Count
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Get_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint32_t *count)
{
	return SDM35541_Read_Register_32(handle, func_block,
					SDM35541_DAC_CLK, count);
}

/*******************************************************************************
 Set Digital to Analog Clock Rate
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Rate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_dac_freq_t freq, uint32_t rate, float *actualRate)
{
	uint32_t primary_clock;
	if (SDM35541_DAC_Set_Frequency(handle, func_block, freq)) {
		return -1;
	}

	switch (freq) {
	case SDM35541_DAC_FREQ_8_MHZ:
		primary_clock = 8000000L;
		break;
	case SDM35541_DAC_FREQ_20_MHZ:
		primary_clock = 20000000L;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	if (rate > 20000000L || rate == 0) {
		errno = EINVAL;
		return -1;
	}

	rate = primary_clock / rate;
	*actualRate = (float)primary_clock / rate;
	return SDM35541_DAC_Set_Count(handle, func_block, rate - 1);
}

/*******************************************************************************
 Set Digital to Analog Clock Trigger Stop
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Clock_Stop(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_clk_stop_t stop)
{
	if (validate_dac_clk_stop(stop)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle,
					func_block,
					SDM35541_DAC_CLK_STOP, stop);
}

/*******************************************************************************
 Set Digital to Analog Clock Trigger Start
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Clock_Start(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_clk_start_t start)
{
	if (validate_dac_clk_start(start)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle,
					func_block,
					SDM35541_DAC_CLK_START, start);
}

/*******************************************************************************
 DAC Start - Software
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Start(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block)
{
	uint32_t arg;

	return SDM35541_Read_Register_32(handle, func_block,
				       SDM35541_DAC_CLK_ST, &arg);
}

/*******************************************************************************
 DAC Stop - Software
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Stop(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_DAC_CLK_ST, NO_ARG);
}

/*******************************************************************************
 Setup DAC
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Setup(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac,
            SDM35541_dac_range_t range,
            SDM35541_dac_update_src_t src, SDM35541_dac_mode_t mode)
{
	int status = 0;

	status |= SDM35541_DAC_Set_Range(handle, func_block, dac, range);
	status |= SDM35541_DAC_Set_Update_Source(handle, func_block, dac, src);
	status |= SDM35541_DAC_Set_Mode(handle, func_block, dac, mode);
	return status;
}

/*******************************************************************************
 Reset DAC FIFO
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Reset(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac)
{
	uint16_t offset;

	switch (dac) {
	case SDM35541_DAC1:
		offset = SDM35541_DAC1_RESET;
		break;
	case SDM35541_DAC2:
		offset = SDM35541_DAC2_RESET;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block, offset, NO_ARG);
}

/******************************************************************************
Clear DAC FIFO
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Clear(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac)
{
	uint16_t offset;

	switch (dac) {
	case SDM35541_DAC1:
		offset = SDM35541_DAC1_CLEAR;
		break;
	case SDM35541_DAC2:
		offset = SDM35541_DAC2_CLEAR;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block, offset, NO_ARG);
}

/*******************************************************************************
 Enable/Disable Start/Stop mode
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_CLK_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
			SDM35541_dac_clk_mode_t clk_mode)
{
	if (validate_dac_clk_mode(clk_mode)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_DAC_CLK_MODE, clk_mode);
}

/*##############################################################################
 HighSpeed Digital Functions
 #############################################################################*/
/*******************************************************************************
 HighSpeed Digital Software Sample
 ******************************************************************************/
SDM35541LIB_API int SDM35541_HSDIN_Software_Sample(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block)
{
	return
	    SDM35541_Write_Register_32(handle, func_block, SDM35541_TIMER,
				     NO_ARG);
}

/*******************************************************************************
 Set HighSpeed Digital Sampling Signal select
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_HSDIN_Sample_Signal(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
			   SDM35541_hsdin_signal_t signal)
{
	if (validate_hsdin_signal(signal)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_HSDIN_START, signal);
}

/******************************************************************************
 Clear HSDIN FIFO
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_HSDIN_Clear(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_HSDIN_FIFO_CLR, NO_ARG);
}

/*******************************************************************************
 Read HSDIN FIFO
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_HSDIN_FIFO_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * data)
{
	return SDM35541_Read_Register_16(handle, func_block,
				       SDM35541_HSDIN_FIFO, data);
}

/*##############################################################################
 SyncBus Functions
 #############################################################################*/
/*******************************************************************************
 Set SyncBus Source Select
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_SBUS_Set_Source(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_sbus_t sbus,
		       SDM35541_sbus_src_t src)
{
	uint16_t offset;
	if (validate_sbus_source(src)) {
		return -1;
	}
	switch (sbus) {
	case SDM35541_SBUS0:
		offset = SDM35541_SBUS0_SOURCE;
		break;
	case SDM35541_SBUS1:
		offset = SDM35541_SBUS1_SOURCE;
		break;
	case SDM35541_SBUS2:
		offset = SDM35541_SBUS2_SOURCE;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return SDM35541_Write_Register_32(handle, func_block, offset, src);
}

/*******************************************************************************
 Enable/Disable SyncBus
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_SBUS_Enable(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_sbus_t sbus, uint16_t enable)
{
	uint16_t offset;
	switch (sbus) {
	case SDM35541_SBUS0:
		offset = SDM35541_SBUS0_ENABLE;
		break;
	case SDM35541_SBUS1:
		offset = SDM35541_SBUS1_ENABLE;
		break;
	case SDM35541_SBUS2:
		offset = SDM35541_SBUS2_ENABLE;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return SDM35541_Write_Register_16(handle, func_block, offset, enable);
}

/*##############################################################################
 External Trigger/Interrupt Configuration
 #############################################################################*/
/*******************************************************************************
 External Trigger Polarity select
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ETRIG_Polarity_Select(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_ext_polarity_t polarity)
{
	if (validate_ext_polarity(polarity)) {
		return -1;
	}
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_ETRG_POLARITY, polarity);
}

/*******************************************************************************
 External Interrupt Polarity select
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_EINT_Polarity_Select(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_ext_polarity_t polarity)
{
	if (validate_ext_polarity(polarity)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_EINT_POLARITY, polarity);
}

/*##############################################################################
 About Counter
 #############################################################################*/
/*******************************************************************************
 Get About Counter value
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ACNT_Get_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * data)
{
	return SDM35541_Read_Register_16(handle, func_block, SDM35541_ABOUT_CNT,
				       data);
}

/*******************************************************************************
 Set About Counter value
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ACNT_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t data)
{
	return
	    SDM35541_Write_Register_16(handle, func_block, SDM35541_ABOUT_CNT,
				     data);
}

/*##############################################################################
 Delay Counter
 #############################################################################*/
/*******************************************************************************
 Get Delay Counter value
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DCNT_Get_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * data)
{
	return SDM35541_Read_Register_16(handle, func_block, SDM35541_DELAY_CNT,
				       data);
}

/*******************************************************************************
 Set Delay Counter value
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DCNT_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t data)
{
	return
	    SDM35541_Write_Register_16(handle, func_block, SDM35541_DELAY_CNT,
				     data);
}

/*##############################################################################
 Digital Input/Output
 #############################################################################*/
/*******************************************************************************
 Write Digital Port Output Line
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Set_Port(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dio_port_t port,
		    uint8_t data)
{
	uint16_t offset;
	switch (port) {
	case SDM35541_DIO_PORT0:
		offset = SDM35541_DIO0;
		break;
	case SDM35541_DIO_PORT1:
		offset = SDM35541_DIO1;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}
	return SDM35541_Write_Register_8(handle, func_block, offset, data);
}

/*******************************************************************************
 Read Digital Ports Input Line
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Port(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dio_port_t port,
		    uint8_t * data)
{
	uint16_t offset;

	switch (port) {
	case SDM35541_DIO_PORT0:
		offset = SDM35541_DIO0;
		break;
	case SDM35541_DIO_PORT1:
		offset = SDM35541_DIO1;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return SDM35541_Read_Register_8(handle, func_block, offset, data);
}

/*******************************************************************************
 Read DIO IRQ Status
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * data)
{
	return SDM35541_Read_Register_8(handle, func_block,
				      SDM35541_DIO_STATUS, data);
}

/*******************************************************************************
 Clear Digital IRQ Status
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Clear_IRQ(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block)
{
	uint8_t data = 0;
    uint8_t mask = 3;

	/*
	 * Modify bits 0 and 1 to logic 0.
	 */
	if (SDM35541_Modify_Register_8
	    (handle, func_block, SDM35541_DIO_STATUS, data, mask) != 0) {
		return -1;
	}

	/*
	 * Once the status register is set, this read will actually clear the IRQ.
	 */
	return SDM35541_Read_Register_8(handle, func_block,
				      SDM35541_DIO_CTRL, &data);
}

/******************************************************************************
 Clear Digital Chip
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Reset(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block)
{
	uint8_t data, mask;
	data = 0x00;
	mask = 0x03;

	/*
	 * Modify bits 0 and 1 to logic 0.
	 */

	if (SDM35541_Modify_Register_8
	    (handle, func_block, SDM35541_DIO_STATUS, data, mask) != 0) {
		return -1;
	}

	/*
	 * Once the status register is set, this write will reset the DIO.
	 */

	return SDM35541_Write_Register_8(handle, func_block,
				       SDM35541_DIO_CTRL, NO_ARG);
}

/*******************************************************************************
 Get Port 0 Direction
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Direction(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dio_port_t port, uint8_t *direction)
{
	int result;
	uint8_t data, mask;

	if (port == SDM35541_DIO_PORT0) {
		mask = 0x03;
		data = 0x01;

		/*
		 * Port 0 is bit adjustable and you must first select the register
		 * that contains the Port 0 direction from the DIO Status register.
		 */
		result = SDM35541_Modify_Register_8
		    (handle, func_block, SDM35541_DIO_STATUS, data, mask);
		
		if (result) {
			return -1;
		}

		/*
		 * Now read the direction to the Port 0 Direction register.
		 */
		result = SDM35541_Read_Register_8(handle, func_block,
					       SDM35541_DIO_CTRL, direction);
		
	} else if (port == SDM35541_DIO_PORT1) {
		/*
		 * Port 1 is byte adjustable and is controlled by a single bit
		 * in the DIO Status register.
		 */
		result = SDM35541_Read_Register_8(handle, func_block,
						SDM35541_DIO_STATUS, direction);
		
		*direction = (*direction & 0x04) >> 2;

	} else {
		result = -1;
	}

	return result;
}

/*******************************************************************************
 Set Port 0 Direction
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Set_Direction(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dio_port_t port, uint8_t direction)
{
	uint8_t data, mask;

	if (port == SDM35541_DIO_PORT0) {
		mask = 0x03;
		data = 0x01;

		/*
		 * Port 0 is bit adjustable and you must first select the register
		 * that contains the Port 0 direction from the DIO Status register.
		 */

		if (SDM35541_Modify_Register_8
		    (handle, func_block, SDM35541_DIO_STATUS, data, mask)
		    != 0) {
			return -1;
		}

		/*
		 * Now write the direction to the Port 0 Direction register.
		 */

		return SDM35541_Write_Register_8(handle, func_block,
					       SDM35541_DIO_CTRL, direction);
	} else if (port == SDM35541_DIO_PORT1) {
		mask = 0x04;

		if (direction > 0) {
			data = 0x04;
		} else {
			data = 0x00;
		}

		/*
		 * Port 1 is byte adjustable and is controlled by a single bit
		 * in the DIO Status register.
		 */

		return SDM35541_Modify_Register_8(handle, func_block,
						SDM35541_DIO_STATUS, data,
						mask);
	} else {
		return -1;
	}
}


/*******************************************************************************
 Set Port 0 Mask
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Set_Mask(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t mask)
{
	uint8_t data, bit_mask;
	bit_mask = 0x03;
	data = 0x02;
	/*
	 * Select the mask register by settings bits 0 and 1 of the status register.
	 */
	if (SDM35541_Modify_Register_8
	    (handle, func_block, SDM35541_DIO_STATUS, data, bit_mask)
	    != 0) {
		return -1;
	}
	/*
	 * Now write the mask.
	 */
	return SDM35541_Write_Register_8(handle, func_block,
				       SDM35541_DIO_CTRL, mask);
}


/*******************************************************************************
 Get Port 0 Mask
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Mask(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t *mask)
{
	uint8_t data, bit_mask;
	bit_mask = 0x03;
	data = 0x02;

	/*
	 * Select the mask register by settings bits 0 and 1 of the status register.
	 */
	if (SDM35541_Modify_Register_8
	    (handle, func_block, SDM35541_DIO_STATUS, data, bit_mask)
	    != 0) {
		return -1;
	}

	/*
	 * Now read the mask.
	 */
	return SDM35541_Read_Register_8(handle, func_block,
				       SDM35541_DIO_CTRL, mask);
}


/*******************************************************************************
 Set Port 0 Compare
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Set_Compare(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t compare)
{
	uint8_t data, mask;
	mask = 0x03;
	data = 0x03;
	/*
	 * Select the compare register from the status register
	 */
	if (SDM35541_Modify_Register_8
	    (handle, func_block, SDM35541_DIO_STATUS, data, mask)
	    != 0) {
		return -1;
	}
	/*
	 * Write the compare value.
	 */
	return SDM35541_Write_Register_8(handle, func_block,
				       SDM35541_DIO_CTRL, compare);
}

/*******************************************************************************
 Get Port 0 Compare
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Compare(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * compare)
{
	uint8_t data, mask;
	mask = 0x03;
	data = 0x03;
	/*
	 * Select the compare register from the status register
	 */
	if (SDM35541_Modify_Register_8
	    (handle, func_block, SDM35541_DIO_STATUS, data, mask)
	    != 0) {
		return -1;
	}
	/*
	 * Read the compare value.
	 */
	return SDM35541_Read_Register_8(handle, func_block,
				      SDM35541_DIO_CTRL, compare);
}

/*******************************************************************************
 Set DIO IRQ Mode
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_IRQ_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_dio_mode_t mode)
{
	uint8_t data, mask = 0x08;
	switch (mode) {
	case SDM35541_DIO_MODE_EVENT:
		data = 0x00;
		break;
	case SDM35541_DIO_MODE_MATCH:
		data = 0x08;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	return SDM35541_Modify_Register_8(handle, func_block,
					SDM35541_DIO_STATUS, data, mask);
}

/*******************************************************************************
 Set DIO Sample Clock
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Clock(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_dio_clk_t clock)
{
	uint8_t data, mask = 0x20;

	switch (clock) {
	case SDM35541_DIO_CLK_8MHZ:
		data = 0x00;
		break;
	case SDM35541_DIO_CLK_UTC1:
		data = 0x20;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	return SDM35541_Modify_Register_8(handle, func_block,
					SDM35541_DIO_STATUS, data, mask);
}

/*******************************************************************************
 Enable/Disable DIO IRQ
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Enable_IRQ(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t enable)
{
	uint8_t data, mask = 0x10;

	if (enable == 0x00) {
		data = 0x00;
	} else {
		data = 0x10;
	}

	return SDM35541_Modify_Register_8(handle, func_block,
					SDM35541_DIO_STATUS, data, mask);
}

/*##############################################################################
 User I/O Signal Functions
 #############################################################################*/
/*******************************************************************************
 Select User I/O Signal
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UIO_Select(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_uio_channel_t channel, SDM35541_uio_source_t source)
{
	uint16_t offset;

	if (validate_uio_select(source)) {
		return -1;
	}

	switch (channel) {
	case SDM35541_UIO0:
		offset = SDM35541_UIO0_SELECT;
		break;
	case SDM35541_UIO1:
		offset = SDM35541_UIO1_SELECT;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return SDM35541_Write_Register_16(handle, func_block, offset, source);
}

/*******************************************************************************
 Read User Inputs
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UIO_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint32_t * data)
{
	return SDM35541_Read_Register_32(handle, func_block,
				       SDM35541_USER_IO, data);
}

/*******************************************************************************
 Write User Outputs
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_UIO_Write(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint32_t data)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_USER_IO, data);
}

/*##############################################################################
 McBSP Functions
 #############################################################################*/
/*******************************************************************************
 Enable A/D FIFO to DSP
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_McBSP_ADC_FIFO(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t enable)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_MCBSP_AD_CTRL, enable);
}

/*******************************************************************************
 Enable D/A FIFO to DSP
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_McBSP_DAC_FIFO(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t enable)
{
	return SDM35541_Write_Register_32(handle, func_block,
					SDM35541_MCBSP_DA_CTRL, enable);
}

/*##############################################################################
 SDM7540 Family specific functions
 #############################################################################*/


/*******************************************************************************
 Write to SRAM
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_SRAM_Write(struct SDM35541_Board_Descriptor *handle, 
			const struct SDM35541_Function_Block *func_block,
			uint16_t address, uint32_t data)
{
	address &= 0b11;
	return SDM35541_Write_Register_32(handle, func_block, SDM35541_SRAM + address * 4, data);
}


/*******************************************************************************
 Read from SRAM
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_SRAM_Read(struct SDM35541_Board_Descriptor *handle, 
			const struct SDM35541_Function_Block *func_block,
			uint16_t address, uint32_t *data)
{
	address &= 0b11;
	return SDM35541_Read_Register_32(handle, func_block, SDM35541_SRAM + address * 4, data);
}


/*******************************************************************************
 Calibrate an SDM7540
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Calibrate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t dac1_value,
            uint16_t dac2_value,
            SDM35541_dac_range_t dac1_range,
            SDM35541_dac_range_t dac2_range)
{
	int status;

	if (validate_dac_range(dac1_range) || validate_dac_range(dac2_range)) {
		return -1;
	}

	/*
	 * Write DAC1 Value to SRAM
	 */
	status =
	    SDM35541_SRAM_Write(handle, func_block, 0, dac1_value);

	if (status != 0) {
		return 1;
	}

	/*
	 * Write DAC2 Value to SRAM
	 */
	status =
	    SDM35541_SRAM_Write(handle, func_block, 1, dac2_value);

	if (status != 0) {
		return 2;
	}

	/*
	 * Write DAC1 Range to SRAM
	 */
	status =
	    SDM35541_SRAM_Write(handle, func_block, 2, dac1_range);

	if (status != 0) {
		return 3;
	}

	/*
	 * Write DAC2 Range to SRAM
	 */
	status =
	    SDM35541_SRAM_Write(handle, func_block, 3, dac2_range);

	if (status != 0) {
		return 4;
	}

	/*
	 * Set the device's calibration flag to be read by the DSP
	 */
	return SDM35541_Write_Register_32(handle,
					func_block,
					SDM35541_DSP, SDM35541_DSP_CAL_AUTO);
}


/*******************************************************************************
 Get the DSP Bootloader Version
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DSP_Get_Version(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint32_t *major_minor)
{
	int result, retries = 0;
    uint8_t is_complete;
	uint16_t data;

	result = SDM35541_DSP_CMD_Send(handle, func_block, SDM35541_DSP_BOOT_VERSION);

	if (result) {
		return -1;
	}

	/* Check for complete command or time out after 35ms */
    do {
        result = SDM35541_DSP_CMD_Complete(handle, func_block, &is_complete);

		if (result) {
			return -2;
		}

		if (is_complete) {
			break;
		}

		retries++;
		SDM35541_Micro_Sleep(1000);		
    } while (retries < 35);

	/* If timeout occurred */
	if (retries == 35) {
		return -3;
	}

	SDM35541_Read_Register_16(handle, func_block, SDM35541_SRAM, &data);
	data &= 0xffff;
	*major_minor = data << 16;
	SDM35541_Read_Register_16(handle, func_block, SDM35541_SRAM + 4, &data);
	*major_minor |= data & 0xffff;

	return 0;
}


/*******************************************************************************
 Issue a DSP command
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DSP_CMD_Send(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_dsp_command_t command)
{
	if (validate_dsp_command(command)) {
		return -1;
	}

	return SDM35541_Write_Register_32(handle,
					func_block, SDM35541_DSP, command);
}

/*******************************************************************************
 Check for Compeletion of a DSP command
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DSP_CMD_Complete(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * data)
{
	return SDM35541_Read_Register_8(handle, func_block, SDM35541_DSP,
				      data);
}

/*******************************************************************************
 Check Status of DSP Command
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DSP_CMD_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dsp_command_t command)
{
	int status;
	uint32_t data;

	if (validate_dsp_command(command)) {
		return -1;
	}

	status = SDM35541_Read_Register_32(handle,
						func_block,
						SDM35541_DSP, &data);

	if (status != 0) {
		return -1;
	}

	data = data >> 8;

	return !(~(data) && command);
}

/*******************************************************************************
 Read analog connection DIO mask
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Get_Mask(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_algdio_mask_t * pin1, SDM35541_algdio_mask_t * pin2)
{
	int SDM35541_status;
	uint8_t data;

	SDM35541_status = SDM35541_Read_Register_8(handle,
					       func_block,
					       SDM35541_ALGDIO_MASK, &data);

	if (SDM35541_status != 0) {
		return -1;
	}

	if (data & 0x01)
		*pin1 = SDM35541_ALGDIO_UNMASKED;
	else
		*pin1 = SDM35541_ALGDIO_MASKED;

	if (data & 0x02)
		*pin2 = SDM35541_ALGDIO_UNMASKED;
	else
		*pin2 = SDM35541_ALGDIO_MASKED;

	return 0;
}

/*******************************************************************************
 Write analog connection DIO mask
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Set_Mask(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_algdio_mask_t pin1, SDM35541_algdio_mask_t pin2)
{
	int SDM35541_status;
	uint8_t mask;

	SDM35541_status = SDM35541_Read_Register_8(handle,
					       func_block,
					       SDM35541_ALGDIO_MASK, &mask);

	if (SDM35541_status != 0) {
		return -1;
	}

	mask &= ~0x03;
	mask |= pin1 + (pin2 << 1);

	return SDM35541_Write_Register_8(handle,
				       func_block,
				       SDM35541_ALGDIO_MASK, mask);
}

/*******************************************************************************
 Read Analog connection DIO direction
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Get_Direction(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_algdio_direction_t * pin1,
            SDM35541_algdio_direction_t * pin2)
{
	int SDM35541_status;
	uint8_t data = 0x00;

	SDM35541_status = SDM35541_Read_Register_8(handle,
					       func_block,
					       SDM35541_ALGDIO_DIR, &data);

	if (SDM35541_status != 0) {
		return -1;
	}

	if (data & 0x01) {
		*pin1 = SDM35541_ALGDIO_OUTPUT;
	} else {
		*pin1 = SDM35541_ALGDIO_INPUT;
	}

	if (data & 0x02) {
		*pin2 = SDM35541_ALGDIO_OUTPUT;
	} else {
		*pin2 = SDM35541_ALGDIO_INPUT;
	}

	return 0;
}

/*******************************************************************************
 Write analog conenction DIO direction
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Set_Direction(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_algdio_direction_t pin1,
            SDM35541_algdio_direction_t pin2)
{
	uint8_t data = 0x00;

	if (pin1) {
		data |= 0x01;
	} else {
		data &= ~0x01;
	}

	if (pin2) {
		data |= 0x02;
	} else {
		data &= ~0x02;
	}

	return SDM35541_Write_Register_8(handle,
				       func_block,
				       SDM35541_ALGDIO_DIR, data);
}

/*******************************************************************************
 Read analog connection DIO data from output pins
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Set_Data(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t pin1, uint8_t pin2)
{
	uint8_t data = 0x00;

	if (pin1) {
		data |= 0x01;
	} else {
		data &= ~0x01;
	}

	if (pin2) {
		data |= 0x02;
	} else {
		data &= ~0x02;
	}

	return SDM35541_Write_Register_8(handle,
				       func_block,
				       SDM35541_ALGDIO_DATA, data);
}

/*******************************************************************************
 Write analog connection DIO data to output pins
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Get_Data(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t * pin1, uint8_t * pin2)
{
	int status;
	uint8_t data = 0x00;

	status = SDM35541_Read_Register_8(handle,
					       func_block,
					       SDM35541_ALGDIO_DATA, &data);

	if (data & 0x01) {
		*pin1 = 0xFF;
	}

	if (data & 0x02) {
		*pin2 = 0xFF;
	}

	return status;
}

/*******************************************************************************
 Read analog connection DIO IRQ status
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Get_IRQ_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * status)
{
	return SDM35541_Read_Register_8(handle,
				      func_block,
				      SDM35541_ALGDIO_INT, status);
}

SDM35541LIB_API
int SDM35541_ALGDIO_Set_IRQ_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t status)
{
	return SDM35541_Write_Register_8(handle,
				      func_block,
				      SDM35541_ALGDIO_MASK, status);
}


SDM35541LIB_API
int SDM35541_Get_Temp(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * temp)
{
	int SDM35541_status = 0;
	uint32_t data = 0;

	/*
	 * Check if sensor is busy
	 */
	do {
        
		SDM35541_status |= SDM35541_Write_Register_32(handle,
							  func_block,
							  SDM35541_I2C_ADDR,
							  0x98);

		SDM35541_status |= SDM35541_Write_Register_32(handle,
							  func_block,
							  SDM35541_I2C_PTR,
							  0x02);

		SDM35541_status |= SDM35541_Write_Register_32(handle,
							  func_block,
							  SDM35541_I2C_GO,
							  0x02);

		SDM35541_Micro_Sleep(1000);

		SDM35541_status |= SDM35541_Read_Register_32(handle,
							 func_block,
							 SDM35541_I2C_READ,
							 &data);

		if (SDM35541_status != 0) {
			return -1;
		}

	} while ((data & 0x80) == 0x80);
	/*
	 * configure sensor for one-shot reading
	 */
	SDM35541_status |= SDM35541_Write_Register_32(handle,
						  func_block,
						  SDM35541_I2C_ADDR, 0x98);

	SDM35541_status |= SDM35541_Write_Register_32(handle,
						  func_block,
						  SDM35541_I2C_PTR, 0x0F);

	SDM35541_status |= SDM35541_Write_Register_32(handle,
						  func_block,
						  SDM35541_I2C_DATA, 0x0A);

	SDM35541_status |= SDM35541_Write_Register_32(handle,
						  func_block,
						  SDM35541_I2C_GO, 0x03);

	if (SDM35541_status != 0) {
		return -1;
	}

	SDM35541_Micro_Sleep(1000);

	/*
	 * wait for the sensor to finish reading
	 */
	do {

		SDM35541_status |= SDM35541_Write_Register_32(handle,
							  func_block,
							  SDM35541_I2C_ADDR,
							  0x98);

		SDM35541_status |= SDM35541_Write_Register_32(handle,
							  func_block,
							  SDM35541_I2C_PTR,
							  0x02);

		SDM35541_status |= SDM35541_Write_Register_32(handle,
							  func_block,
							  SDM35541_I2C_GO,
							  0x02);
                              
        SDM35541_Micro_Sleep(1000);

		SDM35541_status |= SDM35541_Read_Register_32(handle,
							 func_block,
							 SDM35541_I2C_READ,
							 &data);

		if (SDM35541_status != 0) {
			return -1;
		}

	} while ((data & 0x80) == 0x80);
	/*
	 * read the temp
	 */
	SDM35541_status |= SDM35541_Write_Register_32(handle,
						  func_block,
						  SDM35541_I2C_ADDR, 0x98);

	SDM35541_status |= SDM35541_Write_Register_32(handle,
						  func_block,
						  SDM35541_I2C_PTR, 0x00);

	SDM35541_status |= SDM35541_Write_Register_32(handle,
						  func_block,
						  SDM35541_I2C_GO, 0x02);

    SDM35541_Micro_Sleep(1000);

	SDM35541_status |= SDM35541_Read_Register_32(handle,
						 func_block,
						 SDM35541_I2C_READ, &data);

	if (SDM35541_status != 0) {
		return -1;
	}
	/*
	 * temp is offset by 64 when in extended temp range
	 */
	*temp = (data & 0xFF) - 64;

	return 0;
}
