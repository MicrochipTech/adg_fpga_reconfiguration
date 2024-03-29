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
/*  Module:         dpsecurity.c                                            */
/*                                                                          */
/*  Description:    Contains initialization and data checking functions.    */
/*                                                                          */
/* ************************************************************************ */

#include "dpuser.h"
#ifdef ENABLE_G3_SUPPORT

#include "dputil.h"
#include "dpalg.h"
#include "dpG3alg.h"
#include "dpsecurity.h"
#include "dpcom.h"
#include "dpjtag.h"

#ifdef SECURITY_SUPPORT
/*****************************************************************************/
/*  NVM Action Functions                                                     */
/*****************************************************************************/
#ifndef DISABLE_SEC_SPECIFIC_ACTIONS
void dp_erase_security_action(void)
{
    device_security_flags |= IS_ERASE_ONLY;
    device_security_flags |= IS_RESTORE_DESIGN;
    dp_erase_security();
    return;
}

void dp_program_security_action(void)
{
    dp_erase_security();
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & SEC_DAT_SUPPORT_BIT)
        {
            dp_program_security();
        }
    }
    return;
}

/*****************************************************************************/
/*  Programming Support Functions                                            */
/*****************************************************************************/
void dp_erase_security(void)
{
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nErase Security...");
    #endif
    dp_flush_global_buf1();
    global_buf1[0] = UROW_ERASE_BITS_BYTE0;
    global_buf1[1] = UROW_ERASE_BITS_BYTE1;
    global_buf1[2] = UROW_ERASE_BITS_BYTE2;
    
    /* This is for FROM erase.  Need to get which bits are set to erase from the data file. */
    global_buf1[0] |= 0xeu;
    
    dp_exe_erase();
    
    
    return;
}
#endif

void dp_program_security(void)
{
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nProgramming Security...");
    #endif
    dp_write_sec_key();
    if (error_code == DPE_SUCCESS)
    {
        if (dat_support_status & AES_DAT_SUPPORT_BIT)
        {
            dp_write_enc_key();
        }
        if (error_code == DPE_SUCCESS)
        {
            dp_match_security();
            if ((device_security_flags & SEC_KEY_OK) == 0u)
            {
                error_code = DPE_MATCH_ERROR;
            }
            else
            {
                dp_program_ulock();
            }
        }
    }
    
    return;
}


void dp_write_sec_key(void)
{
    opcode = ISC_PROGRAM_UKEY;
    IRSCAN_in();
    dp_get_and_DRSCAN_in(UKEY_ID, UKEY_BIT_LENGTH, 0u);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_UKEY_CYCLES);
    dp_poll_device();
    
    return;
}

void dp_write_enc_key(void)
{
    
    if (device_family & DUAL_KEY_BIT)
    {
        global_uchar1 = 0u;
        dp_set_aes_mode();
    }    
    
    opcode = ISC_PROGRAM_DMK;
    IRSCAN_in();
    dp_get_and_DRSCAN_in(DMK_ID, DMK_BIT_LENGTH, 0u);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_DMK_CYCLES);
    
    dp_poll_device();
    if (error_code == DPE_SUCCESS)
    {
        dp_verify_enc_key();
    }
    return;
}


