#include "..\ddini.h"
#include "..\ResFile.h"
#include "..\FastDraw.h"
#include "..\mgraph.h"
#include "..\mouse.h"
#include "..\menu.h"
#include "..\MapDiscr.h"
#include "..\multipl.h"
#include "..\fog.h"
#include "..\walls.h"
#include "..\Nature.h"
#include <time.h>
#include "..\Nucl.h"
#include "..\Megapolis.h"
#include "..\Dialogs.h"
#include "..\fonts.h"
#include "..\GSound.h"
#include "..\3DGraph.h"
#include "..\3DMapEd.h"
#include "..\MapSprites.h"
#include <assert.h>
#include <math.h>
#include "..\NewMon.h"
#include "..\IconTool.h"
#include "..\GP_Draw.h"
#include "..\3DRandMap.h"
#include <crtdbg.h>
#include "..\ActiveScenary.h"
#include "..\DrawForm.h"
#include "..\Conststr.h"
#include <Process.h>
#include "..\MPlayer.h"
#include "..\Recorder.h"
#include "..\GSINC.H"
#include "..\TopoGraf.h"
#include "..\PeerClass.h"
#include "..\CEngine\goaceng.h"
#include "..\StrategyResearch.h"
#include "..\Safety.h"
#include "..\EinfoClass.h"
#include "stats\gstats.h"
#include "stats\gpersist.h"

void CheckExistingSaves();
#pragma pack(4)
#include "GP.h"
#pragma pack(1)

int TestUNIQ(char* email,char* Nick);
void MakeUniqLastName(char* email,char* Nick,char* LastName);
void ShowCentralMessage(char* Message,int GPIDX);
extern char PlName[64];
extern int CurProfileID;
extern int menu_x_off, menu_y_off;
GPConnection* LOGIN_gp=0;
void DarkScreen();
char* GetTextByID(char* ID);
bool InitUser(int ProfileID,char* password);
void ProcessScreen();
void GSYSDRAW();
void SendPlayerRequest(char* Nick,char* mail);
extern int ShowGameScreen;
void ActiveProcess(){
	if(ShowGameScreen){
		ProcessScreen();
		GSYSDRAW();
	};
};
class PLAYERLOGIN{
public:
    PLAYERLOGIN()
    {};
    ~PLAYERLOGIN()
    {};
	bool Connected;
	bool Logged;
	bool Error;
	byte Rank;
	GPConnection gp;
	char Nick[64];
	char EMail[128];
	char Password[64];
	GPProfile ProfileID;
	void Setup();
	bool LogIn(char* Name,char* Password);//true-logged ok, false - profile not exsists
};
PLAYERLOGIN LOGIN;
void ErrorCallback(GPConnection * connection, void* arg, void* param){
};

bool CHECKBOXCLICK(SimpleDialog* SD){
	GP_Button* GPB=(GP_Button*)SD;
	if(GPB->ActiveFrame==9){
		GPB->ActiveFrame=11;
		GPB->PassiveFrame=12;
	}else{
		GPB->ActiveFrame=9;
		GPB->PassiveFrame=10;
	};
	return false;
};
bool ReadWinString(GFILE* F,char* STR,int Max);
void FREEPLIST(char** PLLIST,char** EMAILS,char** PASWRD,int NPLR){
	for(int i=0;i<NPLR;i++){
		free(PLLIST[i]);
		free(EMAILS[i]);
		free(PASWRD[i]);
	};
	if(NPLR){
		free(PLLIST);
		free(EMAILS);
		free(PASWRD);
	};
};
#define FreePLRS FREEPLIST(PLLIST,EMAILS,PASWRD,NPLR)
extern int ItemChoose;
bool MMItemChoose(SimpleDialog* SD);
bool ProcessMessages();
extern bool KeyPressed;
void Conn_callback(GPConnection * connection,void * arg,void * param);
void ShowCentralMessage2(char* Message,int GPIDX);
void ShowCentralMessage3(char* Message,int GPIDX);
void WaitWithError(char* ID,int GPID){
	char ccc[1256];
	strcpy(ccc,GetTextByID(ID));

	//gpGetErrorString(&LOGIN.gp,ccc);
	ShowCentralMessage3(GetTextByID(ID),GPID);
	FlipPages();
	KeyPressed=0;
	do{
		ProcessMessages();
	}while(!KeyPressed);
};
void ShowCentralMessage4(char* Message,int GPIDX,int DX);
void WaitWithError4(char* ID,int GPID){
	char ccc[4096];
	strcpy(ccc,GetTextByID(ID));

	//gpGetErrorString(&LOGIN.gp,ccc);
	ShowCentralMessage4(GetTextByID(ID),GPID,90);
	FlipPages();
	KeyPressed=0;
	do{
		ProcessMessages();
	}while(!KeyPressed);
};

char* CODESTR1="AxDfRew34UinGtKv";
char* CODESTR2="KJhilurhflikhsgh";
void printhex(char* s,byte v){
	char* hids="0123456789ABCDEF";
	s[0]=hids[v>>4];
	s[1]=hids[v&15];
	s[2]=0;
};
void EncodePassword(char* Pass,char* result){
	char pass[128];
	strcpy(pass,Pass);
	int L=strlen(pass);
	for(int i=0;i<L;i++)pass[i]=(pass[i]+CODESTR1[i&15])^CODESTR2[i&15];
	sprintf(result,"{PS}");
	for(int i=0;i<L;i++)printhex(result+strlen(result),pass[i]);
};
byte GetHByChar(byte v){
	if(v>='0'&&v<='9')return v-'0';
	if(v>='A'&&v<='F')return v-'A'+10;
	if(v>='a'&&v<='f')return v-'a'+10;
	return 0;
};
byte GetHex(char* s){
	return GetHByChar(s[1])+(GetHByChar(s[0])<<4);
};
void DecodePassword(char* source,char* pass){
	char* src=source;
	if(source[0]=='{'&&source[1]=='P'&&source[2]=='S'&&source[3]=='}'){
		source+=2;
		char cc1[12];
		cc1[2]=0;
		int i=0;
		do{
			source+=2;
			cc1[0]=source[0];
			cc1[1]=source[1];
			pass[i]=GetHex(cc1);
			i++;
			pass[i]=0;
		}while(source[0]&&source[1]);
		int L=(strlen(src)-4)>>1;
		for(int i=0;i<L;i++)pass[i]=(pass[i]^CODESTR2[i&15])-CODESTR1[i&15];
		pass[L]=0;
	}else strcpy(pass,source);
};
bool ProcessNewInternetLogin();


