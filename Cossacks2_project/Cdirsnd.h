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

#define MAXSND 600 
#define MAXSND1 601
#define MAX_INSTANCES 1024

class CSDLSound
{
public:
    Mix_Chunk* m_chunks[MAXSND1];           // Sound chunks
    int m_channels[MAXSND1];                 // Channel assignments
    DWORD m_bufferSizes[MAXSND1];
    byte SoundCtg[MAXSND1];
    byte SoundForCtg[MAXSND1];
    int m_refCount[MAXSND1];
    int SoundCtgFreq[256];
    int CurrSoundCtgFreq[256];
    unsigned short CtgSoundID[256][16];
    byte CtgNSounds[16];
    byte StartGroupFreq[256];
    byte FinalGroupFreq[256];
    int LastDecTime;

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
    
    void SetVolume(unsigned int bufferNum, int vol);
    void SetPan(unsigned int bufferNum, int pan);
    bool PlaySoundSDL(unsigned int bufferNum, bool loop = false);
    bool StopSound(unsigned int bufferNum);
    bool PlayCoorSound(unsigned int bufferNum, int x, int vx);
    void MarkSoundLikePlaying(unsigned int bufferNum, int x);
    void ControlPan(unsigned int bufferNum);
    bool IsPlaying(unsigned int bufferNum);
    int GetPos(unsigned int bufferNum);
    void ProcessSoundSystem();
    
    // Sound categories
    void ClearSoundCategories() {
        memset(SoundCtgFreq, 0, sizeof(SoundCtgFreq));
        memset(CurrSoundCtgFreq, 0, sizeof(CurrSoundCtgFreq));
        memset(&CtgSoundID, 0, sizeof(CtgSoundID));
        memset(CtgNSounds, 0, sizeof(CtgNSounds));
        memset(StartGroupFreq, 0, sizeof(StartGroupFreq));
        memset(FinalGroupFreq, 0, sizeof(FinalGroupFreq));
        memset(SoundForCtg, 0, sizeof(SoundForCtg));
        memset(SoundCtg, 0, sizeof(SoundCtg));
        LastDecTime = SDL_GetTicks();
    }
    
    void SetSoundCategory(unsigned short SoundID, byte ctg, byte forctg) {
        if(SoundID < MAXSND1) {
            SoundCtg[SoundID] = ctg;
            SoundForCtg[SoundID] = forctg;
        }
    }
    
    void ClearGroupSound(byte ctg) {
        CtgNSounds[ctg] = 0;
    }
    
    void AddGroupSound(byte ctg, unsigned short SoundID) {
        if(CtgNSounds[ctg] < 16) {
            CtgSoundID[ctg][CtgNSounds[ctg]] = SoundID;
            CtgNSounds[ctg]++;
        }
    }
    
    void SetGroupOptions(byte ctg, int StartFreq, int EndFreq) {
        StartGroupFreq[ctg] = StartFreq;
        FinalGroupFreq[ctg] = EndFreq;
    }
    
    void StopCtgSounds() {
        memset(SoundCtgFreq, 0, sizeof(SoundCtgFreq));
    }

    void ReleaseAll();

private:
    struct SoundInstance {
        int channel;
        int bufferNum;
        bool active;
    };
    
    // Instance tracking arrays
    SoundInstance m_activeInstances[MAX_INSTANCES];  // Array of structs, not pointers
    int m_instanceCount;
    
    // Each buffer can have multiple instances
    int m_bufferInstanceLists[MAXSND1][16];
    int m_bufferInstanceCount[MAXSND1];
    
    // Filename storage for duplication
    char m_filenames[MAXSND1][256];
    
    int m_nextInstanceId;
    
    // Helper methods
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