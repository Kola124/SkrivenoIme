//
//	UDP based Communication Core
//	GSC Game World
//	� 2001 Serguei I. Ivantsov aka ManOwaR Linux Lover
//
//	Jun 22, 2001		V0.1
//	Jul 09, 2001		V0.2
//	Aug	06, 2001		V0.4
// 
//
// ��� ���������� ������ �� NAT-��, ��������� ������ ������������� �������������
// ����� Microsoft �� ���������� UDP-������� ����� ����

#define _COOL_
//#define CC_DEBUG

#ifdef CC_DEBUG
#define _log_message(message) DebugMessage(message)
#else
#define _log_message(message) 
#endif //CC_DEBUG

#ifndef _COMM_CORE_H_INCLUDED_
#define _COMM_CORE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Winsock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <assert.h>
#ifdef CC_DEBUG
#include <stdio.h>
#endif //CC_DEBUG

#pragma warning (disable : 4200)

#pragma pack(1)

// ---------------------------------------------------------------------------------------------

typedef in_addr PEER_ADDR;
typedef u_short	PEER_PORT;
typedef u_short	PEER_ID,*LPPEER_ID;
	
// ---------------------------------------------------------------------------------------------
// ���������
#define PROTO_ID				0x47534370	// ������������� ��������� ('GSCp')
#define DATA_PORT				34000		// ���� ������ (34000)
#define RETRY_COUNT				8			// ������� ��� ����� �������� �������� �����
											// � ������ ���������������
											// ����� ���� ����� ��������� �� �������
#define RETRY_TIME				2800		// �������� ������� �������� ����� � �������������
#define MAX_QUEUE_LEN			4096		// ������������ ����� ������� �������
											// ��������� �������������
#define MAX_PEERS				7			// ������������ ����� ������ 
#define CC_PROTO_VERSION		0x03		// ������ ����������������� ���������
#define MAX_PACKET_STAMP		0xEE6B2800	// ������������ ����� ������ (������������ ��� �������������)
#define RECV_BUFFER_LENGTH		8192		// ������ ��������� ������ (8 ��������)
#define MAX_HOST_NAME			32			// ������������ ����� ����� �����

// ---------------------------------------------------------------------------------------------
// �������� �����
typedef struct PEER_ENTRY{						// ����
	PEER_ADDR	m_ex_Addr;						// ������� ����� ����� (��� NAT-�)
	PEER_PORT	m_ex_Port;						// ������� ���� ����� (��� NAT-�)
	PEER_ID		m_Id;							// ������������� ����� � ������ �������
	BOOL		m_bAlive;						// �������� �� ���� �� ������ ������
	BOOL		m_bOverNAT;						// ��������� �� ���� �� NAT-�� �� ��������� � �������
	u_short		m_uLatency;						// ����� ������ �����
	CHAR		m_szUserName[MAX_HOST_NAME];	// �������� ����� / ������������
	u_short		m_uUserDataSize;				// ������ ���������������� ������
	LPBYTE		m_lpbUserData;					// ��������� �� ���������������� ������
	CHAR		m_szCCUID[23];					// ���������� ������������� �����
} *LPPEER_ENTRY;

// ---------------------------------------------------------------------------------------------
// ���� ������� (����������� � ���������)
#define CC_PT_FRAME_CONFIRM		0x01	// ���� ������������ ����� ������ ������
#define CC_PT_HOST_ALIVE		0x02	// ���� ������������, ��� �� �����; ��������� Latency
#define CC_PT_SEND_DATA			0x03	// ����� ���������������� ������� ��� �������������
#define CC_PT_SEND_DATA_SECURE	0x04	// ����� ���������������� ������� � ��������������
#define CC_PT_CHCK_HOST			0x05	// ����� ������� ����������� �����; ��������� Latency
#define CC_PT_TRY_CONNECT		0x06	// ������� ����������� � �������
#define CC_PT_CONNECT_OK		0x07	// ���������� �� ����������� � �������
#define CC_PT_CONNECT_REJECT	0x08	// ����� � ����������� � �������
#define CC_PT_SERVER_LIST		0x09	// �������� ������ ������ (�� �������)
#define CC_PT_HOST_EXIT			0x0A	// ���� ������� �� ����
#define CC_PT_HOST_DROP			0x0B	// ���� �������� �� ���� (�� �������)
#define CC_PT_CONNECT_DIFF		0x0C	// ����� �������� � ����� �� ������� �����, ���� �� ����� ��� ������
#define CC_PT_HOST_DROP_OK		0x0D	// ������ ������������ ���������� �����

