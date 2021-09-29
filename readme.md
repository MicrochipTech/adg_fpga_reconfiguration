![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)

# FPGA reconfiguration with a microcontroller example

This repository contains the MPLAB® Harmony 3 example for FPGA
reconfiguration using a microcontroller.

- [Contents Summary](#contents-summary)
- [FPGA Reconfiguration Example Summary](#fpga-reconfiguration-example-summary)
- [Building The Application](#building-the-application)
- [MPLAB Harmony Configurations](#mplab-harmony-configurations)
- [DirectC implementation for SAMRH71F20-EK board](#directc-implementation-for-samrh71f20-ek-board)
- [Hardware Setup](#hardware-setup)
- [Running The Application](#running-the-application)

---

## Contents Summary

| Folder               | Description                                               |
| ---                  | ---                                                       |
| fpga_reconfiguration | FPGA reconfiguration Example application                  |

---

## FPGA Reconfiguration Example Summary

This example shows how to reprogram a FPGA using the DirectC library running on a Microchip microcontroller. When a button is pressed, the FPGA bitstream (DAT format) is first copied from the SDCARD to the external SDRAM memory then sent to the FPGA via JTAG connexion. The DAT files stored on the SDCARD are associated to a given button based on a specific naming convention.

---

## Building The Application 
To build the application, open the project file in MPLABX IDE.

**Application Path:**   
* **For SAMRH71F20-EK board** :fpga_reconfiguration\firmware\fpga_reconfiguration_sam_rh71_ek.X

The application can be built to reconfigure either RTG4 or PolarFire FPGA. The targeted device should be selected by defining the corresponding define in the file ```fpga_reconfiguration\firmware\src\DirectC\dpuser.h```:
* Define ```ENABLE_RTG4_SUPPORT``` for RTG4 device.
* Define ```ENABLE_G5_SUPPORT``` for PolarFire device. 

---

## MPLAB Harmony Configurations 

Refer to the MHC project graph for the components used and the respective configuration options.

* PIO configuration
    * External memory controller (HEMC) pins are configured with custom drive strength.
    * The peripheral pin use is configured for pins used by FLEXCOM1 and FLEXCOM4.
    * Pin PA1 is configure as input.
    * Pins PA0 and PA2 to PA5 are configured as outputs.
* FLEXCOM Peripheral:
    * FLEXCOM1
        * This peripheral is used in UART configuration for standard print and application logs.
    * FLEXCOM4
        * This peripheral is used in SPI configuration to communicate with an SDCARD.
* TC0 Peripheral:
    * Timer configured to provide Time System Service.
* HEMC Peripheral:
    * Peripheral configured to initialize and map the external SDRAM memory of the SAMRH71-EK board.
* SDCARD (SPI) Driver
    * Driver library that provides application ready routines to read and write to the SDCARD.
* FILE SYSTEM System Service
    * Provides an API allowing the user to manage the filesystem. 
* Harmony Core Service
    * Required by the SDCARD (SPI) Driver
* TIME System Service
    * Required by the SDCARD (SPI) Driver

---

## DirectC implementation for SAMRH71F20-EK board.

These user defined sections are implemented for SAMRH71 in the DirectC library:
* JTAG interface:
    * Functions implemented : ```dp_jtag_init```, ```dp_jtag_tms```, ```dp_jtag_tms_tdi```, ```dp_jtag_tms_tdi_tdo```.
    * The implementation is done to reduce at a minimum the number of instructions.
* Delay function:
    * Implements the function ```dp_delay``` using the System Timer.
* Avionic mode for RTG4:
    * Implements the function ```dp_exit_avionics_mode``` to exit avionic mode.
    * **Important Note** : The function was implemented but not tested as the avionic mode was not activated in the bitstreams used with this demo.

The DirectC library is build with the following options:
* The macro USE_PAGING is not defined : We set the ```image_buffer``` pointer to the image in the system memory (external SDRAM).
* Only one of the following define should be define : ```ENABLE_RTG4_SUPPORT``` or ```ENABLE_G5_SUPPORT```.

---

## Hardware Setup

1. Project directc_mem_sam_rh71_ek.X
    * Hardware Used
        * SAM RH71 Evaluation Kit
        * RTG4 DEV KIT REV B or PolarFire DEV KIT
        * [I/O1 Xplained Pro Extension Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/ATIO1-XPRO)
        * Jumper wires.
        * JTAG debugger probe for SAMRH71-EK (ICD4, J32, SAM-ICE, ...)
    * Hardware Setup
        * Connect the debugger probe to J33 on the SAMRH71-EK board
        * Connect the J15 USB port of the board to a computer using a mini USB cable.
        * Check the DEV KIT jumpers configuration
            * RTG4 DEV KIT:
                * J32 : Pin 2-3 are for external programming using FlashPro4.
            * PolarFire DEV KIT:
                * J28 : Pin 1-2 are open to select the external JTAG.
        * Connect the followings signals between the SAMRH71-EK board and the DEV KIT board:
            * RTG4 DEV KIT board:
                | SAMRH71F20-EK        | RTG4 DEV KIT REV B  |
                | -------------        |:------------------: |
                | GND                  |   GND   (J22.10)    |
                | PA0 (J24.7)          |   TDI   (J22.9)     | 
                | PA1 (J24.9)          |   TDO   (J22.3)     | 
                | PA2 (J24.16)         |   TMS   (J22.5)     |
                | PA3 (J24.10)         |   TCK   (J22.1)     |
                | PA4 (J24.8)          |   TRST  (J22.8)     | 
                | PA5 (J24.6)          |   DEVRST_N          | 

            * PolarFire DEV KIT board:
                | SAMRH71F20-EK        | PolarFire DEV KIT REV D  |
                | -------------        |:------------------: |
                | GND                  |   GND   (J32.10)    |
                | PA0 (J24.7)          |   TDI   (J32.9)     | 
                | PA1 (J24.9)          |   TDO   (J32.3)     | 
                | PA2 (J24.16)         |   TMS   (J32.5)     |
                | PA3 (J24.10)         |   TCK   (J32.1)     |
                | PA4 (J24.8)          |   TRST  (J32.8)     | 
    
        * Prepare the SDCARD by copying the generated DAT files that will be used by DirectC, each filename should start with the associated button:
            * For PB0 : PB0_[...].dat
            * For PB1 : PB1_[...].dat
        * Connect the I/O1 Xplained Pro Extension Kit on connector J25 of the SAMRH71-EK board.

---

## Running The Application

1. Build and Program the application using the MPLAB X IDE.
2. Press PB0 on the SAMRH71-EK board to start programming the "PB0_[...].dat" file from the SDCARD.
    * The LED on the I/O1 Xplained board will blink during the copy of the file to the SDRAM memory.
    * During Erase and programming phases, the LEDs, LED0 to LED4 will blink: 
        * LED0 blinks between 0 and 25 %
        * LED0 is on and LED1 blinks between 25 and 50 %
        * LED0, LED1 are on and LED2 blinks between 50 and 75 %
        * LED0 through LED2 are on and LED3 blinks between 75 and 100 %
    * Once the programming is done, all 4 LEDs are turn OFF.
        * If the 4 LEDs stay on for several seconds after programing, it means that an error occurred.
