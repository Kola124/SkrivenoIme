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
#include <crtdbg.h>
#include "ActiveScenary.h"
#include "DrawForm.h"
#include "Conststr.h"
#include <Process.h>
#include "MPlayer.h"
#include "Recorder.h"
#include "GSINC.H"
#include "TopoGraf.h"
#include "PeerClass.h"
#include "CEngine\goaceng.h"
#include "StrategyResearch.h"
#include "Safety.h"
#include "EinfoClass.h"
#include "mode.h"
#pragma pack(4)
#include "InetRaiting\stats\gstats.h"
#include "InetRaiting\stats\gpersist.h"
#include "InetRaiting\gp.h"
#pragma pack(1)
#include "IR.H"
#include "bmptool.h"
#include "diplomacy.h"
#include "ZBUFFER.h"

#ifdef _USE3D
#include "NewMon3D.h"
#include "..\3DLIB\gMotor\mCamera.h"
#include "Scape3D.h"
#include "..\3DLIB\gMotor\gpText.h"
#endif // _USE3D

#define M4CellLX 32
#define M4CellLY 32

byte* Tmp4XBuf=NULL;
short* TmpOwnerX=NULL;
short* TmpOwnerY=NULL;

int CurTmpPos=0;
int MaxTmpPos=0;

byte** ReadyMap=NULL;
word*  PosMap=NULL;
byte * MiniMap=NULL;
//byte*  ReadyMap=NULL;
int OFSTBL[4096];
int MM_Lx;
int MM_Ly;
int MM_Y0;
int MMSZ=0;
int MMR_Lx;
int MMR_Ly;
//V1--\
//     ----\
//          ===V3
//     ----/
//V2--/
extern short randoma[8192];
int GPLP=0;
#define SETPX1 \
	off=ofs-OFSTBL[64+(H>>8)];\
	c0=MiniMap[off];\
	if(V>170*256)c=TexPtr[tofs];\
	else if(V>85*256)c=trans8[TexPtr[tofs]+(c0<<8)];\
	if(V>85*256){\
		MiniMap[off]=c;\
		//assert(c!=232);\
	};
#define SETPX \
	off=ofs-OFSTBL[64+(H>>8)];\
	if(off>=0&&off<MMSZ){\
		c0=MiniMap[off];\
		if(V>190*256)c=TexPtr[tofs];\
		else if(V>153*256)c=trans4[(int(TexPtr[tofs])<<8)+c0];\
		else if(V>102*256)c=trans8[TexPtr[tofs]+(c0<<8)];\
		else if(V>65*256 )c=trans4[TexPtr[tofs]+(c0<<8)];\
		if(V>51*256)MiniMap[off]=c;\
	};
#define SETLIT \
	off=ofs-OFSTBL[64+(H>>8)];\
	if(off>=0&&off<MMSZ){\
		MiniMap[off]=darkfog[16384+MiniMap[off]+(L&0xFF00)+(randoma[GPLP&8191]&0x100)-256];\
		GPLP++;\
	};
	
/*

#define SETPX \
	off=ofs-OFSTBL[H>>8];\
	c=MiniMap[off];\
	if(V>128)c=TexPtr[tofs];\
	MiniMap[off]=c;//darkfog[16384+c+(L&0xFF00)];\
	assert(V>=-3);
*/

#ifdef _USE3D
/*---------------------------------------------------------------------------*/
/*	Func:	FindRelativeZ	
/*	Desc:	Projects point (px, py) from screen space onto the ground plane
/*			and finds its z-value relatively to the (cx, cy, 0)
/*---------------------------------------------------------------------------*/
float FindRelativeZ( float px, float py, float cx, float cy )
{
	return c_Cos30 * (cy - py); 
}
#endif // _USE3D

