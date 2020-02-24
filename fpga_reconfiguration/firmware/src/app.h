/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
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

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "definitions.h"                // SYS function prototypes

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
#define NUM_BUTTONS                                (3)
    
    
// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_STATE_INIT=0,

    /* Mount SDCARD device */
    APP_MOUNT_DISK,

    /* Set the current drive */
    APP_SET_CURRENT_DRIVE,

    /* List the ".dat" files on SDCARD */
    APP_LIST_FILES,

    /* Wait user select the ".dat" file to load */
    APP_WAIT_PUSH_BUTTON,

    /* Open the selected file */
    APP_OPEN_FILE,

    /* Copy content of ".dat" file in external SDRAM memory */
    APP_COPY_DAT_IMAGE,

    /* Initialize directC library */
    APP_DIRECTC_INIT,

    /* Select the directC command to execute */
    APP_DIRECTC_SET_CMD,

    /* Execute directC command */
    APP_DIRECTC_TOP,

    /* An app error has occurred */
    APP_ERROR,
} APP_STATES;

// *****************************************************************************
/* Selected button values

  Summary:
    Application enumeration for selected button.

  Description:
    This enumeration defines the selected button values.
*/

typedef enum
{
    APP_SELECT_BUTTON_PB0 = 0,
    APP_SELECT_BUTTON_PB1 = 1,
    APP_SELECT_BUTTON_PB2 = 2,
    APP_SELECT_BUTTON_NONE = 0xFF,
} APP_SELECT_BUTTON;

// *****************************************************************************
/* DirectC command states

  Summary:
    Application enumeration for DirectC command states.

  Description:
    This enumeration defines the DirectC command states.
*/

typedef enum
{
    APP_DIRECTC_STATE_INIT = 0,
    APP_DIRECTC_STATE_PROGRAM = 1,
    APP_DIRECTC_STATE_VERIFY = 2,
} APP_DIRECTC_STATE;

// *****************************************************************************
/* Application LED toggle Data

  Summary:
    Holds LEDs state for toggling.

  Description:
    This structure holds the data for LED toggling.
 */

typedef struct
{
    bool                      ledReadOn;
    uint32_t                  nextToggle;
} APP_LED_READ_TOGGLE;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* SYS_FS File handle */
    SYS_FS_HANDLE               fileHandle;

    /* The application's current state */
    APP_STATES                  state;
    
    /* ".dat" file names associated to the push buttons */
    char                        datFileName[NUM_BUTTONS][300];

    /* User selected button */
    APP_SELECT_BUTTON           selectedButton;

    /* Number of bytes read in opened file */
    int32_t                     nBytesRead;

    /* Total number of bytes read in opened file */
    int32_t                     nSumBytesRead;

    /* File read toggling LED status */
    APP_LED_READ_TOGGLE         ledReadToggle;

    /* Current DirectC State */
    APP_DIRECTC_STATE           directcState;
} APP_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks( void );



#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

