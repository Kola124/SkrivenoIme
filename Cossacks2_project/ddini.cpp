/***********************************************************************
 *Direct Draw initialisation module                    
 *
 * This module creates the Direct Draw object with the primary surface
 * and a backbuffer and sets 800x600x8 display mode.
 *
 ***********************************************************************/
#define __ddini_cpp_
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h" 
#include "mode.h" 
#include "MapDiscr.h" 
#include "fog.h"
#include "GSound.h"
#include "fonts.h" 
#include "VirtScreen.h"    
 
void DDLog (LPSTR sz,...)
{
	
        char ach[256];
        va_list va;

        va_start( va, sz );
        vsprintf ( ach, sz, va );   
        va_end( va );
		FILE* f=fopen("DDraw.log","a");
		fprintf(f,ach);
		fclose(f);
	
};
 
#ifdef _USE3D

#include "GP_Draw.h"

IRenderSystem*	IRS;
float			g_dbgZ;
int				VBUF;
 
IRenderSystem* GetRenderSystemDX();

#pragma pack(push)
#pragma pack(8)
extern GP_System GPS;
#pragma pack(pop)
#endif

void Rept (LPSTR sz,...);
CEXPORT int ModeLX[32];
CEXPORT int ModeLY[32];
CEXPORT int NModes=0;
void SERROR();
void SERROR1();
void SERROR2();
void PropCopy();
void InitRLCWindows();
//#define COPYSCR
const int InitLx=1024;
const int InitLy=768;
CEXPORT int RealLx;
CEXPORT int RealLy;
CEXPORT int SCRSizeX;
CEXPORT int SCRSizeY;
CEXPORT int RSCRSizeX;
CEXPORT int RSCRSizeY;
CEXPORT int COPYSizeX;
CEXPORT int Pitch;
LPDIRECTDRAW            lpDD=NULL;      // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
BOOL                    bActive;        // is application active?
BOOL                    CurrentSurface; //=FALSE if backbuffer
                                        // is active (Primary surface is visible)
										//=TRUE if  primary surface is active
										// (but backbuffer is visible)
BOOL                    DDError;        //=FALSE if Direct Draw works normally 
DDSURFACEDESC           ddsd;
PALETTEENTRY            GPal[256];
LPDIRECTDRAWPALETTE     lpDDPal;

extern bool PalDone;
extern word PlayerMenuMode;
typedef struct zzz{			
	BITMAPINFO bmp;
	PALETTEENTRY XPal[255];
};
CEXPORT
byte GetPaletteColor(int r,int g,int b){
	int dmax=10000;
	int bestc=0;
	for(int i=0;i<256;i++){
		int d=abs(r-GPal[i].peRed)+abs(g-GPal[i].peGreen)+abs(b-GPal[i].peBlue);
		if(d<dmax){
			dmax=d;
			bestc=i;
		};
	};
	return bestc;
};
zzz xxt;
//typedef byte barr[ScreenSizeX*ScreenSizeY];
void* offScreenPtr;
/*
 * Flipping Pages
 */

