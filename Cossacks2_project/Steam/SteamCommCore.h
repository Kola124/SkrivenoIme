//
// Steam-based Communication Core
// Converted from UDP CommCore to Steam Networking
// Compatible replacement for CCommCore
//

#ifndef _STEAM_COMM_CORE_H_INCLUDED_
#define _STEAM_COMM_CORE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include <Windows.h>
#include <assert.h>
#include <vector>
#include <map>

// Steam SDK headers
#include "..\Steam\steam_api.h"
#include "..\Steam\isteamnetworking.h"
#include "..\Steam\isteammatchmaking.h"
#include "..\Steam\isteamfriends.h"

#pragma pack(1)

// ---------------------------------------------------------------------------------------------
// Keep existing types for compatibility
typedef CSteamID PEER_ADDR;  // Changed: was in_addr, now CSteamID
typedef u_short  PEER_PORT;  // Kept for compatibility, but not really used in Steam
typedef u_short  PEER_ID, *LPPEER_ID;

// ---------------------------------------------------------------------------------------------
// Constants
#define MAX_PEERS               7       // Maximum number of hosts
#define MAX_HOST_NAME           32      // Maximum length of host name
#define RECV_BUFFER_LENGTH      8192    // Receive buffer size

// ---------------------------------------------------------------------------------------------
// Peer entry structure
typedef struct PEER_ENTRY
{
    CSteamID    m_SteamID;                      // Steam ID (replaces IP address)
    PEER_PORT   m_ex_Port;                      // Kept for compatibility (unused)
    PEER_ID     m_Id;                           // Peer ID in server list
    BOOL        m_bAlive;                       // Is peer available
    BOOL        m_bOverNAT;                     // Always FALSE with Steam (handles NAT)
    u_short     m_uLatency;                     // Response time
    CHAR        m_szUserName[MAX_HOST_NAME];    // Peer/user name
    u_short     m_uUserDataSize;                // User data size
    LPBYTE      m_lpbUserData;                  // Pointer to user data
    CHAR        m_szCCUID[23];                  // Global host identifier (Steam ID as string)
} *LPPEER_ENTRY;

// ---------------------------------------------------------------------------------------------
// Enums
enum ConnectState
{
    csNone, csWait, csRejected, csConnected, csTimedOut, csBadProto, csSynch
};

enum SessionState
{
    ssNone, ssOpen, ssClosed
};

// ---------------------------------------------------------------------------------------------
// Error codes
#define BAD_PEER_ID         255

// ---------------------------------------------------------------------------------------------
// Callback function types (must match original CommCore signatures)
typedef VOID(__stdcall *LP_CC_IDLE_PROC)();
typedef BOOL(__stdcall *LP_CC_ENUM_PROC)(PEER_ID, LPCSTR);

// ---------------------------------------------------------------------------------------------
// Main Steam Communication Core class
class CSteamCommCore
{
public:
    // Callback handler for Steam
    LP_CC_IDLE_PROC lpIdleProc;
    LP_CC_ENUM_PROC lpEnumProc;

    // ---------------------------------------------------------------------------------------------
    // Core functions (compatible with old CommCore API)
    
    BOOL SendToPeer(PEER_ID piNumber, LPBYTE lpbBuffer, u_short u_Size, BOOL bSecure = FALSE);
    BOOL SendToAll(LPBYTE lpbBuffer, u_short u_Size, BOOL bSecure = FALSE);
    u_short ReceiveData(LPBYTE lpbBuffer, LPPEER_ID lpPeerId = NULL);
    
    BOOL SendDropClient(PEER_ID PeerID);
    BOOL InitClient(LPCSTR lpcszServerIP, LPCSTR lpcszUserName, unsigned short port);
    BOOL DoneClient();
    BOOL DoneServer();
    BOOL DeletePeer(PEER_ID piNumber);
    BOOL InitServer(LPCSTR lpcszSessionName, LPCSTR lpcszUserName);
    
    // Not needed with Steam, but kept for compatibility
    BOOL QueueProcess() { return TRUE; }  // Steam handles this automatically
    
    BOOL SendServerList();
    BOOL IsOverNAT(PEER_ID PeerId) { return FALSE; }  // Steam handles NAT automatically
    
    LPCSTR GetUserName(PEER_ID PeerId);
    BOOL SetUserName(LPCSTR lpcszUserName);
    BOOL SendUserName();
    
    BOOL SetSessionName(LPCSTR lpcszSessionName);
    
    BOOL GetUserData(PEER_ID PeerId, LPBYTE lpbUserData, u_short* puUserDataSize);
    BOOL SetUserData(const LPBYTE lpcbUserData, u_short uUserDataSize);
    BOOL SendUserData();
    
