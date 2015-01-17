/*******************************************************************************
* Common audio plugin specifications:  version #1.1 created by zilmar          *
* Revised 2014 by Iconoclast for more compliance, portability and readability. *
*                                                                              *
* All questions or suggestions should go through the EmuTalk plugin forum.     *
* http://www.emutalk.net/forums/showforum.php?f=31                             *
*******************************************************************************/

#ifndef _CONTR_H_INCLUDED__
#define _CONTR_H_INCLUDED__

#include "my_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define PLUGIN_TYPE_CONTROLLER	    4

#ifndef PLUGIN_API_VERSION
#define PLUGIN_API_VERSION      0x0101
#endif

/*
 * emulation of controller accessories
 *
 * Emulation of the rumble pak and transfer pak accessories is not
 * implemented for non-raw data currently.  Work it out within the plugin.
 */
#define PLUGIN_NONE                 1
#define PLUGIN_MEMPAK               2
#define PLUGIN_RUMBLE_PAK           3
#define PLUGIN_TRANSFER_PAK         PLUGIN_TANSFER_PAK
#if (PLUGIN_API_VERSION >= 0x0101)
#define PLUGIN_RAW                  5
#endif

/* original, typo'd macro name from original spec file */
#define PLUGIN_TANSFER_PAK          4

/* probably supposed to delete below comment by zilmar, don't know yet */
/******************************************************************************
* Note about controller plugins:
* The rumble pak needs a function for the force feedback joystick, and
* the transfer pak probably needs a function for the plugin to be able to select
* the GB ROM and EEPROM....  Maybe this should be done by the emulator instead
* of the plugin, but I think it probably should be done by the plugin.  I will
* see about adding these functions in the next release of these specifications.
*******************************************************************************/

/*
 * Try declaring CONTROL_INFO structure instance as:
 * `CONTROL_INFO CONTROL_INFO_NAME;'
 *
 * Doing the traditional `CONTROL_INFO SI_info' declaration requires
 * accessing the RCP information in a less portable way.
 */
#ifndef CONTROL_INFO_NAME
#define CONTROL_INFO_NAME       RCP_info_SI
#define GET_SI_INFO(member)     ((CONTROL_INFO_NAME).member)
#define GET_RCP_REG(member)     (*(CONTROL_INFO_NAME).member)
#endif

/***** structures *****/

typedef struct {
    u16 Version;        /* Set to PLUGIN_API_VERSION. */
    u16 Type;           /* Set to PLUGIN_TYPE_CONTROLLER. */
    char Name[100];     /* plugin title, to help the user select plugins */
    int Reserved1;
    int Reserved2;
} PLUGIN_INFO;

typedef struct {
    int Present; /* of Boolean significance:  plugged in? */
    int RawData; /* of Boolean significance:  low-level processing */
    int Plugin; /* an enumerated value among the five PLUGIN_* macros */
} CONTROL;

/*
 * added for compatibility with zilmar's old button names
 */

#define stick_x         X_AXIS
#define stick_y         Y_AXIS

#define U_JPAD          U_DPAD
#define D_JPAD          D_DPAD
#define L_JPAD          L_DPAD
#define R_JPAD          R_DPAD

#define L_CBUTTONS      L_CBUTTON
#define D_CBUTTONS      D_CBUTTON
#define U_CBUTTONS      U_CBUTTON
#define R_CBUTTONS      R_CBUTTON

#define SWAP_HALFWORD_BYTES
#if defined(__BIG_ENDIAN__) | (__BYTE_ORDER != __LITTLE_ENDIAN)
#undef SWAP_HALFWORD_BYTES
#endif

