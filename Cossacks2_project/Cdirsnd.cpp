///////////////////////////////////////////////////////////
// CSDLSOUND.CPP -- SDL_mixer implementation replacing DirectSound
///////////////////////////////////////////////////////////

#include "Cdirsnd.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Set to 1 to re-enable the Y-based volume attenuation in
// PlayCoorSound() (see ComputeDepthScalePercent() below). Currently
// off: pan-only positional audio, per playtesting feedback that the
// depth effect had issues worth revisiting before turning back on.
#define ENABLE_DEPTH_SCALING 0

///////////////////////////////////////////////////////////
// CCategoryTracker -- see Cdirsnd.h for what this is for.
// Pure bookkeeping, no SDL_mixer calls.
///////////////////////////////////////////////////////////
void CCategoryTracker::Clear()
{
    memset(SoundCtgFreq, 0, sizeof(SoundCtgFreq));
    memset(CurrSoundCtgFreq, 0, sizeof(CurrSoundCtgFreq));
    memset(CtgSoundID, 0, sizeof(CtgSoundID));
    memset(CtgNSounds, 0, sizeof(CtgNSounds));
    memset(StartGroupFreq, 0, sizeof(StartGroupFreq));
    memset(FinalGroupFreq, 0, sizeof(FinalGroupFreq));
    memset(SoundForCtg, 0, sizeof(SoundForCtg));
    memset(SoundCtg, 0, sizeof(SoundCtg));
}

void CCategoryTracker::StopAll()
{
    memset(SoundCtgFreq, 0, sizeof(SoundCtgFreq));
}

void CCategoryTracker::SetCategory(unsigned short soundId, byte ctg, byte forCtg)
{
    SoundCtg[soundId] = ctg;
    SoundForCtg[soundId] = forCtg;
}

void CCategoryTracker::CopyFrom(unsigned short dstSoundId, unsigned short srcSoundId)
{
    SoundCtg[dstSoundId] = SoundCtg[srcSoundId];
    SoundForCtg[dstSoundId] = SoundForCtg[srcSoundId];
}

void CCategoryTracker::AddGroupSound(byte ctg, unsigned short soundId)
{
    if (CtgNSounds[ctg] < 16)
    {
        CtgSoundID[ctg][CtgNSounds[ctg]] = soundId;
        CtgNSounds[ctg]++;
    }
}

void CCategoryTracker::ClearGroupSound(byte ctg)
{
    CtgNSounds[ctg] = 0;
}

void CCategoryTracker::SetGroupOptions(byte ctg, int startFreq, int endFreq)
{
    StartGroupFreq[ctg] = startFreq;
    FinalGroupFreq[ctg] = endFreq;
}

int CCategoryTracker::MaxFrequency() const
{
    int maxfr = 0;
    for (int i = 0; i < 256; i++)
        if (SoundCtgFreq[i] > maxfr) maxfr = SoundCtgFreq[i];
    return maxfr;
}

void CCategoryTracker::DecayFrequencies()
{
    memcpy(SoundCtgFreq, CurrSoundCtgFreq, sizeof(SoundCtgFreq));
    memset(CurrSoundCtgFreq, 0, sizeof(CurrSoundCtgFreq));
}

///////////////////////////////////////////////////////////
// CSDLSound::CSDLSound()
///////////////////////////////////////////////////////////
CSDLSound::CSDLSound()
{
    m_initialized = false;
    ClearSoundCategories();
    m_currentBufferNum = 0;
    m_instanceCount = 0;
    m_nextInstanceId = 1;
    
    for (unsigned int x = 0; x < MAXSND1; ++x)
    {
        m_chunks[x].reset();
        m_bufferSizes[x] = 0;
        Volume[x] = 100;
        SrcX[x] = 0;
        SrcY[x] = 0;
        BufIsRun[x] = 0;
        m_channels[x] = -1;
        m_bufferInstanceCount[x] = 0;
        m_filenames[x][0] = '\0';
        
        for (int i = 0; i < 16; i++) {
            m_bufferInstanceLists[x][i] = -1;
        }
    }
    
    for (int i = 0; i < MAX_INSTANCES; i++) {
        m_activeInstances[i].active = false;
        m_activeInstances[i].channel = -1;
        m_activeInstances[i].bufferNum = 0;
    }
}

