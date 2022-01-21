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
/*  Module:         dpRTG4alg.c                                             */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#ifdef ENABLE_RTG4_SUPPORT
#include "dputil.h"
#include "dpcom.h"
#include "dpalg.h"
#include "dpRTG4alg.h"
#include "dpjtag.h"

DPUCHAR rtg4_pgmmode;
DPUCHAR rtg4_pgmmode_flag;
DPUCHAR rtg4_shared_buf[688]; // Maximum of 768
DPUCHAR rtg4_poll_buf[17];
DPULONG rtg4_poll_index;

DPUINT  rtg4_prev_failed_component = 0;
DPULONG rtg4_prev_failed_block = 0;
DPUINT  rtg4_prev_unique_error_code = 0;
DPUCHAR rtg4_prev_error_code = 0;
DPUCHAR rtg4_prev_bserror_code = 0;

DPUINT  rtg4_current_failed_component = 0;
DPULONG rtg4_current_failed_block = 0;
DPUINT  rtg4_current_unique_error_code = 0;
DPUCHAR rtg4_current_error_code = 0;
DPUCHAR rtg4_current_bserror_code = 0;


/****************************************************************************
* Purpose: main entry function                                              
*  This function needs to be called from the main application function with 
*  the approppriate action code set to intiate the desired action.
****************************************************************************/
DPUCHAR dp_top_rtg4 (void) 
{
    dp_init_vars();
    dp_init_RTG4_vars();
    goto_jtag_state(JTAG_TEST_LOGIC_RESET,0u);
    dp_check_RTG4_action();
    if (error_code == DPE_SUCCESS)
    {
        dp_perform_RTG4_action();
    }
    
    return error_code;
}

void dp_init_RTG4_vars(void)
{
    rtg4_pgmmode_flag = FALSE;
    return;
}

void dp_RTG4_exit_avionics_mode(void)
{
    /* In order to exit avioncs mode, trstb must be held high and then either power cycle the device 
    or toggle devrst pin */
    return;
}


void dp_check_RTG4_action(void)
{
    if ( (Action_code == DP_READ_IDCODE_ACTION_CODE) || (Action_code == DP_DEVICE_INFO_ACTION_CODE) )
    {
        #ifndef ENABLE_DISPLAY
        error_code = DPE_CODE_NOT_ENABLED;
        #endif 
    }
    else if (! (
    (Action_code == DP_ERASE_ACTION_CODE) ||
    (Action_code == DP_PROGRAM_ACTION_CODE) ||
    (Action_code == DP_REPROGRAM_INFLIGHT_ACTION_CODE) ||
    (Action_code == DP_VERIFY_ACTION_CODE) ||
    (Action_code == DP_CHECK_BITSTREAM_ACTION_CODE) ||
    (Action_code == DP_VERIFY_DIGEST_ACTION_CODE)
    ))
    {
        error_code = DPE_ACTION_NOT_SUPPORTED;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nInvalid action.");
        #endif
    }
    return;
}

void dp_perform_RTG4_action (void)
{
    #ifdef ENABLE_DISPLAY
    if (Action_code == DP_READ_IDCODE_ACTION_CODE)
    {
        dp_read_idcode_action();
        Action_done = TRUE;
    }
    else if (Action_code == DP_DEVICE_INFO_ACTION_CODE)
    {
        dp_RTG4M_device_info_action();
        Action_done = TRUE;
    }
    #endif
    if (Action_done == FALSE)
    {
        dp_check_image_crc();
        if (error_code == DPE_SUCCESS)
        {
            dp_check_RTG4_device_ID();
            if (error_code == DPE_SUCCESS)
            {
                switch (Action_code) 
                {
                    case DP_ERASE_ACTION_CODE: 
                    dp_RTG4M_erase_action();
                    break;         
                    case DP_PROGRAM_ACTION_CODE: 
                    dp_RTG4M_program_action();
                    break;
                    case DP_REPROGRAM_INFLIGHT_ACTION_CODE:
                    dp_RTG4M_reprogram_inflight_action();
                    break;
                    case DP_VERIFY_ACTION_CODE: 
                    dp_RTG4M_verify_action();
                    break;
                    case DP_CHECK_BITSTREAM_ACTION_CODE: 
                    dp_RTG4M_check_bitstream_action();
                    break;
                    case DP_VERIFY_DIGEST_ACTION_CODE: 
                    dp_RTG4M_verify_digest_action();
                    break;
                }
            }
        }
    }
    dp_RTG4M_exit();
    return;
}


void dp_RTG4M_erase_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nPerforming erase action: ");
    #endif  
    
    dp_RTG4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        rtg4_pgmmode = 0x1u;
        dp_RTG4M_set_mode();
        
        if (error_code == DPE_SUCCESS)
        {
            
            /* Global unit1 is used to hold the number of components */
            global_uint2 = 2u;
            dp_RTG4M_process_data(RTG4M_erasedatastream_ID);
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_ERASE_ERROR;
            }
        }
    }
    return;
}

