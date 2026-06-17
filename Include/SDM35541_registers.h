/**
    @file

    @brief
        Defines for the SDM35541 Registers (Offsets)

    $Id: SDM35541_registers.h 154807 2026-06-02 15:14:13Z asutton $
*/

//----------------------------------------------------------------------------
//  COPYRIGHT (C) RTD EMBEDDED TECHNOLOGIES, INC.  ALL RIGHTS RESERVED.
//
//  This software package is dual-licensed.  Source code that is compiled for
//  kernel mode execution is licensed under the GNU General Public License
//  version 2.  For a copy of this license, refer to the file
//  LICENSE_GPLv2.TXT (which should be included with this software) or contact
//  the Free Software Foundation.  Source code that is compiled for user mode
//  execution is licensed under the RTD End-User Software License Agreement.
//  For a copy of this license, refer to LICENSE.TXT or contact RTD Embedded
//  Technologies, Inc.  Using this software indicates agreement with the
//  license terms listed above.
//----------------------------------------------------------------------------

#ifndef __SDM35541_REGISTERS_H__
#define __SDM35541_REGISTERS_H__


 /**
  * @defgroup SDM35541_Register_Offsets SDM35541 Register Offsets
  * @{
  */

/******************************************************************
 *  General Board Control (BAR0)
 ******************************************************************/
/**
 * @brief
 *     Offset to General Board Control (BAR0) Format ID register
 */
#define SDM35541_OFFSET_GBC_FORMAT			0x00

/**
 * @brief
 *     Offset to General Board Control (BAR0) Format ID register
 */
#define SDM35541_OFFSET_GBC_REV			0x01

/**
 * @brief
 *     Offset to General Board Control (BAR0) EOI (End of Interrupt) register
 */
#define SDM35541_OFFSET_GBC_END_INTERRUPT		0x02

/**
 * @brief
 *     Offset to General Board Control (BAR0) Board Reset register
 */
#define SDM35541_OFFSET_GBC_BOARD_RESET		0x03

/**
 * @brief
 *     Offset to General Board Control (BAR0) PDP Number register
 */
#define SDM35541_OFFSET_GBC_PDP_NUMBER		0x04

/**
 * @brief
 *     Offset to General Board Control (BAR0) FPGA Build register
 */
#define SDM35541_OFFSET_GBC_FPGA_BUILD		0x08

/**
 * @brief
 *     Offset to General Board Control (BAR0) System Clock register
 */
#define SDM35541_OFFSET_GBC_SYS_CLK_FREQ		0x0c


/**
 * @brief
 *     Offset to General Board Control (BAR0) IRQ Status register.  Each bit
 *     corresponds to a function block.
 */
#define SDM35541_OFFSET_GBC_IRQ_STATUS		0x10

/**
 * @brief
 *     Offset to General Board Control (BAR0) DMA IRQ Status register.  Each bit
 *     corresponds to a function block.
 */
#define SDM35541_OFFSET_GBC_DMA_IRQ_STATUS		0x18

/**
 * @brief
 *     Offset to the beginning of the Function Blocks section of the GBC.
 */
#define SDM35541_OFFSET_GBC_FB_START			0x20

/**
 * @brief
 *     Size of the function block entries in the GBC
 */
#define SDM35541_GBC_FB_BLK_SIZE			0x10

/**
 * @brief
 *     Offset to Function Block ID, from the start of the function block
 *     section.
 */
#define SDM35541_OFFSET_GBC_FB_ID			0x00

/**
 * @brief
 *     Bit mask for TYPE portion of FB ID
 */
#define SDM35541_FB_ID_TYPE_MASK		0x0000FFFF

/**
 * @brief
 *     Bit mask for SUBTYPE portion of FB ID
 */
#define SDM35541_FB_ID_SUBTYPE_MASK		0x00FF0000

/**
 * @brief
 *     Bit mask for TYPE REV portion of FB ID
 */
#define SDM35541_FB_ID_TYPE_REV_MASK		0xFF000000

/**
 * @brief
 *     Offset to the FB Offset in the GBC, from the start of the
 *     FB data block.
 */
#define SDM35541_OFFSET_GBC_FB_OFFSET		0x04

/**
 * @brief
 *     Offset to the FB DMA Offset in the GBC, from the start of the
 *     FB data block.
 */
#define SDM35541_OFFSET_GBC_FB_DMA_OFFSET		0x08