int mrand();
extern byte trans4[65536];
extern byte trans8[65536];
extern byte darkfog[40960];
void DrawTriangleR(int x,int y,int H1,int H2,int H3,
				   int L1,int L2,int L3,int V1,int V2,int V3,
				   byte* TexPtr){
	GPLP+=rand();
	//assert(V1==255&&V2==255&V3==255);
	//assert(!(V1>0&&V2>0&&V3<255));
	int ofs=x+(MM_Y0+y)*MM_Lx;
	L1<<=8;
	L2<<=8;
	L3<<=8;
	H1<<=8;
	H2<<=8;
	H3<<=8;
	V1<<=8;
	V2<<=8;
	V3<<=8;

	int Dvx=(V3-((V1+V2)>>1))>>3;
	int Dvy=(V2-V1)>>3;
	int DLx=(L3-((L1+L2)>>1))>>3;
	int DLy=(L2-L1)>>3;
	int DHx=(H3-((H1+H2)>>1))>>3;
	int DHy=(H2-H1)>>3;
	int L=L1;
	int H=H1;
	int V=V1;
	int tofs=(mrand()&15)+((mrand()&15)<<8);

	int off;
	byte c;
	int c0;

	for(int i=0;i<8;i++){
		SETPX;	
		tofs+=256;
		L+=DLy;
		H+=DHy;
		V+=Dvy;
		ofs+=MM_Lx;
	};
	tofs+=1-512;
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	V+=Dvx-Dvy-Dvy;
	ofs+=1-MM_Lx-MM_Lx;
	for(int i=0;i<6;i++){
		SETPX;
		tofs-=256;
		L-=DLy;
		H-=DHy;
		V-=Dvy;
		ofs-=MM_Lx;
	};
	tofs+=1+256;
	L+=DLx+DLy;
	H+=DHx+DHy;
	V+=Dvx+Dvy;
	ofs+=MM_Lx+1;
	for(int i=0;i<6;i++){
		SETPX;	
		tofs+=256;
		L+=DLy;
		H+=DHy;
		V+=Dvy;
		ofs+=MM_Lx;
	};
	tofs+=1-512;
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	V+=Dvx-Dvy-Dvy;
	ofs+=1-MM_Lx-MM_Lx;
	for(int i=0;i<4;i++){
		SETPX;
		tofs-=256;
		L-=DLy;
		H-=DHy;
		V-=Dvy;
		ofs-=MM_Lx;
	};
	tofs+=1+256;
	L+=DLx+DLy;
	H+=DHx+DHy;
	V+=Dvx+Dvy;
	ofs+=MM_Lx+1;
	for(int i=0;i<4;i++){
		SETPX;	
		tofs+=256;
		L+=DLy;
		H+=DHy;
		V+=Dvy;
		ofs+=MM_Lx;
	};
	tofs+=1-512;
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	V+=Dvx-Dvy-Dvy;
	ofs+=1-MM_Lx-MM_Lx;
	for(int i=0;i<2;i++){
		SETPX;
		tofs-=256;
		L-=DLy;
		H-=DHy;
		V-=Dvy;
		ofs-=MM_Lx;
	};
	tofs+=1+256;
	L+=DLx+DLy;
	H+=DHx+DHy;
	V+=Dvx+Dvy;
	ofs+=MM_Lx+1;
	for(int i=0;i<2;i++){
		SETPX;	
		tofs+=256;
		L+=DLy;
		H+=DHy;
		V+=Dvy;
		ofs+=MM_Lx;
	};

};
//            V1
//      .     .
//V3          .
//      .     .
//            V2
void DrawTriangleL(int x,int y,int H1,int H2,int H3,
				   int L1,int L2,int L3,int V1,int V2,int V3,
				   byte* TexPtr){
	GPLP+=rand();
	//assert(V1==255&&V2==255&V3==255);
	int ofs=x+(MM_Y0+y)*MM_Lx;
	L1<<=8;
	L2<<=8;
	L3<<=8;
	H1<<=8;
	H2<<=8;
	H3<<=8;
	V1<<=8;
	V2<<=8;
	V3<<=8;
	int Dvx=(V3-((V1+V2)>>1))>>3;
	int Dvy=(V2-V1)>>3;
	int DLx=(L3-((L1+L2)>>1))>>3;
	int DLy=(L2-L1)>>3;
	int DHx=(H3-((H1+H2)>>1))>>3;
	int DHy=(H2-H1)>>3;
	int L=L1;
	int H=H1;
	int V=V1;
	int tofs=(mrand()&15)+((mrand()&15)<<8);

	int off;
	byte c;
	int c0;
	ofs--;
	V+=Dvx;

	for(int i=0;i<8;i++){
		SETPX;	
		tofs+=256;
		L+=DLy;
		H+=DHy;
		V+=Dvy;
		ofs+=MM_Lx;
	};
	tofs+=1-512;
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	V+=Dvx-Dvy-Dvy;
	ofs+=-1-MM_Lx-MM_Lx;
	for(int i=0;i<6;i++){
		SETPX;
		tofs-=256;
		L-=DLy;
		H-=DHy;
		V-=Dvy;
		ofs-=MM_Lx;
	};
	tofs+=1+256;
	L+=DLx+DLy;
	H+=DHx+DHy;
	V+=Dvx+Dvy;
	ofs+=MM_Lx-1;
	for(int i=0;i<6;i++){
		SETPX;	
		tofs+=256;
		L+=DLy;
		H+=DHy;
		V+=Dvy;
		ofs+=MM_Lx;
	};
	tofs+=1-512;
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	V+=Dvx-Dvy-Dvy;
	ofs+=-1-MM_Lx-MM_Lx;
	for(int i=0;i<4;i++){
		SETPX;
		tofs-=256;
		L-=DLy;
		H-=DHy;
		V-=Dvy;
		ofs-=MM_Lx;
	};
	tofs+=1+256;
	L+=DLx+DLy;
	H+=DHx+DHy;
	V+=Dvx+Dvy;
	ofs+=MM_Lx-1;
	for(int i=0;i<4;i++){
		SETPX;	
		tofs+=256;
		L+=DLy;
		H+=DHy;
		V+=Dvy;
		ofs+=MM_Lx;
	};
	tofs+=1-512;
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	V+=Dvx-Dvy-Dvy;
	ofs+=-1-MM_Lx-MM_Lx;
	for(int i=0;i<2;i++){
		SETPX;
		tofs-=256;
		L-=DLy;
		H-=DHy;
		V-=Dvy;
		ofs-=MM_Lx;
	};
	tofs+=1+256;
	L+=DLx+DLy;
	H+=DHx+DHy;
	V+=Dvx+Dvy;
	ofs+=MM_Lx-1;
	for(int i=0;i<2;i++){
		SETPX;	
		tofs+=256;
		L+=DLy;
		H+=DHy;
		V+=Dvy;
		ofs+=MM_Lx;
	};
};
void LiteTriangleR(int x,int y,int H1,int H2,int H3,
				   int L1,int L2,int L3){
	//assert(V1==255&&V2==255&V3==255);
	//assert(!(V1>0&&V2>0&&V3<255));
	int ofs=x+(MM_Y0+y)*MM_Lx;
	L1<<=8;
	L2<<=8;
	L3<<=8;
	H1<<=8;
	H2<<=8;
	H3<<=8;

	int DLx=(L3-((L1+L2)>>1))>>3;
	int DLy=(L2-L1)>>3;
	int DHx=(H3-((H1+H2)>>1))>>3;
	int DHy=(H2-H1)>>3;
	int L=L1;
	int H=H1;

	int off;
	byte c;
	int c0;

	for(int i=0;i<8;i++){
		SETLIT;	
		L+=DLy;
		H+=DHy;
		ofs+=MM_Lx;
	};
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	ofs+=1-MM_Lx-MM_Lx;
	for(int i=0;i<6;i++){
		SETLIT;	
		L-=DLy;
		H-=DHy;
		ofs-=MM_Lx;
	};
	L+=DLx+DLy;
	H+=DHx+DHy;
	ofs+=MM_Lx+1;
	for(int i=0;i<6;i++){
		SETLIT;	
		L+=DLy;
		H+=DHy;
		ofs+=MM_Lx;
	};
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	ofs+=1-MM_Lx-MM_Lx;
	for(int i=0;i<4;i++){
		SETLIT;	
		L-=DLy;
		H-=DHy;
		ofs-=MM_Lx;
	};
	L+=DLx+DLy;
	H+=DHx+DHy;
	ofs+=MM_Lx+1;
	for(int i=0;i<4;i++){
		SETLIT;	
		L+=DLy;
		H+=DHy;
		ofs+=MM_Lx;
	};
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	ofs+=1-MM_Lx-MM_Lx;
	for(int i=0;i<2;i++){
		SETLIT;	
		L-=DLy;
		H-=DHy;
		ofs-=MM_Lx;
	};
	L+=DLx+DLy;
	H+=DHx+DHy;
	ofs+=MM_Lx+1;
	for(int i=0;i<2;i++){
		SETLIT;	
		L+=DLy;
		H+=DHy;
		ofs+=MM_Lx;
	};

};
//            V1
//      .     .
//V3          .
//      .     .
//            V2
void LiteTriangleL(int x,int y,int H1,int H2,int H3,
				   int L1,int L2,int L3){
	//assert(V1==255&&V2==255&V3==255);
	int ofs=x+(MM_Y0+y)*MM_Lx;
	L1<<=8;
	L2<<=8;
	L3<<=8;
	H1<<=8;
	H2<<=8;
	H3<<=8;
	int DLx=(L3-((L1+L2)>>1))>>3;
	int DLy=(L2-L1)>>3;
	int DHx=(H3-((H1+H2)>>1))>>3;
	int DHy=(H2-H1)>>3;
	int L=L1;
	int H=H1;

	int off;
	byte c;
	int c0;
	ofs--;

	for(int i=0;i<8;i++){
		SETLIT;	
		L+=DLy;
		H+=DHy;
		ofs+=MM_Lx;
	};
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	ofs+=-1-MM_Lx-MM_Lx;
	for(int i=0;i<6;i++){
		SETLIT;	
		L-=DLy;
		H-=DHy;
		ofs-=MM_Lx;
	};
	L+=DLx+DLy;
	H+=DHx+DHy;
	ofs+=MM_Lx-1;
	for(int i=0;i<6;i++){
		SETLIT;	
		L+=DLy;
		H+=DHy;
		ofs+=MM_Lx;
	};
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	ofs+=-1-MM_Lx-MM_Lx;
	for(int i=0;i<4;i++){
		SETLIT;	
		L-=DLy;
		H-=DHy;
		ofs-=MM_Lx;
	};
	L+=DLx+DLy;
	H+=DHx+DHy;
	ofs+=MM_Lx-1;
	for(int i=0;i<4;i++){
		SETLIT;	
		L+=DLy;
		H+=DHy;
		ofs+=MM_Lx;
	};
	L+=DLx-DLy-DLy;
	H+=DHx-DHy-DHy;
	ofs+=-1-MM_Lx-MM_Lx;
	for(int i=0;i<2;i++){
		SETLIT;	
		L-=DLy;
		H-=DHy;
		ofs-=MM_Lx;
	};
	L+=DLx+DLy;
	H+=DHx+DHy;
	ofs+=MM_Lx-1;
	for(int i=0;i<2;i++){
		SETLIT;	
		L+=DLy;
		H+=DHy;
		ofs+=MM_Lx;
	};
};
#define VMAX1 (255-(randoma[X1&8191]&127))
#define VMAX2 (255-(randoma[X2&8191]&127))
#define VMAX3 (255-(randoma[X3&8191]&127))
#define VMIN1 (randoma[X1&8191]&127)
#define VMIN2 (randoma[X2&8191]&127)
#define VMIN3 (randoma[X3&8191]&127)
extern byte *tex1;
int GetBmOfst(int i);
void DrawTriR(int x,int y,
			  int H1,int H2,int H3,
			  int L1,int L2,int L3,
			  int T1,int T2,int T3,
			  int X1,int X2,int X3){
	if(T1<=T2&&T1<=T3){
		DrawTriangleR(x,y,H1,H2,H3,L1,L2,L3,255,255,255,tex1+GetBmOfst(T1));
		if(T1!=T2)DrawTriangleR(x,y,H1,H2,H3,L1,L2,L3,VMIN1,VMAX2,T2==T3?VMAX3:VMIN3,tex1+GetBmOfst(T2));
		if(T1!=T3&&T2!=T3)DrawTriangleR(x,y,H1,H2,H3,L1,L2,L3,VMIN1,VMIN2,VMAX3,tex1+GetBmOfst(T3));
	}else
	if(T2<=T3&&T2<=T1){
		DrawTriangleR(x,y,H1,H2,H3,L1,L2,L3,255,255,255,tex1+GetBmOfst(T2));
		if(T2!=T1)DrawTriangleR(x,y,H1,H2,H3,L1,L2,L3,VMAX1,VMIN2,T1==T3?VMAX3:VMIN3,tex1+GetBmOfst(T1));
		if(T2!=T3&&T1!=T3)DrawTriangleR(x,y,H1,H2,H3,L1,L2,L3,VMIN1,VMIN2,VMAX3,tex1+GetBmOfst(T3));
	}else
	if(T3<=T1&&T3<=T2){
		DrawTriangleR(x,y,H1,H2,H3,L1,L2,L3,255,255,255,tex1+GetBmOfst(T3));
		if(T3!=T2)DrawTriangleR(x,y,H1,H2,H3,L1,L2,L3,T1==T2?VMAX1:VMIN1,VMAX2,VMIN3,tex1+GetBmOfst(T2));
		if(T1!=T3&&T1!=T2)DrawTriangleR(x,y,H1,H2,H3,L1,L2,L3,VMAX1,VMIN2,VMIN3,tex1+GetBmOfst(T1));
	};
	LiteTriangleR(x,y,H1,H2,H3,L1,L2,L3);
};
void DrawTriL(int x,int y,
			  int H1,int H2,int H3,
			  int L1,int L2,int L3,
			  int T1,int T2,int T3,
			  int X1,int X2,int X3){
	if(T1<=T2&&T1<=T3){
		DrawTriangleL(x,y,H1,H2,H3,L1,L2,L3,255,255,255,tex1+GetBmOfst(T1));
		if(T1!=T2)DrawTriangleL(x,y,H1,H2,H3,L1,L2,L3,VMIN1,VMAX2,T2==T3?VMAX3:VMIN3,tex1+GetBmOfst(T2));
		if(T1!=T3&&T2!=T3)DrawTriangleL(x,y,H1,H2,H3,L1,L2,L3,VMIN1,VMIN2,VMAX3,tex1+GetBmOfst(T3));
	}else
	if(T2<=T3&&T2<=T1){
		DrawTriangleL(x,y,H1,H2,H3,L1,L2,L3,255,255,255,tex1+GetBmOfst(T2));
		if(T2!=T1)DrawTriangleL(x,y,H1,H2,H3,L1,L2,L3,VMAX1,VMIN2,T1==T3?VMAX3:VMIN3,tex1+GetBmOfst(T1));
		if(T2!=T3&&T1!=T3)DrawTriangleL(x,y,H1,H2,H3,L1,L2,L3,VMIN1,VMIN2,VMAX3,tex1+GetBmOfst(T3));
	}else
	if(T3<=T1&&T3<=T2){
		DrawTriangleL(x,y,H1,H2,H3,L1,L2,L3,255,255,255,tex1+GetBmOfst(T3));
		if(T3!=T2)DrawTriangleL(x,y,H1,H2,H3,L1,L2,L3,T1==T2?VMAX1:VMIN1,VMAX2,VMIN3,tex1+GetBmOfst(T2));
		if(T1!=T3&&T1!=T2)DrawTriangleL(x,y,H1,H2,H3,L1,L2,L3,VMAX1,VMIN2,VMIN3,tex1+GetBmOfst(T1));
	};
	LiteTriangleL(x,y,H1,H2,H3,L1,L2,L3);
};
void SaveBMP8(char* Name,int lx,int ly,byte* Data);
void DrawMiniMaskOnScreen(byte* Buf,int x,int y,int cx,int cy,int BufWidth);
void DrawMiniSubWaterSquare(byte* Buf,int x,int y,int cx,int cy,int BufWidth);
extern int MAXSPR;
struct OneSprInfo{
	short x,y,file,spr;
};
#define MAXLY 3000
class SmallZBuffer{
public:
	int MaxLines;
	OneSprInfo* SPRS[MAXLY];
	int NSprs[MAXLY];
	int MaxSprs[MAXLY];
	SmallZBuffer(int Ny);
	~SmallZBuffer();
	void Add(int y,int sx,int sy,int FID,int spr);
	void Draw();
};
SmallZBuffer::SmallZBuffer(int Ny){
	//memset(this,0,sizeof* this);
	//SPRS=(OneSprInfo**)malloc(4*Ny);
	memset(SPRS,0,4*Ny);
	//NSprs=(int*)malloc(4*Ny);
	memset(NSprs,0,4*Ny);
	//MaxSprs=(int*)malloc(4*Ny);
	memset(MaxSprs,0,4*Ny);
	MaxLines=Ny;
};
SmallZBuffer::~SmallZBuffer(){
	for(int i=0;i<MaxLines;i++){
		if(SPRS[i])free(SPRS[i]);
		SPRS[i]=NULL;
	};
	//if(SPRS)free(SPRS);
	//if(NSprs)free(NSprs);
	//if(MaxSprs)free(MaxSprs);
};
void SmallZBuffer::Add(int y,int sx,int sy,int FID,int spr){
	if(y>=0&&y<MaxLines){
		if(NSprs[y]>=MaxSprs[y]){
			MaxSprs[y]+=4;
			SPRS[y]=(OneSprInfo*)realloc(SPRS[y],MaxSprs[y]*sizeof OneSprInfo);
		};
		OneSprInfo* OSI=SPRS[y]+NSprs[y];
		NSprs[y]++;
		OSI->file=FID;
		OSI->spr=spr;
		OSI->x=sx;
		OSI->y=sy;
	};
};
int NDR=0;
void SmallZBuffer::Draw(){
	NDR=0;
	for(int i=0;i<MaxLines;i++){
		int N=NSprs[i];
		if(N){
			OneSprInfo* OSI=SPRS[i];
			for(int j=0;j<N;j++){
				GPS.ShowGP(OSI[j].x,OSI[j].y,OSI[j].file,OSI[j].spr,0);
				NDR++;
			};
		};
	};
};
int RSIZE;
void GenerateMiniMap(){
//#ifdef _USE3D
//	return;
//#endif // _USE3D

	TempWindow TW;
	PushWindow(&TW);
	void* sptr=ScreenPtr;
	MM_Lx=(1920<<ADDSH)+2;
	MM_Ly=(MM_Lx>>1)+2;
	MM_Y0=64;
	MMR_Lx=(MM_Lx/M4CellLX)+1;
	MMR_Ly=(MM_Ly/M4CellLY)+1;
	if(!Tmp4XBuf){
		MaxTmpPos=8000000/(M4CellLX*M4CellLY);
		Tmp4XBuf=(byte*)malloc(MaxTmpPos*M4CellLX*M4CellLY);
		TmpOwnerX=(short*)malloc(MaxTmpPos*2);
		TmpOwnerY=(short*)malloc(MaxTmpPos*2);
	};
	if(!ReadyMap){
		ReadyMap=(byte**)malloc(MMR_Lx*MMR_Ly*4);
		PosMap=(word*)malloc(MMR_Lx*MMR_Ly*2);
	};
	memset(ReadyMap,0,MMR_Lx*MMR_Ly*4);
	memset(PosMap,0xFF,MMR_Lx*MMR_Ly*2);
	memset(TmpOwnerX,0xFF,MaxTmpPos*2);
	memset(TmpOwnerY,0xFF,MaxTmpPos*2);
	CurTmpPos=0;
	MMSZ=MM_Lx*(MM_Ly*2+MM_Y0*2);
	RSIZE=MMR_Lx*MMR_Ly;
	return;
};

