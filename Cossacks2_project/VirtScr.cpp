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
#include "dialogs.h"
#include <assert.h>
#include <math.h>
#include "Masks.h"
#include "fonts.h"
#include "3DGraph.h"
#include "VirtScreen.h"
#include "MapSprites.h"
#include "NewMon.h"
#include "DrawForm.h"
#include <crtdbg.h>
#ifdef _USE3D

#include "..\3DLIB\gMotor\IRenderSystem.h"  
#include "..\3DLIB\gMotor\GPSystem.h"
#include "GP_Draw.h"

extern IRenderSystem*	IRS;
#endif //_USE3D
extern byte *tex1;
extern RLCTable SimpleMask;
int mul3(int);
int prp34(int i);
//class that is used for fast cashing of the 3D surface

VirtualScreen SVSC;
void CheckSVSC(int set){
	return;
	int nn=SVSC.MaxTMY;
	int dy=SVSC.MaxTMX;
	int pos=set;
	/*
	for(int i=0;i<nn-3;i++){
		assert(SVSC.TriangMap[pos]-2<=SVSC.LoTriMap[pos]);
		pos+=dy;
	};
	*/
};
VirtualScreen::VirtualScreen(){
	CellQuotX=NULL;
};
#define xnew(s,t) (t*)malloc((s)*sizeof(t))
void VirtualScreen::SetSize(int scLx,int scLy){
#ifdef _USE3D
	void SetupGBUF(int LX,int LY);
	SetupGBUF(scLx,scLy);
#endif
	if(CellQuotX){
		free(VirtualScreenPointer-RealVLx-RealVLx);
		free(CellQuotX);
		free(CellQuotY);
		free(CellFlags);
		free(MarkedX);
		free(TriangMap);
		free(LoTriMap);
	};
	MemReport("VirtualScreen(Start)");
	CellSX=TriUnit*2;
	CellSY=TriUnit34*8;
	int N=scLx/CellSX;
	if(N&1)N+=3;else N+=2;
	MaxSizeX=N*CellSX;
	N=scLy/CellSY;
	MaxSizeY=(N+2)*CellSY;
	//MaxSizeX=1600;
	//MaxSizeY=1200;
	RealVLx=MaxSizeX;
	RealVLy=MaxSizeY;
	//CellSX=TriUnit*2;
	//CellSY=TriUnit34*8;
	ShiftsPerCellX=div(CellSX,32).quot;
	ShiftsPerCellY=div(CellSY,mul3(8)).quot;
	CellNX=div(RealVLx,CellSX).quot;
	CellNY=div(RealVLy,CellSY).quot;
	RealVLx=CellNX*CellSX;
	RealVLy=CellNY*CellSY;
	NCells=CellNX*CellNY;
	CellQuotX=xnew(NCells,byte);
	CellQuotY=xnew(NCells,byte);
	CellFlags=xnew(NCells,byte);
	memset(CellQuotX,0,NCells);
	memset(CellQuotY,0,NCells);
	memset(CellFlags,0,NCells);
	Lx=128<<ADDSH;
	Ly=128<<ADDSH;
	VirtualScreenPointer=(xnew(RealVLx*(RealVLy+4),byte))+RealVLx+RealVLx;
	memset(VirtualScreenPointer-(RealVLx<<1),255,RealVLx<<1);
	memset(VirtualScreenPointer+(RealVLx*RealVLy),255,RealVLx<<1);
	MaxTMX=div(256<<ADDSH,ShiftsPerCellX).quot;
	MaxTMY=div(256<<ADDSH,ShiftsPerCellY).quot+1;
	TriangMap=xnew(MaxTMX*MaxTMY,int);
	LoTriMap=xnew(MaxTMX*MaxTMY,int);
	MarkedX=xnew(MaxTMX,byte);
	memset(MarkedX,1,MaxTMX);
	Grids=false;
	MemReport("VirtualScreen(End)");
};
VirtualScreen::~VirtualScreen(){
	free(VirtualScreenPointer-RealVLx-RealVLx);
	free(CellQuotX);
	free(CellQuotY);
	free(CellFlags);
	free(MarkedX);
	free(TriangMap);
	free(LoTriMap);
};
void VirtualScreen::SetVSParameters(int sLx,int sLy){
	Lx=sLx;
	Ly=sLy;
};

void CopyTo16(int x,int y,byte* Src,int Pitch,int Lx,int Ly){
	Lx&=~3;
	if(!(Lx&&Ly))return;
#ifdef _USE3D
	//definitions
	void Update8to16(byte* src,int sPitch,int x,int y,int Lx,int Ly);
	void Copy16(byte* Src,int SrcPitch,byte* Dst,int DstPitch,int Lx,int Ly);
	Update8to16(Src,Pitch,x,y,Lx,Ly);
	return;
#else //_USE3D
	try{
		int ofs=int(ScreenPtr)+x+y*ScrWidth;
		int SDX=ScrWidth-Lx;
		int BDX=Pitch-Lx;
		int LL=Lx>>2;
		__asm{
			push esi
			push edi
			push ecx
			mov  esi,Src
			mov  edi,ofs
			cld
xxx:
			mov  ecx,LL
			rep  movsd
			add  esi,BDX
			add  edi,SDX
			dec  Ly
			jnz  xxx
			pop  ecx
			pop  edi
			pop  esi
		};
	}catch(...){};
#endif //_USE3D
};
void Copy16(byte* Src,int SrcPitch,byte* Dst,int DstPitch,int Lx,int Ly);
void LogIt(LPSTR sz,...);
void VirtualScreen::CopyVSPart(int vx,int vy,int sx,int sy,int SizeX,int SizeY){
#ifdef _USE3D
	void CopyTo16(int x,int y,byte* Src,int Pitch,int Lx,int Ly);
	bool LockBackBuffer(int& Pitch,void** ptr);
	void UnlockBackBuffer();
	int Pitch;
	void* ptr;
	//if(LockBackBuffer(Pitch,&ptr)){
		//try{
			int vsofs=int(VirtualScreenPointer)+vx+vy*RealVLx;
			//int scofs=int(ptr)+(sx<<1)+sy*Pitch;
			if(SizeX&&SizeY){
				CopyTo16(sx,sy,(byte*)vsofs,RealVLx,SizeX,SizeY);
			};
		//}catch(...){};
		//UnlockBackBuffer();
	//};
	return;
#else 
	if(!(SizeY&&SizeY))return;
	//debugging control
	//assert(sx>=0&&sy>=0&&sx+SizeX<=SCRSizeX&&sy+SizeY<=SCRSizeY);
	//-----------------
	int vsofs=int(VirtualScreenPointer)+vx+vy*RealVLx;
	int scofs=int(ScreenPtr)+sx+sy*SCRSizeX;
	int szx4=SizeX>>2;
	int szx1=SizeX&3;
	int addvs=RealVLx-SizeX;
	int addsc=SCRSizeX-SizeX;
	__asm{
		push	esi
		push	edi
		pushf
		cld
		mov		edi,scofs
		mov		esi,vsofs
		mov		edx,SizeY
lpp1:	mov		ecx,szx4
		rep		movsd
		mov		ecx,szx1
		rep		movsb
		add		esi,addvs
		add		edi,addsc
		dec		edx
		jnz		lpp1
		popf
		pop		edi
		pop		esi
	};
#endif // _USE3D
};
void VirtualScreen::CopyVSPartMMX(int vx,int vy,int sx,int sy,int SizeX,int SizeY){
#ifdef _USE3D
	return;
#endif // _USE3D

	if(!(SizeY&&SizeY))return;
	//debugging control
	//assert(sx>=0&&sy>=0&&sx+SizeX<=SCRSizeX&&sy+SizeY<=SCRSizeY);
	//-----------------
	int vsofs=int(VirtualScreenPointer)+vx+vy*RealVLx;
	int scofs=int(ScreenPtr)+sx+sy*SCRSizeX;
	int szx4=SizeX>>3;
	int szx1=SizeX&7;
	int addvs=RealVLx-SizeX;
	int addsc=SCRSizeX-SizeX;
	__asm{
		push	esi
		push	edi
		pushf
		cld
		mov		edi,scofs
		mov		esi,vsofs
		mov		edx,SizeY
lpp1:	mov		ecx,szx4
		jcxz	lpp3
lpp2:	movq	mm0,[esi]
		add		esi,8;
		movq	[edi],mm0
		add		edi,8
		dec		cx
		jnz		lpp2
lpp3:	mov		ecx,szx1
		rep		movsb
		add		esi,addvs
		add		edi,addsc
		dec		edx
		jnz		lpp1
		popf
		pop		edi
		pop		esi
		emms
	};
};
extern int RealLx;
extern int RealLy;
#define SRECT(r,a,b,c,d) r.left=a;r.top=b;r.right=c;r.bottom=d;
#define SPT(p,a,b) p.x=a;p.y=b;
int PrevMapX=-10000;
int PrevMapY=-10000;
#ifdef _USE3D

