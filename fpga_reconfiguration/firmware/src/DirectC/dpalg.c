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
/*  Module:         dpalg.c                                                 */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */
#include "DirectC/dpuser.h"
#include "DirectC/dputil.h"
#include "DirectC/dpalg.h"
#include "DirectC/G3Algo/dpG3alg.h"
#include "G4Algo/dpG4alg.h"
#include "G5Algo/dpG5alg.h"
#include "RTG4algo/dpRTG4alg.h"
#include "DirectC/JTAG/dpjtag.h"
#include "DirectC/dpcom.h"

DPUCHAR Action_code; /* used to hold the action codes as defined in dpalg.h */
DPUCHAR Action_done; /* used to hold the action codes as defined in dpalg.h */
DPUCHAR opcode; /* Holds the opcode value of the IR register prior to loading */

DPULONG device_ID;  /* Holds the device ID */
DPUCHAR device_rev; /* Holds the device revision */
DPUCHAR device_family = 0U;    /* Read from the data file AFS, or G3 */

#ifdef ENABLE_DISPLAY
DPULONG old_progress = 0;
DPULONG new_progress;
#endif


#ifdef ENABLE_G3_SUPPORT
DPUINT device_bsr_bit_length; /* Holds the bit length of the BSR register */
#endif

/* DataIndex variable is used to keep track of the position of the data 
* loaded in the file 
*/
DPULONG DataIndex;   

/* error_code holds the error code that could be set in the programming 
* functions 
*/
DPUCHAR error_code; 

DPUCHAR dp_top (void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nIdentifying device...");
    #endif
    
    error_code = DPE_SUCCESS;
    dp_init_com_vars();
    /* This function call is exit Avionics mode on RTG4 */
    dp_exit_avionics_mode();
    dp_check_and_get_image_size();

    if (error_code == DPE_SUCCESS)
    {
        goto_jtag_state(JTAG_TEST_LOGIC_RESET,0u);
        error_code = DPE_CODE_NOT_ENABLED;
        Action_done = FALSE;
        
        #ifdef ENABLE_G3_SUPPORT
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nLooking for A3P/AFS device...");
        #endif
        
        error_code = DPE_SUCCESS;
        dp_read_idcode();
        dp_check_device_ID();
        if (error_code == DPE_SUCCESS)
        {
            dp_top_g3();
            Action_done = TRUE;
        }
        #endif
        
        #ifdef ENABLE_G4_SUPPORT
        if (Action_done == FALSE)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nLooking for M2S/M2GL device...");
            #endif
            error_code = DPE_SUCCESS;
            dp_read_idcode();            
            dp_check_G4_device_ID();
            if ((error_code == DPE_SUCCESS) && (device_family == G4_FAMILY))
            {
                dp_top_g4 ();
                Action_done = TRUE;
            }
        }
        #endif
        
        #ifdef ENABLE_RTG4_SUPPORT
        if (Action_done == FALSE)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nLooking for RTG4 device...");
            #endif
            error_code = DPE_SUCCESS;
            dp_read_idcode();            
            dp_check_RTG4_device_ID();
            if ((error_code == DPE_SUCCESS) && (device_family == RTG4_FAMILY))
            {
                dp_top_rtg4 ();
                Action_done = TRUE;
            }
        }
        #endif
        
        #ifdef ENABLE_G5_SUPPORT
        if (Action_done == FALSE)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nLooking for MPF device...");
            #endif
            error_code = DPE_SUCCESS;
            dp_read_idcode();            
            dp_check_G5_device_ID();
            if ((error_code == DPE_SUCCESS) && (device_family == G5_FAMILY))
            {
                dp_top_g5 ();
                Action_done = TRUE;
            }
        }
        #endif
        
    }
    return error_code;
}

void dp_read_idcode(void)
{
    
    opcode = IDCODE;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    DRSCAN_out(IDCODE_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
    device_ID = (DPULONG)global_buf1[0] | (DPULONG)global_buf1[1] << 8u | 
    (DPULONG)global_buf1[2] << 16u | (DPULONG)global_buf1[3] << 24u;
    device_rev = (DPUCHAR) (device_ID >> 28);
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nActID = ");
    dp_display_value(device_ID,HEX);
    #endif
    
    
    return;
}

void dp_read_usercode(void)
{
    
    opcode = USERCODE;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    DRSCAN_out(USERCODE_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nUser Code = ");
    dp_display_array(global_buf1,4u,HEX);
    #endif
    
    
    return;
}


#ifdef ENABLE_DISPLAY
void dp_read_idcode_action(void)
{
    return;
}
#endif

/* *************** End of File *************** */