/******************************************************************
 *  DMA Control (BAR2)
 ******************************************************************/
/**
 * @brief
 *     Offset to the DMA Action Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_ACTION		0x00

/**
 * @brief
 *     Offset to the DMA Setup Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_SETUP		0x01

/**
 * @brief
 *     Offset to the DMA Status (Overflow) Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_STAT_OVERFLOW	0x02

/**
 * @brief
 *     Offset to the DMA Status (Underflow) Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_STAT_UNDERFLOW	0x03

/**
 * @brief
 *     Offset to the DMA Current Count Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_CURRENT_COUNT	0x04

/**
 * @brief
 *     Offset to the DMA Current Buffer Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_CURRENT_BUFFER	0x07

/**
 * @brief
 *     Offset to the DMA Write FIFO Count Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_WR_FIFO_CNT	0x08

/**
 * @brief
 *     Offset to the DMA Read FIFO Count Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_RD_FIFO_CNT	0x0A

/**
 * @brief
 *     Offset to the DMA Status (Used) Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_STAT_USED		0x0C

/**
 * @brief
 *     Offset to the DMA Status (Invalid) Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_STAT_INVALID		0x0D

/**
 * @brief
 *     Offset to the DMA Status (Complete) Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_STAT_COMPLETE		0x0E

/**
 * @brief
 *     Offset to the DMA Last Action Register (BAR2)
 */
#define SDM35541_OFFSET_DMA_LAST_ACTION		0x0F

/**
 * @brief
 *     Offset to the start of the buffer control section (BAR2)
 */
#define SDM35541_OFFSET_DMA_BUFF_START		0x10

/**
 * @brief
 *     Offset to the buffer status register, from the start of the buffer
 *     control section (BAR2)
 */
#define SDM35541_OFFSET_DMA_BUFFER_STAT		0x02

/**
 * @brief
 *     Offset to the buffer control register, from the start of the buffer
 *     control section (BAR2)
 */
#define SDM35541_OFFSET_DMA_BUFFER_CTRL		0x03

/**
 * @brief
 *     Offset to the buffer size register, from the start of the buffer
 *     control section (BAR2)
 */
#define SDM35541_OFFSET_DMA_BUFFER_SIZE		0x04

/**
 * @brief
 *     Offset to the buffer address register, from the start of the buffer
 *     control section (BAR2)
 */
#define SDM35541_OFFSET_DMA_BUFFER_ADDRESS	0x08


/******************************************************************
 *  All Function Blocks (BAR2)
 ******************************************************************/
/**
 * @brief
 *     Offset to the DMA Channels count of the function block (BAR2)
 */
#define SDM35541_OFFSET_FB_DMA_CHANNELS	0x06

/**
 * @brief
 *     Offset to the DMA buffers count of the function block (BAR2)
 */
#define SDM35541_OFFSET_FB_DMA_BUFFERS	0x07

/**
 * @brief
 *     Offset to the beginning of the Function Block control section in BAR2.
 */
#define SDM35541_OFFSET_FB_CTRL_START	0x08



#define SDM35541_FUNCTION_BLOCK_ADC1002    0

/**
 * @brief
 * Read Master/Target Only mode and Firmware version.
 */
#define SDM35541_MT_MODE           0x0000

/**
 * @brief
 * Read/Write user inputs.
 */
#define SDM35541_USER_IO           0x0008

/**
 * @brief
 * Start/Stop software clock
 */
#define SDM35541_DAC_CLK_ST        0x000c

/**
 * @brief
 * Read FIFO Status
 */
#define SDM35541_FIFO_STATUS       0x0010

/**
 * @brief
 * Software DAC1 Update
 */
#define SDM35541_DAC1_UPDATE       0x0014

/**
 * @brief
 * Software DAC2 Update
 */
#define SDM35541_DAC2_UPDATE       0x0018

/**
 * @brief
 * Software simultaneous DAC1 and DAC2 Update
 */
#define SDM35541_DAC_UPDATE        0x0024

/**
 * @brief
 * Start/Stop software Pacer
 */
#define SDM35541_PACER             0x0028

/**
 * @brief
 * Read: Read Timer Counter Status
 * Write: Software HSDIN Sample Command
 */
#define SDM35541_TIMER             0x002c

/**
 * @brief
 * Read: Read interrupt status
 * Write: Write IT enable mask
 */
