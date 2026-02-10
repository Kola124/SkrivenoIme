/*==========================================================================
 *
 *  Copyright (C) 1997-1998 Andrew(GSC). All Rights Reserved.
 *
 ***************************************************************************/
#define NAME "CEW_KERNEL"
#ifndef _USE3D
#define TITLE "American Conquest"
#endif // !_USE3D
//#define VIEW_TOP
//#define WIN32_LEAN_AND_MEAN
#include "ddini.h"
#ifdef STEAM
#include "./Steam/steam_api.h"
#endif

bool window_mode;
bool bStretchMode;
int screen_width;
int screen_height;
double screen_ratio;
DWORD window_style = WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

#include "wintab\wintab.h"
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
#include "dialogs.h"
#include <assert.h>
#include "3DSurf.h"
#include "CDirSnd.h"
#include "GSound.h"
#include "Video.h"
#include "dplay.h"
#include "MapSprites.h"
#include "VirtScreen.h"
#include <crtdbg.h>
#include "RealWater.h"
#include "Transport.h"
#include "AntiBug.h"
#include "3DBars.h"
#include "TopoGraf.h"
#include "StrategyResearch.h"
#include "Towers.h"
#include "Safety.h"
#include "NewAI.h"
#include "Danger.h"
#include "Sort.h"
#include "Recorder.h"
#include "MPlayer.h"
#include "EinfoClass.h"
#include "3DmapEd.h"
#include "ActiveScenary.h"
#include "fonts.h"
#include "diplomacy.h"
#define INDDEX1
#include "GP_Draw.h"
#include "Dialogs\InitFonts.h"
//#include "mp3\mpl.h"
#include "mp3\oggvor.h"
#include "Path.h"
#include "vital\vital_def.h"

#ifdef _USE3D
#define TITLE "Cossacks2"
#include "..\3DLIB\gMotor\mCamera.h"
#endif // _USE3D

void CreateMapShot();
extern bool GameInProgress;
extern BlockBars LockBars;
extern BlockBars UnLockBars;
int DrawPixMode=0;
int DrawGroundMode=0;
int CurGroundTexture=0;
#include "FEX\FEXModule.h"
//#define GERMANVERSION
extern bool LockPause;
extern bool Recreate;
extern bool MiniActive;
void CmdChangeSpeed();
int FrmDec=2;
int SpeedSh=1;
void CheckGP();
void SelectAllBuildings(byte NI);
int LASTRAND,LASTIND;
void EraseAreas();
byte PauseMode=0;
bool VHMode=0;
int LastAttackDelay=0;
void ProcessFishing();
void RenderAllMap();
char* FormationStr=NULL;
extern char SaveFileName[128];
extern bool SetDestMode;
bool FastMode=false;
int GoldID;
int FoodID;
int StoneID;
int TreeID;
int CoalID;
int IronID;
CEXPORT char BuildVersion[32];
extern bool GoAndAttackMode;
extern int Inform;
extern int ShowGameScreen;
extern byte ScanPressed[256];
extern bool AttGrMode;

//Last used display resolutions for both modes
int exRealLx, exRealLy;
int ex_other_RealLx, ex_other_RealLy;//Necessary for saving settings

void InitFishMap();
void LoadMessages();
/*int exRealLx;
int exRealLy;*/
int exFMode=1;
extern bool FullScreenMode;
extern bool GameNeedToDraw;
void GSSetup800();
void ClearMaps();
void CreateTotalLocking();
void CreateUnitsLocking();
bool TransMode;
byte LockGrid;
byte TopTypeMode=0;
bool PeaceMode;
byte LockMode;
void ProcessSprites();
void Reset3D();
void Init3DMapSystem();
void CreateRandomHMap();
void SetLight(int Ldx,int Ldy,int Ldz);
static int Light=0;
int HiStyle;
extern bool ChangeNation;
extern bool MultiTvar;
extern DPID MyDPID;
extern DPID ServerDPID;
extern int curdx;
extern int curdy;
void Loadtextures();
void TestTriangle();
extern bool CheapMode;
extern int WarSound;
extern int WorkSound;
extern int OrderSound;
extern int MidiSound;
#ifndef SPEEDFIX
extern int FPSTime;
#endif
extern int NMyUnits;
extern int NThemUnits;
void LoadNewAimations();
void MFix();
void WinnerControl(bool);
bool Fixed;

#ifdef SPEEDFIX

//Timespan in ms after last LastCTRLPressTime which allows setting unit control groups
const int kCtrlStickyTime = 50;

//Minimal delay between two PostDrawGameProcess() returns, in ms
const unsigned int kPostDrawInterval = 33;//~60 Hz

//Time of the last PostDrawGameProcess() return 
unsigned long prev_postdraw_time = 0;

#endif

bool MUSTDRAW;
bool SHOWSLIDE=true;
int  HISPEED=0;
int Shifter;
int Multip;
bool MiniMode;
int AutoTime;
extern int sfVersion;
void OnMouseMoveRedraw();
void GetForces();
bool HealthMode;
#define TIMER_ID        1
#define TIMER_RATE      50
extern int MenuType;
extern bool MakeMenu;
extern byte SpecCmd;
extern int RealLx;
extern int RealLy;
extern bool BuildMode;
extern bool CINFMOD;
extern word rpos;
extern bool realLpressed;
extern bool realRpressed;
extern int MaxAllowedComputerAI;
extern bool GetCoord;
extern bool InGame;
extern bool InEditor;
extern int FogMode;
extern CSDLSound CDIRSND;  // Global instance
extern CSDLSound* CDS;
extern int RealPause;
void ProcessUFO();
void SaveGame(char* fnm,char* gg,int ID);
SFLB_DLLEXPORT SFLB_LoadGame(char* fnm,bool LoadNation);
void makeFden();
bool FullMini=true;
//byte Quality;
HugeExplosion HE;
bool FASTMODE;
SFLB_DLLEXPORT InitDialogs();
int processMainMenu();
//+¨óđýř÷đˇř  ˙đ¨đűűőűąýţ ňv˙ţűý őüvż ÷đôđ˘
typedef void EventHandPro(void*);
void HandleMultiplayer();
bool EgoFlag;
int tmtmt;
int REALTIME;
bool MEditMode;
bool EditMapMode;
int HeightEditMode;
bool HelpMode;
bool ChoosePosition;
int  ReliefBrush;
int  CostThickness;
int  TerrBrush;
int BlobMode;
int MaxSizeX;
int MaxSizeY;
byte EditMedia;
extern int curptr;
void InitDestn();
Menu MainMenu;
Menu Options;
Menu LoadFile;
Menu About;
word Creator;
Nation WEP;
Weapon FlyFire1;
Weapon Vibux1;
Weapon Fire1;
Weapon Lasso;
Weapon Molnia;
Weapon Sphere;
Weapon MolniaVibux;
Weapon Arrow;
Weapon Flystar;
Weapon Magvib;
byte PlayerMask;
City CITY[8];

/*City CITY1;
City CITY2;
City CITY3;
City CITY4;
City CITY5;
City CITY6;
City CITY7;*/
static word MsPerFrame=40;
int Flips;
CEXPORT int LastKey;
CEXPORT bool KeyPressed;
int NMONS;
bool DeathMode;
bool AttackMode;
bool InfoMode;
int WaterEditMode;
void SetEgoFlag(bool EgoMod);
extern word PlayerMenuMode;
WIN32_FIND_DATA findfi;
struct EventsTag{
	EventHandPro* Pro;
	int	Type;
	int	Handle;
	bool Blocking;
	void* Param;
};
#define maxTask 32
void PlayerMenuWork();
int GetResID(char*);
EventsTag Events[maxTask];
void CmdSetSpeed(byte Speed);
int RegisterEventHandler(EventHandPro* pro,int Type,void* param){
	int i;
	for(i=0;int(Events[i].Pro)&&i<maxTask;i++);
	if(i>=maxTask)return -1;
	Events[i].Pro=pro;
	Events[i].Type=Type;
	Events[i].Handle=i;
	Events[i].Blocking=false;
	Events[i].Param=param;
	return i;
};
void CloseEventHandler(int i){
	memset(&Events[i],0,sizeof Events[i]);
};

HWND                hwnd;
RLCTable RCross;
RLCTable mRCross;
//fonts

int cadr;
int tima;
int tmtim;
/*
 * Timer Callback 
 */
int xxx;
void ShowFon1();
void WaterCorrection();
extern bool TexMapMod;
extern bool RiverEditMode;
void ClearCurve();
extern bool TexPieceMode;
extern int DrawPixMode;
void SetRoadsEditMode(bool Rmode);
void ClearModes(){
	DrawPixMode=0;
	DrawGroundMode=0;
	HeightEditMode=false;
	MEditMode=false;
	LockMode=0;
	WaterEditMode=false;
	SetWallBuildMode(0xFF,0);
	TexMapMod=false;
	RiverEditMode=0;
	ClearCurve();
	TexPieceMode=0;
	SetRoadsEditMode(0);
};
void TimerProc(void)
{
	
	//LockSurface();
	//FlipPages();
	
	if(PlayerMenuMode==1){
		ShowFon1();
		for(int j=0;j<maxTask;j++)
		if(Events[j].Pro)(*(Events[j].Pro))(Events[j].Param);
		SetRLCWindow(0,1,MaxSizeX,RSCRSizeY-1,ScrWidth);
		//-ShowMPtr();
	}else{
		//ShowMPtr();
		//ShowMap();
		SetRLCWindow(0,1,MaxSizeY,RSCRSizeY-1,ScrWidth);
		//-ShowMPtr();
	};
	HandleMouse(mouseX,mouseY);
	MFix();
	FlipPages();
	//UnlockSurface();
	
	
}
//Loading...
void LoadEconomy();
void LoadNations();
void LoadWeapon();
void LoadNation(char* fn, byte msk, byte NIndex, byte NatID);
void LoadAllNewMonsters();
void InitNewMonstersSystem();
void LoadWaveAnimations();
extern NewAnimation* Shar;
void DoGen();
void InitDeathList();
GFILE* F=NULL;
void OpenMemReport(){
	//F=Gopen("mem.lst","w");
};
void CloseMemReport(){
	//Gclose(F);
	F=NULL;
};
void MemReport(char* str){
	/*
	if(!F)OpenMemReport();
	_CrtMemState M_state;
	_CrtMemCheckpoint(&M_state);
	Gprintf(F,"%s : %d\n" ,str,M_state.lSizes[1]);
	*/
};
char* GetTextByID(char* ID);
void LoadBorders();
extern int ddd;
void SetupArrays();
extern byte* RivDir;
void Init_GP_IMG();
void ReadClanData();
void ReadMoraleData();
void LoadCTG();
void ReadChainObjectsDesc();

//Calculates window coordinates and locks cursor inside client area
void ClipCursorToWindowArea()
{
    if (!window_mode)
    {
        // In fullscreen, SDL handles cursor clipping
        return;
    }

    if (!InGame && !InEditor)
    {
        ClipCursor(NULL);
        return;
    }

    RECT client_coords;
    GetClientRect(hwnd, &client_coords);
    MapWindowPoints(hwnd, NULL, (LPPOINT)&client_coords, 2);

    client_coords.right--;
    client_coords.bottom--;

    ClipCursor(&client_coords);
}

void ResizeAndCenterWindow()
{
    if (!window_mode)
    {//Just in case
        return;
    }

    RECT window_size;
    window_size.top = 0;
    window_size.left = 0;
    window_size.right = RealLx;
    window_size.bottom = RealLy;
    AdjustWindowRect(&window_size, window_style, FALSE);

    int width = window_size.right - window_size.left;
    int height = window_size.bottom - window_size.top;

    int x = screen_width / 2 - width / 2;
    int y = screen_height / 2 - height / 2;

    if (x < 0)
    {
        x = 0;
    }
    if (y < 0)
    {
        y = 0;
    }

    MoveWindow(hwnd, x, y, width, height, TRUE);

    ClipCursorToWindowArea();

    SetCursorPos(screen_width / 2, screen_height / 2);
}

