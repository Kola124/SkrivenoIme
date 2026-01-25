#include "ddini.h"
#include <stdlib.h>
#include "ResFile.h"
#include "Fastdraw.h"
#include "MapDiscr.h"
#include "mouse.h"
#include "mode.h"
#include "fog.h"
#include "walls.h"
#include "Nature.h"
#include <crtdbg.h>
#include <math.h>
#include "Megapolis.h"
#include "fonts.h"
#include "WeaponID.h"
#include "3DSurf.h"
#include "GSound.h"
#include <assert.h>
#include "3DMapEd.h"
#include "MapSprites.h"
#include "GP_Draw.h"
#include "RealWater.h"
#include "ZBuffer.h"
#include "NewAI.h"
#include "TopoGraf.h"
#include "StrategyResearch.h"
#include "Curve.h"
#include "NewMon.h"
#include "ActiveScenary.h"
#include "ConstStr.h"
#include "DrawForm.h"
#include "IconTool.h"
#include "Dialogs.h"
#include "Path.h"
#include "Sort.h"
#include "MPlayer.h"
#include "Graphs.h"
#include "Safety.h"
#include "EinfoClass.h"

#ifdef _USE3D
#include "..\3DLIB\gMotor\mCamera.h"
#include "Scape3D.h"
extern Camera CAM;
#endif // _USE3D

int LastActionX=0;
int LastActionY=0;

extern int UNI_LINEDLY1;
extern int UNI_LINEDY1;
extern int UNI_LINEDLY2;
extern int UNI_LINEDY2;
extern int UNI_HINTDY1;
extern int UNI_HINTDLY1;
extern int UNI_HINTDY2;
extern int UNI_HINTDLY2;

bool NoPFOG=0;
int Inform=0;
bool LockPause=0;
bool MiniActive=0;
void ShowShipsDest(Brigade* BR);
void DrawDangerMap();
word GetDir(int,int);
void HandleGeology();
void HandleShar(Nation* NT);
word CurZone;
int MRealX,MRealY;
void TestBlob();
bool NoText=false;
void DrawFog();
void ShowAllBars();
extern bool MEditMode;
extern byte EditMedia;
extern bool BuildWall;
extern int LOADED;
extern bool SHOWSLIDE;
extern bool CINFMOD;
void ShowMines(NewMonster* NM);
void ShowRLCItemDarkN(int x,int y,lpRLCTable lprt,int n,int Ints);
void ShowRLCItemRedN(int x,int y,lpRLCTable lprt,int n,int Ints);
int CheckCreationAbility(byte NI,NewMonster* NM,int* x2i,int* y2i,word* BLD,int NBLD);
void CmdGoToTransport(byte NI,word ID);
bool GetRealCoords(int xs,int ys,int* xr,int* yr);
extern bool KeyPressed;
extern bool Mode3D;
extern bool FullMini;
void TestTriangle();
extern int time1,time2,time3,time4,time5;
int time6,time7,time8;
extern int PortBuiX,PortBuiY;
extern int HintX;
extern int HintY;
extern bool CheapMode;
extern int NMyUnits;
extern int NThemUnits;
extern RLCFont FPassive;
extern RLCFont FActive;
extern RLCFont FDisable;
int GetRLen(char* s,RLCFont* font);
word GetEnemy(int x,int y,byte NI);
int COUNTER;
extern int BlobMode;
extern int LASTRAND,LASTIND;
void ShowRMap();
extern int FogMode;
extern int RealLx;
extern int RealLy;
extern int Pitch;
extern Forces NForces[8];
extern bool GetCoord;
extern UniqMethood* UNIM;
void LShow();
//Карта неустранимых препятствий
extern int PathAsks;
extern int NMONS;
extern bool AttackMode;
extern int nEused;
extern bool EUsage[1024];
extern Nation* curn;
extern int counter;
extern bool InfoMode;
extern int WaitCycle;
extern int tmtmt;
extern bool EditMapMode;
extern int HeightEditMode;
extern bool HelpMode;
extern bool ChoosePosition;
extern int LastKey;
extern bool PeaceMode;
extern byte LockMode;
const byte CLRT[8]={0xD0,0xD4,0xD8,0xDC,0xE0,0xE4,0xE8,0xAD};
void ShowDestn();
void ShowMiniDestn();
extern byte RNTB[64][256];
void CreateMiniMap();
word Nsel;
int TerrHi;

bool BuildMode;
byte SpecCmd;
OneSlide* OSB;
byte blx;
byte bly;
word BuildingID;
Nation* BNat;
extern int NAsk;
extern int curptr;
int	smapx;
int	smapy;
int	smaplx;
int	smaply;
int	mapx;
int	mapy;
int	msx;
int msy;
int minix;
int	miniy;
static int StartX=0;
static int StartY=0;
bool MiniMade;
char Prompt[80];
int PromptTime;
byte minimap[maxmap][maxmap];
byte Locking[1024];//Инф. о блокировании поверхности
#define ScreenSizeX32 ScreenSizeX-32
//Массив все монстров на карте
OneObject* Group[ULIMIT];
//Mассив для локальных команд
//Индексы использования
bool	AsmUsage[MaxAsmCount];
char	AsmBuf[MaxAsmCount*OneAsmSize];
int		LastAsmRequest;
//Массив для команд первого уровня
bool	OrdUsage[MaxOrdCount];
Order1  OrdBuf[MaxOrdCount];
int		LastOrdRequest;
//network sequence errors enumeration
extern int SeqErrorsCount;
extern int LastRandDif;
extern int curdx;
extern int curdy;
//---------------------------------------------G R I D S------------------------------------//
int GridX=4;
int GridY=4;
int GridLx=16;
int GridLy=24;
int GridNx=230/16;
int GridNy=230/32;
int GridMode=0;

void SelectNextGridMode(){
	switch(GridMode){
	case 0:
		GridMode=1;
		//GridLx=8;
		//GridLy=12;
		GridLx=8;
		GridLy=12;
		break;
	case 1:
		GridMode=2;
		//GridLx=12;
		//GridLy=18;
		GridLx=16;
		GridLy=24;
		break;
	case 2:
		GridMode=3;
		//GridLx=16;
		//GridLy=24;
		GridLx=24;
		GridLy=36;
		break;
	case 3:
		GridMode=4;
		//GridLx=20;
		//GridLy=30;
		GridLx=32;
		GridLy=48;
		break;
	case 4:
		GridMode=5;
		//GridLx=24;
		//GridLy=36;
		GridLx=40;
		GridLy=60;
		break;
	case 5:
		GridMode=0;
		break;
	};
};
void DeleteInArea(int x0,int y0,int x1,int y1);
void DelInGrid(int x,int y){
	if(!GridMode)return;
	int Gx=((x-GridX*64)>>6)/GridLx;
	int Gy=((y-GridY*32)>>5)/GridLy;
	if(Gx>=0&&Gx<GridNx&&Gy>=0&&Gy<GridNy){
		DeleteInArea((Gx*GridLx+GridX)*64-1,(Gy*GridLy+GridY)*32-1,((Gx+1)*GridLx+GridX)*64+1,((Gy+1)*GridLy+GridY)*32+1);
	};
};
void DrawGrids(){
	int scshf=0;
	if(LMode)scshf=2;
	int x0=(mapx-GridX*2)/(2*GridLx);
	int Nx=2+smaplx/(2*GridLx);
	int yMin,yMax,xMin,xMax;
	GridNx=(msx-GridX*4)/(GridLx*2);
	GridNy=(msx-GridY*4)/(GridLy*2);

	yMin=(GridY<<1)-mapy;
	if(yMin<0)yMin=0;
	yMax=((GridY+GridLy*GridNy)<<1)-mapy;
	if(yMax>smaply)yMax=smaply;
	yMin=smapy+(yMin<<4);
	yMax=smapy+(yMax<<4);

	xMin=(GridX<<1)-mapx;
	if(xMin<0)xMin=0;
	xMax=((GridX+GridLx*GridNx)<<1)-mapx;
	if(xMax>smaplx)xMax=smaplx;
	xMin=smapx+(xMin<<5);
	xMax=smapx+(xMax<<5);


	for(int i=0;i<Nx+1;i++){
		int x=(GridX+(i+x0)*GridLx)<<1;
		if(x>=mapx&&x<=mapx+smaplx){
			x=smapx+((x-mapx)<<5);
			DrawLine(x>>scshf,yMin>>scshf,x>>scshf,yMax>>scshf,0x9A);
		};
	};
	int y0=(mapy-GridY*2)/(2*GridLy);
	int Ny=2+smaply/(2*GridLy);
	for(int i=0;i<Ny+1;i++){
		int y=(GridY+(i+y0)*GridLy)<<1;
		if(y>=mapy&&y<=mapy+smaply){
			y=smapy+((y-mapy)<<4);
			DrawLine(xMin>>scshf,y>>scshf,xMax>>scshf,y>>scshf,0x9A);
		};
	};
	if(GetKeyState(VK_CONTROL)&0x8000){
		int Dx=GridLx*32;
		for(int i=0;i<Nx+2;i++){
			int x1=(GridX+(i+x0)*GridLx)<<1;
			int x=smapx+((x1-mapx)<<5);
			if(x1>=mapx&&x1<=mapx+smaplx){
				DrawLine((x+1)>>scshf,yMin>>scshf,(x+1)>>scshf,yMax>>scshf,0x9A);
				DrawLine((x-1)>>scshf,yMin>>scshf,(x-1)>>scshf,yMax>>scshf,0x9A);
			};
			if(x0+i)DrawLine((x-Dx)>>scshf,yMin>>scshf,(x-Dx)>>scshf,yMax>>scshf,0x9A);
			if(x0+i)DrawLine((x+Dx)>>scshf,yMin>>scshf,(x+Dx)>>scshf,yMax>>scshf,0x9A);
		};
		int y0=(mapy-GridY*2)/(2*GridLy);
		int Ny=2+smaply/(2*GridLy);
		int Dy=GridLy*16;
		for(int i=0;i<Ny+2;i++){
			int y1=(GridY+(i+y0)*GridLy)<<1;
			int y=smapy+((y1-mapy)<<4);
			if(y1>=mapy&&y1<=mapy+smaply){
				DrawLine(xMin>>scshf,(y-1)>>scshf,xMax>>scshf,(y-1)>>scshf,0x9A);
				DrawLine(xMin>>scshf,(y+1)>>scshf,xMax>>scshf,(y+1)>>scshf,0x9A);
			};
			if(y0+i)DrawLine(xMin>>scshf,(y-Dy)>>scshf,xMax>>scshf,(y-Dy)>>scshf,0x9A);
			if(y0+i)DrawLine(xMin>>scshf,(y+Dy)>>scshf,xMax>>scshf,(y+Dy)>>scshf,0x9A);
		};
		x0--;
		y0--;
		if(x0<0)x0=0;
		if(y0<0)y0=0;
		int NX=(smaplx>>3)+2;
		int NY=(smaply>>3)+2;
		int X0=(-(mapx&7))<<5;
		int Y0=(-(mapy&7))<<4;
		for(int IX=0;IX<NX;IX++)
			for(int IY=0;IY<NY;IY++){
				int xx=X0+IX*256;
				int yy=Y0+IY*128;
				DrawLine((xx>>scshf)-4,(yy>>scshf)-4,(xx>>scshf)+4,(yy>>scshf)+4,0x9A);
				DrawLine((xx>>scshf)+4,(yy>>scshf)-4,(xx>>scshf)-4,(yy>>scshf)+4,0x9A);
				xx+=128;
				yy+=64;
				DrawLine((xx>>scshf)-4,(yy>>scshf)-4,(xx>>scshf)+4,(yy>>scshf)+4,0x7A);
				DrawLine((xx>>scshf)+4,(yy>>scshf)-4,(xx>>scshf)-4,(yy>>scshf)+4,0x7A);
			};


		for(int ix=0;ix<Nx+2;ix++)
			for(int iy=0;iy<Ny+2;iy++){
				int x=(ix+ix+x0+x0+1)*GridLx+GridX+GridX;
				int y=(iy+iy+y0+y0+1)*GridLy+GridY+GridY;
				if(x>=mapx&&x<=mapx+smaplx&&y>=mapy&&y<=mapy+smaply&&x0+ix<GridNx&&y0+iy<GridNy){
					int N=ix+x0+(iy+y0)*GridNx;
					char cc[5];
					if(N<10)sprintf_s( cc,sizeof(cc),"00%d",N);
					else if(N<100)sprintf_s( cc,sizeof(cc),"0%d",N);
					else sprintf_s( cc,sizeof(cc),"%d",N);
					ShowString((smapx+(x-mapx)*32)>>scshf,(smapy+(y-mapy)*16)>>scshf,cc,&WhiteFont);
				};
			};
	};
};
//------------------------------------------------------------------------------------------//
void ShowNewMonsters();
//Фон
int ConvY(int y){
	return div24(y<<5);
}
void RedSquare(int x,int y){
	/*
	if(x<mapx||x>=mapx+smaplx||y<mapy||y>=mapy+smaply)return;
	int sco=int(ScreenPtr)+smapx+((x-mapx)<<5)+
		(((y-mapy)<<5)+smapy)*SCRSizeX;
	int ddx=SCRSizeX+SCRSizeX-32;
	__asm{
		push	edi
		mov		edi,sco
		mov		eax,16
uyu:	mov		ecx,16
uuu:	mov		byte ptr [edi],clrRed
		add		edi,2
		loop	uuu
		add		edi,ddx
		dec		al
		jnz		uyu

		pop		edi
	};
	*/
};
void RedMiniSquare(int x,int y){
	/*
	if(x<mapx||x>=mapx+smaplx||y<mapy||y>=mapy+smaply)return;
	int sco=int(ScreenPtr)+smapx+((x-mapx)<<4)+
		(((y-mapy)<<4)+smapy)*SCRSizeX;
	int ddx=SCRSizeX+SCRSizeX-16;
	__asm{
		push	edi
		mov		edi,sco
		mov		eax,8
uyu:	mov		ecx,8
uuu:	mov		byte ptr [edi],clrRed
		add		edi,2
		loop	uuu
		add		edi,ddx
		dec		al
		jnz		uyu

		pop		edi
	};
	*/
};
void RedBar(int x,int y,int lx,int ly){
	for(int i=0;i<lx;i++)
		for(int j=0;j<ly;j++)
			RedSquare(x+i,y+j);
};
void RedMiniBar(int x,int y,int lx,int ly){
	for(int i=0;i<lx;i++)
		for(int j=0;j<ly;j++)
			RedMiniSquare(x+i,y+j);
};
void WhiteSquare(int x,int y){
	if(x<mapx||x>=mapx+smaplx||y<mapy||y>=mapy+smaply)return;
	int sco=int(ScreenPtr)+smapx+((x-mapx)<<5)+
		(((y-mapy)<<5)+smapy)*SCRSizeX;
	int ddx=SCRSizeX+SCRSizeX-32;
	__asm{
		push	edi
		mov		edi,sco
		mov		eax,16
uyu:	mov		ecx,16
uuu:	mov		byte ptr [edi],255
		add		edi,2
		loop	uuu
		add		edi,ddx
		dec		al
		jnz		uyu

		pop		edi
	};
};
void WhiteMiniSquare(int x,int y){
	if(x<mapx||x>=mapx+smaplx||y<mapy||y>=mapy+smaply)return;
	int sco=int(ScreenPtr)+smapx+((x-mapx)<<4)+
		(((y-mapy)<<4)+smapy)*SCRSizeX;
	int ddx=SCRSizeX+SCRSizeX-16;
	__asm{
		push	edi
		mov		edi,sco
		mov		eax,8
uyu:	mov		ecx,8
uuu:	mov		byte ptr [edi],255
		add		edi,2
		loop	uuu
		add		edi,ddx
		dec		al
		jnz		uyu

		pop		edi
	};
};
void WhiteBar(int x,int y,int lx,int ly){
	for(int i=0;i<lx;i++)
		for(int j=0;j<ly;j++)
			WhiteSquare(x+i,y+j);
};
void WhiteMiniBar(int x,int y,int lx,int ly){
	for(int i=0;i<lx;i++)
		for(int j=0;j<ly;j++)
			WhiteMiniSquare(x+i,y+j);
};
extern int CurPalette;
void OutErr(LPCSTR s)
{
	char pal[128];
	sprintf(pal,"%d\\agew_1.pal",CurPalette);
	LoadPalette(pal);
	MessageBox(hwnd,s,"Loading failed...",MB_ICONWARNING|MB_OK);
};
bool ManualFogCheck(int xx,int yy,int dx){
	/*
	if(EditMapMode)return true;
	int x,y;
	for(int i=0;i<dx;i++)
		for(int j=0;j<dx;j++){
			x=xx+i;
			y=yy+j;
			if(x<=1||x>=msx||y<=1||y>=msy)return false;
			if(fmap[y][x]<1000)return false;
		};
		*/
	return true;
};
void AssignHint1(char* s,int time);
//Получить блок для InLineCom
char* GetAsmBlock(){
	if(LastAsmRequest>=MaxAsmCount)LastAsmRequest=0;
    int i=0;
	for(i=LastAsmRequest;i<MaxAsmCount&&AsmUsage[i];i++);
	if(i<MaxAsmCount){
		LastAsmRequest=i+1;
		AsmUsage[i]=true;
		return &AsmBuf[i*OneAsmSize];
	} else{
		for(i=0;i<LastAsmRequest&&AsmUsage[i];i++);
		if(i<LastAsmRequest){
			LastAsmRequest=i+1;
			AsmUsage[i]=true;
			return &AsmBuf[i*OneAsmSize];
		} else return NULL;
	};
};
void FreeAsmBlock(char* p ){
	int i=(int(p)-int(AsmBuf))>>OneAShift;
	AsmUsage[i]=false;
	LastAsmRequest=i;
};
void InitAsmBuf(){
	memset(AsmUsage,0,sizeof AsmUsage);
	memset(AsmBuf,0,sizeof AsmBuf);
	LastAsmRequest=0;
};
//Получить блок для LocalOrder
int CHSM1;
void GetOrdSum(){
	return;
	CHSM1=0;
	for(int i=0;i<MaxOrdCount;i++){
		if(OrdUsage[i])CHSM1+=i;
	};
};
void ChkOrdSum(){
	return;
	int xxx=CHSM1;
	//GetOrdSum();
	//assert(xxx=CHSM1);
};
Order1* GetOrdBlock(){
	Order1* OR1=new Order1;
	memset(OR1,0,sizeof Order1);
	return OR1;
	int i=0;
	//ChkOrdSum();
	if(LastOrdRequest>=MaxOrdCount)LastOrdRequest=0;
	for(i=LastOrdRequest;i<MaxOrdCount&&OrdUsage[i];i++);
	if(i<MaxOrdCount){
		LastOrdRequest=i+1;
		OrdUsage[i]=true;
		//GetOrdSum();
		memset(&OrdBuf[i],0,sizeof Order1);
		return &OrdBuf[i];
	} else{
		for(i=0;i<LastOrdRequest&&OrdUsage[i];i++);
		if(i<LastOrdRequest){
			LastOrdRequest=i+1;
			OrdUsage[i]=true;
			//GetOrdSum();
			memset(&OrdBuf[i],0,sizeof Order1);
			return &OrdBuf[i];
		} else{
			//GetOrdSum();			
			return NULL;
		};
	};
};
void OneObject::FreeOrdBlock(Order1* p ){
	delete(p);
	return;
	ChkOrdSum();
	byte ot=p->OrderType;
	if(ot==3){
		Attack=false;
		EnemyID=0xFFFF;
		//EnemyDist=5000;
	};
	if(ot==22)MoveInRect=false;
	int i=div(int(p)-int(OrdBuf),sizeof Order1).quot;
	OrdUsage[i]=false;
	LastOrdRequest=i;
	GetOrdSum();
};
void InitOrdBuf(){
	memset(OrdUsage,0,sizeof OrdUsage);
	memset(OrdBuf,0,sizeof OrdBuf);
	//LastOrdRequest=0;
};

void LoadLock()
{
	ResFile f=RReset("lock.dat");
	if(f==INVALID_HANDLE_VALUE)OutErr("Can't load locking.");
	RBlockRead(f,Locking,RFileSize(f));
	RClose(f);
};
void LoadFon()
{
	
};
void ShowFon()
{
	return;

};
void ShowFon1()
{

};
void ShowBorder()
{
	return;

};
int mul3(int x){
	return x+x;//x+x+x;
};
void LoadTiles()
{
	
};
void SaveWalls(ResFile ff1);
//void LoadWalls(ResFile ff1);
char MapName[128];
extern short HiMap[256][256];//Height of surface
byte mapPos[16];
int RES[8][8];

void CreateUnit(Nation* NT,byte x,byte y,word ID){
	
};
void PostLoadExtendedMap(char* s){
	memset(fmap,0,LX_fmap*LX_fmap*2);
	ResFile f1=RReset(s);
	int pos=12+16+128+8*8*4+(msx*msy)+(msx+1)*(msy+1)*2;
	RSeek(f1,pos);
	//Load units
	int NU;
	RBlockRead(f1,&NU,4);
	pos+=(NU+1)<<2;
	RSeek(f1,pos);
	RBlockRead(f1,&NU,4);
	byte NNUM,xu,yu;
	word NIndex;
	for(int i=0;i<NU;i++){
		RBlockRead(f1,&NIndex,2);
		RBlockRead(f1,&NNUM,1);
		RBlockRead(f1,&xu,1);
		RBlockRead(f1,&yu,1);
		Nation* NT=&NATIONS[NNUM];
		CreateUnit(NT,xu,yu,NIndex);
	};
	//walls
	//LoadWalls(f1);
	RClose(f1);
	memset(fmap,0,LX_fmap*LX_fmap*2);
};
void RestoreLock(int x,int y,int lx,int ly){
	
};
void ClearMaps();