///////////////////////////////////////////////////////////
// CSDLSound::CreateSDLSound()
///////////////////////////////////////////////////////////
void CSDLSound::CreateSDLSound()
{
    if(m_initialized)
        return;
    
    // Initialize SDL Audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        //fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return;
    }
    
    // Load the Ogg Vorbis codec so oggvor.cpp's Mix_LoadMUS() calls can
    // stream .ogg music directly, in-process (no more vopl.exe helper).
    // Not fatal if it fails: WAV sound effects still work either way,
    // only background music would be affected.
    Mix_Init(MIX_INIT_OGG);
    
    // Initialize SDL_mixer with more channels for simultaneous sounds
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        //fprintf(stderr, "Mix_OpenAudio failed: %s\n", Mix_GetError());
        return;
    }
    
    // Allocate plenty of mixing channels for simultaneous sounds.
    // MAX_INSTANCES (Cdirsnd.h) is sized relative to this -- if you
    // raise this, consider raising MAX_INSTANCES too.
    Mix_AllocateChannels(64);  // More than enough for gunshots
    
    m_initialized = true;
}

///////////////////////////////////////////////////////////
// CSDLSound::~CSDLSound()
///////////////////////////////////////////////////////////
CSDLSound::~CSDLSound()
{
    if (m_initialized)
        ReleaseAll();
}

///////////////////////////////////////////////////////////
// CSDLSound::ReleaseAll()
///////////////////////////////////////////////////////////
void CSDLSound::ReleaseAll()
{
    // Stop all sounds
    Mix_HaltChannel(-1);
    
    // Resetting each shared_ptr drops one reference; Mix_FreeChunk
    // (via the shared_ptr's deleter, set up in LoadWAV) fires
    // automatically once the LAST reference to a given chunk goes
    // away -- including chunks shared by DuplicateSound(). No manual
    // dedup/refcounting pass needed anymore.
    for (unsigned int x = 0; x < MAXSND1; ++x)
        m_chunks[x].reset();
    
    Mix_CloseAudio();
    memset(BufIsRun, 0, sizeof(BufIsRun));
    ClearSoundCategories();
    
    // Reset all arrays
    for (unsigned int x = 0; x < MAXSND1; ++x)
    {
        m_bufferSizes[x] = 0;
        m_filenames[x][0] = '\0';
        m_bufferInstanceCount[x] = 0;
        m_channels[x] = -1;
    }
    
    m_initialized = false;
}

///////////////////////////////////////////////////////////
// CSDLSound::LoadWAV()
// Replacement for CreateSoundBuffer
///////////////////////////////////////////////////////////
unsigned int CSDLSound::LoadWAV(const char* filename)
{
    if (!m_initialized)
        return 0;
    
    if (m_currentBufferNum >= MAXSND)
        return 0;
    
    unsigned int bufferNum = ++m_currentBufferNum;
    
    // Load the WAV file. The shared_ptr's deleter is Mix_FreeChunk
    // itself, so however many slots end up pointing at this same
    // Mix_Chunk (see DuplicateSound), it gets freed exactly once,
    // automatically, when the last of them goes away.
    Mix_Chunk* raw = Mix_LoadWAV(filename);
    if (raw == NULL)
    {
        --m_currentBufferNum;
        return 0;
    }
    
    m_chunks[bufferNum] = std::shared_ptr<Mix_Chunk>(raw, Mix_FreeChunk);
    m_bufferSizes[bufferNum] = raw->alen;
    
    // Store filename for reference
    strncpy(m_filenames[bufferNum], filename, 255);
    m_filenames[bufferNum][255] = '\0';
    
    return bufferNum;
}

