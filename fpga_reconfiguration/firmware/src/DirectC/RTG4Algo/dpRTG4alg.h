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
/*  Module:         dpRTG4alg.h                                             */
/*                                                                          */
/*  Description:    Contains function prototypes.                           */
/*                                                                          */
/* ************************************************************************ */

#ifndef INC_DPRTG4ALG_H
#define INC_DPRTG4ALG_H

/* 
* Data block ID definitions
*/
#define RTG4M_ALGO_VERSION					1u
#define RTG4M_DIRECTC_VERSION				0u

#define RTG4M_Header_ID						0u
#define RTG4M_BsrPattern_ID					3u  
#define RTG4M_BsrPatternMask_ID				4u  
#define RTG4M_NUMBER_OF_BLOCKS_ID			5u
#define RTG4M_datastream_ID					8u
#define RTG4M_erasedatastream_ID			9u

#define RTG4M_DEVICE_FAMILY_OFFSET			36u
#define RTG4M_DEVICE_FAMILY_BYTE_LENGTH		1u
#define RTG4M_ID_OFFSET						37u
#define RTG4M_ID_BYTE_LENGTH				4u
#define RTG4M_ID_MASK_OFFSET				41u
#define RTG4M_ID_MASK_BYTE_LENGTH			4u
#define RTG4M_SILSIG_OFFSET					45u
#define RTG4M_SILSIG_BYTE_LENGTH			4u
#define RTG4M_CHECKSUM_OFFSET				49u
#define RTG4M_CHECKSUM_BYTE_LENGTH			2u
#define RTG4M_NUMOFBSRBITS_OFFSET			51u
#define RTG4M_NUMOFBSRBITS_BYTE_LENGTH		2u
/*
* RTG4Main JTAG instructions
*/
#define RTG4M_ISC_NOOP					0x8fu
#define RTG4M_FRAME_INIT				0xd8u
#define RTG4M_READ_DESIGN_INFO			0xa6u
#define RTG4M_READ_PROG_INFO			0xe7u
#define RTG4M_READ_BUFFER				0xf2u
#define RTG4M_READ_DSN					0xf0u
#define RTG4M_QUERY_SECURITY			0xdau
#define RTG4M_JTAG_RELEASE				0xafu
#define RTG4M_FRAME_DATA				0xeeu
#define RTG4M_FRAME_STATUS				0xc9u
#define RTG4M_VERIFY_DIGEST				0xdcu
#define RTG4M_READ_DIGEST				0xdbu

#define	RTG4M_MAX_CONTROLLER_POLL			1000000u

#define RTG4_ISC_STATUS_REGISTER_BIT_LENGTH	32u
#define RTG4M_STANDARD_CYCLES				3u
#define RTG4M_STANDARD_DELAY				10u
#define RTG4M_RESET_CYCLES					5u
#define RTG4M_RESET_DELAY					1000u

#define RTG4M_STATUS_REGISTER_BIT_LENGTH			64u
#define RTG4M_DESIGN_INFO_REGISTER_BIT_LENGTH		8u
#define RTG4M_FRAME_BIT_LENGTH						128u
#define RTG4M_FRAME_STATUS_BIT_LENGTH				129u

DPUCHAR dp_top_rtg4 (void);
void dp_init_RTG4_vars(void);
void dp_RTG4_exit_avionics_mode(void);
void dp_check_RTG4_action(void);
void dp_perform_RTG4_action (void);

/* Supported Actions */
void dp_RTG4M_device_info_action(void);
void dp_RTG4M_erase_action(void);
void dp_RTG4M_program_action(void);
void dp_RTG4M_verify_action(void);
void dp_RTG4M_check_bitstream_action(void);
void dp_RTG4M_verify_digest_action(void);
//
void dp_check_RTG4_device_ID (void);
void dp_RTG4M_poll_device_ready(void);
void dp_RTG4M_check_core_status(void);
void dp_RTG4M_read_design_info(void);
void dp_RTG4M_read_prog_info(void);
void dp_RTG4M_read_dsn(void);
void dp_RTG4M_query_security(void);
void dp_RTG4M_read_digest(void);
void dp_RTG4M_read_shared_buffer(DPUCHAR ucNumOfBlocks);
void dp_RTG4M_load_bsr(void);
void dp_RTG4M_perform_isc_enable(void);
void dp_RTG4M_process_data(DPUCHAR BlockID);
void dp_RTG4M_get_data_status(void);

/* Initialization functions */
void dp_RTG4M_device_poll(DPUCHAR bits_to_shift, DPUCHAR Busy_bit);
void dp_RTG4M_initialize(void);
void dp_RTG4M_exit(void);
void dp_RTG4M_set_mode(void);


#endif /* INC_DPRTG4ALG_H */

/* *************** End of File *************** */