//extern int BestBPP;
extern int SCRSZY;
void ClearRGB(){
	if(!bActive)return;
	memset(RealScreenPtr,0,RSCRSizeX*SCRSZY);
};
//#define TEST16
#ifndef TEST16
bool test16=1;
#endif
//#ifdef TEST16
//bool test16=1;
word PAL16[256];
int P16Idx=-1;
extern int CurPalette;
void CheckPal16(){
	//if(CurPalette!=P16Idx&&CurPalette==0){
		P16Idx=CurPalette;
		for(int i=0;i<256;i++)PAL16[i]=(GPal[i].peBlue>>3)+((GPal[i].peGreen>>2)<<5)+((GPal[i].peRed>>3)<<11);
	//};
};
int NATT=10;
void CheckPal16x(){
	for(int i=0;i<256;i++)PAL16[i]=(GPal[i].peBlue>>3)+((GPal[i].peGreen>>2)<<5)+((GPal[i].peRed>>3)<<11);
};
void ChangeColorFF(){
	if(!RealScreenPtr)return;
	try{
		int DD=10000;
		int c=0xFF;
		for(int i=0;i<255;i++){
			int D=255+255+255-GPal[i].peBlue-GPal[i].peGreen-GPal[i].peRed;
			if(D<DD){
				c=i;
				DD=D;
			};
		};
		int sz=RSCRSizeX*RealLy;
		for(int i=0;i<sz;i++)((byte*)RealScreenPtr)[i]=0;//if(((byte*)RealScreenPtr)[i]==0xFF)((byte*)RealScreenPtr)[i]=c;
	}catch(...){};
};
void Copy16(byte* Src,int SrcPitch,byte* Dst,int DstPitch,int Lx,int Ly){
	CheckPal16x();
	int DXR=DstPitch-(Lx<<1);
	int XDX=(SrcPitch-Lx);
	int NY=Ly;
	int NX=Lx>>1;
	__asm{
		push esi
		push edi
		mov  esi,Src
		mov  edi,Dst
LPP0:
		mov  ecx,NX
		xor  eax,eax
LPP1:
		mov  bx,[esi]
		mov  al,bl
		mov  dx,[PAL16+eax*2];
		rol  edx,16
		mov  al,bh
		mov  dx,[PAL16+eax*2];
		rol  edx,16
		mov  [edi],edx
		add  edi,4
		add  esi,2
		dec  ecx
		jnz  LPP1
		add  edi,DXR
		add  esi,XDX
		dec  NY
		jnz  LPP0
		pop  edi
		pop  esi
	}
};
//#endif
CEXPORT
void FlipPages(void)
{

#ifdef _USE3D
	void Test3D();
	Test3D();
	float fps = Stats::GetFPS(); 
	char strfps[128];
	sprintf( strfps, "fps:% 3.f", fps );
	ShowString( 5, 5, strfps, &BlackFont );
	ShowString( 4, 4, strfps, &WhiteFont );
	GPS.OnFrame();
	IRS->OnFrame();
	IRS->ClearDeviceZBuffer();
	return;
#endif // _USE3D

	if(!bActive)return;
	if (window_mode){
		if(PlayerMenuMode!=1){
		//	ProcessFog();
		//	ShowFoggedBattle();
		};
        HDC WH = GetDC(hwnd);
		//memcpy(xxt.XPal,&GPal[1],sizeof xxt.XPal);
		for(int i=0;i<256;i++){
			xxt.bmp.bmiColors[i].rgbRed=GPal[i].peRed;
			xxt.bmp.bmiColors[i].rgbBlue=GPal[i].peBlue;
			xxt.bmp.bmiColors[i].rgbGreen=GPal[i].peGreen;
		};
		xxt.bmp.bmiHeader.biSize=sizeof BITMAPINFOHEADER;
		xxt.bmp.bmiHeader.biWidth=SCRSizeX;
		xxt.bmp.bmiHeader.biHeight=-SCRSizeY;
		xxt.bmp.bmiHeader.biPlanes=1;
		xxt.bmp.bmiHeader.biBitCount=8;
		xxt.bmp.bmiHeader.biCompression=BI_RGB;
		xxt.bmp.bmiHeader.biSizeImage=0;

		int z=StretchDIBits(WH,
            0,0,
            COPYSizeX,RSCRSizeY,
			0,MaxSizeY-RSCRSizeY,
            COPYSizeX,RSCRSizeY,
            RealScreenPtr,
            &xxt.bmp,
			DIB_RGB_COLORS,SRCCOPY);
		ReleaseDC(hwnd,WH);

		//StretchDIBits(WH,smapx,smapy,smaplx*32,smaply*32,
		//	smapx,smapy,smaplx*32,smaply*32,RealScreenPtr,&xxt.bmp,
		//	DIB_RGB_COLORS,SRCCOPY);
		return;
	};	
#ifdef COPYSCR
	/*__asm{
		push	esi
		push	edi
		mov		esi,ScreenPtr
		mov		edi,RealScreenPtr
		mov		ecx,120000
		cld
		rep		movsd
		pop		edi
		pop		esi
	}*/
	//if(PlayerMenuMode==1){
	//return;
	{
#ifdef TEST16
		Copy16();
		return;
#endif
		int ofs=0;
		int	lx=COPYSizeX>>2;
		//int	ly=SCRSizeY;
		int	ly=RealLy;
		int	addOf=SCRSizeX-(lx<<2);
		int RaddOf=RSCRSizeX-(lx<<2);

		__asm{
			push	esi
			push	edi	
			mov		esi,ScreenPtr
			mov		edi,RealScreenPtr
			add		esi,ofs
			add		edi,ofs
			cld
			mov		eax,ly
xxx:
			mov		ecx,lx
			rep		movsd
			add		esi,addOf
			add		edi,RaddOf
			dec		eax
			jnz		xxx
		};
		return;
	};
	
	int ofs=smapx+smapy*SCRSizeX;
	int ofs1=smapx+smapy*RSCRSizeX;
	int	lx=smaplx<<3;
	int	ly=smaply<<5;
	if(lx==0)lx=800;
	if(ly==0)ly=600;
	int	addOf=SCRSizeX-(lx<<2);
	int		RaddOf=RSCRSizeX-(lx<<2);;
	__asm{
		push	esi
		push	edi
		mov		esi,ScreenPtr
		mov		edi,RealScreenPtr
		add		esi,ofs
		add		edi,ofs1
		cld
		mov		eax,ly
xxxx:
		mov		ecx,lx
		rep		movsd
		add		esi,addOf
		add		edi,RaddOf
		dec		eax
		jnz		xxxx
	};
	/*
	ofs=minix+miniy*SCRSizeX;
	ofs1=minix+miniy*RSCRSizeX;
	lx=msx>>3;
	ly=msy>>1;
	addOf=SCRSizeX-(lx<<2);
	RaddOf=RSCRSizeX-(lx<<2);
	__asm{
		push	esi
		push	edi
		mov		esi,ScreenPtr
		mov		edi,RealScreenPtr
		add		esi,ofs
		add		edi,ofs1
		cld
		mov		eax,ly
yyy:
		mov		ecx,lx
		rep		movsd
		add		esi,addOf
		add		edi,RaddOf
		dec		eax
		jnz		yyy
	};
	*/
	//PropCopy();
	return;
#else

	if(DDDebug) return;
	CurrentSurface=!CurrentSurface;
	while( 1 )
    {
        HRESULT ddrval;
        ddrval = lpDDSPrimary->Flip( NULL, 0 );
        if( ddrval == DD_OK )
        {
            break;
        }
        if( ddrval == DDERR_SURFACELOST )
        {
            ddrval = lpDDSPrimary->Restore();
            if( ddrval != DD_OK )
            {
                break;
            }
        }
        if( ddrval != DDERR_WASSTILLDRAWING ) 
        { 
                break;
        }
    }
	LockSurface();
	UnlockSurface();
#endif
}
/*
 * Getting Screen Pointer
 *
 * You will ge the pointer to the invisible area of the screen
 * i.e, if primary surface is visible, then you will obtain the
 * pointer to the backbuffer.
 * You must call UnlockSurface() to allow Windows draw on the screen
 */
