/*********************************************************************** 
 * SDL2 initialisation module (migrated from DirectDraw)                  
 *
 * This module creates the SDL2 window with renderer
 * and sets up display modes.
 *
 ***********************************************************************/
#define __ddini_cpp_
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mode.h"
#include "MapDiscr.h"
#include "fog.h"
#include "GSound.h"
#include "fonts.h"
#include "VirtScreen.h"

#include "include\SDL.h"
#include "include\SDL_syswm.h"
#include <vector>
#include <algorithm>
//#include <string>

LPDIRECTDRAW lpDD = nullptr;
LPDIRECTDRAWSURFACE lpDDSPrimary = nullptr;
LPDIRECTDRAWSURFACE lpDDSBack = nullptr;
DDSURFACEDESC ddsd;

extern HWND hwnd;
extern int mouseX;
extern int mouseY;

#ifdef _WIN32
    #undef main  // SDL2 redefines main on Windows
#endif

void DDLog(LPSTR sz, ...)
{
    char ach[256];
    va_list va;

    va_start(va, sz);
    vsprintf(ach, sz, va);
    va_end(va);
    FILE* f = fopen("DDraw.log", "a");
    if (f) {
        fprintf(f, "%s", ach);
        fclose(f);
    }
}
CEXPORT
void DDLog2(LPSTR sz, ...)
{
    char ach[256];
    va_list va;

    va_start(va, sz);
    vsprintf(ach, sz, va);
    va_end(va);
    FILE* f = fopen("ErrorLog.log", "a");
    if (f) {
        fprintf(f, "%s", ach);
        fclose(f);
    }
}

#ifdef _USE3D
#include "GP_Draw.h"

IRenderSystem* IRS;
float g_dbgZ;
int VBUF;

IRenderSystem* GetRenderSystemDX();

#pragma pack(push)
#pragma pack(8)
extern GP_System GPS;
#pragma pack(pop)
#endif

void Rept(LPSTR sz, ...);
int ModeLX[32];
int ModeLY[32];
int NModes = 0;
void SERROR();
void SERROR1();
void SERROR2();
void PropCopy();
void InitRLCWindows();
//#define COPYSCR
const int InitLx = 1024;
const int InitLy = 768;
CEXPORT int RealLx;
CEXPORT int RealLy;
CEXPORT int SCRSizeX;
CEXPORT int SCRSizeY;
CEXPORT int RSCRSizeX;
CEXPORT int RSCRSizeY;
CEXPORT int COPYSizeX;
CEXPORT int Pitch;

extern int MaxSizeX;  // Usually 1024 or larger
extern int MaxSizeY;  // Usually 768 or larger

// SDL2 graphics objects
SDL_Window* sdlWindow = NULL;
SDL_Renderer* sdlRenderer = NULL;
SDL_Texture* sdlTexture = NULL;
SDL_Surface* sdlSurface = NULL;
BOOL bActive = TRUE;
BOOL CurrentSurface = FALSE;
BOOL DDError = FALSE;
BOOL DDDebug = FALSE;
SDL_Color GPal[256];
bool PalDone = false;
extern word PlayerMenuMode;
extern HWND hwnd;
extern bool window_mode;

int desktopWidth = 0;
int desktopHeight = 0;
int scaleFactor = 1;

typedef struct zzz {
    BITMAPINFO bmp;
    PALETTEENTRY XPal[255];
} zzz;

zzz xxt;
void* offScreenPtr = NULL;

int SCRSZY = 0;

int CalculateIntegerScale(int desktopW, int desktopH, int gameW, int gameH) {
    int scaleX = desktopW / gameW;
    int scaleY = desktopH / gameH;
    
    // Use the smaller scale to ensure the game fits on screen
    int scale = (scaleX < scaleY) ? scaleX : scaleY;
    
    // Ensure at least 1x scaling
    if (scale < 1) scale = 1;
    
    DDLog("Integer scale calculation: Desktop=%dx%d, Game=%dx%d, ScaleX=%d, ScaleY=%d, Final=%dx\n",
          desktopW, desktopH, gameW, gameH, scaleX, scaleY, scale);
    
    return scale;
}

CEXPORT
byte GetPaletteColor(int r, int g, int b) {
    int dmax = 10000;
    int bestc = 0;
    for (int i = 0; i < 256; i++) {
        int d = abs(r - GPal[i].r) + abs(g - GPal[i].g) + abs(b - GPal[i].b);
        if (d < dmax) {
            dmax = d;
            bestc = i;
        }
    }
    return bestc;
}

void ClearRGB() {
    if (!bActive) return;
    if (RealScreenPtr) {
        memset(RealScreenPtr, 0, RSCRSizeX * SCRSZY);
    }
}

word PAL16[256];
int P16Idx = -1;
extern int CurPalette;

