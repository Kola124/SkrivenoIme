///////////////////////////////////////////////////////////
// CSDLSOUND.H -- SDL_mixer replacement for CDirSound
///////////////////////////////////////////////////////////

#ifndef __CSDLSOUND_H
#define __CSDLSOUND_H

#include <..\include\SDL.h>
#include <..\include\SDL_mixer.h>
#include <windows.h>
#include "cwave.h"
#include <mmsystem.h>
#include <memory>

#define MAXSND 1024 
#define MAXSND1 2048

// CreateSDLSound() calls Mix_AllocateChannels(64) -- that's the real
// ceiling on how many sounds can physically mix at once. MAX_INSTANCES
// used to be 1024, i.e. 16x more logical instance slots than the mixer
// can ever use concurrently. Keeping some headroom for instances that
// are mid-cleanup, but not 16x of it.
#define MAX_INSTANCES 128

///////////////////////////////////////////////////////////
// CCategoryTracker
//
// Bookkeeping for SoundList.txt's INGROUP/FORGROUP sound categories:
// how often a category of one-shot sounds has fired recently, and
// which looping "ambience bed" sound to fade in once a category gets
// busy enough (see CSDLSound::MarkSoundLikePlaying/ProcessSoundSystem).
//
// This is pure bookkeeping with no SDL_mixer calls in it, so it can't
// accidentally touch a channel or a chunk -- CSDLSound asks it
// questions ("is this category busy?", "which sound should I fade
// in?") and performs the actual playback/volume changes itself. That
// split used to be tangled together in one class; this keeps the
// "how loud/frequent has category N been" state independent of "how
// do I talk to SDL_mixer".
///////////////////////////////////////////////////////////
class CCategoryTracker
{
public:
	void Clear();
	void StopAll();

	void SetCategory(unsigned short soundId, byte ctg, byte forCtg);
	void CopyFrom(unsigned short dstSoundId, unsigned short srcSoundId);
	void AddGroupSound(byte ctg, unsigned short soundId);
	void ClearGroupSound(byte ctg);
	void SetGroupOptions(byte ctg, int startFreq, int endFreq);

	byte CategoryOf(unsigned short soundId) const      { return SoundCtg[soundId]; }
	byte GroupCategoryOf(unsigned short soundId) const { return SoundForCtg[soundId]; }

	void NoteAttempt(byte ctg) { CurrSoundCtgFreq[ctg]++; }
	int  FreqOf(byte ctg) const     { return SoundCtgFreq[ctg]; }
	int  StartFreq(byte ctg) const  { return StartGroupFreq[ctg]; }
	int  FinalFreq(byte ctg) const  { return FinalGroupFreq[ctg]; }

	int            NumGroupSounds(byte ctg) const          { return CtgNSounds[ctg]; }
	unsigned short GroupSoundAt(byte ctg, int idx) const    { return CtgSoundID[ctg][idx]; }

	// Highest recent-fire frequency across all categories (drives TIME1
	// in ProcessSoundSystem). Call BEFORE DecayFrequencies() each tick --
	// it reads the not-yet-decayed counts, matching the original order.
	int MaxFrequency() const;

	// Periodic decay: rolls this tick's counts into the "current"
	// frequency and resets the accumulator. Called every ~10 ticks from
	// ProcessSoundSystem.
	void DecayFrequencies();

private:
	byte           SoundCtg[MAXSND1];
	byte           SoundForCtg[MAXSND1];
	int            SoundCtgFreq[256];
	int            CurrSoundCtgFreq[256];
	unsigned short CtgSoundID[256][16];
	byte           CtgNSounds[16];
	byte           StartGroupFreq[256];
	byte           FinalGroupFreq[256];
};

class CSDLSound
{
public:
    std::shared_ptr<Mix_Chunk> m_chunks[MAXSND1];  // shared: DuplicateSound() shares one chunk across slots
    int m_channels[MAXSND1];                        // channel of this buffer's most recent instance, or -1
    DWORD m_bufferSizes[MAXSND1];

    short Volume[MAXSND1];
    short SrcX[MAXSND1];
    short SrcY[MAXSND1];
    byte BufIsRun[MAXSND1];
    unsigned int m_currentBufferNum;
    