#define SDM35541_INT               0x0030

/**
 * @brief
 * Read: Clear ITs via mask
 * Write: Set IT clear mask
 */
#define SDM35541_CLEAR_IT          0x0034

/**
 * @brief
 * Read: Read IT overrrun
 * Write: Clear IT overrun
 */
#define SDM35541_IT_OVERRUN        0x0038

/**
 * @brief
 * Read/Write Pacer Clock
 */
#define SDM35541_PCLK_CNT          0x0040

/**
 * @brief
 * Read/Write Burst Clock
 */
#define SDM35541_BURST_CNT         0x0044

/**
 * @brief
 * Read/Write ADC sample counter
 */
#define SDM35541_ADC_SAMPLE        0x0048

/**
 * @brief
 * Read/Write DAC1 update counter
 */
#define SDM35541_DAC1_SAMPLE       0x004c

/**
 * @brief
 * Read/Write DAC2 update counter
 */
#define SDM35541_DAC2_SAMPLE       0x0050

/**
 * @brief
 * Read/Write delay counter
 */
#define SDM35541_DELAY_CNT         0x0054

/**
 * @brief
 * Read/Write about counter
 */
#define SDM35541_ABOUT_CNT         0x0058

/**
 * @brief
 * Read/Write DAC clock
 */
#define SDM35541_DAC_CLK           0x005c

/**
 * @brief
 * Read/Write UTC0 value
 */
#define SDM35541_UTC0              0x0060

/**
 * @brief
 * Read/Write UTC1 value
 */
#define SDM35541_UTC1              0x0064

/**
 * @brief
 * Read/Write UTC2 value
 */
#define SDM35541_UTC2              0x0068

/**
 * @brief
 * UTC Control
 */
#define SDM35541_UTC_CTRL          0x006c

/**
 * @brief
 * Read/Program digital input port 0
 */
#define SDM35541_DIO0              0x0070

/**
 * @brief
 * Read/Program digital input port 1
 */
#define SDM35541_DIO1              0x0074

/**
 * @brief
 * Clear digital IRQ status, read/program 
 * port 0 direction, mask, or compare register
 */
#define SDM35541_DIO_CTRL          0x0078

/**
 * @brief
 * Read DIO Status or Program digital control/interrupts
 */
#define SDM35541_DIO_STATUS        0x007c

/**
 * @brief
 * Read: DSP Command register to be written from
 * the Host side and read from the DSP
 * Write: DSP status written to by the DSP and 
 * read from the Host side
 */
#define SDM35541_DSP               0x00a8

/**
 * @brief
 * I2C Bus Address
 */
#define SDM35541_I2C_ADDR          0x00b8

/**
 * @brief
 * I2C Bus Pointer
 */
#define SDM35541_I2C_PTR           0x00bc

/**
 * @brief
 * I2C Bus Data
 */
#define SDM35541_I2C_DATA          0x00c0

/**
 * @brief
 * I2C Bus Go
 */
#define SDM35541_I2C_GO            0x00c4

/**
 * @brief
 * I2C Bus Read
 */
#define SDM35541_I2C_READ          0x00c8

/**
 * @brief
 * Read/Write analog connection DIO mask
 */
#define SDM35541_ALGDIO_MASK       0x00d8

/**
 * @brief
 * Read/Write analog connection DIO data
 */
#define SDM35541_ALGDIO_DATA       0x00dc

/**
 * @brief
 * read/Write analog connection DIO direction
 */
#define SDM35541_ALGDIO_DIR        0x00e0

/**
 * @brief
 * Read analog connection DIO IRQ status
 */
#define SDM35541_ALGDIO_INT        0x00e4

/**
 * @brief
 * Software board reset
 */
#define SDM35541_BOARD_RESET       0x00f8

/**
 * @brief
 * DMA Channel 0 Source
 */
#define SDM35541_DMA0_SRC          0x00fc

/**
 * @brief
 * DMA Channel 1 Source
 */
#define SDM35541_DMA1_SRC          0x0100

/**
 * @brief
 * Select ADC Conversion Signal
 */
#define SDM35541_ADC_CONV          0x0104

/**
 * @brief
 * Select Burst Clock Start Trigger
 */
#define SDM35541_BURST_START       0x0108

/**
 * @brief
 * Select Pacer Clock Start Trigger
 */
