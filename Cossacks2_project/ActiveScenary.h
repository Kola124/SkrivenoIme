struct GAMEOBJ{
	word Index=0;
	word Serial=0;
	int  Type=0;
};
class UnitsGroup;
struct GrpOrder;
typedef void GrpLink(UnitsGroup* UG);
struct GrpOrder{
	GrpOrder* Next=nullptr;
	GrpLink* Link=nullptr;
	void* Data=nullptr;
	int DataSize=0;
	DWORD ID=0;
};
void CheckDynamicalPtr(void* ptr);
class UnitsGroup;
class UnitsGroup{
public:
	word* IDS=nullptr;
	word* SNS=nullptr;
	int N=0;
	int Max=0;

	byte NI=0;
	byte NMASK=0;
	int Index=0;
	GrpOrder* GOrder=nullptr;

	//-----------debug---------
	char* file=nullptr;
	int   Line=0;
	//-------------------------

	GrpOrder* CreateOrder(byte Type);
	void DeleteLastOrder();
	void ClearOrders();
	void Allocate(int n){
		CheckDynamicalPtr(IDS);
		CheckDynamicalPtr(SNS);
		if(n>Max){
			Max=n+32;
			IDS=(word*)realloc(IDS,Max<<1);
			SNS=(word*)realloc(SNS,Max<<1);
		};
		CheckDynamicalPtr(IDS);
		CheckDynamicalPtr(SNS);
	};
	void AddNewUnit(OneObject* OB);
	void RemoveUnit(OneObject* OB);
	void CopyUnits(UnitsGroup* Dest,int start,int Nc,bool add,bool remove){
		CheckDynamicalPtr(IDS);
		CheckDynamicalPtr(SNS);
		if(start>=N)start=N-1;
		if(start<0)start=0;
		if(start+Nc>N)Nc=N-start;
		if(add){
			Dest->Allocate(Dest->N+Nc);
			memcpy(Dest->IDS+Dest->N,IDS+start,Nc<<1);
			memcpy(Dest->SNS+Dest->N,SNS+start,Nc<<1);
			Dest->N+=Nc;
		}else{
			Dest->Allocate(Nc);
			memcpy(Dest->IDS,IDS+start,Nc<<1);
			memcpy(Dest->SNS,SNS+start,Nc<<1);
			Dest->N=Nc;
		};
		if(remove&&Nc){
			int ncp=(N-start-Nc)<<1;
			memcpy(IDS+start,IDS+start+Nc,ncp);
			memcpy(SNS+start,SNS+start+Nc,ncp);
			N-=Nc;
		};
		CheckDynamicalPtr(IDS);
		CheckDynamicalPtr(SNS);
	};
	UnitsGroup(){
		memset(this,0,sizeof *this);
	};
	~UnitsGroup(){
		ClearOrders();
		if(IDS)free(IDS);
		if(SNS)free(SNS);
		memset(this,0,sizeof *this);
	};
};
struct UnitsPosition{
	word* Type=nullptr;
	int*  coor=nullptr;
	int N=0;
};
struct ZonesGroup{
	word* ZoneID=nullptr;
	int N=0;
};
typedef void StdVoid();
struct GTimer{
	int Time=0;
	bool Used=0;
	bool First=0;
};
struct LightSpot{
	int x=0,y=0,Type=0;
};
class ScenaryInterface{
public:
	void** SaveZone=nullptr;
	int *  SaveSize=nullptr;
	int    NSaves=0;
	int    MaxSaves=0;
	HINSTANCE hLib=0;
	char*  DLLName=nullptr;

	UnitsGroup* UGRP=nullptr;
	int    NUGRP=0;
	int    MaxUGRP=0;
	
	UnitsPosition* UPOS=nullptr;
	int    NUPOS=0;
	int    MaxUPOS=0;

	ZonesGroup* ZGRP=nullptr;
	int    NZGRP=0;
	int    MaxZGRP=0;

	char** Messages=nullptr;
	int    NMess=0;
	int    MaxMess=0;

	char** Sounds=nullptr;
	int    NSnd=0;
	int    MaxSnds=0;

	int NErrors=0;

	//char*  MissText;
	//int    TextSize;

	int NPages=0;
	int MaxPages=0;
	char** Page=nullptr;
	int*   PageSize=nullptr;
	char** PageID=nullptr;
	char** PageBMP=nullptr;

	bool   TextDisable[52];
	
	bool   StandartVictory=0;
	bool   Victory=0;
	char*  VictoryText=nullptr;

	bool   LooseGame=0;
	char*  LooseText=nullptr;

	char*  CentralText=nullptr;
	int CTextTime=0;

	GTimer TIME[32];
	word   TRIGGER[512];
	LightSpot LSpot[64];
	StdVoid* ScenaryHandler=nullptr;
	ScenaryInterface();
	~ScenaryInterface();
	void Load(char* Name,char* Text);
	void UnLoading();
};
extern ScenaryInterface SCENINF;
class SingleMission{
public:
	char* ID=nullptr;
	char* DLLPath=nullptr;
	char* MapName=nullptr;
	char* Name=nullptr;
	char* Description=nullptr;
	int NIntro=0;
	char** Intro=nullptr;
};
class MissPack{
public:
	SingleMission* MISS=nullptr;
	int NMiss=0;
	int MaxMiss=0;
	int CurrentMission=0;
	int* SingleMS=nullptr;
	int MSMiss=0;
	void LoadMissions();
	MissPack();
	~MissPack();
};

class OneBattle{
public:
	char* ID=nullptr;
	char* Map=nullptr;
	char* Text=nullptr;
	char* Brief=nullptr;
	char* BigMap=nullptr;
	char* MiniMap=nullptr;
	char* BigHeader=nullptr;
	char* SmallHeader=nullptr;
	char* Date=nullptr;
	char* RedUnits=nullptr;
	char* BlueUnits=nullptr;
	int NHints=0;
	char** Hints=nullptr;
	int* Coor=nullptr;
};
class OneWar{
public:
	char* Name=nullptr;
	char* Date=nullptr;
	char* Text=nullptr;
	int NBatles=0;
	int* BattleList=nullptr;
};
class WarPack{
public:
	int NWars=0;
	OneWar* Wars=nullptr;
	int NBattles=0;
	OneBattle* Battles=nullptr;
	WarPack();
	~WarPack();
};
extern WarPack WARS;
extern MissPack MISSLIST;
struct SingleCampagin{
	char* CampMessage=nullptr;
	char* CampText=nullptr;
	char* CampBmp=nullptr;
	int NMiss=0;
	int* Miss=nullptr;
	DWORD* OpenIndex=nullptr;
};
class CampaginPack{
public:
	int NCamp=0;
	SingleCampagin* SCamp=nullptr;
	CampaginPack();
	~CampaginPack();
};
extern CampaginPack CAMPAGINS;
extern byte CurAINation;
extern City* CCIT;
extern Nation* CNAT;
extern bool AiIsRunNow;
extern int CurrentCampagin;
extern int CurrentMission;
CEXPORT void LoadPlayerData();
CEXPORT void SavePlayerData();