#define CC_PT_SEND_USER_NAME	0x0E	// ������������ �������� ���� ���
#define CC_PT_SEND_USER_DATA	0x0F	// ������������ �������� ���� ������

#define CC_PT_SEND_NEW_NAME		0x10	// ������ �������� ������ �� ������������ ����� ������������
#define CC_PT_SEND_NEW_DATA		0x11	// ������ �������� ������ �� ������������ ������ ������������

//
// ---------------------------------------------------------------------------------------------
enum ConnectState{
	csNone,csWait,csRejected,csConnected,csTimedOut,csBadProto,csSynch
};

enum SessionState{
	ssNone,ssOpen,ssClosed
};

// ---------------------------------------------------------------------------------------------
// ������
#define	BAD_PEER_ID				255		// �������� ������������� �����
#define CE_NOT_SERVER			0x01	// ������� ����������� �� � �������
#define CE_NO_ROOM				0x02	// ��� �������� ������ �����������
#define CE_NO_ERROR				0x03	// ��� ������
#define CE_BAD_VERSION			0x04	// �������� ������ ���������
#define CE_BAD_MSG_SIZE			0x05	// ������������ ����� ���������
#define CE_SESSION_CLOSED		0x06	// ���������� ���������� ����, �.�. ������ ��� �������

// ---------------------------------------------------------------------------------------------
// ��������� �������

// ����� ��� ����� ��� ���������
typedef struct CC_PK_RAW_FRAME{
	u_long	m_lProto;				// ��� ���������
	u_short	m_uType;				// ��� ������
	u_long	m_lStamp;				// ���������� ����� ������; 0, ���� �� ��������� �������������
	PEER_ID	m_PeerId;				// ������������� �����������
// ----------------------------------
	BYTE	m_bData[];				// ������������� ��� ������ ������ ���������� �����
} *LPCC_PK_RAW_FRAME;

// CC_PT_FRAME_CONFIRM
typedef struct CC_PK_FRAME_CONFIRM{
	u_long	m_lConfirmStamp;		// ���������� ����� ������, �������������� �����
} *LPCC_PK_FRAME_CONFIRM;

// CC_PT_SEND_DATA
typedef struct CC_PK_SEND_DATA{
	u_short	m_uSize;				// ������ ���� ������
	BYTE	m_bData[];				// ���� ������ ���������� �����
} *LPCC_PK_SEND_DATA;

// CC_PT_TRY_CONNECT
typedef struct CC_PK_TRY_CONNECT{
	CHAR		m_cProtoVersion;	// ����� ������ ��������� / ����������
	CHAR		m_szUserName[MAX_HOST_NAME];
	CHAR		m_szCCUID[23];
	u_short		m_uAddrCount;
	DWORD		m_dwAddrList[];
} *LPCC_PK_TRY_CONNECT;

// CC_PT_CONNECT_OK
typedef struct CC_PK_CONNECT_OK{
	PEER_ID		m_Id;							// ����� ����� � ������ �������
	CHAR		m_szSessionName[MAX_HOST_NAME];	// �������� ������
	DWORD		m_dwOptions;
} *LPCC_PK_CONNECT_OK;

// CC_PT_CONNECT_REJECT
typedef struct CC_PK_CONNECT_REJECT{
	u_short		m_uReason;			// ������� ������
} *LPCC_PK_CONNECT_REJECT;

