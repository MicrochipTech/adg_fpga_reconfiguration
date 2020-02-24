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
/*  Version 4.1     Release date January 29, 2018                           */
/*                                                                          */
/* ************************************************************************ */
/*                                                                          */
/*  Module:         dputil.c                                                */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */

#include "DirectC/dpuser.h"
#include "DirectC/dputil.h"
#include "DirectC/dpcom.h"
#include "DirectC/dpalg.h"

/*
* General purpose Global variables needed in the program
*/
DPUCHAR global_uchar1;	/* Global tmp should be used once and released	*/
DPUCHAR global_uchar2;	/* Global tmp should be used once and released	*/
DPULONG global_ulong1;
DPULONG global_ulong2;
DPUINT global_uint1;
DPUINT global_uint2;
/* global_buf_SIZE needs to be set to 16 for G3 support.  Increasing the value does not hurt but not needed
as long as it does not exceed 255 */
DPUCHAR global_buf1[global_buf_SIZE];	/* General purpose global_buf1fer */
DPUCHAR global_buf2[global_buf_SIZE];	/* global_buffer to hold UROW data */



void dp_flush_global_buf1(void)
{
    DPUCHAR index;
    for (index=0u; index < global_buf_SIZE; index++)
    {
        global_buf1[index] = 0u;
    }
    return;
}

void dp_flush_global_buf2(void)
{
    DPUCHAR index;
    for (index=0u; index < global_buf_SIZE; index++)
    {
        global_buf2[index]=0u;
    }
    return;
}

void dp_init_vars(void)
{
    error_code = DPE_SUCCESS;
    return;
}

/*
* Module: dp_check_image_crc
* 		purpose: Performs crc on the entire image.  
* Return value: 
* 	DPINT: User defined integer value which reports DPE_SUCCESS if there is a match or DPE_CRC_MISMATCH if failed. 
* 
*/
void dp_check_image_crc(void)
{
    DPUINT expected_crc;
    #ifdef PERFORM_CRC_CHECK
    #ifdef ENABLE_DISPLAY        
    dp_display_text((DPCHAR*)"\r\nChecking data CRC...");
    #endif
    #endif
    
    
    global_ulong1 = dp_get_bytes(Header_ID,0u,4u);
    if ( (global_ulong1 == 0x69736544u) || (global_ulong1 == 0x65746341u) || (global_ulong1 == 0x2D4D3447u) || (global_ulong1 == 0x34475452u) || (global_ulong1 == 0x2D4D3547u))
    {
        requested_bytes = 0u;
        image_size = dp_get_bytes(Header_ID,IMAGE_SIZE_OFFSET,4u); 
        expected_crc = (DPUINT) dp_get_bytes(Header_ID,image_size - 2u,2u);
        #ifdef ENABLE_DISPLAY        
        dp_display_text((DPCHAR*)"\r\nExpected CRC=");
        dp_display_value( expected_crc ,HEX);
        #endif
        if (image_size == 0u)
        {
            error_code = DPE_CRC_MISMATCH;
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nData file is not loaded... \r\n");
            #endif
        }
        else 
        {
            #ifdef PERFORM_CRC_CHECK
            #ifdef ENABLE_DISPLAY      
            dp_display_text((DPCHAR*)"\r\nCalculating actual CRC.  Please wait...\r\n");
            #endif
            /* Global_uint is used to hold the value of the calculated CRC */
            global_uint1 = 0u;
            /* DataIndex is used to keep track the byte position in the image that is needed per get_data_request */
            #ifdef ENABLE_DISPLAY
            old_progress = 0;
            #endif    
            DataIndex = 0u;
            requested_bytes = image_size - 2u;
            while (requested_bytes)
            {
                page_buffer_ptr = dp_get_data(Header_ID,DataIndex*8u);
                if (return_bytes > requested_bytes )
                    return_bytes = requested_bytes;
                for (global_ulong1=0u; global_ulong1< return_bytes;global_ulong1++)
                {
                    global_uchar1 = page_buffer_ptr[global_ulong1];
                    dp_compute_crc();
                }
                DataIndex += return_bytes;
                requested_bytes -= return_bytes;
                
                #ifdef ENABLE_DISPLAY
                new_progress = (DataIndex *100 / (image_size - 2u));
                if (new_progress != old_progress)
                {
                    #ifdef ENABLE_EMBEDDED_SUPPORT
                    dp_report_progress(new_progress);
                    #endif
                    old_progress = new_progress;
                }
                #endif
            }
            
            if (global_uint1 != expected_crc)
            {
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nCRC verification failed.  Expected CRC = ");
                dp_display_value(global_uint1,HEX);
                dp_display_text((DPCHAR*)" Actual CRC = ");
                dp_display_value((DPUINT) dp_get_bytes(Header_ID,image_size - 2,2),HEX);
                dp_display_text((DPCHAR*)"\r\n");
                #endif
                error_code = DPE_CRC_MISMATCH;
            }
            #else
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\n\r\nWARNING: Skipping CRC verification...\r\n");
            #endif
            #endif
        }
    }
    else
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nData file is not valid. ");
        #endif
        error_code = DPE_CRC_MISMATCH;
    }
    /* User defined */
    /* Reset elapsed time if applicable */
    /* End of User defined */
    return;
}


