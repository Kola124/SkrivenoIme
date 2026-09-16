#define MaxPL 8
struct EXBUFFER{
	DWORD Size=0;
	bool Enabled=0;
	DWORD Sign=0;//0xF376425E
	DWORD RealTime=0;//if(??==0xFFFFFFFF)-empty buffer
	DWORD RandIndex=0;
    byte  Data[4096] = {0};
};
struct OnePing{
	int FromTime=0;
	int ToTime=0;
	int BackTime=0;
};
class PingsSet{
public:
	DWORD DPID=0;
	int NPings=0;
	int MaxPings=0;
	OnePing* Pings=nullptr;
};
class PingSumm{
public:
	int NPL=0;
	PingsSet* PSET=nullptr;
	PingSumm();
	~PingSumm();
	void ClearPingInfo();
	void AddPing(DWORD DPID,DWORD From,DWORD To,DWORD Back);
	void AddPlayer(DWORD DPID);
	int GetTimeDifference(DWORD DPID);
	int CheckPlayer(DWORD DPID);
};
extern PingSumm PSUMM;
struct BACKUPSTR{
	DWORD  ID=0;
	DWORD RealTime=0;
    byte* Data = nullptr;
	int   L=0;
};
class PLAYERSBACKUP{
public:
    BACKUPSTR BSTR[32] = {0};
	int NBDATA=0;
	PLAYERSBACKUP();
	~PLAYERSBACKUP();
	void Clear();
	void AddInf(byte* BUF,int L,DWORD ID,int RT);
	void SendInfoAboutTo(DWORD ID,DWORD TO,DWORD RT);
};
struct SingleRetr{
	DWORD IDTO=0;
	DWORD IDFROM=0;
	DWORD RT=0;
};
class RETRANS{
public:
	SingleRetr* TOT=nullptr;
	int NRET=0;
	int MaxRET=0;
	RETRANS();
	~RETRANS();
	void AddOneRet(DWORD TO,DWORD From,DWORD RT);
	void AddSection(DWORD TO,DWORD From,DWORD RT);
	void CheckRetr(DWORD From,DWORD RT);
	void Clear();
};
extern PLAYERSBACKUP PBACK;
extern RETRANS RETSYS;
struct RoomInfo{
    char Name[128] = {0};
    char Nick[64] = {0};
    char RoomIP[32] = {0};
	DWORD Profile=0;
    char GameID[64] = {0};
	int MaxPlayers=0;

    //Additional members to pass data from server to main exe / CommCore
    long player_id=0; //Necessary for host to send udp hole punching packets
    unsigned short port=0; //Udp hole punching port or real port of game host
    unsigned udp_interval=0; //Udp hole punching packet interval
    char udp_server[16] = {0}; //IP of udp hole punching server
};
CIMPORT
int Process_GSC_ChatWindow(bool Active,RoomInfo* RIF);
extern bool UseGSC_Login;
extern RoomInfo GlobalRIF;
CIMPORT
void LeaveGSCRoom();
CIMPORT
void StartGSCGame(char* Options,char* Map,int NPlayers,int* Profiles,char** Nations,int* Teams,int* Colors);
struct OnePlayerReport{
	DWORD Profile=0;
	byte State=0;
	word Score=0;
	word Population=0;
    DWORD ReachRes[6] = {0};
	word NBornP=0;
	word NBornUnits=0;
};
CIMPORT
void ReportGSCGame(int time,int NPlayers,OnePlayerReport* OPR);
CIMPORT
void ReportAliveState(int NPlayers,int* Profiles);