void CheckPal16() {
    P16Idx = CurPalette;
    for (int i = 0; i < 256; i++) {
        PAL16[i] = (GPal[i].b >> 3) + ((GPal[i].g >> 2) << 5) + ((GPal[i].r >> 3) << 11);
    }
}

void CheckPal16x() {
    for (int i = 0; i < 256; i++) {
        PAL16[i] = (GPal[i].b >> 3) + ((GPal[i].g >> 2) << 5) + ((GPal[i].r >> 3) << 11);
    }
}

void ChangeColorFF() {
    if (!RealScreenPtr) return;
    try {
        int DD = 10000;
        int c = 0xFF;
        for (int i = 0; i < 255; i++) {
            int D = 255 + 255 + 255 - GPal[i].b - GPal[i].g - GPal[i].r;
            if (D < DD) {
                c = i;
                DD = D;
            }
        }
        int sz = RSCRSizeX * RealLy;
        for (int i = 0; i < sz; i++) ((byte*)RealScreenPtr)[i] = 0;
    }
    catch (...) {}
}

void UpdateSDLPalette() {
    if (!sdlSurface || !sdlSurface->format->palette) return;
    
    for (int i = 0; i < 256; i++) {
        GPal[i].a = 255;
    }
    
    SDL_SetPaletteColors(sdlSurface->format->palette, GPal, 0, 256);
}

bool ProcessMessagesSDL() {
    SDL_Event event;
    bool hasMessages = false;
    
    SDL_PumpEvents();
    
    while (SDL_PollEvent(&event)) {
        hasMessages = true;
        
        switch (event.type) {
            case SDL_QUIT:
                DDLog("SDL_QUIT event received\n");
                PostMessage(hwnd, WM_QUIT, 0, 0);
                break;
                
            case SDL_MOUSEMOTION:
                // SDL gives us coordinates in logical space (game coordinates)
                // when using RenderSetLogicalSize, which is perfect!
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                
                PostMessage(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(mouseX, mouseY));
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                mouseX = event.button.x;
                mouseY = event.button.y;
                
                if (event.button.button == SDL_BUTTON_LEFT) {
                    PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, 
                               MAKELPARAM(event.button.x, event.button.y));
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    PostMessage(hwnd, WM_RBUTTONDOWN, MK_RBUTTON, 
                               MAKELPARAM(event.button.x, event.button.y));
                } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                    PostMessage(hwnd, WM_MBUTTONDOWN, MK_MBUTTON,
                               MAKELPARAM(event.button.x, event.button.y));
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                mouseX = event.button.x;
                mouseY = event.button.y;
                
                if (event.button.button == SDL_BUTTON_LEFT) {
                    PostMessage(hwnd, WM_LBUTTONUP, 0, 
                               MAKELPARAM(event.button.x, event.button.y));
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    PostMessage(hwnd, WM_RBUTTONUP, 0, 
                               MAKELPARAM(event.button.x, event.button.y));
                } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                    PostMessage(hwnd, WM_MBUTTONUP, 0,
                               MAKELPARAM(event.button.x, event.button.y));
                }
                break;
                
            case SDL_MOUSEWHEEL:
                {
                    short delta = (short)(event.wheel.y * 120);
                    PostMessage(hwnd, WM_MOUSEWHEEL, MAKEWPARAM(0, delta), 
                               MAKELPARAM(mouseX, mouseY));
                }
                break;
                
            case SDL_KEYDOWN:
                {
                    WPARAM vk = event.key.keysym.scancode;
                    LPARAM lp = 1 | (event.key.keysym.scancode << 16);
                    if (event.key.repeat) {
                        lp |= (1 << 30);
                    }
                    PostMessage(hwnd, WM_KEYDOWN, vk, lp);
                }
                break;
                
            case SDL_KEYUP:
                {
                    WPARAM vk = event.key.keysym.scancode;
                    LPARAM lp = 1 | (event.key.keysym.scancode << 16) | (1 << 30) | (1 << 31);
                    PostMessage(hwnd, WM_KEYUP, vk, lp);
                }
                break;
                
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        DDLog("SDL_WINDOWEVENT_FOCUS_GAINED - setting bActive=TRUE\n");
                        bActive = TRUE;
                        PostMessage(hwnd, WM_ACTIVATEAPP, TRUE, 0);
                        break;
                        
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        DDLog("SDL_WINDOWEVENT_FOCUS_LOST - setting bActive=FALSE\n");
                        bActive = FALSE;
                        PostMessage(hwnd, WM_ACTIVATEAPP, FALSE, 0);
                        break;
                        
                    case SDL_WINDOWEVENT_EXPOSED:
                        PostMessage(hwnd, WM_PAINT, 0, 0);
                        break;
                }
                break;
        }
    }
    
    return hasMessages;
}

CEXPORT void UpdateGlobalHWND(HWND newHwnd) {
    hwnd = newHwnd;
}

