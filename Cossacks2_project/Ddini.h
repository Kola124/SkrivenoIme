/*********************************************************************** 
 *SDL initialisation module (migrated from DirectDraw)                  
 *
 * This module creates the SDL window with the primary surface
 * and sets up display modes.
 *
 ***********************************************************************/
#ifndef __DDINI_H_
#define __DDINI_H_

//#include "afx.h"
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"
#include "lines.h"
#include "protest.h"

// SDL headers
#include "include\SDL.h"
#include "include\SDL_syswm.h"

#define CEXPORT __declspec(dllexport)
#define CIMPORT __declspec(dllimport)
//#define STARFORCE
#define MAKE_PTC
#define DLLEXPORT extern "C" __declspec(dllexport)

#ifndef STARFORCE
 #define DLL0(x) x()
 #define DLL1(x,y) x(y)
 #define DLL2(x,y,z) x(y,z)
 #define DLL3(x,y,z,t) x(y,z,t)
 #define SFLB_DLLEXPORT void
#else
 #define SFLB_DLLEXPORT extern "C" void __declspec(dllexport) __cdecl
 #define DLL0(x) x()
 #define DLL1(x,y) x(y)
 #define DLL2(x,y,z) x(y,z)
 #define DLL3(x,y,z,t) x(y,z,t)
#endif

CIMPORT void CheckDipBuilding(byte NI, int Index);

// SDL replacements for DirectDraw objects
extern SDL_Window* sdlWindow;         // SDL2 window
extern SDL_Renderer* sdlRenderer;     // SDL2 renderer
extern SDL_Texture* sdlTexture;       // SDL2 texture for software rendering
extern SDL_Surface* sdlSurface;       // SDL2 software surface
extern BOOL bActive;                  // is application active?
extern BOOL CurrentSurface;           // surface state
extern BOOL DDError;                  // error state
extern BOOL DDDebug;
extern HWND hwnd;
extern bool window_mode;


#define LPDIRECTDRAW void*
#define LPDIRECTDRAWSURFACE void*
/*
struct DDSURFACEDESC {
    int dummy;      
    void* lpSurface; 
};
*/
extern LPDIRECTDRAW lpDD;          
extern LPDIRECTDRAWSURFACE lpDDSPrimary; 
extern LPDIRECTDRAWSURFACE lpDDSBack;   

extern DDSURFACEDESC   ddsd;

// Display mode enumeration
extern int ModeLX[32];
extern int ModeLY[32];
extern int NModes;

extern SDL_Color GPal[256];
extern bool PalDone;

CEXPORT void UpdateGlobalHWND(HWND newHwnd);
/*  Create SDL graphics objects
 * 
 * This procedure creates SDL window with primary surface and 
 * sets up display mode.
 */
bool CreateDDObjects(HWND hwnd);

/*  Create RGB mode objects (for higher color depths)
 */
bool CreateRGBDDObjects(HWND hwnd);
bool CreateRGB640DDObjects(HWND hwnd);

/*     Closing all graphics objects
 */
void FreeDDObjects(void);

/*
 * Flipping Pages
 */
CEXPORT
void FlipPages(void);

/*
 * Getting Screen Pointer
 */
void LockSurface(void);

/*
 *  Unlocking the surface 
 */
void UnlockSurface(void);

/*
 * Getting then DC of the active area of the screen
 */
HDC GetSDC(void);

// Setting the palette
CEXPORT
void LoadPalette(LPCSTR lpFileName);
CEXPORT
void SlowLoadPalette(LPCSTR lpFileName);
CEXPORT
void SlowUnLoadPalette(LPCSTR lpFileName);
CEXPORT
void SetDarkPalette();

// Palette utilities
CEXPORT
byte GetPaletteColor(int r, int g, int b);
CEXPORT
void GetPalColor(byte idx, byte* r, byte* g, byte* b);

// Mode enumeration
bool EnumModesOnly();
void DelLog();

// Debug functions
void SetDebugMode();
void NoDebugMode();

// SDL2-specific helper
void InitSDLIfNeeded();
void UpdateSDLPalette();
void CreateSoftwareBuffer();

#endif //__DDINI_H_

#include "newmemory.h"