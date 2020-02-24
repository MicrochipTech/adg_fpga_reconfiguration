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
/*  Module:         dpG5alg.c                                               */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */

#include "DirectC/dpuser.h"
#ifdef ENABLE_G5_SUPPORT
#include "DirectC/dputil.h"
#include "DirectC/dpcom.h"
#include "DirectC/dpalg.h"
#include "dpG5alg.h"
#include "DirectC/JTAG/dpjtag.h"

DPUCHAR g5_pgmmode;
DPUCHAR g5_pgmmode_flag;
DPUCHAR g5_shared_buf[80]; 
DPUCHAR g5_poll_buf[17];
DPULONG g5_poll_index;


/****************************************************************************
* Purpose: main entry function                                              
*  This function needs to be called from the main application function with 
*  the approppriate action code set to intiate the desired action.
****************************************************************************/
DPUCHAR dp_top_g5 (void) 
{
    dp_init_vars();
    dp_init_G5_vars();
    goto_jtag_state(JTAG_TEST_LOGIC_RESET,0u);
    dp_check_G5_action();
    if (error_code == DPE_SUCCESS)
    {
        dp_perform_G5_action();
    }
    
    return error_code;
}

void dp_init_G5_vars(void)
{
    g5_pgmmode = 0u;
    g5_pgmmode_flag = FALSE;
    
    return;
}

void dp_check_G5_action(void)
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
    (Action_code == DP_VERIFY_ACTION_CODE) ||
    (Action_code == DP_ENC_DATA_AUTHENTICATION_ACTION_CODE) ||
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

void dp_perform_G5_action (void)
{
    #ifdef ENABLE_DISPLAY
    if (Action_code == DP_READ_IDCODE_ACTION_CODE)
    {
        dp_read_idcode_action();
        Action_done = TRUE;
    }
    else if (Action_code == DP_DEVICE_INFO_ACTION_CODE)
    {
        dp_G5M_device_info_action();
        Action_done = TRUE;
    }
    #endif
    if (Action_done == FALSE)
    {
        dp_check_image_crc();
        if (error_code == DPE_SUCCESS)
        {
            dp_check_G5_device_ID();
            if (error_code == DPE_SUCCESS)
            {
                switch (Action_code) 
                {
                    case DP_ERASE_ACTION_CODE: 
                    dp_G5M_erase_action();
                    break;         
                    case DP_PROGRAM_ACTION_CODE: 
                    dp_G5M_program_action();
                    break;
                    case DP_VERIFY_ACTION_CODE: 
                    dp_G5M_verify_action();
                    break;
                    case DP_ENC_DATA_AUTHENTICATION_ACTION_CODE: 
                    dp_G5M_enc_data_authentication_action();
                    break;
                    case DP_VERIFY_DIGEST_ACTION_CODE: 
                    dp_G5M_verify_digest_action();
                    break;
                }
            }
        }
    }
    dp_G5M_exit();
    return;
}

void dp_G5M_erase_action(void)
{
    dp_G5M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        g5_pgmmode = 0x1u;
        dp_G5M_set_mode();
        
        
        if (error_code == DPE_SUCCESS)
        {
            /* Global unit1 is used to hold the number of components */
            global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G5M_NUMOFCOMPONENT_OFFSET,G5M_NUMOFCOMPONENT_BYTE_LENGTH);
            global_uint2 = global_uint1 - ((DPUINT)dp_get_bytes(Header_ID,G5M_ERASEDATASIZE_OFFSET,G5M_DATASIZE_BYTE_LENGTH) - 1u);
            
            dp_G5M_process_data(G5M_erasedatastream_ID);
            
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_ERASE_ERROR;
            }
        }
    }
    return;
}

void dp_G5M_program_action(void)
{
    dp_G5M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        g5_pgmmode = 0x1u;
        dp_G5M_set_mode();
        
        
        if (error_code == DPE_SUCCESS)
        {
            global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G5M_DATASIZE_OFFSET,G5M_DATASIZE_BYTE_LENGTH);
            global_uint2 = 1u;
            dp_G5M_process_data(G5M_datastream_ID);
            
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_CORE_PROGRAM_ERROR;
            }
        }
        
    }
    
    return;
}

void dp_G5M_verify_action(void)
{
    dp_G5M_initialize();
    
    if (error_code == DPE_SUCCESS)
    {
        g5_pgmmode = 0x2u;
        dp_G5M_set_mode();
        
        
        if (error_code == DPE_SUCCESS)
        {
            /* Global unit1 is used to hold the number of components */
            global_uint2 = 1u;
            global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G5M_DATASIZE_OFFSET,G5M_DATASIZE_BYTE_LENGTH);
            dp_G5M_process_data(G5M_datastream_ID);
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_VERIFY_ERROR;
            }
        }
    }
    
    return;
}

void dp_G5M_enc_data_authentication_action(void)
{
    dp_G5M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        g5_pgmmode = 0x0u;
        dp_G5M_set_mode();
        
        
        if (error_code == DPE_SUCCESS)
        {
            /* Global unit1 is used to hold the number of components */
            global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G5M_DATASIZE_OFFSET,G5M_DATASIZE_BYTE_LENGTH);
            global_uint2 = 1u;
            
            dp_G5M_process_data(G5M_datastream_ID);
            
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_AUTHENTICATION_FAILURE;
            }
        }
        
    }
    return;
}

