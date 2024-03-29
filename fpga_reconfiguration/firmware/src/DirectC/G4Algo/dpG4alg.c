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
/*  Module:         dpG4alg.c                                               */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#ifdef ENABLE_G4_SUPPORT
#include "dputil.h"
#include "dpcom.h"
#include "dpalg.h"
#include "dpG4alg.h"
#include "dpjtag.h"

DPUCHAR pgmmode;
DPUCHAR pgmmode_flag;
DPUCHAR envm_only_flag;
DPUCHAR sec_ul;
DPUCHAR shared_buf[768]; // Maximum of 768 bytes
DPUCHAR poll_buf[17];
DPULONG poll_index;
DPUCHAR SYSREG_TEMP[4];
DPUCHAR component_type;
DPUCHAR envm_module_id = 0u;
DPUCHAR prep_bitstream = 0u;
DPUINT  certificate_size = 0u;
DPUCHAR certificate_support = 0u;

DPUCHAR security_queried;

/****************************************************************************
* Purpose: main entry function                                              
*  This function needs to be called from the main application function with 
*  the approppriate action code set to intiate the desired action.
****************************************************************************/
DPUCHAR dp_top_g4 (void) 
{
    dp_init_vars();
    dp_init_G4_vars();
    goto_jtag_state(JTAG_TEST_LOGIC_RESET,0u);
    dp_check_G4_action();
    if (error_code == DPE_SUCCESS)
    {
        dp_perform_G4_action();
    }
    
    return error_code;
}

void dp_init_G4_vars(void)
{
    envm_only_flag = FALSE;
    pgmmode = 0u;
    pgmmode_flag = FALSE;
    security_queried = FALSE;
    sec_ul = 0u;
    return;
}

void dp_check_G4_action(void)
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
    (Action_code == DP_VERIFY_DIGEST_ACTION_CODE) ||
    (Action_code == DP_READ_DEVICE_CERTIFICATE_ACTION_CODE)
    ))
    {
        error_code = DPE_ACTION_NOT_SUPPORTED;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nInvalid action.");
        #endif
    }
    return;
}

void dp_perform_G4_action (void)
{
    #ifdef ENABLE_DISPLAY
    dp_G4M_display_bitstream_digest();
    dp_G4M_read_certificate();
    #endif
    if (error_code == DPE_SUCCESS)
    {  
        #ifdef ENABLE_DISPLAY
        if (Action_code == DP_READ_IDCODE_ACTION_CODE)
        {
            dp_read_idcode_action();
            Action_done = TRUE;
        }
        else if (Action_code == DP_DEVICE_INFO_ACTION_CODE)
        {
            dp_G4M_device_info_action();
            Action_done = TRUE;
        }
        else if(Action_code == DP_READ_DEVICE_CERTIFICATE_ACTION_CODE)
        {
            dp_display_text((DPCHAR*)"\r\nReading device certificate...");
            dp_G4M_read_device_certificate_action();
            Action_done = TRUE;
        }            
        #endif
        if (Action_done == FALSE)
        {
            dp_check_image_crc();
            if (error_code == DPE_SUCCESS)
            {
                dp_check_G4_device_ID();
                if (error_code == DPE_SUCCESS)
                {
                    switch (Action_code) 
                    {
                        case DP_ERASE_ACTION_CODE: 
                        dp_G4M_erase_action();
                        break;         
                        case DP_PROGRAM_ACTION_CODE: 
                        dp_G4M_program_action();
                        break;
                        case DP_VERIFY_ACTION_CODE: 
                        dp_G4M_verify_action();
                        break;
                        case DP_ENC_DATA_AUTHENTICATION_ACTION_CODE: 
                        dp_G4M_enc_data_authentication_action();
                        break;
                        case DP_VERIFY_DIGEST_ACTION_CODE: 
                        dp_G4M_verify_digest_action();
                        break;
                    }
                }
            }
        }
        dp_G4M_exit();
    }
    return;
}


void dp_G4M_erase_action(void)
{
    #ifdef  ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nErasing FPGA Array...");
    #endif
    dp_G4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        dp_G4M_prepare_bitstream();
        pgmmode = 0x1u;
        dp_set_mode();
        
        if (error_code == DPE_SUCCESS)
        {
            
            /* Global unit1 is used to hold the number of components */
            global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_NUMOFCOMPONENT_OFFSET,G4M_NUMOFCOMPONENT_BYTE_LENGTH);
            global_uint2 = global_uint1 - ((DPUINT)dp_get_bytes(Header_ID,G4M_ERASEDATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH) - 1u);
            
            dp_G4M_process_data(G4M_erasedatastream_ID);
            
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_ERASE_ERROR;
            }
            
        }
    }
    return;
}

void dp_G4M_program_action(void)
{
    #ifdef ENABLE_DISPLAY  
    dp_display_text((DPCHAR*)"\r\nProgramming FPGA Array...");
    #endif
    dp_G4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        dp_G4M_prepare_bitstream();
    }
    
    
    if ((error_code == DPE_SUCCESS) && (sec_ul & 0x2u))
    {
        pgmmode = 0x1u;
        dp_set_mode();
        if (error_code == DPE_SUCCESS)
        {
            global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_NUMOFCOMPONENT_OFFSET,G4M_NUMOFCOMPONENT_BYTE_LENGTH);
            global_uint2 = global_uint1 - ((DPUINT)dp_get_bytes(Header_ID,G4M_ERASEDATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH) - 1u);
            dp_G4M_process_data(G4M_erasedatastream_ID);
            
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_ERASE_ERROR;
            }
        }
    }
    if (error_code == DPE_SUCCESS)
    {
        /* Global unit1 is used to hold the number of components */
        global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_DATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH);
        global_uint2 = 1u;
        dp_G4M_setup_eNVM(G4M_datastream_ID);
        
        if (error_code == DPE_SUCCESS)
        {
            pgmmode = 0x1u;
            dp_set_mode();
            if (error_code == DPE_SUCCESS)
            {
                global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_DATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH);
                global_uint2 = 1u;
                dp_G4M_process_data(G4M_datastream_ID);
                if(error_code != DPE_SUCCESS)
                {
                    error_code = DPE_CORE_PROGRAM_ERROR;
                }
                else
                {
                    dp_G4M_post_setup_eNVM();
                }
            }
        }
    }
    
    
    return;
}

void dp_G4M_verify_action(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nVerifying FPGA Array...");  
    #endif
    dp_G4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        dp_G4M_prepare_bitstream();
    }
    
    
    global_uint2 = 1u;
    global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_DATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH);
    dp_G4M_setup_eNVM(G4M_datastream_ID);
    
    if (error_code == DPE_SUCCESS)
    {
        pgmmode = 0x2u;
        dp_set_mode();
        
        if (error_code == DPE_SUCCESS)
        {
            /* Global unit1 is used to hold the number of components */
            if ((DPUINT)dp_get_bytes(Header_ID,G4M_VERIFYDATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH) != 0u)
            {
                /* Both global_unit1 and global_unit2 have DATASIZE, ENVMDATASIZE and ENVMVERIFYDATASIZE; */
                global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_DATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH) +
                (DPUINT)dp_get_bytes(Header_ID,G4M_ENVMDATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH) +
                (DPUINT)dp_get_bytes(Header_ID,G4M_ENVMVERIFYDATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH);
                global_uint2 = global_uint1 + 1u;
                global_uint1 += (DPUINT)dp_get_bytes(Header_ID,G4M_VERIFYDATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH);
                dp_G4M_process_data(G4M_VerifyDataStream_ID);
            }
            else
            {
                global_uint2 = 1u;
                global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_DATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH);
                dp_G4M_process_data(G4M_datastream_ID);
            }
            
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_CORE_VERIFY_ERROR;
            }
            else
            {
                dp_G4M_post_setup_eNVM();
            }
        }
    }
    
    return;
}