bool Copy16dest=0;
void Draw3DFactures(int dstX,int dstY,int dstDevice,int x,int y,int Lx,int Ly);
void DrawTriStrip(int DevID,int x,int y,int mx,int Gy,int mLx,int GLy,int ScShift);
void CopyMRects(int TextureID,int TexSizeX,int TexSizeY,RECT* RC,int N,POINT* pt);
void FillTexture(int ID,DWORD COLOR,int DR){
	IRS->SetRenderTarget(ID);
	BaseMesh BM;
	BM.create(4,6,vfTnL);
	VertexTnL* VR=(VertexTnL*)BM.getVertexData();
	VR[0].x=DR;
	VR[0].y=DR;
	VR[0].diffuse=COLOR;
	VR[0].w=1.0;
	VR[0].z=0;

	VR[1].x=256-DR;
	VR[1].y=DR;
	VR[1].diffuse=COLOR;
	VR[1].w=1.0;
	VR[1].z=0;

	VR[2].x=DR;
	VR[2].y=256-DR;
	VR[2].diffuse=COLOR;
	VR[2].w=1.0;
	VR[2].z=0;

	VR[3].x=256-DR;
	VR[3].y=256-DR;
	VR[3].diffuse=COLOR;
	VR[3].w=1.0;
	VR[3].z=0;

	word* ids=BM.getIndices();
	ids[0]=0;
	ids[1]=1;
	ids[2]=2;
	ids[3]=1;
	ids[4]=3;
	ids[5]=2;

	BM.setNInd(6);
	BM.setNVert(4);
	BM.setShader(IRS->GetShaderID("fill"));
	IRS->Draw(BM);
};
void RenderScreenArea(int BackBufferX,int BackBufferY,int MapX,int MapY,int MpLx,int MpLy);
class GroundBuffer{
	// -- Inserted by Silver ---21.04.2003
	// -- reason: need for update surfaces when screen resolution changes
	class RefreshCallback : public TextureClient
	{
		GroundBuffer*	papa;
	public:
		void	SetPapa( GroundBuffer* gb ) { papa = gb; }
		
		virtual void Invalidate( TextureDescr* texture )
		{
			assert( papa );
			papa->MakeAllDirty();
		}
	};
	static RefreshCallback refreshCallback;
	// -- end of change -- 21.04.2003
	
public:
	int BBSize;
	int BackBufferIDS[256];
	int NBX,NBY,BSZ;
	int ZBufferID;
	short* QuotX;
	short* QuotY;
	int CellLX;
	int CellLY;
	int NCX;
	int NCY;
	void CopyFromMultiBuffer(RECT* RC,int N,POINT* pt){
		RECT R;
		POINT p;
		for(int i=0;i<N;i++){
			int x0=RC->left;
			int y0=RC->top;
			int x1=RC->right;
			int y1=RC->bottom;
			int XX=x0&0xFF00;
			int YY=y0&0xFF00;
			int XR=XX+256;
			int YD=YY+256;
			int dx=0;
			int dy=0;
			do{
				R.left=x0-XX;
				R.top=y0-YY;
				R.right=XR<x1?XR-XX:x1-XX;
				R.bottom=YD<y1?YD-YY:y1-YY;
				p.x=pt->x+dx;
				p.y=pt->y+dy;
				CopyMRects(BackBufferIDS[(x0>>8)+(y0>>8)*NBX],256,256,&R,1,&p);
				dx+=XR-x0;
				x0=XR;
				XX+=256;
				XR+=256;
				if(x0>=x1){
					dx=0;
					x0=RC->left;
					x1=RC->right;
					XX=x0&0xFF00;
					XR=XX+256;
					dy+=YD-y0;
					y0=YD;
					YY+=256;
					YD+=256;
					if(y0>=y1)break;
				};
			}while(1);
			RC++;
			pt++;
		};
	};
	GroundBuffer(){
		memset(this,0,sizeof*this);
		refreshCallback.SetPapa( this );
	};
	~GroundBuffer(){Erase();};
	void Create(int SizeX,int SizeY,int OneLx,int OneLy){
		Erase();
		CellLX=OneLx;
		CellLY=OneLy;
		NCX=SizeX/OneLx;
		NCY=SizeY/OneLy;
		QuotX=new short[NCX*NCY];
		QuotY=new short[NCX*NCY];
		MakeAllDirty();
		NBX=SizeX>>8;
		if(NBX*256<SizeX)NBX++;
		NBY=SizeY>>8;
		if(NBY*256<SizeY)NBY++;
		int sz=NBX*NBY;
		TextureDescr td;
		td.setValues(256,256,cfBackBufferCompatible,mpVRAM,1,tuRenderTarget);
		for(int i=BSZ;i<sz;i++){
			char texName[64];
			sprintf( texName, "BackBuffer%d", i ); 
			BackBufferIDS[i]=IRS->CreateTexture(texName,td, &refreshCallback);
			
			FillTexture(BackBufferIDS[i],0xFF00FF00,0);
			FillTexture(BackBufferIDS[i],0xFFFF0000,3);
			FillTexture(BackBufferIDS[i],0xFF0000FF,8);
			char ss[20];
			sprintf(ss,"%d",i);
			ShowString(100,100,ss,&BigYellowFont);
			GPS.FlushBatches();
		};
		BSZ=NBX*NBY;
	};
	void Erase(){
		if(QuotX)delete[](QuotX);
		if(QuotY)delete[](QuotY);
		QuotX=NULL;
		QuotY=NULL;
	};
	void MakeAllDirty(){
		if(QuotX&&QuotY){
			int sz=NCX*NCY*2;
			memset(QuotX,0xFF,sz);
			memset(QuotX,0xFF,sz);
		};
	};
	void MakeDirty(int mx,int my,int szx,int szy){
		for(int iy=0;iy<szy;iy++){
			int qy=(my+iy)/NCY;
			int py=(my+iy)%NCY;
			int qx=mx/NCX;
			int px=mx%NCX;
			int ofs=px+py*NCX;
			for(int ix=0;ix<szx;ix++){
				if(QuotX[ofs]==qx&&QuotY[ofs]==qy){
					QuotX[ofs]=-1;
					QuotY[ofs]=-1;
				};
				if(px==NCX-1){
					px=0;
					qx++;
					ofs-=NCX-1;
				}else{
					px++;
					ofs++;
				};
			};
		};
	};
	void RefreshForCurrentPosirtion(int mx,int my){
		int NC=16<<(5-Shifter);
		int Summ=0;
		for(int iy=0;iy<NCY;iy++){
			int qy=(my+iy)/NCY;
			int py=(my+iy)%NCY;
			int qx=mx/NCX;
			int px=mx%NCX;
			int ofs=px+py*NCX;
			for(int ix=0;ix<NCX;ix++){
				if(QuotX[ofs]!=qx||QuotY[ofs]!=qy){
					int DirtyLx=1;
					int DirtyLy=1;
					int dpx=px;
					int dqx=qx;
					int ofs2=ofs;
					if(dpx<NCX-1){
						dpx++;
						ofs2++;
						for(int dx=ix+1;dx<NCX&&dpx<NCX&&(QuotX[ofs2]!=qx||QuotY[ofs2]!=qy);dx++){
							DirtyLx++;
							dpx++;
							ofs2++;
						};
					};
					int RLX=DirtyLx<8?DirtyLx*NC:8*NC;
					Summ=RLX;
					int ok=1;
					int dpy=py+1;
					for(int dy=iy+1;dy<NCY&&dpy<NCY&&ok&&Summ<8000;dy++,dpy++){
						dpx=px;
						dqx=qx;
						ofs2=ofs+DirtyLy*NCX;
						for(int dx=0;dx<DirtyLx&&ok;dx++){
							if(QuotX[ofs2]==qx&&QuotY[ofs2]==qy)ok=0;
							dpx++;
							ofs2++;
						};
						if(ok){
							DirtyLy++;
							Summ+=RLX;
						};
					};
					for(int dy=0;dy<DirtyLy;dy++){
						dpx=px;
						dqx=qx;
						ofs2=ofs+dy*NCX;
						for(int dx=0;dx<DirtyLx;dx++){
							QuotX[ofs2]=qx;
							QuotY[ofs2]=qy;
							dpx++;
							ofs2++;
						};
					};
					RenderScreenArea(px*CellLX,py*CellLY,mx+ix,my+iy,DirtyLx,DirtyLy);
					Summ=0;
				};
				if(px==NCX-1){
					px=0;
					qx++;
					ofs-=NCX-1;
				}else{
					px++;
					ofs++;
				};
			};
		};
	};
};