void dp_RTG4M_reprogram_inflight_action(void)
{
    DPUCHAR done = 0u;
    DPUINT pgm_attempts = 0;
    DPUINT vfy_attempts = 0;
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nPerforming Reprogram_inFlight action: ");
    #endif  
    
    dp_RTG4M_clear_errors();
    
    dp_RTG4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        while ((!done) && (pgm_attempts < RT4G_MAX_PROGRAM_ATTEMPTS))
        {
            error_code = DPE_SUCCESS;
            vfy_attempts = 0;
            pgm_attempts++;
            dp_RTG4M_do_program();
            if (error_code != DPE_SUCCESS)
            {
                // If the previous programming run failed on the same location, abort
                if (
                (rtg4_current_failed_component == rtg4_prev_failed_component) &&
                (rtg4_current_failed_block == rtg4_prev_failed_block) &&
                (rtg4_current_error_code == rtg4_prev_error_code) &&
                (rtg4_current_bserror_code == rtg4_prev_bserror_code) &&
                (rtg4_current_unique_error_code == rtg4_prev_unique_error_code)
                )
                {
                    break;
                }
                else // Otherwise, save the error and try again
                {
                    rtg4_prev_failed_component = rtg4_current_failed_component;
                    rtg4_prev_failed_block = rtg4_current_failed_block;
                    rtg4_prev_error_code = rtg4_current_error_code;
                    rtg4_prev_bserror_code = rtg4_current_bserror_code;
                    rtg4_prev_unique_error_code = rtg4_current_unique_error_code;
                }
            }
            else
            {
                dp_RTG4M_clear_errors();
                
                while ((!done) && (vfy_attempts < RT4G_MAX_VERIFY_ATTEMPTS))
                {
                    error_code = DPE_SUCCESS;                  
                    vfy_attempts++;
                    dp_RTG4M_do_verify();
                    if (error_code == DPE_SUCCESS)
                    {
                        done = 1u;
                    }
                    else if ( // If the error is the same as the previous error, abort.  It will attempt to reprogram
                    (rtg4_current_failed_component == rtg4_prev_failed_component) &&
                    (rtg4_current_failed_block == rtg4_prev_failed_block) &&
                    (rtg4_current_error_code == rtg4_prev_error_code) &&
                    (rtg4_current_bserror_code == rtg4_prev_bserror_code) &&
                    (rtg4_current_unique_error_code == rtg4_prev_unique_error_code)
                    )
                    {
                        dp_RTG4M_clear_errors();
                        break;
                    }
                    else
                    { // Otherwise, save the errors and try verify again.
                        rtg4_prev_failed_component = rtg4_current_failed_component;
                        rtg4_prev_failed_block = rtg4_current_failed_block;
                        rtg4_prev_error_code = rtg4_current_error_code;
                        rtg4_prev_bserror_code = rtg4_current_bserror_code;
                        rtg4_prev_unique_error_code = rtg4_current_unique_error_code;
                    }
                }
            }
        }
    }
    return;
}

void dp_RTG4M_clear_errors(void)
{
    rtg4_prev_failed_component = 0;
    rtg4_current_failed_component = 0;
    rtg4_prev_failed_block = 0;
    rtg4_current_failed_block = 0;
    rtg4_prev_error_code = 0; 
    rtg4_current_error_code = 0;
    rtg4_prev_bserror_code = 0;
    rtg4_current_bserror_code = 0;
    rtg4_prev_unique_error_code = 0;
    rtg4_current_unique_error_code = 0 ;
    
    return;
}

void dp_RTG4M_program_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nPerforming program action: ");
    #endif  
    
    dp_RTG4M_initialize();
    
    if (error_code == DPE_SUCCESS)
    {
        dp_RTG4M_do_program();
    }
    return;
}

void dp_RTG4M_do_program(void)
{
    dp_RTG4M_check_cycle_count();
    if (error_code == DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY  
        dp_display_text((DPCHAR*)"\r\nPerforming stand alone program...");
        #endif
        rtg4_pgmmode = 0x1u;
        dp_RTG4M_set_mode();
        
        if (error_code == DPE_SUCCESS)
        {
            
            global_uint2 = 1u;
            dp_RTG4M_process_data(RTG4M_datastream_ID);
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_CORE_PROGRAM_ERROR;
            }
            
        }
    }
    return;
}


void dp_RTG4M_verify_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nPerforming verify action: ");
    #endif  
    
    dp_RTG4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        dp_RTG4M_do_verify();
    }
    
    return;
}

void dp_RTG4M_do_verify(void)
{
    #ifdef ENABLE_DISPLAY    
    dp_display_text((DPCHAR*)"\r\nPerforming stand alone verify...");
    #endif
    rtg4_pgmmode = 0x2u;
    dp_RTG4M_set_mode();
    if (error_code == DPE_SUCCESS)
    {
        
        global_uint2 = 1u;
        dp_RTG4M_process_data(RTG4M_datastream_ID);
        if(error_code != DPE_SUCCESS)
        {
            error_code = DPE_CORE_PROGRAM_ERROR;
        }
    }
    return;
}


void dp_RTG4M_check_bitstream_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nPerforming check bitstream action: ");
    #endif  
    
    dp_RTG4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        rtg4_pgmmode = 0x0u;
        dp_RTG4M_set_mode();
        
        if (error_code == DPE_SUCCESS)
        {
            /* Global unit2 is used to hold the number of components */
            global_uint2 = 1u;
            dp_RTG4M_process_data(RTG4M_datastream_ID);
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_AUTHENTICATION_FAILURE;
            }
        }
    }
    return;
}

