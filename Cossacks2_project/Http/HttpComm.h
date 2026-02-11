// HttpComm.h: interface for the CHttpComm class.
//
//////////////////////////////////////////////////////////////////////

enum ConnectionStage{csConnecting,csSending,csReceiving,csWaiting,csError};

#define RECV_BUFFER_SIZE	16*1024

typedef struct SHttpQuery
{
	DWORD				m_dwHandle=0;			// handle
	SOCKET				m_Socket=0;			// socket
	LPSTR				m_lpszQuery=0;		// query
	LPVOID				m_lpvBuffer=0;		// receive buffer
	DWORD				m_dwDataSize=0;		// received data size
	ConnectionStage		m_Stage;			// connection stage
}*PHttpQuery;

class CHttpComm  
{
public:
	VOID		ProcessRequests();
	DWORD		AddRequest(LPCSTR lpcszURL);
	int			GetData(DWORD dwHandle, LPVOID lpvBuffer, DWORD dwBufferSize);
	VOID		FreeData(DWORD dwHandle);
	CHttpComm();
	virtual ~CHttpComm();
protected:
	BOOL		m_bInitialized=0;
	CHAR		m_szProxyAddr[255]={0};
	DWORD		m_bUseProxy=0;
	DWORD		m_dwProxyPort=0;
	DWORD		m_dwRequestCount=0;
	DWORD		m_dwHandleAuto=0;
	PHttpQuery	m_pRequestList=0;
};