///////////////////////////////////////////////////////////
// CSDLSound::DuplicateSound()
// Replacement for DuplicateSoundBuffer
///////////////////////////////////////////////////////////
unsigned int CSDLSound::DuplicateSound(unsigned int bufferNum)
{
    if (!m_initialized)
        return 0;
    
    if (m_currentBufferNum >= MAXSND)
        return 0;
    
    if (bufferNum == 0 || bufferNum > m_currentBufferNum || !m_chunks[bufferNum])
        return 0;
    
    unsigned int newBufferNum = ++m_currentBufferNum;
    
    // SHARE the same chunk instead of loading a new copy. This copies
    // the shared_ptr, which bumps its internal reference count -- no
    // separate m_refCount[] bookkeeping needed.
    m_chunks[newBufferNum] = m_chunks[bufferNum];
    m_bufferSizes[newBufferNum] = m_bufferSizes[bufferNum];
    
    // Copy filename for reference (optional)
    strcpy(m_filenames[newBufferNum], m_filenames[bufferNum]);
    
    m_categories.CopyFrom(newBufferNum, bufferNum);
    
    return newBufferNum;
}

int CSDLSound::FindFreeInstanceSlot()
{
    for (int i = 0; i < MAX_INSTANCES; i++) {
        if (!m_activeInstances[i].active) {
            return i;
        }
    }
    return -1; // No free slots
}

///////////////////////////////////////////////////////////
// CSDLSound::SDLSoundOK()
///////////////////////////////////////////////////////////
bool CSDLSound::SDLSoundOK()
{
    return m_initialized;
}

///////////////////////////////////////////////////////////
// Helper function to convert DirectSound-style volume to SDL volume
// Convention: -10000 to 0 (decibels * 100). This is the scale used
// for SOUND EFFECTS throughout GameSound.cpp (e.g. "vol -= (100-
// WarSound)*40"). NOTE: this is NOT the same convention the music
// layer uses -- PlayMP3.cpp/oggvor.cpp pass a plain 0-100 linear
// volume into ov_SetVolume(), converted separately there. The two
// were never unified because effects volume is baked into a lot of
// existing tuning math (see AddWarEffect/AddWorkEffect/AddOrderEffect
// in GameSound.cpp) that would need re-tuning by ear if the scale
// changed, not just recompiling.
// SDL_mixer: 0 to 128
///////////////////////////////////////////////////////////
static int ConvertVolume(int dsVolume)
{
    if (dsVolume <= -10000)
        return 0;
    if (dsVolume >= 0)
        return MIX_MAX_VOLUME;
    
    // Approximate conversion from decibels to linear
    // dB = 20 * log10(linear)
    // linear = 10^(dB/20)
    double db = dsVolume / 100.0;
    double linear = pow(10.0, db / 20.0);
    int volume = (int)(linear * MIX_MAX_VOLUME);
    
    if (volume < 0) volume = 0;
    if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
    
    return volume;
}

///////////////////////////////////////////////////////////
// Helper function to convert DirectSound pan to SDL pan
// DirectSound: -10000 (left) to +10000 (right)
// SDL_mixer: 0 (left) to 255 (right), 127 is center
///////////////////////////////////////////////////////////
static int ConvertPan(int dsPan)
{
    // Convert from -10000/+10000 to 0-255
    int pan = ((dsPan + 10000) * 255) / 20000;
    if (pan < 0) pan = 0;
    if (pan > 255) pan = 255;
    return pan;
}

extern int CenterX;

///////////////////////////////////////////////////////////
// ComputePan()
//
// Position -> DirectSound-style pan (-4000..4000), shared by
// PlayCoorSound() (initial pan when a coordinate-tracked sound
// starts) and ControlPan() (per-tick update while it keeps playing).
// This used to be written out twice, identically, in this same file;
// collapsing it to one function doesn't change behavior, just removes
// the risk of the two copies quietly drifting apart later.
//
// NOTE: GameSound.cpp's AddWarEffect/AddWorkEffect/AddOrderEffect use
// a DIFFERENT pan formula for the same conceptual purpose. That's a
// separate, pre-existing inconsistency (not introduced here) -- left
// alone deliberately, since unifying it would change how those three
// effect types sound and that's a tuning call, not a refactor.
///////////////////////////////////////////////////////////
static int ComputePan(int x, int centerX)
{
    int pan = (x - centerX) << 1;
    if (pan < -4000) pan = -4000;
    if (pan > 4000) pan = 4000;
    return pan;
}

