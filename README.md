
Windows Software (SDM35541)

Supports Windows 11 (64 bit)

> SWP-700010188 rev B
>
> Version: v1.0.1.155292

Copyright (c), RTD Embedded Technologies, Inc. All Rights Reserved

Using this software indicates acceptance of the RTD End-User Software
License Agreement.  See the file LICENSE.TXT.

## Table of Contents

- [Supported Hardware](#supported-hardware)
- [About the Software](#about-the-software)
- [Installing the Driver](#installing-the-driver)
- [Supported Compilers](#supported-compilers)
- [Library Interface](#library-interface)
- [Header Files](#header-files) 
- [Example Programs](#example-programs)
- [Getting Technical Support](#getting-technical-support)

## Supported Hardware

This software supports the following RTD boards:

- [SDM35541](https://www.rtd.com/PC104/DM/analog%20IO/SDM35541.htm)

## About the Software

This software package contains a driver, library and example programs for
the Microsoft Windows operating system. This software provides support for the
SDM35541 dataModule in the following versions of Microsoft Windows:

- Windows 11 (64-bit)

Full source code for the user-mode DLL library and example programs is included 
in this package. The kernel-mode driver source code may be licensed separately 
after the purchase of a SDM35541 board. Please contact RTD for more information.

## Installing the Driver

The driver can be found within the 'Executables' release package under the 
assets of the latest Github release. Once there, navigate to the `Install\` 
directory, right click 'SDM35541.inf' and select 'Install'. Afterwards, you can 
check the installation by finding the board in the Device Manager under 
'RTD dataModules'. (It should be shown without a yellow exclamation mark.)

If updating the driver, simply follow the above steps with the latest
version of the driver.

As well as the driver itself, you also must install the included Microsoft 
Visual C++ Redistributable package. This may also be found in the `Install\`
directory, as `vc_redist.x64.exe`

Please note RTD only provides our Windows drivers precompiled and signed, 
we do not provide their source code.

## Supported Compilers

The driver was built using the Windows Driver Kit (Build 10.0.26100.0), MSVC 
(Build 143) and version 1.15 of the KMDF (Kernel Mode Driver Framework).
The example programs were compiled using Microsoft Visual Studio 2022. You 
are welcome to port the examples to the compiler of your choice.

## Library Interface

The directory `Library\` contains source code related to the user library.

The SDM35541 library is created with a file name of `SDM35541Lib_x64.dll` and is dynamically linked. Please see the files included in this directory for documentation on utilizing the library's API.

SDM35541Lib - API for the SDM35541 Library

## Header Files

The directory `Include\` contains all header files needed by the example programs, library, and user applications.

## Example Programs

The `Examples\` directory contains example programs (with full source) to
demonstrate how to use this software. Pre-compiled versions of the examples are
stored in `Examples\`. There is a more detailed 
description (including setup) of each example below.

NOTE:
If you recompile any of the example programs and are going to run
them on a system other than where they were compiled, make sure to
install the Redistributable Package for your version of Visual Studio
on to the target system. This is to avoid any issues with missing or
incompatible DLLs. The Redistributable Package used to compile the
example programs are included in the `Install\` directory.


### [AboutInterrupt](Examples/AboutInterrupt/AboutInterrupt.cpp)

Demonstrates the use of the About Counter interrupt.
Samples are gathered via the Pacer Clock which is triggered
by User Timer/Counter 2.  The About Counter is loaded to
trigger an interrupt every 100 samples. The program will 
automatically quit after 10 interrupts.

Setup:
No particular setup is required for this example.

Usage:
Run the program and follow the instructions on-screen.

### [Adc](Examples/Adc/Adc.cpp)

This example program demonstrates the use of
a single ADC channel. The ADC FIFO is filled
with samples and then prints a full FIFO of
data to the screen.

Setup:
Connect the signal of interest to AINx.

Usage:
Run the program and follow the instructions on-screen.

### [AdcDma](Examples/AdcDma/AdcDma.cpp)

This example program demonstrates the use of
the ADC and DMA. The example will collect
data from the ADC via DMA, and then write the
data out to a file on disk. The data can then
be plotted using the gnuplot script plot.plt

Setup:
Connect the signal of interest to AINx.

Usage:
Run the program and follow the instructions on-screen.

### [AdcDacSimul](Examples/AdcDacSimul/AdcDacSimul.cpp)

This example program demonstrates the use of
the ADC and DAC at the same time using DMA.
The example will collect data from the ADC
via DMA, and then write the data out to a
file on disk. The data can then be plotted
using the gnuplot script plot.plt

Setup:
Connect the signal of interest to AINx and a
scope to AOUTx. Alternatively, connect AOUTx
to AINx to loop the DAC to the ADC.

Usage:
Run the program and follow the instructions on-screen.

### [AdcHsdinSimul](Examples/AdcHsdinSimul/AdcHsdinSimul.cpp)

This example program demonstrates the use
of the ADC and the High Speed Digital Input
(HSDIN) at the same time using DMA. The
example will collect data from the ADC
and HSDIN via DMA, and then print the
data to the screen.

Setup:
Connect the signal of interest to AIN1, connect
digital signals to the High Speed Inputs.

Usage:
Run the program and follow the instructions on-screen.

### [AdcBurst](Examples/AdcBurst/AdcBurst.cpp)

This example program demonstrates the use of
the Pacer Clock and Burst Clock. This example
will collect ADC samples nearly simultaneously
using the Burst Clock whenever the Pacer Clock
triggers it. Samples are acquired until the FIFO
is filled then they are printed to the screen.

Setup:
Connect multiple signals to AIN channels.

Usage:
Run the program and follow the instructions on-screen.

### [AdcExternalTrigger](Examples/AdcExternalTrigger/AdcExternalTrigger.cpp)

This example program uses the external trigger
to toggle the Pacer Clock. While the External
Trigger is high the Pacer Clock will run and
while it is low the Pacer Clock will stop.
The status of the Pacer Clock will be printed to
the screen as External Trigger Edge interrupts
are received by the user-space ISR.

Setup:
Connect the signal of interest to AIN1.

Usage:
Run the program and follow the instructions on-screen.

### [AnalogDio](Examples/AnalogDio/AnalogDio.cpp)

This example program demonstrates interrupts on the two
DIO pins on the analog connector (CN9). This program sets pin 1
as an output and pin 2 is set as an input with rising edge interrupt
enabled. Pin 1 is toggled until 10 interrupts are received.

Setup:
Connect Digital I/O 0 to Digital I/O 1 (CN9, Pin 1
to Pin 2).

Usage:
Run the program and follow the instructions on-screen.


### [Calibrate](Examples/Calibrate/Calibrate.cpp)

Calibrates the DAC at a specified range using
the onboard DSP and sets the DAC to a provided
value after calibration.

Setup:
No particular setup is required for this example.

Usage:
Run the program and follow the instructions on-screen.

### [CgtResetInterrupt](Examples/CgtResetInterrupt/CgtResetInterrupt.cpp)

This example program demonstrates the reset
interrupt on the channel gain table. The
channel gain table is loaded with 16 entries to
read samples from each ADC channel. After all
16 channels are sampled, the CGT reset interrupt
fires and each channel is sampled again. This
repeats until 64 reset interrupts occur.

Setup:
No particular setup is required for this example.

Usage:
Run the program and follow the instructions on-screen.

### [Dac](Examples/Dac/Dac.cpp)

This example program demonstrates the use of the DAC.
A sawtooth wave will be displayed on whichever
channel the user specifies.

Setup:
Connect an oscilloscope to an AOUT channel.

Usage:
Run the program and follow the instructions on-screen.

### [DacDma](Examples/DacDma/DacDma.cpp)

This example program demonstrates the use of the
DAC and DMA. A sine wave is generated in the program,
written to a DMA buffer, and then sent to the DAC in
a repeating loop, thus providing a continuous cycling signal.

Setup:
Connect an oscilloscope to an AOUT channel.

Usage:
Run the program and follow the instructions on-screen.

### [DelayInterrupt](Examples/DelayInterrupt/DelayInterrupt.cpp)

This example program demonstrates the use of the delay
counter. The delay counter will wait until 10 samples
have been read, then the about counter will sample
10 times. An interrupt will occur each time the delay
counter hits 0 delay counter.

Setup:
No particular setup is required for this example.

Usage:
Run the program and follow the instructions on-screen.

### [DioEvent](Examples/DioEvent/DioEvent.cpp)

This example program demonstrates the use of the DIO event
interrupt mode. This mode triggers an interrupt anytime
the value of PORT 0 changes. Ten interrupts will occur
before the program exits.

Setup:
Connect DIO port 0 to DIO port 1.

Usage:
Run the program and follow the instructions on-screen.

### [DioMatch](Examples/DioMatch/DioMatch.cpp)

This example program demonstrates the use of the DIO match
interrupt mode. This mode triggers an interrupt anytime
the value of PORT 1 matches the compare register.
Two interrupts will occur before the program exits.

Setup:
Connect DIO port 0 to DIO port 1.

Usage:
Run the program and follow the instructions on-screen.

### [DspVersion](Examples/DspVersion/DspVersion.cpp)

This example program reports the version of the DSP firmware.

Setup:
No particular setup is required for this example.

Usage:
Run the program and follow the instructions on-screen.

### [ExternalTriggerInterrupt](Examples/ExternalTriggerInterrupt/ExternalTriggerInterrupt.cpp)

This example program demonstrates the use of UTC1 to
generate interrupts on the External Trigger Pin.

Setup:
Connect pin 21 to pin 35 on CN6.

Usage:
Run the program and follow the instructions on-screen.

### [Hsdin](Examples/Hsdin/Hsdin.cpp)

This example program demonstrates the use of the High
Speed Digital Input using UTC0 as the sampling trigger.
After data is collected, it is printed out to the screen.

Setup:
Connect digital signal(s) of interest to the High Speed Inputs.

Usage:
Run the program and follow the instructions on-screen.

### [HsdinDma](Examples/HsdinDma/HsdinDma.cpp)

This example program demonstrates the use of the High
Speed Digital Input. It configures the HSDIN port
to use the UTC1 as the demand mode source. After
data is collected, it is printed out to the screen.

Setup:
Connect digital signal(s) of interest to the High Speed Inputs.

Usage:
Run the program and follow the instructions on-screen.

### [ListFunctionBlocks](Examples/ListFunctionBlocks/ListFunctionBlocks.cpp)

This example program demonstrates accessing the board-level 
registers to access the function blocks on the board.  The 
program will query every function block location to see if a 
valid function block type exists there, and if it does, it will
display that type on the screen.  In this way, it will give an
inventory of the function blocks on the board.

Setup:
No particular setup is required for this example.

Usage:
Run the program and follow the instructions on-screen.

### [Temperature](Examples/Temperature/Temperature.cpp)

This example program demonstrates the temperature sensor
on the board. It will continually display the temperature
on the screen until stopped.

Setup:
No particular setup is required for this example.

Usage:
Run the program and follow the instructions on-screen.

### [Timer](Examples/Timer/Timer.cpp)

This example program demonstrates the use of the User
Timer Counter. The program will continue counting until
the user quits.

Setup:
No particular setup is required for this example.

Usage:
Run the program and follow the instructions on-screen.

### [TimerInterrupt](Examples/TimerInterrupt/TimerInterrupt.cpp)

This example program demonstrates the use of the User Timer
Counter (UTC) interrupts. An interrupt will occur on UTC1
out and UTC1 inverted out. The program will continue
counting until each UTC interrupt fires 10 times.

Setup:
No particular setup is required for this example.

Usage:
Run the program and follow the instructions on-screen.

## Supported Compilers

The driver was built using the Windows Driver Kit (Build 10.0.26100.0) and
version 1.15 of the KMDF (Kernel Mode Driver Framework). The example programs were
compiled using Microsoft Visual Studio 2022. You are welcome to
port the examples to the compiler of your choice.

## Known Limitations

DMA performance is highly dependent on the system being used. There is
also some timing and resource variability inherent the Windows operating
system that will affect performance. Depending on the system resources 
available and CPU loading presented by other applications and services
running at the same time, it can be possible for the board and kernel 
driver to overwhelm the user application, leading to a loss of data. 

## Getting Technical Support

If you require additional support with this product, or any other products
from RTD Embedded Technologies, contact us using the information below:

RTD Embedded Technologies, Inc. \
103 Innovation Blvd. \
State College, PA 16803 USA

Telephone: (814) 234-8087 \
Fax: (814) 234-5218 \
Sales Information and Quotes: sales@rtd.com \
Technical Assistance: techsupport@rtd.com \
Web Site: https://www.rtd.com