void dp_program_ulock(void)
{
    DPUCHAR index;
    
    
    dp_flush_global_buf1();
    
    if (device_family & (AFS_BIT | SFS_BIT))
    {
        global_uint1 = ULOCK_AFS_BIT_LENGTH;
        /* Read security configuration from the data file */
        global_ulong2 = dp_get_bytes(ULOCK_ID,0u, 3u);
        /* Set the register content to be loaded into the chip */
        global_buf1[0] = (DPUCHAR) global_ulong2;
        global_buf1[1] = (DPUCHAR) ((global_ulong2 >> 8u) | 0x80u);
        global_buf1[2] = (DPUCHAR) ((global_ulong2 >> 16u) & 0x3Fu);
        global_ulong2 = dp_get_bytes(SILSIG_ID,0u,4u);
        global_buf1[3] = (DPUCHAR) global_ulong2;
        global_buf1[4] = (DPUCHAR) (global_ulong2 >> 8u);
        global_buf1[5] = (DPUCHAR) (global_ulong2 >> 16u);
        global_buf1[6] = (DPUCHAR) (global_ulong2 >> 24u);
    }
    else
    {
        global_uint1 = ULOCK_A3P_AGL_BIT_LENGTH;
        /* Read security configuration from the data file */
        global_ulong2 = dp_get_bytes(ULOCK_ID,0u,2u);
        /* Set the register content to be loaded into the chip */
        global_buf1[0] = (DPUCHAR) (global_ulong2 | 0x8u);
        global_buf1[1] = (DPUCHAR) ((global_ulong2 >> 8u) & 0xF3u);
        global_ulong2 = dp_get_bytes(SILSIG_ID,0u,4u);
        global_buf1[1] |= (DPUCHAR) (global_ulong2 << 4u);
        global_buf1[2] = (DPUCHAR) (global_ulong2 >> 4u);
        global_buf1[3] = (DPUCHAR) (global_ulong2 >> 12u);
        global_buf1[4] = (DPUCHAR) (global_ulong2 >> 20u);
        global_buf1[5] = (DPUCHAR) (global_ulong2 >> 28u);
    }
    
    opcode = ISC_PROGRAM_SECURITY;
    IRSCAN_in();
    DRSCAN_in(0u, global_uint1, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_SECURITY_CYCLES);
    
    dp_poll_device();
    if (error_code == DPE_SUCCESS)
    {
        opcode = ISC_QUERY_SECURITY;
        IRSCAN_in();
        goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_QUERY_SECURITY_CYCLES);
        
        dp_flush_global_buf2();
        DRSCAN_out(global_uint1, (DPUCHAR*)DPNULL, global_buf2);
        if (device_family & (AFS_BIT | SFS_BIT))
        {    
            for (index = 0u; index <= 6u; index++)
            {
                if (index == 2u)
                {
                    global_uchar2 = 0x3Fu;
                }
                else
                {
                    global_uchar2 = 0xFFu;
                }
                if ((global_buf1[index] & global_uchar2) != (global_buf2[index] & global_uchar2))
                {
                    error_code = DPE_ULOCK_ERROR;
                    break;
                }
            }
        }
        else 
        {    
            for (index = 0u; index <= 5u; index++)
            {
                if (index == 1u)
                {
                    global_uchar2 = 0xF3u;
                }
                else if (index == 5u)
                {
                    global_uchar2 = 0xFu;
                }
                else
                {
                    global_uchar2 = 0xFFu;
                }
                if ((global_buf1[index] & global_uchar2) != (global_buf2[index] & global_uchar2))
                {
                    error_code = DPE_ULOCK_ERROR;
                    break;
                }
            }
        }
    }
    
    
    return;
}
#endif


void dp_match_security(void)
{
    
    opcode = ISC_MATCH_UKEY;
    IRSCAN_in();
    
    dp_get_and_DRSCAN_in(UKEY_ID, UKEY_BIT_LENGTH, 0u);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_MATCH_UKEY_CYCLES);
    dp_delay(ISC_MATCH_UKEY_DELAY);
    
    DRSCAN_out(DMK_BIT_LENGTH, (DPUCHAR*)DPNULL, global_buf1);
    if ((global_buf1[0] & 0x3u) == 0x1u)
    {
        device_security_flags |= SEC_KEY_OK;
    }
    
    if (dat_support_status & SEC_DAT_SUPPORT_BIT)
    {
        
        global_ulong2 = dp_get_bytes(UKEY_ID,0u, 4u);
        global_ulong2 |= dp_get_bytes(UKEY_ID,4u, 4u);
        global_ulong2 |= dp_get_bytes(UKEY_ID,8u, 4u);
        global_ulong2 |= dp_get_bytes(UKEY_ID,12u, 4u);
        
        
        if (global_ulong2 == 0u)
        {
            device_security_flags |= PERM_LOCK_BIT;
        }
    }
    
    
    return;
}