extern int SMinY;
extern int SMaxY;

///////////////////////////////////////////////////////////
// ComputeDepthScalePercent()
//
// Stereo audio has exactly one spatial axis (left/right); it can't
// represent "higher on screen" on its own. This fakes it the way most
// 2D/isometric games do: treat vertical screen position as a proxy for
// depth, and pull the volume down for sounds further "up" (further
// from the camera, toward the top of the view) than for the same
// sound lower on screen. Combined with ComputePan()'s left/right
// result, "upper right" now reads as "panned right AND noticeably
// quieter/more distant" instead of being indistinguishable from plain
// "right".
//
// Curve: the bottom third of the visible screen (SMinY/SMaxY, the
// same play-area bounds AddEffectV() checks for its own visibility
// gating) is left at full volume -- "close to the camera" reads as
// untouched, not just "less attenuated". From that boundary upward,
// volume scales down linearly (a gradual fade, not a hard step) to
// MIN_DEPTH_SCALE_PERCENT at the very top edge of the screen and
// beyond (clamped, so something far off the top of the visible area
// doesn't keep fading past that floor).
//
// IMPORTANT: this returns a PERCENTAGE, applied by multiplying the
// already-converted 0-128 SDL volume, not another dB value added
// before conversion. An earlier version of this did the latter
// (subtracting more dB before ConvertVolume()'s exponential curve),
// and that's what caused "can't hear it at all" at the top of the
// screen: ConvertVolume()'s dB->linear conversion, combined with
// integer truncation to an SDL volume of 0-128, rounds anything past
// roughly -42dB down to a literal 0 -- and that -42dB budget gets used
// up fast once you ADD it to whatever dB penalty the sound already had
// (off-screen fringe volume, the WarSound effects-volume slider,
// etc.) -- two negative dB values stacking can silently zero out well
// before either looks extreme on its own. Scaling the final linear
// volume by a percentage instead means depth can only ever make a
// sound quieter in proportion to what it would otherwise have been --
// it can't be the thing that flips an already-nonzero sound to
// literal silence.
///////////////////////////////////////////////////////////
#define MIN_DEPTH_SCALE_PERCENT 12  // volume % at the very top of the screen -- keep > 0 or the floor below can't help

#if ENABLE_DEPTH_SCALING
static int ComputeDepthScalePercent(int y, int screenMinY, int screenMaxY)
{
    int screenHeight = screenMaxY - screenMinY;
    if (screenHeight <= 0) return 100; // screen bounds not set up yet -- don't guess

    // Y increases downward, so "bottom third of the screen" is the
    // largest values of y; boundaryY is where that bottom third starts.
    int boundaryY = screenMaxY - (screenHeight / 3);

    if (y >= boundaryY) return 100; // bottom third (and anything below it): full volume, untouched

    int aboveBoundary = boundaryY - y;
    int attenuationRange = boundaryY - screenMinY; // distance from the boundary up to the top edge
    if (attenuationRange <= 0) return 100;

    if (aboveBoundary > attenuationRange) aboveBoundary = attenuationRange; // clamp for anything above/off-screen

    int dropRange = 100 - MIN_DEPTH_SCALE_PERCENT;
    return 100 - ((dropRange * aboveBoundary) / attenuationRange);
}
#endif

///////////////////////////////////////////////////////////
// CSDLSound::SetVolume()
//
// Runtime volume adjustment for an already-playing (or not-yet-
// playing) buffer. Most callers should prefer passing vol directly to
// PlaySoundSDL()/PlayCoorSound() instead -- this remains for cases
// that need to change the volume of something already in flight (see
// ControlPan/ProcessSoundSystem's category fade).
///////////////////////////////////////////////////////////
void CSDLSound::SetVolume(unsigned int bufferNum, int vol)
{
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return;
    
    if (!m_chunks[bufferNum])
        return;
    
    int sdlVol = ConvertVolume(vol);
    
    // Set volume on the CHANNEL, not the chunk, so different
    // instances of the same buffer can have different volumes.
    int channel = m_channels[bufferNum];
    if (channel >= 0)
    {
        Mix_Volume(channel, sdlVol);
    }
    else
    {
        // No channel assigned (nothing of this buffer playing right
        // now) -- fall back to the chunk's default volume, which will
        // apply the next time it's played without an explicit vol.
        Mix_VolumeChunk(m_chunks[bufferNum].get(), sdlVol);
    }
}

