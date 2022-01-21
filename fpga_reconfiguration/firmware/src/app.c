/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>

#include "app.h"
#include "definitions.h"                // SYS function prototypes
#include "DirectC/dpuser.h"
#include "DirectC/dputil.h"
#include "DirectC/dpalg.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define SDCARD_MOUNT_NAME              ("/mnt/mydrive")
#define SDCARD_DEV_NAME                ("/dev/mmcblka1")
#define APP_DATA_LEN                   (512)
#define APP_LED_READ_TOGGLE_ITER       (50)

#if(defined(ENABLE_G5_SUPPORT) && defined(ENABLE_RTG4_SUPPORT))
    #error "Current example only handle one device family selected"
#else
    #ifdef ENABLE_G5_SUPPORT
        #define APP_SELECTED_DEVICE_FAMILLY (G5_FAMILY)
    #else
        #ifdef ENABLE_RTG4_SUPPORT
            #define APP_SELECTED_DEVICE_FAMILLY (RTG4_FAMILY)
        #else
            #error "Device Family case not supported in this example"
        #endif
    #endif
#endif

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/
APP_DATA appData;

// *****************************************************************************
/* ".dat" file extension string */
const char dat_extention[5] = {0x2e, 0x64, 0x61, 0x74, };

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    appData.selectedButton = APP_SELECT_BUTTON_NONE;
    memset(&(appData.datFileName[0][0]), 0, sizeof(appData.datFileName[0]));
    memset(&(appData.datFileName[1][0]), 0, sizeof(appData.datFileName[1]));
    memset(&(appData.datFileName[2][0]), 0, sizeof(appData.datFileName[2]));
    LED_READ_Set();
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;

            if (appInitialized)
            {
#ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"=> Mount SD card and list .dat files:\r\n");
#endif
                appData.state = APP_MOUNT_DISK;
            }

            break;
        }
        
        case APP_MOUNT_DISK:
            if(SYS_FS_Mount(SDCARD_DEV_NAME, SDCARD_MOUNT_NAME, FAT, 0, NULL) != 0)
            {
                /* The disk could not be mounted. Try
                 * mounting again until success. */
                appData.state = APP_MOUNT_DISK;
            }
            else
            {
                /* Mount was successful. Unmount the disk, for testing. */
                appData.state = APP_SET_CURRENT_DRIVE;
            }
            break;

        case APP_SET_CURRENT_DRIVE:
            if(SYS_FS_CurrentDriveSet(SDCARD_MOUNT_NAME) == SYS_FS_RES_FAILURE)
            {
                /* Error while setting current drive */
                appData.state = APP_ERROR;
            }
            else
            {
                /* Open a file for reading. */
                appData.state = APP_LIST_FILES;                
            }
            break;
            
        case APP_LIST_FILES:
        {
            SYS_FS_FSTAT stat;

            appData.fileHandle = SYS_FS_DirOpen("/mnt/mydrive/");

            if(appData.fileHandle != SYS_FS_HANDLE_INVALID)
            {
                bool isDirReadEnd = false;
                while(isDirReadEnd == false)
                {
                    if(SYS_FS_DirRead(appData.fileHandle, &stat) == SYS_FS_RES_FAILURE)
                    {
                        // Directory read failed.
                        appData.state = APP_ERROR;
                        isDirReadEnd = true;
                    }
                    else
                    {
                        char *ptr = strrchr(&(stat.fname[0]), 0x2e); // Iso period
                        if(ptr != NULL)
                        {
                            if (strncmp(dat_extention, ptr, 4) == 0 )
                            {
                                ptr = &(stat.fname[0]);
                                if (strncmp("PB0", ptr, 3) == 0 )
                                {
                                    strncpy(&(appData.datFileName[0][0]), &(stat.fname[0]), 300);
#ifdef ENABLE_DISPLAY
                                    dp_display_text((DPCHAR*)"BP0 : ");
                                    dp_display_text((DPCHAR*)&appData.datFileName[0][0]);
                                    dp_display_text((DPCHAR*)"\r\n");
#endif
                                }
                                else if (strncmp("PB1", ptr, 3) == 0 )
                                {
                                    strncpy(&(appData.datFileName[1][0]), &(stat.fname[0]), 300);
#ifdef ENABLE_DISPLAY
                                    dp_display_text((DPCHAR*)"BP1 : ");
                                    dp_display_text((DPCHAR*)&appData.datFileName[1][0]);
                                    dp_display_text((DPCHAR*)"\r\n");
#endif
                                }
                                else if (strncmp("PB2", ptr, 3) == 0 )
                                {
                                    strncpy(&(appData.datFileName[2][0]), &(stat.fname[0]), 300);
#ifdef ENABLE_DISPLAY
                                    dp_display_text((DPCHAR*)"BP2 : ");
                                    dp_display_text((DPCHAR*)&appData.datFileName[2][0]);
                                    dp_display_text((DPCHAR*)"\r\n");
#endif
                                }
                            }
                        }

                        // Directory read succeeded.
                        if ((stat.lfname[0] == '\0') && (stat.fname[0] == '\0'))
                        {
                            // reached the end of the directory.
                            isDirReadEnd = true;
                        }
                    }
                }
            }

            SYS_FS_DirClose(appData.fileHandle);

            appData.state = APP_WAIT_PUSH_BUTTON;
        }
            break;
            
        case APP_WAIT_PUSH_BUTTON:
            if ( (appData.datFileName[0][0] != '\0') && (PB0_Get() == PB0_STATE_PRESSED) )
            {
                appData.selectedButton = APP_SELECT_BUTTON_PB0;
            }
            else if ( (appData.datFileName[1][0] != '\0') && (PB1_Get() == PB1_STATE_PRESSED) )
            {
                appData.selectedButton = APP_SELECT_BUTTON_PB1;
            }
            else if ( (appData.datFileName[2][0] != '\0') && (PB2_Get() == PB2_STATE_PRESSED) )
            {
                appData.selectedButton = APP_SELECT_BUTTON_PB2;
            }
            
            if (appData.selectedButton != APP_SELECT_BUTTON_NONE)
            {
                appData.state = APP_OPEN_FILE;
            }
            break;
            
        case APP_OPEN_FILE:
#ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"Selected .dat File : ");
            dp_display_value(appData.selectedButton,DEC);
            dp_display_text((DPCHAR*)" : ");
            dp_display_text((DPCHAR*)&(appData.datFileName[appData.selectedButton][0]));
            dp_display_text((DPCHAR*)"\r\n");
#endif

            appData.fileHandle = SYS_FS_FileOpen(appData.datFileName[appData.selectedButton], (SYS_FS_FILE_OPEN_READ));

            if(appData.fileHandle != SYS_FS_HANDLE_INVALID)
            {
                /* Copy data */
                image_buffer = (DPUCHAR*)0x64000010;
                appData.nSumBytesRead = 0;
                appData.ledReadToggle.ledReadOn = false;
                appData.state = APP_COPY_DAT_IMAGE;
                appData.selectedButton = APP_SELECT_BUTTON_NONE;
            }
            else
            {
                /* Could not open the file. Error out*/
                appData.state = APP_ERROR;
            }
            break;
            
        case APP_COPY_DAT_IMAGE:
            
            appData.nBytesRead = SYS_FS_FileRead(appData.fileHandle, (void *)(image_buffer+appData.nSumBytesRead), APP_DATA_LEN);
            
            if (appData.nBytesRead == -1)
            {
                /* There was an error while reading the file.
                 * Close the file and error out. */
                SYS_FS_FileClose(appData.fileHandle);
                appData.state = APP_ERROR;
            }
            else
            {
                appData.nSumBytesRead += appData.nBytesRead;
                
                if (appData.ledReadToggle.nextToggle > 0)
                {
                    appData.ledReadToggle.nextToggle--;
                }
                else
                {
                    appData.ledReadToggle.nextToggle = APP_LED_READ_TOGGLE_ITER;
                    if (appData.ledReadToggle.ledReadOn)
                    {
                        LED_READ_Set();
                        appData.ledReadToggle.ledReadOn = false;
                    }
                    else
                    {
                        LED_READ_Clear();
                        appData.ledReadToggle.ledReadOn = true;
                    }                
                }

                if(SYS_FS_FileEOF(appData.fileHandle) == 1)    /* Test for end of file */
                {                                        
                    /* Continue the read and write process, until the end of file is reached */
                    SYS_FS_FileClose(appData.fileHandle);
                    LED_READ_Set();
                    appData.state = APP_DIRECTC_INIT;
                }
            }
            break;

        case APP_DIRECTC_INIT:
        {
            error_code = DPE_SUCCESS;
            
            dp_check_and_get_image_size();
            
            if (error_code != DPE_SUCCESS)
            {
                appData.state = APP_ERROR;
            }
            else
            {
#ifdef ENABLE_DISPLAY
                dp_display_text((DPCHAR*)"\r\nValid Dat file in memory");
#endif
                device_family = APP_SELECTED_DEVICE_FAMILLY;
                appData.directcState = APP_DIRECTC_STATE_INIT;
                appData.state = APP_DIRECTC_SET_CMD;
            }

        }
            break;

        case APP_DIRECTC_SET_CMD:
        {
            LED0_Off();
            LED1_Off();
            LED2_Off();
            LED3_Off();

#ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\ndirectcState = ");
            dp_display_value(appData.directcState,DEC);
#endif
            if (appData.directcState == APP_DIRECTC_STATE_INIT)
            {
                Action_code = DP_REPROGRAM_INFLIGHT_ACTION_CODE;
                appData.directcState = APP_DIRECTC_STATE_PROGRAM;
                appData.state = APP_DIRECTC_TOP;
            }
            else
            {
                appData.state = APP_WAIT_PUSH_BUTTON;
            }
        }
            break;   
            
        case APP_DIRECTC_TOP:
        {
#ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nAction_code = ");
            dp_display_value(Action_code,DEC);
#endif
            dp_top();
#ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"\r\nExit code = ");
            dp_display_value(error_code,DEC);
#endif

            if (error_code == DPE_SUCCESS)
            {
                appData.state = APP_DIRECTC_SET_CMD;
            }
            else
            {
                appData.state = APP_ERROR;
            }
        }
            break;

        case APP_ERROR:
            /* The application comes here when the demo has failed. */
#ifdef ENABLE_DISPLAY
            dp_display_text((DPCHAR*)"APP_ERROR\r\n");
#endif
            LED0_On();
            LED1_On();
            LED2_On();
            LED3_On();
            
            /* Wait until external reset */
            while(1);
            break;

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