void dp_read_device_security(void)
{
    
    #ifdef ENABLE_DISPLAY
    dp_display_text((DPCHAR*)"\r\nReading Security...");
    #endif
    
    if (device_family & (AFS_BIT | SFS_BIT))
    {
        global_uint1 = ULOCK_AFS_BIT_LENGTH;
    }
    else
    {
        global_uint1 = ULOCK_A3P_AGL_BIT_LENGTH;
    }
    opcode = ISC_QUERY_SECURITY;
    IRSCAN_in();
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_QUERY_SECURITY_CYCLES);
    dp_flush_global_buf2();
    /* reset only the security bit locations */
    device_security_flags &= 0xFF000000;
    DRSCAN_out(global_uint1, (DPUCHAR*)DPNULL, global_buf2);
    if (device_family & (AFS_BIT | SFS_BIT))
    {
        device_security_flags |=   (DPULONG) global_buf2[0] | 
        ((DPULONG) global_buf2[1] << 8) | 
        ((DPULONG) global_buf2[2] << 16) ;
    }
    /* This step is to line up the security flags for AFS and A3P.  This is usedful so that the mask bits are the same for both families */
    else
    {
        device_security_flags |=  (DPULONG) global_buf2[0] << 12 | 
        (DPULONG) (global_buf2[1] & 0xFu) << 20;
    }
    
    
    return;
}
#ifdef SILSIG_SUPPORT
void dp_program_silsig(void)
{
    
    global_ulong1 = dp_get_bytes(SILSIG_ID,0u,4u);
    if (global_ulong1)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nProgramming SILSIG...");
        #endif
        
        if (device_family & (AFS_BIT | SFS_BIT))
        {
            global_uint1 = ULOCK_AFS_BIT_LENGTH;
            /* Set the register content to be loaded into the chip */
            global_buf1[0] = (DPUCHAR) device_security_flags;
            global_buf1[1] = (DPUCHAR) (device_security_flags >> 8u);
            global_buf1[2] = (DPUCHAR) (device_security_flags >> 16u);
            global_buf1[3] = (DPUCHAR) global_ulong1;
            global_buf1[4] = (DPUCHAR) (global_ulong1 >> 8u);
            global_buf1[5] = (DPUCHAR) (global_ulong1 >> 16u);
            global_buf1[6] = (DPUCHAR) (global_ulong1 >> 24u);
        }
        else
        {
            global_uint1 = ULOCK_A3P_AGL_BIT_LENGTH;
            /* Read security configuration from the data file */
            global_buf1[0] = (DPUCHAR) (device_security_flags >> 12);
            global_buf1[1] = (DPUCHAR) (device_security_flags >> 20) & 0xFu;
            global_buf1[1] |= (DPUCHAR) (global_ulong1 << 4);
            global_buf1[2] = (DPUCHAR) (global_ulong1 >> 4);
            global_buf1[3] = (DPUCHAR) (global_ulong1 >> 12);
            global_buf1[4] = (DPUCHAR) (global_ulong1 >> 20);
            global_buf1[5] = (DPUCHAR) (global_ulong1 >> 28);
        }
        
        
        opcode = ISC_PROGRAM_SECURITY;
        IRSCAN_in();
        DRSCAN_in(0u, global_uint1, global_buf1);
        goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_PROGRAM_SECURITY_CYCLES);
        
        dp_poll_device();
        if (error_code != DPE_SUCCESS)
        {
            error_code = DPE_ULOCK_ERROR;
        }
    }
    
    return;
}
#endif