#define SDM35541_PACER_START       0x010c

/**
 * @brief
 * Select Pacer Clock Stop Trigger
 */
#define SDM35541_PACER_STOP        0x0110

/**
 * @brief
 * About Counter Stop Enable
 */
#define SDM35541_ACNT_ENABLE       0x0114

/**
 * @brief
 * Pacer Clock Start Trigger Mode
 */
#define SDM35541_PACER_MODE        0x0118

/**
 * @brief
 * Select HighSpeed Digital Sampling Signal
 */
#define SDM35541_HSDIN_START       0x011c

/**
 * @brief
 * Clear HighSpeed Digital FIFO
 */
#define SDM35541_HSDIN_FIFO_CLR    0x0120

/**
 * @brief
 * Clear ADC FIFO
 */
#define SDM35541_ADC_FIFO_CLR      0x0124

/**
 * @brief
 * Write CGT Multi-Channel
 */
#define SDM35541_CGT_WRITE         0x0128

/**
 * @brief
 * Write CGT Latch Single-Channel
 */
#define SDM35541_CGT_LATCH         0x012c

/**
 * @brief
 * Write Digital Table
 */
#define SDM35541_DT_WRITE          0x0130

/**
 * @brief
 * Enable CGT
 */
#define SDM35541_CGT_ENABLE        0x0134

/**
 * @brief
 * Enable Digital Table
 */
#define SDM35541_DT_ENABLE         0x0138

/**
 * @brief
 * Table Pause Enable
 */
#define SDM35541_PAUSE_TABLE       0x013c

/**
 * @brief
 * Clear CGT
 */
#define SDM35541_CGT_CLEAR         0x0144

/**
 * @brief
 * Reset CGT
 */
#define SDM35541_CGT_RESET         0x0140

/**
 * @brief
 * Select ADC Sample Counter Source
 */
#define SDM35541_ADC_SCNT_SRC      0x0170

/**
 * @brief
 * Select Pacer Clock
 */
#define SDM35541_PACER_SELECT      0x0178

/**
 * @brief
 * Select DAC Clock Start
 */
#define SDM35541_DAC_CLK_START     0x01cc

/**
 * @brief
 * Select DAC Clock Mode
 */
#define SDM35541_DAC_CLK_MODE      0x01e0

/**
 * @brief
 * Select DAC Clock Stop
 */
#define SDM35541_DAC_CLK_STOP      0x01d0

/**
 * @brief
 * Select DAC1 Output Range
 */
#define SDM35541_DAC1_RANGE        0x0148

/**
 * @brief
 * Select DAC1 Update Source
 */
#define SDM35541_DAC1_SRC          0x014c

/**
 * @brief
 * Select DAC1 Cycle Mode
 */
#define SDM35541_DAC1_CYCLE        0x0150

/**
 * @brief
 * Reset DAC1 FIFO
 */
#define SDM35541_DAC1_RESET        0x0154

/**
 * @brief
 * Clear DAC1 FIFO
 */
#define SDM35541_DAC1_CLEAR        0x0158

/**
 * @brief
 * Select DAC2 Output Range
 */
#define SDM35541_DAC2_RANGE        0x015c

/**
 * @brief
 * Select DAC2 Update Source
 */
#define SDM35541_DAC2_SRC          0x0160

/**
 * @brief
 * Select DAC2 Cycle Mode
 */
#define SDM35541_DAC2_CYCLE        0x0164

/**
 * @brief
 * Clear DAC2 FIFO
 */
#define SDM35541_DAC2_CLEAR        0x016c

/**
 * @brief
 * Reset DAC2 FIFO
 */
#define SDM35541_DAC2_RESET        0x0168

/**
 * @brief
 * Select SyncBus 0 Source
 */
#define SDM35541_SBUS0_SOURCE      0x017c

/**
 * @brief
 * Syncbus 0 Enable
 */
#define SDM35541_SBUS0_ENABLE      0x0180

/**
 * @brief
 * Select SyncBus 1 Source
 */
#define SDM35541_SBUS1_SOURCE      0x0184

/**
 * @brief
 * SyncBus 1 Enable
 */
#define SDM35541_SBUS1_ENABLE      0x0188

/**
 * @brief
 * Select SyncBus 2 Source
 */
#define SDM35541_SBUS2_SOURCE      0x0190

/**
 * @brief
 * SyncBus 2 Enable
 */