void dp_G5M_verify_digest_action(void)
{
    dp_G5M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_query_security();
    }
    if ((error_code == DPE_SUCCESS) && ((g5_shared_buf[1] & G5M_UL_EXTERNAL_DIGEST_CHECK) == G5M_UL_EXTERNAL_DIGEST_CHECK) )
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"r\nExternal digest check via JTAG/SPI Slave is disabled.");
        #endif
        error_code = DPE_VERIFY_DIGEST_ERROR;
    }
    
    if (error_code == DPE_SUCCESS)
    {
        global_buf1[0] = 0x1u;
        
        opcode = G5M_CHECK_DIGESTS;
        IRSCAN_in();
        DRSCAN_in(0u, G5M_SECURITY_STATUS_REGISTER_BIT_LENGTH, global_buf1);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);	
        dp_delay(G5M_STANDARD_DELAY);
        dp_G5M_device_poll(16u, 15u);
        
        
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to verify digest.");
            #endif
            error_code = DPE_VERIFY_DIGEST_ERROR;
        }
        else
        {
            if (g5_poll_buf[1] == 0x40u)
            {
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nFailed to verify digest.");
                #endif
                error_code = DPE_VERIFY_DIGEST_ERROR;
            }
            else 
            {
                #ifdef ENABLE_DISPLAY
                if ((g5_poll_buf[0] & 0x1u )== 0x1u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- FPGA Fabric digest verification: PASS");
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- FPGA Fabric digest verification: FAIL");
                }
                if ((g5_poll_buf[0] & 0x2u )== 0x2u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- Fabric Configuration digest verification: PASS");
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- Fabric Configuration digest verification: FAIL");
                }
                if ((g5_poll_buf[0] & 0x4u )== 0x4u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- sNVM digest verification: PASS");
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- sNVM digest verification: FAIL");
                }
                if ((g5_poll_buf[0] & 0x8u )== 0x8u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- User security policies segment digest verification: PASS" );
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- User security policies segment digest verification: FAIL" );
                }
                if ((g5_poll_buf[0] & 0x10u )== 0x10u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- SMK segment digest verification: PASS");
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- SMK segment digest verification: FAIL");
                }
                if ((g5_poll_buf[0] & 0x20u )== 0x20u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- User Public Key segment digest verification: PASS");
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- User Public Key segment digest verification: FAIL");
                }
                if ((g5_poll_buf[0] & 0x40u )== 0x40u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- UPK1 segment digest verification: PASS" );
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- UPK1 segment digest verification: FAIL" );
                }
                if ((g5_poll_buf[0] & 0x80u )== 0x80u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- UEK1 segment digest verification: PASS" );
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- UEK1 segment digest verification: FAIL" );
                }
                if ((g5_poll_buf[1] & 0x1u )== 0x1u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- DPK segment digest verification: PASS" );
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- DPK segment digest verification: FAIL" );
                }
                if ((g5_poll_buf[1] & 0x2u )== 0x2u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- UPK2 segment digest verification: PASS" );
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- UPK2 segment digest verification: FAIL" );
                }
                if ((g5_poll_buf[1] & 0x4u )== 0x4u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- UEK2 segment digest verification: PASS" );
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- UEK2 segment digest verification: FAIL" );
                }
                if ((g5_poll_buf[1] & 0x10u )== 0x10u)
                {
                    dp_display_text((DPCHAR*)"\r\n --- Factory row and factory key segment digest verification: PASS");
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nWarning: --- Factory row and factory key segment digest verification: FAIL");
                }
                #endif
            }
        }
    }
    return;
}

void dp_G5M_check_core_status(void)
{
    
    opcode = G5M_ISC_NOOP;
    IRSCAN_out(&global_uchar1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,1u);
    
    #ifdef ENABLE_DISPLAY
    if ((global_uchar1 & 0x80u) == 0x80u)
    {
        dp_display_text((DPCHAR*)"\r\nFPGA Array is programmed and enabled.");
    }
    else
    {
        dp_display_text((DPCHAR*)"\r\nFPGA Array is not enabled.");
    }
    #endif
    
}

#ifdef ENABLE_DISPLAY
void dp_G5M_device_info_action(void)
{
    dp_display_text((DPCHAR*)"\r\n\r\nDevice info:");
    dp_read_usercode();
    dp_G5M_check_core_status();
    dp_G5M_read_design_info();
    dp_G5M_read_debug_info();
    dp_G5M_read_fsn();
    dp_G5M_read_tvs_monitor();
    dp_G5M_query_security();
    dp_G5M_dump_security();
    
    return;
}