bool EnterEMail(char* EMail){
	EMail[0]=0;
#ifdef SCREENFIX
	DialogsSystem DSS(menu_x_off, menu_y_off);
#else
    DialogsSystem DSS(0, 0);
#endif
	SQPicture BACK("Interface\\Background_Wizard.bmp");
	LocalGP WBT("Interface\\LogWizard");
	LocalGP BTNS("Interface\\Login");
	DSS.addPicture(NULL,0,0,&BACK,&BACK,&BACK);

	GP_Button* Prev=DSS.addGP_Button(NULL,90,536,WBT.GPID,4,5);
	Prev->OnUserClick=&MMItemChoose;
	Prev->UserParam=1;

	GP_Button* Cancel=DSS.addGP_Button(NULL,247,536,WBT.GPID,0,1);
	Cancel->OnUserClick=&MMItemChoose;
	Cancel->UserParam=mcmCancel;

	GP_Button* Next=DSS.addGP_Button(NULL,406,536,WBT.GPID,2,3);
	Next->OnUserClick=&MMItemChoose;
	Next->UserParam=2;


	DSS.addTextButton(NULL,76,188,GetTextByID("WZR_H0"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5,GetTextByID("WZR_00"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5+26,GetTextByID("WZR_01"),&YellowFont,&YellowFont,&YellowFont,0);
	int DX=60;
	DSS.addTextButton(NULL,76+DX,219+5+26*4,GetTextByID("WZR_02"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76+DX,219+5+26*6,GetTextByID("WZR_03"),&YellowFont,&YellowFont,&YellowFont,0);

	DSS.addGP_Button(NULL,76+DX,219+2+26*5,BTNS.GPID,13,14);
	InputBox* IB=DSS.addInputBox(NULL,76+DX,219+26*5,EMail,120,230,26,&YellowFont,&WhiteFont);
	IB->Active=1;
	ItemChoose=-1;
	do{
		Next->Enabled=(strlen(EMail)>0&&strstr(EMail,"@")&&strstr(EMail,"."));
		ProcessMessages();
		DSS.ProcessDialogs();
		DSS.RefreshView();
	}while(ItemChoose==-1);
	return ItemChoose==2;
};
int EnterNewNick(char* Nick){
#ifdef SCREENFIX
    DialogsSystem DSS(menu_x_off, menu_y_off);
#else
    DialogsSystem DSS(0, 0);
#endif
	SQPicture BACK("Interface\\Background_Wizard.bmp");
	LocalGP WBT("Interface\\LogWizard");
	LocalGP BTNS("Interface\\Login");
	DSS.addPicture(NULL,0,0,&BACK,&BACK,&BACK);

	GP_Button* Prev=DSS.addGP_Button(NULL,90,536,WBT.GPID,4,5);
	Prev->OnUserClick=&MMItemChoose;
	Prev->UserParam=1;

	GP_Button* Cancel=DSS.addGP_Button(NULL,247,536,WBT.GPID,0,1);
	Cancel->OnUserClick=&MMItemChoose;
	Cancel->UserParam=mcmCancel;

	GP_Button* Next=DSS.addGP_Button(NULL,406,536,WBT.GPID,2,3);
	Next->OnUserClick=&MMItemChoose;
	Next->UserParam=2;


	DSS.addTextButton(NULL,76,188,GetTextByID("WZR_H4"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5,GetTextByID("WZR_40"),&YellowFont,&YellowFont,&YellowFont,0);

	DSS.addTextButton(NULL,76,219+5+26*2,GetTextByID("WZR_41"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5+26*3,GetTextByID("WZR_42"),&YellowFont,&YellowFont,&YellowFont,0);

	int DX=110;
	DSS.addTextButton(NULL,76+DX,219+5+26*5,GetTextByID("WZR_43"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addGP_Button(NULL,76+DX,219+2+26*6,BTNS.GPID,13,14);

	InputBox* IB=DSS.addInputBox(NULL,76+DX,219+26*6,Nick,120,230,26,&YellowFont,&WhiteFont);
	IB->Active=1;
	ItemChoose=-1;
	do{
		Next->Enabled=strlen(Nick)>0;
		ProcessMessages();
		DSS.ProcessDialogs();
		DSS.RefreshView();
	}while(ItemChoose==-1);
	return ItemChoose;
};
int CreateNewAccount(char* EMail,char* Password,bool* Store){
#ifdef SCREENFIX
    DialogsSystem DSS(menu_x_off, menu_y_off);
#else
    DialogsSystem DSS(0, 0);
#endif
	SQPicture BACK("Interface\\Background_Wizard.bmp");
	LocalGP WBT("Interface\\LogWizard");
	LocalGP BTNS("Interface\\Login");
	DSS.addPicture(NULL,0,0,&BACK,&BACK,&BACK);

	GP_Button* Prev=DSS.addGP_Button(NULL,90,536,WBT.GPID,4,5);
	Prev->OnUserClick=&MMItemChoose;
	Prev->UserParam=1;

	GP_Button* Cancel=DSS.addGP_Button(NULL,247,536,WBT.GPID,0,1);
	Cancel->OnUserClick=&MMItemChoose;
	Cancel->UserParam=mcmCancel;

	GP_Button* Next=DSS.addGP_Button(NULL,406,536,WBT.GPID,2,3);
	Next->OnUserClick=&MMItemChoose;
	Next->UserParam=2;

	DSS.addTextButton(NULL,76,188       ,GetTextByID("WZR_H1"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5     ,GetTextByID("WZR_10"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5+26  ,GetTextByID("WZR_11"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5+26*2,GetTextByID("WZR_12"),&YellowFont,&YellowFont,&YellowFont,0);

	DSS.addTextButton(NULL,76,219+5+26*4,GetTextByID("WZR_13"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5+26*6,GetTextByID("WZR_14"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5+26*7,GetTextByID("WZR_15"),&YellowFont,&YellowFont,&YellowFont,0);

	GP_Button* RemPassw=DSS.addGP_Button(NULL,76,219+1+26*10,BTNS.GPID,11,12);
	RemPassw->OnUserClick=&CHECKBOXCLICK;
	DSS.addTextButton(NULL,76+32,219+5+26*10,GetTextByID("WZR_16"),&YellowFont,&YellowFont,&YellowFont,0);

	char CheckPass[128];
	Password[0]=0;
	CheckPass[0]=0;
	int DX=120;
	DSS.addGP_Button(NULL,76+DX,219+1+26*4,BTNS.GPID,13,14);
	InputBox* IB1=DSS.addInputBox(NULL,76+DX,219+26*4,EMail,40,240,26,&YellowFont,&WhiteFont);
	IB1->Enabled=0;
	DSS.addGP_Button(NULL,76+DX,219+1+26*6,BTNS.GPID,13,14);
	InputBox* IB2=DSS.addInputBox(NULL,76+DX,219+26*6,Password,40,240,26,&YellowFont,&WhiteFont);
	IB2->Active=1;
	DSS.addGP_Button(NULL,76+DX,219+1+26*7,BTNS.GPID,13,14);
	InputBox* IB3=DSS.addInputBox(NULL,76+DX,219+26*7,CheckPass,40,240,26,&YellowFont,&WhiteFont);
	ItemChoose=-1;
	do{
		if(strlen(Password)==0||strcmp(Password,CheckPass))Next->Enabled=0;
		else Next->Enabled=1;
		ProcessMessages();
		DSS.ProcessDialogs();
		DSS.RefreshView();
	}while(ItemChoose==-1);
	*Store=RemPassw->ActiveFrame==11;
	return ItemChoose;
};
int EnterAccountPassword(char* Password,bool First,char* mail){
	Password[0]=0;
#ifdef SCREENFIX
    DialogsSystem DSS(menu_x_off, menu_y_off);
#else
    DialogsSystem DSS(0, 0);
#endif
	SQPicture BACK("Interface\\Background_Wizard.bmp");
	LocalGP WBT("Interface\\LogWizard");
	LocalGP BTNS("Interface\\Login");
	DSS.addPicture(NULL,0,0,&BACK,&BACK,&BACK);

	GP_Button* Prev=DSS.addGP_Button(NULL,90,536,WBT.GPID,4,5);
	Prev->OnUserClick=&MMItemChoose;
	Prev->UserParam=1;

	GP_Button* Cancel=DSS.addGP_Button(NULL,247,536,WBT.GPID,0,1);
	Cancel->OnUserClick=&MMItemChoose;
	Cancel->UserParam=mcmCancel;

	GP_Button* Next=DSS.addGP_Button(NULL,406,536,WBT.GPID,2,3);
	Next->OnUserClick=&MMItemChoose;
	Next->UserParam=2;


	DSS.addTextButton(NULL,76,188,GetTextByID("WZR_H2"),&YellowFont,&YellowFont,&YellowFont,0);
	if(First)DSS.addTextButton(NULL,76,219+5,GetTextByID("WZR_20"),&YellowFont,&YellowFont,&YellowFont,0);
	else DSS.addTextButton(NULL,76,219+5,GetTextByID("WZR_20W"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76,219+5+26,GetTextByID("WZR_21"),&YellowFont,&YellowFont,&YellowFont,0);
	int DX=100;
	DSS.addTextButton(NULL,76,219+5+26*5,GetTextByID("WZR_22"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76+DX,219+5+26*6,GetTextByID("WZR_23"),&YellowFont,&YellowFont,&YellowFont,0);
	DSS.addTextButton(NULL,76+DX,219+5+26*7,GetTextByID("WZR_24"),&YellowFont,&YellowFont,&YellowFont,0);

	DSS.addGP_Button(NULL,76+DX,219+2+26*5,BTNS.GPID,13,14);
	InputBox* IB=DSS.addInputBox(NULL,76+DX,219+26*5,Password,120,230,26,&YellowFont,&WhiteFont);
	IB->Active=1;
	IB->Anonim=1;

	TextButton* TB=DSS.addTextButton(NULL,76,219+26*10,GetTextByID("WZR_25"),&WhiteFont,&YellowFont,&YellowFont,0);
	TB->OnUserClick=&MMItemChoose;
	TB->UserParam=85;
	
	ItemChoose=-1;
	do{
		Next->Enabled=strlen(Password)>0;
		ProcessMessages();
		DSS.ProcessDialogs();
		DSS.RefreshView();
		if(ItemChoose==85){
			char ccc[256];
			sprintf_s(ccc,sizeof(ccc),"http://gamespyarcade.com/software/reqemail.asp?email=%s",mail);
			ClearScreen();
			FlipPages();
			ShellExecute(NULL,NULL,ccc,"","",SW_SHOWMAXIMIZED);
			ItemChoose=-1;
		};
	}while(ItemChoose==-1);
	return ItemChoose;
};

char* LASTNICK;
void E_Search(GPConnection * connection,void * arg,void * param){
	GPProfileSearchResponseArg* RES=(GPProfileSearchResponseArg*)arg;
	int* par=(int*)param;
	if(RES->result==GP_NO_ERROR){
		if(RES->numMatches){
			*par=2;
			strcpy(LASTNICK,RES->matches[0].nick);
		}else *par=1;
	}else *par=3;
};
void Conn_callback(GPConnection * connection,void * arg,void * param){
	GPConnectResponseArg* CONN=(GPConnectResponseArg*)arg;
	PLAYERLOGIN* LOG=(PLAYERLOGIN*)param;
	if(CONN->result==GP_NO_ERROR){
		LOG->Error=0;
		LOG->ProfileID=CONN->profile;
		LOG->Logged=1;
	}else{
		LOG->Error=1;
	};
};
extern int RealLx;
extern int RealLy;
void ShowCentralMessage2(char* Message,int GPIDX){
	int L=GetRLCStrWidth(Message,&WhiteFont);
	int DX=200;
	int DY=25;
	DrawStdBar2((RealLx-L-64)/2-DX,(RealLy-70)/2+18-DY,(RealLx+L+64)/2-DX,(RealLy-70)/2+55-DY,GPIDX);
	ShowString((RealLx-L)/2+1-DX,(RealLy-GetRLCHeight(YellowFont.RLC,'W'))/2+1-DY,Message,&BlackFont);
	ShowString((RealLx-L)/2-DX,(RealLy-GetRLCHeight(YellowFont.RLC,'W'))/2-DY,Message,&YellowFont);
};
void GetAllPar3(int LXMAX,int* N,int* LMaxX,char* Message){
	int NStr=1;
	int pos=0;
	int LM=0;
	int L=strlen(Message);
	int L0=0;
	do{
		if(L0>LM)LM=L0;
		if(Message[pos]==' '&&L0>LXMAX){
			NStr++;
			L0=0;
			pos++;
		}else{
			int DX;
			L0+=GetRLCWidthUNICODE(YellowFont.RLC,(byte*)(Message+pos),&DX);
			pos+=DX;
		};
		if(L0>LM)LM=L0;
	}while(Message[pos]);
	*N=NStr;
	*LMaxX=LM;
};
extern int CurPalette;
void ShowCentralMessage4(char* Message,int GPIDX,int DX){
	int LXMAX=400;
	char ccx[256];
	int L0=0;
	int NStr=1;
	int pos=0;
	int LM=0;
	int L=strlen(Message);
	GetAllPar3(LXMAX,&NStr,&LM,Message);
	int N1,LM1;
	int DXXX=32;
	do{
		GetAllPar3(LXMAX-DXXX,&N1,&LM1,Message);
		DXXX+=32;
	}while(N1==NStr&&N1>1);
	LXMAX-=DXXX-64;
	GetAllPar3(LXMAX,&NStr,&LM,Message);
	ccx[0]=0;
	pos=0;
	int pos1=0;
	
	int DY=25;
	if(CurPalette==2)DrawStdBar2((RealLx-64-LM)/2-DX,(RealLy-70)/2+18-DY-(NStr-1)*13,(RealLx+64+LM)/2-DX,(RealLy-70)/2+55-DY+(NStr-1)*13,GPIDX);
	else{
		DX=0;
		DrawStdBar((RealLx-64-LM)/2-DX,(RealLy-70)/2+18-DY-(NStr-1)*13,(RealLx+64+LM)/2-DX,(RealLy-70)/2+55-DY+(NStr-1)*13);
	};
	int y0=(RealLy-GetRLCHeight(YellowFont.RLC,'W'))/2+1-DY-(NStr-1)*13;
	L0=0;
	do{
		if((Message[pos]==' '&&L0>LXMAX)){
			int LL=GetRLCStrWidth(ccx,&YellowFont);
			ShowString((RealLx-LL)/2+1-DX,y0+1,ccx,&BlackFont);
			ShowString((RealLx-LL)/2-DX,y0,ccx,&YellowFont);
			pos1=0;
			ccx[0]=0;
			L0=0;
			pos++;
			y0+=26;
		}else{
			int DX;
			L0+=GetRLCWidthUNICODE(YellowFont.RLC,(byte*)(Message+pos),&DX);
			memcpy(ccx+pos1,Message+pos,DX);
			pos+=DX;
			pos1+=DX;
			ccx[pos1]=0;
		};
		if(!Message[pos]){
			int LL=GetRLCStrWidth(ccx,&YellowFont);
			ShowString((RealLx-LL)/2+1-DX,y0+1,ccx,&BlackFont);
			ShowString((RealLx-LL)/2-DX,y0,ccx,&YellowFont);
		};
	}while(Message[pos]);
};
void ShowCentralMessage3(char* Message,int GPIDX){
	ShowCentralMessage4(Message,GPIDX,206);
};
void New_profile(GPConnection * connection,void * arg,void * param){
	GPNewProfileResponseArg* RES=(GPNewProfileResponseArg*)arg;
	PLAYERLOGIN* LOG=(PLAYERLOGIN*)param;
	if(RES->result==GP_NO_ERROR){
		LOG->Error=0;
		LOG->ProfileID=RES->profile;
		LOG->Logged=1;
	}else{
		LOG->Error=1;
		LOG->Logged=0;
	};
};
void E_Search2(GPConnection * connection,void * arg,void * param){
	GPProfileSearchResponseArg* RES=(GPProfileSearchResponseArg*)arg;
	ComboBox* LB=(ComboBox*)param;
	if(RES->result==GP_NO_ERROR&&RES->numMatches){
		for(int i=0;i<RES->numMatches;i++)LB->AddLine(RES->matches[i].nick);	
	};
};
void Create_account(GPConnection * connection,void * arg,void * param){
	GPConnectResponseArg* CONN=(GPConnectResponseArg*)arg;
	PLAYERLOGIN* LOG=(PLAYERLOGIN*)param;
	if(CONN->result==GP_NO_ERROR){
		LOG->Error=0;
		LOG->ProfileID=CONN->profile;
		LOG->Logged=1;
	}else{
		LOG->Error=1;
	};
};
bool CHKCLICK(SimpleDialog* SD){
	GP_Button* GB=(GP_Button*)SD;
	GB->ActiveFrame=8;
	GB->PassiveFrame=9;
	return true;
};
//0-cancel,1-previous,2-new profile,3-existing

void GETPROFINFO(int ProfileID){

};
bool ProcessPlayerForm(GPGetInfoResponseArg* INFO){
#ifdef SCREENFIX
    DialogsSystem DSS(menu_x_off, menu_y_off);
#else
    DialogsSystem DSS(0, 0);
#endif
	SQPicture BACK("Interface\\Background_Internet_Form.bmp");
	LocalGP WBT("Interface\\LogWizard");
	LocalGP BTNS("Interface\\Login");
	LocalGP BTNS2("Interface\\PlayerForm");
	LocalGP SCROL("Interface\\_Slider");
	DSS.addPicture(NULL,0,0,&BACK,&BACK,&BACK);
	GP_Button* OkBtn=DSS.addGP_Button(NULL,139,479,BTNS2.GPID,2,3);
	OkBtn->OnUserClick=&MMItemChoose;
	OkBtn->UserParam=mcmOk;
	GP_Button* CancelBtn=DSS.addGP_Button(NULL,327,479,BTNS2.GPID,0,1);
	CancelBtn->OnUserClick=&MMItemChoose;
	CancelBtn->UserParam=mcmCancel;

	DSS.addTextButton(NULL,266+9,268+2,INFO->nick,&YellowFont,&YellowFont,&YellowFont,0);
	ComboBox* COUNTRY=DSS.addGP_ComboBox(NULL,266,294,BTNS.GPID,0,9,0,&WhiteFont,&YellowFont,NULL);
	VScrollBar* VS1;
	COUNTRY->AssignScroll(&DSS,&VS1,SCROL.GPID,0,14);
	GFILE* F=Gopen("Players\\Countries.dat","r");
	if(F){
		char ccc[128];
		char CCN[16];
		ReadWinString(F,CCN+1,15);
		if(INFO->countrycode[0]){
			strcpy(CCN+1,INFO->countrycode);
			_strupr(CCN);
		};
		CCN[0]='=';
		do{
			ReadWinString(F,ccc,128);
			if(ccc[0]){
				if(strstr(ccc,CCN)){
					COUNTRY->CurLine=COUNTRY->NLines;
					COUNTRY->YPos=COUNTRY->CurLine;
				};
				char* cc=strchr(ccc,'=');
				if(cc)cc[0]=0;
				COUNTRY->AddLine(ccc);
			};
		}while(ccc[0]);
		Gclose(F);
	};
	if(COUNTRY->YPos+COUNTRY->MaxLY>COUNTRY->NLines)COUNTRY->YPos=COUNTRY->NLines-COUNTRY->MaxLY;
	VS1->SPos=COUNTRY->YPos*10;
	VS1->SMaxPos=(COUNTRY->NLines-COUNTRY->MaxLY)*10;
	ComboBox* DAY=DSS.addGP_ComboBoxDLX(NULL,266,320,57,BTNS.GPID,0,9,0,&WhiteFont,&YellowFont,NULL);
	VScrollBar* VS2;
	DAY->AssignScroll(&DSS,&VS2,SCROL.GPID,0,12);
	for(int i=1;i<32;i++){
		char cc[12];
		sprintf_s( cc,sizeof(cc),"%d",i);
		DAY->AddLine(cc);
	};
	ComboBox* MONTH=DSS.addGP_ComboBoxDLX(NULL,328,320,96,BTNS.GPID,0,9,0,&WhiteFont,&YellowFont,NULL);
	VScrollBar* VS3;
	MONTH->AssignScroll(&DSS,&VS3,SCROL.GPID,0,12);
	for(int i=0;i<12;i++){
		char cc[12];
		sprintf_s( cc,sizeof(cc),"MONTH_%d",i);
		MONTH->AddLine(GetTextByID(cc));
	};
	ComboBox* YEAR=DSS.addGP_ComboBoxDLX(NULL,429,320,77,BTNS.GPID,0,9,0,&WhiteFont,&YellowFont,NULL);
	VScrollBar* VS4;
	YEAR->AssignScroll(&DSS,&VS4,SCROL.GPID,0,12);
	for(int i=1900;i<2100;i++){
		char cc[12];
		sprintf_s( cc,sizeof(cc),"%d",i);
		YEAR->AddLine(cc);
	};
	if(INFO->birthyear<1900)INFO->birthyear=1900;
	if(INFO->birthyear>2099)INFO->birthyear=2099;
	if(INFO->birthmonth<1)INFO->birthmonth=1;
	if(INFO->birthmonth>12)INFO->birthmonth=12;
	if(INFO->birthday<1)INFO->birthday=1;
	if(INFO->birthday>31)INFO->birthday=31;
	DAY->CurLine=INFO->birthday-1;
	MONTH->CurLine=INFO->birthmonth-1;
	YEAR->CurLine=INFO->birthyear-1900;

	ComboBox* GENDER=DSS.addGP_ComboBox(NULL,266,346,BTNS.GPID,0,9,0,&WhiteFont,&YellowFont,NULL);
	GENDER->AddLine(GetTextByID("GEN_M"));
	GENDER->AddLine(GetTextByID("GEN_F"));
	GENDER->AddLine(GetTextByID("GEN_U"));
	if(INFO->sex==GP_MALE)GENDER->CurLine=0;
	else if(INFO->sex==GP_FEMALE)GENDER->CurLine=1;
	else GENDER->CurLine=2;
	
	char icq[32];
	DSS.addTextButton(NULL,266+9,372+2,INFO->email,&YellowFont,&YellowFont,&YellowFont,0);
	sprintf(icq,"%d",INFO->icquin);
	DSS.addGP_Button(NULL,266,398,BTNS.GPID,13,14);
	DSS.addInputBox(NULL,266,398,icq,20,230,22,&YellowFont,&WhiteFont);
	DSS.addGP_Button(NULL,266,398+26,BTNS.GPID,13,14);
	DSS.addInputBox(NULL,266,398+26,INFO->homepage,GP_HOMEPAGE_LEN-1,230,22,&YellowFont,&WhiteFont);
	ItemChoose=-1;
	do{
		ProcessMessages();
		DSS.ProcessDialogs();
		DSS.RefreshView();
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
		//1.Country
		char* cntr=COUNTRY->Lines[COUNTRY->CurLine];
		GFILE* F=Gopen("Players\\Countries.dat","r");
		if(F){
			char ccc[128];
			ReadWinString(F,ccc,15);
			do{	
				ReadWinString(F,ccc,128);
				if(ccc[0]){
					char* cc=strchr(ccc,'=');
					if(cc)cc[0]=0;
					if(!_stricmp(ccc,cntr)){
						strcpy(INFO->countrycode,cc+1);
					};
				};
			}while(ccc[0]);
			Gclose(F);
		};
		//birthday
		INFO->birthday=DAY->CurLine+1;
		INFO->birthmonth=MONTH->CurLine+1;
		INFO->birthyear=YEAR->CurLine+1900;
		//gender
		switch(GENDER->CurLine){
		case 0:
			INFO->sex=GP_MALE;
			break;
		case 1:
			INFO->sex=GP_FEMALE;
			break;
		case 2:
			INFO->sex=GP_PAT;
			break;
		};
		//icq
		INFO->icquin=atoi(icq);
		return true;
	}else return false;
};
bool GPI_Answer=0;
void GPI_callback(GPConnection* gp,void* arg,void* par){
	GPGetInfoResponseArg* GIRA=(GPGetInfoResponseArg*)arg;
	GPGetInfoResponseArg* GPI=(GPGetInfoResponseArg*)par;
	*GPI=*GIRA;
	GPI_Answer=1;
};

struct SRRESULT{
	bool NOTUNICAL;
	bool FINAL;
	bool Error;
	char* email;
};
bool CheckUNIQ(char* email,char* Nick,char* LastName);
void UNIQ_callback(GPConnection* gp,void* arg,void* par){
	GPProfileSearchResponseArg* RES=(GPProfileSearchResponseArg*)arg;
	SRRESULT* SUNI=(SRRESULT*)par;
	if(RES->result==GP_NO_ERROR){
		for(int i=0;i<RES->numMatches;i++)if(_stricmp(RES->matches[i].email,SUNI->email)){
			if(CheckUNIQ(RES->matches[i].email,RES->matches[i].nick,RES->matches[i].lastname)){
				SUNI->NOTUNICAL=1;
			};
		};
		if(RES->more==GP_DONE)SUNI->FINAL=1;
	}else SUNI->Error=1;

};
bool CheckUNIQ(char* email,char* Nick,char* LastName){
	char* ss=strstr(LastName,"~@~");
	if(ss){
		int v=0;
		int z=sscanf(ss+3,"%d",&v);
		if(z==1){
			int vv=0;
			char NICK[128];
			char EMAIL[128];
			strcpy(NICK,Nick);
			strcpy(EMAIL,email);
			_strupr(NICK);
			_strupr(EMAIL);
			int L1=strlen(Nick);
			int L2=strlen(email);
			if(L1&&L2){
				int N=L1*L2;
				for(int j=0;j<N;j++)vv=vv+EMAIL[j%L2]*NICK[j%L1];
				vv^=0x543819FA;
				vv=abs(vv);
				if(vv==v)return true;
			};
			return false;
		}else return false;
	}else return false;
};
void MakeUniqLastName(char* email,char* Nick,char* LastName){
	char* ss=strstr(LastName,"~@~");
	if(ss)ss[0]=0;
	int vv=0;
	char NICK[128];
	char EMAIL[128];
	strcpy(NICK,Nick);
	strcpy(EMAIL,email);
	_strupr(NICK);
	_strupr(EMAIL);
	int L1=strlen(Nick);
	int L2=strlen(email);
	if(L1&&L2){
		int N=L1*L2;
		for(int j=0;j<N;j++)vv=vv+EMAIL[j%L2]*NICK[j%L1];
		vv^=0x543819FA;
		vv=abs(vv);
		LastName[15]=0;
		strcat(LastName,"~@~");
		sprintf(LastName+strlen(LastName),"%d",vv);
	};
};
//-1 -back,0-cancel,1-use it ,2-use it like original
void MarkNameLikeUnical(int,bool);
void CheckOriginalNick(int ProfileID){
    if (!LOGIN.Logged)
    {
        return;
    }
};
//----------------------Personal user data--------------------//
struct OneRatedPlayer{
	byte Rank;
	byte State;
	byte Mask;
	byte Color;
	byte NationID;
	int  Profile;
	int  Score;
	char Nick[32];
	word MaxScore;
	word MaxPopul;
	byte ScoreG[32];
	byte Popul[32];
};
struct SysTime{
	word Year;
	word Month;
	word Day;
	word DayOfWeek;
	word Hour;
	word Min;
};
class CurrentGame{
public:
	bool Active;
	char GameName[128];
	char MapName[128];
	int  ProfileID;
	char Nick[64];
	char InGameNick[64];
	char password[64];
	char email[127];
	bool Broken;
	bool Finished;
	
	SysTime StartTime;
	SysTime LastLoadTime;
	SysTime CurTime;

	int PlayingTime;
	int  NPL;
	byte GameType;
	OneRatedPlayer PLAYERS[8];
//-----------------------------//
	void CreateGame(
		char* GameName,
		char* Map,
		char* Nick
		);
	void UpdateGame();
	void SaveGameToFile();
	void LoadGameFromFile();
	void DeleteThisGameFromFile();
	bool UpdateGlobalInfo();
	void AssignVictory(char* Nick);
	void AssignDefeat(char* Nick);
	void AssignKick(char* Nick);
	int GetAddScore();
};
extern word NPlayers;
extern City CITY[8];
extern DWORD MyDPID;
int SortPlayers(byte* Res,int* par);
void CurrentGame::CreateGame(char* pGameName,char* pMap,char* pNick){
    Active = false;
};

extern int GameTime;
void CurrentGame::UpdateGame(){
	if(Active){
		/*
		GetSystemTime(&SYSTM);
		CurTime.Year=SYSTM.wYear;
		CurTime.Month=SYSTM.wMonth;
		CurTime.Day=SYSTM.wDay;
		CurTime.Hour=SYSTM.wHour;
		CurTime.Min=SYSTM.wMinute;
		CurTime.DayOfWeek=SYSTM.wDayOfWeek;
		*/
		PlayingTime=GameTime;
		byte ord[8];
		int  par[8];
		int no=SortPlayers(ord,par);
		byte prevms=0;
		int CurTm=0;
		SYSTEMTIME SYSTM;
		GetSystemTime(&SYSTM);
		CurTime.Year=SYSTM.wYear;
		CurTime.Month=SYSTM.wMonth;
		CurTime.Day=SYSTM.wDay;
		CurTime.Hour=SYSTM.wHour;
		CurTime.Min=SYSTM.wMinute;
		CurTime.DayOfWeek=SYSTM.wDayOfWeek;
		
		if(no){
			for(int q=0;q<no;q++){
				byte ms=NATIONS[NatRefTBL[ord[q]]].NMask;
				if(NATIONS[NatRefTBL[ord[q]]].AI_Enabled){
					Active=0;
					DeleteThisGameFromFile();
					return;
				};
				byte c=NatRefTBL[ord[q]];
				int v=ord[q];
				if(!(ms&prevms)){
					CurTm++;
				};
				int PIDIDX=-1;
				for(int w=0;w<NPlayers;w++)if(PINFO[w].ColorID==ord[q])PIDIDX=w;
				if(PIDIDX==-1){
					if(NATIONS[c].AI_Enabled){
						strcpy(PLAYERS[q].Nick,"Computer");
					}else{
						strcpy(PLAYERS[q].Nick,"???");
					};
					PLAYERS[q].Rank=0;
				}else{
					strcpy(PLAYERS[q].Nick,PINFO[PIDIDX].name);
					PLAYERS[q].Rank=PINFO[PIDIDX].Rank;
					PLAYERS[q].Profile=PINFO[PIDIDX].ProfileID;
				};
				PLAYERS[q].Mask=NATIONS[c].NMask;
				PLAYERS[q].Color=c;
				PLAYERS[q].State=3;
				if(CITY[c].Account>=0)PLAYERS[q].Score=CITY[c].Account;
				else PLAYERS[q].Score=0;

				//determining the nation
				int NID=255;
				
				for(int j=0;j<MAXOBJECT;j++){
					OneObject* OB=Group[j];
					if(OB&&OB->NNUM==c&&OB->newMons->Peasant&&!OB->Sdoxlo){
						NID=OB->Ref.General->NatID;
					};
				};
				PLAYERS[q].NationID=NID;
				if(PINFO[q].PlayerID==MyDPID){
					strcpy(InGameNick,PINFO[q].name);
				};

				memset(PLAYERS[q].Popul,0,sizeof PLAYERS[q].Popul);
				memset(PLAYERS[q].ScoreG,0,sizeof PLAYERS[q].ScoreG);

				if(NATIONS[c].NAccount>1){
					int NN=NATIONS[c].NAccount-1;
					int MaxP=1;
					for(int i=0;i<NN;i++){
						int v=NATIONS[c].Account[i];
						if(v>MaxP)MaxP=v;
					};
					for(int i=0;i<32;i++){
						PLAYERS[q].ScoreG[i]=(NATIONS[c].Account[(NN*i)/31]*100)/MaxP;
					};
					PLAYERS[q].MaxScore=MaxP;
				};
				if(NATIONS[c].NPopul>1){
					int NN=NATIONS[c].NPopul-1;
					int MaxP=1;
					for(int i=0;i<NN;i++){
						int v=NATIONS[c].Popul[i];
						if(v>MaxP)MaxP=v;
					};
					for(int i=0;i<32;i++){
						PLAYERS[q].Popul[i]=(NATIONS[c].Popul[(NN*i)/31]*100)/MaxP;
					};
					PLAYERS[q].MaxPopul=MaxP;
				};
			};
			NPL=no;
		};
		SaveGameToFile();
	};
};
void CurrentGame::AssignVictory(char* Nick){
	byte MyMask=0;
	for(int i=0;i<NPL;i++)if(!strcmp(PLAYERS[i].Nick,Nick)){
		PLAYERS[i].State=0;
		MyMask=PLAYERS[i].Mask;
	};
	for(int i=0;i<NPL;i++)if(PLAYERS[i].State!=1&&PLAYERS[i].Mask&MyMask){
		PLAYERS[i].State=0;
	}else{
		PLAYERS[i].State=1;
	};
	if(!Broken)Finished=1;
};
void CurrentGame::AssignDefeat(char* Nick){
	bool ALLMYLOOSE=1;
	byte MyMask=0;
	for(int i=0;i<NPL;i++)if(!strcmp(PLAYERS[i].Nick,Nick)){
		PLAYERS[i].State=1;
		MyMask=PLAYERS[i].Mask;
	};
	if(MyMask){
		byte HimMask1=0;
		byte HimMask2=0;
		for(int i=0;i<NPL;i++){
			if(PLAYERS[i].Mask&MyMask){
				if(PLAYERS[i].State!=1)ALLMYLOOSE=0;
			}else{
				if(HimMask1){
					if(!(PLAYERS[i].Mask&HimMask1))HimMask2=PLAYERS[i].Mask;
				}else{
					HimMask1=PLAYERS[i].Mask;
				};
			};
		};
		if(ALLMYLOOSE&&!HimMask2){
			for(int i=0;i<NPL;i++)if(PLAYERS[i].State!=1&&!(PLAYERS[i].Mask&MyMask)){
				PLAYERS[i].State=0;
			};
		};
	};
	if(!Broken)Finished=1;
};
void CurrentGame::AssignKick(char* Nick){
	for(int i=0;i<NPL;i++)if(!strcmp(PLAYERS[i].Nick,Nick)){
		PLAYERS[i].State=2;
	};
};
void SetNormAttr();
void CurrentGame::SaveGameToFile(){
	if(!Active)return;
	//char ccc[100];
	int t0=(StartTime.Min+StartTime.Hour*60)%200;
	ResFile F1=RReset("cew.dll");
	if(F1==INVALID_HANDLE_VALUE)return;
	RSeek(F1,20532+t0);
	int v=1;
	RBlockWrite(F1,&v,1);
	RSeek(F1,20532+200+t0*(sizeof(CurrentGame)+4));
	CurrentGame XXX;
	XXX=*this;
	byte* DATA=(byte*)(&XXX);
	byte* Key=(byte*)randoma;
	for(int i=0;i<sizeof XXX;i++)DATA[i]^=Key[i];
	RBlockWrite(F1,&XXX,sizeof XXX);
	int S=0;
	for(int i=0;i<sizeof CurrentGame;i++)S+=DATA[i];
	RBlockWrite(F1,&S,4);
	RClose(F1);
	SetNormAttr();
};
void CurrentGame::LoadGameFromFile(){
	ResFile F=RReset("cew.dll");
	if(F==INVALID_HANDLE_VALUE)return;
	RSeek(F,20532);
	int cur=0;
	int sz=0;
	int t=GetTickCount()&4096;
	for(int p=0;p<t;p++)rand();
	int NMAX=0;
	byte HEADR[200];
	RBlockRead(F,HEADR,200);
NEXTFILE:
	bool SUCCESS=0;
	do{
		cur=rand()%200;
		SUCCESS=HEADR[cur];
		NMAX++;
	}while((!SUCCESS)&&NMAX<5000);
	//ResFile F=RReset("Players\\INGP.DLL");
	if(SUCCESS){
		RSeek(F,20532+200+cur*(sizeof(CurrentGame)+4));
		RBlockRead(F,this,sizeof CurrentGame);
		int S;
		RBlockRead(F,&S,4);
		byte* Key=(byte*)randoma;
		byte* xx=(byte*)this;
		int S1=0;
		for(int i=0;i<sizeof CurrentGame;i++)S1+=xx[i];
		if(S!=S1)Active=false;
		else{
			for(int i=0;i<sizeof CurrentGame;i++)xx[i]^=Key[i];
		};
		if(!Finished){
			//checking the start time
			SYSTEMTIME ST;
			GetSystemTime(&ST);
			FILETIME FT;
			SystemTimeToFileTime(&ST,&FT);
			SYSTEMTIME ST0;
			memset(&ST0,0,sizeof ST0);
			ST0.wMinute=StartTime.Min;
			ST0.wHour=StartTime.Hour;
			ST0.wDay=StartTime.Day;
			ST0.wMonth=StartTime.Month;
			ST0.wYear=StartTime.Year;
			FILETIME FT0;
			SystemTimeToFileTime(&ST0,&FT0);
			LARGE_INTEGER LI0;
			LARGE_INTEGER LI;
			memcpy(&LI0,&FT0,8);
			memcpy(&LI,&FT,8);
			LI.QuadPart-=LI0.QuadPart;
			memcpy(&FT,&LI,8);
			int dt=10000000;
			if(FileTimeToSystemTime(&FT,&ST)){
				dt=(ST.wDay-1)*24+ST.wHour;
			};
			if(dt<24){//!!!!!!!!!
				RClose(F);
				Active=0;
				F=INVALID_HANDLE_VALUE;
				goto NEXTFILE;
			};
		};
	}else Active=false;
	RClose(F);
	SetNormAttr();
};
void CurrentGame::DeleteThisGameFromFile(){
	int cur=0;
	int sz=0;
	//char ccc[100];
	ResFile F=RReset("cew.dll");
	if(F==INVALID_HANDLE_VALUE)return;
	int t0=(StartTime.Min+StartTime.Hour*60)%200;
	RSeek(F,20532+t0);
	int v=0;
	RBlockWrite(F,&v,1);
	RClose(F);
	SetNormAttr();
};
struct OneIChunk{
	byte Index;
	word size;
	word Summ;
	byte Data[64];
};
struct CurUpload{
	bool Authenticated:1;
	bool Started:1;
	bool Finished:1;
	bool Error:1;
	bool DataSent:1;
	int  CurProfile;
	int  CurPage;
};
struct CurDownload{
	bool Started;
	bool Finished;
	bool Error;
	int  CurProfile;
	int  CurPage;
};
class InternetStatsChunks{
public:
	int CurIndex;
	bool Ready;
	bool Error;
	int N;
	OneIChunk** Chunks;
	CurDownload CDOWN;
	CurUpload CUPLD;

	InternetStatsChunks();
	void Init();
	void Clear();
	int ReadChunkFrom(byte* Sourse);
	void WriteChunk(byte* Src,int L,byte Type);
	bool Upload(int ProfileID,char* Password,int Page);
	bool Download(int ProfileID,int Page);
	//----downloading process----//
	void StartDownload(int ProfileID,int Page);
	void ProcessDownload();
	void StartUpload(int ProfileID,char* Password,int Page);
	void ProcessUpload();
};
InternetStatsChunks::InternetStatsChunks(){
	Init();
};

void InternetStatsChunks::Init(){
	N=0;
	CurIndex=0;
	Chunks=NULL;
};
void InternetStatsChunks::Clear(){
	for(int i=0;i<N;i++)free(Chunks[i]);
	if(Chunks)free(Chunks);
	Chunks=NULL;
	N=0;
};

bool ProfIsInit=0;
void InitProfileSystem(){
	//if(!LOGIN.Logged){
	//	ProfIsInit=0;
	//	return;
	//};
	gcd_gamename[0]='g';gcd_gamename[1]='m';gcd_gamename[2]='t';gcd_gamename[3]='e';
	gcd_gamename[4]='s';gcd_gamename[5]='t';gcd_gamename[6]='\0';
	gcd_secret_key[0]='H';gcd_secret_key[1]='A';gcd_secret_key[2]='6';gcd_secret_key[3]='z';
	gcd_secret_key[4]='k';gcd_secret_key[5]='S';gcd_secret_key[6]='\0';
	if(ProfIsInit)CloseStatsConnection();
	ProfIsInit=0;
	int result = InitStatsConnection(0);
	if (result == GE_NOERROR){
		ProfIsInit=1;
	}else{
		LocalGP BOR2("Interface\\Bor2");
		WaitWithError("DATASERVERR",BOR2.GPID);
		//LoginError(BOR2.GPID);
	};
};
void GetData_callback(int localid, int profileid, persisttype_t type, 
					  int index, int success, char *data, int len, 
					  void *instance){
	InternetStatsChunks* ISS=(InternetStatsChunks*)instance;
	if(success){
		ISS->Ready=1;
		if(data[0]=='G'&&data[1]=='I'&&data[2]=='R'&&data[3]=='S'){
			//1. decoding
			for(int i=4;i<len;i++)data[i]^=randoma[(i+255)&8191];
			data+=4;
			//2. splitting links
			int pos=0;
			len-=4;
			do{
				word L=*((word*)(data+1+pos));
				word S=*((word*)(data+3+pos));
				if(pos+L<=len&&L){
					word S0=0;
					for(int j=0;j<L;j++)S0+=byte(data[pos+j+5]);
					if(S==S0){
						ISS->Chunks=(OneIChunk**)realloc(ISS->Chunks,4*ISS->N+4);
						ISS->Chunks[ISS->N]=(OneIChunk*)malloc(sizeof(OneIChunk)-64+L);
						memcpy(ISS->Chunks[ISS->N],data+pos,L+5);
						ISS->N++;
						pos+=L+5;
					}else pos++;
				}else pos++;
			}while(pos<len);
		};
		ISS->Ready=1;
	}else ISS->Error=1;
};

bool InternetStatsChunks::Download(int ProfileID,int Page){
	Clear();
	Ready=0;
	Error=0;
	GetPersistData(0,ProfileID,pd_public_rw,Page,&GetData_callback,this);
	do{
		ProcessMessages();
		PersistThink();
		ActiveProcess();
	}while(!(Error||Ready));
	return Ready;
};
void GetData_callback2(int localid, int profileid, persisttype_t type,
					  int index, int success, char *data, int len, 
					  void *instance){

	InternetStatsChunks* ISS=(InternetStatsChunks*)instance;
	if(profileid==ISS->CDOWN.CurProfile){
		GetData_callback(localid,profileid,type,index,success,data,len,instance);
		if(success){
			ISS->CDOWN.Error=0;
			ISS->CDOWN.Finished=1;
		}else{
			ISS->CDOWN.Error=1;
		};
	};
};
void InternetStatsChunks::StartDownload(int ProfileID,int Page){
	Clear();
	Ready=0;
	Error=0;
	CDOWN.CurProfile=ProfileID;
	CDOWN.Error=0;
	CDOWN.Finished=0;
	CDOWN.Started=1;
	CDOWN.CurPage=Page;
	GetPersistData(0,ProfileID,pd_public_rw,Page,&GetData_callback2,this);
};
void PersAuthCallback2(int localid, int profileid, int authenticated, 
					  char *errmsg, void *instance){
	InternetStatsChunks* ISC=(InternetStatsChunks*)instance;
	if(authenticated){
		ISC->CUPLD.Authenticated=1;
	}else ISC->CUPLD.Error=1;
};
void PersDataSaveCallback2(int localid, int profileid, persisttype_t type, 
						  int index, int success, void *instance){
	InternetStatsChunks* ISC=(InternetStatsChunks*)instance;
	if(success){
		ISC->CUPLD.Finished=1;
	}else ISC->CUPLD.Error=1;
};
void InternetStatsChunks::ProcessUpload(){
	ProcessMessages();
	PersistThink();
	ActiveProcess();
	if(CUPLD.Error)return;
	if(!CUPLD.Finished){
		if(CUPLD.Authenticated&&!CUPLD.DataSent){
			int GL=4;
			for(int j=0;j<N;j++)GL+=Chunks[j]->size+5;
			byte* GData=znew(byte,GL);
			GData[0]='G';
			GData[1]='I';
			GData[2]='R';
				GData[3]='S';
			int pos=4;
			for(int j=0;j<N;j++){
				memcpy(GData+pos,Chunks[j],5+Chunks[j]->size);
				pos+=5+Chunks[j]->size;
			};
			for(int j=4;j<GL;j++)GData[j]^=randoma[(j+255)&8191];
				SetPersistData(0,CUPLD.CurProfile,pd_public_rw,CUPLD.CurPage,(char*)GData,GL,PersDataSaveCallback2,this);
			free(GData);
			CUPLD.DataSent=1;
		};
	};
};
void PersDataSaveCallback(int localid, int profileid, persisttype_t type, 
						  int index, int success, void *instance){
	int* res=(int*)instance;
	if(success){
		*res=1;
	}else *res=0;
};
void InternetStatsChunks::StartUpload(int ProfileID,char* Password,int Page){
	char validate[33];
	GenerateAuth(GetChallenge(NULL),Password,validate);
	int res=-1;
	CUPLD.Authenticated=0;
	CUPLD.Error=0;
	CUPLD.CurProfile=ProfileID;
	CUPLD.CurPage=Page;
	CUPLD.Finished=0;
	CUPLD.Started=1;
	CUPLD.DataSent=0;
	PreAuthenticatePlayerPM(0,ProfileID,validate,PersAuthCallback2,this);
};

void InternetStatsChunks::ProcessDownload(){
	PersistThink();
};
void PersAuthCallback(int localid, int profileid, int authenticated, 
					  char *errmsg, void *instance){
	int* res=(int*)instance;
	if(authenticated){
		*res=1;
	}else *res=0;
};



bool InternetStatsChunks::Upload(int ProfileID,char* Password,int Page){
	char validate[33];
	GenerateAuth(GetChallenge(NULL),Password,validate);
	int res=-1;
	PreAuthenticatePlayerPM(0,ProfileID,validate,PersAuthCallback,&res);
	do{
		ProcessMessages();
		PersistThink();
		ActiveProcess();
	}while(res==-1);
	if(res==1){
		res=-1;
		int GL=4;
		for(int j=0;j<N;j++)GL+=Chunks[j]->size+5;
		byte* GData=znew(byte,GL);
		GData[0]='G';
		GData[1]='I';
		GData[2]='R';
		GData[3]='S';
		int pos=4;
		for(int j=0;j<N;j++){
			memcpy(GData+pos,Chunks[j],5+Chunks[j]->size);
			pos+=5+Chunks[j]->size;
		};
		for(int j=4;j<GL;j++)GData[j]^=randoma[(j+255)&8191];
		SetPersistData(0,ProfileID,pd_public_rw,Page,(char*)GData,GL,PersDataSaveCallback,&res);
		free(GData);

		do{
			ProcessMessages();
			PersistThink();
			ActiveProcess();
		}while(res==-1);
		return res;
	}else return false;
};
void InternetStatsChunks::WriteChunk(byte* Src,int L,byte Type){
	if(!L)return;
	Chunks=(OneIChunk**)realloc(Chunks,4*N+4);
	Chunks[N]=(OneIChunk*)malloc(5+L);
	Chunks[N]->Index=Type;
	Chunks[N]->size=L;
	word S=0;
	for(int i=0;i<L;i++){
		S+=Src[i];
	};
	Chunks[N]->Summ=S;
	memcpy(Chunks[N]->Data,Src,L);
	N++;
};
void CheckProfileInit(){
	if(!ProfIsInit){
		InitProfileSystem();
	};
};
#define WRDT(tp,val) (*((tp*)(BUFF+bps)))=val;bps+=sizeof(tp);
/*
bool CurrentGame::UpdateGlobalInfo(){
	if(Active){
		if(!ProfIsInit){
			InitProfileSystem();
		};
		InternetStatsChunks ISC;
		if(ISC.Download(ProfileID,0)){
			int NREC=0;
			int NVict=0;
			int CURSCORE=0;
			for(int i=0;i<ISC.N;i++){
				if(ISC.Chunks[i]->Index==0){
					int w1=*((DWORD*)ISC.Chunks[i]->Data);
					int w2=*((DWORD*)(ISC.Chunks[i]->Data+4));
					int w3=*((DWORD*)(ISC.Chunks[i]->Data+8));
					int sco1=w1^DWORD(ProfileID);
					int sco2=w2^0xFB47E6C3;
					if(sco1==sco2){
						NREC=w3;
						NVict=w3;
						CURSCORE=sco1;
					};
				};
				if(ISC.Chunks[i]->Index==27){
					int w1=*((DWORD*)ISC.Chunks[i]->Data);
					int w2=*((DWORD*)(ISC.Chunks[i]->Data+4));
					int w3=*((DWORD*)(ISC.Chunks[i]->Data+8));
					int w4=*((DWORD*)(ISC.Chunks[i]->Data+12));
					int sco1=w1^DWORD(ProfileID);
					int sco2=w2^0xFB47E6C3;
					if(sco1==sco2){
						NREC=w3;
						NVict=w4;
						CURSCORE=sco1;
					};
				};
			};
			ISC.Clear();
			if(NREC&&(NREC%10!=0)){
				if(!ISC.Download(ProfileID,2+(NREC/10))){
					//unable to save results right now
					return false;
				};
			};
			byte BUFF[2048];
			int bps=0;

			SYSTEMTIME ST;
			ST.wDay=StartTime.Day;
			ST.wDayOfWeek=StartTime.DayOfWeek;
			ST.wHour=StartTime.Hour;
			ST.wMilliseconds=0;
			ST.wMinute=StartTime.Min;
			ST.wMonth=StartTime.Month;
			ST.wSecond=0;
			ST.wYear=StartTime.Year;
			FILETIME FT;
			SystemTimeToFileTime(&ST,&FT);

			SYSTEMTIME ST1;
			ST1.wDay=CurTime.Day;
			ST1.wDayOfWeek=CurTime.DayOfWeek;
			ST1.wHour=CurTime.Hour;
			ST1.wMilliseconds=0;
			ST1.wMinute=CurTime.Min;
			ST1.wMonth=CurTime.Month;
			ST1.wSecond=0;
			ST1.wYear=CurTime.Year;
			FILETIME FT1;
			SystemTimeToFileTime(&ST1,&FT1);

			FILETIME DT;
			LARGE_INTEGER LI1;
			LARGE_INTEGER LI2;
			LARGE_INTEGER LI3;
			memcpy(&LI1,&FT,8);
			memcpy(&LI2,&FT1,8);
			LI3.QuadPart=LI2.QuadPart-LI1.QuadPart;
			memcpy(&DT,&LI3,8);
			FileTimeToSystemTime(&DT,&ST);

			DWORD Date=StartTime.Min+StartTime.Hour*60+StartTime.Day*1440+
				StartTime.Month*44640+(StartTime.Year-2000)*535680;

			WRDT(DWORD,Date);
			WRDT(byte,NPL);
			Date=ST.wMinute+ST.wHour*60+(ST.wDay-1)*60*24;
			WRDT(word,Date);
			WRDT(byte,GameType);
			WRDT(byte,0);
			WRDT(byte,strlen(MapName));
			memcpy(BUFF+bps,MapName,strlen(MapName));
			bps+=strlen(MapName);
			WRDT(byte,strlen(GameName));
			memcpy(BUFF+bps,GameName,strlen(GameName));
			bps+=strlen(GameName);
			for(int i=0;i<NPL;i++){
				WRDT(byte,PLAYERS[i].Rank);
				WRDT(byte,PLAYERS[i].State);
				WRDT(DWORD,PLAYERS[i].Score);
				WRDT(byte,PLAYERS[i].Mask);
				WRDT(byte,PLAYERS[i].Color);
				WRDT(byte,PLAYERS[i].NationID);
				WRDT(byte,PLAYERS[i].Reserved[0]);
				WRDT(byte,PLAYERS[i].Reserved[1]);
				WRDT(byte,PLAYERS[i].Reserved[2]);
				WRDT(byte,PLAYERS[i].Reserved[3]);
				WRDT(byte,strlen(PLAYERS[i].Nick));
				memcpy(BUFF+bps,PLAYERS[i].Nick,strlen(PLAYERS[i].Nick));
				bps+=strlen(PLAYERS[i].Nick);
			};
			ISC.WriteChunk(BUFF,bps,2);
			NREC++;
			int dsc=GetAddScore();
			CURSCORE+=dsc;
			if(dsc>0)
			ISC.Upload(ProfileID,password,2+(NREC/10));
			ISC.Clear();
			DWORD XX[4];
			XX[0]=CURSCORE^DWORD(ProfileID);
			XX[1]=CURSCORE^0xFB47E6C3;
			XX[2]=NREC;
			XX[3]=NVict;
			ISC.WriteChunk((byte*)XX,16,27);
			bool resx=ISC.Upload(ProfileID,password,0);
			//RewriteTop100(ProfileID,CURSCORE);
			return resx;
		};
	};
	return false;
};
*/
int GetRankByScore(int Score);

void EncodeGS_Password(char* pass,char* result){
	result[0]=strlen(pass);
	char cc3[512];
	strcpy(cc3,pass);
	do{
		strcat(cc3,pass);
	}while(strlen(cc3)<64);
	cc3[64]=0;
	for(int i=0;i<1024;i++){
		int v1=randoma[i+i]&511;
		int v2=randoma[i+i+1]&511;
		int vo1=v1>>3;
		int vo2=v2>>3;
		int p1=v1&7;
		int p2=v2&7;
		byte b1=cc3[vo1];
		byte b2=cc3[vo2];
		byte mb1=b1&(1<<p1);
		byte mb2=b2&(1<<p2);
		b1&=~(1<<p1);
		b2&=~(1<<p2);
		if(mb1)b2|=1<<p2;
		if(mb2)b1|=1<<p1;
		cc3[vo1]=b1;
		cc3[vo2]=b2;
	};
	memcpy(result+1,cc3,65);
};
void DecodeGS_Password(char* pass,char* result){
	byte Len=result[0];
	char* cc3=result+1;
	for(int i=1023;i>=0;i--){
		int v1=randoma[i+i]&511;
		int v2=randoma[i+i+1]&511;
		int vo1=v1>>3;
		int vo2=v2>>3;
		int p1=v1&7;
		int p2=v2&7;
		byte b1=cc3[vo1];
		byte b2=cc3[vo2];
		byte mb1=b1&(1<<p1);
		byte mb2=b2&(1<<p2);
		b1&=~(1<<p1);
		b2&=~(1<<p2);
		if(mb1)b2|=1<<p2;
		if(mb2)b1|=1<<p1;
		cc3[vo1]=b1;
		cc3[vo2]=b2;
	};
	pass[Len]=0;
	memcpy(pass,cc3,Len);
};

bool InitUser(int ProfileID,char* password){
	//if(!ProfIsInit){
	InitProfileSystem();
	if(!ProfIsInit)return false;
	//};
	InternetStatsChunks ISC;
	bool GETSCORE=0;
	if(!ISC.Download(ProfileID,0)){
		//need to create start data
		byte xx3[65];
		InternetStatsChunks ISC1;
		EncodeGS_Password(password,(char*)xx3);
		ISC1.WriteChunk(xx3,64,0);
		ISC1.Upload(ProfileID,password,1);

		char tmp_pass[128];
		DecodeGS_Password((char*)xx3,tmp_pass);

		DWORD XX[3];
		XX[0]=DWORD(ProfileID);
		XX[1]=0xFB47E6C3;
		XX[2]=0;
		ISC.WriteChunk((byte*)XX,12,0);
		return ISC.Upload(ProfileID,password,2)&&ISC.Upload(ProfileID,password,0);
	}else{
		//if(!((GetTickCount()>>8)&7)){
			byte xx3[65];
			InternetStatsChunks ISC1;
			EncodeGS_Password(password,(char*)xx3);
			ISC1.WriteChunk(xx3,64,0);
			ISC1.Upload(ProfileID,password,1);

			char tmp_pass[128];
			DecodeGS_Password(tmp_pass,(char*)xx3);
		//};
		LOGIN.Rank=0;
		int SCORE=0;
		for(int i=0;i<ISC.N;i++){
			if(ISC.Chunks[i]->Index==0||ISC.Chunks[i]->Index==27||ISC.Chunks[i]->Index==28){
				int w1=*((DWORD*)ISC.Chunks[i]->Data);
				int w2=*((DWORD*)(ISC.Chunks[i]->Data+4));
				int w3=*((DWORD*)(ISC.Chunks[i]->Data+8));
				int sco1=w1^DWORD(ProfileID);
				int sco2=w2^0xFB47E6C3;
				if(sco1==sco2){
					LOGIN.Rank=GetRankByScore(sco1);
					SCORE=sco1;
					GETSCORE=1;
				};
			};
		};
		//SCORE=19;
		/*
		if(GetKeyState(VK_CONTROL)&0x8000){
			SCORE=0;
			DWORD XX[3];
			XX[0]=SCORE^DWORD(ProfileID);
			XX[1]=SCORE^0xFB47E6C3;
			XX[2]=0;
			ISC.Clear();
			ISC.WriteChunk((byte*)XX,12,0);
			ISC.Upload(ProfileID,password,4);
			ISC.Upload(ProfileID,password,3);
			ISC.Upload(ProfileID,password,2);
			ISC.Upload(ProfileID,password,0);
		};
		*/
		
	};
	if(LOGIN.Logged&&LOGIN.ProfileID){
		if(!GETSCORE){
			int v1,v2,z;
			FILE* F=fopen("Internet\\prof.bak","r");
			if(F){
				do{
					z=fscanf(F,"%d%d",&v1,&v2);
					if(z==2){
						if(v1==LOGIN.ProfileID)LOGIN.Rank=v2;
					};
				}while(z==2);
				fclose(F);
			};
		};
		FILE* F=fopen("Internet\\prof.bak","a");
		fprintf(F,"%d %d\n",LOGIN.ProfileID,LOGIN.Rank);
		fclose(F);
	};

	return true;
};


CurrentGame CURIGAME;

int PREVUPDATETIME=0;
void UPDATEIGAME(){
	if(CURIGAME.Active){
		if((!PREVUPDATETIME)||(GetTickCount()-PREVUPDATETIME>60000)){
			CURIGAME.UpdateGame();
			PREVUPDATETIME=GetTickCount();
		};
	};
};
extern int ShowGameScreen;
void CheckExistingSaves();
void LOOSEIGAME(char* Nick){
	if(CURIGAME.Active){
		CURIGAME.UpdateGame();
		CURIGAME.AssignDefeat(Nick);
		CURIGAME.SaveGameToFile();
		ShowGameScreen=1;
		CheckExistingSaves();
		ShowGameScreen=0;
		CURIGAME.Active=0;
	};
};
void LOOSEANDEXITFAST(){
	if(CURIGAME.Active){
		for(int i=0;i<NPlayers;i++)if(PINFO[i].ColorID==MyNation){
			CURIGAME.UpdateGame();
			CURIGAME.AssignDefeat(PINFO[i].name);
			CURIGAME.SaveGameToFile();
			CURIGAME.Active=0;
		};
	};
};
void WINIGAME(char* Nick){
	if(CURIGAME.Active){
		CURIGAME.UpdateGame();
		CURIGAME.AssignVictory(Nick);
		CURIGAME.SaveGameToFile();
		ShowGameScreen=1;
		CheckExistingSaves();
		ShowGameScreen=0;
		CURIGAME.Active=0;
	};
};
void STARTIGAME(char* GameName,char* Map,char* Nick,DWORD G_GUID);
void UPDATEIGAME();
void LOOSEIGAME(char* Nick);
void WINIGAME(char* Nick);
char WinScoreTable[162]={
	5,-2,	10,-2,	20,-2,	30,-2,	40,-2,	50,-2,	60,-2,	70,-2,	80,-2,
	5,-2,	5,-2,	10,-2,	20,-2,	30,-2,	40,-2,	50,-2,	60,-2,	70,-2,
	5,-2,	5,-2,	5,-2,	10,-2,	20,-2,	30,-2,	40,-2,	50,-2,	60,-2,
	5,-2,	5,-2,	5,-2,	5,-2,	10,-2,	20,-2,	30,-2,	40,-2,	50,-2,
	5,-4,	5,-2,	5,-2,	5,-2,	5,-2,	10,-2,	20,-2,	30,-2,	40,-2,
	5,-6,	5,-4,	5,-2,	5,-2,	5,-2,	5,-2,	10,-2,	20,-2,	30,-2,
	5,-8,	5,-6,	5,-4,	5,-2,	5,-2,	5,-2,	5,-2,	10,-2,	20,-2,
	5,-16,	5,-8,	5,-6,	5,-4,	5,-2,	5,-2,	5,-2,	5,-2,	10,-2,
	5,-32,	5,-16,	5,-8,	5,-6,	5,-4,	5,-2,	5,-2,	5,-2,	5,-2};
int CurrentGame::GetAddScore(){
	byte MyMask=0;
	int MyIndex=-1;
	for(int i=0;i<NPL;i++){
		if(!strcmp(this->PLAYERS[i].Nick,InGameNick)){
			MyMask=PLAYERS[i].Mask;
			MyIndex=i;
		};
	};
	if(MyIndex==-1)return 0;
	int NFriends=0;
	int NOpp=0;
	int MyRank=PLAYERS[MyIndex].Rank;
	int SummOpRank=0;
	int SummFrRank=0;
	int ADDSCO=0;
	for(int i=0;i<NPL;i++){
		if(PLAYERS[i].Mask&MyMask){
			NFriends++;
			SummFrRank+=PLAYERS[i].Rank;
		}else{
			NOpp++;
			SummOpRank+=PLAYERS[i].Rank;
			ADDSCO+=WinScoreTable[(PLAYERS[i].Rank+MyRank*9)*2];
		};
	};
	if(!(NOpp&&NFriends))return 0;
	SummOpRank/=NOpp;
	SummFrRank/=NFriends;
	if(PLAYERS[MyIndex].State==0){//Victory!
		int ADDS=ADDSCO/NFriends;
		if(ADDS<5)ADDS=5;
		return ADDS;//WinScoreTable[(SummFrRank*9+SummOpRank)*2];
	}else{
		return WinScoreTable[(SummFrRank*9+SummOpRank)*2+1];
	};
};
int GetLogRank(){
	if(LOGIN.Logged)return LOGIN.Rank;
	else return 0;
};
void CheckExistingSaves(){
    if (!LOGIN.Logged)return;
    //CurrentGame CGM;
    CURIGAME.Active = 0;
    CURIGAME.LoadGameFromFile();
    CURIGAME.Active = 0;
};
void DontMakeRaiting(){
	if(CURIGAME.Active){
		CURIGAME.DeleteThisGameFromFile();
		CURIGAME.Active=0;
	};
};
void StopRaiting(){
	if(CURIGAME.Active){
		CURIGAME.Active=0;
	};
};
int PBackTime=0;
void DontMakeRaiting();
void ProcessUpdate(){
	if(PBackTime==0){
		PBackTime=GetTickCount()-100000;
	};
	if(GetTickCount()-PBackTime>100000){
		if(NPlayers<=1)DontMakeRaiting();
		PBackTime=GetTickCount();
		if(CURIGAME.Active)CURIGAME.UpdateGame();
	};
};
void GETCOUNTRY(char* code,char* res){
	strcpy(res,code);
	GFILE* F=Gopen("Players\\Countries.dat","r");
	if(F){
		char ccc[128];
		char CCN[16];
		ReadWinString(F,CCN+1,15);
		strcpy(CCN+1,code);
		_strupr(CCN);
		CCN[0]='=';
		do{
			ReadWinString(F,ccc,128);
			if(ccc[0]){
				if(strstr(ccc,CCN)){
					char* cc=strchr(ccc,'=');
					if(cc)cc[0]=0;
					strcpy(res,ccc);
				};
			};
		}while(ccc[0]);
		Gclose(F);
	};
};
//---------------------TOP100-------------------//

struct OneTopClient{
	int ProfileID;
	int Score;
	int NBattl;
	int NVict;
	int NIncomp;
	int LastRequestTime;

	bool OLD_Profile:1;
	bool OLD_PLINF:1;
	bool OLD_PARAM:1;
};

int TopProfile=5358612;
// Nick:     T100CEW_3742B
// Password: AV7LxvTuQ01w
//email      top100@top100.com
bool FirstTop100=1;
void RewriteTop100(int ProfileID,int Score);

int res3=-1;

int SECCOD0=0;
int SECCOD1=0;
int SECCOD2=0;
int SECCOD3=0;

void AddStrToURL(char* url,char* var,char* value,DWORD* HASH){
	char VAL[512];
	strcpy(VAL,value);
	int CODE=0;
	for(int i=0;i<strlen(VAL);i++){
		byte c=VAL[i];
		if(c<=32||c==34 ||c=='%'||c=='&' ||c=='*'
			    ||c==39 ||c=='+'||c=='\\'||c=='/'
			    ||c=='<'||c=='>'||c=='?' ||c==0xA0||c==0xAD)c='_';
		VAL[i]=c;
		CODE+=int(c)*int(c)+SECCOD0;
		CODE&=65535;
	};
	CODE*=SECCOD1;
	CODE&=0xFFFF;
	*HASH+=CODE*SECCOD2;
	*HASH&=0xFFFFFF;
	*HASH^=SECCOD3;
	sprintf(url+strlen(url),"%s=%s&",var,VAL);
};
void AddIntToURL(char* url,char* var,int value,DWORD* HASH){
	char cc2[32];
	sprintf(cc2,"%d",value);
	AddStrToURL(url,var,cc2,HASH);
};
void AddIntIdxToURL(char* url,char* var,int value,DWORD* HASH,int idx){
	char cc2[32];
	char cc3[32];
	sprintf(cc2,"%d",value);
	sprintf(cc3,"%s%d",var,idx);
	AddStrToURL(url,cc3,cc2,HASH);
};
void AddStrIdxToURL(char* url,char* var,char* value,DWORD* HASH,int idx){
	char cc3[32];
	sprintf(cc3,"%s%d",var,idx);
	AddStrToURL(url,cc3,value,HASH);
};
char HISET[100]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char GETCOD(int v){
	if(v<0)v=0;
	if(v>99)v=99;
	return HISET[v*60/100];
};
bool TestHash1(char*);				 
void RunHTTPC();
void SendPHPString(CurrentGame* CGM){
	GFILE* F=Gopen("Internet\\alternative.dat","r");
	if(!F)return;
	char ABBREV[256];
	char PHP[256];
	char MODE[128];
	bool r1,r2,r3;
	bool unfound=1;
	DWORD HASH=0;
	do{
		r1=ReadWinString(F,ABBREV,256);
		r2=ReadWinString(F,MODE,256);
		r3=ReadWinString(F,PHP,256);
		if(r1&&r2&&r3){
			if(strstr(CGM->GameName,ABBREV))unfound=false;
		}else return;
	}while(unfound);
	SECCOD0=0;
	SECCOD1=0;
	SECCOD2=0;
	SECCOD3=0;
	int L=strlen(ABBREV);
	for(int i=0;i<L;i++){
		SECCOD0+=ABBREV[i];
		SECCOD1+=ABBREV[i]*2;
		SECCOD2+=ABBREV[i]*3;
		SECCOD3+=ABBREV[i]*4;
	};
	for(int i=0;i<L-1;i++){
		SECCOD0+=ABBREV[i]*ABBREV[i+1]*4;
		SECCOD1+=ABBREV[i]*ABBREV[i+1]*3;
		SECCOD2+=ABBREV[i]*ABBREV[i+1]*2;
		SECCOD3+=ABBREV[i]*ABBREV[i+1];
	};
	for(int i=0;i<L-2;i++){
		SECCOD0+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*17;
		SECCOD1+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*13;
		SECCOD2+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*11;
		SECCOD3+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2];
	};
	for(int i=0;i<L-3;i++){
		SECCOD0+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*ABBREV[i+3]*11;
		SECCOD1+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*ABBREV[i+3]*21;
		SECCOD2+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*ABBREV[i+3]*3;
		SECCOD3+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*ABBREV[i+3];
	};
	SECCOD0&=32767;
	SECCOD1&=32767;
	SECCOD2&=32767;
	SECCOD3&=32767;

	Gclose(F);
	char ccc[4096];
	strcpy(ccc,PHP);
	char tmp[256];
	strcpy(tmp,CGM->GameName);
	char* sss=strstr(tmp," #GUID:");
	if(sss)sss[0]=0;
	if(tmp[0]==127)AddStrToURL(ccc,"GNAME",tmp+5,&HASH);
	else AddStrToURL(ccc,"GNAME",tmp,&HASH);
	//!!!!!!!
	if(sss)AddStrToURL(ccc,"GUID",sss+7,&HASH);
	else AddStrToURL(ccc,"GUID","00000000",&HASH);
	//!!!!!!!
	AddStrToURL(ccc,"MAP",CGM->MapName,&HASH);
	AddIntToURL(ccc,"GTIME",CGM->PlayingTime,&HASH);
	char cc5[32];
	sprintf(cc5,"%d,%d,%d,%d,%d",CGM->StartTime.Min,CGM->StartTime.Hour,CGM->StartTime.Day,CGM->StartTime.Month,CGM->StartTime.Year);
	AddStrToURL(ccc,"GSTART",cc5,&HASH);
	AddIntToURL(ccc,"NPLR",CGM->NPL,&HASH);
	int NTM=0;
	byte TML[8];
	for(int i=0;i<CGM->NPL;i++){
		AddStrIdxToURL(ccc,"PL",CGM->PLAYERS[i].Nick,&HASH,i);
		AddIntIdxToURL(ccc,"CL",CGM->PLAYERS[i].Color,&HASH,i);
		AddIntIdxToURL(ccc,"NT",CGM->PLAYERS[i].NationID,&HASH,i);
		switch(CGM->PLAYERS[i].State){
		case 0:
			AddStrIdxToURL(ccc,"ST","V",&HASH,i);
			break;
		case 1:
			AddStrIdxToURL(ccc,"ST","D",&HASH,i);
			break;
		default:
			AddStrIdxToURL(ccc,"ST","U",&HASH,i);
			break;
		};
		int mtm=-1;
		byte ms=CGM->PLAYERS[i].Mask;
		for(int j=0;j<NTM&&mtm==-1;j++)if(TML[j]&ms)mtm=j;
		if(mtm==-1){
			TML[NTM]=ms;
			mtm=NTM;
			NTM++;
		};
		AddIntIdxToURL(ccc,"TM",mtm+1,&HASH,i);
		AddIntIdxToURL(ccc,"score",CGM->PLAYERS[i].Score,&HASH,i);
		AddIntIdxToURL(ccc,"ID",CGM->PLAYERS[i].Profile,&HASH,i);
		if(!strcmp(MODE,"FULL")){
			//population
			AddIntIdxToURL(ccc,"MXP",CGM->PLAYERS[i].MaxPopul,&HASH,i);
			char cc6[256];
			cc6[0]=0;
			for(int p=0;p<32;p++){
				cc6[p]=GETCOD(CGM->PLAYERS[i].Popul[p]);
			};
			cc6[32]=0;
			AddStrIdxToURL(ccc,"STP",cc6,&HASH,i);
			//score
			AddIntIdxToURL(ccc,"MXS",CGM->PLAYERS[i].MaxScore,&HASH,i);
			cc6[0]=0;
			for(int p=0;p<32;p++){
				cc6[p]=GETCOD(CGM->PLAYERS[i].ScoreG[p]);
			};
			cc6[32]=0;
			AddStrIdxToURL(ccc,"STS",cc6,&HASH,i);
		};
	};
	sprintf_s(ccc+strlen(ccc), sizeof(ccc + strlen(ccc)), "SEQ=%d",HASH);
	//TestHash1(ccc);
	FILE* F1=fopen("Internet\\upload.dat","a");
	if(F1){
		fprintf(F1,"%s\n",ccc);
		fclose(F1);
		RunHTTPC();
	};
};
bool CheckGameTime();

bool IsGameActive(){
	return CURIGAME.Active;
};
int GetCurGamePtr(byte** Ptr){
	*Ptr=(byte*)&CURIGAME;
	return sizeof CURIGAME;
};
bool CheckLogin(){
	return LOGIN.Logged;
};
void UpdateCurGame(){
	if(CURIGAME.Active){
		SYSTEMTIME SYSTM;
		GetSystemTime(&SYSTM);
		CURIGAME.LastLoadTime.Year=SYSTM.wYear;
		CURIGAME.LastLoadTime.Month=SYSTM.wMonth;
		CURIGAME.LastLoadTime.Day=SYSTM.wDay;
		CURIGAME.LastLoadTime.Hour=SYSTM.wHour;
		CURIGAME.LastLoadTime.Min=SYSTM.wMinute;
		CURIGAME.LastLoadTime.DayOfWeek=SYSTM.wDayOfWeek;
		memcpy(&CURIGAME.CurTime,&CURIGAME.LastLoadTime,sizeof CURIGAME.LastLoadTime);
	};
};
bool CheckGameTime(){
	if(CURIGAME.Active){
		SYSTEMTIME ST;
		memset(&ST,0,sizeof ST);
		ST.wMinute=CURIGAME.CurTime.Min;
		ST.wHour=CURIGAME.CurTime.Hour;
		ST.wDay=CURIGAME.CurTime.Day;
		ST.wMonth=CURIGAME.CurTime.Month;
		ST.wYear=CURIGAME.CurTime.Year;
		FILETIME FT;
		SystemTimeToFileTime(&ST,&FT);
		SYSTEMTIME ST0;
		memset(&ST0,0,sizeof ST0);
		ST0.wMinute=CURIGAME.LastLoadTime.Min;
		ST0.wHour=CURIGAME.LastLoadTime.Hour;
		ST0.wDay=CURIGAME.LastLoadTime.Day;
		ST0.wMonth=CURIGAME.LastLoadTime.Month;
		ST0.wYear=CURIGAME.LastLoadTime.Year;
		FILETIME FT0;
		SystemTimeToFileTime(&ST0,&FT0);
		LARGE_INTEGER LI0;
		LARGE_INTEGER LI;
		memcpy(&LI0,&FT0,8);
		memcpy(&LI,&FT,8);
		LI.QuadPart-=LI0.QuadPart;
		memcpy(&FT,&LI,8);
		int dt=0;
		if(FileTimeToSystemTime(&FT,&ST)){
			dt=(ST.wDay-1)*24*60+ST.wHour*60+ST.wMinute;
		};
		return dt>=10;
	}else return false;
};
void SetNormAttr(){
	HANDLE H=CreateFile("cew.dll",GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(H==INVALID_HANDLE_VALUE)return;
	FILETIME F1,F2,F3;
	GetFileTime(H,&F1,&F2,&F3);
	F3=F1;
	F2=F2;
	SetFileTime(H,&F1,&F2,&F3);
	CloseHandle(H);
};
void CheckAttr(){
	/*
	HANDLE H=CreateFile("cew.dll",GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(H==INVALID_HANDLE_VALUE)return;
	FILETIME F1,F2,F3;
	GetFileTime(H,&F1,&F2,&F3);
	LONG
	if(memcmp(&F3,&F1,8)){
		CloseHandle(H);
		ResFile F=RReset("cew.dll");
		if(F!=INVALID_HANDLE_VALUE){
			RSeek(F,20532);
			byte DATA[200];
			memset(DATA,0,200);
			RBlockWrite(F,DATA,200);
			RClose(F);
		};
	}else CloseHandle(H);
	SetNormAttr();
	*/
};
bool TestHash1(char* s);
void TestHash(){
	return;
	char* ABBREV="CC@W";
	SECCOD0=0;
	SECCOD1=0;
	SECCOD2=0;
	SECCOD3=0;
	int L=strlen(ABBREV);
	for(int i=0;i<L;i++){
		SECCOD0+=ABBREV[i];
		SECCOD1+=ABBREV[i]*2;
		SECCOD2+=ABBREV[i]*3;
		SECCOD3+=ABBREV[i]*4;
	};
	for(int i=0;i<L-1;i++){
		SECCOD0+=ABBREV[i]*ABBREV[i+1]*4;
		SECCOD1+=ABBREV[i]*ABBREV[i+1]*3;
		SECCOD2+=ABBREV[i]*ABBREV[i+1]*2;
		SECCOD3+=ABBREV[i]*ABBREV[i+1];
	};
	for(int i=0;i<L-2;i++){
		SECCOD0+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*17;
		SECCOD1+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*13;
		SECCOD2+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*11;
		SECCOD3+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2];
	};
	for(int i=0;i<L-3;i++){
		SECCOD0+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*ABBREV[i+3]*11;
		SECCOD1+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*ABBREV[i+3]*21;
		SECCOD2+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*ABBREV[i+3]*3;
		SECCOD3+=ABBREV[i]*ABBREV[i+1]*ABBREV[i+2]*ABBREV[i+3];
	};
	SECCOD0&=32767;
	SECCOD1&=32767;
	SECCOD2&=32767;
	SECCOD3&=32767;



	DWORD Hash=0;
	//char URL[4096];
	SECCOD0=30209;
	SECCOD1=22165;
	SECCOD2=29993;
	SECCOD3=13245;
	TestHash1("www.mgadek.com/tools/ccaw_games_results.php?GNAME=CC@W_GAME!&GUID=9D0FE50A&MAP=RN0_5AC1_22505_00506000_5300000.m3d&GTIME=667&GSTART=13,12,20,9,2001&NPLR=2&PL0=[GSC]DEMON&CL0=5&NT0=5&ST0=V&TM0=1&score0=15401&MXP0=258&STP0=333444469BDFIJMOSTUWYabehikptxxj&MXS0=143&STS0=224479BGKNOQSUWYbccdfgghhllmmpvx&PL1=Drew&CL1=3&NT1=255&ST1=D&TM1=2&score1=653&MXP1=192&STP1=55566667ACDFGILMSVXadgjmpgjmotxl&MXS1=98&STS1=4AAAGGGKUUVVYcddklpqqrssuqrsvxsu&SEQ=3901964");
	/*
	AddStrToURL(URL,"","CC@W_GAME!",&Hash);
	//AddStrToURL(URL,"","9D0FE50A",&Hash);
	AddStrToURL(URL,"","RN0_5AC1_22505_00506000_5300000.m3d",&Hash);
	AddStrToURL(URL,"","667",&Hash);
	AddStrToURL(URL,"","13,12,20,9,2001",&Hash);
	AddStrToURL(URL,"","2",&Hash);
	AddStrToURL(URL,"","[GSC]DEMON",&Hash);
	AddStrToURL(URL,"","5",&Hash);
	AddStrToURL(URL,"","5",&Hash);
	AddStrToURL(URL,"","V",&Hash);
	AddStrToURL(URL,"","1",&Hash);
	AddStrToURL(URL,"","15401",&Hash);
	AddStrToURL(URL,"","258",&Hash);
	AddStrToURL(URL,"","333444469BDFIJMOSTUWYabehikptxxj",&Hash);
	AddStrToURL(URL,"","143",&Hash);
	AddStrToURL(URL,"","224479BGKNOQSUWYbccdfgghhllmmpvx",&Hash);
	AddStrToURL(URL,"","Drew",&Hash);
	AddStrToURL(URL,"","3",&Hash);
	AddStrToURL(URL,"","255",&Hash);
	AddStrToURL(URL,"","D",&Hash);
	AddStrToURL(URL,"","2",&Hash);
	AddStrToURL(URL,"","653",&Hash);
	AddStrToURL(URL,"","192",&Hash);
	AddStrToURL(URL,"","55566667ACDFGILMSVXadgjmpgjmotxl",&Hash);
	AddStrToURL(URL,"","98",&Hash);
	AddStrToURL(URL,"","4AAAGGGKUUVVYcddklpqqrssuqrsvxsu",&Hash);
	*/
};
bool TestHash1(char* s){
	char ccc[4096];
	strcpy(ccc,s);
	char* cc=strstr(ccc,"?");
	if(!cc)return false;
	cc++;
	char VAR[48];
	char VALUE[256];
	DWORD HASH=0;
	char URL[4096]="";
	bool FINAL=0;
	DWORD FINHASH;
	do{
		char* cc3=strstr(cc,"=");
		if(cc3){
			int L=cc3-cc;
			memcpy(VAR,cc,L);
			VAR[L]=0;
			char* cc4=strstr(cc3,"&");
			if(cc4){
				L=cc4-cc3-1;
				memcpy(VALUE,cc3+1,L);
				VALUE[L]=0;
				AddStrToURL(URL,VAR,VALUE,&HASH);
				cc=cc4+1;
			}else{ 
				FINAL=1;
				int z=sscanf(cc3+1,"%d",&FINHASH);
			};
		};
	}while(!FINAL);
	assert(FINHASH==HASH);
};
int GetGSC_Profile();
int GetMyProfile(){
	if(UseGSC_Login){
		return GetGSC_Profile();
	}else return LOGIN.ProfileID;
};
void SetBrokenState(){
	if(CURIGAME.Active){
		CURIGAME.Broken=1;
	};
};
//--------------------------CLANFILTER-----------------------

#include "..\http\HttpComm.h"
int T0=0;
char CLANR[32]="";
char PLNICK[32]="";
CHttpComm HTTPC;
DWORD SendHTTPRequest(char* URL){
	return HTTPC.AddRequest(URL);
};
bool CheckHTTPAnswer(DWORD Handle,int* size,byte* Data){
	HTTPC.ProcessRequests();
	*size=HTTPC.GetData(Handle,(void*)Data,*size);
	return (*size)!=0;
};
bool RejectThisPlayer=0;
DWORD CHECK_HANDLE=0;
DWORD SERV_HANDLES[6];
int NSERV=0;
DWORD SendOneRequest(char* serv,char* param){
	char ccc[512];
	sprintf_s(ccc,sizeof(ccc),"%s?%s",serv,param);
	return SendHTTPRequest(ccc);
};
int GetStrHASH1(char* s){
	int L=strlen(s);
	DWORD S=0;
	for(int i=0;i<L;i++){
		S+=s[i];
	};
	for(int i=0;i<L-1;i++){
		S+=int(s[i])*int(s[i+1]);
	};
	for(int i=0;i<L-2;i++){
		S-=int(s[i])*int(s[i+1])*int(s[i+2]);
	};
	for(int i=0;i<L-3;i++){
		S+=int(s[i])*int(s[i+1])*int(s[i+2])*int(s[i+3]);
	};
	for(int i=0;i<L-4;i++){
		S-=int(s[i])*int(s[i+1])*int(s[i+2])*int(s[i+3])*int(s[i+4]);
	};
	for(int i=0;i<L-5;i++){
		S+=int(s[i])*int(s[i+1])*int(s[i+2])*int(s[i+3])*int(s[i+4])*int(s[i+5]);
	};
	return S;
};
void SendAllRequests(char* clan,char* nick,char* mail){
	RejectThisPlayer=0;
	char ccc[512];
	sprintf_s(ccc,sizeof(ccc),"com=check&clan=%s&user=%s&mail=%s&code=%d",clan,nick,mail,GetTickCount()&65535);
	NSERV=0;
	GFILE* F=Gopen("Internet\\serv.dat","r");
	if(F){
		char cc[256];
		int z=0;
		do{
			int hash=0;
			z=Gscanf(F,"%s%d",cc,&hash);
			if(z==2&&NSERV<6){
				if(hash==GetStrHASH1(cc)){
					SERV_HANDLES[NSERV]=SendOneRequest(cc,ccc);
					NSERV++;
				};
			};
		}while(z==2&&NSERV<6);
		Gclose(F);
	};
	if(!NSERV){
		SERV_HANDLES[NSERV]=SendOneRequest("www.gsc-game.net/games/Cossacks-AW/scripts/checkuser.php",ccc);
		NSERV++;
	};
	CHECK_HANDLE=1;
};
char CLANX[256];
void SendPlayerRequest(char* Nick,char* mail){
	//char ccc[256];
	char CLAN[256];
	strcpy(CLAN,Nick);
	if(Nick[0]!='['){
		CHECK_HANDLE=0;
		return;
	};
	char* CC2=strstr(CLAN,"]");
	if(!CC2){
		CHECK_HANDLE=0;
		return;
	}else CC2[0]=0;
	_strupr(CLAN);
	char* nick=CC2+1;
	_strupr(nick);
	for(int i=0;i<strlen(nick);i++){
		char c=nick[i];
		if(c=='['||c==']'||c=='?'||c=='&'||c=='%'||c==39||c=='"'||c=='+'||c=='-'||c=='^')c='_';
		nick[i]=c;
	};
	strcpy(CLANX,CLAN+1);
	strcpy(CLANR,CLANX);
	strcpy(PLNICK,nick);
	SendAllRequests(CLANX,PLNICK,mail);
};
void SendPlayerRequest(){
	SendPlayerRequest(LOGIN.Nick,LOGIN.EMail);
};
void WaitWithMessage(char* Message);
int GetStrHASH(char* s){
	int L=strlen(s);
	DWORD S=0;
	for(int i=0;i<L;i++){
		S+=s[i];
	};
	for(int i=0;i<L-1;i++){
		S+=int(s[i])*int(s[i+1]);
	};
	for(int i=0;i<L-2;i++){
		S+=int(s[i])*int(s[i+1])*int(s[i+2]);
	};
	for(int i=0;i<L-3;i++){
		S+=int(s[i])*int(s[i+1])*int(s[i+2])*int(s[i+3]);
	};
	for(int i=0;i<L-4;i++){
		S+=int(s[i])*int(s[i+1])*int(s[i+2])*int(s[i+3])*int(s[i+4]);
	};
	for(int i=0;i<L-5;i++){
		S+=int(s[i])*int(s[i+1])*int(s[i+2])*int(s[i+3])*int(s[i+4])*int(s[i+5]);
	};
	return S;
};
bool CheckPlayerToExit(){
	if(CHECK_HANDLE){
		for(int i=0;i<NSERV;i++){
			char STR[4096];
			int sz=4096;
			if(!T0)T0=GetTickCount();
			if(CheckHTTPAnswer(SERV_HANDLES[i],&sz,(byte*)STR)){
				STR[255]=0;
				int HC1=0;
				int HC2=0;
				int zz=sscanf(STR,"%d%d",&HC1,&HC2);
				if(zz==2){
					if((HC1+37)*13==HC2){
						LocalGP BOR2("Interface\\bor2");
						sprintf(STR,GetTextByID("NO_CLAN_MEMBER_[%s]"),CLANX);
						WaitWithError4(STR,BOR2.GPID);
						HTTPC.FreeData(SERV_HANDLES[i]);
						CHECK_HANDLE=0;
						T0=0;
						RejectThisPlayer=1;
						return true;
					}else if((HC1+97)*17==HC2){
						HTTPC.FreeData(SERV_HANDLES[i]);
						CHECK_HANDLE=0;
						T0=0;
						return false;
					};
				};
			};
			if(T0&&GetTickCount()-T0>12000){
				//search for passport
				bool GOODPL=0;
				char STR3[128];
				sprintf(STR3,"[%s]%s",CLANR,PLNICK);
				int HASH=GetStrHASH(STR3);
				GFILE* GF=Gopen("Internet\\PASS.DAT","r");
				if(GF){
					int z=0;
					DWORD pass=0;
					do{
						z=Gscanf(GF,"%d",&pass);
						if(z==1){
							if(HASH==pass)GOODPL=1;
						};
					}while(z==1);
					Gclose(GF);
				};
				if(!GOODPL){
					LocalGP BOR2("Interface\\bor2");
					sprintf(STR,GetTextByID("CANT_CHECK[%s]"),CLANR);
					WaitWithError4(STR,BOR2.GPID);
					//HTTPC.FreeData(CHECK_HANDLE);
					CHECK_HANDLE=0;
					T0=0;
					RejectThisPlayer=1;
					return true;
				}else{
					CHECK_HANDLE=0;
					T0=0;
					return false;
				};
			};
		};
	};
	return false;
};