void dp_RTG4M_verify_digest_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nPerforming verify digest action: ");
    #endif  
    
    dp_RTG4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        
        opcode = RTG4M_VERIFY_DIGEST;
        IRSCAN_in();
        dp_flush_global_buf1();
        global_buf1[0] = 0x1u;
        DRSCAN_in(0u, RTG4M_STATUS_REGISTER_BIT_LENGTH, global_buf1);
        
        goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
        dp_delay(RTG4M_STANDARD_DELAY);
        
        
        opcode = RTG4M_VERIFY_DIGEST;
        dp_RTG4M_device_poll(64u, 63u);
        if (error_code != DPE_SUCCESS)
        {
            error_code = DPE_VERIFY_DIGEST_ERROR;
            unique_exit_code = 32777;
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to verify digest: Instruction timed out.");
            dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
            dp_display_value(unique_exit_code, HEX);
            #endif
        }
        else
        {
            if (( rtg4_poll_buf[1] & 0x40u) == 0x40u)
            {
                unique_exit_code = 32777;
                error_code = DPE_VERIFY_DIGEST_ERROR;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nFPGA array digest check is disabled.");
                #endif
            }
            else if (( rtg4_poll_buf[1] & 0x80u) == 0x80u)
            {
                unique_exit_code = 32776;
                error_code = DPE_VERIFY_DIGEST_ERROR;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nDevice is blank.");
                #endif
            }
            else
            {
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nFCRC: ");
                dp_display_array(&rtg4_poll_buf[2],4u,HEX);
                #endif
                if (( rtg4_poll_buf[0] & 0x1u) == 0x1u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nFPGA Fabric digest verification: PASS.");
                    #endif
                }
                else
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nFPGA Fabric digest verification: FAIL.");
                    #endif
                    unique_exit_code = 32779;
                    error_code = DPE_VERIFY_DIGEST_ERROR;
                }
                if (( rtg4_poll_buf[0] & 0x4u) == 0x0u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nFactory row segment digest verification: FAIL.");
                    #endif
                    unique_exit_code = 32780;
                    error_code = DPE_VERIFY_DIGEST_ERROR;
                }
                #ifdef ENABLE_DISPLAY
                if (unique_exit_code != DPE_SUCCESS)
                {
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
                    dp_display_value(unique_exit_code, HEX);
                }
                #endif
            }
        }
    }
    return;
}

void dp_RTG4M_check_core_status(void)
{
    
    opcode = RTG4M_ISC_NOOP;
    IRSCAN_out(&global_uchar1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,1u);
    
    if ((global_uchar1 & 0x4u) == 0x4u)
    {
        core_is_enabled = 1;
    }
    else
    {
        core_is_enabled = 0;
    }
    
    return;
}


#ifdef ENABLE_DISPLAY
void dp_RTG4M_display_core_status(void)
{
    #ifdef ENABLE_DISPLAY
    if (core_is_enabled == 1)
    {
        dp_display_text((DPCHAR*)"\r\nFPGA Array is programmed and enabled.");
    }
    else if (core_is_enabled == 0)
    {
        dp_display_text((DPCHAR*)"\r\nFPGA Array is not enabled.");
    }
    else
    {
        dp_display_text((DPCHAR*)"\r\nWarning: CoreEnable bit is not inspected.");
    }
    #endif
    
    return;
}

void dp_RTG4M_device_info_action(void)
{
    dp_RTG4M_initialize();
    dp_read_idcode();
    
    
    opcode = USERCODE;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,1u);
    DRSCAN_out(IDCODE_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
    dp_display_text((DPCHAR*)"\r\nSILSIG: ");
    dp_display_array(global_buf1,4,HEX);
    
    
    dp_RTG4M_check_core_status();
    dp_RTG4M_display_core_status();
    dp_RTG4M_read_design_info();
    dp_RTG4M_read_prog_info();
    dp_RTG4M_query_security();
    dp_RTG4M_read_digest();
    
    return;
}

void dp_RTG4M_read_design_info(void)
{
    opcode = RTG4M_READ_DESIGN_INFO;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4M_DESIGN_INFO_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
    dp_delay(RTG4M_STANDARD_DELAY);
    opcode = RTG4M_READ_DESIGN_INFO;
    dp_RTG4M_device_poll(8u, 7u);
    if (error_code == DPE_SUCCESS)
    {
        dp_RTG4M_read_shared_buffer(4u);
        
        if (error_code == DPE_SUCCESS)
        {
            dp_display_text((DPCHAR*)"\r\nREAD_DESIGN_INFO:\r\n");
            dp_display_array(rtg4_shared_buf,50u,HEX);
            
            dp_display_text((DPCHAR*)"\r\nDesign Name: ");
            
            for (global_uchar1 = 2u; global_uchar1 < 32u; global_uchar1++)
            {
                dp_display_value(rtg4_shared_buf[global_uchar1],CHR);
            }
            dp_display_text((DPCHAR*)"\r\nChecksum: ");
            dp_display_array(rtg4_shared_buf,2u,HEX);
            dp_display_text((DPCHAR*)"\r\nDESIGNVER: ");
            dp_display_array(&rtg4_shared_buf[32],2u,HEX);
        }
    }
    return;
}

void dp_RTG4M_read_prog_info(void)
{
    opcode = RTG4M_READ_PROG_INFO;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4M_FRAME_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
    dp_delay(RTG4M_STANDARD_DELAY);
    
    opcode = RTG4M_READ_PROG_INFO;
    dp_RTG4M_device_poll(128u, 127u);
    if (error_code == DPE_SUCCESS)
    {
        dp_display_text((DPCHAR*)"\r\nPROG_INFO: ");
        dp_display_array(rtg4_poll_buf,16u,HEX);
        if  ( ((rtg4_poll_buf[8] & 0x3f) != 0u) && ((rtg4_poll_buf[8] & 0x3f) != 0x3fu) )
        {
            dp_display_text((DPCHAR*)"\r\nAlgorithm Version: ");
            dp_display_value((rtg4_poll_buf[8] & 0x3f),DEC);
        }
        
        global_uchar1 = ((rtg4_poll_buf[8] >> 6) | (rtg4_poll_buf[9] << 2)) & 0xfu;
        dp_display_text((DPCHAR*)"\r\nProgrammer code: ");
        dp_display_value(global_uchar1, DEC);
        
        global_uchar1 = ((rtg4_poll_buf[10] >> 1)) & 0x3fu;
        dp_display_text((DPCHAR*)"\r\nSoftware version code: ");
        dp_display_value(global_uchar1, DEC);
        
        global_uchar1 = ((rtg4_poll_buf[10] >> 7) | (rtg4_poll_buf[11] << 1)) & 0x7u;
        dp_display_text((DPCHAR*)"\r\nProgramming Software code: ");
        dp_display_value(global_uchar1, DEC);
        
        global_uchar1 = ((rtg4_poll_buf[11] >> 2)) & 0x7u;
        dp_display_text((DPCHAR*)"\r\nProgramming Interface Protocol code: ");
        dp_display_value(global_uchar1, DEC);
        
        global_uchar1 = ((rtg4_poll_buf[11] >> 5)) & 0x7u;
        dp_display_text((DPCHAR*)"\r\nProgramming File Type code: ");
        dp_display_value(global_uchar1, DEC);
        
    }
    
    return;
}

