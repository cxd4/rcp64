/*******************************************************************************
* Common graphics plugin specifications:  version #1.3 created by zilmar       *
* Revised 2014 by Iconoclast for more compliance, portability and readability. *
*                                                                              *
* All questions or suggestions should go through the EmuTalk plugin forum.     *
* http://www.emutalk.net/forums/showforum.php?f=31                             *
*******************************************************************************/

/*
 * Notes:
 *
 * Setting the approprate bits in MI_INTR_REG and calling CheckInterrupts
 * (which are both passed to the plugin via InitiateGFX) will generate an
 * interrupt from within the plugin.
 *
 * The setting of the RSP flags and generation of an SP interrupt should NOT
 * be done within a plugin of this type.
 */

#ifndef _GFX_H_INCLUDED__
#define _GFX_H_INCLUDED__

#include "my_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define PLUGIN_TYPE_GFX             2

#ifndef PLUGIN_API_VERSION
#define PLUGIN_API_VERSION      0x0103
#endif

/* old names from the original specification file */
#define hInst               hinst
#define MemorySwapped       MemoryBswaped

/*
 * Declare GFX_INFO structure instance as:  `GFX_INFO GFX_INFO_NAME;'
 * ... for the ability to use the below convenience macros.
 *
 * Doing the traditional `GFX_INFO gfx_info' declaration has also worked but
 * requires accessing the RCP registers in a less portable way, for example:
 * `*(gfx_info).MI_INTR_REG |= MI_INTR_MASK_VI;'
 * versus
 * `GET_RCP_REG(MI_INTR_REG) |= MI_INTR_MASK_VI;'.
 */
#ifndef GFX_INFO_NAME
#define GFX_INFO_NAME           RCP_info_VI
#define GET_GFX_INFO(member)    ((GFX_INFO_NAME).member)
#define GET_RCP_REG(member)     (*(GFX_INFO_NAME).member)
#endif

/***** structures *****/

typedef struct {
    u16 Version;        /* Set to PLUGIN_API_VERSION. */
    u16 Type;           /* Set to PLUGIN_TYPE_GFX. */
    char Name[100];     /* plugin title, to help the user select plugins */

    /* If the plugin supports these memory options, then set them to true. */
    int NormalMemory;   /* a normal byte array */
    int MemorySwapped;  /* a normal byte array choosing the client-side,
                           native hardware's endian over the MIPS target's */
} PLUGIN_INFO;

typedef struct {
    p_void hWnd;        /* render window */
    p_void hStatusBar;  /* NULL if the render window has no status bar */

    int MemoryBswaped;

#if (PLUGIN_API_VERSION >= 0x0102)
    pu8 HEADER; /* 64-byte ROM header (sensitive to MemorySwapped flag) */
#endif
    pu8 RDRAM; /* CPU-RCP dynamic RAM (sensitive to MemorySwapped flag) */
    pu8 DMEM; /* high 4K of SP cache memory (sensitive to MemorySwapped flag) */
    pu8 IMEM; /* low 4K of SP cache memory (sensitive to MemorySwapped flag) */

    pu32 MI_INTR_REG;

    pu32 DPC_START_REG;
    pu32 DPC_END_REG;
    pu32 DPC_CURRENT_REG;
    pu32 DPC_STATUS_REG;
    pu32 DPC_CLOCK_REG;
    pu32 DPC_BUFBUSY_REG;
    pu32 DPC_PIPEBUSY_REG;
    pu32 DPC_TMEM_REG;

    pu32 VI_STATUS_REG;
    pu32 VI_ORIGIN_REG;
    pu32 VI_WIDTH_REG;
    pu32 VI_INTR_REG;
    pu32 VI_V_CURRENT_LINE_REG;
    pu32 VI_TIMING_REG;
    pu32 VI_V_SYNC_REG;
    pu32 VI_H_SYNC_REG;
    pu32 VI_LEAP_REG;
    pu32 VI_H_START_REG;
    pu32 VI_V_START_REG;
    pu32 VI_V_BURST_REG;
    pu32 VI_X_SCALE_REG;
    pu32 VI_Y_SCALE_REG;

    p_func CheckInterrupts;
} GFX_INFO;

