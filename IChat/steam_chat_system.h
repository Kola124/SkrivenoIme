// Steam_ChatSystem - Drop-in replacement for GameSpy ChatSystem
// Same interface as cs_chat.h, but uses Steam instead of GameSpy

#pragma once

#ifdef STEAM

#include "..\Cossacks2_project\Steam\steam_api.h"
#include "..\Cossacks2_project\Steam\isteammatchmaking.h"
#include "..\Cossacks2_project\Steam\isteamfriends.h"

// Keep the same structures for compatibility
struct OneChatPlayer {
    char Nick[32];
    char Info[64];
    char mail[64];
    DWORD IP;
    bool InfoReqSent : 1;
    bool ValidInfo : 1;
    bool ValidMail : 1;
    bool Muted : 1;
};

class ChatMsg {
public:
    char** Nick;
    char** Message;
    int NMsg;
    int MaxMsg;
    
    ChatMsg() {
        memset(this, 0, sizeof *this);
    }
    
    ~ChatMsg() {
        for (int i = 0; i < NMsg; i++) {
            free(Message[i]);
            free(Nick[i]);
        }
        if (Message) free(Message);
        if (Nick) free(Nick);
        memset(this, 0, sizeof *this);
    }
    
    void Add(char* nick, char* msg) {
        if (NMsg >= MaxMsg) {
            MaxMsg += 16;
            Message = (char**)realloc(Message, MaxMsg << 2);
            Nick = (char**)realloc(Nick, MaxMsg << 2);
        }
        Message[NMsg] = (char*)malloc(strlen(msg) + 1);
        strcpy(Message[NMsg], msg);
        Nick[NMsg] = (char*)malloc(strlen(nick) + 1);
        strcpy(Nick[NMsg], nick);
        NMsg++;
    }
    
    bool RemoveOne(char* nick, char* buf, int Len) {
        if (NMsg) {
            strcpy(nick, Nick[0]);
            strncpy(buf, Message[0], Len - 1);
            buf[Len - 1] = 0;
            free(Nick[0]);
            free(Message[0]);
            if (NMsg > 1) {
                memmove(Nick, Nick + 1, (NMsg - 1) << 2);
                memmove(Message, Message + 1, (NMsg - 1) << 2);
            }
            NMsg--;
            return true;
        }
        return false;
    }
};

class OneChannel {
public:
    int NPlayers;
    int MaxPlayers;
    OneChatPlayer* Players;
};

#define NCHNL 2

class Steam_ChatSystem {
public:
    // Keep same member variables for compatibility
    void* chat;  // Placeholder (not used with Steam)
    void* globalCallbacks;  // Placeholder
    void* channelCallbacks;  // Placeholder
    bool Connected;
    bool Error;
    bool enterChannelSuccess[NCHNL];
    
    char serverAddress[128];
    int port;
    char chatNick[128];
    char chatUser[128];
    char chatName[128];
    char chatChannel[NCHNL][128];
    bool quit;
    int LastAccessTime;
    int CurChannel;
    
    OneChannel CCH[NCHNL];
    
    int NAbsPlayers;
    int MaxAbsPlayers;
    OneChatPlayer* AbsPlayers;
    
    ChatMsg Private;
    ChatMsg Common[NCHNL];
    
    // Steam-specific members
    CSteamID currentLobby;
    int lastLobbyUpdate;
    
    Steam_ChatSystem() {
        memset(this, 0, sizeof *this);
        lastLobbyUpdate = 0;
    }
    
    ~Steam_ChatSystem() {
        for (int q = 0; q < NCHNL; q++) {
            if (CCH[q].Players) free(CCH[q].Players);
        }
        if (AbsPlayers) free(AbsPlayers);
        memset(this, 0, sizeof *this);
    }
    
    // ========================================================================
    // Public API - Same as GameSpy version
    // ========================================================================
    
    int NPlayers() {
        return CCH[CurChannel].NPlayers;
    }
    
    OneChatPlayer* Players() {
        return CCH[CurChannel].Players;
    }
    