bool Loading()
{
	LoadCTG();
	ReadMoraleData();
	ReadClanData();
	//SetupArrays();
	RivDir=NULL;
	MemReport("Start");
	Init_GP_IMG();
	DWORD SectorsPerCluster;
	DWORD BytesPerSector; 
	DWORD NumberOfFreeClusters; 
	DWORD TotalNumberOfClusters;
	char CRDIR[200];
	GetWindowsDirectory(CRDIR,200);
	CRDIR[3]=0;
	
	//GetDiskFreeSpaceEx(CRDIR,&SectorsPerCluster,
	//							&BytesPerSector, 
	//							&NumberOfFreeClusters,
	//							&TotalNumberOfClusters);
	bool waserror=0;
	GetCurrentDirectory(200,CRDIR);
	CRDIR[3]=0;
	//GetDiskFreeSpace(CRDIR,&SectorsPerCluster,
	//							&BytesPerSector, 
	//							&NumberOfFreeClusters,
	//							&TotalNumberOfClusters);
	InitDeathList();
	MemReport("InitDeathList");
	InitNewMonstersSystem();
#ifdef COSSACKS2
	ReadChainObjectsDesc();
#endif //COSSACKS2
	//int GPID=GPS.PreLoadGPImage("romw");
	
	InitFonts();
	LoadBorders();
	MemReport("InitNewMonstersSystem");
	//LoadMessages();
	MemReport("LoadMessages");
	LoadNations();
	MemReport("LoadNations");
	LoadFon();
	MemReport("LoadFon");
	LoadRDS();
	MemReport("LoadRDS");

	GoldID=GetResID("GOLD");
	FoodID=GetResID("FOOD");
	StoneID=GetResID("STONE");
	TreeID=GetResID("WOOD");
	CoalID=GetResID("COAL");
	IronID=GetResID("IRON");
	LoadEconomy();
	MemReport("LoadEconomy");

	Loadtextures();
	MemReport("LoadTextures");
	LoadFog(1);
	MemReport("LoadFog");
	LoadTiles();
	MemReport("LoadTiles");
	//LoadWalls();
	LoadLock();
	MemReport("LoadLock");
	//LoadMap("map13h.mpf");
	LoadPointer("cursor_y.rlc");
	MemReport("LoadPointer");
	LoadNewAimations();
	MemReport("LoadNewAnimations");
	LoadWeapon();
	MemReport("LoadWeapon");
	InitExplosions();
	MemReport("InitExplosions");
    InitSprites();
	MemReport("InitSprites");
	//DoGen();
	MemReport("DoGen");
	LoadAllWalls();
	MemReport("LoadAllWalls");
    LoadAllNewMonsters();
	MemReport("LoadAllNewMonsters");
	LoadWaveAnimations();
#ifdef GERMANVERSION
	ResFile F=RReset("romw.gp");
	int sz=RFileSize(F);
	byte* buf=znew(byte,sz);
	RBlockRead(F,buf,sz);
	RClose(F);
#endif
	MemReport("LoadWaveAnimations");
	LoadAllNations(1,0);
	MemReport("LoadAllNations0");
	LoadAllNations(2,1);
	MemReport("LoadAllNations1");
	LoadAllNations(4,2);
	MemReport("LoadAllNations2");
	LoadAllNations(8,3);
	MemReport("LoadAllNations3");
	LoadAllNations(16,4);
	MemReport("LoadAllNations4");
	LoadAllNations(32,5);
	MemReport("LoadAllNations5");
	LoadAllNations(64,6);
	MemReport("LoadAllNations6");
	LoadAllNations(128,7);
	MemReport("LoadAllNations7");
#ifdef GERMANVERSION
	int SM=0;
	for(int p=0;p<sz;p++)SM+=buf[p];
	free(buf);
#endif
	CITY[0].CreateCity(0);
	MemReport("CreateCity0");
	CITY[1].CreateCity(1);
	MemReport("CreateCity1");
	CITY[2].CreateCity(2);
	MemReport("CreateCity2");
	CITY[3].CreateCity(3);
	MemReport("CreateCity3");
	CITY[4].CreateCity(4);
	MemReport("CreateCity4");
	CITY[5].CreateCity(5);
	MemReport("CreateCity5");
	CITY[6].CreateCity(6);
	MemReport("CreateCity6");
	CITY[7].CreateCity(7);
	MemReport("CreateCity7");
	InitTopChange();
	MemReport("InitTopChange");
	LoadPalettes();
	MemReport("LoadPalettes");
	InitPrpBar();
	MemReport("InitPrpBar");
	SetMyNation(0);
#ifdef GERMANVERSION
	ddd=SM-1101909;
#endif
	//InitSafeInfo();
	MemReport("InitSafeInfo");
	//InitSuperMortiraCells();
	MemReport("InitSuperMortiraCells");
	//CreateEnmBuildList();
	MemReport("CreateEnmBuildList");
	//CreateProtectionMap();
	FormationStr=GetTextByID("FORMATION");
	//InitL3();
	return 1;
};
extern int CurPalette;
void SaveScreenShot(char* Name){
	byte PAL[1024];
	memset(PAL,0,1024);
	char ccx[120];
	sprintf(ccx,"%d\\agew_1.pal",CurPalette);
	ResFile f=RReset(ccx);
	for(int i=0;i<256;i++){
		int ofs=i<<2;
		RBlockRead(f,PAL+ofs+2,1);
		RBlockRead(f,PAL+ofs+1,1);
		RBlockRead(f,PAL+ofs,1);
	};
	RClose(f);
	f=RRewrite(Name);
	int i=0x4D42;
	RBlockWrite(f,&i,2);
	i=RealLx*RealLy+1080;
	RBlockWrite(f,&i,4);
	i=0;
	int j=0x436;
	RBlockWrite(f,&i,4);
	RBlockWrite(f,&j,4);
	j=0x28;
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&RealLx,4);
	RBlockWrite(f,&RealLy,4);
	j=0x080001;
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&i,4);
	RBlockWrite(f,&i,4);
	j=0x0B12;
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&i,4);
	RBlockWrite(f,&i,4);
	RBlockWrite(f,PAL,1024);
	for(int i=0;i<RealLy;i++){
		char* pos=(char*)(int(ScreenPtr)+(RealLy-i-1)*SCRSizeX);
		RBlockWrite(f,pos,RealLx);
	};
	RClose(f);
};
void SaveBMP8(char* Name,int lx,int ly,byte* Data){
	byte PAL[1024];
	memset(PAL,0,1024);
	char ccc[128];
    int i;
	sprintf_s(ccc,sizeof(ccc),"%d\\agew_1.pal",CurPalette);
	ResFile f=RReset(ccc);
	for(i =0;i<256;i++){
		int ofs=i<<2;
		RBlockRead(f,PAL+ofs+2,1);
		RBlockRead(f,PAL+ofs+1,1);
		RBlockRead(f,PAL+ofs,1);
	};
	RClose(f);
	f=RRewrite(Name);
	i=0x4D42;
	RBlockWrite(f,&i,2);
	i=lx*ly+1080;
	RBlockWrite(f,&i,4);
	i=0;
	int j=0x436;
	RBlockWrite(f,&i,4);
	RBlockWrite(f,&j,4);
	j=0x28;
	RBlockWrite(f,&j,4);
	int LX=lx;
	int LY=ly;
	RBlockWrite(f,&LX,4);
	RBlockWrite(f,&LY,4);
	j=0x080001;
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&i,4);
	RBlockWrite(f,&i,4);
	j=0x0B12;
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&i,4);
	RBlockWrite(f,&i,4);
	RBlockWrite(f,PAL,1024);
	for( i=0;i<LY;i++){
		char* pos=(char*)(int(Data)+(ly-i-1)*lx);
		RBlockWrite(f,pos,lx);
	};
	RClose(f);
};
void SaveMiniScreenShot(char* Name){
	byte PAL[1024];
	memset(PAL,0,1024);
	ResFile f=RReset("agew_1.pal");
	for(int i=0;i<256;i++){
		int ofs=i<<2;
		RBlockRead(f,PAL+ofs+2,1);
		RBlockRead(f,PAL+ofs+1,1);
		RBlockRead(f,PAL+ofs,1);
	};
	RClose(f);
	f=RRewrite(Name);
	int i=0x4D42;
	RBlockWrite(f,&i,2);
	i=RealLx*RealLy+1080;
	RBlockWrite(f,&i,4);
	i=0;
	int j=0x436;
	RBlockWrite(f,&i,4);
	RBlockWrite(f,&j,4);
	j=0x28;
	RBlockWrite(f,&j,4);
	int LX=RealLx>>2;
	int LY=RealLy>>2;
	RBlockWrite(f,&LX,4);
	RBlockWrite(f,&LY,4);
	j=0x080001;
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&i,4);
	RBlockWrite(f,&i,4);
	j=0x0B12;
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&j,4);
	RBlockWrite(f,&i,4);
	RBlockWrite(f,&i,4);
	RBlockWrite(f,PAL,1024);
	for(int i=0;i<LY;i++){
		char* pos=(char*)(int(ScreenPtr)+(RealLy-i*4-1)*SCRSizeX);
		for(int j=0;j<LX;j++)RBlockWrite(f,pos+j*4,1);
	};
	RClose(f);
};
void SaveScreen(){
	int i = 99;
	char ccc[128];
	CreateDirectory("Screenshots",0);
	for(i=0;i<1000;i++){
		sprintf_s(ccc,sizeof(ccc),"Screenshots\\screen%d.bmp",i);
		ResFile f=RReset(ccc);
		if(f==INVALID_HANDLE_VALUE){
			RClose(f);
			goto zzz;
		};
		RClose(f);
	};
zzz:
	sprintf_s(ccc,sizeof(ccc),"Screenshots\\screen%d.bmp",i);
	SaveScreenShot(ccc);
};
extern bool NoText;
extern bool SHOWSLIDE;
void GFieldShow();
void SaveMiniScreenShot(char* Name);
void MiniRenderAllMap(){
	SHOWSLIDE=true;
	int nx=div(msx,smaplx).quot;
	int ny=div(msy,smaply).quot;
	//if(nx>3)nx=3;
	//if(ny>3)ny=3;
	char ccc[128];
	NoText=true;
	for(int y=0;y<ny;y++){
		for(int x=0;x<nx;x++){
			mapx=x*smaplx;
			mapy=y*smaply;
			GFieldShow();
			FlipPages();
			int p=x+y*nx;
			if(p<10)sprintf_s(ccc,sizeof(ccc),"scr00%d.bmp",p);
			else if(p<100)sprintf_s(ccc,sizeof(ccc),"scr0%d.bmp",p);
			else sprintf_s(ccc,sizeof(ccc),"scr%d.bmp",p);
			SaveMiniScreenShot(ccc);
		};
	};
	NoText=false;
};
void RenderAllMap(){
	if(GetKeyState(VK_SHIFT)&0x8000){
		MiniRenderAllMap();
		return;
	};
	SHOWSLIDE=true;
	int nx=div(msx,smaplx).quot;
	int ny=div(msy,smaply).quot;
	//if(nx>3)nx=3;
	//if(ny>3)ny=3;
	char ccc[128];
	NoText=true;
	for(int y=0;y<ny;y++){
		for(int x=0;x<nx;x++){
			mapx=x*smaplx;
			mapy=y*smaply;
			GFieldShow();
			FlipPages();
			int p=x+y*nx;
			if(p<10)sprintf_s(ccc,sizeof(ccc),"scr00%d.bmp",p);
			else if(p<100)sprintf_s(ccc,sizeof(ccc),"scr0%d.bmp",p);
			else sprintf_s(ccc,sizeof(ccc),"scr%d.bmp",p);
			SaveScreenShot(ccc);
		};
	};
	NoText=false;
};
/*
 * finiObjects
 *
 * finished with all objects we use; release them
 */
static void finiObjects( void )
{
   FreeDDObjects();
} /* finiObjects */


void MyMCINotify(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
  if((wParam==MCI_NOTIFY_SUCCESSFUL)&&(lParam==LPARAM(wAVIDeviceID)))
  {
	  //CloseAVI(hwnd);
	  Lpressed=true;
  }
  if((wParam==MCI_NOTIFY_SUCCESSFUL)&&(lParam==LPARAM(wMIDDeviceID)))
  {
	  //CloseMIDI(hwnd);
  }
};

#define MaxQu 32
MouseStack MSTC[MaxQu];
MouseStack CURMS;
int NInStack=0;
void AddMouseEvent(int x,int y,bool L,bool R){
	if(NInStack<MaxQu){
		MSTC[NInStack].x=x;
		MSTC[NInStack].y=y;
		MSTC[NInStack].Lpressed=L;
		MSTC[NInStack].Rpressed=R;
		MSTC[NInStack].rLpressed=L;
		MSTC[NInStack].rRpressed=R;
		MSTC[NInStack].Control=(GetKeyState(VK_CONTROL)&0x8000)!=0;
		MSTC[NInStack].Shift=(GetKeyState(VK_SHIFT)&0x8000)!=0;
		NInStack++;
	};
};
int LastUMX=0;
int LastUMY=0;
int LastUTime=0;
MouseStack* ReadMEvent(){
	if(NInStack){
		CURMS=MSTC[0];
		if(NInStack>1)memcpy(MSTC,MSTC+1,(NInStack-1)*sizeof MouseStack);
		NInStack--;
		return &CURMS;
	};
	return NULL;
};
void ClearMStack(){
	NInStack=0;
};
extern bool unpress;
extern byte ScanPressed[256];
CEXPORT
void UnPress(){
	for(int i=0;i<NInStack;i++){
		MSTC[i].Lpressed=0;
		MSTC[i].Rpressed=0;
	};
	unpress=1;
	memset(ScanPressed,0,256);
	Lpressed=0;
	Rpressed=0;
};
extern int CurPalette;
LRESULT CD_MCINotify(WPARAM wFlags, LONG lDevId);
int SHIFT_VAL=0;
void HandleMouse(int x,int y);
extern bool PalDone;
byte KeyStack[32];
byte AsciiStack[32];
int NKeys=0;
//int KeyPos=0;
byte LastAsciiKey=0;
void AddKey(byte Key,byte Ascii){
	if(NKeys>=32){
		memcpy(KeyStack,KeyStack+1,31);
		memcpy(AsciiStack,AsciiStack+1,31);
		NKeys--;
	};
	KeyStack[NKeys]=Key;
	AsciiStack[NKeys]=Ascii;
	NKeys++;
};
byte LastAscii=0;
wchar_t last_unicode = 0;
int ReadKey(){
	if(NKeys){
		byte c=KeyStack[0];
		LastAscii=AsciiStack[0];
		if(NKeys){
			memcpy(KeyStack,KeyStack+1,NKeys-1);
			memcpy(AsciiStack,AsciiStack+1,NKeys-1);
		};
		NKeys--;
		return c;
	}else return -1;
};
void ClearKeyStack(){
	NKeys=0;
};
extern bool GUARDMODE;
extern bool PATROLMODE;
extern byte NeedToPopUp;
short WheelDelta=0;
void IAmLeft();
void ReverseLMode();
void LOOSEANDEXITFAST();
extern bool DoNewInet;
bool ReadWinString(GFILE* F,char* STR,int Max);
extern bool RUNMAPEDITOR;
extern bool RUNUSERMISSION;
extern char USERMISSPATH[128];
void OnWTPacket(WPARAM wSerial, LPARAM hCtx);
//MOUSE HANDLING, HANDLING OF MOUSE, MOUSE BUTTONS
long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    RECT        rc;
    static BYTE phase = 0;
