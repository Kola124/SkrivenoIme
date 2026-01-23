#include ".\New\UdpHolePuncher.h"
#include "Limitations.h"
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "multipl.h"
#include "fog.h"
#include "walls.h"
#include "Nature.h"
#include <time.h>
#include "Nucl.h"

#include "Megapolis.h"
#include "Dialogs.h"
#include "fonts.h"
#include "GSound.h"
#include "3DGraph.h"
#include "3DMapEd.h"
#include "MapSprites.h"
#include <assert.h>
#include <math.h>
#include "NewMon.h"
#include "IconTool.h"
#include "GP_Draw.h"
#include "3DRandMap.h"
#include "path.h"
#include <crtdbg.h>
#include "ActiveScenary.h"
#include "DrawForm.h"
#include "Conststr.h"
#include <Process.h>
#include "MPlayer.h"
#include "Recorder.h"
#include "GSINC.H"
#include "TopoGraf.h"
//#include "PeerClass.h"
#include "CEngine\goaceng.h"
#include "StrategyResearch.h"
#include "Safety.h"
#include "EinfoClass.h"
#include "mode.h"
//#pragma pack(4)
//#include "InetRaiting\stats\gstats.h"
//#include "InetRaiting\stats\gpersist.h"
//#include "InetRaiting\gp.h"
#pragma pack(1)
#include "IR.H"
#include "bmptool.h"
#include "diplomacy.h"
#include "Dialogs\AllDialogs.h"
#define IDIRECTPLAY2_OR_GREATER
#include "dplay.h"
#include "dplobby.h"

#include "vital\vital_def.h"
//#define AI_CHEATS
//#define FRANCEVERSION
//#define SINGLETESTNETWORK
//#define TESTVERSION
//#define TUTORIAL_DEMOVERSION

//#define AC_BATTLE_ONLY

//#define INTF_AC
#define INTF_AC_ADD

UdpHolePuncher udp_hole_puncher;

extern bool RUNMAPEDITOR;
extern bool RUNUSERMISSION;
extern char USERMISSPATH[128];

extern int CurrentSinBattle;

byte RedColor2=0xBB;
extern int CurrentAnswer;
bool CommEnterName();
bool NotifyFirewallState();
void UnPress();
int CurrentNation=-1;
extern int PitchTicks;
extern int MaxPingTime;
void StartPing(DWORD DPID,int ID);
void EndPing(int ID);
bool CheckLogin();
char* GetLString(DWORD DPID);
#define MinPing0 50
void ShowLoading();
CEXPORT int CurProtocol=0;
extern char CHATSTRING[256];
extern DWORD CHATDPID;
void SendChat(char* str,bool);
extern int ModeLX[32];
extern int ModeLY[32];
extern int NModes;
extern int RM_LandType;
extern int RM_Resstart;
extern int RM_Restot;
extern int screen_width;
extern int screen_height;
void CenterScreen();
bool Camp_EnterName();
bool ProcessSingleCampagin(int n);
void CreateNationalMaskForMap(char* Name);
extern bool realLpressed;
void CreateMiniMap();
void CreateGamesList(ListBox* LB);
char* GetTextByID(char* ID);
void CreateRandomMap(char* Name);
void RandomMapDialog(char* Result);
void RandomMapDialog1(char* Result,int N,char* Nats);
bool IgnoreSlow=false;
void PrepareGameMedia(byte myid,bool);
void PrepareGameMedia(byte myid,bool SaveNR,bool DisableDLL);
extern int exRealLx;
extern int exRealLy;
bool CHKV(char* Name);
bool WaitingHostGame(int);
void NewMap(int szX,int szY);
bool WaitingJoinGame(int);
void SlowLoadPalette(LPCSTR lpFileName);
void SlowUnLoadPalette(LPCSTR lpFileName);
void SetDarkPalette();
void CopyToScreen(int x,int y,int Lx,int Ly);
void ChooseInterior(int Type);
void Save3DMap(char* Map);
void Load3DMap(char* Map);
void EditMiniMap();
extern short WheelDelta;
SFLB_DLLEXPORT LoadMEDButtons();
int time1,time2,time3,time4,time5;
extern int time8;
SFLB_DLLEXPORT processMLoadGame();
extern char GameName[128];
bool ContinueGame;
extern word rpos;
char CurrentMap[64];
void CBar(int x,int y,int Lx,int Ly,byte c);
void PrepareEditMedia(byte myid);
extern bool EditMapMode;
extern int RES[8][8];
extern byte mapPos[16];
void PreLoadExtendedMap(char* s);
void PostLoadExtendedMap(char* s);
void InitGame();
int nsmaplx;
int nsmaply;
int GameMode;
extern int LastKey;
extern bool KeyPressed;
void MFix();
//SQPicture MainMenuGround;
CEXPORT int WarSound;
CEXPORT int WorkSound;
CEXPORT int OrderSound;
CEXPORT int MidiSound;
/*
SQPicture w164x28d;
SQPicture r164x28e;
SQPicture r164x28p;
SQPicture w224x28d;
SQPicture r224x28e;

SQPicture sb_up_e;
SQPicture sb_up_p;
SQPicture sb_up_d;

SQPicture sb_dn_e;
SQPicture sb_dn_p;
SQPicture sb_dn_d;

SQPicture sb_lf_e;
SQPicture sb_lf_p;
SQPicture sb_lf_d;

SQPicture sb_ri_e;
SQPicture sb_ri_p;
SQPicture sb_ri_d;

SQPicture sb_vbar;
SQPicture sb_vbare;
SQPicture sb_hbar;
SQPicture sb_hbare;

SQPicture sb_btn;

SQPicture chk1_on;
SQPicture chk1_off;
SQPicture chk2_on;
SQPicture chk2_off;
*/
//SQPicture w160x160;
//SQPicture w160x256;
//SQPicture y288x256;
//SQPicture y288x128;
//SQPicture w288x256;
//SQPicture w288x128;

SQPicture LoPanel;
RLCTable TPassive;
RLCTable TActive;
RLCTable TDisable;
RLCFont FPassive;
RLCFont FActive;
RLCFont FDisable;
extern bool CheapMode;
int PanelLx;
int PanelLy;
bool MultiplayerStart;
bool GameNeedToDraw;
extern bool ChoosePosition;
extern word NPlayers;
void CloseMPL();
void IBEnumeratePlayers(InputBox** IB);
void LBEnumerateSessions(ListBox* LB,int);
void AnalyseMessages();
bool CreateNamedSession(char* Name,DWORD User2,int Max);
bool InGame;
bool InEditor;
bool JoinNameToSession(int ns,char* Name);
void FreeDDObjects( void );
bool StartIGame(bool);
extern bool GameInProgress;
byte MyRace;
extern LPDIRECTPLAY3A		lpDirectPlay3A;
extern word PlayerMenuMode;
extern LPVOID lplpConnectionBuffer[16];
extern bool SHOWSLIDE;
void PlayVideo();
void ProcessNetCash();
void ProcessReceive();
int NPROCM=0;
int TPROCM=0;
int LastCTRLPressTime=0;
void ProcessLogin();
int PrevProgStage=0;
char* PROGSTR=NULL;
extern int CurPalette;
extern int RealLx;
extern int RealLy;
void ShowCharUNICODE(int x,int y,byte* strptr,lpRLCFont lpr);
void ShowProgStr(char* ss,int prog){
	
	if(CurPalette){
		LocalGP LOAD("Interf2\\_Loading");
		int x=(RealLx-GPS.GetGPWidth(LOAD.GPID,0))/2;
		int y=(RealLy-GPS.GetGPHeight(LOAD.GPID,0))/2;
		GPS.ShowGP(x,y,LOAD.GPID,1,0);		
	}else{
		DrawStdBar((RealLx-330)/2+20,(RealLy-70)/2+18,(RealLx-330)/2+318,(RealLy-70)/2+55);
	};
	
	int L=strlen(ss);
	L=(L*prog)/100;
	int x0=(RealLx-GetRLCStrWidth(ss,&fon30y1))/2+5;
	int y0=(RealLy-GetRLCHeight(fon30y1.RLC,'W'))/2-7;
	int spos=0;
	do{
		if(spos<=L)ShowCharUNICODE(x0,y0,(byte*)(ss+spos),&fon30y1);
		else ShowCharUNICODE(x0,y0,(byte*)(ss+spos),&fon30y3);
		int LL;
		x0+=GetRLCWidthUNICODE(fon30y3.RLC,(byte*)(ss+spos),&LL);
		spos+=LL;
	}while(ss[spos]);
	CopyToScreen(0,0,RealLx,RSCRSizeY);
};
void ProcessNewInternet();
int PrevT=0;
int COUNTER1=0;
int LastBActiveTime=0;
void IAmLeft();
extern byte SpecCmd;
void ShowOptMessage(char* ID,int v){
	char* str=GetTextByID(ID);
	if(str[0]!='#'){
		char HHH[512];
		sprintf(HHH,str,v);
		AssignHint1(HHH,200,32);
	};
};
bool NeedToPerformGSC_Report=0;
bool NeedToReportInGameStats=0;
int PrevReportTime=0;
int LastTimeReport_tmtmt=0;
extern int tmtmt;
extern City CITY[8];
void EndGSC_Reporting(){
	NeedToReportInGameStats=0;
	NeedToPerformGSC_Report=0;
	LeaveGSCRoom();
};
void ProcessExplorer(int Index);
bool PDIF_Inside=0;
extern int TIME1;
int PrevProcTime=0;
int PrevDelayTime=0;
int PrevInGameAliveTime=0;
extern DWORD MyDPID;
void ProcessMP3(bool anyway);
CIMPORT
void GetGameID(char* s);
void ExplorerOpenRef(int Index,char* ref);
bool ServerResponce=1;
CEXPORT
bool ProcessMessages(){
    if (PDIF_Inside)return false;
    int TT = GetTickCount();
    if (!PrevDelayTime)PrevDelayTime = TT;
    if (TT - PrevDelayTime > 560) {
        Sleep(2);
        PrevDelayTime = TT;
        ProcessMP3(false);
    };
    if (!PrevProcTime)PrevProcTime = TT;
    if (TT - PrevProcTime > 3000) {
        PDIF_Inside = 1;
        if (ProcessDownloadInternetFiles)ProcessDownloadInternetFiles();
        ProcessExplorer(16);
        PDIF_Inside = 0;
        PrevProcTime = TT;
    };
    if (NeedToPerformGSC_Report) {
        int T = GetTickCount();
        if (!PrevReportTime)PrevReportTime = T;
        if (T - PrevReportTime > 120000) {
            int NZeros = 0;
            int PROFS[8];
            for (int i = 0; i < NPlayers; i++) {
                if (!PINFO[i].ProfileID)NZeros++;
                PROFS[i] = PINFO[i].ProfileID;
            };
            if (!NZeros) {
                ReportAliveState(NPlayers, PROFS);
                PrevReportTime = T;
            };
        };
    };
    if (NeedToReportInGameStats && UseGSC_Login) {
        int T = GetTickCount();
        if (!PrevInGameAliveTime)PrevInGameAliveTime = T;
        if (T - PrevInGameAliveTime > 30000) {
            if (!ServerResponce) {
                AssignHint1(GetTextByID("SERVERWARNING"), 200, 32);
            };
            char GMID[64];
            GetGameID(GMID);
            if (GMID[0]) {
                char REF[128];
                sprintf(REF, "GW|gmalive&%s&%X&1", GMID, &ServerResponce);
                ExplorerOpenRef(0, REF);
                PrevInGameAliveTime = T;
                ServerResponce = 0;
            };
        };
        if (tmtmt - LastTimeReport_tmtmt >= 512) {
            int NMN[8] = { 0,0,0,0,0,0,0 };
            for (int i = 0; i < MAXOBJECT; i++) {
                OneObject* OB = Group[i];
                if (OB && (OB->Hidden || !OB->Sdoxlo) && (!OB->NewBuilding)) {
                    NMN[OB->NNUM]++;
                };
            };
            OnePlayerReport OPR[8];
            for (int i = 0; i < NPlayers; i++)if (PINFO[i].PlayerID == MyDPID) {
                int NI = PINFO[i].ColorID;
                int NP = 0;
                int NW = 0;
                int NN = NATIONS[NI].NMon;
                for (int j = 0; j < NN; j++) {
                    NewMonster* NM = NATIONS[NI].Mon[j]->newMons;
                    if (NM->Peasant)NP += NATIONS[NI].NProduced[j];
                    else if (!NM->Building)NW += NATIONS[NI].NProduced[j];
                };

                OPR[0].NBornP = NP;
                OPR[0].NBornUnits = NW;
                OPR[0].Population = NMN[PINFO[i].ColorID];
                OPR[0].Profile = PINFO[i].ProfileID;
                for (int p = 0; p < 6; p++)OPR[i].ReachRes[p] = NATIONS[NI].ResTotal[p];
                OPR[0].Score = CITY[NI].Account / 100;
                OPR[0].State = NATIONS[NI].VictState;
                for (int q = 0; q < 6; q++)OPR[0].ReachRes[q] = NATIONS[NI].ResTotal[q];
            };
            ReportGSCGame(tmtmt, NPlayers, OPR);
            LastTimeReport_tmtmt = tmtmt;
        };
    };
    int T1 = GetTickCount();
    if ((!PrevT) || T1 - PrevT > 10) {
        int TT0 = T1;
        ProcessNewInternet();
        TT0 = GetTickCount() - T1;
        if (TT0 > COUNTER)COUNTER = TT0;
        //ProcessLogin();
        //TPEN.Process();
        //NPROCM++;
        //int T0=GetTickCount();
        if (GetKeyState(VK_CONTROL) & 0x8000) {
            LastCTRLPressTime = GetTickCount();
        };
        TT0 = GetTickCount();
        ProcessReceive();
        TT0 = GetTickCount() - TT0;
        if (TT0 > COUNTER1)COUNTER1 = TT0;
        ProcessNetCash();
        PrevT = T1;
        if (bActive) {
            LastBActiveTime = T1;
        }
        else {
            if (GameInProgress && NPlayers > 1 && T1 - LastBActiveTime > 20000) {
                if (NATIONS[MyNation].VictState != 2) {
                    IAmLeft();
                    SpecCmd = 199;
                    ShowOptMessage("#ALTTABDEFEAT", 0);
                };
            };
        };
    };
    MSG         msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
        {
            //FreeDDObjects();
            //PostQuitMessage(msg.wParam);
            //TPROCM+=GetTickCount()-T0;
            return true;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    //TPROCM+=GetTickCount()-T0;
    if (GPROG.NWeights) {
        if ((!PrevProgStage) || GetTickCount() - PrevProgStage > 300) {
            PrevProgStage = GetTickCount();
            if (!PROGSTR)PROGSTR = GetTextByID("PROGSTR%s");
            char ccx[128];
            char tmp[32];
            DWORD CID = GPROG.StageID[GPROG.CurStage];
            tmp[0] = char(CID >> 24);
            tmp[1] = char((CID >> 16) & 255);
            tmp[2] = char((CID >> 8) & 255);
            tmp[3] = char((CID) & 255);
            tmp[4] = ' ';
            tmp[5] = 0;
            int cpr = GPROG.GetCurProgress();
            sprintf(tmp + 5, "%d%%", cpr);
            sprintf(ccx, PROGSTR, tmp);
            ShowProgStr(ccx,/*PROGSTR,*/cpr);
        };
    };
    return false;
};
extern bool EnterChatMode;
void ProcessChatKeys();
void ProcessVotingKeys();
bool ProcessMessagesEx(){
	if(GetKeyState(VK_CONTROL)&0x8000){
		LastCTRLPressTime=GetTickCount();
	};
	ProcessVotingKeys();
	if(EnterChatMode){
		ProcessChatKeys();
	};
	MSG         msg;
	//ProcessNetCash();
	bool MSR=0;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		if (msg.message == WM_QUIT)
		{
			//FreeDDObjects();
			//PostQuitMessage(msg.wParam);
			return true;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		MSR=1;
	}
	return MSR;
};
void normstr(char* str);
CEXPORT char PlName[64];
void ProcessHelp();
SFLB_DLLEXPORT SFLB_ReadFirstPlayerName(char* ccc);
#ifdef MAKE_PTC
SFLB_DLLEXPORT SFLB_LoadMEDButtons();
SFLB_DLLEXPORT SFLB_InitDialogs(){
	DLL1(SFLB_ReadFirstPlayerName,PlName);
	unsigned long dws=20;
	if(!PlName[0]){
		GetComputerName(PlName,&dws);
		normstr(PlName);
	};
    FPassive=YellowFont;
    FActive=WhiteFont;
    FDisable=YellowFont;
	DLL0(SFLB_LoadMEDButtons);
};
#endif
CEXPORT int ItemChoose;
CEXPORT
bool MMItemChoose(SimpleDialog* SD){
	ItemChoose=SD->UserParam;
	Lpressed=false;
	return true;
};

bool CHANGESORT(SimpleDialog* SD) {
    /*
    if (TPEN.SORTTYPE == SD->UserParam)TPEN.SORTTYPE = -TPEN.SORTTYPE;

    else TPEN.SORTTYPE = SD->UserParam;
    */
    Lpressed = 0;
    return true;
    
};

CEXPORT
void StdKeys(){
	if(KeyPressed&&(LastKey==27||LastKey==13)){
		if(LastKey==13)ItemChoose=mcmOk;
		if(LastKey==27)ItemChoose=mcmCancel;
		KeyPressed=0;
		LastKey=0;
	};
};
extern bool LB_SORT_ITEM;
void InstallMap(ListBox* LB,char* Name){
	LB_SORT_ITEM=1;
	LB->AddItem(Name,0);
	LB_SORT_ITEM=0;
};
void CreateMapsList(ListBox* LB){
	TGSCFindInfo* FD=GSFILES.gFindFirst("*.m3d");
	if(FD){
		InstallMap(LB,FD->m_FileName);
		while(GSFILES.gFindNext(FD))InstallMap(LB,FD->m_FileName);
	};
};
void InstallNewMap(ListBox* LB,WIN32_FIND_DATA* FD){
	LB->AddItem(FD->cFileName,0);
};
void CreateNewMapsList(ListBox* LB){
	TGSCFindInfo* FD=GSFILES.gFindFirst("*.m3d");
	if(FD){
		InstallMap(LB,FD->m_FileName);
		while(GSFILES.gFindNext(FD))InstallMap(LB,FD->m_FileName);
	};
};
//----------------Graphics menu----------------//
bool InitScreen();
int OldSizeX;
int OldSizeY;
int mul3(int);
extern LPDIRECTDRAW            lpDD;
#ifdef SPEEDFIX
unsigned long GetRealTime();
#else
int GetRealTime();
#endif
bool CheckMode(){
	return true;
	SQPicture Pan("pan17.bpx");
	RLCFont gf24w(GETS("@SMFONT1"));
	RLCFont gf24y(GETS("@SMFONT0"));
	int mmlx=Pan.GetLx();
	int mmly=Pan.GetLy();
	ContinueGame=false;
	DialogsSystem GMM(smapx+(nsmaplx<<4)-(mmlx>>1),smapy+(mul3(nsmaply)<<2)-(mmly>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&Pan,&Pan,&Pan);
	//TextButton* Mess=GMM.addTextButton(PIC1,288>>1,30,"Íîðìàëüíî ?",&FActive,&FPassive,&FDisable,1);
	TextButton* OkBtn=GMM.addTextButton(PIC1,GETV("VMQOKX"),GETV("VMQOKY"),GETS("@VMQOKSTR"),&gf24w,&gf24y,&gf24y,1);
	TextButton* CancelBtn=GMM.addTextButton(PIC1,GETV("VMQNOX"),GETV("VMQNOY"),GETS("@VMQNOSTR"),&gf24w,&gf24y,&gf24y,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	int time=GetRealTime();
	ItemChoose=-1;
	do{
		ProcessMessages();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1&&GetRealTime()-time<8000);
	if(ItemChoose==mcmOk)return true;
	else return false;

};
void GSSetup800();
void DrawAllScreen();
extern int COPYSizeX;
extern int RealLx;
extern int RealLy;
void SERROR(){
	PlayEffect(0,0,0);
	int time=GetRealTime();
	do{}while(GetRealTime()-time<1000);
};
void SERROR1(){
	PlayEffect(1,0,0);
	int time=GetRealTime();
	do{}while(GetRealTime()-time<1000);
};
void SERROR2(){
	PlayEffect(2,0,0);
	int time=GetRealTime();
	do{}while(GetRealTime()-time<1000);
};
extern int SCRSZY;
void ClearScreen(){
	if(RealScreenPtr)memset(RealScreenPtr,0,RSCRSizeX*SCRSZY);
};
extern byte fog[8192+1024];
#ifdef MAKE_PTC
CEXPORT
void DarkScreen(){
#ifdef _USE3D
	return;
#endif // _USE3D
	
	byte* sptr=(byte*)ScreenPtr;
	int sz=ScrWidth*RealLy;
	//for(int i=0;i<sz;i++)sptr[i]=fog[4096+1024+sptr[i]];
	int ofs=0;
	int ry=RealLy>>1;
	for(int iy=0;iy<ry;iy++){
		ofs=ScrWidth*iy*2;
		for(int ix=0;ix<RealLx;ix+=2){
			sptr[ofs]=0;
			ofs+=2;
		};
		ofs=ScrWidth*(iy*2+1)+1;
		for(int ix=0;ix<RealLx;ix+=2){
			sptr[ofs]=0;
			ofs+=2;
		};
	};
};
#endif
extern int CurPalette;
#ifdef MAKE_PTC
void ShowLoading(){
	DarkScreen();
	if(CurPalette){
		LocalGP LOAD("Interf2\\_Loading");
		int x=(RealLx-GPS.GetGPWidth(LOAD.GPID,0))/2;
		int y=(RealLy-GPS.GetGPHeight(LOAD.GPID,0))/2;
		GPS.ShowGP(x,y,LOAD.GPID,0,0);
	}else{
		DrawStdBar((RealLx-330)/2+20,(RealLy-70)/2+18,(RealLx-330)/2+318,(RealLy-70)/2+55);
	};
	char* ss=GetTextByID("LOADING");
	ShowString((RealLx-GetRLCStrWidth(ss,&fon30y3))/2+5,(RealLy-GetRLCHeight(fon30y3.RLC,'W'))/2-7,ss,&fon30y3);
	CopyToScreen(0,0,RealLx,RSCRSizeY);
};
#endif

void ResizeAndCenterWindow();

void SetGameDisplayModeAnyway(int SizeX,int SizeY){
	ClearScreen();
	RealLx=SizeX;
	RealLy=SizeY;
	//FreeDDObjects();
	InitScreen();
	GSSetup800();
	//SetCursorPos(512,300);
	//SetMPtr(512,300,0);
    DrawAllScreen();
    if (window_mode)
    {
        ResizeAndCenterWindow();
    }
	
#ifdef _USE3D

	RSCRSizeX = SizeX;
	RSCRSizeY = SizeY;
	SCRSizeX  = SizeX;
	SCRSizeY  = SizeY;
	COPYSizeX = SizeX;

	ScreenProp prop;
	prop.width		 = SizeX;
	prop.height		 = SizeY;
	prop.colorFormat = cfBackBufferCompatible;
	if (window_mode)
	{
		prop.fullScreen  = false; 
	}
	else
	{
		prop.fullScreen  = true; 
	}
	prop.wholeScreenWnd  = true; 
	extern IRenderSystem* IRS;
	IRS->SetScreenProperties( prop );
#endif // _USE3D
};
bool SetGameDisplayMode(int SizeX,int SizeY){
	ClearScreen();
	RealLx=SizeX;
	RealLy=SizeY;
	exRealLx=RealLx;
	exRealLy=RealLy;
	//FreeDDObjects();
	InitScreen();
	GSSetup800();
	DrawAllScreen();
	//SetCursorPos(512,300);
	//SetMPtr(512,300,0);
    if (window_mode)
    {
        ResizeAndCenterWindow();
    }
	/*if (CheckMode())return true;
	else{
		RealLx=OldSizeX;
		RealLy=OldSizeY;
		//FreeDDObjects();
		InitScreen();
		GSSetup800();
		DrawAllScreen();
		return false;
	};*/
	return false;
};
extern int ScrollSpeed;
#ifndef SPEEDFIX
CEXPORT
int FPSTime;
#endif

//-----------------Speed menu------------------//
void ProcessSpeed(int x,int y){
	
};
//----------------SOUND----------------

void ProcessSound(int x,int y){
	
};
//-----------------Options menu----------------//
void ProcessOptions(int x,int y){
	
};
//-----------------SINGLE PLAYER---------------//
void UnLoading();
static byte Ntn[8];
bool NatChoose(SimpleDialog* SD){
	BpxTextButton* TB=(BpxTextButton*)SD;
	char* qq=TB->Message;
	if(qq[0]=='Î')strcpy(TB->Message,"ËÞÄÈ");
	else strcpy(TB->Message,"ÎÐÊÈ");
	SD->NeedToDraw=true;
	Lpressed=true;
	return true;
};	
int coorX[8];
int coorY[8];
void setCoor(){
	for(int i=0;i<8;i++){
		coorX[i]=-1;
		coorY[i]=-1;
	};
};
void LoadAIFromDLL(byte Nat,char* Name);
void LoadNation(char* fn,byte msk,byte NIndex,byte);
extern City CITY[8];
void ProcessMissions(){
	
};
void OldprocessSinglePlayer(){
	
};
//-----------------WAITING FOR OPPONENTS(HOST)-------------------//
bool processHostConnection(){
	return 0;
};
//-----------------WAITING FOR OPPONENTS(JOIN)-------------------//
bool processJoinConnection(){
	return 0;
};
//--------------------JOIN  TO SESSION---------------------//
bool processJoinGame(){
	return false;
};
extern char ROOMNAMETOCONNECT[128];
bool JOINTOINETGAME(char* SNAME){

	return false;
};
bool ProcessOneBattle(int BtlID);
CEXPORT int menu_x_off = 0;
CEXPORT int menu_y_off = 0;
int menu_hint_x = 18;
int menu_hint_y = 701;
//--------------------HOST THE SESSION---------------------//
bool processHostGame(){
	return false;
};
bool MPL_EnterName();
extern CEXPORT char IPADDR[128];
void WaitWithMessage(char* Message);
void NoWaitWithMessage(char* Message);
#ifdef MAKE_PTC
int ProcessInternetConnection(bool Active);
bool FindSessionAndJoin(char* Name, char* Nick, bool Style, unsigned short port);
bool CreateSession(char* SessName,char* Name,DWORD User2,bool Style,int MaxPlayers);
extern int GMTYPE;
int IEMMOD=0;
bool DoNewInet=0;
//bool InternetGameLogin();
extern int GMMAXPL;
extern CEXPORT bool TOTALEXIT;
bool ProcessNewInternetLogin();
extern char** NatsIDS;

#endif
bool ProcessOneBattle(int BtlID);
int ProcessWars();

//--------------------MULTIPLAYER GAME---------------------//
extern char SaveFileName[128];
void OldprocessMultiplayer(){
	
};
//---------------------Map options-------------------------//
//int MO_LandType=0;
//int MO_StartRes=0;
//int MO_ResOnMap=0;
char MapScenaryDLL[200]="";
void ProcessMapOptions(){
	LocalGP FONT("vinBig");
	RLCFont fntb(FONT.GPID);
	fntb.SetBlackColor();
	RLCFont fntr(FONT.GPID);
	fntr.SetRedColor();
	int x0=(RealLx-64*7)>>1;
	int y0=(RealLy-64*2)>>1;
	DialogsSystem GMM(x0,y0);
	char DLLSC[200];
	strcpy(DLLSC,MapScenaryDLL);
	GMM.addTextButton(NULL,32*7,-20,GetTextByID("SCDLL"),&BigYellowFont,&BigYellowFont,&BigYellowFont,1);
	TextButton* Ok=GMM.addTextButton(NULL,32*7-100,80,"Ok",&fntr,&fntb,&fntb,1);
	Ok->UserParam=mcmOk;
	Ok->OnUserClick=&MMItemChoose;
	TextButton* Cancel=GMM.addTextButton(NULL,32*7+100,80,"Cancel",&fntr,&fntb,&fntb,1);
	Cancel->UserParam=mcmCancel;
	Cancel->OnUserClick=&MMItemChoose;
	int ibx=21;
	int iby=44;
	int iblx=400;
	int ibly=20;
	InputBox* EB=GMM.addInputBox(NULL,ibx,iby,DLLSC,100,iblx,ibly,&BlackFont,&RedFont);
	EB->Active=1;
	ItemChoose=-1;
	do{
		DrawPaperPanel(x0,y0,x0+64*7,y0+64*2);
		Xbar(x0+ibx,y0+iby,iblx+10,ibly+4,0x3C);
		GMM.MarkToDraw();
		ProcessMessages();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		strcpy(MapScenaryDLL,DLLSC);
	};
};
char DEFPLNAMES[8][64];
//--------------------RESOURCE EDITOR----------------------//
CEXPORT void MemScreenPart(int x,int y,int lx,int ly,byte* Ptr);
extern int CBB_GPFILE;
int ProcessResEdit(){
	LocalGP BTNS("Interf2\\AI_Res");

	int x0=(RealLx-532)>>1;
	int y0=(RealLy-540)>>1;
	DialogsSystem DSS(x0,y0);

	byte* Back=(byte*)malloc(RealLx*RealLy);
	MemScreenPart(0,0,RealLx,RealLy,Back);	
	
	DSS.addBPXView(NULL,-x0,-y0,RealLx,RealLy,1,1,1,Back,NULL);
	
	DSS.addGPPicture(NULL,0,0,BTNS.GPID,0);
	DSS.addGPPicture(NULL,0,0,BTNS.GPID,1);
	
	// Fonts
	RLCFont* FWin = &fon16y5;
	RLCFont* FABut = &fon18y5;
	RLCFont* FPBut = &fon18y5;
	RLCFont* FSYellow = &fon16y3;
	RLCFont* FYellow = &fon18y3;

	// Text
	DSS.addTextButton(NULL,294,34,GetTextByID("INTF_EDIT_RESOURCE_AI"),FWin,FWin,FWin,1);

	DSS.addTextButton(NULL,50,80,GetTextByID("INTF_EDIT_PLAYER"),FYellow,FYellow,FYellow,0);
	DSS.addTextButton(NULL,298,80,GetTextByID("INTF_EDIT_NAME"),FYellow,FYellow,FYellow,0);

	DSS.addTextButton(NULL,50,143,GetTextByID("INTF_EDIT_WOOD"),FSYellow,FSYellow,FSYellow,0);
	DSS.addTextButton(NULL,50,143+1*26,GetTextByID("INTF_EDIT_FOOD"),FSYellow,FSYellow,FSYellow,0);
	DSS.addTextButton(NULL,50,143+2*26,GetTextByID("INTF_EDIT_STONE"),FSYellow,FSYellow,FSYellow,0);
	DSS.addTextButton(NULL,50,143+3*26,GetTextByID("INTF_EDIT_GOLD"),FSYellow,FSYellow,FSYellow,0);
	DSS.addTextButton(NULL,50,143+4*26,GetTextByID("INTF_EDIT_IRON"),FSYellow,FSYellow,FSYellow,0);
	DSS.addTextButton(NULL,50,143+5*26,GetTextByID("INTF_EDIT_COAL"),FSYellow,FSYellow,FSYellow,0);

	DSS.addTextButton(NULL,268,147,GetTextByID("INTF_EDIT_LANDTYPE"),FSYellow,FSYellow,FSYellow,0);
	DSS.addTextButton(NULL,268,147+52,GetTextByID("INTF_EDIT_STARTRES"),FSYellow,FSYellow,FSYellow,0);
	DSS.addTextButton(NULL,268,147+2*52,GetTextByID("INTF_EDIT_MAPRES"),FSYellow,FSYellow,FSYellow,0);

	DSS.addTextButton(NULL,50,333,GetTextByID("INTF_EDIT_SCENDLL"),FYellow,FYellow,FYellow,0);
	
	
	ComboBox* PLAYER=DSS.addGP_ComboBoxDLX(NULL,126,79,157,CBB_GPFILE,0,9,9,&WhiteFont,&YellowFont,NULL);
	PLAYER->Center-=5;
	PLAYER->FontDy-=2;
	PLAYER->OneDy-=1;

	char* cc=GetTextByID(GetTextByID("$PLAYER%D"));
	for(int i=0;i<8;i++){
		char ccx[128];
		sprintf(ccx,cc,i+1);
		PLAYER->AddLine(ccx);
	};
	PLAYER->CurLine=MyNation;
	PLAYER->AddLine(GetTextByID("$PLAYERALL"));

	char ResST[22][16];
	char PLNAMES[9][64];
	for(int i=0;i<8;i++)strcpy(PLNAMES[i],DEFPLNAMES[i]);
	PLNAMES[8][0]=0;
	byte RESRID[8];
	int RESLOC[8];
	int NRes=0;
	for(int i=0;i<8;i++){
		if(RDS[i].Enabled){
			int resid=i;
			if(i==1) resid=3;
				else if(i==3) resid=1;
			RESRID[NRes]=resid;
			_itoa(RES[MyNation][resid],ResST[i],10);
			RESLOC[resid]=RES[MyNation][resid];
			NRes++;
		};
	};

	for(int i=0;i<NRes;i++){
		DSS.addInputBox(NULL,128,139+26*i,ResST[i],12,105,24,&YellowFont,&WhiteFont);
	};
	InputBox* IBN=DSS.addInputBox(NULL,336,335,PLNAMES[MyNation],32,270,24,&YellowFont,&WhiteFont);

	char DLLPT[200];
	strcpy(DLLPT,MapScenaryDLL);
	DSS.addInputBox(NULL,235,331,DLLPT,190,270,24,&YellowFont,&WhiteFont);
	RandomMapDesc RMP;
	RMP.Load("Terrain\\Res.dat");
	ComboBox* CBSTYLE=DSS.addGP_ComboBoxDLX(NULL,267,167,255,CBB_GPFILE,0,9,0,&WhiteFont,&YellowFont,NULL);
	CBSTYLE->Center-=5;
	CBSTYLE->FontDy-=2;
	CBSTYLE->OneDy-=1;
	for(int p=0;p<RMP.NSTY;p++){
		CBSTYLE->AddLine(RMP.STY[p].Name);
	};
	CBSTYLE->CurLine=RM_LandType;
	ComboBox* CBRESST=DSS.addGP_ComboBoxDLX(NULL,267,167+52,255,CBB_GPFILE,0,9,0,&WhiteFont,&YellowFont,NULL);
	CBRESST->Center-=5;
	CBRESST->FontDy-=2;
	CBRESST->OneDy-=1;
	for(int p=0;p<RMP.NRES;p++){
		CBRESST->AddLine(RMP.RES[p].Name);
	};
	CBRESST->CurLine=RM_Resstart;
	ComboBox* CBRESTOT=DSS.addGP_ComboBoxDLX(NULL,267,167+52*2,255,CBB_GPFILE,0,9,0,&WhiteFont,&YellowFont,NULL);
	CBRESTOT->Center-=5;
	CBRESTOT->FontDy-=2;
	CBRESTOT->OneDy-=1;
	for(int p=0;p<RMP.NMINES;p++){
		CBRESTOT->AddLine(RMP.MINES[p].Name);
	};
	CBRESTOT->CurLine=RM_Restot;
	ItemChoose=-1;

	GP_TextButton* OkBtn=DSS.addGP_TextButton(NULL,69,373,GetTextByID("INTF_GENERATE"),BTNS.GPID,5,FABut,FPBut);
	OkBtn->OnUserClick=&MMItemChoose;
	OkBtn->UserParam=mcmOk;
	GP_TextButton* CancelBtn=DSS.addGP_TextButton(NULL,301,373,GetTextByID("INTF_CANCEL"),BTNS.GPID,3,FABut,FPBut);
	CancelBtn->OnUserClick=&MMItemChoose;
	CancelBtn->UserParam=mcmCancel;

	OkBtn->FontDy += 4;
	CancelBtn->FontDy += 4;
	OkBtn->FontDx += 8;
	CancelBtn->FontDx += 8;

	int prevNat=MyNation;
	do{
		if(PLAYER->CurLine!=prevNat){
			prevNat=PLAYER->CurLine;
			if(prevNat<8){
				for(int i=0;i<NRes;i++){
					RESLOC[RESRID[i]]=RES[prevNat][RESRID[i]];
					sprintf(ResST[i],"%d",RESLOC[RESRID[i]]);
				};
			};
			IBN->Str=PLNAMES[prevNat];
		};
		for(int i=0;i<NRes;i++){
			RESLOC[RESRID[i]]=atoi(ResST[i]);
		};
		ProcessMessages();
		DSS.ProcessDialogs();
		DSS.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		if(PLAYER->CurLine==8){
			for(int i=0;i<8;i++){
				for(int j=0;j<NRes;j++){
					RES[i][RESRID[j]]=RESLOC[RESRID[j]];
				};
			};
		}else{
			for(int i=0;i<NRes;i++){
				RES[prevNat][RESRID[i]]=RESLOC[RESRID[i]];
			};
		};
		if(ItemChoose==mcmOk){
			RM_LandType=CBSTYLE->CurLine;
			RM_Resstart=CBRESST->CurLine;
			RM_Restot=CBRESTOT->CurLine;
		};
		strcpy(MapScenaryDLL,DLLPT);
	};
	free(Back);
	return ItemChoose;
};
//---------------------------------------------------------//
int TM_Angle0=0;
int TM_Angle1=89;
int TM_Tan0;
int TM_Tan1;
int TM_Height0=0;
int TM_Height1=1024;
bool TexMapMod=false;


//---------------------------------------------------------//
void ProcessFranceMission();
bool SingleOptions();

CEXPORT int NameChoose=-1;
CEXPORT
bool MMChooseName(SimpleDialog* SD){
	NameChoose=SD->UserParam;
	Lpressed=false;
	return false;
};
void __stdcall CDGINIT_INIT2();
void __stdcall CDGINIT_INIT1();
#ifdef MAKE_PTC
SFLB_DLLEXPORT SFLB_ReadFirstPlayerName(char* ccc){
	CDGINIT_INIT1();
	CDGINIT_INIT2();
	GFILE* F=Gopen("players.txt","r");
	int N=0;
	ccc[0]=0;
	if(F){
		ccc[0]=0;
		int cc=0;
		int nn=0;
		while(!(cc==0x0A||cc==EOF)){
			cc=Ggetch(F);
			if(!(cc==0x0A||cc==EOF)){
				ccc[nn]=cc;
				nn++;
			}else{
				//Ggetch(f);
			};
		};
		ccc[nn]=0;
		Gclose(F);
	};
};
#endif
bool ENRETVAL=0;
#ifdef MAKE_PTC

void EnterName(char* BackGround){
	ENRETVAL=EnterName();
};

#endif
bool MPL_EnterName(){
	EnterName("Interface\\Multi_opt\\Background_Multi_Options.bmp");
	return ENRETVAL;
};
bool Camp_EnterName(){
	EnterName("Interface\\Campagins\\Background_Campagin.bmp");
	return ENRETVAL;
};
bool CommEnterName(){
	EnterName("Interface\\MainMenu\\Background_main_opt.bmp");
	return ENRETVAL;
};
CEXPORT char IPADDR[128]="";
CEXPORT bool TOTALEXIT=0;

extern DPID MyDPID;
bool PIEnumeratePlayers(PlayerInfo* PIN,bool DoMsg);
PlayerInfo PINFO[8];
char Names[32][8];
int tmm1,tmm2,tmm3;
extern int COUN;
extern char** NatNames;
extern char** NatScripts;
extern int NNations;
char* Unknm="?";
char* GetPName(int i){
	for(int j=0;j<8;j++)if(PINFO[j].ColorID==i)return PINFO[j].name;
	return Unknm;
};
bool ColorMouseOver(SimpleDialog* SD){
	if(!SD->Enabled)return false; 
	GPPicture* CB=(GPPicture*)SD;
	if(Lpressed){
		CB->Nation++;
		if(CB->Nation>6)CB->Nation=0;
		Lpressed=false;
	};
	if(Rpressed){
		CB->Nation--;
		if(CB->Nation<0)CB->Nation=6;
		Rpressed=false;
	};
	return true;
};
bool ChangeAlias(SimpleDialog* SD){
	if((!SD->Enabled)||(!SD->Visible))return false;
	GPPicture* GPP=(GPPicture*)SD;
	if(Lpressed){
		if(GPP->Nation<7)GPP->Nation++;
		else GPP->Nation=0;
		Lpressed=0;
	};
	if(Rpressed){
		if(GPP->Nation>0)GPP->Nation--;
		else GPP->Nation=7;
		Rpressed=0;

	};
	return true;
};
extern int tmtmt;
bool GetPreview(char* Name,byte* Data);
void Decode(byte* data,byte* xlt,int Size){
	for(int i=0;i<Size;i++)data[i]=xlt[data[i]];
};
char* UNKN="?UNKNOWN?";
char* GetPlNameByID(DWORD ID){
	for(int i=0;i<NPlayers;i++){
		if(PINFO[i].PlayerID==ID){
			return PINFO[i].name;
		};
	};
	return UNKN;
};
void ControlLingvo(char* str){
	char* c1=strstr(str,"fuck");
	if(c1){
		c1[0]='?';
		c1[1]='?';
		c1[2]='?';
		c1[3]='?';
	};
	c1=strstr(str,"FUCK");
	if(c1){
		c1[0]='?';
		c1[1]='?';
		c1[2]='?';
		c1[3]='?';
	};
};
void StopConnectionToSession(LPDIRECTPLAY3A lpDirectPlay3A);
extern int GLOBALTIME;
extern int PGLOBALTIME;
extern int CurrentStartTime[8];
extern int NextStartTime[8];
CEXPORT
void SendPings();
bool CheckPingsReady();

void ClearCTime(){
	memset(CurrentStartTime,0xFF,8*4);
	memset(NextStartTime,0xFF,8*4);
};
int LoadGP=0;
void ShowCentralMessage(char* Message,int GPIDX){
	char cc[256];
	strcpy(cc,GPS.GPName(GPIDX));
	_strupr(cc);
	if(!strcmp(cc,"INTERFACE\\BOR2")){
		if(!LoadGP){
			LoadGP=GPS.PreLoadGPImage("Interface\\_Loading");
			if(LoadGP>0)GPS.LoadGP(LoadGP);
		};
		if(LoadGP>0){
			int L=GPS.GetGPWidth(LoadGP,0);
			int DX=0;
			GPS.ShowGP((RealLx-L)/2,(RealLy-80)/2,LoadGP,0,0);
			L=GetRLCStrWidth(Message,&WhiteFont);
			ShowString((RealLx-L)/2+1-DX,(RealLy-GetRLCHeight(YellowFont.RLC,'W'))/2+1,Message,&BlackFont);
			ShowString((RealLx-L)/2-DX,(RealLy-GetRLCHeight(YellowFont.RLC,'W'))/2,Message,&YellowFont);
			return;
		};
	};
	int L=GetRLCStrWidth(Message,&WhiteFont);
	int DX=0;
	DrawStdBar2((RealLx-L-64)/2-DX,(RealLy-70)/2+18,(RealLx+L+64)/2-DX,(RealLy-70)/2+55,GPIDX);
	ShowString((RealLx-L)/2+1-DX,(RealLy-GetRLCHeight(YellowFont.RLC,'W'))/2+1,Message,&BlackFont);
	ShowString((RealLx-L)/2-DX,(RealLy-GetRLCHeight(YellowFont.RLC,'W'))/2,Message,&YellowFont);
};
CEXPORT
void ShowClanString(int x,int y,char* s,byte State,RLCFont* Fn,RLCFont* Fn1,int DY);
void xLine(int x,int y,int x1,int y1,byte c);

int SearchPlayer(char* Nick) {
    /*
    for (int i = 0; i < TPEN.NPlayers; i++) {

        if (!strcmp(Nick, TPEN.Players[i].Name))return i;

    };

    for (i = 0; i < TPEN.ABPL.NPlayers; i++) {

        if (!strcmp(Nick, TPEN.ABPL.Names[i]))return i + TPEN.NPlayers;

    };
    */
    return -1;

};

void Draw_PLISTA(int x, int y, int Lx, int Ly, int Index, byte Active, int param) {
    /*
    if (Index < TPEN.NPlayers) {

        int GPF = param;

        RLCFont* FONT = Active == 2 ? &WhiteFont : &YellowFont;

        RLCFont* FONT1 = NULL;

        char cc3[256];

        strcpy(cc3, TPEN.Players[Index].Name);

        //char* cc=strstr(cc3,"[inside]");

        //if(cc)cc[0]=0;

        bool Really = 1;

        int DYF = 0;

        if (TPEN.Players[Index].ProfState == 15 && TPEN.Players[Index].ProfileID) {

            //strcat(cc3,"(R)");

            FONT = Active == 2 ? &SpecialWhiteFont : &SpecialYellowFont;

            FONT1 = &SpecialRedFont;

            DYF = -5;

            Really = 1;

        };

        if (TPEN.Players[Index].ProfState == 1 ||

            (TPEN.Players[Index].ProfState == 3 && TPEN.Players[Index].ProfileID)) {

            strcat(cc3, "(?)");

        };

        if (Really)ShowClanString(x + 45 - 6, y + 3, cc3, Active, FONT, FONT1, DYF);

        else {

            if (FONT1)ShowString(x + 45 - 6 + 1, y + 3 + DYF + 1, cc3, FONT1);

            ShowString(x + 45 - 6, y + 3 + DYF, cc3, FONT);

        };

        int png = TPEN.Players[Index].Ping;

        int spp = 0;

        if (png < 0)spp = 16;

        else if (png < 300)spp = 12;

        else if (png < 500)spp = 13;

        else if (png < 900)spp = 14;

        else spp = 15;

        GPS.ShowGP(x, y, GPF, spp, 0);

        if (TPEN.Players[Index].Muted) {

            xLine(x + 45 - 7, y + 12, x + 45 + GetRLCStrWidth(cc3, &YellowFont), y + 12, RedColor2);

            xLine(x + 45 - 7, y + 13, x + 45 + GetRLCStrWidth(cc3, &YellowFont), y + 13, RedColor2);

        };



    }
    else {

        Index -= TPEN.NPlayers;

        //if(Index<TPEN.NRoomPlayers){

        if (Index < TPEN.ABPL.NPlayers) {

            RLCFont* FONT = &RedFont;

            int GPF = param;

            ShowString(x + 45, y + 3, TPEN.ABPL.Names[Index], FONT);

            GPS.ShowGP(x, y, GPF, 17, 0);

        };

    };*/

};

char* BATTLTXT=NULL;
char* DEATHTXT=NULL;
CEXPORT void LimitString(char* str,lpRLCFont FONT,int L){
	int L0;
	do{
		L0=GetRLCStrWidth(str,FONT);
		if(L0>L){
			int LL=strlen(str);
			str[LL-4]='.';
			str[LL-3]='.';
			str[LL-2]='.';
			str[LL-1]=0;
		};
	}while(L0>L);
};
extern word dwVersion;
void CreateRoomsHintString(int Idx,char* hint,bool Active){
	hint[0]=0;
    /*if (Idx < TPEN.NFRooms) {

        int Index = TPEN.FiltRef[Idx];

        if (Index < 1000000) {

            if (Index >= TPEN.NRooms)return;

            GServer G = TPEN.Room[Index].G;

            strcpy(hint, TPEN.Room[Index].Name);

            char* cc = ServerGetStringValue(G, "mapname", "");

            cc = strstr(cc, "|");

            if (cc) {

                strcat(hint, ", ");

                strcat(hint, cc + 1);

                strcat(hint, "\\");

            }
            else strcat(hint, "\\");

            cc = ServerGetStringValue(G, "gamemode", "x");

            if (!strcmp(cc, "closedplaying")) {

                strcat(hint, GetTextByID("GAMINPR"));

            }
            else {

                if (strcmp(cc, "wait")) {

                    strcat(hint, GetTextByID("GAMINAR"));

                }
                else {

                    if (Active) {

                        cc = ServerGetStringValue(G, "gamever", "0");

                        char test[16];

                        sprintf(test, "%d.%d%d", dwVersion / 100, (dwVersion / 10) % 10, dwVersion % 10);

                        if (strcmp(cc, test)) {

                            sprintf(hint + strlen(hint), GetTextByID("IINVVER"), test);

                        }
                        else {

                            int max = ServerGetIntValue(G, "maxplayers", 0);

                            int np = ServerGetIntValue(G, "numplayers", 0);

                            if (np >= max) {

                                strcat(hint, GetTextByID("ROMFULL"));

                            }
                            else {

                                cc = ServerGetStringValue(G, "password", "0");

                                if (cc[0] == '1') {

                                    strcat(hint, GetTextByID("RMPASSW"));

                                }
                                else {

                                    int L = GetCEW(G, 1);

                                    if (L == 3)strcat(hint, GetTextByID("CANJOIN3")); else

                                        if (L == 2)strcat(hint, GetTextByID("CANJOIN2")); else

                                            if (L == 1)strcat(hint, GetTextByID("CANJOIN1")); else

                                                strcat(hint, GetTextByID("CANJOIN0"));

                                };

                            };

                        };

                    };

                };

            };

        };

    }
    else hint[0] = 0;
    */
};

void CreatePlayerHintString(int Index, char* hint) {
    /*
    if (Index < TPEN.NPlayers) {

        if (TPEN.Players[Index].Ping == -1) {

            sprintf(hint, GetTextByID("DBLCLICKU"),

                TPEN.Players[Index].Name);

        }
        else {

            sprintf(hint, GetTextByID("DBLCLICK"),

                TPEN.Players[Index].Name, TPEN.Players[Index].Ping);

        };

    }
    else {

        Index -= TPEN.NPlayers;

        if (Index < TPEN.ABPL.NPlayers) {

            sprintf(hint, GetTextByID("PLINGAME"), TPEN.ABPL.Names[Index]);

        };

    };*/

};

void Draw_RLIST(int x, int y, int Lx, int Ly, int Idx, byte Active, int param) {
    /*
    if (Idx < TPEN.NFRooms) {

        int Index = TPEN.FiltRef[Idx];

        if (Index < 1000000) {

            int Sign = 0;

            if (Index >= TPEN.NRooms)return;

            int GPF = param;

            RLCFont* FONT = Active == 2 ? &WhiteFont : &YellowFont;

            GServer GG = TPEN.Room[Index].G;

            char ccc3[256];

            char* ccx1 = ServerGetStringValue(GG, "password", "0");

            strcpy(ccc3, TPEN.Room[Index].Name);

            char* ccv1 = ServerGetStringValue(GG, "mapname", "0");

            ccv1 = strstr(ccv1, "|");

            if (ccv1) {

                strcat(ccc3, ", ");

                strcat(ccc3, ccv1 + 1);

            }

            if (ccx1[0] == '1')Sign = 1;//strcat(ccc3," [PASSWORD]");

            LimitString(ccc3, FONT, 310);

            ShowString(x + 45, y + 3, ccc3, FONT);

            int png = ServerGetPing(GG);

            char cc[32];

            if (png != 9999) {

                sprintf_s( cc,sizeof(cc), "%d", png);

                ShowString(486 - GetRLCStrWidth(cc, FONT) / 2, y + 3, cc, FONT);

            };

            char* ccx = ServerGetStringValue(GG, "gamever", "?");

            if (ccx) {

                ShowString(552 - GetRLCStrWidth(ccx, FONT) / 2, y + 3, ccx, FONT);

            };

            int max = ServerGetIntValue(GG, "maxplayers", 0);

            int np = ServerGetIntValue(GG, "numplayers", 0);

            sprintf_s( cc,sizeof(cc), "%d/%d", np, max);

            ShowString(422 - GetRLCStrWidth(cc, FONT) / 2, y + 3, cc, FONT);

            if (!BATTLTXT) {

                BATTLTXT = GetTextByID("BATTLTXT");

                DEATHTXT = GetTextByID("DEATHTXT");

            };

            int TYPE = GetCEW(GG, 0);

            if (TYPE) {

                ShowString(593, y + 3, BATTLTXT, FONT);

            }
            else {

                ShowString(593, y + 3, DEATHTXT, FONT);

            };

            ccx = ServerGetStringValue(GG, "gamemode", "x");

            if (!strcmp(ccx, "closedplaying")) {

                GPS.ShowGP(x, y, GPF, 17, 0);

            }
            else

                if (strcmp(ccx, "wait")) {

                    GPS.ShowGP(x, y, GPF, 60, 0);//spy icon

                }
                else if (Sign) {

                    GPS.ShowGP(x, y, GPF, 61, 0);//lock

                };

            int Lev = GetCEW(GG, 1);

            if (Lev && Lev < 4)GPS.ShowGP(731, y + 2, GPF, 61 + Lev, 0);

           

            //int png=TPEN.Players[Index].Ping;

            //int spp=0;

            //if(png<0)spp=16;

            //else if(png<300)spp=12;

            //else if(png<500)spp=13;

            //else if(png<900)spp=14;

            //else spp=15;

            //GPS.ShowGP(x,y,GPF,spp,0);

            //

        }
        else {

            Index -= 1000000;

            if (Index < TPEN.NRInGame) {

                RoomInGame* RIG = TPEN.RInGame + Index;

                int GPF = param;

                RLCFont* FONT = &RedFont;

                ShowString(x + 45, y + 3, RIG->Name, FONT);

                char cc[32];

                sprintf_s( cc,sizeof(cc), "%d", RIG->NPlayers);

                ShowString(422 - GetRLCStrWidth(cc, FONT) / 2, y + 3, cc, FONT);

                GPS.ShowGP(x, y, GPF, 17, 0);

                if (!BATTLTXT) {

                    BATTLTXT = GetTextByID("BATTLTXT");

                    DEATHTXT = GetTextByID("DEATHTXT");

                };

                if (RIG->Type) {

                    ShowString(593, y + 3, BATTLTXT, FONT);

                }
                else {

                    ShowString(593, y + 3, DEATHTXT, FONT);

                };

            };

        };

    };*/
};
char SessionName[128]="";
char SessPassword[64]="";
int GMMOD=0;
int GMLEV=0;
int GMMAXPL=7;
CEXPORT void DRAWBOX(int x,int y,int Lx,int Ly,int Idx,byte Active,int param){
	DrawStdBar2(x,y,x+Lx-1,y+Ly-1,param);
}
bool EnterPassword(){
	LocalGP BARS("Interface\\bor2");
	DarkScreen();
#ifdef SCREENFIX
	DialogsSystem DSS(menu_x_off, menu_y_off);
#else
    DialogsSystem DSS(0, 0);
#endif
	SessPassword[0]=0;
	CustomBox* CBOX=DSS.addCustomBox(RealLx/2-170,RealLy/2-44,340,80,&DRAWBOX);
	CBOX->param=BARS.GPID;
	InputBox* IB=DSS.addInputBox(NULL,RealLx/2-143,RealLy/2-2,SessPassword,40,160,20,&WhiteFont,&YellowFont);
	IB->Active=1;
	char* cc=GetTextByID("ENTERPASSWORD");
	DSS.addTextButton(NULL,RealLx/2,RealLy/2-40,cc,&BigYellowFont,&BigYellowFont,&BigYellowFont,1);
	ColoredBar* CB=DSS.addColoredBar(RealLx/2-150,RealLy/2-4,300,24,0xAD);
	CB->Style=1;
	ItemChoose=-1;
	LastKey=0;
	KeyPressed=0;
	do{
		ProcessMessages();
		StdKeys();
		DSS.MarkToDraw();
		DSS.ProcessDialogs();
		DSS.RefreshView();
	}while(ItemChoose==-1);
	return ItemChoose==mcmOk;
};
void EnterPersonalMessage(char* Nick){
	char nick[128];
	strcpy(nick,Nick);
	LocalGP BARS("Interface\\bor2");
	DarkScreen();
#ifdef SCREENFIX
	DialogsSystem DSS(menu_x_off, menu_y_off);
#else
    DialogsSystem DSS(0, 0);
#endif
	int DL=60;
	int Y0=RealLy/2-44+10+70+3;
	int X0=RealLx/2-70-DL-80;
	DSS.addGPPicture(NULL,X0,Y0,BARS.GPID,14);
	GPS.ShowGP(X0,Y0,BARS.GPID,17,0);
	GP_TextButton* OK=DSS.addGP_TextButton(NULL,X0+9,Y0+20,GetTextByID("@OK"),BARS.GPID,15,&WhiteFont,&YellowFont);
	OK->OnUserClick=&MMItemChoose;
	OK->UserParam=mcmOk;

	X0=RealLx/2+70+DL-80;
	DSS.addGPPicture(NULL,X0,Y0,BARS.GPID,14);
	GPS.ShowGP(X0,Y0,BARS.GPID,17,0);
	GP_TextButton* CANCEL=DSS.addGP_TextButton(NULL,X0+9,Y0+20,GetTextByID("@CANCEL"),BARS.GPID,15,&WhiteFont,&YellowFont);
	CANCEL->OnUserClick=&MMItemChoose;
	CANCEL->UserParam=mcmCancel;

	char MESSAGE[256]="";
	
	CustomBox* CBOX=DSS.addCustomBox(RealLx/2-170-DL,RealLy/2-44+10,340+DL*2,80-10,&DRAWBOX);
	CBOX->param=BARS.GPID;
	InputBox* IB=DSS.addInputBox(NULL,RealLx/2-143-10-DL,RealLy/2-2,MESSAGE,120,290+DL*2,20,&WhiteFont,&YellowFont);
	IB->Active=1;
	char cc2[128];
	sprintf(cc2,GetTextByID("ENTERMESS"),nick);
	DSS.addTextButton(NULL,RealLx/2,RealLy/2-40+10,cc2,&YellowFont,&YellowFont,&YellowFont,1);
	ColoredBar* CB=DSS.addColoredBar(RealLx/2-150-DL,RealLy/2-4+1,300+DL*2,24-1,135);//107
	CB->Style=1;
	

	ItemChoose=-1;
	LastKey=0;
	KeyPressed=0;
	do{
		ProcessMessages();
		StdKeys();
		DSS.MarkToDraw();
		DSS.ProcessDialogs();
		DSS.RefreshView();
	}while(ItemChoose==-1&&!GameInProgress);
    /*if (ItemChoose == mcmOk) {

        if (TPEN.Peer && TPEN.Connected) {

            if (MESSAGE[0]) {

                peerMessagePlayer(TPEN.Peer, nick, MESSAGE);

                char ccc[128];

                sprintf_s(ccc,sizeof(ccc), "%s-->%s", TPEN.MyNick, nick);

                TPEN.GlobalChat.Add(ccc, MESSAGE);

            };



        };

    };*/

	ItemChoose=-1;
};

/*bool CreateSessionDialog() {

    if (SessionName[0] == 127)strcpy(SessionName, SessionName + 5);

    LocalGP BTNS("Interface\\CreateIGame");

    DialogsSystem DSS(((1024 - 452) >> 1) - 80, (768 - 237 - 26) >> 1);

    GPS.ShowGP(DSS.BaseX, DSS.BaseY, BTNS.GPID, 1, 1);

    DSS.addGPPicture(NULL, 0, 0, BTNS.GPID, 0);

    GP_Button* OKBTN = DSS.addGP_Button(NULL, 67, 192 + 26, BTNS.GPID, 4, 5);

    OKBTN->UserParam = mcmOk;

    OKBTN->OnUserClick = &MMItemChoose;

    GP_Button* CANCELBTN = DSS.addGP_Button(NULL, 256, 192 + 26, BTNS.GPID, 2, 3);

    CANCELBTN->UserParam = mcmCancel;

    CANCELBTN->OnUserClick = &MMItemChoose;

    InputBox* SESSN = DSS.addInputBox(NULL, 191, 58, SessionName, 120, 230, 20, &YellowFont, &WhiteFont);

    SESSN->Active = 1;

    InputBox* SESSPS = DSS.addInputBox(NULL, 191, 136 + 26 + 3, SessPassword, 63, 230, 20, &YellowFont, &WhiteFont);

    GPPicture* GPNP = DSS.addGPPicture(NULL, 195 + (236 - 195) * (GMMAXPL - 2), 111 + 26, BTNS.GPID, 15);

    GP_TextButton* NPL[6];

    for (int i = 0; i < 6; i++) {

        char cc1[3] = "2";

        cc1[0] = '2' + i;

        NPL[i] = DSS.addGP_TextButton(NULL, 195 + (236 - 195) * i, 111 + 26, cc1, BTNS.GPID, 14, &WhiteFont, &YellowFont);

        NPL[i]->OnUserClick = &MMItemChoose;

        NPL[i]->UserParam = i;

    };

    ComboBox* LEV = DSS.addGP_ComboBox(NULL, 195, 85, BTNS.GPID, 6, 9, 0, &WhiteFont, &YellowFont, NULL);

    LEV->AddLine(GetTextByID("LEV_0M"));

    LEV->AddLine(GetTextByID("LEV_1M"));

    LEV->AddLine(GetTextByID("LEV_2M"));

    LEV->AddLine(GetTextByID("LEV_3M"));

    LEV->CurLine = GMLEV;



    ComboBox* GCB = DSS.addGP_ComboBox(NULL, 195, 85 + 26, BTNS.GPID, 6, 9, 0, &WhiteFont, &YellowFont, NULL);

    GCB->AddLine(GetTextByID("Deathmatch"));

    GCB->AddLine(GetTextByID("Battle"));

    GCB->AddLine(GetTextByID("Rated_Deathmatch"));

    GCB->CurLine = 2;

    ItemChoose = -1;

    do {

        ProcessMessages();

        StdKeys();

        DSS.ProcessDialogs();

        DSS.RefreshView();

        GPNP->x = DSS.BaseX + 195 + (236 - 195) * (GMMAXPL - 2);

        if (GCB->CurLine == 1) {

            for (int i = 0; i < 6; i++) {

                NPL[i]->Visible = 0;

                NPL[i]->Enabled = 0;

            };

            GPNP->Visible = 0;

        }
        else {

            for (int i = 0; i < 6; i++) {

                NPL[i]->Visible = 1;

                NPL[i]->Enabled = 1;

                

                //if(i==GMMAXPL-2){

                //    NPL[i]->ActiveFont=&BigWhiteFont;

                //    NPL[i]->PassiveFont=&BigYellowFont;

                //    NPL[i]->FontDy=-4;

                //}else{

                //    NPL[i]->ActiveFont=&WhiteFont;

                //    NPL[i]->PassiveFont=&YellowFont;

                //    NPL[i]->FontDy=0;

                //};

                

            };

            GPNP->Visible = 1;

        };

        if (ItemChoose >= 0 && ItemChoose < 7) {

            GMMAXPL = ItemChoose + 2;

            ItemChoose = -1;

        };

        for (int i = 0; i < 6; i++) {

            NPL[i]->Sprite = 14 + (i == (GMMAXPL - 2));

        };

        OKBTN->Enabled = SessionName[0] != 0;

    } while (ItemChoose == -1);

    if (GMMOD = GCB->CurLine == 2) {

        char ccc[256];

        strcpy(ccc, "0017");

        strcat(ccc, SessionName);

        strcpy(SessionName, ccc);

    };

    GMMOD = GCB->CurLine & 1;

    GMLEV = LEV->CurLine;

    return ItemChoose == mcmOk;

};*/

void NoWaitWithMessage(char* Message){
	LocalGP BOR2("Interface\\bor2");
	DarkScreen();
	ShowCentralMessage(Message,BOR2.GPID);
	FlipPages();
};
void WaitWithMessage(char* Message){
	LocalGP BOR2("Interface\\bor2");
	DarkScreen();
	ShowCentralMessage(Message,BOR2.GPID);
	FlipPages();
	int T=GetTickCount();
	KeyPressed=0;
	do{
		ProcessMessages();
	}while(GetTickCount()-T<3000&&!KeyPressed);
	KeyPressed=0;
};
char ROOMNAMETOCONNECT[128];
extern char LobbyVersion[32];
extern word dwVersion;


/*bool CheckJoin(GServer G) {

    char* ccx = ServerGetStringValue(G, "gamemode", "x");

    char* ccv = ServerGetStringValue(G, "gamever", "x");

    //sprintf(cc3,"%d.%d%d",dwVersion/100,(dwVersion/10)%10,dwVersion%10);

    return strcmp(ccx, "wait") == 0 && !strcmp(ccv, LobbyVersion);

};*/


int NCHATS=0;
int MAXCHATS=0;
char** ChatMess=NULL;
char** ChatSender=NULL;
CEXPORT void AddPrimitiveChat(char* Nick,char* str,
				  char** &ChatMess,char** &ChatSender,
				  int &NCHATS,int &MAXCHATS){
	if(NCHATS>=MAXCHATS){
		MAXCHATS+=32;
		ChatMess=(char**)realloc(ChatMess,4*MAXCHATS);
		ChatSender=(char**)realloc(ChatSender,4*MAXCHATS);
	};
	if(NCHATS>512){
		free(ChatMess[0]);
		free(ChatSender[0]);
		memcpy(ChatMess,ChatMess+1,NCHATS*4);
		memcpy(ChatSender,ChatSender+1,NCHATS*4);
		NCHATS--;
	};
	ChatMess[NCHATS]=znew(char,strlen(str)+1);
	strcpy(ChatMess[NCHATS],str);
	ChatSender[NCHATS]=znew(char,strlen(Nick)+1);
	strcpy(ChatSender[NCHATS],Nick);
	NCHATS++;
};
CEXPORT void AddChatString(char* Nick,char* str,int MaxLx,lpRLCFont FONT,
				  char** &ChatMess,char** &ChatSender,
				  int &NCHATS,int &MAXCHATS){
	bool FLine=1;
	char ACCAMULATOR[256];
	int pp=0;
	int sp=0;
	int LL=strlen(str);
	sprintf(ACCAMULATOR,"%s: ",Nick);
	int LName=GetRLCStrWidth(ACCAMULATOR,FONT);
	if(LName==32)LName++;
	ACCAMULATOR[0]=0;
	do{
		int Lx=GetRLCStrWidth(ACCAMULATOR,FONT)+LName;
		int Lx1=strlen(ACCAMULATOR);
		ACCAMULATOR[Lx1]=str[sp];
		ACCAMULATOR[Lx1+1]=0;
		sp++;
		if(Lx>MaxLx){
			if(LName!=32){
				AddPrimitiveChat(Nick,ACCAMULATOR,ChatMess,ChatSender,NCHATS,MAXCHATS);
				LName=32;
				ACCAMULATOR[0]=0;
			}else{
				AddPrimitiveChat("",ACCAMULATOR,ChatMess,ChatSender,NCHATS,MAXCHATS);
				ACCAMULATOR[0]=0;
			};
		};
	}while(sp<LL);
	if(ACCAMULATOR[0]){
		if(LName!=32){
			AddPrimitiveChat(Nick,ACCAMULATOR,ChatMess,ChatSender,NCHATS,MAXCHATS);
			LName=32;
			ACCAMULATOR[0]=0;
		}else{
			AddPrimitiveChat("",ACCAMULATOR,ChatMess,ChatSender,NCHATS,MAXCHATS);
			ACCAMULATOR[0]=0;
		};
	};
};

/*bool SENDPRIVMESS(SimpleDialog* SD) {

    ComplexBox* CB = (ComplexBox*)SD;

    if (CB->CurrentItem >= 0) {

        if (CB->CurrentItem < TPEN.NPlayers) {

            EnterPersonalMessage(TPEN.Players[CB->CurrentItem].Name);

        }
        else {

            if (CB->CurrentItem - TPEN.NPlayers < TPEN.ABPL.NPlayers) {

                EnterPersonalMessage(TPEN.ABPL.Names[CB->CurrentItem - TPEN.NPlayers]);

            };

        };

    };

    KeyPressed = 0;

    LastKey = 0;

    return true;

};*/

bool DisableCreate=0;
extern bool UnderFirewall;

extern char LobbyVersion[32];
void CheckProfileInit();
int GetGerbByScore(int Score){
	if(Score<5*6)return Score/5;
	Score-=5*6;
	if(Score<10*7)return 6+Score/10;
	Score-=10*7;
	if(Score<20*4)return 13+Score/20;
	Score-=20*4;
	if(Score<30*2)return 17+Score/30;
	Score-=30*2;
	if(Score<50*3)return 19+Score/50;
	Score-=50*3;
	if(Score<100*3)return 22+Score/100;
	Score-=100*3;
	if(Score<150*5)return 25+Score/150;
	Score-=150*5;
	int rr=30+Score/200;
	if(rr>41)rr=41;
	return rr;
};
int GetRankByScore(int Score){
	int rr=GetGerbByScore(Score);
	if(rr<5 )return 0;
	if(rr<12)return 1;
	if(rr<16)return 2;
	if(rr<18)return 3;
	if(rr<21)return 4;
	if(rr<24)return 5;
	if(rr<29)return 6;
	if(rr<34)return 7;
	return 8;
};
extern char** NatNames;


/*void GPI_callback3(GPConnection* gp, void* arg, void* par) {

    GPGetInfoResponseArg* GIRA = (GPGetInfoResponseArg*)arg;

    GPGetInfoResponseArg* GPI = (GPGetInfoResponseArg*)par;

    *GPI = *GIRA;

};

extern GPConnection* LOGIN_gp;*/

void GETCOUNTRY(char* code,char* res);

void DRAW_TOP100(int x, int y, int Lx, int Ly, int Index, byte Active, int param) {

    if (Index >= 0 && Index < 100) {

        int fdy = 3;

        RLCFont* FNT = &YellowFont;

        if (Active == 2) {

            FNT = &WhiteFont;

        };
        /*
        if (T100.CLIENT[Index].PLINF

            && T100.CLIENT[Index].PLINF->nick[0]) {

            ShowString(x + 40, y + fdy, T100.CLIENT[Index].PLINF->nick, FNT);

            char ccc[64];

            if (T100.CLIENT[Index].Score) {

                sprintf_s(ccc,sizeof(ccc), "%d", T100.CLIENT[Index].Score - 1);

            }
            else {

                strcpy(ccc, "???");

            };

            ShowString(x + 698 - GetRLCStrWidth(ccc, FNT) / 2, y + fdy, ccc, FNT);

            sprintf_s(ccc,sizeof(ccc), "%d", Index + 1);

            ShowString(x + 15 - GetRLCStrWidth(ccc, FNT) / 2, y + fdy, ccc, FNT);

            strcpy(ccc, T100.CLIENT[Index].PLINF->homepage);

            ShowString(x + 261 - GetRLCStrWidth(ccc, FNT) / 2, y + fdy, ccc, FNT);

            SYSTEMTIME BIRTH;

            SYSTEMTIME CURTIME;

            FILETIME FT_BIRTH;

            FILETIME FT_CURTIME;

            GetSystemTime(&CURTIME);

            memset(&BIRTH, 0, sizeof BIRTH);

            BIRTH.wYear = T100.CLIENT[Index].PLINF->birthyear;

            BIRTH.wMonth = T100.CLIENT[Index].PLINF->birthmonth;

            BIRTH.wDay = T100.CLIENT[Index].PLINF->birthday;

            SystemTimeToFileTime(&BIRTH, &FT_BIRTH);

            SystemTimeToFileTime(&CURTIME, &FT_CURTIME);

            LARGE_INTEGER L_BIRTH;

            LARGE_INTEGER L_CURTIME;

            memcpy(&L_BIRTH, &FT_BIRTH, 8);

            memcpy(&L_CURTIME, &FT_CURTIME, 8);

            L_CURTIME.QuadPart -= L_BIRTH.QuadPart;

            memcpy(&FT_CURTIME, &L_CURTIME, 8);

            FileTimeToSystemTime(&FT_CURTIME, &CURTIME);

            sprintf_s(ccc,sizeof(ccc), "%d", CURTIME.wYear - 1601);

            ShowString(x + 378 - GetRLCStrWidth(ccc, FNT) / 2, y + fdy, ccc, FNT);



            int NT = T100.CLIENT[Index].NBattl;

            int NV = T100.CLIENT[Index].NVict;

            int NI = T100.CLIENT[Index].NIncomp;



            sprintf_s(ccc,sizeof(ccc), "%d", NT + NI);

            ShowString(x + 459 - GetRLCStrWidth(ccc, FNT) / 2, y + fdy, ccc, FNT);

            sprintf_s(ccc,sizeof(ccc), "%d", NV);

            ShowString(x + 539 - GetRLCStrWidth(ccc, FNT) / 2, y + fdy, ccc, FNT);

            if (NT)sprintf_s(ccc,sizeof(ccc), "%d(%d%%)", NI, NI * 100 / (NT + NI));

            else sprintf_s(ccc,sizeof(ccc), "%d", NI);

            ShowString(x + 618 - GetRLCStrWidth(ccc, FNT) / 2, y + fdy, ccc, FNT);

        };
        */
    };

};

typedef byte GR_ARR[8][32]; 
int DrawGraph(Canvas* CAN,int y0,int Npt,GR_ARR Data,byte* Col,int N,word* Max,char* Text){
	byte COL=0x7C;
	byte COL1=0x88;
	int L1=16;
	int L2=102;
	int L3=L2-2;
	int Lx=391;
	CAN->AddRect(0,y0,Lx+2,L1,COL);
	CAN->AddText(10,y0+2,Text,&SmallWhiteFont);
	y0+=L1;

	CAN->AddRect(0,y0,Lx+2,L2,COL);
	CAN->AddBar(1,y0+1,Lx,L2-2,COL1);
	int MaxV=1;
	for(int i=0;i<N;i++)if(Max[i]>MaxV)MaxV=Max[i];
	int divisor=100*MaxV;
	for(int i=0;i<N;i++){
		byte C=0xD0+Col[i]*4;
		byte* data=&Data[i][0];
		int Maxi=Max[i];
		for(int j=1;j<Npt;j++){
			int x0=1+((j-1)*Lx)/Npt;
			int x1=1+(j*Lx)/Npt;
			CAN->AddLine(1+((j-1)*Lx)/Npt,
						 y0+1+L3-(int(data[j-1])*Maxi*(L3-2))/divisor,
				         1+(j*Lx)/Npt,
						 y0+1+L3-(int(data[j])*Maxi*(L3-2))/divisor,C);
		};
	};
	char cc[32];
	sprintf_s( cc,sizeof(cc),"%d",MaxV);
	CAN->AddText(10,y0+2,cc,&SmallWhiteFont);
	return L2+L1;
};
int  RESPOND=0;
//1-ok
//2-password
//3-invalid version
//4-room is full
//5-not created
//6-room is in the game
DWORD  PASSHASH=0;
word JP_ver=0;
DWORD CalcPassHash(char* pass){
	int L=strlen(pass);
	DWORD S=0;
	for(int i=0;i<L;i++)S+=pass[i];
	for(int i=0;i<L-1;i++)S+=pass[i]*pass[i+1];
	for(int i=0;i<L-2;i++)S+=pass[i]*pass[i+1]*pass[i+2];
	for(int i=0;i<L-3;i++)S+=pass[i]*pass[i+1]*pass[i+2]*pass[i+3];
	for(int i=0;i<L-4;i++)S+=pass[i]*pass[i+1]*pass[i+2]*pass[i+3]*pass[i+4];
	for(int i=0;i<L-5;i++)S+=pass[i]*pass[i+1]*pass[i+2]*pass[i+3]*pass[i+4]*pass[i+5];
	return S;
};
bool GetPLIP(char* Nick,char* IP);

//extern char RESPNICK[128];

bool TryToJoinToPlayer(char* name){
	char Name[128];
	strcpy(Name,name);
	LocalGP BOR2("Interface\\bor2");
	RESPOND=0;
	PASSHASH=0;
	JP_ver=0;
    //peerMessagePlayer(TPEN.Peer, Name, "@@@I_WANT_TO_JOIN");
	int T0=GetTickCount();
	DarkScreen();
	ShowCentralMessage(GetTextByID("ICJOIN"),BOR2.GPID);
	FlipPages();
	int TT=GetTickCount();
	do{
		if(GetTickCount()-TT>3000){
            //peerMessagePlayer(TPEN.Peer, Name, "@@@I_WANT_TO_JOIN");
			TT=GetTickCount();
		};
        //if (RESPOND && _stricmp(RESPNICK, Name))RESPOND = 0;
		ProcessMessages();
	}while(GetTickCount()-T0<20000&&RESPOND==0);
	char* ERR=NULL;
	switch(RESPOND){
	case 0:
		ERR=GetTextByID("JNPG_E3");
		break;
	case 2:
		if(EnterPassword()){
			DWORD V=CalcPassHash(SessPassword);
			int T0=GetTickCount();
			do{
				ProcessMessages();
			}while(GetTickCount()-T0<1000);
			if(V!=PASSHASH){
				ERR=GetTextByID("JNPG_E6");
				return false;
			};
		}else return false;
		break;
	case 3:
		ERR=GetTextByID("JNPG_E2");
		break;
	case 4:
		ERR=GetTextByID("JNPG_E4");
		break;
	case 5:
		ERR=GetTextByID("JNPG_E1");
		break;
	case 6:
		ERR=GetTextByID("JNPG_E5");
		break;
	};
	if(ERR){
		DarkScreen();
		ShowCentralMessage(ERR,BOR2.GPID);
		FlipPages();
		int T0=GetTickCount();
		KeyPressed=0;
		do{
			ProcessMessages();
		}while(GetTickCount()-T0<10000&&!KeyPressed);
		return false;
	};
	return true;
};
CEXPORT void RunHTTPC(){
	STARTUPINFO			StartUpInfo;
	PROCESS_INFORMATION		ProcessInformation;
	memset(&StartUpInfo,0x00,sizeof(STARTUPINFO));
	StartUpInfo.cb=sizeof(STARTUPINFO);	
	StartUpInfo.wShowWindow=SW_SHOWMINNOACTIVE;
	StartUpInfo.dwFlags=STARTF_USESHOWWINDOW;

	CreateProcess(	"httpc.exe",
				NULL,
				NULL,
				NULL,
				FALSE,
				0x00,
				NULL,
				NULL,
				&StartUpInfo,
				&ProcessInformation);
};
bool GetPLIP(char* Nick,char* IP);
bool T100_cmd_done=0;
void DecodeGS_Password(char* pass,char* result);
bool CheckPlayerToExit();
extern bool RejectThisPlayer;

void ResizeAndCenterWindow();

void SendPlayerRequest();
bool UseGSC_Login=0;
CEXPORT bool CheckGSC_Login(){
	return UseGSC_Login;
};
RoomInfo GlobalRIF;
int GetGSC_Profile(){
	return GlobalRIF.Profile;
};

int ProcessInternetConnection(bool Active){
    return Process_GSC_ChatWindow(Active, &GlobalRIF);
    if(UseGSC_Login){

		int r=Process_GSC_ChatWindow(Active,&GlobalRIF);

		return r;

	};

	if(Active){

		ItemChoose=-1;

		GameInProgress=0;

		PlayerMenuMode=1;


		//for(int i=0;i<NCHATS;i++){

		//	free(ChatMess[i]);

		//	free(ChatSender[i]);

		//};

		//if(MAXCHATS){

		//	free(ChatMess);

		//	free(ChatSender);

		//};

		//NCHATS=0;

		//MAXCHATS=0;

		//ChatMess=NULL;

		//ChatSender=NULL;

	

    };

    ClearScreen();

    LoadFog(2);

    LoadPalette("2\\agew_1.pal");

    LocalGP BTNS("Interface\\igame");

    LocalGP OK1("Interface\\anm_start_1_on");

    LocalGP OK2("Interface\\anm_start_1_off");

    LocalGP CANCEL1("Interface\\anm_cancel_on");

    LocalGP CANCEL2("Interface\\anm_cancel_off");

    LocalGP DIS("Interface\\an_start_disable");

    LocalGP SCROL("Interface\\_Slider");

    LocalGP BOR2("Interface\\bor2");

    LocalGP IBTN("Interface\\IButtons");

    LocalGP GERB("Interface\\gerb_low2");

    LocalGP TOPGP("Interface\\top100");

    LocalGP SYM3("Interface\\isymb3");

    SQPicture Back("Interface\\Backgroung_Internet_Game.bmp");

    SQPicture IPLInfoBack("Interface\\Background_Player_Info.bmp");

    SQPicture ITOPBACK("Interface\\Background_Top100.bmp");

    Back.Draw(0, 0);

    DarkScreen();

    ShowCentralMessage(GetTextByID("ICCONN"), BOR2.GPID);

    FlipPages();

    CheckProfileInit();

    /*if (!TPEN.Connected) {

        Back.Draw(0, 0);

        DarkScreen();

        ShowCentralMessage(GetTextByID("ICCONN"), BOR2.GPID);

        FlipPages();

        TPEN.Connect(PlName);

        bool NeedDraw = 1;

        do {

            ProcessMessages();

            if (bActive) {

                if (NeedDraw)FlipPages();

                NeedDraw = 0;

            }
            else NeedDraw = 1;

            if (GetTickCount() - TPEN.ConnStartTime > 60000) {

                TPEN.ErrorType = 1;

            };

        } while (!(TPEN.Connected || TPEN.ErrorType));

        if (TPEN.ErrorType) {

            Back.Draw(0, 0);

            DarkScreen();

            ShowCentralMessage(GetTextByID("ICUNCON"), BOR2.GPID);

            FlipPages();

            bool NeedDraw = 1;

            KeyPressed = 0;

            int t0 = GetTickCount();

            do {

                ProcessMessages();

                if (bActive) {

                    if (NeedDraw)FlipPages();

                    NeedDraw = 0;

                }
                else NeedDraw = 1;

            } while ((GetTickCount() - t0 < 20000) && !KeyPressed);

            KeyPressed = 0;

            peerShutdown(TPEN.Peer);

            TPEN.Peer = NULL;

            return false;

        };

        TPEN.MyIP[0] = 0;

        GetPLIP(TPEN.MyNick, TPEN.MyIP);

    };

    if (TPEN.MyRoom && Active)TPEN.LeaveMyRoom();
    */
        
#ifdef SCREENFIX
    DialogsSystem DSS(menu_x_off, menu_y_off);
#else
    DialogsSystem DSS(0, 0);
#endif



    LocalGP HFONT("rom10");

    RLCFont hfnt(HFONT.GPID);

    hfnt.SetWhiteColor();

    DSS.HintFont = &hfnt;

    if (!window_mode)
    {
        menu_hint_x = 18 + menu_x_off;
        menu_hint_y = 701 + menu_y_off;
    }
    else {
        menu_hint_x = 18;
        menu_hint_y = 701;
    }
#ifdef SCREENFIX
    DSS.HintY = menu_hint_y;

    DSS.HintX = menu_hint_x;
#else
    DSS.HintY = 18;

    DSS.HintX = 701;
#endif



    DSS.addPicture(NULL, 0, 0, &Back, &Back, &Back);

    int PLIX = 32;

    int PLIY = 123;



    Picture* IPINF = DSS.addPicture(NULL, PLIX, PLIY, &IPLInfoBack, &IPLInfoBack, &IPLInfoBack);

    IPINF->Visible = 0;

    int GRBX = 158 - 177;

    int GRBY = 123 - 133 + 8;

    //GPPicture* GERBACK=DSS.addGPPicture(NULL,PLIX+GRBX,PLIY+GRBY,GERB.GPID,0);

    //GERBACK->Visible=0;

    GPPicture* GERBVAL = DSS.addGPPicture(NULL, PLIX + GRBX, PLIY + GRBY, GERB.GPID, 1);

    GERBVAL->Visible = 0;

    //ListBox* PILB=DSS.addListBox(NULL,PLIX+327,PLIY+36,8,300,26,&YellowFont,&YellowFont,NULL);

    //PILB->Visible=0;

    Canvas* PILB = DSS.AddCanvas(PLIX + 337, PLIY + 36, 394, 215);

    PILB->BottomY = 0;



    VScrollBar* PIVS = DSS.addNewGP_VScrollBar(NULL, PLIX + 745, PLIY + 28, 226, 1, 0, SCROL.GPID, 0);

    PIVS->Visible = 0;

    PILB->VS = PIVS;

    PIVS->ScrDy = 100;

    PIVS->OnesDy = 32;

    //top100

    Picture* TOPBACK = DSS.addPicture(NULL, PLIX, PLIY, &ITOPBACK, &ITOPBACK, &ITOPBACK);

    TOPBACK->Visible = 0;

    TOPBACK->Enabled = 0;

    ComplexBox* TOPBOX = DSS.addComplexBox(PLIX + 4, PLIY + 36, 8, 26, &DRAW_TOP100, TOPGP.GPID, 0);

    TOPBOX->Visible = 0;

    TOPBOX->Enabled = 0;

    TOPBOX->N = 100;

    TOPBOX->OnUserClick = &MMItemChoose;

    TOPBOX->Hint = GetTextByID("INHIN2");

    TOPBOX->UserParam = 43;



    VScrollBar* TOPVS = DSS.addNewGP_VScrollBar(NULL, PLIX + 745, PLIY + 28, 226, 1, 0, SCROL.GPID, 0);

    TOPVS->Visible = 0;

    TOPBOX->VS = TOPVS;

    TOPVS->Visible = 0;

    TOPVS->Enabled = 0;

    TOPVS->ScrDy = 70;

    TOPVS->OnesDy = 10;



    byte BCL = 79;

    ColoredBar* TOPBTN = DSS.addViewPort2(632 - 10, 613, 25, 19, BCL);

    TOPBTN->OnUserClick = &MMItemChoose;

    TOPBTN->Hint = GetTextByID("INHIN1");

    TOPBTN->UserParam = 10;

    ColoredBar* PLIBTN = DSS.addViewPort2(658 - 10, 613, 26, 19, BCL);

    PLIBTN->OnUserClick = &MMItemChoose;

    PLIBTN->Hint = GetTextByID("INHIN2");

    PLIBTN->UserParam = 11;

    ColoredBar* LETBTN = DSS.addViewPort2(686 - 10, 613, 27, 19, BCL);

    LETBTN->OnUserClick = &MMItemChoose;

    LETBTN->Hint = GetTextByID("INHIN4");

    LETBTN->UserParam = 31;

    ColoredBar* SRVBTN = DSS.addViewPort2(607 - 10, 613, 25, 19, BCL);

    SRVBTN->OnUserClick = &MMItemChoose;

    SRVBTN->Hint = GetTextByID("INHIN3");

    SRVBTN->UserParam = 13;

    GPPicture* KICKPIC = DSS.addGPPicture(NULL, 714 - 10 + 2, 613, SYM3.GPID, 0);

    ColoredBar* KICKBTN = DSS.addViewPort2(714 - 10, 613, 25, 19, BCL);

    KICKBTN->OnUserClick = &MMItemChoose;

    char* MUTE = GetTextByID("INHIN5");

    char* NOMUTE = GetTextByID("INHIN6");

    KICKBTN->Hint = MUTE;

    KICKBTN->UserParam = 14;

    if (Active) {

        ColoredBar* JOINPL = DSS.addViewPort2(715 - 10 + 25, 613, 23, 19, BCL);

        JOINPL->OnUserClick = &MMItemChoose;

        JOINPL->Hint = GetTextByID("JOINPLGM");

        JOINPL->UserParam = 15;

    }
    else {

        DSS.addGPPicture(NULL, 715 - 10 + 25, 613, SYM3.GPID, 1);

    };

    CHATSTRING[0] = 0;

    ChatViewer* CHVIEW = DSS.addChatViewer(NULL, 42, 418 + 5, 7, 26, 462, &ChatMess, &ChatSender, &NCHATS);

    char CHATMESSAGE[256] = "";

    InputBox* CHATBOX = DSS.addInputBox(NULL, 122, 612, CHATMESSAGE, 200, 392, 20, &YellowFont, &WhiteFont);

    CHATBOX->Active = 1;

    VScrollBar* CHSCR = DSS.addNewGP_VScrollBar(NULL, 505, 411, 197, 1, 0, SCROL.GPID, 0);

    CHSCR->Visible = 0;



    KeyPressed = 0;

    VScrollBar* RMVS = DSS.addNewGP_VScrollBar(NULL, 778, 152, 193, 1, 0, SCROL.GPID, 0);

    RMVS->ScrDy = 6 * 10;

    RMVS->OnesDy = 10;

    ComplexBox* RMLIST = DSS.addComplexBox(37, 155 + 5, 7, 26, &Draw_RLIST, BTNS.GPID, 0);

    RMLIST->param = BTNS.GPID;

    RMLIST->VS = RMVS;



    VScrollBar* PLVS = DSS.addNewGP_VScrollBar(NULL, 778, 438, 197 - 26, 1, 0, SCROL.GPID, 0);

    PLVS->ScrDy = 6 * 10;

    PLVS->OnesDy = 10;

    ComplexBox* PLLIST = DSS.addComplexBox(559, 447, 6, 26, &Draw_PLISTA, BTNS.GPID, 6);

    PLLIST->param = BTNS.GPID;

    PLLIST->VS = PLVS;

    //PLLIST->OnUserClick=&MMItemChoose;

    //PLLIST->UserParam=31;

    int NL = 3;

    if (!Active)NL--;

    int LONE = 32;

    int LLX = 60;

    int LY1 = LONE * NL;

    int xc = 916;

    int yc = 380;

    //CustomBox* BARX=DSS.addCustomBox(xc-LLX,yc-LY1/2-20,LLX*2,LY1+40,&DRAWBOX);

    //BARX->param=BOR2.GPID;

    GP_Button* CREATE = NULL;

    GP_Button* JOIN = NULL;

    GP_Button* REFRESH = NULL;

    if (!Active) {

        CREATE = DSS.addGP_Button(NULL, 862, 668, IBTN.GPID, 7, 6);

        CREATE->UserParam = 78;

        CREATE->OnUserClick = &MMItemChoose;

        CREATE->Hint = GetTextByID("IG_BACK");

        REFRESH = DSS.addGP_Button(NULL, 862, 668 - 100, IBTN.GPID, 11, 10);

        REFRESH->UserParam = 77;

        REFRESH->OnUserClick = &MMItemChoose;

        REFRESH->Hint = GetTextByID("IG_REFRESH");

    }
    else {

        DSS.addGPPicture(NULL, 862, 668 - 100, IBTN.GPID, 3);

        JOIN = DSS.addGP_Button(NULL, 862, 668 - 100, IBTN.GPID, 5, 4);

        JOIN->UserParam = mcmJoin;

        JOIN->OnUserClick = &MMItemChoose;

        JOIN->Hint = GetTextByID("IG_JOIN");



        DSS.addGPPicture(NULL, 862, 668 - 200, IBTN.GPID, 0);

        CREATE = DSS.addGP_Button(NULL, 862, 668 - 200, IBTN.GPID, 2, 1);

        CREATE->UserParam = mcmOk;

        CREATE->OnUserClick = &MMItemChoose;

        CREATE->Hint = GetTextByID("IG_CREATE");



        REFRESH = DSS.addGP_Button(NULL, 862, 668 - 300, IBTN.GPID, 11, 10);

        REFRESH->UserParam = 77;

        REFRESH->OnUserClick = &MMItemChoose;

        REFRESH->Hint = GetTextByID("IG_REFRESH");

    };

    //VideoButton* OkBtn=DSS.addVideoButton(NULL,862,568,OK1.GPID,OK2.GPID);

    //OkBtn->Hint=GetTextByID("MOSTART");

    //OkBtn->UserParam=mcmOk;

    //OkBtn->OnUserClick=&MMItemChoose;

    if (Active) {

        VideoButton* CancelBtn = DSS.addVideoButton(NULL, 862, 668, CANCEL1.GPID, CANCEL2.GPID);

        CancelBtn->UserParam = mcmCancel;

        CancelBtn->OnUserClick = &MMItemChoose;

        CancelBtn->Hint = GetTextByID("IG_CANCEL");

    };



    //FILTERS

    char* ALLTX = GetTextByID("ALL");

    ComboBox* PLAYERS = DSS.addGP_ComboBox(NULL, 398, 352, BTNS.GPID, 18, 9, 0, &WhiteFont, &YellowFont, NULL);

    PLAYERS->Hint = GetTextByID("FIL_PLAYERS");

    PLAYERS->FontDx -= 4;

    PLAYERS->AddLine(ALLTX);

    PLAYERS->AddLine("1");

    PLAYERS->AddLine("2");

    PLAYERS->AddLine("3");

    PLAYERS->AddLine("4");

    PLAYERS->AddLine("5");

    PLAYERS->AddLine("6");

    PLAYERS->AddLine(">1");

    PLAYERS->AddLine(">2");

    PLAYERS->AddLine(">3");

    PLAYERS->AddLine(">4");

    PLAYERS->AddLine(">5");

    PLAYERS->AddLine(">6");

    ComboBox* PING = DSS.addGP_ComboBox(NULL, 456, 352, BTNS.GPID, 26, 9, 0, &WhiteFont, &YellowFont, NULL);

    PING->Hint = GetTextByID("FIL_PING");

    PING->FontDx -= 4;

    PING->AddLine(ALLTX);

    PING->AddLine("<100");

    PING->AddLine("<200");

    PING->AddLine("<300");

    PING->AddLine("<400");

    PING->AddLine("<500");

    PING->AddLine("<600");

    PING->AddLine("<700");

    PING->AddLine("<800");

    ComboBox* VERSION = DSS.addGP_ComboBox(NULL, 524, 352, BTNS.GPID, 26, 9, 0, &WhiteFont, &YellowFont, NULL);

    VERSION->Hint = GetTextByID("FIL_VERS");

    char cc[32];

    sprintf_s( cc,sizeof(cc), "%s", LobbyVersion);

    VERSION->AddLine(ALLTX);

    VERSION->AddLine(cc);

    VERSION->FontDx -= 4;

    ComboBox* TYPE = DSS.addGP_ComboBox(NULL, 590, 352, BTNS.GPID, 34, 9, 0, &WhiteFont, &YellowFont, NULL);

    TYPE->Hint = GetTextByID("FIL_TYPE");

    TYPE->AddLine(ALLTX);

    TYPE->AddLine(GetTextByID("DEATHTXT"));

    TYPE->AddLine(GetTextByID("BATTLTXT"));

    ComboBox* LEVEL = DSS.addGP_ComboBox(NULL, 723, 352, BTNS.GPID, 18, 9, 0, &WhiteFont, &YellowFont, NULL);

    LEVEL->Hint = GetTextByID("FIL_LEVEL");

    LEVEL->FontDx -= 4;

    LEVEL->OneDx -= 4;

    LEVEL->AddLine(ALLTX);

    LEVEL->AddLine(GetTextByID("LEV_1"));

    LEVEL->AddLine(GetTextByID("LEV_2"));

    LEVEL->AddLine(GetTextByID("LEV_3"));



    char MASK[64] = "";

    GP_Button* CBI[6];

    InputBox* MSIB = DSS.addInputBox(NULL, 73, 351, MASK, 60, 300, 20, &YellowFont, &WhiteFont);

    MSIB->Hint = GetTextByID("FIL_NAME");

    int ccc = 152;

    SimpleDialog* SORT_Name = DSS.addViewPort(70, 129, 321, 19);

    SORT_Name->Hint = GetTextByID("SRT_ROOM");

    SORT_Name->UserParam = 1;

    SORT_Name->OnUserClick = &CHANGESORT;

    CBI[0] = DSS.addGP_Button(NULL, 69 + 3, 128, BTNS.GPID, 51, 50);

    SimpleDialog* SORT_Players = DSS.addViewPort(395, 128, 55, 19);

    SORT_Players->Hint = GetTextByID("SRT_PLAYERS");

    SORT_Players->UserParam = 2;

    SORT_Players->OnUserClick = &CHANGESORT;

    CBI[1] = DSS.addGP_Button(NULL, 395 + 3, 128, BTNS.GPID, 55, 54);

    SimpleDialog* SORT_Ping = DSS.addViewPort(454, 129, 63, 19);

    SORT_Ping->Hint = GetTextByID("SRT_PING");

    SORT_Ping->UserParam = 3;

    SORT_Ping->OnUserClick = &CHANGESORT;

    CBI[2] = DSS.addGP_Button(NULL, 453 + 3, 128, BTNS.GPID, 53, 52);

    SimpleDialog* SORT_Vers = DSS.addViewPort(522, 129, 62, 19);

    SORT_Vers->Hint = GetTextByID("SRT_VERS");

    SORT_Vers->UserParam = 4;

    SORT_Vers->OnUserClick = &CHANGESORT;

    CBI[3] = DSS.addGP_Button(NULL, 521 + 3, 128, BTNS.GPID, 59, 58);

    SimpleDialog* SORT_Type = DSS.addViewPort(588, 129, 200 - 61, 19);

    SORT_Type->Hint = GetTextByID("SRT_TYPE");

    SORT_Type->UserParam = 5;

    SORT_Type->OnUserClick = &CHANGESORT;

    CBI[4] = DSS.addGP_Button(NULL, 587 + 3, 128, BTNS.GPID, 57, 56);

    SimpleDialog* SORT_Level = DSS.addViewPort(722, 129, 55, 19);

    SORT_Level->Hint = GetTextByID("SRT_LEVEL");

    SORT_Level->UserParam = 6;

    SORT_Level->OnUserClick = &CHANGESORT;

    CBI[5] = DSS.addGP_Button(NULL, 723, 128, BTNS.GPID, 66, 65);

    int LOSPR[6] = { 50,54,52,58,56,65 };

    //-------//

    int NCLINES = 0;

    char* PNAME = "";

    int i;

    ItemChoose = -1;

    //TPEN.FILTER.CRC += 10;

    bool FIRST = 1;

    char ROOMLISTHINT[512] = "";

    char PLLISTHINT[512] = "";

    RMLIST->Hint = ROOMLISTHINT;

    PLLIST->Hint = PLLISTHINT;

    TextButton* TBNP = DSS.addTextButton(NULL, 785, 417, "                        ", &SmallYellowFont, &SmallYellowFont, &SmallYellowFont, 0);

    TextButton* TBNG = DSS.addTextButton(NULL, 386, 132, "                        ", &SmallYellowFont, &SmallYellowFont, &SmallYellowFont, 0);

    char* tx_NPL = GetTextByID("NPLAYMSS");

    char* tx_NRM = GetTextByID("NROOMMSS");

    int PLINFMOD = 0;

    InternetStatsChunks ISC;

    ISC.Clear();

    //GPGetInfoResponseArg GPI;

    if (Active) {

        RunHTTPC();

    };

    char CURRNICK[128];

    CURRNICK[0] = 0;

    int CurLBP = PLAYERS->CurLine;

    bool DOPASS = 0;

    if (Active && CheckLogin()) {

        SendPlayerRequest();

    };
    /*
    do {

        if (CURRNICK[0] && CurLBP == PLLIST->CurrentItem && CurLBP != -1) {

            int cp = SearchPlayer(CURRNICK);

            if (cp != -1)PLLIST->CurrentItem = cp;

        };

        if (PLLIST->CurrentItem >= 0) {

            int v = PLLIST->CurrentItem;

            if (v < TPEN.NPlayers) {

                strcpy(CURRNICK, TPEN.Players[v].Name);

                CurLBP = v;

            }
            else {

                v -= TPEN.NPlayers;

                if (v < TPEN.ABPL.NPlayers) {

                    strcpy(CURRNICK, TPEN.ABPL.Names[v]);

                    CurLBP = v + TPEN.NPlayers;

                };

            };

        };



        if (PLLIST->CurrentItem >= 0) {

            if (PLLIST->CurrentItem < TPEN.NPlayers) {

                if (TPEN.Players[PLLIST->CurrentItem].Muted) {

                    KICKBTN->Hint = NOMUTE;

                    KICKPIC->Visible = 0;

                }
                else {

                    KICKBTN->Hint = MUTE;

                    KICKPIC->Visible = 1;

                };

            }
            else KICKBTN->Hint = MUTE;

        }
        else KICKBTN->Hint = MUTE;

        if (PLINFMOD == 1 || PLINFMOD == 2) {

            for (int p = 0; p < 6; p++) {

                CBI[p]->Enabled = 0;

                CBI[p]->Visible = 0;

            };

            PLAYERS->Visible = 0;

            PING->Visible = 0;

            VERSION->Visible = 0;

            TYPE->Visible = 0;

            LEVEL->Visible = 0;



            SORT_Name->Visible = 0;

            SORT_Players->Visible = 0;

            SORT_Ping->Visible = 0;

            SORT_Vers->Visible = 0;

            SORT_Type->Visible = 0;

            SORT_Level->Visible = 0;

            MSIB->Enabled = 0;

            MSIB->Visible = 0;



            RMLIST->Visible = 0;

            RMLIST->Enabled = 0;

            RMVS->Visible = 0;

            RMVS->Enabled = 0;



            IPINF->Visible = 1;

            IPINF->Enabled = 1;

            //GERBACK->Visible=1;

            //GERBACK->Enabled=1;

            GERBVAL->Visible = 1;

            GERBVAL->Enabled = 1;

            PILB->Visible = 1;

            PILB->Enabled = 1;



            TOPBACK->Visible = 0;

            TOPBOX->Visible = 0;

            TOPVS->Visible = 0;

            TOPBACK->Enabled = 0;

            TOPBOX->Enabled = 0;

            TOPVS->Enabled = 0;

            //PIVS->Visible=PILB->NItems>PILB->ny;

            TBNG->Visible = 0;

            if (PLINFMOD == 1) {

                if (GPI.result != GP_NO_ERROR) {

                    PLINFMOD = 2;

                    int y0 = PILB->BottomY;

                    PILB->AddText(16, y0, GetTextByID("PIF_UNACC"), &RedFont);

                    PILB->BottomY += 18;

                };

                if (GPI.nick[0] && GPI.result == GP_NO_ERROR) {

                    PLINFMOD = 2;

                    byte cc = 0x7C;

                    int y0 = PILB->BottomY;

                    int tdy = 5;

                    int msx1 = 120;

                    char ccc[256];



                    PILB->AddRect(0, y0, 393, 27, cc);

                    PILB->AddLine(msx1, y0, msx1, y0 + 26, cc);

                    PILB->AddText(16, y0 + tdy, GetTextByID("PIF_ST0"), &YellowFont);

                    GETCOUNTRY(GPI.countrycode, ccc);

                    PILB->AddText(msx1 + 16, y0 + tdy, ccc, &YellowFont);

                    y0 += 26;



                    PILB->AddRect(0, y0, 393, 27, cc);

                    PILB->AddLine(msx1, y0, msx1, y0 + 26, cc);

                    PILB->AddText(16, y0 + tdy, GetTextByID("PIF_ST1"), &YellowFont);

                    sprintf_s(ccc,sizeof(ccc), "%d.%d.%d", GPI.birthday, GPI.birthmonth, GPI.birthyear);

                    PILB->AddText(msx1 + 16, y0 + tdy, ccc, &YellowFont);

                    y0 += 26;



                    PILB->AddRect(0, y0, 393, 27, cc);

                    PILB->AddLine(msx1, y0, msx1, y0 + 26, cc);

                    PILB->AddText(16, y0 + tdy, GetTextByID("PIF_ST2"), &YellowFont);

                    switch (GPI.sex) {

                    case GP_MALE:

                        PILB->AddText(msx1 + 16, y0 + tdy, GetTextByID("GEN_M"), &YellowFont);

                        break;

                    case GP_FEMALE:

                        PILB->AddText(msx1 + 16, y0 + tdy, GetTextByID("GEN_F"), &YellowFont);

                        break;

                    default:

                        PILB->AddText(msx1 + 16, y0 + tdy, GetTextByID("GEN_U"), &YellowFont);

                        break;

                    };

                    y0 += 26;



                    PILB->AddRect(0, y0, 393, 27, cc);

                    PILB->AddLine(msx1, y0, msx1, y0 + 26, cc);

                    PILB->AddText(16, y0 + tdy, GetTextByID("PIF_ST3"), &YellowFont);

                    PILB->AddText(msx1 + 16, y0 + tdy, GPI.email, &YellowFont);

                    y0 += 26;



                    if (GPI.icquin) {

                        PILB->AddRect(0, y0, 393, 27, cc);

                        PILB->AddLine(msx1, y0, msx1, y0 + 26, cc);

                        PILB->AddText(16, y0 + tdy, GetTextByID("ICQ#"), &YellowFont);

                        sprintf_s(ccc,sizeof(ccc), "%d", GPI.icquin);

                        PILB->AddText(msx1 + 16, y0 + tdy, ccc, &YellowFont);

                        y0 += 26;

                    };



                    if (GPI.homepage[0]) {

                        PILB->AddRect(0, y0, 393, 27, cc);

                        PILB->AddLine(msx1, y0, msx1, y0 + 26, cc);

                        PILB->AddText(16, y0 + tdy, GetTextByID("PIF_ST5"), &YellowFont);

                        PILB->AddText(msx1 + 16, y0 + tdy, GPI.homepage, &YellowFont);

                        y0 += 26;

                    };

                    //y0+=16;

                    PILB->BottomY = y0;

                    if (DOPASS)ISC.StartDownload(GPI.profile, 1);

                    else ISC.StartDownload(GPI.profile, 0);

                };

            };

            if (ISC.CDOWN.Started && (ISC.CDOWN.Finished || ISC.Error)) {

                if (ISC.CDOWN.CurPage == 1 && !ISC.CDOWN.Error) {

                    char res[128], rrr[128];

                    DecodeGS_Password(res, (char*)ISC.Chunks[0]->Data);

                    sprintf(rrr, "password: %s", res);

                    PILB->AddText(0, PILB->BottomY, rrr, &YellowFont);

                    PILB->BottomY += 24;

                    ISC.Clear();

                    ISC.StartDownload(GPI.profile, 0);

                };

                if (ISC.CDOWN.CurPage == 0 && !ISC.CDOWN.Error) {

                    int NREC = 0;

                    int CURSCORE = 0;

                    int NINCOMP = 0;

                    for (int i = 0; i < ISC.N; i++) {

                        if (ISC.Chunks[i]->Index == 0 || ISC.Chunks[i]->Index == 27) {

                            int ProfileID = ISC.CDOWN.CurProfile;

                            int w1 = *((DWORD*)ISC.Chunks[i]->Data);

                            int w2 = *((DWORD*)(ISC.Chunks[i]->Data + 4));

                            int w3 = *((DWORD*)(ISC.Chunks[i]->Data + 8));



                            int sco1 = w1 ^ DWORD(ProfileID);

                            int sco2 = w2 ^ 0xFB47E6C3;

                            if (sco1 == sco2) {

                                NREC = w3;

                                CURSCORE = sco1;

                            };

                        };

                        if (ISC.Chunks[i]->Index == 28) {

                            int ProfileID = ISC.CDOWN.CurProfile;

                            int w1 = *((DWORD*)ISC.Chunks[i]->Data);

                            int w2 = *((DWORD*)(ISC.Chunks[i]->Data + 4));

                            int w3 = *((DWORD*)(ISC.Chunks[i]->Data + 8));

                            int w5 = *((DWORD*)(ISC.Chunks[i]->Data + 16));

                            int w6 = *((DWORD*)(ISC.Chunks[i]->Data + 20));

                            int sco1 = w1 ^ DWORD(ProfileID);

                            int sco2 = w2 ^ 0xFB47E6C3;

                            int incomp1 = (w5 + DWORD(ProfileID)) ^ (0x6523A4 + DWORD(ProfileID));

                            int incomp2 = (w6 + DWORD(ProfileID)) ^ (0x1234E6 - DWORD(ProfileID));



                            if (sco1 == sco2 && incomp1 == incomp2) {

                                NREC = w3;

                                CURSCORE = sco1;

                                NINCOMP = incomp1;

                            };

                        };

                    };

                    ISC.Clear();

                    ISC.CDOWN.Started = 0;

                    if (NREC) {

                        ISC.StartDownload(ISC.CDOWN.CurProfile, 2 + NREC);

                    };

                    char ccc[128];

                    char cc2[36];

                    sprintf(cc2, "RS_RANK_%d", GetRankByScore(CURSCORE));

                    sprintf_s(ccc,sizeof(ccc), GetTextByID("SCORANK"), CURSCORE, GetTextByID(cc2));

                    PILB->AddText(0, PILB->BottomY, ccc, &YellowFont);

                    PILB->BottomY += 24;

                    if (NREC) {

                        sprintf_s(ccc,sizeof(ccc), GetTextByID("TX_INCOMP"), NINCOMP, NREC + NINCOMP, (NINCOMP * 100) / (NREC + NINCOMP));

                        PILB->AddText(0, PILB->BottomY, ccc, &YellowFont);

                        PILB->BottomY += 24;

                    };

                    GERBVAL->SpriteID = GetGerbByScore(CURSCORE) + 1;

                    GERBVAL->Visible = 1;

                }
                else if (ISC.CDOWN.CurPage > 1) {

                    if (!ISC.CDOWN.Error) {

                        int TextDy = 5;

                        int XB0 = 0;

                        int XB1 = 18;

                        int XB2 = XB1 + 18;

                        int XB3 = XB2 + 123;

                        int XB4 = XB3 + 64;

                        int XB5 = XB4 + 90;

                        int XB6 = XB5 + 80;

                        byte COL = 0x7C;

                        for (int j = ISC.N - 1; j >= 0; j--) {

                            char ccc[128];

                            char MapName[128];

                            char GameName[128];

                            if (ISC.Chunks[j]->Index == 81) {

                                byte* data = ISC.Chunks[j]->Data;

                                int pos = 0;

                                DWORD Date = *((DWORD*)data);

                                int Min = Date % 60;

                                Date /= 60;

                                int Hour = Date % 24;

                                Date /= 24;

                                int Day = (Date % 31) + 1;

                                Date /= 31;

                                int Month = (Date % 12) + 1;

                                Date /= 12;

                                int Year = 2000 + Date;

                                int Npl = data[4];

                                word PTime = *((word*)(data + 5));

                                int DScore = data[8];

                                int LMap = data[9];

                                memcpy(MapName, data + 10, LMap);

                                MapName[LMap] = 0;

                                int LGame = data[10 + LMap];

                                memcpy(GameName, data + 11 + LMap, LGame);

                                GameName[LGame] = 0;

                                int Y0 = PILB->BottomY;



                                RLCFont* FNT = &YellowFont;

                                PILB->AddRect(XB0, Y0, 393, 27, COL);

                                PILB->AddRect(XB0 + 1, Y0 + 1, 393 - 2, 27 - 2, COL);

                                int xx3 = 120;

                                PILB->AddLine(xx3, Y0, xx3, Y0 + 26, COL);

                                sprintf_s(ccc,sizeof(ccc), "%d.%d.%d %d:%d%d", Day, Month, Year, Hour, Min / 10, Min % 10);

                                PILB->AddCText((xx3 + XB0) >> 1, Y0 + TextDy, ccc, FNT);

                                sprintf_s(ccc,sizeof(ccc), "%s (%d min)", GameName, PTime);

                                PILB->AddText(xx3 + 16, Y0 + TextDy, ccc, FNT);

                                Y0 += 26;

                            

                                //sprintf_s(ccc,sizeof(ccc),"Map: %s",MapName);

                                //PILB->AddRect(XB0,Y0,393,27,COL);

                                //PILB->AddText(0,Y0,ccc,&YellowFont);

                                //Y0+=26;

                            



                                pos = 11 + LMap + LGame;

                                //PILB->AddItem("Players:",0);

                                int TeamID = 0;

                                byte pmask = 0;



                                byte Popul[8][32];

                                byte SCORE[8][32];

                                word MaxPopul[8];

                                word MaxScore[8];

                                byte ColorN[8];



                                for (int i = 0; i < Npl; i++) {



                                    PILB->AddRect(XB0, Y0, 393, 27, COL);

                                    PILB->AddLine(XB1, Y0, XB1, Y0 + 26, COL);

                                    PILB->AddLine(XB2, Y0, XB2, Y0 + 26, COL);

                                    PILB->AddLine(XB3, Y0, XB3, Y0 + 26, COL);

                                    PILB->AddLine(XB4, Y0, XB4, Y0 + 26, COL);

                                    PILB->AddLine(XB5, Y0, XB5, Y0 + 26, COL);



                                    int LNick = data[pos + 6 + 7];

                                    memcpy(MapName, data + pos + 7 + 7, LNick);

                                    MapName[LNick] = 0;

                                    DWORD Score = *((DWORD*)(data + pos + 2));

                                    byte Mask = data[pos + 6];

                                    if (!(Mask & pmask)) {

                                        pmask = Mask;

                                        TeamID++;

                                    };

                                    byte Color = data[pos + 7];

                                    ColorN[i] = Color;

                                    byte NationID = data[pos + 8];

                                    //sprintf_s(ccc,sizeof(ccc),"%s (%.2f), ",MapName,float(Score)/100);

                                    sprintf_s(ccc,sizeof(ccc), "%d", TeamID);

                                    int xc = (XB1 + XB2) >> 1;

                                    PILB->AddBar(xc - 4, Y0 + 13 - 4, 8, 8, 0xD0 + Color * 4);

                                    PILB->AddCText((XB0 + XB1) >> 1, Y0 + TextDy, ccc, FNT);

                                    PILB->AddText(XB2 + 5, Y0 + TextDy, MapName, FNT);

                                    sprintf_s(ccc,sizeof(ccc), "%.2f", float(Score) / 100);

                                    PILB->AddCText((XB3 + XB4) >> 1, Y0 + TextDy, ccc, FNT);

                                    if (NationID < NNations)PILB->AddCText((XB4 + XB5) >> 1, Y0 + TextDy, NatNames[NationID], FNT);

                                    ccc[0] = 0;

                                    switch (data[pos + 1]) {

                                    case 1://defeat

                                        strcat(ccc, "Defeat.");

                                        break;

                                    case 0://victory

                                        strcat(ccc, "Victory!");

                                        break;

                                    case 3://disconnectted

                                        strcat(ccc, "???");

                                        break;

                                    default:

                                        strcat(ccc, "?unknown?");

                                        break;

                                    };

                                    pos += 7 + 7 + LNick;

                                    PILB->AddCText((XB5 + XB6) >> 1, Y0 + TextDy, ccc, FNT);

                                    Y0 += 26;

                                    MaxScore[i] = word(*((DWORD*)(data + pos)));

                                    MaxPopul[i] = word(*((DWORD*)(data + pos + 2)));

                                    pos += 4;

                                    memcpy(SCORE[i], data + pos, 32);

                                    pos += 32;

                                    memcpy(Popul[i], data + pos, 32);

                                    pos += 32;

                                };



                                word CSCOR = *((word*)(data + pos));

                                char dscor = data[pos + 2];

                                //pos+3;



                                Y0 += DrawGraph(PILB, Y0, 32, SCORE, ColorN, Npl, MaxScore, GetTextByID("HDR_SCORE:"));

                                Y0 += DrawGraph(PILB, Y0, 32, Popul, ColorN, Npl, MaxPopul, GetTextByID("HDR_POPUL:"));

                                sprintf_s(ccc,sizeof(ccc), GetTextByID("HDR_DSCORE"), CSCOR, dscor);

                                PILB->AddText(0, Y0, ccc, FNT);

                                Y0 += 26;

                                Y0 += 16;

                                PILB->BottomY = Y0;

                            };

                        };

                    };

                    ISC.Clear();

                    if (ISC.CDOWN.CurPage > 2) {

                        ISC.StartDownload(ISC.CDOWN.CurProfile, ISC.CDOWN.CurPage - 1);

                    };

                };

            };

            if (ISC.Error)PLINFMOD = 0;

            if (!GERBVAL->SpriteID)GERBVAL->Visible = 0;

        }
        else if (PLINFMOD == 3) {

            TOPBACK->Visible = 1;

            TOPBOX->Visible = 1;

            TOPBACK->Enabled = 1;

            TOPBOX->Enabled = 1;

            MSIB->Enabled = 0;

            MSIB->Visible = 0;

            RMLIST->Visible = 0;

            RMLIST->Enabled = 0;



            IPINF->Visible = 0;

            IPINF->Enabled = 0;

            GERBVAL->Visible = 0;

            GERBVAL->Enabled = 0;

            PILB->Visible = 0;

            PILB->Enabled = 0;

            PIVS->Visible = 0;

            RMVS->Visible = 0;

            RMVS->Enabled = 0;

            for (int p = 0; p < 6; p++) {

                CBI[p]->Enabled = 0;

                CBI[p]->Visible = 0;

            };

            PLAYERS->Visible = 0;

            PING->Visible = 0;

            VERSION->Visible = 0;

            TYPE->Visible = 0;

            LEVEL->Visible = 0;

            TBNG->Visible = 0;



            SORT_Name->Visible = 0;

            SORT_Players->Visible = 0;

            SORT_Ping->Visible = 0;

            SORT_Vers->Visible = 0;

            SORT_Type->Visible = 0;

            SORT_Level->Visible = 0;



            TOPBACK->Visible = 1;

            TOPBACK->Enabled = 1;

            TOPBOX->Visible = 1;

            TOPBOX->Enabled = 1;

        }
        else {

            RMLIST->Visible = 1;

            RMLIST->Enabled = 1;



            MSIB->Enabled = 1;

            MSIB->Visible = 1;



            IPINF->Visible = 0;

            IPINF->Enabled = 0;

            //GERBACK->Visible=0;

            //GERBACK->Enabled=0;

            GERBVAL->Visible = 0;

            GERBVAL->Enabled = 0;

            PILB->Visible = 0;

            PILB->Enabled = 0;

            PIVS->Visible = 0;

            for (int p = 0; p < 6; p++) {

                CBI[p]->Enabled = 1;

                CBI[p]->Visible = 1;

            };

            PLAYERS->Visible = 1;

            PING->Visible = 1;

            VERSION->Visible = 1;

            TYPE->Visible = 1;

            LEVEL->Visible = 1;

            TBNG->Visible = 1;



            SORT_Name->Visible = 1;

            SORT_Players->Visible = 1;

            SORT_Ping->Visible = 1;

            SORT_Vers->Visible = 1;

            SORT_Type->Visible = 1;

            SORT_Level->Visible = 1;



            TOPBACK->Visible = 0;

            TOPBOX->Visible = 0;

            TOPVS->Visible = 0;

            TOPBACK->Enabled = 0;

            TOPBOX->Enabled = 0;

            TOPVS->Enabled = 0;

        };

        if (UnderFirewall) {

            DisableCreate = NotifyFirewallState();

            UnderFirewall = 0;

        };

        sprintf(TBNP->Message, tx_NPL, TPEN.NPlayers + TPEN.ABPL.NPlayers);

        sprintf(TBNG->Message, tx_NRM, RMLIST->N);

        TBNP->x = 785 - GetRLCStrWidth(TBNP->Message, &YellowFont);

        TBNG->x = 386 - GetRLCStrWidth(TBNG->Message, &YellowFont);

        if (!Active) {

            PIEnumeratePlayers(PINFO, false);

            SendPings();

            if (NPlayers > 7)NPlayers = 7;

        };

        for (i = 0; i < 6; i++) {

            if (abs(TPEN.SORTTYPE) == i + 1) {

                CBI[i]->PassiveFrame = LOSPR[i];

            }
            else {

                CBI[i]->PassiveFrame = 1000;

            }

            //CBI[i]->Visible=abs(TPEN.SORTTYPE)==i+1;

            //CBI[i]->Enabled=abs(TPEN.SORTTYPE)==i+1;

            //CBI[i]->Style=1;

        };

        if (RMLIST->M_OvrItem >= 0) {

            CreateRoomsHintString(RMLIST->M_OvrItem, ROOMLISTHINT, Active);

        };

        if (PLLIST->M_OvrItem >= 0) {

            CreatePlayerHintString(PLLIST->M_OvrItem, PLLISTHINT);

        };

        TPEN.FILTER.CHK_Ver = VERSION->CurLine != 0;

        TPEN.FILTER.CHK_Ping = PING->CurLine != 0;

        TPEN.FILTER.MinPing = PING->CurLine * 100;

        TPEN.FILTER.CHK_Type = TYPE->CurLine;

        TPEN.FILTER.CHK_Level = LEVEL->CurLine;

        strcpy(TPEN.FILTER.MASKSTR, MASK);

        if (PLAYERS->CurLine) {

            if (PLAYERS->CurLine < 7) {

                TPEN.FILTER.CHK_Players = 1;

                TPEN.FILTER.CHK_MinPlayers = 0;

                TPEN.FILTER.NPlayers = PLAYERS->CurLine;

            }
            else {

                TPEN.FILTER.CHK_Players = 0;

                TPEN.FILTER.CHK_MinPlayers = 1;

                TPEN.FILTER.NPlayers = PLAYERS->CurLine - 6;

            };

        }
        else {

            TPEN.FILTER.CHK_Players = 0;

            TPEN.FILTER.CHK_MinPlayers = 0;

        };

        TPEN.ProcessRoomsInGame();

        PLLIST->N = TPEN.NPlayers + TPEN.ABPL.NPlayers;//TPEN.NRoomPlayers;

        GServer CSRV = NULL;

        if (RMLIST->CurrentItem != -1 && RMLIST->CurrentItem < TPEN.NFRooms && TPEN.FiltRef[RMLIST->CurrentItem] < 1000000) {

            CSRV = TPEN.Room[TPEN.FiltRef[RMLIST->CurrentItem]].G;

        };

        TPEN.ProcessFilter();

        RMLIST->CurrentItem = -1;

        for (i = 0; i < TPEN.NFRooms; i++)if (TPEN.FiltRef[i] < 1000000)if (TPEN.Room[TPEN.FiltRef[i]].G == CSRV)

            RMLIST->CurrentItem = i;

        if (PLLIST->CurrentItem >= PLLIST->N)PLLIST->CurrentItem = PLLIST->N - 1;

        RMLIST->N = TPEN.NFRooms;//+TPEN.NRInGame;

        if (RMLIST->FirstVisItem + RMLIST->NOnScr >= RMLIST->N)

            RMLIST->FirstVisItem = RMLIST->N - RMLIST->NOnScr;

        if (RMLIST->FirstVisItem < 0)RMLIST->FirstVisItem = 0;

        if (RMLIST->CurrentItem >= RMLIST->N)RMLIST->CurrentItem = RMLIST->N - 1;

        int NCL = 7;

        bool AddChat = FIRST;

        FIRST = 0;

        if (TPEN.GlobalChat.NCStr) {

            for (int j = 0; j < TPEN.GlobalChat.NCStr; j++) {

                AddChatString(

                    TPEN.GlobalChat.CStr[j].PlName,

                    TPEN.GlobalChat.CStr[j].Message, 430,

                    &WhiteFont, ChatMess, ChatSender, NCHATS, MAXCHATS);

                AddChat = 1;

            };

            TPEN.GlobalChat.Clear();

        };

        PNAME = "";

        if (NCHATS > NCL) {

            CHSCR->SMaxPos = NCHATS - NCL;

            if (AddChat)CHSCR->SPos = CHSCR->SMaxPos;

            CHSCR->Visible = 1;

            CHVIEW->ChatDY = CHSCR->SPos;

        }
        else {

            CHSCR->Visible = 0;

            CHSCR->SPos = 0;

            CHVIEW->ChatDY = 0;

        };

        ProcessMessages();

        ISC.ProcessDownload();

        T100.Process();

        if (!TPEN.Connected) {

            DarkScreen();

            ShowCentralMessage(GetTextByID("ICCONN"), BOR2.GPID);

            FlipPages();

            TPEN.Connect(PlName);

            bool NeedDraw = 1;

            do {

                ProcessMessages();

                if (bActive) {

                    if (NeedDraw)FlipPages();

                    NeedDraw = 0;

                }
                else NeedDraw = 1;

                if (GetTickCount() - TPEN.ConnStartTime > 60000) {

                    TPEN.ErrorType = 1;

                };

            } while (!(TPEN.Connected || TPEN.ErrorType));

            if (TPEN.ErrorType) {

                Back.Draw(0, 0);

                DarkScreen();

                ShowCentralMessage(GetTextByID("ICUNCON"), BOR2.GPID);

                FlipPages();

                bool NeedDraw = 1;

                KeyPressed = 0;

                int t0 = GetTickCount();

                do {

                    ProcessMessages();

                    if (bActive) {

                        if (NeedDraw)FlipPages();

                        NeedDraw = 0;

                    }
                    else NeedDraw = 1;

                } while ((GetTickCount() - t0 < 20000) && !KeyPressed);

                KeyPressed = 0;

                peerShutdown(TPEN.Peer);

                TPEN.Peer = NULL;

                return false;

            };

            TPEN.MyIP[0] = 0;

            GetPLIP(TPEN.MyNick, TPEN.MyIP);

        };

        if (KeyPressed && LastKey == 13) {

            KeyPressed = 0;

            if (CHATMESSAGE[0]) {

                TPEN.SendGlobalChat(CHATMESSAGE);

                CHATMESSAGE[0] = 0;

            };

        };

        if (GameInProgress)return 0;

        DSS.MarkToDraw();

        DSS.ProcessDialogs();

        DSS.RefreshView();



        if (RMLIST->CurrentItem >= 0 && TPEN.FiltRef && (!DisableCreate)

            && TPEN.FiltRef[RMLIST->CurrentItem] < 1000000

            && TPEN.FiltRef[RMLIST->CurrentItem] < TPEN.NRooms

            && RMLIST->CurrentItem < TPEN.NFRooms

            && CheckJoin(TPEN.Room[TPEN.FiltRef[RMLIST->CurrentItem]].G)) {

            if (JOIN) {

                JOIN->Visible = 1;

                JOIN->Enabled = 1;

            };

        }
        else {

            if (JOIN) {

                JOIN->Visible = 0;

                JOIN->Enabled = 0;

            };

        };

        if (CREATE) {

            CREATE->Visible = !DisableCreate;

            CREATE->Enabled = !DisableCreate;

        }

        if (ItemChoose == mcmJoin) {

            if (RMLIST->CurrentItem >= 0 && TPEN.FiltRef[RMLIST->CurrentItem] < 1000000 && RMLIST->CurrentItem < TPEN.NFRooms

                && CheckJoin(TPEN.Room[TPEN.FiltRef[RMLIST->CurrentItem]].G)) {

                char* gg = ServerGetStringValue(TPEN.Room[TPEN.FiltRef[RMLIST->CurrentItem]].G, "password", "0");

                bool okk = 0;

                if (gg[0] == '1') {

                    okk = EnterPassword();

                    KeyPressed = 0;

                }
                else {

                    SessPassword[0] = 0;

                    okk = 1;

                };

                if (okk) {

                    DarkScreen();

                    ShowCentralMessage(GetTextByID("ICJOIN"), BOR2.GPID);

                    FlipPages();

                    strcpy(TPEN.HostMessage, TPEN.Room[TPEN.FiltRef[RMLIST->CurrentItem]].Name);

                    strcpy(ROOMNAMETOCONNECT, TPEN.Room[TPEN.FiltRef[RMLIST->CurrentItem]].Name);

                    TPEN.JoinRoom(TPEN.Room + TPEN.FiltRef[RMLIST->CurrentItem], SessPassword);

                    if (!TPEN.MyRoom) {

                        //Unable to connect.

                        ItemChoose = -1;

                    }
                    else {

                        if (SessPassword[0] == '1') {

                            do {

                                ProcessMessages();

                            } while (TPEN.MyRoom && !TPEN.MyRoom->RoomConnected);

                            if (TPEN.MyRoom && TPEN.MyRoom->RoomConnected) {

                                strcpy(IPADDR, ServerGetAddress(TPEN.Room[TPEN.FiltRef[RMLIST->CurrentItem]].G));

                                return 2;

                            }
                            else {

                                WaitWithMessage(GetTextByID("ICUNJ"));

                                IPADDR[0] = 0;

                            };

                        }
                        else {

                            strcpy(IPADDR, ServerGetAddress(TPEN.Room[TPEN.FiltRef[RMLIST->CurrentItem]].G));

                            return 2;

                        };

                    };

                };

            };

            ItemChoose = -1;

        }
        else

            if (ItemChoose == mcmOk) {

                if (CreateSessionDialog()) {

                    DarkScreen();

                    ShowCentralMessage(GetTextByID("ICCREATE"), BOR2.GPID);

                    FlipPages();

                    if (GMMOD & 1)TPEN.CreateStagingRoom(SessionName, SessPassword, 2, GMMOD, GMLEV);

                    else TPEN.CreateStagingRoom(SessionName, SessPassword, GMMAXPL, GMMOD, GMLEV);

                    return 1 + GMMOD * 2;

                }
                else {

                    ItemChoose = -1;

                };

            }
            else if (ItemChoose == 77) {

                ItemChoose = -1;

                TPEN.RefreshSessions();

            }
            else if (ItemChoose == 31) {

                ItemChoose = -1;

                //if(PLLIST->CurrentItem>=0&&PLLIST->CurrentItem<TPEN.NPlayers){

                //	EnterPersonalMessage(TPEN.Players[PLLIST->CurrentItem].Name);

                //};

                if (PLLIST->CurrentItem >= 0) {

                    if (PLLIST->CurrentItem < TPEN.NPlayers) {

                        EnterPersonalMessage(TPEN.Players[PLLIST->CurrentItem].Name);

                    }
                    else {

                        if (PLLIST->CurrentItem - TPEN.NPlayers < TPEN.ABPL.NPlayers) {

                            EnterPersonalMessage(TPEN.ABPL.Names[PLLIST->CurrentItem - TPEN.NPlayers]);

                        };

                    };

                };

                KeyPressed = 0;

                LastKey = 0;

            }
            else if (ItemChoose == 10) {//top100

                if (CheckLogin()) {

                    //if(!T100_cmd_done){

                    T100.Clear();

                    T100.Download();

                    T100.Started = 1;

                    //	T100_cmd_done=1;

                    //};

                    TOPVS->SPos = 0;

                    PLINFMOD = 3;

                };

                ItemChoose = -1;

            }
            else if (ItemChoose == 11) {//player info

                if (PLLIST->CurrentItem >= 0 && CheckLogin()) {

                    if (PLLIST->CurrentItem < TPEN.NPlayers) {

                        PILB->L = 0;

                        PILB->BottomY = 0;

                        if (TPEN.Players[PLLIST->CurrentItem].ProfileID) {

                            //ISC.StartDownload(TPEN.Players[PLLIST->CurrentItem].ProfileID,0);

                            ISC.CDOWN.Started = 0;

                            ISC.Error = 0;

                            ISC.Clear();

                            PLINFMOD = 1;

                            char* Name = TPEN.Players[PLLIST->CurrentItem].Name;

                            PILB->AddText(2, PILB->BottomY + 2, Name, &BigRedFont);

                            PILB->AddText(0, PILB->BottomY, Name, &BigYellowFont);

                            PILB->BottomY += 32;

                            memset(&GPI, 0, sizeof GPI);

                            GPResult Res = gpGetInfo(LOGIN_gp, TPEN.Players[PLLIST->CurrentItem].ProfileID, GP_DONT_CHECK_CACHE, GP_NON_BLOCKING, &GPI_callback3, &GPI);

                            PIVS->SPos = 0;

                        };

                    };

                };

                ItemChoose = -1;

                GERBVAL->SpriteID = 0;

            }
            else if (ItemChoose == 43) {//t100 click

                int Index = TOPBOX->CurrentItem;

                if (Index < 100 && T100.CLIENT[Index].PLINF) {

                    int prof = T100.CLIENT[Index].PLINF->profile;

                    if (prof) {

                        PILB->L = 0;

                        PILB->BottomY = 0;

                        ISC.CDOWN.Started = 0;

                        ISC.Error = 0;

                        ISC.Clear();

                        PLINFMOD = 1;

                        char* Name = T100.CLIENT[Index].PLINF->nick;

                        PILB->AddText(2, PILB->BottomY + 2, Name, &BigRedFont);

                        PILB->AddText(0, PILB->BottomY, Name, &BigYellowFont);

                        PILB->BottomY += 32;

                        memset(&GPI, 0, sizeof GPI);

                        GPResult Res = gpGetInfo(LOGIN_gp, prof, GP_DONT_CHECK_CACHE, GP_NON_BLOCKING, &GPI_callback3, &GPI);

                        PIVS->SPos = 0;

                    };

                    GERBVAL->SpriteID = 0;

                };

                ItemChoose = -1;

            }
            else if (ItemChoose == 13) {//servers list

                PLINFMOD = 0;

                ItemChoose = -1;

            }
            else if (ItemChoose == 14) {

                if (PLLIST->CurrentItem >= 0) {

                    if (PLLIST->CurrentItem < TPEN.NPlayers) {

                        TPEN.Players[PLLIST->CurrentItem].Muted = !TPEN.Players[PLLIST->CurrentItem].Muted;

                    };

                };

                ItemChoose = -1;

            }
            else if (ItemChoose == 15) {

                if (PLLIST->CurrentItem < TPEN.NPlayers) {

                    if (TryToJoinToPlayer(TPEN.Players[PLLIST->CurrentItem].Name)) {

                        return 2;

                    }
                    else ItemChoose = -1;

                };



            };

        if (RejectThisPlayer || CheckPlayerToExit())ItemChoose = mcmCancel;

    }while (ItemChoose == -1 && !GameInProgress);

    if (Active && TPEN.Connected) {

        TPEN.Disconnect();

    };*/

    return 0; 
};
CEXPORT
void SendPings();
bool CheckPingsReady();
int GetAveragePing();
extern int CurStatus;
void ReceiveAll();
void CreateDiffStr(char* str);
extern int NPings;
int GetMaxRealPing();
extern int CurrentMaxPing[8];
int PPTIME=0;
bool SendToAllPlayers(DWORD Size,LPVOID lpData);
int GetMapSUMM(char* Name);
int GetReadyPercent();
bool CheckExistConn();
void PrintBadConn(char* str);
char* _engNOCONN="No direct connection established with:";
byte MPL_NatRefTBL[8]={0,1,2,3,4,5,6,7};
void SetStdTBL(){
	for(int i=0;i<8;i++)MPL_NatRefTBL[i]=i;
};
int GetLogRank();
void SETPLAYERDATA(DWORD ID,void* Data,int size,bool);
void SETPLAYERNAME(DPNAME* lpdpName,bool);
extern DPID ServerDPID;
void ClearLPACK();
bool GetPreviewName(char* Name,char* ccc);
void DeepDeletePeer(DWORD ID);
int GetMyProfile();

int SetCurrentBattle(int BattleID,
					  SQPicture* Prev,
					  ComboBox** MNATION,
					  TextViewer* BTTXT){
	if(BattleID<0)return 0;
	if(BattleID>=WARS.NBattles)return WARS.NBattles-1;
	Prev->LoadPicture(WARS.Battles[BattleID].MiniMap);
	for(int i=0;i<2;i++){
		int cl=MNATION[i]->CurLine;
		MNATION[i]->Clear();
		MNATION[i]->AddLine(WARS.Battles[BattleID].RedUnits);
		MNATION[i]->AddLine(WARS.Battles[BattleID].BlueUnits);
		MNATION[i]->CurLine=cl;
	};
	ResFile FF1=RReset(WARS.Battles[BattleID].Brief);
	if(FF1!=INVALID_HANDLE_VALUE){
		ResFile FF2=RRewrite("miss.txt");
		if(FF2!=INVALID_HANDLE_VALUE){
			RBlockWrite(FF2,WARS.Battles[BattleID].BigHeader,strlen(WARS.Battles[BattleID].BigHeader));
			char c=0x0D;
			RBlockWrite(FF2,&c,1);
			c=0x0A;
			RBlockWrite(FF2,&c,1);
			c=0x0D;
			RBlockWrite(FF2,&c,1);
			c=0x0A;
			RBlockWrite(FF2,&c,1);
			byte* Data=znew(byte,RFileSize(FF1));
			RBlockRead(FF1,Data,RFileSize(FF1));
			RBlockWrite(FF2,Data,RFileSize(FF1));
			RClose(FF2);
		};
		RClose(FF1);
	};
	BTTXT->LoadFile("miss.txt");
	return BattleID;
};
void CreateNationalMaskForRandomMap(char*);
void CreateMaskForSaveFile(char*);
void CreateNationalMaskForMap(char*);
int DetermineNationAI(byte Nat){
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->NNUM==Nat&&OB->newMons->Peasant&&!OB->Sdoxlo){
			char* name=OB->Ref.General->MonsterID;
			for(int j=0;j<GlobalAI.NAi;j++){
				if(!strcmp(GlobalAI.Ai[j].Peasant_ID,name))return j;
			};
		};
	};
	return -1;
};
bool GetPreview(char* Name,byte* Data);
bool RunEnciclopia(SimpleDialog* SD){
	WinExec("HView.exe",SW_NORMAL);
	/*
	char cc[256];
	GetWindowsDirectory(cc,256);
	char cc1[300];
	sprintf(cc1,"%s\\command\\start.exe",cc);
	GFILE* f=Gopen("Info\\index.htm","r");
	if(f){
		Gclose(f);
		char cc2[300];
		sprintf(cc2,"%s /max Info\\index.htm",cc1);
		WinExec(cc2,SW_MINIMIZE);
		return 0;
	};
	for(int i='d';i<='z';i++){
		char cd[2]="a";
		char cc3[300];
		cd[0]=i;
		sprintf(cc3,"%s:\\GSC_Info\\index.htm",cd);
		GFILE* f=Gopen(cc3,"r");
		if(f){
			Gclose(f);
			char cc2[300];
			sprintf(cc2,"%s /max %s",cc1,cc3);
			WinExec(cc2,SW_MINIMIZE);
			return 0;
		};

	};
	*/
	return 0;
};
extern int RunMethod;

int ProcessWars();
void processBattleMultiplayer();
/*
Accept      - 153x239
Cancel      - 304x239
Low_Speed   - 224x120
High_Speed  - 397x120
Low_Scrol   - 224x151
High_Scrol  - 397x151
Low_Music   - 224x182
High_Music  - 397x182
Low_Sound   - 224x214
High_Sound  - 397x214

Ñëàéäåð:
Low - ëåâàÿ âåðõíÿÿ òî÷êà ìèíèìàëüíîãî çíà÷åíèÿ,
High - ëåâàÿ âåðõíÿÿ òî÷êà ìàêñèìàëüíî çíà÷åíèÿ.

Low_Speed   - 233x120
High_Speed  - 383x120
Low_Scrol   - 233x151
High_Scrol  - 383x151
Low_Music   - 233x182
High_Music  - 383x182
Low_Sound   - 233x214
High_Sound  - 383x214
*/
extern int exRealLx;
extern int exRealLy;
extern int exFMode;
int GetCDVolume();
void SetCDVolume(int);
int PlayMode=2;
void StopPlayCD();
void PlayRandomTrack();
extern byte TracksMask[16];
CEXPORT char RECFILE[128]="";

bool CreateMultiplaterInterface();
void ProcessMissions();
void PlayVideo();
extern char BuildVersion[32];
void SlideShow();
void ShowPreview();
extern byte PlayGameMode;
extern bool RetryVideo;
int GetRndVid(int N);
void CloseLogin();
void processMLoadGame();
bool InMainMenuLoop=0;

//--------------=< G A M E   S C R E E N >=------------//
extern int PrpX;
extern int PrpY;
extern int PrpNx;
extern int PrpNy;
extern int AblX;
extern int AblY;
extern int AblNx;
extern int AblNy;
extern int IconLx;
extern int AddIconLx;
extern int AddIconLy;
//extern int IconLx;
extern int IconLy;
#ifdef SCREENFIX
DialogsSystem GSYS(menu_x_off, menu_y_off);
#else
DialogsSystem GSYS(0, 0);
#endif
SimpleDialog* GVPort;
SimpleDialog* MiniVPort;
SimpleDialog* AblVPort;
SimpleDialog* PrpVPort;
void GSYSDRAW(){
	GSYS.RefreshView();
};
//Picture* Pan1;
//Picture* Pan2;
//Picture* Pan3;
ColoredBar* Pan4;
ColoredBar* Pan5;
bool GameExit;
int MenuType;
bool MakeMenu;

int NNames;
char* names[128];
char* fnames[128];
int   serials[128];
void ClearNames(){
	for(int i=0;i<128;i++)names[i]=NULL;
	NNames=0;
};
void FreeNames(){
	for(int i=0;i<128;i++)if(names[i]){
		free(names[i]);
		free(fnames[i]);
		names[i]=NULL;
		fnames[i]=NULL;
	};
	NNames=0;
};
void ErrorBox(char* str){
	return;
};
extern int sfVersion;
#ifdef MAKE_PTC
void InstallName(ListBox* LB,WIN32_FIND_DATA* FD,char* StartDir){
	char CCC[256];
	sprintf_s(CCC,sizeof(CCC),"%s%s",StartDir,FD->cFileName);
	ResFile ff1=RReset(CCC);
	if(ff1!=INVALID_HANDLE_VALUE){
		int sig,lap;
		RBlockRead(ff1,&sig,4);
		if(sig=='FSAK'||sig=='[CER'||sig==']CER'){
			if(sig=='[CER'||sig==']CER')sig=sfVersion;
			else{
				RBlockRead(ff1,&sig,4);
				RBlockRead(ff1,&lap,4);
			};
			if(sig==sfVersion&&NNames<512){
				//int nlen=0;
				//RBlockRead(ff1,&nlen,2);
				//nlen=0;
				//RBlockRead(ff1,&nlen,1);
				//RBlockRead(ff1,nam,nlen);
				RClose(ff1);
				LB->AddItem(CCC,NNames);
				fnames[NNames]=znew(char,strlen(CCC)+1);
				//serials[NNames]=lap;
				//strcpy(names[NNames],nam);
				strcpy(fnames[NNames],CCC);
				NNames++;
			}else RClose(ff1);
		}else RClose(ff1);
	};
};
#endif

//----------------LOAD GAME--------------//
void CmdLoadNetworkGame(byte NI,int ID,char* Name);
SFLB_DLLEXPORT SFLB_LoadGame(char* fnm,bool LoadNation);
void OldProcessGLoadGame(){
	
};
extern EXBUFFER EBufs[MaxPL];
DWORD MAPREPL[8];
bool CheckFileIdentity(char* Name){
	if(NPlayers==1)return true;
	DWORD FID[512];
	FID[0]='FIDN';
	DWORD S=GetMapSUMM(Name);
	FID[1]=S;
	if(FID[1]==0xFFFFFFFF)return false;
	memset(FID+2,0,128);
	strcpy((char*)(FID+2),Name);
	memset(MAPREPL,0,8*4);
	for(int i=0;i<NPlayers;i++)
		if(PINFO[i].PlayerID==MyDPID)MAPREPL[i]=S;
	SendToAllPlayers(128+8,FID);
	int t0=GetTickCount();
	int t1=t0;
	do{
		if(GetTickCount()-t1>500){
			t1=GetTickCount();
			SendToAllPlayers(128+8,FID);
		};
		ProcessMessages();
		int OK=1;
		for(int i=0;i<NPlayers;i++)if(EBufs[i].Enabled){
			if(MAPREPL[i]){
				if(MAPREPL[i]!=S)return false;
			}else OK=0;
		};
		if(OK)return true;
	}while(GetTickCount()-t0<10000);
	return false;
};

void ProcessFranceMission(){
	SQPicture Pan1("Interface\\Missions\\MTFrance_Back_1.bmp");
	SQPicture Pan2("Interface\\Missions\\MTFrance_Back_2.bmp");
	SlowUnLoadPalette("1\\agew_1.pal");
	Pan1.Draw(0,0);
	FlipPages();
	SlowLoadPalette("1\\agew_1.pal");
	int t0=GetRealTime();
	KeyPressed=0;
	do{
		ProcessMessages();
	}while(!(GetRealTime()-t0>15000||Lpressed||KeyPressed));
	if(LastKey==27)return;
	LastKey=0;
	SlowUnLoadPalette("1\\agew_1.pal");
	Pan2.Draw(0,0);
	FlipPages();
	SlowLoadPalette("1\\agew_1.pal");
	t0=GetRealTime();
	KeyPressed=0;
	do{
		ProcessMessages();
	}while(!(GetRealTime()-t0>15000||Lpressed||KeyPressed));
	if(LastKey==27)return;

	strcpy(CurrentMap,MISSLIST.MISS[0].MapName);
	CreateNationalMaskForMap(CurrentMap);
	CurrentCampagin=-1;
	CurrentMission=-1;
	PrepareGameMedia(0,0,1);
	InitGame();
	ItemChoose=mcmSingle;
	//InitExplosions();
	MISSLIST.CurrentMission=0;
	SCENINF.Load(MISSLIST.MISS[0].DLLPath,MISSLIST.MISS[0].Description);
};
//----------------SAVE GAME--------------//
void CmdSaveNetworkGame(byte NI,int ID,char* Name);
bool CheckName(char* str){
	for(int i=0;i<NNames;i++)if(strstr(names[i],str))return false;
	return true;
};
bool ChangeNation;
bool MultiTvar;
extern int FogMode;
bool EnterVal(int * val,char* Message){
	Lpressed=false;
	char passw[128];
	passw[0]=0;
	ItemChoose=-1;
	sprintf(passw,"%d",*val);
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(288>>1),smapy+(mul3(nsmaply)<<2)-(288>>1));
	//Picture* PIC1=DSY.addPicture(NULL,0,0,&y288x128,&y288x128,&y288x128);
	TextButton* LoadBtn=DSY.addTextButton(NULL,288>>1,5,Message,&FActive,&FPassive,&FDisable,1);
	InputBox* IB=DSY.addInputBox(NULL,30,45,passw,20,224,28,&WhiteFont,&WhiteFont);
	IB->Active=true;
	TextButton* OkBtn=DSY.addTextButton(NULL,55+164/2,90,"OK",&WhiteFont,&YellowFont,&YellowFont,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	KeyPressed=false;
	do{
		DrawStdBar(DSY.BaseX,DSY.BaseY,DSY.BaseX+288,DSY.BaseY+128);
		DSY.MarkToDraw();
		ProcessMessages();
		if(KeyPressed){
			if(LastKey==13){
				ItemChoose=mcmOk;
				KeyPressed=false;
			};
			if(LastKey==27){
				ItemChoose=mcmCancel;
				KeyPressed=false;
			};
		};
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		int z=sscanf(passw,"%d",val);
		if(z==0)return false;
		else return true;
	}else return false;
};
bool EnterStr(char * str,char* Message){
	Lpressed=false;
	char passw[128];
	passw[0]=0;
	ItemChoose=-1;
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(288>>1),smapy+(mul3(nsmaply)<<2)-(288>>1));
	//Picture* PIC1=DSY.addPicture(NULL,0,0,&y288x128,&y288x128,&y288x128);
	TextButton* LoadBtn=DSY.addTextButton(NULL,288>>1,5,Message,&YellowFont,&YellowFont,&YellowFont,1);
	InputBox* IB=DSY.addInputBox(NULL,30,45,str,30,224,28,&WhiteFont,&WhiteFont);
	IB->Active=true;
	TextButton* OkBtn=DSY.addTextButton(NULL,55+164/2,90,"OK",&WhiteFont,&YellowFont,&YellowFont,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	KeyPressed=false;
	do{
		DrawStdBar(DSY.BaseX,DSY.BaseY,DSY.BaseX+288,DSY.BaseY+128);
		DSY.MarkToDraw();
		ProcessMessages();
		if(KeyPressed){
			if(LastKey==13){
				ItemChoose=mcmOk;
				KeyPressed=false;
			};
			if(LastKey==27){
				ItemChoose=mcmCancel;
				KeyPressed=false;
			};
		};
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		return true;
	}else return false;
};
void MemScreenPart(int x,int y,int lx,int ly,byte* Ptr){
#ifndef _USE3D
	for(int yy=0;yy<ly;yy++){
		int ofs=x+(y+yy)*ScrWidth;
		memcpy(Ptr,((byte*)ScreenPtr)+ofs,lx);
		Ptr+=lx;
	};
#endif
};
int EnterHi(int * val,int Type){
	
	LocalGP LONGBOX("Interface\\LongBox0");
	Lpressed=false;
	char passw[128];
	passw[0]=0;
	ItemChoose=-1;
	sprintf(passw,"%d",*val);
	int LX=400;
	int LY=118;
	byte* PTR=znew(byte,(LX+64)*(LY+200));
	MemScreenPart(((RealLx-LX)>>1)-32,((RealLy-LY)>>1)-16,LX+64,LY+200,PTR);
	DialogsSystem DSY((RealLx-LX)>>1,(RealLy-LY)>>1);
	DSY.addBPXView(NULL,-32,-16,LX+64,LY+200,1,1,1,PTR,NULL);
	DSY.addBorder(0,0,LX,LY,0,1);
	DSY.addGPPicture(NULL,LX/4-75,LY+6,LONGBOX.GPID,8);
	DSY.addGPPicture(NULL,LX/4-75,LY+6,LONGBOX.GPID,11);
	DSY.addGPPicture(NULL,3*LX/4-75,LY+6,LONGBOX.GPID,8);
	DSY.addGPPicture(NULL,3*LX/4-75,LY+6,LONGBOX.GPID,11);
	GP_TextButton* OkBtn=DSY.addGP_TextButton(NULL,LX/4-75+23,LY+6+20,GetTextByID("@OK"),LONGBOX.GPID,9,&WhiteFont,&YellowFont);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	GP_TextButton* CancelBtn=DSY.addGP_TextButton(NULL,3*LX/4-75+23,LY+6+20,GetTextByID("@CANCEL"),LONGBOX.GPID,9,&WhiteFont,&YellowFont);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	//Picture* PIC1=DSY.addPicture(NULL,0,0,&y288x256,&y288x256,&y288x256);
	DSY.addTextButton(NULL,6,1,GetTextByID("$#HIHDR"),&YellowFont,&YellowFont,&YellowFont,0);
	TextButton* LoadBtn=DSY.addTextButton(NULL,10,87,GetTextByID("$#HEIGHT"),&YellowFont,&YellowFont,&YellowFont,0);
	InputBox* IB=DSY.addInputBox(NULL,120,80,passw,20,224,28,&WhiteFont,&WhiteFont);
	ColoredBar* CBR=DSY.addColoredBar(120,80,LX-140,23,0x4c);
	CBR->Style=1;
	DSY.addTextButton(NULL,10,51,GetTextByID("$#ACTION"),&YellowFont,&YellowFont,&YellowFont,0);
	ComboBox* CB=DSY.addGP_ComboBoxDLX(NULL,120,44,LX-140,LONGBOX.GPID,0,9,9,&WhiteFont,&YellowFont,NULL);
	CB->AddLine("Cubic line");
	CB->AddLine("Quad. line");
	CB->AddLine("Softing");
#ifdef _USE3D
	char* GetRoadsNames();
	CB->AddComplexLine(GetRoadsNames());
#else
	CB->AddLine("Road1");
	CB->AddLine("Road2");
	CB->AddLine("Road3");
	CB->CurLine=Type-1;
	IB->Active=true;
#endif
	KeyPressed=false;
	do{
		//DrawStdBar(DSY.BaseX,DSY.BaseY,DSY.BaseX+288,DSY.BaseY+256);
		DSY.MarkToDraw();
		ProcessMessages();
		if(KeyPressed){
			if(LastKey==13){
				ItemChoose=mcmOk;
				KeyPressed=false;
			};
			if(LastKey==27){
				ItemChoose=mcmCancel;
				KeyPressed=false;
			};
		};
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	free(PTR);
	if(ItemChoose==mcmOk){
		int z=sscanf(passw,"%d",val);
		if(*val<-500)*val=0;
		if(*val>500)*val=500;
		if(z==0)return false;
		else return CB->CurLine+1;
	}else return false;
};
DLLEXPORT
void ShowVictory();
DLLEXPORT
void SelChangeNation(byte SrcNat,byte DstNat);

void EnterChat(){
	if(NPlayers>1)return;
	Lpressed=false;
	char passw[128];
	passw[0]=0;
	ItemChoose=-1;
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(288>>1),smapy+(mul3(nsmaply)<<2)-(288>>1));
	//Picture* PIC1=DSY.addPicture(NULL,0,0,&y288x128,&y288x128,&y288x128);
	TextButton* LoadBtn=DSY.addTextButton(NULL,288>>1,5,"Enter password:",&FActive,&FPassive,&FDisable,1);
	InputBox* IB=DSY.addInputBox(NULL,30,45,passw,20,224,28,&WhiteFont,&WhiteFont);
	IB->Active=true;
	TextButton* OkBtn=DSY.addTextButton(NULL,55+164/2,90,"OK",&WhiteFont,&YellowFont,&YellowFont,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	KeyPressed=false;
	do{
		DrawStdBar(DSY.BaseX,DSY.BaseY,DSY.BaseX+288,DSY.BaseY+128);
		DSY.MarkToDraw();
		ProcessMessages();
		if(KeyPressed&&LastKey==13){
			ItemChoose=mcmOk;
			KeyPressed=false;
		};
		DSY.ProcessDialogs();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		if(!strcmp(passw,"qwe")){
			ChangeNation=1;
			MultiTvar=1;
			FogMode=0;
		}else
		if(!strcmp(passw,"VICTORY")){
#ifndef AI_CHEATS
				CurrentSinBattle=-1;
#endif
			if(NPlayers<2)ShowVictory();
		}else
		if(!strcmp(passw,"ntch"))ChangeNation=!ChangeNation;
		if(!strcmp(passw,"units"))MultiTvar=!MultiTvar;
		if(!strcmp(passw,"viewall"))FogMode=!FogMode;
		if(!strcmp(passw,"babki")&&CurrentSinBattle==0){
#ifndef AI_CHEATS
				CurrentSinBattle=-1;
#endif
			AddXRESRC(MyNation,0,150000);
			AddXRESRC(MyNation,1,150000);
			AddXRESRC(MyNation,2,150000);
			AddXRESRC(MyNation,3,150000);
			AddXRESRC(MyNation,4,150000);
			AddXRESRC(MyNation,5,150000);
		}else{
			char cc1[64];
			int n1,n2;
			int z=sscanf(passw,"%s%d%d",cc1,&n1,&n2);
			if(z==3&&!strcmp(cc1,"change")){
				if(n1>=0&&n1<8&&n2>=0&&n2<=8)SelChangeNation(n1,n2);
			};
			if(NPlayers==1){
				char cc2[128];
				z=sscanf(passw,"%s%s",cc1,cc2);
				if(z==2&&!strcmp(cc1,"changeunit")){
					word Type=0xFFFF;
					for(int i=0;i<NATIONS->NMon;i++){
						if(!strcmp(cc2,NATIONS->Mon[i]->MonsterID)){
							GeneralObject* GO=NATIONS->Mon[i];
							if(!GO->newMons->Building){
								//found!!!
								int N=NSL[MyNation];
								word* IDS=Selm[MyNation];
								word* SMS=SerN[MyNation];
								for(int j=0;j<N;j++){
									word MID=IDS[j];
									if(MID!=0xFFFF){
										OneObject* OB=Group[MID];
										if(OB&&!OB->Sdoxlo){
											OB->Ref.General=GO;
											OB->NIndex=i;
											OB->newMons=GO->newMons;
										};
									};
								};
							};
						};
					};
				};
			};
		};
	};
};
void SaveGame(char* fnm,char* Messtr,int ID);

//----------------SAVE MAP--------------//
void NormalName(char* s){
	if(!strstr(s,".m3d")){
		strcat(s,".m3d");
	};
};
bool LBCLICK(SimpleDialog* SD){
	ListBox* LB=(ListBox*)SD;
	if(LB->CurItem!=-1){
		strcpy(GameName,(LB->GetItem(LB->CurItem))->Message);
	};
	return false;
};
void SaveExtendedMap(char* fnm);
void PushScreen(byte** Ptr);
void PopScreen(byte* Ptr);
bool ProcessSavingFile(char* Mask,char* DestName,int Header,bool clear);
void ProcessGSaveMap(){
	byte* p;
	PushScreen(&p);
	if(strstr(GameName,".sav")||strstr(GameName,".SAV"))GameName[0]=0;
	if(ProcessSavingFile("*.m3d",GameName,7,0)){
		NormalName(GameName);
		ResFile F=RReset(GameName);
		if(F!=INVALID_HANDLE_VALUE){
			RClose(F);
			char ccc[128];
			strcpy(ccc,GameName);
			char* cc=strstr(ccc,".m3d");
			if(cc){
				strcpy(cc,".bak");
				DeleteFile(cc);
				rename(GameName,ccc);
			};
		};
		Save3DMap(GameName);
	};
	PopScreen(p);
};
//----------------LOAD Map--------------//
void PreLoadExtendedMap(char* fnm);
void PostLoadExtendedMap(char* fnm);
bool ProcessLoadingFile(char* Mask,char* DestName,int Header);
extern bool PeaceMode;
void ProcessGLoadMap(){
	byte* p;
	PushScreen(&p);
	if(ProcessLoadingFile("*.m3d",GameName,4)){
		Load3DMap(GameName);
		PeaceMode=1;
	};
	PopScreen(p);
	/*
	SQPicture Pan("Pan15.bpx");
	SQPicture scr_U("scr_v_u.bpx");
	SQPicture scr_D("scr_v_d.bpx");
	SQPicture scr_V("scr_v.bpx");
	SQPicture scr_Line("scr_15.bpx");
	RLCFont gf24w(GETS("@SMFONT1"));
	RLCFont gf24y(GETS("@SMFONT0"));
	RLCFont ListF1(GETS("@CULISTF1"));
	RLCFont ListF0(GETS("@CULISTF0"));
	int mmlx=Pan.GetLx();
	int mmly=Pan.GetLy();

	DialogsSystem DSY(smapx+(nsmaplx<<4)-(mmlx>>1),smapy+(mul3(nsmaply)<<2)-(mmly>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&Pan,&Pan,&Pan);
	//TextButton* LoadBtn=DSY.addTextButton(PIC1,352>>1,30,"ÇÀÃÐÓÇÈÒÜ ÊÀÐÒÓ",&FActive,&FPassive,&FDisable,1);
	ListBox* LB=DSY.addListBox(PIC1,GETV("LMLSTX"),GETV("LMLSTY"),GETV("LMLSTNY"),GETV("LMLSTLX"),GETV("LMLSTLY"),&ListF1,&ListF0,NULL);
	VScrollBar* VS=DSY.addVScrollBar(PIC1,GETV("LMSCRX"),GETV("LMSCRY"),1000,4,&scr_U,&scr_U,&scr_U,&scr_D,&scr_D,&scr_D,&scr_Line,&scr_Line,&scr_V);
	LB->VS=VS;
	TextButton* OkBtn=DSY.addTextButton(PIC1,GETV("LMOKX"),GETV("LMOKY"),GETS("@LMOKSTR"),&gf24w,&gf24y,&gf24y,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	TextButton* CancelBtn=DSY.addTextButton(PIC1,GETV("LMNOX"),GETV("LMNOY"),GETS("@LMNOSTR"),&gf24w,&gf24y,&gf24y,1);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	CreateNewMapsList(LB);
	ItemChoose=-1;
	do{
		DSY.MarkToDraw();
		ProcessMessages();
		StdKeys();
		if(LB->CurItem==-1){
			OkBtn->Enabled=false;
			if(ItemChoose==mcmOk)ItemChoose=-1;
		}else OkBtn->Enabled=true;
		DSY.ProcessDialogs();
		DSY.MarkToDraw();
		DSY.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk&&LB->CurItem!=-1){
		Load3DMap((LB->GetItem(LB->CurItem))->Message);
		strcpy(GameName,(LB->GetItem(LB->CurItem))->Message);
	};
	//FreeNames();
	*/
};
//-----------------OPTIONS---------------//
	

//---------------------------------------//
void NewMap(int szX,int szY);
extern byte mapPos[16];
void ClearAllZones();
void CreateNewMap(int SizeX,int SizeY){
	UnLoading();
	InitAllGame();
	NewMap(SizeX,SizeY);
	SetMyNation(0);
	ItemChoose=mcmSingle;
	msx=SizeX;
	msy=SizeY;
	InitExplosions();
	//ClearAllZones();
};
//--------Choose units Dialog------------//
void CreateUnitList(ListBox* LB,int Country){
	Nation* NT=&NATIONS[MyNation];
	LB->CurItem=0;
	LB->FirstItem=0;
	LB->FLItem=0;
	LB->VS->SPos=0;
	if(!Country){
		LB->AddStaticItem("UNITS:    ",0);
		for(int i=0;i<NT->NMon;i++){
			NewMonster* NM=NT->Mon[i]->newMons;
			if(!NM->Building)
				LB->AddItem(NM->Message,i);
		};
		LB->AddStaticItem("BUILDINGS:   ",0);
		for(int i=0;i<NT->NMon;i++){
			NewMonster* NM=NT->Mon[i]->newMons;
			if(NM->Building)
				LB->AddItem(NM->Message,i);
		};
		return;
	};
	Country--;
	int NUNITS=NT->NUnits[Country];
	if(NUNITS){
		for(int j=0;j<NUNITS;j++){
			int UIDS=NT->UnitsIDS[Country][j];
			if(UIDS==0xFFFF){
				LB->AddStaticItem(NT->UnitNames[Country][j],0xFFFF);
			}else{
				NewMonster* NM=NT->Mon[UIDS]->newMons;
				LB->AddItem(NM->Message,UIDS);
			};
		};
	};
};
extern int NNations;
extern char** NatNames;
void UnPress();
void ChooseUnit(){
	ItemChoose=-1;
	SQPicture Pan("Units.bpx");
	//SQPicture Lstr("195x20.bpx");
	SQPicture scr_U("scr_v_u.bpx");
	SQPicture scr_D("scr_v_d.bpx");
	SQPicture scr_V("scr_v.bpx");
	SQPicture scr_Line("scr_13.bpx");
	RLCFont BtnF0(GETS("@CUFONT0"));
	RLCFont BtnF1(GETS("@CUFONT1"));
	RLCFont BtnL0(GETS("@CULISTF0"));
	RLCFont BtnL1(GETS("@CULISTF1"));
	RLCFont SelF (GETS("@CULISTF2"));
	int mmlx=Pan.GetLx();
	int mmly=Pan.GetLy();
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(mmlx>>1),smapy+(mul3(nsmaply)<<2)-(mmly>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&Pan,&Pan,&Pan);
	//TextButton* LoadBtn=DSY.addTextButton(PIC1,352>>1,30,"ÂÛÁÎÐ ÎÁÚÅÊÒÀ",&FActive,&FPassive,&FDisable,1);
	ListBox* LBU=DSY.addListBox(PIC1,GETV("CUUNITX"),GETV("CUUNITY"),GETV("CUNY"),GETV("CULX"),GETV("CULY"),&BtnL1,&BtnL0,NULL);
	ListBox* LBB=DSY.addListBox(PIC1,GETV("CUBUILDX"),GETV("CUBUILDY"),GETV("CUNY"),GETV("CULX"),GETV("CULY"),&BtnL1,&BtnL0,NULL);
	LBB->DFont=&SelF;
	VScrollBar* VSU=DSY.addVScrollBar(PIC1,GETV("CUUNITSX"),GETV("CUUNITSY"),1000,4,&scr_U,&scr_U,&scr_U,&scr_D,&scr_D,&scr_D,&scr_Line,&scr_Line,&scr_V);
	VScrollBar* VSB=DSY.addVScrollBar(PIC1,GETV("CUBUILDSX"),GETV("CUBUILDSY"),1000,4,&scr_U,&scr_U,&scr_U,&scr_D,&scr_D,&scr_D,&scr_Line,&scr_Line,&scr_V);

	LBU->VS=VSU;
	LBB->VS=VSB;

	TextButton* OkBtn=DSY.addTextButton(PIC1,GETV("CUOKX"),GETV("CUOKY"),GETS("@CUOKSTR"),&BtnF0,&BtnF1,&BtnF1,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	TextButton* CancelBtn=DSY.addTextButton(PIC1,GETV("CUNOX"),GETV("CUNOY"),GETS("@CUNOSTR"),&BtnF0,&BtnF1,&BtnF1,1);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	Nation* NT=&NATIONS[MyNation];
	LBU->AddItem("ALL UNITS",0);
	for(int j=0;j<NNations;j++){
		LBU->AddItem(NatNames[j],j+1);
	};
	LBU->CurItem=0;
	LBU->FLItem=0;
	LBB->CurItem=0;
	LBB->FLItem=0;
	VSU->SPos=0;
	VSB->SPos=0;
	int CurNation=0;
	CreateUnitList(LBB,CurNation);
	ItemChoose=-1;
	int curpan=-1;
	do{
		ProcessMessages();
		if(!LBB->NItems)OkBtn->Enabled=false;
		else OkBtn->Enabled=true;
		DSY.MarkToDraw();
		DSY.ProcessDialogs();
		//DSY.MarkToDraw();
		DSY.RefreshView();
		if(CurNation!=LBU->CurItem){
			LBB->ClearItems();
			CurNation=LBU->CurItem;
			CreateUnitList(LBB,CurNation);
		};
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		if(LBB->NItems){
			Creator=(LBB->GetItem(LBB->CurItem))->Param1;
		}else Creator=4096+255;
		ContinueGame=true;
	};
	UnPress();
};
void ChooseUnitOld(){
	ItemChoose=-1;
	SQPicture Pan("Units.bpx");
	SQPicture Lstr("195x20.bpx");
	SQPicture scr_U("scr_v_u.bpx");
	SQPicture scr_D("scr_v_d.bpx");
	SQPicture scr_V("scr_v.bpx");
	SQPicture scr_Line("scr_13.bpx");
	RLCFont BtnF0(GETS("@CUFONT0"));
	RLCFont BtnF1(GETS("@CUFONT1"));
	RLCFont BtnL0(GETS("@CULISTF0"));
	RLCFont BtnL1(GETS("@CULISTF1"));
	int mmlx=Pan.GetLx();
	int mmly=Pan.GetLy();
	DialogsSystem DSY(smapx+(nsmaplx<<4)-(mmlx>>1),smapy+(mul3(nsmaply)<<2)-(mmly>>1));
	Picture* PIC1=DSY.addPicture(NULL,0,0,&Pan,&Pan,&Pan);
	//TextButton* LoadBtn=DSY.addTextButton(PIC1,352>>1,30,"ÂÛÁÎÐ ÎÁÚÅÊÒÀ",&FActive,&FPassive,&FDisable,1);
	ListBox* LBU=DSY.addListBox(PIC1,GETV("CUUNITX"),GETV("CUUNITY"),GETV("CUNY"),GETV("CULX"),GETV("CULY"),&BtnL1,&BtnL0,NULL);
	ListBox* LBB=DSY.addListBox(PIC1,GETV("CUBUILDX"),GETV("CUBUILDY"),GETV("CUNY"),GETV("CULX"),GETV("CULY"),&BtnL1,&BtnL0,NULL);
	
	VScrollBar* VSU=DSY.addVScrollBar(PIC1,GETV("CUUNITSX"),GETV("CUUNITSY"),1000,4,&scr_U,&scr_U,&scr_U,&scr_D,&scr_D,&scr_D,&scr_Line,&scr_Line,&scr_V);
	VScrollBar* VSB=DSY.addVScrollBar(PIC1,GETV("CUBUILDSX"),GETV("CUBUILDSY"),1000,4,&scr_U,&scr_U,&scr_U,&scr_D,&scr_D,&scr_D,&scr_Line,&scr_Line,&scr_V);

	LBU->VS=VSU;
	LBB->VS=VSB;

	TextButton* OkBtn=DSY.addTextButton(PIC1,GETV("CUOKX"),GETV("CUOKY"),GETS("@CUOKSTR"),&BtnF0,&BtnF1,&BtnF1,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	TextButton* CancelBtn=DSY.addTextButton(PIC1,GETV("CUNOX"),GETV("CUNOY"),GETS("@CUNOSTR"),&BtnF0,&BtnF1,&BtnF1,1);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	Nation* NT=&NATIONS[MyNation];
	for(int j=0;j<NT->NMon;j++){
		NewMonster* NM=NT->Mon[j]->newMons;
		if(NM->Building)LBB->AddItem(NT->Mon[j]->Message,j);
		else LBU->AddItem(NT->Mon[j]->Message,j);
	};
	LBU->CurItem=0;
	LBU->FLItem=0;
	LBB->CurItem=0;
	LBB->FLItem=0;
	
	ItemChoose=-1;
	int curpan=-1;
	do{
		ProcessMessages();
		if(curpan==-1)OkBtn->Enabled=false;
		else OkBtn->Enabled=true;
		DSY.MarkToDraw();
		DSY.ProcessDialogs();
		//DSY.MarkToDraw();
		DSY.RefreshView();
		if(curpan==-1){
			if(LBU->CurItem!=-1)curpan=1;
			if(LBB->CurItem!=-1)curpan=2;
		};
		if(curpan==1&&LBB->CurItem!=-1){
			LBU->CurItem=-1;
			curpan=2;
		};
		if(curpan==2&&LBU->CurItem!=-1){
			LBB->CurItem=-1;
			curpan=1;
		};
	}while(ItemChoose==-1);
	if(curpan!=-1&&ItemChoose==mcmOk){
		if(curpan==1)Creator=(LBU->GetItem(LBU->CurItem))->Param1;
		if(curpan==2)Creator=(LBB->GetItem(LBB->CurItem))->Param1;
		ContinueGame=true;
	};
	UnPress();
};
//------MAIN MENU IN MAP EDITOR----------//

int ProcessE_GMainMenu();
int ProcessGMapEditMainMenu(){
	return ProcessE_GMainMenu();
	ContinueGame=false;
	SQPicture EPanel("edmenu.bpx");
	int mmlx=EPanel.GetLx();
	int mmly=EPanel.GetLy();
	RLCFont gf32w(GETS("@EMFONT1"));
	RLCFont gf32y(GETS("@EMFONT0"));
	DialogsSystem GMM(smapx+(nsmaplx<<4)-(mmlx>>1),smapy+(mul3(nsmaply)<<2)-(mmly>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&EPanel,&EPanel,&EPanel);
	int emmx=GETV("EMMENUX");
	TextButton* ExitBtn=GMM.addTextButton(PIC1,emmx,GETV("EMRETY"),GETS("@EMRET"),&gf32w,&gf32y,&gf32y,1);
	TextButton* SaveBtn=GMM.addTextButton(PIC1,emmx,GETV("EMSAVEMY"),GETS("@EMSAVEM"),&gf32w,&gf32y,&gf32y,1);
	SaveBtn->UserParam=mcmSave;
	SaveBtn->OnUserClick=&MMItemChoose;
	TextButton* LoadBtn=GMM.addTextButton(PIC1,emmx,GETV("EMLOADMY"),GETS("@EMLOADM"),&gf32w,&gf32y,&gf32y,1);
	LoadBtn->UserParam=mcmLoad;
	LoadBtn->OnUserClick=&MMItemChoose;
	TextButton* NewBtn=GMM.addTextButton(PIC1,emmx,GETV("EMNEWMY"),GETS("@EMNEWM"),&gf32w,&gf32y,&gf32y,1);
	TextButton* HelpBtn=GMM.addTextButton(PIC1,emmx,GETV("EMOPTY"),GETS("@EMOPT"),&gf32w,&gf32y,&gf32y,1);
	HelpBtn->UserParam=mcmOptions;
	HelpBtn->OnUserClick=&MMItemChoose;
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	TextButton* ResumeBtn=
		GMM.addTextButton(PIC1,emmx,GETV("EMCANCELY"),GETS("@EMCANCEL"),&gf32w,&gf32y,&gf32y,1);
	ResumeBtn->UserParam=mcmResume;
	ResumeBtn->OnUserClick=&MMItemChoose;
	ItemChoose=-1;
	do{
		ProcessMessages();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmLoad){
		ProcessGLoadMap();
		GMM.MarkToDraw();
		ItemChoose=mcmResume;
		//if(!ContinueGame)goto stg;
	};
	if(ItemChoose==mcmSave){
		ProcessGSaveMap();
		GMM.MarkToDraw();
		ItemChoose=mcmResume;
		//if(!ContinueGame)goto stg;
	};
	if(ItemChoose==mcmOptions){
		ProcessGameOptions();
		//ProcessOptions(smapx+(nsmaplx<<4)-(288>>1),smapy+(nsmaply<<3)-(288>>1));
		ContinueGame=true;
		ItemChoose=mcmResume;
	};
	return ItemChoose;	
}; 
//------------------Selecting Texture dialog--------------
extern byte *tex1;
extern byte EditMedia;
extern bool MEditMode;
extern bool EditMapMode;
extern int HeightEditMode;
word TexList[128];
word NTextures;
void RefreshED_BV();
word GetTexture(){
	if(!NTextures)return 0;
	return TexList[(rando()*NTextures)>>15];
};
void MSetTexture(int tex,bool add){
	if(add){
		if(NTextures>=128)return;
		TexList[NTextures]=tex;
		NTextures++;
		Lpressed=0;
	}else{
		TexList[0]=tex;
		NTextures=1;
	};
	RefreshED_BV();
};
extern int MaxTex;
int SelectTexture(){
	ContinueGame=false;
	GameMode=2;
	NTextures=0;
	bool MIMOD=MiniMode;
	int mmmx=mapx;
	int mmmy=mapy;
	//ClearMiniMode();
	SQPicture Pan("textur.bpx");
	//SQPicture Lstr("195x20.bpx");
	SQPicture scr_U("scr_v_u.bpx");
	SQPicture scr_D("scr_v_d.bpx");
	SQPicture scr_V("scr_v.bpx");
	SQPicture scr_Line("scr_12.bpx");
	RLCFont f24y(GETS("@STFONT0"));
	RLCFont f24w(GETS("@STFONT1"));
	int mmlx=Pan.GetLx();
	int mmly=Pan.GetLy();
	DialogsSystem GMM(smapx+(nsmaplx<<4)-(mmlx>>1),smapy+(mul3(nsmaply)<<2)-(mmly>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&Pan,&Pan,&Pan);
	TextButton* OkBtn=
		GMM.addTextButton(PIC1,GETV("STOKX"),GETV("STOKY"),GETS("@STOKMESS"),&f24w,&f24y,&f24y,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	TextButton* CancelBtn=
		GMM.addTextButton(PIC1,GETV("STCANCELX"),GETV("STCANCELY"),GETS("@STNOMESS"),&f24w,&f24y,&f24y,1);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	VScrollBar* VS=GMM.addVScrollBar(PIC1,GETV("STSCRX"),GETV("STSCRY"),(MaxTex>>2)-4,0,&scr_U,&scr_U,&scr_U,&scr_D,&scr_D,&scr_D,&scr_Line,&scr_Line,&scr_V);
	BPXView* BV=GMM.addBPXView(PIC1,GETV("STWINX"),GETV("STWINY"),48,48,4,4,MaxTex>>2,tex1,VS);
	BV->DECLX=16;
	ItemChoose=-1;
	do{
		ProcessMessages();
		GMM.MarkToDraw();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1&&BV->ChoosedPos==-1);
	if(ItemChoose==mcmCancel){
		UnPress();
		return false;
	};
	if(BV->ChoosedPos!=-1){
		EditMedia=BV->ChoosedPos;
		HeightEditMode=false;
		MEditMode=true;
		Lpressed=false;
		TexList[0]=BV->ChoosedPos;
		NTextures=1;
	}else{
		int ntx=0;
		int nn=BV->Nx*BV->RealNy;
		for(int i=0;i<nn;i++){
			if(BV->Choosed[i]){
				for(int j=0;j<BV->Choosed[i];j++){
					if(ntx<256){
						TexList[ntx]=i;
						ntx++;
					};
				};
			};
		};
		if(ntx){
			NTextures=ntx;
			HeightEditMode=false;
			MEditMode=true;
			Lpressed=false;
		};
	};
	GameMode=0;
	UnPress();
	return ItemChoose;	
};
//----------MAIN MENU IN GAME------------//
bool IsGameActive();
int LastLookTime=0;


//---------------------------------------//
extern int HintX;
extern int HintY;
byte ZoneOpt[128];
//  /-------------------------------------------------------\
//  | /--------\  2    3    4    5    6    7    8   /------\|       
//  | | 1      |                                    |      ||
//  | |        |  9    10   11   12  ...            |      ||
//  | |        |                                    |      ||
//  | \--------/  ...                               \------/|
//  \-------------------------------------------------------/
// ZoneOpt[i]:  0   -   empty
//              |1  -   used now
//              |2  -   was used
int Zone1X;
int Zone1Y;
int Zone1LX;
int Zone1LY;
int ZonesNy;
int ZonesNx;
int LeftListNx;
int PanelY;
int MessageUX;
int MessageUY;
int PropertyX;
int PropertyY;
//---------
int BrigPnX;
int BrigPnY;
int BrigNx;
int BrigNy;
extern IconSet PrpPanel;
extern IconSet AblPanel;
extern IconSet UpgPanel;
extern IconSet BrigPanel;
extern IconSet GeneralPanel;
void CopyBPX2SCR(int xs,int ys,int xb,int yb,int Lx,int Ly,SQPicture* P){
    if(!Ly)return;
    word* Pic=(word*)P->GetPixelData();
    int BLx=P->GetLx();
    int BLy=P->GetLy();
    int bofs=xb+yb*BLx+int(Pic);
    int sofs=xs+ys*SCRSizeX+int(ScreenPtr);
    int badd=BLx-Lx;
    int sadd=SCRSizeX-Lx;
    int Lx4=Lx>>2;
    int Lx1=Lx&3;
    __asm{
        pushf
        push    esi
        push    edi
        cld
        mov     esi,bofs
        mov     edi,sofs
        mov     edx,Ly
lpp1:   mov     ecx,Lx4
        rep     movsd
        mov     ecx,Lx1
        rep     movsb
        add     esi,badd
        add     edi,sadd
        dec     edx
        jnz     lpp1
        pop     edi
        pop     esi
        popf
    };

};
void ShowTextDiscription();
void DelUnusedZones(){
    for(int i=0;i<128;i++)if(ZoneOpt[i])ZoneOpt[i]=2;
    if(PrpPanel.NIcons==1){
        //CopyBPX2SCR(Zone1X,PanelY+Zone1Y,Zone1X,Zone1Y,Zone1LX,Zone1LY,&LoPanel);
		//Xbar(Zone1X,PanelY+Zone1Y+1,Zone1LX,Zone1LY,0xD0+(MyNation<<2));
		//Xbar(Zone1X-1,PanelY+Zone1Y,Zone1LX+2,Zone1LY+2,0xD0+(MyNation<<2)+1);
        ZoneOpt[0]|=1;
		int N=AblPanel.NIcons;
        for(int i=0;i<N;i++){
            div_t ddt=div(i,AblNx);
            if(AblPanel.Icons[i].Visible)ZoneOpt[ddt.rem+ddt.quot*ZonesNx+1]=1;
        };
		/*
		N=UpgPanel.NIcons;
        for(int i=0;i<N;i++){
            div_t ddt=div(i,AblNx);
            ZoneOpt[ddt.rem+ddt.quot*ZonesNx+1]|=1;
        };
		*/
    }else{
        //if(ZoneOpt[0])
        //    CopyBPX2SCR(Zone1X,PanelY+Zone1Y,Zone1X,Zone1Y,Zone1LX,Zone1LY,&LoPanel);
		/*
        int N=PrpPanel.NIcons;
        for(int i=0;i<N;i++){
            div_t ddt=div(i,PrpNx);
            ZoneOpt[ddt.rem+ddt.quot*ZonesNx+1]|=1;
        };
		*/
        int N=AblPanel.NIcons;
        for(int i=0;i<N;i++){
            div_t ddt=div(i,AblNx);
            if(AblPanel.Icons->Visible)ZoneOpt[ddt.rem+ddt.quot*ZonesNx+1]=1;
        };
    };
    int maxzon=ZonesNx*ZonesNy;
    for(int i=1;i<=maxzon;i++){
        if(ZoneOpt[i]==2){
            div_t ddt=div(i-1,ZonesNx);
            int x=ddt.rem*(IconLx+AddIconLx)+PrpX;
            int y=ddt.quot*(IconLy+AddIconLy)+PropertyY;
            //CopyBPX2SCR(x,y+PanelY,x,y,IconLx+2,IconLy+2,&LoPanel);
        };
    };
    for(int i=0;i<maxzon;i++)if(ZoneOpt[i]==2)ZoneOpt[i]=0;
};
void EconomePostDraw();
extern bool ECOSHOW;
extern byte PlayGameMode;
extern bool PAGE1PRESENT;
extern bool PAGE2PRESENT;
extern bool PAGE3PRESENT;
bool Page3HaveItems=0;
bool Page2HaveItems=0;
bool Page1HaveItems=0;
NewMonster* LASTNM=NULL;
extern bool RejectInterface;
int GenPnX;
void DrawZones(){
	if(PlayGameMode==1||RejectInterface)return;
	RunPF(5,"DrawZones");
	bool P1=Page1HaveItems;
	bool P2=Page2HaveItems;
	bool P3=Page3HaveItems;

	if(PAGE1PRESENT&&!P1)AblPanel.Icons[0].Visible=0;
	if(PAGE2PRESENT&&!P2)AblPanel.Icons[12].Visible=0;
	if(PAGE3PRESENT&&!P3)AblPanel.Icons[24].Visible=0;

	bool UNKPAGE=0;
	if(LASTNM){
		if(LASTNM->CurrPage==0&&!P1)UNKPAGE=1;
		if(LASTNM->CurrPage==1&&!P2)UNKPAGE=1;
		if(LASTNM->CurrPage==2&&!P3)UNKPAGE=1;
		if(UNKPAGE){
			if(Page1HaveItems)LASTNM->CurrPage=0;
			else if(Page2HaveItems)LASTNM->CurrPage=1;
			else if(Page3HaveItems)LASTNM->CurrPage=2;
			else LASTNM->CurrPage=0;
			AblPanel.Page=LASTNM->CurrPage;
		};
	};
    if(PrpPanel.NIcons==1){
		int Max1=AblPanel.GetMaxX(12);
		int Max2=UpgPanel.GetMaxX(12);
		if(Max2>Max1)Max1=Max2;
		int x0=Zone1X+6;
		int y0=Zone1Y+PanelY;
		int x1=AblX-4+2;
		int y1=y0+(IconLy*3)+5;
		if(Max1){
			DrawLeftRect(x0,y0,x1,y1);
			DrawRightRect(x1,y0,x1+(IconLx+2)*Max1+2,y1);
		}else{
			DrawStdRect(x0,y0,x1,y1);
		};
		PrpPanel.DrawIconSet(Zone1X+6,Zone1Y+PanelY,1,1,0);
        AblPanel.DrawIconSet(AblX,AblY,AblNx,AblNy,0);
		UpgPanel.DrawIconSet(PrpX,PrpY,PrpNx,PrpNy,0);
        ShowTextDiscription();
		if(ECOSHOW)EconomePostDraw();
    }else{
		int Max1=AblPanel.GetMaxX(12);
		int Max2=PrpPanel.GetMaxX(12);
		if(Max2>Max1)Max1=Max2;
		if(Max1){
			int y0=Zone1Y+PanelY;
			int x1=AblX-4+2;
			int y1=y0+(IconLy*3)+5;
			DrawStdRect(x1,y0,x1+(IconLx+2)*Max1+2,y1);
			PrpPanel.DrawIconSet(PrpX,PrpY,PrpNx,PrpNy,0);
			AblPanel.DrawIconSet(AblX,AblY,AblNx,AblNy,0);
		};
    };
    GenPnX = BrigPnX + 224;
	/*if (!EditMapMode)*/BrigPanel.DrawIconSet(BrigPnX, BrigPnY, BrigNx, BrigNy, 0);
    GeneralPanel.DrawIconSet(GenPnX, BrigPnY, BrigNx, BrigNy, 0);
	ECOSHOW=0;
	Page3HaveItems=0;
	Page2HaveItems=0;
	Page1HaveItems=0;
	StopPF(5);
};
void CopyZonesToScreen(){
    if(ZoneOpt[0]){
        CopyToScreen(Zone1X,Zone1Y,Zone1LX,Zone1LY);
    };
    int maxzon=ZonesNx*ZonesNy;
    for(int i=1;i<=maxzon;i++)if(ZoneOpt[i]){
        div_t ddt=div(i-1,ZonesNx);
        int x=ddt.rem*(IconLx+AddIconLx)+PrpX;
        int y=ddt.quot*(IconLy+AddIconLy)+PropertyY;
        CopyToScreen(x,y,IconLx+2,IconLy+2);
    };
};
bool NeedLoadGamePalette;
bool FullScreenMode=1;
void CreateINFORM();
extern int InfAddY;
extern int InfDY;
extern int InfDX;
extern int InfY1;
extern int InfY2;
DialogsSystem GLOBHINT(0,0);
void GSSetup800(){
	SetDarkPalette();
	CBar(0,0,SCRSizeX,SCRSizeY,0);
    memset(ZoneOpt,0,128);
	int DMiniX,DMiniY;
	int PanelLy;

	BrigPnX=6;
	BrigPnY=16+15;
	BrigNx=5;
	BrigNy=7;
	//if(RealLy>=768)
	InfAddY=16;
	//else InfAddY=0;
	InfDY=32+InfAddY;
	InfY1=450+InfAddY;
	InfY2=450-22+InfAddY;
	if(RealLy<=600){
		InfAddY=8;
		InfDY=32+InfAddY;
		InfY2=450-22-12-8;
		InfY1=450-12-8;
	}else{
		InfAddY=16;
		InfDY=32+InfAddY;
		InfY1=450+InfAddY;
		InfY2=450-22+InfAddY;
		if(RealLy<768){
			InfY1+=RealLy-768;
			InfY2+=RealLy-768;
		};
	};
	CreateINFORM();

	if(!CHKV("Zone1X")){
		Zone1X=6;
		Zone1Y=6;
		Zone1LX=132+40;
		Zone1LY=100;
		ZonesNy=6;
		ZonesNx=12;
		LeftListNx=3;
		MessageUX=75;
		MessageUY=8;
		PanelLy=144;
		PropertyX=142+40;
		PropertyY=6;
		IconLx=40;
		IconLy=40;
		DMiniX=152;
		DMiniY=7;
		AddIconLx=2;
		AddIconLy=2;
	}else{
		Zone1X=GETV("Zone1X");;
		Zone1Y=GETV("Zone1Y");
		Zone1LX=GETV("Zone1LX");
		Zone1LY=GETV("Zone1LY");
		ZonesNy=GETV("ZonesNy");
		ZonesNx=GETV("ZonesNx");
		LeftListNx=GETV("LeftListNx");
		MessageUX=GETV("MessageUX");;
		MessageUY=GETV("MessageUY");
		PanelLy=GETV("PanelLy");
		PropertyX=GETV("PropertyX");
		PropertyY=GETV("PropertyY");
		IconLx=GETV("IconLx");
		IconLy=GETV("IconLy");
		DMiniX=GETV("DMiniX");
		DMiniY=GETV("DMiniY");
		AddIconLx=GETV("AddIconLx");
		AddIconLy=GETV("AddIconLy");
	};
	MiniMode=false;
	Shifter=5;
	if(LMode)Shifter=3;
	Multip=0;
    //-------
    int scly=RealLy;
	if(!FullScreenMode)scly-=PanelLy;
    int CLY=mul3(32)>>2;
    int Y1=RealLy-PanelLy;
    smaply=div(scly,CLY).quot;
    smaplx=RealLx>>5;
	if(LMode){
		smaplx<<=2;
		smaply<<=2;
	};
    smapx=0;
	if(LMode)smapy=RealLy-smaply*4;
    else smapy=RealLy-smaply*16;
    minix=RealLx-DMiniX;
    miniy=RealLy-PanelLy+DMiniY;
    nsmaplx=smaplx;
	nsmaply=smaply;
	HintX=smapx+4;
	HintY=Y1-40;
    MiniMode=false;
    //ZonesNx=div(RealLx-PropertyX-DMiniX,IconLx+AddIconLx).quot;
    //if(ZonesNx<=7)PrpNx=3;else PrpNx=4;
	Multip=0;
    PrpX=PropertyX;
    PrpY=Y1+PropertyY;
	PrpNx=ZonesNx>>1;
    PrpNy=ZonesNy;

    AblX=PrpX;//+(PrpNx)*(IconLx+2);
    AblY=Y1+PropertyY;
    AblNy=ZonesNy;
    AblNx=PrpNx=ZonesNx;

    GSYS.~DialogsSystem();
	
    LoPanel.~SQPicture();
	//LoPanel.LoadPicture("800.bmp");
	/*
    switch(RealLx){
    case 1024:
        //LoPanel.LoadPicture("1024.bpx");
        break;
    case 1152:
        //LoPanel.LoadPicture("1152.bpx");
        break;
    case 1280:
        //LoPanel.LoadPicture("1280.bpx");
        break;
    default:
        LoPanel.LoadPicture("800.bmp");
    };
	*/
	GVPort=GSYS.addViewPort(0,0,RealLx,RealLy);
    PanelY=RealLy-PanelLy;
    MessageUY+=PanelY;
	//GSYS.addPicture(NULL,0,PanelY,&LoPanel,&LoPanel,&LoPanel);
	//MiniVPort=GSYS.addViewPort(minix,miniy,msx>>1,msy>>1);
	//AblVPort=GSYS.addViewPort(AblX,AblY,AblNx*(IconLx+2),AblNy*(IconLy+2));
	//PrpVPort=GSYS.addViewPort(PrpX,PrpY,PrpNx*(IconLx+2),PrpNy*(IconLy+2));
	//agew.pal");//grassw.pal");//dmw.pal");//"darkw.pal");
	NeedLoadGamePalette=true;
	if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
	if(mapy+smaply>msy+1)mapy=msy-smaply+1;
	
	GLOBHINT.HintX=-1;
	GLOBHINT.HintY=100;
	GLOBHINT.HintFont=&SmallBlackFont1;

	int x1=RealLx-716+25;
	int dx0 = 110;
	
	SimpleDialog* MONEY;
	GLOBHINT.CloseDialogs();
	MONEY=GLOBHINT.addViewPort(x1,7,dx0-5,20);
	MONEY->Hint=RESHINT0;
	MONEY=GLOBHINT.addViewPort(x1+dx0,7,dx0-5,20);
	MONEY->Hint=RESHINT1;
	MONEY=GLOBHINT.addViewPort(x1+dx0*2,7,dx0-5,20);
	MONEY->Hint=RESHINT2;
	MONEY=GLOBHINT.addViewPort(x1+dx0*3,7,dx0-5,20);
	MONEY->Hint=RESHINT3;
	MONEY=GLOBHINT.addViewPort(x1+dx0*4,7,dx0-5,20);
	MONEY->Hint=RESHINT4;
	MONEY=GLOBHINT.addViewPort(x1+dx0*5,7,dx0-5,20);
	MONEY->Hint=RESHINT5;	

};
void GFieldShow();
void GMiniShow();
void GlobalHandleMouse();
void DRAW_MAP_TMP();
void DrawAllScreen(){
    if (NeedLoadGamePalette) {
        if (IgnoreSlow) {
            LoadPalette("0\\agew_1.pal");
            LoadFog(0);
        }
        else {
            SlowLoadPalette("0\\agew_1.pal");
            LoadFog(0);
        };
        CreateMiniMap();
    };
	NeedLoadGamePalette=false;
	//COUNTER++;
	GFieldShow();
	//Pan1->OnDraw(Pan1);
	//LoPanel.Draw(0,PanelY);
	GMiniShow();
	//Pan2->OnDraw(Pan2);
	ShowProp();
	//Pan3->OnDraw(Pan3);
	ShowAbility();
	//Pan4->OnDraw(Pan4);
	//if(Pan5)Pan5->OnDraw(Pan5);
    memset(ZoneOpt,0,128);
    DrawZones();
	DRAW_MAP_TMP();
	CopyToScreen(0,0,RealLx,RSCRSizeY);
	GlobalHandleMouse();//mouseX,mouseY);
	MFix();
};
void GlobalHandleMouse();

void FastScreenProcess() {
	RunPF(7,"FastScreenProcess");
	if(NeedLoadGamePalette){
		if(IgnoreSlow){
			LoadPalette("0\\agew_1.pal");
			LoadFog(0);
		}else{
			SlowLoadPalette("0\\agew_1.pal");
			LoadFog(0);
		};
		CreateMiniMap();
	};
    //SERROR2();
	///RunPF(14);
	SUSP("GFIELDSHOW");
	RunPF(8,"GFieldShow");
	GFieldShow();
	StopPF(8);
	ProcessMessages();
	if(SHOWSLIDE){
		RunPF(11,"GMiniShow");
		GMiniShow();
		StopPF(11);
		ShowProp();
		ShowAbility();
		SUSP("DRAWZONES");
		DrawZones();
		DRAW_MAP_TMP();
		RunPF(10,"GSYS.ProsessDialogs");
		GSYS.ProcessDialogs();
		StopPF(10);
		GVPort->NeedToDraw=true;
	};
	SUSP("HANDLEMOUS");
	//HandleMouse(mouseX,mouseY);
	RunPF(9,"GlobalHandleMouse");
	GlobalHandleMouse();
	StopPF(9);
	MFix();
	if(SHOWSLIDE){
	//	SERROR2();
		int time0=GetRealTime();
		//CopyToScreen(0,0,RealLx,RSCRSizeY);
		SUSP("REFRESHVIEW");
		GSYS.RefreshView();
		 //CopyZonesToScreen();
		//FlipPages();
		time8=GetRealTime()-time0;
	};
	SUSP("ENDFAST");
	
	NeedLoadGamePalette=false;
	//StopPF(14);
    //SERROR1();
	StopPF(7);
};

void PreDrawGameProcess();
void PostDrawGameProcess();
void IAmLeft();
extern bool LockPause;
bool ShowStatistics();
extern bool ShowStat;
extern int WaitState;
void CmdEndGame(byte NI,byte state,byte cause);
int StartAboutTime=0;
int NAboutLn=-1;
void ShowAbout(){
	TempWindow TM;
	PushWindow(&TM);
	IntersectWindows(0,smapy,smaplx*32,smaply*16);
	int y0=RealLy-(GetTickCount()-StartAboutTime)/50;
	char pp[100];
	if(NAboutLn==-1){
		for(int i=0;i<200;i++){
			sprintf(pp,"ABLINE%d",i);
			if(pp==GetTextByID(pp)){
				NAboutLn=i;
				i=300;
			};
		};
	};
	if(NAboutLn==-1)return;
	int LDis=35;
	RLCFont* TextFon=&fon30y3;
	for(int i=0;i<NAboutLn;i++){
		sprintf(pp,"ABLINE%d",i);
		char* tx=GetTextByID(pp);
		if(y0>-LDis){
			ShowString((RealLx-GetRLCStrWidth(tx,TextFon))>>1,y0,tx,TextFon);
		}
		y0+=LDis;
	};
	PopWindow(&TM);
};
bool Lobby=0;
void REPLAY();
void LOOSEANDEXITFAST();
void EndGSC_Reporting();
bool ShowPostScreen=0;
void PlayGame(){
    InGame = true;
    if (window_mode)
    {//Explicit call in case if game starts at 1024x768
        ResizeAndCenterWindow();
    }
    //Reset resolution for main menu
    if (window_mode)
    {//Dont't do in fullscreen to prevent menu stretching
        if (RealLx != 1024 || RealLy != 768)
        {
            SetGameDisplayModeAnyway(1024, 768);
        }
    }
    else
    {//Always go for native screen resolution when showing menu in fullscreen
        SetGameDisplayModeAnyway(screen_width, screen_height);
    }
	GSSetup800();
	LoadFog(0);
	InitGame();
	if(exRealLx!=RealLx)SetGameDisplayModeAnyway(exRealLx,exRealLy);
	DrawAllScreen();
	GameNeedToDraw=false;
	GameExit=false;
	MakeMenu=false;
	
	ShowPostScreen=0;
	if(PlayGameMode)StartAboutTime=GetTickCount();
StartPlay:
	do{
		if(RUNMAPEDITOR||RUNUSERMISSION){
			GameExit=true;
		};
		time1=GetRealTime();
		ProcessMessages();
		if(bActive){
			GameMode=0;
			PreDrawGameProcess();
			//time3=GetRealTime()-time1;
			SUSP("Process1");
			ProcessMessages();
			if(GameNeedToDraw){
				//COUNTER++;
				SUSP("DrawAllScr");
				DrawAllScreen();
				GameNeedToDraw=false;
			}else{
				SUSP("FastScreen");
				FastScreenProcess();
			};
			//DrawAllScreen();
			//DrawAllScreen();
			SUSP("Process2");
			ProcessMessages();
			time1=GetRealTime();
			SUSP("PostDraw");
			PostDrawGameProcess();
			time5=GetRealTime()-time1;
			if(MakeMenu){
				switch(MenuType){
				case 1:	if(ProcessGMainMenu()==mcmExit){
							if(NPlayers>1&&!NATIONS[MyNation].VictState){
								LockPause=1;
								WaitState=1;
								if(SCENINF.hLib&&!SCENINF.StandartVictory){
									SCENINF.LooseGame=1;
								};
								if(NATIONS[MyNation].VictState!=2)CmdEndGame(MyNation,1,101);
								int t0=GetRealTime();
								do{
									ProcessMessages();
								}while(GetRealTime()-t0<500);
							};
							GameExit=true;
							ShutdownMultiplayer(0);
							CloseMPL();
							//IAmLeft();
						};
						break;
				case 2: ChooseUnit();
					    break;
				case 3: EnterChat();
						break;
				case 4: ProcessHelp();
						break;
				case 5: SelectTexture();
						break;
				};
				MakeMenu=false;
			}else{
				if(GameExit){
					IAmLeft();
					LOOSEANDEXITFAST();
					CloseMPL();
					ShutdownMultiplayer(0);
				};
			};
		}else GameNeedToDraw=true;
resgame:;
	}while(!GameExit);
	Lobby=0;
	PlayGameMode=0;
	if(RecordMode&&!SCENINF.hLib){
		RGAME.Save();
		RecordMode=0;
	};
	EndGSC_Reporting();
	UseGSC_Login=0;
	if(RetryVideo){
		GFILE* f=Gopen("Demo\\demo.run","r");
		if(f){
			int N;
			int z=Gscanf(f,"%d",&N);
			if(z==1){
				int M=GetRndVid(N);
				char NAME[64];
				for(int i=0;i<M;i++)Gscanf(f,"%s",NAME);
				Gscanf(f,"%s",NAME);
				RGAME.ReadStream(NAME);
				if(RGAME.Size){
					PlayGameMode=1;
					RetryVideo=1;
					KeyPressed=0;
				};
			};
			Gclose(f);
			GameExit=0;
			goto StartPlay;
		}else RetryVideo=0;
	};
	IgnoreSlow=true;
	//SlowUnLoadPalette("0\\agew_1.pal");
	int ExRX=RealLx;
	int ExRY=RealLy;
	//if(RealLx!=1024||RealLy!=768)SetGameDisplayModeAnyway(1024,768);
	if(ShowStat){
		if(!ShowStatistics()){
			GameExit=false;
			WaitState=1;
			if(RealLx!=ExRX||RealLy!=ExRY)SetGameDisplayModeAnyway(ExRX,ExRY);
			if(ItemChoose==131)REPLAY();
			goto resgame;
		};
		SlowUnLoadPalette("0\\agew_1.pal");
	};
	ClearScreen();
	IgnoreSlow=false;
};
void DrawAllEditScreen(){
	if(NeedLoadGamePalette){
		if(IgnoreSlow){
			LoadPalette("0\\agew_1.pal");
			LoadFog(0);
		}else{
			SlowLoadPalette("0\\agew_1.pal");
			LoadFog(0);
		};
		CreateMiniMap();
	};
	NeedLoadGamePalette=false;
	GFieldShow();
	//Pan1->OnDraw(Pan1);
	GMiniShow();
	//Pan2->OnDraw(Pan2);
	ShowProp();
	//Pan3->OnDraw(Pan3);
	ShowAbility();
	//Pan4->OnDraw(Pan4);
	//if(Pan5)Pan5->OnDraw(Pan5);
	DRAW_MAP_TMP();
	CopyToScreen(0,0,RealLx,RSCRSizeY);
};
void DRAW_MAP_TMP();
void FastEditScreenProcess(){
	//RunPF(14);
	GFieldShow();
	ProcessMessages();
	GMiniShow();
	//Pan1->OnDraw(Pan1);
	//Pan3->OnDraw(Pan3);
	ShowProp();
	ShowAbility();
	//Pan1->NeedToDraw=false;
	DRAW_MAP_TMP();
	GSYS.ProcessDialogs();
	//Pan1->NeedToDraw=false;
	//Pan2->NeedToDraw=false;
	//Pan3->NeedToDraw=false;
	GVPort->NeedToDraw=true;
	MiniVPort->NeedToDraw=true;
	AblVPort->NeedToDraw=true;
	PrpVPort->NeedToDraw=true;
	//HandleMouse(mouseX,mouseY);
	GlobalHandleMouse();
	MFix();
	GSYS.RefreshView();
	//StopPF(14);
};
void CheckGP();
void EditGame(){
    InEditor = true;
    if (window_mode)
    {//Explicit call in case if game starts at 1024x768
        ResizeAndCenterWindow();
    }
	LoadFog(0);
	GSSetup800();
	if(exRealLx!=RealLx)SetGameDisplayModeAnyway(exRealLx,exRealLy);
	DrawAllEditScreen();
	GameNeedToDraw=false;
	GameExit=false;
	MakeMenu=false;
	do{
		ProcessMessages();
		if(bActive){
			if(RUNUSERMISSION){
				GameExit=true;
			};
			if(RUNMAPEDITOR){
				Load3DMap(USERMISSPATH);
				strcpy(GameName,USERMISSPATH);
				RUNMAPEDITOR=0;
			};
			//SERROR2();
			//RunPF(15);
			PreDrawGameProcess();
			//SERROR1();
			SUSP("Process3");
			ProcessMessages();
			//if(GameNeedToDraw){
			//	DrawAllScreen();
			//	GameNeedToDraw=false;
			//}else 
			SUSP("FastScreen1");
//			StopPF(15);
			FastScreenProcess();
			//DrawAllScreen();
			//SERROR();
			SUSP("Process4");
			ProcessMessages();
			SUSP("PostDraw");
			//RunPF(16);
			PostDrawGameProcess();
//			StopPF(16);
			if(MakeMenu){
				switch(MenuType){
				case 1:	if(ProcessGMapEditMainMenu()==mcmExit)GameExit=true;
						break;
				case 2: ChooseUnit();
					    break;
				case 3: EnterChat();
						break;
				case 31: ProcessResEdit();
						break;
				case 5: SelectTexture();
						break;
				case 6: EditMiniMap();
						break;
				case 7: ChooseInterior(0);
						break;
				case 8: ChooseInterior(1);
						break;
				case 9: ChooseInterior(2);
						break;
				};
				MakeMenu=false;
			};
		}else GameNeedToDraw=true;
	}while(!GameExit);
	IgnoreSlow=true;
    //Reset resolution for main menu
    if (window_mode)
    {//Dont't do in fullscreen to prevent menu stretching
        if (RealLx != 1024 || RealLy != 768)
        {
            SetGameDisplayModeAnyway(1024, 768);
        }
    }
    else
    {//Always go for native screen resolution when showing menu in fullscreen
        SetGameDisplayModeAnyway(screen_width, screen_height);
    }
	IgnoreSlow=false;
};
//--------------ALL GAME IS IN THIS PROCEDURE!-------------//
BOOL FAR PASCAL EnumAddressCallback1(
	REFGUID guidDataType,
	DWORD dwDataSize,
	LPCVOID lpData,
	LPVOID lpContext
  ){
	if(guidDataType==DPAID_INet){
		strcpy(IPADDR,(char*)lpData);
		return false;
	};
	return true;
};

void ShowFailure(int CreateGame){
	ClearScreen();
	LoadFog(0);
	LoadPalette("0\\agew_1.pal");
	KeyPressed=0;
	bool NEEDDRAW=1;
	do{
		if(bActive){
			if(NEEDDRAW){
				CBar(0,0,RealLx,RealLy,0);
				char* ss[5];
				int NS=0;
				//if(CreateGame){
				char ccx[256];
				if(CreateGame==1){
					ss[0]=GetTextByID("LOBUNC");
					ss[1]=GetTextByID("LOBPR");
					NS=2;
				}else if(CreateGame==0){
					ss[0]=GetTextByID("LOBUNJ1");
					sprintf(ccx,GetTextByID("LOBUNJ5"),IPADDR);
					ss[1]=GetTextByID("LOBUNJ2");
					ss[2]=ccx;
					ss[3]=GetTextByID("LOBPR");
					NS=4;
				}else{
					ss[0]=GetTextByID("LOBUNJ3");
					sprintf(ccx,GetTextByID("LOBUNJ5"),IPADDR);
					ss[1]=GetTextByID("LOBUNJ4");
					ss[2]=ccx;
					ss[3]=GetTextByID("LOBPR");
					NS=4;
				};
				int L=0;
				for(int i=0;i<NS;i++){
					int LL=GetRLCStrWidth(ss[i],&BigWhiteFont);
					if(LL>L)L=LL;
				};
				DrawStdBar(512-32-L/2,380-NS*15-15,512+32+L/2,380+NS*15+15);
				for(int i=0;i<NS;i++)
					ShowString(512-GetRLCStrWidth(ss[i],&BigWhiteFont)/2,380-NS*15+i*30,ss[i],&BigWhiteFont);
				FlipPages();
			};
			NEEDDRAW=0;
		}else NEEDDRAW=true;
		ProcessMessages();
	}while(!KeyPressed);
	KeyPressed=0;
	ClearScreen();
	LoadFog(2);
	LoadPalette("2\\agew_1.pal");
};
void WaitWithMessage(char* Message);
bool AskQuestion2(char* Header,char* Text,char* OKTXT,char* CANCELTXT){
	ItemChoose=-1;
	int MPPLX=450;
	int MPPLY=350;
	int MPLDX=20;
	int MPRDX=20;
	int MPDDY=20;
	int MPUDY=20;
	LocalGP BOR2("Interface\\Bor2");
	DialogsSystem GMM((RealLx-MPPLX)>>1,(RealLy-MPPLY)>>1);
	
	
	int DY=0;
	
	int LX=0;
	int LY=0;
	GMM.addTextButton(NULL,MPPLX/2,20,Header,&BigWhiteFont,&BigWhiteFont,&BigWhiteFont,1);
	DY+=40;
	LY=40;
	TextViewer* TV;
	TV=GMM.addTextViewer(NULL,MPLDX,MPUDY+LY,MPPLX-MPLDX-MPRDX,MPPLY-LY-MPDDY-MPUDY-40,Text,&YellowFont);
	ItemChoose=-1;
	LY+=TV->NLines*TV->SymSize;
	TV->y1=TV->y;
	TextButton* YesBtn=GMM.addTextButton(NULL,MPPLX/4,LY+30,OKTXT,&BigWhiteFont,&BigYellowFont,&BigYellowFont,1);
	YesBtn->UserParam=mcmOk;
	YesBtn->OnUserClick=&MMItemChoose;
	TextButton* NoBtn=GMM.addTextButton(NULL,MPPLX*3/4,LY+30,CANCELTXT,&BigWhiteFont,&BigYellowFont,&BigYellowFont,1);
	NoBtn->UserParam=mcmCancel;
	NoBtn->OnUserClick=&MMItemChoose;
	LY+=70;
	int DDD=(MPPLY-LY)/2;
	GMM.BaseY+=DDD;
	for(int i=0;i<MAXDLG;i++)if(GMM.DSS[i]){
		GMM.DSS[i]->y+=DDD;
		GMM.DSS[i]->y1+=DDD;
	};
	MPPLY=LY;
	DrawStdBar2(GMM.BaseX,GMM.BaseY,GMM.BaseX+MPPLX,GMM.BaseY+MPPLY,BOR2.GPID);
	ItemChoose=-1;
	do{
		DrawStdBar2(GMM.BaseX,GMM.BaseY,GMM.BaseX+MPPLX,GMM.BaseY+MPPLY,BOR2.GPID);
		ProcessMessages();
		if(KeyPressed){
			if(LastKey==13)ItemChoose=mcmOk;
			else if(LastKey==27)ItemChoose=mcmCancel;
			KeyPressed=false;
		};
		GMM.MarkToDraw();
		GMM.ProcessDialogs();
		GMM.RefreshView();
	}while(ItemChoose==-1);
	UnPress();
	if(ItemChoose==mcmOk){
		ItemChoose=-1;
		return true;
	}else{
		ItemChoose=-1;
		return false;
	};
};
bool NotifyFirewallState(){
	char* txt=GetTextByID("FRW_TXT");
	ResFile F=RRewrite("tmp.txt");
	RBlockWrite(F,txt,strlen(txt));
	RClose(F);
	return AskQuestion2(GetTextByID("FRW_0"),"tmp.txt",GetTextByID("FRW_YES"),GetTextByID("FRW_IGN"));
};
bool DPL_CreatePlayer(LPDIRECTPLAY3A lpDirectPlay3A,
					  LPGUID lpguidSessionInstance, LPDPNAME lpszPlayerName,bool Host);
#ifdef MAKE_PTC
int CheckLobby(){

#ifndef _USE3D
	WindX=0;
	WindY=0;
	WindX1=1023;
	WindY1=767;
	WindLx=1024;
	WindLy=768;
#else
	GPS.SetClipArea( 0, 0, 1024, 768 );
#endif

	LPDIRECTPLAYLOBBYA	lpDPlayLobbyA = NULL;
	LPDIRECTPLAYLOBBY2A	lpDPlayLobby2A = NULL;
	if FAILED(DirectPlayLobbyCreate(NULL, &lpDPlayLobbyA, NULL, NULL, 0)) return false;
	// get ANSI DirectPlayLobby2 interface
	HRESULT hr = lpDPlayLobbyA->QueryInterface(IID_IDirectPlayLobby2A, (LPVOID *) &lpDPlayLobby2A);
	if FAILED(hr)return false;
	DWORD Size=0;
	HRESULT dpres=lpDPlayLobby2A->GetConnectionSettings(0,NULL,&Size);
	if(!Size){
		lpDPlayLobby2A->Release();
		return false;
	};
	LPDPLCONNECTION cset=(LPDPLCONNECTION)malloc(Size);
	dpres=lpDPlayLobby2A->GetConnectionSettings(0,cset,&Size);
	if(dpres==DP_OK){
		LPDIRECTPLAY2A lpd2A;
		CreateMultiplaterInterface();
		if(lpDirectPlay3A){
			LoadFog(2);
			LoadPalette("2\\agew_1.pal");
			ShowLoading();
			//assert(0);
			char cc[128]="";
			if(cset->lpSessionDesc->lpszSessionNameA)
				strcpy(cc,cset->lpSessionDesc->lpszSessionNameA);
			cc[8]=0;
			bool BATTL=!strcmp(cc,"[BATTLE]");
			if(BATTL){
				cset->lpSessionDesc->dwMaxPlayers=2;
				cset->lpSessionDesc->dwUser2=1;
			}else{
				cset->lpSessionDesc->dwMaxPlayers=7;
				cset->lpSessionDesc->dwUser2=0;
			};
			dpres=lpDPlayLobby2A->Connect(0,&lpd2A,NULL);
			lpDPlayLobby2A->EnumAddress(&EnumAddressCallback1,cset->lpAddress,cset->dwAddressSize,NULL);

			if(dpres!=DP_OK){
				if(BATTL){
					if(cset->lpSessionDesc->dwFlags==DPLCONNECTION_CREATESESSION)ShowFailure(1);
					else ShowFailure(2);
				}else{
					//lpDPlayLobby2A->EnumAddress(&EnumAddressCallback1,cset->lpAddress,cset->dwAddressSize,NULL);
					ShowFailure(cset->lpSessionDesc->dwFlags==DPLCONNECTION_CREATESESSION);
				};
				lpDPlayLobby2A->Release();
				return false;
			};
			lpDPlayLobby2A->Release();
			dpres= lpd2A->QueryInterface(IID_IDirectPlay3A, (LPVOID*)&lpDirectPlay3A);
			if(dpres!=DP_OK)return false;
			ClearScreen();
			LoadFog(2);
			LoadPalette("2\\agew_1.pal");
			if(dpres==DP_OK){
				if(cset->dwFlags==DPLCONNECTION_CREATESESSION){
					DPL_CreatePlayer(lpDirectPlay3A,&cset->guidSP,cset->lpPlayerName,1);
					if(!BATTL){
						if(!MPL_WaitingGame(true,0))return false;
					}else{
						int BTLID=1;//ProcessWars();
						if(!MPL_WaitingBattleGame(true,1))return false;
					};
					StopConnectionToSession(lpDirectPlay3A);
					StartIGame(0);
				}else{
					DPL_CreatePlayer(lpDirectPlay3A,&cset->guidSP,cset->lpPlayerName,1);
					if(!BATTL){
						if(!MPL_WaitingGame(false,0))return false;
					}else{
						if(!MPL_WaitingBattleGame(false,1))return false;
					};
				};
				return 1;
			}else{
				return false;
			};
		}else{
			return false;
		};
	}else{
		return false;
	};
};
#endif
int prevVid=-1;
int prevVid1=-1;
int prevVid2=-1;
int prevVid3=-1;
int GetRndVid(int N){
	int dt=GetTickCount()&1023;
	for(int i=0;i<dt;i++)rand();
	int cv=-1;
	if(N>4){
		int na=0;
		do{
			int n=rand()%N;
			if(n!=prevVid&&n!=prevVid1&&n!=prevVid2&&n!=prevVid3)cv=n;
			na++;
		}while(cv==-1&&na<100);
	};
	if(cv==-1&&N>3){
		int na=0;
		do{
			int n=rand()%N;
			if(n!=prevVid&&n!=prevVid1&&n!=prevVid2)cv=n;
			na++;
		}while(cv==-1&&na<100);
	};
	if(cv==-1&&N>2){
		int na=0;
		do{
			int n=rand()%N;
			if(n!=prevVid&&n!=prevVid1)cv=n;
			na++;
		}while(cv==-1&&na<100);
	};
	if(cv==-1&&N>1){
		int na=0;
		do{
			int n=rand()%N;
			if(n!=prevVid)cv=n;
			na++;
		}while(cv==-1&&na<100);
	};
	if(cv==-1)cv=0;
	prevVid3=prevVid2;
	prevVid2=prevVid1;
	prevVid1=prevVid;
	prevVid=cv;
	return cv;
};
void PlayVideo();
//#ifdef MAKE_PTC

void AllGame() {
    if (!window_mode)
    {//Calculate offsets for centering menu in fullscreen mode
        menu_x_off = (screen_width - 1024) / 2;
        menu_y_off = (screen_height - 768) / 2;
    }
	if(CheckLobby()){
		EditMapMode=0;
		Lobby=1;
	}else{
		PlayVideo();
		ShowPreview();
	};
stgame:
	int mret;
	if(Lobby)mret=mcmSingle;
	else mret=processMainMenu();
	if(mret==mcmExit)return;
	if(mret==mcmSingle){
        if(EditMapMode){
            EditGame();
        }
		else PlayGame();
		UnLoading();
		goto stgame;
	}else goto stgame;
};
/*void AllGame()
{
    int menuChoice;
    if (!window_mode)
    {//Calculate offsets for centering menu in fullscreen mode
        menu_x_off = (screen_width - 1024) / 2;
        menu_y_off = (screen_height - 768) / 2;
    }
    do
    {
        menuChoice = processMainMenu();
        if (menuChoice == mcmSingle)
        {
            if (EditMapMode)
            {
                //Map editor loop
                EditGame();
            }
            else
            {
                //Game loop
                PlayGame();
            }
            //Zero variables and pointers
            UnLoading();
        }
    } while (mcmExit != menuChoice);
}*/

//#else
/*
void AllGame(){
	GFILE* f=Gopen("Demo\\demo.run","r");
	if(f){
		int N;
		int z=Gscanf(f,"%d",&N);
		if(z==1){
			int M=GetRndVid(N);
			char NAME[64];
			for(int i=0;i<M;i++)Gscanf(f,"%s",NAME);
			Gscanf(f,"%s",NAME);
			RGAME.ReadStream(NAME);
			if(RGAME.Size){
				PlayGameMode=1;
				ItemChoose=43;
				RetryVideo=1;
				KeyPressed=0;
			};
		};
		Gclose(f);
		PlayGame();
	};
};
*/
//#endif
extern int PLNAT[8];
void PrepareGameMedia(byte myid,bool);
void MakeGlobalReport();
void FreeArrays();
void SetupArrays();
void CreateTotalLocking();
void TestRoads();
void ResearchIslands();
void PrepareEditMedia(byte myid){
		FreeArrays();
		ADDSH=1;
		SetupArrays();
		rpos=0;
		setCoor();
		UnLoading();
		NewMap(240<<ADDSH,240<<ADDSH);
		SetMyNation(0);
		for(int i=0;i<8;i++){
			if(!PLNAT[i])
				LoadAllNations(1<<i,i);
			else LoadAllNations(1<<i,i);
			SetMyNation(myid);
		};
		ItemChoose=mcmSingle;
		//MultiplayerStart=true;
		InitExplosions();
		PrepareToEdit();
		ItemChoose=mcmSingle;
		InitGame();
		MakeGlobalReport();
		EraseAreas();
		rando();//!!
		CreateTotalLocking();
		TestRoads();
		ResearchIslands();
		if(InitDipForThisMap)InitDipForThisMap();
		for(int i=0;i<8;i++)
			for(int j=0;j<8;j++)SetXRESRC(i,j,1000000);
};
void RedrawGameBackground(){
	if(GameMode==1){//main menu
		//int mmlx=MainMenuGround.PicPtr[0];
		//int mmly=MainMenuGround.PicPtr[1];
		//MainMenuGround.Draw((RealLx-mmlx)>>1,(RSCRSizeY-mmly)>>1);
		FlipPages();
	}else{
		DrawAllScreen();		
	};
};

extern bool ProtectionMode;
int ProcessComplexQuestion(int Nx,char* Bmp1,byte or1,char* Text1,char* Bmp2,byte or2,char* Text2,char* Quest){
	int Lx=Nx<<6;
	DialogsSystem GMM((RealLx-Lx)>>1,80);
	SQPicture SPU;
	SPU.SetPixelData(NULL);
	LocalGP CPIC1;
	LocalGP CPIC2;
	int pid1=-1;
	int pid2=-1;
	char cc1[100];
	if(or1>2)or1=0;
	if(or2>2)or2=0;
	if(strcmp(Bmp1,"NONE")){
		sscanf(Bmp1,"%s%d",cc1,&pid1);
		if(pid1!=-1){
			CPIC1.Load(cc1);
		};
	};
	if(strcmp(Bmp2,"NONE")){
		sscanf(Bmp2,"%s%d",cc1,&pid2);
		if(pid2!=-1){
			CPIC2.Load(cc1);
		};
	};

	ResFile RF=RRewrite("tmp1.txt");
	RBlockWrite(RF,Text1,strlen(Text1));
	RClose(RF);
	RF=RRewrite("tmp2.txt");
	RBlockWrite(RF,Text2,strlen(Text2));
	RClose(RF);

	LocalGP LGP;
	LocalGP FONT("rom11");
	//LocalGP FONT1("rom11r");
	LocalGP FONTB("vinBig");
	LocalGP FONTRB("vinBig");

	RLCFont fntb(FONTB.GPID);
	RLCFont fntrb(FONTRB.GPID);
	RLCFont f16x16w(FONT.GPID);
	RLCFont f16x16g(FONT.GPID);
	f16x16w.SetRedColor();
	fntrb.SetRedColor();

	int y0=16;
	int LX1=0;
	int LY1=0;
	int x0=16;
	int plx=0;
	if(pid1!=-1){
		LX1=GPS.GetGPWidth(CPIC1.GPID,pid1);
		LY1=GPS.GetGPHeight(CPIC1.GPID,pid1)+16;
		if(or1==1){
			GMM.addGPPicture(NULL,x0,y0,CPIC1.GPID,pid1);
			x0+=LX1;
			plx=LX1;
		}else if(or1==2){
			GMM.addGPPicture(NULL,Lx-16-LX1,y0,CPIC1.GPID,pid1);
			plx=LX1;
		}else{
			GMM.addGPPicture(NULL,(Lx-LX1)>>1,y0,CPIC1.GPID,pid1);
			y0+=LY1+8;
		};
	};
	TextViewer* TV1;
	TextViewer* TV2;
	TV1=GMM.addTextViewer(NULL,x0,y0,Lx-32-plx,1000,"tmp1.txt",&f16x16w);
	TV1->Enabled=0;
	int dy=TV1->NLines*TV1->SymSize;
	if(or1)y0+=dy>LY1?dy:LY1;
	else y0+=dy;
	int LX2=0;
	int LY2=0;
	x0=16;
	plx=0;
	if(pid2!=-1){
		LX2=GPS.GetGPWidth(CPIC2.GPID,pid2);
		LY2=GPS.GetGPHeight(CPIC2.GPID,pid2)+16;
		if(or2==1){
			GMM.addGPPicture(NULL,x0,y0,CPIC2.GPID,pid2);
			x0+=LX2;
			plx=LX2;
		}else if(or2==2){
			GMM.addGPPicture(NULL,Lx-16-LX2,y0,CPIC2.GPID,pid2);
			plx=LX2;
		}else{
			GMM.addGPPicture(NULL,(Lx-LX1)>>1,y0,CPIC2.GPID,pid2);
			y0+=LY2+8;
		};
	}else y0+=18;;
	TV2=GMM.addTextViewer(NULL,x0,y0,Lx-32-plx,1000,"tmp2.txt",&f16x16g);
	TV2->Enabled=0;
	dy=TV2->NLines*TV2->SymSize;
	if(or1)y0+=dy>LY2?dy:LY2;
	else y0+=dy;
	int Ny=((y0+38)>>6)+1;
	int NTB=1;
	int LL=strlen(Quest);
	for(int j=0;j<LL;j++)if(Quest[j]=='|')NTB++;
	x0=Lx/2/NTB;
	int DX=Lx/NTB;
	char cc[128];
	int pos;
	for(int q=0;q<NTB;q++){
		pos=0;
		char c;
		do{
			c=Quest[0];
			if(c!='|'){
				cc[pos]=c;
				pos++;
				cc[pos]=0;
			};
			Quest++;
		}while(c!=0&&c!='|');
		TextButton* TB=GMM.addTextButton(NULL,x0+q*DX,Ny*64-33,GetTextByID(cc),&fntrb,&fntb,&fntb,1);
		TB->OnUserClick=&MMItemChoose;
		TB->UserParam=q;
		if(PlayGameMode){
			TB->Enabled=0;
			if(q==CurrentAnswer)TB->PassiveFont=&fntrb;
		};
	};
	int DDY=(RealLy-64*Ny)/2-80;
	GMM.BaseY+=DDY;
	for(int p=0;p<MAXDLG;p++){
		if(GMM.DSS[p]&&GMM.DSS[p]->Visible){
			GMM.DSS[p]->y+=DDY;
			GMM.DSS[p]->y1+=DDY;
		};
	};
	ItemChoose=-1;
	DrawPaperPanelShadow((RealLx-Lx)>>1,80+DDY-32,((RealLx-Lx)>>1)+Lx,80+DDY+Ny*64+20);
	int T0=GetTickCount();
	do{
		ProcessMessages();
		//if(KeyPressed&&(LastKey==13||LastKey==27)){
		//	KeyPressed=false;
		//	ItemChoose=mcmOk;
		//};
		DrawPaperPanel((RealLx-Lx)>>1,80+DDY-32,((RealLx-Lx)>>1)+Lx,80+DDY+Ny*64+20);
		GMM.MarkToDraw();
		GMM.ProcessDialogs();
		GMM.RefreshView();
		if(PlayGameMode&&GetTickCount()-T0>5000)ItemChoose=CurrentAnswer;
	}while(ItemChoose==-1);
	ContinueGame=true;
	UnPress();
	return ItemChoose;
};

bool AskMissionQuestion(char* Bmp,char* Text){
	
	int MPPLX=GETV("MPPLX");
	int MPPLY=GETV("MPPLY");
	int MPLDX=GETV("MPLDX");
	int MPRDX=GETV("MPRDX");
	int MPDDY=GETV("MPDDY");
	int MPUDY=GETV("MPDDY");

	/*
	//char* BmpName=GETS("@MISSPANEL");
	DialogsSystem GMM(smapx+(nsmaplx<<4)-(MPPLX>>1),smapy+(mul3(nsmaply)<<2)-(MPPLY>>1));
	//SQPicture SP(BmpName);
	
	SQPicture SPU;
	SPU.PicPtr=NULL;
	//if(strcmp(Bmp,"NONE")){
	//	SPU.LoadPicture(Bmp);
	//};

	ResFile RF=RRewrite("tmp.txt");
	RBlockWrite(RF,Text,strlen(Text));
	RClose(RF);
	LocalGP LGP;
	LocalGP FONT("rom11");
	LocalGP FONTB("vinBig");
	//LocalGP FONTRB("vinrBig");
	RLCFont f16x16w(FONT.GPID);//"rom11.rlc");
	RLCFont fntb(FONTB.GPID);
	RLCFont fntrb(FONTB.GPID);
	fntrb.SetRedColor();
	LocalGP CPIC;
	int pid=-1;
	char cc1[100];
	if(strcmp(Bmp,"NONE")){
		sscanf(Bmp,"%s%d",cc1,&pid);
		if(pid!=-1){
			CPIC.Load(cc1);
		};
	};	
	int DY=0;
	if(SPU.PicPtr){
		GMM.addPicture(NULL,0,0,&SPU,&SPU,&SPU);
		DY=SPU.GetLy();
	};
	int LX=0;
	int LY=0;
	if(pid!=-1){
		LX=GPS.GetGPWidth(CPIC.GPID,pid);
		LY=GPS.GetGPHeight(CPIC.GPID,pid);
		GMM.addGPPicture(NULL,(MPPLX-LX)>>1,MPUDY,CPIC.GPID,pid);
	};
	TextViewer* TV;
	TV=GMM.addTextViewer(NULL,MPLDX,MPUDY+LY,MPPLX-MPLDX-MPRDX,MPPLY-LY-MPDDY-MPUDY-40,"tmp.txt",&f16x16w);
	ItemChoose=-1;
	*/

	// Font
	RLCFont* FWindow = &fon16y5;
	RLCFont* FButton = &fon18y5;
	RLCFont* FWhite = &fon18w;
	RLCFont* FYellow = &fon18y3;
	RLCFont* FText = &fon18y3;
	RLCFont* FHint = &fon13y2;

	LocalGP MSG("interf2\\messages");
	LocalGP BTN("interf2\\button2");
	LocalGP SCROL("Interf2\\VScroll2");
	LocalGP LN("interf2\\line2");

	int WinID=1;

	int W=GPS.GetGPWidth(MSG.GPID,WinID*2);
	int H=GPS.GetGPHeight(MSG.GPID,WinID*2);
	int x0=(RealLx-W)/2;
	int y0=(RealLy-H)/2;
	
	int TitleY=34;

	int BX0=75;
	int BX1=307;
	int BY=521;

	int PY=75;

	int TX0=55;
	int TX1=520;
	int TY1=495;

	DialogsSystem GMM(x0,y0);

	GPS.ShowGP(x0,y0,MSG.GPID,WinID*2+1,0);
	GMM.addGPPicture(NULL,0,0,MSG.GPID,WinID*2);

	SQPicture SPU;
	SPU.SetPixelData(NULL);
	LocalGP CPIC;
	int pid=-1;
	char cc1[100];
	if(strcmp(Bmp,"NONE")){
		sscanf(Bmp,"%s%d",cc1,&pid);
		if(pid!=-1){
			CPIC.Load(cc1);
		};
	};

	ResFile RF=RRewrite("tmp.txt");
	RBlockWrite(RF,Text,strlen(Text));
	RClose(RF);
	LocalGP LGP;

	// title text
	GMM.addTextButton(NULL,W>>1,TitleY,GetTextByID("INTF_MESSAGE"),FWindow,FWindow,FWindow,1);

	// picture
	int LX=0;
	int LY=0;
	if(pid!=-1){
		LX=GPS.GetGPWidth(CPIC.GPID,pid);
		LY=GPS.GetGPHeight(CPIC.GPID,pid)+16;
		GMM.addGPPicture(NULL,(W-LX)>>1,PY,CPIC.GPID,pid);
	};
	
	// line delimiter
	int DW=GPS.GetGPWidth(LN.GPID,0);
	int DH=GPS.GetGPHeight(LN.GPID,0);
	GP_Button* Line=GMM.addGP_Button(NULL,(W-DW)>>1,PY+LY,LN.GPID,0,0);

	// message text	
	int SBW=GPS.GetGPWidth(SCROL.GPID,0);
	int TY0=PY+LY+10+DH;
	VScrollBar* SB=GMM.addNewGP_VScrollBar(NULL,TX1-SBW,TY0,TY1-TY0,1,0,SCROL.GPID,0);
	TextViewer* TV=GMM.addTextViewer(NULL,TX0,TY0,TX1-SBW-5-TX0,TY1-TY0,"tmp.txt",FText);
	TV->AssignScroll(SB);
	TV->SymSize=21;
	TV->PageSize=div(TY1-TY0,TV->SymSize).quot;
	//TV->

	// butons
	//TextButton* YesBtn=GMM.addTextButton(NULL,MPPLX/4,441,GetTextByID("@MSYES"),&fntrb,&fntb,&fntb,1);
	GP_TextButton* YesBtn=GMM.addGP_TextButton(NULL,BX0,BY,GetTextByID("@MSOK"),BTN.GPID,3,FButton,FButton);
	YesBtn->UserParam=mcmOk;
	YesBtn->OnUserClick=&MMItemChoose;
	if(PlayGameMode){
		//if(CurrentAnswer)YesBtn->PassiveFont=&fntrb;
		YesBtn->Enabled=1;
	};

	//TextButton* NoBtn=GMM.addTextButton(NULL,MPPLX*3/4,441,GetTextByID("@MSNO"),&fntrb,&fntb,&fntb,1);
	GP_TextButton* NoBtn=GMM.addGP_TextButton(NULL,BX1,BY,GetTextByID("@MSNO"),BTN.GPID,3,FButton,FButton);
	NoBtn->UserParam=mcmCancel;
	NoBtn->OnUserClick=&MMItemChoose;
	if(PlayGameMode){
		//if(!CurrentAnswer)NoBtn->PassiveFont=&fntrb;
		NoBtn->Enabled=1;
	};

	ItemChoose=-1;
	//DrawPaperPanelShadow(GMM.BaseX,GMM.BaseY,GMM.BaseX+MPPLX,GMM.BaseY+MPPLY);
	int T0=GetTickCount();
	do{
		//DrawPaperPanel(GMM.BaseX,GMM.BaseY,GMM.BaseX+MPPLX,GMM.BaseY+MPPLY);
		ProcessMessages();
		if(KeyPressed){
			if(LastKey==13)ItemChoose=mcmOk;
			else if(LastKey==27)ItemChoose=mcmCancel;
			KeyPressed=false;
		};
		GMM.MarkToDraw();
		GMM.ProcessDialogs();
		GMM.RefreshView();
		if(PlayGameMode&&GetTickCount()-T0>5000){
			ItemChoose=CurrentAnswer==0?mcmCancel:mcmOk;
		};
	}while(ItemChoose==-1);

	ContinueGame=true;
	UnPress();
	if(ItemChoose==mcmOk)return true;
	else return false;
};
//----------------------MiniMap editor interface------------------
SQPicture trans1u;
SQPicture trans1p;
SQPicture trans2u;
SQPicture trans2p;
SQPicture trans3u;
SQPicture trans3p;
SQPicture trans4u;
SQPicture trans4p;
SQPicture trans5u;
SQPicture trans5p;
SQPicture trans6u;
SQPicture trans6p;
SQPicture pen1u;
SQPicture pen1p;
SQPicture pen2u;
SQPicture pen2p;
SQPicture x480;
SQPicture getc_p;
SQPicture getc_u;
SQPicture btflu;
SQPicture btflu0;
SQPicture btrough;
SQPicture btrough0;
SQPicture btsoft;
SQPicture btsoft0;
SQPicture btless;
SQPicture btless0;
SQPicture prepu;
SQPicture prepp;
byte* higrad=NULL;
int grsize;
#ifdef MAKE_PTC
SFLB_DLLEXPORT SFLB_LoadMEDButtons(){
	
};
#endif
//-------------------MiniMap images----------------
int HiLx;
int HiLy;
int HiStepX;
int HiStepY;
int winX;
int winY;
int winX1;
int winY1;
byte* HiMap;
byte* HiMap1;

void SetupHiMap(){
	if(RealLx>=1024){
		HiLx=496;//248;
		HiLy=496;
	}else{
		HiLx=248;
		HiLy=248;
	};
	HiStepX=div(msx<<(8+5),HiLx).quot;
	HiStepY=div(msy<<(8+5),HiLy).quot;
	HiMap=znew(byte,HiLx*HiLy);
	HiMap1=znew(byte,HiLx*HiLy);
};
void DeleteHiMap(){
	free(HiMap);
	free(HiMap1);
};
byte GetPaletteColor(int r,int g,int b);
void CreateHiMap(){
	if(!higrad){
		grsize=24;
		higrad=znew(byte,grsize);
	};
	int mpx=256;
	int mpy=256;
	for(int iy=0;iy<HiLy;iy++){
		for(int ix=0;ix<HiLx;ix++){
			int Hig=GetHeight(mpx>>8,mpy>>8)>>4;
			if(Hig<0)Hig=0;
			if(Hig<grsize)Hig=higrad[Hig];else Hig=higrad[grsize-1];
			HiMap[ix+iy*HiLx]=Hig;
			mpx+=HiStepX;
		};
		mpx=256;
		mpy+=HiStepY;
	};
};
//----------------------LOADING THE FILE--------------------//
void CreateFilesList(char* Mask,ListBox* LB){
	WIN32_FIND_DATA FD;
	HANDLE H=FindFirstFile(Mask,&FD);
	if(H!=INVALID_HANDLE_VALUE){
		do{
			LB->AddItem(FD.cFileName,0);
		}while(FindNextFile(H,&FD));
		FindClose(H);
	};
};


//------------------TRANSFORMATIONS LIBRARY-----------------
int xc,yc;//center of the transformation
static int HiMode=0;
static int tpr1=256;//Height
static int tpr2=0;  //R1
static int tpr3=256;//R2
static int tpr4=512;//frquency
static int tpr5=512;//Amplitude
static int tpr6=512;//Phase
static int HiBrush=0;
static int HiRound=0;
int getR(double a,int r){
	int nn=tpr4>>6;
	double tm=double(GetRealTime())/1000;
	return int(r*((sin(a*(nn)+tpr6/256+tm)+
				   sin(a*(nn+1)+tpr6/256+tm*1.5)+
				   sin(a*(nn+2)+tpr6/256-tm/2)+
				   sin(a*(nn+3)+tpr6/256+tm*1.4)+
				   sin(a*(nn+4)+tpr6/256-tm*1.5)+
				   sin(a*(nn+5)+tpr6/256+tm*1.3)+
				   sin(a*(nn+6)+tpr6/256-tm*1.1)+
				   sin(a*(nn+7)+tpr6/256+tm*0.8)+
				   sin(a*(nn+8)+tpr6/256+tm*0.7)+
				   sin(a*(nn+9)+tpr6/256+tm*0.6)
				   )/10+
		+1+(tpr5>>6))/(2+(tpr5>>6)));
};
void Pix(int x,int y,byte c){
	if(x>=winX&&y>=winY&&x<winX1&&y<winY1){
		(HiMap1+x-winX+(y-winY)*HiLx)[0]=c;
		//((byte*)(int(ScreenPtr)+x+y*SCRSizeX))[0]=c;
		//((byte*)(int(RealScreenPtr)+x+y*RSCRSizeX))[0]=c;
	};
};
void Circ(int x,int y,int r,int n,byte c){
	r=(r*HiLx)/496;
	if(HiRound==1){
		for(int i=0;i<n;i++){
			double alpha=3.1415*2*i/n;
			int r2=getR(alpha,r);
			Pix(int(x+r2*sin(alpha)),int(y+r2*cos(alpha)),c);
		};
	}else{
		double time=double(GetRealTime())/1000;
		for(int i=0;i<n;i++){
			double alpha=3.1415*2*i/n;
			Pix(int(x+r*sin(alpha)),int(y+r*cos(alpha)),c);
		};
	};
};
int func1(int r){
	if(r<tpr2)return 1024;
	if(r>tpr2+tpr3)return 0;
	int dr=r-tpr2-(tpr3>>1);
	if(dr>0)return div((tpr2+tpr3-r)*(tpr2+tpr3-r)<<11,tpr3*tpr3).quot;
	else return 1024-div((r-tpr2)*(r-tpr2)<<11,tpr3*tpr3).quot;
};

int func2(int r){
	if(r<tpr2)return 1024;
	if(r>tpr2+tpr3)return 0;
	int dr=r-tpr2-tpr3;
	return div((dr*dr)<<10,tpr3*tpr3).quot;
};
int func3(int r){
	if(r<tpr2)return 1024;
	if(r>tpr2+tpr3)return 0;
	int dr=tpr2-r;
	return 1024-div((dr*dr)<<10,tpr3*tpr3).quot;
};
int func4(int r){
	if(r<tpr2)return 1024;
	if(r>tpr2+tpr3)return 0;
	int dr=r-tpr2;
	return div(dr<<10,tpr3).quot;
};
int func5(int r){
	if(r<tpr2)return 1024;
	if(r>tpr2+tpr3)return 0;
	int dr=r-tpr2-(tpr3>>1);
	if(dr>0)return div((tpr2+tpr3-r)*(tpr2+tpr3-r)<<11,tpr3*tpr3).quot;
	else return 1024-div((r-tpr2)*(r-tpr2)<<11,tpr3*tpr3).quot;
};
typedef int fpar(int);
int UniTrans(int x,int y,int Hi,fpar* ff1){
	if(!tpr3)tpr3=1;
	int pk;
	int r=int(sqrt((x-xc)*(x-xc)+(y-yc)*(y-yc)));
	if(HiRound==1){
		int tp2=tpr2;
		int tp3=tpr3;
		double ang=atan2(x-xc,y-yc);
		tpr2=getR(ang,tpr2);
		tpr3=getR(ang,tpr3);
		pk=ff1(r);
		tpr2=tp2;
		tpr3=tp3;
	}else pk=ff1(r);
	switch(HiMode){
	case 0:
		return Hi+(((tpr1-Hi)*pk)>>10);
	case 1:{
				int za=(GetHeight(x-32,y)+GetHeight(x+32,y)+
				   GetHeight(x,y-32)+GetHeight(x,y+32))>>2;
				return div(tpr1*za+(Hi<<9),512+tpr1).quot;
		   };
	case 2:
		return Hi+((tpr1*pk)>>15);
	case 3:
		pk=Hi-((tpr1*pk)>>15);
		if(pk<0)pk=0;
		return pk;
	default:
		return Hi;
	};
};
int Transf1(int x,int y,int Hi){
	return UniTrans(x,y,Hi,&func1);
};
int Transf2(int x,int y,int Hi){
	return UniTrans(x,y,Hi,&func2);
};
int Transf3(int x,int y,int Hi){
	return UniTrans(x,y,Hi,&func3);
};
int Transf4(int x,int y,int Hi){
	return UniTrans(x,y,Hi,&func4);
};
int Transf5(int x,int y,int Hi){
	return UniTrans(x,y,Hi,&func5);
};
//----------------------------------------------------------
void CopyToOffScreen(int zx,int zy,
					 int srLx,int srLy,
					 byte* data);
void CopyToRealScreen(int zx,int zy,
					 int srLx,int srLy,
					 byte* data);
void CreateMiniMap();
void ClearRender();
int mul3(int);
bool PrepareMap(SimpleDialog* SD){
	for(int x=0;x<VertInLine;x++){
		for(int y=MaxTH-2;y>0;y--){
			int vert=x+y*VertInLine;
			int hi0=(mul3(GetTriY(vert))>>2)-THMap[vert];
			int hi1=(mul3(GetTriY(vert+VertInLine))>>2)-THMap[vert+VertInLine];
			if(hi1-hi0<0)THMap[vert]+=hi0-hi1;
		};
	};
	ItemChoose=-2;
	return true;
};
bool RandomMap(SimpleDialog* SD){
	for(int x=0;x<VertInLine;x++){
		for(int y=MaxTH-2;y>0;y--){
			int vert=x+y*VertInLine;
			THMap[vert]+=((rando()&3)<<1)-3;
		};
	};
	ItemChoose=-2;
	return true;
};
bool LoadBitmapLikeGrayscale(char* Name,int* Lx,int* Ly,byte** res);
int GetResVal(byte* res,int LX,int LY,int RLX,int RLY,int x,int y);
bool LoadClick(SimpleDialog* SD){
	char BMPS[128];
	if(ProcessLoadingFile("UserBitmaps\\*.bmp",BMPS,3)){
		byte* DATA;
		char NAME[128];
		sprintf(NAME,"UserBitmaps\\%s",BMPS);
		int LX,LY;
		if(LoadBitmapLikeGrayscale(NAME,&LX,&LY,&DATA)){
			int maxx=7680<<ADDSH;
			int maxv=(MaxTH+1)*MaxTH;
			int h0=GetResVal(DATA,LX,LY,maxx,maxx,0,0);
			for(int i=0;i<maxv;i++){
				int x=GetTriX(i);
				int y=GetTriY(i);
				THMap[i]=GetResVal(DATA,LX,LY,maxx,maxx,x,y)-h0;
			};
		};
	};
	ItemChoose=-2;
	return true;
};
int MapGP=-1;
int ScrollGP=-1;
void ProcessTerrainEditor();
void EditMiniMap(){
	// Fonts
	RLCFont* FWin = &fon16y5;
	RLCFont* FAPad = &fon18w;
	RLCFont* FPPad = &fon18y3;
	RLCFont* FABut = &fon18y5;
	RLCFont* FPBut = &fon18y5;
	RLCFont* FText = &fon18y3;
	RLCFont* FWhite = &fon18w;
	RLCFont* FYellow = &fon18y3;
	RLCFont* FASmCmb = &fon16y1;
	RLCFont* FPSmCmb = &fon16y3;

	// Graphics
	LocalGP CMB("Interf2\\Combo2");
	LocalGP BTNS("Interf2\\EditRelief");

	SetupHiMap();
	CreateHiMap();

	int BTDX;
	int BTDY;
	int SCROLDX;
	int SCROLDY;
	int TLX;
	int TLY;
	int SID;
	int pdx;
	int pdy;
	int LOADDX;
	if(RealLx<1024){
		BTDX=0;
		BTDY=0;
		SCROLDX=0;
		SCROLDY=0;
		TLX=519;
		TLY=466;
		SID=0;
		pdx=4;
		pdy=3;
		LOADDX=0;
	}else{
		BTDX=121;
		BTDY=659-421+2;
		SCROLDX=540-301;
		SCROLDY=0;
		TLX=760;
		TLY=698;
		SID=47;
		pdx=0;
		pdy=0;
		LOADDX=656-415-1;
	};

	//Implementation
	short* VERHI=new short[(MaxTH+1)*MaxTH];
	memcpy(VERHI,THMap,(MaxTH+1)*MaxTH*2);
	
	int x=(RealLx-TLX)>>1;
	int y=(RealLy-TLY)>>1;
	DialogsSystem EMM(x,y);	
	
	//EMM.addBPXView(NULL,-x0,-y0,RealLx,RealLy,1,1,1,VERHI,NULL);

	EMM.addGPPicture(NULL,0,0,BTNS.GPID,0);	
	GPS.ShowGP(x,y,BTNS.GPID,1,0);

	GP_TextButton* PrepareBtn=EMM.addGP_TextButton(NULL,334,621,GetTextByID("INTF_SMOOTH_HILL"),BTNS.GPID,7,FABut,FPBut);
	PrepareBtn->OnUserClick=&PrepareMap;

	GP_TextButton* CancelBtn=EMM.addGP_TextButton(NULL,566,621,GetTextByID("INTF_CANCEL"),BTNS.GPID,7,FABut,FPBut);
	CancelBtn->OnUserClick=&MMItemChoose;
	CancelBtn->UserParam=mcmCancel;

	GP_TextButton* OkBtn=EMM.addGP_TextButton(NULL,102,621,GetTextByID("INTF_GENERATE"),BTNS.GPID,7,FABut,FPBut);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	
	GP_TextButton* LoadBtn=EMM.addGP_TextButton(NULL,607,567,GetTextByID("INTF_BITM_LOAD"),BTNS.GPID,33,FPSmCmb,FPSmCmb);
	LoadBtn->UserParam=mcmLoad;
	LoadBtn->OnUserClick=&MMItemChoose;	
	LoadBtn->FontDy-=1;

	int scrox=GETV("RESCRX");
	int scroy=GETV("RESCRY");

	int VSx0=591;
	int VSy0=115;
	int VSLx=194;
	int DYY=49;
	VScrollBar* HB1=EMM.addGP_ScrollBarL(NULL,VSx0,VSy0,1024,64,MapGP+1,4,VSLx,12,0,0);
	VScrollBar* HB2=EMM.addGP_ScrollBarL(NULL,VSx0,VSy0+DYY,1024,256,MapGP+1,4,VSLx,12,0,0);
	VScrollBar* HB3=EMM.addGP_ScrollBarL(NULL,VSx0,VSy0+DYY*2,1024,512,MapGP+1,4,VSLx,12,0,0);
	VScrollBar* HB4=EMM.addGP_ScrollBarL(NULL,VSx0,VSy0+DYY*3,1024,64,MapGP+1,4,VSLx,12,0,0);
	VScrollBar* HB5=EMM.addGP_ScrollBarL(NULL,VSx0,VSy0+DYY*4,1024,64,MapGP+1,4,VSLx,12,0,0);
	VScrollBar* HB6=EMM.addGP_ScrollBarL(NULL,VSx0,VSy0+DYY*5,1024,64,MapGP+1,4,VSLx,12,0,0);
	HB1->SPos=tpr1;
	HB2->SPos=tpr2;
	HB3->SPos=tpr3;
	HB4->SPos=tpr4;
	HB5->SPos=tpr5;
	HB6->SPos=tpr6;
	scrox=320-40;
	scroy=30;
	int BTX0=591;
	BTDX=31;
	int BTY0=440;	
	CheckBox* TRN1=EMM.addGP_CheckBox(NULL,BTX0,BTY0,"",&YellowFont,&YellowFont,1,0,BTNS.GPID,8,9,10);	
	CheckBox* TRN3=EMM.addGP_CheckBox(NULL,BTX0+BTDX,BTY0,"",&YellowFont,&YellowFont,1,0,BTNS.GPID,11,12,13);
	CheckBox* TRN4=EMM.addGP_CheckBox(NULL,BTX0+BTDX*2,BTY0,"",&YellowFont,&YellowFont,1,0,BTNS.GPID,14,15,16);	
	CheckBox* TRN5=EMM.addGP_CheckBox(NULL,BTX0+BTDX*3,BTY0,"",&YellowFont,&YellowFont,1,0,BTNS.GPID,17,18,19);	
	CheckBox* TRN6=EMM.addGP_CheckBox(NULL,BTX0+BTDX*4,BTY0,"",&YellowFont,&YellowFont,1,0,BTNS.GPID,20,21,22);	
	if(HiBrush==0)TRN1->State=1;
	if(HiBrush==1)TRN3->State=1;
	if(HiBrush==2)TRN4->State=1;
	if(HiBrush==3)TRN5->State=1;
	if(HiBrush==4)TRN6->State=1;
	
	int BTY1=BTY0+56;
	CheckBox* Absolut=EMM.addGP_CheckBox(NULL,BTX0,BTY1,"",&YellowFont,&YellowFont,2,0,BTNS.GPID,35,36,37);	
	CheckBox* Soft   =EMM.addGP_CheckBox(NULL,BTX0+BTDX,BTY1,"",&YellowFont,&YellowFont,2,0,BTNS.GPID,38,39,40);	
	CheckBox* More   =EMM.addGP_CheckBox(NULL,BTX0+BTDX*2,BTY1,"",&YellowFont,&YellowFont,2,0,BTNS.GPID,41,42,43);	
	CheckBox* Less   =EMM.addGP_CheckBox(NULL,BTX0+BTDX*3,BTY1,"",&YellowFont,&YellowFont,2,0,BTNS.GPID,44,45,46);	
	if(HiMode==0)Absolut->State=1;
	if(HiMode==1)Soft   ->State=1;
	if(HiMode==2)More   ->State=1;
	if(HiMode==3)Less   ->State=1;
	int BTX1=BTX0+138;
	CheckBox* PEN1=EMM.addGP_CheckBox(NULL,BTX1,BTY1,"",&YellowFont,&YellowFont,3,0,BTNS.GPID,23,24,25);	
	CheckBox* PEN2=EMM.addGP_CheckBox(NULL,BTX1+BTDX,BTY1,"",&YellowFont,&YellowFont,3,0,BTNS.GPID,26,27,28);	
	if(HiRound==0)PEN1->State=1;
	if(HiRound==1)PEN2->State=1;
	if(HiMode==0)Absolut->State=1;
	int BTX2=BTX0+169;
	CheckBox* GETCL=EMM.addGP_CheckBox(NULL,BTX2,BTY0,"",&YellowFont,&YellowFont,4,0,BTNS.GPID,29,30,31);
	GETCL->Transparent=false;
	int px0=65;
	int py0=88;
	BPXView* BV=EMM.addBPXView(NULL,px0,py0,HiLx,HiLy,1,1,1,HiMap1,NULL);
	BV->EnableSelection=false;
	
	// Text
	EMM.addTextButton(NULL,257+328/2,35,GetTextByID("INTF_EDIT_RELIEF"),FWin,FWin,FWin,1);
	int tx0=VSx0+5;
	int tdy=-31;
	int ty0=VSy0+tdy;
	EMM.addTextButton(NULL,tx0,ty0,GetTextByID("INTF_HEIGHT_EMI"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	EMM.addTextButton(NULL,tx0,ty0+DYY,GetTextByID("INTF_RADIUS_EMI"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	EMM.addTextButton(NULL,tx0,ty0+DYY*2,GetTextByID("INTF_WIDTH_CRO"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	EMM.addTextButton(NULL,tx0,ty0+DYY*3,GetTextByID("INTF_JAG_NU"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	EMM.addTextButton(NULL,tx0,ty0+DYY*4,GetTextByID("INTF_JAG_DE"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	EMM.addTextButton(NULL,tx0,ty0+DYY*5,GetTextByID("INTF_JAG_PH"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	tdy=-20;
	EMM.addTextButton(NULL,BTX0+5,BTY0+tdy,GetTextByID("INTF_JAG_TY"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	EMM.addTextButton(NULL,BTX2-30,BTY0+tdy,GetTextByID("INTF_HEIGHT"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	EMM.addTextButton(NULL,BTX0+5,BTY1+tdy,GetTextByID("INTF_PRESSURE_TYPE"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	EMM.addTextButton(NULL,BTX1+5,BTY1+tdy,GetTextByID("INTF_BRUSH"),FPSmCmb,FPSmCmb,FPSmCmb,0);
	
	ItemChoose=-1;
	winX=BV->x;
	winY=BV->y;
	winX1=BV->x1;
	winY1=BV->y1;
	do{
		tpr1=HB1->SPos;
		tpr2=HB2->SPos;
		tpr3=HB3->SPos;
		tpr4=HB4->SPos;
		tpr5=HB5->SPos;
		tpr6=HB6->SPos;
		if(Absolut->State)HiMode=0;
		if(Soft   ->State)HiMode=1;
		if(More   ->State)HiMode=2;
		if(Less   ->State)HiMode=3;
		if(PEN1->State)HiRound=0;
		if(PEN2->State)HiRound=1;
		if(TRN1->State)HiBrush=0;
		if(TRN3->State)HiBrush=1;
		if(TRN4->State)HiBrush=2;
		if(TRN5->State)HiBrush=3;
		if(TRN6->State)HiBrush=4;
		if(ItemChoose==-2){
			CreateHiMap();
			ItemChoose=-1;
		};
		if(BV->MouseOver&&Lpressed&&!GETCL->State){
			xc=int((mouseX-BV->x)*HiStepX)>>8;
			yc=int((mouseY-BV->y)*HiStepY)>>8;
			if(TRN1->State)
				CreateEffect(xc,yc,tpr2+tpr3,&Transf1);
			//if(TRN2->State)
			//	CreateEffect(xc,yc,tpr2+tpr3,&Transf2);
			if(TRN3->State)
				CreateEffect(xc,yc,tpr2+tpr3,&Transf2);
			if(TRN4->State)
				CreateEffect(xc,yc,tpr2+tpr3,&Transf3);
			if(TRN5->State)
				CreateEffect(xc,yc,tpr2+tpr3,&Transf4);
			if(TRN6->State)
				CreateEffect(xc,yc,tpr2+tpr3,&Transf5);
			if(!HiMode)Lpressed=false;
			CreateHiMap();
			//EMM.MarkToDraw();
			GETCL->State=0;
			GETCL->NeedToDraw=true;
		};
		if(BV->MouseOver&&GETCL->State){
			xc=int((mouseX-BV->x)*HiStepX)>>8;
			yc=int((mouseY-BV->y)*HiStepY)>>8;
			HB1->SPos=GetHeight(xc,yc);
			CreateHiMap();
			if(Lpressed)GETCL->State=0;
			Lpressed=false;
			//EMM.MarkToDraw();
		};
		memcpy(HiMap1,HiMap,HiLx*HiLy);
		Circ(mouseX,mouseY,tpr2>>5,tpr2>>2,255);
		Circ(mouseX,mouseY,(tpr2+tpr3)>>5,(tpr2+tpr3)>>2,255);
		ProcessMessages();
		EMM.MarkToDraw();
		EMM.ProcessDialogs();
		//CopyToRealScreen(EMM.BaseX+24,EMM.BaseY+24,HiLx,HiLy,HiMap);
		EMM.RefreshView();

		if(ItemChoose==mcmLoad){
			LoadClick(LoadBtn);
		}
	}while(ItemChoose<0);
	DeleteHiMap();
	if(ItemChoose==mcmCancel){
		memcpy(THMap,VERHI,(MaxTH+1)*MaxTH*2);
	};
	free(VERHI);
	ItemChoose=-1;
	CreateMiniMap();
	ClearRender();
	UnPress();
};
//---------------------------------------------------------------//
//-------------/    Choosing elements of interface     \---------//
//-------------\         trees,stones,....             /---------//
//---------------------------------------------------------------//
int InteriorType;
word TreeItems[256];
word NTreeItems;
extern int BlobMode;
void ChooseTree(){
	SQPicture PAD("trees.bpx");
	SQPicture scr_Line("Scr_10.bpx");
	SQPicture scr_L("Scr_L.bpx");
	SQPicture scr_R("Scr_R.bpx");
	SQPicture scr_U_G("Scr_U_G.bpx");
	RLCFont f24y("gf24y.rlc");
	RLCFont f24w("gf24w.rlc");
	int mmlx=PAD.GetLx();
	int mmly=PAD.GetLy();
	DialogsSystem GMM(smapx+(smaplx<<4)-(mmlx>>1),smapy+(mul3(smaply)<<2)-(mmly>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&PAD,&PAD,&PAD);
	int RT;
	if(!GPS.RLCImage[TREES.FileID]){
		GPS.LoadGP(TREES.FileID);
	};
	RT=TREES.FileID;
	RLCListBox* RB=GMM.addRLCListBox(PIC1,83,62,384,255,RT,0,254);
	//RLCListBox* RB=GMM.addRLCListBox(PIC1,83,62,384,127,RT,0,254);
	//RLCListBox* RB1=GMM.addRLCListBox(PIC1,83,62+128,384,127,RT,0,254);
	VScrollBar* SB=GMM.addHScrollBar(PIC1,65,328,RB->MaxXpos,0,&scr_L,&scr_L,&scr_L,&scr_R,&scr_R,&scr_R,&scr_Line,&scr_Line,&scr_U_G);
	TextButton* OkBtn=GMM.addTextButton(PIC1,181,379-3,"Äà",&f24w,&f24y,&f24y,1);
	TextButton* CancelBtn=GMM.addTextButton(PIC1,365-3,379-3,"Îòìåíà",&f24w,&f24y,&f24y,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	ItemChoose=-1;
	do{
		ProcessMessages();
		RB->XPos=SB->SPos;
		//RB1->XPos=SB->SPos+322;
		GMM.MarkToDraw();
		GMM.ProcessDialogs();
		//if(RB->Done){
		//	memset(RB1->Choosed,0,RB->NItems);
		//	RB->Done=false;
		//};
		//if(RB1->Done){
		//	memset(RB->Choosed,0,RB->NItems);
		//	RB1->Done=false;
		//};
		//for(int z=0;z<RB->NItems;z++){
		//	RB->Choosed[z]|=RB1->Choosed[z];
		//	RB1->Choosed[z]|=RB->Choosed[z];
		//};
		GMM.RefreshView();
	}while(ItemChoose==-1);
	NTreeItems=0;
	if(ItemChoose==mcmOk){
		for(int z=0;z<RB->NItems;z++)if(RB->Choosed[z]&&NTreeItems<256){
			TreeItems[NTreeItems]=z;
			NTreeItems++;
		};
		if(NTreeItems){
			HeightEditMode=2;
			MEditMode=false;
			BlobMode=-1;
			InteriorType=0;
		};
	};
	UnPress();
};
void ChooseStone(){
	SQPicture PAD("stones.bpx");
	SQPicture scr_Line("Scr_10.bpx");
	SQPicture scr_L("Scr_L.bpx");
	SQPicture scr_R("Scr_R.bpx");
	SQPicture scr_U_G("Scr_U_G.bpx");
	RLCFont f24y("gf24y.rlc");
	RLCFont f24w("gf24w.rlc");
	int mmlx=PAD.GetLx();
	int mmly=PAD.GetLy();
	DialogsSystem GMM(smapx+(smaplx<<4)-(mmlx>>1),smapy+(mul3(smaply)<<2)-(mmly>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&PAD,&PAD,&PAD);
	int RT;
	if(!GPS.RLCImage[STONES.FileID]){
		GPS.LoadGP(STONES.FileID);
	};
	RT=STONES.FileID;
	RLCListBox* RB=GMM.addRLCListBox(PIC1,83,62,384,127,RT,0,254);
	RLCListBox* RB1=GMM.addRLCListBox(PIC1,83,62+128,384,127,RT,0,254);
	VScrollBar* SB=GMM.addHScrollBar(PIC1,65,328,RB->MaxXpos,0,&scr_L,&scr_L,&scr_L,&scr_R,&scr_R,&scr_R,&scr_Line,&scr_Line,&scr_U_G);
	TextButton* OkBtn=GMM.addTextButton(PIC1,181,379-3,"Äà",&f24w,&f24y,&f24y,1);
	TextButton* CancelBtn=GMM.addTextButton(PIC1,365-3,379-3,"Îòìåíà",&f24w,&f24y,&f24y,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	ItemChoose=-1;
	do{
		ProcessMessages();
		RB->XPos=SB->SPos;
		RB1->XPos=SB->SPos+322;
		GMM.MarkToDraw();
		GMM.ProcessDialogs();
		if(RB->Done){
			memset(RB1->Choosed,0,RB->NItems);
			RB->Done=false;
		};
		if(RB1->Done){
			memset(RB->Choosed,0,RB->NItems);
			RB1->Done=false;
		};
		for(int z=0;z<RB->NItems;z++){
			RB->Choosed[z]|=RB1->Choosed[z];
			RB1->Choosed[z]|=RB->Choosed[z];
		};
		GMM.RefreshView();
	}while(ItemChoose==-1);
	NTreeItems=0;
	if(ItemChoose==mcmOk){
		for(int z=0;z<RB->NItems;z++)if(RB->Choosed[z]&&NTreeItems<256){
			TreeItems[NTreeItems]=z;
			NTreeItems++;
		};
		if(NTreeItems){
			HeightEditMode=2;
			MEditMode=false;
			BlobMode=-1;
			InteriorType=1;
		};
	};
	UnPress();
};
void ChooseHole(){
	SQPicture PAD("stones.bpx");
	SQPicture scr_Line("Scr_10.bpx");
	SQPicture scr_L("Scr_L.bpx");
	SQPicture scr_R("Scr_R.bpx");
	SQPicture scr_U_G("Scr_U_G.bpx");
	RLCFont f24y("gf24y.rlc");
	RLCFont f24w("gf24w.rlc");
	int mmlx=PAD.GetLx();
	int mmly=PAD.GetLy();
	DialogsSystem GMM(smapx+(smaplx<<4)-(mmlx>>1),smapy+(mul3(smaply)<<2)-(mmly>>1));
	Picture* PIC1=GMM.addPicture(NULL,0,0,&PAD,&PAD,&PAD);
	int RT;
	if(!GPS.RLCImage[HOLES.FileID]){
		GPS.LoadGP(HOLES.FileID);
	};
	RT=HOLES.FileID;
	RLCListBox* RB=GMM.addRLCListBox(PIC1,83,62,384,127,RT,0,254);
	RLCListBox* RB1=GMM.addRLCListBox(PIC1,83,62+128,384,127,RT,0,254);
	VScrollBar* SB=GMM.addHScrollBar(PIC1,65,328,RB->MaxXpos,0,&scr_L,&scr_L,&scr_L,&scr_R,&scr_R,&scr_R,&scr_Line,&scr_Line,&scr_U_G);
	TextButton* OkBtn=GMM.addTextButton(PIC1,181,379-3,"Äà",&f24w,&f24y,&f24y,1);
	TextButton* CancelBtn=GMM.addTextButton(PIC1,365-3,379-3,"Îòìåíà",&f24w,&f24y,&f24y,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	ItemChoose=-1;
	do{
		ProcessMessages();
		RB->XPos=SB->SPos;
		RB1->XPos=SB->SPos+322;
		GMM.MarkToDraw();
		GMM.ProcessDialogs();
		if(RB->Done){
			memset(RB1->Choosed,0,RB->NItems);
			RB->Done=false;
		};
		if(RB1->Done){
			memset(RB->Choosed,0,RB->NItems);
			RB1->Done=false;
		};
		for(int z=0;z<RB->NItems;z++){
			RB->Choosed[z]|=RB1->Choosed[z];
			RB1->Choosed[z]|=RB->Choosed[z];
		};
		GMM.RefreshView();
	}while(ItemChoose==-1);
	NTreeItems=0;
	if(ItemChoose==mcmOk){
		for(int z=0;z<RB->NItems;z++)if(RB->Choosed[z]&&NTreeItems<256){
			TreeItems[NTreeItems]=z;
			NTreeItems++;
		};
		if(NTreeItems){
			HeightEditMode=2;
			MEditMode=false;
			BlobMode=-1;
			InteriorType=2;
		};
	};
	UnPress();
};
void ChooseInterior(int Type){
	switch(Type){
	case 0:
		ChooseTree();
		return;
	case 1:
		ChooseStone();
		return;
	case 2:
		ChooseHole();
		return;
	};
};

int GetTreeItem(){
	return TreeItems[(rando()*NTreeItems)>>15];
};
//Multiplayer Interface

//-------------Multiplayer settings--------------

bool WaitingGame(bool Host){
	char STRI[128];
	SQPicture   MPL    ("multi_m0.bpx");
	InputBox*   MNAME  [8];
	ComboBox*   MNATION[8];
	ColoredBar* MCOLOR [8];
	TextButton* MGRP   [8];
	RLCFont     FontA(GETS("@MNMENUACTF"));
	RLCFont     FontP(GETS("@MNMENUPASF"));
#ifdef SCREENFIX
	DialogsSystem MPLAY(menu_x_off, menu_y_off);
#else
    DialogsSystem MPLAY(0, 0);
#endif
	MPLAY.addPicture(NULL,0,0,&MPL,&MPL,&MPL);
	char NAMY[16];
	int NameX=GETV("MINAMEX");
	int NatX=GETV("MINATX");
	int ClrX=GETV("MICLRX");
	int CmdX=GETV("MIGRPX");
	int NameLx=GETV("MINAMELX");
	int StrLy=GETV("MINAMELY");
	int ClrLx=GETV("MICLRLX");
	int NatLx=GETV("MINATLX");
	InputBox* IBB1=MPLAY.addInputBox(NULL,0,0,STRI,64,300,32,&FontP,&FontP);
	LoadPalette("2w.pal");
	for(int i=0;i<8;i++){
		sprintf(NAMY,"MIY%d",i+1);
		int y1=GETV(NAMY);
		MNAME[i]=MPLAY.addInputBox(NULL,NameX,y1,PINFO[i].name,16,NameLx,StrLy,&FontP,&FontA);
		MNATION[i]=MPLAY.addComboBox(NULL,NatX+8,y1+3,NatLx-16,StrLy-3,20,0,3,&FontA,&FontP,NULL);
		for(int j=0;j<GlobalAI.NAi;j++){
			MNATION[i]->AddLine(GlobalAI.Ai[j].Message);
		};
		MNATION[i]->FontDx=4;
		MNATION[i]->FontDy=2;
		MCOLOR[i]=MPLAY.addColoredBar(ClrX,y1,ClrLx,StrLy,0x80);
		MCOLOR[i]->OnMouseOver=&ColorMouseOver;
		sprintf(NAMY,"%d",i);
		MGRP[i]=MPLAY.addTextButton(NULL,CmdX,y1,NAMY,&FontA,&FontP,&FontP,0);
		

	};
	TextButton* OkBtn;
	if(Host)OkBtn=MPLAY.addTextButton(NULL,GETV("MIOKX"),GETV("MIOKY"),GETS("@MIHOSTOK"),&FontA,&FontP,&FontP,1);
	else OkBtn=MPLAY.addTextButton(NULL,GETV("MIOKX"),GETV("MIOKY"),GETS("@MIJOINOK"),&FontA,&FontP,&FontP,1);
	TextButton* CancelBtn=MPLAY.addTextButton(NULL,GETV("MICANCELX"),GETV("MICANCELY"),GETS("@MICANCEL"),&FontA,&FontP,&FontP,1);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;
	ItemChoose=-1;
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	PIEnumeratePlayers(PINFO,0);
	byte CUSED=0;
	for(int i=0;i<NPlayers;i++){
		if(PINFO[i].PlayerID!=MyDPID){
			CUSED|=(1<<PINFO[i].ColorID);
		};
	};
	int cc1=0;
	byte mss1=1;
	while(CUSED&mss1){
		cc1++;
		mss1<<=1;
	};
	for(int i=0;i<NPlayers;i++){
		if(PINFO[i].PlayerID==MyDPID){
			PINFO[i].ColorID=cc1;
			MCOLOR[i]->color=cc1+0x80;
			if(Host)PINFO[i].Ready=1;
			else PINFO[i].Ready=0;
		}else{
			PINFO[i].Ready=0;
		};
	};
	do{
		tmm1=GetRealTime();
		ProcessMessages();
		tmm1=GetRealTime()-tmm1;
		tmm2=GetRealTime();
		AnalyseMessages();
		tmm2=GetRealTime()-tmm2;
		MPLAY.MarkToDraw();
		MPLAY.ProcessDialogs();
		tmm3=GetRealTime();
		MPLAY.RefreshView();
		tmm3=GetRealTime()-tmm3;
		sprintf(STRI,"%d %d %d %d",COUN,tmm1,tmm2,tmm3);
		for(int i=0;i<8;i++){
			if(i<NPlayers){
				if(PINFO[i].PlayerID==MyDPID){
					PINFO[i].NationID=MNATION[i]->CurLine;
					PINFO[i].ColorID=MCOLOR[i]->color-0x80;
					lpDirectPlay3A->SetPlayerData(MyDPID,(void*)&PINFO[i].NationID,4,DPSET_REMOTE);
					DPNAME dpName;
					ZeroMemory(&dpName, sizeof(DPNAME));
					dpName.dwSize = sizeof(DPNAME);
					dpName.lpszShortNameA = MNAME[i]->Str;
					dpName.lpszLongNameA = NULL;
					lpDirectPlay3A->SetPlayerName(MyDPID,&dpName,DPSET_REMOTE);
					MNATION[i]->Enabled=true;
					MCOLOR[i]->Enabled=true;
					MGRP[i]->Enabled=true;
				}else{
					MNATION[i]->Enabled=false;
					MCOLOR[i]->Enabled=false;
					MGRP[i]->Enabled=false;
				};
				MNATION[i]->Visible=true;
				MCOLOR[i]->Visible=true;
				MGRP[i]->Visible=true;
			}else{
				MNATION[i]->Enabled=false;
				MCOLOR[i]->Enabled=false;
				MGRP[i]->Enabled=false;
				MNATION[i]->Visible=false;
				MCOLOR[i]->Visible=false;
				MGRP[i]->Visible=false;
			};
		};
		PIEnumeratePlayers(PINFO,0);
		for(int i=0;i<8;i++){
			if(i<NPlayers){
				MNATION[i]->CurLine=PINFO[i].NationID;
				MCOLOR[i]->color=PINFO[i].ColorID+0x80;
			};
		};
		for(int i=0;i<NPlayers;i++){
			if(PINFO[i].PlayerID==MyDPID&&ItemChoose==mcmOk&&!Host){
				PINFO[i].Ready=1;
			};
		};
		bool GMREADY=true;
		for(int i=0;i<NPlayers;i++){
			if(PINFO[i].PlayerID!=MyDPID&&!PINFO[i].Ready)GMREADY=false;
		};
		int MSS1=0;
		for(int i=0;i<NPlayers;i++){
			byte MSS2=1<<PINFO[i].ColorID;
			if(MSS1&MSS2)GMREADY=false;
			MSS1|=MSS2;
			if(!PINFO[i].Ready)GMREADY=false;
		};
		if(Host){
			OkBtn->Enabled=GMREADY;
		};
		//checking readyness
		if(ItemChoose==mcmOk&&!Host)ItemChoose=-1;
	}while(ItemChoose==-1&&PlayerMenuMode==1);
	return true;
};
bool WaitingHostGame(int ID){
	if(ID)MPL_WaitingBattleGame(true,ID-1);
	else MPL_WaitingGame(true,0);
	
	if(ItemChoose==mcmCancel){
		CloseMPL();
		ItemChoose=-1;
	}else{
		if(ItemChoose==mcmOk){
			//ShowLoading();
			StopConnectionToSession(lpDirectPlay3A);
			StartIGame(0);
			return true;
		};
	};
	return false;
};
bool WaitingJoinGame(int ID){
	PlayerMenuMode=1;
	if(ID)MPL_WaitingBattleGame(false,ID-1);
	else MPL_WaitingGame(false,0);
	if(ItemChoose==mcmCancel){
		CloseMPL();
		ItemChoose=-1;
	}else{
		//ShowLoading();
		if(!PlayerMenuMode)return true;
	};
	return false;
};
extern bool NotInGame;
void CenterScreen();
extern bool NoWinner;
void CreateInfoMap();
extern word COMPSTART[8];
DLLEXPORT
void StartAI(byte Nat,char* Name,int Land,int Money,int ResOnMap,int Difficulty);
extern int RM_LandType;
extern int RM_Resstart;
extern int RM_Restot;
void FreeArrays();
void SetupArrays();
extern byte* RivDir;
extern byte XVIIIState;
void PerformNewUpgrade(Nation* NT,int UIndex,OneObject* OB);
extern int RunDataSize;
extern byte RunData[2048];
void StartAIEx(byte Nat,char* Name,int Land,int Money,int ResOnMap,int Difficulty);
void CleanRec();
void LoadMessagesFromFile(char* Name);
extern byte BaloonState;
extern byte CannonState;
extern byte XVIIIState;
extern byte CaptState;
extern byte SaveState;
void GetOptionsFromMap(char* Name){
	if(Name[0]=='R'&&Name[1]=='N'&&Name[3]==' '){
		int v1, v2, v3, ADD_PARAM;
		char ccc[32];
		int z=sscanf(Name,"%s%x%x%x%d",ccc,&v1,&v2,&v3,&ADD_PARAM);
		if(z==5){
			BaloonState=(ADD_PARAM/10)%10;
			CannonState=(ADD_PARAM/100)%10;
			XVIIIState=(ADD_PARAM/10000)%10;
			CaptState=(ADD_PARAM/100000)%10;
			SaveState=(ADD_PARAM/1000000)%10;
		};
        /*int options = 0;
        int z = sscanf(Name, "%*s %*s %*s %*s %d", &options);//BUGFIX: proper parsing
        if (1 == z)
        {
            //Decode 7-digit number into game settings
            int option_values[11] = { 0 };

            DecodeOptionsFromNumber(options, option_values);

            BaloonState = option_values[0];
            CannonState = option_values[1];
            XVIIIState = option_values[2];
            CaptState = option_values[3];
            SaveState = option_values[4];
        }*/
	};
};
extern char AI_Log[256];
extern char CurrentGameLogDir[256];
void WriteAI_Comment(char* name,int idx,char* mask,...){
	if(AI_Log[0]&&CurrentGameLogDir[0]){
		char cc2[512];
		sprintf(cc2,"%s\\%s%d.dat",CurrentGameLogDir,name,idx);
		FILE* F=fopen(cc2,"a");
		if(F){
			va_list va;
			va_start(va,mask);
			vfprintf(F,mask,va);
			va_end(va);
			fclose(F);
		};
	};
};
void WriteResources(char* name,int idx,int* res){
	WriteAI_Comment(name,idx,"W:%7d G:%7d S:%7d F:%7d I:%7d C:%7d",res[0],res[1],res[2],res[3],res[4],res[5]);
};
void WriteResources(char* name,int idx,word* res){
	WriteAI_Comment(name,idx,"W:%7d G:%7d S:%7d F:%7d I:%7d C:%7d",res[0],res[1],res[2],res[3],res[4],res[5]);
};
void WriteUpgradeAction(byte NI,NewUpgrade* NU){
	WriteAI_Comment("",NI,"%6d: UPGRADE %s (%s)\n",tmtmt,NU->Name,NU->Message);
	WriteAI_Comment("",NI,"COST  :");
	WriteResources("",NI,NU->Cost);
	WriteAI_Comment("",NI,"\nTOTAL :");
	int RS[6];
	for(int j=0;j<6;j++)RS[j]=XRESRC(NI,j);
	WriteResources("",NI,RS);
	WriteAI_Comment("",NI,"\nPERCENT:");
	for(int j=0;j<6;j++)if(RS[j])RS[j]=NU->Cost[j]*100/RS[j];
	WriteResources("",NI,RS);
	WriteAI_Comment("",NI,"\n\n");
};
void WriteUnitAction(byte NI,word NIndex,int* R0,int* R1){
	GeneralObject* GO=NATIONS[NI].Mon[NIndex];
	NewMonster* NM=GO->newMons;
	int NU=CITY[NI].UnitAmount[NIndex];
	int CC[6];
	for(int i=0;i<6;i++)CC[i]=R1[i]-R0[i];
	if(NM->Building){
		WriteAI_Comment("",NI,"%6d: BUILDING %s [now:%d] (%s)\n",tmtmt,GO->MonsterID,NU,GO->Message);
		WriteAI_Comment("",NI,"COST   :");
		WriteResources("",NI,CC);
		WriteAI_Comment("",NI,"\nTOTAL  :");
		int RS[6];
		//for(int j=0;j<6;j++)RS[j]=XRESRC(NI,j);
		WriteResources("",NI,R0);
		WriteAI_Comment("",NI,"\nPERCENT:");
		for(int j=0;j<6;j++)if(R0[j])RS[j]=CC[j]*100/R0[j];
		WriteResources("",NI,RS);
		WriteAI_Comment("",NI,"\n\n");
	}else if(NU%10==0){
		WriteAI_Comment("",NI,"%6d: UNITS_GROW %s [now:%d] (%s)\n",tmtmt,GO->MonsterID,NU,GO->Message);
		WriteAI_Comment("",NI,"COST  :");
		WriteResources("",NI,CC);
		WriteAI_Comment("",NI,"\nTOTAL  :");
		int RS[6];
		//for(int j=0;j<6;j++)RS[j]=XRESRC(NI,j);
		WriteResources("",NI,R0);
		WriteAI_Comment("",NI,"\nPERCENT:");
		for(int j=0;j<6;j++)if(R0[j])RS[j]=CC[j]*100/R0[j];
		WriteResources("",NI,RS);
		WriteAI_Comment("",NI,"\n\n");
	};
};
/*
void WriteGameSnapshot(byte NI){
	int TotPS=0;
	int PS_MineG=0;
	int PS_MineI=0;
	int PS_MineC=0;
	int PS_MineS=0;
	int PS_Field=0;
	int PS_Build=0;
	int PS_Trees=0;
	int PS_Idle=0;
};*/
CEXPORT void ActivateDipDialog(char* request);
void PrepareGameMedia(byte myid,bool SaveNR,bool DisableDLL){
	RandomMapDesc RMP;
	RMP.Load("Terrain\\Res.dat");
		rpos=0;
		setCoor();
		//FreeArrays();
		if(!(strstr(CurrentMap,".SAV")||strstr(CurrentMap,".sav"))){
			if(CurrentMap[0]=='R'&&CurrentMap[1]=='N'&&CurrentMap[3]==' '){
				ADDSH=CurrentMap[2]-'0'+1;
			}else{
			};
		}else{
			NoWinner=1;
		};
		SetupArrays();
		NewMap(240<<ADDSH,240<<ADDSH);
		byte NRTB[8];
		memcpy(NRTB,NatRefTBL,8);
		UnLoading();
		if(SaveNR)memcpy(NatRefTBL,NRTB,8);
		
		SetMyNation(0);
		SetMyNation(PINFO[myid].ColorID);
		for(int i=0;i<8;i++){
			//Loading nation
			LoadAllNations(1<<i/*PINFO[i].ColorID*/,i/*PINFO[i].ColorID*/);
		};
		PrepareToGame();
		int RD_VERSION=101;
		//RUN DATA
		RunMethod=0xFFFFFFFE;
		RunData[0]=101;//version of the run data
		RunData[1]=byte(dwVersion);//version of the game
		RunData[2]=MyNation;
		RunDataSize=3;
		memcpy(RunData+RunDataSize,PINFO,sizeof PINFO);
		RunDataSize+=sizeof PINFO;
		memcpy(RunData+RunDataSize,COMPSTART,sizeof COMPSTART);
		RunDataSize+=sizeof COMPSTART;
		memcpy(RunData+RunDataSize,&RM_LandType,4);
		RunDataSize+=4;
		memcpy(RunData+RunDataSize,&RM_Resstart,4);
		RunDataSize+=4;
		memcpy(RunData+RunDataSize,&RM_Restot,4);
		RunDataSize+=4;
		memcpy(RunData+RunDataSize,NRTB,8);
		RunDataSize+=8;
		assert(RunDataSize<2048);
		//----------
		//Aliances
			for(int i=NPlayers;i<7;i++){
				if(COMPSTART[i]){
					word W=COMPSTART[i];
					int Nat=W>>11;
					int Color=W&15;
					int Diff=(W>>8)&7;
					int Team=(W>>4)&15;
					PINFO[i].ColorID=Color;
					PINFO[i].NationID=Nat;
					PINFO[i].GroupID=Team;
					PINFO[i].MapStyle=Diff;
					switch(Diff){
					case 1:
						strcpy(PINFO[i].name,"Easy");
						break;
					case 2:
						strcpy(PINFO[i].name,"Normal");
						break;
					case 3:
						strcpy(PINFO[i].name,"Hard");
						break;
					case 4:
						strcpy(PINFO[i].name,"Very hard");
						break;
					};
				}else{
					PINFO[i].MapStyle=0;
				};
			};

			for(int i=0;i<7;i++)if(PINFO[i].name[0]||i<NPlayers||PINFO[i].MapStyle){
				if(PINFO[i].GroupID==0)NATIONS[NatRefTBL[PINFO[i].ColorID]].NMask=1<<NatRefTBL[PINFO[i].ColorID];
				else{
					int gid=PINFO[i].GroupID;
					byte mas=0;
					for(int j=0;j<7;j++)if(PINFO[j].name[0]||j<NPlayers||PINFO[j].MapStyle){
						if(PINFO[j].GroupID==gid)mas|=1<<NatRefTBL[PINFO[j].ColorID];
					};
					NATIONS[NatRefTBL[PINFO[i].ColorID]].NMask=mas;
				};
			};
//#ifndef SINGLETESTNETWORK
		//};
//#endif
		CheapMode=false;
		ItemChoose=mcmSingle;
		MultiplayerStart=true;
		InitExplosions();
		InitGame();
		NotInGame=true;
		if(!(strstr(CurrentMap,".SAV")||strstr(CurrentMap,".sav"))){
			byte RRR[8];
			memcpy(RRR,NatRefTBL,8);
			byte NMASKS[8];
			for(int i=0;i<8;i++)NMASKS[i]=NATIONS[i].NMask;
			Load3DMap(CurrentMap);
			GetOptionsFromMap(CurrentMap);
			for(int i=0;i<8;i++)NATIONS[i].NMask=NMASKS[i];
			if(MapScenaryDLL[0]){
				if(!DisableDLL){
					char ccc[128];
					char cc3[128];
					strcpy(ccc,MapScenaryDLL);
					char* cc1=strstr(ccc,".");
					if(cc1)strcpy(cc1,".txt");
					strcpy(cc3,MapScenaryDLL);
					cc1=strstr(cc3,".");
					if(cc1)strcpy(cc1,".str");
					FILE* F=fopen(cc1,"r");
					if(F){
						fclose(F);
						LoadMessagesFromFile(cc3);
					};
					SCENINF.Load(MapScenaryDLL,ccc);//"Battles\\Battles.txt");
				};
			}else{
				for(int i=NPlayers;i<7;i++){
					if(COMPSTART[i]){
						int Diff=PINFO[i].MapStyle;
						int c=PINFO[i].ColorID;
						int Nat=-1;
						for(int j=0;j<MAXOBJECT;j++){
							OneObject* OB=Group[j];
							if(OB&&OB->NNUM==c&&(!OB->Sdoxlo)&&OB->newMons->Peasant){
								Nat=OB->Ref.General->NatID;
								j=MAXOBJECT+1;
							};
						};
						if(Nat!=-1){
							char ccx[48];
							char* NtID=NatsIDS[Nat];
							sprintf(ccx,"%s.%d",NtID,0);
							StartAIEx(PINFO[i].ColorID,ccx,RMP.STY[RM_LandType].AI_Style,RM_Resstart,RM_Restot,Diff-1);
						};
					};
				};
			};
			memcpy(NatRefTBL,RRR,8);
		}else{
			NoWinner=1;
		};
		GNFO.Clear();
		GNFO.Setup();
//#ifdef SINGLETESTNETWORK
		//running AI
		
		/*
#else
		if(NPlayers>1){
			//running AI
			for(int i=NPlayers;i<7;i++){
				if(COMPSTART[i]){
					int Diff=PINFO[i].MapStyle;
					int Nat=PINFO[i].NationID;
					char ccx[48];
					char* NtID=GlobalAI.Ai[Nat].NationID;
					sprintf(ccx,"%s.%d",NtID,0);
					StartAI(PINFO[i].ColorID,ccx,RM_LandType,RM_Resstart,RM_Restot,Diff-1);
				};
			};
		};
#endif
		*/
		HideFlags();
		NotInGame=false;
		for(int q=0;q<8;q++){
			for(int p=0;p<8;p++){
				SetXRESRC(q,p,RES[q][p]);
			};
		};
		CenterScreen();
		CreateInfoMap();
		InitExplosions();
		for(int i=0;i<8;i++){
			CITY[i].EnumUnits();
			CITY[i].RefreshAbility();
		};
		if(XVIIIState==2){
			int N=NATIONS[0].NUpgrades;
			for(int i=0;i<N;i++){
				for(int NI=0;NI<7;NI++){
					NewUpgrade* NU=NATIONS[NI].UPGRADE[i];
					if((!NU->Done)&&NU->Options&8){
						PerformNewUpgrade(NATIONS+NI,i,NULL);
						NU->Done=1;
						NU->PermanentEnabled=0;
					};
				};
			};
		};
		for(int i=0;i<MAXOBJECT;i++){
			OneObject* OB=Group[i];
			if(OB){
				OB->NMask=NATIONS[OB->NNUM].NMask;
			};
		};
		CleanRec();
		if(InitDipForThisMap)InitDipForThisMap();
		if(AI_Log[0]){
			SYSTEMTIME ST;
			GetLocalTime(&ST);
			char CNAME[256]="";
			DWORD sz=255;
			GetComputerName(CNAME,&sz);
			sprintf(CurrentGameLogDir,"%s%s_%d.%d.%d_%d_%d",AI_Log,CNAME,ST.wDay,ST.wMonth,ST.wYear,ST.wHour,ST.wMinute);
			if(!CreateDirectory(CurrentGameLogDir,0)){
				CurrentGameLogDir[0]=0;
			}else{

			};
		}else{
			CurrentGameLogDir[0]=0;
		};
		WriteAI_Comment("strart_conditions",0,"map            :%s\n",PINFO[0].MapName);
		WriteAI_Comment("strart_conditions",0,"Land type      :%d\n",PINFO[0].MapStyle);
		WriteAI_Comment("strart_conditions",0,"Climatic zone  :%d\n",PINFO[0].MapStyle);
		WriteAI_Comment("strart_conditions",0,"Start resources:");
		WriteResources("strart_conditions",0,RES[0]);
		WriteAI_Comment("strart_conditions",0,"\n");
		WriteAI_Comment("strart_conditions",0,"mines          :%d\n",PINFO[0].ResOnMap);
		for(int i=0;i<7;i++)if(i<NPlayers){
			int nid=PINFO[i].NationID;
			if(nid>=0&&nid<GlobalAI.NAi){
				WriteAI_Comment("strart_conditions",0,"PLAYER: %3d %12.12s %12.12s\n",PINFO[i].ColorID,PINFO[i].name,GlobalAI.Ai[nid].NationID);
			};
		}else if(NATIONS[i].AI_Enabled){
			int nid=PINFO[i].NationID;
			if(nid>=0&&nid<GlobalAI.NAi){
				WriteAI_Comment("strart_conditions",0,"AI    : %3d %12.12s %12.12s\n",PINFO[i].ColorID,PINFO[i].name,GlobalAI.Ai[nid].NationID);
			};
		};
		//diplomasy starting
		void HideDipDialog();
		if(CurrentSinBattle==-1)ActivateDipDialog("DS|cnlist");
		HideDipDialog();

};
void PrepareGameMedia(byte myid,bool SaveNR){
	PrepareGameMedia(myid,SaveNR,0);
};
//----------------------RANDOM MAP GENERATING DIALOG------------------
void RandomMapDialog(char* Result){
	DialogsSystem DSY((RealLx-352)>>1,(RealLy-352)>>1);
	RLCFont FONT1("rf32y.rlc");
	RLCFont FONT2(GETS("@SMPASSIVE"));
	RLCFont FONT3(GETS("@SMACTIVE"));
	DSY.addTextButton(NULL,352>>1,20,GETS("@RMIHEADER"),&FONT1,&FONT1,&FONT1,1);
	int y=60;
	int x0=20;
	int x=195;
	RandomMapDesc RMP;
	RMP.Load("Terrain\\Res.dat");
	DSY.addTextButton(NULL,x0,y,GETS("@RMISTYLE"),&FONT2,&FONT2,&FONT2,0);
	ComboBox* CBSTYLE=DSY.addComboBox(NULL,x,y,144,25,20,0,255,&FONT2,&FONT3,NULL);
	for(int p=0;p<RMP.NSTY;p++){
		CBSTYLE->AddLine(RMP.STY[p].Name);
	};
	y+=40;
	DSY.addTextButton(NULL,x0,y,GETS("@RMIMOUNTAINS"),&FONT2,&FONT2,&FONT2,0);
	ComboBox* CBMOUNT=DSY.addComboBox(NULL,x,y,144,25,20,0,255,&FONT2,&FONT3,NULL);
	for(int p=0;p<RMP.NRelief;p++){
		CBMOUNT->AddLine(RMP.Relief[p]);
	};
	y+=40;
	int CurSty=0;
	DSY.addTextButton(NULL,x0,y,GETS("@RMIPLAYERS"),&FONT2,&FONT2,&FONT2,0);
	ComboBox* CBPLAY=DSY.addComboBox(NULL,x,y,144,25,20,0,255,&FONT2,&FONT3,NULL);
	for(int p=0;p<RMP.STY[CurSty].NPl;p++){
		char ccc[16];
		sprintf_s(ccc,sizeof(ccc)," %d",RMP.STY[CurSty].Players[p].NPlayers);
		CBPLAY->AddLine(ccc);
	};
	y+=40;
	DSY.addTextButton(NULL,x0,y,GETS("@RMIRESSTART"),&FONT2,&FONT2,&FONT2,0);
	ComboBox* CBRESST=DSY.addComboBox(NULL,x,y,144,25,20,0,255,&FONT2,&FONT3,NULL);
	for(int p=0;p<RMP.NRES;p++){
		CBRESST->AddLine(RMP.RES[p].Name);
	};
	y+=40;
	DSY.addTextButton(NULL,x0,y,GETS("@RMIRESTOT"),&FONT2,&FONT2,&FONT2,0);
	ComboBox* CBRESTOT=DSY.addComboBox(NULL,x,y,144,25,20,0,255,&FONT2,&FONT3,NULL);
	for(int p=0;p<RMP.NMINES;p++){
		CBRESTOT->AddLine(RMP.MINES[p].Name);
	};
	y+=40;
	DSY.addTextButton(NULL,x0,y,"Code:",&FONT2,&FONT2,&FONT2,0);
	char ccc[128];
	sprintf_s(ccc,sizeof(ccc),"%d",GetRealTime()&32767);
	InputBox* IB=DSY.addInputBox(NULL,x,y,ccc,20,144,28,&FONT2,&FONT2);
	y+=40;
	TextButton* OkBtn=DSY.addTextButton(NULL,10+164/2,315,"OK",&FONT2,&FONT3,&FONT3,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	TextButton* CancelBtn=DSY.addTextButton(NULL,180+164/2,315,"ÎÒÌÅÍÀ",&FONT2,&FONT3,&FONT3,1);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;

	ItemChoose=-1;
	do{
		ProcessMessages();
		if(KeyPressed&&(LastKey==13||LastKey==27)){
			KeyPressed=false;
			ItemChoose=mcmOk;
		};
		DrawStdBar(DSY.BaseX,DSY.BaseY,DSY.BaseX+351,DSY.BaseY+351);
		DSY.MarkToDraw();
		DSY.ProcessDialogs();
		DSY.RefreshView();
		if(CurSty!=CBSTYLE->CurLine){
			CurSty=CBSTYLE->CurLine;
			CBPLAY->Clear();
			for(int p=0;p<RMP.STY[CurSty].NPl;p++){
				char ccc[16];	
				sprintf_s(ccc,sizeof(ccc)," %d",RMP.STY[CurSty].Players[p].NPlayers);
				CBPLAY->AddLine(ccc);
			};
		};
	}while(ItemChoose==-1);
	UnPress();
	if(ItemChoose==mcmOk){
		int r=GetRealTime()&65535;
		sscanf(ccc,"%d",&r);
		int q=CBSTYLE->CurLine+(CBPLAY->CurLine<<4)+(CBMOUNT->CurLine<<8)+(CBRESST->CurLine<<12)+
			(CBRESTOT->CurLine<<16);
		sprintf(Result,"RND %X %X 11111111.m3d",r,q);
	}else Result[0]=0;
};
int RM_LandType =0;
int RM_Resstart =0;
int RM_Restot   =0;
void EnterRandomParams(){
	DialogsSystem DSY((RealLx-64*6)>>1,(RealLy-64*5)>>1);
	LocalGP vinB("vinBig");
	LocalGP vin("vin");
	RLCFont fnti(vinB.GPID);
	RLCFont fntir(vinB.GPID);
	fntir.SetRedColor();
	RLCFont fntis(vin.GPID);
	RLCFont fntisr(vin.GPID);
	fntisr.SetRedColor();
	DSY.addTextButton(NULL,32*6,10,GetTextByID("AIPARM"),&BigYellowFont,&BigYellowFont,&BigYellowFont,1);
	int y=85;
	int x0=10;
	int x=200;
	RandomMapDesc RMP;
	RMP.Load("Terrain\\Res.dat");
	DSY.addTextButton(NULL,x0,y,GETS("@RMISTYLE"),&fntis,&fntis,&fntis,0);
	ComboBox* CBSTYLE=DSY.addComboBox(NULL,x,y,164,25,20,0x3B,255,&fntisr,&fntis,NULL);
	for(int p=0;p<RMP.NSTY;p++){
		CBSTYLE->AddLine(RMP.STY[p].Name);
	};
	CBSTYLE->CurLine=RM_LandType;
	y+=50;
	DSY.addTextButton(NULL,x0,y,GETS("@RMIRESSTART"),&fntis,&fntis,&fntis,0);
	ComboBox* CBRESST=DSY.addComboBox(NULL,x,y,164,25,20,0x3B,255,&fntisr,&fntis,NULL);
	for(int p=0;p<RMP.NRES;p++){
		CBRESST->AddLine(RMP.RES[p].Name);
	};
	CBRESST->CurLine=RM_Resstart;
	y+=45;
	DSY.addTextButton(NULL,x0,y,GETS("@RMIRESTOT"),&fntis,&fntis,&fntis,0);
	ComboBox* CBRESTOT=DSY.addComboBox(NULL,x,y,164,25,20,0x3B,255,&fntisr,&fntis,NULL);
	for(int p=0;p<RMP.NMINES;p++){
		CBRESTOT->AddLine(RMP.MINES[p].Name);
	};
	CBRESTOT->CurLine=RM_Restot;
	y+=55;
	TextButton* OkBtn=DSY.addTextButton(NULL,92-5,y,"OK",&fntir,&fnti,&fnti,1);
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	TextButton* CancelBtn=DSY.addTextButton(NULL,180+82,y,"Cancel",&fntir,&fnti,&fnti,1);
	CancelBtn->UserParam=mcmCancel;
	CancelBtn->OnUserClick=&MMItemChoose;

	ItemChoose=-1;
	do{
		int x0=(RealLx-64*6)>>1;
		int y0=(RealLy-64*4)>>1;
		DrawPaperPanel(x0,y0,x0+64*6,y0+64*4);
		ProcessMessages();
		DSY.MarkToDraw();
		DSY.ProcessDialogs();
		DSY.RefreshView();
		if(KeyPressed){
			if(LastKey==13)ItemChoose=mcmOk;
			if(LastKey==27)ItemChoose=mcmCancel;
			KeyPressed=false;
		};
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		RM_LandType=CBSTYLE->CurLine;
		RM_Resstart=CBRESST->CurLine;
		RM_Restot=CBRESTOT->CurLine;
	};
	UnPress();
};
void RandomMapDialog1(char* Result,int NPlayers,char* Nats){
	
};
int CurrentCampagin=-1;
int CurrentMission=-1;
int CurrentSinBattle=-1;
//int NSinBattle;

struct OneAddMission{
	char Title[128];
	char Map[128];
	char Preview[128];
	char Bitmap[128];
};
class AddMissionsPack{
public:
	OneAddMission* Pack;
	int NMiss;
	AddMissionsPack();
	~AddMissionsPack();
};
bool ReadWinString(GFILE* F,char* STR,int Max);
AddMissionsPack::AddMissionsPack(){
	Pack=NULL;
	NMiss=0;
	WIN32_FIND_DATA SR;
	HANDLE H=FindFirstFile("UserMissions\\*.add",&SR);
	if(H!=INVALID_HANDLE_VALUE){
		do{
			char ccc[128];
			sprintf_s(ccc,sizeof(ccc),"UserMissions\\%s",SR.cFileName);
			GFILE* F=Gopen(ccc,"r");
			if(F){
				Pack=(OneAddMission*)realloc(Pack,sizeof(OneAddMission)*(NMiss+1));
				memset(Pack+NMiss,0,sizeof OneAddMission);
				ReadWinString(F,Pack[NMiss].Title,127);
				ReadWinString(F,Pack[NMiss].Map,127);
				ReadWinString(F,ccc,127);
				_strupr(ccc);
				char* c1=strstr(ccc,".DLL");
				if(!c1)c1=strstr(ccc,".CMS");
				if(c1){
					strcpy(c1,".dsc");
					strcpy(Pack[NMiss].Preview,ccc);
					strcpy(c1,".bmp");
					FILE* F1=fopen(ccc,"r ");
					if(F1){
						strcpy(Pack[NMiss].Bitmap,ccc);
						fclose(F1);
					};
				};
				Gclose(F);
				NMiss++;
			};
		}while(FindNextFile(H,&SR));
		FindClose(H);
	};
};
AddMissionsPack::~AddMissionsPack(){
	if(Pack)free(Pack);
	memset(this,0,sizeof AddMissionsPack);
};

void SlideShow(){
	GFILE* f=Gopen("Slides\\pic.lst","r");
	if(f){
		char cc[128];
		int z;
		do{
			z=Gscanf(f,"%s",cc);
			if(z==1){
				ResFile F=RReset(cc);
				if(F!=INVALID_HANDLE_VALUE){
					RClose(F);
#ifdef SCREENFIX
					DialogsSystem DSS(menu_x_off, menu_y_off);
#else
                    DialogsSystem DSS(0, 0);
#endif
					SQPicture PIC;
					PIC.LoadPicture(cc);
					DSS.addPicture(NULL,0,0,&PIC,&PIC,&PIC);
					Lpressed=0;
					KeyPressed=0;
					PIC.Draw(0,0);
					FlipPages();
					SlowLoadPalette("2\\agew_1.pal");
					do{
						DSS.ProcessDialogs();
						DSS.RefreshView();
						ProcessMessages();
					}while(!(KeyPressed||Lpressed));
					SlowUnLoadPalette("2\\agew_1.pal");
				};
			};
		}while(z==1);
	};
};
void ShowPreview(){
	ResFile F=RReset("Slides\\Preview.bmp");
	if(F!=INVALID_HANDLE_VALUE){
#ifndef _USE3D
		WindX=0;
		WindY=0;
		WindLx=1024;
		WindLy=768;
		WindX1=1023;
		WindY1=767;
#else
		GPS.SetClipArea( 0, 0, 1024, 768 );
#endif
		RClose(F);
		ClearScreen();
		LoadFog(2);
#ifdef SCREENFIX
		DialogsSystem DSS(menu_x_off, menu_y_off);
#else
        DialogsSystem DSS(0, 0);
#endif
		SQPicture PIC;
		PIC.LoadPicture("Slides\\Preview.bmp");
		DSS.addPicture(NULL,0,0,&PIC,&PIC,&PIC);
		Lpressed=0;
		KeyPressed=0;
		PIC.Draw(0,0);
		FlipPages();
		SlowLoadPalette("2\\agew_1.pal");
		int t0=GetTickCount();
		do{
			DSS.ProcessDialogs();
			DSS.RefreshView();
			ProcessMessages();
		}while(!(KeyPressed||Lpressed||GetTickCount()-t0>3000));
		SlowUnLoadPalette("2\\agew_1.pal");
	};
};
void ClearScreen();
char* ROME[6]={"II","III","IV","V","VI","VII"};
int ShowStatScreen(bool Next,bool Prev,byte Kind);
int ShowUserStat(bool Prev,bool Next);
int LPAL=1;
void PrintNum(int N,char* res){
	char tmp[32];
	int ps=0;
	bool sign=N<0;
	N=abs(N);
	while(N!=0){
		int v=N%10;
		N=N/10;
		tmp[ps]='0'+v;
		ps++;
		if((ps==3||ps==7||ps==11||ps==15)&&N!=0){
			tmp[ps]='.';
			ps++;
		};
	};
	if(sign){
		for(int j=0;j<ps;j++)res[j+1]=tmp[ps-j-1];
		res[ps+1]=0;
		res[0]='-';
	}else{
		for(int j=0;j<ps;j++)res[j]=tmp[ps-j-1];
		res[ps]=0;
	};
};
bool ShowStatistics(){
	//return true;
	int res=0;
	int cur=0;
	LPAL=1;
	do{
		switch(cur){
		case 0:
			res=ShowStatScreen(true,false,0);
			break;
		case 1:
			res=ShowStatScreen(true,true,1);
			break;
		case 2:
			res=ShowUserStat(true,false);
			break;
		};
		if(res==1)cur--;
		if(res==4)cur++;
		cur%=3;
	}while(res!=2&&res!=3&&res!=131);
	return res==3; 
};
void CreateCommName(byte Nat,char* Res){
	Res[0]=0;
	for(int i=0;i<7;i++){
		if(NatRefTBL[i]==Nat){
			if(Res[0])strcat(Res,"+");
			strcat(Res,GetPName(i));
		};
	};
};

int stats_x_off, stats_y_off;
void CalculateStatsScreenOffsets(int stats_width)
{
    if (window_mode)
    {//Show in top center of window
        stats_x_off = RealLx / 2 - stats_width / 2;
        stats_y_off = 25;
    }
    else
    {//Fullscreen, center stats
        stats_x_off = (RealLx - 1024) / 2;
        stats_y_off = (RealLy - 768) / 2;
    }
    if (768 == RealLy)
    {//Smallest resolution, don't offset stats vertically or the buttons won't fit
        stats_y_off = 0;
    }
}

int ShowStatScreen(bool Next,bool Prev,byte Kind){
	bool ExStyle=(NPlayers<2&&!PlayGameMode);
	int x=9;
	int y=13;
	int Lx=1006;
	int Ly=690;
	//ClearScreen();
	int NNAT=0;
	int NATS[7];
	int MaxPop=0;
	int MaxUpgTime=0;
	int MaxGrTime=0;
	int ODY=24;
	LocalGP BTNS;
	SQPicture BACK("Interf2\\statistic_background.bmp");
	BTNS.Load("Interf2\\stats");
	y+=ODY;
	Ly-=ODY;
	word* Val[8];
	int NVal[8];
	char* Header;
	if(Kind){
		for(int k=0;k<7;k++){
			Val[k]=NATIONS[k].Account;
			NVal[k]=NATIONS[k].NAccount;
		};
		Header=GetTextByID("ST_ACC");
	}else{
		for(int k=0;k<7;k++){
			Val[k]=NATIONS[k].Popul;
			NVal[k]=NATIONS[k].NPopul;
		};
		Header=GetTextByID("ST_MAX");
	};
	for(int i=0;i<7;i++){
		int max=0;
		Nation* NT=NATIONS+i;
		int N=NVal[i];
		word* VAL=Val[i];
		MaxGrTime=N<<8;
		for(int j=0;j<N;j++)if(VAL[j]>max)max=VAL[j];
		if(max){
			NATS[NNAT]=i;
			NNAT++;
			if(max>MaxPop)MaxPop=max;
		};
		N=NT->NUpgMade;
		if(N&&NT->UpgTime[N-1]>MaxUpgTime)MaxUpgTime=NT->UpgTime[N-1];
	};
	int sdx=3;
	int slx=Lx-70-sdx*2;
	MaxUpgTime+=(40*MaxUpgTime)/(slx-40);
	if(MaxGrTime>MaxUpgTime)MaxUpgTime=MaxGrTime;
	int LXX[8];
	int XXI;
	int YYI[8];
	if(MaxPop<5)MaxPop=5;
	if(NNAT){
		int OneLy=59+4;
		int OneDy=8+2;
		int GrLy=Ly-NNAT*OneLy;
		int GrpX=40;
		int GrpY=40;
		int GrpLx=Lx-2*GrpX;
		int GrpLy=GrLy-2*GrpY;
		int deal=(MaxPop*40)/GrpLy;
		if(deal<5)deal=5;
		else if(deal<10)deal=10;
		else if(deal<15)deal=15;
		else if(deal<20)deal=20;
		else if(deal<25)deal=25;
		else if(deal<40)deal=40;
		else if(deal<50)deal=50;
		else if(deal<75)deal=75;
		else if(deal<100)deal=100;
		else if(deal<150)deal=150;
		else if(deal<200)deal=200;
		else if(deal<250)deal=250;
		else if(deal<300)deal=300;
		else if(deal<400)deal=400;
		else if(deal<500)deal=500;
		else deal=750;
		int NGridY=(MaxPop/deal)+1;
		MaxPop=NGridY*deal;
#ifdef SCREENFIX
		DialogsSystem STAT(menu_x_off, menu_y_off);
#else
        DialogsSystem STAT(0, 0);
#endif
		STAT.addPicture(NULL,0,0,&BACK,&BACK,&BACK);
		int DDX=110;
		//if(!ExStyle)DDX=0;
		if(ExStyle){
			GP_TextButton* BT_REPLAY=STAT.addGP_TextButton(NULL,415+6,723,GetTextByID("REPLAY"),BTNS.GPID,6,&fon18y5,&fon18y5);
			BT_REPLAY->OnUserClick=&MMItemChoose;
			BT_REPLAY->UserParam=131;
		}else{
			GP_TextButton* BT_REPLAY=STAT.addGP_TextButton(NULL,415+6,723,GetTextByID("REPLAY"),BTNS.GPID,6,&fon18y3,&fon18y3);
			BT_REPLAY->OnUserClick=&MMItemChoose;
			BT_REPLAY->UserParam=131;
			BT_REPLAY->Enabled=0;
		};
		//GP_Button* BT=STAT.addGP_Button(NULL,263-DDX,723,BTNS.GPID,0,1);
		GP_TextButton* RGAME=STAT.addGP_TextButton(NULL,263+36-DDX,723,GetTextByID("ST_GAM"),BTNS.GPID,6,&fon18y5,&fon18y5);
		RGAME->OnUserClick=&MMItemChoose;
		RGAME->UserParam=2;

		GP_TextButton* RMENU;
		if(PlayGameMode){
			RMENU=STAT.addGP_TextButton(NULL,526+17+DDX,723,GetTextByID("ST_MEN"),BTNS.GPID,6,&fon18y5,&fon18y5);
			RMENU->Enabled=0;
		}else RMENU=STAT.addGP_TextButton(NULL,526+17+DDX,723,GetTextByID("ST_MEN"),BTNS.GPID,6,&fon18y5,&fon18y5);
		RMENU->OnUserClick=&MMItemChoose;
		RMENU->UserParam=3;

		GP_Button* BACK=STAT.addGP_Button(NULL,218-11-DDX,723,BTNS.GPID,5,1);
		BACK->OnUserClick=&MMItemChoose;
		BACK->UserParam=1;
		BACK->Enabled=Prev;
		BACK->Visible=Prev;

		GP_Button* NEXT=STAT.addGP_Button(NULL,790-16+DDX,723,BTNS.GPID,4,0);
		NEXT->OnUserClick=&MMItemChoose;
		NEXT->UserParam=4;
		NEXT->Enabled=Next;
		NEXT->Visible=Next;

		STAT.HintFont=&SmallBlackFont1;
		STAT.HintX=-1;
		STAT.HintY=250;
		char ccc[100];
		STAT.addTextButton(NULL,512,26,Header,&fon18y5,&fon18y5,&fon18y5,1);
		int yy=y+GrLy;
		int xp=-1000;
		for(int i=0;i<NNAT;i++){
			xp=-1000;
			Nation* NT=NATIONS+NATS[i];
			char commname[128];
			CreateCommName(NATS[i],commname);
			//char* pnm=GetPName(NATS[i]);
			LXX[i]=GetRLCStrWidth(commname,&SmallWhiteFont1)+40;
			int dx=x+GrpX+(GrpLx-LXX[i])/2;
			STAT.addTextButton(NULL,dx+33,yy-8,commname,&SmallWhiteFont1,&SmallWhiteFont1,&SmallWhiteFont1,0);
			STAT.addColoredBar(dx+6,yy-2,24,1,0xD0+NATS[i]*4);
			XXI=x+34;
			YYI[i]=yy;
			int N=NT->NUpgMade;
			int DXX=10;
			ColoredBar* CB=STAT.addColoredBar(GrpX+DXX,yy-10,1024-GrpX*2-DXX*2,62,0xEC);
			CB->Style=1;
			CB=STAT.addColoredBar(GrpX+DXX,yy+6,1024-GrpX*2-DXX*2,0,0xEC);
			CB->Style=1;
			//CB=STAT.addColoredBar(GrpX+DXX,yy-10,1024-GrpX*2-DXX*2,16,0xEC);
			CB=STAT.addColoredBar(GrpX+DXX-1,yy-1-10,1024-GrpX*2+2-DXX*2,62+2,0xEC);
			CB->Style=1;
			for(int j=0;j<N;j++){
				int xx=x+3+30+(NT->UpgTime[j]*slx)/MaxUpgTime;
				if(xx<xp){
					xx=xp+2;
				};
				xp=xx;
				NewUpgrade* NU=NT->UPGRADE[NT->UpgIDS[j]];
				if(NU->CtgUpgrade!=23){
					SimpleDialog* SD;
					if(NU->IconFileID!=0xFFFF)SD=STAT.addGPPicture(NULL,xx,yy+OneDy,NU->IconFileID,NU->IconSpriteID);
					else SD=STAT.addColoredBar(xx,yy+OneDy,40,40,0);
					if(SD)SD->Hint=NU->Message;
					ColoredBar* CB=STAT.addColoredBar(xx,yy+OneDy,40,40,0xD0+NATS[i]*4);
					if(CB)CB->Style=1;
					if(SD)SD->Child=CB;
					/*
					if(NU->Level>1){
						TextButton* TB=STAT.addTextButton(NULL,xx+2,yy+OneDy+25,ROME[NU->Level-2],&SmallWhiteFont,&SmallWhiteFont,&SmallWhiteFont,0);
						TB->Enabled=0;
						CB->Child=TB;
					};
					*/
				};
			};
			yy+=OneLy;
		};
		ItemChoose=-1;
		KeyPressed=0;
		Lpressed=0;
		byte mm=1;
		do{
			//GPS.ShowGP(0,0,BTNS.GPID,2,0);
			//for(int i=0;i<NNAT;i++)DrawStHeader(GrpX+x+GrpLx/2,YYI[i],LXX[i]);
			STAT.MarkToDraw();
			STAT.ProcessDialogs();
			//DrawStatTable(x,y-ODY,Lx,GrLy+ODY,OneLy,NNAT);
#ifdef SCREENFIX
			int gx0=GrpX+x+ menu_x_off;
			int gx1=GrpX+x+GrpLx-1+ menu_x_off;
			int gy0=GrpY+y+ menu_y_off;
			int gy1=GrpY+y+GrpLy-1+ menu_y_off;
#else
            int gx0 = GrpX + x ;
            int gx1 = GrpX + x + GrpLx - 1 ;
            int gy0 = GrpY + y ;
            int gy1 = GrpY + y + GrpLy - 1 ;
#endif
			Xbar(gx0-1,gy0-1,gx1-gx0+4,gy1-gy0+4,0xEC);
			//Xbar(gx0-2,gy0-2,gx1-gx0+5,gy1-gy0+5,0x3A);
			//Xbar(gx0-3,gy0-3,gx1-gx0+7,gy1-gy0+7,0x39);
			//Xbar(gx0-4,gy0-4,gx1-gx0+9,gy1-gy0+9,0xAE);
			//Xbar(gx0-5,gy0-5,gx1-gx0+11,gy1-gy0+11,0xAE);
			//Xbar(gx0-6,gy0-6,gx1-gx0+13,gy1-gy0+13,0xAE);
			//drawing grids
			for(int ix=0;ix<8;ix++){
#ifdef SCREENFIX
				int gx0=x+GrpX+((GrpLx*ix)>>3)+ menu_x_off;
				int gx1=x+GrpX+((GrpLx*(ix+1))>>3)+ menu_x_off;
#else
                int gx0 = x + GrpX + ((GrpLx * ix) >> 3) ;
                int gx1 = x + GrpX + ((GrpLx * (ix + 1)) >> 3) ;
#endif
				for(int iy=0;iy<NGridY;iy++){
#ifdef SCREENFIX
					int gy0=y+GrpY+((GrpLy*iy)/NGridY)+ menu_y_off;
					int gy1=y+GrpY+((GrpLy*(iy+1))/NGridY)+ menu_y_off;
#else
                    int gy0 = y + GrpY + ((GrpLy * iy) / NGridY) ;
                    int gy1 = y + GrpY + ((GrpLy * (iy + 1)) / NGridY) ;
#endif
					Xbar(gx0,gy0,gx1-gx0+1,gy1-gy0+1,0xEC);
					//Xbar(gx0+1,gy0+1,gx1-gx0-1,gy1-gy0-1,0x3B);
					//Xbar(gx0+2,gy0+2,gx1-gx0-3,gy1-gy0-3,0x3A);
					//Xbar(gx0+3,gy0+3,gx1-gx0-5,gy1-gy0-5,0x39);
					//Xbar(gx0+4,gy0+4,gx1-gx0-7,gy1-gy0-7,0xAE);
					//Xbar(gx0+5,gy0+5,gx1-gx0-9,gy1-gy0-9,0xAE);
					//Xbar(gx0+6,gy0+6,gx1-gx0-11,gy1-gy0-11,0xAE);
					if(ix==0){
						sprintf_s(ccc,sizeof(ccc),"%d",(NGridY-iy-1)*deal);
						ShowString(gx0+3,gy1-14,ccc,&SmallBlackFont);
					};
				};
			};
			int X0,Y0,X1,Y1;
			for(int i=0;i<NNAT;i++){
				Nation* NT=NATIONS+NATS[i];
				byte c=NATS[i]*4+0xD0;
				int N=NVal[NATS[i]];
				word* VAL=Val[NATS[i]];
				for(int j=0;j<N;j++){
					if(j>0){
						X0=X1;
						Y0=Y1;
					};
#ifdef SCREENFIX
					X1=x+GrpX+((j<<8)*GrpLx)/MaxUpgTime+ menu_x_off;
					Y1=y+GrpY+GrpLy-(VAL[j]*(GrpLy-10))/MaxPop+ menu_y_off;
#else
                    X1 = x + GrpX + ((j << 8) * GrpLx) / MaxUpgTime ;
                    Y1 = y + GrpY + GrpLy - (VAL[j] * (GrpLy - 10)) / MaxPop ;
#endif
					if(j>0){
						DrawLine(X0,Y0,X1,Y1,c);
						DrawLine(X0+1,Y0,X1+1,Y1,c);
						DrawLine(X0+1,Y0+1,X1+1,Y1+1,c);
						DrawLine(X0,Y0+1,X1,Y1+1,c);
					};
				};
			};
			ProcessMessages();
			STAT.RefreshView();
			if(LPAL){
				SlowLoadPalette("0\\agew_1.pal");
				LPAL=0;
			};
			//if(KeyPressed||Lpressed){
			//	ItemChoose=1;
			//};
		}while(ItemChoose==-1);
	}else return 3;
	KeyPressed=false;
	Lpressed=0;
	UnPress();
	return ItemChoose;
};
int ShowUnitsList(int x,int y,int Lx,word* Value);
#define Statfont &fon16y3
int ShowUserStat(bool Prev,bool Next){
	bool ExStyle=(NPlayers<2&&!PlayGameMode);
	int x=9;
	int y=13;
	int Lx=1006;
	int Ly=690;
	LocalGP BTNS;
	SQPicture BACK2("Interf2\\statistic_background.bmp");
	BTNS.Load("Interf2\\stats");
#ifdef SCREENFIX
	DialogsSystem STAT(menu_x_off, menu_y_off);
#else
    DialogsSystem STAT(0, 0);
#endif
	STAT.addPicture(NULL,0,0,&BACK2,&BACK2,&BACK2);
	int DDX=110;
	//if(!ExStyle)DDX=0;
	if(ExStyle){
		GP_TextButton* BT_REPLAY=STAT.addGP_TextButton(NULL,415+6,723,GetTextByID("REPLAY"),BTNS.GPID,6,&fon18y5,&fon18y5);
		BT_REPLAY->OnUserClick=&MMItemChoose;
		BT_REPLAY->UserParam=131;
	}else{
		GP_TextButton* BT_REPLAY=STAT.addGP_TextButton(NULL,415+6,723,GetTextByID("REPLAY"),BTNS.GPID,6,&fon18y3,&fon18y3);
		BT_REPLAY->OnUserClick=&MMItemChoose;
		BT_REPLAY->UserParam=131;
		BT_REPLAY->Enabled=0;
	};
	//GP_Button* BT=STAT.addGP_Button(NULL,263-DDX,723,BTNS.GPID,0,1);
	GP_TextButton* RGAME=STAT.addGP_TextButton(NULL,263+36-DDX,723,GetTextByID("ST_GAM"),BTNS.GPID,6,&fon18y5,&fon18y5);
	RGAME->OnUserClick=&MMItemChoose;
	RGAME->UserParam=2;

	GP_TextButton* RMENU;
	if(PlayGameMode){
		RMENU=STAT.addGP_TextButton(NULL,526+17+DDX,723,GetTextByID("ST_MEN"),BTNS.GPID,6,&fon18y5,&fon18y5);
		RMENU->Enabled=0;
	}else RMENU=STAT.addGP_TextButton(NULL,526+17+DDX,723,GetTextByID("ST_MEN"),BTNS.GPID,6,&fon18y5,&fon18y5);
	RMENU->OnUserClick=&MMItemChoose;
	RMENU->UserParam=3;

	GP_Button* BACK=STAT.addGP_Button(NULL,218-11-DDX,723,BTNS.GPID,5,1);
	BACK->OnUserClick=&MMItemChoose;
	BACK->UserParam=1;
	BACK->Enabled=Prev;
	BACK->Visible=Prev;

	GP_Button* NEXT=STAT.addGP_Button(NULL,790-16+DDX,723,BTNS.GPID,4,0);
	NEXT->OnUserClick=&MMItemChoose;
	NEXT->UserParam=4;
	NEXT->Enabled=Next;
	NEXT->Visible=Next;
	ListBox* LB=STAT.addListBox(NULL,x+25+20,y+35,30,180,26,&WhiteFont,&YellowFont,NULL);
	byte NATS[8];
	int NNAT=0;
	for(int i=0;i<7;i++){
		int max=0;
		Nation* NT=NATIONS+i;
		int N=NT->NPopul;
		word* VAL=NT->Popul;
		for(int j=0;j<N;j++)if(VAL[j]>max)max=VAL[j];
		if(max){
			NATS[NNAT]=i;
			NNAT++;
			char commname[256];
			CreateCommName(i,commname);
			LimitString(commname,&YellowFont,200);
			LB->AddItem(commname,i);
		};
	};
	LB->CurItem=0;
	ItemChoose=-1;
	int mm=1;
	char* hdrs[8];
	hdrs[0]=GetTextByID("ST_TOTAL");
	hdrs[1]=GetTextByID("ST_UPGR");
	hdrs[2]=GetTextByID("ST_MINE");
	hdrs[3]=GetTextByID("ST_UNITS");
	hdrs[4]=GetTextByID("ST_BLD");
	hdrs[5]=GetTextByID("ST_LIFE");
	hdrs[6]=GetTextByID("ST_BUY");
	hdrs[7]=GetTextByID("ST_SELL");
	char* STHDR=GetTextByID("ST_UHDR");
	char* KHDR=GetTextByID("ST_KILL");
	char* PHDR=GetTextByID("ST_PROD");
	int curnat=0;
#ifdef SCREENFIX
	int ux=230-30+ menu_x_off;
	int uy=320+ menu_y_off;
#else
    int ux = 230 - 30;
    int uy = 320;
#endif
	int ulx=353;
	int uly=373;
	int kx=ux+ulx+45;
#ifdef SCREENFIX
	VScrollBar* PBR=STAT.addNewGP_VScrollBar(NULL,ux+ulx+11-15- menu_x_off,uy+1+2- menu_y_off,uly-2-2,1,0,3,0);
	VScrollBar* KBR=STAT.addNewGP_VScrollBar(NULL,kx+ulx+11-15- menu_x_off,uy+1+2- menu_y_off,uly-2-2,1,0,3,0);
#else
    VScrollBar* PBR = STAT.addNewGP_VScrollBar(NULL, ux + ulx + 11 - 15 , uy + 1 + 2 , uly - 2 - 2, 1, 0, 3, 0);
    VScrollBar* KBR = STAT.addNewGP_VScrollBar(NULL, kx + ulx + 11 - 15 , uy + 1 + 2 , uly - 2 - 2, 1, 0, 3, 0);
#endif
	PBR->ScrDy=200;
	KBR->ScrDy=200;
	do{
		ProcessMessages();
		STAT.MarkToDraw();
		STAT.ProcessDialogs();
		curnat=LB->CurItem;
		int cnt=NATS[curnat];
		int* ResSrc=(int*)(&NATIONS[cnt].ResTotal);
		//GPS.ShowGP(0,0,BTNS.GPID,2,0);
		//DrawStatTable1(x,y,x+Lx-1,y+Ly-1,x+200);
		for(int i=0;i<LB->NItems;i++){
			CBar(LB->x-15,LB->y+15+i*25,12,3,0xD0+NatRefTBL[NATS[i]]*4);
		};
#ifdef SCREENFIX
		ShowString(512-GetRLCStrWidth(STHDR,&fon18y5)/2+ menu_x_off,26+ menu_y_off,STHDR,&fon18y5);
#else
        ShowString(512 - GetRLCStrWidth(STHDR, &fon18y5) / 2 , 26, STHDR, &fon18y5);
#endif
		ShowString(ux+(ulx>>1)+7-(GetRLCStrWidth(PHDR,&YellowFont)>>1),uy-22,PHDR,Statfont);
		ShowString(kx+(ulx>>1)+7-(GetRLCStrWidth(KHDR,&YellowFont)>>1),uy-22,KHDR,Statfont);
#ifdef SCREENFIX
		int rx=190+ menu_x_off;
		int ry=y+48+ menu_y_off;
#else
        int rx = 190;
        int ry = y + 48;
#endif
		int rx1=rx+219+5;
		int rxl=80;
		for(int q=0;q<6;q++){
			ShowString(rx1+rxl*q+rxl+(rxl>>1)-(GetRLCStrWidth(RDS[q].Name,&YellowFont)>>1),ry+6,RDS[q].Name,Statfont);
		};
		Hline(rx,ry,rx1+rxl*7,0xEC);
		ry+=26;
		Vline(rx,ry-26,ry+8*26,0xEC);
		for(int q=0;q<8;q++){
			Hline(rx,ry,rx1+rxl*7,0xEC);
			ShowString(rx+15,ry+6,hdrs[q],Statfont);
			for(int j=0;j<6;j++){
				char cc1[32];
				PrintNum(ResSrc[q*8+j],cc1);
				//sprintf(cc1,"%d",ResSrc[q*8+j]);
				ShowString(rx1+rxl*j+rxl+(rxl>>1)-(GetRLCStrWidth(cc1,&YellowFont)>>1),ry+6,cc1,Statfont);
				if(q==0){
					Vline(rx1+rxl*j+rxl,ry-26,ry+8*26,0xEC);
				};
			};
			if(q==0){
				Vline(rx1+rxl*7,ry-26,ry+8*26,0xEC);
			};
			ry+=26;
		};
		Hline(rx,ry,rx1+rxl*7,0xEC);
		TempWindow TMW;
		PushWindow(&TMW);
		Xbar(ux-10,uy,ulx+25+10,uly,0xEC);
		IntersectWindows(ux-8,uy+1,ux+ulx+15,uy+uly-2);
		int y0=uy-PBR->SPos;
		int yfin=ShowUnitsList(ux,y0,ulx-12,NATIONS[cnt].NProduced);
		int mps=yfin-uly-2+3;
		if(mps>0){
			PBR->Visible=1;
			PBR->Enabled=1;
			PBR->SMaxPos=mps;
		}else{
			PBR->Visible=0;
			PBR->Enabled=0;
			PBR->SPos=0;
		};
		PopWindow(&TMW);
		Xbar(kx-10,uy,ulx+25+10,uly,0xEC);
		IntersectWindows(kx-8,uy+1,kx+ulx+15,uy+uly-2);
		y0=uy-KBR->SPos;
		yfin=ShowUnitsList(kx,y0,ulx-12,NATIONS[cnt].NKilled);
		mps=yfin-uly-2+3;
		if(mps>0){
			KBR->Visible=1;
			KBR->Enabled=1;
			KBR->SMaxPos=mps;
		}else{
			KBR->Visible=0;
			KBR->Enabled=0;
			KBR->SPos=0;
		};
		PopWindow(&TMW);
		STAT.RefreshView();
		if(mm){
			//SlowLoadPalette("0\\agew_1.pal");
			//mm=0;
		};
		//if(KeyPressed||Lpressed){
		//	ItemChoose=1;
		//};
	}while(ItemChoose==-1);
	KeyPressed=false;
	Lpressed=0;
	UnPress();
	return ItemChoose;
};
void CenterScreen(){
	int xc=0;
	int yc=0;
	int N=0;
	int mnt=NatRefTBL[MyNation];
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->NNUM==mnt){
			xc+=OB->RealX>>4;
			yc+=OB->RealY>>4;
			N++;
		};
	};
	if(N){
		int x=xc/N;
		int y=yc/N;
		x=(x-(RealLx>>1))>>5;
		y=(y-RealLy)>>5;
		if(x<1)x=1;
		if(y<1)y=1;
		if(x+smaplx>=msx-1)x=msx-smaplx-1;
		if(y+smaply>=msy-1)y=msy-smaply-1;
		mapx=x;
		mapy=y;
	};
};
extern bool EnterChatMode;
extern char ChatString[128];
bool Superuser=0;
void AssignHint1(char* s,int time);
void CmdGiveMoney(byte SrcNI,byte DstNI,byte Res,int Amount);
void CmdMoney(byte NI);
int ReadKey();
extern int NKeys;
extern char CHATSTRING[256];
int ChatCursPos=0;
bool CheckFNSend(int idx);
extern byte LastAscii;
void HandleSMSChat(char* Mess);
void HandlwSMSMouse();
void CmdOfferVoting();
extern int CurAINF_Show;
DLLEXPORT
int FindNextZoneOnTheSafeWay(int Start,int Fin,short* DangerMap,int* MaxDanger,int DangSteps);
DLLEXPORT void SelectUnits(GAMEOBJ* Units,bool Add);
int WayID=0;
extern int NWay;
extern bool WayMode;

CIMPORT void AddStorm(GAMEOBJ* Src, byte Owner, int Diff);
CIMPORT void AddFirers(GAMEOBJ* Src,byte Owner);
CIMPORT void AddPsKillers(GAMEOBJ* Src,byte Owner,bool SeakMine=true);
CIMPORT void AddTomahawks(GAMEOBJ* Src,byte Owner,word Base=0,int DriftRadius=0, int CriticalMass=0);

CEXPORT void AddToGChat(char* Nick, char* Message) {
}
CEXPORT void ClearGChat() {
}

DLLEXPORT void RegisterDynGroup(GAMEOBJ* Units);
DLLEXPORT void SaveSelectedUnits(byte NI,GAMEOBJ* Units,bool Add);
GAMEOBJ TempGrp;

CIMPORT void MindCheats(byte NI, char* com);
byte MindNI=0xFF;

extern word FULLWAY[128];
extern int NWPTS;
extern int TOPTYPE;

int DrawPathID=0xFFFF;
int DrawDestID=0xFFFF;
void ProcessChatKeys(){
KRT:
	if(!NKeys)return;
	LastKey=ReadKey();
	KeyPressed=0;
	if(LastKey==27){
		EnterChatMode=0;
		ChatString[0]=0;
		return;
	};
	if(LastKey==13){
		HandleSMSChat(ChatString);
		EnterChatMode=0;
		AssignHint1(ChatString,250,16+NatRefTBL[MyNation]);
		SendChat(ChatString,(GetKeyState(VK_CONTROL)&0x8000)!=0);
		ChatCursPos=0;
		if(NPlayers<2){
#ifdef AI_CHEATS
			if(strstr(ChatString,"FWAY")){
				int T0,T1;
				char SS[128];
				int z=sscanf(ChatString,"%s%d%d",SS,&T0,&T1);
				if(z==3){
					short DM[2048];
					int MAXD;
					memset(DM,0,sizeof DM);
					FindNextZoneOnTheSafeWay(T0,T1,DM,&MAXD,256);
					CurAINF_Show=7;
				};
			}else
			if(strstr(ChatString,"fway")){
				int T0,T1;
				char SS[128];
				int z=sscanf(ChatString,"%s%d%d",SS,&T0,&T1);
				if(z==3){
					int NA=GetNAreas();
					NWPTS=1;
					FULLWAY[0]=T0;
					do{
						T0=GetMotionLinks(T0*NA+T1);
						if(T0>=0xFFFE) break;
						FULLWAY[NWPTS]=T0;
						NWPTS++;
					}while(T0!=T1&&NWPTS<127);
					CurAINF_Show=7;
				};
			}else
			if(strstr(ChatString,"locktype")){
				char Com[128];
				int t;
				int z=sscanf(ChatString,"%s%d",Com,&t);
				if(z>1){
					if(t<0||t>=NMFIELDS) TOPTYPE=0;
						else TOPTYPE=t;
				}
			}else
			if(strstr(ChatString,"toplink")||strstr(ChatString,"tlink")){	// show top link
				word Top,LockType;
				char Com[128];
				int z=sscanf(ChatString,"%s%d%d",Com,&Top,&LockType);
				if(z>1){
					if(z==2) LockType=0;
					int NA=GetNAreas(LockType);
					if(Top<NA){						
						Area* AR=GetTopMap(Top,LockType);
						int NLink=AR->NLinks;
						word* Link=AR->Link;
						for(int i=0;i<NLink;i++){
							FULLWAY[i+i]=Top;
							FULLWAY[i+i+1]=Link[i+i];
						}
						NWPTS=NLink<<1;						
					}
					CurAINF_Show=7;
				}
			}else
			if(strstr(ChatString,"SW "))
			{
				char tmp[8];
				int nat;
				if(sscanf(ChatString,"%s %d",tmp, &nat))
					if(nat>-1&&nat<8)
						if(NSL[nat]>0)
						{
							OneObject* OB=Group[Selm[nat][0]];
							if(OB&&OB->BrigadeID!=0xFFFF)
							{
								void AddBrigadeInWiselyFormations(Brigade* B);
								AddBrigadeInWiselyFormations(&(CITY[nat].Brigs[OB->BrigadeID]));
							}
						}
			}
			else
#endif
			/*
			if(!strcmp(ChatString,"SGAI0")){
				CurAINF_Show=0;
			}else
			if(!strcmp(ChatString,"SGAI1")){
				CurAINF_Show=1;
			}else
			if(!strcmp(ChatString,"SGAI2")){
				CurAINF_Show=2;
			}else
			if(!strcmp(ChatString,"SGAI3")){
				CurAINF_Show=3;
			}else
			if(!strcmp(ChatString,"SGAI4")){
				CurAINF_Show=4;
			}else
			if(!strcmp(ChatString,"SGAI5")){
				CurAINF_Show=5;
			}else
			if(!strcmp(ChatString,"SGAI6")){
				CurAINF_Show=6;
			}else
			if(!strcmp(ChatString,"SGAI7")){
				CurAINF_Show=7;
			}else
			if(!strcmp(ChatString,"NOSGAI")){
				CurAINF_Show=-1;
			}else
			*/
			if(strstr(ChatString,"path")){
				char ID[512];
				int PARAM=0;
				int z=sscanf(ChatString,"%s%d",ID,&PARAM);
				if(z==2){
					DrawPathID=PARAM;
				}
			}else
			if(strstr(ChatString,"dest")){
				char ID[512];
				int PARAM=0;
				int z=sscanf(ChatString,"%s%d",ID,&PARAM);
				if(z==2){
					DrawDestID=PARAM;
				}
			}else
			if(!strcmp(ChatString,"qwe")){
#ifndef AI_CHEATS
				CurrentSinBattle=-1;
#endif
				ChangeNation=1;
				MultiTvar=1;
				FogMode=0;
			}else
			if(!strcmp(ChatString,"VICTORY")){
#ifndef AI_CHEATS
				CurrentSinBattle=-1;
#endif
				if(NPlayers<2)ShowVictory();
			}else
			if(!strcmp(ChatString,"ntch"))ChangeNation=!ChangeNation;
			if(!strcmp(ChatString,"units"))MultiTvar=!MultiTvar;
			if(!strcmp(ChatString,"viewall")){
#ifndef AI_CHEATS
				CurrentSinBattle=-1;
#endif
				FogMode=!FogMode;
			}
#ifdef AI_CHEATS
			char* c=NULL;
			if(strstr(ChatString,"setmind")){				
				char com[200];
				byte nat;
				int z=sscanf(ChatString,"%s%d",com,&nat);
				if(z==2&&nat<7){
					MindNI=nat;
				}
			}else
			if(c=strstr(ChatString,"mind")){				
				if(MindNI!=0xFF){
					//char com[200];
					//int z=sscanf(ChatString,"%s",com);
					//if(z==2){
						MindCheats(MindNI, c+4);
					//}				
				}
			}else
			if(strstr(ChatString,"addtom")){
				char com[200];
				byte nat;
				int strelok=0;
				int z=sscanf(ChatString,"%s%d%d",com,&nat,&strelok);
				if(z>2){
					if(TempGrp.Index!='UNIT') RegisterDynGroup(&TempGrp);
					SaveSelectedUnits(nat,&TempGrp,0);
					AddTomahawks(&TempGrp,nat,strelok);
				}
			}else
			if(strstr(ChatString,"addstm")){
				char com[200];
				byte nat;
				int dif=0;
				int z=sscanf(ChatString,"%s%d%d",com,&nat,&dif);
				if(z>1){
					if(TempGrp.Index!='UNIT') RegisterDynGroup(&TempGrp);
					SaveSelectedUnits(nat,&TempGrp,0);
					AddStorm(&TempGrp,nat,dif);
				}
			}else
#endif
			if(!strcmp(ChatString,"babki")){
#ifndef AI_CHEATS
				CurrentSinBattle=-1;
#endif
				CmdMoney(MyNation);
			}
			else if(NPlayers<2){
				char cc1[64];
				int n1,n2;
				int z=sscanf(ChatString,"%s%d%d",cc1,&n1,&n2);
				if(z==3&&!strcmp(cc1,"change")){
					if(n1>=0&&n1<8&&n2>=0&&n2<=8)SelChangeNation(n1,n2);
				};
				char cc2[128];
				z=sscanf(ChatString,"%s%s",cc1,cc2);
				if(z==2&&!strcmp(cc1,"changeunit")){
					word Type=0xFFFF;
					for(int i=0;i<NATIONS->NMon;i++){
						if(!strcmp(cc2,NATIONS->Mon[i]->MonsterID)){
							GeneralObject* GO=NATIONS->Mon[i];
							if(!GO->newMons->Building){
								//found!!!
								int N=NSL[MyNation];
								word* IDS=Selm[MyNation];
								word* SMS=SerN[MyNation];
								for(int j=0;j<N;j++){
									word MID=IDS[j];
									if(MID!=0xFFFF){
										OneObject* OB=Group[MID];
										if(OB&&!OB->Sdoxlo){
											OB->Ref.General=GO;
											OB->NIndex=i;
											OB->newMons=GO->newMons;
										};
									};
								};
							};
						};
					};
				};
			};
		}else{
			//if(!strcmp(ChatString,"SuPeRuSeR"))Superuser=1;
		};
		if(ChatString[0]=='g'&&ChatString[1]=='i'&&ChatString[2]=='v'&&ChatString[3]=='e'&&ChatString[4]==' '){
			char* sss=ChatString+5;
			char cc1[100];
			int p=0;
			char c;
			while(sss[0]==' ')sss++;
			do{
				c=sss[0];
				cc1[p]=c;
				if(c!=' ')p++;
				sss++;
			}while(c!=0&&c!=' '&&p<20);
			cc1[p]=0;
			if(c==0)return;
			int res=-1;
			if(!strcmp(cc1,"wood"))res=0;
			else if(!strcmp(cc1,"gold"))res=1;
			else if(!strcmp(cc1,"stone"))res=2;
			else if(!strcmp(cc1,"food"))res=3;
			else if(!strcmp(cc1,"iron"))res=4;
			else if(!strcmp(cc1,"coal"))res=5;
			if(res==-1)return;
			while(sss[0]==' ')sss++;
			p=0;
			do{
				c=sss[0];
				cc1[p]=c;
				if(c>='0'&&c<='9')c=0;
				cc1[p]=c;
				if(c){
					sss++;
					p++;
				};
			}while(c!=0&&p<40);
			int L;
			do{
				L=strlen(cc1);
				if(L&&cc1[L-1]==' ')cc1[L-1]=0;
				else L=0;
			}while(L);
			int Him=-1;
			for(int i=0;i<NPlayers;i++){
				if(!strcmp(cc1,PINFO[i].name))Him=PINFO[i].ColorID;
			};
			if(Him==-1)return;
			while(sss[0]==' ')sss++;
			int Amn;
			int z=sscanf(sss,"%d",&Amn);
			if(z!=1)return;
			CmdGiveMoney(MyNation,Him,res,Amn);
		};
#ifdef AI_CHEATS
		if(ChatString[0]=='t'&&ChatString[1]=='o'&&ChatString[2]=='p'&&ChatString[3]==' '){
			int tz,locktype;
			int z=sscanf(ChatString+4,"%d%d",&tz,&locktype);
			if(z){
				if(z==1) locktype=0;
				if(tz>=0&&tz<GetNAreas(locktype)){
					Area* AR=GetTopMap(tz,locktype);
					mapx=(AR->x<<1)-(smaplx>>1);
					mapy=(AR->y<<1)-(smaply>>1);
					if(mapx<=0)mapx=1;
					if(mapy<=0)mapy=1;
					if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
					if(mapy+smaply>msy+1)mapy=msy-smaply+1;
				};
			};
		};
		if(ChatString[0]=='s'&&ChatString[1]=='e'&&ChatString[2]=='g'&&ChatString[3]==' '){
			int g;
			int n=sscanf(ChatString+4,"%d",&g);
			if(n==1){
				GAMEOBJ grp;
				grp.Index=g;
				grp.Type='UNIT';
				SelectUnits(&grp,0);
			}
		}
		if(ChatString[0]=='w'&&ChatString[1]=='a'&&ChatString[2]=='y'){
			WayMode=1;
			NWay=0;
			if(NSL[MyNation]){
				WayID=Selm[MyNation][0];
			};
		}
#endif
		ChatString[0]=0;
		return;
	};
	if(LastKey>=VK_F1&&LastKey<VK_F9){
		if(CheckFNSend(LastKey-VK_F1)){
			EnterChatMode=0;	
			NKeys=0;
			return;
		};
	};
	
	if(LastKey==VK_LEFT){
		if(ChatCursPos>0)ChatCursPos--;
		KeyPressed=0;
		LastKey=0;
		goto KRT;
	}else
	if(LastKey==VK_RIGHT){
		if(ChatCursPos<int(strlen(ChatString)))ChatCursPos++;
		KeyPressed=0;
		LastKey=0;
		goto KRT;
	}else
	if(LastKey==VK_END){
		ChatCursPos=strlen(ChatString);
		KeyPressed=0;
		LastKey=0;
		goto KRT;
	}else
	if(LastKey==VK_HOME){
		ChatCursPos=0;
		KeyPressed=0;
		LastKey=0;
		goto KRT;
	}else
	if(LastKey==VK_BACK){
		if(ChatCursPos>0){
			strcpy(ChatString+ChatCursPos-1,ChatString+ChatCursPos);
			ChatCursPos--;
		};
		//if(strlen(IB->Str)>0)IB->Str[strlen(IB->Str)-1]=0;
		goto KRT;
	}else
	if(LastKey==46){//VK_DEL
		if(ChatCursPos<int(strlen(ChatString)))strcpy(ChatString+ChatCursPos,ChatString+ChatCursPos+1);
		//if(strlen(IB->Str)>0)IB->Str[strlen(IB->Str)-1]=0;
		goto KRT;
	}else{
		if(LastAscii){
			LastKey=LastAscii;
			char xx[2];
			xx[1]=0;
			xx[0]=char(LastKey);
			if(strlen(ChatString)+1<80){
				char ccc[2048];
				strcpy(ccc,ChatString);
				ccc[ChatCursPos]=0;
				strcat(ccc,xx);
				strcat(ccc,ChatString+ChatCursPos);
				strcpy(ChatString,ccc);
				ChatCursPos++;
			};
			goto KRT;
		};
	};
	if(NKeys)goto KRT;
	ChatString[80]=0;
	if(ChatCursPos>int(strlen(ChatString)))ChatCursPos=strlen(ChatString);
/*







	//if(!KeyPressed)return;
KRT:
	if(!NKeys)return;
	LastKey=ReadKey();
	KeyPressed=0;
	if(LastKey==27){
		EnterChatMode=0;
		ChatString[0]=0;
		return;
	};
	if(LastKey>=VK_F1&&LastKey<=VK_F9){
		if(CheckFNSend(LastKey-VK_F1)){
			EnterChatMode=0;	
			NKeys=0;
			return;
		};
	};
	if(LastKey==13){
		EnterChatMode=0;
		AssignHint1(ChatString,250,16+MyNation);
		SendChat(ChatString,GetKeyState(VK_CONTROL)&0x8000);
		if(NPlayers<2){
			if(!strcmp(ChatString,"www")){
				ChangeNation=1;
				MultiTvar=1;
				FogMode=0;
			}else
			if(!strcmp(ChatString,"VICTORY")){
				if(NPlayers<2)ShowVictory();
			}else
			if(!strcmp(ChatString,"IZMENA")||!strcmp(ChatString,"izmena"))ChangeNation=!ChangeNation;
			if(!strcmp(ChatString,"MULTITVAR")||!strcmp(ChatString,"multitvar"))MultiTvar=!MultiTvar;
			if(!strcmp(ChatString,"SUPERVIZOR")||!strcmp(ChatString,"supervizor"))FogMode=!FogMode;
			if(!strcmp(ChatString,"MONEY")||!strcmp(ChatString,"money"))CmdMoney(MyNation);
			else if(NPlayers<2){
				char cc1[64];
				int n1,n2;
				int z=sscanf(ChatString,"%s%d%d",cc1,&n1,&n2);
				if(z==3&&!strcmp(cc1,"change")){
					if(n1>=0&&n1<8&&n2>=0&&n2<=8)SelChangeNation(n1,n2);
				};
			};
		}else{
			//if(!strcmp(ChatString,"SuPeRuSeR"))Superuser=1;
		};
		if(ChatString[0]=='g'&&ChatString[1]=='i'&&ChatString[2]=='v'&&ChatString[3]=='e'&&ChatString[4]==' '){
			char* sss=ChatString+5;
			char cc1[100];
			int p=0;
			char c;
			while(sss[0]==' ')sss++;
			do{
				c=sss[0];
				cc1[p]=c;
				if(c!=' ')p++;
				sss++;
			}while(c!=0&&c!=' '&&p<20);
			cc1[p]=0;
			if(c==0)return;
			int res=-1;
			if(!strcmp(cc1,"wood"))res=0;
			else if(!strcmp(cc1,"gold"))res=1;
			else if(!strcmp(cc1,"stone"))res=2;
			else if(!strcmp(cc1,"food"))res=3;
			else if(!strcmp(cc1,"iron"))res=4;
			else if(!strcmp(cc1,"coal"))res=5;
			if(res==-1)return;
			while(sss[0]==' ')sss++;
			p=0;
			do{
				c=sss[0];
				cc1[p]=c;
				if(c>='0'&&c<='9')c=0;
				cc1[p]=c;
				if(c){
					sss++;
					p++;
				};
			}while(c!=0&&p<40);
			int L;
			do{
				L=strlen(cc1);
				if(L&&cc1[L-1]==' ')cc1[L-1]=0;
				else L=0;
			}while(L);
			int Him=-1;
			for(int i=0;i<NPlayers;i++){
				if(!strcmp(cc1,PINFO[i].name))Him=PINFO[i].ColorID;
			};
			if(Him==-1)return;
			while(sss[0]==' ')sss++;
			int Amn;
			int z=sscanf(sss,"%d",&Amn);
			if(z!=1)return;
			CmdGiveMoney(MyNation,Him,res,Amn);
		};
		ChatString[0]=0;
		return;
	};
	if(LastKey==VK_BACK){
		if(strlen(ChatString)>0)ChatString[strlen(ChatString)-1]=0;
		return;
	}else{
		LastKey=LastAscii;
		if(LastKey){
		
			char xx[2];
			xx[1]=0;
			xx[0]=char(LastKey);
			if(strlen(ChatString)<80)strcat(ChatString,xx);
		};
	};
	if(NKeys)goto KRT;
	*/
};
extern char CHATSTRING[256];
extern DWORD CHATDPID;
void ShowMultiplayerChat();
void ShowChat(){
	if(NPlayers>0){
		ShowMultiplayerChat();
		return;
	};
	if(EnterChatMode){
		int y0=smapy+(smaply<<(Shifter-2));
		int x0=smapx+(smaplx<<(Shifter-1));
		int Dy=GetRLCHeight(BigYellowFont.RLC,'y')+5;
		int L0=GetRLCStrWidth(ENCHAT,&BigYellowFont);
		int L=(GetRLCStrWidth(ChatString,&YellowFont)+L0)>>1;
		if(L<100)L=100;
		DrawStdBar(x0-L-16,y0-8,x0+L+16,y0+20+7);
		ShowString(x0-L-4,y0-5,ENCHAT,&BigWhiteFont);
		if((GetRealTime()/200)&1)ShowString(x0-L+L0+4,y0-4+7,ChatString,&YellowFont);
		else{
			int xx=0;
			for(int i=0;i<ChatCursPos;i++)xx+=GetRLCWidth(YellowFont.RLC,ChatString[i]);
			//sprintf(cc2,"%s|",ChatString);
			ShowString(x0-L+L0+4,y0-4+7,ChatString,&YellowFont);
			ShowString(x0-L+L0+xx+4,y0-4+7,"|",&YellowFont);
		};
	};
	if(CHATSTRING[0]){
		for(int i=0;i<NPlayers;i++)if(PINFO[i].PlayerID==CHATDPID){
			char cc1[250];
			sprintf(cc1,"%s: %s",PINFO[i].name,CHATSTRING);
			AssignHint1(cc1,250,16+NatRefTBL[PINFO[i].ColorID]);
			PlayEffect(GetSound("START"),0,0);
			CHATSTRING[0]=0;
			i=NPlayers;
		};
	};
};
int NGSPlayers;
char GSPLAYERS[9][32];
bool ProcessIndividualMessages(char* nick,char* message){
	if(GameInProgress){
		char cc3[1024];
		sprintf(cc3,GetTextByID("COMCHAT"),nick,message);
		AssignHint1(cc3,400,0);
		PlayEffect(GetSound("START"),0,0);
		if(NGSPlayers>7)return true;
		for(int i=0;i<NGSPlayers;i++){
			if(!_stricmp(GSPLAYERS[i],nick))return true;
		};
		strcpy(GSPLAYERS[NGSPlayers],nick);
		NGSPlayers++;
		return true;
	}else return false;
};

char* CHATHELP1=NULL;
char* CHATHELP2=NULL;
char* CHATHELP3=NULL;
void DrawDoubleTable2(int x0,int y0,int x1,int y1,int ym);
void ShowMultiplayerChat(){
	if(EnterChatMode){
		if(!CHATHELP1){
			CHATHELP1=GetTextByID("CHATHELP1");
			CHATHELP2=GetTextByID("CHATHELP2");
			CHATHELP3=GetTextByID("CHATHELP3");
		};
		int DDD=20;
		int y0=smapy+(smaply<<(Shifter-2))-50-DDD/2;
		int x0=smapx+(smaplx<<(Shifter-1));
		int Dy=GetRLCHeight(BigYellowFont.RLC,'y')+5;
		int L0=GetRLCStrWidth(ENCHAT,&BigYellowFont);
		int L=GetRLCStrWidth(ChatString,&YellowFont)+L0;
		int L1=GetRLCStrWidth(CHATHELP1,&SmallWhiteFont);
		int L2=GetRLCStrWidth(CHATHELP2,&SmallWhiteFont);
		if(L2>L1)L1=L2;
		char cc3[256];
		for(int i=0;i<NGSPlayers;i++){
			sprintf(cc3,CHATHELP3,i+1,GSPLAYERS);
			L2=GetRLCStrWidth(cc3,&SmallWhiteFont);
			if(L2>L1)L1=L2;
		};
		if(L<L1)L=L1;
		L>>=1;
		if(L<100)L=100;
		//DrawStdBar(x0-L-16,y0-8,x0+L+16,y0+20+7);
		DrawDoubleTable2(x0-L-16,y0-8,x0+L+16,y0+20+7+DDD*(2+NGSPlayers)+16,y0+20+7);
		ShowString(x0-L-4,y0-5-8,ENCHAT,&BigWhiteFont);
		if((GetRealTime()/200)&1)ShowString(x0-L+L0+4,y0-4+7,ChatString,&YellowFont);
		else{
			int xx=0;
			for(int i=0;i<ChatCursPos;i++)xx+=GetRLCWidth(YellowFont.RLC,ChatString[i]);
			//sprintf(cc2,"%s|",ChatString);
			ShowString(x0-L+L0+4,y0-4+7,ChatString,&YellowFont);
			ShowString(x0-L+L0+xx+4,y0-4+7,"|",&YellowFont);
		};
		ShowString(x0-GetRLCStrWidth(CHATHELP1,&SmallYellowFont)/2,y0+20+7+6+DDD/2,CHATHELP1,&SmallYellowFont);
		ShowString(x0-GetRLCStrWidth(CHATHELP2,&SmallYellowFont)/2,y0+20+7+6+DDD/2+DDD,CHATHELP2,&SmallYellowFont);
		for(int i=0;i<NGSPlayers;i++){
			sprintf(cc3,CHATHELP3,i+1,GSPLAYERS[i]);
			L2=GetRLCStrWidth(cc3,&SmallWhiteFont);
			ShowString(x0-L2/2,y0+20+7+6+DDD/2+DDD*(i+2),cc3,&SmallYellowFont);
		};
	};
	if(CHATSTRING[0]){
		for(int i=0;i<NPlayers;i++)if(PINFO[i].PlayerID==CHATDPID){
			char cc1[250];
			sprintf(cc1,"%s: %s",PINFO[i].name,CHATSTRING);
			AssignHint1(cc1,250,16+NatRefTBL[PINFO[i].ColorID]);
			PlayEffect(GetSound("START"),0,0);
			CHATSTRING[0]=0;
			i=NPlayers;
		};
	};
};
bool CheckFNSend(int idx){
    if (!EnterChatMode)return false;

    if (idx < NGSPlayers) {

        /*if (TPEN.Peer && TPEN.Connected && ChatString[0]) {

            peerMessagePlayer(TPEN.Peer, GSPLAYERS[idx], ChatString);

            char cc3[1024];

            sprintf(cc3, GetTextByID("CHATHELP4"), GSPLAYERS[idx], ChatString);

            AssignHint1(cc3, 100, 0);

            ChatString[0] = 0;

        };*/

        EnterChatMode = 0;

        return true;

    };

    return false;
};
void GetTimeString(int t,char* str){
	int h=t/3600;
	int m=(t/60)%60;
	int s=(t%60);
	sprintf(str,"%d:%d%d:%d%d",h,m/10,m%10,s/10,s%10);
};
int SortPlayers(byte* Res,int* par);
void ShowVictInFormat(char* Res,int idx,byte Type);
/*void SendInfoString(char* nick) {

    if (!(TPEN.Peer && TPEN.Connected && !TPEN.ChatConnected))return;

    char BIGINFO[2048];

    sprintf(BIGINFO, "@@@GMRIN %s\\", TPEN.HostMessage);

    switch (PINFO[0].VictCond) {

    case 0:

        strcat(BIGINFO, "Total Destruction\\");

        break;

    case 1:

        strcat(BIGINFO, "Territory Capture ");

        GetTimeString(PINFO[0].GameTime, BIGINFO + strlen(BIGINFO));

        strcat(BIGINFO, "\\");

        break;

    case 2:

        strcat(BIGINFO, "Game on score ");

        GetTimeString(PINFO[0].GameTime, BIGINFO + strlen(BIGINFO));

        strcat(BIGINFO, "\\");

        break;

    default:

        return;

    };

    byte ord[8];

    int  par[8];

    int no = SortPlayers(ord, par);

    byte prevms = 0;

    int teamidx = 0;

    if (no) {

        byte prevms = 0;

        for (int q = 0; q < no; q++) {

            byte ms = NATIONS[ord[q]].NMask;

            if (!(ms & prevms))teamidx++;

            prevms = ms;

            char ccr[64];

            ShowVictInFormat(ccr, ord[q], PINFO[0].VictCond);

            sprintf(BIGINFO + strlen(BIGINFO), "%d %s (%s)\\", teamidx, GetPName(ord[q]), ccr);

        };

    };

    if (strlen(BIGINFO) < 200)peerMessagePlayer(TPEN.Peer, nick, BIGINFO);

};*/
#include "Dialogs\EditorToolsPanel.h"
//-----------------Clans support----------------//
struct ClanInfo{
	char* ClanName;
	SQPicture Passive;
	SQPicture Active;
	SQPicture Over;
};
ClanInfo* CLINFO=NULL;
int NClans=0;
extern bool SafeLoad;
void ReadClanData(){
	byte pset[256];
	ResFile F=RReset("1\\old2new.set");
	if(F==INVALID_HANDLE_VALUE)return;
	RBlockRead(F,pset,256);
	RClose(F);
	SafeLoad=1;
	WIN32_FIND_DATA FD;
	HANDLE H=FindFirstFile("Clans\\*.clan",&FD);
	if(H!=INVALID_HANDLE_VALUE){
		do{
			char ccc[128];
			sprintf_s(ccc,sizeof(ccc),"Clans\\%s",FD.cFileName);
			FILE* F=fopen(ccc,"r");
			if(F){
				char cc1[128];
				char cc2[128];
				char cc3[128];
				int z=fscanf(F,"%s%s%s",cc1,cc2,cc3);
				if(z==3){
					SQPicture P1;
					sprintf_s(ccc,sizeof(ccc),"Clans\\%s",cc1);
					P1.LoadPicture(ccc);
					if(P1.GetLx()<256&&P1.GetLy()==20){
						sprintf_s(ccc,sizeof(ccc),"Clans\\%s",cc2);
						P1.LoadPicture(ccc);
						if(P1.GetLx()<256&&P1.GetLy()==20){
							sprintf_s(ccc,sizeof(ccc),"Clans\\%s",cc3);
							P1.LoadPicture(ccc);
							if(P1.GetLx()<256&&P1.GetLy()==20){
								CLINFO=(ClanInfo*)realloc(CLINFO,(NClans+1)*sizeof ClanInfo);
								memset(CLINFO+NClans,0,sizeof ClanInfo);
								sprintf_s(ccc,sizeof(ccc),"Clans\\%s",cc1);
								CLINFO[NClans].Over.LoadPicture(ccc);
								CLINFO[NClans].Over.Xlat(pset);
								sprintf_s(ccc,sizeof(ccc),"Clans\\%s",cc2);
								CLINFO[NClans].Active.LoadPicture(ccc);
								CLINFO[NClans].Active.Xlat(pset);
								sprintf_s(ccc,sizeof(ccc),"Clans\\%s",cc3);
								CLINFO[NClans].Passive.LoadPicture(ccc);
								CLINFO[NClans].Passive.Xlat(pset);
								strcpy(ccc,FD.cFileName);
								char* ccx=strstr(ccc,".clan");
								if(!ccx)ccx=strstr(ccc,".CLAN");
								if(!ccx)ccx=strstr(ccc,".Clan");
								if(ccx)ccx[0]=0;
								CLINFO[NClans].ClanName=znew(char,strlen(ccc)+1);
								strcpy(CLINFO[NClans].ClanName,ccc);
								NClans++;
							};
						};
					};
				};
				fclose(F);
			};
		}while(FindNextFile(H,&FD));
		FindClose(H);
	};
	SafeLoad=0;
	//sorting
	bool change;
	ClanInfo CIN;
	do{
		change=0;
		for(int i=1;i<NClans;i++){
			if(strlen(CLINFO[i-1].ClanName)<strlen(CLINFO[i].ClanName)){
				CIN=CLINFO[i-1];
				CLINFO[i-1]=CLINFO[i];
				CLINFO[i]=CIN;
				change=1;
			};
		};
	}while(change);
	memset(&CIN,0,sizeof CIN);

};
CEXPORT
void ShowClanString(int x,int y,char* s,byte State,RLCFont* Fn,RLCFont* Fn1,int DY){
	char ccc[64];
	char NICK[128];
	char CLAN[128];
	strcpy(NICK,s);
	_strupr(NICK);
	strcpy(ccc,s);
	for(int i=0;i<NClans;i++){
		strcpy(CLAN,CLINFO[i].ClanName);
		_strupr(CLAN);
		char* cc=strstr(NICK,CLAN);
		if(cc&&cc==NICK){
			//ok, found!
			cc=ccc;
			cc+=strlen(CLINFO[i].ClanName);
			SQPicture* p;
			switch(State){
			case 0:
				p=&CLINFO[i].Passive;
				break;
			case 1:
				p=&CLINFO[i].Active;
				break;
			case 2:
				p=&CLINFO[i].Over;
				break;
			};
			p->Draw(x,y-2);
			if(Fn1)ShowString(x+p->GetLx()+1+3,y+DY+1,cc,Fn1);
			ShowString(x+p->GetLx()+3,y+DY,cc,Fn);
			return;
		};
	};
	if(Fn1)ShowString(x+1,y+DY+1,s,Fn1);
	ShowString(x,y+DY,s,Fn);
	return;
};
//--------------DIPLOMACY IMPLEMENTATION-------------
#ifdef SCREENFIX
DialogsSystem DIP_DSS(menu_x_off, menu_y_off);
#else
DialogsSystem DIP_DSS(0, 0);
#endif
bool DIP_DSS_Init=0;
bool DIP_DSS_Vis=0;
char CUR_Request[256]="LF|LW_new|open&req1.txt";
void RunExplorer(int Index,char* ref,int x,int y,int x1,int y1);
int LastRealLX=-1;
void SetupDIP_DSS(){
	LastRealLX=RealLx;
	DIP_DSS.CloseDialogs();
	int DD1=75;
	int D=18;
	int x0=RealLx-InfDX;
	int y0=InfDY;
	DIP_DSS.BaseX=x0-1;
	DIP_DSS.BaseY=y0+31;
	//DIP_DSS.addBorder(0,-31,InfDX-10,RealLy-380,0,1);
	DIP_DSS.addTextButton(NULL,10,-29,GetTextByID("DIP_HDR"),&YellowFont,&YellowFont,&YellowFont,0);
	RunExplorer(1,CUR_Request,DIP_DSS.BaseX-2,DIP_DSS.BaseY-2,DIP_DSS.BaseX+InfDX-10+3,RealLy-298);
};
void ExplorerResize(int Index,int x,int y,int x1,int y1);
extern int InformMode;
int PrevDipDL=0;
extern int DipDlgDX;
void ProcessDIP_DSS(){
	if(RealLx!=LastRealLX||PrevDipDL!=InfDX){
		LastRealLX=RealLx;
		DIP_DSS.CloseDialogs();
		int DD1=75;
		int D=18;
		int x0=RealLx-InfDX;
		int y0=InfDY;
		DIP_DSS.BaseX=x0-1;
		DIP_DSS.BaseY=y0+31;
		//DIP_DSS.addBorder(0,-31,InfDX-10,RealLy-380,0,1);
		//DIP_DSS.addTextButton(NULL,10,-24,GetTextByID("DIP_HDR"),&YellowFont,&YellowFont,&YellowFont,0);
		ExplorerResize(1,DIP_DSS.BaseX-2,DIP_DSS.BaseY-2,DIP_DSS.BaseX+InfDX-10+3,InfY1-27);
	};
	PrevDipDL=InfDX;
	if((!Inform)||(Inform&&InformMode!=4)||(EditMapMode&&!OptHidden))return;
	if(NoPress&&mouseX>RealLx-InfDX-32)return;
	if(!DIP_DSS_Init){
		DIP_DSS_Init=1;
		SetupDIP_DSS();
	};
	if(DIP_DSS_Vis){
		DIP_DSS.ProcessDialogs();
		ProcessExplorer(1);
	};
};
CEXPORT
void ActivateDipDialog(char* request){
	Inform=1;
	InformMode=4;
	OptHidden=1;
	DIP_DSS_Vis=1;
	strcpy(CUR_Request,request);
	if(!DIP_DSS_Init){
		DIP_DSS_Init=1;
		SetupDIP_DSS();
	}else RunExplorer(1,CUR_Request,DIP_DSS.BaseX-2,DIP_DSS.BaseY-2,DIP_DSS.BaseX+InfDX-10+3,InfY1-27);
};
CEXPORT
void ActivateCMLInterface(int Idx,char* request){
	RunExplorer(Idx,request,0,0,RealLx,RealLy);
};
CEXPORT
void DrawCMLInterface(int Idx){
	ProcessExplorer(Idx);		
};
CEXPORT
void ChangeDipDialog(char* request){
	strcpy(CUR_Request,request);
	if(!DIP_DSS_Init){
		DIP_DSS_Init=1;
		SetupDIP_DSS();
	}else RunExplorer(1,CUR_Request,DIP_DSS.BaseX-2,DIP_DSS.BaseY-2,DIP_DSS.BaseX+InfDX-10+3,InfY1-27);
};
extern int InformMode;
CEXPORT
void PopupDipDialog(){
	Inform=1;
	OptHidden=1;
	DIP_DSS_Vis=1;
	InformMode=4;
};
extern int InfDX0;
void FlipDipDialog(){
	if(Inform||!OptHidden)DIP_DSS_Vis=0;
	if(!DIP_DSS_Vis){
		Inform=1;
		InformMode=4;
		OptHidden=1;
		DIP_DSS_Vis=1;
	}else{
		Inform=0;
		DIP_DSS_Vis=0;
		InfDX=InfDX0;
	};
};
void HideDipDialog(){
	Inform=0;
	DIP_DSS_Vis=0;
	InfDX=InfDX0;
};
void CmdSendData(byte* Data,int size);
byte* DIP_Data[64];
int DIP_DSize[64];
int NDipItems=0;
CEXPORT
void SendDipCommand(char* Data,int size){
	if(NDipItems<64){
		DIP_Data[NDipItems]=(byte*)malloc(size);
		memcpy(DIP_Data[NDipItems],Data,size);
		DIP_DSize[NDipItems]=size;
		NDipItems++;
	};
	//CmdSendData((byte*)Data,size);
};
//#define AC_DEMO
__declspec(dllexport)
bool CheckUsingAI(){
	for(int i=0;i<8;i++)if(NATIONS[i].AI_Enabled)return true;
	return false;
};
//#define  EDITORONLY
bool PIEnumeratePlayers(PlayerInfo* PIN,bool DoMsg);
CEXPORT bool EnumPlr(){
	return PIEnumeratePlayers(PINFO,false);
};
extern int curptr;
extern int MouseNext[256];
int prevptr=-1;
CEXPORT
void SetCurPtr(int v){
	if(v==0&&curptr)prevptr=curptr;
	else curptr=prevptr;
	int N=30;
	int v0=v;
	do{
		v0=MouseNext[v0];
		if(v0==curptr)return;
		N--;
	}while(N);
	curptr=v;
};
void DelFinSpaces(char* s){
	int L=strlen(s);
	while(L>0&&s[L-1]==' ')L--;
	s[L]=0;
};
void ParseMP3_String(char* src,char* txt,char* mp3){
	char* ms=strstr(src,"MP3:");
	if(ms){
		strcpy(txt,src);
		txt[ms-src]=0;
		ms+=4;
		while(ms[0]==' ')ms++;
		strcpy(mp3,ms);
	}else{
		strcpy(txt,src);
		mp3[0]=0;
	};
	DelFinSpaces(txt);
	DelFinSpaces(mp3);
};
char* GetCurrentMP3File();
void StopPlayingMP3();
void PlayMP3File(char* FileName);
BOOL mpl_IsStreamFinished(void);
DWORD ov_GetStreamLength(void);

#include "Dialogs\ProcessSingleMission.h"
#include "Dialogs\ProcessSingleCampagin.h"
#include "Dialogs\EnterName.h"
#include "Dialogs\MainMenu.h"
#include "Dialogs\MenuOptions.h"
#include "Dialogs\ProcessCampagins.h"
#include "Dialogs\ProcessMultiplayer.h"
#include "Dialogs\ProcessSinglePlayer.h"
#include "Dialogs\SelectProtocol.h"
#include "Dialogs\SingleOptions.h"
#include "Dialogs\MultiplayerRoom.h"
#include "Dialogs\SelectSingleMission.h"
#include "Dialogs\MLoadGame.h"
#include "Dialogs\InGameLoadGame.h"
#include "Dialogs\InGameSaveGame.h"
#include "Dialogs\GameMainMenu.h"
#include "Dialogs\EditMainMenu.h"
#include "Dialogs\ProcessLoadingFile.h"
#include "Dialogs\ProcessSavingFile.h"
#include "Dialogs\SelectGameToJoin.h"
#include "Dialogs\GameOptions.h"
#include "Dialogs\HistoryBattle.h"
#include "Dialogs\Messages.h"
#include "Dialogs\PostCampaignMessage.h"
#include "Dialogs\SelectSingleBattle.h"