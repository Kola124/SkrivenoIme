#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "fog.h"
#include "Megapolis.h"
#include <assert.h>
#include "walls.h"
#include "mode.h"
#include "GSound.h"
#include "MapSprites.h"
#include "NewMon.h"
#include "Math.h"
#include "RealWater.h"
#include "NewUpgrade.h"
#include "ZBuffer.h"
#include "Path.h"
#include "Transport.h"
#include "3DBars.h"
#include "CDirSnd.h"
#include "NewAI.h"
#include "3DMapEd.h"
#include "TopoGraf.h"
#include "Towers.h"
#include "Fonts.h"
#include "Safety.h"
#include "3DGraph.h"
#include "Nature.h"
#include "ConstStr.h"
#define NTicks 100
#define NOLAG
DWORD PF_Average[NTicks];
DWORD PF_Prev[NTicks];
int PF_PrevTime=0;
LARGE_INTEGER PF_Start[NTicks];
LARGE_INTEGER PF_Finish[NTicks];
char* PF_Message[NTicks];
char* Messages[NTicks];
int NTimers;
class PFRM{
public:
	PFRM();
//	~PFRM();
};
PFRM CCCL;
PFRM::PFRM(){
	memset(PF_Average,0,sizeof PF_Average);
	memset(PF_Prev,0,sizeof PF_Prev);
	memset(PF_Start,0,sizeof PF_Start);
	memset(PF_Finish,0,sizeof PF_Start);
	memset(PF_Message,0,sizeof PF_Message);
};
void RunPF(int i,const char* Desc){
#ifndef NOLAG
	QueryPerformanceCounter(&PF_Start[i]);
	PF_Message[i]=(char*)Desc;
#endif
};
void StopPF(int i){
#ifndef NOLAG
	QueryPerformanceCounter(&PF_Finish[i]);
	PF_Finish[i].QuadPart=PF_Finish[i].QuadPart-PF_Start[i].QuadPart;
#endif
};
extern int RealLx;
void ShowPF(){
#ifdef NOLAG
	return;
#endif
//	StopPF(13);
	char cc[128];
	int max=0;
	for(int i=0;i<100;i++)if(PF_Start[i].LowPart||PF_Start[i].HighPart)max=i+1;
	LARGE_INTEGER FREQ;
	if(!QueryPerformanceFrequency(&FREQ))return;
	//FREQ.QuadPart=Int64ShraMod32(FREQ.QuadPart,10);
	for(int i=0;i<max;i++){
		LARGE_INTEGER DD;
		DD.QuadPart=PF_Finish[i].QuadPart;//-PF_Start[i].QuadPart;
		int dt=MulDiv(DD.LowPart,1000000,FREQ.LowPart);
		dt*=100;
		if(!PF_Average[i])PF_Average[i]=dt/100;
		int TT=PF_Average[i]=(PF_Average[i]*30+dt)/31;
		TT/=10000;
		sprintf_s( cc,sizeof(cc),"%d :  %d.%d",i,TT/10,TT%10);
		/*
		TT/=100;
		int exp=6;
		while(TT>=100){
			TT/=10;
			exp--;
		};

		if(TT<10)sprintf_s( cc,sizeof(cc),"%d :  %d E-%d",i,TT,exp);
		else sprintf_s( cc,sizeof(cc),"%d : %d E-%d",i,TT,exp);
		*/
		if(PF_Prev[i]>=PF_Average[i])ShowString(RealLx-300,200+i*15,cc,&SmallWhiteFont);
		else ShowString(RealLx-300,200+i*15,cc,&SmallYellowFont);
		if(PF_Message[i])ShowString(RealLx-305-GetRLCStrWidth(PF_Message[i],&SmallYellowFont),200+i*15,PF_Message[i],&SmallYellowFont);
	};
	if(GetTickCount()-PF_PrevTime>4000){
		for(int i=0;i<max;i++){
			PF_Prev[i]=(PF_Average[i]*130)/100;
		};
		PF_PrevTime=GetTickCount();
	};
	//RunPF(13);
};
int AddTime=0;
int NeedAddTime=0;
int PREVT=0;
int GetTicksEx(){
	int T=GetTickCount();
	//T+=T>>10;
	return T;
};
int GetDADDT(int DT){
	DT>>=7;
	if(DT<50)return 1;
	if(DT<100)return 2;
	if(DT<150)return 3;
	if(DT<220)return 4;
	if(DT<350)return 5;
	if(DT<400)return 6;
	if(DT<600)return 7;
	if(DT<1000)return 10;
	return 20;
};
#ifndef SPEEDFIX
int GetRealTime() {
#else
unsigned long GetRealTime() {
    return GetTickCount();
#endif

	/*
	LARGE_INTEGER FREQ;
	if(!QueryPerformanceFrequency(&FREQ))return GetTickCount();
	LARGE_INTEGER DD;
	QueryPerformanceCounter(&DD);
	double Freq=double(FREQ.LowPart)+double(FREQ.HighPart)*double(0x100000000);
	double Time=double(DD.LowPart)+double(DD.HighPart)*double(0x100000000);
    */
#ifndef SPEEDFIX
    int T=GetTickCount();//GetTicksEx();
    //T+=T>>10;
    if(PREVT){
        if(T-PREVT>5){
            if(NeedAddTime<AddTime){
                AddTime-=GetDADDT(AddTime-NeedAddTime);
                if(AddTime<NeedAddTime)AddTime=NeedAddTime;
            };
            if(NeedAddTime>AddTime){
                AddTime+=GetDADDT(NeedAddTime-AddTime);
                if(AddTime>NeedAddTime)AddTime=NeedAddTime;
            };
            PREVT=T;
        };
    }else PREVT=T;
    return T+(AddTime>>7);
#endif
};
int GetAbsoluteRealTime(){
	//return GetTickCount();
	/*
	LARGE_INTEGER FREQ;
	if(!QueryPerformanceFrequency(&FREQ))return GetTickCount();
	LARGE_INTEGER DD;
	QueryPerformanceCounter(&DD);
	double Freq=double(FREQ.LowPart)+double(FREQ.HighPart)*double(0x100000000);
	double Time=double(DD.LowPart)+double(DD.HighPart)*double(0x100000000);
	return int((Time*1000)/Freq);
	*/
	return GetTickCount();//GetTicksEx();
};