int SCRSZY=0;
void LockSurface(void)
{
	FILE* LR=fopen("lock.report","w");
	if(LR)fprintf(LR,"DDError:%d\n",DDError);
	long dderr=0;
	if (window_mode)
	{
		ScreenPtr=(void*)(int(offScreenPtr)+MaxSizeX*32);
		ddsd.lpSurface=ScreenPtr;
		RealScreenPtr=ScreenPtr;
		return;
	}
	if (DDError){
		if(LR)fclose(LR);
		return;
	};
#ifdef COPYSCR
	if ((dderr=lpDDSPrimary->Lock(NULL,&ddsd,
							    DDLOCK_SURFACEMEMORYPTR|
								DDLOCK_WAIT,NULL))!=DD_OK) DDError=true;//true ;
	DDLog("lpDDSPrimary->Lock:%d\n",dderr);
	DDLog("Lock result:%d\n",dderr);
	DDLog("ptr:%d pitch=%d ly=%d\n",ddsd.lpSurface,ddsd.lPitch,ddsd.dwHeight);
	RSCRSizeX=ddsd.lPitch;
#else
	if ((dderr=lpDDSBack->Lock(NULL,&ddsd,
							    DDLOCK_SURFACEMEMORYPTR|
								DDLOCK_WAIT,NULL))!=DD_OK) DDError=0;//true ;
	RSCRSizeX=ddsd.lPitch;
#endif
#ifdef COPYSCR
	ScreenPtr=(void*)(int(offScreenPtr)+MaxSizeX*32);
	//ddsd.lpSurface=ScreenPtr;
	RealScreenPtr=ScreenPtr;
	RealScreenPtr=ddsd.lpSurface;
	SCRSZY=ddsd.dwHeight;
	//Rept("RealPtr:%d\n",int(ddsd.lpSurface));
	ClearScreen();
#else
	ScreenPtr=ddsd.lpSurface;
	offScreenPtr=ScreenPtr;
	if (lpDDSPrimary->Lock(NULL,&ddsd,
							    DDLOCK_SURFACEMEMORYPTR|
								DDLOCK_WAIT,NULL)!=DD_OK) DDError=0;//true ;
	RealScreenPtr=ScreenPtr;
#endif
	if(LR)fclose(LR);
}
/*
 *  Unlocking the surface 
 *
 *  You must unlock the Video memory for Windows to work properly
 */
void UnlockSurface(void)
{
	if(window_mode) return;
	if (DDError)  return;
	//Back Buffer is active
#ifdef COPYSCR
	if (lpDDSPrimary->Unlock(NULL)!=DD_OK) DDError=true ;
#else
	if (lpDDSBack->Unlock(NULL)!=DD_OK) DDError=true ;
	if (lpDDSPrimary->Unlock(NULL)!=DD_OK) DDError=true ;
#endif
}
/*
 * Getting then DC of the active (invisible) area of the screen
 */
HDC GetSDC(void)
{
	if(window_mode) return 0;
	HDC hdc;
	if (DDError) return 0;
	if (CurrentSurface)
	{
		//Back Buffer is active
		if (lpDDSPrimary->GetDC(&hdc)!=DD_OK) DDError=true ;
	}else
	{
		//Primary Surface is active
		if (lpDDSBack->GetDC(&hdc)!=DD_OK) DDError=true;
	}
	return hdc;
}
/*
 * Timer Callback 
 */
bool m640_16=0;
bool m640_24=0;
bool m640_32=0;
bool m1024_768=0;
int BestVX=640;
int BestVY=480;
int BestBPP=32;
HRESULT CALLBACK ModeCallback(LPDDSURFACEDESC pdds, LPVOID lParam)
{
	//Rept("ModeCallBack\n");

    /*int width = pdds->dwWidth;
    int height = pdds->dwHeight;
    int bpp    = pdds->ddpfPixelFormat.dwRGBBitCount;
	if(width==640&&height==480){
		if(bpp==32)m640_32=1;
		if(bpp==24)m640_24=1;
		if(bpp==16)m640_16=1;
	};
	if(bpp==8&&NModes<32&&width>=1024){//1024){
		if(width==1024&&height==768)m1024_768=1;
		ModeLX[NModes]=width;
		ModeLY[NModes]=height;
		NModes++;
		//Rept("AddMode: %dx%d \n",width,height);
	};*/

    if (1024 > pdds->dwWidth || 768 > pdds->dwHeight)
    {//Don't allow for resolutions less than 1024 x 768 ot bigger than 1920x[...]
        return S_FALSE;
    }

    /*if (1920 < pdds->dwWidth)
    {//Also disable all resolutions above ~1920 px wide for fairness reasons
        return S_FALSE;
    }*/

    if (32 == pdds->ddpfPixelFormat.dwRGBBitCount)
    {
        ModeLX[NModes] = pdds->dwWidth;
        ModeLY[NModes] = pdds->dwHeight;
        NModes++;
    }

    //return S_TRUE to stop enuming modes, S_FALSE to continue
    return S_FALSE;
}
bool EnumModesOnly(){
	//HRESULT ddrval = DirectDrawCreate( NULL, &lpDD, NULL );
    HRESULT ddrval = DirectDrawCreate_wrapper( NULL, &lpDD, NULL );

	if(ddrval==DD_OK){

        lpDD->EnumDisplayModes(0, NULL, NULL, ModeCallback);
        lpDD->Release();
        lpDD = NULL;
        
        /*lpDD->EnumDisplayModes(0, NULL, NULL, ModeCallback);
		lpDD->Release();
		lpDD=NULL;
		if(m640_32)BestBPP=32;
		else if(m640_24)BestBPP=24;
		else if(m640_16)BestBPP=16;
		if(!m1024_768){
			if(MessageBox(hwnd,"Dilplay mode 1024x768x8 not found. Cossacks should not run.","Loading error",MB_RETRYCANCEL)!=IDRETRY)
				exit(0);
		};*/


		return true;
    }
    else{
		MessageBox(hwnd,"Unable to initialise Direct Draw. Cossacks should not run.","Loading error",MB_ICONSTOP);
		exit(0);
	};
};

