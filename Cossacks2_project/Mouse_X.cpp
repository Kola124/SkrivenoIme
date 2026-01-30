#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mapdiscr.h"
#include "mgraph.h"
#include "mode.h"
#include "GP_Draw.h"
#define MaxMX 32
#define MsizeX 32
extern int CurrentCursorGP;
extern int SCRSizeX;
extern int SCRSizeY;
extern int RSCRSizeX;
extern int RSCRSizeY;
extern int COPYSizeX;
RLCTable mptr;
bool realLpressed;
bool realRpressed;
int curptr;
int mouseX;
int	mouseY;
bool Lpressed;
bool Rpressed;
static char buf1[1024];
static char buf2[1024];
static char buf1o[1024];
static char buf2o[1024];
static int OldMX;
static int OldMY;
static int MX;
static int MY;
static bool LockMouse;
extern bool xLockMouse;
int curdx;
int curdy;
bool MNotRemoved;
typedef unsigned short word;
int GetF(word k);
//extern word fmap[256][256];
extern int mapx;
extern int mapy;
extern int smapx;
extern int smapy;

#ifdef _USE3D
extern IRenderSystem* IRS;


#include "..\3DLIB\gMotor\kThread.h"

class MouseThread : public Thread
{
	int nTimer;
public:
	MouseThread() : nTimer(0), Thread(thtMessagePump) {}
	DWORD OnStart()
	{
		nTimer = SetTimer( 0, 0, 33, 0 );
		return 0;
	}

	DWORD OnMessage( const MSG& msg )
	{
		if (msg.message == WM_TIMER && msg.wParam == nTimer)
		{
			ShowCursor( TRUE );
			POINT pt;
			GetCursorPos( &pt );
			if (IRS) IRS->SetCursorPosition( pt.x, pt.y, true );
		}
		return 0;
	}

}; // class MouseThread

#endif // _USE3D