#if (!defined ENABLE_CODE_SPACE_OPTIMIZATION)
void dp_check_dual_key(void)
{
    if (
    ((device_ID & AXXE600X_ID_MASK) == (AXXE600X_ID & AXXE600X_ID_MASK)) || 
    ((device_ID & AXXE1500X_ID_MASK) == (AXXE1500X_ID & AXXE1500X_ID_MASK)) || 
    ((device_ID & AXXE3000X_ID_MASK) == (AXXE3000X_ID & AXXE3000X_ID_MASK)) || 
    ( ( (device_ID & AFS600_ID_MASK) == (AFS600_ID & AFS600_ID_MASK) ) && (device_rev > 1u) ) ||
    ((device_ID & AFS1500_ID_MASK) == (AFS1500_ID & AFS1500_ID_MASK)))
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nChecking for Dual Key Device...");
        #endif
        
        global_uchar1 = 1u;
        dp_set_aes_mode();
        dp_init_aes();
        dp_verify_fc_dmk();
        /* Need to reset to DMK mode */
        
        global_uchar1 = 0u;
        dp_set_aes_mode();
        dp_init_aes();
        
    }
    return;
}

#endif
#ifndef ENABLE_CODE_SPACE_OPTIMIZATION

void dp_verify_id_dmk(void)
{
    if ( 
    ((device_ID & AXX030X_ID_MASK) != (AXX030X_ID & AXX030X_ID_MASK)) && 
    ((device_ID & AGLP030X_ID_MASK) != (AGLP030X_ID & AGLP030X_ID_MASK)) &&
    (device_ID != AXXN010X_ID) && 
    (device_ID != AXXN020X_ID)
    ) 
    {
        if (device_family & DUAL_KEY_BIT)
        {
            
            global_uchar1 = 1u;
            dp_set_aes_mode();
        }
        
        dp_init_aes();
        dp_verify_m7_dmk();
        if(error_code == DPE_SUCCESS)
        {
            dp_verify_m1_dmk();
            if(error_code == DPE_SUCCESS)
            {
                dp_verify_p1_dmk();
                if (error_code == DPE_SUCCESS)
                {
                    if (device_family & DUAL_KEY_BIT)
                    {
                        if ( ((device_security_flags & M7_DEVICE) == 0u) && ((device_security_flags & M1_DEVICE) == 0u) && ((device_security_flags & P1_DEVICE) == 0u))
                        {
                            #ifdef ENABLE_DISPLAY
                            dp_display_text((DPCHAR*)"\r\nError: Core enabled device detected...");
                            #endif
                            error_code = DPE_IDCODE_ERROR;
                        }
                    }
                }
            }
        }
    }
    return;
}

void dp_verify_m7_dmk(void)
{
    global_buf1[0]=M7KDATA0;
    global_buf1[1]=M7KDATA1;
    global_buf1[2]=M7KDATA2;
    global_buf1[3]=M7KDATA3;
    global_buf1[4]=M7KDATA4;
    global_buf1[5]=M7KDATA5;
    global_buf1[6]=M7KDATA6;
    global_buf1[7]=M7KDATA7;
    global_buf1[8]=M7KDATA8;
    global_buf1[9]=M7KDATA9;
    global_buf1[10]=M7KDATA10;
    global_buf1[11]=M7KDATA11;
    global_buf1[12]=M7KDATA12;
    global_buf1[13]=M7KDATA13;
    global_buf1[14]=M7KDATA14;
    global_buf1[15]=M7KDATA15;
    
    opcode = ISC_VERIFY_DMK;
    IRSCAN_in();
    DRSCAN_in(0u, DMK_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_VERIFY_DMK_CYCLES);
    dp_delay(ISC_VERIFY_DMK_DELAY);
    DRSCAN_out(DMK_BIT_LENGTH, (DPUCHAR*)DPNULL, global_buf2);
    
    if ((global_buf2[15] & 0x80u) == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to verify AES Sec...");
        #endif
        error_code = DPE_AES_SEC_ERROR;
    }
    else if (global_buf2[15] & 0x40u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nM7 Device Detected...");
        #endif
        device_security_flags |= M7_DEVICE;
    }
    else 
    {
    }
    if (error_code == DPE_SUCCESS)
    {
        global_uchar1 = (DPUCHAR) dp_get_bytes(Header_ID,M_DEVICE_OFFSET,1u);
        
        if ((device_security_flags & M7_DEVICE) && (global_uchar1 != M7))
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nError: M7 Device Detected. Data file is not compiled for M7 Device...");
            #endif
            error_code = DPE_IDCODE_ERROR;
        }
        else if (((device_security_flags & M7_DEVICE) == 0u) && (global_uchar1 == M7))
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nError: M7 Device is not Detected. Data file is compiled for M7 Device...");
            #endif
            error_code = DPE_IDCODE_ERROR;
        }
        else
        {
        }
    }
    
    return;
}