void dp_RTG4M_read_dsn(void)
{
    opcode = RTG4M_READ_DSN;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4M_FRAME_STATUS_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
    opcode = RTG4M_READ_DSN;
    dp_RTG4M_device_poll(129u, 128u);
    if ((error_code == DPE_POLL_ERROR) && (unique_exit_code == DPE_SUCCESS))
    {
        unique_exit_code = 32769;
        dp_display_text((DPCHAR*)"\r\nFailed to read DSN.\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);
    }
    else
    {
        dp_display_text((DPCHAR*)"\r\n=====================================================================");
        dp_display_text((DPCHAR*)"\r\nDSN: ");
        dp_display_array(rtg4_poll_buf, 16u, HEX);
        dp_display_text((DPCHAR*)"\r\n=====================================================================");
    }
    return;
}


void dp_RTG4M_read_digest(void)
{
    opcode = RTG4M_READ_DIGEST;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
    dp_delay(RTG4M_STANDARD_DELAY);
    opcode = RTG4M_READ_DIGEST;
    dp_RTG4M_device_poll(64u, 63u);
    if (error_code == DPE_SUCCESS)
    {
        if (( rtg4_poll_buf[0] & 0x1u) == 0x1u)
        {
            dp_display_text((DPCHAR*)"\r\nFPGA array digest read is protected.");
        }
        else if (( rtg4_poll_buf[0] & 0x2u) == 0x2u)
        {
            dp_display_text((DPCHAR*)"\r\nDevice is blank and digest cannot be read.");
        }
        else if (( rtg4_poll_buf[0] & 0x4u) == 0x4u)
        {
            dp_display_text((DPCHAR*)"\r\nFailed to read digest.");
        }
        else
        {
            dp_display_text((DPCHAR*)"\r\nFCRC: ");
            dp_display_array(&rtg4_poll_buf[2],4u,HEX);
        }
    }
    return;
}

#endif

/* Checking device ID function.  ID is already read in dpalg.c */
void dp_check_RTG4_device_ID (void)
{
    
    /* DataIndex is a variable used for loading the array data but not used now.  
    * Therefore, it can be used to store the Data file ID for */
    DataIndex = dp_get_bytes(Header_ID,RTG4M_ID_OFFSET,RTG4M_ID_BYTE_LENGTH);
    
    global_ulong1 = dp_get_bytes(Header_ID,RTG4M_ID_MASK_OFFSET,4U);
    device_exception = (DPUCHAR)dp_get_bytes(Header_ID,RTG4M_DEVICE_EXCEPTION_OFFSET, RTG4M_DEVICE_EXCEPTION_BYTE_LENGTH);
    
    device_ID &= global_ulong1;
    DataIndex &= global_ulong1;
    
    
    /* Identifying target device and setting its parms */
    if ( (DataIndex & 0xfff) == MICROSEMI_ID)
    {
        if (device_ID == DataIndex )
        {
            if ( (device_exception == RT4G150_ES_DEVICE_CODE) && (device_rev > 1u))
            {
                unique_exit_code = 32773;
                error_code = DPE_IDCODE_ERROR;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nFailed to verify IDCODE");
                dp_display_text((DPCHAR*)"\r\nRT4G150_ES DAT file is not compatible with RT4G150 production devices.");
                dp_display_text((DPCHAR*)"\r\nYou must use a DAT file for RT4G150 device."); 
                dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
                dp_display_value(unique_exit_code, HEX);
                #endif
            }
            else if ( (device_exception == RT4G150_DEVICE_CODE) && (device_rev < 2u))
            {
                unique_exit_code = 32774;
                error_code = DPE_IDCODE_ERROR;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nFailed to verify IDCODE\r\nRT4G150 DAT file is not compatible with RT4G150_ES devices.\r\nYou must use a DAT file for RT4G150_ES device."); 
                dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
                dp_display_value(unique_exit_code, HEX);
                #endif
            }
            else
            {
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nActID = ");
                dp_display_value(device_ID,HEX);
                dp_display_text((DPCHAR*)" ExpID = ");
                dp_display_value(DataIndex,HEX);
                dp_display_text((DPCHAR*)"\r\nDevice Rev = ");
                dp_display_value(device_rev,HEX);
                #endif
                device_family = (DPUCHAR) dp_get_bytes(Header_ID,RTG4M_DEVICE_FAMILY_OFFSET,RTG4M_DEVICE_FAMILY_BYTE_LENGTH);
            }
        }
        else
        {
            error_code = DPE_IDCODE_ERROR;
            unique_exit_code = 32772;
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)" ExpID = ");
            dp_display_value(DataIndex,HEX);
            dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
            dp_display_value(unique_exit_code, HEX);
            #endif
        }
    }
    else
    {
        error_code = DPE_IDCODE_ERROR;
    }
    
    return;
}

/* Check if system controller is ready to enter programming mode */
void dp_RTG4M_device_poll(DPUCHAR bits_to_shift, DPUCHAR Busy_bit)
{
    for ( rtg4_poll_index = 0U; rtg4_poll_index <= RTG4M_MAX_CONTROLLER_POLL; rtg4_poll_index++ )
    {
        IRSCAN_in();
        DRSCAN_out(bits_to_shift, (DPUCHAR*)DPNULL, rtg4_poll_buf);
        dp_delay(RTG4M_STANDARD_DELAY);
        if ( ((rtg4_poll_buf[Busy_bit/8] & (1 << (Busy_bit % 8))) == 0x0u))
        {
            break;
        }
    }
    if(rtg4_poll_index > RTG4M_MAX_CONTROLLER_POLL)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nDevice polling failed.");
        #endif
        error_code = DPE_POLL_ERROR;
    }
    
    return;
}

