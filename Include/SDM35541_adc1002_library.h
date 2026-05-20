/**
    @file

    @brief
        Register definitions for SDM35541 devices

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
	
	$Id: SDM35541_adc1002_library.h 152277 2026-02-17 16:08:38Z bkorpacz $
*/

#ifndef __SDM35541_ADC1002_LIBRARY_H__
#define __SDM35541_ADC1002_LIBRARY_H__

#include "SDM35541_gbc_library.h"

#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief
 *      This macro will return the sample portion of raw analog data
 *
 * @param
 *      data
 *
 *      The raw analog data
 *
 * @return
 *      The 12 bit signed analog sample
 *
 * @note
 *      The value returned by this macro should be stored in an int16_t
 */

#define SDM35541_ADC_ANALOG_DATA(data) (((int16_t) (data)) >> 3)

/*=============================================================================
Enumerations
 =============================================================================*/


/**
 * @defgroup SDM35541_Types_DSP_Enumerations SDM35541 type DSP enumerations
 * @{
 */
/**
 * @brief
 *      DSP Command
 */
typedef enum {
    /**
     * DAC Auto Calibration
     */
	SDM35541_DSP_CAL_AUTO = 1,
    /**
     * Internal Flash Download
     */
	SDM35541_DSP_FLASH_DOWNLOAD = 2,
    /**
     * Run User Program
     */
	SDM35541_DSP_USER_RUN = 3,
    /**
     * Upgrade User Program
     */
	SDM35541_DSP_USER_UPGRADE = 4,
    /**
     * Erase Internal Flash
     */
	SDM35541_DSP_INT_FLASH_ERASE = 5,
    /**
     * Erase External Flash
     */
	SDM35541_DSP_EXT_FLASH_ERASE = 6,
    /**
     * Check if DSP still alive
     */
	SDM35541_DSP_ATTENTION = 7,
    /**
     * Load Default Calibration
     */
	SDM35541_DSP_CAL_DEFAULT = 8,
    /**
     * Set Default Calibration
     */
	SDM35541_DSP_CAL_SET = 9,
    /**
     * Get Calibration Algorithm Version
     */
	SDM35541_DSP_CAL_VERSION = 10,
    /**
     * Get Boot Loader Version
     */
	SDM35541_DSP_BOOT_VERSION = 11
} SDM35541_dsp_command_t;
 /**
  * @} SDM35541_Types_DSP_Enumerations
  */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 DMA Enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_DMA_Enumerations SDM35541 type DMA
 * @{
 */

/**
 * @brief
 * DMA Control Flag
 */
typedef enum {
    /**
     * DMA Initialized
     */
	SDM35541_DMA_FLAG_INIT = 0x01,
    /**
     * DMA Memory Map
     */
	SDM35541_DMA_FLAG_MMAP = 0x02,
    /**
     * DMA Reset DREQ
     */
	SDM35541_DMA_FLAG_RESET = 0x04,
    /**
     * DMA Non Demand Mode
     */
	SDM35541_DMA_FLAG_NONDEMAND = 0x08,
    /**
     * DMA Channel Status
     */
	SDM35541_DMA_FLAG_STATUS = 0x10,
    /**
     * DMA Arbitrary
     */
	SDM35541_DMA_FLAG_ARB = 0x20
} SDM35541_dma_flag_t;
/**
 * @brief
 * DMA Status Reset Flag
 */
typedef enum {
    /**
     * DMA 0
     */
	SDM35541_DMA_RESET_SEL = 0x01,
    /**
     * DMA Channel Reset Value
     */
	SDM35541_DMA_RESET_VAL = 0x10
} SDM35541_dma_reset_t;
/**
 * @brief
 * DMA Demand Mode Source
 */
typedef enum {
    /**
     * Request Disable
     */
	SDM35541_DMA_DEMAND_DISABLE = 0,
    /**
     * A/D Sample Counter
     */
	SDM35541_DMA_DEMAND_SCNT_ADC = 1,
    /**
     * D/A 1 Sample Counter
     */
	SDM35541_DMA_DEMAND_SCNT_DAC1 = 2,
    /**
     * D/A 2 Sample Counter
     */
	SDM35541_DMA_DEMAND_SCNT_DAC2 = 3,
    /**
     * User Timer/Counter 1
     */
	SDM35541_DMA_DEMAND_UTC1 = 4,
    /**
     * A/D FIFO Half Full
     */
	SDM35541_DMA_DEMAND_FIFO_ADC = 8,
    /**
     * D/A 1 FIFO Half Empty
     */
	SDM35541_DMA_DEMAND_FIFO_DAC1 = 9,
    /**
     * D/A 2 FIFO Half Empty
     */
	SDM35541_DMA_DEMAND_FIFO_DAC2 = 10,
} SDM35541_dma_request_t;
/**
 * @brief
 * DMA Local Source
 */
typedef enum {
    /**
     * DMA A/D FIFO
     */
	SDM35541_DMA_FIFO_ADC = 0,
    /**
     * DMA D/A 1 FIFO
     */
	SDM35541_DMA_FIFO_DAC1,
    /**
     * DMA D/A 2 FIFO
     */
	SDM35541_DMA_FIFO_DAC2,
    /**
     * DMA HSDIN FIFO
     */
	SDM35541_DMA_FIFO_HSDIN
} SDM35541_dma_source_t;

/**
 * @brief
 * DMA Channel
 */
typedef enum {
    /**
     * MURAL DMA Channel 0
     */
	SDM35541_DMA_CHANNEL_ADC = 0,
    /**
     * MURAL DMA Channel 1
     */
    SDM35541_DMA_CHANNEL_DAC1 = 1,
    /**
     * MURAL DMA Channel 2
     */
    SDM35541_DMA_CHANNEL_DAC2 = 2,
    /**
     * MURAL DMA Channel 3
     */
    SDM35541_DMA_CHANNEL_HSDIN = 3,
} SDM35541_dma_channel_t;
/**
 * @} SDM35541_Types_DMA_Enumerations
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Interrupt Enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_INT_Enumerations SDM35541 type Interrupt
 * @{
 */
/**
 * @brief
 * Interrupt Sources. The lower 16-bits are interrupts generated from
 * the IT register. The upper 16-bits are DMA and ALGDIO interrupts.
 */
typedef enum {
    /**
     * FIFO Write
     */
	SDM35541_INT_FIFO_WRITE = 0x0001,
    /**
     *  Reset CGT
     */
	SDM35541_INT_CGT_RESET = 0x0002,
    /**
     * Reserved
     */
	SDM35541_INT_RESERVED = 0x0004,
    /**
     * Pause CGT
     */
	SDM35541_INT_CGT_PAUSE = 0x0008,
    /**
     * About Counter Out
     */
	SDM35541_INT_ABOUT = 0x0010,
    /**
     * Delay Counter Out
     */
	SDM35541_INT_DELAY = 0x0020,
    /**
     * A/D Sample Counter
     */
	SDM35541_INT_SCNT_ADC = 0x0040,
    /**
     * D/A 1 Update Counter
     */
	SDM35541_INT_SCNT_DAC1 = 0x0080,
    /**
     * D/A 2 Update Counter
     */
	SDM35541_INT_SCNT_DAC2 = 0x0100,
    /**
     * User Timer/Counter 1 Out
     */
	SDM35541_INT_UTC1 = 0x0200,
    /**
     * Inverted User Timer/Counter 1 Out
     */
	SDM35541_INT_UTC1_INV = 0x0400,
    /**
     * User Timer/Counter 2 Out
     */
	SDM35541_INT_UTC2 = 0x0800,
    /**
     * Digital Interrupt
     */
	SDM35541_INT_DIO = 0x1000,
    /**
     * External Interrupt
     */
	SDM35541_INT_EXTERNAL = 0x2000,
    /**
     * External Trigger Rising Edge
     */
	SDM35541_INT_ETRIG_RISING = 0x4000,
    /**
     * External Trigger Falling Edge
     */
	SDM35541_INT_ETRIG_FALLING = 0x8000,
    /**
     * DMA Channel 0 -- ENABLED BY DEFAULT
     */
	SDM35541_INT_DMA_0 = 0x00200000,
    /**
     * DMA Channel 1 -- ENABLED BY DEFAULT
     */
	SDM35541_INT_DMA_1 = 0x00400000,
    /**
     * Analog DIO Pin 1 Pos Edge
     */
	SDM35541_INT_ALGDIO_POS_PIN1 = 0x04000000,
    /**
     * Analog DIO Pin 2 Pos Edge
     */
	SDM35541_INT_ALGDIO_POS_PIN2 = 0x08000000,
    /**
     * Analog DIO Pin 1 Neg Edge
     */
	SDM35541_INT_ALGDI0_NEG_PIN1 = 0x10000000,
    /**
     * Analog DIO Pin 2 Neg Edge
     */
	SDM35541_INT_ALGDIO_NEG_PIN2 = 0x20000000
} SDM35541_int_source_t;
/**
 * @} SDM35541_Types_INT_Enumerations
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 User Analog DIO Enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_ALGDIO_Enumerations SDM35541 type Analog DIO
 * @{
 */
/**
 * @brief
 * Analog DIO Mask
 */
typedef enum {
    /**
     * Masked
     */
	SDM35541_ALGDIO_MASKED = 0,
    /**
     * Unmasked
     */
	SDM35541_ALGDIO_UNMASKED
} SDM35541_algdio_mask_t;

/**
 * @brief
 * Analog DIO Pins
 */
typedef enum {
    /**
     * Pin 1
     */
	SDM35541_ALGDIO_PIN1 = 0,
    /**
     * Pin 2
     */
	SDM35541_ALGDIO_PIN2
} SDM35541_algdio_pin_t;

/**
 * @brief
 * Analog DIO Direction
 */
typedef enum {
    /**
     * Input
     */
	SDM35541_ALGDIO_INPUT = 0,
    /**
     * Output
     */
	SDM35541_ALGDIO_OUTPUT
} SDM35541_algdio_direction_t;
/**
 * @} SDM35541_Types_ALGDIO_Enumerations
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 User I/O Enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_UIO_Enumerations SDM35541 type User Output
 * @{
 */
/**
 * @brief
 * User I/O Channel
 */
typedef enum {
    /**
     * User I/O Channel 0
     */
	SDM35541_UIO0 = 0,
    /**
     * User I/O Channel 1
     */
	SDM35541_UIO1
} SDM35541_uio_channel_t;

/**
 * @brief
 * User I/O Source
 */
typedef enum {
    /**
     * A/D Conversion Signal
     */
	SDM35541_UIO_ADC = 0,
    /**
     * D/A 1 Update
     */
	SDM35541_UIO_DAC1,
    /**
     * D/A 2 Update
     */
	SDM35541_UIO_DAC2,
    /**
     * Software Programmable
     */
	SDM35541_UIO_PRG
} SDM35541_uio_source_t;

/**
 * @} SDM35541_Types_UIO_Enumerations
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Digital Input/Output Enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_DIO_Enumerations SDM35541 type Digital Input/Output
 * @{
 */
/**
 * @brief
 * Digital I/O Clock
 */
typedef enum {
    /**
     * 8MHZ Clock
     */
	SDM35541_DIO_CLK_8MHZ = 0,
    /**
     * Programmable Clock
     */
	SDM35541_DIO_CLK_UTC1
} SDM35541_dio_clk_t;

/**
 * @brief
 * Digital I/O IRQ Mode
 */
typedef enum {
    /**
     * Event Mode
     */
	SDM35541_DIO_MODE_EVENT = 0,
    /**
     * Match Mode
     */
	SDM35541_DIO_MODE_MATCH
} SDM35541_dio_mode_t;

/**
 * @brief
 * Digital I/O Port
 */
typedef enum {
    /**
     * Port 0
     */
	SDM35541_DIO_PORT0 = 0,
    /**
     * Port 1
     */
	SDM35541_DIO_PORT1
} SDM35541_dio_port_t;
/**
 * @} SDM35541_Types_DIO_Enumerations
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 External Trigger/Interrupt Enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_EXT_Enumerations SDM35541 type External Trigger/Interrupt
 * @{
 */
/**
 * @brief
 * Polarity
 */
typedef enum {
    /**
     * Positive Edge
     */
	SDM35541_EXT_POLARITY_POS = 0,
    /**
     * Negative Edge
     */
	SDM35541_EXT_POLARITY_NEG,
} SDM35541_ext_polarity_t;
/**
 * @} SDM35541_Types_EXT_Enumerations
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SyncBus Enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_SBUS_Enumerations SDM35541 type SyncBus enumerations
 * @{
 */
/**
 * @brief
 * SyncBus Enumerations
 */
typedef enum {
    /**
     * SyncBus 0
     */
	SDM35541_SBUS0 = 0,
    /**
     * SyncBus 1
     */
	SDM35541_SBUS1,
    /**
     * SyncBus 2
     */
	SDM35541_SBUS2
} SDM35541_sbus_t;

/**
 * @brief
 * SyncBus Source Select
 */
typedef enum {
    /**
     * Software A/D Start
     */
	SDM35541_SBUS_SRC_SOFT_ADC = 0,
    /**
     * Pacer Clock
     */
	SDM35541_SBUS_SRC_PCLK = 1,
    /**
     * Software Pacer Start
     */
	SDM35541_SBUS_SRC_PCLK_START = 1,
    /**
     * Burst Clock
     */
	SDM35541_SBUS_SRC_BCLK = 2,
    /**
     * Software Pacer Stop
     */
	SDM35541_SBUS_SRC_PCLK_STOP = 2,
    /**
     * Digital Interrupt
     */
	SDM35541_SBUS_SRC_DIG_IT = 3,
    /**
     * Software D/A1 Update
     */
	SDM35541_SBUS_SRC_DAC1 = 3,
    /**
     * External Trigger
     */
	SDM35541_SBUS_SRC_ETRIG = 4,
    /**
     * Software D/A2 Update
     */
	SDM35541_SBUS_SRC_DAC2 = 4,
    /**
     * Simultaneous D/A Update
     */
	SDM35541_SBUS_SRC_DAC_UPDATE = 5,
    /**
     * External Pacer Clock
     */
	SDM35541_SBUS_SRC_EPCLK = 5,
    /**
     * D/A Clock
     */
	SDM35541_SBUS_SRC_DAC_CLK = 6,
    /**
     * External Trigger
     */
	SDM35541_SBUS_SRC_ETRIG2 = 6,
    /**
     * User Timer/Counter 2 Out
     */
	SDM35541_SBUS_SRC_UTC2 = 7
} SDM35541_sbus_src_t;

/**
 * @} SDM35541_Types_SBUS_Enumerations
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 HighSpeed Digital Enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_HSDIN_Enumerations SDM35541 type HighSpeed Digital enumerations
 * @{
 */
/**
 * @brief
 * HSDIN Sampling Signal
 */
typedef enum {
    /**
     * Software
     */
	SDM35541_HSDIN_SIGNAL_SOFTWARE = 0,
    /**
     * A/D Conversion Signal
     */
	SDM35541_HSDIN_SIGNAL_ADC,
    /**
     * User Timer/Counter 0 Out
     */
	SDM35541_HSDIN_SIGNAL_UTC0,
    /**
     * User Timer/Counter 1 Out
     */
	SDM35541_HSDIN_SIGNAL_UTC1,
    /**
     * User Timer/Counter 2 Out
     */
	SDM35541_HSDIN_SIGNAL_UTC2,
    /**
     * External Pacer Clock
     */
	SDM35541_HSDIN_SIGNAL_EPCLK,
    /**
     * External Trigger
     */
	SDM35541_HSDIN_SIGNAL_ETRIG
} SDM35541_hsdin_signal_t;

/**
 * @} SDM35541_Types_HSDIN_Enumerations
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Digital to Analog enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_DAC_Enumerations SDM35541 type Digital to Analog enumerations
 * @{
 */
/**
 * @brief
 * DAC primary clock source
 */
typedef enum {
    /**
     * 8 MHz Clock
     */
	SDM35541_DAC_FREQ_8_MHZ = 0,
    /**
     * 20 MHz Clock
     */
	SDM35541_DAC_FREQ_20_MHZ
} SDM35541_dac_freq_t;

/**
 * @brief
 * DAC clock stop source
 */
typedef enum {
    /**
     * Software Pacer Stop
     */
	SDM35541_DAC_CLK_STOP_SOFTWARE_PACER = 0,
    /**
     * External trigger
     */
	SDM35541_DAC_CLK_STOP_ETRIG,
    /**
     * Digital Interrupt
     */
	SDM35541_DAC_CLK_STOP_DIG_IT,
    /**
     * User Timer/Counter 2 out
     */
	SDM35541_DAC_CLK_STOP_UTC2,
    /**
     * Syncbus 0
     */
	SDM35541_DAC_CLK_STOP_SBUS0,
    /**
     * Syncbus 1
     */
	SDM35541_DAC_CLK_STOP_SBUS1,
    /**
     * Syncbus 2
     */
	SDM35541_DAC_CLK_STOP_SBUS2,
    /**
     * Software DAC clock stop
     */
	SDM35541_DAC_CLK_STOP_SOFTWARE,
    /**
     * DAC1 Update Counter
     */
	SDM35541_DAC_CLK_STOP_DAC1_UCNT,
    /**
     * DAC2 Update Counter
     */
	SDM35541_DAC_CLK_STOP_DAC2_UCNT
} SDM35541_dac_clk_stop_t;

/**
 * @brief
 * DAC clock start source
 */
typedef enum {
    /**
     * Software pacer start
     */
	SDM35541_DAC_CLK_START_SOFTWARE_PACER = 0,
    /**
     * External trigger
     */
	SDM35541_DAC_CLK_START_ETRIG,
    /**
     * Digital Interrupt
     */
	SDM35541_DAC_CLK_START_DIG_IT,
    /**
     * User Timer/Counter 2 out
     */
	SDM35541_DAC_CLK_START_UTC2,
    /**
     * SyncBus 0
     */
	SDM35541_DAC_CLK_START_SBUS0,
    /**
     * SyncBus 1
     */
	SDM35541_DAC_CLK_START_SBUS1,
    /**
     * SyncBus 2
     */
	SDM35541_DAC_CLK_START_SBUS2,
    /**
     * Software DAC clock start
     */
	SDM35541_DAC_CLK_START_SOFTWARE
} SDM35541_dac_clk_start_t;
/**
 * @brief
 * DAC Cycle Mode
 */
typedef enum _SDM35541_dac_mode {
    /**
     * Not cycle
     */
	SDM35541_DAC_MODE_NOT_CYCLE = 0,
    /**
     * Cycle
     */
	SDM35541_DAC_MODE_CYCLE
} SDM35541_dac_mode_t;

/**
 * @brief
 * DAC Update Source
 */
typedef enum {
    /**
     * Software DAC Update
     */
	SDM35541_DAC_UPDATE_SOFTWARE = 0,
    /**
     * CGT Controlled Update
     */
	SDM35541_DAC_UPDATE_CGT,
    /**
     * DAC Clock
     */
	SDM35541_DAC_UPDATE_CLOCK,
    /**
     * External pacer clock
     */
	SDM35541_DAC_UPDATE_EPCLK,
    /**
     * Syncbus 0
     */
	SDM35541_DAC_UPDATE_SBUS0,
    /**
     * Syncbus 1
     */
	SDM35541_DAC_UPDATE_SBUS1,
    /**
     * Syncbus 2
     */
	SDM35541_DAC_UPDATE_SBUS2
} SDM35541_dac_update_src_t;

/**
 * @brief
 * DAC Output Range
 */
typedef enum {
    /**
     * Unipolar 0V to 5V
     */
	SDM35541_DAC_RANGE_UNIPOLAR_5 = 0,
    /**
     * Unipolar 0V to 10V
     */
	SDM35541_DAC_RANGE_UNIPOLAR_10,
    /**
     * Bipolar -5V to 5V
     */
	SDM35541_DAC_RANGE_BIPOLAR_5,
    /**
     * Bipolar -10V to 10V
     */
	SDM35541_DAC_RANGE_BIPOLAR_10
} SDM35541_dac_range_t;

/**
 * @brief
 * DAC channels
 *
 * Note: These are given these values specifically so
 * they can be bitwise combined and compared.
 */
typedef enum {
     /**
      * Digital to Analog channel 1
      */
	SDM35541_DAC1 = 1,
     /**
      * Digital to Analog channel 2
      */
	SDM35541_DAC2
} SDM35541_dac_channel_t;

/**
 * @brief
 * DAC Clock Mode
 */
typedef enum {
    /**
     * Free Run Mode
     */
	SDM35541_DAC_CLK_FREE_RUN = 0,
    /**
     * Start/Stop Mode
     */
	SDM35541_DAC_CLK_START_STOP
} SDM35541_dac_clk_mode_t;
/**
 * @} SDM35541_Types_DAC_Enumerations
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Analog to Digital enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_ADC_Enumerations SDM35541 type Analog to Digital enumerations
 * @{
 */

/**
 * @brief
 * ADC Sample Counter Source
 */
typedef enum {
    /**
     * Reset Channel Gain Table
     */
	SDM35541_ADC_SCNT_SRC_CGT = 0,
    /**
     * A/D FIFO Write
     */
	SDM35541_ADC_SCNT_SRC_FIFO
} SDM35541_adc_scnt_src_t;

/**
 * @brief
 * ADC Conversion Signal Select
 */
typedef enum {
    /**
     * Software
     */
	SDM35541_ADC_CONV_SIGNAL_SOFTWARE = 0,
    /**
     * Pacer Clock
     */
	SDM35541_ADC_CONV_SIGNAL_PCLK,
    /**
     * Burst Clock
     */
	SDM35541_ADC_CONV_SIGNAL_BCLK,
    /**
     * Digital Interrupt
     */
	SDM35541_ADC_CONV_SIGNAL_DIG_IT,
    /**
     * DAC1 Marker Bit 1
     */
	SDM35541_ADC_CONV_SIGNAL_DAC1_MRKR1,
    /**
     * DAC2 Marker Bit 2
     */
	SDM35541_ADC_CONV_SIGNAL_DAC2_MRKR1,
    /**
     * SyncBus 0
     */
	SDM35541_ADC_CONV_SIGNAL_SBUS0,
    /**
     * SyncBus 1
     */
	SDM35541_ADC_CONV_SIGNAL_SBUS1,
    /**
     * SyncBus 2
     */
	SDM35541_ADC_CONV_SIGNAL_SBUS2
} SDM35541_adc_conv_signal_t;

/**
 * @} SDM35541_Types_ADC_Enumerations
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Burst Clock enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_BCLK_Enumerations SDM35541 type Burst Clock enumerations
 * @{
 */

/**
 * @brief
 * Burst Clock primary frequency
 */
typedef enum {
    /**
     * 8 MHz Clock
     */
	SDM35541_BCLK_FREQ_8_MHZ = 0,
    /**
     * 20 MHz Clock
     */
	SDM35541_BCLK_FREQ_20_MHZ
} SDM35541_bclk_freq_t;

/**
 * @brief
 * Burst Clock Start Trigger
 */
typedef enum {
    /**
     * Software A/D
     */
	SDM35541_BCLK_START_SOFTWARE = 0,
    /**
     * Pacer Clock
     */
	SDM35541_BCLK_START_PACER,
    /**
     * External Trigger
     */
	SDM35541_BCLK_START_ETRIG,
    /**
     * Digital Interrupt
     */
	SDM35541_BCLK_START_DIG_IT,
    /**
     * SyncBus 0
     */
	SDM35541_BCLK_START_SBUS0,
    /**
     * SyncBus 1
     */
	SDM35541_BCLK_START_SBUS1,
    /**
     * SyncBus 2
     */
	SDM35541_BCLK_START_SBUS2
} SDM35541_bclk_start_t;
/**
 * @} SDM35541_Types_BCLK_Enumerations
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Pacer Clock enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_PCLK_Enumerations SDM35541 type Pacer Clock enumerations
 * @{
 */

/**
 * @brief
 * Pacer Clock Trigger Mode
 */
typedef enum _SDM35541_pclk_mode {

    /**
     * Single Cycle Mode
     */
	SDM35541_PCLK_NO_REPEAT = 0,

    /**
     * Repeat Mode
     */
	SDM35541_PCLK_REPEAT
} SDM35541_pclk_mode_t;

/**
 * @brief
 * Pacer Clock Stop
 */
typedef enum {
    /**
     * Software
     */
	SDM35541_PCLK_STOP_SOFTWARE = 0,
    /**
     * External Trigger
     */
	SDM35541_PCLK_STOP_ETRIG,
    /**
     * Digital Interrupt
     */
	SDM35541_PCLK_STOP_DIGITAL_IT,
    /**
     * About Counter
     */
	SDM35541_PCLK_STOP_ACNT,
    /**
     * User Timer/Counter 2 Out
     */
	SDM35541_PCLK_STOP_UTC2,
    /**
     * SyncBus 0
     */
	SDM35541_PCLK_STOP_SBUS0,
    /**
     * SyncBus 1
     */
	SDM35541_PCLK_STOP_SBUS1,
    /**
     * SyncBus 2
     */
	SDM35541_PCLK_STOP_SBUS2,
    /**
     * About Software
     */
	SDM35541_PCLK_STOP_ASOFTWARE,
    /**
     * About External Trigger
     */
	SDM35541_PCLK_STOP_AETRIG,
    /**
     * About Digital Interrupt
     */
	SDM35541_PCLK_STOP_ADIGITAL_IT,
    /**
     * Reserved
     */
	SDM35541_PCLK_STOP_RES,
    /**
     * About User Timer/Counter 2 Out
     */
	SDM35541_PCLK_STOP_AUTC2,
    /**
     * About SyncBus 0
     */
	SDM35541_PCLK_STOP_ASBUS0,
    /**
     * About SyncBus 1
     */
	SDM35541_PCLK_STOP_ASBUS1,
    /**
     * About SyncBus 2
     */
	SDM35541_PCLK_STOP_ASBUS2
} SDM35541_pclk_stop_t;

/**
 * @brief
 * Pacer Clock Start
 */
typedef enum {
    /**
     * Software
     */
	SDM35541_PCLK_START_SOFTWARE = 0,
    /**
     * External Trigger
     */
	SDM35541_PCLK_START_ETRIG,
    /**
     * Digital Interrupt
     */
	SDM35541_PCLK_START_DIGITAL_IT,
    /**
     * User Timer/Counter 2 Out
     */
	SDM35541_PCLK_START_UTC2,
    /**
     * SyncBus 0
     */
	SDM35541_PCLK_START_SBUS0,
    /**
     * SyncBus 1
     */
	SDM35541_PCLK_START_SBUS1,
    /**
     * SyncBus 2
     */
	SDM35541_PCLK_START_SBUS2,
    /**
     * Reserved
     */
	SDM35541_PCLK_START_RES,
    /**
     * Delayed Software
     */
	SDM35541_PCLK_START_DSOFTWARE,
    /**
     * Delayed External Trigger
     */
	SDM35541_PCLK_START_DETRIG,
    /**
     * Delayed Digital Interrupt
     */
	SDM35541_PCLK_START_DDIGITAL_IT,
    /**
     * Delayed User Timer/Counter 2 Out
     */
	SDM35541_PCLK_START_DUTC2,
    /**
     * Delayed SyncBus 0
     */
	SDM35541_PCLK_START_DSBUS0,
    /**
     * Delayed SyncBus 1
     */
	SDM35541_PCLK_START_DSBUS1,
    /**
     * Delayed SyncBus 2
     */
	SDM35541_PCLK_START_DSBUS2,
    /**
     * External Trigger Gated
     */
	SDM35541_PCLK_START_ETRIG_GATE
} SDM35541_pclk_start_t;
/**
 * @brief
 * Pacer Clock Select
 */
typedef enum {

    /**
     * External Pacer Clock
     */
	SDM35541_PCLK_EXTERNAL = 0,

    /**
     * Internal Pacer Clock
     */
	SDM35541_PCLK_INTERNAL
} SDM35541_pclk_select_t;

 /**
 * @brief
 * Pacer Clock Frequency Select
 */
typedef enum {

    /**
     * 8Mhz Frequency
     */
	SDM35541_PCLK_FREQ_8_MHZ = 0,

    /**
     * 20Mhz Frequency
     */
	SDM35541_PCLK_FREQ_20_MHZ
} SDM35541_pclk_freq_t;

/**
 * @} SDM35541_Types_PCLK_Enumerations
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
8254 timer/counter enumerations
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @defgroup SDM35541_Types_UTC_Enumerations SDM35541 type timer/counter enumerations
 * @{
 */

/**
 * @brief
 *      8254 timers/counters
 */
typedef enum {

    /**
     * Timer 0 on 8254 chip
     */

	SDM35541_UTC_0 = 0,

    /**
     * Timer 1 on 8254 chip
     */

	SDM35541_UTC_1,

    /**
     * Timer 2 on 8254 chip
     */

	SDM35541_UTC_2
} SDM35541_utc_timer_t;

/**
 * @brief
 *      8254 timer/counter clock selectors
 */

typedef enum {

    /**
     * 8 MHz clock
     */

	SDM35541_CUTC_8_MHZ = 0,

    /**
     * External Timer Counter Clock 1
     */

	SDM35541_CUTC_EXT_TC_CLOCK_1 = 1,

    /**
     * External Timer Counter Clock 2
     */

	SDM35541_CUTC_EXT_TC_CLOCK_2 = 2,

    /**
     * External Pacer Clock
     */

	SDM35541_CUTC_EXT_PCLK = 3,

    /**
     * User Timer/Counter 0 Out
     */

	SDM35541_CUTC_UTC_0_OUT = 4,

    /**
     * User Timer/Counter 1 Out
     */

	SDM35541_CUTC_UTC_1_OUT = 4,

    /**
     * High Speed Digital Input Sample Signal
     */

	SDM35541_CUTC_HSDIN_SIGNAL = 5
} SDM35541_utc_clk_t;

/**
 * @brief
 *      8254 timer/counter gate selectors
 */
typedef enum {

    /**
     * Logic 0
     */

	SDM35541_GUTC_NOT_GATED = 0,

    /**
     * Logic 1
     */

	SDM35541_GUTC_GATED = 1,

    /**
     * 8254 timer/counter
     */

	SDM35541_GUTC_EXT_TC_CLK_1 = 2,

    /**
     * 8254 timer/counter
     */

	SDM35541_GUTC_EXT_TC_CLK_2 = 3,

    /**
     * 8254 timer/counter
     */

	SDM35541_GUTC_UTC_0_OUT = 4,

    /**
     * 8254 timer/counter
     */

	SDM35541_GUTC_UTC_1_OUT = 4,

} SDM35541_utc_gate;

/**
 * @brief
 *      8254 timer/counter waveform mode selectors
 */
typedef enum {

    /**
     * Event counter
     */

	SDM35541_UTC_MODE_EVENT_COUNTER = 0,

    /**
     * Programmable one shot
     */

	SDM35541_UTC_MODE_PROG_ONE_SHOT,

    /**
     * Rate generator
     */

	SDM35541_UTC_MODE_RATE_GENERATOR,

    /**
     * Square wave generator
     */

	SDM35541_UTC_MODE_SQUARE_WAVE,

    /**
     * Software triggered strobe
     */

	SDM35541_UTC_MODE_SOFTWARE_STROBE,

    /**
     * Hardware triggered strobe
     */

	SDM35541_UTC_MODE_HARDWARE_STROBE
} SDM35541_utc_mode;

/**
 * @} SDM35541_Types_UTC_Enumerations
 */

/**
 * @} SDM35541_Types_Enumerations
 */

/*=============================================================================
Structures
 =============================================================================*/

/**
 * @defgroup SDM35541_Types_Structures SDM35541 type definition structures
 * @{
 */
/**
 * @brief
 *      FIFO status
 */
typedef enum {
    /**
     * DAC1 FIFO Not Empty
     */
	SDM35541_FIFO_DAC1_NOT_EMPTY = 0x0001,
    /**
     * DAC1 FIFO Half Empty
     */
	SDM35541_FIFO_DAC1_HALF_EMPTY = 0x0002,
    /**
     * DAC1 FIFO Not Full
     */
	SDM35541_FIFO_DAC1_NOT_FULL = 0x0004,
    /**
     * DAC2 FIFO Not Empty
     */
	SDM35541_FIFO_DAC2_NOT_EMPTY = 0x0010,
    /**
     * DAC2 FIFO Half Empty
     */
	SDM35541_FIFO_DAC2_HALF_EMPTY = 0x0020,
    /**
     * DAC2 FIFO Not Full
     */
	SDM35541_FIFO_DAC2_NOT_FULL = 0x0040,
    /**
     * ADC FIFO Not Empty
     */
	SDM35541_FIFO_ADC_NOT_EMPTY = 0x0100,
    /**
     * ADC FIFO Half Empty
     */
	SDM35541_FIFO_ADC_HALF_EMPTY = 0x0200,
    /**
     * ADC FIFO Not Full
     */
	SDM35541_FIFO_ADC_NOT_FULL = 0x0400,
    /**
     * HSDIN FIFO Not Empty
     */
	SDM35541_FIFO_HSDIN_NOT_EMPTY = 0x1000,
    /**
     * HSDIN FIFO Half Empty
     */
	SDM35541_FIFO_HSDIN_HALF_EMPTY = 0x2000,
    /**
     * HSDIN FIFO Not Full
     */
	SDM35541_FIFO_HSDIN_NOT_FULL = 0x4000
} SDM35541_fifo_status_t;

/**
 * @brief
 *      Interrupts status
 */
struct SDM35541_int_status {

    /**
     * Number of interrupts remaining in the interrupt status queue
     */

	int int_remaining;

    /**
     * Number of interrupts missed
     */

	unsigned int int_missed;

    /**
     * Interrupt Status
     */

	uint32_t status;
};

/**
 * @brief
 *      Channel gain table entry
 */
typedef struct {
    /**
     * Analog input channel
     */
	uint8_t channel:4;
    /**
     * Gain
     */
	uint8_t gain:3;
    /**
     * AGND/AINSENSE
     */
	uint8_t nrse:1;
    /**
     * Output Range
     */
	uint8_t range:2;
    /**
     * Single/Differential
     */
	uint8_t ground:1;
    /**
     * Pause
     */
	uint8_t pause:1;
    /**
     * DAC1 Update
     */
	uint8_t dac1:1;
    /**
     * DAC2 Update
     */
	uint8_t dac2:1;
    /**
     * Skip
     */
	uint8_t skip:1;
    /**
     * Reserved
     */
	uint8_t reserved:1;
} SDM35541_cgt_entry_t;


/********************************************************************************
 @brief
    Enable one or more SDM35541 interrupt source(s).

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	int_source

	Interrupt source to enable.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_Interrupt_Enable(struct SDM35541_Board_Descriptor *handle,
			const struct SDM35541_Function_Block *func_block,
			SDM35541_int_source_t mask);


/********************************************************************************
 @brief
    Disable one or more SDM35541 interrupt source(s).

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	int_source

	Interrupt source to disable.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_Interrupt_Disable(struct SDM35541_Board_Descriptor *handle,
			const struct SDM35541_Function_Block *func_block,
			SDM35541_int_source_t mask);

/******************************************************************************
 * @brief
 *      Gets the SDM35541_IT_OVERRUN register
 * @param
 *      handle
 * 
 *      Pointer to the board handler.
 * 
 * @param
 *      func_block
 * 
 *      Pointer to the adc_1002 function block.
 * 
 * @param
 *      reg
 *      
 *      pointer to IT_OVERRUN value
 * 
 * @return
    0
    Success.

 * @return
    -1
    Failure.
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Get_IT_Overrun(struct SDM35541_Board_Descriptor *handle,
			const struct SDM35541_Function_Block *func_block,
			uint16_t *reg);



/******************************************************************************
 * @brief
 *      Clears the SDM35541_INT register
 * 
 * @param
 *      handle
 * 
 *      Pointer to the board handler.
 * 
 * @param
 *      func_block
 * 
 *      Pointer to the adc_1002 function block.
 * 
 * @param
 *      mask
 * 
 *      Bitmasks of the interrupts to clear. Only the lower
 *      16-bits of this enum can be used.
 * 
 * @return
    0
    Success.

 * @return
    -1
    Failure.
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Clear_IT_Mask(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_int_source_t mask);


/******************************************************************************
 * @brief
 *      Enables interrupts in the SDM35541_INT register
 * 
 * @param
 *      handle
 * 
 *      Pointer to the board handler.
 * 
 * @param
 *      func_block
 * 
 *      Pointer to the adc_1002 function block.
 * 
 * @param
 *      mask
 * 
 *      Bitmasks of the interrupts to enable. Only the lower
 *      16-bits of this enum can be used.
 * 
 * @return
    0
    Success.

 * @return
    -1
    Failure.
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Set_IT_Mask(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_int_source_t mask);


/******************************************************************************
 * @brief
 *      Gets the SDM35541_INT register
 * 
 * @param
 *      handle
 * 
 *      Pointer to the board handler.
 * 
 * @param
 *      func_block
 * 
 *      pointer to the adc_1002 function block
 * 
 * @param
 *      reg
 *  
 *      Pointer to output bitmask of the interrupts fired.
 * 
 * @return
    0
    Success.

 * @return
    -1
    Failure.
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Get_IT_Mask(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t *reg);

            
/******************************************************************************
 * @brief
 *      Clears the SDM35541_IT_OVERRUN register
 * 
 * @param
 *      handle
 * 
 *      Pointer to the board handler.
 * @param
 *      func_block
 * 
 *      Pointer to the adc_1002 function block.
 * 
 * @return
 *      0
 *      Success.
 *
 * @return
        -1
        Failure.
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_Clear_IT_Overrun(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block);


/*********************************************************************************
 @brief
    Set the demand mode request source for a specified DMA channel.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    channel

    The specified channel for which to set the request source.

 @param
    request

    The demand mode request source to set.

 @return
    0

    Success

 @return
    -1

    Failure

 @internal
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DMA_Request_Source(struct SDM35541_Board_Descriptor *handle,
			  const struct SDM35541_Function_Block *func_block,
			  SDM35541_dma_channel_t channel,
			  SDM35541_dma_request_t request);


/******************************************************************************
 * @brief
 *      Sets the FIFO target and demand type for the DMA engine.
 * 
 * @param
 *      handle
 * 
 *      Pointer to the board handler.
 * @param
 *      func_block
 * 
 *      Pointer to the adc_1002 function block
 * 
 * @param
 *       channel
 *       
 *       DMA FIFO to use (0 or 1)
 * @param
 *       dma_source
 *       
 *       Whether ADC, DAC, or HSDIN will be used for DMA
 * 
 * @param
 *       request
 * 
 *       Sets demand type
 * 
 * @return
    0
    Success.

 * @return
    -1
    Failure.
 * 
 * @return
 *      0
 *      Success.
 *
 * @return
        -1
        Failure.
 ******************************************************************************/
SDM35541LIB_API
int SDM35541_DMA_Set_Source(struct SDM35541_Board_Descriptor *handle,
            const struct SDM35541_Function_Block *func_block,
            unsigned int channel, SDM35541_dma_source_t dma_source,
            SDM35541_dma_request_t request);


/**********************************************************************************
 @brief
    Retrieve the FIFO size of the board from the kernel space device descriptor.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    data

    Address of the variable in which to store the fifo size.

 @retval
    0

    Success

 @retval
    -1

    Failure

 @note

    This function does not calculate the value of the FIFO upon each call.  The
    FIFO size is determined at 'insmod' time and is stored in the drivers device
    descriptor.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_FIFO_Size(struct SDM35541_Board_Descriptor * handle, unsigned int *data);


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
User Timer/Counter Functions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @defgroup SDM35541_Library_UTC_Functions SDM35541 user library user timer/counter control
 * @{
 */

/*********************************************************************************
 @brief
    Set a User Timer/Counter Clock Source

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	utc

	User Timer/Counter we are configuring.

 @param
	source

	The User Timer/Counter source to be set.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Set_Clock_Source(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_utc_timer_t utc, SDM35541_utc_clk_t source);


/*********************************************************************************
 @brief
    Set a User Timer/Counter Gate

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	utc

	User Timer/Counter we are configuring.

 @param
	gate

	The User Timer/Counter gate option.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Set_Gate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_utc_timer_t utc, SDM35541_utc_gate gate);


/*********************************************************************************
 @brief
    Set a User Timer/Counter Mode

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	utc

	User Timer/Counter we are configuring.

 @param
	mode

	The User Timer/Counter mode to be set.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Set_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_utc_timer_t utc, SDM35541_utc_mode mode);

            
/*********************************************************************************
 @brief
    Get a User Timer/Counter Mode

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	utc

	Which user timer/counter's mode to read.

 @param
	mode

	Variable to store the retrieved mode value.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Get_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_utc_timer_t utc, uint16_t * mode);


/**
*******************************************************************************
 @brief
    Set a User Timer/Counter Divisor

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	utc

	User Timer/Counter we are configuring.

 @param
	rate

	The rate to set for this User Timer/Counter

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Set_Divisor(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_utc_timer_t utc, uint16_t divisor);


/**
*******************************************************************************
 @brief
    Return current value of a User Timer/Counter

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	utc

	User Timer/Counter we are configuring.

 @param
	count

	Variable to store the current count in.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Get_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		     SDM35541_utc_timer_t utc, uint16_t * count);


/*********************************************************************************
 @brief
    Return current status of a User Timer/Counter

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	utc_select

	User Timer/Counter we are configuring.

 @param
	utc_status

	Variable to store the current status in.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Get_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_utc_timer_t utc_select, uint8_t * utc_status);

/*********************************************************************************
 @brief
    Setup a User Timer/Counter

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	utc

	User Timer/Counter we are configuring.

 @param
	source

	The User Timer/Counter source to be set.

 @param
	gate

	The User Timer/Counter gate option to set.

 @param
	mode

	The User Timer/Counter mode option to set.

 @param
	divisor

	The divisor to set the User Timer/Counter with.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UTC_Setup(struct SDM35541_Board_Descriptor * handle,
        const struct SDM35541_Function_Block *func_block,
        SDM35541_utc_timer_t utc,
        SDM35541_utc_clk_t source,
        SDM35541_utc_gate gate, SDM35541_utc_mode mode, uint16_t divisor);
/**
 * @} SDM35541_Library_UTC_Funtions
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 General Status Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief
 * @{ SDM35541_Library_STATUS_Functions SDM35541 user library status
 */

 /*********************************************************************************
 @brief
    Get current FIFO Status

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	fifo_status

	Variable in which to store the current status.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_FIFO_Get_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * fifo_status);


/********************************************************************************
 @brief
    Get status of pacer/burst clocks.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	status

	Variable in which to store the current status.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_CLK_Get_Status(struct SDM35541_Board_Descriptor * handle, 
            const struct SDM35541_Function_Block *func_block, uint16_t * status);
/**
 * @} SDM35541_Library_STATUS_Functions
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Burst Clock Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @defgroup SDM35541_Library_BCLK_Functions SDM35541 user library burst clock control
 * @{
 */
/********************************************************************************
 @brief
   	Get the current Burst Clock count

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Address of the variable to store the value.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Get_Count(struct SDM35541_Board_Descriptor * handle, 
            const struct SDM35541_Function_Block *func_block, uint16_t * data);


/********************************************************************************
 @brief
   	Set the current Burst Clock count.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Value to write to the Burst Clock count.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t data);


/********************************************************************************
 @brief
   	Set the Burst Clock rate.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	freq

	Set the Burst Clock primary frequency.

 @param
	rate

	The desired Burst Clock rate.

 @param
	actualRate

	The actual rate set.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Set_Rate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_bclk_freq_t freq, float rate, float *actualRate);


/********************************************************************************
 @brief
   	Set Burst Clock start trigger

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	start

	Start trigger to set.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Set_Start(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_bclk_start_t start);


/********************************************************************************
 @brief
   	Set the Burst Clock primary frequency

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	freq

	Frequency to select.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Set_Frequency(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, 
            SDM35541_bclk_freq_t freq);


/********************************************************************************
 @brief
    Setup Burst Clock

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	start

	Burst Clock start trigger.

 @param
	freq

	Burst Clock primary frequency.

 @param
	rate

	Rate at which to set the clock.

 @param
	actualRate

	Rate at which the clock is actually set.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_BCLK_Setup(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_bclk_start_t start,
            SDM35541_bclk_freq_t freq, float rate, float *actualRate);
/**
 * @} SDM35541_Library_BCLK_Funtions
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Pacer Clock Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @defgroup SDM35541_Library_PCLK_Functions SDM35541 user library pacer clock control
 * @{
 */

/*********************************************************************************
 @brief
    Set the Pacer Clock frequency

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	pclk_freq

	Frequency to set for the pacer clock.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Frequency(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_freq_t pclk_freq);


/********************************************************************************
 @brief
   	Set the Pacer Clock source

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	pclk_select

	Source for the Pacer Clock

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Source(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_select_t pclk_select);


/********************************************************************************
 @brief
   	Set the Pacer Clock start trigger

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	pclk_start

	Start trigger for the Pacer Clock

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Start(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_start_t pclk_start);


/********************************************************************************
 @brief
   	Set the Pacer Clock stop trigger

 @param
    handle

    Address of device's library board descriptor.

 @param
	pclk_stop

	Stop trigger for the Pacer Clock

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Stop(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_stop_t pclk_stop);


/********************************************************************************
 @brief
   	Read the current pacer clock value

 @param
    handle

    Address of device's library board descriptor.

 @param
	pacer_value

	Address of the variable to store the value of the pacer

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint32_t * pacer_value);


/********************************************************************************
 @brief
   	Set the Pacer Clock trigger mode.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	pclk_mode

	Mode in which to set the pacer clock.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Trigger_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_mode_t pclk_mode);


/*********************************************************************************
 @brief
   	Set the Pacer Clock Count

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	count

	Pacer Clock count

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint32_t count);


/********************************************************************************
 @brief
   	Set the Pacer Clock Rate.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	freq

	Pacer Clock primary frequency.

 @param
	rate

	Rate desired or the Pacer Clock.

 @param
	actualRate

	Address to store the actual rate value.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Set_Rate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_freq_t freq, float rate, float *actualRate);


/********************************************************************************
 @brief
   	Setup the Pacer Clock

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	pclk_select

	Select Internal/External Pacer Clock.

 @param
	pclk_freq

	Select the primary clock frequency.

 @param
	pclk_mode

	Select the trigger mode.

 @param
	pclk_start

	Select the start trigger.

 @param
	pclk_stop

	Select the stop trigger.

 @param
	rate

	Desired rate for the Pacer Clock.

 @param
	actualRate

	Rate actually set.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Setup(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_pclk_select_t pclk_select,
            SDM35541_pclk_freq_t pclk_freq,
            SDM35541_pclk_mode_t pclk_mode,
            SDM35541_pclk_start_t pclk_start,
            SDM35541_pclk_stop_t pclk_stop, float rate, float *actualRate);


/*********************************************************************************
 @brief
   	Software Pacer Clock Start.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Start(struct SDM35541_Board_Descriptor * handle, 
            const struct SDM35541_Function_Block *func_block);


/********************************************************************************
 @brief
   	Software Pacer Clock Stop.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_PCLK_Stop(struct SDM35541_Board_Descriptor * handle, 
            const struct SDM35541_Function_Block *func_block);
/**
 * @} SDM35541_Library_PCLK_Functions
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Channel Gain Table Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @defgroup SDM35541_Library_CGT_Functions SDM35541 user library channel gain table
 * @{
 */


/********************************************************************************
 @brief
    Enable/disable A/D channel gain table.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	enable

	A 0 denotes CGT Disable and CG Latch Enable, a 1 denotes CGT Enable and
	CG Latch Disable.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Enable(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t enable);


/********************************************************************************
 @brief
    Enable/disable Digital Table.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	enable

	Enable/Disable the Digital Table and Digital I/O Port 1.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DT_Enable(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t enable);

            
/********************************************************************************
 @brief
    Write ADC channel gain table latch for single channel sampling.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	cgt

	Channel gain table entry to write to the latch.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Latch(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_cgt_entry_t cgt);


/********************************************************************************
 @brief
    Write a channel gain table entry.  This function utilizes
    SDM35541_CGT_Create_Entry() to create the 16 bit entry.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	cgt

	The channel gain table entry to write.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Write(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_cgt_entry_t cgt);

            
/********************************************************************************
 @brief
    Reset Channel Gain Table.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Reset(struct SDM35541_Board_Descriptor * handle, 
            const struct SDM35541_Function_Block *func_block);

/********************************************************************************
 @brief
    Clear Channel Gain Table.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Clear(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block);


/********************************************************************************
 @brief
    Create a channel gain table entry.

 @param
    cgt

    Struct that holds the values for the channel gain table.

 @param
	cgt_entry

	The channel gain table converted to a uint16_t for register entry.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

 @note
    This function should not be called by the user.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Create_Entry(SDM35541_cgt_entry_t * cgt, uint16_t * cgt_entry);


/********************************************************************************
 @brief
    Write Digital Table entry.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Entry to add to the digital table.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DT_Write_Entry(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t data);

/********************************************************************************
 @brief
    Pause the Channel Gain Table

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	pause

	Enable/Disable CGT Pause.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note

	Pause is ignored in burst mode.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_CGT_Pause(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t pause);

            
/**
 * @} SDM35541_Library_CGT_Functions
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Analog to Digital Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief
 * @{ SDM35541_Library_ADC_Functions SDM35541 user library analog to digital
 */
/**
********************************************************************************
 @brief
    Read a value from the A/D FIFO

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	value

	Address of the variable to store the value read.

 @return
    0

    Success

 @return
	-1

    Failure.
	errno may be set as follows:
		@arg \c
			EINVAL		pointer was NULL.

    Please see the close(2) man page for information on other possible values
    errno may have in this case.

********************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_FIFO_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * value);


/********************************************************************************
 @brief
    Analog to Digital Software Sample

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_Software_Sample(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block);


/********************************************************************************
 @brief
    Select the A/D Conversion Signal

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	adc_conv_signal

	The A/D conversion signal to select.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_Conv_Signal(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_adc_conv_signal_t adc_conv_signal);

/********************************************************************************
 @brief
    Select the A/D Sample Counter Source

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	src

	The selected sample counter source.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_SCNT_Source(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_adc_scnt_src_t src);


/********************************************************************************
 @brief
    Enable/Disable About Counter stop

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	enable

	Enable/Disable.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_About_Enable(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t enable);


/********************************************************************************
 @brief
    Clear Analag to Digital FIFO

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_Clear(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block);


/********************************************************************************
 @brief
    Read the value in the A/D Sample Counter

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Address of the variable to store the data.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_SCNT_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t * data);


/*********************************************************************************
 @brief
    Load a value into the A/D Sample Counter.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Value to load into the sample counter.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ADC_SCNT_Load(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t data);

            
/**
 * @} SDM35541_Library_ADC_Functions
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Digital to Analog Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief
 * @{ SDM35541_Library_DAC_Functions SDM35541 user library digital to analog
 */
/**
*******************************************************************************
 @brief
    Cause a DAC software update on the specified channel.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specific DAC channel(s).

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Soft_Update(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t dac);


/********************************************************************************
 @brief
    Get DAC update counter for a specified channel.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specific DAC channel.

 @param
	data

	Address of the variable to store the data.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Get_Update_Counter(struct SDM35541_Board_Descriptor * handle,
                const struct SDM35541_Function_Block *func_block,
                SDM35541_dac_channel_t dac, uint16_t * data);

/********************************************************************************
 @brief
    Set the DAC update counter for a specified channel.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specific DAC channel.

 @param
	data

	The value to write to the DAC update counter

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Update_Counter(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac, uint16_t data);


/********************************************************************************
 @brief
    Set the DAC output range for a specified channel.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specified DAC channel.

 @param
	range

	The specified output range.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Range(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac, SDM35541_dac_range_t range);


/********************************************************************************
 @brief
    Set the DAC Update Source for the specified channel.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specified DAC channel.

 @param
	src

	The specified update source.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Update_Source(struct SDM35541_Board_Descriptor * handle,
                const struct SDM35541_Function_Block *func_block,
                SDM35541_dac_channel_t dac,
                SDM35541_dac_update_src_t src);


/********************************************************************************
 @brief
    Set the DAC mode for a specified channel.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specified DAC channel.

 @param
	mode

	The specified mode.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_dac_channel_t dac, SDM35541_dac_mode_t mode);


/********************************************************************************
 @brief
    Write a value to the DAC FIFO of a specified channel.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specified DAC channel.

 @param
	data

	Value to write to the DAC FIFO.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_FIFO_Write(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac, uint16_t data);


/********************************************************************************
 @brief
    Set the primary slock frequency for DAC conversion.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	freq

	The specified primary clock frequency.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Frequency(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_freq_t freq);


/********************************************************************************
 @brief
    Set the DAC Clock Count.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	count

	The value to which to set the DAC Clock Count

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
	This function calls SDM35541_DAC_Set_Clock()
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint32_t count);

            
/********************************************************************************
 @brief
    Gets the DAC Clock Count.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	count

	Address of the variable to store the DAC clock count.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Get_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint32_t *count);


/********************************************************************************
 @brief
    Set the DAC conversion rate.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	freq

	The specified primary frequency.

 @param
	rate

	The chosen rate for conversion (in Hz).

 @param
	actualRate

	Address of the variable to store the precise rate the clock was set to.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
	This function calls SDM35541_DAC_Set_Clock()
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Rate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_dac_freq_t freq, uint32_t rate, float *actualRate);


/********************************************************************************
 @brief
    Set the DAC Clock Stop Value

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	stop

	The selected clock stop value to be written.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
	This function calls SDM35541_DAC_Set_Clock()
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Clock_Stop(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_clk_stop_t stop);


/********************************************************************************
 @brief
    Set the DAC Clock Start Value

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	start

	The selected clock start value to be written.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
	This function calls SDM35541_DAC_Set_Clock()
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_Clock_Start(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_clk_start_t start);


/********************************************************************************
 @brief
    Causes a DAC Software Start.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
	This function calls SDM35541_DAC_Set_Clock()
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Start(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block);


/********************************************************************************
 @brief
    Causes a DAC Software Stop.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
	This function calls SDM35541_DAC_Set_Clock()
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Stop(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block);



 /********************************************************************************
 @brief
    Setup a DAC channel.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specified DAC channel.

 @param
	range

	The specified DAC range.

 @param
	src

	The specified DAC update source.

 @param
	mode

	The specified DAC mode.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Setup(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac,
            SDM35541_dac_range_t range,
            SDM35541_dac_update_src_t src, SDM35541_dac_mode_t mode);


/********************************************************************************
 @brief
    Reset a DAC Fifo.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specified DAC channel.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Reset(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac);


/********************************************************************************
 @brief
    Clear a DAC Fifo.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	dac

	The specified DAC channel.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Clear(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dac_channel_t dac);


/********************************************************************************
 @brief
    Set DAC Clock Mode

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	clk_mode

	The mode set the DAC Clock (Free Run or Start/Stop).

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DAC_Set_CLK_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
			SDM35541_dac_clk_mode_t clk_mode);


/**
 * @} SDM35541_Library_DAC_Functions
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Highspeed Digital Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief
 * @{ SDM35541_Library_HSDIN_Functions SDM35541 user library high speed digital
 */
/**
*******************************************************************************
 @brief
    Software high speed digital input sample command

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
	This function calls SDM35541_DAC_Set_Clock()
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_HSDIN_Software_Sample(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block);

        
/********************************************************************************
 @brief
    Set HighSpeed digital sampling signal.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	signal

	Sampling signal to select.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_HSDIN_Sample_Signal(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
			   SDM35541_hsdin_signal_t signal);


/********************************************************************************
 @brief
    Clear High Speed Digital FIFO

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_HSDIN_Clear(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block);


/********************************************************************************
 @brief
    Read value from High Speed Digital FIFO

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Address of the variable to store the data.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_HSDIN_FIFO_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * data);


/**
 * @} SDM35541_Library_HSDIN_Functions
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 SyncBus Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief
 * @{ SDM35541_Library_SBUS_Functions SDM35541 user library syncbus
 */
/**
*******************************************************************************
 @brief
	Set SyncBus Source

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	sbus

	The specified SyncBus.

 @param
	src

	Source to set for the specified SyncBus.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_SBUS_Set_Source(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_sbus_t sbus,
		       SDM35541_sbus_src_t src);


/********************************************************************************
 @brief
    Enable/Disable Syncbus

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	sbus

	The specified SyncBus.

 @param
	enable

	Value determining whether to enable/disable the syncbus.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 *******************************************************************************/
SDM35541LIB_API
int SDM35541_SBUS_Enable(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_sbus_t sbus, uint16_t enable);


/**
 * @} SDM35541_Library_SBUS_Functions
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 External Trigger/Interrupt Polarity Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief
 * @{ SDM35541_Library_EXT_Functions SDM35541 user library external trigger/interrupt
 */
/**
*******************************************************************************
 @brief
    Set the External Trigger polarity

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	polarity

	Positive/Negative polarity select.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ETRIG_Polarity_Select(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_ext_polarity_t polarity);

/********************************************************************************
 @brief
    Set the External Interrupt polarity

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	polarity

	Positive/Negative polarity select.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_EINT_Polarity_Select(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_ext_polarity_t polarity);


/**
 * @} SDM35541_Library_EXT_Functions
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 About Counter Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief
 * @{ DM75xx_Library_ACNT_Functions DM75xx user library about counter
 */
/********************************************************************************
 @brief
    Get About Counter value

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Address of the variable to store the value.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ACNT_Get_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * data);


/********************************************************************************
 @brief
    Set the About Counter value.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Value at which to set the About Counter.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ACNT_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t data);


/**
 * @} DM75xx_Library_ACNT_Functions
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Delay Counter Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief
 * @{ DM75xx_Library_DCNT_Functions DM75xx user library delay counter
 */
/********************************************************************************
 @brief
    Get the Delay Counter value.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Address of the variable to store the value.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DCNT_Get_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t * data);


/********************************************************************************
 @brief
    Set the Delay Counter value.

 @param
    handle

	Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Value at which to set the Delay Counter.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

	Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DCNT_Set_Count(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint16_t data);
/**
 * @} DM75xx_Library_DCNT_Functions
 */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Digital Input/Output
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief
 * @{ DM75xx_Library_DIO_Functions DM75xx user library digital input/output
 */
/********************************************************************************
 @brief
    Set a specified Digital I/O Port to the given value.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	port

	The specified Digital I/O Port.

 @param
	data

	The value to set on the Digital I/O Port.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Set_Port(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dio_port_t port,
		    uint8_t data);


/********************************************************************************
 @brief
    Get the value from the specified Digital I/O Port.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	port

	The specified Digital I/O Port.

 @param
	data

	The address of the variable to store the Digital I/O Port's value.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Port(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dio_port_t port,
		    uint8_t * data);


/********************************************************************************
 @brief
    Get the Digital I/O Status byte.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	data

	Address of the variable to store the status byte.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * data);


/********************************************************************************
 @brief
    Clear Digital I/O IRQ Status

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Clear_IRQ(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block);


/********************************************************************************
 @brief
    Clear Digital I/O Chip.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Reset(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Set the direction of the specified Digital I/O Port.

@param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

@param
	port

	The specified Digital I/O Port.

@param
	direction

	The direction to set for the specified Digital I/O Port.

@retval
    0

    Success.

@retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

@note
	Port 0 is bit directional and Port 1 is byte directional.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Set_Direction(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dio_port_t port, uint8_t direction);

/**
*******************************************************************************
@brief
    Get the direction of the specified Digital I/O Port.

@param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

@param
	port

	The specified Digital I/O Port.

@param
	direction

	Address of the variable to store the DIO port's direction.

@retval
    0

    Success.

@retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

@note
	Port 0 is bit directional and Port 1 is byte directional.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Direction(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dio_port_t port, uint8_t *direction);


/********************************************************************************
 @brief
    Set Digital I/O Port 0 Mask.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	mask

	The mask to set for Digital I/O Port 0.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
    A mask value can only be set for Digital I/O Port 0.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Set_Mask(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t mask);


/********************************************************************************
 @brief
    Get Digital I/O Port 0 Mask.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	mask

	The mask to set for Digital I/O Port 0.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
    A mask value can only be read for Digital I/O Port 0.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Mask(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t *mask);


/********************************************************************************
 @brief
    Set the compare register for Digital I/O Port 0

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	compare

	The value to compare for Match Mode.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
    A compare value can only be set for Digital I/O Port 0.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Set_Compare(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t compare);


/********************************************************************************
 @brief
    Get the compare register for Digital I/O Port 0

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	compare

	Address of the variable to store the value in the compare register.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 @note
	This register is used as a latch when in event mode.  The value that caused
	the event will be latched to this register and can subsequently be read.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Get_Compare(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * compare);


/********************************************************************************
 @brief
    Set the IRQ Mode for Digital I/O.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	mode

	Set event or match mode IRQ.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_IRQ_Mode(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_dio_mode_t mode);


/********************************************************************************
 @brief
    Set the Digital I/O Sample Clock.

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	clock

	The clock

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Clock(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, SDM35541_dio_clk_t clock);


/********************************************************************************
 @brief
    Enable/Disable Digital I/O Interrupts

 @param
    handle

    Address of device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
	enable

	0 for Disable anything else for Enable

 @retval
    0

    Success.

 @retval
    -1

    Failure.

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DIO_Enable_IRQ(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t enable);


/**
 * @} DM75xx_Library_DIO_Functions
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 User I/O Functions.
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
 * @brief
 * @{ DM75xx_Library_UIO_Functions DM75xx user library user I/O
 */
/*********************************************************************************
 @brief
    Selects the source of a user I/O signal

 @param
    handle

    Address of device's library board descriptor

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    channel

    The user output channel on which the signal will be sent

 @param
    source

    The source for the signal

 @retval
    0

    Success

 @retval
    -1

    Failure

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UIO_Select(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_uio_channel_t channel, SDM35541_uio_source_t source);


/*********************************************************************************
 @brief
    Read the current status of the user I/O

 @param
    handle

    Address of device's library board descriptor

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    data

    Address of the variable to store the read value.

 @retval
    0

    Success

 @retval
    -1

    Failure

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UIO_Read(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint32_t * data);


/********************************************************************************
 @brief
    Write the value of the user I/O

 @param
    handle

    Address of device's library board descriptor

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    data

    Value to write to the user I/O

 @retval
    0

    Success

 @retval
    -1

    Failure

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_UIO_Write(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint32_t data);


/**
 * @} DM75xx_Library_UIO_Functions
 */
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 McBSP Functions
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/**
* @brief
* @{ DM75xx_Library_McBSP_Functions DM75xx user library mcbsp
*/
/********************************************************************************
 @brief
    Enable/Disable A/D FIFO to DSP

 @param
    handle

    Address of device's library board descriptor

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    enable

    0x00 disables, 0xFF enables

 @retval
    0

    Success

 @retval
    -1

    Failure

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_McBSP_ADC_FIFO(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t enable);

/**
 *******************************************************************************
 @brief
    Enable/Disable D/A FIFO to DSP

 @param
    handle

    Address of device's library board descriptor

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    enable

    0x00 disables, 0xFF enables

 @retval
    0

    Success

 @retval
    -1

    Failure

    Please see the ioctl(2) man page for information on possible values errno
    may have in this case.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_McBSP_DAC_FIFO(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t enable);

/**
 * @} DM75xx_Library_McBSP_Functions
 */


/********************************************************************************
 @brief
    Read an address from SRAM.

 @param
    handle

    Address of the device's library board descriptor.

 @param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

 @param
    address

    Offset in SRAM. There are 4 32-bit addresses so only the 2 LSBs of
    this value is used.

 @param
    data

    Address of the variable to store the data.


 @retval
    0

    Success.

 @retval
    -1

    Failure.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_SRAM_Read(struct SDM35541_Board_Descriptor *handle, 
			const struct SDM35541_Function_Block *func_block,
			uint16_t address, uint32_t *data);


/********************************************************************************
 @brief
    Write to an SRAM address.

 @param
    handle

    Address of the device's library board descriptor.

 @param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

 @param
    address

    Offset in SRAM. There are 4 32-bit addresses so only the 2 LSBs of
    this value is used.

 @param
    data

   Word to write to SRAM.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_SRAM_Write(struct SDM35541_Board_Descriptor *handle, 
			const struct SDM35541_Function_Block *func_block,
			uint16_t address, uint32_t data);

/********************************************************************************
 @brief
    Calibrate the DSP on SDM35541

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    dac1_value

    Value to set on DAC1 after calibration.

 @param
    dac2_value

    Value to set on DAC2 after calibration.

 @param
    dac1_range

    The voltage range by which to calibrate dac1.

 @param
    dac2_range

    The voltage range by which to calibrate dac2.

 @retval
    0

    Success.

 @retval
    -1

    Failure.

 ********************************************************************************/
SDM35541LIB_API
int SDM35541_Calibrate(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint16_t dac1_value,
            uint16_t dac2_value,
            SDM35541_dac_range_t dac1_range,
            SDM35541_dac_range_t dac2_range);


/********************************************************************************
 @brief
    Issue a command to the 7540 onboard DSP.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    command

    The DSP Command to issue.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DSP_CMD_Send(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
		    SDM35541_dsp_command_t command);


/********************************************************************************
 @brief
    Gets the version of the bootloader on the DSP.

 @param
    handle

    Address of the device's library board descriptor.

 @param
    major_minor

    Address of the variable to store the version number.
    The result will be the major number stored in the upper
    16-bits and the minor number in the lower 16-bits.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DSP_Get_Version(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint32_t *major_minor);


/********************************************************************************
 @brief
    Checks if the last command given to the DSP is finished.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    data

    This value will be zero if the DSP has completed the instruction and greater
    than zero if an instruction is still being executed.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DSP_CMD_Complete(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * data);

/********************************************************************************
 @brief
    Checks whether or not a command successfully completed on the DSP.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    command

    The DSP command status that is being checked.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_DSP_CMD_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_dsp_command_t command);

/********************************************************************************
 @brief
    Get the the mask of the Analog DIO.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    pin1

    Pin1 mask enabled/disabled.

 @param
    pin2

    Pin2 mask enabled/disabled.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Get_Mask(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_algdio_mask_t * pin1, SDM35541_algdio_mask_t * pin2);


/********************************************************************************
 @brief
    Set the Analog DIO Mask.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    pin1

    Pin1 mask enabled/disabled.

 @param
    pin2

    Pin2 mask enabled/disabled.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Set_Mask(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_algdio_mask_t pin1, SDM35541_algdio_mask_t pin2);


/********************************************************************************
 @brief
    Get the Analog DIO Direction.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    pin1

    Pin1 Direction 0 = Input, Positive vals = Output.

 @param
    pin2

    Pin2 Direction 0 = Input, Positive vals = Output.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Get_Direction(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_algdio_direction_t * pin1,
            SDM35541_algdio_direction_t * pin2);


/********************************************************************************
 @brief
    Set the Analog DIO Direction.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    pin1

    Pin1 Direction 0 = Input, Positive vals = Output.

 @param
    pin2

    Pin2 Direction 0 = Input, Positive vals = Output.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Set_Direction(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            SDM35541_algdio_direction_t pin1,
            SDM35541_algdio_direction_t pin2);


/********************************************************************************
 @brief
    Set the Analog DIO pin values.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    pin1

    Value at logic high (0xFF) or logic low (0x00).

 @param
    pin2

    Value at logic high (0xFF) or logic low (0x00).

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Set_Data(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t pin1, uint8_t pin2);


/********************************************************************************
 @brief
    Get the Analog DIO pin values.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    pin1

    Value at logic high (0xFF) or logic low (0x00).

 @param
    pin2

    Value at logic high (0xFF) or logic low (0x00).

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Get_Data(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block,
            uint8_t * pin1, uint8_t * pin2);


/******************************************************************************** 
 @brief
    Get Analog DIO IRQ Status.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    status

    The IRQ status.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Get_IRQ_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * status);


/******************************************************************************** 
 @brief
    Get Analog DIO IRQ Status.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    status

    The IRQ status.

 @retval
    0

    Success.

 @retval
    -1

    Failure.
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_ALGDIO_Set_IRQ_Status(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t status);


/********************************************************************************
 @brief
    Get the temperature from the board.

 @param
    handle

    Address of the device's library board descriptor.

 @param
      func_block
 
      Pointer to the adc_1002 function block

 @param
    temp

    The temperature returned from the board.

 @retval
    0

    Success

 @retval
    -1

    Failure
 ********************************************************************************/
SDM35541LIB_API
int SDM35541_Get_Temp(struct SDM35541_Board_Descriptor * handle,
            const struct SDM35541_Function_Block *func_block, uint8_t * temp);


#ifdef __cplusplus
}
#endif
#endif /* __SDM35541_registers_h__ */