void ClearMaps();
extern int MAXSPR;
void InitRiv();
extern MotionField UnitsField;
void ClearPixUndoStack();
void ClearAllLModeData();
void ResetGroundCache();
void NewMap(int szX,int szY)
{
#ifdef _USE3D
	ResetGroundCache();
#endif
	ClearAllLModeData();
	ClearPixUndoStack();
	PromptTime=0;
	MAXOBJECT=0;
	MAXSPR=0;
	memset(Group,0,sizeof Group);
	memset(NLocks,0,sizeof NLocks);
	ClearMaps();
	BuildMode=false;
	memset(&HiMap[0][0],0,sizeof HiMap);
	for(int i=0;i<NMFIELDS;i++){
		MFIELDS[i].ClearMaps();
	}
	//MFIELDS[1].ClearMaps();
	UnitsField.ClearMaps();
	MiniMade=false;
	Nsel=0;	
	InitRenderMap();
	msx=szX;
	msy=szY;
	InitRiv();
};
void CreateMiniMap();
const int drx[8]={0,1,1,1,0,-1,-1,-1};
const int dry[8]={-1,-1,0,1,1,1,0,-1};
extern bool HealthMode;
void GetRect(OneObject* ZZ,int* x,int* y,int* Lx,int* Ly){
	int scx,scy;
	if(ZZ->newMons){
		NewMonster* NMN=ZZ->newMons;
		scx=(ZZ->RealX>>4)-(NMN->RectLx>>1)+NMN->RectDx;
		scy=(mul3(ZZ->RealY)>>6)-(NMN->RectLy)+NMN->RectDy;
		if(Mode3D)scy-=ZZ->RZ;
		*x=scx;
		*y=scy;
		*Lx=NMN->RectLx;
		*Ly=NMN->RectLy;
		return;
	};
};
int YELLOWCL=0;
int RCOLOR=0;
byte GetPaletteColor(int r,int g,int b);
int SB_img=-1;
void DrawMarker(OneObject* OB){
#ifdef _USE3D
	extern float g_dbgZ;
	if(SB_img==-1){
		SB_img=GPS.PreLoadGPImage("selbar");
	};
	g_dbgZ+=0.01;
	GPS.ShowGP((OB->RealX>>4)-(mapx<<5)-16,(OB->RealY>>5)-(mapy<<4)-OB->RZ-16,SB_img,1,0);
	g_dbgZ-=0.01;
	return;
#endif
	if(OB&&!OB->Wall){
		if(!YELLOWCL){
			YELLOWCL=GetPaletteColor(255,255,0);
			RCOLOR=0xD0;//GetPaletteColor(255,0,0);
		};
		int x,y,Lx,Ly;
		GetRect(OB,&x,&y,&Lx,&Ly);
		if(LMode){
			x=(x>>2)-(mapx<<3);
			y=(y>>2)-(mapy<<2);
			Lx>>=2;
			Ly>>=2;
		}else{
			x+=smapx-(int(mapx)<<5);
			y+=smapy-(mul3(int(mapy))<<3);
		};
		byte cl=YELLOWCL;
		if(OB->ForceX||OB->ForceY)cl=RCOLOR;
		byte c1=0;
		byte c2=2;
		byte dc=4;
		int ddy=-12;
		Hline(x+c1,y,x+c2,cl);
		Hline(x+Lx-1-c2,y,x+Lx-1-c1,cl);
		Hline(x+c1,y+Ly-1,x+c2,cl);
		Hline(x+Lx-1-c2,y+Ly-1,x+Lx-1-c1,cl);
		Vline(x,y+c1,y+c2,cl);
		Vline(x+Lx-1,y+c1,y+c2,cl);
		Vline(x,y+Ly-1-c2,y+Ly-1-c1,cl);
		Vline(x+Lx-1,y+Ly-1-c2,y+Ly-1-c1,cl);
		
	};
};
void DrawImMarker(OneObject* OB){
	if(OB){
		int x,y,Lx,Ly;
		GetRect(OB,&x,&y,&Lx,&Ly);
		x--;
		y--;
		Lx+=2;
		Ly+=2;
		if(LMode){
			x-=mapx<<Shifter;
			y-=mapy<<(Shifter+1);
			Lx>>=2;
			Ly>>=2;
		}else{
			x+=smapx-(int(mapx)<<5);
			y+=smapy-(mul3(int(mapy))<<3);
		};
		byte cl=0xCD;
		//if(OB->ForceX||OB->ForceY)cl=0xCD;
		byte c1=0;
		byte c2=2;
		byte dc=4;
		int ddy=-12;
		Hline(x+c1,y,x+c2,cl);
		Hline(x+Lx-1-c2,y,x+Lx-1-c1,cl);
		Hline(x+c1,y+Ly-1,x+c2,cl);
		Hline(x+Lx-1-c2,y+Ly-1,x+Lx-1-c1,cl);
		Vline(x,y+c1,y+c2,cl);
		Vline(x+Lx-1,y+c1,y+c2,cl);
		Vline(x,y+Ly-1-c2,y+Ly-1-c1,cl);
		Vline(x+Lx-1,y+Ly-1-c2,y+Ly-1-c1,cl);
	};
};
void CBar(int x,int y,int Lx,int Ly,byte c);

#ifdef EW
extern City CITY[8];
#endif
void DrawHealth(OneObject* OB){
	if(LMode)return;
	if(OB->Sdoxlo)return;
	int x,y,Lx,Ly;
	GetRect(OB,&x,&y,&Lx,&Ly);
	x+=smapx-(int(mapx)<<5);
	y+=smapy-(mul3(int(mapy))<<3);
	byte c1=0;
	byte c2=2;
	byte dc=4;
	int ddy=-12;
	int mhs=Lx-c2-c2-dc-dc;
	int hs=mhs;
	if(OB->MaxLife>0){
		hs=div(mhs*OB->Life,OB->MaxLife).quot;
	};
	Hline(-2+x+c2+dc+1,y+ddy+1,x+c2+hs+dc+1,clrGreen);
	Hline(-2+x+c2+dc+1,y+ddy,x+c2+hs+dc+1,clrGreen);
#ifdef EW
    if (!OB->NewBuilding) {
        if ((OB->Morale - 330000) > 0) {

            byte drzava = OB->Nat->NMask;
            byte boja = 0;

            if (drzava == 128) {
                boja = GetPaletteColor(103, 63, 19);
            }
            else if (drzava == 64) {
                boja = GetPaletteColor(223, 223, 223);
            }
            else if (drzava == 32) {
                boja = GetPaletteColor(39, 39, 59);
            }
            else if (drzava == 16) {
                boja = GetPaletteColor(239, 131, 19);
            }
            else if (drzava == 8) {
                boja = GetPaletteColor(151, 71, 175);
            }
            else if (drzava == 4) {
                boja = GetPaletteColor(43, 179, 147);
            }
            else if (drzava == 2) {
                boja = GetPaletteColor(0, 59, 191);
            }
            else if (drzava == 1) {
                boja = GetPaletteColor(163, 0, 0);
            }

            if (OB->BrigadeID != 0xFFFF) {
                int M = CITY[OB->NNUM].Brigs[OB->BrigadeID].Morale;
                int MaxM = CITY[OB->NNUM].Brigs[OB->BrigadeID].MaxMorale;
                hs = div(mhs * (M - 330000), (MaxM - 330000)).quot;
            }
            else {
                hs = div(mhs * (OB->Morale - 330000), (OB->MaxMorale - 330000)).quot;
            }

            if (OB->newMons->ShowDelay) {
                Hline(-2 + x + c2 + dc + 1, y + ddy + 4, x + c2 + hs + dc + 1, boja);
                Hline(-2 + x + c2 + dc + 1, y + ddy + 5, x + c2 + hs + dc + 1, boja);

            }
            else {
                Hline(-2 + x + c2 + dc + 1, y + ddy + 2, x + c2 + hs + dc + 1, boja);
                Hline(-2 + x + c2 + dc + 1, y + ddy + 3, x + c2 + hs + dc + 1, boja);
            }
        };


    }
#endif
	if(OB->newMons->ShowDelay){
		int mxd=1;
		if(OB->MaxDelay)mxd=OB->MaxDelay;
		else mxd=1;
		if(mxd<OB->delay)mxd=OB->delay;
		hs=(mhs*(mxd-OB->delay))/mxd;
		Hline(-2+x+c2+dc+1,y+ddy+2,x+c2+hs+dc+1,0xD0);
		Hline(-2+x+c2+dc+1,y+ddy+3,x+c2+hs+dc+1,0xD0);
#ifndef EW
		Vline(-2+x+c2+dc,y+ddy,y+ddy+3,255);
		Vline(-2+x+Lx-1-c2,y+ddy,y+ddy+3,255);
	}else{
		Vline(-2+x+c2+dc,y+ddy,y+ddy+1,255);
		Vline(-2+x+Lx-1-c2,y+ddy,y+ddy+1,255);
	};
#else
        if (OB->MaxMorale > 0) {
            Vline(-2 + x + c2 + dc, y + ddy, y + ddy + 5, 255);
            Vline(-2 + x + Lx - 1 - c2, y + ddy, y + ddy + 5, 255);
        }
        else {
            Vline(-2 + x + c2 + dc, y + ddy, y + ddy + 3, 255);
            Vline(-2 + x + Lx - 1 - c2, y + ddy, y + ddy + 3, 255);
        }
}
    else {
        if (OB->MaxMorale > 0) {
            Vline(-2 + x + c2 + dc, y + ddy, y + ddy + 3, 255);
            Vline(-2 + x + Lx - 1 - c2, y + ddy, y + ddy + 3, 255);
        }
        else {
            Vline(-2 + x + c2 + dc, y + ddy, y + ddy + 1, 255);
            Vline(-2 + x + Lx - 1 - c2, y + ddy, y + ddy + 1, 255);
        }
    };
#endif
	if(OB->NewBuilding){
		int x0=-2+x+c2+dc+1;
		int y0=y+ddy+2;
		CBar(x0,y0-12,8,8,0xD0+OB->NNUM*4);
		char cc[128];
		if(OB->NMask&NATIONS[NatRefTBL[MyNation]].NMask||OB->WatchNInside){
			sprintf_s( cc,sizeof(cc),"%d/%d",OB->NInside,OB->Ref.General->MoreCharacter->MaxInside+OB->AddInside);
		}else{
			sprintf_s( cc,sizeof(cc),"?/%d",OB->Ref.General->MoreCharacter->MaxInside+OB->AddInside);
		};
		int H=GetRLCHeight(SmallWhiteFont.RLC,'C');
		ShowString(x0+12,y0-12+8-H,cc,&SmallWhiteFont);
	};
};
void DrawColorMarker(OneObject* OB,byte cl){
	if(OB){
		int x,y,Lx,Ly;
		GetRect(OB,&x,&y,&Lx,&Ly);
		x+=smapx-(int(mapx)<<5);
		y+=smapy-(mul3(int(mapy))<<3);
		if(OB->ForceX||OB->ForceY)cl=clrRed;
		byte c1=0;
		byte c2=5;
		byte dc=4;
		int ddy=-12;
		Hline(x+c1,y,x+c2,cl);
		Hline(x+Lx-1-c2,y,x+Lx-1-c1,cl);
		Hline(x+c1,y+Ly-1,x+c2,cl);
		Hline(x+Lx-1-c2,y+Ly-1,x+Lx-1-c1,cl);
		Vline(x,y+c1,y+c2,cl);
		Vline(x+Lx-1,y+c1,y+c2,cl);
		Vline(x,y+Ly-1-c2,y+Ly-1-c1,cl);
		Vline(x+Lx-1,y+Ly-1-c2,y+Ly-1-c1,cl);
		/*
		Hline(x+1,y+1,x+1,cl);
		Hline(x+1,y+Lx-2,x+1,cl);
		Hline(x+Lx-2,y+1,x+Lx-2,cl);
		Hline(x+Lx-2,y+Lx-2,x+Lx-2,cl);*/
	};
};
void DrawMiniMarker(OneObject* OB){
	if(OB){
		int x,y,Lx,Ly;
		GetRect(OB,&x,&y,&Lx,&Ly);
		x=x>>1;
		y=y>>1;
		Lx=Lx>>1;
		Ly=Ly>>1;
		x+=smapx-(int(mapx)<<4);
		y+=smapy-(mul3(int(mapy))<<2);
		byte cl=clrYello;
		byte c1=0;
		byte c2=2;
		byte dc=4;
		int ddy=-12;
		Hline(x+c1,y,x+c2,cl);
		Hline(x+Lx-1-c2,y,x+Lx-1-c1,cl);
		Hline(x+c1,y+Ly-1,x+c2,cl);
		Hline(x+Lx-1-c2,y+Ly-1,x+Lx-1-c1,cl);
		Vline(x,y+c1,y+c2,cl);
		Vline(x+Lx-1,y+c1,y+c2,cl);
		Vline(x,y+Lx-1-c2,y+Ly-1-c1,cl);
		Vline(x+Lx-1,y+Ly-1-c2,y+Ly-1-c1,cl);
		if(HealthMode){
			int mhs=Lx-c2-c2-dc-dc;
			int hs=mhs;
			if(OB->MaxLife>0){
				hs=div(mhs*OB->Life,OB->MaxLife).quot;
			};
			Vline(-2+x+c2+dc,y+ddy,y+ddy+2,255);
			Vline(-2+x+Lx-1-c2,y+ddy,y+ddy+2,255);
			Vline(-2+x+c2+dc+1,y+ddy,y+ddy,255);
			Vline(-2+x+c2+dc+1,y+ddy+2,y+ddy+2,255);
			Vline(-2+x+Lx-1-c2-1,y+ddy,y+ddy,255);
			Vline(-2+x+Lx-1-c2-1,y+ddy+2,y+ddy+2,255);
			Hline(-2+x+c2+dc+1,y+ddy+1,x+c2+hs+dc+1,clrGreen);
		};
		/*
		Hline(x+1,y+1,x+1,cl);
		Hline(x+1,y+Lx-2,x+1,cl);
		Hline(x+Lx-2,y+1,x+Lx-2,cl);
		Hline(x+Lx-2,y+Lx-2,x+Lx-2,cl);*/
	};
};
void ShowHealth(OneObject* ZZ){
	if(!(ZZ->Selected&GM(MyNation)))return;
	DrawMarker(ZZ);
	return; 
};
void ShowMiniHealth(OneObject* ZZ){
	if(!(ZZ->Selected&GM(MyNation)))return;
	DrawMiniMarker(ZZ);
	return; 
};
extern void MakeFog(int x,int y);
void ShowFog(OneObject* ZZ){
	
};
void ShowProperty(OneObject* OB){

};
static int tmt=0;
char* GetTextByID(char* ID);
void CBar(int x,int y,int Lx,int Ly,byte c);

void GetDiscreta(int* x,int* y,int val);
char* WINNER=NULL;
char* LOOSER=NULL;
int PrevMissTime=0;
int FirstMissTime=0;
extern bool GameExit;
void GetSquare();
char* GetPName(int i);
int NatSquare[8]={0,0,0,0,0,0,0,0};
int SumSquare[8];
int SumAccount[8];
extern City CITY[8];
extern word NPlayers;
void CreateSumm(){
	memset(SumSquare,0,7*4);
	memset(SumAccount,0,7*4);
	for(int i=0;i<7;i++){
		byte ms0=1<<i;
		byte ms=NATIONS[i].NMask;
		if(ms==ms0){
			SumSquare[i]=NatSquare[i];
			SumAccount[i]=CITY[i].Account;
		}else{
			int ss=0;
			int sa=0;
			int na=0;
			for(int j=0;j<7;j++){
				if(NATIONS[j].NMask&ms){
					ss+=NatSquare[j];
					sa+=CITY[j].Account;
					na++;
				};
			};
			SumSquare[i]=ss/na;
			SumAccount[i]=sa/na;

		};
	};
};