typedef struct {
#ifdef SWAP_HALFWORD_BYTES
    unsigned R_JPAD      :  1;
    unsigned L_JPAD      :  1;
    unsigned D_JPAD      :  1;
    unsigned U_JPAD      :  1;
    unsigned START_BUTTON:  1;
    unsigned Z_TRIG      :  1;
    unsigned B_BUTTON    :  1;
    unsigned A_BUTTON    :  1;

    unsigned R_CBUTTONS  :  1;
    unsigned L_CBUTTONS  :  1;
    unsigned D_CBUTTONS  :  1;
    unsigned U_CBUTTONS  :  1;
    unsigned R_TRIG      :  1;
    unsigned L_TRIG      :  1;
    unsigned Reserved1   :  1;
    unsigned Reserved2   :  1;
#else
    unsigned R_CBUTTONS  :  1;
    unsigned L_CBUTTONS  :  1;
    unsigned D_CBUTTONS  :  1;
    unsigned U_CBUTTONS  :  1;
    unsigned R_TRIG      :  1;
    unsigned L_TRIG      :  1;
    unsigned Reserved1   :  1;
    unsigned Reserved2   :  1;

    unsigned R_JPAD      :  1;
    unsigned L_JPAD      :  1;
    unsigned D_JPAD      :  1;
    unsigned U_JPAD      :  1;
    unsigned START_BUTTON:  1;
    unsigned Z_TRIG      :  1;
    unsigned B_BUTTON    :  1;
    unsigned A_BUTTON    :  1;
#endif

#ifdef SWAP_HALFWORD_BYTES
    signed   stick_y     :  8;
    signed   stick_x     :  8;
#else
    signed   stick_x     :  8;
    signed   stick_y     :  8;
#endif

#if 0
    unsigned char errno; /* not supported by this plugin system */
#endif
} OS_CONT_PAD;

typedef union {
    u32 Value;
    u16 halfwords[2];
    i8 bytes[4];
    OS_CONT_PAD cont_pad;
} BUTTONS;

/* old names from the original specification file */
#define hInst               hinst
#define MemorySwapped       MemoryBswaped

#if (PLUGIN_API_VERSION >= 0x0101)
typedef struct {
    p_void hMainWindow;
    p_void hInst;

    int MemorySwapped;
    pu8 HEADER; /* 64-byte ROM header (sensitive to MemorySwapped flag) */
    CONTROL * Controls; /* pointer to an array of 4 controllers */
} CONTROL_INFO;
#endif

/******************************************************************************
* name     :  CloseDLL
* optional :  no
* call time:  when the emulator is shutting down or chooses to free memory
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL CloseDLL(void);

/******************************************************************************
* name     :  ControllerCommand
* optional :  yes
* call time:  when raw data has been sent to a specific controller and is ready
*             for processing
* input    :  Control:  controller number (0 to 3) or -1 upon end of PIF RAM
*             Command:  pointer to the raw data to be processed
* output   :  none
* notes    :  This function is only needed if the plugin is allowing raw data,
*             or if it is set to process raw data, which looks like this:
*                 initialize controller:  01 03 00 FF FF FF
*                 read controller      :  01 04 01 FF FF FF FF
*******************************************************************************/
EXPORT void CALL ControllerCommand(int Control, pu8 Command);

/******************************************************************************
* name     :  DllAbout
* optional :  yes
* call time:  upon a request to see information about the plugin (e.g., authors)
* input    :  a pointer to the window that called this function
* output   :  none
*******************************************************************************/
EXPORT void CALL DllAbout(p_void hParent);

/******************************************************************************
* name     :  DllConfig
* optional :  yes
* call time:  upon a request to configure the plugin (e.g., change settings)
* input    :  a pointer to the window that called this function
* output   :  none
*******************************************************************************/
EXPORT void CALL DllConfig(p_void hParent);

/******************************************************************************
* name     :  DllTest
* optional :  yes
* call time:  upon a request to test the plugin (e.g., system capabilities)
* input    :  a pointer to the window that called this function
* output   :  none
*******************************************************************************/
EXPORT void CALL DllTest(p_void hParent);

/******************************************************************************
* name     :  GetDllInfo
* optional :  no
* call time:  during the enumeration of valid plugins the emulator can load
* input    :  a pointer to a PLUGIN_INFO stucture used to determine support
* output   :  none
*******************************************************************************/
EXPORT void CALL GetDllInfo(PLUGIN_INFO * PluginInfo);