    // No longer needed - Steam handles NAT automatically
    BOOL SendUdpHolePunch(sockaddr* server, char* content, const int content_len) { return TRUE; }
    
    VOID GetServerAddress(LPSTR lpszServerAddress);
    BOOL EnumPeers();
    
    BOOL InitNetwork();
    BOOL CloseNetwork();
    
    CSteamCommCore();
    virtual ~CSteamCommCore();
    
    // Accessors
    PEER_ID GetPeerID() { return m_piNumber; }
    u_short GetPeersCount() { return m_uPeerCount; }
    LPCSTR GetSessionName() { return m_szSessionName; }
    u_short GetMaxPeers() { return m_uMaxPeers; }
    VOID SetMaxPeers(u_short uMaxPeers) { m_uMaxPeers = uMaxPeers; }
    VOID CloseSession() { m_ssState = ssClosed; }
    BOOL IsClient() { return !m_bServer; }
    BOOL IsServer() { return m_bServer; }
    VOID SetOptions(DWORD dwOptions) { m_dwOptions = dwOptions; }
    DWORD GetOptions() { return m_dwOptions; }
    
    DWORD GetRxBytes() { return m_dwRxBytes; }
    DWORD GetTxBytes() { return m_dwTxBytes; }
    DWORD GetNxBytes() { return 0; }  // Not tracked in Steam
    
    ULONGLONG GetRecvTimeOut() { return (GetTickCount() - m_dwLastPacketTime); }

protected:
    // ---------------------------------------------------------------------------------------------
    // Member variables
    CHAR        m_szUserName[MAX_HOST_NAME];
    CHAR        m_szSessionName[MAX_HOST_NAME];
    CHAR        m_szCCUID[23];
    
    CSteamID    m_HostSteamID;          // Steam ID of host
    CSteamID    m_MySteamID;            // My Steam ID
    CSteamID    m_CurrentLobby;         // Current lobby ID
    
    ULONGLONG   m_dwLastPacketTime;
    u_short     m_uMaxPeers;
    
    DWORD       m_dwRxBytes;
    DWORD       m_dwTxBytes;
    DWORD       m_dwOptions;
    
    SessionState    m_ssState;
    ConnectState    m_csState;
    u_short         m_uRejectReason;
    
    BOOL        m_bServer;
    PEER_ID     m_piNumber;
    PEER_ID     m_piAutoInc;
    
    LPBYTE      m_lpbUserData;
    u_short     m_uUserDataSize;
    
    PEER_ENTRY  m_PeerList[MAX_PEERS];
    u_short     m_uPeerCount;
    
    // Receive buffer
    LPBYTE      m_lpbRecvBuffer;
    
    // Steam callback handlers
    void OnLobbyCreated(LobbyCreated_t* pCallback);
    void OnLobbyEntered(LobbyEnter_t* pCallback);
    void OnLobbyDataUpdate(LobbyDataUpdate_t* pCallback);
    void OnLobbyChatUpdate(LobbyChatUpdate_t* pCallback);
    void OnP2PSessionRequest(P2PSessionRequest_t* pCallback);
    void OnP2PSessionConnectFail(P2PSessionConnectFail_t* pCallback);
    
    // Steam callback objects (member variables)
    CCallback<CSteamCommCore, LobbyCreated_t> m_CallbackLobbyCreated;
    CCallback<CSteamCommCore, LobbyEnter_t> m_CallbackLobbyEntered;
    CCallback<CSteamCommCore, LobbyDataUpdate_t> m_CallbackLobbyDataUpdate;
    CCallback<CSteamCommCore, LobbyChatUpdate_t> m_CallbackLobbyChatUpdate;
    CCallback<CSteamCommCore, P2PSessionRequest_t> m_CallbackP2PSessionRequest;
    CCallback<CSteamCommCore, P2PSessionConnectFail_t> m_CallbackP2PSessionConnectFail;
    
    // Helper functions
    u_short GetPeerById(PEER_ID PeerId);
    u_short GetPeerBySteamID(CSteamID steamID);
    BOOL DropPeer(u_short uPeer);
    VOID Cleanup();
    VOID ProcessP2PPackets();
    BOOL SendSteamPacket(CSteamID target, LPBYTE lpbBuffer, u_short uSize, BOOL bReliable);
    VOID UpdatePeerListFromLobby();
    VOID SteamIDToCCUID(CSteamID steamID, LPSTR lpszCCUID);
};

#pragma pack()

#endif // _STEAM_COMM_CORE_H_INCLUDED_