int GetBestPlayer(){
	CreateSumm();
	int maxn=-1;
	int maxs=0;
	int maxidv=0;
	for(int i=0;i<7;i++){
		int v=SumAccount[i];//*10000+CITY[i].Account;
		if(v>maxs){
			maxs=v;
			maxn=i;
			maxidv=CITY[i].Account;
		};
		if(v==maxs&&CITY[i].Account>maxidv){
			maxidv=CITY[i].Account;
			maxn=i;
		};
	};
	return maxn;
};
int SortPlayers(byte* Res,int* par){
	CreateSumm();
	int con=PINFO[0].VictCond;
	int q;
	SortClass SC;
	SC.CheckSize(8);
	int np=0;
	byte usemask=0;
	for(int q=0;q<7;q++){
		int xx=NatRefTBL[q];
		if(NATIONS[xx].ThereWasUnit/*NtNUnits[xx]*/){
			byte ms=NATIONS[xx].NMask;
			if(!(usemask&ms)){
				usemask|=ms;
				SC.Parms[np]=-SumAccount[xx];
				SC.Uids[np]=ms;
				np++;
			};
		};
	};
	SC.NUids=np;
	SC.Sort();
	word msks[8];
	SC.Copy(msks);
	int ps=0;
	for(q=0;q<np;q++){
		byte msk=byte(msks[q]);
		int mp=0;
		byte m0=1;
		for(int p=0;p<7;p++){
			int xx=NatRefTBL[p];
			m0=1<<xx;
			if(msk&m0&&NATIONS[xx].ThereWasUnit/*NtNUnits[xx]*/){
				SC.Parms[mp]=-CITY[xx].Account;
				SC.Uids[mp]=p;
				mp++;
			};
			m0<<=1;
		};
		if(mp){
			SC.NUids=mp;
			SC.Sort();
			for(int p=0;p<mp;p++){
				Res[ps]=SC.Uids[p];
				par[ps]=SC.Parms[p];
				ps++;
			};
		};
	};
	return ps;
};
void ShowVictInFormat(char* Res,int idx,byte Type){
	sprintf(Res,"%.2f",float(CITY[idx].Account)/100);
};
bool NoWinner=0;
bool ShowStat=0;
extern bool NOPAUSE;
void CmdEndGame(byte NI,byte state,byte cause);
#ifdef SPEEDFIX
unsigned long GetRealTime();
#else
int GetRealTime();
#endif
extern int WaitState;
extern bool EnterChatMode;
extern int ShowGameScreen;
extern byte PlayGameMode;
void ShowCentralText0(char* sss){
	char ssp[400];
	int ny=0;
	int L=strlen(sss);
	for(int i=0;i<L;i++)if(sss[i]=='\\')ny++;
	int MaxLx=0;
	byte c;
	int MaxCur=0;
	int p=0;
	do{
		c=sss[p];
		p++;
		if(c!='\\'&&c){
			int L=1;
			MaxCur+=GetRLCWidthUNICODE(BigYellowFont.RLC,(byte*)(sss+p),&L);
			p+=L-1;
		}else{
			if(MaxCur>MaxLx)MaxLx=MaxCur;
			MaxCur=0;
		};
	}while(c);
	int y0=smapy+(smaply<<(Shifter-2));
	int x0=smapx+(smaplx<<(Shifter-1));
	int Dy=GetRLCHeight(BigYellowFont.RLC,'y')+5;
	y0-=((Dy*ny)>>1)+32;
	MaxLx>>=1;
	DrawStdBar1(x0-MaxLx-64,y0-8,x0+MaxLx+64,y0+20+Dy*ny+7);
	p=0;
	int p0=0;
	do{
		c=sss[p];
		p++;
		if(c!='\\'&&c){
			ssp[p0]=c;
			ssp[p0+1]=0;
			p0++;
		}else{
			int L=GetRLCStrWidth(ssp,&BigYellowFont)>>1;
			ShowString(x0-L,y0-9,ssp,&BigWhiteFont);
			y0+=Dy;
			p0=0;
		};
	}while(c);
};
void ShowWinner(){
	if(NoWinner||PlayGameMode==1)return;
	GetSquare();
	if(!WINNER){
		WINNER=GetTextByID("WINNER");
	};
	if(!LOOSER){
		LOOSER=GetTextByID("LOOSER");
	};
	char sss[512]="";
	int MyVirtNation=NatRefTBL[MyNation];
	if((!PlayGameMode)&&PINFO[0].VictCond&&!PINFO[0].GameTime){
		if(PINFO[0].VictCond){
			//Land scope
			int n=GetBestPlayer();
			if(((1<<n)&NATIONS[MyVirtNation].NMask)&&NMyUnits){
				strcpy(sss,WINNER);
				CmdEndGame(MyNation,2,111);
				LockPause=1;
				if(GetRealTime()-PrevMissTime>7000){
					PrevMissTime=GetRealTime();
					FirstMissTime=GetRealTime();
				}else{
					if(WaitState==2){
						GameExit=true;
						ShowStat=1;
						PrevMissTime=0;
						SCENINF.Victory=false;
					};
					PrevMissTime=GetRealTime();
				};
			}else{
				strcpy(sss,LOOSER);
				LockPause=1;
				CmdEndGame(MyNation,1,112);
				if(GetRealTime()-PrevMissTime>7000){
					PrevMissTime=GetRealTime();
					FirstMissTime=GetRealTime();
				}else{
					PrevMissTime=GetRealTime();
					if(WaitState==2){
						GameExit=true;
						ShowStat=1;
						SCENINF.LooseGame=false;
						PrevMissTime=0;
					};
				};
			};
		};
	};
	if(SCENINF.hLib&&!SCENINF.StandartVictory){
		if(SCENINF.LooseGame){
			if(!WaitState)WaitState=1;
			if(SCENINF.LooseText)
				strcpy(sss,SCENINF.LooseText);
			else strcpy(sss,LOOSER);
			if(GetRealTime()-PrevMissTime>7000){
				PrevMissTime=GetRealTime();
				FirstMissTime=GetRealTime();
			}else{
				PrevMissTime=GetRealTime();
				if(GetRealTime()-FirstMissTime>7000||WaitState==2){
					GameExit=true;
					ShowStat=0;
					SCENINF.LooseGame=false;
					PrevMissTime=0;
				};
			};
		}else
		if(SCENINF.Victory){
			if(!WaitState)WaitState=1;
			if(SCENINF.VictoryText)
				strcpy(sss,SCENINF.VictoryText);
			else strcpy(sss,WINNER);
			if(GetRealTime()-PrevMissTime>7000){
				PrevMissTime=GetRealTime();
				FirstMissTime=GetRealTime();
			}else{
				if(GetRealTime()-FirstMissTime>7000||WaitState==2){
					GameExit=true;
					ShowStat=0;
					PrevMissTime=0;
					SCENINF.Victory=false;
				};
				PrevMissTime=GetRealTime();
				if(CurrentCampagin!=-1&&CurrentMission!=-1){
					int idx=CAMPAGINS.SCamp[CurrentCampagin].OpenIndex[CurrentMission];
					int id=(idx>>8)&255;
					if(id<CAMPAGINS.SCamp[CurrentCampagin].NMiss)CAMPAGINS.SCamp[CurrentCampagin].OpenIndex[id]|=1;
					id=(idx>>16)&255;
					if(id<CAMPAGINS.SCamp[CurrentCampagin].NMiss)CAMPAGINS.SCamp[CurrentCampagin].OpenIndex[id]|=1;
					id=(idx>>24)&255;
					if(id<CAMPAGINS.SCamp[CurrentCampagin].NMiss)CAMPAGINS.SCamp[CurrentCampagin].OpenIndex[id]|=1;
					SavePlayerData();
				};
			};
		}else
		if(SCENINF.CTextTime>0&&SCENINF.CentralText){
			strcpy(sss,SCENINF.CentralText);
		};
	}else{
		if(NATIONS[MyNation].VictState&&!sss[0]){
			if(NATIONS[MyNation].VictState==1){
				strcpy(sss,LOOSER);
				LockPause=1;
			}else if(NATIONS[MyNation].VictState==3){
				strcpy(sss,DISCONNECTED);
				LockPause=1;
			}else{
				strcpy(sss,WINNER);
				LockPause=1;
				if(SCENINF.hLib&&CurrentCampagin!=-1&&CurrentMission!=-1){
					int idx=CAMPAGINS.SCamp[CurrentCampagin].OpenIndex[CurrentMission];
					int id=(idx>>8)&255;
					if(id<CAMPAGINS.SCamp[CurrentCampagin].NMiss)CAMPAGINS.SCamp[CurrentCampagin].OpenIndex[id]|=1;
					id=(idx>>16)&255;
					if(id<CAMPAGINS.SCamp[CurrentCampagin].NMiss)CAMPAGINS.SCamp[CurrentCampagin].OpenIndex[id]|=1;
					id=(idx>>24)&255;
					if(id<CAMPAGINS.SCamp[CurrentCampagin].NMiss)CAMPAGINS.SCamp[CurrentCampagin].OpenIndex[id]|=1;
					SavePlayerData();
				};
			};
			if(WaitState==2){
				GameExit=true;
				ShowStat=SCENINF.hLib==NULL;
				PrevMissTime=0;
				SCENINF.Victory=false;
				SCENINF.LooseGame=false;
			};
		}else{
			if(!NMyUnits){
				LockPause=1;
				CmdEndGame(MyNation,1,113);
				strcpy(sss,LOOSER);
				if(GetRealTime()-PrevMissTime>7000){
					PrevMissTime=GetRealTime();
					FirstMissTime=GetRealTime();
				}else{
					PrevMissTime=GetRealTime();
					if(WaitState==2){
						GameExit=true;
						ShowStat=1;
						SCENINF.LooseGame=false;
						PrevMissTime=0;
					};
				};
			}else{
				if(!NThemUnits){
					LockPause=1;
					CmdEndGame(MyNation,2,114);
					strcpy(sss,WINNER);	
					LockPause=1;
					if(GetRealTime()-PrevMissTime>7000){
						PrevMissTime=GetRealTime();
						FirstMissTime=GetRealTime();
					}else{
						if(WaitState==2){
							GameExit=true;
							ShowStat=1;
							PrevMissTime=0;
							SCENINF.Victory=false;
						};
						PrevMissTime=GetRealTime();
					};
				};
			};
		};
	};
	if(SCENINF.CTextTime&&SCENINF.CentralText&&SCENINF.CentralText[0]=='#'){
		strcpy(sss,SCENINF.CentralText+1);
		if(SCENINF.CTextTime)SCENINF.CTextTime--;
	};
	if((!sss[0])&&SCENINF.CTextTime&&SCENINF.CentralText){
		strcpy(sss,SCENINF.CentralText);
	};
	if(ShowGameScreen==2)strcpy(sss,SER_SAVE);
	if(sss[0]&&!EnterChatMode){
		ShowCentralText0(sss);
		
	};
};
//final procedures for graphics output
void ProcessWaveFrames();
void ShowVisualLess(int yend);
extern word rpos;
int time0;
word PreRpos;
void ShowPulse();
void AddPulse(word ObjID,byte c);
void ProcessFog1();
void CreateFogImage();
extern int DoLink_Time,SearchVictim_Time,CheckCapture_Time;
void SyncroDoctor();
void PrepareSound();
void ProcessSelectedTower();
extern City CITY[8];
void DrawBorder(Brigade* BR);
int PreviewBrig=-1;;
void GenShow();
void DrawAZones();
void ScenaryLights();
extern int NInGold[8];
extern int NInIron[8];
extern int NInCoal[8];
extern int NInStone[8];
void ShowCostPlaces();
int CheckSmartCreationAbility(byte NI,NewMonster* NM,int* x2i,int* y2i);
bool Tutorial=0;
bool NoPress=0;
bool TutOver=0;
DWORD SStartTime=0;
int SStartX=0;
int SStartY=0;
int SCntrX=0;
int SCntrY=0;
extern int LastTimeStage;
extern int RealPause;
extern int CurrentStartTime[8];
extern int StartTime[8];
extern int LastStartTime[8];
extern int LastGameLength[8];
extern int CurrentStepTime;
void ShowChat();
void xLine(int x,int y,int x1,int y1,byte c);
void ShowExpl();
extern int BrigPnX;
extern int BrigPnY;
void ShowAbout();
void DrawShar(Nation* NT);
extern PingSumm PSUMM;
int GetMaxRealPing();
int GetAveragePing();
void CreateDiffStr(char* str);
extern int AddTime;
extern int NeedAddTime;
extern int NSTT;
int PRVGT=0;
int GetPing(DWORD pid);
extern DWORD RealTime;
extern bool LockFog;
extern byte SYNBAD[8];
void ProcessRivEditor();
extern int NPROCM;
extern int TPROCM;
void DrawSMS();
void DRAW_MAP_TMP();
void ShowLockBars();
void ProcessVotingView();
void ProcessVotingKeys();
extern int PeaceTimeLeft;
void CmdEndPT();
int GameTime=0;
char* TMMS_NOPT=NULL;
char* TMMS_PT=NULL;
extern int CURTMTMT;
extern byte BaloonState;
extern byte CannonState;
extern byte XVIIIState;
extern int COUNTER1;
extern int COUNTER2;
void DottedLine(int x1,int y1,int x2,int y2,byte c){
	if(y1==y2){
		int n=(x2-x1)>>1;
		int xx=x1;
		for(int i=0;xx<x2;i++){
			if(i&1);
			int xx2=xx+4;
			if(xx2>x2)xx2=x2;
			if(!(i&1))xLine(xx,y1,xx2,y2,c);
			xx+=4;
		};
	}else xLine(x1,y1,x2,y2,c);
};
extern bool RecordMode;
bool IsGameActive();
bool CheckGameTime();
void AddRecordAndRatingSymbols(char* s){
	char ccc[256];
	ccc[0]=0;
	if(IsGameActive()){
		if(CheckGameTime()){
			strcpy(ccc,"0019");
		}else{
			strcpy(ccc,"[ 0019-?]");
		};
	};
	if(RecordMode)strcat(ccc,"001A");
	strcat(ccc,s);
	strcpy(s,ccc);
};
extern int CurPressure;
int GetTZValue(int ix,int iy);
void ShowX1(){
	int dy=smaply>>1;
	for(int ix=0;ix<smaplx;ix++){
		for(int iy=0;iy<dy;iy++){
			char cc[16];
			int ddy=(mapy&1)*16;
			sprintf_s( cc,sizeof(cc),"%d",GetTZValue(mapx+ix,(mapy>>1)+iy));
			ShowString(ix*32,(iy*32)-ddy,cc,&fn8);
			Hline(ix*32,(iy*32)-ddy,ix*32+1,255);
		};
	};
};
void ShowTexPreview(int xc,int yc);
extern int DrawPixMode;
extern int TIME1;
void ShowIllustrations();
void DrawLGround();
void MiniShowNewMonsters();
#define LGP(x) (LMode?GP_L_IDXS[x]:x)
#define LZM(x) (LMode?(x>>2):x)
void DrawLModeFog();
int CurAINF_Show=-1;
void DetermineWhereUnitIs(word Index,char* dest);
void DrawWay();
void AddWPoint(int x,int y);
int GetApproxDeep(int x,int y);
extern int MINESPRITEID;
extern int WayID;
extern int GT0,GT1,GT2,GT3,GT4,GT5;
extern bool CanProduce;
bool SHOWZONES=0;
#ifdef _USE3D
void ShowAllMarkers(){
	GPS.FlushBatches();
	int N=ImNSL[MyNation];
	word* IDS=ImSelm[MyNation];
	word* SNS=ImSerN[MyNation];
	for(int i=0;i<N;i++){
		word MID=IDS[i];
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&OB->Serial==SNS[i]&&!(OB->Sdoxlo||OB->BrigadeID!=0xFFFF)){
				DrawMarker(OB);
			};
		};
	};
	GPS.FlushBatches();
};
#endif
void GFieldShow(){
	PrepareSound();
	int i;
	int uuu=(tmt&31);
	char ggg[600];
	tmt++;
	time1=GetRealTime();
	time0=GetRealTime();

#ifdef _USE3D
	IRS->SetCurrentShader(0);
	float scale		= float(1<<(5-Shifter));
	float ScaledLy	= RealLy * scale;

	Vector3D pos(	mapx * c_WorldGridStepX + RealLx*scale / 2,  
					mapy * c_WorldGridStepY + ScaledLy,     
					0.0f );
	Vector3D dir( CAM.getDir() ); 
	CAM.setLookAt( pos );
	dir *= -c_CameraDistance; 
	pos += dir;
	CAM.setPos( pos );

	float viewVolumeWidth = RealLx * scale;
	
	extern float CamZn, CamZf;
	CamZn =	c_CameraDistance - ScaledLy * c_Cos30 - c_MaxMapHeight / c_Cos30;
	CamZf = c_CameraDistance + ScaledLy * c_Cos30; 
	
	CAM.setViewVolumeWidth	( viewVolumeWidth			);
	CAM.setViewPort			( 0, 0, viewVolumeWidth, ScaledLy	);
	CAM.setZn				( CamZn						);
	CAM.setZf				( CamZf						);

	if (GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CAPITAL) < 0)
	{
		void SetPerspCamera();
		SetPerspCamera();
	}
	else
	{
		CAM.setOrtho();
	}

	IRS->SetViewMatrix		( CAM.getViewM()		); 
	IRS->SetProjectionMatrix( CAM.getProjM()		); 
	IRS->SetWorldMatrix		( Matrix4D::identity	); 

	//HandleWater();
	void FillGroundZBuffer();
	//FillGroundZBuffer();
	