/******************************************************************************
* name     :  GetKeys
* optional :  uh ... possibly?  still have to test
* call time:  on a regular basis, in order to keep uploading possible changes to
*             any of the controllers' current states back to the CPU emulator
* input    :  Control:  controller number (0 to 3)
*             Keys   :  pointer to a server-side BUTTONS structure that should
*                       receive the client-side controller state from the plugin
* output   :  none
*******************************************************************************/
EXPORT void CALL GetKeys(int Control, BUTTONS * Keys);

/******************************************************************************
* name     :  InitiateControllers
* optional :  no
* call time:  after the emulator has successfully loaded the plugin but needs
*             to know how each of the controllers should be handled
* input    :  hMainWindow:  pointer to the main window (version #1.0 only)
*             Controls   :  array of 4 CONTROL structures (version #1.0 only)
*             ControlInfo:  a [pointer to a(?)] CONTROL_INFO structure unifying
*                           the above two parameters into a single structure
* output   :  none
* notes    :  In some ways, the #1.0 declaration of this function may be
*             prefered, but it seems to be inconsistent with the nature of
*             downloading the RSP_INFO, GFX_INFO and AUDIO_INFO structures for
*             initializing the RCP over the other plugin types.  Although, it
*             can be argued that RCP PI and SI have little to no integration to
*             plugins of this type, which concentrate more on just controllers.
*******************************************************************************/
#if (PLUGIN_API_VERSION == 0x0100)
EXPORT void CALL InitiateControllers(p_void hMainWindow, CONTROL Controls[4]);
#elif (PLUGIN_API_VERSION < 0x0102) | (0 == 0)
EXPORT void CALL InitiateControllers(CONTROL_INFO ControlInfo); /* wrong? */
#else
EXPORT void CALL InitiateControllers(CONTROL_INFO * ControlInfo);
#endif

/******************************************************************************
* name     :  ReadController
* optional :  yes
* call time:  when the raw data in PIF RAM is about to be read, after locally
*             processing from within the controller plugin first
* input    :  Control:  controller number (0 to 3) or -1 upon end of PIF RAM
*          :  Command:  pointer to the raw data to be processed
* output   :  none
* notes    :  This function is only needed if the plugin is allowing raw data.
*******************************************************************************/
EXPORT void CALL ReadController(int Control, pu8 Command);

/******************************************************************************
* name     :  RomClosed
* optional :  no
* call time:  when unloading the ROM (sometimes when emulation ends)
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL RomClosed(void);

/******************************************************************************
* name     :  RomOpen
* optional :  no
* call time:  when the emulation thread loads ROM, after InitiateControllers
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL RomOpen(void);

/******************************************************************************
* name     :  WM_KeyDown
* optional :  yes
* call time:  when a key-down message has been sent to the main window and is
*             then to be decoded by the controller plugin, by whatever means
* input    :  wParam:  message code, if the API encodes as unsigned integers
*             lParam:  long message code, if the API encodes as long pointers
* output   :  none
* notes    :  Numerous complications with ABI issues in Microsoft's compilers
*             versus the LP64 model prevent a reliable, 64-bit message system
*             from being truly cross-target.  32-bit precision should suffice.
*******************************************************************************/
EXPORT void CALL WM_KeyDown(unsigned int wParam, i32 lParam);

/******************************************************************************
* name     :  WM_KeyUp
* optional :  yes
* call time:  when a key-up message has been sent to the main window and is
*             then to be decoded by the controller plugin, by whatever means
* input    :  wParam:  message code, if the API encodes as unsigned integers
*             lParam:  long message code, if the API encodes as long pointers
* output   :  none
* notes    :  See Iconoclast's notes for the above `WM_KeyDown' procedure.
*******************************************************************************/
EXPORT void CALL WM_KeyUp(unsigned int wParam, i32 lParam);

#if defined(__cplusplus)
}
#endif

#endif
