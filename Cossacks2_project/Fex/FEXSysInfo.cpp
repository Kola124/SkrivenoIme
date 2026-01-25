//#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#define COMPILE_MULTIMON_STUBS
#include "Include\\multimon.h"
#include "Include\\basetsd.h"
#include "FEXModule.h"
#include "..\include\SDL.h"

extern char FEX_SystemVersionString[1024];

//-----------------------------------------------------------------------------
// Name: FEX_GetDXVersion()
// Desc: For SDL2 migration, we return a fake version or check for SDL2
//-----------------------------------------------------------------------------
DWORD FEX_GetDXVersion()
{
    // For SDL2 migration, we can either:
    // 1. Return a fake version to keep old code happy
    // 2. Check for SDL2 version
    // 3. Always return a high version number
    
    // Option 1: Return a high version number to indicate "modern graphics"
    return 0x0900; // Return "DirectX 9" equivalent
    
    // Option 2: Actually check SDL2 version
    /*
    SDL_version compiled;
    SDL_version linked;
    
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    
    // Convert SDL version to a format similar to DirectX version
    DWORD sdlVersion = (linked.major << 8) | (linked.minor << 4) | linked.patch;
    
    // Map SDL version to approximate DirectX equivalent
    if (sdlVersion >= 0x20200) // SDL 2.2.0+
        return 0x0B00; // DX11 equivalent
    else if (sdlVersion >= 0x20000) // SDL 2.0.0+
        return 0x0900; // DX9 equivalent
    else
        return 0x0800; // DX8 equivalent
    */
}
//-----------------------------------------------------------------------------
bool FEX_GetFreeSpace(int DiskNum, __int64& i64TotalBytes, __int64& i64FreeBytes)
{
    char _disk[8];
    char* disk = _disk;
    disk[0] = 'a';
    disk[1] = ':';
    disk[2] = '\\';
    disk[3] = 0;

    if(DiskNum == -1)
        disk = NULL;
    else
        disk[0] += DiskNum;
    
    __int64 i64FreeBytesToCaller;
    GetDiskFreeSpaceEx(disk, (PULARGE_INTEGER)&i64FreeBytesToCaller,
                                     (PULARGE_INTEGER)&i64TotalBytes,
                                     (PULARGE_INTEGER)&i64FreeBytes);
    return true;
}
//-----------------------------------------------------------------------------
void FEX_ADD_STR(char* str, ...) 
{
    static char tmp[256];
    if(!str)    return;

    va_list args;
    va_start(args, str);
    vsprintf(tmp, str, args);
    va_end(args);
    strcat(FEX_SystemVersionString, tmp);
}
//-----------------------------------------------------------------------------
char* FEX_GetSystemVersion(void)
{
    #define BUFSIZE 80

    struct myOSVERSIONINFOEX {
        DWORD dwOSVersionInfoSize;
        DWORD dwMajorVersion;
        DWORD dwMinorVersion;
        DWORD dwBuildNumber;
        DWORD dwPlatformId;
        TCHAR szCSDVersion[ 128 ];
        WORD wServicePackMajor;
        WORD wServicePackMinor;
        WORD wSuiteMask;
        BYTE wProductType;
        BYTE wReserved;
    };
   OSVERSIONINFOEX osvi;

   BOOL bOsVersionInfoEx;

    FEX_SystemVersionString[0] = 0;
   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if(!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi)))
   {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if(!GetVersionEx((OSVERSIONINFO*)&osvi)) 
            return NULL;
   }

   switch(osvi.dwPlatformId)
   {
      // Tests for Windows NT product family.
      case VER_PLATFORM_WIN32_NT:

            // Test for the product.
         if(osvi.dwMajorVersion <= 4)
            FEX_ADD_STR("Microsoft Windows NT ");
            
         if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
            FEX_ADD_STR("Microsoft Windows 2000 ");

         if(bOsVersionInfoEx)  // Use information from GetVersionEx.
         { 
                // Test for the workstation type.
            if((*(myOSVERSIONINFOEX*)&osvi).wProductType == 0x0000001)//VER_NT_WORKSTATION)
            {
               if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
                  FEX_ADD_STR("Microsoft Windows XP ");

               if((*(myOSVERSIONINFOEX*)&osvi).wSuiteMask & 0x00000200)//VER_SUITE_PERSONAL)
                  FEX_ADD_STR("Home Edition ");
               else
                  FEX_ADD_STR("Professional ");
            }
            // Test for the server type.
            else 
                if((*(myOSVERSIONINFOEX*)&osvi).wProductType == 0x0000003)//VER_NT_SERVER)
            {
               if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
                  FEX_ADD_STR("Microsoft Windows .NET ");

               if((*(myOSVERSIONINFOEX*)&osvi).wSuiteMask & 0x00000080)//VER_SUITE_DATACENTER)
                  FEX_ADD_STR("DataCenter Server ");
               else 
                    {
                        if((*(myOSVERSIONINFOEX*)&osvi).wSuiteMask & 0x00000002)//VER_SUITE_ENTERPRISE)
                        {
                            if(osvi.dwMajorVersion == 4)
                                FEX_ADD_STR("Advanced Server ");
                            else
                                FEX_ADD_STR("Enterprise Server ");
                        }
                else 
                        {
                            if((*(myOSVERSIONINFOEX*)&osvi).wSuiteMask == 0x00000400)//VER_SUITE_BLADE)
                                FEX_ADD_STR("Web Server ");
                            else
                                FEX_ADD_STR("Server ");
                        }
                    }
            }
         }
         else   // Use the registry on early versions of Windows NT.
         {
            HKEY hKey;
            char szProductType[BUFSIZE];
            DWORD dwBufLen=BUFSIZE;
            LONG lRet;

            lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
               "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
               0, KEY_QUERY_VALUE, &hKey);
            if(lRet != ERROR_SUCCESS)
                return NULL;

            lRet = RegQueryValueEx(hKey, "ProductType", NULL, NULL,
               (LPBYTE) szProductType, &dwBufLen);
            if((lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE))
                return NULL;

            RegCloseKey(hKey);

            if(lstrcmpi("WINNT", szProductType) == 0)
               FEX_ADD_STR("Professional ");
            if(lstrcmpi("LANMANNT", szProductType) == 0)
               FEX_ADD_STR("Server ");
            if(lstrcmpi("SERVERNT", szProductType) == 0)
               FEX_ADD_STR("Advanced Server ");
         }

            // Display version, service pack (if any), and build number.
         if(osvi.dwMajorVersion <= 4)
         {
            FEX_ADD_STR("version %d.%d %s (Build %d)",
               osvi.dwMajorVersion,
               osvi.dwMinorVersion,
               osvi.szCSDVersion,
               osvi.dwBuildNumber & 0xFFFF);
         }
         else
         { 
            FEX_ADD_STR("%s (Build %d)",
               osvi.szCSDVersion,
               osvi.dwBuildNumber & 0xFFFF);
         }
         break;
      // Test for the Windows 95 product family.
      case VER_PLATFORM_WIN32_WINDOWS:
         if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
         {
             FEX_ADD_STR("Microsoft Windows 95 ");
             if(osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B')
                FEX_ADD_STR("OSR2 ");
         } 

         if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
         {
             FEX_ADD_STR("Microsoft Windows 98 ");
             if(osvi.szCSDVersion[1] == 'A')
                FEX_ADD_STR("SE ");
         } 

         if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
         {
             FEX_ADD_STR("Microsoft Windows Millennium Edition ");
         } 
         break;
   }
   
   // Add SDL2 version info
   SDL_version compiled;
   SDL_version linked;
   
   SDL_VERSION(&compiled);
   SDL_GetVersion(&linked);
   
   FEX_ADD_STR("\nSDL Version: Compiled %d.%d.%d, Linked %d.%d.%d",
       compiled.major, compiled.minor, compiled.patch,
       linked.major, linked.minor, linked.patch);
   
   return FEX_SystemVersionString;
}
//-----------------------------------------------------------------------------
char* FEX_GetDisplayName(void)
{
    struct DISPLAY_DEVICE_FULL
    {
        DWORD  cb;
        TCHAR  DeviceName[32];
        TCHAR  DeviceString[128];
        DWORD  StateFlags;
        TCHAR  DeviceID[128];
        TCHAR  DeviceKey[128];
    };
    DISPLAY_DEVICE_FULL dd;
   ZeroMemory(&dd, sizeof(dd));
   dd.cb = sizeof(dd);
    static char str[1024];
    str[0] = 0;

   HINSTANCE hUser32 = LoadLibrary("user32.dll");
   if(hUser32 == NULL)
   {
      OutputDebugString("Couldn't LoadLibrary user32.dll\r\n");
      return str;
   }

    typedef BOOL (WINAPI *EnumDisplayDevicesFn)(
        LPCTSTR lpDevice,               
        DWORD iDevNum,                  
        PDISPLAY_DEVICE lpDisplayDevice,
        DWORD dwFlags);

    EnumDisplayDevicesFn g_pfnEnumDisplayDevices;

    g_pfnEnumDisplayDevices = (EnumDisplayDevicesFn)GetProcAddress(hUser32, 
                                                                    "EnumDisplayDevicesA");

    if(!g_pfnEnumDisplayDevices)
   {
      OutputDebugString("Couldn't GetProcAddress of EnumDisplayDevicesA\r\n");
      FreeLibrary(hUser32);
      return str;
   }

   for(int i = 0; g_pfnEnumDisplayDevices(NULL, i, (DISPLAY_DEVICE*)&dd, 0); i++)
   {
        // Ignore NetMeeting's mirrored displays
      if((dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) == 0)
      {
            strcat(str, dd.DeviceString);
            strcat(str, "\n");
        }
   }
   FreeLibrary(hUser32);
    
    // Add SDL video driver info
    SDL_RendererInfo rendererInfo;
    int numRenderers = SDL_GetNumRenderDrivers();
    
    if (numRenderers > 0) {
        strcat(str, "\nSDL Renderers:\n");
        for (int i = 0; i < numRenderers; i++) {
            if (SDL_GetRenderDriverInfo(i, &rendererInfo) == 0) {
                strcat(str, "  ");
                strcat(str, rendererInfo.name);
                strcat(str, "\n");
            }
        }
    }
    
    return str;
}
//-----------------------------------------------------------------------------
// Winmm.lib is required!
//-----------------------------------------------------------------------------
char* FEX_GetAudioName(void)
{
    WAVEINCAPS wic;
    static char str[(MAXPNAMELEN+1)*16+1] = "";
    for(int i = 0; i < 16; i++)
    {
        ZeroMemory(&wic, sizeof(WAVEINCAPS));
        if(waveInGetDevCaps(i, &wic, sizeof(WAVEINCAPS)) != MMSYSERR_NOERROR) break;
        strcat(str, wic.szPname);
        strcat(str, "\n");
    }
    
    // Add SDL audio driver info
    int numAudioDrivers = SDL_GetNumAudioDrivers();
    if (numAudioDrivers > 0) {
        strcat(str, "\nSDL Audio Drivers:\n");
        for (int i = 0; i < numAudioDrivers; i++) {
            const char* driverName = SDL_GetAudioDriver(i);
            if (driverName && strlen(driverName) > 0) {
                strcat(str, "  ");
                strcat(str, driverName);
                strcat(str, "\n");
            }
        }
    }
    
    // Add current SDL audio driver
    const char* currentAudioDriver = SDL_GetCurrentAudioDriver();
    if (currentAudioDriver) {
        strcat(str, "\nCurrent SDL Audio Driver: ");
        strcat(str, currentAudioDriver);
        strcat(str, "\n");
    }
    
    return str;
}
//-----------------------------------------------------------------------------
// New function to get SDL2-specific info
//-----------------------------------------------------------------------------
char* FEX_GetSDLInfo(void)
{
    static char sdlInfo[1024] = "";
    sdlInfo[0] = 0;
    
    // Get SDL version
    SDL_version compiled;
    SDL_version linked;
    
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    
    sprintf(sdlInfo, "SDL Version:\n");
    sprintf(sdlInfo + strlen(sdlInfo), "  Compiled: %d.%d.%d\n", 
            compiled.major, compiled.minor, compiled.patch);
    sprintf(sdlInfo + strlen(sdlInfo), "  Linked: %d.%d.%d\n\n", 
            linked.major, linked.minor, linked.patch);
    
    // Get video drivers
    sprintf(sdlInfo + strlen(sdlInfo), "Video Drivers:\n");
    int numVideoDrivers = SDL_GetNumVideoDrivers();
    for (int i = 0; i < numVideoDrivers; i++) {
        const char* driverName = SDL_GetVideoDriver(i);
        if (driverName) {
            sprintf(sdlInfo + strlen(sdlInfo), "  %s\n", driverName);
        }
    }
    
    // Get current video driver
    const char* currentVideoDriver = SDL_GetCurrentVideoDriver();
    if (currentVideoDriver) {
        sprintf(sdlInfo + strlen(sdlInfo), "\nCurrent Video Driver: %s\n", currentVideoDriver);
    }
    
    // Get renderer info
    SDL_RendererInfo rendererInfo;
    int numRenderers = SDL_GetNumRenderDrivers();
    if (numRenderers > 0) {
        sprintf(sdlInfo + strlen(sdlInfo), "\nAvailable Renderers:\n");
        for (int i = 0; i < numRenderers; i++) {
            if (SDL_GetRenderDriverInfo(i, &rendererInfo) == 0) {
                sprintf(sdlInfo + strlen(sdlInfo), "  %s", rendererInfo.name);
                
                // Add flags info
                if (rendererInfo.flags & SDL_RENDERER_SOFTWARE)
                    strcat(sdlInfo, " (Software)");
                if (rendererInfo.flags & SDL_RENDERER_ACCELERATED)
                    strcat(sdlInfo, " (Accelerated)");
                if (rendererInfo.flags & SDL_RENDERER_PRESENTVSYNC)
                    strcat(sdlInfo, " (VSync)");
                if (rendererInfo.flags & SDL_RENDERER_TARGETTEXTURE)
                    strcat(sdlInfo, " (Target Textures)");
                
                strcat(sdlInfo, "\n");
            }
        }
    }
    
    // Get display info
    int numDisplays = SDL_GetNumVideoDisplays();
    sprintf(sdlInfo + strlen(sdlInfo), "\nDisplays: %d\n", numDisplays);
    
    for (int i = 0; i < numDisplays; i++) {
        SDL_DisplayMode currentMode;
        if (SDL_GetCurrentDisplayMode(i, &currentMode) == 0) {
            sprintf(sdlInfo + strlen(sdlInfo), 
                    "  Display %d: %dx%d @ %dHz, %s\n",
                    i, 
                    currentMode.w, currentMode.h,
                    currentMode.refresh_rate,
                    SDL_GetPixelFormatName(currentMode.format));
        }
    }
    
    return sdlInfo;
}
//-----------------------------------------------------------------------------
// Function to check if SDL2 was initialized successfully
//-----------------------------------------------------------------------------
bool FEX_IsSDLInitialized(void)
{
    return (SDL_WasInit(0) != 0);
}
//-----------------------------------------------------------------------------
// Function to get SDL initialization flags
//-----------------------------------------------------------------------------
Uint32 FEX_GetSDLInitFlags(void)
{
    return SDL_WasInit(SDL_INIT_EVERYTHING);
}
//-----------------------------------------------------------------------------
// Function to get SDL audio device count
//-----------------------------------------------------------------------------
int FEX_GetSDLAudioDeviceCount(bool capture)
{
    return SDL_GetNumAudioDevices(capture ? 1 : 0);
}
//-----------------------------------------------------------------------------