    void AddPlayer(char* Nick, int c) {
        for (int i = 0; i < CCH[c].NPlayers; i++)
            if (!strcmp(Nick, CCH[c].Players[i].Nick)) return;
        
        if (CCH[c].NPlayers >= CCH[c].MaxPlayers) {
            CCH[c].MaxPlayers += 64;
            CCH[c].Players = (OneChatPlayer*)realloc(CCH[c].Players, CCH[c].MaxPlayers * sizeof(OneChatPlayer));
        }
        
        memset(CCH[c].Players + CCH[c].NPlayers, 0, sizeof(OneChatPlayer));
        strcpy(CCH[c].Players[CCH[c].NPlayers].Nick, Nick);
        
        // Get Steam info
        CSteamID steamID = Steam_FindPlayerBySteamName(Nick);
        if (steamID.IsValid()) {
            const char* personaName = SteamFriends()->GetFriendPersonaName(steamID);
            if (personaName) {
                strncpy(CCH[c].Players[CCH[c].NPlayers].Info, personaName, 63);
                CCH[c].Players[CCH[c].NPlayers].InfoReqSent = 1;
            }
        }
        
        CCH[c].NPlayers++;
    }
    
    void DelPlayer(char* Nick, int c) {
        for (int i = 0; i < CCH[c].NPlayers; i++) {
            if (!strcmp(CCH[c].Players[i].Nick, Nick)) {
                if (i < CCH[c].NPlayers - 1) {
                    memcpy(CCH[c].Players + i, CCH[c].Players + i + 1,
                           (CCH[c].NPlayers - i - 1) * sizeof(OneChatPlayer));
                }
                CCH[c].NPlayers--;
                return;
            }
        }
    }
    
    void AddAbsentPlayer(char* Nick) {
        for (int i = 0; i < NAbsPlayers; i++)
            if (!strcmp(Nick, AbsPlayers[i].Nick)) return;
        
        if (NAbsPlayers >= MaxAbsPlayers) {
            MaxAbsPlayers += 64;
            AbsPlayers = (OneChatPlayer*)realloc(AbsPlayers, MaxAbsPlayers * sizeof(OneChatPlayer));
        }
        
        memset(AbsPlayers + NAbsPlayers, 0, sizeof(OneChatPlayer));
        strcpy(AbsPlayers[NAbsPlayers].Nick, Nick);
        NAbsPlayers++;
    }
    
    void DelAbsentPlayer(char* Nick) {
        for (int i = 0; i < NAbsPlayers; i++) {
            if (!strcmp(AbsPlayers[i].Nick, Nick)) {
                if (i < NAbsPlayers - 1) {
                    memcpy(AbsPlayers + i, AbsPlayers + i + 1,
                           (NAbsPlayers - i - 1) * sizeof(OneChatPlayer));
                }
                NAbsPlayers--;
                return;
            }
        }
    }
    
    void CheckMessage(char* message) {
        // Keep same logic
        char* cc;
        char cc3[128];
        BOOL PRESENT;
        
        cc = strstr(message, "@@@ADDABPL ");
        if (cc) {
            cc += 11;
            do {
                char* cc1 = strstr(cc, " ");
                if (cc1) {
                    int L = cc1 - cc;
                    memcpy(cc3, cc, L);
                    cc3[L] = 0;
                    cc = cc1 + 1;
                } else {
                    strcpy(cc3, cc);
                    cc = NULL;
                }
                
                PRESENT = 0;
                for (int q = 0; q < NCHNL; q++)
                    for (int i = 0; i < CCH[q].NPlayers; i++)
                        if (!strcmp(CCH[q].Players[i].Nick, cc3)) PRESENT = 1;
                
                if (!PRESENT) AddAbsentPlayer(cc3);
            } while (cc);
        }
    }
    
    void Setup() {
        // Steam doesn't need server address/port
        strcpy(serverAddress, "steam://");
        port = 0;
        strcpy(chatChannel[0], "Steam_Lobby");
        strcpy(chatChannel[1], "Steam_Global");
    }
    
    // ========================================================================
    // ConnectToChat - STEAM VERSION (replaces GameSpy connection)
    // ========================================================================
    