//    HDC         hdc;
	

    switch( message )
    {
	case WT_PACKET:
		OnWTPacket(wParam,lParam);
		break;

	case 0xABCD:
		{
			GFILE* F=Gopen("UserMissions\\start.dat","r");
			if(F){
				ReadWinString(F,USERMISSPATH,120);
				Gclose(F);
				if(lParam==1)RUNMAPEDITOR=1;
				if(lParam==0)RUNUSERMISSION=1;
			};
		};
		break;
	case 0x020A://WM_MOUSEWHEEL:
		WheelDelta=(short)HIWORD(wParam);

#ifdef _USE3D
		{
			
			if (GetKeyState(VK_CONTROL) < 0 && 
				GetKeyState(VK_SPACE) < 0	&&
				GetKeyState(VK_SHIFT) < 0)
			{
				extern Camera CAM;
				float rotAng = 0.002f * (float)WheelDelta;
				CAM.RotateAroundLookAtPoint( rotAng );
			}
			
		}
#endif // _USE3D
#ifdef COSSACKS2
		if(WheelDelta<0){
			SpecCmd=173;
		};
		if(WheelDelta>0){
			SpecCmd=174;
		};
#endif //COSSACKS2

		break;
	case MM_MCINOTIFY:
		MyMCINotify(hWnd,wParam,lParam);
		CD_MCINotify(wParam,lParam);
		break;
	case WM_LBUTTONDOWN:
		wParam=wParam|MK_LBUTTON;
		Lpressed=true;
		realLpressed=true;
		Fixed=false;
		//xLpressed=true;
		SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
		AddMouseEvent(mouseX,mouseY,Lpressed,Rpressed);
		//HandleMouse(mouseX,mouseY);
		break;
	case WM_LBUTTONUP:
		wParam=wParam&!MK_LBUTTON;
		if(Fixed)Lpressed=false;
		realLpressed=false;
		//xLpressed=false;
		SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
		AddMouseEvent(mouseX,mouseY,Lpressed,Rpressed);
		//HandleMouse(mouseX,mouseY);
        if (!BuildMode//BUGFIX: Prevent unit selection while placing buildings
            && (abs(mouseX - LastUMX) + abs(mouseY - LastUMY)) < 16
            && GetTickCount() - LastUTime < 600)
        {
            //Select all units of selected type on screen
            SpecCmd = 241;
        }
		//if((abs(mouseX-LastUMX)+abs(mouseY-LastUMY))<16&&GetTickCount()-LastUTime<600)SpecCmd=241;
		LastUMX=mouseX;
		LastUMY=mouseY;
		LastUTime=GetTickCount();
		break;
	case WM_RBUTTONDOWN:
		wParam=wParam|MK_RBUTTON;
		Rpressed=true;
		realRpressed=true;
		Fixed=false;
		//xRpressed=true;
		if(ScreenPtr)SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
		AddMouseEvent(mouseX,mouseY,Lpressed,Rpressed);
		//HandleMouse(mouseX,mouseY);
		break;
	case WM_RBUTTONUP:
		wParam=wParam&!MK_RBUTTON;
		Rpressed=false;
		if(Fixed)Rpressed=false;
		realRpressed=false;
		//xRpressed=false;
		if(ScreenPtr)SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
		AddMouseEvent(mouseX,mouseY,Lpressed,Rpressed);
		//HandleMouse(mouseX,mouseY);
		break;
    case WM_MBUTTONDOWN:
        wParam = wParam | MK_MBUTTON;
        //xLpressed=true;
        ReverseLMode();
        SetMPtr(LOWORD(lParam), HIWORD(lParam), wParam);
        AddMouseEvent(mouseX, mouseY, Lpressed, Rpressed);
        //HandleMouse(mouseX,mouseY);
        break;
	case WM_MOUSEMOVE:
#ifndef _USE3D
		if(ScreenPtr)
#endif // _USE3D
		if(LOWORD(lParam)!=mouseX||HIWORD(lParam)!=mouseY){
			SetMPtr(LOWORD(lParam),HIWORD(lParam),wParam);
			OnMouseMoveRedraw();
		};
		//ShowMPtra();
		break;
	case WM_LBUTTONDBLCLK:
		//SpecCmd=241;
		break;

    case WM_EXITSIZEMOVE:
        //Adjust cursor zone after window was moved
        ClipCursorToWindowArea();
        break;

    case WM_SIZE:
        if (SIZE_RESTORED == wParam)
        {//Restore cursor zone after window was minimized
            ClipCursorToWindowArea();
        }
        break;

    case WM_SETFOCUS:
        //Restore cursor zone after alt-tab
        ClipCursorToWindowArea();
        break;

    case WM_ACTIVATEAPP:
        bActive = TRUE;
		if(bActive){
			if(lpDDSPrimary){
				//LockSurface();
				//UnlockSurface();
				//FreeDDObjects();
				CreateDDObjects(hwnd); 
				LockSurface();
				UnlockSurface();
				LoadFog(CurPalette);
				char cc[64];
				sprintf_s( cc,sizeof(cc),"%d\\agew_1.pal",CurPalette);
				PalDone=0;
				LoadPalette(cc);
			};
		};
        break;

    case WM_CREATE:
        break;

    case WM_SETCURSOR:
        SetCursor(NULL);
        return TRUE;

    case WM_TIMER:
        // Flip surfaces
        if( bActive )
        {
			//ShowMPtra();
			//TimerProc();
		}
        break;
	
	
    case WM_KEYDOWN:
        if (wParam < 256)
        {
            ScanPressed[wParam] = 1;
        }

        LastKey = wParam;
        KeyPressed = true;

        if (LastKey == VK_F11)
        {
            SaveScreen();
        }

        /*
        //Can't see where it was supposed to work. Cut it out.
        if (( !GameInProgress ) && LastKey == 'R' &&
            GetKeyState( VK_CONTROL ) & 0x8000)
        {
            //RecordMode = !RecordMode;//BUGFIX: remove switching record mode in real time
        }
        */

        {
            int nVirtKey = (int)wParam;
            int lKeyData = lParam;
            byte PST[256];
            GetKeyboardState(PST);

            word ascii_key;
            int result = ToAscii(nVirtKey, lKeyData, PST, &ascii_key, 0);

            WCHAR u_buf[5] = {};
            if (1 <= ToUnicode(nVirtKey, lKeyData, PST, u_buf, 4, 0))
            {//Valid UTF character
                wchar_t unicode_char = u_buf[0];
                if (1040 <= unicode_char && 1103 >= unicode_char)
                {//UTF code is in cyrillic range
                    //Adjust ascii code to match sprite index in mainfont.gp file
                    //Sprites 192 to 255 ('А' to 'я')
                    //(taken from russian cossacks version ALL.GSC)
                    ascii_key = unicode_char - 848;
                }
            }

            if (1 == result)
            {
                LastAsciiKey = ascii_key;
            }
            else
            {
                LastAsciiKey = 0;
            }

            AddKey(wParam, LastAsciiKey);
        }
        break;
    case WM_PAINT:
		/*
        BeginPaint( hWnd, &ps );
        GetClientRect(hWnd, &rc);
        SetBkColor( ps.hdc, RGB( 0, 0, 255 ) );
        SetTextColor( ps.hdc, RGB( 255, 255, 0 ) );
        EndPaint( hWnd, &ps );
		*/
        break;
	case WM_CLOSE:
		if(DoNewInet){
			IAmLeft();
			LOOSEANDEXITFAST();
			//wait(100);
		};
		//return false;
    case WM_DESTROY:
		
        finiObjects();
        PostQuitMessage( 0 );
		ov_Done();
		exit(0);
        break;
    }
	try{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}catch(...){
		ExitProcess(0);
		return 1;
	};
} /* WindowProc */

/*
 * doInit - do work required for every instance of the application:
 *                create the window, initialize data
 */