void dp_RTG4M_read_shared_buffer(DPUCHAR ucNumOfBlocks)
{
    
    dp_flush_global_buf1();
    
    for (global_uchar1 = 0u; global_uchar1 < ucNumOfBlocks; global_uchar1++)
    {
        global_buf1[0] = (global_uchar1 << 1u);
        opcode = RTG4M_READ_BUFFER;
        IRSCAN_in();
        DRSCAN_in(0u, RTG4M_FRAME_STATUS_BIT_LENGTH, global_buf1);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
        dp_delay(RTG4M_STANDARD_DELAY);
        opcode = RTG4M_READ_BUFFER;
        dp_RTG4M_device_poll(129u, 128u);
        for (global_uchar2 = 0;global_uchar2 < 16u; global_uchar2++)
        {
            rtg4_shared_buf[global_uchar1*16u + global_uchar2] = rtg4_poll_buf[global_uchar2];
        }
    }
    
    
    return;
}

void dp_RTG4M_poll_device_ready(void)
{
    opcode = RTG4M_ISC_NOOP;
    for ( rtg4_poll_index = 0U; rtg4_poll_index <= RTG4M_MAX_CONTROLLER_POLL; rtg4_poll_index++ )
    {
        IRSCAN_in();
        goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
        dp_delay(RTG4M_STANDARD_DELAY);
        DRSCAN_out(8u, (DPUCHAR*)DPNULL, rtg4_poll_buf);
        
        if ((rtg4_poll_buf[0] & 0x80u) == 0x0u)
        {
            break;
        }
    }
    if(rtg4_poll_index > RTG4M_MAX_CONTROLLER_POLL)
    {
        error_code = DPE_POLL_ERROR;  
        unique_exit_code = 32770;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nDevice is busy.");
        dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    
    return;
}
/****************************************************************************
* Purpose: Loads the BSR regsiter with data specified in the data file.  
*   If BSR_SAMPLE is enabled, the data is not loaded.  Instead, the last known
*   State of the IOs is maintained by stepping through DRCapture JTAG state.
****************************************************************************/
void dp_RTG4M_load_bsr(void)
{
    DPUCHAR capture_last_known_io_state = 0;
    DPUINT index;
    DPUCHAR mask;
    DPUCHAR c_mask;
    DPUINT bsr_bits;
    
    dp_RTG4M_check_core_status();
    
    bsr_bits = (DPUINT) dp_get_bytes(RTG4M_Header_ID,RTG4M_NUMOFBSRBITS_OFFSET,RTG4M_NUMOFBSRBITS_BYTE_LENGTH);
    opcode = ISC_SAMPLE;
    IRSCAN_in();
    
    mask = dp_get_bytes(RTG4M_BsrPattern_ID,0u,1u);
    if (return_bytes)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nLoading BSR...");
        #endif
        dp_get_and_DRSCAN_in(RTG4M_BsrPattern_ID, bsr_bits, 0u);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    }
    
    
    /* Capturing the last known state of the IOs is only valid if the core
    was programmed.  Otherwise, load the BSR with what is in the data file. */
    if (core_is_enabled == 1)
    {
        for (index = 0; index < (DPUINT)(bsr_bits + 7u ) / 8u; index++)
        {
            if (dp_get_bytes(RTG4M_BsrPatternMask_ID,index,1u) != 0)
            {
                capture_last_known_io_state = 1;
                break;
            }
        }
        if (capture_last_known_io_state)
        {
            if (bsr_bits > MAX_BSR_BIT_SIZE)
            {
                #ifdef ENABLE_DISPLAY              
                dp_display_text((DPCHAR*)"\r\nError: number of bsr bits > max buffer size.\r\nSkipping maintain last known state of the IOs...");
                #endif
            }
            else 
            {
                DRSCAN_out(bsr_bits, (DPUCHAR*)DPNULL, bsr_sample_buffer);
                
                for (index = 0; index < (DPUINT)(bsr_bits + 7u ) / 8u; index++)
                {
                    bsr_buffer[index] = dp_get_bytes(RTG4M_BsrPattern_ID,index,1u);
                    mask = dp_get_bytes(RTG4M_BsrPatternMask_ID,index,1u);
                    
                    if (mask != 0u)
                    {
                        c_mask = ~mask;
                        bsr_buffer[index] = (bsr_buffer[index] & c_mask) | (bsr_sample_buffer[index] & mask);
                    }
                }
                
                opcode = ISC_SAMPLE;
                IRSCAN_in();
                DRSCAN_in(0, bsr_bits, bsr_buffer);
                goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
            }
        }
    }
    
    return;
}