GroundBuffer::RefreshCallback GroundBuffer::refreshCallback;

#define MAXLY 3000
struct OneSprInfo{
	short x,y,file,spr;
};
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
int TGP=-1;
void DrawTreesOnGround(int BackBufferX,int BackBufferY,int MapX,int MapY,int MpLx,int MpLy){
	if(TGP==-1){
		TGP=GPS.PreLoadGPImage("TreesAll");
		if(TGP!=-1)GPS.LoadGP(TGP);
	};
	SetWind(BackBufferX,BackBufferY,MpLx<<Shifter,MpLy);
	
	ViewPort VP=GPS.GetClipArea();
	int WindX=VP.x;
	int WindY=VP.y;
	int WindX1=VP.x+VP.width-1;
	int WindY1=VP.y+VP.height-1;
	SmallZBuffer ZB(800+MpLy);	
	int NT=0;
	int SE=0;
	int NS=0;
	int SH=5-Shifter;

	int DX=(MapX<<5)-(BackBufferX<<SH);
	int DY=(MapY-BackBufferY)<<SH;

	int spx0=(MapX>>2)-2;
	int spx1=(MapX>>2)+(MpLx>>2)+2;
	int spy0=(MapY>>(Shifter+1))-2;
	int spy1=(MapY>>(Shifter+1))+(MpLy>>(Shifter+1))+6;

	if(spx0<0)spx0=0;else
	if(spx0>=VAL_SPRNX)spx0=VAL_SPRNX-1;
	if(spy0<0)spy0=0;else
	if(spy0>=VAL_SPRNX)spy0=VAL_SPRNX-1;
	if(spx1<0)spx1=0;else
	if(spx1>=VAL_SPRNX)spx1=VAL_SPRNX-1;
	GPS.SetScale(1/float(1<<SH));

	for(int spx=spx0;spx<=spx1;spx++){
		int ofst=spx+(spy0<<SprShf);
		for(int spy=spy0;spy<=spy1;spy++){
			int N=NSpri[ofst];
			int* List=SpRefs[ofst];
			if(N&&List){
				for(int i=0;i<N;i++){
					OneSprite* OS=Sprites+List[i];
					int SX=OS->x-DX;
					int SY=(OS->y>>1)-DY;
					int FID=OS->SG->FileID;
					if(OS->SG==&TREES)FID=TGP;
					int sgi=OS->SGIndex;
					int SPR=OS->SG->ObjChar[sgi].SpriteIndex;
					int sdx=OS->SG->Dx[sgi];
					int sdy=OS->SG->Dy[sgi]+OS->z;
					int W=GPS.GetGPWidth(FID,SPR)>>SH;
					int H=GPS.GetGPHeight(FID,SPR)>>SH;
					int spx=(SX-sdx)>>SH;
					int spy=(SY-sdy)>>SH;
					SX>>=SH;
					SY>>=SH;
					if(spx<WindX1&&spy<WindY1&&spx+W>=WindX&&spy+H>=WindY){
						if(OS->Enabled){
							if(OS->SG==&STONES||OS->SG==&HOLES){
								GPS.ShowGP(spx,spy,FID,SPR,0);
								NS++;
							};
							if(OS->SG==&TREES){
								int sgi=OS->SGIndex;
								ZB.Add(SY+300,spx,spy,FID,SPR);
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
	GPS.SetScale(1);
};
GroundBuffer GBUF;
void RenderScreenArea(int BackBufferX,int BackBufferY,int MapX,int MapY,int MpLx,int MpLy){
	//int DVID=IRS->GetVBufferID();
	//IRS->SaveTexture(DVID,"001.dds");
	int BX0=BackBufferX&0xFF00;
	int BY0=BackBufferY&0xFF00;
	int x1=BackBufferX+(MpLx<<5);
	int y1=BackBufferY+(MpLy<<4);
	int BX1=(x1-1)&0xFF00;
	int BY1=(y1-1)&0xFF00;
	for(int ix=BX0;ix<=BX1;ix+=256){
		for(int iy=BY0;iy<=BY1;iy+=256){
			int X0=ix>BackBufferX?ix:BackBufferX;
			int Y0=iy>BackBufferY?iy:BackBufferY;
			int X1=ix+256<x1?ix+256:x1;
			int Y1=iy+256<y1?iy+256:y1; 
			int ID=(X0>>8)+(Y0>>8)*GBUF.NBX;
			if(X1>X0&&Y1>Y0)DrawTriStrip(GBUF.BackBufferIDS[ID],X0-ix,Y0-iy,(MapX+((X0-BackBufferX)>>5))<<(5-Shifter),(MapY<<4)+Y0-BackBufferY,(X1-X0)>>Shifter,Y1-Y0,5-Shifter);
			
			TempWindow TW;
			PushWindow(&TW);
			GPS.FlushBatches();
			
			IRS->SetRenderTarget(GBUF.BackBufferIDS[ID]);
			DrawTreesOnGround(X0-ix,Y0-iy,(MapX+((X0-BackBufferX)>>5))<<(5-Shifter),(MapY<<4)+Y0-BackBufferY,(X1-X0)>>Shifter,Y1-Y0);			
			GPS.FlushBatches();
			IRS->SetRenderTarget(0);
			
			PopWindow(&TW);
			
		};
	};
	SetWind(0,0,RealLx,RealLy);
	//DrawTriStrip(DVID,BackBufferX,BackBufferY,MapX,MapY<<4,MpLx,MpLy<<4,0);
	//Draw3DFactures(BackBufferX,BackBufferY,DVID,MapX<<5,MapY<<4,MpLx<<5,MpLy<<4);
	//IRS->SaveTexture(DVID,"001.dds");
};
void SetupGBUF(int LX,int LY){
	GBUF.Create(LX,LY,32,16);
};
void MakeDirtyGBUF(int mx,int my,int sx,int sy){
	GBUF.MakeDirty(mx,my,sx,sy);
};
void MakeAllDirtyGBUF(){
	GBUF.MakeAllDirty();
};
BaseMesh* COPYMESH=NULL;
void CopyMRects(int TextureID,int TexSizeX,int TexSizeY,RECT* RC,int N,POINT* pt){
	IRS->SetCurrentShader(0);
	if(!COPYMESH){
		COPYMESH=new BaseMesh;
		COPYMESH->create(16,24,vfTnL);
		COPYMESH->setShader(IRS->GetShaderID("copy")); 
		word* idx=COPYMESH->getIndices();
		for(int i=0;i<4;i++){
			int v6=i*6;
			int v4=i*4;
			idx[v6  ]=v4  ;
			idx[v6+1]=v4+1;
			idx[v6+2]=v4+2;
			idx[v6+3]=v4+1;
			idx[v6+4]=v4+3;
			idx[v6+5]=v4+2;
		};
	};
	VertexTnL* VR=(VertexTnL*)COPYMESH->getVertexData();
	COPYMESH->setNInd(N*6);
	COPYMESH->setNVert(N*4);
	COPYMESH->setTexture(TextureID);
	for(int i=0;i<N;i++){
		int imlx=RC[i].right-RC[i].left;
		int imly=RC[i].bottom-RC[i].top;
		VR[0].x=pt[i].x;
		VR[0].y=pt[i].y;
		VR[0].w=1;
		VR[0].z=0;
		VR[0].u=float(RC[i].left)/TexSizeX;
		VR[0].v=float(RC[i].top)/TexSizeY;

		VR[1].x=pt[i].x+imlx;
		VR[1].y=pt[i].y;
		VR[1].w=1;
		VR[1].u=float(RC[i].left+imlx)/TexSizeX;
		VR[1].v=float(RC[i].top)/TexSizeY;
		VR[1].z=0;

		VR[2].x=pt[i].x;
		VR[2].y=pt[i].y+imly;
		VR[2].w=1;
		VR[2].u=float(RC[i].left)/TexSizeX;
		VR[2].v=float(RC[i].top+imly)/TexSizeY;
		VR[2].z=0;

		VR[3].x=pt[i].x+imlx;
		VR[3].y=pt[i].y+imly;
		VR[3].w=1;
		VR[3].u=float(RC[i].left+imlx)/TexSizeX;
		VR[3].v=float(RC[i].top+imly)/TexSizeY;
		VR[3].z=0;
		//for(int j=0;j<4;j++){
		//	int dy=VR[j].y;
		//	VR[j].x=512+(VR[j].x-512)*3000/(3000-dy);
		//	VR[j].y=dy*4000/(4000-dy);
		//};
		VR+=4;
	};
	IRS->Draw(*COPYMESH);
};
#endif //_USE3D

void VirtualScreen::CopyVSToScreen(){
#ifdef _USE3D
	//int VID=IRS->GetVBufferID();
	//if(VID)
	{
		IRS->SetCurrentShader(0);
		void ProcessLightMap();
		ProcessLightMap();
		GBUF.RefreshForCurrentPosirtion(mapx>>(5-Shifter),mapy>>(5-Shifter));
		//copying from surface to back buffer
		RECT CR[4];
		POINT PT[4];
		int Ncr=0;
		int xs=mapx<<Shifter;
		int ys=mapy<<(Shifter-1);
		int xp=xs%RealLx;
		int yp=ys%RealLy;
		xs=RealLx-(xp%RealLx);
		ys=RealLy-(yp%RealLy);
		if(xp&&yp){
			SRECT(CR[0],0,0,xp,yp);
			SPT(PT[0],xs,ys);

			SRECT(CR[1],xp,0,RealLx,yp);
			SPT(PT[1],0,ys);

			SRECT(CR[2],0,yp,xp,RealLy);
			SPT(PT[2],xs,0);

			SRECT(CR[3],xp,yp,RealLx,RealLy);
			SPT(PT[3],0,0);

			Ncr=4;
		}else if(xp){
			SRECT(CR[0],0,0,xp,RealLy);
			SPT(PT[0],xs,0);

			SRECT(CR[1],xp,0,RealLx,RealLy);
			SPT(PT[1],0,0);

			Ncr=2;
		}else if(yp){
			SRECT(CR[0],0,0,RealLx,yp);
			SPT(PT[0],0,ys);

			SRECT(CR[1],0,yp,RealLx,RealLy);
			SPT(PT[1],0,0);

			Ncr=2;
		}else{
			SRECT(CR[0],0,0,RealLx,RealLy);
			SPT(PT[0],0,0);
			Ncr=1;
		};

		//CopyMRects(VID,1024,1024,CR,Ncr,PT);
		GBUF.CopyFromMultiBuffer(CR,Ncr,PT);
		return;
	};
#endif
	int TotLx=smaplx<<5;
	int TotLy=mul3(smaply)<<3;
	int vsx=div(mapx<<5,RealVLx).rem;
	int vsy=div(mul3(mapy)<<3,RealVLy).rem;
	int Lx0,Ly0,Lx1,Ly1;
	if(vsx+TotLx<=RealVLx){
		Lx0=TotLx;
		Lx1=0;
	}else{
		Lx0=RealVLx-vsx;
		Lx1=TotLx-Lx0;
	};
	if(vsy+TotLy<=RealVLy){
		Ly0=TotLy;
		Ly1=0;
	}else{
		Ly0=RealVLy-vsy;
		Ly1=TotLy-Ly0;
	};
	CopyVSPart(vsx,vsy,smapx,smapy,Lx0,Ly0);
	CopyVSPart(0,vsy,smapx+Lx0,smapy,Lx1,Ly0);
	CopyVSPart(vsx,0,smapx,smapy+Ly0,Lx0,Ly1);
	CopyVSPart(0,0,smapx+Lx0,smapy+Ly0,Lx1,Ly1);
};
void VirtualScreen::RenderVSPart(int QuotX,int QuotY,int cx,int cy,int clx,int cly){
	if(!(clx&&cly))return;
	//debugging part
	//assert(cx>=0&&cy>=0&&cx+clx<=CellNX&&cy+cly<=CellNY);
	//--------------
	int spos=cx+cy*CellNX;
	int StartCellY;
	int NCellY;
	bool CellStart=false;
	for(int px=0;px<clx;px++){
		int pos=spos+px;
		for(int py=0;py<cly;py++){
			if((!CellFlags[pos])||CellQuotX[pos]!=QuotX||CellQuotY[pos]!=QuotY){
				//need to be rendered
				if(CellStart)NCellY++;
				else{
					CellStart=true;
					StartCellY=py+cy;
					NCellY=1;
				};
			}else{
				if(CellStart){
					RenderVerticalSet(QuotX,QuotY,px+cx,StartCellY,NCellY);
					CellStart=false;
				};
			};
			pos+=CellNX;
		};
		if(CellStart){
			RenderVerticalSet(QuotX,QuotY,px+cx,StartCellY,NCellY);
			CellStart=false;
		};
	};
};
void VirtualScreen::RefreshSurface(){
	//calculating starting cell
	int scx=div(mapx,ShiftsPerCellX).quot;
	int scy=div(mapy,ShiftsPerCellY).quot;
	int scx1=div(mapx+smaplx-1,ShiftsPerCellX).quot;
	int scy1=div(mapy+smaply-1,ShiftsPerCellY).quot;
	int clnx=scx1-scx+1;
	int clny=scy1-scy+1;
	int vsx=div(scx,CellNX).rem;
	int vsy=div(scy,CellNY).rem;
	int Lx0,Ly0,Lx1,Ly1;
	if(vsx+clnx<=CellNX){
		Lx0=clnx;
		Lx1=0;
	}else{
		Lx0=CellNX-vsx;
		Lx1=clnx-Lx0;
	};
	if(vsy+clny<=CellNY){
		Ly0=clny;
		Ly1=0;
	}else{
		Ly0=CellNY-vsy;
		Ly1=clny-Ly0;
	};
	int QuotX=div(scx,CellNX).quot;
	int QuotY=div(scy,CellNY).quot;
	RenderVSPart(QuotX,QuotY,vsx,vsy,Lx0,Ly0);
	RenderVSPart(QuotX+1,QuotY,0,vsy,Lx1,Ly0);
	RenderVSPart(QuotX,QuotY+1,vsx,0,Lx0,Ly1);
	RenderVSPart(QuotX+1,QuotY+1,0,0,Lx1,Ly1);
};
extern byte ExtTex[256][4];
extern short randoma[8192];
byte DTX(byte v,int t){
	return ExtTex[v][randoma[t&8191]&3];
};
extern byte TileMap[256];
int VirtualScreen::ShowLimitedSector(int i,bool Mode3D,int HiLine,int LoLine,int QuotX,int QuotY){
	//COUNTER++;
	if(i>=MaxTH*MaxTH*2)return 0;
	//determination of the parameters of the triangle
	//1.Coordinates&type 
	int TriType;
	int x1,y1,z1,x2,y2,z2,x3,y3,z3,yy1,yy2,yy3;
	int TriStartY=div((i>>2),MaxSector).quot;
	int StartVertex=TriStartY+((i>>2)<<1);
	int StartSide=(i>>2)*6;
	int Tex1,Tex2,Tex3;
	TriType=i&3;
	int TriStartX=((i>>2)-TriStartY*MaxSector)*(4*TriUnit);
	TriStartY=TriStartY*(TriUnit+TriUnit);
	bool Visible=true;
	switch(TriType){
	case 0:
		x1=x2=TriStartX;
		y1=TriStartY;
		yy1=prp34(y1);
		y2=TriStartY+TriUnit+TriUnit;
		yy2=prp34(y2);
		x3=TriStartX+TriUnit+TriUnit;
		y3=TriStartY+TriUnit;
		yy3=prp34(y3);
		if(Mode3D){
			z1=yy1-THMap[StartVertex]-AddTHMap(StartVertex);
			z2=yy2-THMap[StartVertex+VertInLine]-AddTHMap(StartVertex+VertInLine);
			z3=yy3-THMap[StartVertex+VertInLine+1]-AddTHMap(StartVertex+VertInLine+1);
		}else{
			z1=yy1;
			z2=yy2;
			z3=yy3;
		};
		if(z1>=z2)Visible=false;
		break;
	case 1:
		x1=TriStartX;
		y1=TriStartY;
		yy1=prp34(y1);
		x2=x3=TriStartX+TriUnit+TriUnit;
		y2=TriStartY-TriUnit;
		yy2=prp34(y2);
		y3=TriStartY+TriUnit;
		yy3=prp34(y3);
		if(Mode3D){
			z1=yy1-THMap[StartVertex]-AddTHMap(StartVertex);
			z2=yy2-THMap[StartVertex+1]-AddTHMap(StartVertex+1);
			z3=yy3-THMap[StartVertex+VertInLine+1]-AddTHMap(StartVertex+VertInLine+1);
		}else{
			z1=yy1;
			z2=yy2;
			z3=yy3;
		};
		if(z2>=z3)Visible=false;
		break;
	case 2:
		x1=TriStartX+TriUnit+TriUnit+TriUnit+TriUnit;
		y1=TriStartY;
		yy1=prp34(y1);
		x2=x3=TriStartX+TriUnit+TriUnit;
		y2=TriStartY-TriUnit;
		yy2=prp34(y2);
		y3=TriStartY+TriUnit;
		yy3=prp34(y3);
		if(Mode3D){
			z1=yy1-THMap[StartVertex+2]-AddTHMap(StartVertex+2);
			z2=yy2-THMap[StartVertex+1]-AddTHMap(StartVertex+1);
			z3=yy3-THMap[StartVertex+VertInLine+1]-AddTHMap(StartVertex+VertInLine+1);
		}else{
			z1=yy1;
			z2=yy2;
			z3=yy3;
		};
		if(z2>=z3)Visible=false;
		break;
	case 3:
		x1=x2=TriStartX+TriUnit+TriUnit+TriUnit+TriUnit;
		y1=TriStartY;
		yy1=prp34(y1);
		y2=TriStartY+TriUnit+TriUnit;
		yy2=prp34(y2);
		x3=TriStartX+TriUnit+TriUnit;
		y3=TriStartY+TriUnit;
		yy3=prp34(y3);
		if(Mode3D){
			z1=yy1-THMap[StartVertex+2]-AddTHMap(StartVertex+2);
			z2=yy2-THMap[StartVertex+VertInLine+2]-AddTHMap(StartVertex+VertInLine+2);
			z3=yy3-THMap[StartVertex+VertInLine+1]-AddTHMap(StartVertex+VertInLine+1);
		}else{
			z1=yy1;
			z2=yy2;
			z3=yy3;
		};
		if(z1>=z2)Visible=false;
	};
	
	if(!Visible)return 0;
	//Now we are ready to render
	int f1,f2,f3;
	switch(TriType){
	case 0:
		f1=GetLighting(StartVertex);
		f2=GetLighting(StartVertex+VertInLine);
		f3=GetLighting(StartVertex+VertInLine+1);
		Tex1=GetVTex(StartVertex);
		Tex2=GetVTex(StartVertex+VertInLine);
		Tex3=GetVTex(StartVertex+VertInLine+1);
		break;
	case 1:
		f1=GetLighting(StartVertex);
		f2=GetLighting(StartVertex+1);
		f3=GetLighting(StartVertex+VertInLine+1);
		Tex1=GetVTex(StartVertex);
		Tex2=GetVTex(StartVertex+1);
		Tex3=GetVTex(StartVertex+VertInLine+1);
		break;
	case 2:
		f1=GetLighting(StartVertex+2);
		f2=GetLighting(StartVertex+1);
		f3=GetLighting(StartVertex+VertInLine+1);
		Tex1=GetVTex(StartVertex+2);
		Tex2=GetVTex(StartVertex+1);
		Tex3=GetVTex(StartVertex+VertInLine+1);
		break;
	case 3:
		f1=GetLighting(StartVertex+2);
		f2=GetLighting(StartVertex+VertInLine+2);
		f3=GetLighting(StartVertex+VertInLine+1);
		Tex1=GetVTex(StartVertex+2);
		Tex2=GetVTex(StartVertex+VertInLine+2);
		Tex3=GetVTex(StartVertex+VertInLine+1);
	};
	int xt1,yt1,xt2,yt2,xt3,yt3;
	if((TexFlags[Tex1]&TEX_NORMALPUT)||(TexFlags[Tex2]&TEX_NORMALPUT)||(TexFlags[Tex3]&TEX_NORMALPUT)){
		xt1=-(x1>>1)-y1;
		yt1=(((x1+x1+x1)>>1)-y1)>>1;
		xt2=-(x2>>1)-y2;
		yt2=(((x2+x2+x2)>>1)-y2)>>1;
		xt3=-(x3>>1)-y3;
		yt3=(((x3+x3+x3)>>1)-y3)>>1;
	}else{
		xt1=((x1+y1)*3)>>2;
		yt1=((y1-x1)*3)>>2;
		xt2=((x2+y2)*3)>>2;
		yt2=((y2-x2)*3)>>2;
		xt3=((x3+y3)*3)>>2;
		yt3=((y3-x3)*3)>>2;
	};


	//-------  Del this
	//xt1-=xt1>>2;
	//yt1-=yt1>>2;
	//xt2-=xt2>>2;
	//yt2-=yt2>>2;
	//xt3-=xt3>>2;
	//yt3-=yt3>>2;
	//-------
	int xmin=xt1;
	if(xt2<xmin)xmin=xt2;
	if(xt3<xmin)xmin=xt3;
	int ymin=yt1;
	if(yt2<ymin)ymin=yt2;
	if(yt3<ymin)ymin=yt3;
	xmin-=xmin&63;
	ymin-=ymin&63;
	xt1-=xmin;
	xt2-=xmin;
	xt3-=xmin;
	yt1-=ymin;
	yt2-=ymin;
	yt3-=ymin;
	int RSIZE=0;
	int DXX=QuotX*RealVLx;
	int DYY=QuotY*RealVLy;
	x1=x1-DXX;
	z1=z1-DYY;
	x2=x2-DXX;
	z2=z2-DYY;
	x3=x3-DXX;
	z3=z3-DYY;
	if((z1<HiLine&&z2<HiLine&&z3<HiLine)||(z1>LoLine&&z2>LoLine&&z3>LoLine))return 0;
	if((Tex1==Tex2&&Tex1==Tex3)||(TriType==7)){
		if(TileMap[Tex1]){
			int dx=randoma[((i*23)/17)&8191]&63;
			int dy=randoma[((i*29)/17)&8191]&63;
			xt1+=dx;
			xt2+=dx;
			xt3+=dx;
			yt1+=dy;
			yt2+=dy;
			yt3+=dy;
		}else
		if(ExtTex[Tex1][0]!=ExtTex[Tex1][1]){
			int dx=randoma[((i*23)/17)&8191]&63;
			xt1+=dx;
			xt2+=dx;
			xt3+=dx;
		};
		DirectRenderTriangle64(x1,z1,x2,z2,x3,z3,
		                              xt1,yt1,xt2,yt2,xt3,yt3,
									  f1,f2,f3,VirtualScreenPointer,
									  tex1+GetBmOfst(DTX(Tex1,(i*19)/17)),
									  HiLine,LoLine,RealVLx);
	}else{
		switch(TriType){
		case 0:
			PrepareIntersection1(Tex2,Tex1,Tex3,xt2,yt2,
				SECTMAP(StartSide),
				SECTMAP(StartSide+2),
				SECTMAP(StartSide+SectInLine+1),
				SimpleMask,tex1);
			DirectRenderTriangle64(x1,z1,x3,z3,x2,z2,
		                              //63,31,0,63,0,0,
									  30,15,0,31,0,0,
									  f1,f3,f2,VirtualScreenPointer,
									  ResultMask,
									  HiLine,LoLine,RealVLx);
			break;
		case 1:
			PrepareIntersection2(Tex1,Tex3,Tex2,xt3,yt3,
				SECTMAP(StartSide+2),
				SECTMAP(StartSide+3),
				SECTMAP(StartSide+1),
				SimpleMask,tex1);
			DirectRenderTriangle64(x1,z1,x3,z3,x2,z2,
		                              //63,0,0,31,63,63,
									  30,1,1,14,30,30,
									  f1,f3,f2,VirtualScreenPointer,
									  ResultMask,
									  HiLine,LoLine,RealVLx);
			break;
		case 2:
			PrepareIntersection1(Tex3,Tex2,Tex1,xt3,yt3,
				SECTMAP(StartSide+3),
				SECTMAP(StartSide+4),
				SECTMAP(StartSide+5),
				SimpleMask,tex1);
			DirectRenderTriangle64(x1,z1,x3,z3,x2,z2,
		                              //0,63,0,0,63,31,
									  1,30,1,1,30,14,
									  f1,f3,f2,VirtualScreenPointer,
									  ResultMask,
									  HiLine,LoLine,RealVLx);
			break;
		case 3:
			PrepareIntersection2(Tex3,Tex2,Tex1,xt2,yt2,
				SECTMAP(StartSide+SectInLine+4),
				SECTMAP(StartSide+6),
				SECTMAP(StartSide+5),
				SimpleMask,tex1);
			DirectRenderTriangle64(x1,z1,x3,z3,x2,z2,
		                              //63,63,63,0,0,31,
									  30,30,30,1,1,14,
									  f1,f3,f2,VirtualScreenPointer,
									  ResultMask,
									  HiLine,LoLine,RealVLx);
			break;
		};
	};
	return 1;
};
void CheckFirstLine();
void DrawInBuffer(byte* Buffer,int x0,int y0,int x1,int y1,int xr0,int yr0,int BufWidth);
void DrawOnePixCell(byte* Buf,int x,int y,int cx,int cy,int BufWidth);
void VirtualScreen::RenderVerticalSet(int QuotX,int QuotY,int cx,int cy,int cly){
	//debugging part
	//assert(cy>=0&&cy+cly<=CellNY);
	//--------------
	int NRend=0;
	int HiLine=cy*CellSY;
	int LoLine=(cy+cly)*CellSY-1;
	int cost=QuotX*CellNX+cx+(QuotY*CellNY+cy)*MaxTMX;
	CheckFirstLine();
	CheckVLINE(cost);
	int Tstart=TriangMap[cost];
	//assert(Tstart!=4166);
	if(Tstart==-1){
		int cost1=cost;
		cost1-=MaxTMX;
		while(cost1>=0&&Tstart==-1){
			CheckVLINE(cost1);
			Tstart=TriangMap[cost1];
			cost1-=MaxTMX;
		};
		if(Tstart==-1){
			if(cx&1)Tstart=(cx<<1)+1;
			else Tstart=(cx<<1);
		};
	};
	int TStart0=Tstart;
	//assert(Tstart!=4166);
	cost+=(cly)*MaxTMX;
	CheckVLINE(cost);
	int Tend=LoTriMap[cost];
	if(Tend==-1){
		int cmax=MaxTMX*MaxTMY;
		cost+=MaxTMX;
		while(Tend==-1&&cost<cmax){
			CheckVLINE(cost);
			Tend=LoTriMap[cost];
			cost+=MaxTMX;
		};
		if(Tend==-1){
			if(cx&1)Tend=(cx<<1)+1+(MaxTH-1)*MaxTH*2;
			else Tend=(cx<<1)+(MaxTH-1)*MaxTH*2;
		};
	};
	int pos=cx+CellNX*cy;
	for(int j=0;j<cly;j++){
		CellQuotX[pos]=QuotX;
		CellQuotY[pos]=QuotY;
		CellFlags[pos]=1;
		pos+=CellNX;
	};
	if(cx&1){
		while(Tstart!=Tend&&Tstart<Tend+2){
			NRend+=ShowLimitedSector(Tstart,Mode3D,HiLine,LoLine,QuotX,QuotY);
			if(Tstart&1)Tstart+=MaxTH+MaxTH-1;
			else Tstart+=1;
		};
		if(TStart0!=Tend)
			NRend+=ShowLimitedSector(Tend,Mode3D,HiLine,LoLine,QuotX,QuotY);
	}else{
		while(Tstart!=Tend&&Tstart<Tend+2){
			NRend+=ShowLimitedSector(Tstart,Mode3D,HiLine,LoLine,QuotX,QuotY);
			if(Tstart&1)Tstart-=1;
			else Tstart+=MaxTH+MaxTH+1;
		};
		if(TStart0!=Tend)
		   NRend+=ShowLimitedSector(Tend,Mode3D,HiLine,LoLine,QuotX,QuotY);
	};
	//return;
	//assert(NRend);
	if(Grids){
		//marking grids
		int xxx=cx*CellSX;
		int miny=100000;
		int cdx=cx+QuotX*CellNX;
		int vend=VertInLine*div(Tend,MaxTH+MaxTH).quot+cdx;
		int vstart=VertInLine*div(TStart0,MaxTH+MaxTH).quot+cdx;
		int dy0=QuotY*RealVLy;
		for(int v=vend;v>=vstart;v-=VertInLine){
			int yy=(mul3(GetTriY(v))>>2)-dy0;
			if(Mode3D)yy-=THMap[v]+AddTHMap(v);
			if(yy<miny){
				miny=yy;
				if(yy>=HiLine&&yy<=LoLine){
					int ofst=yy*RealVLx+xxx;
					VirtualScreenPointer[ofst]=254;
					VirtualScreenPointer[ofst+1]=254;
					if(yy<LoLine){
						ofst+=RealVLx;
						VirtualScreenPointer[ofst]=254;
						VirtualScreenPointer[ofst+1]=254;
					};
				};
			};
		};
	};
	DrawInBuffer(SVSC.VirtualScreenPointer,cx*SVSC.CellSX,cy*SVSC.CellSY,(cx+1)*SVSC.CellSX-1,(cy+cly)*SVSC.CellSY-1,
		(QuotX*SVSC.CellNX+cx)*SVSC.CellSX,(QuotY*SVSC.CellNY+cy)*SVSC.CellSY,SVSC.RealVLx);
	int Ny=(cly*SVSC.CellSY)>>5;
	int Ny0=SVSC.CellSY>>5;
	for(int i=0;i<Ny;i++)
		DrawOnePixCell(SVSC.VirtualScreenPointer,cx*SVSC.CellSX,cy*SVSC.CellSY+(i<<5),QuotX*SVSC.CellNX+cx,(QuotY*SVSC.CellNY+cy)*Ny0+i,SVSC.RealVLx);
	Sequrity();
};
void VirtualScreen::CreateTrianglesMapping(){
	int NELM=MaxTMX*MaxTMY;
	memset(TriangMap,0xFF,NELM<<2);
	memset(LoTriMap,0xFF,NELM<<2);
	/*
	for(int nx=0;nx<MaxTH*2;nx++){
		CreateVerticalTrianglesMapping(nx);
	};
	*/
};
int GetMaxTriY(int StartTri,int InsTri,bool Minimax){
	div_t ddt=div(StartTri,MaxTH*2);
	int StartVertex=ddt.quot*VertInLine+(ddt.rem>>1);
	int y1,y2,y3;
	if(Mode3D){
		switch(InsTri){
		case 0:
			y1=(mul3(GetTriY(StartVertex))>>2)-THMap[StartVertex]-AddTHMap(StartVertex);
			y2=(mul3(GetTriY(StartVertex+VertInLine))>>2)-THMap[StartVertex+VertInLine]-AddTHMap(StartVertex+VertInLine);
			y3=(mul3(GetTriY(StartVertex+VertInLine+1))>>2)-THMap[StartVertex+VertInLine+1]-AddTHMap(StartVertex+VertInLine+1);
			break;
		case 1:
			y1=(mul3(GetTriY(StartVertex))>>2)-THMap[StartVertex]-AddTHMap(StartVertex);
			y2=(mul3(GetTriY(StartVertex+1))>>2)-THMap[StartVertex+1]-AddTHMap(StartVertex+1);
			y3=(mul3(GetTriY(StartVertex+VertInLine+1))>>2)-THMap[StartVertex+VertInLine+1]-AddTHMap(StartVertex+VertInLine+1);
			break;
		case 2:
			y1=(mul3(GetTriY(StartVertex+1))>>2)-THMap[StartVertex+1]-AddTHMap(StartVertex+1);
			y2=(mul3(GetTriY(StartVertex+2))>>2)-THMap[StartVertex+2]-AddTHMap(StartVertex+2);
			y3=(mul3(GetTriY(StartVertex+VertInLine+1))>>2)-THMap[StartVertex+VertInLine+1]-AddTHMap(StartVertex+VertInLine+1);
			break;
		case 3:
			y1=(mul3(GetTriY(StartVertex+2))>>2)-THMap[StartVertex+2]-AddTHMap(StartVertex+2);
			y2=(mul3(GetTriY(StartVertex+VertInLine+2))>>2)-THMap[StartVertex+VertInLine+2]-AddTHMap(StartVertex+VertInLine+2);
			y3=(mul3(GetTriY(StartVertex+VertInLine+1))>>2)-THMap[StartVertex+VertInLine+1]-AddTHMap(StartVertex+VertInLine+1);
		};
	}else{
		switch(InsTri){
		case 0:
			y1=(mul3(GetTriY(StartVertex))>>2);
			y2=(mul3(GetTriY(StartVertex+VertInLine))>>2);
			y3=(mul3(GetTriY(StartVertex+VertInLine+1))>>2);
			break;
		case 1:
			y1=(mul3(GetTriY(StartVertex))>>2);
			y2=(mul3(GetTriY(StartVertex+1))>>2);
			y3=(mul3(GetTriY(StartVertex+VertInLine+1))>>2);
			break;
		case 2:
			y1=(mul3(GetTriY(StartVertex+1))>>2);
			y2=(mul3(GetTriY(StartVertex+2))>>2);
			y3=(mul3(GetTriY(StartVertex+VertInLine+1))>>2);
			break;
		case 3:
			y1=(mul3(GetTriY(StartVertex+2))>>2);
			y2=(mul3(GetTriY(StartVertex+VertInLine+2))>>2);
			y3=(mul3(GetTriY(StartVertex+VertInLine+1))>>2);
		};
	};
	if(Minimax){
		if(y1>y2&&y1>y3)return y1;
		if(y2>y1&&y2>y3)return y2;
		return y3;
	}else{
		if(y1<y2&&y1<y3)return y1;
		if(y2<y1&&y2<y3)return y2;
		return y3;
	};
};
void VirtualScreen::CreateVerticalTrianglesMapping(int VertSet){
	if(VertSet>=MaxTMX)return;
	int pos=VertSet;
	for(int i=0;i<MaxTMY;i++){
		TriangMap[pos]=-1;
		LoTriMap[pos]=-1;
		pos+=MaxTMX;
	};
	int VStart=((VertSet&65534)<<1)+(MaxTH-2)*MaxTH*2;
	int y1,yind;
	if(VertSet&1){
		for(int ny=MaxTH-2;ny>=0;ny--){
			y1=GetMaxTriY(VStart,3,true);
			yind=div(y1,CellSY).quot;
			if(yind<MaxTMY&&yind>=0)TriangMap[yind*MaxTMX+VertSet]=VStart+3;
			y1=GetMaxTriY(VStart,2,true);
			yind=div(y1,CellSY).quot;
			if(yind<MaxTMY&&yind>=0)TriangMap[yind*MaxTMX+VertSet]=VStart+2;
			VStart-=MaxTH*2;
		};
		VStart+=MaxTH*2;
		for(int ny=MaxTH-2;ny>=0;ny--){
			y1=GetMaxTriY(VStart,2,false);
			yind=div(y1,CellSY).quot;
			if(yind<MaxTMY&&yind>=0)LoTriMap[yind*MaxTMX+VertSet]=VStart+2;
			y1=GetMaxTriY(VStart,3,false);
			yind=div(y1,CellSY).quot;
			if(yind<MaxTMY&&yind>=0)LoTriMap[yind*MaxTMX+VertSet]=VStart+3;
			VStart+=MaxTH*2;
		};
	}else{
		for(int ny=MaxTH-2;ny>=0;ny--){
			y1=GetMaxTriY(VStart,0,true);
			yind=div(y1,CellSY).quot;
			if(yind<MaxTMY&&yind>=0)TriangMap[yind*MaxTMX+VertSet]=VStart;
			y1=GetMaxTriY(VStart,1,true);
			yind=div(y1,CellSY).quot;
			if(yind<MaxTMY&&yind>=0)TriangMap[yind*MaxTMX+VertSet]=VStart+1;
			VStart-=MaxTH*2;
		};
		VStart+=MaxTH*2;
		for(int ny=MaxTH-2;ny>=0;ny--){
			y1=GetMaxTriY(VStart,1,false);
			yind=div(y1,CellSY).quot;
			if(yind<MaxTMY&&yind>=0)LoTriMap[yind*MaxTMX+VertSet]=VStart+1;
			y1=GetMaxTriY(VStart,1,false);
			yind=div(y1,CellSY).quot;
			if(yind<MaxTMY&&yind>=0)LoTriMap[yind*MaxTMX+VertSet]=VStart;
			VStart+=MaxTH*2;
		};
	};
	Sequrity();
	//CheckSVSC(VertSet);
	//assert(_CrtCheckMemory());	
};
void VirtualScreen::CheckVLINE(int V){
	int v=V%MaxTMX;
	if(MarkedX[v]){
		CreateVerticalTrianglesMapping(v);
		MarkedX[v]=0;
	};
};
void UpdateDirtyPieces();
void VirtualScreen::Execute(){
	/*
	for(int i=0;i<MaxTMX;i++)if(MarkedX[i]){
		CreateVerticalTrianglesMapping(i);
		MarkedX[i]=0;
	};
	*/
	UpdateDirtyPieces();
#ifndef _USE3D
	RefreshSurface();
#endif
	CopyVSToScreen();
};
void VirtualScreen::Zero(){
	memset(VirtualScreenPointer,0xCD,RealVLx*RealVLy);
	CreateTrianglesMapping();
};
int GetHiDiff(int xx,int yy){
	int x=(xx<<4)+8;
	int y=(yy<<4)+8;
	int hi1=abs(GetHeight(x-16,y)-GetHeight(x+16,y));
	int hi2=abs(GetHeight(x,y-16)-GetHeight(x,y+16));
	if(abs(hi1)>abs(hi2))return hi1;else return hi2;
};
int GetBigHiDiff(int xx,int yy){
	int x=(xx<<4)+8;
	int y=(yy<<4)+8;
	int hi1=abs(GetHeight(x-32,y)-GetHeight(x+32,y));
	int hi2=abs(GetHeight(x,y-32)-GetHeight(x,y+32));
	if(abs(hi1)>abs(hi2))return hi1;else return hi2;
};
void SetLockPoint(int xx,int yy){
	int ddif=GetHiDiff(xx,yy);
	if(ddif>14&&GetBigHiDiff(xx,yy)>14)BSetPt(xx,yy);
	else BClrPt(xx,yy);
};
void VirtualScreen::RefreshTriangle(int i){
	//assert(_CrtCheckMemory());
	int ost=i%4096;
	div_t ddt=div(i,MaxTH*2);
	int sx=ddt.rem>>1;
	MarkedX[sx]=1;
	div_t sxdt=div(sx,CellNX);
	int Miny=div(GetMaxTriY(i&0xFFFFFFFC,i&3,false),CellSY).quot;
	int Maxy=div(GetMaxTriY(i&0xFFFFFFFC,i&3,true ),CellSY).quot-Miny+1;
	if(Miny<0)Miny=0;
	div_t sydt=div(Miny,CellNY);
	sx=sxdt.rem;
	int ofst=sx+sydt.rem*CellNX;
	int maxo=NCells;
	for(int dsy=0;dsy<Maxy;dsy++){
		if(CellQuotX[ofst]==sxdt.quot&&CellQuotY[ofst]==sydt.quot){
			if(ofst>=0&&ofst<maxo)CellFlags[ofst]=0;
		};
		sydt.rem++;
		if(sydt.rem>=CellNY){
			ofst=sx;
			sydt.rem=0;
			sydt.quot++;
		}else ofst+=CellNX;
	};


	//assert(_CrtCheckMemory());
	//Locking
	/*if(!(i&1)){
		div_t qq=div(i>>1,MaxTH);
		int xx=qq.rem<<1;
		int yy=qq.quot<<1;
		if(qq.rem&1){
			SetLockPoint(xx,yy);
			SetLockPoint(xx+1,yy);
			SetLockPoint(xx,yy+1);
			SetLockPoint(xx+1,yy+1);
		}else{
			SetLockPoint(xx,yy+1);
			SetLockPoint(xx+1,yy+1);
			SetLockPoint(xx,yy+2);
			SetLockPoint(xx+1,yy+2);
		};
	};
	*/
};
int CheckPt(int x,int y);
int GetNP(int x,int y){
	int np=0;
	if(CheckPt(x-1,y))np++;
	if(CheckPt(x+1,y))np++;
	if(CheckPt(x,y-1))np++;
	if(CheckPt(x,y+1))np++;
	if(CheckPt(x-1,y-1))np++;
	if(CheckPt(x-1,y+1))np++;
	if(CheckPt(x+1,y-1))np++;
	if(CheckPt(x+1,y+1))np++;
	return np;
};
void PrepareLandLocking(){
	int maxx=MaxTH<<1;
	int xx,yy, ix;
	for(ix=0;ix<MaxTH;ix++)
		for(int iy=0;iy<MaxTH;iy++){
			xx=iy+iy;
			yy=ix+ix;
			if(CheckPt(xx,yy)){
				int np=GetNP(xx,yy);
				if(np<=3)BClrPt(xx,yy);
			};
		};
	for(ix=0;ix<MaxTH;ix++)
		for(int iy=0;iy<MaxTH;iy++){
			xx=iy+iy+1;
			yy=ix+ix;
			if(CheckPt(xx,yy)){
				int np=GetNP(xx,yy);
				if(np<=3)BClrPt(xx,yy);
			};
		};
	for(ix=0;ix<MaxTH;ix++)
		for(int iy=0;iy<MaxTH;iy++){
			xx=iy+iy;
			yy=ix+ix+1;
			if(CheckPt(xx,yy)){
				int np=GetNP(xx,yy);
				if(np<=3)BClrPt(xx,yy);
			};
		};
	for(ix=0;ix<MaxTH;ix++)
		for(int iy=0;iy<MaxTH;iy++){
			xx=iy+iy+1;
			yy=ix+ix+1;
			if(CheckPt(xx,yy)){
				int np=GetNP(xx,yy);
				if(np<=3)BClrPt(xx,yy);
			};
		};
};
void VirtualScreen::RefreshScreen(){
	memset(MarkedX,1,MaxTMX);
	memset(CellFlags,0,CellNX*CellNY);
	memset(CellQuotX,0,NCells);
	memset(CellQuotY,0,NCells);
	memset(CellFlags,0,NCells);

	//for(int i=0;i<MaxTH*MaxTH*2;i++)RefreshTriangle(i);
	//softing surface
	int maxx=MaxTH<<1;
	//PrepareLandLocking();
	//PrepareLandLocking();
};
void VirtualScreen::ShowVerticalGrids(int QuotX,int QuotY,int cx,int cy,int cly){

};
void VirtualScreen::Sequrity(){
	int* tt=(int*)(VirtualScreenPointer-(RealVLx<<1));
	int nn=RealVLx>>1;
	//for(int i=0;i<nn;i++)assert(tt[i]==-1);
	tt=(int*)(VirtualScreenPointer+RealVLx*RealVLy);
	//for(int i=0;i<nn;i++)assert(tt[i]==-1);
};
int AddTHMap(int i){
	return (TexFlags[TexMap[i]]&8?0:word(randoma[word(i%8133)])&7);
};

#ifndef _USE3D
//----------------------Showing the single triangle---------------//
void ShowClippedTriangle_VS(int x,int y,byte* data){
	int dx=((int*)data)[1];
	int dy=((int*)data)[2];
	int x1=x+dx;
	int y1=y+dy;
	if(x1<0)return;
	int dofs=int(data);
	int realof=dofs+12;
	int NY=(((int*)data)[0])&65535;
	int DOfst=0;
	if(y1<WindY){
		int rdy=WindY-y1;
		if(NY<=rdy)return;
		NY-=rdy;
		y1=WindY;
		__asm{
			mov		ecx,rdy
			mov		ebx,realof
			mov		edx,DOfst
lopp1:		mov		ax,[ebx]
			movsx	eax,ax	
			add		edx,eax
			xor		eax,eax
			mov		ax,[ebx+2]
			add		ebx,eax
			add		ebx,4
			dec		ecx
			jnz		lopp1
			//test	dx,0x8000
			//jz		lopp2
			//or		edx,0xFFFF0000
			mov		DOfst,edx
			mov		realof,ebx
		};
	};
	if(y1+NY>WindY1)NY=WindY1-y1+1;
	if(NY<=0)return;
	int ofst=int(ScreenPtr)+y1*ScrWidth+x-dx;
	int vsx=((mapx<<5)+x-dx)%SVSC.RealVLx;
	int vsy=((mapy<<4)+y1)%SVSC.RealVLy;
	int vsptr=int(SVSC.VirtualScreenPointer)+vsx+vsy*SVSC.RealVLx;
	int dof=vsptr-ofst;
	int ddof=SVSC.RealVLx-ScrWidth;
	//int TempDI;
	//int  FinalESI;
	int MaxNY=SVSC.RealVLy-vsy;
	int SVSZ=SVSC.RealVLx*SVSC.RealVLy;
	__asm{
		push	esi
		push	edi
		pushf
		mov		edi,ofst
		add		edi,DOfst
		cld	
		mov		edx,realof
		xor		ecx,ecx
		xor		eax,eax
lpp1:	
		mov		ax,[edx]
		movsx   eax,ax

		add		edi,eax
		xor		ecx,ecx
		mov		cx,[edx+2]
		add     edx,4
		add     edx,ecx//
		inc     cx
			
		mov     ebx,edi
		add     edi,eax
		mov     esi,edi
		add     esi,dof

		rep movsb

		mov     edi,ebx
		add     edi,ScrWidth
		dec     MaxNY
		jnz     FFR1
		mov     MaxNY,1000
		mov     eax,SVSZ
		sub     dof,eax
FFR1:
		dec		NY
		jnz     lpp1

		popf
		pop		edi
		pop		esi
	};
};

#endif // !_USE3D