CEXPORT
void FlipPages(void)
{
#ifdef _USE3D
    void Test3D();
    Test3D();
    float fps = Stats::GetFPS();
    char strfps[128];
    sprintf(strfps, "fps:% 3.f", fps);
    ShowString(5, 5, strfps, &BlackFont);
    ShowString(4, 4, strfps, &WhiteFont);
    GPS.OnFrame();
    IRS->OnFrame();
    IRS->ClearDeviceZBuffer();
    return;
#endif

    if (!bActive) {
        DDLog("FlipPages: Not active, skipping\n");
        return;
    }
    
    if (!sdlRenderer || !sdlTexture || !sdlSurface || !offScreenPtr) {
        DDLog("FlipPages: Missing required objects - renderer=%p, texture=%p, surface=%p, offscreen=%p\n",
              sdlRenderer, sdlTexture, sdlSurface, offScreenPtr);
        return;
    }
    
    // Lock SDL surface
    if (SDL_MUSTLOCK(sdlSurface)) {
        if (SDL_LockSurface(sdlSurface) < 0) return;
    }
    
    // Copy from offscreen buffer to SDL surface
    byte* src = (byte*)ScreenPtr;
    byte* dst = (byte*)sdlSurface->pixels;
    int srcPitch = MaxSizeX;
    int dstPitch = sdlSurface->pitch;
    
    for (int y = 0; y < RealLy; y++) {
        memcpy(dst + y * dstPitch, src + y * srcPitch, RealLx);
    }
    
    // Unlock SDL surface
    if (SDL_MUSTLOCK(sdlSurface)) {
        SDL_UnlockSurface(sdlSurface);
    }
    
    // Update palette
    if (sdlSurface->format->palette) {
        SDL_SetPaletteColors(sdlSurface->format->palette, GPal, 0, 256);
    }
    
    // Convert to 32-bit and update texture
    SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    if (format) {
        SDL_Surface* rgbSurface = SDL_ConvertSurface(sdlSurface, format, 0);
        SDL_FreeFormat(format);
        
        if (rgbSurface) {
            SDL_UpdateTexture(sdlTexture, NULL, rgbSurface->pixels, rgbSurface->pitch);
            SDL_FreeSurface(rgbSurface);
        }
    }
    
    // Render (logical size handles scaling automatically)
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);
    
    CurrentSurface = !CurrentSurface;
}



void LockSurface(void)
{
    if (DDError) return;

    ScreenPtr = (void*)((byte*)offScreenPtr + MaxSizeX * 32);
    RealScreenPtr = ScreenPtr;
    
    SCRSizeX = MaxSizeX;
    SCRSizeY = MaxSizeY;
    RSCRSizeX = MaxSizeX;
    SCRSZY = MaxSizeY;
    Pitch = MaxSizeX;
}

void UnlockSurface(void)
{
    if (DDError) return;
    // Nothing to unlock for our offscreen buffer
}

HDC GetSDC(void)
{
    if (window_mode) return 0;
    // SDL2 doesn't provide direct DC access
    return 0;
}

void SetDebugMode()
{
    DDDebug = true;
}

void NoDebugMode()
{
    DDDebug = false;
}

bool m640_16 = 0;
bool m640_24 = 0;
bool m640_32 = 0;
bool m1024_768 = 0;
int BestVX = 640;
int BestVY = 480;
int BestBPP = 32;

// Initialize SDL2 if not already initialized
void InitSDLIfNeeded() {
    static bool sdl_initialized = false;
    if (!sdl_initialized) {
        DDLog("Initializing SDL2...\n");
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            char error[256];
            sprintf(error, "Unable to initialize SDL2: %s", SDL_GetError());
            DDLog("SDL_Init failed: %s\n", SDL_GetError());
            MessageBox(hwnd, error, "Loading error", MB_ICONSTOP);
            exit(0);
        }
        sdl_initialized = true;
        DDLog("SDL2 initialized successfully\n");
    }
}