#endif // _USE3D

	if(SHOWSLIDE){
		SetRLCWindow(0,0,RealLx,RealLy,SCRSizeX);
		if(LMode){
#ifdef _USE3D
			TestTriangle();	
			FillGroundZBuffer();
			//ShowAllMarkers();
			MiniShowNewMonsters();
#else
			DrawLGround();
			MiniShowNewMonsters();
            WallHandleDraw();
#endif
		}else{ 
			TestTriangle();	
			TestBlob();
			time6=GetRealTime()-time0;
			time0=GetRealTime();
#ifdef _USE3D
			FillGroundZBuffer();
			ShowAllMarkers();
			MiniShowNewMonsters();
#else
			ShowNewMonsters();
#endif
			time0=GetRealTime();
			WallHandleDraw();
		};
	};

	ShowExpl();
	for(int v=0;v<8;v++)DrawShar(NATIONS+v);
    //ShowVisualLess(100024);
	if(SHOWSLIDE){
		for(int i=0;i<MBR4;i++){
			DWORD p=BMASK[i];
			if(p){
				for(int j=0;j<32;j++){
					if(p&1){
						Brigade* BR=CITY[NatRefTBL[MyNation]].Brigs+(i<<5)+j;
						if(BR->Enabled)DrawBorder(BR);
					};
					p>>=1;
				};
			};
		};

#ifdef _USE3D
		GPS.FlushBatches();
		GPS.EnableZBuffer( true );

		static int shNoShadow = RC::iRS->GetShaderID( "spriteNoShadow" );
		GPS.SetCurShader( shNoShadow );
		
#endif // _USE3D

		ShowZBuffer();

#ifdef _USE3D
		static int shShadow = IRS->GetShaderID( "spriteShadow" );
		GPS.SetCurShader( shShadow );
		void ShowZInternal( bool filterTrees );
		ShowZInternal( true );

		GPS.EnableZBuffer( false );
		
		HandleWater();

		void ShowMCam();
		ShowMCam();

#endif // _USE3D

		ShowPulse();
		if(CINFMOD)ShowAllBars();
		//ShowNewMonsters();
		if(!NoText)LShow();
		extern int DrawPathID;
		extern int DrawDestID;
		if(DrawDestID!=0xFFFF){
			OneObject* OB=Group[DrawDestID];
			if(OB){
				int DX=OB->DestX>>4;
				int DY=OB->DestY>>4;
				int Z=GetHeight(DX,DY);
				DX=DX-(mapx<<5);
				DY=(DY>>1)-Z-(mapy<<4);
				xLine(DX-5,DY-5,DX+5,DY+5,0xFF);
				xLine(DX-5,DY+5,DX+5,DY-5,0xFF);
			};
		};
		if(DrawPathID!=0xFFFF){
			OneObject* OB=Group[DrawPathID];
			if(OB){
				if(OB->PathX&&OB->NIPoints){
					int N=OB->NIPoints;
					for(int i=0;i<N;i++){
						int DX=(int(OB->PathX[i])<<4)+8;
						int DY=(int(OB->PathY[i])<<4)+8;
						int Z=GetHeight(DX,DY);
						DX=DX-(mapx<<5);
						DY=(DY>>1)-Z-(mapy<<4);
						xLine(DX-5,DY,DX+5,DY,0xFF);
						xLine(DX,DY+5,DX,DY-5,0xFF);
					};
				};
			};
		};
	};
	if(!SHOWSLIDE)return;
	if(EditMapMode||SHOWZONES){
		DrawAZones();
		if(!NoText){
			for(int i=0;i<8;i++){
				int x=mapPos[i<<1];
				int y=mapPos[(i<<1)+1];
				if(x>=mapx&&y>=mapy&&x<mapx+smaplx&&y<mapy+smaply){
					sprintf(ggg,"%d",i+1);
					ShowString(smapx+((x-mapx)<<5)+8,smapy+((y-mapy)<<5)+2,ggg,&BigWhiteFont);
				};
			};
		};
	};
	int v1=3;
	int v2=158;
	if(EditMapMode&&!NoText){
		if(ChoosePosition)
			sprintf(ggg,GETSPOS,MyNation+1);//"Игрок %d : Выбор стартовой позиции."
		else sprintf(ggg,PLAYMES,MyNation+1);//"Игрок %d"
		ShowString(smapx+256,smapy+64,ggg,&WhiteFont);
	};
	if(BuildMode&&!NoText){
		GeneralObject* GO=BNat->Mon[BuildingID];
		NewMonster* NM=GO->newMons;
        int xxx,yyy;
        if(GetRealCoords(mouseX,mouseY,&xxx,&yyy)){
            if(NM->SpriteObject&&NM->SpriteVisual!=0xFFFF){
				if(!LMode){
	                SprGroup* SG=&COMPLEX;
					int id=NM->SpriteVisual;
					ObjCharacter* OC=&SG->ObjChar[id];
					if(OC->Stand){
						int tm1=div(tmtmt,OC->Delay).quot;
						int fr=div(tm1,OC->Frames).rem;
						int spr=fr*OC->Parts;
						int XX;//=mouseX-SG->Dx[id];
						int YY;//=mouseY-SG->Dy[id];
						NewAnimation* NA=OC->Stand;
						GetDiscreta(&xxx,&yyy,OC->Discret);
						XX=xxx-(mapx<<5)-SG->Dx[id];
						YY=(yyy>>1)-(mapy<<4)-GetHeight(xxx,yyy)-SG->Dy[id];
				
						if(CheckDist(xxx,yyy,SG->Radius[id])){
							if(NA->LineInfo){
								for(int p=0;p<NA->NFrames;p++){
									NewFrame* OF=&NA->Frames[p];
									GPS.ShowGP(XX,YY,OF->FileID,OF->SpriteID,0);
								};
							}else{
								for(int p=0;p<OC->Parts;p++){
									NewFrame* OF=&NA->Frames[spr+p];
									GPS.ShowGP(XX,YY,OF->FileID,OF->SpriteID,0);
								};
							};
						}else{
							if(NA->LineInfo){
								for(int p=0;p<NA->NFrames;p++){
									NewFrame* OF=&NA->Frames[p];
									GPS.ShowGPRedN(XX,YY,OF->FileID,OF->SpriteID,0,16);
								};
							}else{
								for(int p=0;p<OC->Parts;p++){
									NewFrame* OF=&NA->Frames[spr+p];
									GPS.ShowGPRedN(XX,YY,OF->FileID,OF->SpriteID,0,16);
								};
							};
						};
					};
				};
            }else{
                if(NM->Wall){
                    if(!BuildWall)SetWallBuildMode(NatRefTBL[MyNation],BuildingID);
                    BuildMode=false;
                }else{;
       		        ShowMines(NM);
					BuildWall=0;
					if(!GO->Enabled)BuildMode=false;
      		        int xxx,yyy;
    		        if(GetRealCoords(mouseX,mouseY,&xxx,&yyy)){
	    		        int x0=mouseX+LZM(NM->PicDx);
	    		        int y0=mouseY+LZM(NM->PicDy);
	    		        NewAnimation* UP=&NM->StandHi;
	    		        NewAnimation* DN=&NM->StandLo;
						NewAnimation* WR=&NM->Work;
	    		        int NNN=5+int(8*(sin(double(GetRealTime())/100)+2));
						int xx3=xxx<<4;
						int yy3=yyy<<4;
						int AddGP=-1;
						int sprs[MaxAStages];
						if(NM->CompxCraft){
							AddGP=NM->CompxCraft->Stages[0].GPID;
							for(int i=0;i<MaxAStages;i++){
								if(NM->CompxCraft->Stages[i].GPID!=0){
									sprs[i]=NM->CompxCraft->Stages[i].Empty.SpriteID;
								}else{
									sprs[i]=-1;
								};
							};
						};
						MINESPRITEID=-1;
	    		        if(CheckSmartCreationAbility(NatRefTBL[MyNation],NM,&xx3,&yy3)!=-1){
							if(MINESPRITEID!=-1){
								OneSprite* OS=Sprites+MINESPRITEID;
								if(OS->Enabled){
									ObjCharacter* OC=OS->OC;
									char* S=NULL;
									switch(OC->IntResType){
									case 0:
										S=GetTextByID("MINEID0");
										break;
									case 1:
										S=GetTextByID("MINEID1");
										break;
									case 2:
										S=GetTextByID("MINEID2");
										break;
									case 3:
										S=GetTextByID("MINEID3");
										break;
									case 4:
										S=GetTextByID("MINEID4");
										break;
									case 5:
										S=GetTextByID("MINEID5");
										break;
									};
									if(S){
										AssignHint(S,8);
									};
								};
							};
							x0=LZM((xx3>>4)-(mapx<<5)+NM->PicDx);
							y0=LZM((yy3>>5)-(mapy<<4)+NM->PicDy-GetHeight(xx3>>4,yy3>>4));
							if(DN->LineInfo){
								for(int p=0;p<DN->NFrames;p++){
									NewFrame* OF=&DN->Frames[p];
									GPS.ShowGPDarkN(x0,y0,LGP(OF->FileID),OF->SpriteID,0,NNN);
								};
							}else{
	    						if(UP->Enabled){
	    							for(int i=0;i<UP->Parts;i++){
	    								NewFrame* NF=&UP->Frames[i];
										GPS.ShowGPDarkN(x0+i*UP->PartSize,y0,LGP(NF->FileID),NF->SpriteID,0,NNN);
	    							};
	    						};
	    						if(DN->Enabled){
	    							for(int i=0;i<DN->Parts;i++){
	    								NewFrame* NF=&DN->Frames[i];
										GPS.ShowGPDarkN(x0+i*DN->PartSize,y0,LGP(NF->FileID),NF->SpriteID,0,NNN);
	    							};
	    						};
							};
							if(WR->Enabled){
	    						NewFrame* NF=WR->Frames;
								GPS.ShowGPDarkN(x0+LZM(NF->dx),y0+LZM(NF->dy),LGP(NF->FileID),NF->SpriteID,0,NNN);
							};

							if(NM->Port){
								int px=(PortBuiX<<4)-(mapx<<5);
								int py=(PortBuiY<<3)-(mapy<<4)-GetHeight(PortBuiX<<4,PortBuiY<<4);
								int LL=LZM(NM->BuiDist<<3);
								NewAnimation* BUANM=NM->BuiAnm;
								if(BUANM){
									NewFrame* NF=&BUANM->Frames[div(tmtmt,BUANM->NFrames).rem];
									px+=LZM(NF->dx);
									py+=LZM(NF->dy);
									GPS.ShowGPDarkN(px-LL-LL,py,LGP(NF->FileID),NF->SpriteID,0,NNN);
									GPS.ShowGPDarkN(px+LL+LL,py,LGP(NF->FileID),NF->SpriteID,0,NNN);
									GPS.ShowGPDarkN(px,py-LL,LGP(NF->FileID),NF->SpriteID,0,NNN);
									GPS.ShowGPDarkN(px,py+LL,LGP(NF->FileID),NF->SpriteID,0,NNN);
								};
							};
							if(AddGP!=-1){
								for(int i=0;i<MaxAStages;i++){
									if(sprs[i]!=-1){
										GPS.ShowGPDarkN(x0,y0,LGP(AddGP),sprs[i],0,NNN);
									};
								};
							};
	    		        }else{
							if(DN->LineInfo){
								for(int p=0;p<DN->NFrames;p++){
									NewFrame* OF=&DN->Frames[p];
									GPS.ShowGPRedN(x0,y0,LGP(OF->FileID),OF->SpriteID,0,NNN);
								};
							}else{
		    			        if(UP->Enabled){
		    				        for(int i=0;i<UP->Parts;i++){
		    					        NewFrame* NF=&UP->Frames[i];
										GPS.ShowGPRedN(x0+i*UP->PartSize,y0,NF->FileID,NF->SpriteID,0,NNN);
		    				        };
		    			        };
		    			        if(DN->Enabled){
		    				        for(int i=0;i<DN->Parts;i++){
		    					        NewFrame* NF=&DN->Frames[i];
										GPS.ShowGPRedN(x0+LZM(i*DN->PartSize),y0,LGP(NF->FileID),NF->SpriteID,0,NNN);
		    				        };
								};
	    			        };
							if(WR->Enabled){
	    						NewFrame* NF=WR->Frames;
								GPS.ShowGPRedN(x0+LZM(NF->dx),y0+LZM(NF->dy),LGP(NF->FileID),NF->SpriteID,0,NNN);
							};
							if(AddGP!=-1){
								for(int i=0;i<MaxAStages;i++){
									if(sprs[i]!=-1){
										GPS.ShowGPRedN(x0,y0,LGP(AddGP),sprs[i],0,NNN);
									};
								};
							};
	    		        };
                    };
                };
	    	};
	    };
    };
	/*
	if(GetKeyState(VK_CONTROL)&0x8000){
		sprintf(ggg,"NPROCM=%d",NPROCM);
		ShowString(10,200,ggg,&WhiteFont);
		sprintf(ggg,"TPROCM=%d",TPROCM);
		ShowString(10,220,ggg,&WhiteFont);
		sprintf(ggg,"NUNSENT=%d",PBACK.NBDATA);
		ShowString(10,240,ggg,&WhiteFont);
		sprintf(ggg,"NURETR=%d",RETSYS.NRET);
		ShowString(10,260,ggg,&WhiteFont);
	};
	*/
    SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	
	//if(BuildWall)TMPCluster.ShowCluster();
	//if(FogMode)ShowFoggedBattle();
	DrawSMS();
	ProcessSelectedTower();
	ScenaryLights();
	ShowIllustrations();
	if(SHOWSLIDE&&FogMode&&BaloonState!=1&&(!NATIONS[NatRefTBL[MyNation]].Vision)&&!NoText){
		if(LMode){
			if(NOPAUSE&&!(LockFog||NoPFOG))ProcessFog1();
#ifndef _USE3D
			DrawLModeFog();
#else
			DrawFog();
#endif
		}else{
			time0=GetRealTime();
			if(NOPAUSE&&!(LockFog||NoPFOG))ProcessFog1();
//#ifndef _USE3D
			CreateFogImage();
//#endif
			DrawFog();
			time4=GetRealTime()-time0;
		};
		/*
		if(Tutorial&&!MiniActive){
			int LX=GetRLCStrWidth(TUTORIAL,&RedFont);
			DrawStdHeader(RealLx>>1,37,LX+16);
			if(mouseY<22+23&&mouseY>23&&mouseX>(RealLx-LX)/2-8&&mouseX<(RealLx+LX)/2+8&&!NoPress){
				ShowString((RealLx-LX)>>1,26,TUTORIAL,&RedFont);
				TutOver=true;
			}else{
				ShowString((RealLx-LX)>>1,26,TUTORIAL,&BlackFont);
				TutOver=false;
			};
		}else TutOver=false;;
		*/
	};
	DrawCurves();
	//---------
	
	//---------

	//ShowX1();
	ShowCostPlaces();
	GenShow();////////////////////////////////////////
	if(GridMode)DrawGrids();
	ProcessVotingView();
	ShowChat();
	if(NoText)return;
	DrawDangerMap();
	ProcessHint();
	ShowDestn();
	time7=GetRealTime()-time0;
	if(SeqErrorsCount){
		//SYN.Save();
		int TTT=GetTickCount()/1500;
		if(TTT&1)ShowString(HintX,HintY-100,DELSYNC,&BigWhiteFont);
		else ShowString(HintX,HintY-100,DELSYNC,&BigRedFont);
		SeqErrorsCount=0;
		SyncroDoctor();
	};//else SYN.Clear();
	if(EditMapMode){
		ProcessRivEditor();
		if(LockMode)ShowLockBars();
		if(DrawPixMode&&(GetKeyState(VK_MENU)&0x8000))ShowTexPreview(mouseX,mouseY);
	}else ShowWinner();
	
	//DRAW_MAP_TMP();
	//information
	int xxx0=RealLx-400;
	int yyy0=smapy+RealLy-170;
	if(HeightEditMode==1){
		ShowString(xxx0,yyy0,HIEDMOD,&YellowFont);//"Редактирование высот."
		yyy0-=20;
	};
	if(HeightEditMode==2){
		ShowString(xxx0,yyy0,ADDSPOB,&YellowFont);//"Добавить спрайтовые объекты."
		yyy0-=20;
	};
	if(HeightEditMode==3){
		ShowString(xxx0,yyy0,DELSPOB,&YellowFont);//"Удалить объекты."
		yyy0-=20;
	};
	if(MEditMode){
		ShowString(xxx0,yyy0,TEXMODE,&YellowFont);//"Режим текстурирования."
		yyy0-=20;
	};
	if(LockMode==1){
		ShowString(xxx0,yyy0,UNBLOCK,&YellowFont);//"Режим принудительной разблокировки."
		yyy0-=20;
	};
	if(LockMode==2){
		ShowString(xxx0,yyy0,DOBLOCK,&YellowFont);//"Режим принудительной блокировки."
		yyy0-=20;
	};
	if(LockMode==3){
		ShowString(xxx0,yyy0,DFBLOCK,&YellowFont);//"Режим блокировки по умолчанию."
		yyy0-=20;
	};
	if(PeaceMode){
		ShowString(xxx0,yyy0,PEACMOD,&YellowFont);//"Режим мира."
		yyy0-=20;
	};
	if(WaterEditMode){
		switch(WaterEditMode){
		case 1:
			ShowString(xxx0,yyy0,SETWATT,&YellowFont);//"Режим редактирования воды: установка воды."
			break;
		case 2:
			ShowString(xxx0,yyy0,DELWATT,&YellowFont);//"Режим редактирования воды: стирание воды."
			break;
		case 3:
			ShowString(xxx0,yyy0,SETCOST,&YellowFont);//"Режим редактирования воды: установка отмели."
			break;
		case 4:
			ShowString(xxx0,yyy0,DELCOST,&YellowFont);//"Режим редактирования воды: стирание отмели."
			break;
		};
		yyy0-=20;
	};

	if(!(PlayGameMode==1||(SCENINF.hLib&&!CanProduce)||EditMapMode)){
		int NG=NATIONS[NatRefTBL[MyNation]].NGidot;
		int NF=NATIONS[NatRefTBL[MyNation]].NFarms;
		if(NF<=NG){
			GPS.ShowGPRedN(BrigPnX+42,BrigPnY,BordGP,92,MyNation,9+int(8*sin(float(GetTickCount())/100)));
		}else if(NF-NG<5){
			GPS.ShowGP(BrigPnX+42,BrigPnY,BordGP,92,MyNation);
		};
	};
	if(tmtmt>20/*&&PINFO[0].VictCond*/&&!Inform){
		if(LastTimeStage==-1){
			LastTimeStage=GetRealTime();
		}else{
			int dd=GetRealTime()-LastTimeStage;
			if(!NOPAUSE){
				dd=0;
				LastTimeStage=GetRealTime();
			};
			if(dd>1000){
				dd/=1000;
				if(PINFO[0].GameTime>dd)PINFO[0].GameTime-=dd;
				else PINFO[0].GameTime=0;
				GameTime+=dd;
				if(PeaceTimeLeft>1){
					PeaceTimeLeft-=dd;
					if(PeaceTimeLeft<1)PeaceTimeLeft=1;
				}else if(PeaceTimeLeft==1){
					CmdEndPT();
				};
				LastTimeStage+=dd*1000;
			};
		};
		char cc1[100];
		char ccr[24];
		int DY=0;
		if(PINFO[0].VictCond){
			int t=PINFO[0].GameTime;
			int s=t%60;
			int m=(t/60)%60;
			int h=t/3600;
			if(s<10&&m<10)sprintf(cc1,"%d:0%d:0%d",h,m,s);
			else if(s<10&&m>=10)sprintf(cc1,"%d:%d:0%d",h,m,s);
			else if(s>10&&m<10)sprintf(cc1,"%d:0%d:%d",h,m,s);
			else sprintf(cc1,"%d:%d:%d",h,m,s);	
			char cc4[100];
			//cc4[0]=0;
			//AddRecordAndRatingSymbols(cc4);
			ShowString(minix+2/*+GetRLCStrWidth(cc4,&SmallBlackFont)*/,miniy-22+2-4+DY,cc1,&SmallBlackFont);
			//AddRecordAndRatingSymbols(cc1);
			ShowString(minix,miniy-22-4+DY,cc1,&SmallWhiteFont);
			DY=-14;
		};
		byte ord[8];
		int  par[8];
		int no=SortPlayers(ord,par);
		byte prevms=0;
		if(!(SCENINF.hLib||PlayGameMode==1)){
			if(PeaceTimeLeft){
				if(!TMMS_PT)TMMS_PT=GetTextByID("TMMS_PT");
				int H1=GameTime/3600;
				int H2=PeaceTimeLeft/3600;
				int M1=(GameTime/60)%60;
				int M2=(PeaceTimeLeft/60)%60;
				int S1=GameTime%60;
				int S2=PeaceTimeLeft%60;
				sprintf(cc1,TMMS_PT,H1,M1/10,M1%10,S1/10,S1%10,(tmtmt+CURTMTMT)/25,H2,M2/10,M2%10,S2/10,S2%10);
				char cc4[100];
				cc4[0]=0;
				AddRecordAndRatingSymbols(cc4);
				ShowString(minix+2+GetRLCStrWidth(cc4,&SmallBlackFont),miniy-22+2-4+DY,cc1,&SmallBlackFont);
				AddRecordAndRatingSymbols(cc1);
				ShowString(minix,miniy-22-4+DY,cc1,&SmallWhiteFont);
			}else{
				if(!TMMS_NOPT)TMMS_NOPT=GetTextByID("TMMS_NOPT");
				int H1=GameTime/3600;
				int M1=(GameTime/60)%60;
				int S1=GameTime%60;
				char cc4[100];
				cc4[0]=0;
				sprintf(cc1,TMMS_NOPT,H1,M1/10,M1%10,S1/10,S1%10,(tmtmt+CURTMTMT)/25);
				AddRecordAndRatingSymbols(cc4);
				ShowString(minix+2+GetRLCStrWidth(cc4,&SmallBlackFont),miniy-22+2-4+DY,cc1,&SmallBlackFont);
				AddRecordAndRatingSymbols(cc1);
				ShowString(minix,miniy-22-4+DY,cc1,&SmallWhiteFont);
			};
			if(no/*&&NPlayers>1*/){
				int y=miniy-22-4-no*14+DY;
				
				for(int q=0;q<no;q++){
					byte ms=NATIONS[NatRefTBL[ord[q]]].NMask;
					byte c=0xD0+4*NatRefTBL[ord[q]];
					if(!(ms&prevms)){
                        int w;
						for(w=q;w<no&&(NATIONS[NatRefTBL[ord[w]]].NMask&ms);w++);
						w-=q;
						if(w>1){
							int y0=y+1+2;
							int y1=y+10+(w-1)*14;
							int yc=(y0+y1)>>1;
							int x=minix-2;
							xLine(x,y0,x-1,y0+1,c);
							xLine(x-2,y0+2,x-2,yc-2,c);
							xLine(x-3,yc-1,x-4,yc,c);
							xLine(x-3,yc+1,x-2,yc+2,c);
							xLine(x-2,yc+3,x-2,y1-2,c);
							xLine(x-1,y1-1,x,y1,c);
							char cc[32];
							cc[0]=0;

							switch(PINFO[0].VictCond){
							case 1:
								sprintf_s( cc,sizeof(cc),"%d",SumSquare[NatRefTBL[ord[q]]]);
								break;
							default:
								sprintf_s( cc,sizeof(cc),"%.2f",float(SumAccount[NatRefTBL[ord[q]]])/100);
							};
							int xt=x-7-GetRLCStrWidth(cc,&SmallWhiteFont);
							ShowString(xt,yc-5,cc,&SmallBlackFont);
							ShowString(xt-1,yc-6,cc,&SmallWhiteFont);
						};
					};
					prevms=ms;
					if((!SYNBAD[ord[q]])||(GetTickCount()>>9)&1){
						CBar(minix+3,y+5,4,4,c);
						Xbar(minix+2,y+4,6,6,c+1);
						Xbar(minix+1,y+3,8,8,c+3);
					};
					ShowVictInFormat(ccr,NatRefTBL[ord[q]],PINFO[0].VictCond);
					sprintf(cc1,"%s (%s)",GetPName(ord[q]),ccr);
					ShowString(minix+18+2,y+2,cc1,&SmallBlackFont);
					ShowString(minix+18,y,cc1,&SmallWhiteFont);
					int L=GetRLCStrWidth(cc1,&SmallWhiteFont);
					int vs=NATIONS[ord[q]].VictState;
					if(vs==1)xLine(minix-1,y+8,minix+20+L,y+8,c);
					if(vs==3)DottedLine(minix-1,y+8,minix+20+L,y+8,c);
					y+=14;
				};
			};
		};
	};
	if(Group[WayID])AddWPoint(Group[WayID]->RealX>>4,(Group[WayID]->RealY>>5)-Group[WayID]->RZ);
	DrawWay();
	if(PlayGameMode==1)ShowAbout();
	if(!(NOPAUSE||LockPause||EnterChatMode)){
		int L=GetRLCStrWidth(PAUSETEXT,&BigWhiteFont);
		int x=(RealLx-L)>>1;
		int y=RealLy>>1;
		DrawStdBar1(x-64,y-8,x+L+64,y+20+7);
		//ShowString(x,y-2,PAUSETEXT,&BigBlackFont);
		ShowString(x-2,y-4-4,PAUSETEXT,&BigWhiteFont);
	};

	int t0=GetRealTime();
	if(!PRVGT)PRVGT=t0;
	if(t0-PRVGT>2000){
		ADDGR(0,t0,0,0xFF);
		ADDGR(0,t0,(AddTime*100)>>7,0xD0);
		ADDGR(0,t0,(NeedAddTime*100)>>7,0xD4);
		ADDGR(1,t0,CurrentStepTime,0xD0);
		if(PSUMM.NPL&&PSUMM.PSET[0].NPings>5){
			for(int i=0;i<NPlayers;i++){
				int p=GetPing(PINFO[i].PlayerID);
				ADDGR(2,t0,p,0xD0+PINFO[i].ColorID*4);
			};
		};
		PRVGT=t0;
	};
	DrawAllGrp();
	if(!InfoMode)ShowPF();
	int uxxx=mapx+((mouseX-smapx)>>5);
	int uyyy=mapy+div24(mouseY-smapy);
	//OutCInf(uxxx,uyyy);
	uxxx=(mapx<<1)+((mouseX-smapx)>>4);
	uyyy=(mapy<<1)+((mouseY-smapy)>>3);
	//if(EditMapMode)return;
	if(InfoMode)return;
	int ssy=smapy+3;
	
	ssy+=100;
	sprintf(ggg,"Hi=%d" ,TerrHi);
	ShowString(smapx,ssy,ggg,&WhiteFont);
	ssy+=20;
	int cxx=MRealX>>7;
	int cyy=MRealY>>7;

	cxx=MRealX>>6;
	cyy=MRealY>>6;
	sprintf(ggg,"TopZone:%d",GetTopRef(cxx+cyy*TopLx));
	CurZone=GetTopRef(cxx+cyy*TopLx);
	ShowString(smapx,ssy,ggg,&WhiteFont);
	ssy+=20;
	sprintf(ggg,"Deep:%d",GetApproxDeep(MRealX,MRealY));
	ShowString(smapx,ssy,ggg,&WhiteFont);
	ssy+=20;
	/*
	sprintf(ggg,"LastKey=%d" ,LastKey);
	ShowString(smapx,ssy,ggg,&WhiteFont);
	ssy+=16;
	*/
	
	sprintf(ggg,"FPS:%d" ,Flips);
	counter=0;
	ShowString(smapx,ssy,ggg,&WhiteFont);
	ssy+=20;
	extern int LastMx;
	extern int LastMy;
	sprintf(ggg,"x=%d y=%d" ,LastMx,LastMy);
	counter=0;
	ShowString(smapx,ssy,ggg,&WhiteFont);
	ssy+=20;
	sprintf(ggg,"TEST:%d" ,TIME1);
	ShowString(smapx,ssy,ggg,&WhiteFont);
	ssy+=20;

	CEXPORT byte* GetBattleArmyValue();
	byte* AV=GetBattleArmyValue();
	if(AV){
		int val=0;
		for(int i=0;i<MAXOBJECT;i++){
			OneObject* OB=Group[i];
			if(OB&&(!OB->Sdoxlo||OB->Hidden)&&OB->NNUM==MyNation){
				val+=AV[OB->NIndex];
			}
		}
		char Buf[128];
		sprintf(Buf,"Units value: %d",val);
		ShowString(smapx,ssy,Buf,&WhiteFont);
		ssy+=20;
	}

	if(NSL[MyNation]){
		word MID=Selm[MyNation][0];
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB){
				sprintf(ggg,"%s : ID=%d  x=%d  y=%d",OB->Ref.General->Message,OB->Index,OB->RealX>>4,OB->RealY>>4);
				ShowString(smapx,ssy,ggg,&WhiteFont);
				ssy+=20;

				char ordmes[1024];
				strcpy(ordmes,"Groups:");
				DetermineWhereUnitIs(MID,ordmes+strlen(ordmes));
				ShowString(smapx,ssy,ordmes,&WhiteFont);
				ssy+=16;

				sprintf(ggg,"Brigade: %d  BrIndex: %d",OB->BrigadeID,OB->BrIndex);
				ShowString(smapx,ssy,ggg,&WhiteFont);
				ssy+=16;
				if(OB->BrigadeID!=0xFFFF){
					Brigade* BR=&OB->Nat->CITY->Brigs[OB->BrigadeID];
					if(BR->ArmyID!=0xFFFF){
						sprintf(ggg,"Army: %d ",BR->ArmyID );
						ShowString(smapx,ssy,ggg,&WhiteFont);
						ssy+=16;
					};
				};
				if(OB->LocalOrder){
					Order1* OR1=OB->LocalOrder;
					char OSTR[400]="ORDER:";
					char cc[16];
					int nn=0;
					while(OR1&&nn<20){
						sprintf_s( cc,sizeof(cc)," %d",OR1->OrderType);
						strcat(OSTR,cc);
						OR1=OR1->NextOrder;
						nn++;
					};
					if(OR1)strcat(OSTR,".....");
					ShowString(smapx,ssy,OSTR,&WhiteFont);
					ssy+=16;
				};
			};
		};
	};
#ifdef COSSACKS2
	void ShowAddInfo(int x,int y);
	ShowAddInfo(smapx,ssy);
#endif //COSSACKS2
};
int MiniLx;
int MiniLy;
int MiniX,MiniY;
DWORD BMASK[MBR4]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
void DrawMiniFog();
void ViewRoads(int x,int y);
extern int NROADS;
void ShowSafetyInfo(int x,int y);
void DrawMinAZones(int x,int y);
void DrawMinAZonesVis(int x,int y);
void DrawAreas1(int xs,int ys,int x0,int y0,int Lx,int Ly);
void DrawForces(int xs,int ys,int x0,int y0,int Lx,int Ly);
extern int LastAttackX;
extern int LastAttackY;
extern int AlarmDelay;
extern IconSet UpgPanel;
extern IconSet AblPanel;
void DrawMinAZonesVis(int x,int y);
void DRAW_MAP_TMP();
void DrawLastWayOnMinimap();
#ifdef _USE3D
void CopyTo16(int x,int y,byte* Src,int Pitch,int Lx,int Ly);
byte tempminimap[maxmap][maxmap];
extern int FMSX;
void GMiniShow16(){
	int MAXMP=120;
	if(FullMini){
		if(RealLx==800){
			if(UpgPanel.GetMaxX(12)<10&&AblPanel.GetMaxX(12)<10&&!Inform)MAXMP=480;
		}else MAXMP=480;
	};
	if(!MiniMade)CreateMiniMap();
	int MaxMLX=msx>>ADDSH;
	int MaxMLY=msy>>ADDSH;
	MiniLx=MaxMLX;
	MiniLy=MaxMLY;
	if(MiniLx>MAXMP)MiniLx=MAXMP;
	if(MiniLy>MAXMP)MiniLy=MAXMP;
	MiniX=(mapx+(smaplx>>1)-MiniLx)>>ADDSH;
	MiniY=(mapy+(smaply>>1)-MiniLy)>>ADDSH;
	if(MiniX<0)MiniX=0;
	if(MiniY<0)MiniY=0;
	if(MiniX+MiniLx>=MaxMLX)MiniX=MaxMLX-MiniLx-1;
	if(MiniY+MiniLy>=MaxMLY)MiniY=MaxMLY-MiniLy-1;
	minix=RealLx-MiniLx-12-5;
	miniy=RealLy-MiniLy-16;
	memcpy(&tempminimap,&minimap,sizeof minimap);
	
	OneObject* OO;
	int mxx,myy;
	byte val;
	byte mycl=CLRT[MyNation];
	memset(BMASK,0,sizeof BMASK);
	byte mmsk=GM(MyNation);
	for(int g=0;g<MAXOBJECT;g++){
		OO=Group[g];
		if(OO&&!OO->Sdoxlo){
			mxx=(OO->RealX>>(9+ADDSH));
			myy=(OO->RealY>>(9+ADDSH));
			if(mxx>=0&&myy>=0&&mxx<254&&myy<254){
				val=CLRT[OO->NNUM];
				int scr=int(&tempminimap)+mxx+(myy<<8);
				if(OO->ImSelected&mmsk){
					val=0xFF;
					if(OO->BrigadeID!=0xFFFF){
						Brigade* BR=OO->Nat->CITY->Brigs+OO->BrigadeID;
						if(BR->WarType){
							int pp=BR->ID;
							int idx=pp>>5;
							BMASK[idx]|=(1<<(pp&31));
						};
					};
				};
				__asm{
					mov	eax,scr
					mov	bl,val
					mov	[eax],bl
					mov [eax+1],bl
					add	eax,256
					mov	[eax],bl
					mov [eax+1],bl
				};
			};
		};
	};
	if(EditMapMode){
		int x=mapPos[(MyNation<<1)]>>1;
		int y=mapPos[(MyNation<<1)+1]>>1;
		SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		Hline(minix+x-2-MiniX,miniy+y-MiniY,minix+x-2+4-MiniX,255);
		Vline(minix+x-MiniX,miniy+y-2-MiniY,miniy+y-2+4-MiniX,255);
		ShowMiniDestn();
		if(GetKeyState(VK_CONTROL)&0x8000)DrawMinAZones(minix-MiniX,miniy-MiniY);
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	}else{;
		SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		ShowMiniDestn();
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
		if(GetKeyState(VK_CONTROL)&0x8000)DrawMinAZonesVis(minix-MiniX,miniy-MiniY);
	};
	
	if((GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000)){
		//SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		//DrawForces(minix+2,miniy+2,MiniX+2,MiniY+2,MiniLx-1,MiniLy-1);
		//ViewRoads(minix+2,miniy+2);
		//ShowSafetyInfo(minix-MiniX,miniy-MiniY);
		//if((tmtmt&15)<7&&FullMini){
		//	NatSInf[0].Show(minix,miniy);
		//SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
		//};
	} else NROADS;
	if(PreviewBrig>=0&&PreviewBrig<100){
		SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		Brigade* BR=CITY[NatRefTBL[MyNation]].Brigs+PreviewBrig;
		int x=0;
		int y=0;
		int N=0;
		for(int p=0;p<BR->NMemb;p++){
			word MID=BR->Memb[p];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&OB->Serial==BR->MembSN[p]){
					x+=OB->RealX>>10;
					y+=OB->RealY>>10;
					N++;
				};
			};
		};
		if(N){
			x/=N;
			y/=N;
			x+=minix+1;
			y+=miniy+1;
			x-=MiniX;
			y-=MiniY;
			DrawLine(x-5,y-2,x-5,y+2,clrYello);
			DrawLine(x+5,y-2,x+5,y+2,clrYello);
			DrawLine(x-2,y-5,x+1,y-5,clrYello);
			DrawLine(x-2,y+5,x+2,y+5,clrYello);

			DrawLine(x-5,y-2,x-2,y-5,clrYello);
			DrawLine(x+5,y-2,x+2,y-5,clrYello);
			DrawLine(x-2,y+5,x-5,y+2,clrYello);
			DrawLine(x+2,y+5,x+5,y+2,clrYello);
		};
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	};
	PreviewBrig=-1;
	if(FogMode&&(!NATIONS[NatRefTBL[MyNation]].Vision)&&BaloonState!=1){
		int sofs=int(&tempminimap);
		#define FMSHI 3
		int fofs=int(fmap)+((FMSHI+FMSX*FMSHI)<<1);
		int MMSX=(256>>1);
		int MMSY=(256>>1);
		int addscr=256+256-MMSX-MMSX;
		int F_add=(FMSX<<(ADDSH))-(MMSX<<ADDSH);
		int DDDX=1<<ADDSH;
		__asm{
			push	esi
			push	edi
			pushf
			mov		esi,fofs
			mov		edi,sofs
			mov		edx,256
			mov		ebx,DDDX
	lopx1:
			mov		ecx,MMSX
	lopx3:	mov     ax,[esi]
			add		esi,ebx

			cmp		ax,1000
			ja		lopx2
			mov		word ptr [edi],0
			mov		word ptr [edi+edx],0
	lopx2:	add		edi,2
			dec		ecx
			jnz		lopx3
			add		esi,F_add
			add		edi,addscr
			dec		MMSY
			jnz		lopx1
			popf
			pop		edi
			pop		esi
		};
	};
	HandleGeology();
	extern DWORD GU_COLOR;
	GU_COLOR=0xD0FFFFFF;
	CopyTo16(minix,miniy,((byte*)&tempminimap)+MiniX+MiniY*256,maxmap,MiniLx,MiniLy);
	GU_COLOR=0xFFFFFFFF;
	if(AlarmDelay&&LastAttackX!=-1){
		SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		int x0=minix-MiniX+(LastAttackX>>(9+ADDSH));
		int y0=miniy-MiniY+(LastAttackY>>(9+ADDSH));
		int R=((60-AlarmDelay)%10)*2;
		Xbar(x0-R,y0-R,R+R,R+R,0xFF);
		AlarmDelay--;
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	};
	if(CurAINF_Show!=-1&&CurAINF_Show<7){
		GNFO.EINF[CurAINF_Show]->GAINF.Show(minix,miniy);
	};
	if(CurAINF_Show==7){
		DrawLastWayOnMinimap();
	};
	Xbar(minix+(mapx>>ADDSH)-MiniX,miniy+(mapy>>ADDSH)-MiniY,smaplx>>ADDSH,smaply>>ADDSH,0xFF);//14
	int mx2=MiniLx;
	int my2=MiniLy;
	byte ccc=0xD0+(NatRefTBL[MyNation]<<2);
	Hline(minix-1,miniy-1,minix+mx2,ccc);
	Hline(minix-1,miniy+my2,minix+mx2,ccc);
	Vline(minix-1,miniy-1,miniy+my2,ccc);
	Vline(minix+mx2,miniy,miniy+my2,ccc);
	Hline(minix-2,miniy-2,minix+mx2+1,ccc+1);
	Hline(minix-2,miniy+my2+1,minix+mx2+2,ccc+1);
	Vline(minix-2,miniy-2,miniy+my2+1,ccc+1);
	Vline(minix+mx2+1,miniy-1,miniy+my2+1,ccc+1);
	DrawStdRect(minix-2,miniy-2,minix+mx2+2,miniy+my2+2);
	return;
};
#endif //_USE3D