// CC_PT_SERVER_LIST
typedef struct CC_PK_SERVER_LIST{
	CHAR		m_szSessionName[MAX_HOST_NAME];	// �������� ������
	u_short		m_uCount;						// ���������� ������
	BYTE		m_PeerList[];					// ������ ������ ���������� + ������������ ������ :)
} *LPCC_PK_SERVER_LIST;

// CC_PT_CHCK_HOST
typedef struct CC_PK_CHCK_HOST{
	DWORD		m_dwTickCount;		// ������� ����� �����
} *LPCC_PK_CHCK_HOST;

// CC_PT_HOST_ALIVE
typedef struct CC_PK_HOST_ALIVE{
	DWORD		m_dwTickCount;		// ����� ����� � ������ �������� ������ (�������������)
} *LPCC_PK_HOST_ALIVE;

// CC_PT_HOST_EXIT
typedef struct CC_PK_HOST_EXIT{
	DWORD		m_dwReserved;		//
} *LPCC_PK_HOST_EXIT;

// CC_PT_HOST_DROP
typedef struct CC_PK_HOST_DROP{
	DWORD		m_dwReserved;		//
} *LPCC_PK_HOST_DROP;

// CC_PT_HOST_DROP_OK
typedef struct CC_PK_HOST_DROP_OK{
	DWORD		m_dwReserved;		//
} *LPCC_PK_HOST_DROP_OK;

// CC_PT_SEND_USER_NAME
typedef struct CC_PK_SEND_USER_NAME{
	CHAR		m_szUserName[MAX_HOST_NAME];
} *LPCC_PK_SEND_USER_NAME;

// CC_PT_SEND_USER_DATA
typedef struct CC_PK_SEND_USER_DATA{
	u_short		m_uUserDataSize;
	BYTE		m_UserData[];
} *LPCC_PK_SEND_USER_DATA;

// CC_PT_SEND_NEW_NAME
typedef struct CC_PK_SEND_NEW_NAME{
	PEER_ID		m_PeerId;
	CHAR		m_szUserName[MAX_HOST_NAME];
} *LPCC_PK_SEND_NEW_NAME;

// CC_PT_SEND_NEW_DATA
typedef struct CC_PK_SEND_NEW_DATA{
	PEER_ID		m_PeerId;
	u_short		m_uUserDataSize;
	BYTE		m_UserData[];
} *LPCC_PK_SEND_NEW_DATA;

// ---------------------------------------------------------------------------------------------
// �������� ������ � ������� ���������, ��������� �������������
typedef struct FRAME_ENTRY{						// �����
	LPCC_PK_RAW_FRAME			m_lpFrame;		// ��������� �� �����
	u_short						m_uSize;		// ������ ������
	PEER_ADDR					m_PeerAddr;		// ����� �����
	PEER_PORT					m_PeerPort;		// ���� �����
	DWORD						m_dwSendTime;	// ����� ��������� �������� ������
												// � �������������
	u_short						m_uRetrCount;	// ���������� ������� �������� ������
} *LPFRAME_ENTRY;

// �������� ������� ��������� ������
typedef BOOL (CALLBACK* LP_CC_IDLE_PROC)();
typedef BOOL (CALLBACK* LP_CC_ENUM_PROC)(const PEER_ID PeerID, LPCSTR lpcszPeerName);

// ---------------------------------------------------------------------------------------------
// ����� ���� �����
class CCommCore  
{
// ---------------------------------------------------------------------------------------------
public:
// ---------------------------------------------------------------------------------------------
	LP_CC_IDLE_PROC	lpIdleProc;				// ������� ��������� ������, ���������� ����� �� �����
											// ������ ����������� �������
	LP_CC_ENUM_PROC	lpEnumProc;				// ������� ��������� ������ ��� ������������ ������
// ---------------------------------------------------------------------------------------------
											// ������� ��������� ���������� �����
	BOOL SendToPeer	(PEER_ID piNumber, LPBYTE lpbBuffer, u_short u_Size, BOOL bSecure=FALSE);
											// ������� ��������� ���� ������
	BOOL SendToAll	(LPBYTE lpbBuffer, u_short u_Size, BOOL bSecure=FALSE);
											
