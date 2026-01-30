#include <windows.h>
#include <basetyps.h>
#include <stdio.h>
#include <stdlib.h>
//#include <dsconf.h>
#include <stdlib.h>

#include "vopl_globals.h"
#include "oggvor.h"

#define WAIT_TIME 50

static HANDLE hMutex=0;
static HANDLE hMapping=0;
static LP_COMMAND_BUFFER pbExchange=NULL;
static BOOL bInitialized=FALSE;

void VoplError(const char* pcszError){
	MessageBox(NULL,pcszError,"VOPL-C Critical Error",MB_OK | MB_ICONHAND);
	ExitProcess(1);
}

void ov_Init(HWND hExtWnd)
{
	if(bInitialized)
		return;

	char	szOVSD[512],szDIR[512];
	DWORD	dwSTime;

	GetModuleFileName(NULL,szOVSD,512);
	*(strrchr(szOVSD,'\\'))='\0';
	strcpy(szDIR,szOVSD);
	strcat(szOVSD,"\\vopl.exe");

	char				szRunLine[1024];
	STARTUPINFO			r_si;
	PROCESS_INFORMATION r_pi;

	sprintf(szRunLine,"/HWND=%lu",hExtWnd);

	ZeroMemory(&r_si,sizeof(STARTUPINFO));
	ZeroMemory(&r_pi,sizeof(PROCESS_INFORMATION));

	r_si.cb=sizeof(STARTUPINFO);

	BOOL bStart;

	bStart=CreateProcess(
		szOVSD,
		szRunLine,
		NULL,
		NULL,
		FALSE,
		0x00,
		NULL,
		szDIR,
		&r_si,
		&r_pi);

	if(!bStart)
		VoplError("Can not launch server process (1-1)");

	// Waiting for the process to become visible
	dwSTime=GetTickCount();
	do{
		Sleep(50);
		hMapping=OpenFileMapping(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			VOPL_MAPPING_NAME);

		if(hMapping)
			break;

		if((GetTickCount()-dwSTime)>10000)
			VoplError("Can not open file mapping (1-2)");
	}while(1);

	// Mapping memory
	pbExchange=(LP_COMMAND_BUFFER)MapViewOfFile(
		hMapping,
		FILE_MAP_ALL_ACCESS,
		0,0,
		sizeof(COMMAND_BUFFER));

	if(!pbExchange)
		VoplError("Can not map view of file (1-3)");

	Sleep(100);

	// Creating mutex 
	hMutex=CreateMutex(
		NULL,
		FALSE,
		VOPL_MUTEX_NAME);

	if(!hMutex)
		VoplError("Can not create mutex object (1-4)");

	Sleep(100);

	bInitialized=TRUE;
}

void ov_Play(LPCSTR pcszFileName)
{
	if(!bInitialized)
		return;

	while(1){
		WaitForSingleObject(hMutex,INFINITE);
		if(pbExchange->Command==vcFree){
			pbExchange->Command=vcPlay;
			strcpy(pbExchange->szFileName,pcszFileName);
			pbExchange->Finished=FALSE;
			pbExchange->Length=0;
			ReleaseMutex(hMutex);
			break;
		};
		ReleaseMutex(hMutex);
		Sleep(WAIT_TIME);
	};
}

void ov_Stop(void)
{
	if(!bInitialized)
		return;
#ifdef _USE3D
	int T0=GetTickCount();
	while(GetTickCount()-T0<1000){
#else	
	while(1){
#endif
		WaitForSingleObject(hMutex,INFINITE);
		if(pbExchange->Command==vcFree){
			pbExchange->Command=vcStop;
			ReleaseMutex(hMutex);
			break;
		};
		ReleaseMutex(hMutex);
		Sleep(WAIT_TIME);
	};
}

void ov_Done(void)
{
	if(!bInitialized)
		return;
#ifdef _USE3D
	int T0=GetTickCount();
	while(GetTickCount()-T0<1000){
#else
	while(1){
#endif
		WaitForSingleObject(hMutex,INFINITE);
		if(pbExchange->Command==vcFree){
			pbExchange->Command=vcKill;
			ReleaseMutex(hMutex);
			break;
		};
		ReleaseMutex(hMutex);
		Sleep(WAIT_TIME);
	};
	
	bInitialized=FALSE;
}

void ov_SetVolume(DWORD dwVolume)
{
	if(!bInitialized)
		return;
	
	while(1){
		WaitForSingleObject(hMutex,INFINITE);
		if((pbExchange->Command==vcFree)||(pbExchange->Command==vcVolume)){
			pbExchange->Command=vcVolume;
			pbExchange->Volume=dwVolume;
			ReleaseMutex(hMutex);
			break;
		};
		ReleaseMutex(hMutex);
		Sleep(WAIT_TIME);
	};
}

DWORD ov_GetStreamLength(void)
{
	DWORD dwLen=0;

	if(!bInitialized)
		return dwLen;
	
	WaitForSingleObject(hMutex,INFINITE);
	dwLen=pbExchange->Length;
	ReleaseMutex(hMutex);

	return dwLen;
}

DWORD ov_StreamFinished(void)
{
	DWORD dwFin=1;

	if(!bInitialized)
		return dwFin;

	WaitForSingleObject(hMutex,INFINITE);
	dwFin=pbExchange->Finished;
	ReleaseMutex(hMutex);

	return dwFin;
}

typedef HRESULT (__stdcall *PDLLFUNC)(REFCLSID,REFIID,LPVOID*);

DRIVER_TYPE ov_DriverType(void){
	DRIVER_TYPE dtType=dtEmulated;

	return dtType;
}