void dp_G5M_dump_security(void)
{
    if ((g5_poll_buf[0] & 0x1u) == 0x1u)
    {
        dp_display_text((DPCHAR*)"\r\nSmartDebug user debug access and active probes are disabled.");
    }
    if ((g5_poll_buf[0] & 0x2u) == 0x2u)
    {
        dp_display_text((DPCHAR*)"\r\nSmartDebug sNVM debug is disabled.");
    }
    if ((g5_poll_buf[0] & 0x4u) == 0x4u)
    {
        dp_display_text((DPCHAR*)"\r\nSmartDebug Live probes are disabled.");
    }
    if ((g5_poll_buf[0] & 0x8u) == 0x8u)
    {
        dp_display_text((DPCHAR*)"\r\nUser JTAG interface is disabled");
    }
    if ((g5_poll_buf[0] & 0x10u) == 0x10u)
    {
        dp_display_text((DPCHAR*)"\r\nJTAG boundary scan is disabled.");
    }
    if ((g5_poll_buf[0] & 0x20u) == 0x20u)
    {
        dp_display_text((DPCHAR*)"\r\nReading of temperature and voltage via JTAG/SPI Slave is disabled.");
    }
    if ((g5_poll_buf[1] & 0x1u) == 0x1u)
    {
        dp_display_text((DPCHAR*)"\r\nPlaintext passcode unlock is disabled.");
    }
    if ((g5_poll_buf[1] & 0x2u) == 0x2u)
    {
        dp_display_text((DPCHAR*)"\r\nFabric erase/write is disabled.");
    }
    if ((g5_poll_buf[1] & 0x4u) == 0x4u)
    {
        dp_display_text((DPCHAR*)"\r\nExternal digest check via JTAG/SPI Slave is disabled.");
    }
    if ((g5_poll_buf[1] & 0x8u) == 0x8u)
    {
        dp_display_text((DPCHAR*)"\r\nBack level protection is enabled.");
    }
    if ((g5_poll_buf[1] & 0x10u) == 0x10u)
    {
        dp_display_text((DPCHAR*)"\r\nMicrosemi factory test mode is disabled.");
    }
    else
    {
        dp_display_text((DPCHAR*)"\r\nMicrosemi factory test mode access is allowed.");
    }
    if ((g5_poll_buf[1] & 0x20u) == 0x20u)
    {
        dp_display_text((DPCHAR*)"\r\nAuto Programming and IAP Services are disabled.");
    }
    if ((g5_poll_buf[1] & 0x40u) == 0x40u)
    {
        dp_display_text((DPCHAR*)"\r\nExternal zeroizations via JTAG/SPI Slave is disabled.");
    }
    if ((g5_poll_buf[1] & 0x80u) == 0x80u)
    {
        dp_display_text((DPCHAR*)"\r\nSPI Slave port is disabled.");
    }
    if ((g5_poll_buf[2] & 0x1u) == 0x1u)
    {
        dp_display_text((DPCHAR*)"\r\nUser lock segment is locked. FlashLock/UPK1 is required to make changes to security.");
    }
    if ((g5_poll_buf[2] & 0x2u) == 0x2u)
    {
        dp_display_text((DPCHAR*)"\r\nAuthenticate programming action for JTAG/SPI Slave is disabled.");
    }
    if ((g5_poll_buf[2] & 0x4u) == 0x4u)
    {
        dp_display_text((DPCHAR*)"\r\nProgram action for JTAG/SPI Slave is disabled.");
    }
    if ((g5_poll_buf[2] & 0x8u) == 0x8u)
    {
        dp_display_text((DPCHAR*)"\r\nVerify action for JTAG/SPI Slave is disabled.");
    }
    
    if ((g5_poll_buf[2] & 0x40u) == 0x40u)
    {
        dp_display_text((DPCHAR*)"\r\nBitstream Default encryption key (KLK) is disabled.");
    }
    if ((g5_poll_buf[2] & 0x80u) == 0x80u)
    {
        dp_display_text((DPCHAR*)"\r\nBitstream User Encryption Key 1 is disabled.");
    }
    if ((g5_poll_buf[3] & 0x1u) == 0x1u)
    {
        dp_display_text((DPCHAR*)"\r\nBitstream User Encryption Key 2 is disabled.");
    }
    if ((g5_poll_buf[4] & 0x40u) == 0x40u)
    {
        dp_display_text((DPCHAR*)"\r\nDefault encryption key (KLK) is disabled.");
    }
    if ((g5_poll_buf[4] & 0x80u) == 0x80u)
    {
        dp_display_text((DPCHAR*)"\r\nUser Encryption Key 1 is disabled.");
    }
    if ((g5_poll_buf[5] & 0x1u) == 0x1u)
    {
        dp_display_text((DPCHAR*)"\r\nUser Encryption Key 2 is disabled.");
    }
    if ((g5_poll_buf[6] & 0x10u) == 0x10u)
    {
        dp_display_text((DPCHAR*)"\r\nsNVM write is disabled.");
    }
    if ((g5_poll_buf[6] & 0x20u) == 0x20u)
    {
        dp_display_text((DPCHAR*)"\r\nPUF emulation via JTAG/SPI Slave is disabled.");
    }
    if ((g5_poll_buf[7] & 0x2u) == 0x2u)
    {
        dp_display_text((DPCHAR*)"\r\nUser Key Set 1 is locked. FlashLock/UPK1 is required to make changes.");
    }
    if ((g5_poll_buf[7] & 0x4u) == 0x4u)
    {
        dp_display_text((DPCHAR*)"\r\nUser Key Set 2 is locked. FlashLock/UPK2 is required to make changes.");
    }
    if ((g5_poll_buf[7] & 0x8u) == 0x8u)
    {
        dp_display_text((DPCHAR*)"\r\nMicrosemi factory test access is permanently disabled.");
    }
    if ((g5_poll_buf[7] & 0x10u) == 0x10u)
    {
        dp_display_text((DPCHAR*)"\r\nSmartDebug debugging is permanently disabled.");
    }
    if ((g5_poll_buf[7] & 0x20u) == 0x20u)
    {
        dp_display_text((DPCHAR*)"\r\nFabric erase/write is permanently disabled.");
    }
    if ((g5_poll_buf[7] & 0x40u) == 0x40u)
    {
        dp_display_text((DPCHAR*)"\r\nFlashLock/UPK1 unlocking is permanently disabled");
    }
    if ((g5_poll_buf[7] & 0x80u) == 0x80u)
    {
        dp_display_text((DPCHAR*)"\r\nFlashLock/UPK2 unlocking is permanently disabled.");
    }
    if ((g5_poll_buf[8] & 0x1u) == 0x1u)
    {
        dp_display_text((DPCHAR*)"\r\nFlashLock/DPK unlocking is permanently disabled.");
    }
    if ((g5_poll_buf[8] & 0x2u) == 0x2u)
    {
        dp_display_text((DPCHAR*)"\r\nUPERM segment is permanently locked.");
    }
    return;
}