void dp_G4M_enc_data_authentication_action(void)
{
    #ifdef ENABLE_DISPLAY    
    dp_display_text((DPCHAR*)"\r\nEncrypted data authentication...");  
    #endif
    dp_G4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        pgmmode = 0x0u;
        dp_set_mode();
        
        if (error_code == DPE_SUCCESS)
        {
            /* Global unit1 is used to hold the number of components */
            global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_DATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH);
            global_uint2 = 1u;
            
            dp_G4M_process_data(G4M_datastream_ID);
            
            
            if(error_code != DPE_SUCCESS)
            {
                error_code = DPE_AUTHENTICATION_FAILURE;
            }
            
        }
    }
    return;
}

void dp_G4M_verify_digest_action(void)
{
    #ifdef ENABLE_DISPLAY    
    dp_display_text((DPCHAR*)"\r\nVerifying Digest...");
    #endif  
    dp_G4M_initialize();
    if (error_code == DPE_SUCCESS)
    {
        
        dp_flush_global_buf1();
        
        DataIndex = 0u;
        global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_DATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH);
        for (global_uint2 = 1u; global_uint2 <= global_uint1; global_uint2++)
        {
            /* get the number of blocks */
            /* Global ulong1 is used to hold the number of blocks within the components */
            global_ulong1 = dp_get_bytes(G4M_NUMBER_OF_BLOCKS_ID,(DPULONG)(((global_uint2 - 1u) * 22u) / 8u),4u);
            global_ulong1 >>= ((global_uint2 - 1U)* 22u) % 8u;
            global_ulong1 &= 0x3FFFFFu;
            
            component_type = (DPUCHAR) dp_get_bytes(G4M_datastream_ID,COMPONENT_TYPE_IN_HEADER_BYTE+DataIndex/8,1);
            if (component_type == G4M_FPGA)
            {
                global_buf1[0] |= 0x1u;
            }
            else if (component_type == G4M_ENVM)
            {
                /* Component type is no longer needed since we know it is eNVM.  We need to know which module */
                component_type = (DPUCHAR) dp_get_bytes(G4M_datastream_ID,COMPONENT_TYPE_IN_HEADER_BYTE+ENVM_MODULE_ID_IN_HEADER_BYTE+DataIndex/8,1);
                if (component_type == 0u)
                {
                    global_buf1[0] |= 0x2u;
                }
                else if (component_type == 1u)
                {
                    global_buf1[0] |= 0x4u;
                }
            }
            DataIndex += G4M_FRAME_BIT_LENGTH * global_ulong1;
        }
        global_ulong1 = dp_get_bytes(Header_ID,G4M_ID_MASK_OFFSET,4U);
        if ( ( (device_ID & global_ulong1) == M2S090_ID) || ( (device_ID & global_ulong1) == M2S150_ID) )
        {
            global_buf1[0] |= 0xcu;
        }
        global_buf1[1] |= 0x3cu;
        
        opcode = G4M_CHECK_DIGESTS;
        IRSCAN_in();
        DRSCAN_in(0u, G4M_SECURITY_STATUS_REGISTER_BIT_LENGTH, global_buf1);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
        dp_delay(G4M_STANDARD_DELAY);
        
        opcode = G4M_CHECK_DIGESTS;
        dp_G4M_device_poll(16u, 15u);
        if (error_code == DPE_SUCCESS)
        {
            if (( poll_buf[1] & 0x3c0u) == 0x3cu)
            {
                error_code = DPE_VERIFY_DIGEST_ERROR;
                unique_exit_code = 32787;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nDigest request from SPI/JTAG is protected by user pass key 1.");
                dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                dp_display_value(unique_exit_code, HEX);
                #endif
            }
            else if (( poll_buf[1] & 0x40u) == 0x40u)
            {
                error_code = DPE_VERIFY_DIGEST_ERROR;
                unique_exit_code = 32788;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nFailed to verify digest.");
                dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                dp_display_value(unique_exit_code, HEX);
                #endif
            }
            if ((global_buf1[0] & 0x1u) == 0x1u)
            {
                if (( poll_buf[0] & 0x1u) == 0x1u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nFPGA Fabric digest verification: PASS");
                    #endif
                }
                else
                {
                    error_code = DPE_VERIFY_DIGEST_ERROR;
                    unique_exit_code = 32789;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nFPGA Fabric digest verification: FAIL");
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                    dp_display_value(unique_exit_code, HEX);
                    #endif
                }
                if (( poll_buf[0] & 0x80u) == 0x80u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nFabric configuration segment digest verification: PASS");
                    #endif
                }
                else
                {
                    error_code = DPE_VERIFY_DIGEST_ERROR;
                    unique_exit_code = 32796;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nFabric configuration segment digest verification: FAIL");
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                    dp_display_value(unique_exit_code, HEX);
                    #endif
                }
            }
            if ((global_buf1[0] & 0x2u) == 0x2u)
            {
                if (( poll_buf[0] & 0x2u) == 0x2u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\neNVM_0 digest verification: PASS");
                    #endif
                }
                else
                {
                    error_code = DPE_VERIFY_DIGEST_ERROR;
                    unique_exit_code = 32790;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\neNVM_0 digest verification: FAIL");
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                    dp_display_value(unique_exit_code, HEX);
                    #endif
                }
            }
            if ((global_buf1[0] & 0x4u) == 0x4u)
            {
                if (( poll_buf[0] & 0x4u) == 0x4u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\neNVM_1 digest verification: PASS");
                    #endif
                }
                else
                {
                    unique_exit_code = 32791;
                    error_code = DPE_VERIFY_DIGEST_ERROR;                    
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\neNVM_1 digest verification: FAIL");
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                    dp_display_value(unique_exit_code, HEX);
                    #endif
                }
            }
            if ((sec_ul & 0x2u) == 0x2u)
            {
                if (( poll_buf[0] & 0x8u) == 0x8u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nUser security policies segment digest verification: PASS");
                    #endif
                }
                else
                {
                    unique_exit_code = 32792;
                    error_code = DPE_VERIFY_DIGEST_ERROR;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nUser security policies segment digest verification: FAIL");
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                    dp_display_value(unique_exit_code, HEX);
                    #endif
                }
            }
            dp_get_data(G4M_UPK1_ID,0u);
            if (return_bytes != 0u)
            {
                if (( poll_buf[0] & 0x10u) == 0x10u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nUser key set 1 segment digest verification: PASS");
                    #endif
                }
                else
                {
                    unique_exit_code = 32793;
                    error_code = DPE_VERIFY_DIGEST_ERROR;                    
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nUser key set 1 segment digest verification: FAIL");
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                    dp_display_value(unique_exit_code, HEX);
                    #endif
                }
            }
            dp_get_data(G4M_UPK2_ID,0u);
            if (return_bytes != 0u)
            {
                if (( poll_buf[0] & 0x20u) == 0x20u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nUser key set 2 segment digest verification: PASS");
                    #endif
                }
                else
                {
                    unique_exit_code = 32794;
                    error_code = DPE_VERIFY_DIGEST_ERROR;
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nUser key set 2 segment digest verification: FAIL");
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                    dp_display_value(unique_exit_code, HEX);
                    #endif
                }
            }
            if (( poll_buf[0] & 0x40u) == 0x0u)
            {
                unique_exit_code = 32795;
                error_code = DPE_VERIFY_DIGEST_ERROR;
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nFactory row and factory key segment digest verification: FAIL");
                dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                dp_display_value(unique_exit_code, HEX);
                #endif
            }
        }
    }
    return;
}

void dp_G4M_read_device_certificate_action(void)
{
    dp_G4M_read_certificate();
}

void dp_G4M_check_core_status(void)
{
    
    opcode = G4M_ISC_NOOP;
    IRSCAN_out(&global_uchar1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,1u);
    
    if ((global_uchar1 & 0x4u) == 0x4u)
    {
        core_is_enabled = 1u;
    }
    else
    {
        core_is_enabled = 0u;
    }
    
    
    return;
}

void dp_G4M_display_core_status(void)
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
}


#ifdef ENABLE_DISPLAY
void dp_G4M_device_info_action(void)
{
    dp_G4M_check_core_status();
    dp_G4M_display_core_status();
    if (error_code == DPE_SUCCESS)
    {
        dp_G4M_read_design_info();
        if (error_code == DPE_SUCCESS)
        {
            dp_G4M_read_prog_info();
            if (error_code == DPE_SUCCESS)
            {
                dp_G4M_read_bitstream_digest();
                if (error_code == DPE_SUCCESS)
                {
                    dp_G4M_read_fsn();
                    if (error_code == DPE_SUCCESS)
                    {
                        dp_G4M_read_security();		 
                        dp_G4M_dump_security_info();
                    }
                }
            }
        }
    }
    
    return;
}