/******************************************************************************
* name     :  CaptureScreen
* optional :  yes
* call time:  when the user triggers an event requesting to write the current
*             video picture to the file system
* input    :  a text string representing the file system path for disk writes
* output   :  none
*******************************************************************************/
#if (PLUGIN_API_VERSION >= 0x0103)
EXPORT void CALL CaptureScreen(char * Directory);
#endif

/******************************************************************************
* name     :  ChangeWindow
* optional :  no
* call time:  when the user triggers an event requesting to switch between
*             full-screen and windowed display modes
* input    :  none
* output   :  none
*******************************************************************************/
#if (PLUGIN_API_VERSION < 0x0102)
#error `ChangeWindow` in older, unreleased Gfx #1.1 and 1.0 has unknown form.
#else
EXPORT void CALL ChangeWindow(void);
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
* name     :  DrawScreen
* optional :  no
* call time:  when the emulator framework receives a paint message
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL DrawScreen(void);

/******************************************************************************
* name     :  GetDllInfo
* optional :  no
* call time:  during the enumeration of valid plugins the emulator can load
* input    :  a pointer to a PLUGIN_INFO stucture used to determine support
* output   :  none
*******************************************************************************/
EXPORT void CALL GetDllInfo(PLUGIN_INFO * PluginInfo);

/******************************************************************************
* name     :  InitiateGFX
* optional :  no
* call time:  when the emulator needs to know more information about the plugin
*             before responding to events (e.g., ask to configure the plugin)
*             which may depend on device initialization (e.g., render contexts)
*             (Emulators are free to call this straight after loading instead.)
*             (This function is never to be called from the emulation thread.)
* input    :  a GFX_INFO structure mostly for setting up the RCP memory map
* output   :  whether initiation succeeded:  zero if false, nonzero if true
*******************************************************************************/
EXPORT int CALL InitiateGFX(GFX_INFO Gfx_Info);

/******************************************************************************
* name     :  MoveScreen
* optional :  no
* call time:  in response to the emulator receiving a move message
* input    :  xpos:  x-coordinate of the client area's upper-left corner
*             ypos:  y-coordinate of the client area's upper-left corner
* output   :  none
*******************************************************************************/
EXPORT void CALL MoveScreen(int xpos, int ypos);

/******************************************************************************
* name     :  ProcessDList
* optional :  no
* call time:  when the emulator starts a GBI command (display) list to HLE
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL ProcessDList(void);

/******************************************************************************
* name     :  ProcessRDPList
* optional :  ?? need to test, can't remember
* call time:  when the emulator starts a GBI command (display) list to LLE
* input    :  none
* output   :  none
*******************************************************************************/
#if (PLUGIN_API_VERSION == 0x0100) | (PLUGIN_API_VERSION >= 0x0103)
EXPORT void CALL ProcessRDPList(void);
#endif

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
* call time:  when the emulation thread loads ROM, after InitiateGFX succeeds
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL RomOpen(void);

/******************************************************************************
* name     :  ShowCFB
* optional :  can't remember, need to test (It is, however, purposeless. :))
* call time:  when the emulator decides to remind the plugin to emulate CFB
* input    :  none
* output   :  none
*******************************************************************************/
#if (PLUGIN_API_VERSION >= 0x0103)
EXPORT void CALL ShowCFB(void);
#endif

/******************************************************************************
* name     :  UpdateScreen
* optional :  no
* call time:  in response to a vertical interrupt set in MI_INTR_REG intended
*             to refresh the active video screen
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL UpdateScreen(void);

/******************************************************************************
* name     :  ViStatusChanged
* optional :  no
* call time:  when a new value for VI_CONTROL_REG (VI_STATUS_REG) is detected
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL ViStatusChanged(void);

/******************************************************************************
* name     :  ViWidthChanged
* optional :  no
* call time:  when a new value for VI_H_VIDEO_REG (VI_WIDTH_REG) is detected
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL ViWidthChanged(void);

#if defined(__cplusplus)
}
#endif

#endif
