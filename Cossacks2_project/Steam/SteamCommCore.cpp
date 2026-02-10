//
// Steam-based Communication Core Implementation
// Drop-in replacement for CommCore using Steam Networking
//

#include "SteamCommCore.h"
#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------------------

CSteamCommCore::CSteamCommCore()
    : m_CallbackLobbyCreated(this, &CSteamCommCore::OnLobbyCreated)
    , m_CallbackLobbyEntered(this, &CSteamCommCore::OnLobbyEntered)
    , m_CallbackLobbyDataUpdate(this, &CSteamCommCore::OnLobbyDataUpdate)
    , m_CallbackLobbyChatUpdate(this, &CSteamCommCore::OnLobbyChatUpdate)
    , m_CallbackP2PSessionRequest(this, &CSteamCommCore::OnP2PSessionRequest)
    , m_CallbackP2PSessionConnectFail(this, &CSteamCommCore::OnP2PSessionConnectFail)
{
    m_uPeerCount = 0;
    m_bServer = TRUE;
    m_piNumber = 0;
    m_ssState = ssNone;
    m_csState = csNone;
    m_piAutoInc = 0;
    
    m_lpbRecvBuffer = (LPBYTE)malloc(RECV_BUFFER_LENGTH);
    assert(m_lpbRecvBuffer);
    
    lpIdleProc = NULL;
    lpEnumProc = NULL;
    
    m_szUserName[0] = '\0';
    m_szSessionName[0] = '\0';
    
    m_CurrentLobby = k_steamIDNil;
    m_HostSteamID = k_steamIDNil;
    m_MySteamID = k_steamIDNil;
    
    m_lpbUserData = NULL;
    m_uUserDataSize = 0;
    m_uMaxPeers = MAX_PEERS;
    
    m_dwRxBytes = 0;
    m_dwTxBytes = 0;
    m_dwOptions = 0;
    m_dwLastPacketTime = GetTickCount64();
    
    // Clear peer list
    memset(m_PeerList, 0, sizeof(m_PeerList));
}

// ---------------------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------------------

CSteamCommCore::~CSteamCommCore()
{
    if (m_lpbRecvBuffer)
        free(m_lpbRecvBuffer);
        
    Cleanup();
}