void ProcessGSaveMap();
void EditorKeyCheck();
void ProcessSaveInSquares();
void TestGenMap();
bool ShowStatistics();
bool EnterChatMode=0;
char ChatString[128];
wchar_t unicode_chat_string[128];
void ProcessChatKeys();
extern int WaitState;
bool RetryVideo=0;
extern byte PlayGameMode;
extern bool GameExit;
extern int LastCTRLPressTime;
bool CheckFNSend(int idx);
void ProcessVotingKeys();
extern bool RESMODE;
extern bool OptHidden;
extern word NPlayers;
bool CheckFlagsNeed();
void SetGameDisplayModeAnyway(int SizeX,int SizeY);
void FlipDipDialog();
extern bool DIP_DSS_Vis;
extern bool SHOWZONES;
extern byte InfoStyle;
CEXPORT
void SetCurPtr(int v);
extern int DipDlgDX;
extern int InfDX0;
extern int InfDX;
void GameKeyCheck(){
	if(PlayGameMode==1){
		if(KeyPressed){
			GameExit=true;
			RetryVideo=0;
			KeyPressed=0;
			return;
		};
	};
	ProcessVotingKeys();
	if(EnterChatMode){
		ProcessChatKeys();
		return;
	};
	if(EditMapMode){
		EditorKeyCheck();
		return;
	};
	if(KeyPressed){
		KeyPressed=false;
		int wParam=LastKey;
			switch( wParam ){
				
		        case VK_ESCAPE:
					ClearModes();
					BuildMode=false;
					//BuildWall=false;
					GetCoord=false;
					curptr=0;
					curdx=0;
					curdy=0;
					PauseMode=0;
					SetDestMode=false;
					GoAndAttackMode=false;
					GUARDMODE=0;
					PATROLMODE=0;
					if(WaitState==1)WaitState=2;
					if(ShowGameScreen)ShowGameScreen=2;
					AttGrMode=0;
					//if(PlayerMenuMode==2)
					//MainMenu.ShowModal();
					//Options.ShowModal();
					break;
				//case 'G':
				//	smapx+=16;
				//	smaplx--;
				//	smapy+=16;
				//	smaply--;
				//	ShowFon();
				//	break;
				case 'H':
					//FullScreenMode=!FullScreenMode;
					//GameNeedToDraw=true;
					//GSSetup800();
					//ProcessGSaveMap();
					break;
				case 'E':
					//FastMode=!FastMode;
					break;
				case ' ':
					SpecCmd=111;
					break;
				case 8:
					SpecCmd=112;
					break;
				case 'V':
					/*
					switch(Light){
					case 0:SetLight(0,10,20);
						break;
					case 1:SetLight(0,12,17);
						break;
					case 2:SetLight(0,15,15);
						break;
					case 3:SetLight(0,17,12);
						break;
					case 4:SetLight(0,20,10);
						break;
					case 5:SetLight(0,17,12);
						break;
					case 6:SetLight(0,15,15);
						break;
					case 7:SetLight(0,12,17);
					};
					Light++;
					if(Light>7)Light=0;
					*/
					//SaveGame("save1.wsf","my game");
					break;
				case 'U':
					if(!InfoStyle){
						if(Inform!=2)Inform=2;else Inform=0;
						MiniActive=0;
						DIP_DSS_Vis=0;
						InfDX=InfDX0;
						Recreate=1;
					};
					break;
				case 'M':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=114;
					else FullMini=!FullMini;
					//LoadGame("save1.wsf");
					//MEditMode=!MEditMode;
					//if(!MEditMode)BlobMode=0;
					MiniActive=0;
					Recreate=1;
					break;
		       case VK_F12:
					//UnlockSurface();
					//CloseExplosions();
					//ShutdownMultiplayer();
		            //PostMessage(hwnd, WM_CLOSE, 0, 0);
				    MenuType=1;
					MakeMenu=true;
				    break;
				case VK_F1:
					if(!CheckFNSend(0)){
						MenuType=4;
						MakeMenu=true;
					};
					//Creator=0;
					break;
				case VK_F2:
					CheckFNSend(1);
					break;
				case VK_F3:
					CheckFNSend(2);
					break;
				case VK_F4:
					CheckFNSend(3);
					break;
				case VK_F5:
					CheckFNSend(4);
					break;
				case VK_F6:
					CheckFNSend(5);
					break;
				case VK_F7:
					CheckFNSend(6);
					break;
				case VK_F8:
					//CheckFNSend(7);
					FlipDipDialog();
					break;
				case VK_F9:
					if(!CheckFNSend(8)){
						Creator=4096+255;
					};
					break;
				case 'R':
					//if((!GameInProgress)&&GetKeyState(VK_CONTROL)&0x8000)RecordMode=!RecordMode;
					break;
				case VK_TAB:
				case 192:
					HealthMode=!HealthMode;
					break;
				
				case 46:
					SpecCmd=200;
					break;
				case 'D':
#ifndef SPEEDFIX
					if(!(GetKeyState(VK_CONTROL)&0x8000)){
						if(!(GetKeyState(VK_SHIFT)&0x8000)){
							//FlipDipDialog();
						}else
						if(NPlayers<2){
							if((GetKeyState(VK_SHIFT))&0x8000){//&&PlayGameMode)
								switch(HISPEED){
								case 0:
									HISPEED=1;
									break;
								case 1:
									HISPEED=2;
									break;
								case 2:
									HISPEED=3;
									break;
								default:
									HISPEED=0;
									break;
								};
							};
						};
					}else{
						if(FrmDec==1)CmdSetSpeed(1+128);
						else CmdSetSpeed(128);
					};
#endif
					break;
				case 'A':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=1;
					else if(NSL[MyNation])GoAndAttackMode=1;
					break;
				case 'S':
					//SpecCmd=2;
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=201;
#ifdef VIEW_TOP
					SHOWZONES=!SHOWZONES;
#endif //VIEW_TOP
					break;
				case 'W':
					break;
				case 'J':
					//RSCRSizeX++;
					if(PlayGameMode==2){
						int ExRX=RealLx;
						int ExRY=RealLy;
						if(RealLx!=1024||RealLy!=768)
							SetGameDisplayModeAnyway(1024,768);
						ShowStatistics();
						if(RealLx!=ExRX||RealLy!=ExRY)SetGameDisplayModeAnyway(ExRX,ExRY);
					};
					break;
				case 'K':
					if(GetKeyState(VK_CONTROL)&0x8000)RealPause-=2;
					else RealPause+=2;
					//Quality++;
					//if(Quality>=4)Quality=0;
					//RSCRSizeX--;
					break;
				case 'N':
#ifdef EW

                    //if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=3;
                    //else SpecCmd=4;
                    if (!(GetKeyState(VK_CONTROL) & 0x8000)) LockGrid++;
                    if (LockGrid > NMFIELDS) LockGrid = 0;
#ifdef VIEW_TOP					
                    //if(LockGrid>NMFIELDS) LockGrid=0;
                    if (GetKeyState(VK_CONTROL) & 0x8000) TopTypeMode++;
                    if (TopTypeMode > NMFIELDS) TopTypeMode = 0;

#else
                    //if(LockGrid>1)LockGrid=0;					
#endif //VIEW_TOP

                    MiniActive = 0;
                    Recreate = 1;
#endif
					//switch(RSCRSizeX){
					//	case 800:RSCRSizeX=1024;
					//			break;
					//	case 1024:RSCRSizeX=800+32;
					//			break;
					//	case 1280:RSCRSizeX=1600;
					//		break;
					//	case 1600:RSCRSizeX=800;
					//		break;
					//	default:
					//		RSCRSizeX=800;
					//};
					/*if(RSCRSizeX!=1024)RSCRSizeX=1024;
					else RSCRSizeX=800;*/
					//VHMode=!VHMode;
					break;
				case 'C':
					//CINFMOD=!CINFMOD;
					break;
				case 'F':
				//	//FASTMODE=true;
				//	break;
				case 'G':
					//CreateMapShot();
					//SaveScreen();
					
					break;
				//	//FASTMODE=false;
				//	break;
				case 'X':
					//if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=5;
					//else SpecCmd=6;
					break;
				case 'Q':
#ifndef EW
					//if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=3;
					//else SpecCmd=4;
					if(!(GetKeyState(VK_CONTROL)&0x8000)) LockGrid++;
					if(LockGrid>NMFIELDS) LockGrid=0;
#ifdef VIEW_TOP					
					//if(LockGrid>NMFIELDS) LockGrid=0;
					if(GetKeyState(VK_CONTROL)&0x8000) TopTypeMode++;
					if(TopTypeMode>NMFIELDS) TopTypeMode=0;

#else
					//if(LockGrid>1)LockGrid=0;					
#endif //VIEW_TOP
					
					MiniActive=0;
					Recreate=1;
#endif
					break;
				case 'B':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=9;
					else SpecCmd=10;
					break;
				case 'Z':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=11;
					else SpecCmd=241;
					break;
				//case 'F':
				//	if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=13;
				//	else SpecCmd=14;
				//	break;
				case VK_NUMPAD1:
					if(MEditMode)EditMedia=0;
					else{
#ifdef COSSACKS2
						if(NPlayers<2&&ChangeNation)SetMyNation(0)
						else SpecCmd=181;
#else 
						if(NPlayers<2&&ChangeNation)SetMyNation(0);
#endif
						PlayerMask=1;
					};
					break;
				case VK_NUMPAD2:
					if(MEditMode)EditMedia=1;
					else{
#ifdef COSSACKS2
						if(NPlayers<2&&ChangeNation)SetMyNation(1)
						else SpecCmd=182;
#else
						if(NPlayers<2&&ChangeNation)SetMyNation(1);
#endif
						PlayerMask=2;
					};
					break;
				case VK_NUMPAD3:
					if(MEditMode)EditMedia=2;
					else{
#ifdef COSSACKS2
						if(NPlayers<2&&ChangeNation)SetMyNation(2)
						else SpecCmd=183;
#else
						if(NPlayers<2&&ChangeNation)SetMyNation(2);
#endif
						PlayerMask=4;
					};
					break;
				case VK_NUMPAD4:
					if(MEditMode)EditMedia=3;
					else{
#ifdef COSSACKS2		
						if(NPlayers<2&&ChangeNation)SetMyNation(3)
						else SpecCmd=184;
#else
						if(NPlayers<2&&ChangeNation)SetMyNation(3);
#endif
						PlayerMask=8;
					};
					break;
				case VK_NUMPAD5:
					if(MEditMode)EditMedia=4;
					else{
#ifdef COSSACKS2
						if(NPlayers<2&&ChangeNation)SetMyNation(4)
						else SpecCmd=185;
#else
						if(NPlayers<2&&ChangeNation)SetMyNation(4);
#endif
						PlayerMask=16;
					};
					break;
				case VK_NUMPAD6:
					if(MEditMode)BlobMode=1;
					else{
#ifdef COSSACKS2
						if(NPlayers<2&&ChangeNation)SetMyNation(5)
						else SpecCmd=186;
#else
						if(NPlayers<2&&ChangeNation)SetMyNation(5);
#endif
						PlayerMask=32;
					};
					break;
				case VK_NUMPAD7:
					if(MEditMode)BlobMode=-1;
					else{
#ifdef COSSACKS2
						if(NPlayers<2&&ChangeNation)SetMyNation(6)
						else SpecCmd=187;
#else
						if(NPlayers<2&&ChangeNation)SetMyNation(6);
#endif
						PlayerMask=64;
					};
					break;
				case VK_NUMPAD8:
#ifdef COSSACKS2
					if(NPlayers<2&&ChangeNation)SetMyNation(7)
					else SpecCmd=188;
#else
					if(NPlayers<2&&ChangeNation)SetMyNation(7);
#endif
					PlayerMask=128;
					break;
#ifdef COSSACKS2
				case VK_NUMPAD9:
					SpecCmd=189;
					break;
#endif
				case 'I':
					if(GetKeyState(VK_CONTROL)&0x8000){
						if(!InfoStyle)InfoMode=!InfoMode;
					}else{
						if(!InfoStyle){
							if(Inform!=1)Inform=1;else{
								Inform=0;
								InfDX=InfDX0;
							};
							MiniActive=0;
							Recreate=1;
						};
					};
					//InfoMode=1;
				break;
				case VK_CAPITAL:
					EgoFlag=!EgoFlag;
					SetEgoFlag(EgoFlag);
					break;
				case 'O':
					//if(MsPerFrame)MsPerFrame--;
					if(GetKeyState(VK_CONTROL)&0x8000){
						if(PlayGameMode==2||CheckFlagsNeed()){
							OptHidden=!OptHidden;
							if(!OptHidden){
								Inform=0;
								InfDX=InfDX0;
							};
						};
					}else{
						TransMode=!TransMode;
						MiniActive=0;
						Recreate=1;
					};
					break;
				case 'P':
					//MsPerFrame++;
					//if(GetKeyState(VK_CONTROL)&0x8000)RotatePhiI();
					//RotatePhi();
					if(GetKeyState(VK_CONTROL)&0x8000){
						SpecCmd=113;
					}else
					if(MultiTvar){
						if(!InfoStyle){
							NeedToPopUp=2;
							Inform=0;
							InfDX=InfDX0;
						};
					};
					break;
				case 19://Pause
					if(tmtmt>32&&!LockPause)SpecCmd=137;
					//PauseMode=!PauseMode;
					break;
				case 13:
					//MakeMenu=true;
					//MenuType=3;
					if(!RESMODE){
						EnterChatMode=1;
						ClearKeyStack();
					};
					break;
				case 'L':
					ReverseLMode();
					//LG.CreateLegionFromSelection(PlayerMask);
					//if(!MiniMode)SetMiniMode();
					//else ClearMiniMode();
					break;
				default:
					if(wParam>='0'&&wParam<='9'){
						//if(GetKeyState(VK_SHIFT)&0x8000){
						//	int v=wParam-'0';
						//	SHIFT_VAL=SHIFT_VAL*10+v;
						//}else{
							if(GetTickCount()-LastCTRLPressTime<150){
								CmdMemSelection(MyNation,wParam-'0');
							}else CmdRememSelection(MyNation,wParam-'0'); 
							//if(GetKeyState(VK_CONTROL)&0x8000)
							//	CmdMemSelection(MyNation,wParam-'0');
							//else CmdRememSelection(MyNation,wParam-'0');
						//};
					};
			 };
        
	};
};
void TestRoads();
void CreateFastLocking();
void AddHill();
bool DelCurrentAZone();
void SelectNextGridMode();
void ProcessMapOptions();
void ResearchIslands();
void EnterRandomParams();
void GenerateRandomRoad(int idx);
bool CheckCurve();
void ClearCurve();
extern bool ToolsHidden;
void SetFractalTexture();
void AutoSMSSet();
void UpdateAllPieces();
void PerformPixUndo();
extern int PEN_RADIUS;
extern int PEN_BRIGHTNESS;
void LoadCurPixTexture(char* Name);
bool ManualTex=0;
void STBRR(char c){
	int v=c-'0';
	if(c=='0')v=10;
	if(v<1)v=1;
	if(v>10)v=10;
	if(DrawGroundMode){
		int cs=GetKeyState(VK_CONTROL)&0x8000;
		int as=GetKeyState(VK_SHIFT)&0x8000;
		if(cs&&!as)CurGroundTexture=10+v-1;
		else if(as&&!cs)CurGroundTexture=20+v-1;
		else if(cs&&as)CurGroundTexture=30+v-1;
		else CurGroundTexture=v-1;
		ManualTex=1;
		return;
	};
	if(GetKeyState(VK_CONTROL)&0x8000)PEN_BRIGHTNESS=v*25;
	else{
		char ccc[64];
		sprintf_s(ccc,sizeof(ccc),"DrawTextures\\tex%d.bmp",v);
		LoadCurPixTexture(ccc);
		//PEN_RADIUS=4+v*4;
	};
};
extern int TexStDX;
extern int TexStDY;
void ReverseLMode();
void EditorKeyCheck(){
	if(KeyPressed){
		KeyPressed=false;
		int wParam=LastKey;
			switch( wParam ){
				case VK_RIGHT:
					if(DrawPixMode||DrawGroundMode)TexStDX=(TexStDX+1)&7;
					break;
				case VK_UP:
					if(DrawPixMode||DrawGroundMode)TexStDY=(TexStDY-1)&7;
					break;
				case VK_LEFT:
					if(DrawPixMode||DrawGroundMode)TexStDX=(TexStDX-1)&7;
					break;
				case VK_DOWN:
					if(DrawPixMode||DrawGroundMode)TexStDY=(TexStDY+1)&7;
					break;
				case 'E':
					FastMode=!FastMode;
					break;
				case '0':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);
					break;
				case '1':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);else
						if(GetKeyState(VK_CONTROL)&0x8000)CostThickness=1;
						else ReliefBrush=1;
					break;
				case '2':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);else
					if(GetKeyState(VK_CONTROL)&0x8000)CostThickness=2;
					else ReliefBrush=2;
					break;
				case '3':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);else
					if(GetKeyState(VK_CONTROL)&0x8000)CostThickness=3;
					else ReliefBrush=3;
					break;
				case '4':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);else
					if(GetKeyState(VK_CONTROL)&0x8000)CostThickness=4;
					else ReliefBrush=4;
					break;
				case '5':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);else
					if(GetKeyState(VK_CONTROL)&0x8000)CostThickness=5;
					else ReliefBrush=5;
					break;
				case '6':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);else
					if(GetKeyState(VK_CONTROL)&0x8000)CostThickness=6;
					else ReliefBrush=9;
					break;
				case '7':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);else
					if(GetKeyState(VK_CONTROL)&0x8000)CostThickness=7;
					else ReliefBrush=20;
					break;
				case '8':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);else
					if(GetKeyState(VK_CONTROL)&0x8000)CostThickness=8;
					else ReliefBrush=50;
					break;
				case '9':
					if(DrawPixMode||DrawGroundMode)STBRR(wParam);else
					if(GetKeyState(VK_CONTROL)&0x8000)CostThickness=9;
					else ReliefBrush=100;
					break;
				case 'H':
					//FullScreenMode=!FullScreenMode;
					//GameNeedToDraw=true;
					//GSSetup800();
					RenderAllMap();
					break;
				case 'V':
					if(GetKeyState(VK_CONTROL)&0x8000){
						ClearModes();
						NeedToPopUp=20;
					}else{
						ClearModes();
						NeedToPopUp=22;
						//SetFractalTexture();
						/*
						switch(Light){
						case 0:SetLight(-5,10,20);
							break;
						case 1:SetLight(-5,12,17);
							break;
						case 2:SetLight(-5,15,15);
							break;
						case 3:SetLight(-5,17,12);
							break;
						case 4:SetLight(-5,20,10);
							break;
						case 5:SetLight(-5,17,12);
							break;
						case 6:SetLight(-5,15,15);
							break;
						case 7:SetLight(-5,12,17);
						};
						Light++;
						if(Light>7)Light=0;
						*/
					};
					break;
				case 13:
					if(!(GetKeyState(VK_CONTROL)&0x8000)){
						//MakeMenu=true;
						//MenuType=3;
						EnterChatMode=1;
						ClearKeyStack();
					}else KeyPressed=true;
					break;
				case 'J':
					//RSCRSizeX++;
					//ShowStatistics();
					break;
				case 19://Pause
					if(!LockPause)SpecCmd=137;
					//PauseMode=!PauseMode;
					break;
		        case VK_ESCAPE:
					AttGrMode=0;
					if(CheckCurve()){
						ClearCurve();
					}else{
						ClearModes();
						BuildMode=false;
						//BuildWall=false;
						GetCoord=false;
						curptr=0;
						curdx=0;
						curdy=0;
						GUARDMODE=0;
						PATROLMODE=0;
						PauseMode=false;
						SetDestMode=false;
						GoAndAttackMode=false;
						NeedToPopUp=1;
					};
					//if(PlayerMenuMode==2)
					//MainMenu.ShowModal();
					//Options.ShowModal();
					break;
		       case VK_F12:
					//UnlockSurface();
					//CloseExplosions();
					//ShutdownMultiplayer();
		            //PostMessage(hwnd, WM_CLOSE, 0, 0);
					MenuType=1;
					MakeMenu=true;
				    break;
				case VK_TAB:
				case 192:
					HealthMode=!HealthMode;
					break;
			   case VK_F1:
				    HelpMode=!HelpMode;
				    break;
			   case VK_F2:
				    NeedToPopUp=6;
					break;
				case VK_F3:
					NeedToPopUp=15;
					break;
				case VK_F4:
					NeedToPopUp=4;
					break;
				case VK_F5:
					if(WaterEditMode){
						WaterEditMode=1;
						NeedToPopUp=8;
					}else{
						NeedToPopUp=5;
					};
					break;
				case 'U':
					if(GetKeyState(VK_CONTROL)&0x8000)CINFMOD=!CINFMOD;
					else if(Inform!=2)Inform=2;else Inform=0;
					DIP_DSS_Vis=0;
					extern int InfDX;
					extern int InfDX0;
					InfDX=InfDX0;
					MiniActive=0;
					Recreate=1;
					//CINFMOD=0;
					break;
				case 'F':
					//SVSC.Zero();
					if(GetKeyState(VK_CONTROL)&0x8000){
						TestGenMap();
					}else FogMode=!FogMode;
					//HeightEditMode=false;
					//MEditMode=true;
					//EditMedia=5;
					break;
				case VK_F6:
					if(WaterEditMode){
						WaterEditMode=2;
						NeedToPopUp=9;
					}else{
						ClearModes();
						MakeMenu=true;
						MenuType=6;
					};
					break;
				case VK_F7:
					if(WaterEditMode){
						WaterEditMode=3;
						NeedToPopUp=10;
					}else{
#ifdef _USE3D
						NeedToPopUp=23;
#else
						Reset3D();
#endif
					};
					break;
				case VK_F8:
					if(WaterEditMode){
						WaterEditMode=4;
						NeedToPopUp=11;
					}else{ 
						NeedToPopUp=3;
					};
					break;
				case 'N':
					if(GetKeyState(VK_CONTROL)&0x8000){
						AutoSMSSet();
					}else{
						NeedToPopUp=7;
						//HeightEditMode=3;
						//MEditMode=false;
						//EditMedia=5;
					};
					break;
				case 46:
					if(!DelCurrentAZone())SpecCmd=200;
					break;
#ifndef SPEEDFIX
				case 'D':
					
					if(!(GetKeyState(VK_CONTROL)&0x8000)){
						if((GetKeyState(VK_SHIFT))&0x8000){//&&PlayGameMode)
							switch(HISPEED){
							case 0:
								HISPEED=1;
								break;
							case 1:
								HISPEED=2;
								break;
							case 2:
								HISPEED=3;
								break;
							default:
								HISPEED=0;
								break;
							};
						}else{
							//FlipDipDialog();
						};
					}else{
						if(FrmDec==1)CmdSetSpeed(1+128);
						else CmdSetSpeed(128);
					};
					break;
#endif
				case 'A':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=1;
					else if(NSL[MyNation])GoAndAttackMode=1;
					break;
				case 'S':
					//ClearModes();
					if(GetKeyState(VK_CONTROL)&0x8000){
						//EnterRandomParams();
					}else{
						switch(LockMode){
						case 0:
							NeedToPopUp=12;
							break;
						case 1:
							NeedToPopUp=13;
							break;
						case 2:
							NeedToPopUp=14;
							break;
						case 3:
							NeedToPopUp=1;
							break;
						};
					};
					break;
				case 'W':
					if(GetKeyState(VK_CONTROL)&0x8000)ProcessSaveInSquares();
					else PeaceMode=!PeaceMode;
					break;
					/*
				case 'N':
					switch(RSCRSizeX){
						case 800:RSCRSizeX=1024;
								break;
						case 1024:RSCRSizeX=800+32;
								break;
						case 1280:RSCRSizeX=1600;
							break;
						case 1600:RSCRSizeX=800;
							break;
						default:
							RSCRSizeX=800;
					};*/
					/*if(RSCRSizeX!=1024)RSCRSizeX=1024;
					else RSCRSizeX=800;*/
					//break;
				case 'C':
					//CINFMOD=!CINFMOD;
					if(GetKeyState(VK_CONTROL)&0x8000){
						SpecCmd=217;
					}else{
						WaterEditMode=1;
						NeedToPopUp=8;
					};
					break;
				case 'X':
					//if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=5;
					//else SpecCmd=6;
					if(GetKeyState(VK_CONTROL)&0x8000){
						EraseAreas();
						rando();//!!
						CreateTotalLocking();
						TestRoads();
						ResearchIslands();
					}else{
						ClearModes();
						DrawPixMode=1;
						NeedToPopUp=21;
					};
					break;
				case 'Q':
					if(!(GetKeyState(VK_CONTROL)&0x8000)) LockGrid++;
					if(LockGrid>NMFIELDS) LockGrid=0;
#ifdef VIEW_TOP					
					//if(LockGrid>NMFIELDS) LockGrid=0;
					if(GetKeyState(VK_CONTROL)&0x8000) TopTypeMode++;
					if(TopTypeMode>NMFIELDS) TopTypeMode=0;