void GMiniShow(){
	//Inform=GetKeyState(VK_MENU);
	if(NoText||PlayGameMode==1)return;
#ifdef _USE3D
	GMiniShow16();
	return;
#endif //_USE3D
	int MAXMP=120;
	if(FullMini){
		if(RealLx==800){
			if(UpgPanel.GetMaxX(12)<10&&AblPanel.GetMaxX(12)<10&&!Inform)MAXMP=480;
		}else MAXMP=480;
	};
	if(!MiniMade)CreateMiniMap();
	int MaxMLX=msx>>ADDSH;
	int MaxMLY=msy>>ADDSH;
	MiniLx=MaxMLX;
	MiniLy=MaxMLY;
	if(MiniLx>MAXMP)MiniLx=MAXMP;
	if(MiniLy>MAXMP)MiniLy=MAXMP;
	MiniX=(mapx+(smaplx>>1)-MiniLx)>>ADDSH;
	MiniY=(mapy+(smaply>>1)-MiniLy)>>ADDSH;
	if(MiniX<0)MiniX=0;
	if(MiniY<0)MiniY=0;
	if(MiniX+MiniLx>=MaxMLX)MiniX=MaxMLX-MiniLx-1;
	if(MiniY+MiniLy>=MaxMLY)MiniY=MaxMLY-MiniLy-1;
	minix=RealLx-MiniLx-12-5;
	miniy=RealLy-MiniLy-16;
	int scr=int(ScreenPtr)+minix+miniy*SCRSizeX;
	int til=int(&minimap)+MiniX+MiniY*maxmap;
	int ssx=MiniLx>>2;
	for(int i=0;i<MiniLy;i++){
		__asm{
			push	esi
			push	edi
			mov		ecx,ssx
			cld
			mov		esi,til
			mov		edi,scr
			rep		movsd
			pop		edi
			pop		esi
		}
		scr+=SCRSizeX;
		til+=maxmap;
	};
	OneObject* OO;
	int mxx,myy;
	byte val;
	byte mycl=CLRT[MyNation];
	memset(BMASK,0,sizeof BMASK);
	byte mmsk=GM(MyNation);
	for(int g=0;g<MAXOBJECT;g++){
		OO=Group[g];
		if(OO&&!OO->Sdoxlo){
			mxx=(OO->RealX>>(9+ADDSH))-MiniX;
			myy=(OO->RealY>>(9+ADDSH))-MiniY;
			if(mxx>=0&&myy>=0&&mxx<MiniLx&&myy<MiniLy){
				val=CLRT[OO->NNUM];
				int scr=int(ScreenPtr)+minix+mxx+(myy+miniy)*SCRSizeX;
				//if(OO->PathDelay)val=0xFF;
				if(/*val==mycl&&*/OO->ImSelected&mmsk){
					val=0xFF;
					if(OO->BrigadeID!=0xFFFF){
						Brigade* BR=OO->Nat->CITY->Brigs+OO->BrigadeID;
						if(BR->WarType){
							int pp=BR->ID;
							int idx=pp>>5;
							BMASK[idx]|=(1<<(pp&31));
						};
					};
				};
				__asm{
					mov	eax,scr
					mov	bl,val
					mov	[eax],bl
					mov [eax+1],bl
					add	eax,SCRSizeX
					mov	[eax],bl
					mov [eax+1],bl
				};
			};
		};
	};
	if(EditMapMode){
		int x=mapPos[(MyNation<<1)]>>1;
		int y=mapPos[(MyNation<<1)+1]>>1;
		SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		Hline(minix+x-2-MiniX,miniy+y-MiniY,minix+x-2+4-MiniX,255);
		Vline(minix+x-MiniX,miniy+y-2-MiniY,miniy+y-2+4-MiniX,255);
		ShowMiniDestn();
		if(GetKeyState(VK_CONTROL)&0x8000)DrawMinAZones(minix-MiniX,miniy-MiniY);
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	}else{;
		SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		ShowMiniDestn();
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
		if(GetKeyState(VK_CONTROL)&0x8000)DrawMinAZonesVis(minix-MiniX,miniy-MiniY);
	};
	
	if((GetKeyState(VK_CONTROL)&0x8000)||(GetKeyState(VK_SHIFT)&0x8000)){
		//SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		//DrawForces(minix+2,miniy+2,MiniX+2,MiniY+2,MiniLx-1,MiniLy-1);
		//ViewRoads(minix+2,miniy+2);
		//ShowSafetyInfo(minix-MiniX,miniy-MiniY);
		//if((tmtmt&15)<7&&FullMini){
		//	NatSInf[0].Show(minix,miniy);
		//SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
		//};
	} else NROADS;
	if(PreviewBrig>=0&&PreviewBrig<100){
		SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		Brigade* BR=CITY[NatRefTBL[MyNation]].Brigs+PreviewBrig;
		int x=0;
		int y=0;
		int N=0;
		for(int p=0;p<BR->NMemb;p++){
			word MID=BR->Memb[p];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&OB->Serial==BR->MembSN[p]){
					x+=OB->RealX>>10;
					y+=OB->RealY>>10;
					N++;
				};
			};
		};
		if(N){
			x/=N;
			y/=N;
			x+=minix+1;
			y+=miniy+1;
			x-=MiniX;
			y-=MiniY;
			DrawLine(x-5,y-2,x-5,y+2,clrYello);
			DrawLine(x+5,y-2,x+5,y+2,clrYello);
			DrawLine(x-2,y-5,x+1,y-5,clrYello);
			DrawLine(x-2,y+5,x+2,y+5,clrYello);

			DrawLine(x-5,y-2,x-2,y-5,clrYello);
			DrawLine(x+5,y-2,x+2,y-5,clrYello);
			DrawLine(x-2,y+5,x-5,y+2,clrYello);
			DrawLine(x+2,y+5,x+5,y+2,clrYello);
		};
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	};
	PreviewBrig=-1;
	if(FogMode&&(!NATIONS[NatRefTBL[MyNation]].Vision)&&BaloonState!=1)DrawMiniFog();
	HandleGeology();
	if(AlarmDelay&&LastAttackX!=-1){
		SetRLCWindow(minix,miniy,MiniLx,MiniLy,SCRSizeX);
		int x0=minix-MiniX+(LastAttackX>>(9+ADDSH));
		int y0=miniy-MiniY+(LastAttackY>>(9+ADDSH));
		int R=((60-AlarmDelay)%10)*2;
		Xbar(x0-R,y0-R,R+R,R+R,0xFF);
		AlarmDelay--;
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
	};
	if(CurAINF_Show!=-1&&CurAINF_Show<7){
		GNFO.EINF[CurAINF_Show]->GAINF.Show(minix,miniy);
	};
	if(CurAINF_Show==7){
		DrawLastWayOnMinimap();
	};
	Xbar(minix+(mapx>>ADDSH)-MiniX,miniy+(mapy>>ADDSH)-MiniY,smaplx>>ADDSH,smaply>>ADDSH,0xFF);//14
	int mx2=MiniLx;
	int my2=MiniLy;
	byte ccc=0xD0+(NatRefTBL[MyNation]<<2);
	Hline(minix-1,miniy-1,minix+mx2,ccc);
	Hline(minix-1,miniy+my2,minix+mx2,ccc);
	Vline(minix-1,miniy-1,miniy+my2,ccc);
	Vline(minix+mx2,miniy,miniy+my2,ccc);
	Hline(minix-2,miniy-2,minix+mx2+1,ccc+1);
	Hline(minix-2,miniy+my2+1,minix+mx2+2,ccc+1);
	Vline(minix-2,miniy-2,miniy+my2+1,ccc+1);
	Vline(minix+mx2+1,miniy-1,miniy+my2+1,ccc+1);
	DrawStdRect(minix-2,miniy-2,minix+mx2+2,miniy+my2+2);
};
#define step 1
int StepX=0;
int StepY=0;
void SetEgoFlag(bool Ego){
	
};
extern int  ReliefBrush;
extern int  TerrBrush;
bool SelSoundDone;
extern int SelSoundType;
extern bool SelSoundReady;
void DoSelSound(int type){
	if(!SelSoundDone){
		SelSoundDone=true;
		SelSoundReady=true;
		SelSoundType=type;
	};
};
extern bool Fixed;
extern bool realLpressed;
extern bool realRpressed;
void MFix(){
	Fixed=true;
	if(Lpressed)Lpressed=realLpressed;
	if(Rpressed)Rpressed=realRpressed;
};
void CmdCreateNewTerr(byte NI,int x,int y,word ID);
word GetNewEnemy(int xr,int yr,byte NI);
void ShowPen(int x,int y,int r,byte c);
void AddHi(int x,int y1,int r,int h);
void AverageHi(int x,int y1,int r);
void PlanarHi(int x,int y1,int r);
void delTrees(int x,int y,int r);
void addTrees(int x,int y,int r);
extern int HiStyle;
int Prop43(int y);
bool GetRealCoords(int xs,int ys,int* xr,int* yr){
	int xmx,ymy;
	int smlx=smaplx<<Shifter;
	int smly=mul3(smaply)<<(Shifter-2);
	if(xs>smapx&&ys>smapy&&xs<smapx+smlx&&ys<smly){
		xmx=(mapx<<5)+((xs-smapx)<<(5-Shifter));
		ymy=(mul3(mapy)<<3)+((ys-smapy)<<(5-Shifter));
		if(Mode3D)ymy=ConvScrY(xmx,ymy);
		*yr=Prop43(ymy);
		*xr=xmx;
		return true;
	}else return false;
};
void RandomHi(int x,int y1,int r,int ms,int fnd);
void AddHiPlanar(int x,int y1,int r,int h);
void CreatePlane(int x,int y1,int r);
void SetSpot(int x,int y);
word GetNewFriend(int xr,int yr,byte NI);
bool LockMouse=false;
bool WasSelectionBar=false;
void CmdSelectBuildings(byte NI);
int LastMx;
int LastMy;
void CHKS(){
	//assert(StartX<100000);
};
bool StrelMode=false;
void ProcessRM_Load(int x,int y);
void ProcessRM_Save(int x0,int y0,int x1,int y1);
void ProcessTexPiece(int x,int y,bool press);
CEXPORT
int ScrollSpeed=5;
int LastClickTime=0;
bool LastRealLP=false;
int LastClcX,LastClcY;
extern int SHIFT_VAL;
extern bool SetDestMode;
extern char* FormationStr;
void ControlAZones(int x,int y);
void GoToCurSelPosition();
void CmdSelectIdlePeasants(byte);
void CmdSelectIdleMines(byte);
void ProcessInformation(bool Allow);
int InfAddY=16;
int InfDY=32+InfAddY;
int InfDX=10+243+6;
int InfY1=450+InfAddY;
int InfY2=450-22+InfAddY;
void CmdPause(byte NI);
word AdvancedGetNewEnemy(int rex,int rey,int xr,int yr,byte NI);
word AdvancedGetNewFriend(int rex,int rey,int xr,int yr,byte NI);
extern bool MouseOverZone;
bool MouseOverMiniMenu=0;
int TestCapture(OneObject* OBJ);
bool CheckWallClick(int x,int y);
void CmdSelAllUnits(byte NI);
void CmdSelAllShips(byte NI);
void HandleMouse(int x,int y);
void EDGETEST();
void ControlZones();
void UnPress();
int OldLpressed=-1;
bool OverInform=0;
bool unpress=0;
void GlobalHandleMouse();
void DrawZones();
void GameKeyCheck();
void DRAW_MAP_TMP();
void ProcessScreen(){

	GameKeyCheck();
	NoPFOG=1;
	GFieldShow();
	NoPFOG=0;
	GMiniShow();
	ShowProp();
	ShowAbility();
	DrawZones();
	GlobalHandleMouse();//mouseX,mouseY);
	MFix();
	DRAW_MAP_TMP();
};
void HandleSMSChat(char* Mess);
void HandlwSMSMouse();
void ProcessRESSEND();
void ProcessMapInterface();
extern bool DRAWLOCK;
void ClearModes();
extern bool MultiTvar;
extern bool OptHidden;
bool CheckFlagsNeed();
void CmdVote(byte result);
void ProcessDIP_DSS();
bool AutoSpeed=1;
void CmdSetSpeed(byte);
extern int ExitNI;
void GlobalHandleMouse(){
	if(ExitNI!=-1){
		CmdEndGame(ExitNI,1,0);
		ExitNI=-1;
	};
#ifndef SPEEDFIX
    if (AutoSpeed) {
        if (FrmDec == 1) {
            if (Flips < 22) {
                CmdSetSpeed(1);
            };
        }
        else {
            if (Flips > 28) {
                CmdSetSpeed(0);
            };
        };
    };
#endif
	if(SpecCmd==199){
		CmdEndGame(MyNation,1,115);
		SpecCmd=0;
	};
	if(SpecCmd==171){
		CmdVote(1);
		SpecCmd=0;
	};
	if(SpecCmd==172){
		CmdVote(2);
		SpecCmd=0;
	};
	void CmdMakeOneStep(byte NI,char Dir);
	extern short WheelDelta;
	if(SpecCmd==173){
		CmdMakeOneStep(MyNation,-WheelDelta/120);
		SpecCmd=0;
	};
	if(SpecCmd==174){
		CmdMakeOneStep(MyNation,-WheelDelta/120);
		SpecCmd=0;
	};
	void CmdRotateFormations(byte NI,byte Dir);
	if(SpecCmd==187){
		CmdRotateFormations(MyNation,128+32);
		SpecCmd=0;
	};
	if(SpecCmd==188){
		CmdRotateFormations(MyNation,128+64);
		SpecCmd=0;
	};
	if(SpecCmd==189){
		CmdRotateFormations(MyNation,128+64+32);
		SpecCmd=0;
	};
	if(SpecCmd==184){
		CmdRotateFormations(MyNation,128);
		SpecCmd=0;
	};
	if(SpecCmd==186){
		CmdRotateFormations(MyNation,0);
		SpecCmd=0;
	};
	if(SpecCmd==181){
		CmdRotateFormations(MyNation,64+32);
		SpecCmd=0;
	};
	if(SpecCmd==182){
		CmdRotateFormations(MyNation,64);
		SpecCmd=0;
	};
	if(SpecCmd==183){
		CmdRotateFormations(MyNation,32);
		SpecCmd=0;
	};

	DRAWLOCK=0;
	ProcessInformation(0);
	ProcessDIP_DSS();
	ProcessRESSEND();
	if(NPlayers>1){
		if(CheckFlagsNeed()){
			if(Inform)OptHidden=1;
			ProcessMapInterface();
		};
	}else{
		if(EditMapMode||MultiTvar||PlayGameMode==2){
			if(Inform)OptHidden=1;
			ProcessMapInterface();
		}else if(!BuildWall)ClearModes();
	};
	if(DRAWLOCK){
		
		ProcessTexPiece((mapx<<5)+(RealLx>>1),(mapy<<5)+RealLy,0);
		ShowRMap();
		return;
	};
	OldLpressed=-1;
	unpress=0;
	OverInform=0;
	if(Inform&&!NoPress){
		if(mouseX>InfDY-3&&mouseY<InfY1+8&&mouseX>RealLx-InfDX-3){
			OldLpressed=Lpressed;
			Lpressed=false;
			OverInform=1;
			unpress=1;
			UnPress();
		};
	};
	ShowRMap();
	MouseStack* MEV=ReadMEvent();
	if(MEV){
		int x0=mouseX;
		int y0=mouseY;
		bool lp=Lpressed;
		bool rp=Rpressed;
		bool rlp=realLpressed;
		bool rrp=realRpressed;
		bool Ctrl=(GetKeyState(VK_CONTROL)&0x8000)!=0;
		bool Shift=(GetKeyState(VK_SHIFT)&0x8000)!=0;
		int x,y;
		do{
			x=MEV->x;
			y=MEV->y;
			Lpressed=MEV->Lpressed;
			Rpressed=MEV->Rpressed;
			realLpressed=MEV->rLpressed;
			realRpressed=MEV->rRpressed;
			if(unpress){
				UnPress();
				Lpressed=0;
				Rpressed=0;
			};
			bool ll=Lpressed;
			bool rr=Rpressed;
			ControlZones();
			if(ll&&!Lpressed)unpress=1;
			if(rr&&!Rpressed)unpress=1;
			if(unpress&&SpecCmd==241)SpecCmd=0;
			HandleMouse(x,y);
			MEV=ReadMEvent();
		}while(MEV);
		Lpressed=lp;
		Rpressed=rp;
		if(unpress){
			Lpressed=0;
			Rpressed=0;
			if(SpecCmd==241)SpecCmd=0;
		};
		realLpressed=rlp;
		realRpressed=rrp;
		//if(x!=x0||y!=y0){
		//	ControlZones();
		//	HandleMouse(x0,y0);
		//};
	}else{
		bool ll=Lpressed;
		bool rr=Rpressed;
		ControlZones();
		if(ll&&!Lpressed)unpress=1;
		if(rr&&!Rpressed)unpress=1;
		if(unpress){
			Lpressed=0;
			Rpressed=0;
			if(SpecCmd==241)SpecCmd=0;
		};
		HandleMouse(mouseX,mouseY);
	};
	EDGETEST();
	if(OldLpressed!=-1)Lpressed=OldLpressed!=0;
};
extern char** NatNames;
extern bool RESMODE;
bool GUARDMODE=0;
bool PATROLMODE=0;
extern bool AttGrMode;

int GuardPtr1=10;
int GuardPtr2=12;
int PatrolPtr=13;
int AttGrPtr=9;
int EnterTrPtr=11;
int EnterBuildingPtr=11;
int StormEnemyPtr=15;//8;
int RepairPtr=3;
int AttackPtr=1;
int SetDestPtr=8;
int CapturePtr=2;
int GoToMinePtr=4;
bool CheckTransportEntering(OneObject* OB);
void CmdSetGuardState(byte,word);
extern char HEIGHTSTR[12];
bool CheckCurveLinked(int x,int y);
void CmdAttackGround(byte NI,int x,int y);

byte CANTAKEMASK[16];
byte CANTAKECURSOR[16]={6,5,5,7, 5,5,0,0, 1,0,0,0, 0,0,0,0};

extern int DrawPixMode;
extern bool WT_IS_INIT;

void SetPixSpot(int x,int y,int R,int h0,int h1);
int prevdrx=0;
int prevdry=0;
void SetPixLine(int x0,int y0,int x1,int y1,int R,int h0,int h1);
extern bool PenBackSide;
extern int PEN_RADIUS;
extern int PEN_BRIGHTNESS;
extern int CurDrMod;
void ProcessTablet();
void SetPixLine(int x0,int y0,int x1,int y1,int R,int R1,int h0,int h1,int h01);
int PrevPress=0;
void DrawHole(int x,int y);
extern int DrawGroundMode;
void SetGroundTexRound(int TxIdx,int x0,int y0,int r,int dr);
void ClearGroundTexRound(int x0,int y0,int r,int dr);
extern int CurGroundTexture;
extern int TexStDX;
extern int TexStDY;
byte YELLOW_COLOR=0;
byte BIRUZOVIJ=0;
word GetDoorsOfEnemy(int rex,int rey,int xr,int yr,byte NI);
char EXCOMM[1024];
bool HaveExComm=0;
void CmdAddMoney(byte,DWORD);
CEXPORT
void SetExComm(char* Data,int size){
	if(size<1024){
		memcpy(EXCOMM,Data,size);
		HaveExComm=1;
	};
};