void dp_G5M_read_design_info(void)
{
    opcode = G5M_READ_DESIGN_INFO;
    IRSCAN_in();
    DRSCAN_in(0u, G5M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    dp_delay(G5M_STANDARD_DELAY);
    opcode = G5M_READ_DESIGN_INFO;
    dp_G5M_device_poll(8u, 7u);
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_read_shared_buffer(3u);
        if (error_code == DPE_SUCCESS)
        {
            dp_display_text((DPCHAR*)"\r\nDesign Name: ");
            
            
            for (global_uchar1 = 2u; global_uchar1 < 32u; global_uchar1++)
            {
                dp_display_value(g5_shared_buf[global_uchar1],CHR);
            }
            dp_display_text((DPCHAR*)"\r\nChecksum: ");
            dp_display_array(g5_shared_buf,2u,HEX);
            dp_display_text((DPCHAR*)"\r\nDesign Info: \r\n");
            dp_display_array(g5_shared_buf,34u,HEX);
            dp_display_text((DPCHAR*)"\r\nDESIGNVER: ");
            dp_display_array(&g5_shared_buf[32],2u,HEX);
            dp_display_text((DPCHAR*)"\r\nBACKLEVEL: ");
            dp_display_array(&g5_shared_buf[34],2u,HEX);
            dp_display_text((DPCHAR*)"\r\n-----------------------------------------------------");
        }
    }
    
    return;
}

void dp_G5M_read_debug_info(void)
{
    opcode = G5M_READ_DEBUG_INFO;
    IRSCAN_in();
    DRSCAN_in(0u, G5M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    dp_delay(G5M_STANDARD_DELAY);
    
    opcode = G5M_READ_DEBUG_INFO;
    dp_G5M_device_poll(128u, 127u);
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_read_shared_buffer(5u);
        dp_display_text((DPCHAR*)"\r\nDEBUG_INFO:\r\n");
        dp_display_array(g5_shared_buf,80u,HEX);
        
        
        
        
        global_uint1 = ((g5_shared_buf[61] << 8u) | (g5_shared_buf[60]));
        dp_display_text((DPCHAR*)"\r\nCycle Count: ");
        dp_display_value(global_uint1,DEC);
        
        if (g5_shared_buf[36] == 1u)
        {
            dp_display_text((DPCHAR*)"\r\nProgramming mode: JTAG");
        }
        else if (g5_shared_buf[36] == 2u)
        {
            dp_display_text((DPCHAR*)"\r\nProgramming mode: IAP");
        }
        else if (g5_shared_buf[36] == 3u)
        {
            dp_display_text((DPCHAR*)"\r\nProgramming mode: SPI-Slave");
        }
        
        if (((g5_shared_buf[32] &0x3fu) != 0) && ((g5_shared_buf[32] &0x3fu) != 0x3fu))
        {
            dp_display_text((DPCHAR*)"\r\nAlgorithm version: ");
            dp_display_value(g5_shared_buf[32] & 0x3fu, DEC);
        }
        
    }
    
    return;
}