void dp_verify_m1_dmk(void)
{
    
    global_buf1[0]=M1KDATA0;
    global_buf1[1]=M1KDATA1;
    global_buf1[2]=M1KDATA2;
    global_buf1[3]=M1KDATA3;
    global_buf1[4]=M1KDATA4;
    global_buf1[5]=M1KDATA5;
    global_buf1[6]=M1KDATA6;
    global_buf1[7]=M1KDATA7;
    global_buf1[8]=M1KDATA8;
    global_buf1[9]=M1KDATA9;
    global_buf1[10]=M1KDATA10;
    global_buf1[11]=M1KDATA11;
    global_buf1[12]=M1KDATA12;
    global_buf1[13]=M1KDATA13;
    global_buf1[14]=M1KDATA14;
    global_buf1[15]=M1KDATA15;
    
    opcode = ISC_VERIFY_DMK;
    IRSCAN_in();
    DRSCAN_in(0u, DMK_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_VERIFY_DMK_CYCLES);
    dp_delay(ISC_VERIFY_DMK_DELAY);
    DRSCAN_out(DMK_BIT_LENGTH, (DPUCHAR*)DPNULL, global_buf2);
    if ((global_buf2[15] & 0x80u) == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to verify AES Sec...");
        #endif
        error_code = DPE_AES_SEC_ERROR;
    }
    else if (global_buf2[15] & 0x40u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nM1 Device Detected...");
        #endif
        device_security_flags |= M1_DEVICE;
    }
    else
    {
    }
    if (error_code == DPE_SUCCESS)
    {
        global_uchar1 = (DPUCHAR) dp_get_bytes(Header_ID,M_DEVICE_OFFSET,1u);
        
        if ((device_security_flags & M1_DEVICE) && (global_uchar1 != M1))
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nError: M1 Device Detected. Data file is not compiled for M1 Device...");
            #endif
            error_code = DPE_IDCODE_ERROR;
        }
        else if (((device_security_flags & M1_DEVICE) == 0u) && (global_uchar1 == M1))
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nError: M1 Device is not Detected. Data file is compiled for M1 Device...");
            #endif
            error_code = DPE_IDCODE_ERROR;
        }
        else
        {
        }
    }
    
    return;
}

