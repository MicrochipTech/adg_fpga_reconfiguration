/* ************ MICROSEMI SOC CORP. DIRECTC LICENSE AGREEMENT ************* */
/* ------------------------------------------------------------------------ 
PLEASE READ: BEFORE INSTALLING THIS SOFTWARE, CAREFULLY READ THE FOLLOWING 
MICROSEMI SOC CORP LICENSE AGREEMENT REGARDING THE USE OF THIS SOFTWARE. 
INSTALLING THIS SOFTWARE INDICATES THAT YOU ACCEPT AND UNDERSTAND THIS AGREEMENT 
AND WILL ABIDE BY IT. 

Note: This license agreement (�License�) only includes the following software: 
DirectC. DirectC is licensed under the following terms and conditions.

Hereinafter, Microsemi SoC Corp. shall be referred to as �Licensor� or �Author,� 
whereas the other party to this License shall be referred to as �Licensee.� Each 
party to this License shall be referred to, singularly, as a �Party,� or, 
collectively, as the �Parties.�

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
shall obtain Licensor�s written consent prior to submitting any request for 
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
/*  Module:         dpchain.c                                               */
/*                                                                          */
/*  Description:    Contains chain functions                                */
/*                                                                          */
/* ************************************************************************ */
#include "dpuser.h"
#include "dpchain.h"
#include "dpjtag.h"
#include "dpcom.h"

#ifdef CHAIN_SUPPORT
/* *****************************************************************************
* Variable that must be intialized with appropriate data depending on the chain
* configuration.  See user guide for more information.
*******************************************************************************/
DPUCHAR dp_preir_data[PREIR_DATA_SIZE]={0xff};
DPUCHAR dp_predr_data[PREDR_DATA_SIZE]={0x0};
DPUCHAR dp_postir_data[POSTIR_DATA_SIZE]={0xff};
DPUCHAR dp_postdr_data[POSTDR_DATA_SIZE]={0x0};

DPUINT dp_preir_length = PREIR_LENGTH_VALUE;
DPUINT dp_predr_length = PREDR_LENGTH_VALUE;
DPUINT dp_postir_length = POSTIR_LENGTH_VALUE;
DPUINT dp_postdr_length = POSTDR_LENGTH_VALUE;

/****************************************************************************
* Purpose: clock data stored in tdi_data into the device.
* terminate is a flag needed to determine if shifting to pause state should 
* be done with the last bit shift.
****************************************************************************/
void dp_shift_in(DPULONG start_bit, DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR terminate)
{
    if (current_jtag_state == JTAG_SHIFT_IR)
    {
        if (dp_preir_length > 0U)
        {
            dp_do_shift_in(0U,dp_preir_length, dp_preir_data,0U);
        }
        if (dp_postir_length > 0U)
        {
            dp_do_shift_in(start_bit,num_bits, tdi_data,0U);
            dp_do_shift_in(0U,dp_postir_length, dp_postir_data, terminate);
        }
        else
        {
            dp_do_shift_in(start_bit,num_bits, tdi_data,terminate);
        }	
    }
    else if (current_jtag_state == JTAG_SHIFT_DR)
    {
        if (dp_predr_length > 0U)
        {
            dp_do_shift_in(0U,dp_predr_length, dp_predr_data,0U);
        }
        if (dp_postdr_length > 0U)
        {
            dp_do_shift_in(start_bit,num_bits, tdi_data,0U);
            dp_do_shift_in(0U,dp_postdr_length, dp_postdr_data, terminate);
        }
        else
        {
            dp_do_shift_in(start_bit,num_bits, tdi_data,terminate);
        }
    }
    else
    {
    }
    return;
}
/****************************************************************************
* Purpose:  clock data stored in tdi_data into the device.
*           capture data coming out of tdo into tdo_data.
* This function will always clock data starting bit postion 0.  
* Jtag state machine will always set the pauseDR or pauseIR state at the 
* end of the shift.
****************************************************************************/
void dp_shift_in_out(DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR tdo_data[])
{
    if (current_jtag_state == JTAG_SHIFT_IR)
    {
        if (dp_preir_length > 0U)
        {
            dp_do_shift_in(0U,dp_preir_length, dp_preir_data,0U);
        }
        if (dp_postir_length > 0U)
        {
            dp_do_shift_in_out(num_bits, tdi_data,tdo_data,0U);
            dp_do_shift_in(0U,dp_postir_length, dp_postir_data, 1U);
        }
        else
        {
            dp_do_shift_in_out(num_bits, tdi_data,tdo_data,1U);
        }	
    }
    else if (current_jtag_state == JTAG_SHIFT_DR)
    {
        if (dp_predr_length > 0U)
        {
            dp_do_shift_in(0U,dp_predr_length, dp_predr_data,0U);
        }
        if (dp_postdr_length > 0U)
        {
            dp_do_shift_in_out(num_bits, tdi_data,tdo_data,0U);
            dp_do_shift_in(0U,dp_postdr_length, dp_postdr_data, 1U);
        }
        else
        {
            dp_do_shift_in_out(num_bits, tdi_data,tdo_data,1U);
        }
    }
    else
    {
    }
    return;
}