bool EnumModesOnly() {
    InitSDLIfNeeded();
    
    // Get display count
    int display_count = SDL_GetNumVideoDisplays();
    if (display_count < 1) {
        DDLog("No displays found\n");
        MessageBox(hwnd, "No displays found.", "Error", MB_ICONSTOP);
        return false;
    }
    
    DDLog("Found %d displays\n", display_count);
    
    // Get display modes for primary display
    int mode_count = SDL_GetNumDisplayModes(0);
    if (mode_count < 1) {
        DDLog("No display modes found, using common modes\n");
        // Fallback to common modes
        const int common_modes[][2] = {
            {640, 480},
            {800, 600},
            {1024, 768},
            {1152, 864},
            {1280, 720},
            {1280, 1024},
            {1366, 768},
            {1400, 1050},
            {1440, 900},
            {1600, 900},
            {1600, 1200},
            {1680, 1050},
            {1920, 1080},
            {1920, 1200},
            {2560, 1440},
            {3840, 2160}
        };
        
        NModes = 0;
        for (int i = 0; i < sizeof(common_modes) / sizeof(common_modes[0]) && NModes < 32; i++) {
            ModeLX[NModes] = common_modes[i][0];
            ModeLY[NModes] = common_modes[i][1];
            NModes++;
            DDLog("Added mode: %dx%d\n", ModeLX[NModes-1], ModeLY[NModes-1]);
        }
        return true;
    }
    
    DDLog("Found %d display modes\n", mode_count);
    
    // Get all available modes
    std::vector<SDL_DisplayMode> modes;
    for (int i = 0; i < mode_count; i++) {
        SDL_DisplayMode mode;
        if (SDL_GetDisplayMode(0, i, &mode) == 0) {
            modes.push_back(mode);
            DDLog("Mode %d: %dx%d @ %dHz\n", i, mode.w, mode.h, mode.refresh_rate);
        }
    }
    
    // Sort by resolution (largest first)
    std::sort(modes.begin(), modes.end(), [](const SDL_DisplayMode& a, const SDL_DisplayMode& b) {
        int area_a = a.w * a.h;
        int area_b = b.w * b.h;
        if (area_a != area_b) return area_a > area_b;
        if (a.w != b.w) return a.w > b.w;
        return a.h > b.h;
    });
    
    // Remove duplicates and add to our array
    NModes = 0;
    for (size_t i = 0; i < modes.size() && NModes < 32; i++) {
        // Skip duplicates
        bool duplicate = false;
        for (int j = 0; j < NModes; j++) {
            if (ModeLX[j] == modes[i].w && ModeLY[j] == modes[i].h) {
                duplicate = true;
                break;
            }
        }
        
        if (!duplicate) {
            ModeLX[NModes] = modes[i].w;
            ModeLY[NModes] = modes[i].h;
            NModes++;
            DDLog("Added unique mode: %dx%d\n", modes[i].w, modes[i].h);
        }
    }
    
    return true;
}

void DelLog() {
    DeleteFile("DDraw.log");
}