#else
					//if(LockGrid>1)LockGrid=0;
#endif //VIEW_TOP
					MiniActive=0;
					Recreate=1;
					break;
				case 'B':
					if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=9;
					else SpecCmd=10;
					//ClearMaps();
					//CreateUnitsLocking();
					break;
				case 'Z':
					if(DrawPixMode){
						PerformPixUndo();
					};
					//if(GetKeyState(VK_CONTROL)&0x8000)SpecCmd=11;
					//else SpecCmd=12;
#ifdef _USE3D
					if(GetKeyState(VK_CONTROL)&0x8000)
					{
						extern bool g_bDrawMCam;
						g_bDrawMCam = !g_bDrawMCam;
					}
#endif // _USE3D

					break;
				//case VK_F11:
				//	WaterCorrection();
				//	break;
				case VK_F9:
					MEditMode=false;
					HeightEditMode=false;
					LockMode=0;
					Creator=4096+255;
					NeedToPopUp=1;
					break;
				case VK_NUMPAD1:
					SetMyNation(0);
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=1;
					break;
				case VK_NUMPAD2:
					SetMyNation(1);
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=2;
					break;
				case VK_NUMPAD3:
					SetMyNation(2);
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=4;
					break;
				case VK_NUMPAD4:
					SetMyNation(3);
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=8;
					break;
				case VK_NUMPAD5:
					SetMyNation(4);
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=16;
					break;
				case VK_NUMPAD6:
					SetMyNation(5);
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=32;
					break;
				case VK_NUMPAD7:
					SetMyNation(6);
					MEditMode=false;
					HeightEditMode=false;
					PlayerMask=64;
					break;
				case VK_NUMPAD8:
					SetMyNation(7);
					MEditMode=false;
					HeightEditMode=false;PlayerMask=128;
					break;
				case 'I':
					if(GetKeyState(VK_CONTROL)&0x8000)InfoMode=!InfoMode;
					else if(Inform!=1)Inform=1;else{
						Inform=0;
						InfDX=InfDX0;
					};
					MiniActive=0;
					Recreate=1;
					//InfoMode=1;
					break;
				case 'O':
					if(GetKeyState(VK_CONTROL)&0x8000){
						OptHidden=!OptHidden;
					}else{
						TransMode=!TransMode;
						MiniActive=0;
						Recreate=1;
					};
					break;
					//WaterCorrection();
					//if(MsPerFrame)MsPerFrame--;
				case 'P':
					//MsPerFrame++;
					//if(GetKeyState(VK_CONTROL)&0x8000)RotatePhiI();
					//RotatePhi();
					NeedToPopUp=2;
					break;
				case 'R':
					if(GetKeyState(VK_CONTROL)&0x8000){
#ifdef _USE3D
					extern IRenderSystem* IRS;
					void ReloadRoads();
					void CreateWPalette();

					IRS->RecompileAllShaders();
					IRS->ReloadAllTextures();
					ReloadRoads();
					CreateWPalette();
#endif // _USE3D
						//ProcessMapOptions();
					}else{
						MEditMode=false;
						HeightEditMode=false;
						MakeMenu=true;
						MenuType=31;
#ifdef _USE3D
						void ReloadRoads();
						ReloadRoads();
#endif
					};
					break;
                case 'L':
					/*
					if(!MiniMode)SetMiniMode();
					else ClearMiniMode();
					MEditMode=false;
					*/
					ReverseLMode();
					break;
				case 'T':
					//HeightEditMode=false;
					//ChoosePosition=true;
					if(GetKeyState(VK_CONTROL)&0x8000){
						ToolsHidden=!ToolsHidden;
					};
					break;
				case 'G':
					if(GetKeyState(VK_SHIFT)&0x8000)CreateMapShot();
					if(GetKeyState(VK_CONTROL)&0x8000)SelectNextGridMode();
					//else SaveScreen();
					//SVSC.Grids=!SVSC.Grids;
					//SVSC.RefreshScreen();
					break;
				case 107://NUM +
					if(HeightEditMode)HiStyle=1;
					break;
				case 109://NUM -
					if(HeightEditMode)HiStyle=2;
					break;
				case 106://NUM *
					if(HeightEditMode)HiStyle=3;
					break;
				case 111://NUM /
					if(HeightEditMode)HiStyle=4;
					break;
				case 'M'://NUM 0
					//if(HeightEditMode)HiStyle=5;
					if(GetKeyState(VK_CONTROL)&0x8000){
						//AddHill();
						GenerateRandomRoad(5);
					}else FullMini=!FullMini;
					MiniActive=0;
					Recreate=1;
					break;
				case 33:
					if(HeightEditMode)HiStyle=7;
					break;
				case 34:
					if(HeightEditMode)HiStyle=8;
					break;
				case 36:
					if(HeightEditMode)HiStyle=9;
					break;
				default:
					if(wParam>='0'&&wParam<='9'){
						if(GetKeyState(VK_SHIFT)&0x8000){
							int v=wParam-'0';
							SHIFT_VAL=SHIFT_VAL*10+v;
						}else{
							if(GetTickCount()-LastCTRLPressTime<150){
								CmdMemSelection(MyNation,wParam-'0');
							}else CmdRememSelection(MyNation,wParam-'0'); 
							//if(GetKeyState(VK_CONTROL)&0x8000)
							//	CmdMemSelection(MyNation,wParam-'0');
							//else CmdRememSelection(MyNation,wParam-'0');
						};
					};
			 };
	};
};
void SERROR();
void SERROR1();
void SERROR2();
//bool PalDone;
bool InitScreen(){
#ifdef _USE3D
	SVSC.SetSize(RealLx,RealLy);
	return true;
#endif
	PalDone=false;
	CreateDDObjects(hwnd); 
	PalDone=false;
	LoadPalette("agew_1.pal");//agew.pal");//grassw.pal");//dmw.pal");//"darkw.pal");
    if(!DDError)
    {
		LockSurface();
		UnlockSurface();
		if(!RealScreenPtr){
			MessageBox(hwnd,"Unable to initialise Direct Draw. It is possible that hardware acceleration is turned off.","Loading error[1]",MB_ICONSTOP);
			exit(0);
		};
		//LockSurface();
		//UnlockSurface();
#ifndef _USE3D
		ShowCursor(false);
#endif // !_USE3D
		RECT OOO;
		OOO.left=-16;
		OOO.top=-16;
		OOO.right=1500;
		OOO.bottom=1200;
		ClipCursor(&OOO);
		SetCursorPos(512,300);//!!!!!!!!!!!!!!!!
#ifndef _USE3D
		ShowCursor(false);
#endif // !_USE3D

		OOO.left=0;
		OOO.top=0;
		OOO.right=RealLx-1;
		OOO.bottom=RealLy-1;
		ClipCursor(&OOO);
		return true;
    }else{
		PlayEffect(0,0,0);
	};
	return false;
};

#ifndef _USE3D
bool CreateRGBDDObjects(HWND hwnd);
bool InitRGBScreen(){
	PalDone=false;
	CreateRGBDDObjects(hwnd); 
	PalDone=false;
    if(!DDError)
    {
		LockSurface();
		UnlockSurface();
		//LockSurface();
		//UnlockSurface();
		ShowCursor(true);
		RECT OOO;
		OOO.left=-16;
		OOO.top=-16;
		OOO.right=1500;
		OOO.bottom=1200;
		ClipCursor(&OOO);
		SetCursorPos(512,300);//!!!!!!!!!!!!!!!!
		OOO.left=0;
		OOO.top=0;
		OOO.right=RealLx-1;
		OOO.bottom=RealLy-1;
		ClipCursor(&OOO);
		return true;
	};
	return false;
};
#endif // !_USE3D 

bool CreateRGB640DDObjects(HWND hwnd);
bool InitRGB640Screen(){
	CreateRGB640DDObjects(hwnd); 
    if(!DDError)
    {
		LockSurface();
		UnlockSurface();
		//LockSurface();
		//UnlockSurface();
		ShowCursor(true);
		RECT OOO;
		OOO.left=0;
		OOO.top=0;
		OOO.right=640;
		OOO.bottom=480;
		ClipCursor(&OOO);
		SetCursorPos(639,479);//!!!!!!!!!!!!!!!!
#ifndef _USE3D
		ShowCursor(false);
#endif // !_USE3D

		return true;
	};
	return false;
};

byte NoCD=1;
CEXPORT
void __stdcall CDGINIT_EnCD(){
	NoCD=0;
};
bool ProcessMessages();
bool xLockMouse;
extern int PlayMode;
void StopPlayCD();
void PlayRandomTrack();
bool First=1;
bool ProcessMessagesEx();
void ClearRGB();
BOOL GetCossacksDir(LPSTR lpszPath)
{
	HKEY	hKey;
	ULONG	lValue=255;
	UCHAR	szValue[255];

	if(RegOpenKeyEx(HKEY_CURRENT_USER,
		"SOFTWARE\\GSC Game World\\EW : Cossacks",
		0,KEY_QUERY_VALUE,&hKey)==ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hKey,
			"InstallDir",
			NULL,
			NULL,
			szValue,
			&lValue)==ERROR_SUCCESS)
		{
			strcpy(lpszPath,(LPSTR)szValue);
			RegCloseKey(hKey);
			return TRUE;
		};		

		RegCloseKey(hKey);
		return FALSE;
	}
	else
		return FALSE;
};
void PlayBinkFile(char* path);
bool InVideo=0;
void PlayVideo(){
	char CPATH[256];
	char AviName[256];
	sprintf(AviName,"Video\\1.bik",CPATH);
	ResFile f=RReset(AviName);
	if(f!=INVALID_HANDLE_VALUE){
		RClose(f);
	}else{
		xLockMouse=false;
		if(PlayMode)PlayRandomTrack();
		return;
	};

plavi:
	StopPlayCD();
	ClearScreen();
	InVideo=1;
	InitRGB640Screen();
	MiniMode=false;
	CDIRSND.ReleaseAll();
	
	PlayBinkFile("Video\\1.bik");
	PlayBinkFile("Video\\2.bik");
	PlayBinkFile("Video\\3.bik");
	PlayBinkFile("Video\\4.bik");
	PlayBinkFile("Video\\5.bik");
	
	if(First){
		sprintf(AviName,"Video\\cdv.avi",CPATH);
		ResFile f=RReset(AviName);
		if(f!=INVALID_HANDLE_VALUE){
			RClose(f);
			PlayAVI(hwnd,AviName,0);
			xLockMouse=true;
			do{
				ProcessMessagesEx();
				Sleep(30);
			}while(!(KeyPressed||Lpressed||!bActive));
			CloseAVI(hwnd);
			ClearRGB();
			First=0;
			KeyPressed=0;
			Lpressed=0;
		};
		sprintf(AviName,"%s\\Video\\logo.avi",CPATH);
		f=RReset(AviName);
		if(f!=INVALID_HANDLE_VALUE){
			RClose(f);
			PlayAVI(hwnd,AviName,60);
			xLockMouse=true;
			do{
				ProcessMessagesEx();
				Sleep(30);
			}while(!(KeyPressed||Lpressed||!bActive));
			CloseAVI(hwnd);
			First=0;
			KeyPressed=0;
			Lpressed=0;
			ClearRGB();
		};
	};
	
	sprintf(AviName,"%s\\Video\\kz.avi",CPATH);
	f=RReset(AviName);
	if(f!=INVALID_HANDLE_VALUE){
		RClose(f);
		PlayAVI(hwnd,AviName,60);
		xLockMouse=true;
		do{
			ProcessMessagesEx();
			Sleep(30);
		}while(!(KeyPressed||Lpressed||!bActive));
		CloseAVI(hwnd);
	};
	
	PlayBinkFile("Video\\6.bik");
	PlayBinkFile("Video\\7.bik");
	PlayBinkFile("Video\\8.bik");
	PlayBinkFile("Video\\9.bik");
	PlayBinkFile("Video\\10.bik");
	
	Lpressed=false;
	KeyPressed=false;
	ClearRGB();
	InVideo=0;
	InitScreen();
	
	CDIRSND.CreateSDLSound();
	
	CDS = &CDIRSND;
	
	LoadSounds("SoundList.txt");
	xLockMouse=false;
	if(PlayMode)PlayRandomTrack();
};
extern bool Lpressed;
void FilesExit();
void ShowLoading();
static BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
    WNDCLASS wc;
    char buf[256];

    // Set up and register window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = NAME;
    RegisterClass(&wc);
    
    if (window_mode)
    {
        hwnd = CreateWindow(
            NAME,
            TITLE,
            window_style,
            50, 50,
            RealLx,
            RealLy,
            NULL,
            NULL,
            hInstance,
            NULL
        );
        ResizeAndCenterWindow();
    }
    else
    {
        // For fullscreen, create a window at the GAME resolution, not desktop resolution
        // SDL will handle the actual fullscreen mode switching
        hwnd = CreateWindowEx(
            WS_EX_APPWINDOW,
            NAME,
            TITLE,
            WS_POPUP,
            0, 0,
            RealLx,  // Use game resolution instead of screen_width
            RealLy,  // Use game resolution instead of screen_height
            NULL,
            NULL,
            hInstance,
            NULL
        );
    }

    if (!hwnd)
    {
        return FALSE;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    void ov_Init(HWND hExtWnd);
	ov_Init(hwnd);

    CDIRSND.CreateSDLSound();
    CDS = &CDIRSND;
    LoadSounds("SoundList.txt");
    MemReport("LoadSounds");
    
    ResFile F = RReset("version.dat");
    if (F != INVALID_HANDLE_VALUE) {
        word B = 0;
        RBlockRead(F, &B, 2);
        RClose(F);
        if (B > 102) {
            MessageBox(hwnd, "Unable to use this testing version.", "WARNING!", 0);
            FilesExit();
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;
        }
    }
    
    if (!Loading()) {
        FilesExit();
        PostMessage(hwnd, WM_CLOSE, 0, 0);
        return 0;
    }

    SetCursorPos(512, 300);
#ifndef _USE3D
    ShowCursor(false);
#endif

    CurrentSurface = FALSE;
    KeyPressed = false;

#ifdef SCREENFIX
    // Fullscreen? Prepare for small not stretched menu
    if (!window_mode)
    {
        // Set initial window resolution to native screen resolution
        if (1920 < screen_width)
        {
            // Limit max resolution for menu screen to fullhd
            screen_width = 1920;
            screen_height = 1080;
        }
        RealLx = screen_width;
        RealLy = screen_height;
    }
#endif

    CreateDDObjects(hwnd); 
    MemReport("CreateDDObjects");
    
    CHKALL();
    
    if (!DDError)
    {
#ifndef _USE3D
        LockSurface();
        UnlockSurface();
        LockSurface();
        UnlockSurface();
        if (!RealScreenPtr) {
            MessageBox(hwnd, "Unable to initialise Direct Draw. It is possible that hardware acceleration is turned off.", "Loading error[2]", MB_ICONSTOP);
            exit(0);
        }
#endif

#ifndef _USE3D
        ShowCursor(false);
#endif

        // Set up cursor clipping
        RECT OOO;
        OOO.left = 0;
        OOO.top = 0;
        OOO.right = RealLx - 1;
        OOO.bottom = RealLy - 1;
        
        // Don't use ClipCursor in fullscreen - SDL handles this
        if (window_mode) {
            ClipCursor(&OOO);
        }
        
        SetCursorPos(512, 300);
#ifndef _USE3D
        ShowCursor(false);
#endif

        InVideo = 1;
        
        if (SetTimer(hwnd, TIMER_ID, 20, NULL))
        {
            return TRUE;
        }
    }
    
    wsprintf(buf, "Direct Draw Init Failed\n");
    MessageBox(hwnd, buf, "ERROR", MB_OK);
    finiObjects();
    DestroyWindow(hwnd);
    return FALSE;
}
/* doInit */
void AddDestn(int x, int y);
void ProcessNewMonsters();
int PrevTime=0;
void InitXShift();
void HandleMines();
void ControlGates();
void HealWalls();
void ProcessDeathList();
void ProcessEconomy();
void HandleMission();
//void CheckTops(){
//	int NT=GetNAreas()*GetNAreas();
//};

