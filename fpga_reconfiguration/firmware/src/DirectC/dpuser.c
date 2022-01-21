/* ************ MICROSEMI SOC CORP. DIRECTC LICENSE AGREEMENT ************* */
/* ------------------------------------------------------------------------ 
PLEASE READ: BEFORE INSTALLING THIS SOFTWARE, CAREFULLY READ THE FOLLOWING 
MICROSEMI SOC CORP LICENSE AGREEMENT REGARDING THE USE OF THIS SOFTWARE. 
INSTALLING THIS SOFTWARE INDICATES THAT YOU ACCEPT AND UNDERSTAND THIS AGREEMENT 
AND WILL ABIDE BY IT. 

Note: This license agreement (“License”) only includes the following software: 
DirectC. DirectC is licensed under the following terms and conditions.

Hereinafter, Microsemi SoC Corp. shall be referred to as “Licensor” or “Author,” 
whereas the other party to this License shall be referred to as “Licensee.” Each 
party to this License shall be referred to, singularly, as a “Party,” or, 
collectively, as the “Parties.”

Permission to use, copy, modify, and/or distribute DirectC for any purpose, with
or without fee, is hereby granted by Licensor to Licensee, provided that the 
above Copyright notice and this permission notice appear in all copies, 
modifications and/or distributions of DirectC.

DIRECTC IS PROVIDED "AS IS" AND THE AUTHOR/LICENSOR DISCLAIMS ALL WARRANTIES 
WITH REGARD TO DIRECTC INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS. IN NO EVENT SHALL AUTHOR/LICENSOR BE LIABLE TO LICENSEE FOR ANY DAMAGES, 
INCLUDING SPECIAL, DIRECT,INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
WITH THE USE OR PERFORMANCE OF DIRECTC.

Export Control: Information furnished to Licensee may include United States 
origin technical data. Accordingly, Licensee is responsible for complying with, 
and warrants to Licensor that it will comply with, all U.S. export control laws 
and regulations, including the provisions of the Export Administration Act of 
1979 and the Export Administration Regulations promulgated thereunder, the Arms 
Export Control Act, and the sanctions laws administered by the Office of Foreign 
Assets Control including any other U.S. Government regulation applicable to the 
export, re-export, or disclosure of such controlled technical data (or the 
products thereof) to Foreign Nationals, whether within or without the U.S., 
including those employed by, or otherwise associated with, Licensee. Licensee 
shall obtain Licensor’s written consent prior to submitting any request for 
authority to export any such technical data.

ADR: Any dispute between the Parties arising from or related to this License or 
the subject matter hereof, including its validity, construction or performance 
thereunder, shall be exclusively resolved through arbitration by a mutually 
acceptable impartial and neutral arbitrator appointed by the Judicial 
Arbitration and Mediation Services (JAMS) in accordance with its rules and 
procedures. If the Parties are not able to agree on an arbitrator within 10 days 
of the date of request for mediation is served, then JAMS shall appoint an 
arbitrator. Notice of arbitration shall be served and filed with the JAMS main 
offices in Irvine, California. Each Party shall be responsible for all costs 
associated with the preparation and representation by attorneys, or any other 
persons retained thereby, to assist in connection with any such Arbitration. 
However, all costs charged by the mutually agreed upon Arbitration entity shall 
be equally shared by the Parties. The Party seeking Mediation and/or Arbitration 
as provided herein agrees that the venue for any such Mediation and Arbitration 
shall be selected by the other Party and that such venue must be Los Angeles, 
California; New York, New York; or Chicago, Illinois; whereby the applicable law 
and provisions of the Evidence Code of the State selected thereby shall be 
applicable and shall govern the validity, construction and performance of this 
License.

Governing Law: This license will be governed by the laws of the State of 
California, without regard to its conflict of law provisions.

Entire Agreement: This document constitutes the entire agreement between the 
Parties with respect to the subject matter herein and supersedes all other 
communications whether written or oral.                                     */

/* ************************************************************************ */
/*                                                                          */
/*  JTAG_DirectC    Copyright (C) Microsemi Corporation                     */
/*  Version 2021.2  Release date December 2021                              */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dpuser.c                                                */
/*                                                                          */
/*  Description:    user specific file containing JTAG interface functions  */
/*                  and delay function                                      */
/*                                                                          */
/****************************************************************************/

#include "dpuser.h"
#include "dpalg.h"
#include "dputil.h"
/* 
* User attention:
* Include files needed to support hardware JTAG interface operations.
* 
*/
/* User defined */
#ifdef ENABLE_EMBEDDED_SUPPORT

#endif
/* End of User defined */