bool CreateDDObjects(HWND hwnd)
{
#ifdef _USE3D
    IRS = GetRenderSystemDX();
    assert(IRS);
    IRS->Init(hwnd);
    if (!window_mode) {
        ScreenProp sp = IRS->GetScreenProperties();
        sp.fullScreen = true;
        IRS->SetScreenProperties(sp);
    }
    GPS.Init(IRS);
    void InitGroundZbuffer();
    InitGroundZbuffer();
    DDError = false;
    SCRSizeX = MaxSizeX;
    SCRSizeY = MaxSizeY;
    RSCRSizeX = RealLx;
    COPYSizeX = RealLx;
    RSCRSizeY = RealLy;
    ScrHeight = SCRSizeY;
    ScrWidth = SCRSizeX;
    GPS.SetClipArea(0, 0, RealLx, RealLy);
    BytesPerPixel = 2;
    return true;
#endif

    InitSDLIfNeeded();
    
    DDError = false;
    CurrentSurface = true;

    // Enumerate modes first
    EnumModesOnly();
    
    DDLog("CreateDDObjects: RealLx=%d, RealLy=%d, window_mode=%d, hwnd=%p\n", 
          RealLx, RealLy, window_mode, hwnd);
    
    // Get desktop resolution for fullscreen scaling
    if (!window_mode) {
        SDL_DisplayMode desktopMode;
        if (SDL_GetDesktopDisplayMode(0, &desktopMode) == 0) {
            desktopWidth = desktopMode.w;
            desktopHeight = desktopMode.h;
            DDLog("Desktop resolution: %dx%d @ %dHz\n", 
                  desktopWidth, desktopHeight, desktopMode.refresh_rate);
            
            // Calculate integer scale factor
            scaleFactor = CalculateIntegerScale(desktopWidth, desktopHeight, RealLx, RealLy);
        } else {
            DDLog("Failed to get desktop display mode: %s\n", SDL_GetError());
            desktopWidth = RealLx;
            desktopHeight = RealLy;
            scaleFactor = 1;
        }
    }
    
    // Clean up existing objects if they exist
    if (sdlWindow != NULL) {
        DDLog("SDL objects already exist - performing full cleanup and recreation\n");
        
        if (offScreenPtr) {
            free(offScreenPtr);
            offScreenPtr = NULL;
        }
        
        if (sdlTexture) {
            SDL_DestroyTexture(sdlTexture);
            sdlTexture = NULL;
        }
        
        if (sdlSurface) {
            SDL_FreeSurface(sdlSurface);
            sdlSurface = NULL;
        }
        
        if (sdlRenderer) {
            SDL_DestroyRenderer(sdlRenderer);
            sdlRenderer = NULL;
        }
        
        if (sdlWindow) {
            SDL_DestroyWindow(sdlWindow);
            sdlWindow = NULL;
        }
        
        DDLog("Cleanup complete, recreating from scratch\n");
    }
    
    // Initialize virtual screen FIRST - this sets MaxSizeX/MaxSizeY
    SVSC.SetSize(RealLx, RealLy);
    DDLog("After SVSC.SetSize: MaxSizeX=%d, MaxSizeY=%d\n", MaxSizeX, MaxSizeY);
    
    if (MaxSizeX == 0 || MaxSizeY == 0) {
        DDLog("ERROR: MaxSizeX or MaxSizeY is 0 after SVSC.SetSize!\n");
        DDError = true;
        return false;
    }
    
    // Create window from HWND
    DDLog("Creating SDL window from HWND\n");
    sdlWindow = SDL_CreateWindowFrom((void*)hwnd);
    
    if (!sdlWindow) {
        char buf[256];
        sprintf(buf, "SDL_CreateWindowFrom failed: %s\n", SDL_GetError());
        DDLog("%s", buf);
        MessageBox(hwnd, buf, "ERROR", MB_OK);
        return false;
    }
    
    DDLog("Successfully attached SDL to existing window\n");
    
    // Handle fullscreen mode - use desktop fullscreen for integer scaling
    if (!window_mode) {
        DDLog("Setting up desktop fullscreen mode for integer scaling\n");
        
        // Use desktop fullscreen (borderless window at native resolution)
        if (SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
            DDLog("Desktop fullscreen enabled - will use %dx integer scaling\n", scaleFactor);
        } else {
            DDLog("Desktop fullscreen failed: %s\n", SDL_GetError());
        }
    } else {
        DDLog("Window mode - using normal windowed state\n");
        SDL_SetWindowFullscreen(sdlWindow, 0);
        SDL_SetWindowSize(sdlWindow, RealLx, RealLy);
        scaleFactor = 1; // No scaling in windowed mode
    }
    
    // Show and raise window
    SDL_ShowWindow(sdlWindow);
    SDL_RaiseWindow(sdlWindow);
    
    // Create renderer with specific flags
    DDLog("Creating SDL renderer\n");
    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, rendererFlags);
    
    if (!sdlRenderer) {
        DDLog("Accelerated renderer failed: %s, trying software\n", SDL_GetError());
        sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_SOFTWARE);
        if (!sdlRenderer) {
            char buf[256];
            sprintf(buf, "SDL2 CreateRenderer Failed: %s\n", SDL_GetError());
            DDLog("%s", buf);
            MessageBox(hwnd, buf, "ERROR", MB_OK);
            SDL_DestroyWindow(sdlWindow);
            sdlWindow = NULL;
            return false;
        }
        DDLog("Created software renderer\n");
    } else {
        DDLog("Created accelerated renderer\n");
    }
    
    // Set integer scaling hint BEFORE setting logical size
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest"); // Pixel-perfect scaling
    SDL_RenderSetIntegerScale(sdlRenderer, SDL_TRUE); // Force integer scaling
    
    // Set logical size to game resolution
    if (SDL_RenderSetLogicalSize(sdlRenderer, RealLx, RealLy) != 0) {
        DDLog("Failed to set logical size: %s\n", SDL_GetError());
    } else {
        DDLog("Set logical size: %dx%d (will be integer scaled)\n", RealLx, RealLy);
    }
    
    // Get actual viewport to verify integer scaling
    SDL_Rect viewport;
    SDL_RenderGetViewport(sdlRenderer, &viewport);
    DDLog("Renderer viewport: x=%d, y=%d, w=%d, h=%d\n", 
          viewport.x, viewport.y, viewport.w, viewport.h);
    
    // Disable relative mouse mode
    SDL_SetRelativeMouseMode(SDL_FALSE);
    
    // Configure mouse/window behavior for fullscreen
    if (!window_mode) {
        SDL_SetWindowGrab(sdlWindow, SDL_TRUE);
        DDLog("Window grab enabled for fullscreen\n");
    }
    
    // Create software buffer
    CreateSoftwareBuffer();
    
    if (DDError) {
        char buf[256];
        sprintf(buf, "Failed to create software buffer\n");
        DDLog("%s", buf);
        MessageBox(hwnd, buf, "ERROR", MB_OK);
        return false;
    }
    
    // Set up screen dimensions
    SCRSizeX = MaxSizeX;
    SCRSizeY = MaxSizeY;
    RSCRSizeX = MaxSizeX;
    COPYSizeX = RealLx;
    RSCRSizeY = RealLy;
    ScrHeight = SCRSizeY;
    ScrWidth = SCRSizeX;
    InitRLCWindows();
    
