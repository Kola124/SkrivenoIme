#include "stdio.h"
#include "windows.h"
//#include "stdafx.h"
#include "ResFile.h"
#include <math.h>
#include "bmptool.h"
#include <assert.h>
#include <exception>
#include <mutex>
static std::mutex g_memMutex;
static std::atomic<int> g_totalSize(0);
static std::atomic<int> g_allocSize(0);

#define CEXPORT __declspec(dllexport)
CEXPORT void* _ExMalloc(int Size);
#define znew(t,s) (t*)_ExMalloc((s)*sizeof(t))

void SaveToBMP24(char* Name,int Lx,int Ly,byte* data){
	ResFile f1=RRewrite(Name);
	BMPformat BM;
	BM.bfType='MB';
	int rlx=Lx*3;
	if(rlx&3)rlx=(rlx|3)+1;
	BM.bfSize=(sizeof BMPformat)+rlx*Ly;
	BM.bfReserved1=0;
	BM.bfReserved2=0;
	BM.bfOffBits=(sizeof BMPformat);
	BM.biSize=40;
	BM.biWidth=Lx;
	BM.biHeight=Ly;
	BM.biPlanes=1;
	BM.biBitCount=24;
	BM.biCompression=BI_RGB;
	BM.biSizeImage=0;
	BM.biXPelsPerMeter=0;
	BM.biYPelsPerMeter=0;
	BM.biClrUsed=0; 
	BM.biClrImportant=0; 
	RBlockWrite(f1,&BM,sizeof BM);
	for(int j=0;j<Ly;j++){
		RBlockWrite(f1,&data[3*Lx*(Ly-j-1)],rlx);
	};
	RClose(f1);
};

bool ReadBMP24(char* Name,BMPformat* BM,byte** data){
	ResFile f1=RReset(Name);
	if(f1!=INVALID_HANDLE_VALUE){
		RBlockRead(f1,BM,sizeof BMPformat);
		if(IOresult()||BM->bfType!='MB')return false;
		if(BM->biBitCount!=24)return false;
		*data=znew(byte,BM->biWidth*BM->biHeight*3);
		int wid=BM->biWidth*3;
		int rwid=wid;
		if(wid&3)rwid=(wid|3)+1;
		for(int i=0;i<BM->biHeight;i++){
			RSeek(f1,(sizeof BMPformat)+(BM->biHeight-i-1)*rwid);
			RBlockRead(f1,&((*data)[i*wid]),wid);
		};
		RClose(f1);
		return true;
    }else return false;
};

bool ReadBMP8(char* Name,BMPformat* BM,byte** data){
	ResFile f1=RReset(Name);
	if(f1!=INVALID_HANDLE_VALUE){
		RBlockRead(f1,BM,sizeof BMPformat);
		if(IOresult()||BM->bfType!='MB')return false;
		if(BM->biBitCount!=8)return false;
		*data=znew(byte,BM->biWidth*BM->biHeight);
		int wid=BM->biWidth;
		int rwid=wid;
		if(wid&3)rwid=(wid|3)+1;
		for(int i=0;i<BM->biHeight;i++){
			RSeek(f1,(sizeof BMPformat)+1024+(BM->biHeight-i-1)*rwid);
			RBlockRead(f1,&((*data)[i*wid]),wid);
		};
		RClose(f1);
		return true;
    }else return false;
};

bool ReadBMP8TOBPX(char* Name,byte** data){
	BMPformat BM;
	ResFile f1=RReset(Name);
	if(f1!=INVALID_HANDLE_VALUE){
		RBlockRead(f1,&BM,sizeof BMPformat);
		if(IOresult()||BM.bfType!='MB')return false;
		if(BM.biBitCount!=8)return false;
		*data=znew(byte,BM.biWidth*BM.biHeight+4);
		int wid=BM.biWidth;
		int rwid=wid;
		if(wid&3)rwid=(wid|3)+1;
		((short*)*data)[0]=BM.biWidth;
		((short*)*data)[1]=BM.biHeight;
		for(int i=0;i<BM.biHeight;i++){
			RSeek(f1,/*(sizeof BMPformat)+1024*/BM.bfOffBits+(BM.biHeight-i-1)*rwid);
			RBlockRead(f1,&((*data)[i*wid+4]),wid);
		};
		RClose(f1);
		return true;
    }else return false;
};