void dp_verify_p1_dmk(void)
{
    
    global_buf1[0]=P1KDATA0;
    global_buf1[1]=P1KDATA1;
    global_buf1[2]=P1KDATA2;
    global_buf1[3]=P1KDATA3;
    global_buf1[4]=P1KDATA4;
    global_buf1[5]=P1KDATA5;
    global_buf1[6]=P1KDATA6;
    global_buf1[7]=P1KDATA7;
    global_buf1[8]=P1KDATA8;
    global_buf1[9]=P1KDATA9;
    global_buf1[10]=P1KDATA10;
    global_buf1[11]=P1KDATA11;
    global_buf1[12]=P1KDATA12;
    global_buf1[13]=P1KDATA13;
    global_buf1[14]=P1KDATA14;
    global_buf1[15]=P1KDATA15;
    
    opcode = ISC_VERIFY_DMK;
    IRSCAN_in();
    DRSCAN_in(0u, DMK_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_VERIFY_DMK_CYCLES);
    dp_delay(ISC_VERIFY_DMK_DELAY);
    DRSCAN_out(DMK_BIT_LENGTH, (DPUCHAR*)DPNULL, global_buf2);    
    
    if ((global_buf2[15] & 0x80u) == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to verify AES Sec...");
        #endif
        error_code = DPE_AES_SEC_ERROR;
    }
    else if (global_buf2[15] & 0x40u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nP1 Device Detected...");
        #endif
        device_security_flags |= P1_DEVICE;
    }
    else
    {
    }
    
    if (error_code == DPE_SUCCESS)
    {
        global_uchar1 = (DPUCHAR) dp_get_bytes(Header_ID,M_DEVICE_OFFSET,1u);
        
        if ((device_security_flags & P1_DEVICE) && (global_uchar1 != P1))
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nError: P1 Device Detected. Data file is not compiled for P1 Device...");
            #endif
            error_code = DPE_IDCODE_ERROR;
        }
        else if (((device_security_flags & P1_DEVICE) == 0u) && (global_uchar1 == P1))
        {
            #ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nError: P1 Device is not Detected. Data file is compiled for P1 Device...");
            #endif
            error_code = DPE_IDCODE_ERROR;
        }
        else
        {
        }
    }
    return;
}


#endif
#if (!defined ENABLE_CODE_SPACE_OPTIMIZATION)
void dp_verify_fc_dmk(void)
{
    
    global_buf1[0]=FCBYTE0;
    global_buf1[1]=FCBYTE1;
    global_buf1[2]=FCBYTE2;
    global_buf1[3]=FCBYTE3;
    global_buf1[4]=FCBYTE4;
    global_buf1[5]=FCBYTE5;
    global_buf1[6]=FCBYTE6;
    global_buf1[7]=FCBYTE7;
    global_buf1[8]=FCBYTE8;
    global_buf1[9]=FCBYTE9;
    global_buf1[10]=FCBYTE10;
    global_buf1[11]=FCBYTE11;
    global_buf1[12]=FCBYTE12;
    global_buf1[13]=FCBYTE13;
    global_buf1[14]=FCBYTE14;
    global_buf1[15]=FCBYTE15;
    
    opcode = ISC_VERIFY_DMK;
    IRSCAN_in();
    DRSCAN_in(0u, DMK_BIT_LENGTH, global_buf1);
    goto_jtag_state(JTAG_RUN_TEST_IDLE,ISC_VERIFY_DMK_CYCLES);
    dp_delay(ISC_VERIFY_DMK_DELAY);
    DRSCAN_out(DMK_BIT_LENGTH, (DPUCHAR*)DPNULL, global_buf2);
    
    if ((global_buf2[15] & 0x80u) == 0u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nFailed to verify AES Sec...");
        #endif
        error_code = DPE_AES_SEC_ERROR;
    }
    else if (global_buf2[15] & 0x40u)
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nSingle Key Device Detected...");
        #endif
        if (dat_version != V85_DAT)
        {
            if (device_family & DUAL_KEY_BIT)
            {
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nError: Data file is compiled for Dual Key Device...");
                #endif
                error_code = DPE_IDCODE_ERROR;
            }
        }
    }
    else 
    {
        #ifdef ENABLE_DISPLAY
        dp_display_text((DPCHAR*)"\r\nDual Key Device Detected...");
        #endif
        if (dat_version == V85_DAT)
        {
            device_family |= DUAL_KEY_BIT;
            if (dat_support_status & CORE_DAT_ENCRYPTION_BIT)
            {
                AES_mode_value = 2u;
            }
            else 
            {
                AES_mode_value = 1u;
            }
            dat_support_status |= CORE_DAT_ENCRYPTION_BIT;
        }
        else
        {
            if ( (device_family & DUAL_KEY_BIT) == 0u)
            {
                #ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nError: Data file is compiled for Single Key Device...");
                #endif
                error_code = DPE_IDCODE_ERROR;
            }
        }
    }
    
    return;
}
#endif

#endif /* End of ENABLE_G3_SUPPORT */

/* *************** End of File *************** */