void dp_G4M_read_design_info(void)
{
    opcode = G4M_READ_DESIGN_INFO;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    opcode = G4M_READ_DESIGN_INFO;
    dp_G4M_device_poll(8u, 7u);
    if (error_code == DPE_SUCCESS)
    {
        dp_read_shared_buffer(3u);
        if (error_code == DPE_SUCCESS)
        {
            dp_display_text((DPCHAR*)"\r\nDesign Name: ");
            
            
            for (global_uchar1 = 2u; global_uchar1 < 32u; global_uchar1++)
            {
                dp_display_value(shared_buf[global_uchar1],CHR);
            }
            
            dp_display_text((DPCHAR*)"\r\nChecksum: ");
            dp_display_array(shared_buf,2u,HEX);
            dp_display_text((DPCHAR*)"\r\nDesign Info: ");
            dp_display_array(shared_buf,34u,HEX);
            dp_display_text((DPCHAR*)"\r\nDESIGNVER: ");
            dp_display_array(&shared_buf[32],2u,HEX);
            dp_display_text((DPCHAR*)"\r\nBACKLEVEL: ");
            dp_display_array(&shared_buf[34],2u,HEX);
        }
    }
    
    return;
}

void dp_G4M_read_prog_info(void)
{
    opcode = G4M_READ_PROG_INFO;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    
    opcode = G4M_READ_PROG_INFO;
    dp_G4M_device_poll(128u, 127u);
    if (error_code == DPE_SUCCESS)
    {
        dp_display_text((DPCHAR*)"\r\nPROG_INFO: ");
        dp_display_array(poll_buf,16u,HEX);
        
        dp_display_text((DPCHAR*)"\r\nCYCLE COUNT: ");
        global_uint1 = poll_buf[1]*256+poll_buf[0];
        if ((global_uint1 == 0) || (global_uint1 == 0xffff))
        {
            dp_display_text((DPCHAR*)" Not available");
        }
        else
        {
            dp_display_value(global_uint1, DEC);
        }
        
        if ((poll_buf[6] & 0x1) == 0u)
        {
            dp_display_text((DPCHAR*)"\r\nVCC was programmed at 1.2V");
        }
        else
        {
            dp_display_text((DPCHAR*)"\r\nVCC was programmed at 1.0v");
        }
        if  ( ((poll_buf[8] & 0x3f) != 0u) && ((poll_buf[8] & 0x3f) != 0x3fu) )
        {
            dp_display_text((DPCHAR*)"\r\nAlgorithm Version: ");
            dp_display_value((poll_buf[8] & 0x3f),DEC);
        }
        
        
        global_uchar1 = ((poll_buf[8] >> 6) | (poll_buf[9] << 2)) & 0xfu;
        dp_display_text((DPCHAR*)"\r\nProgrammer code: ");
        dp_display_value(global_uchar1, DEC);
        
        global_uchar1 = ((poll_buf[10] >> 1)) & 0x3fu;
        dp_display_text((DPCHAR*)"\r\nSoftware version code: ");
        dp_display_value(global_uchar1, DEC);
        
        global_uchar1 = ((poll_buf[10] >> 7) | (poll_buf[11] << 1)) & 0x7u;
        dp_display_text((DPCHAR*)"\r\nProgramming Software code: ");
        dp_display_value(global_uchar1, DEC);
        
        global_uchar1 = ((poll_buf[11] >> 2)) & 0x7u;
        dp_display_text((DPCHAR*)"\r\nProgramming Interface Protocol code: ");
        dp_display_value(global_uchar1, DEC);
        
        global_uchar1 = ((poll_buf[11] >> 5)) & 0x7u;
        dp_display_text((DPCHAR*)"\r\nProgramming File Type code: ");
        dp_display_value(global_uchar1, DEC);
        
    }
    
    return;
}

void dp_G4M_read_fsn(void)
{
    opcode = G4M_READ_FSN;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    opcode = G4M_READ_FSN;
    dp_G4M_device_poll(129u, 128u);
    if ((error_code != DPE_SUCCESS) && (unique_exit_code == DPE_SUCCESS))
    {
        unique_exit_code = 32769;
        dp_display_text((DPCHAR*)"\r\nFailed to read DSN.\r\nERROR_CODE: 0x");
        dp_display_value(unique_exit_code, HEX);
    }
    else
    {
        dp_display_text((DPCHAR*)"\r\n=====================================================================");
        dp_display_text((DPCHAR*)"\r\nDSN: ");
        dp_display_array(poll_buf, 16u, HEX);
        dp_display_text((DPCHAR*)"\r\n=====================================================================");
    }
    return;
}

void dp_G4M_read_bitstream_digest(void)
{
    opcode = G4M_READ_DIGESTS;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    opcode = G4M_READ_DIGESTS;
    dp_G4M_device_poll(8u, 7u);
    if (error_code == DPE_SUCCESS)
    {
        dp_read_shared_buffer(6u);
        if (error_code == DPE_SUCCESS)
        {
            dp_display_text((DPCHAR*)"\r\nBitstream Fabric Digest: ");
            dp_display_array(shared_buf,32u,HEX);
            dp_display_text((DPCHAR*)"\r\nBitstream eNVM0 Digest: ");
            dp_display_array(&shared_buf[32],32u,HEX);
            dp_display_text((DPCHAR*)"\r\nBitstream eNVM1 Digest: ");
            dp_display_array(&shared_buf[64],32u,HEX);
        }
    }
    return;
}

#endif

/* Checking device ID function.  ID is already read in dpalg.c */
void dp_check_G4_device_ID (void)
{
    /* DataIndex is a variable used for loading the array data but not used now.  
    * Therefore, it can be used to store the Data file ID for */
    DataIndex = dp_get_bytes(Header_ID,G4M_ID_OFFSET,G4M_ID_BYTE_LENGTH);
    
    
    global_ulong1 = dp_get_bytes(Header_ID,G4M_ID_MASK_OFFSET,4U);
    
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
            
            device_family = (DPUCHAR) dp_get_bytes(Header_ID,G4M_DEVICE_FAMILY_OFFSET,G4M_DEVICE_FAMILY_BYTE_LENGTH);
            certificate_size = NEW_FORMAT_CERTIFICATE_SIZE;
            if ((device_ID == M2S050_ID) && (device_rev < 4u))
                certificate_size = OLD_FORMAT_CERTIFICATE_SIZE;
            if ((device_ID != M2S050_ID) && (device_ID != M2S060_ID) && (device_rev < 1u))
                certificate_size = OLD_FORMAT_CERTIFICATE_SIZE;
        }
        else
        {
            error_code = DPE_IDCODE_ERROR;
            unique_exit_code = 32772;
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)" ExpID = ");
            dp_display_value(DataIndex,HEX);
            dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
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
void dp_G4M_device_poll(DPUCHAR bits_to_shift, DPUCHAR Busy_bit)
{
    for ( poll_index = 0U; poll_index <= G4M_MAX_CONTROLLER_POLL; poll_index++ )
    {
        IRSCAN_in();
        DRSCAN_out(bits_to_shift, (DPUCHAR*)DPNULL, poll_buf);
        dp_delay(G4M_STANDARD_DELAY);
        if ( ((poll_buf[Busy_bit/8] & (1 << (Busy_bit % 8))) == 0x0u))
        {
            break;
        }
    }
    if(poll_index > G4M_MAX_CONTROLLER_POLL)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nDevice polling failed.");
        #endif
        error_code = DPE_POLL_ERROR;
    }
    
    return;
}