byte* ReserveMemoryFor4X(word x,word y){
	if(TmpOwnerX[CurTmpPos]!=-1){
		int ofs=int(TmpOwnerX[CurTmpPos])+int(TmpOwnerY[CurTmpPos])*MMR_Lx;
		//assert(ofs>=0&&ofs<RSIZE);
		ReadyMap[ofs]=NULL;
		PosMap[ofs]=0xFFFF;
	};
	TmpOwnerX[CurTmpPos]=x;
	TmpOwnerY[CurTmpPos]=y;
	byte* mem=Tmp4XBuf+CurTmpPos*M4CellLX*M4CellLY;
	int ofs=int(x)+int(y)*MMR_Lx;
	ReadyMap[ofs]=mem;
	PosMap[ofs]=CurTmpPos;
	CurTmpPos++;
	if(CurTmpPos>=MaxTmpPos)CurTmpPos=0;
	return mem;
};

bool LMode=0;
extern int RealLx;
extern int RealLy;
void PrepareSound();
#ifdef _USE3D
void MakeAllDirtyGBUF();
void ReverseLMode(){
	if(!LMode){
		int dx=mapx+(mouseX>>5);
		int dy=mapy+(mouseY>>4);
		if(GetKeyState(VK_CONTROL)&0x8000){
			Shifter=4;
		}else
		if(GetKeyState(VK_SHIFT)&0x8000){
			Shifter=2;
		}else Shifter=3;
		LMode=1;
		//if(!ReadyMap)GenerateMiniMap();
		smaplx<<=(5-Shifter);
		smaply<<=(5-Shifter);
		smapy=RealLy-(smaply*(16>>(5-Shifter)));

		mapx=dx-(smaplx>>1);
		mapy=dy-(smaply>>1);
		if(mapx<=0)mapx=1;
		if(mapy<=0)mapy=1;
		if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
		if(mapy+smaply>msy+1)mapy=msy-smaply+1;
		SetCursorPos(RealLx>>1,RealLy>>1);
		MakeAllDirtyGBUF();
	}else{
		int dx=mapx+(mouseX>>(Shifter));
		int dy=mapy+(mouseY>>(Shifter-1));
		LMode=0;
		smaplx>>=(5-Shifter);
		smaply>>=(5-Shifter);
		smapy=RealLy-smaply*16;
		Shifter=5;
		mapx=dx-(smaplx>>1);
		mapy=dy-(smaply>>1);
		if(mapx<=0)mapx=1;
		if(mapy<=0)mapy=1;
		if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
		if(mapy+smaply>msy+1)mapy=msy-smaply+1;
		SetCursorPos(RealLx>>1,RealLy>>1);
		MakeAllDirtyGBUF();
	};
	mapx=mapx&0xFFFC;
	mapy=mapy&0xFFFC;
	PrepareSound();
};
#else
void ReverseLMode(){
	if(!LMode){
		int dx=mapx+(mouseX>>5);
		int dy=mapy+(mouseY>>4);

		Shifter=3;
		LMode=1;
		if(!ReadyMap)GenerateMiniMap();
		smaplx<<=2;
		smaply<<=2;
		smapy=RealLy-smaply*4;

		mapx=dx-(smaplx>>1);
		mapy=dy-(smaply>>1);
		if(mapx<=0)mapx=1;
		if(mapy<=0)mapy=1;
		if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
		if(mapy+smaply>msy+1)mapy=msy-smaply+1;
		SetCursorPos(RealLx>>1,RealLy>>1);
	}else{
		int dx=mapx+(mouseX>>3);
		int dy=mapy+(mouseY>>2);

		Shifter=5;
		LMode=0;
		smaplx>>=2;
		smaply>>=2;
		smapy=RealLy-smaply*16;
		
		mapx=dx-(smaplx>>1);
		mapy=dy-(smaply>>1);
		if(mapx<=0)mapx=1;
		if(mapy<=0)mapy=1;
		if(mapx+smaplx>msx+1)mapx=msx-smaplx+1;
		if(mapy+smaply>msy+1)mapy=msy-smaply+1;
		SetCursorPos(RealLx>>1,RealLy>>1);
	};
	mapx=mapx&0xFFFC;
	mapy=mapy&0xFFFC;
	PrepareSound();
};
#endif
extern int RealLx;
extern int RealLy;
int DD=0;
void CheckMM_Change();
void CheckMM4XReady(int x,int y,int Lx,int Ly);
void CopyTo16(int x,int y,byte* Src,int Pitch,int Lx,int Ly);
void DrawOneTerrSquare(byte* offs,int x,int y){
//#ifdef _USE3D
//	return;
//#endif // _USE3D
#ifdef _USE3D
	if (GPS.GetClipArea().IsRectInside( x, y, M4CellLX, M4CellLY ))
	{
		CopyTo16(x,y,offs,M4CellLX,M4CellLX,M4CellLY);
#else
	if(x>=WindX&&y>=WindY&&x+M4CellLX<=WindX1&&y+M4CellLY<=WindY1){
		int mofs=int(ScreenPtr)+x+y*ScrWidth;
		int sdx=ScrWidth-M4CellLX;
		__asm{
			push esi
			push edi
			push ecx
			pushf
			cld
			mov  esi,offs
			mov  edi,mofs
			mov  ebx,M4CellLY
LLP1:		mov  ecx,M4CellLX/4
			rep  movsd
			add  edi,sdx
			dec  ebx
			jnz  LLP1
			popf
			pop  ecx
			pop  edi
			pop  esi
		};
#endif
		return;
	};

#ifndef _USE3D
	if(x+M4CellLX<=WindX||y+M4CellLY<=WindY||x>=WindX1||y>=WindY1)return;
	int txX=0;
	int txY=0;
	int txLx=M4CellLX;
	int txLy=M4CellLY;
	if(x<WindX){
		txX=WindX-x;
		txLx-=txX;
		x=0;
	};
	if(y<WindY){
		txY=WindY-y;
		txLy-=txY;
		y=0;
	};
	if(x+txLx>WindX1){
		txLx=WindX1+1-x;
	};
	if(y+txLy>WindY1){
		txLy=WindY1+1-y;
	};
	if(txX<0||txY<0||txLx<=0||txLy<=0)return;
#else
	const ViewPort& vp = GPS.GetClipArea();
	if (!vp.IsRectInside( x, y, M4CellLX, M4CellLY )) return;
	int txX=0;
	int txY=0;
	int txLx=M4CellLX;
	int txLy=M4CellLY;
	if(x<vp.x){
		txX=vp.x-x;
		txLx-=txX;
		x=0;
	};
	if(y<vp.y){
		txY=vp.y-y;
		txLy-=txY;
		y=0;
	};
	if(x+txLx>vp.GetRight()){
		txLx=vp.GetRight()+1-x;
	};
	if(y+txLy>vp.GetBottom()){
		txLy=vp.GetBottom()+1-y;
	};
	if(txX<0||txY<0||txLx<=0||txLy<=0)return;
#endif // !_USE3D
	
	int toff=int(offs)+txX+txY*M4CellLX;
#ifdef _USE3D
	CopyTo16(x,y,(byte*)toff,M4CellLX,txLx,M4CellLY);
#else
	int mofs=int(ScreenPtr)+x+y*ScrWidth;
	int sdx=ScrWidth-txLx;
	int adm=M4CellLX-txLx;
	__asm{
		push esi
		push edi
		push ecx
		pushf
		cld
		mov  esi,toff
		mov  edi,mofs
		mov  ebx,txLy
LLP2:	mov  ecx,txLx
		shr  ecx,2
		rep  movsd
		add  esi,adm
		add  edi,sdx
		dec  ebx
		jnz  LLP2
		popf
		pop  ecx
		pop  edi
		pop  esi
	};
#endif
	return;
};

void DrawLGround(){
	CheckMM_Change();
	//if(GetKeyState('Q')&0x8000)DD+=4;
	//if(GetKeyState('W')&0x8000)DD-=4;
	int x0=mapx<<3;
	int y0=mapy<<2;
	int Lx=RealLx-DD;
	int Ly=RealLy;
	CheckMM4XReady(x0,y0,Lx,Ly);
	int cx0=((mapx<<3)/M4CellLX)-1;
	int cy0=((mapy<<2)/M4CellLY)-1;
	int cx1=(((mapx+smaplx)<<3)/M4CellLX)+1;
	int cy1=(((mapy+smaply)<<2)/M4CellLY)+1;
	if(cx0<0)cx0=0;
	if(cy0<0)cy0=0;
	if(cx1>=MMR_Lx)cx1=MMR_Lx-1;
	if(cy1>=MMR_Ly)cy1=MMR_Ly-1;
	for(int ix=cx0;ix<=cx1;ix++){
		for(int iy=cy0;iy<=cy1;iy++){
			int ofs=ix+iy*MMR_Lx;
			if(ReadyMap[ofs])DrawOneTerrSquare(ReadyMap[ofs],ix*M4CellLX-x0,iy*M4CellLY-y0);
		};
	};
};
#define zoomsh SCSHIFT
int CLM_Size=8;
int CLM_Shift=2;
int CLM_ShiftY=2;

extern int tmtmt;
#define zoom(x) (x>>zoomsh)
extern int PortBuiX,PortBuiY;
extern bool NOPAUSE;
void ShowFires(OneObject* OB,int x0,int y0);
void DRAW_WAVES();
void PROSESS_WAVES();
void ShowProducedShip(OneObject* Port,int CX,int CY);

#ifdef _USE3D
int zoomex( int val )
{
	if (val > 0)
	{
		return val >> (5 - Shifter);
	}
	else
	{
		return -((-val) >> (5 - Shifter));
	}
}

int unzoomex( int val )
{
	if (val > 0)
	{
		return val << (5 - Shifter);
	}
	else
	{
		return -((-val) << (5 - Shifter));
	}
}
#endif // _USE3D
void AddPointEx(short XL,short YL,short x,short y,OneObject* OB,word FileID,word SpriteID,word FileIDex,word SpriteIDex,int Param1,int Param2);
void SetZBias(int b);
void MiniShowNewMonsters(){

#ifdef _USE3D
	//  sprites on the map are drawn with enabled z-buffer
	GPS.EnableZBuffer( true );
	GPS.SetScale( 1.0f / float( 1 << (5-Shifter) ) );
								
	void Clean_GP_IMG();
	Clean_GP_IMG();
#endif // _USE3D

	int SCSHIFT=5-Shifter;
	CLM_Shift=SCSHIFT;
	CLM_ShiftY=SCSHIFT;
	PROSESS_WAVES();
	DRAW_WAVES();
	ClearZBuffer();
	int x0=mapx<<(5-CLM_Shift);
	int y0=(mapy<<(4-CLM_Shift));
	int Lx1=smaplx<<(5-CLM_Shift);
	int Ly1=smaply<<(4-CLM_Shift);
	int xx,yy;
	int mpdy=mapy<<(4-CLM_Shift);
	int dxx=mapx<<(5-CLM_Shift);
	int dyy=mapy<<(4-CLM_Shift);
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB){
			if(OB->NewMonst&&!OB->Hidden){
				xx=(OB->RealX>>(4+CLM_Shift))-x0;
				yy=(OB->RealY>>(5+CLM_Shift))-y0;
				int zz=yy;
				//if(OB->LockType==1){
				//	int tmm=tmtmt&63;
				//	if(tmm>31)zz++;
				//};
				zz-=zoom(OB->RZ+int(OB->OverEarth));
				if(xx>-32&&zz>-32&&xx<Lx1+32&&zz<Ly1+32){
					NewAnimation* NAM=OB->NewAnm;
					int csp=OB->NewCurSprite;
					NewMonster* NM=OB->newMons;
					if(NAM&&NAM->Enabled&&csp<NAM->NFrames){
#ifdef _USE3D
						if (NM->Monster3D)
						{
							if (OB->AnimFrame3D < NAM->firstFrame || 
								OB->AnimFrame3D > NAM->lastFrame)
							{
								OB->AnimFrame3D = NAM->firstFrame;
							} 

							NM->Monster3D->ProgressAnim( OB->AnimFrame3D );	
							NM->Monster3D->Draw( OB );
							OB->AnimFrame3D++;
						}
#endif // _USE3D
						PlayAnimation(NAM,csp,xx+x0,yy+y0);
						int octs=(NAM->Rotations-1)*2;
						if(!octs){
							NewFrame* NF=&NAM->Frames[csp];
							AddPoint(xx,yy,xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,NF->SpriteID,0,0);
							//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
						}else if(NM->FullRotation){
							//int octs,oc2,sesize,oc1,ocM;
							int sesize=256/NAM->Rotations;
							int sz2=sesize>>1;
							int spr=OB->RealDir-64+sz2;
							if(spr<0)spr+=256;
							spr/=sesize;
							if(spr<0)spr+=NAM->Rotations;
							NewFrame* NF=&NAM->Frames[csp];
							int cspr=spr+NAM->Rotations*NF->SpriteID;
							NewMonster* NM=OB->newMons;
							bool Downpart=false;
							NewAnimation* UpAnm=NULL;
							NewFrame* UNF=NULL;
							if(NM->TwoParts&&(NAM==&NM->Stand||NAM==&NM->MotionL||NAM==&NM->MotionR)){
								Downpart=true;
								UpAnm=NM->UpperPart;
								if(UpAnm){
									UNF=&UpAnm->Frames[csp];
								};
							};
							if(OB->Sdoxlo&&!NAM->NFrames)zz-=256;
							if(OB->Sdoxlo>100){
								AddOptPoint(ZBF_LO,xx,yy-256,xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,cspr,AV_TRANSPARENT);
							}else{
								if(Downpart){
									AddLoPoint(xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,cspr,0,0);
									//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,cspr,NM->WaterCheckDist1,NM->WaterCheckDist2);
									if(UpAnm)AddPoint(xx,yy+zoom(NM->UpperPartShift),xx+zoom(UNF->dx),zz+zoom(UNF->dy),OB,UNF->FileID,cspr,0,0);
								}else{
									if(NM->LinearLength){
										int LL=NM->LinearLength>>1;
										int dx=(LL*TCos[OB->RealDir])>>8;
										int dy=(LL*TSin[OB->RealDir])>>9;
										AddLine(xx-zoom(dx),yy-zoom(dy),xx+zoom(dx),yy+zoom(dy),xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,cspr,0,0);
										//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,cspr,NM->WaterCheckDist1,NM->WaterCheckDist2);
									}else{
										AddPoint(xx,yy,xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,cspr,0,0);
										//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,cspr,NM->WaterCheckDist1,NM->WaterCheckDist2);
									};
								};
								if(NM->FLAGS&&!OB->Sdoxlo){
									int N=NM->FLAGS->N;
									int xc=NM->FLAGS->Xr;
									int yc=-OB->NewAnm->Frames->dy;
									short* pt=NM->FLAGS->Points;
									int SIN=TSin[OB->RealDir];
									int COS=TCos[OB->RealDir];
									for(int i=0;i<N;i++){
										int i3=i<<2;
										int dx=xc-pt[i3+1];
										int dy=xc-pt[i3];
										int dz=-pt[i3+2]+yc+(dy>>1);
										int dx1=((dx*COS-dy*SIN)>>8);		
										int dy1=((dx*SIN+dy*COS)>>9);
										AddPoint(xx+zoom(dx1),yy+zoom(dy1+pt[i3+3]),xx+zoom(dx1-48),yy+zoom(dy1-dz-28),OB,2,(tmtmt+19*i)%52,0,0);
									};
								};
							};
						}else{
							int octs,oc2,sesize,oc1,ocM;
							if(NAM->Rotations&1){
								octs=(NAM->Rotations-1)*2;
								oc2=NAM->Rotations-1;
								if(!octs)octs=1;
								sesize=div(255,octs<<1).quot;
								oc1=octs;
								ocM=oc2;
							}else{
								octs=NAM->Rotations*2;
								oc2=NAM->Rotations-1;
								ocM=oc2+1;
								if(!octs)octs=1;
								sesize=0;
								oc1=octs-1;
							};
							
							//int oc2=NAM->Rotations-1;
							//if(!octs)octs=1;
							//int sesize=div(255,octs<<1).quot;
							byte dir=(((OB->RealDir+64+sesize)&255)*octs)>>8;
							byte dir2=dir;
							
							NewFrame* NF=&NAM->Frames[csp];
							NewMonster* NM=OB->newMons;
							bool Downpart=false;
							NewAnimation* UpAnm=NULL;
							NewFrame* UNF=NULL;
							if(NM->TwoParts&&(NAM==&NM->Stand||NAM==&NM->MotionL||NAM==&NM->MotionR)){
								Downpart=true;
								UpAnm=NM->UpperPart;
								if(UpAnm){
									UNF=&UpAnm->Frames[csp];
								};
							};
							if(OB->Sdoxlo&&!NAM->NFrames)zz-=256;
#ifdef _USE3D
							if(false){
								if(dir<ocM)
									AddPoint(xx,yy,xx-zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0,0);
								else{
									dir=oc1-dir;
									AddPoint(xx,yy,xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0,0);
								};
#else
							if(OB->Sdoxlo>100){
								if(dir<ocM)
									AddOptPoint(ZBF_LO,xx,yy-zoom(256),xx-zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,AV_TRANSPARENT);
								else{
									dir=oc1-dir;
									AddOptPoint(ZBF_LO,xx,yy-zoom(256),xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,AV_TRANSPARENT);
								};
#endif
							}else{
								if(dir<ocM){
									if(Downpart){
										AddLoPoint(xx-zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0,0);
										//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
										if(UpAnm)AddPoint(xx,yy+zoom(NM->UpperPartShift),xx-zoom(UNF->dx),zz+zoom(UNF->dy),OB,UNF->FileID,oc2-dir+NAM->Rotations*UNF->SpriteID+4096,0,0);
									}else{
										if(NM->LinearLength){
											int LL=NM->LinearLength>>1;
											int dx=(LL*TCos[OB->RealDir])>>8;
											int dy=(LL*TSin[OB->RealDir])>>9;
											AddLine(xx-zoom(dx),yy-zoom(dy),xx+zoom(dx),yy+zoom(dy),xx-zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0,0);
											//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
										}else{
											if(NAM->DoubleAnm){
												AddPoint(xx,yy,xx-zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0,0);
												SetZBias(1);
												AddPoint(xx,yy,xx-zoom(NF->dx),zz+zoom(NF->dy),OB,NF[NAM->NFrames].FileID,oc2-dir+NAM->Rotations*NF[NAM->NFrames].SpriteID+4096,0,0);
												SetZBias(0);
											}else AddPoint(xx,yy,xx-zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0,0);
											//AddPoint(xx,yy,xx-zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0,0);
											//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
										};
									};
								}else{
									dir=oc1-dir;
									if(Downpart){
										AddLoPoint(xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0,0);
										//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
										if(UpAnm)AddPoint(xx,yy+zoom(NM->UpperPartShift),xx+zoom(UNF->dx),zz+zoom(UNF->dy),OB,UNF->FileID,oc2-dir+NAM->Rotations*UNF->SpriteID,0,0);
									}else{
										if(NM->LinearLength){
											int LL=NM->LinearLength>>1;
											int dx=(LL*TCos[OB->RealDir])>>8;
											int dy=(LL*TSin[OB->RealDir])>>9;
											AddLine(xx-zoom(dx),yy-zoom(dy),xx+zoom(dx),yy+zoom(dy),xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0,0);
											//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
										}else{
											if(NAM->DoubleAnm){
												AddPoint(xx,yy,xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0,0);
												SetZBias(1);
												AddPoint(xx,yy,xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF[NAM->NFrames].FileID,oc2-dir+NAM->Rotations*NF[NAM->NFrames].SpriteID,0,0);
												SetZBias(0);
											}else AddPoint(xx,yy,xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0,0);
											//AddPoint(xx,yy,xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0,0);
											//AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
										};
									};
								};
								/*
								if(NM->FLAGS&&!OB->Sdoxlo){
									int N=NM->FLAGS->N;
									int xc=NM->FLAGS->Xr;
									int yc=-OB->NewAnm->Frames->dy;
									short* pt=NM->FLAGS->Points;
									int SIN=TSin[OB->RealDir];
									int COS=TCos[OB->RealDir];
									for(int i=0;i<N;i++){
										int i3=i<<2;
										int dx=xc-pt[i3+1];
										int dy=xc-pt[i3];
										int dz=-pt[i3+2]+yc+(dy>>1);
										int dx1=((dx*COS-dy*SIN)>>8);		
										int dy1=((dx*SIN+dy*COS)>>9);
										AddPoint(xx+zoom(dx1),yy+zoom(dy1+pt[i3+3]),xx+zoom(dx1-48),yy+zoom(dy1-dz-28),OB,2,(tmtmt+19*i)%52,0,0);
									};
								};
								if(NM->Reflection){
									csp=0;
									NewAnimation* NAM=NM->Reflection;
									if(OB->Sdoxlo){
										if(OB->NewAnm==&NM->Death)NAM++;
										else if(OB->NewAnm==&NM->DeathLie1)NAM+=2;
										else NAM=NULL;
									};
									if(NAM&&NAM->Rotations!=1){
										int Rocts,Roc2,Rsesize,Roc1,RocM;
										if(NAM->Rotations&1){
											Rocts=(NAM->Rotations-1)*2;
											Roc2=NAM->Rotations-1;
											if(!Rocts)Rocts=1;
											Rsesize=div(255,Rocts<<1).quot;
											Roc1=Rocts;
											RocM=Roc2;
										}else{
											Rocts=NAM->Rotations*2;
											Roc2=NAM->Rotations-1;
											RocM=Roc2+1;
											if(!Rocts)Rocts=1;
											Rsesize=0;
											Roc1=Rocts-1;
										};
										byte Rdir=(((OB->RealDir+64+Rsesize)&255)*Rocts)>>8;
										NewFrame* NF=&NAM->Frames[csp];
										if(Rdir<RocM)
											AddOptPoint(ZBF_LO,0,0,xx-zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,Roc2-Rdir+NAM->Rotations*NF->SpriteID+4096,256+512);
										else{
											Rdir=Roc1-Rdir;
											AddOptPoint(ZBF_LO,0,0,xx+zoom(NF->dx),zz+zoom(NF->dy),OB,NF->FileID,Roc2-Rdir+NAM->Rotations*NF->SpriteID,256+512);
										};
									};
								};
								*/
							};
							if(NM->NVesel&&!OB->Sdoxlo){
								int Dang=(OB->/*GraphDir*/RealDir+128)&255;
								int SIN=TSin[Dang];
								int COS=TCos[Dang];
								NewAnimation* VES=NM->Veslo;
								int cspL=OB->LeftVeslo&127;
								int cspR=OB->RightVeslo&127;
								int LDir=(int(OB->RealDir)+128)&255;
								int RDir=(int(-OB->RealDir)+256)&255;
								NewFrame* VNFL=&VES->Frames[cspL];
								NewFrame* VNFR=&VES->Frames[cspR];
								int ROTS=VES->Rotations;
								if(VES){
									for(int i=0;i<NM->NVesel;i++){
										int vx=NM->VesloX[i];
										int vy=NM->VesloY[i];
										int dx=((vx*COS-vy*SIN)>>8);
										int dy=((vx*SIN+vy*COS)>>9);
										int dx1=((vx*COS+vy*SIN)>>8);
										int dy1=((vx*SIN-vy*COS)>>9);
										if(LDir>=0){
											AddPoint(xx+zoom(dx),yy+zoom(dy),xx+zoom(dx+VNFL->dx),zz+zoom(dy+VNFL->dy),NULL,VNFL->FileID,((LDir*ROTS)>>8)+VNFL->SpriteID*ROTS,0,0);
										}else{
											//AddPoint(xx+dx,yy+dy,(xx+dx-VNFL->dx),(zz+dy+VNFL->dy),NULL,VNFL->FileID,((LDir*ROTS)>>8)+VNFL->SpriteID*ROTS+4096,0,0);
										};
										if(RDir>=0){
											AddPoint(xx+zoom(dx1),yy+zoom(dy1),xx+zoom(dx1-VNFR->dx),zz+zoom(dy1+VNFL->dy),NULL,VNFL->FileID,((RDir*ROTS)>>8)+VNFR->SpriteID*ROTS+4096,0,0);
										}else{
											//AddPoint(xx+dx1,yy+dy1,(xx+dx1-VNFR->dx),(zz+dy1+VNFL->dy),NULL,VNFL->FileID,((-RDir*ROTS)>>7)+VNFR->SpriteID*ROTS+4096,0,0);
										};
									};
									
								};
							};
						};
					};
				};
			}else if(OB->NewBuilding&&!OB->Hidden){
				xx=(OB->RealX>>(4+zoomsh))-x0;
				yy=(OB->RealY>>(5+zoomsh))-y0;
				int zz=yy;
				if(Mode3D)zz-=zoom(OB->RZ);
				NewMonster* NM=OB->newMons;
				//assert(NM);
				if(NM){
					//if(NM->BReflection&&NM->Fist.Enabled){
					//	AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,NM->Fist.Frames[0].SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
					//};
					int xx0=xx+zoom(NM->PicDx);
					int yy0=zz+zoom(NM->PicDy);
					int xx1=xx0+zoom(NM->PicLx)-1;
					int yy1=yy0+zoom(NM->PicLy)-1;
					int CSP;
					//if(NM->Reflection){
						//NewFrame* NF=NM->Reflection->Frames;
						//AddOptPoint(ZBF_LO,0,0,xx0,yy0,NULL,NF->FileID,NF->SpriteID,256+512);
					//};
					if(NM->FLAGS&&OB->LoLayer==&NM->StandLo){
						int xre=(OB->RealX>>(4+zoomsh))-x0;
						int yre=(OB->RealY>>(5+zoomsh))-y0;
						int Yre=yre-OB->RZ;
						int NF=NM->FLAGS->N;
						short* PT=NM->FLAGS->Points;
						for(int i=0;i<NF;i++){
							int i3=i<<2;
							AddPoint(xre,yre,xx0+PT[i3+1]-48,yy0+PT[i3+2]-28,OB,2,(tmtmt+i*19)%52,0,0);
						};
					};
					if(NM->Port){
						int px=(OB->WallX<<(4-zoomsh))-(mapx<<(5-zoomsh));
						int py=(OB->WallY<<(3-zoomsh))-(mapy<<(4-zoomsh))-zoom(GetHeight(PortBuiX<<4,PortBuiY<<4));
						int LL=NM->BuiDist<<(3-zoomsh);
						NewAnimation* BUANM=NM->BuiAnm;
						if(BUANM){
							NewFrame* NF1=&BUANM->Frames[div(tmtmt,BUANM->NFrames).rem];
							//NewFrame* NF2=&BUANM->Frames[div(tmtmt+373,BUANM->NFrames).rem];
							//NewFrame* NF3=&BUANM->Frames[div(tmtmt+977,BUANM->NFrames).rem];
							//NewFrame* NF4=&BUANM->Frames[div(tmtmt+1077,BUANM->NFrames).rem];
							AddOptPoint(ZBF_LO,px,py,px+zoom(NF1->dx),py+zoom(NF1->dy),NULL,NF1->FileID,NF1->SpriteID,AV_TRANSPARENT);
							//AddPoint(px-LL-LL,py,px-LL-LL+NF1->dx,py+NF1->dy,NULL,NF1->FileID,NF1->SpriteID,0,0);
							//AddPoint(px+LL+LL,py,px+LL+LL+NF2->dx,py+NF2->dy,NULL,NF2->FileID,NF2->SpriteID,0,0);
							//AddPoint(px,py+LL,px+NF3->dx,py+LL+NF3->dy,NULL,NF3->FileID,NF3->SpriteID,0,0);
							//AddPoint(px,py-LL,px+NF4->dx,py-LL+NF4->dy,NULL,NF4->FileID,NF4->SpriteID,0,0);
						};
						ShowProducedShip(OB,px,py);
					};
					if(xx1>0&&xx0<=Lx1&&yy1>0&&yy0<Ly1){
						NewAnimation* ANM=OB->HiLayer;
						int ANM_DX=0;
						int ANM_DY=0;
						if(ANM&&ANM->Enabled){
							//if(CSP>=ANM->NFrames){
							//	OB->NewCurSprite=0;
							if(!OB->NewState)CSP=0;
							else CSP=OB->NewCurSprite;
							//};
							int xs=(OB->RealX>>(4+zoomsh))+ANM->StartDx-x0;
							int yy=(OB->RealY>>(5+zoomsh))-y0;
							int ys=yy+zoom(ANM->StartDy);
							ANM_DX=zoom(ANM->StartDx);
							ANM_DY=zoom(ANM->StartDy);
							ys-=zoom(OB->RZ);
							for(int i=0;i<ANM->Parts;i++){
								NewFrame* NF=&ANM->Frames[i+CSP];
								AddHiPoint(xs+zoom(NF->dx),ys+zoom(NF->dy),OB,NF->FileID,NF->SpriteID,0,0);
								xs+=ANM->PartSize;
							};
						};
						ANM=OB->NewAnm;
						CSP=OB->NewCurSprite;
						if(ANM&&ANM->Enabled){
							if(CSP>=ANM->NFrames){
								OB->NewCurSprite=0;
								CSP=0;
							};
							int xre=(OB->RealX>>(4+zoomsh))-x0;
							int yre=(OB->RealY>>(5+zoomsh))-y0;
							int xs=xre+zoom(ANM->StartDx);
							ANM_DX=zoom(ANM->StartDx);
							ANM_DY=zoom(ANM->StartDy);
							//int yy=(mul3(OB->RealY)>>6)-y0;
							int ys=yre+zoom(ANM->StartDy);
							ys-=zoom(OB->RZ);
							for(int i=0;i<ANM->Parts;i++){
								NewFrame* NF=&ANM->Frames[i+CSP];
#ifdef _USE3D
								float CalcWorldZ( int x, int y );
								GPS.DrawSprite( NF->FileID, NF->SpriteID, 
									xs, ys, CalcWorldZ( OB->RealX/16, OB->RealY/16 ), false );
#else //!_USE3D
								AddPoint(xre+zoom(NF->dx),yre+zoom(NM->AnmUpShift+NF->dy),xs+zoom(NF->dx),ys+zoom(NF->dy),OB,NF->FileID,NF->SpriteID,0,0);
#endif // !_USE3D
								xs+=zoom(ANM->PartSize);
							};
                            if(NOPAUSE)OB->NewCurSprite+=ANM->Parts;;
						};
						
						ANM=OB->LoLayer;
						if(ANM&&ANM->Enabled){
							ANM_DX=zoom(ANM->StartDx);
							ANM_DY=zoom(ANM->StartDy);
							if(ANM->LineInfo){
								short* LINF=ANM->LineInfo;
								int nf=ANM->NFrames;
								int pos=0;
								for(int i=0;i<ANM->NFrames;i++){
									int xs=(OB->RealX>>(4+zoomsh))+zoom(ANM->StartDx)-x0;
									int yy=(OB->RealY>>(5+zoomsh))-y0;
									int ys=yy+zoom(ANM->StartDy);
									int ysz=ys;
									ys-=zoom(OB->RZ);
									int x1=zoom(LINF[ pos ]);
									int y1=zoom(LINF[pos+1]);
									int x2=zoom(LINF[pos+2]);
									int y2=zoom(LINF[pos+3]);
									NewFrame* NF=&ANM->Frames[i];
#ifdef _USE3D 
									float scale = GPS.GetScale();
									float rx = float( OB->RealX ) / 16.0f;
									float ry = float( OB->RealY ) / 16.0f;
									float zRx = rx * scale;
									float zRy = ry * scale * 0.5f;
									float viewX = x0;
									float viewY = y0;

									float pivotX, pivotY;
									pivotX = -float( NM->PicDx ) * scale;
									pivotY = -float( NM->PicDy ) * scale;

									float scrX = zRx - pivotX - viewX;
									float scrY = zRy - pivotY - viewY;

									//  x0, y0 are coordinates of up left screen corner in 
									//  game map coordinate system

									GPS.DrawSprite( NF->FileID, NF->SpriteID, 
																scrX, scrY, 0.0f, false, 
																0x00000000 );

									/*
									Vector4D vlt, vrt, vlb;
									
									float zx1 = float( LINF[ pos ] ) * scale;
									float zy1 = float( LINF[pos+1] ) * scale;
									float zx2 = float( LINF[pos+2] ) * scale;
									float zy2 = float( LINF[pos+3] ) * scale;

									float CalcWorldZ( int x, int y );
									float GetCurrentScale();

									float rx = float( OB->RealX ) / 16.0f;
									float ry = float( OB->RealY ) / 16.0f;
									float rz = OB->RZ;
									
									extern float CamZn, CamZf;
									float zs = 0.0f;//CalcWorldZ( rx, ry );// - 16.0f/(CamZf-CamZn);

									const int c_GroundPlane = -10000; 
									const int c_HiPoint = 10000;
									
									if (LINF[pos] == c_GroundPlane)
									{
										/*
										float wx = rx + NM->PicDx; 
										float wy = ry + NM->PicDy * 2 - rz*0.5f;
										float wz = rz*c_Cos30;
										void RegisterVisibleGP(word Index,int FileIndex,int SprIndex,int x,int y);
										RegisterVisibleGP(OB->Index,NF->FileID,NF->SpriteID,zoom(int(rx+NM->PicDx-(mapx<<5))),zoom(int((ry/2)-rz+NM->PicDy-(mapy<<4))));

										GPS.SetScale( 1.0 );
										GPS.DrawWSprite( 
											NF->FileID, NF->SpriteID, 
											wx, wy, wz + 32.0f,
											false, 
											0x0 );
											*//*
										Plane pl;
										float dz = 1.0f/(CamZf - CamZn);
										pl.from3Points( Vector3D( 0, pivotY, 0 ),
														Vector3D( 1, pivotY, 0 ),
														Vector3D( 0, pivotY - 1.0f, dz * 2 ) );   

										void RegisterVisibleGP(word Index,int FileIndex,int SprIndex,int x,int y);
										RegisterVisibleGP(OB->Index,NF->FileID,NF->SpriteID,zoom(int(rx+NM->PicDx-(mapx<<5))),zoom(int((ry/2)-rz+NM->PicDy-(mapy<<4))));
										/*GPS.DrawSpriteAligned( NF->FileID, NF->SpriteID, 
																pl, xs, ys, zs, false, 
																0x00000000 );*//*
										GPS.DrawSprite( NF->FileID, NF->SpriteID, 
																xs, ys, zs, false, 
																0x00000000 );
									}
									else if (LINF[pos] == c_HiPoint)
									{
										assert( false ); 
									}
									else
									{ 
										float z1 = (pivotY - zy1)* c_Cos30/(CamZf - CamZn);
										float z2 = (pivotY - zy2)* c_Cos30/(CamZf - CamZn);

										vlt.set( zx1, zy1, z1 );
										if (zx1 != zx2)
										{
											vrt.set( zx2, zy2, z2 );
										}
										else
										{
											vrt.set( zx2 + 100.0f, zy2, z2 );
										}
										vlb.set( zx1, zy1 - 100.0f, z1 );   

										Plane pl;
										pl.from3Points( vlt, vrt, vlb );
										void RegisterVisibleGP(word Index,int FileIndex,int SprIndex,int x,int y);
										RegisterVisibleGP(OB->Index,NF->FileID,NF->SpriteID,zoom(int(rx+NM->PicDx-(mapx<<5))),zoom(int((ry/2)-rz+NM->PicDy-(mapy<<4))));
										
										GPS.DrawSprite( NF->FileID, NF->SpriteID, 
																xs, ys, zs, false, 
																0x00000000 );
										/*
										GPS.DrawSpriteAligned( NF->FileID, NF->SpriteID, 
																pl, xs, ys, zs, false, 
																0x00000000 );
																*/
									//}
															
/*
										cz /= CamZf - CamZn;
									Matrix4D tr;

									float z1 = FindRelativeZ( zx1, zy1, cx, cy );
									float z2 = FindRelativeZ( zx2, zy2, cx, cy );

									z1 /= CamZf - CamZn;
									z2 /= CamZf - CamZn;

									if (zx2 == zx1 && zy2 == zy1) 
									{
										zx2 += 100;
									}

									tr.translation( -cx, -cy, 0.0f );
									Matrix4D tr1;
									tr1.rotation( Vector3D::oX, -c_PI/2.0f );
									tr *= tr1;
									tr1.translation( cx + rx, cy + ry, zs );
									tr *= tr1;
									GPS.DrawWSpriteTransformed( NF->FileID, NF->SpriteID, tr, 0x00000000 );
*//*
									int cx = zoomex(-NM->PicDx);
									int cy = zoomex(-NM->PicDy);
									if (LINF[pos] != c_GroundPlane) GPS.DrawLine	( xs + zx1,		ysz + zy1,		xs + zx2,	ysz + zy2,	0xFFFF0000 );
									GPS.DrawFillRect( xs + cx - 2,	ysz + cy - 2,	4,			4,			0xFF0000FF );
							*/		
									extern bool CINFMOD;
									if(CINFMOD&&OB->NewBuilding&&!LMode){
										NewMonster* NM=OB->newMons;
										int CX=OB->RealX>>4;
										int CY=OB->RealY>>4;
										int X0=CX+NM->BuildX0;
										int Y0=CY+NM->BuildY0;
										int X1=CX+NM->BuildX1;
										int Y1=CY+NM->BuildY1;
										int D=(Y1-Y0+X1-X0)>>1;
										int D2=D>>1;
										int mpdx=mapx<<5;
										int mpdy=(mapy<<4)+OB->RZ;
										Y0>>=1;
										Y1>>=1;
										X0-=mpdx;Y0-=mpdy;
										X1-=mpdx;Y1-=mpdy;
										void PtLine(int x,int y,int x1,int y1,byte c);
										PtLine(X0,Y0,X0+D,Y0+D2,0xB0);
										PtLine(X0,Y0,X1-D,Y1-D2,0xB0);
										PtLine(X1,Y1,X0+D,Y0+D2,0xB0);
										PtLine(X1,Y1,X1-D,Y1-D2,0xB0);
									};

#endif // _USE3D

#ifndef _USE3D
									if(x1==-10000){
										AddLoPoint(xs+zoom(NF->dx),ys+zoom(NF->dy),OB,NF->FileID,NF->SpriteID,0,0);
									}else
									if(x1==10000){
										AddHiPoint(xs+zoom(NF->dx),ys+zoom(NF->dy),OB,NF->FileID,NF->SpriteID,0,0);
									}else{
										AddLine(xs+x1,ysz+y1,xs+x2,ysz+y2,xs+zoom(NF->dx),ys+zoom(NF->dy),OB,NF->FileID,NF->SpriteID,0,0);
									};
#endif // !_USE3D
									pos+=4;
								};
							}else{
								//if(CSP>=ANM->NFrames){
								//	OB->NewCurSprite=0;
								if(!OB->NewState)CSP=0;
								else CSP=OB->NewCurSprite;
								//};
								int xs=(OB->RealX>>(4+zoomsh))+zoom(ANM->StartDx)-x0;
								int yy=(OB->RealY>>(5+zoomsh))-y0;
								int ys=yy+ANM->StartDy;
								int ysz=ys;
								if(Mode3D)ys-=OB->RZ;
								int np=ANM->Parts-1;
								for(int i=0;i<ANM->Parts;i++){
									NewFrame* NF=&ANM->Frames[i+CSP];
									//if(i!=np)AddToVisual(yy-2048,xs+NF->dx,ys+NF->dy,NULL,NF->FileID,NF->SpriteID);
									//else 
							        AddLoPoint(xs+zoom(NF->dx),ys+zoom(NF->dy),OB,NF->FileID,NF->SpriteID,0,0);
									//AddSprRefl(dxx+xs,dyy+ys,-NF->dx,-NF->dy,NF->FileID,NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
									xs+=ANM->PartSize;
								};
							};
						};
#ifdef CONQUEST
						if(NM->CompxCraft&&(OB->Ready||OB->LocalOrder)&&!OB->Sdoxlo){
							byte mas=NM->CompxCraft->Mask;
							for(int i=0;i<MaxAStages;i++){
								if(mas&(1<<i)){
									byte stt=(OB->StageState>>(i*3))&7;
									int GPID=NM->CompxCraft->Stages[i].GPID;
									if(stt<4){
										OneAddSprite* OAS=(&NM->CompxCraft->Stages[i].Empty)+stt;
										if(OAS->SpriteID!=0xFFFF){
											int xs=(OB->RealX>>(4+zoomsh))+ANM_DX-x0;
											int yy=(OB->RealY>>(5+zoomsh))-y0;
											int ys=yy+ANM_DY;
											int ysz=ys;
											if(Mode3D)ys-=zoom(OB->RZ);
											AddPoint(xs+zoom(OAS->SortX),ysz+zoom(OAS->SortY),xs,ys,OB,GPID,OAS->SpriteID,0,0);
										};
									};
								};
							};
						};
#endif
						if(!OB->Sdoxlo)ShowFires(OB,xx0,yy0);
					}else{
						//if(OB->IFire){
						//	OB->IFire->Erase();
						//	free(OB->IFire);
						//	OB->IFire=NULL;
						//};
					};
				};
			};
		};
	};
	//ShowTrianglesOwerUnits();
	PreShowSprites();
	//ShowVisual();

#ifdef _USE3D
	//  disable z-buffer to let interface elements be drawn in overlay mode
	GPS.EnableZBuffer( false );
#endif // _USE3D

};
int GT0,GT1,GT2,GT3,GT4,GT5;
int PREVLX=0;
byte* BUF=NULL;
int BUFSZ=0;
int StnGP=-1;
int DerGP=-1;
int HolesGP=-1;
void GenerateMiniMapSquare(int x0,int y0,int nx,int ny){
//#ifdef _USE3D
//	return;
//#endif // _USE3D

	if(StnGP==-1){
		StnGP=GPS.PreLoadGPImage("L_Mode\\Stones");
		DerGP=GPS.PreLoadGPImage("L_Mode\\Der");
		HolesGP=GPS.PreLoadGPImage("L_Mode\\Holes");
	};
	int T0=GetTickCount();
	//if(!MiniMap)return;
	
	byte* mptr0=MiniMap;
	
	int LX0=MM_Lx;
	int LY0=MM_Ly;
	int Y0=MM_Y0;
	MM_Lx=nx<<4;
	MM_Ly=ny<<2;
	MM_Y0=256;

	MMSZ=MM_Lx*(MM_Ly+MM_Y0)*2;
	byte* mpart;
	if(MMSZ>BUFSZ){
		mpart=(byte*)realloc(BUF,MMSZ);
		BUF=mpart;
		BUFSZ=MMSZ;
	}else mpart=BUF;
	MiniMap=mpart;

	if(PREVLX!=MM_Lx){
		for(int i=0;i<2000;i++)OFSTBL[i]=(i-64)*MM_Lx;
		PREVLX=MM_Lx;
	};

	TempWindow TW;
	PushWindow(&TW);
	void* sptr=ScreenPtr;

	memset(mpart,0,MMSZ);
	int NX0=x0<<1;
	int NY0=y0;
	int NLX=MM_Lx>>4;
	int NLY=MM_Ly>>2;
	int MYY=(NLY)<<2;
	int ND=0;
	int NND=0;
	GT0=GetTickCount()-T0;
	T0=GetTickCount();
	bool F1=1;
	bool F2=0;
	for(int iy=0;iy<NLY+40&&(F1||F2);iy++){
		F2=0;
		for(int ix=0;ix<NLX;ix++){
			int YY=NY0+iy;
			if(YY>=0&&YY<MaxTH-2){
				int ofs=(NY0+iy)*VertInLine+ix+ix+NX0;
				int VR0=ofs;
				int VR1=ofs+1;
				int VR2=ofs+2;
				int VR3=ofs+VertInLine;
				int VR4=ofs+VertInLine+1;
				int VR5=ofs+VertInLine+2;
				int H0=THMap[VR0]>>1;
				int H1=THMap[VR1]>>1;
				int H2=THMap[VR2]>>1;
				int H3=THMap[VR3]>>1;
				int H4=THMap[VR4]>>1;
				int H5=THMap[VR5]>>1;
				if(H0<0)H0=0;
				if(H1<0)H1=0;
				if(H2<0)H2=0;
				if(H3<0)H3=0;
				if(H4<0)H4=0;
				if(H5<0)H5=0;

				int T0=TexMap[VR0];
				int T1=TexMap[VR1];
				int T2=TexMap[VR2];
				int T3=TexMap[VR3];
				int T4=TexMap[VR4];
				int T5=TexMap[VR5];
				int L0=GetLighting(VR0);
				int L1=GetLighting(VR1);
				int L2=GetLighting(VR2);
				int L3=GetLighting(VR3);
				int L4=GetLighting(VR4);
				int L5=GetLighting(VR5);
				int X0=ix<<4;
				int Y0=iy<<3;
				int YY=Y0>>1;
				int RY0=YY-H0;
				int RY1=YY-2-H1;
				int RY2=YY-H2;
				int RY3=YY+4-H3;
				int RY4=YY+2-H4;
				int RY5=YY+4-H5;
				if(RY0<MYY||RY1<MYY||RY2<MYY||RY3<MYY||RY4<MYY||RY5<MYY){
					F2=1;
					F1=0;
					DrawTriR(X0,Y0,H0,H3,H4,L0,L3,L4,T0,T3,T4,VR0,VR3,VR4);
					DrawTriR(X0+8,Y0-4,H1,H4,H2,L1,L4,L2,T1,T4,T2,VR1,VR4,VR2);
					DrawTriL(X0+8,Y0-4,H1,H4,H0,L1,L4,L0,T1,T4,T0,VR1,VR4,VR0);
					DrawTriL(X0+16,Y0,H2,H5,H4,L2,L4,L4,T2,T5,T4,VR2,VR5,VR4);
					ND++;
				}else NND++;
			};
		};
	};
	GT1=GetTickCount()-T0;
	T0=GetTickCount();
	//shrinking twice
	int ofs0=0;
	int ofs1=MM_Y0*MM_Lx;
	int NN1=MM_Lx*20;
	for(int iy=0;iy<MM_Ly;iy++){
		for(int ix=0;ix<MM_Lx;ix++){
			byte c=MiniMap[ofs1];
			if(c)MiniMap[ofs0]=c;
			else{
				int p=MM_Lx;
				while(p<NN1&&!(c=MiniMap[ofs1+p]))p+=MM_Lx;
				MiniMap[ofs0]=c;
			};
			ofs0++;
			ofs1++;
		};
		ofs1+=MM_Lx;
	};
	GT2=GetTickCount()-T0;
	T0=GetTickCount();
	//render add-details level
	int NDX=MM_Lx>>3;
	int NDY=MM_Ly>>3;
	for(int iy=0;iy<NDY;iy++){
		for(int ix=0;ix<NDX;ix++){
			DrawMiniMaskOnScreen(MiniMap,ix<<3,iy<<3,ix+(x0<<1),iy+(y0>>1),MM_Lx);
			DrawMiniSubWaterSquare(MiniMap,ix<<3,iy<<3,ix+(x0<<1),iy+(y0>>1),MM_Lx);
		};
	};
#ifndef _USE3D
	GT3=GetTickCount()-T0;
	T0=GetTickCount();
	//trees & stones
	SmallZBuffer ZB(MM_Ly+64);
	
	int ww=ScrWidth;
	void* ptr=ScreenPtr;
	ScreenPtr=MiniMap;
	ScrWidth=MM_Lx;
	WindX=0;
	WindY=0;
	WindX1=MM_Lx-1;
	WindY1=MM_Ly-1;
	WindLx=MM_Lx;
	WindLy=ny<<4;

	
	int NT=0;
	int SE=0;
	int NS=0;
	int DX=x0<<4;
	int DY=y0<<2;
	int spx0=(x0>>1)-1;
	int spx1=(x0>>1)+(nx>>1)+1;
	int spy0=(y0>>2)-1;
	int spy1=(y0>>2)+(ny>>2)+1;

	if(spx0<0)spx0=0;else
	if(spx0>=VAL_SPRNX)spx0=VAL_SPRNX-1;
	if(spy0<0)spy0=0;else
	if(spy0>=VAL_SPRNX)spy0=VAL_SPRNX-1;
	if(spx1<0)spx1=0;else
	if(spx1>=VAL_SPRNX)spx1=VAL_SPRNX-1;
	for(int spx=spx0;spx<=spx1;spx++){
		int ofst=spx+(spy0<<SprShf);
		for(int spy=spy0;spy<=spy1;spy++){
			int N=NSpri[ofst];
			int* List=SpRefs[ofst];
			if(N&&List){
				for(int i=0;i<N;i++){
					OneSprite* OS=Sprites+List[i];
					int SX=(OS->x>>2)-DX;
					int SY=(OS->y>>3)-DY;
					if(SX>-16&&SY>-8&&SX<MM_Lx+16&&SY<MM_Ly+30){
						if(OS->Enabled){
							if(OS->SG==&STONES){
								int sgi=OS->SGIndex;
								GPS.ShowGP(SX-(OS->SG->Dx[sgi]>>2),SY-(OS->z>>2)-(OS->SG->Dy[sgi]>>2),StnGP,OS->SG->ObjChar[sgi].SpriteIndex,0);
                            NS++;
							};
							if(OS->SG==&HOLES&&HolesGP>0){
								int sgi=OS->SGIndex;
								GPS.ShowGP(SX-(OS->SG->Dx[sgi]>>2),SY-(OS->z>>2)-(OS->SG->Dy[sgi]>>2),HolesGP,OS->SG->ObjChar[sgi].SpriteIndex,0);
                            NS++;
							};
							if(OS->SG==&TREES){
								int sgi=OS->SGIndex;
								ZB.Add((OS->y>>3)-DY,((OS->x-OS->SG->Dx[sgi])>>2)-DX,(((OS->y>>1)-OS->z-OS->SG->Dy[sgi])>>2)-DY,DerGP,OS->SG->ObjChar[sgi].SpriteIndex);
                            NT++;
							};
						};
					};
				};
			};
			ofst+=VAL_SPRNX;
		};
	};
	ZB.Draw();
	PopWindow(&TW);
#endif//_USE3D
	ScreenPtr=sptr;
	MiniMap=mptr0;
	MM_Lx=LX0;
	MM_Ly=LY0;
	MM_Y0=Y0;
	GT4=GetTickCount()-T0;
	T0=GetTickCount();
	int NX=(nx<<4)/M4CellLX;
	int NY=(ny<<2)/M4CellLY;
	int _X0=(x0<<4)/M4CellLX;
	int _Y0=(y0<<2)/M4CellLY;
	for(int ix=0;ix<NX;ix++){
		for(int iy=0;iy<NY;iy++){
			byte* dat=ReserveMemoryFor4X(_X0+ix,_Y0+iy);
			int ofsp=int(mpart)+(ix+iy*NX*M4CellLY)*M4CellLX;
			int adofs=(NX-1)*M4CellLX;
			__asm{
				push esi
				push edi
				cld
				mov  esi,ofsp
				mov  edi,dat
				mov  ebx,M4CellLY
LPP1:			mov  ecx,M4CellLX/4
				rep  movsd
				add  esi,adofs
				dec  ebx
				jnz  LPP1
				pop  edi
				pop  esi
			};
		};
	};
	//GT5=GetTickCount()-T0;
};
int MM_MinChangeX=10000;
int MM_MinChangeY=10000;
int MM_MaxChangeX=-10000;
int MM_MaxChangeY=-10000;
void ReportCoorChange(int x,int y){
	//return;
	if(!(ReadyMap&&MM_Lx&&MM_Ly))return;
	if(x<0)x=0;
	if(y<0)y=0;
	int mxx=(MM_Lx>>4);
	int myy=(MM_Ly>>2);
	if(x>mxx-1)x=mxx-1;
	if(y>myy-1)y=myy-1;
	if(x<MM_MinChangeX)MM_MinChangeX=x;
	if(y<MM_MinChangeY)MM_MinChangeY=y;
	if(x>MM_MaxChangeX)MM_MaxChangeX=x;
	if(y>MM_MaxChangeY)MM_MaxChangeY=y;
};
void ReportVertexChange(int v){
	//return;
	if(!(ReadyMap&&MM_Lx&&MM_Ly))return;
	int x=(v%VertInLine)>>1;
	int y=v/VertInLine;
	if(x<0)x=0;
	if(y<0)y=0;
	int mxx=(MM_Lx>>4);
	int myy=(MM_Ly>>2);
	if(x>mxx-1)x=mxx-1;
	if(y>myy-1)y=myy-1;
	if(x<MM_MinChangeX)MM_MinChangeX=x;
	if(y<MM_MinChangeY)MM_MinChangeY=y;
	if(x>MM_MaxChangeX)MM_MaxChangeX=x;
	if(y>MM_MaxChangeY)MM_MaxChangeY=y;
};
void CheckMM_Change(){
	//return;
	if(!ReadyMap)return;
	if(MM_MinChangeX<=MM_MaxChangeX){
		int mx0=(MM_MinChangeX<<4)/M4CellLX;
		int mx1=(MM_MaxChangeX<<4)/M4CellLX;
		int my0=(MM_MinChangeY<<2)/M4CellLY;
		int my1=(MM_MaxChangeY<<2)/M4CellLY;
		for(int ix=mx0;ix<=mx1;ix++){
			for(int iy=my0;iy<=my1;iy++){
				int ofs=ix+iy*MMR_Lx;
				int p=PosMap[ofs];
				if(p!=0xFFFF){
					ReadyMap[ofs]=NULL;
					TmpOwnerX[p]=-1;
					TmpOwnerY[p]=-1;
					PosMap[ofs]=0xFFFF;
				};
			};
		};
		//GenerateMiniMapSquare(MM_MinChangeX-8,(MM_MinChangeY-8)&0xFFFE,MM_MaxChangeX-MM_MinChangeX+16,(MM_MaxChangeY-MM_MinChangeY+16)&0xFFFE);
		MM_MinChangeX= 10000;
		MM_MinChangeY= 10000;
		MM_MaxChangeX=-10000;
		MM_MaxChangeY=-10000;
	};
};
void PreShowMiniSprites(){
	int spx0=(mapx-2)>>2;
	int spx1=(mapx+smaplx+2)>>2;
	int spy0=(mapy-2)>>2;
	if(spx0<0)spx0=0;else
	if(spx0>=VAL_SPRNX)spx0=VAL_SPRNX-1;
	if(spy0<0)spy0=0;else
	if(spy0>=VAL_SPRNX)spy0=VAL_SPRNX-1;
	if(spx1<0)spx1=0;else
	if(spx1>=VAL_SPRNX)spx1=VAL_SPRNX-1;

	int x0=mapx<<5;
	int y0=(mapy)<<4;
	int Lx= RealLx;
	int Ly= RealLy;
	int x1=x0+Lx;
	int y1=y0+Ly;
	int SH=5-Shifter;

	for(int spx=spx0;spx<=spx1;spx++){
		int ofst=spx+(spy0<<SprShf);
		int spy=spy0;
		int maxy;
		int xx=(spx<<7)+64;
		do{
			int N=NSpri[ofst];
			int* List=SpRefs[ofst];
			if(N&&List){
				for(int i=0;i<N;i++){
					OneSprite* OS=Sprites+List[i];
					if(OS->Enabled){
						int z=OS->z;//GetHeight(OS->x,OS->y);
						if(!Mode3D)z=0;
						int ry=((OS->y)>>1)-y0;
						int ry1=ry-z;
						int rx=OS->x-x0;
						if(ry1>-128&&ry1<Ly+128&&rx>-128&&rx<Lx+128){
							ObjCharacter* OC=OS->OC;
							SprGroup* SG=OS->SG;
							if(OC->Stand){
								int tm1=div(tmtmt,OC->Delay).quot;
								int fr=div(tm1+OS->x*47+OS->y*83,OC->Frames).rem;
								int spr=fr*OC->Parts;
								int z0=ry+OC->Z0;
								int XX=rx-SG->Dx[OS->SGIndex];
								int YY=ry-SG->Dy[OS->SGIndex]-z;
								NewAnimation* NA=OC->Stand;
								for(int p=0;p<OC->Parts;p++){
									NewFrame* OF=&NA->Frames[spr+p];
									AddPoint(rx>>2,z0>>2,XX>>2,YY>>2,NULL,OF->FileID,OF->SpriteID,0,0);
									z0+=OC->DZ;
								};
							}else{
#ifdef CONQUEST
								if(SG==&SPECIAL){
									int xx=(OS->x)-x0;
									int yy=(OS->y>>1)-y0;
									int zz=yy;
									GeneralObject* GO=NATIONS[7].Mon[OS->NIndex];
									NewAnimation* NAM=&GO->newMons->DeathLie1;
									if(NAM->Enabled){
										int octs,oc2,sesize,oc1,ocM;
										if(NAM->Rotations&1){
											octs=(NAM->Rotations-1)*2;
											oc2=NAM->Rotations-1;
											if(!octs)octs=1;
											sesize=div(255,octs<<1).quot;
											oc1=octs;
											ocM=oc2;
										}else{
											octs=NAM->Rotations*2;
											oc2=NAM->Rotations-1;
											ocM=oc2+1;
											if(!octs)octs=1;
											sesize=0;
											oc1=octs-1;
										};
						
										byte dir=(((OS->Direction+64+sesize)&255)*octs)>>8;
										byte dir2=dir;
										int csp=0;
							
										NewFrame* NF=&NAM->Frames[csp];
										if(dir<ocM)
											AddOptPoint(ZBF_LO,xx>>2,(yy-256)>>2,(xx-NF->dx)>>(5-Shifter),((ry1+NF->dy)>>(5-Shifter)),NULL,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0);
										else{
											dir=oc1-dir;
											AddOptPoint(ZBF_LO,xx>>2,(yy-256)>>2,(xx+NF->dx)>>(5-Shifter),((ry1+NF->dy)>>(5-Shifter)),NULL,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0);
										};
									};
								}else
#endif
								if(SG==&ANMSPR){
									GPS.SetWhiteFont(GP_L_IDXS[SG->FileID]);
									int sp=SG->ObjChar[OS->SGIndex].SpriteIndex;
									int dx=SG->Dx[OS->SGIndex];
									if(sp>=4096)AddSuperLoPoint((rx+dx)>>2,(ry1-SG->Dy[OS->SGIndex])>>2,NULL,SG->FileID,sp,0,0);
									else AddSuperLoPoint((rx-dx)>>2,(ry1-SG->Dy[OS->SGIndex])>>2,NULL,SG->FileID,sp,0,0);
								}else if(SG==&TREES){
#ifdef _USE3D
                                    extern int g_CurTree;
                                    g_CurTree = OS->Index;
								AddPoint(rx>>SH,ry>>SH,(rx-SG->Dx[OS->SGIndex])>>SH,(ry1-SG->Dy[OS->SGIndex])>>SH,NULL,SG->FileID,SG->ObjChar[OS->SGIndex].SpriteIndex,AV_ROUGH,AV_ROUGH);
#else
    								AddPoint(rx>>2,ry>>2,(rx-SG->Dx[OS->SGIndex])>>2,(ry1-SG->Dy[OS->SGIndex])>>2,NULL,SG->FileID,SG->ObjChar[OS->SGIndex].SpriteIndex,AV_ROUGH,AV_ROUGH);
#endif // _USE3D
                                };
							};
						};
					};
				};
			};
			spy++;
			ofst+=VAL_SPRNX;
			maxy=spy<<6;
			if(Mode3D)maxy-=GetHeight(xx,maxy<<1);
		}while(spy<VAL_SPRNX&&maxy<(y1+100));
	};
};
void GSSetup800();
extern int CurPalette;
void ClearAllWaves();
void ClearAllLModeData(){
	try{
		ClearAllWaves();
		if(LMode)ReverseLMode();
		if(ReadyMap){
			free(ReadyMap);
		};
	}catch(...){
	};
	MiniMap=NULL;
	ReadyMap=NULL;
	LMode=0;
};
void CheckMM4XReady(int x,int y,int Lx,int Ly){
	int NX=M4CellLX>>4;
	int NY=M4CellLX>>2;
	bool NeedDraw=0;
	int DRX,DRY,DNY;
	int x0=x/M4CellLX;
	int y0=y/M4CellLY;
	int x1=((x+Lx)/M4CellLX)+1;
	int y1=((y+Ly)/M4CellLY)+1;
	if(x1>=MMR_Lx)x1=MMR_Lx-1;
	if(y1>=MMR_Ly)y1=MMR_Ly-1;
	for(int ix=x0;ix<=x1;ix++){
		for(int iy=y0;iy<=y1;iy++){
			int ofs=ix+iy*MMR_Lx;
			if(ReadyMap[ofs]){
				if(NeedDraw){
					if(DNY==1){
						int NXX=1;
						for(int ixx=ix+1;ixx<=x1;ixx++){
							int ofs=ixx+DRY*MMR_Lx;
							if(!ReadyMap[ofs]){
								NXX++;
								ReadyMap[ofs]=(byte*)1;
								assert(ofs<RSIZE);
							}else ixx=x1+1;
						};
						GenerateMiniMapSquare(DRX*NX,DRY*NY,NX*NXX,DNY*NY);
						NeedDraw=0;
					}else{
						GenerateMiniMapSquare(DRX*NX,DRY*NY,NX,DNY*NY);
						NeedDraw=0;
					};
				};
			}else{
				ReadyMap[ofs]=(byte*)1;
				assert(ofs<RSIZE);
				if(NeedDraw)DNY++;
				else{
					DRX=ix;
					DRY=iy;
					DNY=1;
					NeedDraw=1;
				};
			};
		};
		if(NeedDraw){
			if(DNY==1){
				int NXX=1;
				for(int ixx=ix+1;ixx<=x1;ixx++){
					int ofs=ixx+DRY*MMR_Lx;
					if(!ReadyMap[ofs]){
						NXX++;
						ReadyMap[ofs]=(byte*)1;
						assert(ofs<RSIZE);
					}else ixx=x1+1;
				};
				GenerateMiniMapSquare(DRX*NX,DRY*NY,NX*NXX,DNY*NY);
				NeedDraw=0;
			}else{
				GenerateMiniMapSquare(DRX*NX,DRY*NY,NX,DNY*NY);
				NeedDraw=0;
			};
		};
	};
	//assert(_CrtCheckMemory());
};
