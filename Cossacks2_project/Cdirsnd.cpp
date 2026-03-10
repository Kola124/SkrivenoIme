///////////////////////////////////////////////////////////
// CSDLSOUND.CPP -- SDL_mixer implementation replacing DirectSound
///////////////////////////////////////////////////////////

#include "Cdirsnd.h"

///////////////////////////////////////////////////////////
// SoundInstance struct to track playing sounds
///////////////////////////////////////////////////////////
struct SoundInstance {
    int channel;
    int bufferNum;
    bool active;
};

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
        m_chunks[x] = NULL;
        m_bufferSizes[x] = 0;
        Volume[x] = 100;
        SrcX[x] = 0;
        SrcY[x] = 0;
        BufIsRun[x] = 0;
        m_refCount[x] = 0;
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
    
    // Initialize SDL_mixer with more channels for simultaneous sounds
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        //fprintf(stderr, "Mix_OpenAudio failed: %s\n", Mix_GetError());
        return;
    }
    
    // Allocate plenty of mixing channels for simultaneous sounds
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
    
    // First, collect unique chunks and their reference counts
    Mix_Chunk* uniqueChunks[MAXSND1] = {NULL};
    int uniqueRefCounts[MAXSND1] = {0};
    int uniqueCount = 0;
    
    // Count references for each unique chunk
    for (unsigned int x = 1; x <= m_currentBufferNum && x < MAXSND1; ++x)
    {
        if (m_chunks[x] == NULL) continue;
        
        // Check if we've seen this chunk before
        int found = -1;
        for (int i = 0; i < uniqueCount; i++) {
            if (uniqueChunks[i] == m_chunks[x]) {
                found = i;
                break;
            }
        }
        
        if (found >= 0) {
            // Already seen, add to ref count
            uniqueRefCounts[found] += m_refCount[x];
        } else {
            // New unique chunk
            uniqueChunks[uniqueCount] = m_chunks[x];
            uniqueRefCounts[uniqueCount] = m_refCount[x];
            uniqueCount++;
        }
    }
    
    // Free unique chunks (only once per unique chunk)
    for (int i = 0; i < uniqueCount; i++) {
        if (uniqueChunks[i] != NULL && uniqueRefCounts[i] > 0) {
            Mix_FreeChunk(uniqueChunks[i]);
        }
    }
    
    Mix_CloseAudio();
    memset(BufIsRun, 0, sizeof(BufIsRun));
    ClearSoundCategories();
    
    // Reset all arrays
    for (unsigned int x = 0; x < MAXSND1; ++x)
    {
        m_chunks[x] = NULL;
        m_refCount[x] = 0;
        m_bufferSizes[x] = 0;
        m_filenames[x][0] = '\0';
        m_bufferInstanceCount[x] = 0;
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
    
    // Load the WAV file
    m_chunks[bufferNum] = Mix_LoadWAV(filename);
    if (m_chunks[bufferNum] == NULL)
    {
        --m_currentBufferNum;
        return 0;
    }
    
    m_bufferSizes[bufferNum] = m_chunks[bufferNum]->alen;
    m_refCount[bufferNum] = 1;  // First reference
    
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
    
    if (bufferNum == 0 || bufferNum > m_currentBufferNum || m_chunks[bufferNum] == NULL)
        return 0;
    
    unsigned int newBufferNum = ++m_currentBufferNum;
    
    // SHARE the same chunk instead of loading a new copy
    m_chunks[newBufferNum] = m_chunks[bufferNum];  // Just copy the pointer!
    
    // Copy properties
    m_bufferSizes[newBufferNum] = m_bufferSizes[bufferNum];
    
    // Increase reference count for the shared chunk
    m_refCount[bufferNum]++;  // Increment original's ref count
    m_refCount[newBufferNum] = 1;  // This buffer has 1 reference to the shared chunk
    
    // Copy filename for reference (optional)
    strcpy(m_filenames[newBufferNum], m_filenames[bufferNum]);
    
    SoundCtg[newBufferNum] = SoundCtg[bufferNum];
    SoundForCtg[newBufferNum] = SoundForCtg[bufferNum];
    
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
// Helper function to convert DirectSound volume to SDL volume
// DirectSound: -10000 to 0 (decibels * 100)
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

///////////////////////////////////////////////////////////
// CSDLSound::SetVolume()
///////////////////////////////////////////////////////////
void CSDLSound::SetVolume(unsigned int bufferNum, int vol)
{
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return;
    
    if (m_chunks[bufferNum] == NULL)
        return;
    
    int sdlVol = ConvertVolume(vol);
    
    // Set volume on the CHANNEL, not the chunk
    // This allows different instances to have different volumes
    int channel = m_channels[bufferNum];
    if (channel >= 0)
    {
        Mix_Volume(channel, sdlVol);
    }
    else
    {
        // If no channel assigned yet (sound not playing), set chunk volume as fallback
        Mix_VolumeChunk(m_chunks[bufferNum], sdlVol);
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
    {
        // Sound hasn't started playing yet
        // Store pan setting to apply when it does play
        // For now, we'll store it in Volume array as temporary storage
        // Or create a separate pan cache array
        return;
    }
    
    int sdlPan = ConvertPan(pan);
    
    // SDL_mixer panning: left=255-right, right=left
    Mix_SetPanning(channel, 255 - sdlPan, sdlPan);
}

///////////////////////////////////////////////////////////
// CSDLSound::PlaySound()
///////////////////////////////////////////////////////////
bool CSDLSound::PlaySoundSDL(unsigned int bufferNum, bool loop)
{
    MarkSoundLikePlaying(bufferNum, 0);
    
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return false;
    
    if (m_chunks[bufferNum] == NULL)
        return false;
    
    // Find free instance slot
    int instanceSlot = FindFreeInstanceSlot();
    if (instanceSlot < 0) {
        //fprintf(stderr, "No free instance slots\n");
        return false;
    }
    
    // Play on any available channel
    int loops = loop ? -1 : 0;
    int channel = Mix_PlayChannel(-1, m_chunks[bufferNum], loops);
    
    if (channel < 0) {
        return false;
    }
    
    // Setup instance
    m_activeInstances[instanceSlot].channel = channel;
    m_activeInstances[instanceSlot].bufferNum = bufferNum;
    m_activeInstances[instanceSlot].active = true;
    
    // Add to buffer's instance list
    if (m_bufferInstanceCount[bufferNum] < 16) {
        m_bufferInstanceLists[bufferNum][m_bufferInstanceCount[bufferNum]++] = instanceSlot;
    }
    
    BufIsRun[bufferNum] = 0;
    
    return true;
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
extern int CenterX;
///////////////////////////////////////////////////////////
// CSDLSound::PlayCoorSound()
///////////////////////////////////////////////////////////
bool CSDLSound::PlayCoorSound(unsigned int bufferNum, int x, int vx)
{
    MarkSoundLikePlaying(bufferNum, x);
    
    if (!m_initialized || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return false;
    
    if (m_chunks[bufferNum] == NULL)
        return false;
    
    // Find free instance slot
    int instanceSlot = FindFreeInstanceSlot();
    if (instanceSlot < 0) {
        ////fprintf(stderr, "No free instance slots\n");
        return false;
    }
    
    // Play on any available channel
    int channel = Mix_PlayChannel(-1, m_chunks[bufferNum], 0);
    
    if (channel < 0) {
        return false;
    }
    
    // Setup instance
    m_activeInstances[instanceSlot].channel = channel;
    m_activeInstances[instanceSlot].bufferNum = bufferNum;
    m_activeInstances[instanceSlot].active = true;
    
    // Add to buffer's instance list
    if (m_bufferInstanceCount[bufferNum] < 16) {
        m_bufferInstanceLists[bufferNum][m_bufferInstanceCount[bufferNum]++] = instanceSlot;
    }
    
    BufIsRun[bufferNum] = 1;
    SrcX[bufferNum] = x;
    SrcY[bufferNum] = vx;
    
    // Apply initial pan
    int pan = (x - CenterX) << 1;
    if (pan < -4000) pan = -4000;
    if (pan > 4000) pan = 4000;
    SetPanOnChannel(channel, pan);
    
    return true;
}

void CSDLSound::ControlPan(unsigned int bufferNum)
{
    if (BufIsRun[bufferNum])
    {
        SrcX[bufferNum] += SrcY[bufferNum];
        int pan = (SrcX[bufferNum] - CenterX) << 1;
        if (pan < -4000) pan = -4000;
        if (pan > 4000) pan = 4000;
        SetPan(bufferNum, pan);
        if (rand() < 350) IsPlaying(bufferNum);
    }
}

void CSDLSound::MarkSoundLikePlaying(unsigned int bufferNum, int x)
{
    byte ctg = SoundCtg[bufferNum];
    if (ctg)
    {
        CurrSoundCtgFreq[ctg]++;
        int fr = SoundCtgFreq[ctg];
        if (fr > StartGroupFreq[ctg])
        {
            int D = FinalGroupFreq[ctg] - StartGroupFreq[ctg];
            // Seeking for a free group sound
            int NS = CtgNSounds[ctg];
            int LastPIdx = -1;
            if (NS)
            {
                int NATT = 0;
                do
                {
                    int idx = (NS * rand()) >> 15;
                    NATT++;
                    int bfid = CtgSoundID[ctg][idx];
                    LastPIdx = bfid;
                    if (!IsPlaying(bfid))
                    {
                        SetPan(bfid, 0);
                        SetVolume(bfid, -10000);
                        PlaySoundSDL(bfid, true);  // Loop
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
        if (fr > FinalGroupFreq[ctg]) return;
    }
}

extern int TIME1;
extern int WarSound;
int NCCL = 0;

void CSDLSound::ProcessSoundSystem()
{
    CleanupFinishedInstances();
    int maxfr = 0;
    for (int i = 0; i < MAXSND1; i++)
    {
        if (BufIsRun[i])
        {
            ControlPan(i);
            if (SoundForCtg[i])
            {
                int ctg = SoundForCtg[i];
                int fr = SoundCtgFreq[ctg];
                
                int channel = m_channels[i];
                if (channel >= 0)
                {
                    int v = Mix_Volume(channel, -1);  // Get current volume
                    int dv = abs(v) / 5;
                    if (dv < 10) dv = 10;
                    
                    if (fr > StartGroupFreq[ctg])
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
    
    for (int i = 0; i < 256; i++)
    {
        int fr = SoundCtgFreq[i];
        if (fr > maxfr) maxfr = fr;
    }
    
    NCCL++;
    if (NCCL > 10)
    {
        NCCL = 0;
        memcpy(SoundCtgFreq, CurrSoundCtgFreq, 1024);
        memset(CurrSoundCtgFreq, 0, 1024);
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
            SoundInstance& instance = m_activeInstances[instanceSlot];  // Use reference, not pointer
            
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
        SoundInstance& instance = m_activeInstances[i];  // Use reference
        
        if (instance.active) {
            if (!Mix_Playing(instance.channel)) {
                // Mark as inactive
                instance.active = false;
                
                // Remove from buffer's instance list
                RemoveInstanceFromBuffer(i, instance.bufferNum);
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
            if (CDIRSND.SoundForCtg[i])
            {
                CDIRSND.StopSound(i);
            }
        }
    }
}