///////////////////////////////////////////////////////////
// CSDLSound::SetPan()
///////////////////////////////////////////////////////////
void CSDLSound::SetPan(unsigned int bufferNum, int pan)
{
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return;
    
    int channel = m_channels[bufferNum];
    if (channel < 0)
        return; // nothing of this buffer is playing right now
    
    SetPanOnChannel(channel, pan);
}

///////////////////////////////////////////////////////////
// CSDLSound::SetVolumeOnChannel()
///////////////////////////////////////////////////////////
void CSDLSound::SetVolumeOnChannel(int channel, int vol)
{
    if (channel < 0) return;
    
    int sdlVol = ConvertVolume(vol);
    Mix_Volume(channel, sdlVol);
}

///////////////////////////////////////////////////////////
// CSDLSound::SetPanOnChannel()
///////////////////////////////////////////////////////////
void CSDLSound::SetPanOnChannel(int channel, int pan)
{
    if (channel < 0) return;
    
    int sdlPan = ConvertPan(pan);
    Mix_SetPanning(channel, 255 - sdlPan, sdlPan);
}

///////////////////////////////////////////////////////////
// CSDLSound::PlaySoundSDL()
//
// Atomic play: volume and pan are applied to THIS instance's channel
// before returning, so there's no window where a freshly started
// sound is briefly audible at the wrong volume/pan, and no dependence
// on m_channels[bufferNum] already being valid. This replaces the old
// "CDS->SetVolume(sid,vol); CDS->SetPan(sid,pan); CDS->PlaySoundSDL
// (sid,loop);" three-call pattern that GameSound.cpp used to use --
// see that file for the updated call sites.
///////////////////////////////////////////////////////////
unsigned int CSDLSound::PlaySoundSDL(unsigned int bufferNum, int vol, int pan, bool loop)
{
    MarkSoundLikePlaying(bufferNum, 0);
    
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return 0;
    
    if (!m_chunks[bufferNum])
        return 0;
    
    // Find free instance slot
    int instanceSlot = FindFreeInstanceSlot();
    if (instanceSlot < 0) {
        //fprintf(stderr, "No free instance slots\n");
        return 0;
    }
    
    // Play on any available channel
    int loops = loop ? -1 : 0;
    int channel = Mix_PlayChannel(-1, m_chunks[bufferNum].get(), loops);
    
    if (channel < 0) {
        return 0;
    }
    
    // Setup instance
    m_activeInstances[instanceSlot].channel = channel;
    m_activeInstances[instanceSlot].bufferNum = bufferNum;
    m_activeInstances[instanceSlot].active = true;
    
    // Add to buffer's instance list
    if (m_bufferInstanceCount[bufferNum] < 16) {
        m_bufferInstanceLists[bufferNum][m_bufferInstanceCount[bufferNum]++] = instanceSlot;
    }
    
    // Remember the channel this buffer is (most recently) playing on,
    // so SetVolume()/SetPan()/ProcessSoundSystem can find it later.
    m_channels[bufferNum] = channel;
    
    // Apply this instance's volume/pan immediately, on ITS channel --
    // doesn't disturb any other instance of the same buffer that might
    // already be playing on a different channel.
    Mix_Volume(channel, ConvertVolume(vol));
    SetPanOnChannel(channel, pan);
    
    BufIsRun[bufferNum] = 0;
    
    return (unsigned int)(instanceSlot + 1);
}