void dp_RTG4M_perform_isc_enable(void)
{
    
    rtg4_pgmmode_flag = TRUE;
    
    opcode = ISC_ENABLE;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4_ISC_STATUS_REGISTER_BIT_LENGTH, DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
    dp_delay(RTG4M_STANDARD_DELAY);
    
    opcode = ISC_ENABLE;
    dp_RTG4M_device_poll(32u, 31u);
    
    
    if ( (error_code != DPE_SUCCESS) || ((rtg4_poll_buf[0] & 0x1u) == 1u)	)
    {
        error_code = DPE_INIT_FAILURE;
        unique_exit_code = 32771;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to enter programming mode.");
        dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nISC_ENABLE_RESULT: ");
    dp_display_array(rtg4_poll_buf,4u,HEX);
    
    /* Display CRCERR */
    global_uchar1 = rtg4_poll_buf[0] & 0x1u;
    dp_display_text((DPCHAR*)"\r\nCRCERR: ");
    dp_display_value(global_uchar1,HEX);
    
    /* Display EDCERR */
    global_uchar1 = (rtg4_poll_buf[0] & 0x2u) >> 1u;
    dp_display_text((DPCHAR*)"\r\nECCRCVR: ");
    dp_display_value(global_uchar1,HEX);
    
    /* Display TEMPRANGE */
    global_uchar1 = (rtg4_poll_buf[0] >> 2) & 0x3u;
    if (global_uchar1 == 0u)
    {
        dp_display_text((DPCHAR*)"\r\nTEMPRANGE: COLD");
    }
    else if (global_uchar1 == 1u)
    {
        dp_display_text((DPCHAR*)"\r\nTEMPRANGE: ROOM");
    }
    if (global_uchar1 == 2u)
    {
        dp_display_text((DPCHAR*)"\r\nTEMPRANGE: HOT");
    }
    dp_display_text((DPCHAR*)"\r\nTEMP: ");
    dp_display_value(rtg4_poll_buf[1],HEX);
    #endif
    
    
    return;
}

/* Enter programming mode */
void dp_RTG4M_initialize(void)
{
    dp_RTG4M_poll_device_ready();
    if (error_code == DPE_SUCCESS)
    {
        dp_RTG4M_load_bsr();
        if (error_code == DPE_SUCCESS)
        {
            dp_RTG4M_perform_isc_enable();
        }
    }
    
    return;
}


/* Function is used to exit programming mode */
void dp_RTG4M_exit(void)
{
    if (rtg4_pgmmode_flag == TRUE)
    {
        #ifdef ENABLE_DISPLAY
        dp_RTG4M_read_dsn();
        #endif
        
        opcode = ISC_DISABLE;
        IRSCAN_in();
        goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
        dp_delay(RTG4M_STANDARD_DELAY);
        
        opcode = ISC_DISABLE;
        dp_RTG4M_device_poll(32u, 31u);
        #ifdef ENABLE_DISPLAY
        if ((error_code == DPE_POLL_ERROR) && (unique_exit_code == DPE_SUCCESS))
        {
            dp_display_text((DPCHAR*)"\r\nFailed to disable programming mode.");
        }
        #endif
    }
    
    opcode = RTG4M_EXTEST2;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE, RTG4M_STANDARD_CYCLES);
    dp_delay(RTG4M_EXTEST2_DELAY);
    
    opcode = RTG4M_JTAG_RELEASE;
    IRSCAN_in();
    dp_delay(RTG4M_RESET_DELAY);
    goto_jtag_state(JTAG_TEST_LOGIC_RESET,RTG4M_RESET_CYCLES);
    return;
}

void dp_RTG4M_set_mode(void)
{
    
    dp_flush_global_buf1();
    if (Action_code != DP_ERASE_ACTION_CODE)
    {
        global_buf1[4] |= (RTG4M_ALGO_VERSION & 0x3fu);
        global_buf1[6] |= (RTG4M_DIRECTC_VERSION & 0x3fu) << 1u;
        global_buf1[6] |= (DIRECTC_PROGRAMMING & 0x7u) << 7u;
        global_buf1[7] |= (DIRECTC_PROGRAMMING & 0x7u) >> 1u;
        global_buf1[7] |= 0x4Cu;
    }
    
    global_buf1[0] |= (rtg4_pgmmode & 0x3u);
    
    opcode = RTG4M_FRAME_INIT;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4M_STATUS_REGISTER_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);	
    dp_delay(RTG4M_STANDARD_DELAY);
    
    
    dp_RTG4M_device_poll(64u, 63u);
    #ifdef ENABLE_DISPLAY
    if (error_code != DPE_SUCCESS)
    {
        dp_display_text((DPCHAR*)"r\nFailed to set programming mode.");
    }
    #endif
    
    return;
}