// ---------------------------------------------------------------------------------------------
// Network Initialization (replaces InitNetwork)
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::InitNetwork()
{
    // Check if Steam is running
    if (!SteamAPI_IsSteamRunning())
    {
        return FALSE;
    }
    
    // Get Steam interfaces
    if (!SteamNetworking() || !SteamMatchmaking() || !SteamUser())
    {
        return FALSE;
    }
    
    // Get my Steam ID
    m_MySteamID = SteamUser()->GetSteamID();
    
    // Convert Steam ID to CCUID format
    SteamIDToCCUID(m_MySteamID, m_szCCUID);
    
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Close Network (replaces CloseNetwork)
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::CloseNetwork()
{
    // Leave current lobby if in one
    if (m_CurrentLobby.IsValid())
    {
        SteamMatchmaking()->LeaveLobby(m_CurrentLobby);
        m_CurrentLobby = k_steamIDNil;
    }
    
    // Close all P2P sessions
    for (int i = 0; i < m_uPeerCount; i++)
    {
        if (m_PeerList[i].m_SteamID.IsValid())
        {
            SteamNetworking()->CloseP2PSessionWithUser(m_PeerList[i].m_SteamID);
        }
    }
    
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Initialize Server (replaces InitServer)
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::InitServer(LPCSTR lpcszSessionName, LPCSTR lpcszUserName)
{
    strcpy_s(m_szUserName, sizeof(m_szUserName), lpcszUserName);
    strcpy_s(m_szSessionName, sizeof(m_szSessionName), lpcszSessionName);
    
    m_bServer = TRUE;
    m_piNumber = 1;
    m_piAutoInc = 1;
    m_uPeerCount = 1;
    
    // Setup host peer entry (ourselves)
    m_PeerList[0].m_bAlive = TRUE;
    m_PeerList[0].m_Id = 1;
    m_PeerList[0].m_uLatency = 0;
    m_PeerList[0].m_bOverNAT = FALSE;
    m_PeerList[0].m_SteamID = m_MySteamID;
    m_PeerList[0].m_ex_Port = 0;
    m_PeerList[0].m_lpbUserData = m_lpbUserData;
    m_PeerList[0].m_uUserDataSize = m_uUserDataSize;
    strcpy_s(m_PeerList[0].m_szUserName, sizeof(m_PeerList[0].m_szUserName), m_szUserName);
    memcpy(m_PeerList[0].m_szCCUID, m_szCCUID, 22);
    
    m_HostSteamID = m_MySteamID;
    
    // Create Steam lobby
    ELobbyType lobbyType = (m_dwOptions == 1) ? k_ELobbyTypePrivate : k_ELobbyTypePublic;
    SteamMatchmaking()->CreateLobby(lobbyType, m_uMaxPeers);
    
    // Callback OnLobbyCreated will be triggered
    m_csState = csWait;
    
    // Wait for lobby creation
    DWORD dwStartTime = GetTickCount();
    while (m_csState == csWait && (GetTickCount() - dwStartTime) < 5000)
    {
        SteamAPI_RunCallbacks();
        if (lpIdleProc)
            lpIdleProc();
        Sleep(10);
    }
    
    if (m_csState != csConnected)
        return FALSE;
    
    m_ssState = ssOpen;
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Initialize Client (replaces InitClient)
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::InitClient(LPCSTR lpcszServerIP, LPCSTR lpcszUserName, unsigned short port)
{
    // In Steam version, lpcszServerIP is actually a lobby ID string
    // Format: "lobby_12345678901234567"
    
    strcpy_s(m_szUserName, sizeof(m_szUserName), lpcszUserName);
    
    m_bServer = FALSE;
    m_piNumber = BAD_PEER_ID;
    m_piAutoInc = 1;
    m_uPeerCount = 0;
    
    // Parse lobby ID from string
    CSteamID lobbyID;
    if (strncmp(lpcszServerIP, "lobby_", 6) == 0)
    {
        uint64 lobbyIDNum = _strtoui64(lpcszServerIP + 6, NULL, 10);
        lobbyID.SetFromUint64(lobbyIDNum);
    }
    else
    {
        // Try parsing as direct number
        uint64 lobbyIDNum = _strtoui64(lpcszServerIP, NULL, 10);
        lobbyID.SetFromUint64(lobbyIDNum);
    }
    
    if (!lobbyID.IsValid())
        return FALSE;
    
    // Join the lobby
    SteamMatchmaking()->JoinLobby(lobbyID);
    m_csState = csWait;
    
    // Wait for join result
    DWORD dwStartTime = GetTickCount();
    while (m_csState == csWait && (GetTickCount() - dwStartTime) < 5000)
    {
        SteamAPI_RunCallbacks();
        if (lpIdleProc)
            lpIdleProc();
        Sleep(10);
    }
    
    if (m_csState != csConnected)
        return FALSE;
    
    m_ssState = ssOpen;
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Send to Peer (replaces SendToPeer)
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::SendToPeer(PEER_ID piNumber, LPBYTE lpbBuffer, u_short uSize, BOOL bSecure)
{
    if (piNumber == m_piNumber)
        return TRUE;  // Don't send to ourselves
    
    u_short uPeerNum = GetPeerById(piNumber);
    if (uPeerNum == BAD_PEER_ID)
        return FALSE;
    
    CSteamID targetSteamID = m_PeerList[uPeerNum].m_SteamID;
    
    return SendSteamPacket(targetSteamID, lpbBuffer, uSize, bSecure);
}

// ---------------------------------------------------------------------------------------------
// Send to All (replaces SendToAll)
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::SendToAll(LPBYTE lpbBuffer, u_short u_Size, BOOL bSecure)
{
    BOOL bSuccess = TRUE;
    
    for (int i = 0; i < m_uPeerCount; i++)
    {
        if (m_PeerList[i].m_Id != m_piNumber)
        {
            if (!SendSteamPacket(m_PeerList[i].m_SteamID, lpbBuffer, u_Size, bSecure))
                bSuccess = FALSE;
        }
    }
    
    return bSuccess;
}

// ---------------------------------------------------------------------------------------------
// Receive Data (replaces ReceiveData)
// ---------------------------------------------------------------------------------------------

u_short CSteamCommCore::ReceiveData(LPBYTE lpbBuffer, LPPEER_ID lpPeerId)
{
    // Process incoming P2P packets
    ProcessP2PPackets();
    
    uint32 packetSize;
    
    // Check if packet available
    if (!SteamNetworking()->IsP2PPacketAvailable(&packetSize))
        return 0;
    
    if (packetSize > RECV_BUFFER_LENGTH)
        return 0;
    
    CSteamID senderSteamID;
    uint32 bytesRead;
    
    // Read the packet
    if (!SteamNetworking()->ReadP2PPacket(m_lpbRecvBuffer, RECV_BUFFER_LENGTH, 
                                          &bytesRead, &senderSteamID, 0))
        return 0;
    
    m_dwRxBytes += bytesRead;
    m_dwLastPacketTime = GetTickCount64();
    
    // Find sender in peer list
    u_short senderPeerNum = GetPeerBySteamID(senderSteamID);
    if (senderPeerNum != BAD_PEER_ID)
    {
        if (lpPeerId)
            *lpPeerId = m_PeerList[senderPeerNum].m_Id;
    }
    
    // Copy data to output buffer
    if (lpbBuffer && bytesRead > 0)
    {
        memcpy(lpbBuffer, m_lpbRecvBuffer, bytesRead);
    }
    
    return (u_short)bytesRead;
}

// ---------------------------------------------------------------------------------------------
// Helper: Send Steam Packet
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::SendSteamPacket(CSteamID target, LPBYTE lpbBuffer, u_short uSize, BOOL bReliable)
{
    EP2PSend sendType = bReliable ? k_EP2PSendReliable : k_EP2PSendUnreliable;
    
    BOOL result = SteamNetworking()->SendP2PPacket(target, lpbBuffer, uSize, sendType, 0);
    
    if (result)
    {
        m_dwTxBytes += uSize;
    }
    
    return result;
}

// ---------------------------------------------------------------------------------------------
// Helper: Process P2P Packets
// ---------------------------------------------------------------------------------------------

VOID CSteamCommCore::ProcessP2PPackets()
{
    // This is called from ReceiveData
    // Steam callbacks are processed via SteamAPI_RunCallbacks() in game loop
}

// ---------------------------------------------------------------------------------------------
// Get Peer By ID
// ---------------------------------------------------------------------------------------------

u_short CSteamCommCore::GetPeerById(PEER_ID PeerId)
{
    for (int i = 0; i < m_uPeerCount; i++)
    {
        if (m_PeerList[i].m_Id == PeerId)
            return i;
    }
    return BAD_PEER_ID;
}

// ---------------------------------------------------------------------------------------------
// Get Peer By Steam ID
// ---------------------------------------------------------------------------------------------

u_short CSteamCommCore::GetPeerBySteamID(CSteamID steamID)
{
    for (int i = 0; i < m_uPeerCount; i++)
    {
        if (m_PeerList[i].m_SteamID == steamID)
            return i;
    }
    return BAD_PEER_ID;
}

// ---------------------------------------------------------------------------------------------
// Get User Name
// ---------------------------------------------------------------------------------------------

LPCSTR CSteamCommCore::GetUserName(PEER_ID PeerId)
{
    if (PeerId == m_piNumber)
        return m_szUserName;
    
    u_short uPeerNum = GetPeerById(PeerId);
    if (uPeerNum == BAD_PEER_ID)
        return NULL;
    
    return m_PeerList[uPeerNum].m_szUserName;
}

// ---------------------------------------------------------------------------------------------
// Set User Name
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::SetUserName(LPCSTR lpcszUserName)
{
    strcpy_s(m_szUserName, sizeof(m_szUserName), lpcszUserName);
    return SendUserName();
}

// ---------------------------------------------------------------------------------------------
// Send User Name
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::SendUserName()
{
    if (m_bServer)
    {
        strcpy_s(m_PeerList[0].m_szUserName, sizeof(m_PeerList[0].m_szUserName), m_szUserName);
        return SendServerList();
    }
    
    // Client: send name update to host
    // Steam handles this via lobby data
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Get/Set User Data
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::GetUserData(PEER_ID PeerId, LPBYTE lpbUserData, u_short* puUserDataSize)
{
    if (PeerId == m_piNumber)
    {
        if (lpbUserData && m_lpbUserData)
            memcpy(lpbUserData, m_lpbUserData, m_uUserDataSize);
        if (puUserDataSize)
            *puUserDataSize = m_uUserDataSize;
        return TRUE;
    }
    
    u_short uPeerNum = GetPeerById(PeerId);
    if (uPeerNum == BAD_PEER_ID)
    {
        if (puUserDataSize)
            *puUserDataSize = 0;
        return FALSE;
    }
    
    if (lpbUserData && m_PeerList[uPeerNum].m_lpbUserData)
        memcpy(lpbUserData, m_PeerList[uPeerNum].m_lpbUserData, m_PeerList[uPeerNum].m_uUserDataSize);
    
    if (puUserDataSize)
        *puUserDataSize = m_PeerList[uPeerNum].m_uUserDataSize;
    
    return TRUE;
}

BOOL CSteamCommCore::SetUserData(const LPBYTE lpcbUserData, u_short uUserDataSize)
{
    if (m_lpbUserData)
        free(m_lpbUserData);
    
    m_lpbUserData = (LPBYTE)malloc(uUserDataSize);
    if (!m_lpbUserData)
        return FALSE;
    
    m_uUserDataSize = uUserDataSize;
    memcpy(m_lpbUserData, lpcbUserData, uUserDataSize);
    
    return SendUserData();
}

BOOL CSteamCommCore::SendUserData()
{
    if (!m_lpbUserData)
        return FALSE;
    
    if (m_bServer)
    {
        if (m_PeerList[0].m_lpbUserData)
            free(m_PeerList[0].m_lpbUserData);
        
        m_PeerList[0].m_lpbUserData = (LPBYTE)malloc(m_uUserDataSize);
        if (!m_PeerList[0].m_lpbUserData)
            return FALSE;
        
        memcpy(m_PeerList[0].m_lpbUserData, m_lpbUserData, m_uUserDataSize);
        m_PeerList[0].m_uUserDataSize = m_uUserDataSize;
        
        return SendServerList();
    }
    
    // Client: handled via Steam lobby member data
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Session Management
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::SetSessionName(LPCSTR lpcszSessionName)
{
    strcpy_s(m_szSessionName, sizeof(m_szSessionName), lpcszSessionName);
    
    // Update lobby data if we're the host
    if (m_bServer && m_CurrentLobby.IsValid())
    {
        SteamMatchmaking()->SetLobbyData(m_CurrentLobby, "name", m_szSessionName);
    }
    
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Send Server List (now updates lobby member list)
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::SendServerList()
{
    // With Steam, the peer list is automatically synchronized via lobby
    // Just update our lobby data
    if (m_CurrentLobby.IsValid())
    {
        UpdatePeerListFromLobby();
    }
    
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Update Peer List from Lobby
// ---------------------------------------------------------------------------------------------

VOID CSteamCommCore::UpdatePeerListFromLobby()
{
    if (!m_CurrentLobby.IsValid())
        return;
    
    int numMembers = SteamMatchmaking()->GetNumLobbyMembers(m_CurrentLobby);
    
    // Clear existing peer list (except host)
    for (int i = 1; i < m_uPeerCount; i++)
    {
        if (m_PeerList[i].m_lpbUserData)
            free(m_PeerList[i].m_lpbUserData);
    }
    
    m_uPeerCount = 0;
    
    PEER_ENTRY& myPeer = m_PeerList[m_uPeerCount];
    myPeer.m_bAlive = TRUE;
    myPeer.m_SteamID = m_MySteamID;
    myPeer.m_Id = m_piNumber;  // My peer ID
    myPeer.m_uLatency = 0;
    myPeer.m_bOverNAT = FALSE;
    myPeer.m_ex_Port = 0;
    myPeer.m_lpbUserData = NULL;
    myPeer.m_uUserDataSize = 0;

    // Get my name from Steam Friends API
    if(SteamFriends()) {
        const char* myName = SteamFriends()->GetPersonaName();
        if(myName && myName[0]) {
            strcpy_s(myPeer.m_szUserName, sizeof(myPeer.m_szUserName), myName);
        } else {
            strcpy_s(myPeer.m_szUserName, sizeof(myPeer.m_szUserName), "Player");
        }
    } else {
        // Fallback to stored name
        if(m_szUserName[0]) {
            strcpy_s(myPeer.m_szUserName, sizeof(myPeer.m_szUserName), m_szUserName);
        } else {
            strcpy_s(myPeer.m_szUserName, sizeof(myPeer.m_szUserName), "Player");
        }
    }

    SteamIDToCCUID(m_MySteamID, myPeer.m_szCCUID);

    m_uPeerCount++;  // Increment count to include myself

    // Add host (ourselves if server)
    if (m_bServer)
    {
        m_uPeerCount = 1;
    }
    
    // Add all lobby members
    for (int i = 0; i < numMembers; i++)
    {
        CSteamID memberID = SteamMatchmaking()->GetLobbyMemberByIndex(m_CurrentLobby, i);
        
        // Skip ourselves
        if (memberID == m_MySteamID)
            continue;
        
        if (m_uPeerCount >= MAX_PEERS)
            break;
        
        // Get member name from Steam
        const char* memberName = SteamFriends()->GetFriendPersonaName(memberID);
        
        // Add to peer list
        PEER_ENTRY& peer = m_PeerList[m_uPeerCount];
        peer.m_bAlive = TRUE;
        peer.m_SteamID = memberID;
        peer.m_Id = ++m_piAutoInc;
        peer.m_uLatency = 0;
        peer.m_bOverNAT = FALSE;
        peer.m_ex_Port = 0;
        peer.m_lpbUserData = NULL;
        peer.m_uUserDataSize = 0;
        
        strcpy_s(peer.m_szUserName, sizeof(peer.m_szUserName), memberName);
        SteamIDToCCUID(memberID, peer.m_szCCUID);
        
        m_uPeerCount++;
    }
}

// ---------------------------------------------------------------------------------------------
// Drop Peer
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::DropPeer(u_short uPeer)
{
    if (uPeer >= m_uPeerCount)
        return FALSE;
    
    // Close P2P session
    if (m_PeerList[uPeer].m_SteamID.IsValid())
    {
        SteamNetworking()->CloseP2PSessionWithUser(m_PeerList[uPeer].m_SteamID);
    }
    
    // Free user data
    if (m_PeerList[uPeer].m_lpbUserData)
    {
        free(m_PeerList[uPeer].m_lpbUserData);
    }
    
    // Shift peer list
    memmove(&m_PeerList[uPeer], &m_PeerList[uPeer + 1], 
            (m_uPeerCount - uPeer - 1) * sizeof(PEER_ENTRY));
    
    m_uPeerCount--;
    
    return TRUE;
}

BOOL CSteamCommCore::DeletePeer(PEER_ID piNumber)
{
    u_short uPeer = GetPeerById(piNumber);
    if (uPeer == BAD_PEER_ID)
        return FALSE;
    
    return DropPeer(uPeer);
}

BOOL CSteamCommCore::SendDropClient(PEER_ID PeerID)
{
    u_short uPeerNum = GetPeerById(PeerID);
    if (uPeerNum == BAD_PEER_ID)
        return FALSE;
    
    // Kick from lobby (server only)
    if (m_bServer && m_CurrentLobby.IsValid())
    {
        // Steam doesn't have direct kick, but we can close P2P session
        SteamNetworking()->CloseP2PSessionWithUser(m_PeerList[uPeerNum].m_SteamID);
    }
    
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Cleanup
// ---------------------------------------------------------------------------------------------

VOID CSteamCommCore::Cleanup()
{
    // Free all user data
    for (int i = 0; i < m_uPeerCount; i++)
    {
        if (m_PeerList[i].m_lpbUserData)
        {
            free(m_PeerList[i].m_lpbUserData);
            m_PeerList[i].m_lpbUserData = NULL;
        }
    }
    
    m_uPeerCount = 0;
    
    if (m_lpbUserData)
    {
        free(m_lpbUserData);
        m_lpbUserData = NULL;
    }
    
    m_uUserDataSize = 0;
    
    m_csState = csNone;
    m_ssState = ssNone;
}

BOOL CSteamCommCore::DoneClient()
{
    if (m_CurrentLobby.IsValid())
    {
        SteamMatchmaking()->LeaveLobby(m_CurrentLobby);
        m_CurrentLobby = k_steamIDNil;
    }
    
    Cleanup();
    return TRUE;
}

BOOL CSteamCommCore::DoneServer()
{
    // Kick all clients
    for (int i = 1; i < m_uPeerCount; i++)
    {
        SendDropClient(m_PeerList[i].m_Id);
    }
    
    if (m_CurrentLobby.IsValid())
    {
        SteamMatchmaking()->LeaveLobby(m_CurrentLobby);
        m_CurrentLobby = k_steamIDNil;
    }
    
    Cleanup();
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Enumerate Peers
// ---------------------------------------------------------------------------------------------

BOOL CSteamCommCore::EnumPeers()
{
    if (lpEnumProc)
    {
        for (int i = 0; i < m_uPeerCount; i++)
        {
            BOOL bContinue;
            if (m_PeerList[i].m_Id == m_piNumber)
                bContinue = lpEnumProc(m_piNumber, m_szUserName);
            else
                bContinue = lpEnumProc(m_PeerList[i].m_Id, m_PeerList[i].m_szUserName);
            
            // If callback returns FALSE, stop enumeration
            if (!bContinue)
                break;
        }
    }
    
    return TRUE;
}

// ---------------------------------------------------------------------------------------------
// Get Server Address
// ---------------------------------------------------------------------------------------------

VOID CSteamCommCore::GetServerAddress(LPSTR lpszServerAddress)
{
    if (lpszServerAddress && m_CurrentLobby.IsValid())
    {
        sprintf_s(lpszServerAddress, 64, "lobby_%llu", m_CurrentLobby.ConvertToUint64());
    }
}

// ---------------------------------------------------------------------------------------------
// Convert Steam ID to CCUID
// ---------------------------------------------------------------------------------------------

VOID CSteamCommCore::SteamIDToCCUID(CSteamID steamID, LPSTR lpszCCUID)
{
    sprintf_s(lpszCCUID, 23, "STEAM-%016llX", steamID.ConvertToUint64());
}

// ---------------------------------------------------------------------------------------------
// STEAM CALLBACKS
// ---------------------------------------------------------------------------------------------

void CSteamCommCore::OnLobbyCreated(LobbyCreated_t* pCallback)
{
    if (pCallback->m_eResult != k_EResultOK)
    {
        m_csState = csRejected;
        return;
    }
    
    m_CurrentLobby.SetFromUint64(pCallback->m_ulSteamIDLobby);
    m_HostSteamID = m_MySteamID;
    
    // Set lobby data
    SteamMatchmaking()->SetLobbyData(m_CurrentLobby, "name", m_szSessionName);
    SteamMatchmaking()->SetLobbyData(m_CurrentLobby, "version", "COSSACKS_V1");
    
    char optionsStr[16];
    sprintf_s(optionsStr, sizeof(optionsStr), "%lu", m_dwOptions);
    SteamMatchmaking()->SetLobbyData(m_CurrentLobby, "options", optionsStr);
    
    m_csState = csConnected;
    m_ssState = ssOpen;
}

void CSteamCommCore::OnLobbyEntered(LobbyEnter_t* pCallback)
{
    if (pCallback->m_EChatRoomEnterResponse != k_EChatRoomEnterResponseSuccess)
    {
        m_csState = csRejected;
        return;
    }
    
    m_CurrentLobby.SetFromUint64(pCallback->m_ulSteamIDLobby);
    m_HostSteamID = SteamMatchmaking()->GetLobbyOwner(m_CurrentLobby);
    
    // Get lobby data
    const char* lobbyName = SteamMatchmaking()->GetLobbyData(m_CurrentLobby, "name");
    if (lobbyName)
    {
        strcpy_s(m_szSessionName, sizeof(m_szSessionName), lobbyName);
    }
    
    const char* optionsStr = SteamMatchmaking()->GetLobbyData(m_CurrentLobby, "options");
    if (optionsStr)
    {
        m_dwOptions = atoi(optionsStr);
    }
    
    // Assign ourselves a peer ID
    m_piNumber = ++m_piAutoInc;
    
    // Update peer list
    UpdatePeerListFromLobby();
    
    m_csState = csConnected;
}

void CSteamCommCore::OnLobbyDataUpdate(LobbyDataUpdate_t* pCallback)
{
    if (pCallback->m_ulSteamIDLobby == m_CurrentLobby.ConvertToUint64())
    {
        UpdatePeerListFromLobby();
    }
}

void CSteamCommCore::OnLobbyChatUpdate(LobbyChatUpdate_t* pCallback)
{
    if (pCallback->m_ulSteamIDLobby == m_CurrentLobby.ConvertToUint64())
    {
        UpdatePeerListFromLobby();
    }
}

void CSteamCommCore::OnP2PSessionRequest(P2PSessionRequest_t* pCallback)
{
    // Auto-accept P2P sessions from lobby members
    if (m_CurrentLobby.IsValid())
    {
        // Check if requester is in our lobby
        int numMembers = SteamMatchmaking()->GetNumLobbyMembers(m_CurrentLobby);
        for (int i = 0; i < numMembers; i++)
        {
            CSteamID memberID = SteamMatchmaking()->GetLobbyMemberByIndex(m_CurrentLobby, i);
            if (memberID == pCallback->m_steamIDRemote)
            {
                SteamNetworking()->AcceptP2PSessionWithUser(pCallback->m_steamIDRemote);
                return;
            }
        }
    }
}

void CSteamCommCore::OnP2PSessionConnectFail(P2PSessionConnectFail_t* pCallback)
{
    // Handle P2P connection failure
    // Could log or notify user
}