#ifndef _USE3D
    WindX = 0;
    WindY = 0;
    WindLx = RealLx;
    WindLy = RealLy;
    WindX1 = WindLx - 1;
    WindY1 = WindLy - 1;
#else
    GPS.SetClipArea(0, 0, RealLx, RealLy);
#endif
    
    BytesPerPixel = 1;
    
    SDL_ShowCursor(SDL_ENABLE);
    
    // Initial lock to set up screen pointers
    LockSurface();
    
    // Force window activation in fullscreen mode
    if (!window_mode) {
        SDL_Delay(100);
        SDL_RaiseWindow(sdlWindow);
        SDL_SetWindowInputFocus(sdlWindow);
        ProcessMessagesSDL();
        bActive = TRUE;
        DDLog("Forced bActive=TRUE for fullscreen mode\n");
    }
    
    DDLog("CreateDDObjects completed successfully with %dx integer scaling\n", scaleFactor);
    return true;
}

// CreateSoftwareBuffer - creates the rendering surfaces
void CreateSoftwareBuffer() {
    DDLog("CreateSoftwareBuffer called\n");
    DDLog("RealLx=%d, RealLy=%d, MaxSizeX=%d, MaxSizeY=%d\n", 
          RealLx, RealLy, MaxSizeX, MaxSizeY);
    
    if (MaxSizeX == 0 || MaxSizeY == 0) {
        DDLog("ERROR: MaxSizeX or MaxSizeY is 0!\n");
        DDError = true;
        return;
    }
    
    if (sdlSurface) {
        SDL_FreeSurface(sdlSurface);
        sdlSurface = NULL;
    }
    
    // Create 8-bit surface
    sdlSurface = SDL_CreateRGBSurface(0, RealLx, RealLy, 8, 0, 0, 0, 0);
    
    if (!sdlSurface) {
        DDLog("Failed to create 8-bit surface: %s\n", SDL_GetError());
        DDError = true;
        return;
    }
    
    BytesPerPixel = 1;
    DDLog("Created 8-bit display surface: %dx%d\n", RealLx, RealLy);
    
    // Initialize palette
    if (sdlSurface->format->palette) {
        for (int i = 0; i < 256; i++) {
            GPal[i].r = i;
            GPal[i].g = i;
            GPal[i].b = i;
            GPal[i].a = 255;
        }
        SDL_SetPaletteColors(sdlSurface->format->palette, GPal, 0, 256);
        DDLog("Initialized default palette\n");
    }
    
    // Allocate offscreen buffer
    if (offScreenPtr) {
        free(offScreenPtr);
        offScreenPtr = NULL;
    }
    
    offScreenPtr = malloc(MaxSizeX * (MaxSizeY + 32 * 4));
    if (!offScreenPtr) {
        DDLog("Failed to allocate offscreen buffer\n");
        DDError = true;
        return;
    }
    memset(offScreenPtr, 0, MaxSizeX * (MaxSizeY + 32 * 4));
    DDLog("Allocated offscreen buffer: %d bytes\n", MaxSizeX * (MaxSizeY + 32 * 4));
    
    // Create streaming texture
    if (sdlTexture) {
        SDL_DestroyTexture(sdlTexture);
        sdlTexture = NULL;
    }
    
    sdlTexture = SDL_CreateTexture(sdlRenderer, 
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    RealLx, RealLy);
    
    if (!sdlTexture) {
        DDError = true;
        DDLog("Failed to create texture: %s\n", SDL_GetError());
    } else {
        DDLog("Created streaming texture: %dx%d\n", RealLx, RealLy);
    }
}

#ifndef _USE3D
bool CreateRGBDDObjects(HWND hwnd)
{
    RealLx = 800;
    RealLy = 600;
    SCRSizeX = 800;
    SCRSizeY = 600;
    RSCRSizeX = 800;
    RSCRSizeY = 600;
    COPYSizeX = 800;
    SCRSZY = SCRSizeY;
    return CreateDDObjects(hwnd) ? 1 : 0;
}
#endif // _!USE3D

bool CreateRGB640DDObjects(HWND hwnd)
{
    RealLx = 640;
    RealLy = 480;
    SCRSizeX = 640;
    SCRSizeY = 480;
    RSCRSizeX = 640;
    RSCRSizeY = 480;
    COPYSizeX = 640;
    SCRSZY = SCRSizeY;
    return CreateDDObjects(hwnd) ? 1 : 0;
    return false;
}

int clrRed;
int clrGreen;
int clrBlue;
int clrYello;
int clrWhite;