void dp_G5M_read_tvs_monitor(void)
{
    opcode = G5M_TVS_MONITOR;
    IRSCAN_in();
    DRSCAN_in(0u, G5M_FRAME_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    dp_delay(G5M_STANDARD_DELAY);
    opcode = G5M_TVS_MONITOR;
    dp_G5M_device_poll(128u, 127u);
    if (error_code == DPE_SUCCESS)
    {
        dp_display_text((DPCHAR*)"\r\nTVS_MONITOR: ");
        dp_display_array(g5_poll_buf,G5M_FRAME_BYTE_LENGTH,HEX);
    }
    
    return;
}


void dp_G5M_read_fsn(void)
{
    opcode = G5M_READ_FSN;
    IRSCAN_in();
    DRSCAN_in(0u, G5M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    opcode = G5M_READ_FSN;
    dp_G5M_device_poll(129u, 128u);
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\n=====================================================================");
    dp_display_text((DPCHAR*)"\r\nDSN: ");
    dp_display_array(g5_poll_buf, 16u, HEX);
    dp_display_text((DPCHAR*)"\r\n=====================================================================");
    #endif
    
    return;
}
#endif

/* Checking device ID function.  ID is already read in dpalg.c */
void dp_check_G5_device_ID (void)
{
    /* DataIndex is a variable used for loading the array data but not used now.  
    * Therefore, it can be used to store the Data file ID for */
    DataIndex = dp_get_bytes(Header_ID,G5M_ID_OFFSET,G5M_ID_BYTE_LENGTH);
    
    global_ulong1 = dp_get_bytes(Header_ID,G5M_ID_MASK_OFFSET,4U);
    
    device_ID &= global_ulong1;
    DataIndex &= global_ulong1;
    
    
    /* Identifying target device and setting its parms */
    
    if ( (DataIndex & 0xfff) == MICROSEMI_ID)
    {
        if (device_ID == DataIndex )
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nActID = ");
            dp_display_value(device_ID,HEX);
            dp_display_text((DPCHAR*)" ExpID = ");
            dp_display_value(DataIndex,HEX);
            dp_display_text((DPCHAR*)"\r\nDevice Rev = ");
            dp_display_value(device_rev,HEX);
            #endif
            device_family = (DPUCHAR) dp_get_bytes(Header_ID,G5M_DEVICE_FAMILY_OFFSET,G5M_DEVICE_FAMILY_BYTE_LENGTH);
        }
        else
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)" ExpID = ");
            dp_display_value(DataIndex,HEX);
            #endif
            error_code = DPE_IDCODE_ERROR;
        }
    }
    else
    {
        error_code = DPE_IDCODE_ERROR;
    }
    
    return;
}

/* Check if system controller is ready to enter programming mode */
void dp_G5M_device_poll(DPUCHAR bits_to_shift, DPUCHAR Busy_bit)
{
    for ( g5_poll_index = 0U; g5_poll_index <= G5M_MAX_CONTROLLER_POLL; g5_poll_index++ )
    {
        IRSCAN_in();
        DRSCAN_out(bits_to_shift, (DPUCHAR*)DPNULL, g5_poll_buf);
        dp_delay(G5M_STANDARD_DELAY);
        if ( ((g5_poll_buf[Busy_bit/8] & (1 << (Busy_bit % 8))) == 0x0u))
        {
            break;
        }
    }
    if(g5_poll_index > G5M_MAX_CONTROLLER_POLL)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nDevice polling failed: ");
        dp_display_array(g5_poll_buf,16,HEX);
        #endif
        error_code = DPE_POLL_ERROR;
    }
    
    return;
}

void dp_G5M_device_shift_and_poll(DPUCHAR bits_to_shift, DPUCHAR Busy_bit,DPUCHAR Variable_ID,DPULONG start_bit_index)
{
    for ( g5_poll_index = 0U; g5_poll_index <= G5M_MAX_CONTROLLER_POLL; g5_poll_index++ )
    {
        IRSCAN_in();
        dp_get_and_DRSCAN_in_out(Variable_ID, bits_to_shift, start_bit_index, g5_poll_buf);
        //DRSCAN_out(bits_to_shift, (DPUCHAR*)DPNULL, g5_poll_buf);
        dp_delay(G5M_STANDARD_DELAY);
        if ( ((g5_poll_buf[Busy_bit/8] & (1 << (Busy_bit % 8))) == 0x0u))
        {
            break;
        }
    }
    if(g5_poll_index > G5M_MAX_CONTROLLER_POLL)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nDevice polling failed.");
        #endif
        error_code = DPE_POLL_ERROR;
    }
    
    return;
}

void dp_G5M_read_shared_buffer(DPUCHAR ucNumOfBlocks)
{
    
    dp_flush_global_buf1();
    for (global_uchar1 = 0u; global_uchar1 < ucNumOfBlocks; global_uchar1++)
    {
        global_buf1[0] = (global_uchar1 << 1u);
        opcode = G5M_READ_BUFFER;
        IRSCAN_in();
        DRSCAN_in(0u, G5M_FRAME_STATUS_BIT_LENGTH, global_buf1);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
        dp_delay(G5M_STANDARD_DELAY);
        opcode = G5M_READ_BUFFER;
        dp_G5M_device_poll(129u, 128u);
        for (global_uchar2 = 0;global_uchar2 < 16u; global_uchar2++)
        {
            g5_shared_buf[global_uchar1*16u + global_uchar2] = g5_poll_buf[global_uchar2];
        }
    }
    
    
    return;
}

void dp_G5M_poll_device_ready(void)
{
    opcode = G5M_ISC_NOOP;
    for ( g5_poll_index = 0U; g5_poll_index <= G5M_MAX_CONTROLLER_POLL; g5_poll_index++ )
    {
        IRSCAN_in();
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
        dp_delay(G5M_STANDARD_DELAY);
        DRSCAN_out(8u, (DPUCHAR*)DPNULL, g5_poll_buf);
        
        if ((g5_poll_buf[0] & 0x80u) == 0x0u)
        {
            break;
        }
    }
    if(g5_poll_index > G5M_MAX_CONTROLLER_POLL)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nDevice polling failed.");
        #endif
        error_code = DPE_POLL_ERROR;
    }
    
    return;
}