void ResearchCurrentIsland(byte Nat);
void ResearchBestPortToFish(byte Nat);
extern int NInGold[8];
extern int NInIron[8];
extern int NInCoal[8];
extern int NInStone[8];
extern int WasInGold[8];
extern int WasInIron[8];
extern int WasInCoal[8];
extern int WasInStone[8];
extern bool Tutorial;
void ProcessCostPoints();
void CheckArmies(City*);
void CheckGP();
extern int exFMode;
void CmdSetSpeed(byte);
bool NOPAUSE=1;
void EnumPopulation();
extern bool TutOver;
void HandleShar(Nation* NT);
void AddRandomBlobs();
void ProcessMapAutosave();
void CHECKIT(){
//	for(int i=0;i<4;i++)assert(GNFO.EINF[3]->GAINF.ArmDistr);
};
void ProcessGuard();
void DecreaseVeruVPobedu();
int InEnum=0;

#ifdef SPEEDFIX
unsigned long GetRealTime();
#else
int GetRealTime();
#endif
extern "C" CEXPORT
void CreateFields(byte NI);
extern int LastWaterchange;
extern int LastBrightspot;
void RESEARCH_WAVES();
bool ProcessMessagesSDL();
void PreDrawGameProcess() {
	//RunPF(19);
	if(EditMapMode){
		if(LastWaterchange&&GetTickCount()-LastWaterchange>5000){
			EraseAreas();
			CreateTotalLocking();
			LastWaterchange=0;
		};
		if(LastBrightspot&&GetTickCount()-LastBrightspot>3000){
			RESEARCH_WAVES();
			LastBrightspot=0;
		};
	};
	CHECKIT();
	ProcessMapAutosave();
	AddRandomBlobs();
	CDS->ProcessSoundSystem();
	if(NOPAUSE){
		for(int w=0;w<8;w++){
			WasInGold[w]=NInGold[w];
			WasInCoal[w]=NInCoal[w];
			WasInIron[w]=NInIron[w];
			WasInStone[w]=NInStone[w];
			NInGold[w]=0;
			NInCoal[w]=0;
			NInIron[w]=0;
			NInStone[w]=0;
		};
	};
	for(int g=0;g<8;g++){
		if(CITY[g].Account<0)CITY[g].Account=0;
	};
	if(exFMode!=SpeedSh){
#ifdef SPEEDFIX
        CmdSetSpeed(exFMode);
#else
        CmdSetSpeed(exFMode + 128);
#endif
	};
	if((tmtmt&255)==32)EnumPopulation();
	ProcessCostPoints();
	ProcessGuard();
	//RunPF(9);
	if(NOPAUSE){
		ProcessEconomy();
		ProcessDeathList();
		//RefreshSafeMap();
	};
	int tt=tmtmt&255;
	//if(tt==49)CreateEnmBuildList();
	//if(tt==199||tt==199-128)CreateProtectionMap();
	//SUSP("ResearchSafeCells");
	//if(NOPAUSE)ResearchSafeCells(200,400);
	SUSP("HealWalls");
	//HealWalls();
	if(LastAttackDelay)LastAttackDelay--;
	//assert(_CrtCheckMemory());
	addrand(21);
	//ProcessTow();
	addrand(45);
	for(int i=0;i<8;i++){
		memset(NATIONS[i].SoundMask,0,2048);
	};
	//CHKALL();
	InitDestn();
//	StopPF(9);
	GameKeyCheck();
	ProcessMessages();
	//if(!PauseMode){
	if(div(tmtmt,41).rem==2){
		SUSP("CreateStrategyInfo");
		CreateStrategyInfo();
	};
	SUSP("ProcessFishing");
	ProcessFishing();
	SUSP("ControlGates");
	ControlGates();
	SUSP("HandleMines");
	HandleMines();
	SUSP("InitXShift");
	InitXShift();
	SUSP("WinnerControl");
	//RunPF(8);
	addrand(tmtmt);
	if((tmtmt&255)==1)ResearchCurrentIsland((tmtmt>>8)&7);
	if((tmtmt&127)==7)ResearchBestPortToFish((tmtmt>>7)&7);
	if(SHOWSLIDE)WinnerControl(false);
//	StopPF(8);
	//PrevTime=GetRealTime();
#ifdef NEWMORALE
	void RemakeMaxMorale();
	RemakeMaxMorale();
#else //NEWMORALE
	DecreaseVeruVPobedu();
#endif //NEWMORALE
	if(tima!=time(NULL)){
		Flips=tmtim;	
		tmtim=0;
		tima=time(NULL);
	};
	if(!(tmtmt&63)){
		LASTRAND=rando();
		LASTIND=rpos;
	};
	tmtim++;
//	StopPF(19);
	//RunPF(20);
	if(NOPAUSE){
		tmtmt++;
		REALTIME+=FrmDec;
		for(int g=0;g<8;g++){
			if(CITY[g].AutoEraseTime){
				CITY[g].AutoEraseTime--;
				if(!CITY[g].AutoEraseTime){
					int SCORES[8];
					for(int i=0;i<8;i++)SCORES[i]=CITY[i].Account;
					for(int i=0;i<MAXOBJECT;i++){
						OneObject* OB=Group[i];
						if(OB&&(!OB->Sdoxlo)&&(OB->NNUM==g)){
							//erasing
							OB->delay=6000;
							if(OB->LockType)OB->RealDir=32;
							OB->Die();
							OB=Group[i];
							if(OB)OB->Sdoxlo=2500;
						};
					};
					for(int i=0;i<8;i++)CITY[i].Account=SCORES[i];
				};
			};
		};
	};
	SUSP("ProcessSprites");
	//RunPF(7);
	if(NOPAUSE)ProcessSprites();
	//CheckArmies(NULL);
//	StopPF(7);
//	StopPF(20);
	//RunPF(21);
	for(int p=0;p<7;p++)if(!NATIONS[p].Harch){
		int   NMon=NtNUnits[p];
		word* mon=NatList[p];
		for(int q=0;q<NMon;q++){
			word MID=mon[q];
			if(MID!=0xFFFF){
				OneObject* OBJ=Group[MID];
				if(rando()<250&&OBJ&&OBJ->NewBuilding&&OBJ->NInside&&!OBJ->Sdoxlo){
					word Last=OBJ->Inside[OBJ->NInside-1];
					if(Last!=0xFFFF){
						OneObject* OB=Group[Last];
						if(OB){
							OB->Nat->CITY->UnRegisterNewUnit(OB);
							OB->CloseObject();
							OBJ->NInside--;
							DelObject(OB);
							Group[OB->Index]=NULL;
						};
					};
				};
			};
		};
	};

	NMONS=0;
	SUSP("HandleTransport");
	HandleTransport();	
#ifdef _USE3D
	// Piro
	void PiroProcess();
	PiroProcess();
#endif
	//Alexander can insert there his callback
	void WiselyFormationsProcess();
	WiselyFormationsProcess();
	/////////////////////////////////////////
	int tmm=tmtmt&7;
	if(!tmm){
		CITY[0].ProcessCreation();
		if(rando()<2048)CreateFields(0);
	};
	if(NOPAUSE){
		CHECKIT();
		if(tmm==1){
			SUSP("ProcessCreation");
			CITY[1].ProcessCreation();
			if(rando()<2048)CreateFields(1);
		};
		CHECKIT();
		if(tmm==2){
			CITY[2].ProcessCreation();
			if(rando()<2048)CreateFields(2);
		};
		if(tmm==3){
			CITY[3].ProcessCreation();
			if(rando()<2048)CreateFields(3);
		};
		if(tmm==4){
			CITY[4].ProcessCreation();
			if(rando()<2048)CreateFields(4);
		};
		if(tmm==5){
			CITY[5].ProcessCreation();
			if(rando()<2048)CreateFields(5);
		};
		if(tmm==6){
			CITY[6].ProcessCreation();
			if(rando()<2048)CreateFields(6);
		};
		if(tmm==7||TutOver){
			InEnum=1;
			CITY[7].EnumUnits();
			InEnum=2;
			HandleMission();
			InEnum=0;
			TutOver=0;
		};
		//if(tmm==7)CITY[7].ProcessCreation();
		int xt=(tmtmt&255);
		CHECKIT();
		GNFO.Process();
		CHECKIT();
		for(int i=0;i<8;i++){
			Nation* NT=NATIONS+i;
			NT->Harch+=NT->NGidot*ResPerUnit;
			int mult=2000>>SpeedSh;
			int DHarch=NT->Harch/mult;
			if(DHarch){
				if(XRESRC(i,EatenRes)>DHarch){
					AddXRESRC(i,EatenRes,-DHarch);
					NATIONS[i].ResOnLife[EatenRes]+=DHarch;
					NT->AddResource(EatenRes,-DHarch);
					NT->Harch-=mult*DHarch;
					if(!NT->Harch)NT->Harch=1;
				}else{
					SetXRESRC(i,EatenRes,0);
					NT->Harch=0;
				};		

			};
			if(!NT->Harch){
				if(XRESRC(i,FoodID))NT->Harch=1;
				NATIONS[i].ResOnLife[FoodID]++;
			};
			mult=2000000>>SpeedSh;
			for(int j=0;j<8;j++){
				int R=NT->ResRem[j];
				R+=NT->ResSpeed[j]*100;
				div_t dd=div(R,mult);
				R=dd.rem;
				AddXRESRC(i,j,-dd.quot);
				NATIONS[i].ResOnLife[j]+=dd.quot;
				if(XRESRC(i,j)<0){
					NATIONS[i].ResOnLife[j]+=XRESRC(i,j);
					SetXRESRC(i,j,0);
				};
				NT->AddResource(j,-dd.quot);
				NT->ResRem[j]=R;
				if(j==GoldID){
					if(XRESRC(i,j)<2)NT->GoldBunt=true;
					else NT->GoldBunt=false;
				};
			};
		};	
		SUSP("ProcessNewMonsters");
		//RunPF(6);
		CHECKIT();
		ProcessNewMonsters();
//		StopPF(6);
		ObjTimer.Handle();
		CHECKIT();
	};
//	StopPF(21);
	//RunPF(22);
	ProcessExpl();
	for(int i=0;i<8;i++){
		addrand(i);
		HandleShar(NATIONS+i);
	};

#ifndef HASH_TOP
	SUSP("DynamicalTopology");
	ProcessDynamicalTopology();
#endif //HASH_TOP

//	StopPF(22);
};
bool ProcessMessages();
extern word NPlayers;;
void CmdSaveNetworkGame(byte NI,int ID,char* Name);
int SaveTime;
extern char SaveFileName[128];
void ProcessNature();
bool NeedEBuf=0;
int GLOBALTIME=0;
int PGLOBALTIME=0;
int PitchTicks=0;
int MaxPingTime=0;
int RealPause=0;
int RealStTime=0;
int RealGameLength=0;
int CurrentStepTime=80;
#ifdef SPEEDFIX
unsigned long GetRealTime();
#else
int GetRealTime();
#endif
void MakePause(int dt){
	int t0=GetRealTime();
	do{
		ProcessMessages();
	}while(GetRealTime()-t0<dt);
};
int SUBTIME=0;
void ProcessScreen();
void GSYSDRAW();
extern int StepX;
extern int StepY;
int TAverage=50;
void WaitToTime(int Time){
	int dt0=int(Time)-int(GetRealTime());
	bool DoDraw=dt0>(TAverage>>2);
	do{
		if(DoDraw){
			int T0=GetRealTime();
			if(T0-Time<0){
				int tt=T0;
				ProcessScreen();
				GSYSDRAW();
				int dt=GetRealTime()-tt;
				TAverage=(TAverage+TAverage+TAverage+dt)>>2;
				SUBTIME+=GetRealTime()-T0;
			};
		};
		ProcessMessages();
	}while(int(Time)-int(GetRealTime())>0);
	int ttx=GetRealTime();
	//SUBTIME=0;
};
int NeedCurrentTime=0;
extern bool PreNoPause;
extern EXBUFFER EBufs[MaxPL];
void StopPlayCD();
void ProcessUpdate();
extern byte CaptState;
extern byte SaveState;
void WritePitchTicks();
void ReadPichTicks();
void ShowCentralText0(char* sss);
void CmdChangePeaceTimeStage(int Stage);
int PrevCheckTime=0;
extern int PeaceTimeStage;
extern int PeaceTimeLeft;
void PostDrawGameProcess(){
	RGAME.TryToFlushNetworkStream(0);
	if(PlayGameMode==0&&NPlayers<2)PitchTicks=0;
	if(PlayGameMode==0){
		if(MaxPingTime)WaitToTime(NeedCurrentTime);
		else PitchTicks=0;
	};
	if(PlayGameMode){
		ReadPichTicks();
		if(PitchTicks)MaxPingTime=1;
		else MaxPingTime=0;
	}else{
		WritePitchTicks();
	};
	ProcessNature();
	if(MaxPingTime){
		//addrand(CurrentStepTime);
		addrand(PitchTicks);
		//addrand(MaxPingTime);
	};
	NeedEBuf=0;
	GLOBALTIME++;
	if(RealStTime==0)RealGameLength=0;
	NeedCurrentTime+=CurrentStepTime;
	if(GLOBALTIME-PGLOBALTIME>PitchTicks){
		CurrentStepTime-=CurrentStepTime>>5;
		RealGameLength=GetRealTime()-RealStTime;
		HandleMultiplayer();
		SYN1.Clear();
		SYN.Copy(&SYN1);
		//SYN1.Save();
		PreNoPause=0;
		ExecuteBuffer();
		if(PreNoPause)NOPAUSE=0;
		PGLOBALTIME=GLOBALTIME;
		RealStTime=GetRealTime();
		if(PlayGameMode){
			addrand(PitchTicks);
			ReadPichTicks();
			addrand(PitchTicks);
			if(PitchTicks)MaxPingTime=1;
		}else{
			if(NPlayers>1&&MaxPingTime){
				if(CurrentStepTime){
					addrand(PitchTicks);
					PitchTicks=4+((MaxPingTime)/CurrentStepTime);
					addrand(PitchTicks);
					//addrand(MaxPingTime);
				}else PitchTicks=0;
			}else PitchTicks=0;
			WritePitchTicks();
		};
	};
	//int stt0=GetRealTime();
	//if(RealPause>0)MakePause(RealPause/PitchTicks);
	//RealStTime-=GetRealTime()-stt0;
	if(!HISPEED)SHOWSLIDE=true;
	else SHOWSLIDE=!div(tmtmt,HISPEED+1).rem;
	//HandleHExpls();
	//HandleSW();
	int difTime=GetRealTime()-AutoTime;
	ProcessUpdate();
	int MaxDT;
	switch(SaveState){
	case 0:
		MaxDT=60000;
		break;
	case 1:
		MaxDT=60000*2;
		break;
	case 2:
		MaxDT=60000*4;
		break;
	case 3:
		MaxDT=60000*6;
		break;
	case 4:
		MaxDT=60000*8;
		break;
	case 5:
		MaxDT=60000*10;
		break;
	case 6:
		MaxDT=60000*2000;
		break;
	};
	if(difTime>MaxDT&&!(PlayGameMode||SaveState==6)){
		if(NPlayers>1){
			for(int i=0;i<NPlayers;i++){
				if(EBufs[i].Enabled){
					if(PINFO[i].PlayerID==MyDPID){
						int NP=0;
						for(int j=0;j<NPlayers;j++)if(EBufs[j].Enabled)NP++;
						char cc1[128];
						sprintf(cc1,"Save\\NetAutoSave %d players",NP);
						CmdSaveNetworkGame(MyNation,0,cc1);
					};
					i=100;
				};
			};
			//SaveGame("AUTO.sav",SaveFileName,0);

		}else{
			if(!EditMapMode){
				if(NATIONS[MyNation].VictState!=1&&!SCENINF.LooseGame){
					ShowCentralText0(GetTextByID("Autosaving"));
					FlipPages();
					SaveGame("Save\\AUTO.sav","Save\\AUTO.sav",0);
				};
			};
		};
	    AutoTime=GetRealTime();
	};
#ifdef SPEEDFIX
    if (!PrevCheckTime)
    {
        PrevCheckTime = GetRealTime();
    }

    if (GetRealTime() - PrevCheckTime > 90000)
    {
        PrevCheckTime = GetRealTime();
        if (PeaceTimeLeft / 60 < PeaceTimeStage)
        {
            CmdChangePeaceTimeStage(PeaceTimeLeft / 60);
        }
    }
#else
    if (!PrevCheckTime)PrevCheckTime = GetRealTime();
    if (GetRealTime() - PrevCheckTime > 30000) {
        PrevCheckTime = GetRealTime();
        //if(PeaceTimeLeft/60<PeaceTimeStage){
        CmdChangePeaceTimeStage(PeaceTimeLeft / 60);
        //};
    };
#endif
	/*
	if(NPlayers>1&&MyDPID==ServerDPID&&SaveTime-GetRealTime()>60000*5){
		CmdSaveNetworkGame(MyNation,GetRealTime(),"NETWORK SAVE");
		SaveTime=GetRealTime();
	};
	*/
#ifdef SPEEDFIX
    if (0 == prev_postdraw_time)
    {
        prev_postdraw_time = GetRealTime();
    }


    unsigned long time_since_last_call = 0;
    do
    {
        ProcessMessages();
        if (PauseMode)
        {
            GameKeyCheck();
        }
        time_since_last_call = GetRealTime() - prev_postdraw_time;
    } while (PauseMode || time_since_last_call < kPostDrawInterval);

    prev_postdraw_time = GetRealTime();
#else
    if (NPlayers < 2) {
        do {
            ProcessMessages();
            if (PauseMode)GameKeyCheck();
        } while ((int(GetRealTime()) - PrevTime < (FPSTime + FPSTime)) || PauseMode);
    };
    PrevTime = GetRealTime();
#endif
};