void dp_G4M_device_shift_and_poll(DPUCHAR bits_to_shift, DPUCHAR Busy_bit,DPUCHAR Variable_ID,DPULONG start_bit_index)
{
    for ( poll_index = 0U; poll_index <= G4M_MAX_CONTROLLER_POLL; poll_index++ )
    {
        IRSCAN_in();
        dp_get_and_DRSCAN_in_out(Variable_ID, bits_to_shift, start_bit_index, poll_buf);
        //DRSCAN_out(bits_to_shift, (DPUCHAR*)DPNULL, poll_buf);
        dp_delay(G4M_STANDARD_DELAY);
        if ( ((poll_buf[Busy_bit/8] & (1 << (Busy_bit % 8))) == 0x0u))
        {
            break;
        }
    }
    if(poll_index > G4M_MAX_CONTROLLER_POLL)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nDevice polling failed.");
        #endif
        error_code = DPE_POLL_ERROR;
    }
    
    return;
}

void dp_read_shared_buffer(DPUCHAR ucNumOfBlocks)
{
    
    dp_flush_global_buf1();
    for (global_uchar1 = 0u; global_uchar1 < ucNumOfBlocks; global_uchar1++)
    {
        global_buf1[0] = (global_uchar1 << 1u);
        opcode = G4M_READ_BUFFER;
        IRSCAN_in();
        DRSCAN_in(0u, G4M_FRAME_STATUS_BIT_LENGTH, global_buf1);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
        dp_delay(G4M_STANDARD_DELAY);
        opcode = G4M_READ_BUFFER;
        dp_G4M_device_poll(129u, 128u);
        for (global_uchar2 = 0;global_uchar2 < 16u; global_uchar2++)
        {
            shared_buf[global_uchar1*16u + global_uchar2] = poll_buf[global_uchar2];
        }
    }
    
    return;
}