void dp_G5M_set_pgm_mode(void)
{
    opcode = G5M_MODE;
    IRSCAN_in();
    DRSCAN_in(0u, G5M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    dp_delay(G5M_STANDARD_DELAY);
    opcode = G5M_MODE;
    dp_G5M_device_poll(8u, 7u);
    
    return;
}

/****************************************************************************
* Purpose: Loads the BSR regsiter with data specified in the data file.  
*   If BSR_SAMPLE is enabled, the data is not loaded.  Instead, the last known
*   State of the IOs is maintained by stepping through DRCapture JTAG state.
****************************************************************************/
void dp_G5M_load_bsr(void)
{
    dp_G5M_check_core_status();
    
    global_uint1 = (DPUINT) dp_get_bytes(G5M_Header_ID,G5M_NUMOFBSRBITS_OFFSET,G5M_NUMOFBSRBITS_BYTE_LENGTH);
    opcode = ISC_SAMPLE;
    IRSCAN_in();
    
    #ifdef BSR_SAMPLE
    /* Capturing the last known state of the IOs is only valid if the core
    was programmed.  Otherwise, load the BSR with what is in the data file. */
    if ((global_uchar1 & 0x4u) != 0x4u)
    {
        dp_get_bytes(G5M_BsrPattern_ID,0u,1u);
        if (return_bytes)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nWarning: FPGA array is not programmed. Loading BSR register...");
            #endif
            dp_get_and_DRSCAN_in(G5M_BsrPattern_ID, global_uint1, 0u);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
        }
    }
    else 
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nMaintaining last known IO states...");
        #endif
        goto_jtag_state(JTAG_CAPTURE_DR,0u);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    }
    #else
    dp_get_bytes(G5M_BsrPattern_ID,0u,1u);
    if (return_bytes)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nLoading BSR...");
        #endif
        dp_get_and_DRSCAN_in(G5M_BsrPattern_ID, global_uint1, 0u);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    }
    #endif
    
    return;
}

void dp_G5M_perform_isc_enable(void)
{
    
    g5_pgmmode_flag = TRUE;
    dp_flush_global_buf1();
    global_buf1[0] |= (G5M_ALGO_VERSION & 0x3fu);
    global_buf1[2] |= (G5M_DIRECTC_VERSION & 0x3fu) << 1u;
    global_buf1[2] |= (DIRECTC_PROGRAMMING & 0x7u) << 7u;
    global_buf1[3] |= (DIRECTC_PROGRAMMING & 0x7u) >> 1u;
    global_buf1[3] |= (JTAG_PROGRAMMING_PROTOCOL & 0x7u) << 2u;
    
    opcode = G5M_ISC_ENABLE;
    IRSCAN_in();
    DRSCAN_in(0u, ISC_STATUS_REGISTER_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    dp_delay(G5M_STANDARD_DELAY);
    
    opcode = G5M_ISC_ENABLE;
    dp_G5M_device_poll(32u, 31u);
    
    
    if ( (error_code != DPE_SUCCESS) || ((g5_poll_buf[0] & 0x1u) == 1u) )
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to enter programming mode.");
        #endif
        error_code = DPE_INIT_FAILURE;
    }
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nISC_ENABLE_RESULT: ");
    dp_display_array(g5_poll_buf,4u,HEX);
    
    /* Display CRCERR */
    global_uchar1 = g5_poll_buf[0] & 0x1u;
    dp_display_text((DPCHAR*)"\r\nCRCERR: ");
    dp_display_value(global_uchar1,HEX);
    #endif
    
    
    return;
}
/* Enter programming mode */
void dp_G5M_initialize(void)
{
    dp_G5M_poll_device_ready();
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_query_security();
        if ((error_code == DPE_SUCCESS) && ((g5_shared_buf[7] & G5M_UL_USER_KEY1) == G5M_UL_USER_KEY1) )
        {
            dp_G5M_unlock_upk1();
        }
        if ((error_code == DPE_SUCCESS) && ((g5_shared_buf[7] & G5M_UL_USER_KEY2) == G5M_UL_USER_KEY2) )
        {
            dp_G5M_unlock_upk2();
        }
        if (error_code == DPE_SUCCESS)
        {
            dp_G5M_load_bsr();
            if (error_code == DPE_SUCCESS)
            {
                dp_G5M_perform_isc_enable();
            }
        }
    }
    
    return;
}


/* Function is used to exit programming mode */
void dp_G5M_exit(void)
{
    if (g5_pgmmode_flag == TRUE)
    {
        opcode = G5M_ISC_DISABLE;
        IRSCAN_in();
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
        dp_delay(G5M_STANDARD_DELAY);
        
        opcode = G5M_ISC_DISABLE;
        dp_G5M_device_poll(32u, 31u);
        #ifdef ENABLE_DISPLAY
        if (error_code != DPE_SUCCESS)
        {
            dp_display_text((DPCHAR*)"\r\nFailed to disable programming mode.");
        }
        #endif
    }
    #ifdef ENABLE_DISPLAY
    dp_G5M_read_fsn();
    #endif
    goto_jtag_state(JTAG_TEST_LOGIC_RESET,5u);
    return;
}