	u_short ReceiveData	(LPBYTE lpbBuffer, LPPEER_ID lpPeerId=NULL);	// ��������� ���� ����� �� �������
											// ������ ����� ����������, �� ��������� ������������ ������
	BOOL SendDropClient(PEER_ID PeerID);	// ������ ������ ������� �����
	BOOL InitClient	(LPCSTR lpcszServerIP, LPCSTR lpcszUserName);	// IP ����� � ���� ������ � ������� aaa.bbb.ccc.ddd
	BOOL DoneClient ();						// ������ �������� ������ �� �������� ������� 
	BOOL DoneServer ();						// ��������� ������ ������� ������ ��� �����
	BOOL DeletePeer (PEER_ID piNumber);		// ������� ���� �� ������ ������
	BOOL InitServer	(LPCSTR lpcszSessionName, LPCSTR lpcszUserName);	// �������������� ������
	BOOL QueueProcess();					// ������������ ������� �������� �������
											// ������ ����� ����������

	BOOL SendServerList();					// ��������� ������ ������


	BOOL	IsOverNAT(PEER_ID PeerId);	


	LPCSTR	GetUserName(PEER_ID PeerId);	
	BOOL	SetUserName(LPCSTR lpcszUserName);	
	BOOL	SendUserName();

	BOOL	SetSessionName(LPCSTR lpcszSessionName);

	BOOL	GetUserData(PEER_ID PeerId, LPBYTE lpbUserData, u_short * puUserDataSize);
	BOOL	SetUserData(const LPBYTE lpcbUserData, u_short uUserDataSize);
	BOOL	SendUserData();

	VOID	GetServerAddress(LPSTR lpszServerAddress);

	BOOL	EnumPeers();

	BOOL	InitNetwork();					// ������������� �������� ����������
	BOOL	CloseNetwork();					// �������� �������� ����������

	CCommCore		();
	virtual ~CCommCore();

	PEER_ID	GetPeerID()						{	return m_piNumber;		}

	u_short GetPeersCount()					{	return m_uPeerCount;	}
	LPCSTR	GetSessionName()				{	return m_szSessionName;	}

	u_short	GetMaxPeers()					{	return m_uMaxPeers;		}
	VOID	SetMaxPeers(u_short uMaxPeers)	{	m_uMaxPeers=uMaxPeers;	}

	VOID	CloseSession()					{	m_ssState=ssClosed;		}

	BOOL	IsClient()						{	return !m_bServer;		}
	BOOL	IsServer()						{	return m_bServer;		}

	VOID	SetOptions(DWORD dwOptions)		{	m_dwOptions=dwOptions;	}
	DWORD	GetOptions()					{	return m_dwOptions;		}

	// ������� �������� ����
	DWORD	GetRxBytes()					{	return m_dwRxBytes;		}
	// ������� ������� ����
	DWORD	GetTxBytes()					{	return m_dwTxBytes;		}
	// ������� ����������� ����
	DWORD	GetNxBytes()					{	return m_dwNxBytes;		}
	// ����� ����� ����������� ����� ��� ������ ��������� �����
	DWORD	GetRecvTimeOut()				{	return (GetTickCount()-m_dwLastPacketTime);	}

// ---------------------------------------------------------------------------------------------
protected:
// ---------------------------------------------------------------------------------------------
#ifdef CC_DEBUG
	FILE	*	m_DebugStream;	
	VOID		DebugMessage(LPCSTR lpcszMessage);
#endif //CC_DEBUG
	
	CHAR		m_szUserName[MAX_HOST_NAME];		//
//	PEER_ADDR	m_paHostAddr;						//
//	CHAR		m_szDotAddr[18];					//
	PEER_ADDR	m_paServAddr;						//

	DWORD		m_dwAddrList[8];					// ������ ������� ��������� ������ � network order �������
	u_short		m_uAddrCount;						// ���������� ������� ��������� ������

	DWORD		m_dwLastPacketTime;

	u_short		m_uMaxPeers;
	BOOL		m_bOverNAT;