void dp_G4M_poll_device_ready(void)
{
    opcode = G4M_ISC_NOOP;
    for ( poll_index = 0U; poll_index <= G4M_MAX_CONTROLLER_POLL; poll_index++ )
    {
        IRSCAN_in();
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
        dp_delay(G4M_STANDARD_DELAY);
        DRSCAN_out(8u, (DPUCHAR*)DPNULL, poll_buf);
        
        if ((poll_buf[0] & 0x80u) == 0x0u)
        {
            break;
        }
    }
    if(poll_index > G4M_MAX_CONTROLLER_POLL)
    {
        error_code = DPE_POLL_ERROR;  
        unique_exit_code = 32818;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nDevice is busy.");
        dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    
    return;
}

void dp_set_pgm_mode(void)
{
    opcode = G4M_MODE;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    opcode = G4M_MODE;
    dp_G4M_device_poll(8u, 7u);
    
    if (error_code != DPE_SUCCESS)
    {
        unique_exit_code = 32770;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to configure device programming.");
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
void dp_G4M_load_bsr(void)
{
    DPUCHAR capture_last_known_io_state = 0;
    DPUINT index;
    DPUCHAR mask;
    DPUCHAR c_mask;
    DPUINT bsr_bits;
    
    dp_G4M_check_core_status();
    
    bsr_bits = (DPUINT) dp_get_bytes(G4M_Header_ID,G4M_NUMOFBSRBITS_OFFSET,G4M_NUMOFBSRBITS_BYTE_LENGTH);
    opcode = ISC_SAMPLE;
    IRSCAN_in();
    
    dp_get_bytes(G4M_BsrPattern_ID,0u,1u);
    if (return_bytes)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nLoading BSR...");
        #endif
        dp_get_and_DRSCAN_in(G4M_BsrPattern_ID, bsr_bits, 0u);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,0u);
    }
    
    
    /* Capturing the last known state of the IOs is only valid if the core
    was programmed.  Otherwise, load the BSR with what is in the data file. */
    if (core_is_enabled == 1)
    {
        for (index = 0; index < (DPUINT)(bsr_bits + 7u ) / 8u; index++)
        {
            if (dp_get_bytes(G4M_BsrPatternMask_ID,index,1u) != 0)
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
                    bsr_buffer[index] = dp_get_bytes(G4M_BsrPattern_ID,index,1u);
                    mask = dp_get_bytes(G4M_BsrPatternMask_ID,index,1u);
                    
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

void dp_G4M_perform_isc_enable(void)
{
    
    pgmmode_flag = TRUE;
    dp_flush_global_buf1();
    global_buf1[0] |= (G4M_ALGO_VERSION & 0x3fu);
    global_buf1[2] |= (G4M_DIRECTC_VERSION & 0x3fu) << 1u;
    global_buf1[2] |= (DIRECTC_PROGRAMMING & 0x7u) << 7u;
    global_buf1[3] |= (DIRECTC_PROGRAMMING & 0x7u) >> 1u;
    global_buf1[3] |= (JTAG_PROGRAMMING_PROTOCOL & 0x7u) << 2u;
    
    opcode = ISC_ENABLE;
    IRSCAN_in();
    DRSCAN_in(0u, ISC_STATUS_REGISTER_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    
    opcode = ISC_ENABLE;
    dp_G4M_device_poll(32u, 31u);
    if ((poll_buf[0] == global_buf1[0]) && (poll_buf[1] == global_buf1[1]) && (poll_buf[2] == global_buf1[2]))
    {
        error_code = DPE_INIT_FAILURE;
        unique_exit_code = 32815;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nJTAG interface is protected by UPK1.");
        dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    else if ( (error_code != DPE_SUCCESS) || 
    ((poll_buf[0] & 0x1u) == 1u) || 
    ( (poll_buf[0] == 0) && (poll_buf[1] == 0) && (poll_buf[2] == 0) && (poll_buf[3] == 0)))
    {
        error_code = DPE_INIT_FAILURE;
        unique_exit_code = 32771;
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to enter programming mode.");
        dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
        dp_display_value(unique_exit_code, HEX);
        #endif
    }
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nISC_ENABLE_RESULT: ");
    dp_display_array(poll_buf,4u,HEX);
    
    /* Display CRCERR */
    global_uchar1 = poll_buf[0] & 0x1u;
    dp_display_text((DPCHAR*)"\r\nCRCERR: ");
    dp_display_value(global_uchar1,HEX);
    
    /* Display EDCERR */
    global_uchar1 = (poll_buf[0] & 0x2u) >> 1u;
    dp_display_text((DPCHAR*)"\r\nEDCERR: ");
    dp_display_value(global_uchar1,HEX);
    
    if (error_code == DPE_SUCCESS)
    {
        /* Display TEMPRANGE */
        global_uchar1 = (poll_buf[0] >> 2) & 0x7u;
        dp_display_text((DPCHAR*)"\r\nTEMPRANGE:");
        dp_display_value(global_uchar1,HEX);
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
        
        
        /* Display VPPRANGE */
        /*
        global_uchar1 = (poll_buf[0] >> 5) & 0x7u;
        dp_display_text((DPCHAR*)"\r\nVPPRANGE:");
        dp_display_value(global_uchar1,HEX);
        if (global_uchar1 == 0u)
        {
            dp_display_text((DPCHAR*)"\r\nVPPRANGE: LOW");
        }
        else if (global_uchar1 == 1u)
        {
            dp_display_text((DPCHAR*)"\r\nVPPRANGE: NOMINAL");
        }
        if (global_uchar1 == 2u)
        {
            dp_display_text((DPCHAR*)"\r\nVPPRANGE: HIGH");
        }
        */
        /* Display TEMP */
        /*
        dp_display_text((DPCHAR*)"\r\nTEMP:");
        dp_display_value(poll_buf[1],HEX);
        */
        /* Display VPP */
        /*
        dp_display_text((DPCHAR*)"\r\nVPP:");
        dp_display_value(poll_buf[2],HEX);
        */
    }
    #endif
    
    
    return;
}

/* Enter programming mode */
void dp_G4M_initialize(void)
{
    dp_G4M_poll_device_ready();
    
    if (error_code == DPE_SUCCESS)
    {
        dp_set_pgm_mode();
        if (error_code == DPE_SUCCESS)
        {
            dp_G4M_read_security();
            if ((error_code == DPE_SUCCESS) && (sec_ul & 0x2))
            {
                dp_G4M_unlock_upk1();
                if (error_code == DPE_SUCCESS)
                {
                    dp_G4M_unlock_upk2();
                }
            }
        }
        dp_G4M_load_bsr();
        if (error_code == DPE_SUCCESS)
        {
            dp_G4M_check_core_status();
            if (core_is_enabled == 0)
            {
                dp_G4M_set_dpc();
            }
            if (error_code == DPE_SUCCESS)
            {
                dp_G4M_perform_isc_enable();
            }
            if (core_is_enabled == 0)
            {
                dp_G4M_reset_dpc();
            }
        }
    }
    return;
}

void dp_G4M_set_dpc(void)
{
    DPUCHAR dpc_buf[8] = {0x40u, 0x40u, 0x02u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u};
    opcode = G4M_DPC;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_DPC_REGISTER_BIT_LENGTH, dpc_buf);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    
    opcode = G4M_DPC;
    dp_G4M_device_poll(64u, 63u);
}

void dp_G4M_reset_dpc(void)
{
    DPUCHAR dpc_buf[8] = {0x40u, 0x48u, 0x02u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u};
    opcode = G4M_DPC;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_DPC_REGISTER_BIT_LENGTH, dpc_buf);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    
    opcode = G4M_DPC;
    dp_G4M_device_poll(64u, 63u);
}

/* Function is used to exit programming mode */
void dp_G4M_exit(void)
{
    if (pgmmode_flag == TRUE)
    {
        opcode = ISC_DISABLE;
        IRSCAN_in();
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
        dp_delay(G4M_STANDARD_DELAY);
        
        opcode = ISC_DISABLE;
        dp_G4M_device_poll(32u, 31u);
        #ifdef ENABLE_DISPLAY
        if ((error_code != DPE_SUCCESS) && (unique_exit_code == DPE_SUCCESS))
        {
            dp_display_text((DPCHAR*)"\r\nFailed to disable programming mode.");
        }
        #endif
    }
    
    #ifdef ENABLE_DISPLAY
    dp_G4M_read_fsn();
    #endif
    
    opcode = G4M_EXTEST2;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_EXTEST2_DELAY);
    
    goto_jtag_state(JTAG_TEST_LOGIC_RESET,0u);
    return;
}

void dp_set_mode(void)
{
    opcode = G4M_FRAME_INIT;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_STATUS_REGISTER_BIT_LENGTH, &pgmmode);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);	
    dp_delay(G4M_STANDARD_DELAY);
    dp_G4M_device_poll(8u, 7u);
    #ifdef ENABLE_DISPLAY
    if (error_code != DPE_SUCCESS)
    {
        dp_display_text((DPCHAR*)"r\nFailed to set programming mode.");
    }
    #endif
    
    return;
}

void dp_G4M_setup_eNVM(DPUCHAR BlockID)
{
    envm_only_flag = FALSE;
    
    DataIndex = 0u;
    for (; global_uint2 <= global_uint1; global_uint2++)
    {
        /* get the number of blocks */
        /* Global ulong1 is used to hold the number of blocks within the components */
        global_ulong1 = dp_get_bytes(G4M_NUMBER_OF_BLOCKS_ID,(DPULONG)(((global_uint2 - 1u) * 22u) / 8u),4u);
        global_ulong1 >>= ((global_uint2 - 1U)* 22u) % 8u;
        global_ulong1 &= 0x3FFFFFu;
        
        /* Determine component types in bitstream */
        global_uchar1 = (DPUCHAR) dp_get_bytes(BlockID,COMPONENT_TYPE_IN_HEADER_BYTE + DataIndex/8u,1u);
        if ( (global_uchar1 == G4M_FPGA) || (global_uchar1 == G4M_KEYS) )
        {
            envm_only_flag = FALSE;
            break;
        }
        else if(global_uchar1 == G4M_ENVM)
        {
            envm_only_flag = TRUE;
        }
        DataIndex += (G4M_FRAME_BIT_LENGTH * global_ulong1);
    }
    
    
    if (envm_only_flag == TRUE)
    {
        dp_MSS_ADDR_CONFIG();
        if (error_code == DPE_SUCCESS)
        {
            dp_MSS_RD_DATA_SETUP();
            if (error_code == DPE_SUCCESS)
            {
                dp_MSS_ADDR_CONFIG();
                if (error_code == DPE_SUCCESS)
                {
                    dp_MSS_WR_DATA_SETUP();
                }
            }
        }
    }
    return;
}

void dp_G4M_post_setup_eNVM(void)
{
    
    if (envm_only_flag == TRUE)
    {
        dp_MSS_ADDR_CONFIG();
        if (error_code == DPE_SUCCESS)
        {
            dp_MSS_WR_DATA();
        }
    }
    return;
}

void dp_MSS_ADDR_CONFIG(void)
{
    dp_flush_global_buf1();
    
    global_buf1[0] = 0x0cu;
    global_buf1[1] = 0x80u;
    global_buf1[2] = 0x03u;
    global_buf1[3] = 0x40u;
    global_buf1[4] = 0x02u;
    global_buf1[5] = 0x00u;
    global_buf1[6] = 0x00u;
    global_buf1[7] = 0x00u;
    
    opcode = G4M_MSSADDR;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_MSSADDR_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    
    
    opcode = G4M_MSSADDR;
    dp_G4M_device_poll(64u, 63u);
    
    return;
}
void dp_MSS_RD_DATA_SETUP(void)
{
    
    dp_flush_global_buf1();
    global_buf1[0] = 0x04u;
    
    opcode = G4M_MSSRD;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_MSSRD_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    
    
    opcode = G4M_MSSRD;
    dp_G4M_device_poll(16u, 15u);
    #ifdef ENABLE_DISPLAY
    if ( (poll_buf[0] & 0x2u) == 1)
    {
        dp_display_text((DPCHAR*)"\r\nMSS_RD_DATA_SETUP: msserr");
    }
    if ( (poll_buf[0] & 0x1u) == 1)
    {
        dp_display_text((DPCHAR*)"\r\nMSS_RD_DATA_SETUP: secerr");
    }
    #endif
    dp_read_shared_buffer(1u);
    
    for (global_uchar1 = 0u; global_uchar1 < 4u; global_uchar1++)
    {
        SYSREG_TEMP[global_uchar1] = shared_buf[global_uchar1];
    }
    
    return;
}

void dp_MSS_WR_DATA_SETUP(void)
{
    
    dp_flush_global_buf1();
    for(global_uchar1 = 0u; global_uchar1 < 4u; global_uchar1++)
    {
        global_buf1[global_uchar1] = SYSREG_TEMP[global_uchar1];
    }
    global_buf1[0] |= 0xe0u;
    global_buf1[1] |= 0x1fu;
    
    opcode = G4M_MSSWR;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_MSSWR_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    
    
    opcode = G4M_MSSWR;
    dp_G4M_device_poll(32u, 31u);
    #ifdef ENABLE_DISPLAY
    if ( (poll_buf[0] & 0x2u) == 1)
    {
        dp_display_text((DPCHAR*)"\r\nMSS_WR_DATA_SETUP: msserr");
    }
    if ( (poll_buf[0] & 0x1u) == 1)
    {
        dp_display_text((DPCHAR*)"\r\nMSS_WR_DATA_SETUP: secerr");
    }
    #endif
    
    return;
}

void dp_MSS_WR_DATA(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nRestore user FREQRNG setting ");
    dp_display_array(SYSREG_TEMP,4u,HEX);
    dp_display_text((DPCHAR*)"\r\n");
    #endif
    
    opcode = G4M_MSSWR;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_MSSWR_BIT_LENGTH, SYSREG_TEMP);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    
    opcode = G4M_MSSWR;
    dp_G4M_device_poll(32u, 31u);
    #ifdef ENABLE_DISPLAY
    if ( (poll_buf[0] & 0x2u) == 1)
    {
        dp_display_text((DPCHAR*)"\r\nMSS_WR_DATA: msserr");
    }
    if ( (poll_buf[0] & 0x1u) == 1)
    {
        dp_display_text((DPCHAR*)"\r\nMSS_WR_DATA: secerr");
    }
    #endif
    
    
    return;
}
/* global_uint2 is iCurComponentNo
global_uint1 is iCurNumOfComponents 
*/
void dp_G4M_prepare_bitstream(void)
{
    prep_bitstream = 1u;
    pgmmode = 0x2u;
    dp_set_mode();
    
    if ((error_code == DPE_SUCCESS) && (sec_ul & 0x2u))
    {
        if ((DPUINT)dp_get_bytes(Header_ID,G4M_VERIFYDATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH) != 0u)
        {
            /* Both global_unit1 and global_unit2 have DATASIZE, ENVMDATASIZE and ENVMVERIFYDATASIZE; */
            global_uint2 = (DPUINT)dp_get_bytes(Header_ID,G4M_DATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH) +
            (DPUINT)dp_get_bytes(Header_ID,G4M_ENVMDATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH) +
            (DPUINT)dp_get_bytes(Header_ID,G4M_ENVMVERIFYDATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH) + 1u;
            global_uint1 = global_uint2 + 1u;
            dp_G4M_process_predata(G4M_VerifyDataStream_ID);
        }
        else
        {
            global_uint2 = 1u;
            global_uint1 = 2u;
            dp_G4M_process_predata(G4M_datastream_ID);
        }
    }
    else
    {
        global_uint2 = 1u;
        global_uint1 = 2u;
        dp_G4M_process_predata(G4M_datastream_ID);
    }
    prep_bitstream = 0u;
    return;
}