void dp_G5M_set_mode(void)
{
    opcode = G5M_FRAME_INIT;
    IRSCAN_in();
    DRSCAN_in(0u, G5M_STATUS_REGISTER_BIT_LENGTH, &g5_pgmmode);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);	
    dp_delay(G5M_STANDARD_DELAY);
    dp_G5M_device_poll(8u, 7u);
    #ifdef ENABLE_DISPLAY
    if (error_code != DPE_SUCCESS)
    {
        dp_display_text((DPCHAR*)"r\nFailed to set programming mode.");
    }
    #endif
    
    return;
}


void dp_G5M_process_data(DPUCHAR BlockID)
{
    DataIndex = 0u;  
    /* Global unit1 is used to hold the number of components */
    /* Loop through the number of components */
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\n");
    #endif
    
    
    for (; global_uint2 <= global_uint1; global_uint2++)
    {
        /* get the number of blocks */
        /* Global ulong1 is used to hold the number of blocks within the components */
        global_ulong1 = dp_get_bytes(G5M_NUMBER_OF_BLOCKS_ID,(DPULONG)(((global_uint2 - 1u) * 22u) / 8u),4u);
        global_ulong1 >>= ((global_uint2 - 1U)* 22u) % 8u;
        global_ulong1 &= 0x3FFFFFu;
        
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nProcessing component ");
        dp_display_value(global_uint2,DEC);
        dp_display_text((DPCHAR*)". Please wait...");
        #endif
        
        opcode = G5M_FRAME_DATA;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(BlockID, G5M_FRAME_BIT_LENGTH, DataIndex);
        
        
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
            
            goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
            dp_delay(G5M_STANDARD_DELAY);
            
            opcode = G5M_FRAME_DATA;
            if (global_ulong2 == global_ulong1)
            {
                dp_G5M_device_poll(128u, 127u);
            }
            else
            {
                dp_G5M_device_shift_and_poll(128u, 127u, BlockID, DataIndex + G5M_FRAME_BIT_LENGTH);
            }
            if ( ((error_code != DPE_SUCCESS) || ((g5_poll_buf[0] & 0x18u) != 0u)) )
            {
                dp_G5M_get_data_status();
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\ncomponentNo: ");
                dp_display_value(global_uint2, DEC);
                dp_display_text((DPCHAR*)"\r\nblockNo: ");
                dp_display_value(global_ulong2, DEC);
                dp_display_text((DPCHAR*)"\r\nDATA_STATUS_RESULT: ");
                dp_display_array(g5_poll_buf,4u,HEX);
                dp_display_text((DPCHAR*)"\r\nERRORCODE: ");
                dp_display_value((g5_poll_buf[0]>>3u) & 0x1fu,HEX);
                dp_display_text((DPCHAR*)"\r\nAUTHERRCODE: ");
                dp_display_value(g5_poll_buf[1],HEX);
                #endif
                error_code = DPE_PROCESS_DATA_ERROR;
                global_uint2 = global_uint1;
                break;
            }
            
            DataIndex += G5M_FRAME_BIT_LENGTH;
        }
    }
    
    return;
}

void dp_G5M_get_data_status(void)
{
    opcode = G5M_FRAME_STATUS;
    IRSCAN_in();
    DRSCAN_in(0u, G5M_DATA_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    dp_delay(G5M_STANDARD_DELAY);
    
    opcode = G5M_FRAME_STATUS;
    dp_G5M_device_poll(G5M_DATA_STATUS_REGISTER_BIT_LENGTH, G5M_DATA_STATUS_REGISTER_BIT_LENGTH-1);
    
    return;
}

void dp_G5M_read_security(void)
{
    dp_G5M_query_security();
    if (error_code == DPE_SUCCESS)
    {
        dp_G5M_unlock_dpk();
        if (error_code == DPE_SUCCESS)
        {
            dp_G5M_query_security();
            if (error_code == DPE_SUCCESS)
            {
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nWarning: Security cannot be read even after unlocking debug pass key.");	
                #endif
            }
        }
    }
    
    return;
}
void dp_G5M_query_security(void)
{
    opcode = G5M_QUERY_SECURITY;
    IRSCAN_in();
    DRSCAN_in(0u, G5M_SECURITY_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    opcode = G5M_QUERY_SECURITY;
    dp_G5M_device_poll(16u, 15u);
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to query security information.");	
        #endif
    }
    else
    {
        dp_G5M_read_shared_buffer(1u);
        
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\n--- Security locks and configuration settings ---\r\n");	
        dp_display_array(g5_shared_buf,16u,HEX);
        #endif
    }
    return;
}

void dp_G5M_unlock_dpk(void)
{
    dp_get_data(G5M_DPK_ID,0u);
    if (return_bytes == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nWarning: DPK data is missing.");
        #endif
    }
    else
    {
        dp_G5M_load_dpk();
        if (error_code == DPE_SUCCESS)
        {
            opcode = G5M_UNLOCK_DEBUG_PASSCODE;
            IRSCAN_in();
            DRSCAN_in(0u, G5M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
            dp_delay(G5M_STANDARD_DELAY);
        }
        dp_G5M_device_poll(8u, 7u);
        if ((error_code != DPE_SUCCESS) || ((g5_poll_buf[0] & 0x3u) != 0x1u) )
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to unlock debug pass key.");
            #endif
            error_code = DPE_MATCH_ERROR;
        }
        else
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nDebug security (DPK) is unlocked.");
            #endif
        }
    }
    return;
}