void dp_RTG4M_process_data(DPUCHAR BlockID)
{
    
    DataIndex = 0u;  
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\n");
    #endif
    
    /* get the number of blocks */
    /* Global ulong1 is used to hold the number of blocks within the components */
    global_ulong1 = dp_get_bytes(RTG4M_NUMBER_OF_BLOCKS_ID,(DPULONG)(((global_uint2 - 1u) * 22u) / 8u),4u);
    global_ulong1 >>= ((global_uint2 - 1U)* 22u) % 8u;
    global_ulong1 &= 0x3FFFFFu;
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nProcessing component ");
    dp_display_value(global_uint2,DEC);
    dp_display_text((DPCHAR*)". Please wait...");
    #endif
    
    #ifdef ENABLE_DISPLAY
    old_progress = 0;
    #endif    
    for (global_ulong2 = 1u; global_ulong2 <= global_ulong1; global_ulong2++)
    {
        #ifdef ENABLE_DISPLAY
        new_progress = (global_ulong2 *100 / global_ulong1);
        if (new_progress != old_progress)
        {
            #ifdef ENABLE_EMBEDDED_SUPPORT
            dp_report_progress(new_progress);
            #endif
            old_progress = new_progress;
        }
        #endif
        
        opcode = RTG4M_FRAME_DATA;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(BlockID, RTG4M_FRAME_BIT_LENGTH, DataIndex);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
        dp_delay(RTG4M_STANDARD_DELAY);
        
        opcode = RTG4M_FRAME_DATA;
        dp_RTG4M_device_poll(128u, 127u);
        
        if (error_code != DPE_SUCCESS)
        {
            if (Action_code == DP_PROGRAM_ACTION_CODE)
            {
                unique_exit_code = 32788;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"Failed to program device: Instruction timed out." );
                #endif
            }
            else if (Action_code == DP_VERIFY_ACTION_CODE)
            {
                unique_exit_code = 32787;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"Failed to verify device: Instruction timed out." );
                #endif
            }
            else if (Action_code == DP_ERASE_ACTION_CODE)
            {
                unique_exit_code = 32786;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"Failed to erase device: Instruction timed out." );
                #endif
            }
            else if (Action_code == DP_CHECK_BITSTREAM_ACTION_CODE)
            {
                unique_exit_code = 32785;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"Failed to check bitstream: Instruction timed out." );
                #endif
            }
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\ncomponentNo: ");
            dp_display_value(global_uint2, DEC);
            dp_display_text((DPCHAR*)"\r\nblockNo: ");
            dp_display_value(global_ulong2, DEC);
            dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
            dp_display_value(unique_exit_code, HEX);
            #endif
            
            rtg4_current_failed_component = global_uint2;
            rtg4_current_failed_block = global_ulong2;
            rtg4_current_unique_error_code = unique_exit_code;
            
            error_code = DPE_PROCESS_DATA_ERROR;
            break;
        }
        else if(rtg4_poll_buf[0] & 0x18)
        {
            dp_RTG4M_get_data_status();
            if (error_code != DPE_SUCCESS)
            {
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nData Status Instruction timed out.");
                dp_display_text((DPCHAR*)"\r\ncomponentNo: ");
                dp_display_value(global_uint2, DEC);
                dp_display_text((DPCHAR*)"\r\nblockNo: ");
                dp_display_value(global_ulong2, DEC);
                #endif
                rtg4_current_failed_component = global_uint2;
                rtg4_current_failed_block = global_ulong2;
                rtg4_current_unique_error_code = unique_exit_code;
                
                error_code = DPE_PROCESS_DATA_ERROR;
                break;
            }
            else if ((rtg4_poll_buf[0] & 0x4u) != 0u)
            {
                if (rtg4_poll_buf[1] != 0u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nBitstream error.");
                    #endif
                    
                    if ((rtg4_poll_buf[1] == 2u) || (rtg4_poll_buf[1] == 4u) || (rtg4_poll_buf[1] == 8u))
                    {
                        unique_exit_code = 32781;
                        #ifdef ENABLE_DISPLAY
                        dp_display_text((DPCHAR*)"\r\nBitstream or data is corrupted or noisy.");
                        #endif
                    }
                    else if (rtg4_poll_buf[1] == 10u)
                    {
                        unique_exit_code = 32783;
                        #ifdef ENABLE_DISPLAY
                        dp_display_text((DPCHAR*)"\r\nIncorrect DEVICEID.");
                        #endif
                    }
                }              
                if ((rtg4_poll_buf[0] & 0xf8u) == 0x18u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nProgramming mode is not enabled.");
                    #endif
                }
                else if ((rtg4_poll_buf[0] & 0xf8u) == 0x10u)
                {
                    unique_exit_code = 32784;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nOperation has been disabled by programming bitstream settings.");
                    #endif
                }
                else if ((rtg4_poll_buf[0] & 0xf8u) == 0x8u)
                {
                    unique_exit_code = 32775;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nFailed to verify FPGA Array.");              
                    #endif
                }
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\ncomponentNo: ");
                dp_display_value(global_uint2, DEC);
                dp_display_text((DPCHAR*)"\r\nblockNo: ");
                dp_display_value(global_ulong2, DEC);
                dp_display_text((DPCHAR*)"\r\nDATA_STATUS_RESULT: ");
                dp_display_array(rtg4_poll_buf,4u,HEX);
                dp_display_text((DPCHAR*)"\r\nERRORCODE: ");
                dp_display_value((rtg4_poll_buf[0]>>3u) & 0x1fu,HEX);
                dp_display_text((DPCHAR*)"\r\nBSERRCODE: ");
                dp_display_value(rtg4_poll_buf[1],HEX);
                dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
                dp_display_value(unique_exit_code, HEX);
                #endif
                
                rtg4_current_failed_component = global_uint2;
                rtg4_current_failed_block = global_ulong2;
                rtg4_current_error_code = (rtg4_poll_buf[0]>>3u) & 0x1fu;
                rtg4_current_bserror_code = rtg4_poll_buf[1];
                rtg4_current_unique_error_code = unique_exit_code;
                
                dp_RTG4M_read_debug_info();
                if (error_code != DPE_SUCCESS)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nFailed to read debug information." );
                    #endif
                }
                else
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nDebug information: " );
                    dp_display_array(rtg4_poll_buf,16u,HEX);
                    #endif
                }
                error_code = DPE_PROCESS_DATA_ERROR;
                break;
            }
        }
        else
        {
            if ((global_ulong2 % 100u) == 0u)
            {
                dp_RTG4M_get_data_status();
                if ((rtg4_poll_buf[0] &0x2u) != 0x2u)
                {
                    unique_exit_code = 32789;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"Error, device is not ready");
                    dp_display_text((DPCHAR*)"\r\nblockNo: ");
                    dp_display_value(global_ulong2, DEC);
                    dp_display_text((DPCHAR*)"\r\nDATA_STATUS_RESULT: ");
                    dp_display_array(rtg4_poll_buf,4u,HEX);
                    dp_display_text((DPCHAR*)"\r\nERRORCODE: ");
                    dp_display_value((rtg4_poll_buf[0]>>3u) & 0x1fu,HEX);
                    dp_display_text((DPCHAR*)"\r\nBSERRCODE: ");
                    dp_display_value(rtg4_poll_buf[1],HEX);
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
                    dp_display_value(unique_exit_code, HEX);
                    #endif
                    rtg4_current_failed_component = global_uint2;
                    rtg4_current_failed_block = global_ulong2;
                    rtg4_current_error_code = (rtg4_poll_buf[0]>>3u) & 0x1fu;
                    rtg4_current_bserror_code = rtg4_poll_buf[1];
                    rtg4_current_unique_error_code = unique_exit_code;
                    dp_RTG4M_read_debug_info();                    
                    if (error_code != DPE_SUCCESS)
                    {
                        #ifdef ENABLE_DISPLAY
                        dp_display_text((DPCHAR*)"\r\nFailed to read debug information." );
                        #endif
                    }
                    else
                    {
                        #ifdef ENABLE_DISPLAY
                        dp_display_text((DPCHAR*)"\r\nDebug information: " );
                        dp_display_array(rtg4_poll_buf,16u,HEX);
                        #endif
                    }
                    error_code = DPE_PROCESS_DATA_ERROR;
                    break;
                }
            }
        }
        DataIndex += RTG4M_FRAME_BIT_LENGTH;
    }
    
    
    return;
}