void CBar(int x,int y,int Lx,int Ly,byte c);
//retreives data from the screen buffer to field 32x32
#ifdef MAKE_PTC
void GetMData(void* dest,void* src,int x,int y,int SSizeX,int SSizeY){
	if(!bActive)return;
	int Lx=32;
	int Ly=32;
	int x1=x;
	int y1=y;
	int bx=0;//x-coord. on bitbap 32x32
	int by=0;
	if(x1<0){
		bx=-x1;
		Lx+=x1;
		x1=0;
	};
	if(y1<0){
		by=-y1;
		Ly+=y1;
		y1=0;
	};
	if(x1+32>SSizeX)Lx=SSizeX-x1;
	if(y1+32>SSizeY)Ly=SSizeY-y1;
	if(Lx<=0||Ly<=0)return;
	int sofs=int(src)+x1+y1*SSizeX;
	int dofs=int(dest)+bx+(by<<5);
	int Lx4=Lx>>2;
	int Lx1=Lx&3;
	int adds=SSizeX-Lx;
	int addd=32-Lx;
	__asm{
		push	esi
		push	edi
		mov		edx,Ly
		mov		esi,sofs
		mov		edi,dofs
		cld
lpp1:	mov		ecx,Lx4
		jcxz	lpp2
		rep		movsd
lpp2:	mov		ecx,Lx1
		jcxz	lpp3
		rep		movsb
lpp3:	add		esi,adds
		add		edi,addd
		dec		edx
		jnz		lpp1
		pop		edi
		pop		esi
	};
};
bool CmpMData(void* dest,void* src,int x,int y,int SSizeX,int SSizeY){
	int Lx=32;
	int Ly=32;
	int x1=x;
	int y1=y;
	int bx=0;//x-coord. on bitbap 32x32
	int by=0;
	if(x1<0){
		bx=-x1;
		Lx+=x1;
		x1=0;
	};
	if(y1<0){
		by=-y1;
		Ly+=y1;
		y1=0;
	};
	if(x1+32>SSizeX)Lx=SSizeX-x1;
	if(y1+32>SSizeY)Ly=SSizeY-y1;
	if(Lx<=0||Ly<=0)return false;
	int sofs=int(src)+x1+y1*SSizeX;
	int dofs=int(dest)+bx+(by<<5);
	int Lx4=Lx>>2;
	int Lx1=Lx&3;
	int adds=SSizeX-Lx;
	int addd=32-Lx;
	bool notequal=false;
	__asm{
		push	esi
		push	edi
		mov		edx,Ly
		mov		esi,sofs
		mov		edi,dofs
		cld
lpp1:	mov		ecx,Lx4
		jcxz	lpp2
		repe	cmpsd
		jne		noteq
lpp2:	mov		ecx,Lx1
		jcxz	lpp3
		repe	cmpsb
		jne		noteq
lpp3:	add		esi,adds
		add		edi,addd
		dec		edx
		jnz		lpp1
		jmp		lpp4
noteq:	mov		notequal,1
lpp4:	pop		edi
		pop		esi
	};
	return notequal;
};
void RestoreMData(void* scrn,void* buf,void* comp,int x,int y,int SSizeX,int SSizeY){
	if(!bActive)return;
	int Lx=32;
	int Ly=32;
	int x1=x;
	int y1=y;
	int bx=0;//x-coord. on bitbap 32x32
	int by=0;
	if(x1<0){
		bx=-x1;
		Lx+=x1;
		x1=0;
	};
	if(y1<0){
		by=-y1;
		Ly+=y1;
		y1=0;
	};
	if(x1+32>SSizeX)Lx=SSizeX-x1;
	if(y1+32>SSizeY)Ly=SSizeY-y1;
	if(Lx<=0||Ly<=0)return;
	int src1=int(buf)+bx+(by<<5);
	int srcom=int(comp)+bx+(by<<5);
	int scrof=int(scrn)+x1+y1*SSizeX;
	int addscr=SSizeX-Lx;
	int add32=32-Lx;
	__asm{
		push	esi
		push	edi
		mov		edx,Ly
		mov		esi,src1
		mov		ebx,srcom
		mov		edi,scrof
		cld
lpp0:	mov		ecx,Lx
lpp1:	lodsb	
		mov		ah,[edi]
		cmp		ah,[ebx]
		jnz		lpp2
		mov		[edi],al
lpp2:	inc		edi
		inc		ebx
		dec		ecx
		jnz		lpp1
		add		edi,addscr
		add		ebx,add32
		add		esi,add32
		dec		edx
		jnz		lpp0
		pop		edi
		pop		esi
	};
};
void LoadPointer(LPCSTR s){
	LoadRLC(s,&mptr);
	curptr=0;
	//mouseX=0;
	//mouseY=0;
	LockMouse=false;
	memset(buf1,0,sizeof buf1);
	memset(buf2,0,sizeof buf2);
	//MouseOn=false;
	//Transferred=true;
	//curdx=5;
	//curdy=5;
	curdx=0;
	curdy=0;
};
extern int RealLx;
extern int RealLy;
void SetMPtr(int x,int y,int key){
	if(x>RealLx-1)x=RealLy-1;
	if ((x!=mouseX)||(y!=mouseY)){
		mouseX=x;
		mouseY=y;
		//Transferred=false;
		//Transferreda=false;
		realLpressed=key&MK_LBUTTON;
		realRpressed=key&MK_RBUTTON;
	};
};
//Redraw mouse in the offscreen buffer
//and prepare data for onscreen transferring 
int MousDX[256];
int MousDY[256];
int MouseNext[256];
bool Loaded=0;
void RedrawOffScreenMouse(){
	if(!bActive)return;
	curdx=MousDX[curptr];
	curdy=MousDY[curptr];

#ifdef _USE3D
	static int mouseCursorWidth;
	static int mouseCursorHeight;
#endif // _USE3D

	if(!Loaded){
		GFILE* F=Gopen("curs_xy.dat","r");
		if(F){
			int z;
			int p=0;
			do{
				z=Gscanf(F,"%d%d%d",&MousDX[p],&MousDY[p],&MouseNext[p]);
				p++;
			}while(z==3&&p<256);
			Gclose(F);
		}else{
			memset(MousDX,0,sizeof MousDX);
			memset(MousDY,0,sizeof MousDY);
			for(int i=0;i<16;i++)MouseNext[i]=i;
			for(int i=0;i<16;i++)MouseNext[i+16]=i+17;
			MouseNext[31]=16;
		};
		Loaded=1;
	};
	if(xLockMouse)return;
	LockMouse=true;
	MX=mouseX-curdx;
	MY=mouseY-curdy;
#ifdef _USE3D

	
	static bool cursorLoaded = false;
	static MouseThread		mthread;
		
	if (IRS && !cursorLoaded)
	{
		IRS->SetCursorBitmap( "tipaCursor.tga" );  
		cursorLoaded = true;
		mthread.SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );
		//mthread.Run();
	}

	if (IRS) GPS.ShowGP(MX,MY,CurrentCursorGP,curptr,0);

/*
	ShowCursor( FALSE );
	SetCursor( NULL );

	static int lastX = 0, lastY = 0;


	if (abs(lastX - mouseX) > 2 || abs(lastY - mouseY) > 2)
	{
		if (IRS) IRS->SetCursorPosition( mouseX, mouseY, true );
	}
	
	lastX = mouseX;
	lastY = mouseY;

	/*
	GPBatchMode old = GPS.SetBatchMode( bmDrawImmediately );
	extern float g_dbgZ;
	g_dbgZ = 0.0001f;
	
	//GPS.ShowGP( MX, MY, GPS.PreLoadGPImage( "tipaCursor" ), 0, 0 );
	
	GPS.SetBatchMode( old );
/*
	mouseCursorWidth = GPS.GetGPWidth( CurrentCursorGP, curptr );
	mouseCursorHeight = GPS.GetGPHeight( CurrentCursorGP, curptr );

	RECT mrect;
	mrect.left	= MX;
	mrect.top	= MY;
	mrect.right = MX + mouseCursorWidth;
	mrect.bottom= MY + mouseCursorHeight;
	
	if (IRS) IRS->PresentBackBuffer( &mrect );
	*/
	return;