CEXPORT
void LoadPalette(const char* lpFileName)
{
    if (DDError) return;
    
    ResFile pf = RReset(lpFileName);
    memset(&GPal, 0, sizeof(GPal));
    
    if (pf != INVALID_HANDLE_VALUE) {
        for (int i = 0; i < 256; i++) {
            byte rgb[3];
            RBlockRead(pf, rgb, 3);
            GPal[i].r = rgb[0];
            GPal[i].g = rgb[1];
            GPal[i].b = rgb[2];
            GPal[i].a = 255;
        }
        RClose(pf);
        
        UpdateSDLPalette();
        PalDone = true;
        
        DDLog("Loaded palette from %s\n", lpFileName);
    }
    
    clrRed = 0xD0;
    clrWhite= GetPaletteColor(255,255,255);
    clrGreen = GetPaletteColor(0,255,0);
    clrBlue = GetPaletteColor(0, 0, 255);
    clrYello = GetPaletteColor(255, 255, 0);
}

void CBar(int x, int y, int Lx, int Ly, byte c);
void Susp(char* str){
	return;
	if(!window_mode){
		void* oldsof=ScreenPtr;
		ScreenPtr=RealScreenPtr;
		int ScLx=ScrWidth;
		ScrWidth=RealLx;
		CBar(700,1,100,5,0x93);
		ShowString(700,0,str,&fn8);
		ScrWidth=ScLx;
		ScreenPtr=oldsof;
	}else{
		CBar(700,1,100,5,0x93);
		ShowString(700,0,str,&fn8);

		HDC WH=GetWindowDC(hwnd);

		xxt.bmp.bmiHeader.biSize=sizeof BITMAPINFOHEADER;
		xxt.bmp.bmiHeader.biWidth=SCRSizeX;
		xxt.bmp.bmiHeader.biHeight=SCRSizeY;
		xxt.bmp.bmiHeader.biPlanes=1;
		xxt.bmp.bmiHeader.biBitCount=8;
		xxt.bmp.bmiHeader.biCompression=BI_RGB;
		xxt.bmp.bmiHeader.biSizeImage=0;
		int z=StretchDIBits(WH,700,1,100,5,
			700,1,100,5,ScreenPtr,&xxt.bmp,
			DIB_RGB_COLORS,SRCCOPY);
			
	};
};
void SetDarkPalette() {
#ifndef _USE3D
    if (DDError) return;
    ChangeColorFF();
    memset(&GPal, 0, sizeof(GPal));
    
    // Apply dark palette
    UpdateSDLPalette();
#endif //_USE3D
}

CEXPORT
void SlowLoadPalette(LPCSTR lpFileName)
{
#ifdef _USE3D
    LoadPalette(lpFileName);
    return;
#endif
    
    SDL_Color NPal[256];
    if (DDError) return;
    SetDarkPalette();
    
    ResFile pf = RReset(lpFileName);
    memset(&GPal, 0, sizeof(GPal));
    
    if (pf != INVALID_HANDLE_VALUE) {
        for (int i = 0; i < 256; i++) {
            byte rgb[3];
            RBlockRead(pf, rgb, 3);
            GPal[i].r = rgb[0];
            GPal[i].g = rgb[1];
            GPal[i].b = rgb[2];
        }
        RClose(pf);

        // Fade in palette
        if (sdlSurface && sdlSurface->format->palette) {
            DWORD t0 = GetTickCount();
            int mul0 = 0;
            
            do {
                int mul = (GetTickCount() - t0) * 2;
                if (mul > 255) mul = 255;
                
                if (mul != mul0) {
                    for (int j = 0; j < 256; j++) {
                        NPal[j].r = (GPal[j].r * mul) >> 8;
                        NPal[j].g = (GPal[j].g * mul) >> 8;
                        NPal[j].b = (GPal[j].b * mul) >> 8;
                    }
                    
                    SDL_SetPaletteColors(sdlSurface->format->palette, NPal, 0, 256);
                    mul0 = mul;
                }
                
                SDL_Delay(10);
            } while (mul0 < 255);
        }
    }
    clrRed = 0xD0;
    clrWhite= GetPaletteColor(255,255,255);
    clrGreen = GetPaletteColor(0,255,0);
    clrBlue = GetPaletteColor(0, 0, 255);
    clrYello = GetPaletteColor(255, 255, 0);
}

CEXPORT
void SlowUnLoadPalette(LPCSTR lpFileName)
{
    SDL_Color NPal[256];
    if (DDError) return;
    ChangeColorFF();
    
    if (sdlSurface && sdlSurface->format->palette) {
        DWORD t0 = GetTickCount();
        int mul0 = 0;
        
        do {
            int mul = (GetTickCount() - t0) * 2;
            if (mul > 255) mul = 255;
            
            if (mul != mul0) {
                for (int j = 0; j < 256; j++) {
                    NPal[j].r = (GPal[j].r * (255 - mul)) >> 8;
                    NPal[j].g = (GPal[j].g * (255 - mul)) >> 8;
                    NPal[j].b = (GPal[j].b * (255 - mul)) >> 8;
                }
                
                SDL_SetPaletteColors(sdlSurface->format->palette, NPal, 0, 256);
                mul0 = mul;
            }
            
            SDL_Delay(10);
        } while (mul0 < 255);
    }
}
void FreeDDObjects(void)
{
    DDLog("FreeDDObjects called\n");
    
    if (offScreenPtr) {
        free(offScreenPtr);
        offScreenPtr = NULL;
    }
    
    if (sdlTexture) {
        SDL_DestroyTexture(sdlTexture);
        sdlTexture = NULL;
    }
    
    if (sdlSurface) {
        SDL_FreeSurface(sdlSurface);
        sdlSurface = NULL;
    }
    
    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = NULL;
    }
    
    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = NULL;
    }
    
    SDL_Quit();
}
/*void SetDebugMode()
{
    DDDebug =true;
}
void NoDebugMode()
{
    DDDebug =false;
}*/