/* User defined */
/*------------------------------------------------------------------------------
UART selection.
This UART could be used for both message reporting and data communication
*/
#ifdef ENABLE_DISPLAY
#ifdef ENABLE_EMBEDDED_SUPPORT
#include "definitions.h"                // SYS function prototypes
#endif
#endif

/* End of User defined */

/* This variable is used to select external programming vs IAP programming */
DPUCHAR hardware_interface = GPIO_SEL;
DPUCHAR enable_mss_support = FALSE;

#ifdef ENABLE_EMBEDDED_SUPPORT
/* User defined */
static bool progressLedOn;
static uint8_t progressLedBlink = 0;
/* End of User defined */

/*
* User attention:
* jtag_port_reg: 	8 bit Static variable to keep track of the state of all the JTAG pins 
* 					at all times during the programming operation.
* Note: User check the variable size to make sure it can cover the hardware IO register. 
* 
*/
static DPUCHAR __attribute__((tcm)) jtag_port_reg;
/*
* User attention: 
* Module: jtag_inp
* 		purpose: report the logic state of tdo jtag pin
* Arguments: None
* Return value: 8 bit value
* 		0, 0x80
* 
*/
DPUCHAR jtag_inp(void)
{
    /* User defined */
    /* Function not used - Content optimized in calling function */
    /* End of User defined */
    return 0;
}
/*
* User attention: 
* Module: jtag_outp
* 		purpose: Set the JTAG port (all JTAG pins)
* Arguments: 8 bit value containing the new state of all the JTAG pins
* Return value: None
* 
*/
inline void jtag_outp(DPUCHAR outdata)
{
    /* User defined */
    PIOA_REGS->PIO_MSKR = 0x1D;
    PIOA_REGS->PIO_ODSR = outdata;
    /* End of User defined */
}

/*
* No need to change this function
* Module: dp_jtag_init
* 		purpose: Set tck and trstb pins to logic level one
* Arguments:
* 		None
* Return value: None
* 
*/
void dp_jtag_init(void)
{
    jtag_port_reg = TCK | TRST; 
    jtag_outp(jtag_port_reg);
    return;    
}

/*
* No need to change this function
* Module: dp_jtag_tms
* 		purpose: Set tms pin to a logic level one or zero and pulse tck.
* Arguments: 
* 		tms: 8 bit value containing the new state of tms
* Return value: None
* Constraints: Since jtag_outp function sets all the jtag pins, jtag_port_reg is used 
* 				to modify the required jtag pins and preseve the reset.
* 
*/
void __attribute__((tcm)) dp_jtag_tms(DPUCHAR tms)		 
{	
    jtag_port_reg &= ~(TMS | TCK);
    jtag_port_reg |= (tms ? TMS : 0u);
    PIOA_REGS->PIO_MSKR = 0x1D;
    PIOA_REGS->PIO_ODSR = jtag_port_reg;
    jtag_port_reg |= TCK;
    PIOA_REGS->PIO_ODSR = jtag_port_reg;
    return;
}

/*
* No need to change this function
* Module: dp_jtag_tms_tdi
* 		purpose: Set tms amd tdi pins to a logic level one or zero and pulse tck.
* Arguments: 
* 		tms: 8 bit value containing the new state of tms
* 		tdi: 8 bit value containing the new state of tdi
* Return value: None
* Constraints: Since jtag_outp function sets all the jtag pins, jtag_port_reg is used 
* 				to modify the required jtag pins and preseve the reset.
* 
*/
void __attribute__((tcm)) dp_jtag_tms_tdi(DPUCHAR tms, DPUCHAR tdi)
{
    jtag_port_reg &= ~(TMS | TCK | TDI);
    jtag_port_reg |= ((tms ? TMS : 0u) | (tdi ? TDI : 0u));
    PIOA_REGS->PIO_MSKR = 0x1D;
    PIOA_REGS->PIO_ODSR = jtag_port_reg;
    jtag_port_reg |= TCK;
    PIOA_REGS->PIO_ODSR = jtag_port_reg;
    return;    
}