bool LoadBitmapLikeGrayscale(char* Name,int* Lx,int* Ly,byte** res){
	byte* data;
	BMPformat BM;
	if(ReadBMP24(Name,&BM,&data)){
		int LX=BM.biWidth;
		int LY=BM.biHeight;
		*res=znew(byte,LX*LY);
		for(int ix=0;ix<LX;ix++)
			for(int iy=0;iy<LY;iy++){
				int ofs=(ix+iy*LX);
				int ofs3=ofs*3;
				int v=int(data[ofs3]+data[ofs3+1]+data[ofs3+2])/3;
				(*res)[ofs]=v;
			};

		free(data);
		*Lx=BM.biWidth;
		*Ly=BM.biHeight;
		return true;
	};
	return false;
};

int GetResVal(byte* res,int LX,int LY,int RLX,int RLY,int x,int y){
	int vx=(x*LX)/RLX;
	int vy=(y*LY)/RLY;
	int rx=(vx*RLX)/LX;
	int ry=(vy*RLX)/LY;
	int rx1=((vx+1)*RLX)/LX;
	int ry1=((vy+1)*RLY)/LY;
	int vx1=vx+1;
	int vy1=vy+1;
	if(vx<0)vx=0;
	if(vy<0)vy=0;
	if(vx1<0)vx1=0;
	if(vy1<0)vy1=0;
	if(vx>=LX)vx=LX-1;
	if(vy>=LY)vy=LY-1;
	if(vx1>=LX)vx1=LX-1;
	if(vy1>=LY)vy1=LY-1;
	int z1=res[vx+LX*vy];
	int z2=res[vx1+LX*vy];
	int z3=res[vx+LX*vy1];
	int z4=res[vx1+LX*vy1];
	return z1+((x-rx)*(z2-z1))/(rx1-rx)+((y-ry)*(z3-z1))/(ry1-ry)-(((z2+z3-z1-z4)*(x-rx)*(y-ry))/(rx1-rx)/(ry1-ry));
};
byte* DATA;

DWORD GetSumm(char* Name){
	ResFile F=RReset(Name);
	if(F!=INVALID_HANDLE_VALUE){
		int sz=RFileSize(F);
		
		RBlockRead(F,DATA,sz);
		RClose(F);
		DWORD SZZ=0;
		for(int i=0;i<sz;i++)SZZ+=DATA[i];
		SZZ&=65535;
		return SZZ;
	}else return 0;
};
extern HWND hwnd;
void CheckIntegrity(){
	
};
#include <stdlib.h>
#undef malloc
#undef calloc
#undef free
int TotalSize=0;
//#ifndef _USE3D

//#include "FMM\FMM.H"
void* FM_Malloc(int size) {
    if (size <= 0) return nullptr;
    
    std::lock_guard<std::mutex> lock(g_memMutex);
    
    void* ptr = ::calloc(size + sizeof(DWORD), 1);  // Use :: to bypass macro
    if (ptr) {
        ((DWORD*)ptr)[0] = 0xCAFEBABE;
        g_totalSize++;
        return (void*)((DWORD*)ptr + 1);  // Return after header
    }
    return nullptr;
}
void FM_free(void* ptr) {
    if (!ptr || (uintptr_t)ptr < 0x10000) return;
    
    std::lock_guard<std::mutex> lock(g_memMutex);
    
    DWORD* header = ((DWORD*)ptr) - 1;  // Get actual allocation
    
    // Validate header
    if (header[0] != 0xCAFEBABE) return;
    
    header[0] = 0xDEADBEEF;
    ::free(header);  // Use :: to bypass macro
    
    if (g_totalSize > 0) g_totalSize--;
}