void dp_G4M_process_predata(DPUCHAR BlockID)
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
        if (global_uint2 != global_uint1)
        {
            global_ulong1 = dp_get_bytes(G4M_NUMBER_OF_BLOCKS_ID,(DPULONG)(((global_uint2 - 1u) * 22u) / 8u),4u);
            global_ulong1 >>= ((global_uint2 - 1U)* 22u) % 8u;
            global_ulong1 &= 0x3FFFFFu;
        }
        else
        {
            global_ulong1 = 91u;
        }
        
        
        opcode = G4M_FRAME_DATA;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(BlockID, G4M_FRAME_BIT_LENGTH, DataIndex);
        
        for (global_ulong2 = 1u; global_ulong2 <= global_ulong1; global_ulong2++)
        {
            goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
            dp_delay(G4M_STANDARD_DELAY);
            
            opcode = G4M_FRAME_DATA;
            if (global_ulong2 == global_ulong1)
            {
                dp_G4M_device_poll(128u, 127u);
            }
            else
            {
                dp_G4M_device_shift_and_poll(128u, 127u, BlockID, DataIndex + G4M_FRAME_BIT_LENGTH);
            }
            if (error_code != DPE_SUCCESS)
            {
                global_uint2 = global_uint1;
                break;
            }
            else if ((poll_buf[0] & 0x18u) != 0u)
            {
                dp_G4M_get_data_status();
                if (error_code != DPE_SUCCESS)
                {
                    global_uint2 = global_uint1;
                    break;
                }
                else if ((poll_buf[0] & 0x4u) != 0u)
                {
                    global_uint2 = global_uint1;
                    break;
                }
            }
            DataIndex += G4M_FRAME_BIT_LENGTH;
        }
    }
    
    return;
}


void dp_G4M_process_data(DPUCHAR BlockID)
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
        global_ulong1 = dp_get_bytes(G4M_NUMBER_OF_BLOCKS_ID,(DPULONG)(((global_uint2 - 1u) * 22u) / 8u),4u);
        global_ulong1 >>= ((global_uint2 - 1U)* 22u) % 8u;
        global_ulong1 &= 0x3FFFFFu;
        
        if (BlockID == G4M_datastream_ID)
        {
            component_type = (DPUCHAR) dp_get_bytes(G4M_datastream_ID,COMPONENT_TYPE_IN_HEADER_BYTE+DataIndex/8,1);
            
            if (component_type == G4M_ENVM)
            {
                /* Component type is no longer needed since we know it is eNVM.  We need to know which module */
                envm_module_id = (DPUCHAR) dp_get_bytes(G4M_datastream_ID,COMPONENT_TYPE_IN_HEADER_BYTE+ENVM_MODULE_ID_IN_HEADER_BYTE+DataIndex/8,1);
            }            
            certificate_support = (DPUCHAR) dp_get_bytes(G4M_datastream_ID,CERTIFICATE_SUPPORT_BYTE_OFFSET+DataIndex/8,1) & 0x2u;
        }
        
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nProcessing component ");
        dp_display_value(global_uint2,DEC);
        dp_display_text((DPCHAR*)"\r\nNumber of blocks = ");
        dp_display_value(global_ulong1,DEC);
        dp_display_text((DPCHAR*)". Please wait...");
        
        #endif
        
        opcode = G4M_FRAME_DATA;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(BlockID, G4M_FRAME_BIT_LENGTH, DataIndex);
        
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
            
            goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
            dp_delay(G4M_STANDARD_DELAY);
            
            opcode = G4M_FRAME_DATA;
            if (global_ulong2 == global_ulong1)
            {
                dp_G4M_device_poll(128u, 127u);
            }
            else
            {
                dp_G4M_device_shift_and_poll(128u, 127u, BlockID, DataIndex + G4M_FRAME_BIT_LENGTH);
            }
            if (error_code != DPE_SUCCESS)
            {
                error_code = DPE_PROCESS_DATA_ERROR;
                if (Action_code == DP_PROGRAM_ACTION_CODE)
                    unique_exit_code = 32824;
                else if (Action_code == DP_VERIFY_ACTION_CODE)
                    unique_exit_code = 32822;
                else if (Action_code == DP_ERASE_ACTION_CODE)
                    unique_exit_code = 32820;
                else if (Action_code ==DP_ENC_DATA_AUTHENTICATION_ACTION_CODE)
                    unique_exit_code = 32818;
                
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nInstruction timed out.");
                dp_display_text((DPCHAR*)"\r\ncomponentNo: ");
                dp_display_value(global_uint2, DEC);
                dp_display_text((DPCHAR*)"\r\nblockNo: ");
                dp_display_value(global_ulong2, DEC);
                dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
                dp_display_value(unique_exit_code, HEX);
                #endif
                global_uint2 = global_uint1;
                break;
            }
            else if ((poll_buf[0] & 0x18u) != 0u)
            {
                #ifdef ENABLE_DISPLAY
                if (component_type == G4M_BITS)
                    dp_display_text((DPCHAR*)"\r\ncomponent: BITS");
                else if (component_type == G4M_FPGA)
                    dp_display_text((DPCHAR*)"\r\ncomponent: FPGA");
                else if (component_type == G4M_KEYS)
                    dp_display_text((DPCHAR*)"\r\ncomponent: Security");
                else if (component_type == G4M_ENVM)
                    dp_display_text((DPCHAR*)"\r\ncomponent: eNVM");
                else if (component_type == G4M_EOB)
                    dp_display_text((DPCHAR*)"\r\ncomponent: EOB");
                
                dp_display_text((DPCHAR*)"\r\ncomponentNo: ");
                dp_display_value(global_uint2, DEC);
                dp_display_text((DPCHAR*)"\r\nblockNo: ");
                dp_display_value(global_ulong2, DEC);
                dp_display_text((DPCHAR*)"\r\nFRAME_DATA_RESULT: ");
                dp_display_array(poll_buf,2u,HEX);
                #endif              
                
                dp_G4M_get_data_status();
                
                if (error_code != DPE_SUCCESS)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nData status instruction timed out.");
                    #endif
                }
                else if ((poll_buf[0] & 0x4u) != 0u)
                {
                    #ifdef ENABLE_DISPLAY
                    dp_display_text((DPCHAR*)"\r\nDATA_STATUS_RESULT: ");
                    dp_display_array(poll_buf,4u,HEX);
                    dp_display_text((DPCHAR*)"\r\nERRORCODE: ");
                    dp_display_value((poll_buf[0]>>3u) & 0x1fu,HEX);
                    dp_display_text((DPCHAR*)"\r\nAUTHERRCODE: ");
                    dp_display_value(poll_buf[1],HEX);
                    
                    if ((poll_buf[1] == 1u) || (poll_buf[1] == 2u) || (poll_buf[1] == 4u) || (poll_buf[1] == 8u))
                    {
                        unique_exit_code = 32799;
                        dp_display_text((DPCHAR*)"\r\nBitstream or data is corrupted or noisy.");
                    }
                    else if(poll_buf[1] == 3u)
                    {
                        unique_exit_code = 32801;
                        dp_display_text((DPCHAR*)"\r\nInvalid/Corrupted encryption key." );
                    }
                    else if(poll_buf[1] == 5u)
                    {
                        unique_exit_code = 32803;
                        dp_display_text((DPCHAR*)"\r\nBack level not satisfied." );
                    }                    
                    else if(poll_buf[1] == 7u)
                    {
                        unique_exit_code = 32805;
                        dp_display_text((DPCHAR*)"\r\nDSN binding mismatch." );
                    }
                    else if(poll_buf[1] == 9u)
                    {
                        unique_exit_code = 32807;
                        dp_display_text((DPCHAR*)"\r\nBitstream and device mismatch." );
                    }
                    else if(poll_buf[1] == 10u)
                    {
                        unique_exit_code = 32809;
                        dp_display_text((DPCHAR*)"\r\nIncorrect DEVICEID." );
                    }
                    else if(poll_buf[1] == 11u)
                    {
                        unique_exit_code = 32811;
                        dp_display_text((DPCHAR*)"\r\nProgramming file is out of date, please regenerate.");
                    }
                    else if(poll_buf[1] == 12u)
                    {
                        unique_exit_code = 32813;
                        dp_display_text((DPCHAR*)"\r\nProgramming file does not support verification.");
                    }
                    else if(poll_buf[1] == 13u)
                    {
                        unique_exit_code = 32816;
                        dp_display_text((DPCHAR*)"\r\nInvalid or inaccessible Device Certificate.");
                    }
                    if ( (poll_buf[0] & 0xf8u) != 0u)
                    {
                        if ((poll_buf[0] & 0xf8u) == 0x28u)// 5
                        {
                            unique_exit_code = 32776;
                            dp_display_text((DPCHAR*)"\r\nFailed to verify eNVM." );
                        }
                        else if ((poll_buf[0] & 0xf8u) == 0x20u)// 4
                        {
                            unique_exit_code = 32773;
                            dp_display_text((DPCHAR*)"\r\nFailed to program eNVM." );
                        }
                        else if ((poll_buf[0] & 0xf8u) == 0x10u)// 2
                        {
                            unique_exit_code = 32797;
                            dp_display_text((DPCHAR*)"\r\nDevice security prevented operation." );
                        } 
                        else if ( ( (poll_buf[0] & 0xf8u) == 0x8u) && prep_bitstream == 0u )// 1
                        {
                            if (poll_buf[2] == 248)
                            {
                                dp_display_text((DPCHAR*)"\r\nFailed to verify Fabric Configuration." );                          
                            }
                            else if ( (poll_buf[2] == 249) || (poll_buf[2] == 250) || (poll_buf[2] == 251))
                            {
                                dp_display_text((DPCHAR*)"\r\nFailed to verify Security." );
                            }
                            else if ( (poll_buf[2] != 248) && (poll_buf[2] != 249) && (poll_buf[2] != 250) && (poll_buf[2] != 251))
                            {
                                dp_display_text((DPCHAR*)"\r\nFailed to verify FPGA Array." );
                            }
                            unique_exit_code = 32775;
                        } 
                    }
                    dp_display_text((DPCHAR*)"\r\nERROR_CODE: ");
                    dp_display_value(unique_exit_code, HEX);
                    #endif
                }
                error_code = DPE_PROCESS_DATA_ERROR;
                global_uint2 = global_uint1;
                break;
            }
            DataIndex += G4M_FRAME_BIT_LENGTH;
        }
    }
    #ifdef ENABLE_DISPLAY
    if (Action_code == DP_PROGRAM_ACTION_CODE)
    {
        if ((BlockID == G4M_datastream_ID) && (certificate_support))
        {
            if (component_type == G4M_FPGA)
                dp_display_text((DPCHAR*)"\r\nFabric component bitstream digest: ");
            else if (component_type == G4M_KEYS)
                dp_display_text((DPCHAR*)"\r\nSecurity component bitstream digest: ");
            else if (component_type == G4M_ENVM)
            {
                if (envm_module_id == 0)
                    dp_display_text((DPCHAR*)"\r\neNVM_0 component bitstream digest:"); 
                else
                dp_display_text((DPCHAR*)"\r\neNVM_1 component bitstream digest:"); 
            }
            dp_G4M_report_certificate();
        }
    }
    #endif
    return;
}