/*
* No need to change this function
* Module: dp_jtag_tms_tdi_tdo
* 		purpose: Set tms amd tdi pins to a logic level one or zero, 
* 				 pulse tck and return tdi level
* Arguments: 
* 		tms: 8 bit value containing the new state of tms
* 		tdi: 8 bit value containing the new state of tdi
* Return value: 
* 		ret: 8 bit variable ontaining the state of tdo.
* Valid return values: 
* 		0x80: indicating a logic level high on tdo
* 		0: indicating a logic level zero on tdo
* Constraints: Since jtag_outp function sets all the jtag pins, jtag_port_reg is used 
* 				to modify the required jtag pins and preseve the reset.
* 
*/
DPUCHAR __attribute__((tcm)) dp_jtag_tms_tdi_tdo(DPUCHAR tms, DPUCHAR tdi)
{
    uint32_t tdo;
    jtag_port_reg &= ~(TMS | TCK | TDI);
    jtag_port_reg |= ((tms ? TMS : 0u) | (tdi ? TDI : 0u));
    PIOA_REGS->PIO_MSKR = 0x1D;
    PIOA_REGS->PIO_ODSR = jtag_port_reg;
    tdo = PIOA_REGS->PIO_PDSR;
    if ( (tdo&TDO) == 0)
        tdo = 0x00;
    else
        tdo = 0x80;
    jtag_port_reg |= TCK;
    PIOA_REGS->PIO_ODSR = jtag_port_reg;
    return tdo;
}

/*
* User attention: 
* Module: dp_delay
* 		purpose: Execute a time delay for a specified amount of time.
* Arguments: 
* 		microseconeds: 32 bit value containing the amount of wait time in microseconds.
* Return value: None
* 
*/
void dp_delay(DPULONG microseconds)
{
    bool overflow = false;
    uint64_t now = SYS_TIME_Counter64Get();
    uint64_t end = now + SYS_TIME_USToCount(microseconds);
    if (now < end)
        overflow = true;

    do{
       now = SYS_TIME_Counter64Get();
       if ( (overflow == true) && (now < end) )
           overflow = false;
    }
    while ( (overflow == true) && (now < end) );

    return;
}

/*
* User attention: 
* Module: dp_exit_avionics_mode
* 		purpose: Either power cycle RTG4 device or toggle devrst pin.
* Return value: None
* 
*/
void dp_exit_avionics_mode(void)
{
    /* To exit Avionics mode, the JTAG_TRST pin must be held high and DEVRST_N pin must be toggled. */
    jtag_port_reg |= TRST; 
    jtag_outp(jtag_port_reg);
    
    RTG4_DEVRST_N_Clear();
    dp_delay(10);
    RTG4_DEVRST_N_Set();
    dp_delay(10);
    return;
}

#ifdef ENABLE_DISPLAY
void __attribute__((tcm)) dp_report_progress(DPUCHAR value)
{
    /* User defined */
    dp_display_text((DPCHAR*)"\rProgress: ");
    dp_display_value(value,DEC);
    dp_display_text((DPCHAR*)" %");

    if (value < 25)
    {
        progressLedBlink = 0;
        LED1_Off();
        LED2_Off();
        LED3_Off();
    }
    else if (value < 50)
    {
        progressLedBlink = 1;
        LED0_On();
        LED2_Off();
        LED3_Off();
    }
    else if (value < 75)
   {
        progressLedBlink = 2;
        LED0_On();
        LED1_On();
        LED3_Off();
    }
    else
    {
        progressLedBlink = 3;
        LED0_On();
        LED1_On();
        LED2_On();
    }

    if (progressLedOn)
    {
        if (progressLedBlink == 0)
            LED0_Off();
        if (progressLedBlink == 1)
            LED1_Off();
        if (progressLedBlink == 2)
            LED2_Off();
        if (progressLedBlink == 3)
            LED3_Off();
        progressLedOn = false;
    }
    else
    {
        if (progressLedBlink == 0)
            LED0_On();
        if (progressLedBlink == 1)
            LED1_On();
        if (progressLedBlink == 2)
            LED2_On();
        if (progressLedBlink == 3)
            LED3_On();
        progressLedOn = true;
    }                

    /* End of User defined */
    
    return;    
}


void dp_display_text(DPCHAR *text)
{
    /* User defined */
    DPULONG length = 0;
    while (text[length++]!='\0');
    
    /* Send the message */
    FLEXCOM1_USART_Write((unsigned char *)text,(length-1));
    
    
    /* End of User defined */
    return;    
}

void dp_display_value(DPULONG value,DPUINT descriptive)
{
    /* User defined */
    DPUCHAR value_text[256];
    DPULONG length;
    
    if (descriptive == HEX)
    {
        length = int_to_hex_int( value, value_text, 8);
    }
    else 
    {
        length = int_to_dec_int( value, value_text);
    }
    
    /* Send the data */
    FLEXCOM1_USART_Write(value_text, length);
    
    /* End of User defined */
    return;    
}

void dp_display_array(DPUCHAR *outbuf,DPUINT bytes, DPUINT descriptive)
{
    /* User defined */
    int idx;
    for (idx=0;idx<bytes;idx++)
    {
        dp_display_value(outbuf[bytes-1-idx],descriptive);
        dp_display_text((DPCHAR*)" ");
    }
    /* End of User defined */
    return;    
}
#endif
#endif




/* *************** End of File *************** */