#define SDM35541_SBUS2_ENABLE      0x0194

/**
 * @brief
 * Select External Trigger Polarity
 */
#define SDM35541_ETRG_POLARITY     0x019c

/**
 * @brief
 * Select External Interrupt Polarity
 */
#define SDM35541_EINT_POLARITY     0x01a0

/**
 * @brief
 * Select UTC0 Clock
 */
#define SDM35541_UTC0_CLOCK        0x01a4

/**
 * @brief
 * Select UTC0 Gate
 */
#define SDM35541_UTC0_GATE         0x01a8

/**
 * @brief
 * Select UTC1 Clock
 */
#define SDM35541_UTC1_CLOCK        0x01ac

/**
 * @brief
 * Select UTC1 Gate
 */
#define SDM35541_UTC1_GATE         0x01b0

/**
 * @brief
 * Select UTC2 Clock
 */
#define SDM35541_UTC2_CLOCK        0x01b4

/**
 * @brief
 * Select UTC2 Gate
 */
#define SDM35541_UTC2_GATE         0x01b8

/**
 * @brief
 * Select User Output Signal 0
 */
#define SDM35541_UIO0_SELECT       0x01bc

/**
 * @brief
 * Select User Output Signal 1
 */
#define SDM35541_UIO1_SELECT       0x01c0

/**
 * @brief
 * Reset DMA Channel 0 Request Machine
 */
#define SDM35541_DMA_RSTRQST0      0x01c4

/**
 * @brief
 * Reset DMA Channel 1 Request Machine
 */
#define SDM35541_DMA_RSTRQST1      0x01c8

/**
 * @brief
 * Select DAC Clock Start
 */
#define SDM35541_DAC_CLK_START     0x01cc

/**
 * @brief
 * Select DAC Clock Stop
 */
#define SDM35541_DAC_CLK_STOP      0x01d0

/**
 * @brief
 * Select Pacer Clock Frequency
 */
#define SDM35541_PCLK_FREQ         0x01d4

/**
 * @brief
 * Select Burst Clock Frequency
 */
#define SDM35541_BCLK_FREQ         0x01d8

/**
 * @brief
 * Select DAC Clock Source
 */
#define SDM35541_DAC_CLK_SOURCE    0x01dc

/**
 * @brief
 * Select DAC Clock Mode
 */
#define SDM35541_DAC_CLK_MODE      0x01e0

/**
 * @brief
 * ADC FIFO Data to DSP Enable
 */
#define SDM35541_MCBSP_AD_CTRL     0x01e4

/**
 * @brief
 * DAC FIFO Data from DSP Enable
 */
#define SDM35541_MCBSP_DA_CTRL     0x01e8

/**
 * @brief
 * Select FIFO Addressing Mode
 */
#define SDM35541_FIFO_ADR_MODE     0x01ec

/**
 * @brief
 * Read ADC FIFO 
 */
#define SDM35541_ADC_FIFO          0x01f8

/**
 * @brief
 * Read HighSpeed Digital FIFO
 */
#define SDM35541_HSDIN_FIFO         0x01fa

/**
 * @brief
 * Write DAC1 FIFO
 */
#define SDM35541_DAC1_FIFO         0x01fc

/**
 * @brief
 * Write DAC2 FIFO
 */
#define SDM35541_DAC2_FIFO         0x01fe

/**
 * @brief
 *  FIFO Source for DMA Channel 0
 */
#define SDM35541_DMA_CH0_FIFO      0x0200

/**
 * @brief
 * FIFO Source for DMA Channel 1
 */
#define SDM35541_DMA_CH1_FIFO      0x0201

/**
 * @brief
 * 4x32bit Address space for DSP
 */
#define SDM35541_SRAM              0x0FF8












/**
 * @brief
 *     Offset to the Output Value register, from the start of
 *     the Flash control section.
 */
#define SDM35541_OFFSET_FLASH_CS_LEN			0x00
#define SDM35541_OFFSET_FLASH_STATUS			0x01
#define SDM35541_OFFSET_FLASH_START			0x03
#define SDM35541_OFFSET_FLASH_READ			0x04
#define SDM35541_OFFSET_FLASH_WRITE_LO		0x08
#define SDM35541_OFFSET_FLASH_WRITE_HI		0x0c














/**
 * @} SDM35541_Register_Offsets
 */

#endif