#endif //_USE3D
try{
	RestoreMData(ScreenPtr,(void*)buf1,(void*)buf2,OldMX,OldMY,SCRSizeX,SCRSizeY);
	GetMData((void*)buf1,ScreenPtr,MX,MY,SCRSizeX,SCRSizeY);
	//ShowRLCItem(MX+32,MY,&mptr,curptr+4096,0);
	//ShowRLCItem(MX,MY,&mptr,curptr,0);
	GPS.ShowGP(MX,MY,CurrentCursorGP,curptr,0);
	GetMData((void*)buf2,ScreenPtr,MX,MY,SCRSizeX,SCRSizeY);
}catch(...){};
};
//extern bool test16;
void RedrawScreenMouse(){
#ifdef _USE3D
	return;
#endif //_USE3D
	if(!bActive)return;
	if(xLockMouse)return;
    if (bActive || window_mode)//BUGFIX: Cursor shadow trail while showing ingame menues
    {
        return;
    }
try{
	RestoreMData(RealScreenPtr,(void*)buf1o,(void*)buf2o,OldMX,OldMY,RSCRSizeX,RSCRSizeY);
	GetMData((void*)buf1o,RealScreenPtr,MX,MY,RSCRSizeX,RSCRSizeY);
	void* osp=ScreenPtr;
	int osx=SCRSizeX;
	int sw=ScrWidth;
	ScrWidth=RSCRSizeX;
	ScreenPtr=RealScreenPtr;
	SCRSizeX=RSCRSizeX;
	//ShowRLCItem(MX+32,MY,&mptr,curptr+4096,0);
	//ShowRLCItem(MX,MY,&mptr,curptr,0);
	GPS.ShowGP(MX,MY,CurrentCursorGP,curptr,0);
	SCRSizeX=osx;
	ScreenPtr=osp;
	ScrWidth=sw;
	GetMData((void*)buf2o,RealScreenPtr,MX,MY,RSCRSizeX,RSCRSizeY);
}catch(...){};
};
extern bool InVideo;
void OnMouseMoveRedraw(){
#ifndef _USE3D
	if(InVideo||xLockMouse||LockMouse)return;
#endif // !_USE3D
	RedrawOffScreenMouse();
	RedrawScreenMouse();
	LockMouse=false;
	OldMX=MX;
	OldMY=MY;
};
void PostRedrawMouse(){
#ifdef _USE3D
	return;
#endif //_USE3D
	if(xLockMouse)return;
try{
	bool need=true;
	if(MX==OldMX&&MY==OldMY)
		need=CmpMData((void*)buf2o,RealScreenPtr,MX,MY,RSCRSizeX,RSCRSizeY);
	if(need){
		RestoreMData(RealScreenPtr,(void*)buf1,(void*)buf2,MX,MY,RSCRSizeX,RSCRSizeY);
		RestoreMData(RealScreenPtr,(void*)buf1o,(void*)buf2o,OldMX,OldMY,RSCRSizeX,RSCRSizeY);
		GetMData((void*)buf1o,RealScreenPtr,MX,MY,RSCRSizeX,RSCRSizeY);
		void* osp=ScreenPtr;
		int osx=SCRSizeX;
		int sw=ScrWidth;
		ScrWidth=RSCRSizeX;
		ScreenPtr=RealScreenPtr;
		SCRSizeX=RSCRSizeX;
		//WindX=120;
		//WindX1=550;
		//ShowRLCItem(MX+32,MY,&mptr,curptr+4096,0);
		//ShowRLCItem(MX,MY,&mptr,curptr,0);
		GPS.ShowGP(MX,MY,CurrentCursorGP,curptr,0);
		SCRSizeX=osx;
		ScreenPtr=osp;
		ScrWidth=sw;
		GetMData((void*)buf2o,RealScreenPtr,MX,MY,RSCRSizeX,RSCRSizeY);
		OldMX=MX;
		OldMY=MY;
	};
}catch(...){};
	LockMouse=false;
};
#else
void GetMData(void* dest,void* src,int x,int y,int SSizeX,int SSizeY){
};
bool CmpMData(void* dest,void* src,int x,int y,int SSizeX,int SSizeY){
	return false;
};
void RestoreMData(void* scrn,void* buf,void* comp,int x,int y,int SSizeX,int SSizeY){
};
void LoadPointer(LPCSTR s){
};
void SetMPtr(int x,int y,int key){
};
void RedrawOffScreenMouse(){
};
void RedrawScreenMouse(){
};
void OnMouseMoveRedraw(){
};
void PostRedrawMouse(){
};
#endif