    bool ConnectToChat(char* Nick, char* Info, char* Mail, char* Chat, char* C1, char* C2) {
        strcpy(chatNick, Nick);
        strcpy(chatUser, Info);
        strcpy(chatName, Mail);
        Error = 0;
        Disconnect();
        Connected = 0;
        Setup();
        
        // STEAM: Check if Steam is running
        if (!SteamUser() || !SteamUser()->BLoggedOn()) {
            Error = 1;
            return false;
        }
        
        // Initialize Steam networking
        SteamAPI_RunCallbacks();
        
        // Mark as connected (Steam is always "connected" if logged in)
        Connected = 1;
        
        // Setup channels (Steam uses friends list, not IRC channels)
        strcpy(chatChannel[0], "Steam_Friends");
        strcpy(chatChannel[1], "Steam_Lobby");
        
        enterChannelSuccess[0] = true;
        enterChannelSuccess[1] = true;
        
        // Update friends list to populate players
        Steam_UpdateFriendsList();
        
        return Connected;
    }
    
    void Disconnect() {
        if (Connected) {
            for (int i = 0; i < NCHNL; i++) {
                enterChannelSuccess[i] = false;
                CCH[i].NPlayers = 0;
            }
        }
        Connected = 0;
    }
    
    void Process() {
        if (!Connected) return;
        
        // Process Steam callbacks
        SteamAPI_RunCallbacks();
        
        // Update player list periodically
        int currentTime = GetTickCount();
        if (currentTime - lastLobbyUpdate > 1000) {
            Steam_UpdateFriendsList();
            Steam_UpdateLobbyMembers();
            lastLobbyUpdate = currentTime;
        }
        
        // Sort players
        for (int q = 0; q < NCHNL; q++)
            SortPlayers(CCH[q].Players, CCH[q].NPlayers);
        SortPlayers(AbsPlayers, NAbsPlayers);
    }
    
    void SortPlayers(OneChatPlayer* PL, int N) {
        bool change = false;
        do {
            change = false;
            for (int i = 1; i < N; i++) {
                int r = _stricmp(PL[i].Nick, PL[i - 1].Nick);
                if (r < 0) {
                    byte PLX[sizeof(OneChatPlayer)];
                    memcpy(&PLX, PL + i, sizeof(OneChatPlayer));
                    memcpy(PL + i, PL + i - 1, sizeof(OneChatPlayer));
                    memcpy(PL + i - 1, &PLX, sizeof(OneChatPlayer));
                    change = true;
                }
            }
        } while (change);
    }
    
    // ========================================================================
    // Steam-specific helper functions
    // ========================================================================
    
    void Steam_UpdateFriendsList() {
        // Clear current player list
        CCH[0].NPlayers = 0;
        
        // Add online Steam friends
        int friendCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
        for (int i = 0; i < friendCount; i++) {
            CSteamID friendID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
            
            // Only add if online
            EPersonaState state = SteamFriends()->GetFriendPersonaState(friendID);
            if (state != k_EPersonaStateOffline) {
                const char* name = SteamFriends()->GetFriendPersonaName(friendID);
                if (name) {
                    AddPlayer((char*)name, 0);  // Channel 0 = friends list
                }
            }
        }
    }
    
    void Steam_UpdateLobbyMembers() {
        // If in a lobby, update lobby members list
        if (!currentLobby.IsValid()) return;
        
        // Clear current lobby player list
        CCH[1].NPlayers = 0;
        
        // Add lobby members
        int numMembers = SteamMatchmaking()->GetNumLobbyMembers(currentLobby);
        for (int i = 0; i < numMembers; i++) {
            CSteamID memberID = SteamMatchmaking()->GetLobbyMemberByIndex(currentLobby, i);
            const char* name = SteamFriends()->GetFriendPersonaName(memberID);
            if (name) {
                AddPlayer((char*)name, 1);  // Channel 1 = lobby members
            }
        }
    }
    
    CSteamID Steam_FindPlayerBySteamName(const char* name) {
        // Search friends for matching name
        int friendCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
        for (int i = 0; i < friendCount; i++) {
            CSteamID friendID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
            const char* friendName = SteamFriends()->GetFriendPersonaName(friendID);
            if (friendName && strcmp(friendName, name) == 0) {
                return friendID;
            }
        }
        return k_steamIDNil;
    }
};

// Define CSYS as Steam_ChatSystem instead of ChatSystem
#define ChatSystem Steam_ChatSystem
extern Steam_ChatSystem CSYS;

#else

// Use original GameSpy version
#include "cs_chat.h"

#endif // USE_STEAM_NETWORKING