    CSDLSound();
    void CreateSDLSound();
    ~CSDLSound();
    unsigned int LoadWAV(const char* filename);
    unsigned int DuplicateSound(unsigned int bufferNum);
    bool SDLSoundOK();
    
    void SetLastVolume(short Vol) {
        if (m_currentBufferNum > 0 && m_currentBufferNum < MAXSND1) {
            Volume[m_currentBufferNum] = Vol;
        }
    }

    // Atomic play: volume/pan are applied before the instance is first
    // mixed, so there's no "set-then-play" window and no dependence on
    // m_channels[] already holding a valid channel for this buffer.
    // Each call is its own instance -- correct even if another instance
    // of the same buffer is already playing on a different channel.
    // Returns the instance handle (internal slot + 1), or 0 on failure;
    // callers that don't need to touch this instance again can ignore it.
    unsigned int PlaySoundSDL(unsigned int bufferNum, int vol, int pan, bool loop = false);

    // Coordinate-tracked play: pan is derived from x (see ComputePan),
    // and y -- a real vertical screen coordinate, not a velocity (see
    // below) -- feeds ComputeDepthAttenuation() for a volume-based
    // up/down cue, since stereo can't represent elevation on its own.
    // There's no separate pan parameter: it's fully derived from x/y,
    // so one passed in here would just be redundant.
    //
    // NOTE: this used to be named 'vx' and was meant to be a velocity,
    // added into SrcX every tick by ControlPan() to track a moving
    // emitter. Nothing anywhere ever actually passed a nonzero value
    // for it, so that feature never worked; it's been repurposed to
    // carry the real y position instead (previously discarded before
    // it ever reached here -- see AddEffectV() in GameSound.cpp).
    // Position is still only a snapshot taken when the sound starts,
    // not live-tracked -- see the longer note on the .cpp definition.
    unsigned int PlayCoorSound(unsigned int bufferNum, int x, int y, int vol);

    void SetVolume(unsigned int bufferNum, int vol);
    void SetPan(unsigned int bufferNum, int pan);
    bool StopSound(unsigned int bufferNum);
    void MarkSoundLikePlaying(unsigned int bufferNum, int x);
    void ControlPan(unsigned int bufferNum);
    bool IsPlaying(unsigned int bufferNum);
    int GetPos(unsigned int bufferNum);
    void ProcessSoundSystem();
    
    // Sound categories -- thin forwarders to m_categories. Kept as
    // methods on CSDLSound (same names/signatures as before) so nothing
    // calling these elsewhere needs to change.
    void ClearSoundCategories() {
        m_categories.Clear();
        LastDecTime = SDL_GetTicks();
    }
    void SetSoundCategory(unsigned short SoundID, byte ctg, byte forctg) {
        if (SoundID < MAXSND1) m_categories.SetCategory(SoundID, ctg, forctg);
    }
    void ClearGroupSound(byte ctg) { m_categories.ClearGroupSound(ctg); }
    void AddGroupSound(byte ctg, unsigned short SoundID) { m_categories.AddGroupSound(ctg, SoundID); }
    void SetGroupOptions(byte ctg, int StartFreq, int EndFreq) { m_categories.SetGroupOptions(ctg, StartFreq, EndFreq); }
    void StopCtgSounds() { m_categories.StopAll(); }
    bool IsGroupSound(unsigned int bufferNum) const { return m_categories.GroupCategoryOf((unsigned short)bufferNum) != 0; }

    void ReleaseAll();

private:
    int LastDecTime;
    CCategoryTracker m_categories;

    struct SoundInstance {
        int channel;
        int bufferNum;
        bool active;
    };
    
    SoundInstance m_activeInstances[MAX_INSTANCES];
    int m_instanceCount;
    
    int m_bufferInstanceLists[MAXSND1][16];
    int m_bufferInstanceCount[MAXSND1];
    
    char m_filenames[MAXSND1][256];
    
    int m_nextInstanceId;
    
    void CleanupFinishedInstances();
    void SetVolumeOnChannel(int channel, int vol);
    void SetPanOnChannel(int channel, int pan);
    int FindFreeInstanceSlot();
    void RemoveInstanceFromBuffer(int instanceSlot, int bufferNum);

protected:
    bool m_initialized;
};

#endif

#define MaxSnd 1024
#define DECFACTOR 200