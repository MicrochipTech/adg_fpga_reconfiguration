/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    plib_pio.c

  Summary:
    PIO function implementations for the PIO PLIB.

  Description:
    The PIO PLIB provides a simple interface to manage peripheral
    input-output controller.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

#include "plib_pio.h"
#include "interrupts.h"

/******************************************************************************
  Function:
    PIO_Initialize ( void )

  Summary:
    Initialize the PIO library.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_Initialize ( void )
{
 /* Port A Peripheral function GPIO configuration */
	PIOA_REGS->PIO_MSKR = 0x103fU;
	PIOA_REGS->PIO_CFGR = 0x0U;
	
 /* Port A Pin 0 configuration */
	PIOA_REGS->PIO_MSKR = 0x1U;
	PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port A Pin 2 configuration */
	PIOA_REGS->PIO_MSKR = 0x4U;
	PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port A Pin 3 configuration */
	PIOA_REGS->PIO_MSKR = 0x8U;
	PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port A Pin 4 configuration */
	PIOA_REGS->PIO_MSKR = 0x10U;
	PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port A Pin 5 configuration */
	PIOA_REGS->PIO_MSKR = 0x20U;
	PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port A Pin 12 configuration */
	PIOA_REGS->PIO_MSKR = 0x1000U;
	PIOA_REGS->PIO_CFGR = (PIOA_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port A Latch configuration */
	PIOA_REGS->PIO_SODR = 0x20U;
	PIOA_REGS->PIO_CODR = 0x103fU & ~0x20U;

 /* Port B Peripheral function GPIO configuration */
	PIOB_REGS->PIO_MSKR = 0x880000U;
	PIOB_REGS->PIO_CFGR = 0x0U;
	
 /* Port B Pin 19 configuration */
	PIOB_REGS->PIO_MSKR = 0x80000U;
	PIOB_REGS->PIO_CFGR = (PIOB_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port B Pin 23 configuration */
	PIOB_REGS->PIO_MSKR = 0x800000U;
	PIOB_REGS->PIO_CFGR = (PIOB_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port B Latch configuration */
	PIOB_REGS->PIO_CODR = 0x880000U;

 /* Port C Peripheral function A configuration */
	PIOC_REGS->PIO_MSKR = 0x600007LU;
	PIOC_REGS->PIO_CFGR = 0x1U;
	
 /* Port C Peripheral function GPIO configuration */
	PIOC_REGS->PIO_MSKR = 0xe0000008LU;
	PIOC_REGS->PIO_CFGR = 0x0U;
	
 /* Port C Pin 3 configuration */
	PIOC_REGS->PIO_MSKR = 0x8U;
	PIOC_REGS->PIO_CFGR = (PIOC_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port C Latch configuration */
	PIOC_REGS->PIO_SODR = 0x8U;
	PIOC_REGS->PIO_CODR = 0xe0000008LU & ~0x8U;

 /* Port D Peripheral function A configuration */
	PIOD_REGS->PIO_MSKR = 0xffffffffLU;
	PIOD_REGS->PIO_CFGR = 0x1U;
	
 /* Port D Latch configuration */
	PIOD_REGS->PIO_CODR = 0x0LU;

 /* Port E Peripheral function A configuration */
	PIOE_REGS->PIO_MSKR = 0xffU;
	PIOE_REGS->PIO_CFGR = 0x1U;
	

 /* Port F Peripheral function A configuration */
	PIOF_REGS->PIO_MSKR = 0x600087ffU;
	PIOF_REGS->PIO_CFGR = 0x1U;
	
 /* Port F Peripheral function GPIO configuration */
	PIOF_REGS->PIO_MSKR = 0x180000U;
	PIOF_REGS->PIO_CFGR = 0x0U;
	
 /* Port F Pin 2 configuration */
	PIOF_REGS->PIO_MSKR = 0x4U;
	PIOF_REGS->PIO_CFGR = (PIOF_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x20000U;
	
 /* Port F Pin 19 configuration */
	PIOF_REGS->PIO_MSKR = 0x80000U;
	PIOF_REGS->PIO_CFGR = (PIOF_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port F Pin 20 configuration */
	PIOF_REGS->PIO_MSKR = 0x100000U;
	PIOF_REGS->PIO_CFGR = (PIOF_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x100U;
	
 /* Port F Latch configuration */
	PIOF_REGS->PIO_CODR = 0x180000U;

 /* Port G Peripheral function A configuration */
	PIOG_REGS->PIO_MSKR = 0xffffffU;
	PIOG_REGS->PIO_CFGR = 0x1U;
	
 /* Port G Pin 0 configuration */
	PIOG_REGS->PIO_MSKR = 0x1U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 1 configuration */
	PIOG_REGS->PIO_MSKR = 0x2U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 2 configuration */
	PIOG_REGS->PIO_MSKR = 0x4U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 3 configuration */
	PIOG_REGS->PIO_MSKR = 0x8U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 4 configuration */
	PIOG_REGS->PIO_MSKR = 0x10U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 5 configuration */
	PIOG_REGS->PIO_MSKR = 0x20U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 6 configuration */
	PIOG_REGS->PIO_MSKR = 0x40U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 7 configuration */
	PIOG_REGS->PIO_MSKR = 0x80U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 8 configuration */
	PIOG_REGS->PIO_MSKR = 0x100U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 9 configuration */
	PIOG_REGS->PIO_MSKR = 0x200U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 10 configuration */
	PIOG_REGS->PIO_MSKR = 0x400U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 11 configuration */
	PIOG_REGS->PIO_MSKR = 0x800U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 12 configuration */
	PIOG_REGS->PIO_MSKR = 0x1000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 13 configuration */
	PIOG_REGS->PIO_MSKR = 0x2000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 14 configuration */
	PIOG_REGS->PIO_MSKR = 0x4000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 15 configuration */
	PIOG_REGS->PIO_MSKR = 0x8000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 16 configuration */
	PIOG_REGS->PIO_MSKR = 0x10000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 17 configuration */
	PIOG_REGS->PIO_MSKR = 0x20000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 18 configuration */
	PIOG_REGS->PIO_MSKR = 0x40000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 19 configuration */
	PIOG_REGS->PIO_MSKR = 0x80000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 20 configuration */
	PIOG_REGS->PIO_MSKR = 0x100000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 21 configuration */
	PIOG_REGS->PIO_MSKR = 0x200000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 22 configuration */
	PIOG_REGS->PIO_MSKR = 0x400000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	
 /* Port G Pin 23 configuration */
	PIOG_REGS->PIO_MSKR = 0x800000U;
	PIOG_REGS->PIO_CFGR = (PIOG_REGS->PIO_CFGR & (PIO_CFGR_FUNC_Msk)) | 0x10000U;
	


}

// *****************************************************************************
// *****************************************************************************
// Section: PIO APIs which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t PIO_PortRead ( PIO_PORT port )

  Summary:
    Read all the I/O lines of the selected port.

  Description:
    This function reads the live data values on all the I/O lines of the
    selected port.  Bit values returned in each position indicate corresponding
    pin levels.
    1 = Pin is high.
    0 = Pin is low.

    This function reads the value regardless of pin configuration, whether it is
    set as as an input, driven by the PIO Controller, or driven by a peripheral.

  Remarks:
    If the port has less than 32-bits, unimplemented pins will read as
    low (0).
    Implemented pins are Right aligned in the 32-bit return value.
*/
uint32_t PIO_PortRead(PIO_PORT port)
{
    return PIO_REGS->PIO_GROUP[port].PIO_PDSR;
}

// *****************************************************************************
/* Function:
    void PIO_PortWrite (PIO_PORT port, uint32_t mask, uint32_t value);

  Summary:
    Write the value on the masked I/O lines of the selected port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortWrite(PIO_PORT port, uint32_t mask, uint32_t value)
{
    PIO_REGS->PIO_GROUP[port].PIO_MSKR = mask;
    PIO_REGS->PIO_GROUP[port].PIO_ODSR = value;
}

// *****************************************************************************
/* Function:
    uint32_t PIO_PortLatchRead ( PIO_PORT port )

  Summary:
    Read the latched value on all the I/O lines of the selected port.

  Remarks:
    See plib_pio.h for more details.
*/
uint32_t PIO_PortLatchRead(PIO_PORT port)
{
    return PIO_REGS->PIO_GROUP[port].PIO_ODSR;
}

// *****************************************************************************
/* Function:
    void PIO_PortSet ( PIO_PORT port, uint32_t mask )

  Summary:
    Set the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortSet(PIO_PORT port, uint32_t mask)
{
    PIO_REGS->PIO_GROUP[port].PIO_SODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortClear ( PIO_PORT port, uint32_t mask )

  Summary:
    Clear the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortClear(PIO_PORT port, uint32_t mask)
{
    PIO_REGS->PIO_GROUP[port].PIO_CODR = mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortToggle ( PIO_PORT port, uint32_t mask )

  Summary:
    Toggles the selected IO pins of a port.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortToggle(PIO_PORT port, uint32_t mask)
{
    /* Write into Clr and Set registers */
    PIO_REGS->PIO_GROUP[port].PIO_MSKR = mask;
    PIO_REGS->PIO_GROUP[port].PIO_ODSR ^= mask;
}

// *****************************************************************************
/* Function:
    void PIO_PortInputEnable ( PIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as input.

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortInputEnable(PIO_PORT port, uint32_t mask)
{
    PIO_REGS->PIO_GROUP[port].PIO_MSKR = mask;
    PIO_REGS->PIO_GROUP[port].PIO_CFGR &= ~PIO_CFGR_DIR_Msk;	
}

// *****************************************************************************
/* Function:
    void PIO_PortOutputEnable ( PIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as output(s).

  Remarks:
    See plib_pio.h for more details.
*/
void PIO_PortOutputEnable(PIO_PORT port, uint32_t mask)
{
    PIO_REGS->PIO_GROUP[port].PIO_MSKR = mask;
    PIO_REGS->PIO_GROUP[port].PIO_CFGR |= PIO_CFGR_DIR_Msk;
}


/*******************************************************************************
 End of File
*/
