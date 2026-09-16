///////////////////////////////////////////////////////////
// OGGVOR.CPP -- in-process OGG music playback via SDL_mixer
//
// This used to launch a separate helper process (vopl.exe) and talk
// to it through a named shared memory mapping + mutex (see the old
// vopl_globals.h). That's gone now: SDL_mixer already knows how to
// stream Ogg Vorbis directly (Mix_Music), so music plays on the same
// audio device as the WAV sound effects (see Cdirsnd.cpp), inside
// this process, with no IPC and no second executable to ship.
//
// The public API below (ov_Init/ov_Play/ov_Stop/...) is unchanged on
// purpose, so nothing else in the codebase (PlayMP3.cpp, DeviceCD.cpp,
// Ddex1.cpp) needs to change.
///////////////////////////////////////////////////////////

#include <..\include\SDL.h>
#include <..\include\SDL_mixer.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "oggvor.h"

static Mix_Music*  g_pMusic          = NULL;
static volatile BOOL g_bStreamFinished = TRUE;
static int          g_iVolume        = MIX_MAX_VOLUME; // SDL_mixer scale: 0-128

///////////////////////////////////////////////////////////
// Called by SDL_mixer (may be on the audio thread) when the
// current track finishes playing on its own. We only ever set
// a flag here -- no SDL_mixer calls -- so it's safe regardless
// of which thread invokes it.
///////////////////////////////////////////////////////////
static void OnMusicFinishedHook()
{
	g_bStreamFinished = TRUE;
}

///////////////////////////////////////////////////////////
// ov_Init()
//
// The audio device itself (SDL_Init(SDL_INIT_AUDIO) + Mix_OpenAudio)
// is owned by CSDLSound::CreateSDLSound() in Cdirsnd.cpp, which is
// called right after this from Ddex1.cpp's doInit(). There's nothing
// left to spawn or synchronize with here -- just register the
// finished-callback. hExtWnd is kept only so the existing call site
// doesn't need to change; it's unused now.
///////////////////////////////////////////////////////////
void ov_Init(HWND hExtWnd)
{
	Mix_HookMusicFinished(OnMusicFinishedHook);
}

///////////////////////////////////////////////////////////
// ov_Play()
///////////////////////////////////////////////////////////
void ov_Play(LPCSTR pcszFileName)
{
	// Stop and free whatever track was loaded before.
	Mix_HaltMusic();
	if (g_pMusic)
	{
		Mix_FreeMusic(g_pMusic);
		g_pMusic = NULL;
	}

	g_pMusic = Mix_LoadMUS(pcszFileName);
	if (!g_pMusic)
	{
		// Bad/missing file: behave as if the stream is already finished
		// so PlayMP3.cpp's random picker moves on to the next track.
		g_bStreamFinished = TRUE;
		return;
	}

	g_bStreamFinished = FALSE;
	Mix_VolumeMusic(g_iVolume);
	Mix_PlayMusic(g_pMusic, 1); // play once; PlayMP3.cpp drives track changes itself
}

///////////////////////////////////////////////////////////
// ov_Stop()
///////////////////////////////////////////////////////////
void ov_Stop(void)
{
	Mix_HaltMusic();
	if (g_pMusic)
	{
		Mix_FreeMusic(g_pMusic);
		g_pMusic = NULL;
	}
	g_bStreamFinished = TRUE;
}

///////////////////////////////////////////////////////////
// ov_Done()
///////////////////////////////////////////////////////////
void ov_Done(void)
{
	ov_Stop();
	Mix_HookMusicFinished(NULL);
}

///////////////////////////////////////////////////////////
// ov_SetVolume()
//
// Callers (PlayMP3.cpp, via MidiSound/Vol) pass a plain 0-100
// linear volume, NOT the -10000..0 dB-style scale that
// CSDLSound::SetVolume() uses for sound effects. Keep that
// distinction explicit here rather than silently reusing the
// effects conversion curve.
///////////////////////////////////////////////////////////
void ov_SetVolume(DWORD dwVolume)
{
	int vol = (int)dwVolume;
	if (vol < 0)   vol = 0;
	if (vol > 100) vol = 100;

	g_iVolume = (vol * MIX_MAX_VOLUME) / 100;
	Mix_VolumeMusic(g_iVolume);
}

///////////////////////////////////////////////////////////
// ov_GetStreamLength()
//
// Returns length in milliseconds. Mix_MusicDuration() only exists in
// SDL_mixer 2.6+; on older SDL_mixer this stays a stub returning 0,
// same as it effectively was before (nothing in the given code paths
// consumes this value).
///////////////////////////////////////////////////////////
DWORD ov_GetStreamLength(void)
{
	if (!g_pMusic)
		return 0;

#if defined(SDL_MIXER_VERSION_ATLEAST) && SDL_MIXER_VERSION_ATLEAST(2,6,0)
	double secs = Mix_MusicDuration(g_pMusic);
	if (secs < 0.0)
		return 0;
	return (DWORD)(secs * 1000.0);
#else
	return 0;
#endif
}

///////////////////////////////////////////////////////////
// ov_StreamFinished()
///////////////////////////////////////////////////////////
DWORD ov_StreamFinished(void)
{
	return g_bStreamFinished ? 1 : 0;
}

///////////////////////////////////////////////////////////
// ov_DriverType()
///////////////////////////////////////////////////////////
DRIVER_TYPE ov_DriverType(void)
{
	return dtEmulated;
}