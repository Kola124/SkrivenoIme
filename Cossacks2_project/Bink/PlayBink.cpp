#include <windows.h>
#include "..\include\SDL.h"
#include "rad.h"
#include "bink.h"

static void Clear_to_black(HWND window)
{
    PAINTSTRUCT ps;
    HDC dc;

    dc = BeginPaint(window, &ps);
    PatBlt(dc, 0, 0, 4096, 4096, BLACKNESS);
    EndPaint(window, &ps);
}

static void Calc_window_values(HWND window,
    S32* out_window_x,
    S32* out_window_y,
    S32* out_client_x,
    S32* out_client_y,
    S32* out_extra_width,
    S32* out_extra_height)
{
    RECT r, c;
    POINT p;

    p.x = 0;
    p.y = 0;
    ClientToScreen(window, &p);

    *out_window_x = p.x;
    *out_window_y = p.y;

    GetWindowRect(window, &r);

    *out_client_x = p.x - r.left;
    *out_client_y = p.y - r.top;

    GetClientRect(window, &c);

    *out_extra_width = (r.right - r.left) - (c.right - c.left);
    *out_extra_height = (r.bottom - r.top) - (c.bottom - c.top);
}

extern bool KeyPressed;
extern bool Lpressed;
int BinkT0 = 0;
bool MUSTEXIT = 0;

// SDL2 replacements for DirectDraw objects
extern SDL_Window* sdlWindow;
extern SDL_Renderer* sdlRenderer;
extern SDL_Texture* sdlTexture;
extern SDL_Surface* sdlSurface;

bool Show_next_frame(HBINK bink,
    HWND window,
    SDL_Surface* surface,
    S32 window_x,
    S32 window_y)
{
    // Lock the surface if needed
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    // Get surface info
    int pitch = surface->pitch;
    void* pixels = surface->pixels;
    int height = surface->h;

    // Copy Bink frame to SDL surface
    s32 res = BinkCopyToBuffer(bink,
        pixels,
        pitch,
        height,
        window_x, window_y,
        BINKSURFACE32);  // Use 32-bit surface

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }

    if (bink->FrameNum >= bink->Frames - 2) {
        MUSTEXIT = 1;
        return false;
    }
    else {
        if (KeyPressed || Lpressed) {
            MUSTEXIT = 1;
        };
        KeyPressed = 0;
        Lpressed = 0;
        if (MUSTEXIT && GetTickCount() - BinkT0 > 3000) return false;
        BinkNextFrame(bink);
        return true;
    };
}

static void Good_sleep_us(S32 microseconds)
{
    static S32 total_sleep = 0;
    static S32 slept_in_advance = 0;
    static U64 frequency = 1000;
    static S32 got_frequency = 0;

    if (!got_frequency)
    {
        got_frequency = 1;
        QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    }

    total_sleep += microseconds;

    if ((total_sleep - slept_in_advance) > 1000)
    {
        U64 start, end;
        total_sleep -= slept_in_advance;

        QueryPerformanceCounter((LARGE_INTEGER*)&start);
        Sleep(total_sleep / 1000);
        QueryPerformanceCounter((LARGE_INTEGER*)&end);

        end = ((end - start) * (U64)1000000) / frequency;

        slept_in_advance = (U32)end - total_sleep;
        total_sleep %= 1000;
    }
}

extern HWND hwnd;
static HBINK Bink = 0;
static S32 Window_x, Window_y;
static S32 Client_offset_x, Client_offset_y;

void ClearRGB();
void FlipPages();

void PlayBinkFile(char* path) {
    if (MUSTEXIT) return;
    BinkT0 = GetTickCount();

    if (!sdlSurface) return;

    try {
        ClearRGB();
        HWND window = hwnd;
        S32 extra_width, extra_height;
        HCURSOR cursor = 0;
        MSG msg;

        Calc_window_values(window,
            &Window_x, &Window_y,
            &Client_offset_x, &Client_offset_y,
            &extra_width, &extra_height);

        // Initialize Bink
        s32 res = BinkSoundUseDirectSound(0);
        Bink = BinkOpen(path, BINKNOSKIP);

        if (Bink) {
            // Create a texture for Bink video
            SDL_Texture* binkTexture = NULL;
            if (sdlRenderer) {
                // Create a streaming texture for Bink video
                binkTexture = SDL_CreateTexture(sdlRenderer,
                    SDL_PIXELFORMAT_RGBA32,
                    SDL_TEXTUREACCESS_STREAMING,
                    Bink->Width, Bink->Height);
            }

            bool running = true;
            while (running) {
                // Handle Windows messages
                if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
                    if (msg.message == WM_QUIT)
                        break;
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else {
                    // Check if it's time for next frame
                    if (!BinkWait(Bink)) {
                        // Process the Bink frame
                        if (binkTexture) {
                            // Lock texture for direct pixel access
                            void* pixels;
                            int pitch;
                            if (SDL_LockTexture(binkTexture, NULL, &pixels, &pitch) == 0) {
                                // Copy Bink frame directly to texture
                                BinkCopyToBuffer(Bink,
                                    pixels,
                                    pitch,
                                    Bink->Height,
                                    0, 0,
                                    BINKSURFACE32R);  // 32-bit RGB

                                SDL_UnlockTexture(binkTexture);

                                // Clear renderer and draw texture
                                SDL_RenderClear(sdlRenderer);
                                
                                // Center the video
                                SDL_Rect dstRect;
                                int windowWidth, windowHeight;
                                SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight);
                                
                                // Maintain aspect ratio
                                float aspect = (float)Bink->Width / (float)Bink->Height;
                                dstRect.h = windowHeight;
                                dstRect.w = (int)(windowHeight * aspect);
                                if (dstRect.w > windowWidth) {
                                    dstRect.w = windowWidth;
                                    dstRect.h = (int)(windowWidth / aspect);
                                }
                                dstRect.x = (windowWidth - dstRect.w) / 2;
                                dstRect.y = (windowHeight - dstRect.h) / 2;
                                
                                SDL_RenderCopy(sdlRenderer, binkTexture, NULL, &dstRect);
                                SDL_RenderPresent(sdlRenderer);
                            }
                        }
                        else {
                            // Fallback to surface rendering
                            if (!Show_next_frame(Bink,
                                window,
                                sdlSurface,
                                Window_x,
                                Window_y + (480 - Bink->Height) / 2)) {
                                running = false;
                            }
                        }

                        // Advance to next frame
                        if (running) {
                            if (Bink->FrameNum >= Bink->Frames - 2) {
                                running = false;
                            }
                            else {
                                if (KeyPressed || Lpressed) {
                                    MUSTEXIT = 1;
                                }
                                KeyPressed = 0;
                                Lpressed = 0;
                                if (MUSTEXIT && GetTickCount() - BinkT0 > 3000) {
                                    running = false;
                                }
                                BinkNextFrame(Bink);
                            }
                        }
                    }
                    else {
                        // Sleep a bit
                        Good_sleep_us(500);
                    }
                }
            }

            // Cleanup
            if (binkTexture) {
                SDL_DestroyTexture(binkTexture);
            }

            // Close Bink
            BinkClose(Bink);
            Bink = 0;
        }
    }
    catch (...) {
        if (Bink) {
            BinkClose(Bink);
            Bink = 0;
        }
    };
    
    // Force screen redraw after Bink playback
    FlipPages();
}