void dp_do_shift_in(DPULONG start_bit, DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR terminate)
{
    idx = (DPUCHAR) start_bit >> 3;
    bit_buf = 1U << (DPUCHAR)(start_bit & 0x7U);
    if (tdi_data == (DPUCHAR*)DPNULL)
    {
        data_buf = 0U;
    }
    else
    {
        data_buf = tdi_data[idx] >> ((DPUCHAR)(start_bit & 0x7U));
    }
    if (terminate == 0U)
    {
        num_bits++;
    }
    while (--num_bits)
    {
        dp_jtag_tms_tdi(0U, data_buf&0x1U);
        data_buf >>= 1;
        bit_buf <<= 1;
        if ((bit_buf & 0xffU) == 0U )
        {
            bit_buf = 1U;
            idx++;
            if (tdi_data == (DPUCHAR*)DPNULL)
            {
                data_buf = 0U;
            }
            else 
            {
                data_buf = tdi_data[idx];
            }
        }
    }
    if (terminate)
    {
        dp_jtag_tms_tdi(1U, data_buf & 0x1U);
        if (current_jtag_state == JTAG_SHIFT_IR)
        {
            current_jtag_state = JTAG_EXIT1_IR;
        }
        else if (current_jtag_state == JTAG_SHIFT_DR)
        {
            current_jtag_state = JTAG_EXIT1_DR;
        }
        else
        {
        }
    }
    return;
}