//Using Global heap
#define malloc FM_Malloc
#define calloc(a,b) FM_Malloc((a)*(b))
#define free FM_free

//#endif //_USE3D

//#define realloc Grealloc


void FreeGHeap();
void MemError(){
	MessageBox(hwnd,"Out of memory!","ERROR!",0);
	FreeGHeap();
	abort();
};
HANDLE GlobHeap=NULL;
void FreeGHeap(){
	if(GlobHeap)HeapDestroy(GlobHeap);
	GlobHeap=NULL;
};
bool ChechIfInit(){
	if(!GlobHeap){
		GlobHeap=HeapCreate(0,0,0);
		if(!GlobHeap){
			MemError();
			FreeGHeap();
			return false;
		}else{
			//atexit(&FreeGHeap);
		};
	};
	return true;
};
/*
class GHEAP{
public:
	int x;
	GHEAP(){
		x=0;
		ChechIfInit();
	};
	~GHEAP(){
		FreeGHeap();
	};
};
static GHEAP HEAP;
*/
void* Gmalloc(int size) {
    if (!ChechIfInit()) return nullptr;
    return HeapAlloc(GlobHeap, HEAP_ZERO_MEMORY, size);
}
void* Grealloc(void* ptr, int size) {
    if (!ptr) return Gmalloc(size);
    if (!ChechIfInit()) return nullptr;
    return HeapReAlloc(GlobHeap, HEAP_ZERO_MEMORY, ptr, size);
}
void Gfree(void* ptr){
	try{
		if(ChechIfInit()){
			//SetLastError(0);
			HeapFree(GlobHeap,0,ptr);
			/*
			int err=GetLastError();
			if(err){
				LPVOID lpMsgBuf;
				if (!FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					err,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL ))
				{
					// Handle the error.
				return;
				}

				// Process any inserts in lpMsgBuf.
				// ...

					// Display the string.
				MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

				// Free the buffer.
				LocalFree( lpMsgBuf );
			};
			*/
		};
	}catch(...){
		MemError();
	};
};
//-----------------

#define SAFEMEMORY
#define NOERRORS
#define SUPERSAFEREALLOC
void CheckDynamicalPtr(void* ptr){
};
CEXPORT
void _ExFree(void* ptr){
	if(!ptr)return;
	DWORD* Ptr=(DWORD*)ptr;
	try{
		//if(Ptr&&Ptr[0]!=0xcdcdcdcd){
			free(ptr);
			TotalSize--;
		//};
	}catch(...){
	};
};
bool CheckMemBlock(byte* ptr) {
    __try {
        int size = ((DWORD*)ptr)[0];
        if (size <= 0 || size > 100000000) return false;
        return ((DWORD*)ptr)[1] == 'TRTS' && 
               ((DWORD*)(ptr + size + 8))[0] == 'LNIF';
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}
int AllocSize=0;
CEXPORT
void* _ExMalloc(int Size) {
    byte* data = (byte*)::calloc(Size + 12, 1);
    if (!data) {
        MessageBox(hwnd, "Out of memory!", "ERROR!", 0);
        return nullptr;
    }
    
    ((DWORD*)data)[0] = Size;
    ((DWORD*)data)[1] = 'TRTS';
    *((DWORD*)(data + 8 + Size)) = 'LNIF';
    
    g_allocSize.fetch_add(Size);
    return data + 8;
}
CEXPORT
void* _ExRealloc(void* ptr, int Size) {
    if (!ptr) return _ExMalloc(Size);
    
    byte* data = ((byte*)ptr) - 8;
    if (!CheckMemBlock(data)) return nullptr;
    
    int oldSize = ((DWORD*)data)[0];
    void* newPtr = _ExMalloc(Size);
    
    if (newPtr) {
        memcpy(newPtr, ptr, min(Size, oldSize));
        _ExFree(ptr);
    }
    return newPtr;
}