void DelLog(){
	DeleteFile("DDraw.log");
};
bool CreateDDObjects(HWND hwnd)
{

#ifdef _USE3D

	IRS = GetRenderSystemDX(); 
	assert( IRS );

	IRS->Init( hwnd );

	if (!DDDebug)
	{
		ScreenProp sp = IRS->GetScreenProperties();
		sp.fullScreen = true;
		IRS->SetScreenProperties( sp );
	}

	GPS.Init( IRS );

	void InitGroundZbuffer();
	InitGroundZbuffer();
	DDError=false;
	SCRSizeX=MaxSizeX;
	SCRSizeY=MaxSizeY;
	RSCRSizeX=RealLx;
	//Pitch=ddsd.lPitch;
	COPYSizeX=RealLx;
	RSCRSizeY=RealLy;
	ScrHeight=SCRSizeY;
	ScrWidth=SCRSizeX;

#ifdef _USE3D
	GPS.SetClipArea( 0, 0, RealLx, RealLy );
#else
	WindX=0;
	WindY=0;
	WindLx=RealLx;
	WindLy=RealLy;
	WindX1=WindLx-1;
	WindY1=WindLy-1;
#endif // _USE3D
	
	BytesPerPixel=2;

	return true;
#endif // _USE3D
	HRESULT ddrval;
	DDSCAPS ddscaps;
	char    buf[256];
	DDError=false;
	CurrentSurface=true;
	if (window_mode)
	{
	
        SVSC.SetSize(RealLx, RealLy);
        DDError = false;
        SCRSizeX = MaxSizeX;
        SCRSizeY = MaxSizeY;
        COPYSizeX = RealLx;
        RSCRSizeX = RealLx;
        RSCRSizeY = RealLy;
        ScrHeight = SCRSizeY;
        ScrWidth = SCRSizeX;
		InitRLCWindows();
#ifndef _USE3D
		WindX=0;
		WindY=0;
		WindLx=RealLx;
		WindLy=RealLy;
		WindX1=WindLx-1;
		WindY1=WindLy-1;
#else
		GPS.SetClipArea( 0, 0, RealLx, RealLy );
#endif // _USE3D
		BytesPerPixel=1;
		offScreenPtr=(malloc(SCRSizeX*(SCRSizeY+32*4)));

        const int screen_width = GetSystemMetrics(SM_CXSCREEN);
        const int screen_height = GetSystemMetrics(SM_CYSCREEN);

        const int ModeLX_candidates[] = { 1024, 1152, 1280, 1280, 1366, 1600, 1920 };
        const int ModeLY_candidates[] = { 768,  864,  720, 1024,  768,  900, 1080 };

        NModes = 0;
        for (int i = 0; i < 8; i++)
        {
            //Only show resolutions up to current screen resolution
            if (ModeLX_candidates[i] <= screen_width
                && ModeLY_candidates[i] <= screen_height)
            {
                ModeLX[i] = ModeLX_candidates[i];
                ModeLY[i] = ModeLY_candidates[i];
                NModes++;
            }
        }

		/*NModes = 2;
		ModeLX[0]=800;
		ModeLY[0]=600;
		ModeLX[1]=1024;
		ModeLY[1]=768;*/

		return true;
	}
#ifdef COPYSCR

	//offScreenPtr=offScreenPtr=(malloc(MaxSizeX*(MaxSizeY+32*4)));
	SVSC.SetSize(RealLx,RealLy);
	offScreenPtr=offScreenPtr=(malloc(MaxSizeX*(MaxSizeY+32*4)));
#endif
	if(lpDD){
		lpDDSPrimary->Release();
		goto SDMOD;
	};
	lpDD=NULL;

	/*ddrval = DirectDrawCreate(NULL, &lpDD, NULL);
	{
		for(int j=0;j<NModes;j++){
			DDLog("%dx%dx8\n",ModeLX[j],ModeLY[j]);
		};
	};*/

    ddrval = DirectDrawCreate_wrapper(NULL, &lpDD, NULL);

	DDLog("DirectDrawCreate:%d\n",ddrval);
    if( ddrval == DD_OK )
    {
		//Rept("DD_OK\n");
        // Get exclusive mode
SDMOD:;
        ddrval = lpDD->SetCooperativeLevel( hwnd,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
		DDLog("SetCooperativeLevel:%d\n",ddrval);
        if(ddrval == DD_OK )
        {
			//Rept("SetCoopLevel: DD_OK\n");
#ifndef _USE3D
		ShowCursor(false);
#endif // !_USE3D

			//Rept("SetDisplayMode(%d,%d)\n",RealLx,RealLy);
            ddrval = lpDD->SetDisplayMode(RealLx,RealLy,8); //COPYSizeX,RSCRSizeY, 8 );
			DDLog("SetDisplayMode:%d\n",ddrval);
            if( ddrval == DD_OK ) 
            {
				//Rept("SetDisplayMode: DD_OK\n");
                // Create the primary surface with 1 back buffer
                ddsd.dwSize = sizeof( ddsd );
                ddsd.dwFlags = DDSD_CAPS;
                ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
                ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
				DDLog("CreateSurface:%d\n",ddrval);
                if( ddrval == DD_OK )
                {
					//Rept("CreateSurface: DD_OK\n");
                    // Get a pointer to the back buffer
                    //ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                    //ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, 
                    //                                      &lpDDSBack);
					//if (ddrval==DD_OK)
					//{
						//Rept("GetAttachedSurface: DD_OK\n");
						DDError=false;
						SCRSizeX=MaxSizeX;
						SCRSizeY=MaxSizeY;
						RSCRSizeX=RealLx;//ddsd.lPitch;
						Pitch=ddsd.lPitch;
						COPYSizeX=RealLx;
						RSCRSizeY=RealLy;
						ScrHeight=SCRSizeY;
						ScrWidth=SCRSizeX;
#ifndef _USE3D
						WindX=0;
						WindY=0;
						WindLx=RealLx;
						WindLy=RealLy;
						WindX1=WindLx-1;
						WindY1=WindLy-1;
#else
						GPS.SetClipArea( 0, 0, RealLx, RealLy );
#endif // !_USE3D
						BytesPerPixel=1;
						/*
						NModes=0;
						Rept("lpDD=%d\n",int(lpDD));
						lpDD->EnumDisplayModes(0,NULL,NULL,ModeCallback);
						if(NModes==0){
							NModes=1;
							ModeLX[0]=1024;
							ModeLY[0]=768;
						};
						*/
						Rept("Enum:OK\n");
						return true;
					//}
                        // Create a timer to flip the pages
                      /*  if( SetTimer( hwnd, TIMER_ID, 50, NULL ) )
                        {
                             return TRUE;
                        }*/
                }
            }
        }
    }
    wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
    MessageBox( hwnd, buf, "ERROR", MB_OK );
	return false;
}

#ifndef _USE3D

BOOL CreateRGBDDObjects(HWND hwnd)
{
	HRESULT ddrval;
	DDSCAPS ddscaps;
	char    buf[256];
	DDError=false;
	CurrentSurface=true;
	if (window_mode)
	{
		
		DDError=false;
		SCRSizeX=MaxSizeX;
		SCRSizeY=MaxSizeY;
		COPYSizeX=RealLx;
		RSCRSizeX=RealLx;
		RSCRSizeY=RealLy;
		ScrHeight=SCRSizeY;
		ScrWidth=SCRSizeX;
		InitRLCWindows();
		WindX=0;
		WindY=0;
		WindLx=RealLx;
		WindLy=RealLy;
		WindX1=WindLx-1;
		WindY1=WindLy-1;
		BytesPerPixel=1;
		offScreenPtr=(malloc(SCRSizeX*(SCRSizeY+32*4)));
		return true;
	}
#ifdef COPYSCR
	offScreenPtr=offScreenPtr=(malloc(MaxSizeX*(MaxSizeY+32*4)));
#endif
	if(lpDD){
		lpDDSPrimary->Release();
		goto SDMOD;
	};
	ddrval = DirectDrawCreate_wrapper( NULL, &lpDD, NULL );
	DDLog("RGB: DirectDrawCreate:%d\n",ddrval);
    if( ddrval == DD_OK )
    {
        // Get exclusive mode
        ddrval = lpDD->SetCooperativeLevel( hwnd,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
		DDLog("RGB: SetCooperativeLevel:%d\n",ddrval);
        if(ddrval == DD_OK )
        {
SDMOD:;
            ddrval = lpDD->SetDisplayMode(800,600,32); //COPYSizeX,RSCRSizeY, 8 );
			DDLog("RGB: SetDisplayMode:%d\n",ddrval);
            if( ddrval == DD_OK )
            {
                // Create the primary surface with 1 back buffer
                ddsd.dwSize = sizeof( ddsd );
                ddsd.dwFlags = DDSD_CAPS;
                ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
                ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
				DDLog("RGB: CreateSurface:%d\n",ddrval);
                if( ddrval == DD_OK )
                {
					DDError=false;
					SCRSizeX=MaxSizeX;
					SCRSizeY=MaxSizeY;
					RSCRSizeX=RealLx;//ddsd.lPitch;
					Pitch=ddsd.lPitch;
					COPYSizeX=RealLx;
					RSCRSizeY=RealLy;
					ScrHeight=SCRSizeY;
					ScrWidth=SCRSizeX;
					WindX=0;
					WindY=0;
					WindLx=SCRSizeX;
					WindLy=SCRSizeY;
					WindX1=WindLx-1;
					WindY1=WindLy-1;
					BytesPerPixel=1;
					return true;
				}
            }
        }
    }
    wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
    MessageBox( hwnd, buf, "ERROR", MB_OK );
	return false;
}

#endif // _!USE3D

BOOL CreateRGB640DDObjects(HWND hwnd)
{
	HRESULT ddrval;
	DDSCAPS ddscaps;
	char    buf[256];
	DDError=false;
	CurrentSurface=true;
	if (window_mode)
	{
		
		DDError=false;
		SCRSizeX=MaxSizeX;
		SCRSizeY=MaxSizeY;
		COPYSizeX=RealLx;
		RSCRSizeX=RealLx;
		RSCRSizeY=RealLy;
		ScrHeight=SCRSizeY;
		ScrWidth=SCRSizeX;
		InitRLCWindows();
#ifndef _USE3D
		WindX=0;
		WindY=0;
		WindLx=SCRSizeX;
		WindLy=SCRSizeY;
		WindX1=WindLx-1;
		WindY1=WindLy-1;
#else
		GPS.SetClipArea( 0, 0, SCRSizeX, SCRSizeY );
#endif // _USE3D
		BytesPerPixel=1;
		offScreenPtr=(malloc(SCRSizeX*(SCRSizeY+32*4)));
		return true;
	}
#ifdef COPYSCR
	offScreenPtr=offScreenPtr=(malloc(MaxSizeX*(MaxSizeY+32*4)));
#endif
	if(lpDD){
		lpDDSPrimary->Release();
		goto SDMOD;
	};
	ddrval = DirectDrawCreate_wrapper( NULL, &lpDD, NULL );
	DDLog("RGB640: DirectDrawCreate:%d\n",ddrval);
    if( ddrval == DD_OK )
    {
SDMOD:;
        // Get exclusive mode
        ddrval = lpDD->SetCooperativeLevel( hwnd,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
		DDLog("RGB640: SetCooperativeLevel:%d\n",ddrval);
        if(ddrval == DD_OK )
        {
            ddrval = lpDD->SetDisplayMode(640,480,BestBPP); //COPYSizeX,RSCRSizeY, 8 );
			DDLog("RGB640: SetDisplayMode:%d\n",ddrval);
            if( ddrval == DD_OK )
            {
                // Create the primary surface with 1 back buffer
                ddsd.dwSize = sizeof( ddsd );
                ddsd.dwFlags = DDSD_CAPS;
                ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
                ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
				DDLog("RGB640: CreateSurface:%d\n",ddrval);
                if( ddrval == DD_OK )
                {
					DDError=false;
					SCRSizeX=MaxSizeX; 
					SCRSizeY=MaxSizeY;
					RSCRSizeX=RealLx;//ddsd.lPitch;
					Pitch=ddsd.lPitch;
					COPYSizeX=RealLx;
					RSCRSizeY=RealLy;
					ScrHeight=SCRSizeY;
					ScrWidth=SCRSizeX; 
#ifndef _USE3D 
					WindX=0; 
					WindY=0;
					WindLx=SCRSizeX;
					WindLy=SCRSizeY;
					WindX1=WindLx-1;
					WindY1=WindLy-1;
#else
					GPS.SetClipArea( 0, 0, SCRSizeX, SCRSizeY );
#endif // _USE3D
					BytesPerPixel=1;
					return true;
                }
            }
        }
    }
    wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
    MessageBox( hwnd, buf, "ERROR", MB_OK );
	return false;
}

/*   Direct Draw palette loading*/
/*int clrRed;
int clrGreen;
int clrBlue;
int clrYello;*/

void LoadPalette(LPCSTR lpFileName)
{
	if(!lpDD)return;
	//AFile((char*)lpFileName);
	if (window_mode) return;
	if (DDError) return;
	ResFile pf=RReset(lpFileName);
	memset(&GPal,0,1024);
	if (pf!=INVALID_HANDLE_VALUE)
	{
		for(int i=0;i<256;i++){
			RBlockRead(pf,&GPal[i],3);
			//RBlockRead(pf,&xx.bmp.bmiColors[i],3);
		};
		RClose(pf);
		if(!strcmp(lpFileName,"agew_1.pal")){
			int DCL=6;
			int C0=65;//128-DCL*4;
			for(int i=0;i<12;i++){
				int gray=0;
				if(i>2)gray=(i-2)*2;
				if(i>7)gray+=(i-7)*8;
				if(i>9)gray+=(i-10)*10;
				if(i>10)gray+=50;
				gray=gray*6/3;
				//gray=(i+5)*6;
				int rr=0*C0/150+gray*8/2;
				int gg=80*C0/150+gray*6/2;//80
				int bb=132*C0/150+gray*4/2;
				if(rr>255)rr=255;
				if(gg>255)gg=255;
				if(bb>255)bb=255;
				if(i<5){
					rr=rr-((rr*(5-i))/6);
					gg=gg-((rr*(5-i))/6);
					bb=bb-((rr*(5-i))/6);
				};
				if(i<3){
					rr=rr-((rr*(3-i))/4);
					gg=gg-((rr*(3-i))/4);
					bb=bb-((rr*(3-i))/4);
				};
				if(i<2){
					rr=rr-((rr*(2-i))/3);
					gg=gg-((rr*(2-i))/3);
					bb=bb-((rr*(2-i))/3);
				};
				//if(!i){
				//	rr=rr*10/11;
				//	gg=gg*10/11;
				//	bb=bb*10/11;
				//};
				GPal[0xB0+i].peBlue=bb;
				GPal[0xB0+i].peRed=rr;
				GPal[0xB0+i].peGreen=gg;
				C0+=5; 
			};
			ResFile pf=RRewrite(lpFileName);
			for(int i=0;i<256;i++)RBlockWrite(pf,&GPal[i],3);
			RClose(pf);
		};
		if(!window_mode){
#ifndef _USE3D
			if(!PalDone){
				lpDD->CreatePalette(DDPCAPS_8BIT,&GPal[0],&lpDDPal,NULL);
				PalDone=true;
				lpDDSPrimary->SetPalette(lpDDPal);
				//lpDDSBack->SetPalette(lpDDPal);
			}else{
				lpDDPal->SetEntries(0,0,256,GPal);
			};
#else
			CheckPal16();
#endif //_USE3D
			
		};
	}
	/*clrRed = 0xD0;
	clrGreen=GetPaletteColor(0,255,0);
	clrBlue=GetPaletteColor(0,0,255);
	clrYello=GetPaletteColor(255,255,0);*/
}
void CBar(int x,int y,int Lx,int Ly,byte c);
void Susp(char* str){
	return;
	if(!window_mode){
		void* oldsof=ScreenPtr;
		ScreenPtr=RealScreenPtr;
		int ScLx=ScrWidth;
		ScrWidth=RealLx;
		CBar(700,1,100,5,0x93);
		ShowString(700,0,str,&fn8);
		ScrWidth=ScLx;
		ScreenPtr=oldsof;
	}else{
		CBar(700,1,100,5,0x93);
		ShowString(700,0,str,&fn8);

		HDC WH=GetWindowDC(hwnd);

		xxt.bmp.bmiHeader.biSize=sizeof BITMAPINFOHEADER;
		xxt.bmp.bmiHeader.biWidth=SCRSizeX;
		xxt.bmp.bmiHeader.biHeight=SCRSizeY;
		xxt.bmp.bmiHeader.biPlanes=1;
		xxt.bmp.bmiHeader.biBitCount=8;
		xxt.bmp.bmiHeader.biCompression=BI_RGB;
		xxt.bmp.bmiHeader.biSizeImage=0;
		int z=StretchDIBits(WH,700,1,100,5,
			700,1,100,5,ScreenPtr,&xxt.bmp,
			DIB_RGB_COLORS,SRCCOPY);
			
	};
};
void SetDarkPalette(){
#ifndef _USE3D
	if (DDError) return;
	ChangeColorFF();
	memset(&GPal,0,1024);
	if(!window_mode){
		if(!PalDone){
			lpDD->CreatePalette(DDPCAPS_8BIT,&GPal[0],&lpDDPal,NULL);
			PalDone=true;
			lpDDSPrimary->SetPalette(lpDDPal);
		}else{
			lpDDPal->SetEntries(0,0,256,GPal); 
		};
	}
#endif //_USE3D 
};
CEXPORT
void SlowLoadPalette(LPCSTR lpFileName)
{
#ifdef _USE3D
	LoadPalette(lpFileName);
	return;
#endif
	PALETTEENTRY            NPal[256];
	if (DDError) return;
	SetDarkPalette();
	ResFile pf=RReset(lpFileName);
	memset(&GPal,0,1024);
	if (pf!=INVALID_HANDLE_VALUE)
	{
		for(int i=0;i<256;i++){
			RBlockRead(pf,&GPal[i],3);
			//RBlockRead(pf,&xx.bmp.bmiColors[i],3);
		};
		RClose(pf);
		
		if(!strcmp(lpFileName,"agew_1.pal")){
			int DCL=6;
			int C0=65;//128-DCL*4;
			for(int i=0;i<12;i++){
				int gray=0;
				if(i>2)gray=(i-2)*2;
				if(i>7)gray+=(i-7)*8;
				if(i>9)gray+=(i-10)*10;
				if(i>10)gray+=50;
				gray=gray*6/3;
				//gray=(i+5)*6;
				int rr=0*C0/150+gray*8/2;
				int gg=80*C0/150+gray*6/2;//80
				int bb=132*C0/150+gray*4/2;
				if(rr>255)rr=255;
				if(gg>255)gg=255;
				if(bb>255)bb=255;
				if(i<5){
					rr=rr-((rr*(5-i))/6);
					gg=gg-((rr*(5-i))/6);
					bb=bb-((rr*(5-i))/6);
				};
				if(i<3){
					rr=rr-((rr*(3-i))/4);
					gg=gg-((rr*(3-i))/4);
					bb=bb-((rr*(3-i))/4);
				};
				if(i<2){
					rr=rr-((rr*(2-i))/3);
					gg=gg-((rr*(2-i))/3);
					bb=bb-((rr*(2-i))/3);
				};
				//if(!i){
				//	rr=rr*10/11;
				//	gg=gg*10/11;
				//	bb=bb*10/11;
				//};
				GPal[0xB0+i].peBlue=bb;
				GPal[0xB0+i].peRed=rr;
				GPal[0xB0+i].peGreen=gg;
				C0+=5;
			};
			ResFile pf=RRewrite(lpFileName);
			for(int i=0;i<256;i++)RBlockWrite(pf,&GPal[i],3);
			RClose(pf);
		};
		
		if(!window_mode){
			byte* pal=(byte*)NPal;
			byte* pal0=(byte*) GPal;
			int mul=0;
			int t0=GetTickCount();
			int mul0=0;
			do{
				mul=(GetTickCount()-t0)*2;
				if(mul>255)mul=255;
				if(mul!=mul0){
					for(int j=0;j<1024;j++){
						pal[j]=byte((int(pal0[j])*mul)>>8);
					};
					pal[1023]=0;
					pal[1022]=0;
					pal[1021]=0;
					pal[1020]=0;
					lpDDPal->SetEntries(0,0,255,NPal);
				};
				mul0=mul;
			}while(mul!=255);
		};
	}
	/*clrRed = 0xD0;
	clrGreen=GetPaletteColor(0,255,0);
	clrBlue=GetPaletteColor(0,0,255);
	clrYello=GetPaletteColor(255,255,0);*/
}
CEXPORT
void SlowUnLoadPalette(LPCSTR lpFileName)
{
	PALETTEENTRY            NPal[256];
	if (DDError) return;
	ChangeColorFF();
	//ResFile pf=RReset(lpFileName);
	//memset(&GPal,0,1024);
	//if (pf!=INVALID_HANDLE_VALUE)
	//{
		//for(int i=0;i<256;i++){
		//	RBlockRead(pf,&GPal[i],3);
		//	//RBlockRead(pf,&xx.bmp.bmiColors[i],3);
		//};
		//RClose(pf);
		if(!window_mode){
			byte* pal=(byte*)NPal;
			byte* pal0=(byte*) GPal;
			int mul=0;
			int t0=GetTickCount();
			int mul0=0;
			do{
				mul=(GetTickCount()-t0)*2;
				if(mul>255)mul=255;
				if(mul!=mul0){
					for(int j=0;j<1024;j++){
						pal[j]=byte((int(pal0[j])*(255-mul))>>8);
					};
					pal[1023]=0;
					pal[1022]=0;
					pal[1021]=0;
					pal[1020]=0;
					lpDDPal->SetEntries(0,0,255,NPal);
				};
				mul0=mul;
			}while(mul!=255);
		};
	//}
}	
/*     Closing all Direct Draw objects
 *
 * This procedure must be called before the program terminates,
 * otherwise Windows can occur some problems.
 */
void FreeDDObjects( void )
{
	free(offScreenPtr);
	offScreenPtr=NULL;
	if (window_mode)
	{
		//free(offScreenPtr);
		return;
	}
    if( lpDD != NULL )
    {
		/*if( lpDDSBack != NULL )
        {
            lpDDSBack->Release();
            lpDDSBack = NULL;
        };*/
		//ClearScreen();
        if( lpDDSPrimary != NULL )
        {
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        };
		lpDD->Release();
        lpDD = NULL;
    }
}
/*void SetDebugMode()
{
    DDDebug =true;
}
void NoDebugMode()
{
    DDDebug =false;
}*/

CEXPORT
void GetPalColor(byte idx,byte* r,byte* g,byte* b){
	*r=GPal[idx].peRed;
	*g=GPal[idx].peGreen;
	*b=GPal[idx].peBlue;
};

/*
    DirectDraw substitute.
    Uses mdraw.dll instead of the original, ddraw.lib exported DirectDrawCreate().
    Prevents the color palette corruption bug in modern Windows systems.
    No idea what the mdraw.dll funtion does, but you end up with a working
    IDirectDraw interface and no legacy bugs.
*/
HRESULT DirectDrawCreate_wrapper(GUID FAR* lpGUID, LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter)
{
    HMODULE mdrawHandle = LoadLibrary("mdraw.dll");
    if (nullptr != mdrawHandle)
    {
        typedef HRESULT(__stdcall* mdrawProcType)(GUID FAR* lpGUID, LPDIRECTDRAW FAR* lplpDD, IUnknown FAR* pUnkOuter);
        mdrawProcType mdrawProc = (mdrawProcType)GetProcAddress(mdrawHandle, "DirectDrawCreate");
        if (nullptr != mdrawProc)
        {
            HRESULT mdrawResult = mdrawProc(lpGUID, lplpDD, pUnkOuter);
            return mdrawResult;
        }
        FreeLibrary(mdrawHandle);
    }
    return DDERR_GENERIC;
}
//OVO SAM UKRAO OD COSSACK REVAMPA

#ifdef _USE3D
BaseMesh BMS;
bool init=0;
void TnLset(VertexTnL* V,float x,float y,float z,float u,float v,DWORD D,DWORD S){
	V->x=x;
	V->y=y;
	V->z=z;
	V->w=1.0;
	V->u=u;
	V->v=v;
	V->diffuse=D;
	V->specular=S;
};
void TnLset2(VertexTnL2* V,float x,float y,float z,float u,float v,float u2,float v2,DWORD D){
	V->x=x+100;
	V->y=y+100;
	V->z=z;
	V->w=1.0;
	V->u=u;
	V->v=v;
	V->u2=u2;
	V->v2=v2;
	V->diffuse=D;
};
void TestPiro();
float fdx=0;
float fdy=0;
extern int mouseX;
extern int mouseY;
extern int tmtmt;
void Test3D(){
	//Test4Piro();
	//int Id=GPS.PreLoadGPImage("list");
	//GPS.ShowGP(200,200,Id,tmtmt%88,0);

	return;
	//if(!init){
		//if(GetKeyState('A')&0x8000)fdx-=0.001;
		//if(GetKeyState('D')&0x8000)fdx+=0.001;
		//if(GetKeyState('W')&0x8000)fdy-=0.001;
		//if(GetKeyState('X')&0x8000)fdy+=0.001;
		fdx=float(mouseX-100-128)/1500.0;
		fdy=float(mouseY-100-32)/1500.0;

		BMS.create(256,256,vfTnL2,ptTriangleList,false);
		word ID=IRS->GetShaderID("road");
		word TID=IRS->GetTextureID("road.tga");
		BMS.setShader(ID);
		BMS.setTexture(TID);
		BMS.setTexture(TID,1);
		VertexTnL2* VTX=(VertexTnL2*)BMS.getVertexData();
		word* IDX=BMS.getIndices();

		TnLset2(VTX+0,  0  ,  0,   0,   0,   0,      0,   0,0xFFFFFFFF);
		TnLset2(VTX+1,  256,  0,   0,   1,   0,1.0+fdx,   0,0xFFFFFFFF);
		TnLset2(VTX+2,  0  ,256,   0,   0,   1,   0, 1.0+fdy,0xFFFFFFFF);
		TnLset2(VTX+3,  256,256,   0,   1,   1,1+fdx, 1.0+fdy,0xFFFFFFFF);

		IDX[0]=0;
		IDX[1]=1;
		IDX[2]=2;
		IDX[3]=1;
		IDX[4]=3;
		IDX[5]=2;
		BMS.setNInd(6);
		BMS.setNVert(4);
		init=1;
	//};
	IRS->Draw(BMS);
};
#endif