void dp_compute_crc(void)
{
    for (global_uchar2 = 0u; global_uchar2 < 8u; global_uchar2++)
    {
        global_uint2 = (global_uchar1 ^ global_uint1) & 0x01u;
        global_uint1 >>= 1u;
        if (global_uint2)
        {
            global_uint1 ^= 0x8408u;
        }
        global_uchar1 >>= 1u;
    }
    
    return;
}

void dp_check_and_get_image_size(void)
{
    
    global_ulong1 = dp_get_bytes(Header_ID,0u,4u);
    if ( (global_ulong1 == 0x69736544u) || (global_ulong1 == 0x65746341u) || (global_ulong1 == 0x2D4D3447u) || (global_ulong1 == 0x34475452u) || (global_ulong1 == 0x2D4D3547u))
    {
        requested_bytes = 0u;
        image_size = dp_get_bytes(Header_ID,IMAGE_SIZE_OFFSET,4u);  
    }
    else
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nData file is not valid. ");
        #endif
        error_code = DPE_DAT_ACCESS_FAILURE;
    }
    return;
}

#ifdef ENABLE_DISPLAY
#define NB_NIBBLES_IN_INT		8
int int_to_hex_int(int value, unsigned char * p_result, int result_size)
{
    int nibble_idx, nb_nibbles;
    unsigned char conv_array[NB_NIBBLES_IN_INT];
    unsigned int uvalue;
    nibble_idx = 0;
    uvalue = (unsigned int)value;
    
    do {	
        int nibble = uvalue & 0x0F;
        
        if ( nibble < 10 )
            conv_array[nibble_idx] = nibble + '0';
        else
        conv_array[nibble_idx] = nibble  - 10 + 'A';
        uvalue = (uvalue >> 4);
        nibble_idx++;
    } while ( ( nibble_idx < NB_NIBBLES_IN_INT ) && ( uvalue > 0 ) );
    
    nb_nibbles = nibble_idx;
    for ( nibble_idx = 0; (nibble_idx < nb_nibbles) && (nibble_idx < result_size) ;nibble_idx++ )
    {
        p_result[nibble_idx] = conv_array[nb_nibbles - nibble_idx - 1];
    }
    return nibble_idx;
}

int int_to_dec_int (int value, unsigned char * p_result, int result_size)
{
    unsigned char conv_array[NB_NIBBLES_IN_INT];
    unsigned int uvalue;
    unsigned int remainder;
    unsigned int digit_idx,nb_digits;
    
    uvalue = (unsigned int)value;
    digit_idx=0;
    if (uvalue)
    {
        while (uvalue)
        {
            remainder = uvalue % 10;
            conv_array[digit_idx] = remainder + '0';
            uvalue /= 10;
            digit_idx++;
        }
    }
    else 
    {
        conv_array[digit_idx] = '0';
        digit_idx++;
    }
    
    
    nb_digits = digit_idx;
    for ( digit_idx = 0; (digit_idx < nb_digits); digit_idx++ )
    {
        p_result[digit_idx] = conv_array[nb_digits - digit_idx - 1];
    }
    return digit_idx;
}
#endif

/* *************** End of File *************** */