void dp_G5M_unlock_upk1(void)
{
    dp_get_data(G5M_UPK1_ID,0u);
    if (return_bytes == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nWarning: UPK1 data is missing.");
        #endif
    }
    else
    {
        dp_G5M_load_upk1();
        if (error_code == DPE_SUCCESS)
        {
            opcode = G5M_UNLOCK_USER_PASSCODE;
            IRSCAN_in();
            DRSCAN_in(0u, G5M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
            dp_delay(G5M_STANDARD_DELAY);
        }
        dp_G5M_device_poll(8u, 7u);
        if ((error_code != DPE_SUCCESS) || ((g5_poll_buf[0] & 0x3u) != 0x1u) )
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to unlock user pass key 1.");
            #endif
            error_code = DPE_MATCH_ERROR;
        }
        else
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nUser security (DPK1) is unlocked.");
            #endif
        }
    }
    return;
}

void dp_G5M_unlock_upk2(void)
{
    dp_get_data(G5M_UPK2_ID,0u);
    if (return_bytes == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nWarning: UPK2 data is missing.");
        #endif
    }
    else
    {
        dp_G5M_load_upk2();
        if (error_code == DPE_SUCCESS)
        {
            opcode = G5M_UNLOCK_VENDOR_PASSCODE;
            IRSCAN_in();
            DRSCAN_in(0u, G5M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
            dp_delay(G5M_STANDARD_DELAY);
        }
        dp_G5M_device_poll(8u, 7u);
        if ((error_code != DPE_SUCCESS) || ((g5_poll_buf[0] & 0x3u) != 0x1u) )
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to unlock user pass key 2.");
            #endif
            error_code = DPE_MATCH_ERROR;
        }
        else
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nUser security (DPK2) is unlocked.");
            #endif
        }
    }
    return;
}


void dp_G5M_load_dpk(void)
{
    opcode = G5M_KEYLO;
    IRSCAN_in();
    dp_get_and_DRSCAN_in(G5M_DPK_ID, G5M_FRAME_BIT_LENGTH, 0u);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    dp_delay(G5M_STANDARD_DELAY);
    dp_G5M_device_poll(128u, 127u);
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to load keylo. \r\nkeylo_result: ");
        dp_display_array(g5_poll_buf,G5M_FRAME_BYTE_LENGTH,HEX);
        #endif
        error_code = DPE_MATCH_ERROR;
    }
    else
    {
        opcode = G5M_KEYHI;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(G5M_DPK_ID, G5M_FRAME_BIT_LENGTH, G5M_FRAME_BIT_LENGTH);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
        dp_delay(G5M_STANDARD_DELAY);
        dp_G5M_device_poll(128u, 127u);
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to load keyhi. \r\nkeyhi_result: ");
            dp_display_array(g5_poll_buf,G5M_FRAME_BYTE_LENGTH,HEX);
            #endif
            error_code = DPE_MATCH_ERROR;
        }
    }
    
    return;
}

void dp_G5M_load_upk1(void)
{
    opcode = G5M_KEYLO;
    IRSCAN_in();
    dp_get_and_DRSCAN_in(G5M_UPK1_ID, G5M_FRAME_BIT_LENGTH, 0u);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    dp_delay(G5M_STANDARD_DELAY);
    dp_G5M_device_poll(128u, 127u);
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to load keylo. \r\nkeylo_result: ");
        dp_display_array(g5_poll_buf,G5M_FRAME_BYTE_LENGTH,HEX);
        #endif
        error_code = DPE_MATCH_ERROR;
    }
    else
    {
        opcode = G5M_KEYHI;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(G5M_UPK1_ID, G5M_FRAME_BIT_LENGTH, G5M_FRAME_BIT_LENGTH);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
        dp_delay(G5M_STANDARD_DELAY);
        dp_G5M_device_poll(128u, 127u);
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to load keyhi. \r\nkeyhi_result: ");
            dp_display_array(g5_poll_buf,G5M_FRAME_BYTE_LENGTH,HEX);
            #endif
            error_code = DPE_MATCH_ERROR;
        }
    }
    
    return;
}

void dp_G5M_load_upk2(void)
{
    opcode = G5M_KEYLO;
    IRSCAN_in();
    dp_get_and_DRSCAN_in(G5M_UPK2_ID, G5M_FRAME_BIT_LENGTH, 0u);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
    dp_delay(G5M_STANDARD_DELAY);
    dp_G5M_device_poll(128u, 127u);
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to load keylo. \r\nkeylo_result: ");
        dp_display_array(g5_poll_buf,G5M_FRAME_BYTE_LENGTH,HEX);
        #endif
        error_code = DPE_MATCH_ERROR;
    }
    else
    {
        opcode = G5M_KEYHI;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(G5M_UPK2_ID, G5M_FRAME_BIT_LENGTH, G5M_FRAME_BIT_LENGTH);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G5M_STANDARD_CYCLES);
        dp_delay(G5M_STANDARD_DELAY);
        dp_G5M_device_poll(128u, 127u);
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to load keyhi. \r\nkeyhi_result: ");
            dp_display_array(g5_poll_buf,G5M_FRAME_BYTE_LENGTH,HEX);
            #endif
            error_code = DPE_MATCH_ERROR;
        }
    }
    
    return;
}
#endif /* ENABLE_G5_SUPPORT */

/* *************** End of File *************** */