//int NPOrd;
//PositionOrder POrd[32];

bool GetBrCenter(Brigade* BR, int* x, int* y);
void DrawPlaneLine(int x0,int y0,int x1,int y1,byte c);

void ShowBrigPosition(){
#ifndef COSSACKS2
	return;
#endif

}
CIMPORT
void CheckDipBuilding(byte NI, int Index);
CEXPORT
void SetCurPtr(int v);
extern byte* DIP_Data[64];
extern int DIP_DSize[64];
extern int NDipItems;
void CmdSendData(byte* Data,int size);
void ProcessRoadsEdition(int xc,int yc);
void ShiftBuilding(OneObject* OB,int dx,int dy){
	if(OB->NewBuilding){
		NewMonster* NM=OB->newMons;
		int xx,yy;
		OB->GetCornerXY(&xx,&yy);
		if(OB->Stage<OB->Ref.General->MoreCharacter->ProduceStages&&NM->NBLockPt){
			for(int i=0;i<NM->NBLockPt;i++){
				BClrPt(xx+NM->BLockX[i],yy+NM->BLockY[i]);
			};
		}else{
			for(int i=0;i<NM->NLockPt;i++){
				BClrPt(xx+NM->LockX[i],yy+NM->LockY[i]);
			};
		};
		OB->RealX+=dx;
		OB->RealY+=dy;
		OB->RZ=GetHeight(OB->RealX>>4,OB->RealY>>4);
		OB->GetCornerXY(&xx,&yy);
		if(OB->Stage<OB->Ref.General->MoreCharacter->ProduceStages&&NM->NBLockPt){
			for(int i=0;i<NM->NBLockPt;i++){
				BSetPt(xx+NM->BLockX[i],yy+NM->BLockY[i]);
			};
		}else{
			for(int i=0;i<NM->NLockPt;i++){
				BSetPt(xx+NM->LockX[i],yy+NM->LockY[i]);
			};
		};
	};
};
void HandleMouse(int x,int y)
{
	if(NDipItems){
		for(int i=0;i<NDipItems;i++){
			CmdSendData(DIP_Data[i],DIP_DSize[i]);
			free(DIP_Data[i]);
		};
		NDipItems=0;
	};
	bool TAKERES=1;
	int stptr=-1;
	if(HaveExComm){
		switch(EXCOMM[0]){
		case 0://chat message
			AssignHint1(EXCOMM+1,511,32);
			break;
		case 1://money bonus
			CmdAddMoney(MyNation,*((DWORD*)(EXCOMM+1)));
			break;
		};
		HaveExComm=0;
		memset(EXCOMM,0,sizeof EXCOMM);
	};
	PrevPress=CurPressure;
	ProcessTablet();
	HandlwSMSMouse();
	memset(CANTAKEMASK,0,16);

	/*
	if(GoAndAttackMode){
		if(Lpressed){
			Rpressed=Lpressed;
			Lpressed=0;
		};
	};
	*/
	if(GoAndAttackMode&&!NSL[MyNation])GoAndAttackMode=0;
	SetCurPtr(0);
	//ProcessInformation(0);
	if(Rpressed&&BuildMode){
		Rpressed=0;
		BuildMode=false;
		SetWallBuildMode(0xFF,0);
		UnPress();
	};
	if(Rpressed&&BuildWall){
		Rpressed=0;
		SetWallBuildMode(0xFF,0);
		UnPress();
	};
	byte MyMask=NATIONS[MyNation].NMask;
	byte LastSCMD=SpecCmd;
	
	/*
	if(Inform&&!NoPress){
		if(y>InfDY-3&&y<InfY1+8&&x>RealLx-InfDX-3){
			OldLpressed=Lpressed;
			Lpressed=false;
			OverInform=1;
		};
	};
	*/
	NoPress=false;
	if(!(GetKeyState(VK_SHIFT)&0x8000)){
		if(SHIFT_VAL){
			if(SHIFT_VAL>0&&SHIFT_VAL<99){
				CmdSelBrig(MyNation,0,SHIFT_VAL-1);
			};
		};
		SHIFT_VAL=0;
	};
	bool CanCapt=0;
	bool CanWork=0;
	bool CanAttBLD=0;
	bool CanEnter=0;
	bool CanShoot=0;
	bool CanRes=0;
	bool HaveLowUnits=0;
	bool SetDestOnly=1;
	bool CanSetDest=0;
	bool HavePeasants=0;
	bool Artilery=0;
	byte MTMASK=0;

	if(!(MouseOverZone||OverInform||MouseOverMiniMenu)){
#ifdef _USE3D
		void ProcessGModelsEdit();
		ProcessGModelsEdit();
		ProcessRoadsEdition(LastMx,LastMy);
#endif //_USE3D
		int N=ImNSL[MyNation];
		word* uni=ImSelm[MyNation];
		word* sns=ImSerN[MyNation];
		for(int i=0;i<N;i++){
			word MID=uni[i];
			if(MID!=0xFFFF){
				OneObject* ME=Group[MID];
				if(ME&&ME->Serial==sns[i]&&!ME->Sdoxlo){
					NewMonster* NM=ME->newMons;
					if(NM->AttBuild)CanAttBLD=1;
					if(!NM->Capture)CanCapt=1;
					if(NM->Peasant){
						CanWork=1;
						CanRes=1;
#ifndef CONQUEST
						CanTakeMask[0]=1;
						CanTakeMask[1]=1;
						CanTakeMask[2]=1;
						CanTakeMask[3]=1;
						CanTakeMask[4]=1;
						CanTakeMask[5]=1;
#endif
					};
					if(NM->CompxUnit&&NM->CompxUnit->CanTakeExRes){
						for(int j=0;j<16;j++){
							if(NM->CompxUnit->TakeResStage[j]){
								CANTAKEMASK[j]=1;
								CanRes=1;
							};
						};
					};
					if(!NM->Priest)MTMASK|=NM->KillMask;
					if(!(ME->NewBuilding||ME->LockType||NM->Artilery)){
						CanEnter=1;
						if(!NM->HighUnit)HaveLowUnits=1;
					};
					if(NM->Artilery)Artilery=1;
					if(NM->CanShoot)CanShoot=1;
					if(NM->Building){
						CanSetDest=1;
					}else SetDestOnly=0;
					if(NM->Peasant)HavePeasants=1;
				};
			};
		};
		if(!CanSetDest)SetDestOnly=0;
		if(GoAndAttackMode)SetCurPtr(AttackPtr);
		if(SetDestMode)SetCurPtr(SetDestPtr);
		if(AttGrMode)SetCurPtr(AttGrPtr);
	};
	if((!Lpressed)&&WasSelectionBar){
		int N=ImNSL[MyNation];
		if(N){
			word* seli=ImSelm[MyNation];
			for(int k=0;k<N;k++){
				word MID=seli[k];
				if(MID!=0xFFFF){
					OneObject* OB=Group[MID];
					if(OB&&OB->Ready)MakeOrderSound(OB,4);
				};
			};
		};
		WasSelectionBar=false;
	};
	bool Ipanel=true;
	bool cooren=false;
	bool mini=false;
	int xmx,ymy;
	if(x>=minix&&y>=miniy&&x<minix+(msx>>1)&&y<miniy+(msy>>1)&&!LockMouse){
		SetRLCWindow(minix,miniy,msx>>ADDSH,msy>>ADDSH,SCRSizeX);
		Hline(x-4,y,x-4+8,255);
		Vline(x,y-4,y-4+8,255);
		SetRLCWindow(0,0,COPYSizeX,RSCRSizeY,SCRSizeX);
		cooren=true;
		xmx=(x-minix+MiniX)<<ADDSH;
		ymy=(y-miniy+MiniY)<<ADDSH;
		if(xmx<1)xmx=1;
		if(ymy<1)ymy=1;
		if(xmx>=msx)xmx=msx-1;
		if(ymy>=msy)ymy=msy-1;
		xmx=(xmx<<5)+16;
		ymy=(mul3(ymy)<<3)+12;
		mini=true;
	}else{
		int smlx=smaplx<<Shifter;
		int smly=mul3(smaply)<<(Shifter-2);
		//int dvd=24>>(5-Shifter);
		if(x>smapx&&y>smapy&&x<smapx+smlx&&y<smly){
			xmx=(mapx<<5)+((x-smapx)<<(5-Shifter));
			ymy=(mul3(mapy)<<3)+((y-smapy)<<(5-Shifter));
			cooren=true;
		};
	};
	//if(Lpressed&&mini)Lpressed=true;
	int ysy=ymy;
	if(Mode3D&&cooren)ysy=ConvScrY(xmx,ymy);
	int yreal=Prop43(ysy);
	if(cooren){
		LastMx=xmx;
		LastMy=yreal;
	};
	sprintf(HEIGHTSTR,"%d",GetHeight(LastMx,LastMy));
	if(cooren&&EditMapMode){
		ControlAZones(xmx,yreal);
	};
	bool SETDEST=false;
	bool DOGUARD=0;
	bool DOPATROL=0;
	bool DOATTGR=0;
	if(PATROLMODE&&Rpressed){
		DOPATROL=1;
		PATROLMODE=0;
		Lpressed=false;
		Rpressed=false;
		UnPress();
	};
	if((SetDestMode||GUARDMODE||PATROLMODE||AttGrMode)&&(Lpressed||Rpressed)){
		if(SetDestMode)SETDEST=true;
		if(GUARDMODE)DOGUARD=1;
		if(PATROLMODE)DOPATROL=1;
		PATROLMODE=0;
		if(AttGrMode)DOATTGR=1;
		AttGrMode=0;
		SetDestMode=false;
		Lpressed=false;
		Rpressed=false;
		UnPress();
	};
	if(GetCoord){
		if(Lpressed){
			if(cooren){
				if(UNIM)UNIM(0,xmx>>5,ymy>>5);
			};
			Lpressed=false;
			SetCurPtr(0);
			curdx=0;
			curdy=0;
			GetCoord=false;
			DoSelSound(1);
			MFix();
			goto PrInfo;;
		}
		if(Rpressed){
			SetCurPtr(0);
			curdx=0;
			curdy=0;
			GetCoord=false;
			MFix();
			goto PrInfo;;
		};
	};
	if(SpecCmd==137)CmdPause(MyNation);
	if(SpecCmd==1)CmdSelAllUnits(MyNation);
		//CmdCreateGoodSelection(MyNation,0,0,msx<<5,msy<<5);
	if(SpecCmd==2)CmdCreateGoodSelection(MyNation,mapx<<5,mapy<<5,(mapx+smaplx-1)<<5,(mapy+smaply-1)<<5);
	//Select all peasants
	if(SpecCmd==3)CmdCreateGoodKindSelection(MyNation,0,0,msx<<5,mul3(msy)<<3,0);
	//On screen
	if(SpecCmd==4)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,(mapx+smaplx-1)<<5,mul3(mapy+smaply-1)<<3,0);
	//Select all warriors
	if(SpecCmd==5)CmdCreateGoodKindSelection(MyNation,0,0,msx<<5,mul3(msy)<<3,2);
	//On screen
	if(SpecCmd==6)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,mul3(mapx+smaplx-1)<<3,(mapy+smaply-1)<<5,2);
	//Select all towers
	//if(SpecCmd==7)CmdCreateGoodKindSelection(MyNation,0,0,msx<<5,mul3(msy)<<3,1);
	//On screen
	//if(SpecCmd==8)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,mul3(mapx+smaplx-1)<<5,(mapy+smaply-1)<<3,1);
	//Select all buildings
	if(SpecCmd==9)CmdSelectBuildings(MyNation);
	//On screen
	//if(SpecCmd==10)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,mul3(mapx+smaplx-1)<<3,(mapy+smaply-1)<<5,4);
	//Select all dragons
	//if(SpecCmd==13)CmdCreateGoodKindSelection(MyNation,0,0,msx<<5,mul3(msy)<<3,5);
	//On screen
	//if(SpecCmd==14)CmdCreateGoodKindSelection(MyNation,mapx<<5,mapy<<5,mul3(mapx+smaplx-1)<<5,(mapy+smaply-1)<<3,5);
	if(SpecCmd==200) {//DEL key pressed
        if (!RESMODE)
        {//BUGFIX: Don't kill if player is in resource transfer dialog
            CmdDie(MyNation);
        }
    }
	if(SpecCmd==201)CmdSelAllShips(MyNation);
	if(SpecCmd==111)GoToCurSelPosition();
	if(SpecCmd==112){
		if(LastActionX!=0){
			mapx=(LastActionX>>5)-(smaplx>>1);
			mapy=(LastActionY>>5)-(smaply>>1)-(GetHeight(LastActionX,LastActionY)>>(Shifter-1));
			if(mapx<=0)mapx=1;
			if(mapy<=0)mapy=1;
			if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
			if(mapy+smaply>msy+1)mapy=msy-smaply+1;
		};
	};
	if(SpecCmd==113)CmdSelectIdlePeasants(MyNation);
	if(SpecCmd==114)CmdSelectIdleMines(MyNation);
	if(SpecCmd==11){
		word NSel=ImNSL[MyNation];
		if(NSel){
			word MID=ImSelm[MyNation][0];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB)CmdCreateGoodTypeSelection(MyNation,0,0,msx<<5,mul3(msy)<<3,OB->NIndex);
			};
		};
	};
	//if(LastRealLP&&!Lpressed){
	//	if(GetRealTime()-LastClickTime<650&&Norma(x-LastClcX,y-LastClcY)<16){
	//		SpecCmd=241;
	//	};
	//	LastClickTime=GetRealTime();
	//	LastClcX=x;
	//	LastClcY=y;
	//};
	
	WallHandleMouse();
	if(SpecCmd==241){
		word NSel=ImNSL[MyNation];
		if(NSel){
			word MID=ImSelm[MyNation][0];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB)CmdCreateGoodTypeSelection(MyNation,mapx<<5,mapy<<4,(mapx+smaplx-1)<<5,(mapy+smaply-1)<<4,OB->NIndex);
			};
		};
	};
	
	SpecCmd=0;
	//ControlZones();
	LastRealLP=Lpressed;
	//if(BuildWall)goto Edgetest;
	int xxx;
	int yyy;
	if(cooren){
		xxx=xmx>>5;
		yyy=div24(ymy);
		MRealX=xmx;
		MRealY=yreal;
		if(LastSCMD==219)DelInGrid(xmx,ymy);
		if(!mini)ProcessTexPiece(xmx,(mapy<<5)+mouseY*2,Lpressed);
		if(LastSCMD==218){
			ProcessRM_Load(xmx,yreal);
		};
		TerrHi=GetHeight(xmx,yreal);
		if(MEditMode){
#ifdef _USE3D
			if(GetKeyState(VK_SPACE)&0x8000){
				void ShowSqPen(int x,int y1,int r,byte ptc);
				ShowSqPen(xmx,ysy,ReliefBrush<<4,255);
			}else ShowPen(xmx,ysy,ReliefBrush<<4,255);
#else //_USE3D
			ShowPen(xmx,ysy,ReliefBrush<<4,255);
#endif //_USE3D
		};
		if(HeightEditMode==1||HeightEditMode==10||HeightEditMode==11||HeightEditMode==12||HeightEditMode==13||HeightEditMode>=128){//edit height mode
			bool LockKey=0;
			if(Lpressed){
#ifdef _USE3D
		// -- Inserted by Silver ---27.11.2002
		// -- reason:
		//  when height changes, we need to rebuild ground meshes in the ground mesh cache
		void ResetGroundCache();
		ResetGroundCache();
		// -- end of vstavka -- 27.11.2002
#endif // _USE3D
				switch(HeightEditMode){
				case 1:
					HiStyle=1;
					LockKey=1;
					break;
				case 10:
					HiStyle=7;
					LockKey=1;
					break;
				case 11:
					HiStyle=9;
					LockKey=1;
					break;
				case 12:
					HiStyle=3;
					LockKey=1;
					break;
				};
			};
			if(Rpressed){
#ifdef _USE3D
		// -- Inserted by Silver ---27.11.2002
		// -- reason:
		//  when height changes, we need to rebuild ground meshes in the ground mesh cache
		void ResetGroundCache();
		ResetGroundCache();
		// -- end of vstavka -- 27.11.2002
#endif // _USE3D
				switch(HeightEditMode){
				case 1:
					HiStyle=2;
					LockKey=1;
					break;
				case 10:
					HiStyle=8;
					LockKey=1;
					break;
				case 11:
					HiStyle=5;
					LockKey=1;
					break;
				case 12:
					HiStyle=4;
					LockKey=1;
					break;
				};
			};
			bool Shpress=(GetKeyState(VK_SHIFT)&0x8000)||HeightEditMode==13;
			if(CheckCurve()||Shpress){
				bool LINK=CheckCurveLinked(xmx,yreal);
				if((Lpressed&&LINK)||Rpressed)Shpress=0;
				if(LINK)ShowPen(xmx,ysy,8,0xFB);
				if(Lpressed){
					AddPointToCurve(xmx,yreal,!Shpress,2);
					if(!Shpress)Lpressed=0;
				}else if(!Shpress)AddPointToCurve(xmx,yreal,true,2);
			}else ShowPen(xmx,ysy,ReliefBrush<<4,0xFB);
			switch(HiStyle){
			case 1://+
				AddHi(xmx,ysy,ReliefBrush<<4,10);
				break;
			case 2://+
				AddHi(xmx,ysy,ReliefBrush<<4,-10);
				break;
			case 3:// *
				AverageHi(xmx,ysy,ReliefBrush<<4);
				break;
			case 4:// /
				PlanarHi(xmx,ysy,ReliefBrush<<4);
				break;
			case 5:
				RandomHi(xmx,ysy,ReliefBrush<<4,1,1);
				break;
			case 6:
				RandomHi(xmx,ysy,ReliefBrush<<4,3,3);
				break;
			case 7://PgUP
				AddHiPlanar(xmx,ysy,ReliefBrush<<4,3);
				break;
			case 8://PgDN
				AddHiPlanar(xmx,ysy,ReliefBrush<<4,-3);
				break;
			case 9://Home
				CreatePlane(xmx,ysy,ReliefBrush<<4);
				break;
			};
#ifdef _USE3D
			if(HeightEditMode>=128){
				if(Lpressed){
					void AddRandomGSP(int x,int y,byte SType,int R);
					AddRandomGSP(xmx,yreal,HeightEditMode-128,ReliefBrush<<4);
				};
				if(Rpressed){
					void ClearGSInRadius(int x,int y,int R);
					ClearGSInRadius(xmx,yreal,ReliefBrush<<4);
				};
				//Lpressed=0;
			};
#endif
			HiStyle=0;
			if(LockKey)return;
		};
		if(HeightEditMode==2){//Add tree mode
			ShowPen(xmx,ysy,ReliefBrush<<4,0xFA);
		};
		if(HeightEditMode==3){//Delete tree mode
			ShowPen(xmx,ysy,ReliefBrush<<4,0xF9);
		};
		if(LockMode==1){
			//Lock mode
			ShowPen(xmx,ysy,32,0xDC);//ReliefBrush<<4,0xDC);
		};
		if(LockMode==2){
			//Lock mode
			ShowPen(xmx,ysy,32,0xBB);//ReliefBrush<<4,0xBB);
		};
		if(LockMode==3){
			//Lock mode
			ShowPen(xmx,ysy,32,0xD8);//ReliefBrush<<4,0xD8);
		};
		
		if(DrawPixMode&&(Lpressed||Rpressed||CurPressure||PrevPress)){
			int br;
			int BRSZ;
			if(WT_IS_INIT){
				//void SetPixLine(int x0,int y0,int x1,int y1,int R,int R1,int h0,int h1,int h01);
				if(GetKeyState(VK_CONTROL)&0x8000&&Lpressed){
					DrawHole(xmx,yreal);
				}else{
					int br0;
					int BRSZ0;	

					if(CurDrMod&2)BRSZ0=PEN_RADIUS*PrevPress/512;
					else BRSZ0=PEN_RADIUS;
					if(CurDrMod&1)br0=PEN_BRIGHTNESS*PrevPress/512;
					else br0=PEN_BRIGHTNESS;

					if(CurDrMod&2)BRSZ=PEN_RADIUS*CurPressure/512;
					else BRSZ=PEN_RADIUS;
					if(CurDrMod&1)br=PEN_BRIGHTNESS*CurPressure/512;
					else br=PEN_BRIGHTNESS;

					if(br>255)br=255;
					if(br0>255)br0=255;

					if(PenBackSide||(GetKeyState(VK_MENU)&0x8000))SetPixLine(xmx,ymy,prevdrx,prevdry,BRSZ0<<1,BRSZ<<1,255-br,255,255-br0);
					else SetPixLine(xmx,ymy,prevdrx,prevdry,BRSZ,BRSZ0,br,0,br0);
				};
				//if(Rpressed)SetPixLine(xmx,ymy,prevdrx,prevdry,BRSZ<<1,0,255);
			}else{
				BRSZ=PEN_RADIUS;
				br=PEN_BRIGHTNESS;
				if(br>255)br=255;
				if(Lpressed)SetPixLine(xmx,ymy,prevdrx,prevdry,BRSZ,br,0);
				if(Rpressed)SetPixLine(xmx,ymy,prevdrx,prevdry,BRSZ<<1,0,255);
			};
			
		};
		prevdrx=xmx;
		prevdry=ymy;
		if(DrawGroundMode){
			byte cc1=0xB4;
			byte cc2=0xBA;
			bool M3D=Mode3D;
#ifndef _USE3D
			Mode3D=false;
			int rr=ReliefBrush<<4;
			ShowPen(xmx,ymy,rr,cc1);
			int dr=CostThickness<<4;
			if(dr<rr)ShowPen(xmx,ymy,rr+dr,cc2);
			Mode3D=M3D;
#else
			int rr=ReliefBrush<<4;
			ShowPen(xmx,ysy,rr,cc1);
			int dr=CostThickness<<4;
			if(dr<rr)ShowPen(xmx,ysy,rr+dr,cc2);
#endif
			
		};
		if(WaterEditMode&&WaterEditMode<3){
			byte cc1=0xB4;
			byte cc2=0xBA;
			if(WaterEditMode==2){
				cc1=0xA8;
				cc2=0xC8;
			};
			bool M3D=Mode3D;
			Mode3D=false;
			int rr=ReliefBrush<<5;
			ShowPen(xmx,ymy,rr,cc1);
			int dr=CostThickness<<4;
			if(dr<rr)ShowPen(xmx,ymy,rr-dr,cc2);
			Mode3D=M3D;
		};
		if(Lpressed&&!(mini||BuildWall)){
			if(ChoosePosition){
				mapPos[(MyNation<<1)]=xxx;
				mapPos[(MyNation<<1)+1]=yyy;
				ChoosePosition=false;
			};
			if(HeightEditMode==2){
				//CreateBlob(xxx-1,yyy-1,4*BlobMode,ReliefBrush);
				addTrees(xmx,ysy,ReliefBrush<<4);
				goto Edgetest;
			};
			if(HeightEditMode==3){
				//CreateBlob(xxx-1,yyy-1,4*BlobMode,ReliefBrush);
				delTrees(xmx,ysy,ReliefBrush<<4);
				goto Edgetest;
			};
			if(WaterEditMode==1){
				SetWaterSpot(xmx,ysy,ReliefBrush);
				goto Edgetest;
			};
			if(WaterEditMode==2){
				EraseWaterSpot(xmx,ysy,ReliefBrush);
				goto Edgetest;
			};
			if(WaterEditMode==3){
				SetBrightSpot(xmx,ysy,ReliefBrush,true);
				goto Edgetest;
			};
			if(WaterEditMode==4){
				SetBrightSpot(xmx,ysy,ReliefBrush,false);
				goto Edgetest;
			};
			if(MEditMode){
				bool Shpress=(GetKeyState(VK_SHIFT)&0x8000)!=0;
				if(CheckCurve()||Shpress){
					AddPointToCurve(xmx,yreal,!Shpress,1);
					if(!Shpress)Lpressed=0;
				}else SetTexturedRound(xmx,ymy,ReliefBrush*16,EditMedia);
				goto Edgetest;
			};
			if(LockMode==1){
				AddUnLockbar(xmx>>6,yreal>>6);
				goto Edgetest;
			};
			if(LockMode==2){
				AddLockBar(xmx>>6,yreal>>6);
				goto Edgetest;
			};
			if(LockMode==3){
				AddDefaultBar(xmx>>6,yreal>>6);
				goto Edgetest;
			};
			if(Creator<4096+200){
				//for(int i=0;i<2;i++)for(int j=0;j<2;j++)
				CmdCreateNewTerr(MyNation,xmx,ConvY(ysy),Creator|32768);
				//CmdCreateTerrain(MyNation,xxx,yyy,Creator);
				MFix();
				goto PrInfo;;
			};
			if(DrawGroundMode){
#ifdef _USE3D
				SetGroundTexRound(CurGroundTexture,xmx,yreal,ReliefBrush<<4,CostThickness<<4);
#else
				SetGroundTexRound(CurGroundTexture,xmx,ymy,ReliefBrush<<4,CostThickness<<4);
#endif
			};
			if(BuildMode){
				//if(CheckGold(xxx,yyy,BNat->Mon[BuildingID])&&ManualFogCheck(xxx,yyy,NATIONS[MyNation].Mon[BuildingID]->SizeX))
				//	CmdCreateBuilding(MyNation,xxx,yyy,BuildingID);
				int x0=xmx<<4;
				int y0=ConvY(ysy)<<4;
				if(CheckSmartCreationAbility(NatRefTBL[MyNation],NATIONS[MyNation].Mon[BuildingID]->newMons,&x0,&y0)!=-1){;
					CmdCreateBuilding(MyNation,x0,y0,BuildingID);
					if(!(GetKeyState(VK_SHIFT)&0x8000))BuildMode=false;
					Lpressed=false;
					UnPress();
				};
				MFix();
				goto PrInfo;
			};
		};
	};
	if(DrawGroundMode&&(Rpressed||(Lpressed&&PenBackSide))&&!(mini||BuildMode)){
		ClearGroundTexRound(xmx,ymy,ReliefBrush<<4,CostThickness<<4);
	};
	if(cooren){
		//assert(StartX<100000);
		int gx=xmx;
		int gy=ymy;
		int gx1=StartX;
		int gy1=StartY;
		int LStartY=ConvScrY(StartX,StartY);
		if(gx>gx1){int z=gx;gx=gx1;gx1=z;};
		if(gy>gy1){int z=gy;gy=gy1;gy1=z;};
		int xxx=xmx>>5;
		int yyy=div24(ysy);
		int xxx1=StartX>>5;
		int yyy1=div24(LStartY);
		if(xxx1<xxx){int z=xxx;xxx=xxx1;xxx1=z;};
		if(yyy1<yyy){int z=yyy;yyy=yyy1;yyy1=z;};
		if(mini){
			if(Rpressed&&GetKeyState(VK_CONTROL)&0x8000){
				CmdSetDst(MyNation,xmx<<4,yreal<<4);
				SetDestMode=false;
			};
			if(SETDEST){
				CmdSetDst(MyNation,xmx<<4,yreal<<4);
				SetDestMode=false;
			};
		};
		if(Lpressed&&!mini){
			word ENMI=GetNewEnemy(gx,gy,MyNation);
			if(ENMI!=0xFFFF){
				OneObject* OBBE=Group[ENMI];
				if(OBBE){
					char ggg[128];
					if(OBBE->InArmy&&OBBE->BrigadeID!=0xFFFF){
						Brigade* BR=OBBE->Nat->CITY->Brigs+OBBE->BrigadeID;
						int NU=0;
						char* UName=NULL;
						for(int i=NBPERSONAL;i<BR->NMemb;i++){
							word MID=BR->Memb[i];
							if(MID!=0xFFFF){
								OneObject* BOB=Group[MID];
								if(BOB&&BOB->Serial==BR->MembSN[i]){
									NU++;
									UName=BOB->Ref.General->Message;
								};
							};
						};
						if(UName){
							//sprintf(ggg,FormationStr,UName,NU);
                            sprintf(ggg, "Formation: %s (%s)", OBBE->newMons->Message, NatNames[OBBE->Ref.General->NatID]);
							if(GetKeyState(VK_CONTROL)&0x8000){
								sprintf(ggg+strlen(ggg)," [ID=%d]",ENMI);
							};
							AssignHint1(ggg,64);
						};
					}else{
						sprintf(ggg,"%s (%s) (%d/%d)",OBBE->newMons->Message,NatNames[OBBE->Ref.General->NatID],OBBE->Life,OBBE->MaxLife);
						AssignHint1(ggg,64);
					};
					AddPulse(ENMI,0xD0);
				};
			};
			if(!(BuildWall||DrawPixMode||DrawGroundMode)){
				if(!SelSoundDone){
					SelSoundDone=true;
					SelSoundReady=true;
					SelSoundType=0;
				};
				SetRLCWindow(smapx,smapy,smaplx<<Shifter,mul3(smaply)<<(Shifter-2),SCRSizeX);
				int xz1=smapx+((StartX-(mapx<<5))>>(5-Shifter));
				int yz1=smapy+((StartY-(mul3(mapy)<<3))>>(5-Shifter));
				int vdx=smapx-(mapx<<5);
				int vdy=smapy-(mapy<<4);
				if(!BIRUZOVIJ)BIRUZOVIJ=GetPaletteColor(0,255,255);
				Xbar(xz1,yz1,x-xz1,y-yz1,BIRUZOVIJ);
				if(Norma(x-xz1,y-yz1)>12)NoPress=true;
				if(x!=xz1||y!=yz1)Ipanel=false;
				if(LastSCMD==217){
					ProcessRM_Save(xz1-vdx,yz1-vdy,x-vdx,y-vdy);
				};
				
				LockMouse=true;
				int nn=0;
				CmdCreateGoodSelection(MyNation,gx,gy,gx1,gy1);
				WasSelectionBar=true;
			};
		} else{
			if(!mini){
				if(!BuildWall){
					StartX=xmx;
					StartY=ymy;
					SelSoundReady=false;
					SelSoundDone=false;
				};
			};
			LockMouse=false;
		};
		//if(Rpressed)SetSpot(mouseX,mouseY);
		if(Lpressed){
			word fr7=AdvancedGetNewFriend(xmx,yreal,xmx,(yreal>>1)-GetHeight(xmx,yreal),7);
			if(fr7!=0xFFFF)CheckDipBuilding(MyNation,fr7);
		};
		if(ImNSL[MyNation]>0&&!mini){
			word Friend=AdvancedGetNewFriend(xmx,yreal,xmx,(yreal>>1)-GetHeight(xmx,yreal),MyNation);
#ifdef COSSACKS2
			void TestConnector(word CurObj,byte NI);
			if(Friend!=0xFFFF)TestConnector(Friend,MyNation);
#endif //COSSACKS2
			word Att=0xFFFF;
			word Entr=GetDoorsOfEnemy(xmx,yreal,xmx,(yreal>>1)-GetHeight(xmx,yreal),NatRefTBL[MyNation]);;
			if(Entr!=0xFFFF){
				OneObject* OB=Group[Entr];
				if(OB&&(!OB->newMons->UnitAbsorber)&&!(OB->newMons->PeasantAbsorber&&HavePeasants))Entr=0xFFFF;
			};
			if(!CanEnter)Entr=0xFFFF;
			if(MTMASK||CanAttBLD||CanCapt){
				Att=AdvancedGetNewEnemy(xmx,yreal,xmx,(yreal>>1)-GetHeight(xmx,yreal),NatRefTBL[MyNation]);
				if(Att!=0xFFFF&&Entr==0xFFFF&&!GoAndAttackMode){
					OneObject* AOB=Group[Att];
					if(AOB){
						if(AOB->newMons->CanBeKilledInside&&AOB->NInside&&CanShoot)SetCurPtr(AttackPtr);
						if(AOB->newMons->MathMask&MTMASK)SetCurPtr(AttackPtr);
						else{
							if(AOB->NewBuilding||AOB->Wall||AOB->LockType){
								if(CanAttBLD)SetCurPtr(AttackPtr);
							}
						};
						if(curptr!=AttackPtr&&AOB->newMons->Capture&&CanCapt&&TestCapture(AOB)!=-1)SetCurPtr(CapturePtr);
						if(GetKeyState(VK_CONTROL)&0x8000){
							//if(AOB->newMons->CanBeCapturedWhenFree)curptr=StormEnemyPtr;
							//else 
							if(AOB->newMons->UnitAbsorber||(AOB->newMons->PeasantAbsorber&&HavePeasants))SetCurPtr(StormEnemyPtr);
						};
					};
				};
			};
			if(SETDEST){
				CmdSetDst(MyNation,xmx<<4,yreal<<4);
			};
			if(DOATTGR){
				CmdAttackGround(MyNation,xmx<<4,yreal<<4);
			};
			if(Friend!=0xFFFF){
				OneObject* FRI=Group[Friend];
				if(FRI&&FRI->newMons->Transport&&CheckTransportEntering(FRI)&&(CanEnter||Artilery))
					SetCurPtr(EnterTrPtr);
			};
			byte rk=0xFF;
			if(CanWork){
				if(Friend!=0xFFFF){
					OneObject* FR=Group[Friend];
					if(FR&&(FR->NewBuilding)&&!FR->Sdoxlo){
						//if(curptr!=RepairPtr&&FR->newMons->ResConcentrator&&FR->Ready&&FR->Ref.General->MoreCharacter->MaxInside+FR->AddInside>FR->NInside)curptr=GoToMinePtr;
						if(FR->Life<FR->MaxLife&&!(GetKeyState(VK_CONTROL)&0x8000))SetCurPtr(RepairPtr);
					};
				};
				if(CheckWallClick(xmx,yreal)){
					SetCurPtr(RepairPtr);
				};
			};
			if(curptr!=GoToMinePtr&&curptr!=RepairPtr&&CanEnter&&Friend!=0xFFFF){
				OneObject* FR=Group[Friend];
				if(FR&&(FR->NewBuilding)&&FR->Stage>=FR->Ref.General->MoreCharacter->ProduceStages&&!FR->Sdoxlo){
					if((FR->Ready||FR->LocalOrder)&&FR->Ref.General->MoreCharacter->MaxInside+FR->AddInside>FR->NInside){
						if(FR->newMons->PeasantAbsorber){
							if(HavePeasants)SetCurPtr(EnterBuildingPtr);
						}else if(FR->newMons->UnitAbsorber){
							if(HaveLowUnits||!FR->newMons->HighUnitCantEnter)SetCurPtr(EnterBuildingPtr);
						};
					};
				};
			};
			if(CanRes){
				rk=DetermineResource(xmx,yreal);
				if(!curptr){
#ifdef CONQUEST
					if(rk<16&&CANTAKEMASK[rk]){
						SetCurPtr(CANTAKECURSOR[rk]);
						TAKERES=1;
						stptr=curptr;
					};
#else
					if(rk==TreeID&&CANTAKEMASK[rk])SetCurPtr(6);
					if(rk==StoneID)SetCurPtr(5);
					if(rk==FoodID)SetCurPtr(7);
#endif
				};
			};
			if(Entr!=0xFFFF){
				OneObject* OB=Group[Entr];
				if(OB){
					if(OB->newMons->UnitAbsorber)SetCurPtr(StormEnemyPtr);
					else if(OB->newMons->PeasantAbsorber&&HavePeasants)SetCurPtr(StormEnemyPtr);
				};
			};
			if(Rpressed){
				DoSelSound(1);
				Rpressed=false;
				UnPress();
				if(GetKeyState(VK_CONTROL)&0x8000||(Friend!=0xFFFF&&SetDestOnly)){
					CmdSetDst(MyNation,xmx<<4,yreal<<4);
					SetDestMode=false;
					//goto PrInfo;;
				};
				if(!StrelMode){
					StrelMode=true;
					SStartTime=GetTickCount();
					SStartX=xmx;
					SStartY=yreal;
					SCntrX=0;//xmx-SStartX;
					SCntrY=0;//yreal-SStartY;
				}
			}else{				
				if(GetKeyState(VK_ESCAPE)&0x8000){
					// Esc
					StrelMode=false;
				}
				if(StrelMode&&!realRpressed){
					//perform action
					bool AnimCurs=1;
					StrelMode=false;
					int rx=xmx-SStartX;
					int ry=yreal-SStartY+((GetHeight(SStartX,SStartY)-GetHeight(xmx,yreal))<<1);
					int Nr=int(sqrt(rx*rx+ry*ry));
					short DIRECT=512;
					if(Nr>30){
						DIRECT=GetDir(rx,ry);
						//discrete directions
						DIRECT=(DIRECT+8)&0xF0;
						xmx=SStartX;
						yreal=SStartY;
					};
					xmx=SStartX;
					yreal=SStartY;
					if(rk<0xFE&&stptr==curptr&&TAKERES&&Nr<=30){
						CmdTakeRes(MyNation,xmx,yreal,rk);
					};
					//word Friend=GetNewFriend(gx,gy,MyNation);//Mops[yyy][xxx];
					if(Friend!=0xFFFF&&curptr!=5&&curptr!=6&&curptr!=7&&Nr<=30){
						OneObject* OB=Group[Friend];
						if(OB&&OB->NewBuilding&&!OB->Sdoxlo){
							CmdBuildObj(MyNation,Friend);
							if(OB->Stage>=OB->Ref.General->MoreCharacter->ProduceStages)CmdGoToMine(MyNation,Friend);
							AddPulse(Friend,0x9A);
						};
					};
					DetermineWallClick(xmx,yreal);
					if(Friend!=0xFFFF&&Nr<=30){
						OneObject* FOBJ=Group[Friend];
						if(FOBJ){
							if(FOBJ->newMons->Transport){
								CmdGoToTransport(MyNation,Friend);
								AddPulse(Friend,0x9A);
								goto PrInfo;;
							};
						};
					};
					if(Entr!=0xFFFF&&Nr<=30){
						OneObject* AOBJ=Group[Entr];
						if(int(AOBJ)){
							if(AOBJ->newMons->CanBeCapturedWhenFree){
								CmdGoToMine(MyNation,Entr);
							};
						};
					}else
					if(Att!=0xFFFF&&Nr<=30){
						OneObject* AOBJ=Group[Att];
						if(int(AOBJ)){
							if(!(AOBJ->NMask&MyMask)){
								if(GetKeyState(VK_CONTROL)&0x8000){
									if(AOBJ->newMons->CanBeCapturedWhenFree){
										CmdGoToMine(MyNation,Att);
									};
								}else{
									CmdAttackObj(MyNation,Att,DIRECT);
#ifdef COSSACKS2
									GoAndAttackMode=1;
#endif
									CmdSendToXY(MyNation,xmx<<4,yreal<<4,DIRECT);
								};
								AddPulse(Att,0xD0);
							}else CmdSendToXY(MyNation,xmx<<4,yreal<<4,DIRECT);
						}else CmdSendToXY(MyNation,xmx<<4,yreal<<4,DIRECT);
					}else CmdSendToXY(MyNation,xmx<<4,yreal<<4,DIRECT);
				};
			};
			if(GUARDMODE){
				if(DOGUARD){
					CmdSetGuardState(MyNation,Friend);
					GUARDMODE=0;
				};
				if(Friend!=0xFFFF){
					SetCurPtr(GuardPtr2);
				}else{
					SetCurPtr(GuardPtr1);
				};
			};
			if(PATROLMODE){
				SetCurPtr(PatrolPtr);
			};
			if(DOPATROL){
				CmdPatrol(MyNation,xmx<<4,yreal<<4);
			};
		}else{
			if(DOPATROL){
				CmdPatrol(MyNation,xmx<<4,yreal<<4);
			}else
			if(Rpressed&&ImNSL[MyNation]>0){
				CmdSendToXY(MyNation,xmx<<4,yreal<<4,512);
				Rpressed=false;
				UnPress();
			};
			if(!realRpressed)StrelMode=false;
		};
	};
	
	int rx;
	int ry;
	int Nr;
	int H;

	if(StrelMode){
		H=GetHeight(SStartX,SStartY);
		int H1=(GetHeight(xmx,yreal)-H)<<1;		

		rx=(xmx-SStartX)>>(5-Shifter);
		ry=((yreal-SStartY)>>(5-Shifter))-(H1>>(5-Shifter));
		Nr=int(sqrt(rx*rx+ry*ry));
	}

	if(StrelMode){
		if(GetKeyState(VK_MENU)&0x8000){
			SStartX=xmx-SCntrX;
			SStartY=yreal-SCntrY;
		}else{
			SCntrX=xmx-SStartX;
			SCntrY=yreal-SStartY;
		}
		
		if(Nr>30){
			int dx=mapx<<5;
			int dy=mapy<<4;

			int x0=(SStartX-dx)>>(5-Shifter);
			int y0=((SStartY>>1)-dy-H)>>(5-Shifter);

			if(!YELLOW_COLOR){
				YELLOW_COLOR=GetPaletteColor(255,255,0);
			};
			
			int ux=rx*20/Nr;
			int uy=ry*20/Nr;
			int uxt= uy;
			int uyt=-ux;
			//DrawLine(x0,y0,x0+rx,y0+(ry>>1),clrYello);
			DrawLine(x0-uxt,y0-(uyt>>1),x0+uxt,y0+(uyt>>1),YELLOW_COLOR);
			DrawLine(x0+uxt,y0+(uyt>>1),x0+rx-2*ux+uxt,y0+((ry-2*uy+uyt)>>1),YELLOW_COLOR);
			DrawLine(x0+rx-2*ux+uxt,y0+((ry-2*uy+uyt)>>1),x0+rx-2*ux+2*uxt,y0+((ry-2*uy+2*uyt)>>1),YELLOW_COLOR);
			DrawLine(x0+rx-2*ux+2*uxt,y0+((ry-2*uy+2*uyt)>>1),x0+rx,y0+(ry>>1),YELLOW_COLOR);
			DrawLine(x0+rx,y0+(ry>>1),x0+rx-2*ux-2*uxt,y0+((ry-2*uy-2*uyt)>>1),YELLOW_COLOR);
			DrawLine(x0+rx-2*ux-2*uxt,y0+((ry-2*uy-2*uyt)>>1),x0+rx-2*ux-uxt,y0+((ry-2*uy-uyt)>>1),YELLOW_COLOR);
			DrawLine(x0+rx-2*ux-uxt,y0+((ry-2*uy-uyt)>>1),x0-uxt,y0-(uyt>>1),YELLOW_COLOR);

		};
	}
	if(StrelMode&&!LMode){
		
		// draw position		
		byte NI=NatRefTBL[MyNation];
		int N=ImNSL[NI];
		word* ID=ImSelm[NI];
		word* SN=ImSerN[NI];
		word BrigID=0xFFFF;
		for(int i=0;i<N;i++){
			word OID=ID[i];
			if(OID!=0xFFFF){
				OneObject* OB=Group[OID];
				if(OB&&(OB->Serial==SN[i])&&!OB->Sdoxlo&&OB->BrigadeID!=0xFFFF){
					if(BrigID!=0xFFFF){
						if(BrigID!=OB->BrigadeID){
							BrigID=0xFFFF;
							break;
						}
					}else{
						BrigID=OB->BrigadeID;
					}
				}
			}
		}
		if(BrigID!=0xFFFF){
			Brigade* Br=CITY[NI].Brigs+BrigID;
			byte dir;
			if(Nr<30){
				Br->GetCenter(&rx,&ry,&dir);				
				rx=SStartX-rx;
				ry=SStartY-ry;
			}
			//CITY[NI].Brigs[BrigID].CreateSimpleOrderedPositions(SStartX<<4,SStartY<<4,(GetDir(rx,ry)+8)&0xF0);
			int N=PORD.CreateSimpleOrdPos(SStartX<<4,SStartY<<4,(GetDir(rx,ry)+8)&0xF0,Br->NMemb-NBPERSONAL,Br->Memb+NBPERSONAL,ElementaryOrders+Br->WarType-1);
		}else{
		//if(0){
//			ShowBrigPosition();

			//word* SSEL=znew(word,Nsel);
			//bool AllPus=true;
			int xc=0;
			int yc=0;
			//int Np=0;
			int Nu=0;
			int BRIDX[32];
			int BRX[32];
			int BRY[32];
			int NBRG=0;
			int ADX=0;
			int ADY=0;
			
			byte LastNI=NatRefTBL[MyNation];

			byte NI=NatRefTBL[MyNation];
			int Nsel=ImNSL[NI];
			word* SMon=ImSelm[NI];
			word* MSN=ImSerN[NI];

			for(int i=0;i<Nsel;i++){
				word MID=SMon[i];
				if(MID!=0xFFFF){
					OneObject* OB=Group[MID];
					if(OB&&OB->Serial==MSN[i]){
						Nu++;
						xc+=OB->RealX;
						yc+=OB->RealY;
						if(OB->InArmy){
//							byte Usage=OB->newMons->Usage;
//							if(Usage!=PushkaID)AllPus=false;
							//SSEL[Np]=MID;
//							Np++;
//						}else{
							if(OB->BrigadeID!=0xFFFF&&NBRG<32){
								Brigade* BR=CITY[OB->NNUM].Brigs+OB->BrigadeID;
								LastNI=OB->NNUM;
								if(BR->ID<8192&&BR->NMemb){
									BRIDX[NBRG]=BR->ID;
									NBRG++;
									BR->ID+=8192;
									if(BR->WarType){
										int vx=TCos[byte(BR->Direction)]>>2;
										int vy=TSin[byte(BR->Direction)]>>2;
										ADX+=vx;
										ADY+=vy;
									};
								};
							};
						};
					};
				};
			};
			char AverageDir=GetDir(ADX,ADY);
			int BCX=0;
			int BCY=0;

			int Bw[32];
			int Bh[32];

			for(int i=0;i<NBRG;i++){
				Brigade* BR=CITY[LastNI].Brigs+BRIDX[i];
				BR->ID-=8192;
				GetBrCenter(BR,BRX+i,BRY+i);
				BCX+=BRX[i];
				BCY+=BRY[i];

				word type=BR->WarType-1;
				Bw[i]=ElementaryOrders[type].Width;
				Bh[i]=ElementaryOrders[type].Hight;

			};
			int rdx=16;
			int rdy=16;
			
			int x=SStartX;
			int y=SStartY;
			
			/*
			if(LastDirection<256){
				rdx=int(TCos[byte(LastDirection)])<<4;
				rdy=int(TSin[byte(LastDirection)])<<4;
			}else{
				if(Nu){
					xc/=Nu;
					yc/=Nu;
					rdx=xc-x;
					rdy=yc-y;
					LastDirection=GetDir(-rdx,-rdy);
					char DRR=LastDirection;
					if(abs(DRR-AverageDir)<16)LastDirection=AverageDir;
				};
			};
			*/

			char ddir=-AverageDir;
			//char ddir=LastDirection-AverageDir;
			
			int SIN=TSin[byte(ddir)];
			int COS=TCos[byte(ddir)];
			
			if(NBRG){
				BCX/=NBRG;
				BCY/=NBRG;
								
				if(Nr<30){
					LastDirection=GetDir(SStartX-BCX,SStartY-BCY);
				}else{
					LastDirection=GetDir(rx,ry);
				}

				int Bdx[32];
				int Bdy[32];
				for(int i=0;i<NBRG;i++){
					int dx=BRX[i]-BCX;
					int dy=BRY[i]-BCY;
					int Nr=Norma(dx,dy);
					if(Nr>1000){
						dx=(dx<<8)/Nr;
						dy=(dy<<8)/Nr;
					};

					Bdx[i]=(dx*COS-dy*SIN)>>8;
					Bdy[i]=(dx*SIN+dy*COS)>>8;
				}
				
				bool BChk[32];
				word BID;
				
				
				// отодвинуть
				memset(BChk,0,sizeof BChk);				
				do{
					BID=0xFFFF;
					for(int i=0;i<NBRG;i++)
						if(!BChk[i]&&(BID==0xFFFF||Bdx[i]>Bdx[BID])){
							BID=i;
						}
					if(BID!=0xFFFF){
						BChk[BID]=1;						
						for(int i=0;i<NBRG;i++)if(!BChk[i]&&i!=BID){

							int dx=((Bh[i]+Bh[BID])>>1)+40-abs(Bdx[BID]-Bdx[i]);
							int dy=((Bw[i]+Bw[BID])>>1)+20-abs(Bdy[BID]-Bdy[i]);
							
							if(dx>0&&dy>0){
								if(dx+120<dy){
									Bdx[i]-=dx;
								}else{
									if(Bdy[BID]-Bdy[i]>0) Bdy[i]-=dy;
										else Bdy[i]+=dy;
								}
							}
						}
					}
				}while(BID!=0xFFFF);
				
				
				/*
				// по y				
				memset(BChk,0,sizeof BChk);				
				do{
					BID=0xFFFF;
					for(int i=0;i<NBRG;i++)
						if(!BChk[i]&&(BID==0xFFFF||Bdy[i]>Bdy[BID])){
							BID=i;
						}
					if(BID!=0xFFFF){
						BChk[BID]=1;						
						// отодвинуть
						for(int i=0;i<NBRG;i++)
							if(!BChk[i]&&i!=BID&&
								Bdx[i]+(Bh[i]+Bh[BID])/2>Bdx[BID]&&
								Bdy[i]+(Bw[i]+Bw[BID])/2>Bdy[BID]){
								Bdy[i]-=((Bw[i]+Bw[BID]+40)>>1)-Bdy[BID]+Bdy[i];
							}
					}
				}while(BID!=0xFFFF);
				*/
				
				
				ddir=(LastDirection+8)&0xF0;
				SIN=TSin[byte(ddir)];
				COS=TCos[byte(ddir)];

				for(int i=0;i<NBRG;i++){
					int dx=Bdx[i];
					int dy=Bdy[i];
					//int dx=BRX[i]-BCX;
					//int dy=BRY[i]-BCY;
					/*
					int Nr=Norma(dx,dy);
					if(Nr>1000){
						dx=(dx*256)/Nr;
						dy=(dy*256)/Nr;
					};
					*/
					
					BRX[i]=x+((dx*COS-dy*SIN)>>8);
					BRY[i]=y+((dx*SIN+dy*COS)>>8);

					
					Brigade* BR=CITY[LastNI].Brigs+BRIDX[i];
					//int Dir=BR->Direction;
					//if(NBRG==1)Dir=LastDirection;
					//char Dir=LastDirection;
					/*
					char Dir=BR->Direction-AverageDir+LastDirection;
					if(OneDirection)Dir=LastDirection;
					*/
					int N=PORD.CreateSimpleOrdPos(BRX[i]<<4,BRY[i]<<4,(LastDirection+8)&0xF0,BR->NMemb-NBPERSONAL,BR->Memb+NBPERSONAL,ElementaryOrders+BR->WarType-1);

					byte cl=clrYello;
					
					for(int j=0;j<N;j++){
						int xx=PORD.px[j]>>4;
						int yy=PORD.py[j]>>4;
						DrawPlaneLine(xx,yy-2,xx,yy+2,cl);//DrawPlaneLine
						DrawPlaneLine(xx-2,yy,xx+2,yy,cl);
					}
				};
			};
		}
	};