///////////////////////////////////////////////////////////
// CSDLSound::PlayCoorSound()
//
// Same atomic-play idea as PlaySoundSDL(), specialized for
// coordinate-tracked sounds: pan is derived from x via ComputePan(),
// and 'y' -- now a real vertical screen coordinate, see AddEffectV()
// in GameSound.cpp -- feeds ComputeDepthScalePercent() to fake an
// up/down cue via volume, since stereo can't otherwise distinguish
// "upper right" from "right". There's still no separate pan
// parameter: passing one in here would be redundant with what's
// derived from x/y.
//
// NOTE ON MOVEMENT: SrcX/SrcY are stored once here and re-applied by
// ControlPan() every tick for as long as BufIsRun[bufferNum] stays
// set -- but nothing currently updates them again after this point.
// This fixes the missing vertical axis; it does NOT add live tracking
// of a moving emitter's current position frame-by-frame. If you want
// a sound to keep following a moving unit for its whole duration,
// something would need to call SetSoundPosition-style logic (not
// present) with the unit's live coordinates each tick, not just at
// the moment the sound starts.
///////////////////////////////////////////////////////////
unsigned int CSDLSound::PlayCoorSound(unsigned int bufferNum, int x, int y, int vol)
{
    MarkSoundLikePlaying(bufferNum, x);
    
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return 0;
    
    if (!m_chunks[bufferNum])
        return 0;
    
    // Find free instance slot
    int instanceSlot = FindFreeInstanceSlot();
    if (instanceSlot < 0) {
        ////fprintf(stderr, "No free instance slots\n");
        return 0;
    }
    
    // Play on any available channel
    int channel = Mix_PlayChannel(-1, m_chunks[bufferNum].get(), 0);
    
    if (channel < 0) {
        return 0;
    }
    
    // Setup instance
    m_activeInstances[instanceSlot].channel = channel;
    m_activeInstances[instanceSlot].bufferNum = bufferNum;
    m_activeInstances[instanceSlot].active = true;
    
    // Add to buffer's instance list
    if (m_bufferInstanceCount[bufferNum] < 16) {
        m_bufferInstanceLists[bufferNum][m_bufferInstanceCount[bufferNum]++] = instanceSlot;
    }
    
    // Remember the channel this buffer is (most recently) playing on.
    m_channels[bufferNum] = channel;
    
#if ENABLE_DEPTH_SCALING
    // Depth is applied AFTER converting to SDL's 0-128 scale, as a
    // percentage -- not as more dB subtracted beforehand. See the
    // long comment on ComputeDepthScalePercent() for why: additive dB
    // penalties stack and can silently underflow to a literal 0 well
    // before you'd expect, which is what made gunshots at the top of
    // the screen inaudible in an earlier version of this.
    int baseSdlVol = ConvertVolume(vol);
    int depthPct = ComputeDepthScalePercent(y, SMinY, SMaxY);
    int sdlVol = (baseSdlVol * depthPct) / 100;
    if (sdlVol <= 0 && baseSdlVol > 0) sdlVol = 1; // never let depth alone fully silence an audible sound
    Mix_Volume(channel, sdlVol);
#else
    // Depth-based volume disabled -- pan-only, same as before that
    // feature was added. ComputeDepthScalePercent() is left in place
    // (see near the top of this file) in case it's worth revisiting;
    // flip ENABLE_DEPTH_SCALING to 1 above to bring it back.
    Mix_Volume(channel, ConvertVolume(vol));
#endif
    
    BufIsRun[bufferNum] = 1;
    SrcX[bufferNum] = x;
    SrcY[bufferNum] = y;
    
    SetPanOnChannel(channel, ComputePan(x, CenterX));
    
    return (unsigned int)(instanceSlot + 1);
}