CEXPORT

void GetPalColor(byte idx, byte* r, byte* g, byte* b) {
    *r = GPal[idx].r;
    *g = GPal[idx].g;
    *b = GPal[idx].b;
}


/*
    DirectDraw substitute.
    Uses mdraw.dll instead of the original, ddraw.lib exported DirectDrawCreate().
    Prevents the color palette corruption bug in modern Windows systems.
    No idea what the mdraw.dll funtion does, but you end up with a working
    IDirectDraw interface and no legacy bugs.
*/
#ifdef WRAPPER
HRESULT DirectDrawCreate_wrapper(GUID FAR* lpGUID, LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter)
{
    HMODULE mdrawHandle = LoadLibrary("mdraw.dll");
    if (NULL != mdrawHandle)
    {
        typedef HRESULT(__stdcall* mdrawProcType)(GUID FAR* lpGUID, LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter);
        mdrawProcType mdrawProc = (mdrawProcType)GetProcAddress(mdrawHandle, "DirectDrawCreate");
        if (NULL != mdrawProc)
        {
            HRESULT mdrawResult = mdrawProc(lpGUID, lplpDD, pUnkOuter);
            return mdrawResult;
        }
        FreeLibrary(mdrawHandle);
    }
    return DDERR_GENERIC;
}
#endif
//OVO SAM UKRAO OD COSSACK REVAMPA

#ifdef _USE3D
BaseMesh BMS;
bool init=0;
void TnLset(VertexTnL* V,float x,float y,float z,float u,float v,DWORD D,DWORD S){
	V->x=x;
	V->y=y;
	V->z=z;
	V->w=1.0;
	V->u=u;
	V->v=v;
	V->diffuse=D;
	V->specular=S;
};
void TnLset2(VertexTnL2* V,float x,float y,float z,float u,float v,float u2,float v2,DWORD D){
	V->x=x+100;
	V->y=y+100;
	V->z=z;
	V->w=1.0;
	V->u=u;
	V->v=v;
	V->u2=u2;
	V->v2=v2;
	V->diffuse=D;
};
void TestPiro();
float fdx=0;
float fdy=0;
extern int mouseX;
extern int mouseY;
extern int tmtmt;
void Test3D(){
	//Test4Piro();
	//int Id=GPS.PreLoadGPImage("list");
	//GPS.ShowGP(200,200,Id,tmtmt%88,0);

	return;
	//if(!init){
		//if(GetKeyState('A')&0x8000)fdx-=0.001;
		//if(GetKeyState('D')&0x8000)fdx+=0.001;
		//if(GetKeyState('W')&0x8000)fdy-=0.001;
		//if(GetKeyState('X')&0x8000)fdy+=0.001;
		fdx=float(mouseX-100-128)/1500.0;
		fdy=float(mouseY-100-32)/1500.0;

		BMS.create(256,256,vfTnL2,ptTriangleList,false);
		word ID=IRS->GetShaderID("road");
		word TID=IRS->GetTextureID("road.tga");
		BMS.setShader(ID);
		BMS.setTexture(TID);
		BMS.setTexture(TID,1);
		VertexTnL2* VTX=(VertexTnL2*)BMS.getVertexData();
		word* IDX=BMS.getIndices();

		TnLset2(VTX+0,  0  ,  0,   0,   0,   0,      0,   0,0xFFFFFFFF);
		TnLset2(VTX+1,  256,  0,   0,   1,   0,1.0+fdx,   0,0xFFFFFFFF);
		TnLset2(VTX+2,  0  ,256,   0,   0,   1,   0, 1.0+fdy,0xFFFFFFFF);
		TnLset2(VTX+3,  256,256,   0,   1,   1,1+fdx, 1.0+fdy,0xFFFFFFFF);

		IDX[0]=0;
		IDX[1]=1;
		IDX[2]=2;
		IDX[3]=1;
		IDX[4]=3;
		IDX[5]=2;
		BMS.setNInd(6);
		BMS.setNVert(4);
		init=1;
	//};
	IRS->Draw(BMS);
};
#endif