void dp_G4M_get_data_status(void)
{
    opcode = G4M_FRAME_STATUS;
    IRSCAN_in();
    DRSCAN_in(0u, ISC_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    
    opcode = G4M_FRAME_STATUS;
    dp_G4M_device_poll(32u, 31u);
    
    return;
}

void dp_G4M_read_security(void)
{
    dp_G4M_query_security();
    if ((error_code == DPE_SUCCESS) && (security_queried == FALSE) )
    {
        dp_G4M_unlock_dpk();
        if (error_code == DPE_SUCCESS)
        {
            dp_G4M_query_security();
            if ((error_code == DPE_SUCCESS) && (security_queried == FALSE) )
            {
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nWarning: Security cannot be read even after unlocking debug pass key.");	
                #endif
            }
        }
    }
    sec_ul = shared_buf[4] >> 2u;
    return;
}
void dp_G4M_query_security(void)
{
    opcode = G4M_QUERY_SECURITY;
    IRSCAN_in();
    DRSCAN_in(0u, G4M_SECURITY_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    opcode = G4M_QUERY_SECURITY;
    dp_G4M_device_poll(16u, 15u);
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to query security information.");	
        #endif
    }
    else
    {
        dp_read_shared_buffer(3u);
        for (poll_index = 0u;poll_index < 40u; poll_index++)
        {
            if (shared_buf[poll_index] != 0u)
            {
                security_queried = TRUE;
                break;
            }
        }
        
    }
    return;
}

void dp_G4M_dump_security_info(void)
{
    #ifdef ENABLE_DISPLAY
    if (security_queried == TRUE)
    {
        dp_display_text((DPCHAR*)"\r\n--- Security locks and configuration settings ---\r\n");	
        dp_display_array(shared_buf,42u,HEX);
    }
    #endif
}

void dp_G4M_unlock_dpk(void)
{
    dp_get_data(G4M_DPK_ID,0u);
    if (return_bytes == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nWarning: DPK data is missing.");
        #endif
    }
    else
    {
        dp_G4M_load_dpk();
        if (error_code == DPE_SUCCESS)
        {
            opcode = G4M_UNLOCK_DEBUG_PASSCODE;
            IRSCAN_in();
            DRSCAN_in(0u, G4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
            dp_delay(G4M_STANDARD_DELAY);
        }
        dp_G4M_device_poll(8u, 7u);
        if ((error_code != DPE_SUCCESS) || ((poll_buf[0] & 0x3u) != 0x1u) )
        {
            error_code = DPE_MATCH_ERROR;
            unique_exit_code = 32786;
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to unlock debug pass key.");
            dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
            dp_display_value(unique_exit_code, HEX);
            #endif
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

void dp_G4M_unlock_upk1(void)
{
    dp_get_data(G4M_UPK1_ID,0u);
    if (return_bytes == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nWarning: UPK1 data is missing.");
        #endif
    }
    else
    {
        dp_G4M_load_upk1();
        if (error_code == DPE_SUCCESS)
        {
            opcode = G4M_UNLOCK_USER_PASSCODE;
            IRSCAN_in();
            DRSCAN_in(0u, G4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
            dp_delay(G4M_STANDARD_DELAY);
        }
        dp_G4M_device_poll(8u, 7u);
        if ((error_code != DPE_SUCCESS) || ((poll_buf[0] & 0x3u) != 0x1u) )
        {
            error_code = DPE_MATCH_ERROR;
            unique_exit_code = 32784;
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to unlock user pass key 1.");
            dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
            dp_display_value(unique_exit_code, HEX);
            #endif
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

void dp_G4M_unlock_upk2(void)
{
    dp_get_data(G4M_UPK2_ID,0u);
    if (return_bytes == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nWarning: UPK2 data is missing.");
        #endif
    }
    else
    {
        dp_G4M_load_upk2();
        if (error_code == DPE_SUCCESS)
        {
            opcode = G4M_UNLOCK_VENDOR_PASSCODE;
            IRSCAN_in();
            DRSCAN_in(0u, G4M_STATUS_REGISTER_BIT_LENGTH, (DPUCHAR*)(DPUCHAR*)DPNULL);
            goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
            dp_delay(G4M_STANDARD_DELAY);
        }
        dp_G4M_device_poll(8u, 7u);
        if ((error_code != DPE_SUCCESS) || ((poll_buf[0] & 0x3u) != 0x1u) )
        {
            error_code = DPE_MATCH_ERROR;
            unique_exit_code = 32785;
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to unlock user pass key 2.");
            dp_display_text((DPCHAR*)"\r\nERROR_CODE: 0x");
            dp_display_value(unique_exit_code, HEX);
            #endif
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

void dp_G4M_load_dpk(void)
{
    opcode = G4M_KEYLO;
    IRSCAN_in();
    dp_get_and_DRSCAN_in(G4M_DPK_ID, G4M_FRAME_BIT_LENGTH, 0u);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    dp_G4M_device_poll(128u, 127u);
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to load keylo. \r\nkeylo_result: ");
        dp_display_array(poll_buf,G4M_FRAME_BYTE_LENGTH,HEX);
        #endif
        error_code = DPE_MATCH_ERROR;
    }
    else
    {
        opcode = G4M_KEYHI;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(G4M_DPK_ID, G4M_FRAME_BIT_LENGTH, G4M_FRAME_BIT_LENGTH);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
        dp_delay(G4M_STANDARD_DELAY);
        dp_G4M_device_poll(128u, 127u);
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to load keyhi. \r\nkeyhi_result: ");
            dp_display_array(poll_buf,G4M_FRAME_BYTE_LENGTH,HEX);
            #endif
            error_code = DPE_MATCH_ERROR;
        }
    }
    
    return;
}

void dp_G4M_load_upk1(void)
{
    opcode = G4M_KEYLO;
    IRSCAN_in();
    dp_get_and_DRSCAN_in(G4M_UPK1_ID, G4M_FRAME_BIT_LENGTH, 0u);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    dp_G4M_device_poll(128u, 127u);
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to load keylo. \r\nkeylo_result: ");
        dp_display_array(poll_buf,G4M_FRAME_BYTE_LENGTH,HEX);
        #endif
        error_code = DPE_MATCH_ERROR;
    }
    else
    {
        opcode = G4M_KEYHI;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(G4M_UPK1_ID, G4M_FRAME_BIT_LENGTH, G4M_FRAME_BIT_LENGTH);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
        dp_delay(G4M_STANDARD_DELAY);
        dp_G4M_device_poll(128u, 127u);
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to load keyhi. \r\nkeyhi_result: ");
            dp_display_array(poll_buf,G4M_FRAME_BYTE_LENGTH,HEX);
            #endif
            error_code = DPE_MATCH_ERROR;
        }
    }
    
    return;
}

void dp_G4M_load_upk2(void)
{
    opcode = G4M_KEYLO;
    IRSCAN_in();
    dp_get_and_DRSCAN_in(G4M_UPK2_ID, G4M_FRAME_BIT_LENGTH, 0u);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    dp_G4M_device_poll(128u, 127u);
    if (error_code != DPE_SUCCESS)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to load keylo. \r\nkeylo_result: ");
        dp_display_array(poll_buf,G4M_FRAME_BYTE_LENGTH,HEX);
        #endif
        error_code = DPE_MATCH_ERROR;
    }
    else
    {
        opcode = G4M_KEYHI;
        IRSCAN_in();
        dp_get_and_DRSCAN_in(G4M_UPK2_ID, G4M_FRAME_BIT_LENGTH, G4M_FRAME_BIT_LENGTH);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
        dp_delay(G4M_STANDARD_DELAY);
        dp_G4M_device_poll(128u, 127u);
        if (error_code != DPE_SUCCESS)
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nFailed to load keyhi. \r\nkeyhi_result: ");
            dp_display_array(poll_buf,G4M_FRAME_BYTE_LENGTH,HEX);
            #endif
            error_code = DPE_MATCH_ERROR;
        }
    }
    
    return;
}

#ifdef ENABLE_DISPLAY
void dp_G4M_read_certificate(void)
{
    opcode = G4M_READ_DEVICE_CERT;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,G4M_STANDARD_CYCLES);
    dp_delay(G4M_STANDARD_DELAY);
    dp_G4M_device_poll(8u, 7u);
    if (error_code != DPE_SUCCESS)
    {
        dp_display_text((DPCHAR*)"\r\nFailed to read certificate. \r\nPoll buffer: ");
        dp_display_array(poll_buf,G4M_STATUS_REGISTER_BIT_LENGTH,HEX);
        error_code = DPE_POLL_ERROR;
    }
    else
    {
        if ((poll_buf[0] & 0x1u) == 0x1u)
        {
            dp_read_shared_buffer(48u);
            if (error_code == DPE_SUCCESS)
            {
                if ((shared_buf[559] & 0x1u) == 0x1u)// Bit 4472
                {
                    dp_display_text((DPCHAR*)"\r\nFamily: SmartFusion2");
                    dp_display_text((DPCHAR*)"\r\nCortex-M3 Enabled (M3_ALLOWED = 1)");
                    
                }
                else
                {
                    dp_display_text((DPCHAR*)"\r\nFamily: Igloo2");
                    dp_display_text((DPCHAR*)"\r\nCortex-M3 Disabled (M3_ALLOWED = 0)");
                }
                dp_display_text((DPCHAR*)"\r\nProduct: ");
                for (global_uchar1 = 195u; global_uchar1 < 228u; global_uchar1++)
                {
                    dp_display_value(shared_buf[global_uchar1],CHR);
                }
                /* Check bits 4312 - 4327 */
                if ((shared_buf[526] & 0xf0 != 0xf0u) &&
                (shared_buf[527] & 0xff != 0xffu) &&
                (shared_buf[528] & 0x0f != 0x0fu)
                )
                {
                    dp_display_text((DPCHAR*)"\r\nGrade: ");  
                    dp_display_array(&shared_buf[527], 16u, HEX);
                }
                if (Action_code == DP_READ_DEVICE_CERTIFICATE_ACTION_CODE)
                {
                    dp_display_text((DPCHAR*)"\r\nDEVICE_CERTIFICATE(LSB->MSB) ");
                    dp_display_array_reverse(shared_buf, 768u, HEX);
                }
            }
        }
    }
    return;
}

void dp_G4M_display_bitstream_digest(void)
{
    
    DataIndex = 0u;
    global_uint1 = (DPUINT)dp_get_bytes(Header_ID,G4M_DATASIZE_OFFSET,G4M_DATASIZE_BYTE_LENGTH);
    for (global_uint2 = 1u; global_uint2 <= global_uint1; global_uint2++)
    {
        /* get the number of blocks */
        /* Global ulong1 is used to hold the number of blocks within the components */
        global_ulong1 = dp_get_bytes(G4M_NUMBER_OF_BLOCKS_ID,(DPULONG)(((global_uint2 - 1u) * 22u) / 8u),4u);
        global_ulong1 >>= ((global_uint2 - 1U)* 22u) % 8u;
        global_ulong1 &= 0x3FFFFFu;
        
        component_type = (DPUCHAR) dp_get_bytes(G4M_datastream_ID,COMPONENT_TYPE_IN_HEADER_BYTE+DataIndex/8,1);
        if (component_type == G4M_BITS)
        {
            DPUCHAR * data_address = (DPUCHAR*)DPNULL;
            data_address = dp_get_data(G4M_datastream_ID, G4M_BSDIGEST_BYTE_OFFSET * 8 + DataIndex);
            dp_display_text((DPCHAR*)"\r\nBITSTREAM_DIGEST = ");
            dp_display_array(data_address, G4M_BSDIGEST_BYTE_SIZE, HEX);
        }
        DataIndex += G4M_FRAME_BIT_LENGTH * global_ulong1;
    }
    
    
    return;
}
#endif


void dp_G4M_report_certificate(void)
{
    int iBlocks;
    iBlocks = certificate_size / 128u;
    if (iBlocks > 0u)
    {
        iBlocks++;
        dp_read_shared_buffer(iBlocks);
    }
    #ifdef ENABLE_DISPLAY    
    dp_display_array(&shared_buf[18], 32u, HEX);
    #endif
}




#endif /* ENABLE_G4_SUPPORT */

/* *************** End of File *************** */

