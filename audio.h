/*******************************************************************************
* Common audio plugin specifications:  version #1.1 created by zilmar          *
* Revised 2014 by Iconoclast for more compliance, portability and readability. *
*                                                                              *
* All questions or suggestions should go through the EmuTalk plugin forum.     *
* http://www.emutalk.net/forums/showforum.php?f=31                             *
*******************************************************************************/

/*
 * reminder to self -- seems a few people have already been trying to extend
 * the audio plugin specs; jttl added a "PauseAudio" function to his own #1.2
 * specs, and Azimer is aiming to better the audio spec as well; when I start
 * adding stuff from Project64 2.1's unpublished Audio #1.2 specs I will try
 * to merge everyone else's ideas in here as well --Iconoclast
 */

/*
 * Notes:
 *
 * Setting the approprate bits in MI_INTR_REG and calling CheckInterrupts
 * (which are both passed to the plugin via InitiateAudio) will generate an
 * interrupt from within the plugin.
 *
 * The setting of the RSP flags and generation of an SP interrupt should NOT
 * be done within a plugin of this type.
 */

#ifndef _AUDIO_H_INCLUDED__
#define _AUDIO_H_INCLUDED__

#include "my_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define PLUGIN_TYPE_AUDIO           3

#ifndef PLUGIN_API_VERSION
#define PLUGIN_API_VERSION      0x0101
#endif

/*
 * added note by Iconoclast--
 * The below macros are by zilmar and are simply logical guesses.
 * The actual enumerations for TV system format modes are discussed in the
 * RCP reference for the `osTvType' global state.
 */
#define SYSTEM_NTSC                 0
#define SYSTEM_PAL                  1
#define SYSTEM_MPAL                 2

/* old names from the original specification file */
#define hInst               hinst
#define MemorySwapped       MemoryBswaped

/*
 * Declare AUDIO_INFO structure instance as:  `AUDIO_INFO AUDIO_INFO_NAME;'
 * ... for the ability to use the below convenience macros.
 *
 * Doing the traditional `AUDIO_INFO AI_info' declaration has also worked but
 * requires accessing the RCP registers in a less portable way, for example:
 * `*(AI_info).MI_INTR_REG |= MI_INTR_MASK_AI;'
 * versus
 * `GET_RCP_REG(MI_INTR_REG) |= MI_INTR_MASK_AI;'.
 */
#ifndef AUDIO_INFO_NAME
#define AUDIO_INFO_NAME         RCP_info_AI
#define GET_AI_INFO(member)     ((AUDIO_INFO_NAME).member)
#define GET_RCP_REG(member)     (*(AUDIO_INFO_NAME).member)
#endif

/***** structures *****/

typedef struct {
    u16 Version;        /* Set to PLUGIN_API_VERSION. */
    u16 Type;           /* Set to PLUGIN_TYPE_AUDIO. */
    char Name[100];     /* plugin title, to help the user select plugins */

    /* If plugin supports these memory options, then set them to true. */
    int NormalMemory;   /* a normal byte array */
    int MemorySwapped;  /* a normal byte array choosing the client-side,
                           native hardware's endian over the MIPS target's */
} PLUGIN_INFO;

typedef struct {
    p_void hWnd;
    p_void hInst;
    int MemorySwapped;

#if (PLUGIN_API_VERSION >= 0x0101)
    pu8 HEADER; /* 64-byte ROM header (sensitive to MemorySwapped flag) */
#endif
    pu8 RDRAM; /* CPU-RCP dynamic RAM (sensitive to MemorySwapped flag) */
    pu8 DMEM; /* high 4K of SP cache memory (sensitive to MemorySwapped flag) */
    pu8 IMEM; /* low 4K of SP cache memory (sensitive to MemorySwapped flag) */

#if (PLUGIN_API_VERSION < 0x0101)
#error Audio #1.0 specifications unknown; offset for `SystemType' unknown.
    int SystemType;
#endif

    pu32 MI_INTR_REG;

    pu32 AI_DRAM_ADDR_REG;
    pu32 AI_LEN_REG;
    pu32 AI_CONTROL_REG;
    pu32 AI_STATUS_REG;
    pu32 AI_DACRATE_REG;
    pu32 AI_BITRATE_REG;

    p_func CheckInterrupts;
} AUDIO_INFO;

/******************************************************************************
* name     :  AiDacrateChanged
* optional :  no
* call time:  when a new value for AI_DACRATE_REG is detected
* input    :  the system type--either SYSTEM_NTSC, SYSTEM_PAL or SYSTEM_MPAL
* output   :  none
*******************************************************************************/
EXPORT void CALL AiDacrateChanged(int SystemType);

/******************************************************************************
* name     :  AiLenChanged
* optional :  no
* call time:  when a new value for AI_LEN_REG is detected
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL AiLenChanged(void);

/******************************************************************************
* name     :  AiReadLength
* optional :  no
* call time:  when the emulator decides to report the up-to-date sound length
*             value to the audio plugin that AI_LEN_REG should be set equal to
* input    :  none
* output   :  the amount of sound bytes still left to play
*******************************************************************************/
EXPORT u32 CALL AiReadLength(void);

/******************************************************************************
* name     :  AiUpdate
* optional :  maybe (prone to CPU thread leaks if omitted; Mupen64 0.5 segfault)
* call time:  on regular, constant intervals to allow the audio plugin to update
*             things on a regular basis (e.g., check how much of a sound buffer
*             is left to be processed, copy more stuff to the buffer, etc.)
*             (This function was created for the purpose of accessing the
*             message loop of the main window, but this behavior is optional.)
* input    :  If `Wait' is nonzero, then this function should delay processing,
*             possibly until the main window has a message in its message queue.
* output   :  none
*******************************************************************************/
EXPORT void CALL AiUpdate(int Wait);

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
* name     :  GetDllInfo
* optional :  no
* call time:  during the enumeration of valid plugins the emulator can load
* input    :  a pointer to a PLUGIN_INFO stucture used to determine support
* output   :  none
*******************************************************************************/
EXPORT void CALL GetDllInfo(PLUGIN_INFO * PluginInfo);

/******************************************************************************
* name     :  InitiateAudio
* optional :  no
* call time:  when the emulator needs to know more information about the plugin
*             before responding to events (e.g., ask to configure the plugin)
*             which may depend on device initialization (accessing sound cards)
*             (Emulators are free to call this straight after loading instead.)
*             (This function is never to be called from the emulation thread.)
* input    :  a AUDIO_INFO structure mostly for setting up the RCP memory map
* output   :  whether initiation succeeded:  zero if false, nonzero if true
*******************************************************************************/
EXPORT int CALL InitiateAudio(AUDIO_INFO Audio_Info);

/******************************************************************************
* name     :  ProcessAList
* optional :  no
* call time:  when the emulator starts a ABI command (audio) list to HLE
* input    :  none
* output   :  none
*******************************************************************************/
#if (PLUGIN_API_VERSION < 0x0101)
#error Audio #1.0 specifications unknown; old audio lists function name unknown.
#else
EXPORT void CALL ProcessAList(void);
#endif

/******************************************************************************
* name     :  RomClosed
* optional :  no
* call time:  when unloading the ROM (sometimes when emulation ends)
* input    :  none
* output   :  none
*******************************************************************************/
EXPORT void CALL RomClosed(void);

#if defined(__cplusplus)
}
#endif

#endif