void dp_RTG4M_get_data_status(void)
{
    opcode = RTG4M_FRAME_STATUS;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4_ISC_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
    dp_delay(RTG4M_STANDARD_DELAY);
    
    opcode = RTG4M_FRAME_STATUS;
    dp_RTG4M_device_poll(32u, 31u);
    
    return;
}

void dp_RTG4M_read_debug_info(void)
{
    opcode = RTG4M_DEBUG_INFO;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4M_FRAME_STATUS_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
    dp_delay(RTG4M_STANDARD_DELAY);
    
    opcode = RTG4M_DEBUG_INFO;
    dp_RTG4M_device_poll(129u, 128u);
    
    return;
}

void dp_RTG4M_check_cycle_count(void)
{
    DPUINT cycle_count = 0;
    opcode = RTG4M_QUERY_SECURITY;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
    opcode = RTG4M_QUERY_SECURITY;
    dp_RTG4M_device_poll(64u, 63u);
    if (error_code != DPE_SUCCESS)
    {
        unique_exit_code = 32782;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to query programming bitstream settings: Instruction timed out.");	
        dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);        
        #endif
    }
    else
    {
        #ifdef ENABLE_DISPLAY
        
        cycle_count = (rtg4_poll_buf[4] | (rtg4_poll_buf[5] >> 8u)) & 0x3ffu;
        dp_display_text((DPCHAR*)"\r\nCYCLE COUNT: ");
        if (cycle_count != 0x3ffu)
        {
            dp_display_value(cycle_count,DEC);
            if (cycle_count > RTG4M_MAX_ALLOWED_PROGRAMMING_CYCLES)
                dp_display_text((DPCHAR*)"\r\n***** WARNING: MAXIMUM ALLOWED PROGRAMMING CYCLE COUNT IS REACHED *****");
        }
        else
        {
            dp_display_text((DPCHAR*)" Not available. ");
        }
        #endif
    }
    return;
}


void dp_RTG4M_query_security(void)
{
    opcode = RTG4M_QUERY_SECURITY;
    IRSCAN_in();
    DRSCAN_in(0u, RTG4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,RTG4M_STANDARD_CYCLES);
    opcode = RTG4M_QUERY_SECURITY;
    dp_RTG4M_device_poll(64u, 63u);
    if (error_code != DPE_SUCCESS)
    {
        unique_exit_code = 32782;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to query programming bitstream settings: Instruction timed out.");	
        dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
        dp_display_value(unique_exit_code, HEX);        
        #endif
    }
    else
    {
        #ifdef ENABLE_DISPLAY
        if ((rtg4_poll_buf[0] & 0x4u) == 0x4u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- Device is protected with pass key");	
        }
        
        global_uint1 = (rtg4_poll_buf[4] | (rtg4_poll_buf[5] >> 8u)) & 0x3ffu;
        dp_display_text((DPCHAR*)"\r\nCYCLE COUNT: ");
        if (global_uint1 != 0x3ffu)
        {
            dp_display_value(global_uint1,DEC);
        }
        else
        {
            dp_display_text((DPCHAR*)" Not available. ");
        }
        
        
        dp_display_text((DPCHAR*)"\r\n--- Security locks and configuration settings ---\r\n");	
        if ((rtg4_poll_buf[2] & 0x1u) == 0x1u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- JTAG interface is disabled.");	
        }
        if ((rtg4_poll_buf[2] & 0x2u) == 0x2u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- SPI interface is disabled.");	
        }
        if ((rtg4_poll_buf[2] & 0x4u) == 0x4u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- FPGA array erase/write is disabled.");	
        }
        if ((rtg4_poll_buf[2] & 0x8u) == 0x8u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- FPGA array verify is disabled.");	
        }
        if ((rtg4_poll_buf[2] & 0x20u) == 0x20u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- JTAG programming is disabled.");	
        }
        if ((rtg4_poll_buf[2] & 0x40u) == 0x40u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- SPI programming is disabled.");
        }
        if ((rtg4_poll_buf[2] & 0x80u) == 0x80u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- User segment read is disabled.");	
        }
        if ((rtg4_poll_buf[3] & 0x1u) == 0x1u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- System controller suspend mode is enabled.");	
        }
        if ((rtg4_poll_buf[3] & 0x4u) == 0x4u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- Probe write is disabled.");	
        }
        if ((rtg4_poll_buf[3] & 0x8u) == 0x8u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- Probe read is disabled.");	
        }
        if ((rtg4_poll_buf[3] & 0x10u) == 0x10u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- UJTAG access to Fabric is protected.");	
        }
        if ((rtg4_poll_buf[3] & 0x20u) == 0x20u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- Exit power up if voltage is out of range is enabled.");	
        }
        if ((rtg4_poll_buf[3] & 0x40u) == 0x40u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- User lock bits bypass is enabled.");	
        }
        if ((rtg4_poll_buf[3] & 0x80u) == 0x80u)
        {
            dp_display_text((DPCHAR*)"\r\n   --- User passcode is needed for programming.");	
        }
        #endif
    }
    return;
}
#endif /* ENABLE_RTG4_SUPPORT */

/* *************** End of File *************** */