/*
 * WinMain - initialization, message loop
 */
void InitWaves();
void AllGame();
extern byte MI_Mode;
extern int RES[8][8];
void ClearAllLModeData();
void PrepareToEdit(){
	ClearAllLModeData();
	ClearMStack();
	MI_Mode=1;
	ReliefBrush=3;
	TerrBrush=2;
	EditMedia=0;
	HeightEditMode=false;
	MEditMode=false;
	EditMapMode=true;
	FogMode=0;
	HelpMode=true;
	ChoosePosition=false;
	CheapMode=false;
	NMyUnits=1;
	NThemUnits=1;
	AutoTime=GetRealTime()+180000;
    ObjTimer.~TimeReq();
	InitWaves();
	PeaceMode=false;
	LockMode=0;
	SaveFileName[0]=0;
	LockBars.Clear();
	UnLockBars.Clear();
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			RES[i][j]=50000;
		};
	};
};
byte PlayGameMode=0;
extern char CurrentMap[64];
extern int TIMECHANGE[8];
extern int AddTime;
extern int NeedAddTime;
extern byte XVIIIState;
extern char RECFILE[128];
void PerformNewUpgrade(Nation* NT,int UIndex,OneObject* OB);
extern byte MI_Mode;
bool IsGameActive();
void ClearAllLModeData();

void PrepareToGame(){
	ClearAllLModeData();
	if(!PlayGameMode){
		if(NPlayers>1&&(IsGameActive()||UseGSC_Login)&&!RecordMode){
			RecordMode=1;
			sprintf(RECFILE,"Autorecord\\%s",CurrentMap);
		};
	};
	if(RecordMode&&!PlayGameMode){
		//RecordMode=1;
		RGAME.StartRecord(CurrentMap);
	};
	MI_Mode=1;
	memset(TIMECHANGE,0,sizeof TIMECHANGE);
	AddTime=0;
	NeedAddTime=0;
	ClearMStack();
	ReliefBrush=3;
	TerrBrush=2;
	EditMedia=0;
	HeightEditMode=false;
	MEditMode=false;
	EditMapMode=false;
	FogMode=1;
	HelpMode=false;
	ChoosePosition=false;
	CheapMode=false;
	NMyUnits=1;
	NThemUnits=1;
	SaveTime=GetRealTime();
	AutoTime=GetRealTime();
	//if(NPlayers>1)AutoTime=GetRealTime()+180000;
	//else AutoTime=GetRealTime()-60000;
    ObjTimer.~TimeReq();
	InitWaves();
	PeaceMode=false;
	LockMode=0;
	SaveFileName[0]=0;
	FrmDec=2;
	SpeedSh=1;
	exFMode=1;
	CurrentStepTime=80;
};
void ShowIntro(){

};
void SetupRenderStack();
//byte* BLOBA;
lpCHAR FLIST[4096];
int NFILES=0;
void AText(char* str){
	
};
void AFile(char* str){
	AText(str);
};
void SaveAllFiles(){
	AText("btne0.bpx");
	AText("btne1.bpx");
	AText("btne2.bpx");
	AText("btne3.bpx");
	AText("btne4.bpx");
	AText("btne5.bpx");
	AText("btne6.bpx");
	AText("btne7.bpx");
	AText("btne8.bpx");
	AText("btne9.bpx");
	AText("btne10.bpx");
	AText("btne11.bpx");
	AText("btnu0.bpx");
	AText("btnu1.bpx");
	AText("btnu2.bpx");
	AText("btnu3.bpx");
	AText("btnu4.bpx");
	AText("btnu5.bpx");
	AText("btnu6.bpx");
	AText("btnu7.bpx");
	AText("btnu8.bpx");
	AText("btnu9.bpx");
	AText("btnu10.bpx");
	AText("btnu11.bpx");
	AText("hied.bpx");
	AText("scr_l.bpx");
	AText("scr_r.bpx");
	AText("scr_u_g.bpx");
	AText("scrL0.bpx");
	AText("trees.bpx");
	AText("scr_10.bpx");
	AText("stones.bpx");
	AText("pan17.bpx");
	AText("opt.bpx");
	AText("scr_line.bpx");
	AText("Gala1.bpx");
	AText("Gala0.bpx");
	AText("mnmenu0.bpx");
	AText("mnmenu01.bpx");
	AText("pan16.bpx");
	AText("scr_v_u.bpx");
	AText("scr_v_d.bpx");
	AText("scr_v.bpx");
	AText("pan15.bpx");
	AText("scr_15.bpx");
	AText("units.bpx");
	AText("195x20.bpx");
	AText("scr_13.bpx");
	AText("scr_16.bpx");
	AText("edmenu.bpx");
	AText("textur.bpx");
	AText("scr_12.bpx");
	AText("scr_10.bpx");
	AText("multi_m0.bpx");
	AText("gf24y.rlc");
	AText("gf24w.rlc");
	AText("cur_mn.rlc");
	AText("agew_1.pal");
	AText("2w.pal");
	AText("1w.pal");
	/*
	GFILE* f=Gopen("Filelist.lst","w");
	for(int i=0;i<NFILES;i++)Gprintf(f,"%s\n",FLIST[i]);
	Gclose(f);
	*/
};
void CreateRadio();
extern int ScrollSpeed;
void UnLoading();
void EraseRND(){
	char** RNDF=NULL;
	int NRND=0;
	int MaxRND=0;
	DWORD* RndData=NULL;
	word* Ridx=NULL;

	WIN32_FIND_DATA FD;
	HANDLE HF=FindFirstFile("RN? *.m3d",&FD);
	if(HF!=INVALID_HANDLE_VALUE){
		bool r=true;
		do{
			if(NRND>=MaxRND){
				MaxRND+=300;
				RNDF=(char**)realloc(RNDF,4*MaxRND);
				RndData=(DWORD*)realloc(RndData,4*MaxRND);
				Ridx=(word*)realloc(Ridx,2*MaxRND);
			};
			Ridx[NRND]=NRND;
			RNDF[NRND]=znew(char,strlen(FD.cFileName)+1);
			strcpy(RNDF[NRND],FD.cFileName);
			RndData[NRND]=FD.ftCreationTime.dwHighDateTime;
			NRND++;
			r=FindNextFile(HF,&FD)!=0;
		}while(r);
		if(NRND>3){
			SortClass SORT;
			SORT.CheckSize(NRND);
			memcpy(SORT.Parms,RndData,4*NRND);
			memcpy(SORT.Uids,Ridx,2*NRND);
			SORT.NUids=NRND;
			SORT.Sort();
			memcpy(Ridx,SORT.Uids,2*NRND);
			SORT.Copy(Ridx);
			for(int i=0;i<NRND-3;i++){
				DeleteFile(RNDF[Ridx[i]]);
			};
		};
		if(NRND){
			for(int i=0;i<NRND;i++)free(RNDF[i]);
			free(RNDF);
			free(Ridx);
			free(RndData);
		};
	};
};

void __stdcall CDGINIT_INIT3();
void FilesInit();
void FilesExit();
void PlayCDTrack(int Id);
void PlayRandomTrack();
extern int PlayMode;
#ifdef SPEEDFIX
unsigned long GetRealTime();
#else
int GetRealTime();
#endif
void CreateReg(){
	char path [300];
	char path1[350];
	GetCurrentDirectory(300,path);
	int ps1=0;
	int ps=0;
	char c;
	do{
		c=path[ps];
		if(c=='\\'){
			path1[ps1]='\\';
			path1[ps1+1]='\\';
			ps1+=2;
		}else{
			path1[ps1]=c;
			ps1++;
		};
		ps++;
	}while(c);
	GFILE* f=Gopen("Cossacks.reg","w");
	Gprintf(f,"REGEDIT4\n[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\DirectPlay]\n[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\DirectPlay\\Applications]\n[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\DirectPlay\\Applications\\Cossacks]\n\"CommandLine\"=\"\"\n\"CurrentDirectory\"=\"%s\"\n\"File\"=\"dmcr.exe\"\n\"Guid\"=\"{5BFDB060-06A4-11d0-9C4F-00A0C705475f}\"\n\"Path\"=\"%s\"\n",path1,path1);
	Gclose(f);
};
typedef bool tpShowDialog(int NModes,int* Sizex,int* Sizey,int* Current);
tpShowDialog* lpShowDialog;
extern int ModeLX[32];
extern int ModeLY[32];
extern int NModes;
bool EnumModesOnly();
//#ifndef MAKE_PTC
void SetCDVolume(int Vol);
bool RunSMD(){
	bool rs=0;
	if(EnumModesOnly()){
		HINSTANCE hLib=LoadLibrary("SelectMode.dll ");
		if(hLib){
			GFILE* fff=Gopen("mode.dat","rt");
			RealLx=1024;
			RealLy=768;
			exRealLx=1024;
			exRealLy=768;
			WarSound=0;
			WorkSound=0;
			OrderSound=0;
			MidiSound=0;
            int ex_window_x, ex_window_y, ex_x, ex_y;
            int dummy;
            int FPSTimeSF;
            if (window_mode)
            {
                ex_window_x = exRealLx;
                ex_window_y = exRealLy;
                ex_x = ex_other_RealLx;
                ex_y = ex_other_RealLy;
            }
            else
            {
                ex_x = exRealLx;
                ex_y = exRealLy;
                ex_window_x = ex_other_RealLx;
                ex_window_y = ex_other_RealLy;
            }
			if(fff){
#ifdef SPEEDFIX
                Gscanf(fff, "%d%d%d%d%d%d%d%d%d%d", &exRealLx, &exRealLy, &WarSound, &OrderSound, &OrderSound, &MidiSound, &FPSTimeSF, &ScrollSpeed, &exFMode, &PlayMode);
#else
                Gscanf(fff,"%d%d%d%d%d%d%d%d%d%d%d%d%d",&ex_window_x,&ex_window_y,&WarSound,&OrderSound,&OrderSound,&MidiSound,&dummy, &FPSTime,&ScrollSpeed,&exFMode,&PlayMode,&ex_x ,&ex_y);
#endif
                SetCDVolume(MidiSound);
				Gclose(fff);
                
			};
			int cr=0;
			for(int j=0;j<NModes;j++)if(ModeLX[j]==exRealLx&&ModeLY[j]==exRealLy)cr=j;
			lpShowDialog=(tpShowDialog*)GetProcAddress(hLib,"ShowDialog");
			if(lpShowDialog){
				if(lpShowDialog(NModes,ModeLX,ModeLY,&cr)){
					exRealLx=ModeLX[cr];
					exRealLy=ModeLY[cr];
					GFILE* fff=Gopen("mode.dat","wt");
					if(fff){
#ifdef SPEEDFIX
                        Gscanf(fff, "%d%d%d%d%d%d%d%d%d%d", &exRealLx, &exRealLy, &WarSound, &OrderSound, &OrderSound, &MidiSound, &FPSTimeSF, &ScrollSpeed, &exFMode, &PlayMode);
#else
                        Gprintf(fff,"%d %d %d %d %d %d %d %d %d %d %d %d",ex_window_x,ex_window_y,WarSound,OrderSound,OrderSound,MidiSound,FPSTime,ScrollSpeed,exFMode,PlayMode,ex_x ,ex_y);
#endif
                        SetCDVolume(MidiSound);
						Gclose(fff);
					};
					rs=1;
				};
			};
			FreeLibrary(hLib);
		};
	};
	return rs;
};
//#endif
SFLB_DLLEXPORT SFLB_InitDialogs();
CEXPORT
word dwVersion=150;
//#define TESTFORCE
#ifdef STARFORCE
HGLOBAL PTR_AI=NULL;
void __declspec(dllexport) SFINIT0_InitAI(){
	PTR_AI=GlobalAlloc(GMEM_FIXED,4);
	((DWORD*)PTR_AI)[0]=0x4351A234;
};
HGLOBAL PTR_MISS=NULL;
void __declspec(dllexport) SFINIT1_InitMISS(){
	PTR_MISS=GlobalAlloc(GMEM_FIXED,4);
	((DWORD*)PTR_MISS)[0]=0x4351A234;
};
HGLOBAL PTR_LAND=NULL;
void __declspec(dllexport) SFINIT2_InitLAND(){
	PTR_LAND=GlobalAlloc(GMEM_FIXED,4);
	((DWORD*)PTR_LAND)[0]=0x4344EA34;
};
#endif
void Rept (LPSTR sz,...)
{
	/*
        char ach[256];
        va_list va;

        va_start( va, sz );
        vsprintf ( ach, sz, va );   
        va_end( va );
		FILE* f=fopen("log.log","a");
		fprintf(f,ach);
		fclose(f);
	*/
};
void LogIt (LPSTR sz,...)
{
	
        char ach[256];
        va_list va;

        va_start( va, sz );
        vsprintf ( ach, sz, va );   
        va_end( va );
		FILE* f=fopen("log.log","a");
		fprintf(f,ach);
		fclose(f);
	
};
extern bool RUNMAPEDITOR;
extern bool RUNUSERMISSION;
extern char USERMISSPATH[128];
void CheckAttr();
void TestHash();
void CheckIntegrity();
extern bool TOTALEXIT;
int GetRankByScore(int Score);
CEXPORT char LobbyVersion[32] = "1.50";
void InitWinTab();
void StartTest();
void FinExplorer();
void StartExplorer();
char AI_Log[256]="";
char CurrentGameLogDir[256]="";
void ClearMessages();
extern MEMORYSTATUS FEX_MemStatus1;
void DelLog();
void DDLog (LPSTR sz,...);