void dp_do_shift_in_out(DPUINT num_bits, DPUCHAR tdi_data[], DPUCHAR tdo_data[], DPUCHAR terminate)
{
    bit_buf = 1U;
    idx = 0U;
    tdo_data[idx] = 0U;
    
    if (tdi_data == (DPUCHAR*)DPNULL)
    {
        data_buf = 0U;
    }
    else 
    {   
        data_buf = tdi_data[idx];
    }
    
    while (--num_bits)
    {
        if ((bit_buf & 0xffU) == 0U )
        {
            bit_buf = 1U;
            idx++;
            tdo_data[idx] = 0U;
            if (tdi_data == (DPUCHAR*)DPNULL)
            {
                data_buf = 0U;
            }
            else 
            {
                data_buf = tdi_data[idx];
            }
        }
        if (dp_jtag_tms_tdi_tdo(0U, data_buf&0x1U))
        {
            tdo_data[idx] |= bit_buf;
        }
        bit_buf <<= 1;
        data_buf>>=1;
    }
    if ((bit_buf & 0xffU) == 0U )
    {
        bit_buf = 1U;
        idx++;
        tdo_data[idx] = 0U;
        if (tdi_data == (DPUCHAR*)DPNULL)
        {
            data_buf = 0U;
        }
        else 
        {
            data_buf = tdi_data[idx];
        }
    }
    if(terminate)
    {
        if (dp_jtag_tms_tdi_tdo(1U, data_buf&0x1U))
        {
            tdo_data[idx] |= bit_buf;
        }
        if (current_jtag_state == JTAG_SHIFT_IR)
        {
            current_jtag_state = JTAG_EXIT1_IR;
        }
        else if (current_jtag_state == JTAG_SHIFT_DR)
        {
            current_jtag_state = JTAG_EXIT1_DR;
        }
        else
        {
        }
    }
    else 
    {
        if (dp_jtag_tms_tdi_tdo(0U, data_buf&0x1U))
        {
            tdo_data[idx] |= bit_buf;
        }
    }
    return;
}
/****************************************************************************
* Purpose:  Gets the data block specified by Variable_ID from the image dat
* file and clocks it into the device.
****************************************************************************/
void dp_get_and_shift_in(DPUCHAR Variable_ID,DPUINT total_bits_to_shift, DPULONG start_bit_index)
{
    DPULONG page_start_bit_index;
    DPUINT bits_to_shift;
    DPUCHAR terminate;
    page_start_bit_index = start_bit_index & 0x7U;
    requested_bytes = (DPULONG) (page_start_bit_index + total_bits_to_shift + 7U) >> 3U;
    
    if (current_jtag_state == JTAG_SHIFT_IR)
    {
        if (dp_preir_length > 0U)
        {
            dp_do_shift_in(0U,dp_preir_length, dp_preir_data,0U);
        }
    }
    else if (current_jtag_state == JTAG_SHIFT_DR)
    {
        if (dp_predr_length > 0U)
        {
            dp_do_shift_in(0U,dp_predr_length, dp_predr_data,0U);
        }
    }
    else
    {
    }
    
    terminate = 0U;
    while (requested_bytes)
    {
        page_buffer_ptr = dp_get_data(Variable_ID,start_bit_index);
        
        if (return_bytes >= requested_bytes )
        {
            return_bytes = requested_bytes;
            bits_to_shift = total_bits_to_shift;
            terminate = 1U;
            if (((current_jtag_state == JTAG_SHIFT_IR) && dp_postir_length) || ((current_jtag_state == JTAG_SHIFT_DR) && dp_postdr_length))
            {
                terminate =0U;
            }
        }
        else 
        {
            bits_to_shift = (DPUCHAR) (return_bytes * 8U - page_start_bit_index);
        }
        dp_do_shift_in(page_start_bit_index, bits_to_shift, page_buffer_ptr,terminate);
        
        requested_bytes = requested_bytes - return_bytes;
        total_bits_to_shift = total_bits_to_shift - bits_to_shift;
        start_bit_index += bits_to_shift;
        page_start_bit_index = start_bit_index & 0x7U;
    }
    
    if (current_jtag_state == JTAG_SHIFT_IR)
    {
        if (dp_postir_length > 0U)
        {
            dp_do_shift_in(0U,dp_postir_length, dp_postir_data,1U);
        }
    }
    else if (current_jtag_state == JTAG_SHIFT_DR)
    {
        if (dp_postdr_length > 0U)
        {
            dp_do_shift_in(0U,dp_postdr_length, dp_postdr_data,1U);
        }
    }
    else
    {
    }
    return;
}

/****************************************************************************
* Purpose:  Get the data block specified by Variable_ID from the image dat
* file and clocks it into the device.  Capture the data coming out of tdo 
* into tdo_data
****************************************************************************/
void dp_get_and_shift_in_out(DPUCHAR Variable_ID,DPUCHAR total_bits_to_shift, DPULONG start_bit_index,DPUCHAR* tdo_data)
{
    requested_bytes = ((DPULONG)total_bits_to_shift + 7u) >> 3u;
    page_buffer_ptr = dp_get_data(Variable_ID,start_bit_index);
    
    if (return_bytes >= requested_bytes )
    {
        return_bytes = requested_bytes;
        dp_shift_in_out((DPUINT)total_bits_to_shift, page_buffer_ptr,tdo_data);
    }
    else
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nError: Page buffer size is not big enough...");
        #endif
    }
    
    return;
}
#endif

/* *************** End of File *************** */