void CSDLSound::ControlPan(unsigned int bufferNum)
{
    if (BufIsRun[bufferNum])
    {
        // SrcY used to be a "velocity" added into SrcX every tick here
        // -- but nothing anywhere ever passed a nonzero velocity, so
        // that line never actually did anything. Now that SrcY holds
        // a real Y coordinate (see PlayCoorSound), adding it into
        // SrcX every tick would corrupt the x position with the y
        // value, so that line is gone, not just dormant.
        //
        // Depth (from Y) was already baked into the channel's volume
        // once, at PlayCoorSound() time, and Y doesn't change after
        // that (see the "NOTE ON MOVEMENT" comment there), so it
        // doesn't need re-applying here. Only pan gets re-checked each
        // tick, because SrcX DOES still drift for the ambience-bed
        // smoothing case in MarkSoundLikePlaying().
        SetPan(bufferNum, ComputePan(SrcX[bufferNum], CenterX));
        if (rand() < 350) IsPlaying(bufferNum);
    }
}

void CSDLSound::MarkSoundLikePlaying(unsigned int bufferNum, int x)
{
    byte ctg = m_categories.CategoryOf((unsigned short)bufferNum);
    if (ctg)
    {
        m_categories.NoteAttempt(ctg);
        int fr = m_categories.FreqOf(ctg);
        if (fr > m_categories.StartFreq(ctg))
        {
            // Seeking for a free group sound
            int NS = m_categories.NumGroupSounds(ctg);
            int LastPIdx = -1;
            if (NS)
            {
                int NATT = 0;
                do
                {
                    int idx = (NS * rand()) >> 15;
                    NATT++;
                    int bfid = m_categories.GroupSoundAt(ctg, idx);
                    LastPIdx = bfid;
                    if (!IsPlaying(bfid))
                    {
                        // vol=-10000 (silent) + pan=0 (center), looped;
                        // ProcessSoundSystem's category fade below
                        // ramps this up over time.
                        PlaySoundSDL(bfid, -10000, 0, true);
                        NATT = 100;
                        if (x)
                        {
                            BufIsRun[bfid] = 1;
                            SrcX[bfid] = x;
                            SrcY[bfid] = 0;
                        }
                        return;
                    }
                } while (NATT < 2);
                
                if (x && LastPIdx != -1)
                {
                    SrcX[LastPIdx] = (x + SrcX[LastPIdx] * 15) >> 4;
                }
            }
        }
        if (fr > m_categories.FinalFreq(ctg)) return;
    }
}

extern int TIME1;
extern int WarSound;
int NCCL = 0;

void CSDLSound::ProcessSoundSystem()
{
    CleanupFinishedInstances();

    for (int i = 0; i < MAXSND1; i++)
    {
        if (BufIsRun[i])
        {
            ControlPan(i);
            byte ctg = m_categories.GroupCategoryOf((unsigned short)i);
            if (ctg)
            {
                int fr = m_categories.FreqOf(ctg);
                
                int channel = m_channels[i];
                if (channel >= 0)
                {
                    int v = Mix_Volume(channel, -1);  // Get current volume
                    int dv = abs(v) / 5;
                    if (dv < 10) dv = 10;
                    
                    if (fr > m_categories.StartFreq(ctg))
                    {
                        if (v < 102) v = 102;  // ~-8000 dB equivalent
                        if (v < 128 - dv) v += dv;
                        else v = 128;
                    }
                    else
                    {
                        if (v > 0) v -= dv;
                    }
                    
                    int v0 = (WarSound * 128) / 100;
                    if (v > v0) v = v0;
                    
                    if (v <= 0)
                    {
                        StopSound(i);
                    }
                    else
                    {
                        Mix_Volume(channel, v);
                    }
                }
            }
        }
    }
    
    // Read the max BEFORE decaying -- matches the original order, so
    // TIME1 reflects this tick's counts, not next tick's reset ones.
    int maxfr = m_categories.MaxFrequency();
    
    NCCL++;
    if (NCCL > 10)
    {
        NCCL = 0;
        m_categories.DecayFrequencies();
    }
    TIME1 = maxfr;
}

void CSDLSound::RemoveInstanceFromBuffer(int instanceSlot, int bufferNum)
{
    for (int j = 0; j < m_bufferInstanceCount[bufferNum]; j++) {
        if (m_bufferInstanceLists[bufferNum][j] == instanceSlot) {
            // Shift remaining elements
            for (int k = j; k < m_bufferInstanceCount[bufferNum] - 1; k++) {
                m_bufferInstanceLists[bufferNum][k] = m_bufferInstanceLists[bufferNum][k + 1];
            }
            m_bufferInstanceCount[bufferNum]--;
            break;
        }
    }
}