Edgetest:;
PrInfo:;
};
extern int LastScrollTime;
#ifdef CRASHTESTFIX
//Hardcoded speed of arrow key scrolling = 4
const int kKeyboardScrollSpeed = 4;

void EDGETEST()
{
    int x = mouseX;
    int y = mouseY;

    if (SHOWSLIDE)
    {
        bool MoveX = false;
        bool MoveY = false;
        int OLDMX = mapx;
        int OLDMY = mapy;
        OneObject* SOB = NULL;
        //Check arrow keys
        if (!(EnterChatMode || RESMODE))
        {
            OneObject* SOB = NULL;
            if ((!DrawGroundMode) && EditMapMode && ImNSL[MyNation] && ImSelm[MyNation][0] != 0xFFFF) {
                int groupIndex = ImSelm[MyNation][0];
                SOB = Group[groupIndex];
                if (SOB && SOB->NewBuilding) {
                    int dx = 0;
                    int dy = 0;
                    if (GetKeyState(VK_RIGHT) & 0x8000) {
                        dx = 16;
                    };
                    if (GetKeyState(VK_LEFT) & 0x8000) {
                        dx = -16;
                    };
                    if (GetKeyState(VK_UP) & 0x8000) {
                        dy = -32;
                    };
                    if (GetKeyState(VK_DOWN) & 0x8000) {
                        dy = 32;
                    };
                    ShiftBuilding(SOB, dx, dy);
                }
            }
            else {
                if (GetKeyState(VK_RIGHT) & 0x8000)
                {
                    StepX = kKeyboardScrollSpeed;
                    MoveX = true;
                }

                if (GetKeyState(VK_LEFT) & 0x8000)
                {
                    StepX = -kKeyboardScrollSpeed;
                    MoveX = true;
                }

                if (GetKeyState(VK_UP) & 0x8000)
                {
                    StepY = -kKeyboardScrollSpeed;
                    MoveY = true;
                }

                if (GetKeyState(VK_DOWN) & 0x8000)
                {
                    StepY = kKeyboardScrollSpeed;
                    MoveY = true;
                }
            }
        }

        //Check mouse scrolling (it has priority over arrow keys)
        //Left
        if (x < 6)
        {
            if (StepX > 0)
            {
                StepX = 0;
            }
            StepX--;
            if (StepX < -ScrollSpeed)
            {
                StepX = -ScrollSpeed;
            }
            MoveX = true;
        }
        //Up
        if (y < 6)
        {
            if (StepY > 0)
            {
                StepY = 0;
            }
            StepY--;
            if (StepY < -ScrollSpeed)
            {
                StepY = -ScrollSpeed;
            }
            MoveY = true;
        }
        //Right
        if (x > RealLx - 6)
        {
            if (StepX < 0)
            {
                StepX = 0;
            }
            StepX++;
            if (StepX > ScrollSpeed)
            {
                StepX = ScrollSpeed;
            }
            MoveX = true;
        }
        //Down
        if (y > RealLy - 6)
        {
            if (StepY < 0)
            {
                StepY = 0;
            }
            StepY++;
            if (StepY > ScrollSpeed)
            {
                StepY = ScrollSpeed;
            }
            MoveY = true;
        }

        //Sanity checks?
        if (StepX && !MoveX)
        {
            if (StepX > 0)
            {
                StepX--;
            }
            if (StepX < 0)
            {
                StepX++;
            }
        }
        if (StepY && !MoveY)
        {
            if (StepY > 0)
            {
                StepY--;
            }
            if (StepY < 0)
            {
                StepY++;
            }
        }

        //Adjust view window position
        if (StepX < 0)
        {
            mapx -= -StepX / 2;
        }
        else
        {
            mapx += StepX / 2;
        }
        if (StepY < 0)
        {
            mapy -= -StepY / 2;
        }
        else
        {
            mapy += StepY / 2;
        }

        //Stop at map borders
        if (mapx <= 0)
        {
            mapx = 1;
        }
        if (mapy <= 0)
        {
            mapy = 1;
        }
        if (mapx + smaplx > msx + 1)
        {
            mapx = msx - smaplx + 1;
        }
        if (mapy + smaply > msy + 1)
        {
            mapy = msy - smaply + 1;
        }

        //Jump-scroll via minimap clicking
        if ((!LockMouse) && (Lpressed) && (mouseX > minix) && (mouseY > miniy) &&
            (mouseX < minix + (msx / 2)) && (mouseY < miniy + (msy / 2)))
        {
            mapx = (x - minix + MiniX - (smaplx >> (ADDSH + 1))) << ADDSH;
            mapy = (y - miniy + MiniY - (smaply >> (ADDSH + 1))) << ADDSH;

            if (mapx <= 0)
            {
                mapx = 1;
            }
            if (mapy <= 0)
            {
                mapy = 1;
            }

            if (mapx + smaplx > msx + 1)
            {
                mapx = msx - smaplx + 1;
            }
            if (mapy + smaply > msy + 1)
            {
                mapy = msy - smaply + 1;
            }

            if (!FullMini)
            {
                Lpressed = false;
            }
        }

        if (mapx != OLDMX || mapy != OLDMY || Lpressed || Rpressed)
        {
            LastScrollTime = GetTickCount();
        }
    }

    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        curptr = 0;
    }

    //Adjust Lpressed and Rpressed
    MFix();
}
#else
void EDGETEST(){
	int x=mouseX;
	int y=mouseY;
	if(SHOWSLIDE){;
		/*
		if ((x<6)&&(mapx-step>0)) mapx-=step;
		if ((y<6)&&(mapy-step>0)) mapy-=step;
		if ((x>RealLx-29)&&(mapx+smaplx+step<msx)) mapx+=step;
		if ((y>RealLy-6)&&(mapy+smaply+step<msy)) 
			mapy+=step;
		if((!LockMouse)&&(Lpressed)&&(mouseX>minix)&&(mouseY>miniy)&&
			(mouseX<minix+(msx>>1))&&(mouseY<miniy+(msy>>1))){
			mapx=(x-minix+MiniX-(smaplx>>2))<<1;
			mapy=(y-miniy+MiniY-(smaply>>2))<<1;
			if(mapx<=0)mapx=1;
			if(mapy<=0)mapy=1;
			if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
			if(mapy+smaply>msy+1)mapy=msy-smaply+1;
		}
		*/
		bool MoveX=false;
		bool MoveY=false;
		int OLDMX=mapx;
		int OLDMY=mapy;
		if(!(EnterChatMode||RESMODE)){
			OneObject* SOB=NULL;
			if((!DrawGroundMode)&&EditMapMode&&ImNSL[MyNation]&&ImSelm[MyNation][0]!=0xFFFF&&(SOB=Group[ImSelm[MyNation][0]])->NewBuilding){
				int dx=0;
				int dy=0;
				if(GetKeyState(VK_RIGHT)&0x8000){
					dx=16;
				};
				if(GetKeyState(VK_LEFT)&0x8000){
					dx=-16;
				};
				if(GetKeyState(VK_UP)&0x8000){
					dy=-32;
				};
				if(GetKeyState(VK_DOWN)&0x8000){
					dy=32;
				};
				ShiftBuilding(SOB,dx,dy);
			}else{
				if((!DrawGroundMode)&&GetKeyState(VK_RIGHT)&0x8000){
					StepX=4;
					MoveX=true;
				};
				if((!DrawGroundMode)&&GetKeyState(VK_LEFT)&0x8000){
					StepX=-4;
					MoveX=true;
				};
				if((!DrawGroundMode)&&GetKeyState(VK_UP)&0x8000){
					StepY=-4;
					MoveY=true;
				};
				if((!DrawGroundMode)&&GetKeyState(VK_DOWN)&0x8000){
					StepY=4;
					MoveY=true;
				};
			};
		};
		if (x<6){
			if(StepX>0)StepX=0;
			StepX--;
			if(StepX<-ScrollSpeed)StepX=-ScrollSpeed;
			MoveX=true;
		};
		if(y<6){
			if(StepY>0)StepY=0;
			StepY--;
			if(StepY<-ScrollSpeed)StepY=-ScrollSpeed;
			MoveY=true;
		};
		if(x>RealLx-6){
			if(StepX<0)StepX=0;
			StepX++;
			if(StepX>ScrollSpeed)StepX=ScrollSpeed;
			MoveX=true;
		};
		if(y>RealLy-6){
			if(StepY<0)StepY=0;
			StepY++;
			if(StepY>ScrollSpeed)StepY=ScrollSpeed;
			MoveY=true;
		};
		if(StepX&&!MoveX){
			if(StepX>0)StepX--;
			if(StepX<0)StepX++;
		};
		if(StepY&&!MoveY){
			if(StepY>0)StepY--;
			if(StepY<0)StepY++;
		};
		int stepx=StepX;
		int stepy=StepY;
#ifdef _USE3D
		stepx<<=5-Shifter;
		stepy<<=5-Shifter;
#endif
		if(stepx<0)mapx-=(-stepx)>>1;else mapx+=(stepx)>>1;
		if(stepy<0)mapy-=(-stepy)>>1;else mapy+=(stepy)>>1;
		if(mapx<=0)mapx=1;
		if(mapy<=0)mapy=1;
		if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
		if(mapy+smaply>msy+1)mapy=msy-smaply+1;
		if((!LockMouse)&&(Lpressed)&&(mouseX>minix)&&(mouseY>miniy)&&
			(mouseX<minix+(msx>>1))&&(mouseY<miniy+(msy>>1))){
			mapx=(x-minix+MiniX-(smaplx>>(ADDSH+1)))<<ADDSH;
			mapy=(y-miniy+MiniY-(smaply>>(ADDSH+1)))<<ADDSH;
			if(mapx<=0)mapx=1;
			if(mapy<=0)mapy=1;
			if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
			if(mapy+smaply>msy+1)mapy=msy-smaply+1;
			if(!FullMini)Lpressed=false;
		};
#ifdef _USE3D
		DWORD mask=0xFFFFFFF<<(5-Shifter);
		mapx&=mask;
		mapy&=mask;
#endif
		if(mapx!=OLDMX||mapy!=OLDMY||Lpressed||Rpressed)LastScrollTime=GetTickCount();

	};
	if(GetKeyState(
		VK_SHIFT)&0x8000)SetCurPtr(0);
	MFix();
	//if(OldLpressed!=-1)Lpressed=OldLpressed!=0;
PrInfo:;
	
};
#endif
void doooo(){
	Group[0]->Selected=true;
	Nsel=1;
	mapx=20;
	mapy=20;
	//Group[0]->SendTo(32,32,0);
};
void ReRenderMap();
void SetMiniMode(){
	if(MiniMode)return;
	ReRenderMap();
	mapx=mapx-(smaplx>>1);
	mapy=mapy-(smaply>>1);
	if(mapx<1)mapx=1;
	if(mapy<1)mapy=1;
	smaplx=smaplx<<1;
	smaply=smaply<<1;
	MiniMode=true;
	Shifter=4;
	Multip=1;
	if(mapx+smaplx>msx)mapx=msx-smaplx;
	if(mapy+smaply>msy)mapy=msy-smaply;
};
void ClearMiniMode(){
	MiniMode=false;
	ReRenderMap();
	Shifter=5;
	Multip=0;
	mapx+=(smaplx>>2);
	mapy+=(smaply>>2);
	smaplx=(RSCRSizeX-160)>>5;
	smaply=div24(RSCRSizeY);
};
int CurSelPos=0;
void GoToCurSelPosition(){
	if(ImNSL[MyNation]){
		if(CurSelPos>=ImNSL[MyNation])CurSelPos=0;
		int MaxSteps=ImNSL[MyNation];
		OneObject* OBJ=NULL;
		do{
			word MID=ImSelm[MyNation][CurSelPos];
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&OB->Serial==ImSerN[MyNation][CurSelPos])OBJ=OB;
			};
			if(!OBJ){
				CurSelPos++;
				if(CurSelPos>=ImNSL[MyNation])CurSelPos=0;
			};
			MaxSteps--;
		}while(MaxSteps&&OBJ==NULL);
		if(OBJ){
			mapx=(OBJ->RealX>>9)-(smaplx>>1);
			mapy=(OBJ->RealY>>9)-(smaply>>1)-(OBJ->RZ>>4);
			if(mapx<=0)mapx=1;
			if(mapy<=0)mapy=1;
			if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
			if(mapy+smaply>msy+1)mapy=msy-smaply+1;
			int x0=OBJ->RealX;
			int y0=OBJ->RealY;
			OBJ=NULL;
			MaxSteps=ImNSL[MyNation];
			do{
				word MID=ImSelm[MyNation][CurSelPos];
				if(MID!=0xFFFF){
					OneObject* OB=Group[MID];
					if(OB&&OB->Serial==ImSerN[MyNation][CurSelPos]&&Norma(OB->RealX-x0,OB->RealY-y0)>512*16){
						OBJ=OB;
					};
				};
				if(!OBJ){
					CurSelPos++;
					if(CurSelPos>=ImNSL[MyNation])CurSelPos=0;
				};
				MaxSteps--;
			}while(MaxSteps&&OBJ==NULL);
		};
	};
};
#include "Dialogs\UpperMiniMenu.h"
#include "Dialogs\RightUnitInfoPanel.h"
#include "Dialogs\RightInfoPanel.h"
#include "Dialogs\SendResources.h"
//---------------CREATE MAP SHOT----------------//
int mrand();
void SetMPoint(byte* buf,int lx,int ly,int x,int y,byte c){
	if(x<lx&&y<ly&&x>=0&&y>=0){
		buf[x+y*lx]=c;
	};
};
void DrawVLine(byte* buf,int lx,int ly,int x,int y,int z,byte c,byte c1){
	SetMPoint(buf,lx,ly,x,y-z,c);
	//for(int i=0;i<=z;i++)SetMPoint(buf,lx,ly,x,y-i,mrand()&1?c:c1);
};
extern byte TexColors[256];
void SaveBMP8(char* Name,int lx,int ly,byte* Data);
extern byte darkfog[40960];
extern int TEXCOLR[256][64];
int GetLighting(int i);
int GetLightP(int x,int y);
int GetLight(int x,int y);
void CreateMapShot2(){
	int lx=960<<ADDSH;
	int ly=480<<ADDSH;
	int NY=240<<ADDSH;
	byte* buf=(byte*)malloc(lx*ly);
	memset(buf,0xB0,lx*ly);
	int pos=0;
	for(int py=0;py<ly;py++){
		for(int i=0;i<lx;i++){
			//int ppx,ppy;

			int ppx=i>>2;
			int ppy=py>>1;
			int pos=ppx+ppy*VertInLine;
			int tex=TexMap[pos];
			byte c=TEXCOLR[tex][mrand()&63];
			int H=GetHeight(i<<3,py<<4)>>3;
			int Lit=GetLightP(i<<3,py<<4)>>2;
			//int L=Lit>>2;
			//int LL=Lit&3;
			//if(LL>(rand()&3))c=darkfog[16384+c+((L+1)<<8)];
			//else 
			int LL=Lit+(mrand()%1);
			int LL1=Lit+(mrand()%1);
			if(LL<0)LL=0;
			if(LL1<0)LL1=0;
			c=darkfog[16384+c+(LL<<8)];
			byte c1=darkfog[16384+c+(LL1<<8)];
			DrawVLine(buf,lx,ly,i,py,H,c,c1);
		};
	};
	SaveBMP8("shot.bmp",lx,ly,buf);
	free(buf);
};
void GenerateMiniMap();
void CreateMapShot(){
	GenerateMiniMap();
	return;
	int lx=960<<(ADDSH+1);
	int ly=480<<(ADDSH+1);
	int NY=240<<(ADDSH+1);
	byte* buf=(byte*)malloc(lx*ly);
	int pos=0;
	for(int py=0;py<ly;py++){
		for(int i=0;i<lx;i++){
			//int ppx,ppy;

			int ppx=i>>3;
			int ppy=py>>2;
			int pos=ppx+ppy*VertInLine;
			int tex=TexMap[pos];
			byte c=TEXCOLR[tex][mrand()&63];
			int H=GetHeight(i<<2,py<<3)>>2;
			int Lit=GetLightP(i<<2,py<<3)>>2;
			//int L=Lit>>2;
			//int LL=Lit&3;
			//if(LL>(rand()&3))c=darkfog[16384+c+((L+1)<<8)];
			//else
			int LL,LL1;
			if(Lit!=2){
				LL=Lit+(mrand()%3)-1;
				LL1=Lit+(mrand()%3)-1;
			}else{
				LL=Lit;
				LL1=Lit;
			};
			if(LL<0)LL=0;
			if(LL1<0)LL1=0;
			c=darkfog[16384+c+(LL<<8)];
			byte c1=darkfog[16384+c+(LL1<<8)];
			DrawVLine(buf,lx,ly,i,py,H,c,c);
		};
	};
	SaveBMP8("shot.bmp",lx,ly,buf);
	free(buf);
};