	DWORD		m_dwRxBytes;
	DWORD		m_dwTxBytes;
	DWORD		m_dwNxBytes;

	DWORD		m_dwOptions;

	CHAR		m_szSessionName[MAX_HOST_NAME];		//

	CHAR		m_szCCUID[23];

	LPBYTE		m_lpbRecvBuffer;					//

	SessionState	m_ssState;						//
	ConnectState	m_csState;						//
	BOOL			m_bBlockingCall;				// � ������ ���������� �������������� ������
													// ������-�������������

	u_short		m_uRejectReason;					//

	u_short		m_uMaxMsgSize;						//

	SOCKET		m_DataSocket;						// �����, ������������� ���������������� ���������

	u_long		m_lStamp;							// ���������������� ������� �������

	BOOL		m_bServer;							// �������� �� ���� ��������
	PEER_ID		m_piNumber;							// ������������� ����� � ������ �������
													// ��� ������� ������ ����� 0x01
	PEER_ID		m_piAutoInc;						// ���������������� ������� ������

	LPBYTE		m_lpbUserData;						// ������������ ������ � ������������
	u_short		m_uUserDataSize;					// ������ ������������ ������ � ������������

	PEER_ENTRY	m_PeerList[MAX_PEERS];				// ������ ��������� ������
	u_short		m_uPeerCount;						// ���������� ��������� ������

	FRAME_ENTRY m_FrameList[MAX_QUEUE_LEN];			// ������� ���������, ��������� �������������
	u_short		m_uFrameCount;						// ���������� ��������� � �������

// ---------------------------------------------------------------------------------------------

//	PEER_ID		GetIdBySender();					// ���������� ���������� ����� �����
													// �� ��� ������/�����
	u_short GetPeerById(PEER_ID PeerId);			//
	u_short GetPeerByCCUID(LPCSTR lpcszCCUID);
	BOOL	DropPeer(u_short uPeer);					//

	BOOL InitSocket();
	BOOL CloseSocket();
	BOOL InitHost();

	BOOL QueueAddPacket(	PEER_ADDR			PeerAddr, 
							PEER_PORT			PeerPort, 
							LPCC_PK_RAW_FRAME	lpRawFrame,
							u_short				uSize);

	BOOL SendRawPacket(		PEER_ADDR			PeerAddr,				// ����� �����
							PEER_PORT			PeerPort,				// ���� �����
							u_short				uType,					// ��� ������
							LPBYTE				lpbBuffer,				// ����� � �������
							u_short				uSize,					// ������ ������ � ������
							BOOL				bSecureMessage,			// TRUE, ���� ����� ������-��������������� �������� ������
							BOOL				bWaitForCompletion);	// TRUE, ���� ����������� �����,
																		// �.�. ����� ������������� ��� ��������


	BOOL ProcessServerList(LPCC_PK_SERVER_LIST lpServerList);
	
	BOOL ReSendFrame( u_short uFrameNum);
	VOID Cleanup();

	BOOL QueueDropPacket(int iFrameNum);
	BOOL QueueDropConfirmedPacket(u_long lStamp);
	BOOL QueueClearAll();
	BOOL QueuePacketExists(u_long lStamp);

	BOOL SendConfirmDataPacket(sockaddr_in * lpSender, u_long lStamp);
	BOOL SendConnectReject(sockaddr_in *lpSender, u_short uReason);
	BOOL SendConnectOk(sockaddr_in *lpSender, PEER_ID PeerId);
	BOOL SendDropOk(sockaddr_in *lpSender);

	BOOL SendNewName(PEER_ID PeerId);		// �������� ���������� � �����	(������)
	BOOL SendNewData(PEER_ID PeerId);		// �������� ���������� � ����	(������)

	VOID SetCommCoreUID(LPCSTR lpcszCCUID);
	VOID NewCommCoreUID(LPSTR lpszCCUID);
	VOID GetCommCoreUID(LPSTR lpszCCUID);

// ---------------------------------------------------------------------------------------------
};

#pragma warning (default : 4200)

#endif // _COMM_CORE_H_INCLUDED_