int Alert(const char* lpCaption, const char* lpText)
{
#ifndef _WIN32
    fprintf(stderr, "Message: '%s', Detail: '%s'\n", lpCaption, lpText);
    return 0;
#else
    return ::MessageBox(NULL, lpText, lpCaption, MB_OK);
#endif
}

#ifdef STEAM
int SteamInitialisation() {
    if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
    {
        // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
        // local Steam client and also launches this game again.

        // Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
        // removed steam_appid.txt from the game depot.

        return EXIT_FAILURE;
    }
    SteamErrMsg errMsg = { 0 };
    if (SteamAPI_InitEx(&errMsg) != k_ESteamAPIInitResult_OK)
    {
        OutputDebugString("SteamAPI_Init() failed: ");
        OutputDebugString(errMsg);
        OutputDebugString("\n");

        Alert("Fatal Error", "Steam must be running to play this game (SteamAPI_Init() failed).\n");
        return EXIT_FAILURE;
    }
    if (!SteamUser()->BLoggedOn())
    {
        OutputDebugString("Steam user is not logged in\n");
        Alert("Fatal Error", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n");
        return EXIT_FAILURE;
    }
}
#endif
void SetDebugMode();
void NoDebugMode();
#ifdef STEAM
extern "C" __declspec(dllimport) void UpdateSteamExplorer(); 
#endif
extern BOOL DDDebug;
//int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//   LPSTR lpCmdLine, int nCmdShow)
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//TestHash();
#ifdef STEAM
    SteamInitialisation();
#endif
	DelLog();
	FEX_BEGIN();	
	if(FEX_MemStatus1.dwTotalVirtual<1024*1024*700){
		if(MessageBox( hwnd,"Not enough virtual memory (at least 700M required).\nMay be your Windows swap-file is limited.\n", "LOADING ERROR...", MB_RETRYCANCEL )==IDCANCEL)
			return 0;

	};

	FILE* F7=fopen("ai.log","r");
	if(F7){
        if (fscanf(F7, "%s", AI_Log) != 1) {
            // Handle error (file read failed)
            AI_Log[0] = '\0'; // Ensure null termination
        }
		fclose(F7);
	};
	//SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
	
#ifdef TESTFORCE
	SFINIT0_InitAI();
	SFINIT1_InitMISS();
	SFINIT2_InitLAND();
#endif
	CheckIntegrity();
	char* ss=strstr(lpCmdLine,"/MAPEDITOR");
	if(ss){
		RUNMAPEDITOR=1;
		strcpy(USERMISSPATH,ss+11);
	}else{
		ss=strstr(lpCmdLine,"/MISSION");
		if(ss){
			RUNUSERMISSION=1;
			strcpy(USERMISSPATH,ss+9);
		};
	};
    if (strstr(lpCmdLine, "/borderless"))
    {
        window_mode = true;
        window_style = WS_POPUP;
    }
    else if (strstr(lpCmdLine, "/window"))
    {
        window_mode = true;
        window_style = WS_OVERLAPPEDWINDOW;
    }
    else
    {
        window_mode = false;
    }
    if (strstr(lpCmdLine, "/stretch")) {
        bStretchMode = true;
    }
#ifndef CDVERSION
	CDGINIT_EnCD();
	CDGINIT_INIT3();
#endif 
//#ifndef MAKE_PTC
	//if(!RunSMD()){
	//	FilesExit();
	//	PostMessage(hwnd, WM_CLOSE, 0, 0);
	//	return 0;
	//};
//#else
	EnumModesOnly();
//#endif
	CreateReg();
	if(NoCD){
		MessageBox(NULL,"Please insert CD and try again.","CD errror",0);
		return 0;
	};
	FilesInit();

	EraseRND();
    

    ScreenPtr = NULL;

    ChangeNation = false;
    MultiTvar = false;
    MEditMode = false;
    WaterEditMode = false;

	Shifter=5;
	Multip=0;
    AutoTime = 0;
    BlobMode = 0;
    CostThickness = 4;
    EditMedia = 0;

	SetupRenderStack();

	CreateRadio();
	//LoadMediaFile();
	SpecCmd=0;
	sfVersion=291;
	strcpy(BuildVersion,"V 1.50");//"V 1.46");
	//Quality=2;
	RealLx=1024;
	RealLy=768;
	exRealLx=1024;
	exRealLy=768;

	WarSound=0;
	WorkSound=0;
	OrderSound=0;
	MidiSound=0;
    int FPSTimeSF = 50;
	
	InitObjs3();

#ifndef SPEEDFIX
    FPSTime = 50;
#endif

	GFILE* FF1=Gopen("Lobby.txt","r");
	if(FF1){
		int vv=dwVersion;
		Gscanf(FF1,"%d %s %s",&vv,LobbyVersion,BuildVersion);
		dwVersion=dwVersion+(vv<<8);
		Gclose(FF1);
	};
	GFILE* fff=Gopen("mode.dat","rt");
	AText("mode.dat");
	ScrollSpeed=5;
	if(fff){
        //Distinguish between last window adn fullscreen resolutions
        int ex_window_x, ex_window_y, ex_x, ex_y;
        int dummy;
        //7th value was FPSTime
		#ifdef SPEEDFIX
		Gscanf(fff,"%d%d%d%d%d%d%d%d%d%d",&exRealLx,&exRealLy,&WarSound,&OrderSound,&OrderSound,&MidiSound,&FPSTimeSF,&ScrollSpeed,&exFMode,&PlayMode);
		#else
		Gscanf(fff,"%d%d%d%d%d%d%d%d%d%d%d%d%d",&ex_window_x,&ex_window_y,&WarSound,&OrderSound,&OrderSound,&MidiSound,&dummy, &FPSTime,&ScrollSpeed,&exFMode,&PlayMode,&ex_x ,&ex_y);
		#endif
        Gclose(fff);

        //Set last 'global resolution' according to current mode
        if (window_mode)
        {
            exRealLx = ex_window_x;
            exRealLy = ex_window_y;
            ex_other_RealLx = ex_x;
            ex_other_RealLy = ex_y;
        }
        else
        {
            exRealLx = ex_x;
            exRealLy = ex_y;
            ex_other_RealLx = ex_window_x;
            ex_other_RealLy = ex_window_y;
        }
	};
	
#ifndef MAKE_PTC
	RealLx=exRealLx;
	RealLy=exRealLy;
#endif
	bool ExMode=0;
	for(int i=0;i<NModes;i++){
		if(ModeLX[i]==exRealLx&&ModeLY[i]==exRealLy)ExMode=1;
	};
	if(!ExMode){
		exRealLx=1024;
		exRealLy=768;
	};

    screen_width = GetSystemMetrics(SM_CXSCREEN);
    screen_height = GetSystemMetrics(SM_CYSCREEN);

    double scale = 0.01;
    screen_ratio = (double)screen_width / screen_height;
    screen_ratio = (int)(screen_ratio / scale) * scale;

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

	//CreateRandomHMap();
	
    MSG         msg;

    tima = 0;
    PlayerMask = 1;
    Flips = 0;
    tmtim = 0;

    HealthMode = false;
    InfoMode = true;
    DeathMode = false;
    AttackMode = false;
	
	InitFishMap();
	MemReport("InitFishMap");
	SetupGates();
	MemReport("SetupGates");

	// debug
	NoDebugMode();
	//SetDebugMode();
#if defined(VITAL_DEBUG)
	SetDebugMode();
#elif defined(VITAL_NO_DEBUG)
	NoDebugMode();
#endif

	// -- Inserted by Silver ---21.04.2003
	//  ok, folks, here we go ;)
	FILE* fp = fopen( "c:\\dumps\\file_you_do_not_have_on_your_computer.txt", "rt" );
	if (fp)
	{
		SetDebugMode();
	}
	// -- end of change -- 21.04.2003

	LockGrid=false;
	FILE* F=fopen("cew.dll","r");
	if(!F){
		MessageBox(NULL,"CEW.DLL not found. Unable to run Cossacks.","Error...",MB_ICONERROR);
		return 0;
	}else fclose(F);
	CheckAttr();
	//Init3DMapSystem();

	MemReport("Init3DMapSystem");
	SetupNatList();
	MemReport("SetupNatList");
	makeFden();
	MemReport("MakeFDen");
	PlayerMenuMode=1;
	InitStack();
	Creator=4096+255;
	xxx=0;
	cadr=0;
	InitZones();
	MemReport("InitZones");
	InitEBuf();
	MemReport("InitEBuf");
    TransMode=false;
	MUSTDRAW=false;
	InitXShift();
    DLL0(SFLB_InitDialogs);
	InitWater();
	LoadRLC("xrcross.rlc",&RCross);

	memset(Events,0,sizeof Events);
	MemReport("LoadingFonts");
	GetRealTime();

    if( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    };

#ifdef _USE3D
	extern IRenderSystem* IRS;
	ScreenProp prop = IRS->GetScreenProperties();
	prop.width = RealLx;
	prop.height = RealLy;
	IRS->SetScreenProperties( prop );
#endif // _USE3D

	SetCDVolume(MidiSound);
	//mpl_Play("mp3\\co_Green01.mp3");
	//InitWinTab();
	LoadFog(2);
	LoadPalette("1\\agew_1.pal");
	MemReport("doInit");
	SetupMultiplayer(hInstance);
	MemReport("SetupMultiplayer");
	InitMultiDialogs();
	MemReport("InitMultiDialogs");
	CloseMemReport();
	SetupHint();
	tmtmt=0;
	REALTIME=0;
	//processMainMenu();
	KeyPressed=false;
	//SetCursorPos(512,300);
	//mouseX=0;
	//mouseY=0;
	//SetMPtr(512,300,0);
	//mouseX=512;
	//mouseY=300;
	OnMouseMoveRedraw(); 
#ifdef _USE3D
	void LoadAllGModels();
	LoadAllGModels();
#endif //_USE3D
	//if(PlayMode)PlayRandomTrack();
	while (TRUE)
{
    // CRITICAL: Process SDL events FIRST before Windows messages
    // This ensures mouse/keyboard input is properly converted to Windows messages
    ProcessMessagesSDL();
    
    // Now process Windows messages
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            PostQuitMessage(msg.wParam);
            return 1;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(bActive){
        StartExplorer();
#ifdef STEAM
        // Update Steam lobbies
        
        UpdateSteamExplorer();  // Add this!
#endif
        InVideo=0;
        AllGame();
        ClearScreen();
        UnLoading();
        CloseExplosions();
        ShutdownMultiplayer(1);
        SaveAllFiles();
        
        // Save settings...
        int ex_window_x, ex_window_y, ex_x, ex_y;
        int FPSTimeSF;
        
        if (window_mode)
        {
            ex_window_x = exRealLx;
            ex_window_y = exRealLy;
            ex_x = ex_other_RealLx;
            ex_y = ex_other_RealLy;
        }
        else
        {
            ex_x = exRealLx;
            ex_y = exRealLy;
            ex_window_x = ex_other_RealLx;
            ex_window_y = ex_other_RealLy;
        }

        GFILE* fff=Gopen("mode.dat","wt");
        if(fff){
				#ifdef SPEEDFIX
				Gprintf(fff,"%d %d %d %d %d %d %d %d %d %d",exRealLx,exRealLy,WarSound,OrderSound,OrderSound,MidiSound,FPSTimeSF,ScrollSpeed,exFMode,PlayMode);
				#else
				Gprintf(fff,"%d %d %d %d %d %d %d %d %d %d %d %d %d",ex_window_x,ex_window_y,WarSound,OrderSound,OrderSound,MidiSound, 0,FPSTime,ScrollSpeed,exFMode,PlayMode,ex_x ,ex_y);
				#endif
            Gclose(fff);
        };
        
        FilesExit();
        StopPlayCD();
        if(TOTALEXIT)
            ShellExecute(NULL,"open","http://www.goa.com/goa/z-home.asp?gotogame=8247",NULL,NULL,SW_MAXIMIZE);
#ifdef STARFORCE
        if(PTR_AI)GlobalFree(PTR_AI);
        if(PTR_MISS)GlobalFree(PTR_MISS);
#endif
        ClearMessages();
        void FreeAll();
        FreeAll();
        PostMessage(hwnd, WM_CLOSE, 0, 0);
        FinExplorer();
        };
    };
	FEX_END();
    return msg.wParam;

}; /* WinMain */
void CleanAll();
void Free_GP_IMG();
extern int* DMap;
void ClearRadio();
void ClearNatList();
void CloseSoundSystem();
void FreeIcons();
void FreeNations();
void FreeRDS();
void FreeTextures();
extern RLCTable mptr;
void FreeWNames();
void FreeFiresAnm();
void CloseExplosions();
void ClearMonNames();
void FreeAll(){
	ClearMonNames();
	CloseExplosions();
	FreeFiresAnm();
	FreeWNames();
	FreeTextures();
	FreeRDS();
	FreeNations();
	FreeIcons();
	CloseSoundSystem();
	ClearNatList();
	Free_GP_IMG();
	CleanAll();
	free(DMap);
	ClearRadio();
	free(RCross);
	RCross=NULL;
	if(mptr)free(mptr);
	mptr=NULL;
};