///////////////////////////////////////////////////////////
// CSDLSound::StopSound()
///////////////////////////////////////////////////////////

bool CSDLSound::StopSound(unsigned int bufferNum)
{
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return false;
    
    // Stop all instances of this buffer
    for (int i = 0; i < m_bufferInstanceCount[bufferNum]; i++) {
        int instanceSlot = m_bufferInstanceLists[bufferNum][i];
        if (instanceSlot >= 0 && instanceSlot < MAX_INSTANCES && 
            m_activeInstances[instanceSlot].active) {
            
            Mix_HaltChannel(m_activeInstances[instanceSlot].channel);
            m_activeInstances[instanceSlot].active = false;
        }
    }
    
    m_bufferInstanceCount[bufferNum] = 0;
    m_channels[bufferNum] = -1;
    BufIsRun[bufferNum] = 0;
    return true;
}

///////////////////////////////////////////////////////////
// CSDLSound::GetPos()
///////////////////////////////////////////////////////////
int CSDLSound::GetPos(unsigned int bufferNum)
{
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return 0;
    
    // SDL_mixer doesn't provide easy position tracking
    // Return 0 for now - could be enhanced with custom callbacks
    return 0;
}

///////////////////////////////////////////////////////////
// CSDLSound::IsPlaying()
///////////////////////////////////////////////////////////
bool CSDLSound::IsPlaying(unsigned int bufferNum)
{
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return false;
    
    // Clean up finished instances first
    CleanupFinishedInstances();
    
    // Check if any instance of this buffer is playing
    for (int i = 0; i < m_bufferInstanceCount[bufferNum]; i++) {
        int instanceSlot = m_bufferInstanceLists[bufferNum][i];
        if (instanceSlot >= 0 && instanceSlot < MAX_INSTANCES) {
            SoundInstance& instance = m_activeInstances[instanceSlot];
            
            if (instance.active && Mix_Playing(instance.channel)) {
                return true;
            }
        }
    }
    
    return false;
}

///////////////////////////////////////////////////////////
// CSDLSound::CleanupFinishedInstances()
// Call this periodically to clean up finished sounds
///////////////////////////////////////////////////////////
void CSDLSound::CleanupFinishedInstances()
{
    for (int i = 0; i < MAX_INSTANCES; i++) {
        SoundInstance& instance = m_activeInstances[i];
        
        if (instance.active) {
            if (!Mix_Playing(instance.channel)) {
                int finishedBufferNum = instance.bufferNum;
                
                // Mark as inactive
                instance.active = false;
                
                // Remove from buffer's instance list
                RemoveInstanceFromBuffer(i, finishedBufferNum);
                
                // If that was the last instance of this buffer, clear
                // its cached channel so SetVolume()/SetPan() don't act
                // on a stale/reused channel -- and stop ControlPan()
                // from iterating this buffer forever. Previously,
                // BufIsRun only ever got cleared by an explicit
                // StopSound() call, so a one-shot PlayCoorSound() (any
                // positional effect, e.g. gunshots) that finished on
                // its own left BufIsRun set for the rest of the game
                // session -- harmless (ControlPan/SetPan early-return
                // once the channel is -1) but wasted a tick of work,
                // forever, for every buffer that ever played
                // positionally even once.
                if (m_bufferInstanceCount[finishedBufferNum] == 0 &&
                    m_channels[finishedBufferNum] == instance.channel) {
                    m_channels[finishedBufferNum] = -1;
                    BufIsRun[finishedBufferNum] = 0;
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////
// Global helper function
///////////////////////////////////////////////////////////
CSDLSound CDIRSND;

void StopLoopSounds()
{
    CDIRSND.StopCtgSounds();
    for (int i = 0; i < MAXSND1; i++)
    {
        if (CDIRSND.BufIsRun[i])
        {
            if (CDIRSND.IsGroupSound(i))
            {
                CDIRSND.StopSound(i);
            }
        }
    }
}