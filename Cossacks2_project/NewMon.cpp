//New monsters format
//*******************************************************//
//         K   K  EEEEE  RRRR   N    N  EEEEE  L         //
//         K  K   E      R   R  NN   N  E      L         //
//         K K    E      R   R  N N  N  E      L         //
//         KK     EEEE   RRRR   N  N N  EEEE   L         //
//         K K    E      R   R  N   NN  E      L         //
//         K  K   E      R    R N    N  E      L         //
//         K   K  EEEEE  R    R N    N  EEEEE  LLLLL     //
//*******************************************************//
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "ActiveScenary.h"
#include "fog.h"
#include "Megapolis.h"
#include <assert.h>
#include "walls.h"
#include "mode.h"
#include "GSound.h"
#include "MapSprites.h"
#include "NewMon.h"
#include "Math.h"
#include "GP_Draw.h"
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
#include "StrategyResearch.h"
#include "EinfoClass.h"
#include "Sort.h"

#ifdef _USE3D
#include "NewMon3D.h"
#include "..\3DLIB\gMotor\mCamera.h"
#include "Scape3D.h"
#endif // _USE3D

extern byte CaptState;
extern bool NOPAUSE;
extern ReportFn* AttackLink;
void CheckArmies(City*);
void DestructBuilding(OneObject* OB);
int GetTopology(int x,int y, byte LockType=0);
int GetTopology(int* x,int* y, byte LockType=0);
bool CheckTopDirectWay(int x0,int y0,int x1,int y1,byte Type);
word CheckMotionThroughEnemyAbility(OneObject* OB,int px,int py);
bool CheckBuildPossibility(byte NI,int x,int y);
extern bool FastMode;
void GotoFinePosition(OneObject* OB);
extern bool AllowPathDelay;
extern byte   WeaponFlags[32];
NewAnimation* Shar;
NewAnimation* SharBuild;
void CheckUnits();
int SearchStr(char** Res,char* s,int count);
NewAnimation* GetNewAnimationByName(char* Name);
int GetResID(char* gy);
int GetResByName(char* gy);
void PerformMotion2(OneObject* OB);
int GetWeaponType(char* Name);
extern bool PeaceMode;
extern byte LockMode;
extern bool TransMode;
extern char* SoundID[MaxSnd];
extern word NSounds;
int CheckPt(int x,int y);
void LoadIcons();
int Prop43(int y);
extern byte* NPresence;
extern bool Mode3D;
extern short TSin[257];
extern short TCos[257];
extern short TAtg[257];
int PortBuiX,PortBuiY;
void NLine(GFILE* f);
int GetUnitKind(char* Name);
void ShowRLCItemPal(int x,int y,lpRLCTable lprt,int n,byte* Pal);
void ShowRLCItemGrad(int x,int y,lpRLCTable lprt,int n,byte* Pal);
void normstr(char* str);
void InitFlags(GeneralObject* GO);
int mul3(int);
void DrawMarker(OneObject* OB);
void DrawMiniMarker(OneObject* OB);
void ClearMaps();
int GetWallType(char* Name);
short LastDirection=512;
void CheckCapture(OneObject* OBJ);
//01-Order
//02-Attack
//04-select
void AddUnlimitedEffect(int x,int y,int id);
void MakeOrderSound(OneObject* OB,byte SMask){
	if(SMask==4&&OB->NNUM!=MyNation)return;
	Nation* NT=OB->Nat;
	if(NT->SoundMask[OB->NIndex]&SMask)return;
	NT->SoundMask[OB->NIndex]|=SMask;
	NewMonster* NM=OB->newMons;
	int SID=-1;
	switch(SMask){
	case 1:
		SID=NM->AttackSoundID;
		break;
	case 2:
		SID=NM->OrderSoundID;
		break;
	case 4:
		SID=NM->SelectSoundID;
	};
	if(SID==-1)return;
	AddSingleEffect(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,SID);
};
void ConvertToUp(char* str){
	while(str[0]){
		if(str[0]>='a'&&str[0]<='z'){
			str[0]-='a'-'A';
		};
		str++;
	};
};
int GetRAngle(int dx,int dy,int Angle){
	int ang;
	int sx=abs(dx);
	int sy=abs(dy);
	if(!(dx||dy))return 0;
	if(sx>sy){
		ang=div(dy<<8,sx).quot;
		if(dx<0)ang=1024-ang;
	}else{
		ang=512-div(dx<<8,sy).quot;
		if(dy<0)ang=2048-ang;
	};
	ang=Angle-ang;
	while(ang<0)ang+=2048;
	if(ang>1792)ang-=2048;
	return ang;
};
int GetLAngle(int dx,int dy,int Angle){
	int ang;
	int sx=abs(dx);
	int sy=abs(dy);
	if(!(dx||dy))return 0;
	if(sx>sy){
		ang=div(dy<<8,sx).quot;
		if(dx<0)ang=1024-ang;
	}else{
		ang=512-div(dx<<8,sy).quot;
		if(dy<0)ang=2048-ang;
	};
	ang-=Angle;
	while(ang<0)ang+=2048;
	if(ang>1792)ang-=2048;
	return ang;
};
bool PInside(int x,int y,int x1,int y1,int xp,int yp){
	if(xp>=x&&xp<=x1&&yp>=y&&yp<=y1)return true;
	else return false;
};
bool InClOrders=0;
void OneObject::ClearOrders(){
	InClOrders=1;
	//if(int(InLineCom))FreeAsmLink();
	while(LocalOrder)DeleteLastOrder();
	InClOrders=0;
	Attack=0;
    PrioryLevel=0;
	EnemyID=0xFFFF;
}
void InitAllGame(){
	ClearMaps();
	memset(Group,0,sizeof Group);
	memset(Sprites,0,sizeof Sprites);
	memset(NSL,0,sizeof NSL);
};
void DosToWin(char* Str){
	int len=strlen(Str);
	for(int i=0;i<len;i++){
		byte c=Str[i];
		if(c>=0x80&&c<0xB0)c+=0x40;
		else if(c>=0xE0&&c<0xF0)c+=0x10;
		Str[i]=c;
	};
};
extern int CurPalette;
extern bool PalDone;
CEXPORT void ErrM(char* s)
{
	if(PalDone){
		char pal[128];
		sprintf(pal,"%d\\agew_1.pal",CurPalette);
		LoadPalette(pal);
	};
	//MessageBox(hwnd,s,"LOADING FAILED...",MB_ICONWARNING|MB_OK);
	//assert(false);
};
void NEPar(char* name,int line,char* Sect,int Need){
	char gx[128];
	sprintf_s(gx,sizeof(gx),"%s,%d : %s : Not enough parameters. Need: %d",name,line,Sect,Need);
	ErrM(gx);
};
void InvPar(char* name,int line,char* Sect,char* parm){
	char gx[128];
	sprintf_s(gx,sizeof(gx),"%s,%d : %s : Invalid parameter: %s",name,line,Sect,parm);
	ErrM(gx);
};
void IncPar(char* name,int line,char* Sect){
	char gx[128];
	sprintf_s(gx,sizeof(gx),"%s,%d : %s : Incorrect parameters",name,line,Sect);
	ErrM(gx);
};
void UpConv(char* str){
	int i=0;
	char c;
	do{
		c=str[i];
		if(c>='a'&&c<='z')c-=0x20;
		str[i]=c;
		i++;
	}while(c!=0);
};
//------------------------------------------New RLC information
//int NRLFiles;//number of files with RLC pictures
//char* RLNames[512];
//RLCTable RMImages[512];
//RLCTable miniRMImages[512];
//RLCTable SRMImages[512];
//RLCTable miniSRMImages[512];
word RLCNSpr[MaxGPIdx];
word NNewMon;
NewMonster NewMon[750];
char* MonNames[750];
//-----------------New weapon animation discription------------
NewAnimation WNewAnm[768];
char* WAnmNames[768];
word NNewAnm;
//-------------------------------------------------------------
int CurrentCursorGP=0;
int AnmCursorGP=0;
extern int CBB_GPFILE;
void LoadMapInterface();
void InitNewMonstersSystem(){
	char* Name="icons";
	GPS.PreLoadGPImage(Name,false);
	GPS.PreLoadGPImage("smicons",false);
	GPS.PreLoadGPImage("flag",false);
	GPS.PreLoadGPImage("Interf2\\VScroll2",false);
	GPS.PreLoadGPImage("smicons1",false);
#ifdef _USE3D
	CurrentCursorGP = 
#endif //_USE3D
	GPS.PreLoadGPImage("Cursor_00");
	GPS.PreLoadGPImage("Cursor_01");
	GPS.PreLoadGPImage("Cursor_02");
	CBB_GPFILE=GPS.PreLoadGPImage("Interf2\\Combo2");
	LoadMapInterface();
	AnmCursorGP=GPS.PreLoadGPImage("Interface\\CursGo");
	NNewMon=0;
	NNewAnm=0;
	LoadIcons();
	UFONTS.LoadFonts();
};
NewAnimation::NewAnimation(){
	Enabled=false;
	ActiveFrame=0xFF;
	LineInfo=NULL;
};
NewMonster::NewMonster(){
	memset(&MotionL,0,sizeof NewMonster);
};
//--------------------Icons------------------
void ReadKeyCodes();
char* IconNames[4096];
byte KeyCodes[4096][2];
word NIcons;
void LoadMapInterface();
void LoadIcons(){
	NIcons=0;
	GFILE* f=Gopen("IconList.txt","r");
	AFile("IconList.txt");
	char gx[128];
	int z;
	if(f){
		do{
			z=Gscanf(f,"%s",gx);
			if(!strcmp(gx,"[END]"))return;
			assert(NIcons<4096);
			IconNames[NIcons]=znew(char,strlen(gx)+1);
			ConvertToUp(gx);
			strcpy(IconNames[NIcons],gx);
			NIcons++;
		}while(z==1);
	}else{
		ErrM("Unable to open IconList.txt");
	};
	ReadKeyCodes();
};
void FreeIcons(){
	for(int i=0;i<NIcons;i++)free(IconNames[i]);
};
int GetIconByName(char* Name){
	ConvertToUp(Name);
	for(int i=0;i<NIcons;i++){
		if(!strcmp(IconNames[i],Name))return i;
	};
	return -1;
};
char* ReadStatement(char* Src,char* Dst){
	Dst[0]=0;
	if(Src){
		while(Src[0]==' '||Src[0]==9)Src++;
		int z=sscanf(Src,"%s",Dst);
		if(z==1){
			Src+=strlen(Dst);
			return Src;
		}else return 0;
	}else return NULL;
};
#define NKEYS 68
char* KeyNames[NKEYS]={"0","1","2","3","4","5","6","7","8","9",
"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
"NUM0","NUM1","NUM2","NUM3","NUM4","NUM5","NUM6","NUM7","NUM8","NUM9",
"F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","PGUP","PGDN","HOME","END","INS","NUM+","NUM-","NUM*","NUM/","<",">","?"};

byte ScanKeys[NKEYS]={'0','1','2','3','4','5','6','7','8','9',
'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
VK_NUMPAD0,VK_NUMPAD1,VK_NUMPAD2,VK_NUMPAD3,VK_NUMPAD4,VK_NUMPAD5,VK_NUMPAD6,VK_NUMPAD7,VK_NUMPAD8,VK_NUMPAD9,
VK_F1,VK_F2,VK_F3,VK_F4,VK_F5,VK_F6,VK_F7,VK_F8,VK_F9,VK_F10,VK_NEXT,VK_PRIOR,VK_HOME,VK_END,VK_INSERT,107,109,106,111,188,190,191};

bool ReadWinString(GFILE* F,char* STR,int Max);
void ReadKeyCodes(){
	memset(&KeyCodes[0][0],0,sizeof KeyCodes);
	GFILE* F=Gopen("Data\\Keyboard.dat","r");
	if(F){
		char ccc[257];
		bool Done;
		do{
			Done=ReadWinString(F,ccc,256);
			if(Done&&ccc[0]!='/'){
				_strupr(ccc);
				char DST[256];
				char* Next;
				byte ScanCode=0;
				byte KeyState=0;
				int NCODES=0;
				bool IconMode=0;
				Next=ccc;
				do{
					Next=ReadStatement(Next,DST);
					if(Next){
						if(IconMode){
							int id=GetIconByName(DST);
							if(id==-1){
								char err[128];
								sprintf(err,"Unknown icon ID: %s",DST);
								ErrM(err);
							}else{
								KeyCodes[id][0]=ScanCode;
								KeyCodes[id][1]=KeyState;
							};
							}else{
							if(DST[0]==':'){
								IconMode=1;
							}else
							if(!strcmp(DST,"CTRL"))KeyState|=1;
							else if(!strcmp(DST,"ALT"))KeyState|=2;
							else if(!strcmp(DST,"SHIFT"))KeyState|=4;
							else{
								ScanCode=255;
								for(int j=0;j<NKEYS;j++)if(!strcmp(DST,KeyNames[j]))ScanCode=j;
								if(ScanCode==255){
									char err[128];
									sprintf(err,"Unknown key code: '%s'",DST);
									ErrM(err);
									ScanCode=0;
								};	
							};
						};
					};	
				}while(Next);
			};
		}while(Done);
	}else{
		ErrM("Unable to open Data\\Keyboard.dat");
	};
};
//-----------Loading additional New animations----------
extern NewAnimation* Shar;
void FreeWNames(){
	for(int i=0;i<NNewAnm;i++)free(WAnmNames[i]);
};
void LoadNewAimations(){
	GFILE* f1=Gopen("weapon.ads","r");
	AFile("weapon.ads");
	char Fn[128];
	char gx[128];
	char gy[128];
	char gz[128];
	int MaxRLC=-1;
	word RLCRef[128];
	short RLCdx[128];
	short RLCdy[128];
	int z1,z2,z3,p1,p2,p3;
	bool finish=false;
	int Line=1;
	int z=0;
	if(f1){
		do{
			z=Gscanf(f1,"%s",gx);
			if(z!=1)IncPar("weapon.ads",Line,"?");
			if(gx[0]!='/'&&gx[0]!='['){
                if(gx[0]=='@'){
					//Animation;
                    gx[0]='#';
                    WAnmNames[NNewAnm]=znew(char,strlen(gx)+1);
					strcpy(WAnmNames[NNewAnm],gx);
					NewAnimation* NANM=&WNewAnm[NNewAnm];
                    int z4;
					if(NANM){
						z=Gscanf(f1,"%d%d%d%d",&z1,&z2,&z3,&z4);
						if(z!=4)NEPar("weapon.ads",Line,gx,4);
						//NANM->StartDx=NAStartDx;
						//NANM->StartDy=NAStartDy;
						//NANM->Parts=NAParts;
						//NANM->PartSize=NAPartSize;
						NANM->Enabled=true;
						NANM->Rotations=z1;
						NANM->SoundID=-1;
						NANM->ActiveFrame=0;
						NANM->ActivePtX=znew(short,z1+z1+z1);
						NANM->ActivePtY=znew(short,z1+z1+z1);
						NANM->LineInfo=NULL;
						for(int i=0;i<z1+z1+z1;i++){
							NANM->ActivePtX[i]=0;
							NANM->ActivePtY[i]=0;
						};
						NANM->TicksPerFrame=1;
						int dz,nz;
						p1=RLCRef[z2];
						if(z3>=z4){
							if(z3*z1>=GPS.GPNFrames(p1)){
								sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Reference to RLC sprite is beyond the end of RLC file.",Line);
								ErrM(gy);
							};
							dz=-1;
							nz=z3-z4+1;
						}else{
							if(z4*z1>=GPS.GPNFrames(p1)){
								sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Reference to RLC sprite is beyond the end of RLC file.",Line);
								ErrM(gy);
							};
							dz=1;
							nz=z4-z3+1; 


						};
						NANM->NFrames=nz;
						NANM->Frames=new NewFrame[nz];
						for(int i=0;i<nz;i++){
							NewFrame* NF=&NANM->Frames[i];
							NF->FileID=p1;
							NF->SpriteID=z3;
							z3+=dz;
							NF->dx=RLCdx[z2];
							NF->dy=RLCdy[z2];
						};
						assert(NNewAnm<768);
                        NNewAnm++;
						NLine(f1);
						Line++;
					}else{
						gx[0]='@';
						sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Unknown animation type: %s",Line,gx);
						ErrM(gy);
					};
                }else
				if(gx[0]=='$'){
					//Animation;
                    gx[0]='#';
                    WAnmNames[NNewAnm]=znew(char,strlen(gx)+1);
					strcpy(WAnmNames[NNewAnm],gx);
					NewAnimation* NANM=&WNewAnm[NNewAnm];
                    int z4,dx,dy;
					if(NANM){
						z=Gscanf(f1,"%d%d%d%d%d%d",&dx,&dy,&z1,&z2,&z3,&z4);
						if(z!=6)NEPar("weapon.ads",Line,gx,6);
						//NANM->StartDx=NAStartDx;
						//NANM->StartDy=NAStartDy;
						//NANM->Parts=NAParts;
						//NANM->PartSize=NAPartSize;
						NANM->Enabled=true;
						NANM->Rotations=z1;
						NANM->ActiveFrame=0;
						NANM->SoundID=-1;
						NANM->ActivePtX=znew(short,z1+z1);
						NANM->ActivePtY=znew(short,z1+z1);
						NANM->LineInfo=NULL;
						for(int i=0;i<z1+z1+z1;i++){
							NANM->ActivePtX[i]=0;
							NANM->ActivePtY[i]=0;
						};
						NANM->TicksPerFrame=1;
						int dz,nz;
						p1=RLCRef[z2];
						if(z3>=z4){
							if(z3*z1>=GPS.GPNFrames(p1)){
								sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Reference to RLC sprite is beyond the end of RLC file.",Line);
								ErrM(gy);
							};
							dz=-1;
							nz=z3-z4+1;
						}else{
							if(z4*z1>=GPS.GPNFrames(p1)){
								sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Reference to RLC sprite is beyond the end of RLC file.",Line);
								ErrM(gy);
							};
							dz=1;
							nz=z4-z3+1;


						};
						NANM->NFrames=nz;
						NANM->Frames=new NewFrame[nz];
						for(int i=0;i<nz;i++){
							NewFrame* NF=&NANM->Frames[i];
							NF->FileID=p1;
							NF->SpriteID=z3;
							z3+=dz;
							NF->dx=dx;
							NF->dy=dy;
						};
						assert(NNewAnm<768);
                        NNewAnm++;
						NLine(f1);
						Line++;
					}else{
						gx[0]='@';
						sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Unknown animation type: %s",Line,gx);
						ErrM(gy);
					};
                }else
				if(gx[0]=='#'){
					//Animation;
					WAnmNames[NNewAnm]=znew(char,strlen(gx)+1);
					strcpy(WAnmNames[NNewAnm],gx);
					NewAnimation* NANM=&WNewAnm[NNewAnm];
					z=Gscanf(f1,"%d%d",&z1,&z2);
					if(z!=2)IncPar("weapon.ads",Line,gx);
					NANM->Enabled=true;
					NANM->NFrames=z2;
					NANM->Rotations=z1;
					NANM->ActiveFrame=0xFF;
					NANM->ActivePtX=znew(short,z1+z1+z1);
					NANM->ActivePtY=znew(short,z1+z1+z1);
					NANM->SoundID=-1;
					NANM->LineInfo=NULL;
					for(int i=0;i<z1+z1+z1;i++){
						NANM->ActivePtX[i]=0;
						NANM->ActivePtY[i]=0;
					};
					NANM->TicksPerFrame=1;
					NANM->Frames=new NewFrame[z2];
					for(int i=0;i<z2;i++){
						z=Gscanf(f1,"%d%d",&z1,&z3);
						if(z!=2)IncPar("weapon.ads",Line,gx);
						if(z1>MaxRLC){
							sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Incorrect RLC reference: %d",Line,z1);
							ErrM(gy);
						};
						NewFrame* NF=&NANM->Frames[i];
						NF->FileID=RLCRef[z1];
						if(z3>=GPS.GPNFrames(RLCRef[z1])){

							sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Reference to RLC sprite is beyond the end of file: %d",Line,z3);
							ErrM(gy);
						};
						NF->SpriteID=z3;
						NF->dx=RLCdx[z1];
						NF->dy=RLCdy[z1];
					};
					assert(NNewAnm<768);
					NNewAnm++;
					NLine(f1);
					Line++;
				}else
				if(gx[0]=='%'){
					//Animation;
					int ddx,ddy;
					WAnmNames[NNewAnm]=znew(char,strlen(gx)+1);
					strcpy(WAnmNames[NNewAnm],gx);
					NewAnimation* NANM=&WNewAnm[NNewAnm];
					z=Gscanf(f1,"%d%d%d%d",&ddx,&ddy,&z1,&z2);
					if(z!=4)IncPar("weapon.ads",Line,gx);
					NANM->Enabled=true;
					NANM->NFrames=z2;
					NANM->Rotations=z1;
					NANM->ActiveFrame=0xFF;
					NANM->HotFrame=0;
					NANM->SoundID=-1;
					NANM->ActivePtX=znew(short,z1+z1+z1);
					NANM->ActivePtY=znew(short,z1+z1+z1);
					NANM->LineInfo=NULL;
					for(int i=0;i<z1+z1+z1;i++){
						NANM->ActivePtX[i]=0;
						NANM->ActivePtY[i]=0;
					};
					NANM->TicksPerFrame=1;
					NANM->Frames=new NewFrame[z2];
					for(int i=0;i<z2;i++){
						z=Gscanf(f1,"%d%d",&z1,&z3);
						if(z!=2)IncPar("weapon.ads",Line,gx);
						if(z1>MaxRLC){
							sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Incorrect RLC reference: %d",Line,z1);
							ErrM(gy);
						};
						NewFrame* NF=&NANM->Frames[i];
						NF->FileID=RLCRef[z1];
						if(z3>=GPS.GPNFrames(RLCRef[z1])){
							sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Reference to RLC sprite is beyond the end of file: %d",Line,z3);
							ErrM(gy);
						};
						NF->SpriteID=z3;
						NF->dx=ddx;
						NF->dy=ddy;
					};
					NNewAnm++;
					NLine(f1);
					Line++;
				}else{
					//Simple command
					if(!strcmp(gx,"USERLC")){
						z=Gscanf(f1,"%d%s%s%d%d",&p1,gy,gz,&p2,&p3);
						if(z!=5)IncPar("weapon.ads",Line,"USERLC");
						if(p1>MaxRLC)MaxRLC=p1;
						UpConv(gy);
						int nr=GPS.PreLoadGPImage(gy,!strcmp(gz,"SHADOW"));
						RLCRef[p1]=nr;
						RLCdx[p1]=p2;
						RLCdy[p1]=p3;
						NLine(f1);
						Line++;
					}else
					if(!strcmp(gx,"SOUND")){
						z=Gscanf(f1,"%d%s%s",&p1,gy,Fn);
						if(z!=3)IncPar("Weapon.ads",Line,gx);
						NewAnimation* NANI=GetNewAnimationByName(gy);
						if(NANI){
							NANI->SoundID=SearchStr(SoundID,Fn,NSounds);
							if(NANI->SoundID==-1){
								sprintf(gz,"%s,line %d :%s: Unknown sound:%s","Weapon.ads",Line,gx,Fn);
								ErrM(gz);
							};
							NANI->HotFrame=p1;
						}else{
							sprintf(gz,"%s,line %d :%s: Unknown animation:%s","Weapon.ads",Line,gx,gy);
							ErrM(gz);
						};
					}else{
						sprintf_s(gy,sizeof(gy),"Weapon.ads,line %d : Unknown command : %s",Line,gx);
						ErrM(gy);
					};
				};
			}else{
				if(!strcmp(gx,"[END]"))finish=true;
				NLine(f1);
				Line++;
			};
		}while((!finish)&&z!=-1);
		Gclose(f1);
		Shar=GetNewAnimationByName("#SHAR");
		if(!Shar){
			//ErrM("Could not find special animation:#SHAR");
		};
		SharBuild=GetNewAnimationByName("#SHARBUILD");
		if(!SharBuild){
			//ErrM("Could not find special animation:#SHARBUILD");
		};
		return;
	}else{
		if(!f1)ErrM("Could not open Weapon.ads");
		return;
	};

};
NewAnimation* GetNewAnimationByName(char* Name){
	for(int i=0;i<NNewAnm;i++)
		if(!strcmp(WAnmNames[i],Name))return &WNewAnm[i];
	return NULL;
};
//-----------Loading new monsters discriptions----------
NewAnimation* NewMonster::LoadNewAnimationByName(char* gx){
	NewAnimation* NANM=NULL;
	if(!strcmp(gx,"#MOTION_L"))NANM=&MotionL;else
	if(!strcmp(gx,"#MOTION_R"))NANM=&MotionR;else
	if(!strcmp(gx,"#MOTION_LB"))NANM=&MotionLB;else
	if(!strcmp(gx,"#MOTION_RB"))NANM=&MotionRB;else
	if(!strcmp(gx,"#MINISTEP_L"))NANM=&MiniStepL;else
	if(!strcmp(gx,"#MINISTEP_R"))NANM=&MiniStepL;else
	if(!strcmp(gx,"#MINISTEP_LB"))NANM=&MiniStepL;else
	if(!strcmp(gx,"#MINISTEP_RB"))NANM=&MiniStepL;else
	if(!strcmp(gx,"#ATTACK"))NANM=&Attack[0];else
	if(!strcmp(gx,"#PATTACK"))NANM=&PAttack[0];else
	if(!strcmp(gx,"#UATTACK"))NANM=&UAttack[0];else
	if(!strcmp(gx,"#DEATH"))NANM=&Death;else
	if(!strcmp(gx,"#DEATHLIE1"))NANM=&DeathLie1;else
	if(!strcmp(gx,"#DEATHLIE2"))NANM=&DeathLie2;else
	if(!strcmp(gx,"#DEATHLIE3"))NANM=&DeathLie3;else
	if(!strcmp(gx,"#STAND"))NANM=&Stand;else
	if(!strcmp(gx,"#PSTAND"))NANM=&PStand[0];else
	if(!strcmp(gx,"#WORK"))NANM=&Work;else
	if(!strcmp(gx,"#FIST"))NANM=&Fist;else
	if(!strcmp(gx,"#STANDHI"))NANM=&StandHi;else
	if(!strcmp(gx,"#STANDLO"))NANM=&StandLo;else
	if(!strcmp(gx,"#BUILDHI"))NANM=&BuildHi;else
	if(!strcmp(gx,"#BUILD"))NANM=&Build;else
	if(!strcmp(gx,"#DAMAGE"))NANM=&Damage;else
	if(!strcmp(gx,"#TRANS01"))NANM=&Trans01;else
	if(!strcmp(gx,"#TRANS10"))NANM=&Trans10;else
	if(!strcmp(gx,"#ATTACK1"))NANM=&Attack[1];else
	if(!strcmp(gx,"#PSTAND1"))NANM=&PStand[1];else
	if(!strcmp(gx,"#PATTACK0"))NANM=&PAttack[0];else
	if(!strcmp(gx,"#UATTACK0"))NANM=&UAttack[0];else
	if(!strcmp(gx,"#ATTACK0"))NANM=&Attack[0];else
	if(!strcmp(gx,"#PSTAND0"))NANM=&PStand[0];else
	if(!strcmp(gx,"#MOTION_L0"))NANM=&PMotionL[0];else
	if(!strcmp(gx,"#MOTION_R0"))NANM=&PMotionR[0];else
	if(!strcmp(gx,"#MOTION_LB0"))NANM=&PMotionLB[0];else
	if(!strcmp(gx,"#MOTION_RB0"))NANM=&PMotionRB[0];else
	if(!strcmp(gx,"#MINISTEP_L0"))NANM=&PMiniStepL[0];else
	if(!strcmp(gx,"#MINISTEP_R0"))NANM=&PMiniStepL[0];else
	if(!strcmp(gx,"#MINISTEP_LB0"))NANM=&PMiniStepL[0];else
	if(!strcmp(gx,"#MINISTEP_RB0"))NANM=&PMiniStepL[0];else
	if(!strcmp(gx,"#PATTACK1"))NANM=&PAttack[1];else
	if(!strcmp(gx,"#UATTACK1"))NANM=&UAttack[1];else
	if(!strcmp(gx,"#ATTACK1"))NANM=&Attack[1];else
	if(!strcmp(gx,"#PSTAND1"))NANM=&PStand[1];else
	if(!strcmp(gx,"#MOTION_L1"))NANM=&PMotionL[1];else
	if(!strcmp(gx,"#MOTION_R1"))NANM=&PMotionR[1];else
	if(!strcmp(gx,"#MOTION_LB1"))NANM=&PMotionLB[1];else
	if(!strcmp(gx,"#MOTION_RB1"))NANM=&PMotionRB[1];else
	if(!strcmp(gx,"#MINISTEP_L1"))NANM=&PMiniStepL[1];else
	if(!strcmp(gx,"#MINISTEP_R1"))NANM=&PMiniStepL[1];else
	if(!strcmp(gx,"#MINISTEP_LB1"))NANM=&PMiniStepL[1];else
	if(!strcmp(gx,"#MINISTEP_RB1"))NANM=&PMiniStepL[1];else
	if(!strcmp(gx,"#PATTACK2"))NANM=&PAttack[2];else
	if(!strcmp(gx,"#UATTACK2"))NANM=&UAttack[2];else
	if(!strcmp(gx,"#ATTACK2"))NANM=&Attack[2];else
	if(!strcmp(gx,"#PSTAND2"))NANM=&PStand[2];else
	if(!strcmp(gx,"#MOTION_L2"))NANM=&PMotionL[2];else
	if(!strcmp(gx,"#MOTION_R2"))NANM=&PMotionR[2];else
	if(!strcmp(gx,"#MOTION_LB2"))NANM=&PMotionLB[2];else
	if(!strcmp(gx,"#MOTION_RB2"))NANM=&PMotionRB[2];else
	if(!strcmp(gx,"#MINISTEP_L2"))NANM=&PMiniStepL[2];else
	if(!strcmp(gx,"#MINISTEP_R2"))NANM=&PMiniStepL[2];else
	if(!strcmp(gx,"#MINISTEP_LB2"))NANM=&PMiniStepL[2];else
	if(!strcmp(gx,"#MINISTEP_RB2"))NANM=&PMiniStepL[2];else
	if(!strcmp(gx,"#PATTACK3"))NANM=&PAttack[3];else
	if(!strcmp(gx,"#UATTACK3"))NANM=&UAttack[3];else
	if(!strcmp(gx,"#ATTACK3"))NANM=&Attack[3];else
	if(!strcmp(gx,"#PSTAND3"))NANM=&PStand[3];else
	if(!strcmp(gx,"#MOTION_L3"))NANM=&PMotionL[3];else
	if(!strcmp(gx,"#MOTION_R3"))NANM=&PMotionR[3];else
	if(!strcmp(gx,"#MOTION_LB3"))NANM=&PMotionLB[3];else
	if(!strcmp(gx,"#MOTION_RB3"))NANM=&PMotionRB[3];else
	if(!strcmp(gx,"#MINISTEP_L3"))NANM=&PMiniStepL[1];else
	if(!strcmp(gx,"#MINISTEP_R3"))NANM=&PMiniStepL[1];else
	if(!strcmp(gx,"#MINISTEP_LB3"))NANM=&PMiniStepL[1];else
	if(!strcmp(gx,"#MINISTEP_RB3"))NANM=&PMiniStepL[1];else
#ifdef CONQUEST
	if(!strcmp(gx,"#PATTACK4"))NANM=&PAttack[4];else
	if(!strcmp(gx,"#UATTACK4"))NANM=&UAttack[4];else
	if(!strcmp(gx,"#ATTACK4"))NANM=&Attack[4];else
	if(!strcmp(gx,"#PSTAND4"))NANM=&PStand[4];else
	if(!strcmp(gx,"#MOTION_L4"))NANM=&PMotionL[4];else
	if(!strcmp(gx,"#MOTION_R4"))NANM=&PMotionR[4];else
	if(!strcmp(gx,"#MOTION_LB4"))NANM=&PMotionLB[4];else
	if(!strcmp(gx,"#MOTION_RB4"))NANM=&PMotionRB[4];else
	if(!strcmp(gx,"#MINISTEP_L4"))NANM=&PMiniStepL[4];else
	if(!strcmp(gx,"#MINISTEP_R4"))NANM=&PMiniStepL[4];else
	if(!strcmp(gx,"#MINISTEP_LB4"))NANM=&PMiniStepL[4];else
	if(!strcmp(gx,"#MINISTEP_RB4"))NANM=&PMiniStepL[4];else

	if(!strcmp(gx,"#PATTACK5"))NANM=&PAttack[5];else
	if(!strcmp(gx,"#UATTACK5"))NANM=&UAttack[5];else
	if(!strcmp(gx,"#ATTACK5"))NANM=&Attack[5];else
	if(!strcmp(gx,"#PSTAND5"))NANM=&PStand[5];else
	if(!strcmp(gx,"#MOTION_L5"))NANM=&PMotionL[5];else
	if(!strcmp(gx,"#MOTION_R5"))NANM=&PMotionR[5];else
	if(!strcmp(gx,"#MOTION_LB5"))NANM=&PMotionLB[5];else
	if(!strcmp(gx,"#MOTION_RB5"))NANM=&PMotionRB[5];else
	if(!strcmp(gx,"#MINISTEP_L5"))NANM=&PMiniStepL[5];else
	if(!strcmp(gx,"#MINISTEP_R5"))NANM=&PMiniStepL[5];else
	if(!strcmp(gx,"#MINISTEP_LB5"))NANM=&PMiniStepL[5];else
	if(!strcmp(gx,"#MINISTEP_RB5"))NANM=&PMiniStepL[5];

	if(!strcmp(gx,"#PATTACK6"))NANM=&PAttack[6];else
	if(!strcmp(gx,"#UATTACK6"))NANM=&UAttack[6];else
	if(!strcmp(gx,"#ATTACK6"))NANM=&Attack[6];else
	if(!strcmp(gx,"#PSTAND6"))NANM=&PStand[6];else
	if(!strcmp(gx,"#MOTION_L6"))NANM=&PMotionL[6];else
	if(!strcmp(gx,"#MOTION_R6"))NANM=&PMotionR[6];else
	if(!strcmp(gx,"#MOTION_LB6"))NANM=&PMotionLB[6];else
	if(!strcmp(gx,"#MOTION_RB6"))NANM=&PMotionRB[6];else
	if(!strcmp(gx,"#MINISTEP_L6"))NANM=&PMiniStepL[6];else
	if(!strcmp(gx,"#MINISTEP_R6"))NANM=&PMiniStepL[6];else
	if(!strcmp(gx,"#MINISTEP_LB6"))NANM=&PMiniStepL[6];else
	if(!strcmp(gx,"#MINISTEP_RB6"))NANM=&PMiniStepL[6];else

	if(!strcmp(gx,"#PATTACK7"))NANM=&PAttack[7];else
	if(!strcmp(gx,"#UATTACK7"))NANM=&UAttack[7];else
	if(!strcmp(gx,"#ATTACK7"))NANM=&Attack[7];else
	if(!strcmp(gx,"#PSTAND7"))NANM=&PStand[7];else
	if(!strcmp(gx,"#MOTION_L7"))NANM=&PMotionL[7];else
	if(!strcmp(gx,"#MOTION_R7"))NANM=&PMotionR[7];else
	if(!strcmp(gx,"#MOTION_LB7"))NANM=&PMotionLB[7];else
	if(!strcmp(gx,"#MOTION_RB7"))NANM=&PMotionRB[7];else
	if(!strcmp(gx,"#MINISTEP_L7"))NANM=&PMiniStepL[7];else
	if(!strcmp(gx,"#MINISTEP_R7"))NANM=&PMiniStepL[7];else
	if(!strcmp(gx,"#MINISTEP_LB7"))NANM=&PMiniStepL[7];else
	if(!strcmp(gx,"#MINISTEP_RB7"))NANM=&PMiniStepL[7];else

	if(!strcmp(gx,"#PATTACK8"))NANM=&PAttack[8];else
	if(!strcmp(gx,"#UATTACK8"))NANM=&UAttack[8];else
	if(!strcmp(gx,"#ATTACK8"))NANM=&Attack[8];else
	if(!strcmp(gx,"#PSTAND8"))NANM=&PStand[8];else
	if(!strcmp(gx,"#MOTION_L8"))NANM=&PMotionL[8];else
	if(!strcmp(gx,"#MOTION_R8"))NANM=&PMotionR[8];else
	if(!strcmp(gx,"#MOTION_LB8"))NANM=&PMotionLB[8];else
	if(!strcmp(gx,"#MOTION_RB8"))NANM=&PMotionRB[8];else
	if(!strcmp(gx,"#MINISTEP_L8"))NANM=&PMiniStepL[8];else
	if(!strcmp(gx,"#MINISTEP_R8"))NANM=&PMiniStepL[8];else
	if(!strcmp(gx,"#MINISTEP_LB8"))NANM=&PMiniStepL[8];else
	if(!strcmp(gx,"#MINISTEP_RB8"))NANM=&PMiniStepL[8];else

	if(!strcmp(gx,"#PATTACK9"))NANM=&PAttack[9];else
	if(!strcmp(gx,"#UATTACK9"))NANM=&UAttack[9];else
	if(!strcmp(gx,"#ATTACK9"))NANM=&Attack[9];else
	if(!strcmp(gx,"#PSTAND9"))NANM=&PStand[9];else
	if(!strcmp(gx,"#MOTION_L9"))NANM=&PMotionL[9];else
	if(!strcmp(gx,"#MOTION_R9"))NANM=&PMotionR[9];else
	if(!strcmp(gx,"#MOTION_LB9"))NANM=&PMotionLB[9];else
	if(!strcmp(gx,"#MOTION_RB9"))NANM=&PMotionRB[9];else
	if(!strcmp(gx,"#MINISTEP_L9"))NANM=&PMiniStepL[9];else
	if(!strcmp(gx,"#MINISTEP_R9"))NANM=&PMiniStepL[9];else
	if(!strcmp(gx,"#MINISTEP_LB9"))NANM=&PMiniStepL[9];else
	if(!strcmp(gx,"#MINISTEP_RB9"))NANM=&PMiniStepL[9];else

	if(!strcmp(gx,"#PATTACK10"))NANM=&PAttack[10];else
	if(!strcmp(gx,"#UATTACK10"))NANM=&UAttack[10];else
	if(!strcmp(gx,"#ATTACK10"))NANM=&Attack[10];else
	if(!strcmp(gx,"#PSTAND10"))NANM=&PStand[10];else
	if(!strcmp(gx,"#MOTION_L10"))NANM=&PMotionL[10];else
	if(!strcmp(gx,"#MOTION_R10"))NANM=&PMotionR[10];else
	if(!strcmp(gx,"#MOTION_LB10"))NANM=&PMotionLB[10];else
	if(!strcmp(gx,"#MOTION_RB10"))NANM=&PMotionRB[10];else
	if(!strcmp(gx,"#MINISTEP_L10"))NANM=&PMiniStepL[10];else
	if(!strcmp(gx,"#MINISTEP_R10"))NANM=&PMiniStepL[10];else
	if(!strcmp(gx,"#MINISTEP_LB10"))NANM=&PMiniStepL[10];else
	if(!strcmp(gx,"#MINISTEP_RB10"))NANM=&PMiniStepL[10];else

#endif
	if(!strcmp(gx,"#REST"))NANM=&Rest;else
	if(!strcmp(gx,"#REST1"))NANM=&Rest1;else
	if(!strcmp(gx,"#ROTATE_R"))NANM=&RotateR;else
	if(!strcmp(gx,"#ROTATE_L"))NANM=&RotateL;else
	if(!strcmp(gx,"#WORKTREE"))NANM=&WorkTree;else
	if(!strcmp(gx,"#WORKSTONE"))NANM=&WorkStone;else
	if(!strcmp(gx,"#WORKFIELD"))NANM=&WorkField;else
	if(!strcmp(gx,"#BUILDLO_0"))NANM=&PMotionL[0];else
	if(!strcmp(gx,"#BUILDLO_1"))NANM=&PMotionL[1];else
	if(!strcmp(gx,"#BUILDLO_2"))NANM=&PMotionL[2];else
	if(!strcmp(gx,"#BUILDLO_3"))NANM=&PMotionL[3];else
	if(!strcmp(gx,"#BUILDHI_0"))NANM=&PMotionR[0];else
	if(!strcmp(gx,"#BUILDHI_1"))NANM=&PMotionR[1];else
	if(!strcmp(gx,"#BUILDHI_2"))NANM=&PMotionR[2];else
	if(!strcmp(gx,"#BUILDHI_3"))NANM=&PMotionR[3];else
    if(!strcmp(gx,"#HIANIMATION"))NANM=&Work;else
	if(!strcmp(gx,"#TRANSX3"))NANM=&TransX3;else
	if(!strcmp(gx,"#TRANS3X"))NANM=&Trans3X;
	return NANM;
};
int GetMatherialType(char* str){
    if(!strcmp(str,"BODY" ))return 1; else
    if(!strcmp(str,"STONE"))return 2; else
    if(!strcmp(str,"WOOD" ))return 4; else
    if(!strcmp(str,"IRON" ))return 8; else
    if(!strcmp(str,"FLY"  ))return 16;else
	if(!strcmp(str,"BUILDING"  ))return 32;else
	if(!strcmp(str,"WOOD_BUILDING"  ))return 64;else
	if(!strcmp(str,"STENA"))return 128;
    return -1;
};
int GetWeaponIndex(char* str);
char* GetTextByID(char* ID);
extern bool ProtectionMode;
int FindCTG(char* Ctg);
extern int IDLE_PX;
extern int IDLE_PY;
extern int IDLE_MX;
extern int IDLE_MY;
#ifdef GETTIRED
extern int Default_StandTiring;
extern int Default_RestTiring;
extern int Default_MotionTiring;
extern int Default_AttackTiring;
extern int Default_PAttackTiring;
extern int Default_UAttackTiring;
void SetDefaultAnmTiring(NewAnimation* NA,char* Name){
	NA->TiringChange=Default_StandTiring;
	if(strstr(Name,"#ATTACK"))NA->TiringChange=Default_AttackTiring;
	if(strstr(Name,"#MOTION"))NA->TiringChange=Default_MotionTiring;
	if(strstr(Name,"#REST"))NA->TiringChange=Default_RestTiring;
	if(strstr(Name,"#ATTACK"))NA->TiringChange=Default_AttackTiring;
	if(strstr(Name,"#UATTACK"))NA->TiringChange=Default_UAttackTiring;
	if(strstr(Name,"#PATTACK"))NA->TiringChange=Default_PAttackTiring;
};

extern int Morale_MinDueToTired;
extern int Morale_DecWhenTired;
extern City CITY[8];
extern int tmtmt;
__forceinline void ApplyTiring(OneObject* OB){
	int T=OB->GetTired;
	T+=OB->NewAnm->TiringChange<<SpeedSh;
	if(T<0)T=0;
	if(T>100*1000)T=100*1000;
	OB->GetTired=T;
	if(OB->BrigadeID!=0xFFFF){
		Brigade* BR=CITY[OB->NNUM].Brigs+OB->BrigadeID;
		if(tmtmt-BR->LastTireCheckTime>16){
			if(BR->NMemb){
				int AVM=0;
				int NA=0;
				int N=BR->NMemb;
				for(int i=0;i<N;i++){
					word MID=BR->Memb[i];
					if(MID!=0xFFFF){
						OneObject* OB=Group[MID];
						if(OB&&OB->Serial==BR->MembSN[i]){
							NA++;
							AVM+=OB->GetTired/1000;
						};
					};
				};
				if(NA){
					if(AVM<=NA)BR->IsTired=1;
					else BR->IsTired=0;
				};
				BR->LastTireCheckTime=tmtmt;

			};
		};
#ifdef NEWMORALE
		if(BR->IsTired){
			int N=32768/BR->NMemb;
			if(rando()<N){
				if(BR->Morale>Morale_MinDueToTired){
					BR->Morale-=Morale_DecWhenTired<<SpeedSh;
				};
			};
		};
	}else{
		if(T==0&&OB->Morale>Morale_MinDueToTired){
			OB->Morale-=Morale_DecWhenTired<<SpeedSh;
		};
	};
#else //NEWMORALE
	};
#endif //NEWMORALE
};
#define APPLY_TIRING(OB) ApplyTiring(OB)
#else 
#define SetDefaultAnmTiring(x,y)
#define APPLY_TIRING(OB)
#endif //GETTIRED
bool NewMonster::CreateFromFile(char* name){
	if(!name)return false;
	memset(this,0,sizeof *this);
	//NewMonster();
	char Fn[128];
	char gx[128];
	char gy[128];
	char gz[128];
	int MaxRLC=-1;
	word RLCRef[128];
	short RLCdx[128];
	short RLCdy[128];
	byte  RLCParts[128];
	byte  RLCPartSize[128];
	int z1,z2,z3,z4,p1,p2,p3,p4,p5,p6,xx,yy;
	sprintf(Fn,"%s.md",name);
	ProtectionMode=1;
	GFILE* f1=Gopen(Fn,"r");
	ProtectionMode=0;
	AFile(Fn);
	bool finish=false;
	short NAStartDx=0;
	short NAStartDy=0;
	byte NAParts=1;
	byte NAPartSize=96;
	RectDx=0;
	RectDy=3;
	RectLx=40;
	RectLy=64;
	ResEff=100;
	IconFileID=0xFFFF;
	CostPercent=100;
	ProduceStages=64;
	BattleForce=32;
	AnmUpShift=16;
	Archer=0;

	AttackSoundID=-1;
	OrderSoundID=-1;
	SelectSoundID=-1;
	BornSoundID=-1;
	DeathSoundID=-1;
	StrikeSoundID=-1;
	MinRotator=16;
	UnitRadius=16;
	ExitPause=8;
	Ves=10;
	TempDelay=30;
	FireLimit=0;
	StopDistance=256;
	for(int i=0;i<NAttTypes;i++){
		Rate[i]=16;
		WeaponKind[i]=0;
		DamageDecr[i]=65535;
		FearType[i]=0xFF;
	};
	int Line=1;
	int z;
    Sprite=0xFFFF; 
    SpriteVisual=0xFFFF;
	MD_File=znew(char,strlen(name)+1);
	strcpy(MD_File,name);
	ConvertToUp(name);
	sprintf_s(gx,sizeof(gx),"%s.MD",name);
	Message=GetTextByID(gx);
	DstZPoint=30;
	MeatTransformIndex=0xFF; 
	WaterCheckDist1=0;
	WaterCheckDist2=60;
	Page1Icon=0xFFFF;
	Page2Icon=0xFFFF;
	Page3Icon=0xFFFF;
	Page1IconON=0xFFFF;
	Page2IconON=0xFFFF;
	Page3IconON=0xFFFF;
	IdlePPos=IDLE_PX+IDLE_PY*12;
	IdleMPos=IDLE_MX+IDLE_MY*12;
	EuropeanMask=0;
	Psixoz=32;
	IconID=0xFFFF;
#ifdef COSSACKS2
	ComplexObjIndex=0xFFFF;
#endif
#ifdef NEWMORALE
	StartMorale=50;
	MoraleDecSpeed=100;
#endif //NEWMORALE
	if(Message==gx){
		sprintf_s(gy,sizeof(gy),"Could not find name for monster %s",gx);
		ErrM(gy);
	};
#ifndef NEWMORALE
	for(int i=0;i<NFEARSUBJ;i++)FearRadius[i]=255;
#endif
	if(f1){
		do{
			z=Gscanf(f1,"%s",gx);
			if(z!=1)IncPar(name,Line,"COMMAND");
			if(gx[0]!='/'&&gx[0]!='['){
				if(gx[0]=='#'){
					//Animation;
					NewAnimation* NANM=LoadNewAnimationByName(gx);
					if(NANM){
						SetDefaultAnmTiring(NANM,gx);
						z=Gscanf(f1,"%d%d",&z1,&z2);
						if(z!=2)IncPar(name,Line,gx);
						NANM->StartDx=NAStartDx;
						NANM->StartDy=NAStartDy;
						NANM->Parts=NAParts;
						NANM->PartSize=NAPartSize;
						NANM->Enabled=true;
						NANM->NFrames=z2;
						NANM->Rotations=z1;
						int nrot=z1;
						NANM->ActiveFrame=0;
						if(NANM->ActivePtX)free(NANM->ActivePtX);
						if(NANM->ActivePtY)free(NANM->ActivePtY);
						NANM->ActivePtX=znew(short,z1+z1+z1);
						NANM->ActivePtY=znew(short,z1+z1+z1);
						NANM->HotFrame=0;
						NANM->SoundID=-1;
						NANM->LineInfo=NULL;
						for(int i=0;i<z1+z1+z1;i++){
							NANM->ActivePtX[i]=0;
							NANM->ActivePtY[i]=0;
						};
						NANM->TicksPerFrame=1;
						if(NANM->Frames)delete[](NANM->Frames);
						NANM->Frames=new NewFrame[z2];
						for(int i=0;i<z2;i++){
							z=Gscanf(f1,"%d%d",&z1,&z3);
							if(z!=2)IncPar(name,Line,gx);
							if(z1>MaxRLC){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Incorrect RLC reference: %d",name,Line,z1);
								ErrM(gy);
							};
							NewFrame* NF=&NANM->Frames[i];
							NF->FileID=RLCRef[z1];
							if((z3+1)*nrot>GPS.GPNFrames(RLCRef[z1])){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Reference to RLC sprite is beyond the end of file: %d",name,Line,z3);
								ErrM(gy);
							};
							NF->SpriteID=z3;
							NF->dx=RLCdx[z1];
							NF->dy=RLCdy[z1];
						};
						NLine(f1);
						Line++;
					}else{
						sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown animation type: %s",name,Line,gx);
						ErrM(gy);
					};
				}else
				if(gx[0]=='@'){
					//Animation;
					gx[0]='#';
					NewAnimation* NANM=LoadNewAnimationByName(gx);
					if(NANM){
						SetDefaultAnmTiring(NANM,gx);
#ifdef _USE3D
						if (Monster3D)
						{
							NANM->Enabled		= true;
							NANM->NFrames		= 1;
							NANM->Frames		= new NewFrame[1];
							NANM->ActiveFrame	= 0;
							NANM->HotFrame		= 0;
							NANM->Rotations		= 1;
							NANM->SoundID		= -1;
							NANM->TicksPerFrame = 1;

							int nVal = Gscanf( f1, "%d%d", &NANM->firstFrame, &NANM->lastFrame );
							if (nVal != 2) NEPar( name, Line, gx, 2 );
						}
						else
						{
#endif // _USE3D

						z=Gscanf(f1,"%d%d%d%d",&z1,&z2,&z3,&z4);
						if(z!=4)NEPar(name,Line,gx,4);
						NANM->StartDx=NAStartDx;
						NANM->StartDy=NAStartDy;
						NANM->Parts=NAParts;
						NANM->PartSize=NAPartSize;
						NANM->Enabled=true;
						NANM->Rotations=z1;
						NANM->ActiveFrame=0;
						NANM->HotFrame=0;
						NANM->SoundID=-1;
						if(NANM->ActivePtX)free(NANM->ActivePtX);
						if(NANM->ActivePtY)free(NANM->ActivePtY);
						NANM->ActivePtX=znew(short,z1+z1+z1);
						NANM->ActivePtY=znew(short,z1+z1+z1);
						NANM->LineInfo=NULL;
						for(int i=0;i<z1+z1+z1;i++){
							NANM->ActivePtX[i]=0;
							NANM->ActivePtY[i]=0;
						};
						NANM->TicksPerFrame=1;
						int dz,nz;
						p1=RLCRef[z2];
						if(z3>=z4){
							if((z3+1)*z1>GPS.GPNFrames(p1)){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Reference to RLC sprite is beyond the end of RLC file.",name,Line);
								ErrM(gy);
							};
							dz=-1;
							nz=z3-z4+1;
						}else{
							if((z4+1)*z1>GPS.GPNFrames(p1)){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Reference to RLC sprite is beyond the end of RLC file.",name,Line);
								ErrM(gy);
							};
							dz=1;
							nz=z4-z3+1;


						};
						NANM->NFrames=nz;
						if(NANM->Frames)delete[](NANM->Frames);
						NANM->Frames=new NewFrame[nz];
						for(int i=0;i<nz;i++){
							NewFrame* NF=&NANM->Frames[i];
							NF->FileID=p1;
							NF->SpriteID=z3;
							z3+=dz;
							NF->dx=RLCdx[z2];
							NF->dy=RLCdy[z2];
						};
						NLine(f1);
						Line++;

#ifdef _USE3D
						}
#endif // _USE3D

					}else{
						gx[0]='@';
						sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown animation type: %s",name,Line,gx);
						ErrM(gy);
					};
				}else
				if(gx[0]=='$'){
					//Animation;
					gx[0]='#';
					NewAnimation* NANM=LoadNewAnimationByName(gx);
					if(NANM){
						SetDefaultAnmTiring(NANM,gx);
						int NPARTS=0;
						int P_File[128];
						int P_Start[128];
						int P_End[128];
						int NP=0;
						int NF=0;
						
						z=Gscanf(f1,"%d%d",&z1,&NP);
						if(z!=2)NEPar(name,Line,gx,2);
						for(int i=0;i<NP;i++){
							z=Gscanf(f1,"%d%d%d",&P_File[i],&P_Start[i],&P_End[i]);
							if(z!=3)IncPar(name,Line,gx);
							NF+=abs(P_Start[i]-P_End[i])+1;
						};
						NANM->StartDx=NAStartDx;
						NANM->StartDy=NAStartDy;
						NANM->Parts=NAParts;
						NANM->PartSize=NAPartSize;
						NANM->Enabled=true;
						NANM->Rotations=z1;
						NANM->ActiveFrame=0;
						NANM->HotFrame=0;
						NANM->SoundID=-1;
						if(NANM->ActivePtX)free(NANM->ActivePtX);
						if(NANM->ActivePtY)free(NANM->ActivePtY);
						NANM->ActivePtX=znew(short,z1+z1+z1);
						NANM->ActivePtY=znew(short,z1+z1+z1);
						NANM->LineInfo=NULL;
						for(int i=0;i<z1+z1+z1;i++){
							NANM->ActivePtX[i]=0;
							NANM->ActivePtY[i]=0;
						};
						NANM->TicksPerFrame=1;
						NANM->NFrames=NF;
						if(NANM->Frames)delete[](NANM->Frames);
						NANM->Frames=new NewFrame[NF];
						int p=0;
						for(int i=0;i<NP;i++){
							int s=P_Start[i]>P_End[i]?-1:1;
							int fn=P_End[i]+s;
							for(int sp=P_Start[i];sp!=fn;sp+=s){
								NewFrame* NF=&NANM->Frames[p];
								NF->FileID=RLCRef[P_File[i]];
								NF->SpriteID=sp;
								NF->dx=RLCdx[P_File[i]];
								NF->dy=RLCdy[P_File[i]];
								p++;
							};
						};
						NLine(f1);
						Line++;
					}else{
						gx[0]='$';
						sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown animation type: %s",name,Line,gx);
						ErrM(gy);
					};
				}else{
					int Line0=Line;
					//Simple command
					switch(gx[0]){
					case 'A':
						if(!strcmp(gx,"AISHIP")){
							char cc[32];
							int R1,R2;
							int z=Gscanf(f1,"%s%d%d",cc,&R1,&R2);
							if(z==3){
								if(!strcmp(cc,"B"))AI_use_against_buildings=1;
								AI_PreferredAttR_Min=R1;
								AI_PreferredAttR_Max=R2;
							}else IncPar(name,Line,gx);
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ANIMAL")){
							Animal=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ARMATTACK")){
							ArmAttack=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ARTDEPO")){
							p1=5;p2=5;p3=5;p4=5;
							z=Gscanf(f1,"%d%d%d%d",&p1,&p2,&p3,&p4);
							ArtDepo=true;
							ArtCap[0]=p1;
							ArtCap[1]=p2;
							ArtCap[2]=p3;
							ArtCap[3]=p4;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ADDSECTION")){
							//ADDSECTION Index Gp AddPts Empty x y  Stage1 x y Ready x y Dead x y
							int Idx,gp,apt,st1,x1,y1,st2,x2,y2,st3,x3,y3,st4,x4,y4;
							int z=Gscanf(f1,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",&Idx,&gp,&apt,
								&st1,&x1,&y1,
								&st2,&x2,&y2,
								&st3,&x3,&y3,
								&st4,&x4,&y4);
							if(z==15){
								if(!CompxCraft){
									CompxCraft=new ComplexBuilding;
									memset(CompxCraft,0,sizeof ComplexBuilding);
								};
								if(Idx>=MaxAStages){
									sprintf_s(gy,sizeof(gy),"%s,line %d : ADDSECTION : Too large stage index",name,Line);
									ErrM(gy);
								}else{
									CompxCraft->Mask|=1<<Idx;
									CompxCraft->Stages[Idx].GPID=RLCRef[gp];
									CompxCraft->Stages[Idx].AddPoints=apt;
	
									CompxCraft->Stages[Idx].Empty.SpriteID=st1;
									CompxCraft->Stages[Idx].Empty.SortX=x1;
									CompxCraft->Stages[Idx].Empty.SortY=y1;
	
									CompxCraft->Stages[Idx].Stage1.SpriteID=st2;
									CompxCraft->Stages[Idx].Stage1.SortX=x2;
									CompxCraft->Stages[Idx].Stage1.SortY=y2;
		
									CompxCraft->Stages[Idx].Ready.SpriteID=st3;
									CompxCraft->Stages[Idx].Ready.SortX=x3;
									CompxCraft->Stages[Idx].Ready.SortY=y3;
	
									CompxCraft->Stages[Idx].Dead.SpriteID=st4;
										CompxCraft->Stages[Idx].Dead.SortX=x4;
									CompxCraft->Stages[Idx].Dead.SortY=y4;
	
								};
							}else{
								sprintf_s(gy,sizeof(gy),"%s,line %d : ADDSECTION : Not enough or wrong parameters",name,Line);
								ErrM(gy);
							};
						}else
						if(!strcmp(gx,"ATTACK_PAUSE")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,"ATTACK_PAUSE");
							if(p1>=NAttTypes){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Attack type index in ATTACK_PAUSE is too big.",name,Line);
							ErrM(gy);
							};
							AttackPause[p1]=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ATTACK_ANGLES")){
							z=Gscanf(f1,"%d%d%d",&p1,&p2,&p3);
							if(z!=3)IncPar(name,Line,"ATTACK_ANGLES");
							if(p1>=NAttTypes){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Attack type index in SHOT_ANGLE is too big.",name,Line);
								ErrM(gy);
							};
							AngleUp[p1]=p3;
							AngleDn[p1]=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ADD_ATTACK_RADIUS")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,"ADD_ATTACK_RADIUS");
							if(p1>=NAttTypes){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Attack type index in ADD_ATTACK_RADIUS is too big.",name,Line);
								ErrM(gy);
							};
							AttackRadiusAdd[p1]=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ATTACK_RADIUS")){
							z=Gscanf(f1,"%d%d%d",&p1,&p2,&p3);
							if(z!=3)IncPar(name,Line,gx);
							if(p1>=NAttTypes){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Attack type index in ATTACK_RADIUS is too big.",name,Line);
								ErrM(gy);
							};
							AttackRadius1[p1]=p2;
							AttackRadius2[p1]=p3;
							DetRadius1[p1]=p2;
							DetRadius2[p1]=p3;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ATTMASK")){
							z=Gscanf(f1,"%d%d",&p1,&p4);
							if(z!=2)IncPar(name,Line,gx);
							AttackMask[p1]=0;
							for(int k=0;k<p4;k++){
								z=Gscanf(f1,"%s",gy);
								p2=GetMatherialType(gy);
								if(p2!=-1){
									AttackMask[p1]|=p2;
									if(!strcmp(gy,"BUILDING")){
										AttBuild=true;
									};
								}else{
									sprintf(gz,"%s, Line: %d :ATTMASK: Unknown matherial:%s",name,Line,gy);
									ErrM(gz);
								};
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ARCHER")){
							Archer=1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"ANMSH")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							AnmUpShift=p1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"ATTACK_SOUND")){
							z=Gscanf(f1,"%s",gy);
							AttackSoundID=SearchStr(SoundID,gy,NSounds);
							if(AttackSoundID==-1){
								sprintf(gz,"%s,line %d:%s: Unknown sound:%s",name,Line,gx,gy);
							};
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"AUTO_NO_ATTACK")){
							AutoNoAttack=1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"AUTO_STAND_GROUND")){
							AutoStandGround=1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"ARTPODGOTOVKA")){
							Artpodgotovka=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ADDSHOTRADIUS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							AddShotRadius=p1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'B':
						if(!strcmp(gx,"BREFLECT")){
							BReflection=true;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"BUILDING")){
							Building=true;
							Ves=500;
							WaterCheckDist1=180;
							WaterCheckDist2=250;
			                NLine(f1);
							FireLimit=15;
							Line++;
							EuropeanMask=100;
						}else
						if(!strcmp(gx,"BREAKANIMATION")){
							z=Gscanf(f1,"%s",gx);
							if(z!=1)IncPar(name,Line,gx);
							NewAnimation* NANM=LoadNewAnimationByName(gx);
							if(NANM){
								NANM->CanBeBroken=1;
							}else{
								sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown animation type: %s",name,Line,gx);
								ErrM(gy);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"BARABAN")){
							Baraban=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"BORN_SOUND")){
							z=Gscanf(f1,"%s",gy);
							BornSoundID=SearchStr(SoundID,gy,NSounds);
							if(BornSoundID==-1){
								sprintf(gz,"%s,line %d:%s: Unknown sound:%s",name,Line,gx,gy);
							};
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"BATTLEFORCE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							BattleForce=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"BUILDBAR")){
							z=Gscanf(f1,"%d%d%d%d",&p1,&p2,&p3,&p4);
							if(z!=4)IncPar(name,Line,gx);
							BuildX0=PicDx+(p1<<4);
							BuildY0=(PicDy+(p2<<3))<<1;
							BuildX1=PicDx+(p3<<4);
							BuildY1=(PicDy+(p4<<3))<<1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"BUILDLOCKPOINTS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							NBLockPt=p1;
							BLockX=znew(byte,p1);
							BLockY=znew(byte,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,gx);
								BLockX[i]=p2;
								BLockY[i]=p3;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"BORNPOINTS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"BORNPOINTS");
							NBornPt=p1;
							BornPtX=znew(char,p1);
							BornPtY=znew(char,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,"BORNPOINTS");
								BornPtX[i]=p2;
								BornPtY[i]=p3;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"BUILDPOINTS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"BUILDPOINTS");
							NBuildPt=p1;
							BuildPtX=znew(char,p1);
							BuildPtY=znew(char,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,"BUILDPOINTS");
								BuildPtX[i]=p2;
								BuildPtY[i]=p3;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"BUILDSTAGES")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"BUILDSTAGES");
							ProduceStages=p1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'C':
#ifdef COSSACKS2
						if(!strcmp(gx,"COMPLEXOBJECT")){
							z=Gscanf(f1,"%s",gy);
							word GetComplexObjectIndex(char* Name);
							ComplexObjIndex=GetComplexObjectIndex(gy);
							if(ComplexObjIndex==0xFFFF){
								sprintf(gz,"%s,line %d : Unknown complex object : %s",name,Line,gy);
								ErrM(gz);
							};
						}else
#endif //COSSACKS2
						if(!strcmp(gx,"CANSTOPBUILD")){
							CanStopBuild=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CATEGORY")){
							z=Gscanf(f1,"%s",gy);
							int z=FindCTG(gy);
							if(z==-1){
								sprintf(gz,"%s,line %d : Unknown category: %s",name,Line,gy);
								ErrM(gz);
							}else{
								Category=z;
							};
						}else
						if(!strcmp(gx,"COMMANDCENTER")){
							CommandCenter=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CHECKWATER")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,gx);
							WaterCheckDist1=p1;
							WaterCheckDist2=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"COST_FOOD")){
						    z=Gscanf(f1,"%d",&p1);
					        if(z!=1)NEPar(name,Line,"COST_FOOD",1);
				            NeedRes[3]=p1;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CANKILL")){
					        z=Gscanf(f1,"%d",&p1);
				            if(z!=1)IncPar(name,Line,gx);
			                for(int i=0;i<p1;i++){
		                        z=Gscanf(f1,"%s",gx);
	                            if(z!=1)IncPar(name,Line,gx);
								p2=GetMatherialType(gx);
							    if(p2!=-1){
						            KillMask|=p2;
					            }else{
				                    sprintf(gz,"%s, Line: %d :CANKILL: Unknown matherial:%s",name,Line,gx);
			                        ErrM(gz);
		                        };
	                        };
							for(int i=0;i<NAttTypes;i++){
								AttackMask[i]=KillMask;
							};
							NLine(f1);
							Line++;
	                    }else
						if(!strcmp(gx,"COST")){
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"COSTPERCENT")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							CostPercent=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CAPTURE")){
							Capture=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CANTCAPTURE")){
							CantCapture=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CHECKPOINTS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"CHECKPOINTS");
							NCheckPt=p1;
							CheckX=znew(byte,p1);
							CheckY=znew(byte,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,"CHECKPOINTS");
								CheckX[i]=p2;
								CheckY[i]=p3;
							};
							NLine(f1);
							Line++;
							//creating checking round
						    //1. center mass
					        int xc=0;
				            int yc=0;
			                for(int i=0;i<p1;i++){
		                        xc+=CheckX[i];
	                            yc+=CheckY[i];
						    };
					        xc=div(xc,p1).quot;
				            yc=div(yc,p1).quot;
			                int maxd=0;
		                    int r;
	                        for(int i=0;i<p1;i++){
							    r=Norma(int(CheckX[i])-xc,int(CheckY[i])-yc);
						        if(r>maxd)maxd=r;
					        };
				            CenterMX=xc;
			                CenterMY=yc;
		                    BRadius=maxd;
						}else
						if(!strcmp(gx,"CONCENTRATOR")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"CONCENTRATOR");
							NConcPt=p1;
							ConcPtX=znew(char,p1);
							ConcPtY=znew(char,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,"CONCENTRATOR");
								ConcPtX[i]=p2;
								ConcPtY[i]=p3;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CRAFTPOINTS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							NCraftPt=p1;
							CraftPtX=znew(char,p1);
							CraftPtY=znew(char,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,"CONCENTRATOR");
								CraftPtX[i]=p2;
								CraftPtY[i]=p3;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CANBEKILLEDINSIDE")){
							CanBeKilledInside=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CANBECAPTUREDWHENFREE")){
							CanBeCapturedWhenFree=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CANSHOOT")){
							CanShoot=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"CANSTORM")){
							CanStorm=1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'D':
						if(!strcmp(gx,"DOUBLE")){
							z=Gscanf(f1,"%s",gz);
							if(z!=1)IncPar(name,Line,gx);
							NewAnimation* NA=LoadNewAnimationByName(gz);
							if(!NA){
								sprintf_s(gy,sizeof(gy),"%s,line %d : DOUBLE %s - Unknown animation",name,Line,gz);
								ErrM(gy);
							}
							if((!NA->NFrames)||(NA->NFrames&1)){
								sprintf_s(gy,sizeof(gy),"%s,line %d : DOUBLE %s - Invalid amount of frames in animation (%d)!",name,Line,gz,NA->NFrames);
								ErrM(gy);
							};
							NA->NFrames>>=1;
							NA->DoubleAnm=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"DOOR")){
							z=Gscanf(f1,"%d",&p1);
							if(z==1){
								Doors=(ExRect*)malloc(sizeof ExRect+(p1-1)*16);
								Doors->NRects=0;
								for(int i=0;i<p1;i++){
									int x,y,x1,y1;
									z=Gscanf(f1,"%d%d%d%d",&x,&y,&x1,&y1);
									if(z==4){
										int i4=i<<2;
										Doors->Coor[i4  ]=x;
										Doors->Coor[i4+1]=y;
										Doors->Coor[i4+2]=x1;
										Doors->Coor[i4+3]=y1;
										Doors->NRects++;
									};
								};
							}else IncPar(name,Line,gx);
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"DET_RADIUS")){
							z=Gscanf(f1,"%d%d%d",&p1,&p2,&p3);
							if(z!=3)IncPar(name,Line,gx);
							if(p1>=NAttTypes){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Attack type index in ATTACK_RADIUS is too big.",name,Line);
								ErrM(gy);
							};
							DetRadius1[p1]=p2;
							DetRadius2[p1]=p3;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"DEATH_SOUND")){
							z=Gscanf(f1,"%s",gy);
							DeathSoundID=SearchStr(SoundID,gy,NSounds);
							if(DeathSoundID==-1){
								sprintf(gz,"%s,line %d:%s: Unknown sound:%s",name,Line,gx,gy);
							};
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"DESTRUCT")){
			                z=Gscanf(f1,"%d%d",&p1,&p2);
		                    if(z!=2)NEPar(name,Line,"DESTRUCT",2);
	                        Destruct.WProb=p1;
							Destruct.NWeap=p2;
						    Destruct.Weap=znew(word,p2);
					        for(int i=0;i<p2;i++){
				                z=Gscanf(f1,"%s",gz);
			                    if(z!=1)NEPar(name,Line,"DESTRUCT",2+p2);
		                        p3=GetWeaponIndex(gz);
							    if(p3==-1){
									sprintf_s(gy,sizeof(gy),"%s,line %d :DESTRUCT: Unknown weapon ID: %s",name,Line,gz);
							    	ErrM(gy);
						        };
					            Destruct.Weap[i]=p3;
				            };
							NLine(f1);
							Line++;
	                    }else
						if(!strcmp(gx,"DAMPOINTS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"DAMPOINTS");
							NDamPt=p1;
							DamPtX=znew(char,p1);
							DamPtY=znew(char,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,"DAMPOINTS");
								DamPtX[i]=p2;
								DamPtY[i]=p3;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"DAMAGE")){
							z=Gscanf(f1,"%d%d",&p2,&p3);
							if(z!=2)IncPar(name,Line,"DAMAGE");
							MinDamage[p2]=p3;
							MaxDamage[p2]=p3;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"DAMAGEDEC")){
							z=Gscanf(f1,"%d%d",&p2,&p3);
							if(z!=2)IncPar(name,Line,"DAMAGE");
							DamageDecr[p2]=p3;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"DIRECTTRANS")){
							z=Gscanf(f1,"%d%d",&p2,&p3);
							if(z!=2)IncPar(name,Line,gx);
							TransMask[p2]|=1<<p3;
							TransMask[p3]|=1<<p2;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'E':
						if(!strcmp(gx,"EVROUNIT")){
							EuropeanMask=1;							
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"EVROSHOT")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							EuropeanMask=p1;							
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"EXITPAUSE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							ExitPause=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"EXSPRITES")){
	                        z=Gscanf(f1,"%s%d%d%d%d",gy,&BRadius,&Radius1,&MotionDist,&Radius2);
		                    p1=COMPLEX.GetIndexByName(gy);
				            if(p1==-1){
					            sprintf(Fn,"%s, line %d : %s : Unknown Sprite Object : %s",name,Line,gx,gy);
						        ErrM(Fn);
							};
							ExField=1;
					        Sprite=p1;
						    SpriteVisual=p1;
							NLine(f1);
	                        Line++;
		                }else
						if(!strcmp(gx,"EXPLRADIUS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							ExplRadius=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"EXPLMEDIA")){
							z=Gscanf(f1,"%s%d",gy,&p1);
							if(z!=2)NEPar(name,Line,gx,2);
							p2=GetExMedia(gy);
							if(p2==-1){
								sprintf(gz,"%s , line %d : %s :Unknown media :%s",name,Line,gy,gx);
								ErrM(gz);
							};
							ExplosionMedia=p2;
							EMediaRadius=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"EX_SHOTS")){
					        z=Gscanf(f1,"%d",&p1);
				            if(z!=1)IncPar(name,Line,gx);
			                NShotPt=p1;
		                    ShotPtX=znew(short,p1);
	                        ShotPtY=znew(short,p1);
							ShotPtYG=znew(short,p1);
							ShotPtZ=znew(short,p1);
							ShotDelay=znew(short,p1);
							ShotAttType=znew(byte,p1);
							ShotDir=znew(byte,p1);
							ShotDiff=znew(byte,p1);
							ShotMinR=znew(word,p1);
							ShotMaxR=znew(word,p1);

		                    for(int i=0;i<p1;i++){
								//x y z minr maxr dir diff
								int x,y,z,minr,maxr,dir,diff,del,att;
							    int zz=Gscanf(f1,"%d%d%d%d%d%d%d%d%d",&x,&y,&z,&minr,&maxr,&dir,&diff,&del,&att);
						        if(zz!=9)IncPar(name,Line,gx);
					            ShotPtX[i]=x;
				                ShotPtY[i]=y;
								ShotPtYG[i]=y+z;
								ShotPtZ[i]=z;
		                        ShotMinR[i]=minr;
								ShotMaxR[i]=maxr;
								ShotDir[i]=dir;
								ShotDiff[i]=diff;
								ShotDelay[i]=del;
								ShotAttType[i]=att;
								NLine(f1);
								Line++;
		                    };
	                    }else
						if(!strcmp(gx,"EX_SHOTS2")){
					        z=Gscanf(f1,"%d",&p1);
				            if(z!=1)IncPar(name,Line,gx);
			                NShotPt=p1;
		                    ShotPtX=znew(short,p1);
	                        ShotPtY=znew(short,p1);
							ShotPtYG=znew(short,p1);
							ShotPtZ=znew(short,p1);
							ShotDelay=znew(short,p1);
							ShotAttType=znew(byte,p1);
							ShotDir=znew(byte,p1);
							ShotDiff=znew(byte,p1);
							ShotMinR=znew(word,p1);
							ShotMaxR=znew(word,p1);

		                    for(int i=0;i<p1;i++){
								//x y z minr maxr dir diff
								int x,y,z,minr,maxr,dir,diff,del,att,ygr;
							    int zz=Gscanf(f1,"%d%d%d%d%d%d%d%d%d%d",&x,&y,&z,&minr,&maxr,&dir,&diff,&del,&att,&ygr);
						        if(zz!=10)IncPar(name,Line,gx);
					            ShotPtX[i]=x;
				                ShotPtY[i]=y;
								ShotPtYG[i]=ygr;
								ShotPtZ[i]=z-y;
		                        ShotMinR[i]=minr;
								ShotMaxR[i]=maxr;
								ShotDir[i]=dir;
								ShotDiff[i]=diff;
								ShotDelay[i]=del;
								ShotAttType[i]=att;
								NLine(f1);
								Line++;
		                    };
	                    }else
						if(!strcmp(gx,"EXPLPOINTS")){
							int st;
							z=Gscanf(f1,"%d%d",&st,&p1);
							if(z!=2)IncPar(name,Line,gx);
							if(st<MaxAStages&&CompxCraft){
								CompxCraft->Stages[st].NExplPoints=p1;
								CompxCraft->Stages[st].ExplCoor=znew(short,p1*2);
								for(int i=0;i<p1;i++){
									z=Gscanf(f1,"%d%d",&p2,&p3);
									if(z!=2)IncPar(name,Line,gx);
									CompxCraft->Stages[st].ExplCoor[i+i]=p2;
									CompxCraft->Stages[st].ExplCoor[i+i+1]=p3;
								};
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"EXDAMAGE")){
							z=Gscanf(f1,"%d%d%d",&p1,&p2,&p3);
							if(z!=3)IncPar(name,Line,"EXDAMAGE");
							MinDamage[p1]=p2;
							MaxDamage[p1]=p3;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'F':
						if(!strcmp(gx,"FORMLIKESHOOTERS")){
							FormLikeShooters=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FEARRADIUS")){
							int idx,v;
							z=Gscanf(f1,"%d%d",&idx,&v);
							if(z==2&&idx<NFEARSUBJ){
								if(v>255)v=255;
								FearRadius[idx]=v;
							}else{
								sprintf(gz,"%s: Invalid FEARRADIUS parameters",name);
						        ErrM(gz);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FIRELIMIT")){
							int v;
							z=Gscanf(f1,"%d",&v);
							if(z==1){
								FireLimit=v;
							};
							NLine(f1);
							Line++;
						}else
#ifdef NEWMORALE
						if(!strcmp(gx,"FEARSTART")){
							int v;
							z=Gscanf(f1,"%d",&v);
							if(z==1){
								StartMorale=v;								
							}else{
								sprintf(gz,"%s: Invalid FEARSTART parameters",name);
						        ErrM(gz);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FEARNOT")){
							NoFear=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FEARSPEED")){
							int v;
							z=Gscanf(f1,"%d",&v);
							if(z==1){
								MoraleDecSpeed=v;								
							}else{
								sprintf(gz,"%s: Invalid FEARSTART parameters",name);
						        ErrM(gz);
							};
							NLine(f1);
							Line++;
						}
                        else
                            if (!strcmp(gx, "FEARNOT")) {
                                NoFear = 1;
                                NLine(f1);
                                Line++;
                            }
#ifdef NEWMORALEPRIEST
                            else
                                if (!strcmp(gx, "FORMMORALE")) {
                                    FormationMoralist = 1;
                                    NLine(f1);
                                    Line++;
                                }
#endif
                        else
						if(!strcmp(gx,"FEARFACTOR")){
							int idx,v;
							z=Gscanf(f1,"%d%d",&idx,&v);
							NLine(f1);
							Line++;
						}else
#else //NEWMORALE
						if(!strcmp(gx,"FEARFACTOR")){
							int idx,v;
							z=Gscanf(f1,"%d%d",&idx,&v);
							if(z==2&&idx<NFEARSUBJ){
								FearFactor[idx]=v;
							}else{
								sprintf(gz,"%s: Invalid FEARFACTOR parameters",name);
						        ErrM(gz);
							};
							NLine(f1);
							Line++;
						}else
#endif //NEWMORALE
						if(!strcmp(gx,"FEARTYPE")){
							int idx,v;
							z=Gscanf(f1,"%d%d",&idx,&v);
							if(z==2&&idx<NAttTypes&&v<NFEARSUBJ){
								FearType[idx]=v;
							}else{
								sprintf(gz,"%s: Invalid FEARTYPE parameters",name);
						        ErrM(gz);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FRIENDLYFIRE")){
							FriendlyFire=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FULLROTATION")){
							FullRotation=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FLAGS")){
							int xc,N,x1,x2,y2,dy;
							z=Gscanf(f1,"%d%d",&xc,&N);
							if(z==2){
								FLAGS=(Flags3D*)malloc(sizeof(Flags3D)-48*2+N*8);
								FLAGS->N=N;
								FLAGS->Xr=xc;
								for(int i=0;i<N;i++){
									z=Gscanf(f1,"%d%d%d%d",&x1,&x2,&y2,&dy);
									int i3=i<<2;
									FLAGS->Points[i3]=x1;
									FLAGS->Points[i3+1]=x2;
									FLAGS->Points[i3+2]=y2;
									FLAGS->Points[i3+3]=dy;
								};
							}else{
								sprintf(gz,"%s: Invalid FLAGS parameters",name);
						           ErrM(gz);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FARM")){
							NInFarm=UnitsPerFarm;
							Farm=true;
							NLine(f1);
							Ves=50;
							Line++;
						}else
						if(!strcmp(gx,"FORCE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							Force=p1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"FISHING")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,gx);
							FishSpeed=p1;
							FishAmount=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FASTUNIT")){
							z=Gscanf(f1,"%d",&p1);
							MaxAIndex=p1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"FOGGING")){
			                z=Gscanf(f1,"%d%d",&p1,&p2);
		                    if(z!=2)NEPar(name,Line,"FOGGING",2);
	                        Fogging.WProb=div(p1<<15,101).quot;
							Fogging.NWeap=p2;
						    Fogging.Weap=znew(word,p2);
					        for(int i=0;i<p2;i++){
				                z=Gscanf(f1,"%s",gz);
			                    if(z!=1)NEPar(name,Line,"FOGGING",2+p2);
		                        p3=GetWeaponIndex(gz);
							    if(p3==-1){
									sprintf_s(gy,sizeof(gy),"%s,line %d :FOGGING: Unknown weapon ID: %s",name,Line,gz);
							    	ErrM(gy);
						        };
					            Fogging.Weap[i]=p3;
				            };
							NLine(f1);
							Line++;
	                    }else
						if(!strcmp(gx,"FIRE")){
			                z=Gscanf(f1,"%d%d",&p1,&p2);
		                    if(z!=2)NEPar(name,Line,"FIRE",2);
	                        Fire.WProb=div(p1<<15,101).quot;
							Fire.NWeap=p2;
						    Fire.Weap=znew(word,p2);
					        for(int i=0;i<p2;i++){
				                z=Gscanf(f1,"%s",gz);
			                    if(z!=1)NEPar(name,Line,"FOGGING",2+p2);
		                        p3=GetWeaponIndex(gz);
							    if(p3==-1){
									sprintf_s(gy,sizeof(gy),"%s,line %d :FOGGING: Unknown weapon ID: %s",name,Line,gz);
							    	ErrM(gy);
						        };
					            Fire.Weap[i]=p3;
				            };
							NLine(f1);
							Line++;
	                    }else
						if(!strcmp(gx,"FIRES")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							FireX[0]=znew(short,p1);
							FireY[0]=znew(short,p1);
							NFires[0]=p1;
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,gx);
								FireX[0][i]=p2;
								FireY[0][i]=p3;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FOUNDATION")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							NHideTri=p1;
							HideTriX=znew(short,p1);
							HideTriY=znew(short,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,gx);
								HideTriX[i]=p2+PicDx;
								HideTriY[i]=p3+PicDy;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"FORWARD")){
							ShotForward=true;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'G':
						if(!strcmp(gx,"GRENADE")){
#ifdef COSSACKS2
							int n1,n2;
							if(Gscanf(f1,"%d%d",&n1,&n2)==2){
								GrenadeRechargeTime=n2>>3;
								MaxGrenadesInFormation=n1;
							}else IncPar(name,Line,"GRENADE");
#endif
						}else
						if(!strcmp(gx,"GEOMETRY")){
							z=Gscanf(f1,"%d%d%d",&p1,&p2,&p3);
							if(z!=3)IncPar(name,Line,"GEOMETRY");
							Radius1=p1<<4;
							Radius2=p2<<4;
							MotionDist=p3;
							for(int i=0;i<256;i++){
								POneStepDX[i]=(TCos[i]*MotionDist)>>4;
								POneStepDY[i]=(TSin[i]*MotionDist)>>4;
								OneStepDX[i]=(TCos[i]*MotionDist)>>8;
								OneStepDY[i]=(TSin[i]*MotionDist)>>8;
							};
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'H':
						if(!strcmp(gx,"HIGHUNIT")){
							HighUnit=1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"HIGHUNITCANTENTER")){
							HighUnitCantEnter=1;
							Line++;
							NLine(f1);
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'I':
						if(!strcmp(gx,"IDLE")){
							z=Gscanf(f1,"%d%d%d%d",&p1,&p2,&p3,&p4);
							if(z==4){
								IdlePPos=p1+p2*12;
								IdleMPos=p3+p4*12;
							};
						}else
						if(!strcmp(gx,"ICON")){
							z=Gscanf(f1,"%s",&gy);
							if(z!=1)IncPar(name,Line,"ICON");
							p2=GetIconByName(gy);
							if(p2==-1){
								sprintf(gz,"%s,%d : Unknown Icon ID: %s",name,Line,gy);
								ErrM(gz);
							};
							IconFileID=0;
							IconID=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"INFO")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,gx);
							InfType=p1;
							PictureID=p2;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'J':
						{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'K':
						if(!strcmp(gx,"KIND")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"KINO");
							Kind=p1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'L':
						if(!strcmp(gx,"CANNONLIKEMOTION")){
							LikeCannon=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"LOWCOSTRADIUS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							LowCostRadius=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"LINEARSORT")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							LinearLength=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"LINESORT")){
							z=Gscanf(f1,"%s",gz);
							NewAnimation* NANM=LoadNewAnimationByName(gz);
							if(NANM){
								short* LINF=znew(short,NANM->NFrames<<2);
								NANM->LineInfo=LINF;
								int LoIdx[20];
								int NLo=0;
								int MinY=10000;
								int MinX=10000;
								int MaxX=-10000;
								for(int i=0;i<NANM->NFrames;i++){
									int x1,y1,x2,y2;
									z=Gscanf(f1,"%s",gy);
									int p=i<<2;
									if(!strcmp(gy,"LINE")){
										z=Gscanf(f1,"%d%d%d%d",&x1,&y1,&x2,&y2);
										if(z!=4){
											IncPar(name,Line,gx);
										};
										LINF[ p ]=x1;
										LINF[p+1]=y1;
										LINF[p+2]=x2;
										LINF[p+3]=y2;
										if(y1<MinY)MinY=y1;
										if(x1<MinX)MinX=x1;
										if(x1>MaxX)MaxX=x1;
										if(y2<MinY)MinY=y2;
										if(x2<MinX)MinX=x2;
										if(x2>MaxX)MaxX=x2;
									}else
									if(!strcmp(gy,"POINT")){
										z=Gscanf(f1,"%d%d",&x1,&y1);
										if(z!=2){
											IncPar(name,Line,gx);
										};
										int p=i<<2;
										LINF[ p ]=x1;
										LINF[p+1]=y1;
										LINF[p+2]=x1;
										LINF[p+3]=y1;
										if(y1<MinY)MinY=y1;
										if(x1<MinX)MinX=x1;
										if(x1>MaxX)MaxX=x1;
									}else
									if(!strcmp(gy,"GROUND")){
										LINF[p  ]=-10000;
										LINF[p+1]=-10000;
										LINF[p+2]=-10000;
										LINF[p+3]=-10000;
										if(NLo<20){
											LoIdx[NLo]=i;
											NLo++;
										};
									}else
									if(!strcmp(gy,"TOP")){
										LINF[p  ]=10000;
										LINF[p+1]=10000;
										LINF[p+2]=10000;
										LINF[p+3]=10000;
									}else{
										sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown sort type(LINE,POINT,GROUND or TOP expected): %s",name,Line,gy);
										ErrM(gy);
									};
								};
								if(NLo){
									MinY=-10;
									int avx=(MinX+MaxX)>>1;
									for(int i=0;i<NLo;i++){
										int idx=LoIdx[i]<<2;
#ifdef _USE3D
										if(LINF[idx  ]!=-10000)LINF[idx  ]=avx;
#else
										LINF[idx  ]=avx;
#endif
										LINF[idx+1]=MinY;
										LINF[idx+2]=avx;
										LINF[idx+3]=MinY;
									};
								};
							}else{
								sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown animation type: %s",name,Line,gz);
								ErrM(gy);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"LOCATION")){
							z=Gscanf(f1,"%d%d%d%d",&p1,&p2,&p3,&p4);
							if(z!=4)IncPar(name,Line,"LOCATION");
							PicDx=p1;
							PicDy=p2;

							PicLx=p3;
							PicLy=p4;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"LOCKPOINTS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"LOCKPOINTS");
							NLockPt=p1;
							LockX=znew(byte,p1);
							LockY=znew(byte,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,"LOCKPOINTS");
								LockX[i]=p2;
								LockY[i]=p3;
								};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"LIFE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"LIFE");
							Life=p1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'M':
						if(!strcmp(gx,"MOVEBREAK")){
							z=Gscanf(f1,"%s",gx);
							if(z!=1)IncPar(name,Line,gx);
							NewAnimation* NANM=LoadNewAnimationByName(gx);
							if(NANM){
								NANM->MoveBreak=1;
							}else{
								sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown animation type: %s",name,Line,gx);
								ErrM(gy);
							};
							NLine(f1);
							Line++;
						}
#ifdef NEWMORALEPRIEST
                        else
                            if (!strcmp(gx, "MORALEHEALER")) {
                                MoraleHealer = 1;
                                NLine(f1);
                                Line++;
                            }
                        #endif
                            else
						if(!strcmp(gx,"MAXINDEPO")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)NEPar(name,Line,gx,1);
							if(p1>3)InvPar(name,Line,gx,"first parameter must be<4");
							NInArtDepot=p2;
							ArtSet=p1+1;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"MATHERIAL")){
		                    z=Gscanf(f1,"%d",&p1);
	                        if(z!=1)IncPar(name,Line,gx);
							for(int i=0;i<p1;i++){
						        z=Gscanf(f1,"%s",gx);
					            if(z!=1)IncPar(name,Line,gx);
				                p2=GetMatherialType(gx);
			                    if(p2!=-1){
		                            MathMask|=p2;
									if(!strcmp(gx,"BUILDING")){
										AttBuild=true;
									};
						        }else{
					                sprintf(gz,"%s, Line: %d :MATHERIAL: Unknown matherial:%s",name,Line,gy);
			                        ErrM(gz);
		                        };
				            };
	                    }else
						if(!strcmp(gx,"MESSAGE")){
							z=Gscanf(f1,"%s",gz);
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"MFARM")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							NInFarm=p1;
							Farm=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"MULTIWEAPON")){
							int rx,ry;
							z=Gscanf(f1,"%d%d%d%d",&rx,&ry,&p1,&p2);
							if(z!=4)IncPar(name,Line,gx);
							VesloRotX=rx;
							VesloRotY=ry;
							MultiWpZ=p1;
							NMultiWp=p2;
							MultiWpX=znew(short,p2);
							MultiWpY=znew(short,p2);
							for(int i=0;i<p2;i++){
								z=Gscanf(f1,"%d%d",&p3,&p4);
								MultiWpX[i]=(p3-rx);
								MultiWpY[i]=(p4-ry+p1)<<1;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"MEDIA")){
							z=Gscanf(f1,"%s",gy);
							if(!strcmp(gy,"LAND"))LockType=0;
							else if(!strcmp(gy,"WATER"))LockType=1;
							else if(!strcmp(gy,"2"))LockType=2;
							else if(!strcmp(gy,"3"))LockType=3;
							else if(!strcmp(gy,"4"))LockType=4;
							else{
								sprintf(gz,"%s, Line %d :%s: Unknown media: %s",name,Line,gx,gy);
								ErrM(gz);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"MOTIONSTYLE")){
							z=Gscanf(f1,"%s",gy);
							if(!strcmp(gy,"FASTROTATE&MOVE"))MotionStyle=0;
							else if(!strcmp(gy,"SLOWROTATE"))MotionStyle=1;
							else if(!strcmp(gy,"SHEEPS"))MotionStyle=2;
							else if(!strcmp(gy,"COMPLEXROTATE"))MotionStyle=3;
							else if(!strcmp(gy,"ROTATE&MOVE"))MotionStyle=4;
							else if(!strcmp(gy,"NEWSHEEPS"))MotionStyle=5;
							else if(!strcmp(gy,"COMPLEXOBJECT"))MotionStyle=6;
							else if(!strcmp(gy,"SINGLESTEP"))MotionStyle=7;
							else{
								sprintf(gz,"%s, Line %d :%s: Unknown media: %s",name,Line,gx,gy);
								ErrM(gz);
							};
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'N':
						if(!strcmp(gx,"NO25")){
							No25=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"NIKAKIXMAM")){
							NikakixMam=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"NEDORAZVITOE")){
							Nedorazvitoe=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"NAME")){
							z=Gscanf(f1,"%s",gy);
							if(z!=1)IncPar(name,Line,"NAME");
								Name=znew(char,strlen(gy)+1);
							strcpy(Name,gy);
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"NO_HUNGRY")){
							NotHungry=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"NAEMNIK")){
							Behavior=2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"NOFULLDESTRUCT")){
							NoDestruct=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"NOPAUSEDATTACK")){
							z=Gscanf(f1,"%d",&p2);
							if(z!=1)IncPar(name,Line,gx);
							NoWaitMask|=1<<p2;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'O':
						if(!strcmp(gx,"OFFICER")){
							Officer=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ORDER_SOUND")){
							z=Gscanf(f1,"%s",gy);
							OrderSoundID=SearchStr(SoundID,gy,NSounds);
							if(OrderSoundID==-1){
								sprintf(gz,"%s,line %d:%s: Unknown sound:%s",name,Line,gx,gy);
							};
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"OPPOSIT")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,gx);
							MinOposit=p1;
							MaxOposit=p2;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'P':
						if(!strcmp(gx,"PORTIONLIMIT")){
#ifdef COSSACKS2
							z=Gscanf(f1,"%d",&p1);
							if(z==1){
								PortionLimit=p1;
							}else NEPar(name,Line,gx,1);
#endif //COSSACKS2
						}else
						if(!strcmp(gx,"PIECE")){
							char pname[128];
							z=Gscanf(f1,"%s",pname);
							if(z==1){
								PieceName=(char*)malloc(strlen(pname)+1);
								strcpy(PieceName,pname);
							};
						}else
						if(!strcmp(gx,"PSIXOZ")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)NEPar(name,Line,"PEASANTABSORBER",1);
							Psixoz=p1;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PAGE1")){
							z=Gscanf(f1,"%s",gy);
							if(z==1){
								int ic=GetIconByName(gy);
								Page1Icon=ic;
								strcat(gy,"_ON");
								Page1IconON=GetIconByName(gy);
								if(Page1IconON==0xFFFF)Page1IconON=Page1Icon;
							}else NEPar(name,Line,gx,1);
						}else
						if(!strcmp(gx,"PAGE2")){
							z=Gscanf(f1,"%s",gy);
							if(z==1){
								int ic=GetIconByName(gy);
								Page2Icon=ic;
								strcat(gy,"_ON");
								Page2IconON=GetIconByName(gy);
								if(Page2IconON==0xFFFF)Page2IconON=Page2Icon;
							}else NEPar(name,Line,gx,1);
						}else
						if(!strcmp(gx,"PAGE3")){
							z=Gscanf(f1,"%s",gy);
							if(z==1){
								int ic=GetIconByName(gy);
								Page3Icon=ic;
								strcat(gy,"_ON");
								Page3IconON=GetIconByName(gy);
								if(Page3IconON==0xFFFF)Page3IconON=Page3Icon;
							}else NEPar(name,Line,gx,1);
						}else
						if(!strcmp(gx,"PAGE1HINT")){
							z=Gscanf(f1,"%s",gy);
							Page1Hint=GetTextByID(gy);
							if(Page1Hint==gy)Page1Hint=NULL;
						}else
						if(!strcmp(gx,"PAGE2HINT")){
							z=Gscanf(f1,"%s",gy);
							Page2Hint=GetTextByID(gy);
							if(Page2Hint==gy)Page2Hint=NULL;
						}else
						if(!strcmp(gx,"PAGE3HINT")){
							z=Gscanf(f1,"%s",gy);
							Page3Hint=GetTextByID(gy);
							if(Page3Hint==gy)Page3Hint=NULL;
						}else
						if(!strcmp(gx,"PEASANTABSORBER")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)NEPar(name,Line,"PEASANTABSORBER",1);
							PeasantAbsorber=true;
							MaxInside=p1;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PEASANT")){
							Peasant=true;
		                    NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PROTECTION")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%s%d",gy,&p2);
								if(z!=2)IncPar(name,Line,gx);
								int zz2=GetWeaponType(gy);
								if(zz2==-1){
									sprintf(gz,"%s,Line %d :%s: Unknown weapon kind(see NRES.DAT):%s",name,Line,gx,gy);
									ErrM(gz);
								};
								Protection[zz2]=p2;//div(p2*255,100).quot;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PRICE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"COST");
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%s%d",gy,&p2);
								int r=GetResByName(gy);
								if(r>=0&&r<100){
									NeedRes[r]=p2;
								}else{
									sprintf(gz,"%s,line %d:%s: Unknown resource:%s",name,Line,gx,gy);
									ErrM(gz);
								};
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PRODUCER")){
							Producer=true;
	                        z=Gscanf(f1,"%d",&p1);
				            if(z!=1)IncPar(name,Line,gx);
						    ProdType=0;
					        for(int i=0;i<p1;i++){
				                z=Gscanf(f1,"%s",gy);
			                    if(z!=1)IncPar(name,Line,gx);
		                        if(!strcmp(gy,"WOOD" ))ProdType|=1;else
							    if(!strcmp(gy,"GOLD" ))ProdType|=2;else
								if(!strcmp(gy,"STONE"))ProdType|=4;else
							    if(!strcmp(gy,"FOOD" ))ProdType|=8;else
							    if(!strcmp(gy,"IRON" ))ProdType|=16;else
							    if(!strcmp(gy,"COAL" ))ProdType|=32;else{
								    sprintf(gz,"%s,Line %d : %s : Unknown resource type : %s.\n Must be one of WOOD,STONE,GOLD,FOOD,IRON,COAL.",name,Line,gx,gy);
								    ErrM(gz);
							    };
	                        };
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,gx);
							FreeAdd=p1;
							PeasantAdd=p2;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PRIEST")){
							Priest=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PEASANTCONVERTOR")){
							PeasantConvertor=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PORT")){
							z=Gscanf(f1,"%s%d%d",gy,&p1,&p2);
							if(z!=3)IncPar(name,Line,gx);
							BuiAnm=GetNewAnimationByName(gy);
							if(!BuiAnm){
								sprintf(gz,"%s,line %d : Unknown animation: %s",name,Line,gy);
								ErrM(gz);
							};
							BuiDist=p1;
							MaxPortDist=p2;
							Port=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PORTION")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"PORTION");
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%s%d",gy,&p2);
								if(z!=2)NEPar(name,Line,"PORTION",p1);
								byte ms=0;
#ifdef COSSACKS2
								ms=GetResID(gy);
#else
								if(!strcmp(gy,"WOOD")){
									ms=0;
								}else
								if(!strcmp(gy,"GOLD")){
									ms=1;
								}else
								if(!strcmp(gy,"STONE")){
									ms=2;
								}else
								if(!strcmp(gy,"FOOD")){
									ms=3;
								}else{
									InvPar(name,Line,"PORTION",gy);
								};
#endif
								MaxResPortion[ms]=p2;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PROMAX")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							for(int i=0;i<8;i++)PromaxPercent[i]=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"PROMAX2")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,gx);
							if(p1<8&&p1>=0)PromaxPercent[p1]=p2;
							NLine(f1);
							Line++;
						}
                        else
                        if (!strcmp(gx, "PROMAXREDUCTION")) {
                            z = Gscanf(f1, "%d", &p1);
                            if (z != 2)IncPar(name, Line, gx);
                            if (p1 <= 100 && p1 >= 0) BuildingShield=p1;
                            NLine(f1);
                            Line++;
                        }
                        else
						if(!strcmp(gx,"PROMAXCTG")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							PromaxCategory=p1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'Q':
						{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'R':
						if(!strcmp(gx,"RASTRATA_NA_VISTREL2")){
							z=Gscanf(f1,"%d%d%d",&p4,&p5,&p1);
							if(z!=3)IncPar(name,Line,gx);
							if(ShotRes)free(ShotRes);
							ShotRes=znew(word,p1*2);
							NShotRes=p1;
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%s%d",gy,&p2);
									if(z!=2)IncPar(name,Line,gx);
								p3=GetResByName(gy);
								if(p3>=0&&p3<100){
									ShotRes[i*2]=p3;
									ShotRes[i*2+1]=p2;
								}else{
									sprintf(gz,"%s,line %d :%s: Unknown resource:%s",name,Line,gx,gy);
									ErrM(gz);
								};
							};
							ResAttType=p4;
							ResAttType1=p5;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"RATE")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,"RATE");
							if(p1>NAttTypes){
								sprintf_s(gy,sizeof(gy),"%s,line %d : Attack type index in RATE must be less 4",name,Line);
								ErrM(gy);
							};
							Rate[p1]=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"RANGE")){
							z=Gscanf(f1,"%d%d%d%d%d%d",&p1,&p2,&p3,&p4,&p5,&p6);
							if(z!=6)IncPar(name,Line,gx);
							//AttRange=p1<<8;
							//MinAttRange=p2<<8;
							//MaxAttRange=p3<<8;
							//MaxNearRange=p4<<8;
							//AttType=p5;
							//VisRange=p6;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"RESSUBST")){
							ResSubst=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ROTATE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1){
								sprintf_s(gy,sizeof(gy),"%s, Line %d : Invalid %s value",name,Line,gx);
								ErrM(gy);
							};
							MinRotator=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"RAZBROS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							Razbros=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"RESCONSUMER")){
							z=Gscanf(f1,"%s%d",gy,&p2);
							if(z!=2)IncPar(name,Line,gx);
							p1=GetResID(gy);
							if(p1==-1){
								sprintf(gz,"%s , Line %d :%s: Unknown resource ID:%s",name,Line,gx,gy);
								ErrM(gz);
							};
							ResConsID=p1;
							ResConsumer=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"RASTRATA_NA_VISTREL")){
							z=Gscanf(f1,"%d%d",&p4,&p1);
							if(z!=2)IncPar(name,Line,gx);
							if(ShotRes)free(ShotRes);
							ShotRes=znew(word,p1*2);
							NShotRes=p1;
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%s%d",gy,&p2);
								if(z!=2)IncPar(name,Line,gx);
								p3=GetResByName(gy);
								if(p3>=0&&p3<100){
									ShotRes[i*2]=p3;
									ShotRes[i*2+1]=p2;
								}else{
									sprintf(gz,"%s,line %d :%s: Unknown resource:%s",name,Line,gx,gy);
									ErrM(gz);
								};
							};
							ResAttType=p4;
							ResAttType1=p4;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"REFLECT")){
							z=Gscanf(f1,"%s",gy);
							if(z!=1)IncPar(name,Line,gx);
							Reflection=LoadNewAnimationByName(gy);
							if(!Reflection){
								sprintf(gz,"%s,Line %d :%s: Unknown animation:%s",name,Line,gx,gy);
								ErrM(gz);
							};
						}else
						if(!strcmp(gx,"RESOURCEBASE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"RESOURCEBASE");
							word ms=0;
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%s",gy);
								if(z!=1)NEPar(name,Line,"RESOURCEBASE",p1);
								ms|=1<<GetResByName(gy);
								ResConcentrator=ms;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"RECTANGLE")){
							z=Gscanf(f1,"%d%d%d%d",&p1,&p2,&p3,&p4);
							if(z!=4)IncPar(name,Line,"RECTANGLE");
							RectDx=p1;
							RectDy=p2;
							RectLx=p3;
							RectLy=p4;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'S':
						if(!strcmp(gx,"SINGLESHOT")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							SingleShot|=1<<p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SHOWIDLE")){
							ShowIdlePeasants=1;
							ShowIdleMines=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SHOWIDLEPEASANTS")){
							ShowIdlePeasants=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SHOWIDLEMINES")){
							ShowIdleMines=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SHAMAN")){
							Shaman=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"STANDANYWHERE")){
							StandAnywhere=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"STOPDISTANCE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							StopDistance=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SKILLDAMAGEMASK")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							SkillDamageMask=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SKILLDAMAGEBONUS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							SkillDamageBonus=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SKILLDAMAGEFORMBONUS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							SkillDamageFormationBonus=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SKILLDAMAGEFORMBONUSSTEP")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							SkillDamageFormationBonusStep=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SPRITEOBJECT")){
							SpriteObject=true;
				            Building=true;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SETACTIVEPOINT")){
							z=Gscanf(f1,"%s%d",gx,&p1);
							if(z!=2)IncPar(name,Line,"SETACTIVEPOINT");
							NewAnimation* NANM=LoadNewAnimationByName(gx);
							if(NANM){
								for(int j=0;j<NANM->Rotations;j++){
									z=Gscanf(f1,"%d%d",&xx,&yy);
									if(z!=2){
										sprintf_s(gy,sizeof(gy),"%s,line %d : Not enough active points. Must be : %d",name,Line,NANM->Rotations);
										ErrM(gy);
									};
									NANM->ActivePtX[j]=xx;
									NANM->ActivePtY[j]=yy;
									NANM->ActiveFrame=p1;
								};
							}else{
								sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown animation type: %s",name,Line,gx);
								ErrM(gy);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SETACTIVEPOINT2")){
							z=Gscanf(f1,"%s%d",gx,&p1);
							if(z!=2)IncPar(name,Line,gx);
							NewAnimation* NANM=LoadNewAnimationByName(gx);
							if(NANM){
								int N=NANM->Rotations<<1;
								for(int j=0;j<N;j++){
									z=Gscanf(f1,"%d%d",&xx,&yy);
									if(z!=2){
										sprintf_s(gy,sizeof(gy),"%s,line %d : Not enough active points. Must be : %d",name,Line,N);
										ErrM(gy);
									};
									NANM->ActivePtX[j]=xx;
									NANM->ActivePtY[j]=yy;
									NANM->ActiveFrame=p1;
								};
								NANM->DoubleShot=1;
							}else{
								sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown animation type: %s",name,Line,gx);
								ErrM(gy);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SETACTIVEPOINT3")){
							z=Gscanf(f1,"%s%d",gx,&p1);
							if(z!=2)IncPar(name,Line,gx);
							NewAnimation* NANM=LoadNewAnimationByName(gx);
							if(NANM){
								int N=NANM->Rotations*3;
								for(int j=0;j<N;j++){
									z=Gscanf(f1,"%d%d",&xx,&yy);
									if(z!=2){
										sprintf_s(gy,sizeof(gy),"%s,line %d : Not enough active points. Must be : %d",name,Line,N);
										ErrM(gy);
									};
									NANM->ActivePtX[j]=xx;
									NANM->ActivePtY[j]=yy;
									NANM->ActiveFrame=p1;
								};
								NANM->DoubleShot=2;
							}else{
								sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown animation type: %s",name,Line,gx);
								ErrM(gy);
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SEARCH_ENEMY_RADIUS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							VisRange=p1<<4;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SEARCH_ENEMY_RADIUS_SHIFT")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							VisRange=(VisRange*100)/(100+p1);
							VisRangeShift=(VisRange*p1)/100;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SHIELD")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,"SHIELD");
							Shield=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SHOTDELAY")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							TempDelay=p1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"SELECT_SOUND")){
							z=Gscanf(f1,"%s",gy);
							SelectSoundID=SearchStr(SoundID,gy,NSounds);
							if(SelectSoundID==-1){
								sprintf(gz,"%s,line %d:%s: Unknown sound:%s",name,Line,gx,gy);
							};
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"STANDGROUND")){
							CanStandGr=1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"STRIKE_SOUND")){
							z=Gscanf(f1,"%s",gy);
							StrikeSoundID=SearchStr(SoundID,gy,NSounds);
							if(StrikeSoundID==-1){
								sprintf(gz,"%s,line %d:%s: Unknown sound:%s",name,Line,gx,gy);
							};
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"SLOWDEATH")){
							SlowDeath=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SOUND")){
							z=Gscanf(f1,"%d%s%s",&p1,gy,Fn);
							if(z!=3)IncPar(name,Line,gx);
							NewAnimation* NANI=LoadNewAnimationByName(gy);
							if(NANI){
								NANI->SoundID=SearchStr(SoundID,Fn,NSounds);
								if(NANI->SoundID==-1){
									sprintf(gz,"%s,line %d :%s: Unknown sound:%s",name,Line,gx,Fn);
									ErrM(gz);
								};
								NANI->HotFrame=p1;
							}else{
								sprintf(gz,"%s,line %d :%s: Unknown animation:%s",name,Line,gx,gy);
								ErrM(gz);
							};
						}else
						if(!strcmp(gx,"SHOWDELAY")){
							ShowDelay=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SPRITES")){
	                        z=Gscanf(f1,"%s%s",gy,gz);
		                    p1=COMPLEX.GetIndexByName(gy);
			                p2=COMPLEX.GetIndexByName(gz);
				            if(p1==-1){
					            sprintf(Fn,"%s, line %d : %s : Unknown Sprite Object : %s",name,Line,gx,gy);
						        ErrM(Fn);
							};
	                        if(p2==-1){
		                        sprintf(Fn,"%s, line %d : %s : Unknown Sprite Object : %s",name,Line,gx,gz);
			                    ErrM(Fn);
				            };
					        Sprite=p1;
						    SpriteVisual=p2;
							NLine(f1);
	                        Line++;
		                }else
						
						if(!strcmp(gx,"SHOTS")){
	                        z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
						    NShotPt=p1;
					        ShotPtX=znew(short,p1);
				            ShotPtY=znew(short,p1);
							ShotDir=0;
							ShotDir=0;
	                        for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
							    if(z!=2)IncPar(name,Line,gx);
						        ShotPtX[i]=p2;
					            ShotPtY[i]=p3;
				            };
							NLine(f1);
							Line++;
	                    }else
						if(!strcmp(gx,"STAGELOCKPOINTS")){
							z=Gscanf(f1,"%d%d",&p2,&p1);
							if(z!=2)IncPar(name,Line,gx);
	
							NSLockPt[p2]=p1;
							SLockX[p2]=znew(byte,p1);
							SLockY[p2]=znew(byte,p1);
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p4,&p3);
								if(z!=2)IncPar(name,Line,"STAGELOCKPOINTS");
								SLockX[p2][i]=p4;
								SLockY[p2][i]=p3;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"STAGE_FIRES")){
							int st=0;
							z=Gscanf(f1,"%d%d",&st,&p1);
							if(z!=2)IncPar(name,Line,gx);
							if(CompxCraft&&st<MaxAStages){
								if(CompxCraft->Stages[st].FireX[0])free(CompxCraft->Stages[st].FireX[0]);
								if(CompxCraft->Stages[st].FireY[0])free(CompxCraft->Stages[st].FireY[0]);
								CompxCraft->Stages[st].FireX[0]=znew(short,p1);
								CompxCraft->Stages[st].FireY[0]=znew(short,p1);
								CompxCraft->Stages[st].NFires[0]=p1;
								for(int i=0;i<p1;i++){
									z=Gscanf(f1,"%d%d",&p2,&p3);
									if(z!=2)IncPar(name,Line,gx);
									CompxCraft->Stages[st].FireX[0][i]=p2;
									CompxCraft->Stages[st].FireY[0][i]=p3;
								};
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SMOKE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							FireX[1]=znew(short,p1);
							FireY[1]=znew(short,p1);
							NFires[1]=p1;
							for(int i=0;i<p1;i++){
								z=Gscanf(f1,"%d%d",&p2,&p3);
								if(z!=2)IncPar(name,Line,gx);
								FireX[1][i]=p2;
								FireY[1][i]=p3;
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"STAGE_SMOKE")){
							int st=0;
							z=Gscanf(f1,"%d%d",&st,&p1);
							if(z!=2)IncPar(name,Line,gx);
							if(CompxCraft&&st<MaxAStages){
								if(CompxCraft->Stages[st].FireX[1])free(CompxCraft->Stages[st].FireX[1]);
								if(CompxCraft->Stages[st].FireY[1])free(CompxCraft->Stages[st].FireY[1]);
								CompxCraft->Stages[st].FireX[1]=znew(short,p1);
								CompxCraft->Stages[st].FireY[1]=znew(short,p1);
								CompxCraft->Stages[st].NFires[1]=p1;
								for(int i=0;i<p1;i++){
									z=Gscanf(f1,"%d%d",&p2,&p3);
									if(z!=2)IncPar(name,Line,gx);
									CompxCraft->Stages[st].FireX[1][i]=p2;
									CompxCraft->Stages[st].FireY[1][i]=p3;
								};
							};
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SETANMPARAM")){
							z=Gscanf(f1,"%d%d%d%d",&p1,&p2,&p3,&p4);
							if(z!=4)IncPar(name,Line,"SETANMPARAM");
							NAStartDx=p1;
							NAStartDy=p2;
							NAParts=p3;
							NAPartSize=p4;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"SLOWRECHARGE")){
							SlowRecharge=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"STORMFORCE")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							StormForce=p1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'T':
						if(!strcmp(gx,"TIREDCHANGE")){
							z=Gscanf(f1,"%s%d",gy,&p1);
#ifdef GETTIRED
							if(z!=2)NEPar(name,Line,gx,1);
							NewAnimation* NA=LoadNewAnimationByName(gy);
							if(NA)NA->TiringChange=p1;
							else{
								sprintf(gz,"%s,Line %d :%s: Unknown animation:%s",name,Line,gx,gy);
								ErrM(gz);
							};
#endif //GETTIRED
						}else
						if(!strcmp(gx,"TRANSFORMTOMEAT")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)NEPar(name,Line,gx,1);
							MeatTransformIndex=p1;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"TRANSPORT")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)NEPar(name,Line,gx,1);
							Transport=true;
							MaxInside=p1;
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"TORG")){
							Rinok=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"TURNOFF")){
							Behavior=1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"TWOPARTS")){
							z=Gscanf(f1,"%s%d",gy,&p1);
							if(z!=2)IncPar(name,Line,gx);
							UpperPart=LoadNewAnimationByName(gy);
							if(!UpperPart){
								sprintf(gz,"%s,Line %d :%s: Unknown animation:%s",name,Line,gx,gy);
								ErrM(gz);
							};
							TwoParts=true;
							UpperPartShift=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"TAKERESSTAGES")){
							z=Gscanf(f1,"%s%d%d%d",gy,&p1,&p2,&p3);
							if(z==4){
								int r=GetResByName(gy);
								if(r>=0&&r<8&&p1<16&&p1>=0){
									if(!CompxUnit){
										CompxUnit=new ComplexUnitRecord;
										memset(CompxUnit,0,sizeof ComplexUnitRecord);
									};
									CompxUnit->GoWithStage[p1]=p2;
									CompxUnit->TakeResStage[p1]=p3;
									CompxUnit->TransformTo[p1]=r;
									CompxUnit->CanTakeExRes=1;
								}else IncPar(name,Line,gx);
							}else IncPar(name,Line,gx);
						}else
						if(!strcmp(gx,"TIME")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							ProduceStages=p1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'U':
						if(!strcmp(gx,"UNITABSORBER")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)NEPar(name,Line,"UNITABSORBER",1);
							UnitAbsorber=true;
							MaxInside=p1;
					        NLine(f1);
							Line++;
						}else
#ifdef _USE3D
						if (!strcmp( gx, "USEMDL" ))
						{
							z = Gscanf( f1,"%s", gy );
							UpConv( gy );
							Monster3D = new NewMon3D();
							Monster3D->SetModelFile( gy );
							NLine(f1);
							Line++;
						}else
				
#endif // _USE3D
						if(!strcmp(gx,"USERLC")){
							z=Gscanf(f1,"%d%s%s%d%d",&p1,gy,gz,&p2,&p3);
							if(z!=5)IncPar(name,Line,"USERLC");
							if(p1>MaxRLC)MaxRLC=p1;
							UpConv(gy);
							//search for RLC file in global table
							int nr=GPS.PreLoadGPImage(gy,!strcmp(gz,"SHADOW"));
							RLCRef[p1]=nr;
							RLCdx[p1]=p2;
							RLCdy[p1]=p3;
							RLCParts[p1]=1;
							RLCPartSize[p1]=0;
							NLine(f1);
							Line++;
						}
#ifdef NEWMORALEPRIEST
                        else
                            if (!strcmp(gx, "UNITONLY")) {
                                UnitMoralist = 1;
                                NLine(f1);
                                Line++;
                            }
#endif
                        else
						if(!strcmp(gx,"USERLCEXT")){
							z=Gscanf(f1,"%d%d%d%s%s%d%d",&p1,&p4,&p5,gy,gz,&p2,&p3);
							if(z!=7)IncPar(name,Line,"USERLCEXT");
							if(p1>MaxRLC)MaxRLC=p1;
							UpConv(gy);
							//search for RLC file in global table
							int nr=GPS.PreLoadGPImage(gy,!strcmp(gz,"SHADOW"));
							RLCRef[p1]=nr;
							RLCdx[p1]=p2;
							RLCdy[p1]=p3;
								RLCParts[p1]=p4;
							RLCPartSize[p2]=p5;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"UNITKIND")){
							z=Gscanf(f1,"%s",gz);
							if(z!=1)IncPar(name,Line,gx);
							p1=GetUnitKind(gz);
							if(p1==-1){
								sprintf("%s, Line %d :%s: Unknown kind of unit(see [UNITKIND],NRES.DAT):%s",name,Line,gx,gz);
								ErrM(gy);
							};
							Kind=p1;
						}else
						if(!strcmp(gx,"UNITRADIUS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							UnitRadius=p1;
							Line++;
							NLine(f1);
						}else
						if(!strcmp(gx,"USAGE")){
							z=Gscanf(f1,"%s",&gy);
							if(z!=1)IncPar(name,Line,gx);
							if(!strcmp(gy,FarmIDS))Usage=FarmID;else
							if(!strcmp(gy,MelnicaIDS)){
								Usage=MelnicaID;
								Ves=100;
							}else
							if(!strcmp(gy,SkladIDS)){
								Usage=SkladID;
								Ves=30;
							}else
							if(!strcmp(gy,TowerIDS))Usage=TowerID;else
							if(!strcmp(gy,CenterIDS))Usage=CenterID;else
							if(!strcmp(gy,MineIDS))Usage=MineID;else
							if(!strcmp(gy,FieldIDS))Usage=FieldID;else
							if(!strcmp(gy,PeasantIDS))Usage=PeasantID;else
							if(!strcmp(gy,FastHorseIDS))Usage=FastHorseID;else
							if(!strcmp(gy,MortiraIDS)){
								Usage=MortiraID;
								Artilery=true;
								//AttBuild=true;
							}else
							if(!strcmp(gy,PushkaIDS)){
								Usage=PushkaID;
								Artilery=true;
								//AttBuild=true;
							}else
							if(!strcmp(gy,MultiCannonIDS)){
								Usage=MultiCannonID;
								Artilery=true;
								//AttBuild=true;
							}else
							if(!strcmp(gy,GrenaderIDS))Usage=GrenaderID;else
							if(!strcmp(gy,HardWallIDS))Usage=HardWallID;else
							if(!strcmp(gy,WeakWallIDS))Usage=WeakWallID;else
							if(!strcmp(gy,LinkorIDS))Usage=LinkorID;else
							if(!strcmp(gy,WeakIDS))Usage=WeakID;else
							if(!strcmp(gy,FisherIDS))Usage=FisherID;else
							if(!strcmp(gy,ArtDepoIDS))Usage=ArtDepoID;else
							if(!strcmp(gy,SupMortIDS)){
								Usage=SupMortID;
								Artilery=true;
								//AttBuild=true;
							}else
							if(!strcmp(gy,PortIDS))Usage=PortID;else
							if(!strcmp(gy,LightInfIDS))Usage=LightInfID;else
							if(!strcmp(gy,StrelokIDS))Usage=StrelokID;else
							if(!strcmp(gy,HardHorceIDS))Usage=HardHorceID;else
							if(!strcmp(gy,HorseStrelokIDS))Usage=HorseStrelokID;else
							if(!strcmp(gy,FregatIDS))Usage=FregatID;else
							if(!strcmp(gy,GaleraIDS))Usage=GaleraID;else
							if(!strcmp(gy,IaxtaIDS))Usage=IaxtaID;else
							if(!strcmp(gy,ShebekaIDS))Usage=ShebekaID;else
							if(!strcmp(gy,ParomIDS))Usage=ParomID;else 
							if(!strcmp(gy,ArcherIDS))Usage=ArcherID;else
							if(!strcmp(gy,DiplomatIDS))Usage=DiplomatID;
							else IncPar(name,Line,gx);
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"USETRANSX")){
							z=Gscanf(f1,"%d",&p2);
							if(z!=1)IncPar(name,Line,gx);
							TransXMask|=1<<p2;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'V':
						if(!strcmp(gx,"VES")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							Ves=p1;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"VESACCEL")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,gx);
							VesStart=p1;
							VesFin=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"VISION")){
							z=Gscanf(f1,"%d",&p1);
							if(p1<0||p1>12){
								sprintf_s(gy,sizeof(gy),"%s, Line %d :%s: Argument must be 0..8",name,Line,gx);
								ErrM(gy);
							};
							if(z!=1)IncPar(name,Line,gx);
							VisionType=p1;
							NLine(f1);
							Line++;
						}else
                        if(!strcmp(gx,"VESLA")){
							z=Gscanf(f1,"%d%d%s%d",&p1,&p2,gy,&p3);
							if(z!=4)IncPar(name,Line,gx);
							Veslo=LoadNewAnimationByName(gy);
							if(!Veslo){
								sprintf(gz,"%s,Line %d :%s: Unknown animation:%s",name,Line,gx,gy);
								ErrM(gz);
							};
							VesloRotX=p1;
							VesloRotY=p2;
							VesloX=znew(short,p3);
							VesloY=znew(short,p3);
							NVesel=p3;
							int xx,yy;
							for(int i=0;i<p3;i++){
								z=Gscanf(f1,"%d%d",&xx,&yy);
								if(z!=2)IncPar(name,Line,gx);
								VesloX[i]=xx-p1;
								VesloY[i]=(yy-p2)<<1;
							};
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'W':
						if(!strcmp(gx,"WEAPONINSECTOR")){
							WSECT=(WeaponInSector*)malloc(sizeof WeaponInSector);
							z=Gscanf(f1,"%d%d%d%d%d%d%d",&WSECT->AttIndex,&WSECT->RMin,&WSECT->RMax,&WSECT->MaxDamage,&WSECT->MinDamage,&WSECT->Angle,&WSECT->AnglFactor);
							if(z!=7)IncPar(name,Line,gx);
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"WATERCHECKDIST")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,gx);
							WaterCheckDist1=p1;
							WaterCheckDist2=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"WALLOBJECT")){
							Wall=true;
						    SpriteObject=true;
					        Building=true;
				            z=Gscanf(f1,"%s",gy);
			                if(z!=1)IncPar(name,Line,gx);
		                    p1=GetWallType(gy);
	                        if(p1==-1){
								sprintf(gz,"%s, %d :%s: Unknown wall type: %s",name,Line,gx,gy);
							    ErrM(gz);
						    }else{
					            Sprite=p1;
				            };
			                NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"WEAPON")){
							z=Gscanf(f1,"%d%s",&p1,gz);
							if(z!=2)IncPar(name,Line,"WEAPON");
							if(p1>=NAttTypes){
								sprintf_s(gy,sizeof(gy),"%s,line %d :WEAPON: Index too big : %d",name,Line,p1);
								ErrM(gy);
							};
							p2=GetWeaponIndex(gz);
							if(p2==-1){
								sprintf_s(gy,sizeof(gy),"%s,line %d :WEAPON: Unknown weapon ID: %s",name,Line,gz);
								ErrM(gy);
							};
							DamWeap[p1]=WPLIST[p2];
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"WAVES")){
							z=Gscanf(f1,"%d%d%d%d",&p5,&p6,&p1,&p2);
							if(z!=4)IncPar(name,Line,gx);
							WaveDZ=p1;
							NWaves=p2;
							WavePoints=znew(short,p2*2);
							for(int i=0;i<p2;i++){
								z=Gscanf(f1,"%d%d",&p3,&p4);
								if(z!=2)IncPar(name,Line,gx);
								WavePoints[i+i]=p3-p5;
								WavePoints[i+i+1]=(p4-p6-p1)<<1;
							};
							NLine(f1);
							Line++;
						}else
							if(!strcmp(gx,"WATERROUND")){
							WaterActive=true;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"WEAPONKIND")){
							z=Gscanf(f1,"%d%s",&p1,gy);
							if(z!=2)IncPar(name,Line,gx);
							if(p1>=NAttTypes){
								sprintf(gz,"%s,Line %d :%s: Too big attack index",name,Line,gx);
								ErrM(gz);
							};
							int zz2=GetWeaponType(gy);
							if(zz2==-1){
								sprintf(gz,"%s,Line %d :%s: Unknown weapon kind(see NRES.DAT):%s",name,Line,gx,gy);
								ErrM(gz);
							};
							WeaponKind[p1]=zz2;
							if(WeaponFlags[zz2]&(8+16))CanFire=1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'X':
						{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'Y':
						{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					case 'Z':
						if(!strcmp(gx,"ZPOINTS")){
							z=Gscanf(f1,"%d%d",&p1,&p2);
							if(z!=2)IncPar(name,Line,"ZPOINTS");
							SrcZPoint=p1;
							DstZPoint=p2;
							NLine(f1);
							Line++;
						}else
						if(!strcmp(gx,"ZALP")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							if(p1>255)p1=255;
							MaxZalp=p1;
							NLine(f1);
							Line++;
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
						break;
					default:
						if(!strcmp(gx,"3DBARS")){
							z=Gscanf(f1,"%d",&p1);
							if(z!=1)IncPar(name,Line,gx);
							NBars=p1;
							Bars3D=znew(short,p1*5);
							int q=0;
							for(int i=0;i<p1;i++){
								int XB,YB,L1,L2,Hi;
								z=Gscanf(f1,"%d %d %d %d %d",&XB,&YB,&L1,&L2,&Hi);
								if(z!=5)IncPar(name,Line,gx);
								Bars3D[q  ]=XB;
								Bars3D[q+1]=YB;
								Bars3D[q+2]=L1;
								Bars3D[q+3]=L2;
								Bars3D[q+4]=Hi;
								q+=5;
							};
							Line++;
							NLine(f1);
						}else{
							sprintf_s(gy,sizeof(gy),"%s,line %d : Unknown command : %s",name,Line,gx);
							ErrM(gy);
						};
					};
				};
			}else{
				if(!strcmp(gx,"[END]"))finish=true;
				NLine(f1);
				Line++;
			};
		}while((!finish)&&z!=-1);
		Gclose(f1);
		return true;
	}else{
		sprintf_s(gx,sizeof(gx),"Could not open new monster discription file %s.nm",name);
		ErrM(gx);
		return false;
	};
};
extern bool ProtectionMode;
void ClearMonNames(){
	for(int i=0;i<NNewMon;i++)free(MonNames[i]);
};
void LoadAllNewMonsters(){
	ProtectionMode=1;
	GFILE* f1=Gopen("nmlist.txt","r");
	ProtectionMode=0;
	AFile("nmlist.txt");
	if(f1){
		char mname[128];
		int z;
		do{
			mname[0]=0;
			z=Gscanf(f1,"%s",mname);
			if(mname[0]){
				if(NewMon[NNewMon].CreateFromFile(mname)){
					NewMon[NNewMon].MyIndex=NNewMon;
					MonNames[NNewMon]=znew(char,strlen(NewMon[NNewMon].Name)+1);
					strcpy(MonNames[NNewMon],NewMon[NNewMon].Name);
					NNewMon++;
				};
			};
		}while(z!=-1&&mname[0]);
	}else{
		ErrM("Could not open NMLIST.TXT");
	};
};
bool CreateGOByName(GeneralObject* GO,char* name,char* newName){
	Visuals* VS=(Visuals*)GO;
	memset(GO,0,sizeof Visuals);
	for(int i=0;i<NNewMon;i++){
		if(!strcmp(MonNames[i],name)){
			NewMonster* NM=&NewMon[i];
			InitFlags(GO);
			//VS->info.Basic.MinDamage=NM->MinDamage;
			//VS->info.Basic.MaxDamage=NM->MaxDamage;
			VS->newMons=NM;
			//VS->capMagic=NM->Magic;
			//VS->info.Basic.MaxLife=NM->Life;
			//VS->info.Basic.MaxShield=NM->Shield;
			//VS->info.Basic.AttackRange=NM->AttRange;
			//VS->VisRadius=NM->VisRange;
			VS->newMons=NM;
			for(int j=1;j<4;j++){
				GO->ResourceID[j]=255;
				GO->ResAmount[j]=0;
			};
			GO->Message=NM->Message;
			GO->MonsterID=znew(char,strlen(newName)+1);
			strcpy(GO->MonsterID,newName);
			GO->NStages=byte(NM->Time);
			GO->cost=byte(NM->Res1cost);
			GO->Kind=byte(NM->Kind);
			GO->ResourceID[0]=1;
			GO->ResAmount[0]=NM->Res1cost;
			GO->ManualDisable=0;
			if(NM->Res2cost){
				GO->ResourceID[1]=2;
				GO->ResAmount[1]=NM->Res2cost;
			};
			if(NM->Res3cost){
				GO->ResourceID[2]=3;
				GO->ResAmount[2]=NM->Res3cost;
			};
			//GO->SpotSize=NM->VisRange;
			//GO->SpotType=2;
            GO->NStages=byte(NM->ProduceStages);
			GO->MoreCharacter=new AdvCharacter;
			GO->CondEnabled=false;
			GO->Enabled=false;
			GO->Branch=0xFF;
			CreateAdvCharacter(GO->MoreCharacter,GO->newMons);
			return true;
		};
	};
	char ddd[128];
	sprintf(ddd,"Could not create GeneralObject by name: %s",name);
	ErrM(ddd);
	return false;
};
//----------------Elements of the terrain: trees,stones,...-----------
int  SprGroup::GetIndexByName(char* Name){
    if(!strcmp(Name,"NONE"))return 0xFFFF;
	for(int i=0;i<NSpr;i++){
		if(ObjChar[i].Name&&!strcmp(ObjChar[i].Name,Name))return i;
	};
	return -1;
};
int GetResByName(char* gy){
	if(gy[0]=='#'&&gy[1]=='R'){
		gy+=2;
		int v;
		int z=sscanf(gy,"%d",&v);
		if(z==1){
			return v;
		};
	};
    if(!strcmp(gy,"WOOD"))return 0;
	else if(!strcmp(gy,"GOLD"))return 1;
	else if(!strcmp(gy,"STONE"))return 2;
	else if(!strcmp(gy,"FOOD"))return 3;
    else if(!strcmp(gy,"IRON"))return 4;
    else if(!strcmp(gy,"COAL"))return 5;
    else if(!strcmp(gy,"NONE"))return 0xFE;
    else if(!strcmp(gy,"REMOVE"))return 0xFF;
    else return -1;
};
int AddOneGroundType(SprGroup* GRP,int id);
#define xnew(s,t) (t*)malloc((s)*sizeof(t))
void SprGroup::LoadSprites(char* fname){
	char fn[128];
	sprintf(fn,"%s.lst",fname);
	GFILE* f1=Gopen(fn,"r");
	AFile(fn);
	char str[128];
	char gy[128];
	char gx[128];
	int z,nl,dx,dy,rr,z1,z2,parm1,i;
    int Line=1;
    int p4,p5;
	if(f1){
		z=Gscanf(f1,"%s%d",gy,&nl);
		if(z!=2)IncPar(fname,0,"?");
		NSpr=nl;
		Items=xnew(nl,word);
		for( i=0;i<nl;i++)Items[i]=i;
		Dx=xnew(nl,short);
		Dy=xnew(nl,short);				
		Radius=xnew(nl,short);				
		ObjChar=xnew(nl,ObjCharacter);
		memset(ObjChar,0,nl*sizeof ObjCharacter);
		NLine(f1);
        Line++;
		UpConv(gy);
        if(!strcmp(gy,"COMPLEX")){
            //Loading complex
            FileID=0xFFFF;
            for(int i=0;i<nl;i++){
                z=Gscanf(f1,"%s",gx);
                if(gx[0]!=0&&gx[0]!='/'){
                    z=Gscanf(f1,"%d%d%d%s%d%d%d%d",&dx,&dy,&rr,str,&p4,&p5,&z1,&z2);
                    if(z!=8)IncPar(fname,Line,gy);
                    Dx[i]=dx;
	        		Dy[i]=dy;
	    	    	Radius[i]=rr;
		    	    ObjCharacter* OBC=&ObjChar[i];
    		    	OBC->Name=xnew(strlen(gx)+1,char);
					OBC->SpriteIndex=i;
	    	    	strcpy(OBC->Name,gx);
		        	OBC->ResType=0xFF;
					OBC->WorkRadius=32;
		        	OBC->WorkAmount=0;
					OBC->ResPerWork=0;
		    	    OBC->WNextObj=i;
    		    	OBC->DNextObj=i;
	    	    	OBC->DamageAmount=0;
                    OBC->IntResType=0xFF;
                    OBC->IntResPerWork=0;
                    OBC->IntWorkRadius=32;
                    OBC->TimeAmount=0;
                    OBC->TNextObj=i;
                    OBC->Parts=p4;
                    if(!p5)p5=1;
                    OBC->Delay=p5;
                    OBC->Z0=z1;
					OBC->OnGround=0xFFFF;
                    OBC->DZ=z2;
                    OBC->Stand=GetNewAnimationByName(str);
                    char ggg[128];
                    sprintf(ggg,"%sS",str);
                    OBC->Shadow=GetNewAnimationByName(ggg);
                    if(OBC->Stand){
                        OBC->Frames=div(OBC->Stand->NFrames,p4).quot;
                        if(!OBC->Frames){
                            sprintf_s(gx,sizeof(gx),"%s, Line %d : %s : Too many parts (more then sprites in animation) : %s",fname,Line,gy,str);
                            ErrM(gx);
                        };
                    }else{
                        sprintf_s(gx,sizeof(gx),"%s, Line %d : %s : Unknown Animation : %s",fname,Line,gy,str);
                        ErrM(gx);
                    };
                    NLine(f1);
                    Line++;
                }else{
                    NLine(f1);
                    Line++;
                    i--;
                };
            };  
        }else{
    		//search for RLC file in global table
    		int nr=GPS.PreLoadGPImage(gy,false);
			if(!strcmp(fname,"animations"))GPS.SetWhiteFont(nr);
    		FileID=nr;
			int spidx=0;
			int gpidx=0;
	    	for( i=0;i<nl;i++){
                z=Gscanf(f1,"%s",gx);
                if(gx[0]!=0&&gx[0]!='/'){
					if(gx[0]=='#'){
						int N1=0;
						int N2=0;
						int z=Gscanf(f1,"%d%d",&N1,&N2);
						if(z==2){
							char cc[64];
							z=Gscanf(f1,"%s",cc);
							if(z==1){
								z=Gscanf(f1,"%d%d%d",&dx,&dy,&rr);
								if(z==3){
									if(cc[0]=='@'){
										int nnew=NSpr+2*(N2-N1+1)-1;
										Items=(word*)realloc(Items,nnew*2);
										Items[NSpr]=i;
										Dx=(short*)realloc(Dx,nnew*2);
										Dy=(short*)realloc(Dy,nnew*2);
										Radius=(short*)realloc(Radius,nnew*2);				
										ObjChar=(ObjCharacter*)realloc(ObjChar,nnew*sizeof ObjCharacter);
										memset(ObjChar+NSpr,0,(nnew-NSpr)*sizeof ObjCharacter);
										NSpr=nnew;
										for(int j=N1;j<=N2;j++){
											Dx[spidx]=dx;
		        							Dy[spidx]=dy;
											Radius[spidx]=rr;
											ObjCharacter* OBCF=&ObjChar[spidx];
											OBCF->SpriteIndex=gpidx;
											char cc3[64];
											sprintf(cc3,"%s%dF",cc,j);
		 		    						OBCF->Name=xnew(strlen(cc3)+1,char);
		    	    						strcpy(OBCF->Name,cc3);
			        						OBCF->ResType=0xFF;
			        						OBCF->WorkAmount=0;
											OBCF->WorkRadius=32;
											OBCF->ResPerWork=0;
			    	    					OBCF->WNextObj=i;
				    						OBCF->DNextObj=i;
		 		    						OBCF->DamageAmount=0;
			            					OBCF->IntResType=0xFF;
			            					OBCF->IntResPerWork=0;
				        					OBCF->IntWorkRadius=32;
				        					OBCF->TimeAmount=0;
				        					OBCF->TNextObj=i;
											OBCF->Parts=1;
											OBCF->Z0=0;
											OBCF->DZ=0;
											OBCF->Frames=1;
											OBCF->Stand=NULL;
											OBCF->Discret=1;
											OBCF->OnGround=0xFFFF;
											spidx++;
											gpidx++;
										};
										gpidx-=N2-N1+1;
										for(int j=N1;j<=N2;j++){
											Dx[spidx]=dx;
		        							Dy[spidx]=dy;
											Radius[spidx]=rr;
											ObjCharacter* OBCF=&ObjChar[spidx];
											OBCF->SpriteIndex=gpidx+4096;
											char cc3[64];
											sprintf(cc3,"%s%dB",cc,j);
		 		    						OBCF->Name=xnew(strlen(cc3)+1,char);
		    	    						strcpy(OBCF->Name,cc3);
			        						OBCF->ResType=0xFF;
			        						OBCF->WorkAmount=0;
											OBCF->WorkRadius=32;
											OBCF->ResPerWork=0;
			    	    					OBCF->WNextObj=i;
				    						OBCF->DNextObj=i;
		 		    						OBCF->DamageAmount=0;
			            					OBCF->IntResType=0xFF;
			            					OBCF->IntResPerWork=0;
				        					OBCF->IntWorkRadius=32;
				        					OBCF->TimeAmount=0;
				        					OBCF->TNextObj=i;
											OBCF->Parts=1;
											OBCF->Z0=0;
											OBCF->DZ=0;
											OBCF->Frames=1;
											OBCF->Stand=NULL;
											OBCF->Discret=1;
											OBCF->OnGround=0xFFFF;
											spidx++;
											gpidx++;
										};
									}else{
										int nnew=NSpr+N2-N1;
										Items=(word*)realloc(Items,nnew*2);
										Items[NSpr]=i;
										Dx=(short*)realloc(Dx,nnew*2);
										Dy=(short*)realloc(Dy,nnew*2);
										Radius=(short*)realloc(Radius,nnew*2);				
										ObjChar=(ObjCharacter*)realloc(ObjChar,nnew*sizeof ObjCharacter);
										memset(ObjChar+NSpr,0,(nnew-NSpr)*sizeof ObjCharacter);
										NSpr=nnew;
										for( i=N1;i<=N2;i++){
											Dx[spidx]=dx;
		        							Dy[spidx]=dy;
		    	    						Radius[spidx]=rr;
			    	    					ObjCharacter* OBC=&ObjChar[spidx];
											OBC->SpriteIndex=gpidx;
		 		    						OBC->Name=xnew(strlen(gx)+1,char);
		    	    						strcpy(OBC->Name,gx);
			        						OBC->ResType=0xFF;
			        						OBC->WorkAmount=0;
											OBC->WorkRadius=32;
											OBC->ResPerWork=0;
			    	    					OBC->WNextObj=i;
				    						OBC->DNextObj=i;
		 		    						OBC->DamageAmount=0;
			            					OBC->IntResType=0xFF;
			            					OBC->IntResPerWork=0;
				        					OBC->IntWorkRadius=32;
				        					OBC->TimeAmount=0;
				        					OBC->TNextObj=i;
                        					OBC->Parts=1;
											OBC->Z0=0;
											OBC->DZ=0;
											OBC->Frames=1;
											OBC->Stand=NULL;
											OBC->Discret=1;
											OBC->OnGround=0xFFFF;
											spidx++;
											gpidx++;
										};
									};
								};
							};	
						};  
						NLine(f1);
						Line++;
					}else
					if(gx[0]=='@'){

						Items=(word*)realloc(Items,(NSpr+1)*2);
						Items[NSpr]=i;
						Dx=(short*)realloc(Dx,(NSpr+1)*2);
						Dy=(short*)realloc(Dy,(NSpr+1)*2);
						Radius=(short*)realloc(Radius,(NSpr+1)*2);				
						ObjChar=(ObjCharacter*)realloc(ObjChar,(NSpr+1)*sizeof ObjCharacter);
						memset(ObjChar+NSpr,0,sizeof ObjCharacter);
						NSpr++;

						z=Gscanf(f1,"%d%d%d",&dx,&dy,&rr);
		 	    		if(z!=3)IncPar(fname,Line,"");
		        		Dx[spidx]=dx;
		        		Dy[spidx]=dy;
						Dx[spidx+1]=-dx;
		        		Dy[spidx+1]=dy;
		    	    	Radius[spidx]=rr;
						Radius[spidx+1]=rr;
			    	    ObjCharacter* OBCF=&ObjChar[spidx];
						ObjCharacter* OBCB=&ObjChar[spidx+1];
						OBCF->SpriteIndex=gpidx;
						OBCB->SpriteIndex=gpidx+4096;
		 		    	OBCF->Name=xnew(strlen(gx)+2,char);
						OBCB->Name=xnew(strlen(gx)+2,char);
		    	    	strcpy(OBCF->Name,gx);
						strcpy(OBCB->Name,gx);
						strcat(OBCF->Name,"F");
						strcat(OBCB->Name,"B");

			        	OBCF->ResType=0xFF;
						OBCB->ResType=0xFF;

			        	OBCF->WorkAmount=0;
						OBCB->WorkAmount=0;

						OBCF->WorkRadius=32;
						OBCB->WorkRadius=32;

						OBCF->ResPerWork=0;
						OBCB->ResPerWork=0;

			    	    OBCF->WNextObj=i;
						OBCB->WNextObj=i;

				    	OBCF->DNextObj=i;
						OBCB->DNextObj=i;

		 		    	OBCF->DamageAmount=0;
						OBCB->DamageAmount=0;

			            OBCF->IntResType=0xFF;
						OBCB->IntResType=0xFF;

			            OBCF->IntResPerWork=0;
						OBCB->IntResPerWork=0;

				        OBCF->IntWorkRadius=32;
						OBCB->IntWorkRadius=32;

				        OBCF->TimeAmount=0;
						OBCB->TimeAmount=0;

				        OBCF->TNextObj=i;
						OBCB->TNextObj=i;

						OBCF->Parts=1;
                        OBCB->Parts=1;

						OBCF->Z0=0;
						OBCB->Z0=0;

						OBCF->DZ=0;
						OBCB->DZ=0;

						OBCF->Frames=1;
						OBCB->Frames=1;

						OBCF->Stand=NULL;
						OBCB->Stand=NULL;

						OBCF->Discret=1;
						OBCB->Discret=1;

						OBCF->OnGround=0xFFFF;
						OBCB->OnGround=0xFFFF;

						NLine(f1);
						Line++;
						spidx+=2;
						gpidx++;
					}else{
		    		    z=Gscanf(f1,"%d%d%d",&dx,&dy,&rr);
		 	    		if(z!=3)IncPar(fname,Line,"");
		        		Dx[spidx]=dx;
		        		Dy[spidx]=dy;
		    	    	Radius[spidx]=rr;
			    	    ObjCharacter* OBC=&ObjChar[spidx];
						OBC->SpriteIndex=gpidx;
		 		    	OBC->Name=xnew(strlen(gx)+1,char);
		    	    	strcpy(OBC->Name,gx);
			        	OBC->ResType=0xFF;
			        	OBC->WorkAmount=0;
						OBC->WorkRadius=32;
						OBC->ResPerWork=0;
			    	    OBC->WNextObj=i;
				    	OBC->DNextObj=i;
		 		    	OBC->DamageAmount=0;
			            OBC->IntResType=0xFF;
			            OBC->IntResPerWork=0;
				        OBC->IntWorkRadius=32;
				        OBC->TimeAmount=0;
				        OBC->TNextObj=i;
                        OBC->Parts=1;
						OBC->Z0=0;
						OBC->DZ=0;
						OBC->Frames=1;
						OBC->Stand=NULL;
						OBC->Discret=1;
						OBC->OnGround=0xFFFF;
						NLine(f1);
						Line++;
						spidx++;
						gpidx++;
					};
                }else{
                    NLine(f1);
                    Line++;
                };
		    };
        };
    }else{
        sprintf(str,"Could not open sprites discription : %s",fn);
    	ErrM(str);
    };
    Gclose(f1);
	//loading RSR file
	sprintf(fn,"%s.rsr",fname);
	f1=Gopen(fn,"r");
	AFile(fn);
	if(f1){
		int mode=0;
		do{
			z=Gscanf(f1,"%s",gx);
			if(z!=1)IncPar(fn,0,"?");
			if(gx[0]=='['){
				if(!strcmp(gx,"[WORKTRANSFORM]"))mode=1;
				else
				if(!strcmp(gx,"[FIRETRANSFORM]"))mode=2;
				else
				if(!strcmp(gx,"[SOURCES]"))mode=3;
				else
                if(!strcmp(gx,"[TIMETRANSFORM]"))mode=4;
                else
                if(!strcmp(gx,"[INTERNAL_SOURCE]"))mode=5;
                else
				if(!strcmp(gx,"[DISCRET]"))mode=6;
                else
				if(!strcmp(gx,"[LOCK]"))mode=7;
                else
				if(!strcmp(gx,"[GROUND]"))mode=8;
                else
				//new!!!
				if(!strcmp(gx,"[HIDDEN]"))mode=9;
                else
				if(!strcmp(gx,"[AUTOBORN]"))mode=10;
                else
				if(!strcmp(gx,"[WATERDEPENDENT]"))mode=11;
                else
				if(!strcmp(gx,"[ANIMATE]"))mode=12;
                else
				if(!strcmp(gx,"[SECTIONS]"))mode=13;
                else
				if(!strcmp(gx,"[AUTOANIMATE]"))mode=14;
                else
				if(!strcmp(gx,"[RANDOM]"))mode=15;
                else
				if(!strcmp(gx,"[FIXEDPLACE]"))mode=16;
                else
				if(!strcmp(gx,"[SHOTSHIELD]"))mode=17;
                else
				//---end---
				if(!strcmp(gx,"[END]"))mode=255;
				else{
					sprintf_s(gy,sizeof(gy),"Unknown section in <%s.rsr> : %s",fname,gx);
					ErrM(gy);
				};
			}else{
				if(gx[0]=='/')NLine(f1);
				else{
					switch(mode){
					case 1://[WORKTRANSFORM]
					case 2://[FIRETRANSFORM]
                    case 4://[TIMETRANSFORM]
						z=Gscanf(f1,"%s%d",gy,&parm1);
						if(z!=2)IncPar(fn,0,"[WORK/FIRE/TIMETRANSFORM]");
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
						z2=GetIndexByName(gy);
						if(z2==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gy);
							ErrM(str);
						};	
						if(mode==1){
							ObjChar[z1].WNextObj=z2;
							ObjChar[z1].WorkAmount=parm1;
						};
						if(mode==2){
							ObjChar[z1].DNextObj=z2;
							ObjChar[z1].DamageAmount=parm1;
						};
                        if(mode==4){
                            ObjChar[z1].TNextObj=z2;
							ObjChar[z1].TimeAmount=parm1;
                        };
						break;
					case 3://[SOURCES]
						z=Gscanf(f1,"%s%d%d",gy,&parm1,&rr);
						if(z!=3)IncPar(fn,0,"[SOURCES]");
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
                        z2=GetResByName(gy);
                        if(z2==-1){
							sprintf(str,"%s : Unknown resource type :%s",fn,gy);
							ErrM(str);
						};
						ObjChar[z1].ResType=z2;
						ObjChar[z1].ResPerWork=parm1;
						ObjChar[z1].WorkRadius=rr;
						break;
                    case 5://[INTERNAL_SOURCE]
                        z=Gscanf(f1,"%s%d%d",gy,&parm1,&rr);
						if(z!=3)IncPar(fn,0,"[INTERNAL_SOURCE]");
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
                        z2=GetResByName(gy);
                        if(z2==-1){
							sprintf(str,"%s : Unknown resource type :%s",fn,gy);
							ErrM(str);
						};
						ObjChar[z1].IntResType=z2;
						ObjChar[z1].IntResPerWork=parm1;
						ObjChar[z1].IntWorkRadius=rr;
						break;
					case 6:
						z=Gscanf(f1,"%d",&parm1);
						if(z!=1)IncPar(fn,0,"[DISCRET]");
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
						ObjChar[z1].Discret=parm1;
						break;
					case 8://[GROUND]
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						}else ObjChar[z1].OnGround=AddOneGroundType(this,z1);
						break;
					case 15://[RANDOM]
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
						z=Gscanf(f1,"%d",&parm1);
						if(z!=1)IncPar(fn,0,"[RANDOM]");
						ObjChar[z1].NRandom=parm1+1;
						break;
					case 10://[AUTOBORN]
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
						z=Gscanf(f1,"%d",&parm1);
						if(z!=1)IncPar(fn,0,"[AUTOBORN]");
						ObjChar[z1].NBorn=parm1;
						ObjChar[z1].BREF=xnew(parm1,BornRef);
						for( i=0;i<parm1;i++){
							int dx,dy;
							z=Gscanf(f1,"%s%d%d",gy,&dx,&dy);
							if(z==3){
								int z2=GetIndexByName(gy);
								if(z2==-1){
									sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
									ErrM(str);
								}else{
									ObjChar[z1].BREF[i].BDx=dx;
									ObjChar[z1].BREF[i].BDy=dy;
									ObjChar[z1].BREF[i].BornID=z2;
								};
							}else IncPar(fn,0,"[AUTOBORN]");
						};
						break;
					case 12://[ANIMATE]
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
						z=Gscanf(f1,"%d",&parm1);
						if(z!=1)IncPar(fn,0,"[ANIMATE]");
						nl=z1;
						for( i=0;i<parm1;i++){
							z=Gscanf(f1,"%s",gy);
							if(z==1){
								int z2=GetIndexByName(gy);
								if(z2==-1){
									sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
									ErrM(str);
								}else{
									ObjChar[z1].TimeAmount=1;
									ObjChar[z1].TNextObj=z2;
									z1=z2;
									if(i==parm1-1){
										ObjChar[z1].TimeAmount=1;
										ObjChar[z1].TNextObj=nl;
									};
								};
							};
						};
						break;
					case 14://[AUTOANIMATE]
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
						z=Gscanf(f1,"%d",&parm1);
						if(z!=1)IncPar(fn,0,"[AUTOANIMATE]");
						nl=z1;
						for(int i=0;i<parm1;i++){
							ObjChar[nl+i].TimeAmount=1;
							ObjChar[nl+i].TNextObj=nl+((i+1)%parm1);
						};
						break;
					case 16://[FIXEDPLACE]
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
						{
							int dx,dy,ang,r;
							z=Gscanf(f1,"%d%d%d%d",&dx,&dy,&ang,&r);
							if(z!=4)IncPar(fn,0,"[FIXEDPLACE]");
							ObjChar[z1].FixDx=dx;
							ObjChar[z1].FixDy=dy;
							ObjChar[z1].FixDir=ang;
							ObjChar[z1].FixR=r;
						};
						break;
					case 17://[SHOTSHIELD]
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						};
						{
							int a,b;
							z=Gscanf(f1,"%d%d",&a,&b);
							if(z!=2)IncPar(fn,0,"[SHOTSHIELD]");
                            ObjChar[z1].ShieldRadius = a;
							ObjChar[z1].ShieldProbability=b;
						};
						break;
					case 13://[SECTIONS]
						{
							char* sname=GetTextByID(gx);
							SectNames=(char**)realloc(SectNames,(NSections+1)*4);
							SectContence=(int**)realloc(SectContence,(NSections+1)*4);
							NInSect=(int*)realloc(NInSect,(NSections+1)*4);
							int z=Gscanf(f1,"%d",&nl);
							if(z==1){
								SectNames[NSections]=xnew(strlen(sname)+1,char);
								SectContence[NSections]=xnew(nl,int);
								strcpy(SectNames[NSections],sname);
								NInSect[NSections]=nl;
								for(int i=0;i<nl;i++){
									z=Gscanf(f1,"%s",gy);
									if(z==1){
										int z2=GetIndexByName(gy);
										if(z2==-1){
											sprintf(str,"%s : Unknown sprite ID :%s",fn,gy);
											ErrM(str);
										}else SectContence[NSections][i]=z2;
									}else IncPar(sname,0,"[SECTIONS]");
								};
							}else IncPar(sname,0,"[SECTIONS]");
							NSections++;
						};
						break;
					case 7://[LOCK]
						z=Gscanf(f1,"%d",&parm1);
						if(z!=1)IncPar(fn,0,"[LOCK]");
						z1=GetIndexByName(gx);
						if(z1==-1){
							sprintf(str,"%s : Unknown sprite ID :%s",fn,gx);
							ErrM(str);
						}else{
							ObjChar[z1].NLockPt=parm1;
							ObjChar[z1].LockX=xnew(parm1,short);
							ObjChar[z1].LockY=xnew(parm1,short);
							for(int i=0;i<parm1;i++){
								int x,y;
								z=Gscanf(f1,"%d%d",&x,&y);
								if(z==2){
									ObjChar[z1].LockX[i]=x;
									ObjChar[z1].LockY[i]=y;
								}else{
									IncPar(fn,0,"[LOCK]");
								};
							};
						};
						break;
                    };
				};
			};
		}while(mode!=255);
		Gclose(f1);	
	};

};
//-----------------------------------------------------------------------//
//                                                                       //
//                       PHYSICS OF THE NEW MONSTER                      //
//           New monsters choose freedom from cells through cells!       //
//-----------------------------------------------------------------------//
byte* MCount;//amount of monsters in 4x4 cell
byte* TmpMC; //amount of monsters in 4x4 cell,
                          //this is used only for force calculating
//word* NMsList;//array of ID of new monsters
word* BLDList;
void ZMem(byte* pntr,int siz){
	__asm{
		push	edi
		cld
		xor		eax,eax
		mov		edi,pntr
		mov		ecx,siz
		shr		ecx,2
		or		ecx,ecx
		jz		www
//		cli
		rep		stosd
		//sti
www:	pop		edi
	};
};



//Creating list of monsters according to cells

word GetDir(int dx,int dy){
	int PhDir;
	if(dx!=0||dy!=0){
		int adx=abs(dx);
		int ady=abs(dy);
		if(adx>ady)PhDir=byte(TAtg[div(ady<<8,adx).quot]);else
				   PhDir=64-byte(TAtg[div(adx<<8,ady).quot]);
		if(dx<0)PhDir=128-PhDir;
		if(dy<0)PhDir=256-PhDir;
	}else PhDir=64+128;
	return (PhDir+1024)&255;
};
//calculating forces, that acts to every monster
void CalculateForces(){
	int nmon,FX,FY,FX1,FY1,ofs0,ofs1,ofs2,R1,R2,RE1,RE2;
	int ox,oy,nmon1,dr;
	OneObject* OB;
	OneObject* EN;
	int NNN=VAL_MAXCX*(VAL_MAXCX-1)-1;
	int NNN2=(1+VAL_MAXCX)*MAXINCELL;
	int NNN3=(VAL_MAXCX-3)*MAXINCELL;
	for(int i=VAL_MAXCX+1;i<NNN;i++){
		nmon=TmpMC[i];
		ofs2=i<<SHFCELL;
		while(nmon){
			nmon--;
			TmpMC[i]--;
			OB=Group[GetNMSL(ofs2+nmon)];
			if(OB){
				ox=OB->RealX;
				oy=OB->RealY;
				R1=OB->Radius1;
				R2=OB->Radius2;
				ofs0=i-1-VAL_MAXCX;
				ofs1=ofs2-NNN2;
				FX=0;FY=0;
				for(int ddy=0;ddy<3;ddy++){
					for(int ddx=0;ddx<3;ddx++){
						nmon1=TmpMC[ofs0];
						if(nmon1){
							for(int p=0;p<nmon1;p++){
								EN=Group[GetNMSL(ofs1+p)];
								if(EN){
									//CENRAL LOOP !!!
									dr=Norma(ox-EN->RealX,oy-EN->RealY);
									RE1=EN->Radius1;
									RE2=EN->Radius2;
									FX1=0;
									FY1=0;
									if(RE1+R1>=dr){
										//COLLISION!!!!!!!
										EN->Collision=true;
										OB->Collision=true;
										if(dr){
											//Force, that acts on OB
											FX1=div((ox-EN->RealX)<<8,dr).quot;
											FY1=div((oy-EN->RealY)<<8,dr).quot;
										}else{
											FX=256;
											FY=256;
										};
									}else{
										if(RE2+R2>=dr){
											//Inside interaction zone
											//Force, that acts on OB
											FX1=div((ox-EN->RealX)<<5,dr).quot;
											FY1=div((oy-EN->RealY)<<5,dr).quot;
										};
									};
									if(FX1||FY1){
										EN->ForceX-=FX1;
										EN->ForceY-=FY1;
										OB->ForceX+=FX1;
										OB->ForceY+=FY1;
									};
								};
							};
						};
						ofs0++;
						ofs1+=MAXINCELL;
					};
					ofs0+=VAL_MAXCX-3;
					ofs1+=NNN3;
				};
			};

		};
	};
};
//-----------------------VISUALISATION------------------------
//----VISUALISATION DATA----
/*
class ZBuffer{
	short UnitLogY[512];
	short UnitX[512];
	short UnitY[512];
	word  UFileID[512];
	word  USpriteID[512];
	word  NURef[512];
	OneObject* OBJS[512];
	word NVUnits;
	word FirstUN;
	void AddToZBuffer(short uy,short x,short y,OneObject* OB,word FileID,word Sprite);
	void ShowZBuffer();
	void InitZBuffer();
};
void ZBuffer::InitZBuffer(){
	NVUnits=0;
	FirstUN=0;
};
void ZBuffer::AddToZBuffer(short uy,short x,short y,OneObject* OB,word FileID,word Sprite){
	if(NVUnits>=512)return;
	if(!NVUnits){
		NURef[0]=0xFFFF;
		UnitLogY[0]=uy;
		UnitX[0]=x;
		UnitY[0]=y;
		UFileID[0]=FileID;
		USpriteID[0]=Sprite;
		OBJS[0]=OB;
		NVUnits=1;
	}else{
		word un=FirstUN;
		word un0=0xFFFF;
		word un1;
		do{
			un1=NURef[un];
			if(uy<UnitLogY[un]){
				if(un0!=0xFFFF)
					NURef[un0]=NVUnits;
				else FirstUN=NVUnits;
				NURef[NVUnits]=un;
				UnitLogY[NVUnits]=uy;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				OBJS[NVUnits]=OB;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
				NVUnits++;
				return;
			};
			if(un1==0xFFFF){
				NURef[un]=NVUnits;
				NURef[NVUnits]=0xFFFF;
				UnitLogY[NVUnits]=uy;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
				OBJS[NVUnits]=OB;
				NVUnits++;
				return;
			};
			un0=un;
			un=un1;
		}while(true);
	};
};
void ZBuffer::ShowZBuffer(){
	if(!NVUnits)return;
	int Lx1=smaplx<<Shifter;
	int Ly1=mul3(smaply)<<(Shifter-2);
	SetRLCWindow(smapx,smapy,Lx1,Ly1,SCRSizeX);
	word fu=FirstUN;
	word spr,FID;
	do{
		spr=USpriteID[fu];
		FID=UFileID[fu];
		//Shadows
		OneObject* OB=OBJS[fu];
		if(OB&&OB->Selected&&OB->NNUM==MyNation){
			if(MiniMode)DrawMiniMarker(OB);
			else DrawMarker(OB);
		};
		GPS.ShowGP(smapx+UnitX[fu],smapy+UnitY[fu],FID,spr,0);
		fu=NURef[fu];
	}while(fu!=0xFFFF);
};
ZBuffer UpperZBF;
ZBuffer MediumZ[32];
ZBuffer LowerZBF;
*/
short UnitLogY[4096];
short UnitLogX[4096];
short UnitX[4096];
short UnitY[4096];
word  UFileID[4096];
word  USpriteID[4096];
word  NURef[4096];
int   UParam1[4096];
int   UParam2[4096];
OneObject* OBJS[4096];
word NVUnits;
word FirstUN;
// - - - - -//-------\\- - - - - 
//----------\\Hashing//----------
word Hash16   [4096];
word Hash64   [1024];
word Hash256  [256];
void InitHash(){
	memset(Hash16,0,sizeof Hash16);
	memset(Hash64,0,sizeof Hash64);
	memset(Hash256,0,sizeof Hash256);
};

//----------//-------\\----------
void InitVisual(){
	NVUnits=0;
	FirstUN=0;
	InitHash();
};
/*
void AddToVisual(short uy,short x,short y,OneObject* OB,word FileID,word Sprite,int Param1,int Param2){
	int ux=x;
	if(!NVUnits){
		NURef[0]=0xFFFF;
		UnitLogY[0]=uy;
		UnitX[0]=x;
		UnitY[0]=y;
        UParam1[0]=Param1;
        UParam2[0]=Param2;
		UFileID[0]=FileID;
		USpriteID[0]=Sprite;
		OBJS[0]=OB;
		NVUnits=1;
		int uuy=(int(uy)+32768)>>4;
		Hash16[uuy]=0;
		Hash64[uuy>>2]=0;
		Hash256[uuy]>>4=0;
	}else{
		word un;
		int uuy=(int(uy)+32768)>>4;
		un=Hash16[uuy];
		if(un==0xFFFF){
			un=Hash64[uuy>>2];
			if(un==0xFFFF){
				un=Hash256[uuy>>4];
			};
		};
		if(un==0xFFFF)un=FirstUN;
		word un0=0xFFFF;
		word un1;
		do{
			un1=NURef[un];
			if(uy<UnitLogY[un]){
				if(un0!=0xFFFF)
					NURef[un0]=NVUnits;
				else FirstUN=NVUnits;
				NURef[NVUnits]=un;
				UnitLogY[NVUnits]=uy;
				UnitLogX[NVUnits]=ux;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				OBJS[NVUnits]=OB;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
                UParam1[NVUnits]=Param1;
                UParam2[NVUnits]=Param2;
				if(un==Hash16[uuy]){
					Hash16[uuy]=NVUnits;
					uuy>>=2;
					if(Hash64[uuy]==un){
						Hash64[uuy]=NVUnits;
						uuy>>=2;
						if(Hash256[uuy]==un){
							hash256[uuy]=NVUnits;
						};
					};
				};
				NVUnits++;
				return;
			};
			if(un1==0xFFFF){
				NURef[un]=NVUnits;
				NURef[NVUnits]=0xFFFF;
				UnitLogY[NVUnits]=uy;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
                UParam1[NVUnits]=Param1;
                UParam2[NVUnits]=Param2;
				OBJS[NVUnits]=OB;
				NVUnits++;
				return;
			};
			un0=un;
			un=un1;
		}while(true);
	};
};
*/
void AddToVisual(short uy,short x,short y,OneObject* OB,word FileID,word Sprite){
	//assert(RLCNSpr[FileID]>(Sprite&4095));
	if(GPS.GPNFrames(FileID)<=(Sprite&4095))return;
	if(!NVUnits){
		NURef[0]=0xFFFF;
		UnitLogY[0]=uy;
		UnitX[0]=x;
		UnitY[0]=y;
        UParam1[0]=0;
        UParam2[0]=0;
		UFileID[0]=FileID;
		USpriteID[0]=Sprite;
		OBJS[0]=OB;
		NVUnits=1;
	}else{
		word un=FirstUN;
		word un0=0xFFFF;
		word un1;
		do{
			un1=NURef[un];
			if(uy<UnitLogY[un]){
				if(un0!=0xFFFF)
					NURef[un0]=NVUnits;
				else FirstUN=NVUnits;
				NURef[NVUnits]=un;
				UnitLogY[NVUnits]=uy;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				OBJS[NVUnits]=OB;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
                UParam1[NVUnits]=0;
                UParam2[NVUnits]=0;
				NVUnits++;
				return;
			};
			if(un1==0xFFFF){
				NURef[un]=NVUnits;
				NURef[NVUnits]=0xFFFF;
				UnitLogY[NVUnits]=uy;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
                UParam1[NVUnits]=0;
                UParam2[NVUnits]=0;
				OBJS[NVUnits]=OB;
				NVUnits++;
				return;
			};
			un0=un;
			un=un1;
		}while(true);
	};
};
void AddToVisual(short uy,short x,short y,OneObject* OB,word FileID,word Sprite,int Param1,int Param2){
	if(!NVUnits){
		NURef[0]=0xFFFF;
		UnitLogY[0]=uy;
		UnitX[0]=x;
		UnitY[0]=y;
        UParam1[0]=Param1;
        UParam2[0]=Param2;
		UFileID[0]=FileID;
		USpriteID[0]=Sprite;
		OBJS[0]=OB;
		NVUnits=1;
	}else{
		word un=FirstUN;
		word un0=0xFFFF;
		word un1;
		do{
			un1=NURef[un];
			if(uy<UnitLogY[un]){
				if(un0!=0xFFFF)
					NURef[un0]=NVUnits;
				else FirstUN=NVUnits;
				NURef[NVUnits]=un;
				UnitLogY[NVUnits]=uy;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				OBJS[NVUnits]=OB;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
                UParam1[NVUnits]=Param1;
                UParam2[NVUnits]=Param2;
				NVUnits++;
				return;
			};
			if(un1==0xFFFF){
				NURef[un]=NVUnits;
				NURef[NVUnits]=0xFFFF;
				UnitLogY[NVUnits]=uy;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
                UParam1[NVUnits]=Param1;
                UParam2[NVUnits]=Param2;
				OBJS[NVUnits]=OB;
				NVUnits++;
				return;
			};
			un0=un;
			un=un1;
		}while(true);
	};
};
void ShowRLCItemTrans8(int x,int y,lpRLCTable lprt,int n);
void ShowVisual(){
	if(!NVUnits)return;
	int Lx1=smaplx<<Shifter;
	int Ly1=mul3(smaply)<<(Shifter-2);
	SetRLCWindow(smapx,smapy,Lx1,Ly1,SCRSizeX);
	word fu=FirstUN;
	word spr,FID;
	do{
		spr=USpriteID[fu];
		FID=UFileID[fu];
		//Shadows
		OneObject* OB=OBJS[fu];
		if(OB&&OB->Selected&GM(MyNation)){
			if(MiniMode)DrawMiniMarker(OB);
			else DrawMarker(OB);
		};
		GPS.ShowGP(smapx+UnitX[fu],smapy+UnitY[fu],FID,spr,0);
		fu=NURef[fu];
	}while(fu!=0xFFFF);
};
//Param1 values:
//bits 0..2-visibility options
//0-simple
//1-pulsing(Param2-pointer to pulsing data)
//2-transparent(Param2-pointer to transparency data)
//3-encoded with palette(Param2-pointer to palette)
//4-shading with mask(Param2-pointer to gradient data)
extern byte fog[8192+1024];
extern byte wfog[8192];
extern byte rfog[8192];
extern byte darkfog[40960];
extern byte yfog[8192];
extern byte trans8[65536];
void AddToVisual(short uy,short x,short y,OneObject* OB,word FileID,word Sprite,int Options){
    int CType=Options&0x0F;
    int CProp=Options&0xF0;
    int Param1=CType;
    int Param2=0;
    switch(CType){
    case AV_PULSING:
        switch(CProp){
        case AV_RED:
            Param2=int(yfog);
            break;
        case AV_WHITE:
            Param2=int(wfog);
            break;
        case AV_DARK:
            Param2=int(fog+1024);
            break;
        default:
            Param1=0;
        };
        break;
    case AV_TRANSPARENT:
        break;
    case AV_PALETTE:
        switch(CProp){
        case AV_RED:
            Param2=int(yfog+2048);
            break;
        case AV_WHITE:
            Param2=int(wfog+2048);
            break;
        case AV_DARK:
            Param2=int(fog+1024+2048);
            break;
        default:
            Param1=0;
        };
        break;
    case AV_GRADIENT:
        switch(CProp){
        case AV_RED:
            Param2=int(yfog);
            break;
        case AV_WHITE:
            Param2=int(wfog);
            break;
        case AV_DARK:
            Param2=int(fog+1024);
            break;
        default:
            Param1=0;
        };
        break;
    };
	if(!NVUnits){
		NURef[0]=0xFFFF;
		UnitLogY[0]=uy;
		UnitX[0]=x;
		UnitY[0]=y;
        UParam1[0]=Param1;
        UParam2[0]=Param2;
		UFileID[0]=FileID;
		USpriteID[0]=Sprite;
		OBJS[0]=OB;
		NVUnits=1;
	}else{
		word un=FirstUN;
		word un0=0xFFFF;
		word un1;
		do{
			un1=NURef[un];
			if(uy<UnitLogY[un]){
				if(un0!=0xFFFF)
					NURef[un0]=NVUnits;
				else FirstUN=NVUnits;
				NURef[NVUnits]=un;
				UnitLogY[NVUnits]=uy;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				OBJS[NVUnits]=OB;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
                UParam1[NVUnits]=Param1;
                UParam2[NVUnits]=Param2;
				NVUnits++;
				return;
			};
			if(un1==0xFFFF){
				NURef[un]=NVUnits;
				NURef[NVUnits]=0xFFFF;
				UnitLogY[NVUnits]=uy;
				UnitX[NVUnits]=x;
				UnitY[NVUnits]=y;
				UFileID[NVUnits]=FileID;
				USpriteID[NVUnits]=Sprite;
                UParam1[NVUnits]=Param1;
                UParam2[NVUnits]=Param2;
				OBJS[NVUnits]=OB;
				NVUnits++;
				return;
			};
			un0=un;
			un=un1;
		}while(true);
	};
};
void ShowVisualLess(int yend){
	if(!NVUnits)return;
	int Lx1=smaplx<<Shifter;
	int Ly1=mul3(smaply)<<(Shifter-2);
	SetRLCWindow(smapx,smapy,Lx1,Ly1,SCRSizeX);
	word fu=FirstUN;
	word spr,FID;
    bool LocTrans;
	int ylog;
	do{
		ylog=UnitLogY[fu];
		if(ylog<yend){
			NVUnits--;
			spr=USpriteID[fu];
			FID=UFileID[fu];
			//if(spr>=RLCNSpr[FID])spr=RLCNSpr[FID]-1;
            int par1=UParam1[fu];
            int par2=UParam2[fu];
            byte nat=0;
			//Shadows
            LocTrans=false;
			OneObject* OB=OBJS[fu];
            if(OB){
                nat=OB->NNUM;
                if(OB->Selected&GM(MyNation)){
				    if(MiniMode)DrawMiniMarker(OB);
				    else DrawMarker(OB);
                };
			};
            if(TransMode&&OB&&OB->NewBuilding)LocTrans=true;
            switch(par1){
            case AV_NORMAL:
				if(LocTrans)GPS.ShowGPTransparent(smapx+UnitX[fu],smapy+UnitY[fu],FID,spr,nat);
				else GPS.ShowGP(smapx+UnitX[fu],smapy+UnitY[fu],FID,spr,nat);
                break;
            case AV_PULSING:
                {
                    int NNN=5+int(8*(sin(double(GetTickCount())/100)+2));
					GPS.ShowGPPal(smapx+UnitX[fu],smapy+UnitY[fu],FID,spr,nat,(byte*)par2+(NNN<<8));
                };
                break;
            case AV_TRANSPARENT:
				GPS.ShowGPTransparent(smapx+UnitX[fu],smapy+UnitY[fu],FID,spr,nat);
                break;
            case AV_PALETTE:
				GPS.ShowGPPal(smapx+UnitX[fu],smapy+UnitY[fu],FID,spr,nat,(byte*)par2);
                break;
            case AV_GRADIENT:
				GPS.ShowGPGrad(smapx+UnitX[fu],smapy+UnitY[fu],FID,spr,nat,(byte*)par2);
                break;
            };
			fu=NURef[fu];
			FirstUN=fu;
		};
	}while(fu!=0xFFFF&&ylog<yend);
};
bool PInside(int x,int y,int x1,int y1,int xp,int yp);
extern int time1;
extern int time2;
extern int tmtmt;
#ifdef CONQUEST
#define mzoom(x) (LMode?x>>2:x)
#define SCDY 400
void QShowFiresEx(OneObject* OB,int x0,int y0,int FirIDX,NewAnimation** PreFires,NewAnimation** FiresAnm,int NFiresAnm,int FIndex){
	if(!NFiresAnm)return;
	//determining the current stage
	word ms=OB->StageState;
	int curst=0;
	NewMonster* NM=OB->newMons;
	int LF0=0;
	int LF1=OB->Ref.General->MoreCharacter->Life;
	int LF00=LF0;
	int LF10=LF1;
	for(int i=0;i<MaxAStages;i++){
		LF0=LF1;
		word ms1=(ms>>(i*3))&7;
		if(ms1==2){
			LF1+=NM->CompxCraft->Stages[i].AddPoints;
			curst=i+1;
			LF00=LF0;
			LF10=LF1;
		};
	};
	int NFir;
	if(curst)NFir=NM->CompxCraft->Stages[curst-1].NFires[FirIDX];
	else NFir=NM->NFires[FirIDX];

	int scdy=mzoom(mapy<<4);
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	if(NFir){
		int L2=((LF10-LF00)*3)>>2;
		int LM=(LF10*3+LF00)>>2;

		//L2+=L2>>1;
		if(OB->Life>=LM)return;

		int NOG=NFir*(LM-OB->Life)/L2;
		if(NOG>NFir)NOG=NFir;
		FireInfo* GFIN=OB->IFire;
		if(GFIN&&GFIN->BStage!=curst){
			GFIN->Erase();
			free(GFIN);
			OB->IFire=NULL;
			GFIN=NULL;
		};
		if(!GFIN){
			GFIN=new FireInfo;
			for(int k=0;k<2;k++)GFIN->Objs[k]=0;
			OB->IFire=GFIN;
			GFIN->BStage=curst;
		};
		FireObjectInfo* FIN=GFIN->Objs[FIndex];
		if(!FIN){
			FIN=new FireObjectInfo;
			GFIN->Objs[FIndex]=FIN;
			FIN->FireSprite=znew(byte,NFir);
			FIN->FireStage=znew(byte,NFir);
			FIN->NeedFires=NOG;
			FIN->RealFires=0;
			FIN->delay=1+(rand()&3);
			/*
			if(NOG>4){
				FIN->RealFires=NOG;
				for(int i=0;i<NOG;i++){
					NewAnimation* NANM=FiresAnm[div(i,NFiresAnm).rem];
					if(NANM->NFrames){
						FIN->FireStage[i]=2;
						FIN->FireSprite[i]=div(i*133+tmtmt,NANM->NFrames).rem;
						FIN->delay=8+(rand()&15);
					};
				};
			};
			*/
		}else{
			FIN->NeedFires=NOG;
		};
		if(FIN->NeedFires>FIN->RealFires){
			int J=FIN->RealFires;
			if(FIN->delay)FIN->delay--;
			else{
				FIN->RealFires++;
				NewAnimation* NANM=PreFires[J%NFiresAnm];
				if(NANM){
					FIN->FireStage[J]=1;
					FIN->FireSprite[J]=0;
				}else{
					FIN->FireStage[J]=2;
					FIN->FireSprite[J]=0;
				};
				FIN->delay=8+(rand()&15);
			};
			for(int p=0;p<J;p++)if(FIN->FireStage[p]==3){
				FIN->FireStage[p]=2;
				FIN->FireSprite[p]=0;
			};
		};
		if(FIN->NeedFires<FIN->RealFires){
			FIN->RealFires=FIN->NeedFires;
		};
		for(int i=0;i<FIN->RealFires;i++){
			int ANI=div(i,NFiresAnm).rem;
			byte sta=FIN->FireStage[i];
			NewAnimation* NANM=NULL;
			FIN->FireSprite[i]++;
			switch(sta){
			case 1:
				NANM=PreFires[ANI];
				if(FIN->FireSprite[i]==NANM->NFrames){
					FIN->FireSprite[i]=0;
					FIN->FireStage[i]=2;
					sta=2;
				};
				break;
			case 2:
				NANM=FiresAnm[ANI];
				if(FIN->FireSprite[i]==NANM->NFrames){
					FIN->FireSprite[i]=0;
				};
				break;
			};
			switch(sta){
			case 1:
				NANM=PreFires[ANI];
				break;
			case 2:
				NANM=FiresAnm[ANI];
				break;
			case 3:
				NANM=PostFires[0][ANI];
			};
			if(NANM&&NANM->NFrames){
				int csp=FIN->FireSprite[i];
				NewFrame* NF=&NANM->Frames[csp];
				int x=x0;
				int y=y0;
				if(curst==0){
					x+=mzoom(NM->FireX[FIndex][i]);
					y+=mzoom(NM->FireY[FIndex][i]);
				}else{
					x+=mzoom(NM->CompxCraft->Stages[curst-1].FireX[FIndex][i]);
					y+=mzoom(NM->CompxCraft->Stages[curst-1].FireY[FIndex][i]);
				};
				PlayAnimation(NANM,csp,x+(mapx<<5),y+(mapy<<4));
				AddPoint(x,mzoom(OB->RealY>>5)-scdy+SCDY+i,x+mzoom(NF->dx),y+mzoom(NF->dy),NULL,NF->FileID,NF->SpriteID,0,0);
			};
		};
	};
};
#endif
void QShowFires(OneObject* OB,int x0,int y0,int FirIDX,NewAnimation** PreFires,NewAnimation** FiresAnm,int NFiresAnm,int FIndex){
	if(!NFiresAnm)return;
#ifdef CONQUEST
	if(OB->StageState){
		QShowFiresEx(OB,x0,y0,FirIDX,PreFires,FiresAnm,NFiresAnm,FIndex);
		return;
	};
#endif
	int NFir=OB->newMons->NFires[FirIDX];
	NewMonster* NM=OB->newMons;
	int scdy=mzoom(mapy<<4);
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	if(NFir){
		int NOG;
		if(OB->InFire){
			int L2=ADC->Life;
			NOG=div(NFir*(L2-OB->Life),L2).quot;
			if(NOG<4)NOG=4;
			if(NOG>NFir)NOG=NFir;
		}else{
			int L2=(ADC->Life>>1);
			L2+=L2>>1;
			if(OB->Life>=L2||!L2)return;
			NOG=div(NFir*(L2-OB->Life),L2).quot;
		};
		FireInfo* GFIN=OB->IFire;
		if(!GFIN){
			GFIN=new FireInfo;
			for(int k=0;k<2;k++)GFIN->Objs[k]=0;
			OB->IFire=GFIN;
		};
		FireObjectInfo* FIN=GFIN->Objs[FIndex];
		if(!FIN){
			FIN=new FireObjectInfo;
			GFIN->Objs[FIndex]=FIN;
			FIN->FireSprite=znew(byte,NFir);
			FIN->FireStage=znew(byte,NFir);
			FIN->NeedFires=NOG;
			FIN->RealFires=0;
			FIN->delay=1+(rand()&3);
			/*
			if(NOG>4){
				FIN->RealFires=NOG;
				for(int i=0;i<NOG;i++){
					NewAnimation* NANM=FiresAnm[div(i,NFiresAnm).rem];
					if(NANM->NFrames){
						FIN->FireStage[i]=2;
						FIN->FireSprite[i]=div(i*133+tmtmt,NANM->NFrames).rem;
						FIN->delay=8+(rand()&15);
					};
				};
			};
			*/
		}else{
			FIN->NeedFires=NOG;
		};
		if(FIN->NeedFires>FIN->RealFires){
			int J=FIN->RealFires;
			if(FIN->delay)FIN->delay--;
			else{
				FIN->RealFires++;
				NewAnimation* NANM=PreFires[div(J,NFiresAnm).rem];
				if(NANM){
					FIN->FireStage[J]=1;
					FIN->FireSprite[J]=0;
				}else{
					FIN->FireStage[J]=2;
					FIN->FireSprite[J]=0;
				};
				FIN->delay=8+(rand()&15);
			};
			for(int p=0;p<J;p++)if(FIN->FireStage[p]==3){
				FIN->FireStage[p]=2;
				FIN->FireSprite[p]=0;
			};
		};
		if(FIN->NeedFires<FIN->RealFires){
			FIN->RealFires=FIN->NeedFires;
		};
		for(int i=0;i<FIN->RealFires;i++){
			int ANI=div(i,NFiresAnm).rem;
			byte sta=FIN->FireStage[i];
			NewAnimation* NANM=NULL;
			FIN->FireSprite[i]++;
			switch(sta){
			case 1:
				NANM=PreFires[ANI];
				if(FIN->FireSprite[i]==NANM->NFrames){
					FIN->FireSprite[i]=0;
					FIN->FireStage[i]=2;
					sta=2;
				};
				break;
			case 2:
				NANM=FiresAnm[ANI];
				if(FIN->FireSprite[i]==NANM->NFrames){
					FIN->FireSprite[i]=0;
				};
				break;
			};
			switch(sta){
			case 1:
				NANM=PreFires[ANI];
				break;
			case 2:
				NANM=FiresAnm[ANI];
				break;
			case 3:
				NANM=PostFires[0][ANI];
			};
			if(NANM&&NANM->NFrames){
				int csp=FIN->FireSprite[i];
				NewFrame* NF=&NANM->Frames[csp];
				int x=x0+mzoom(NM->FireX[FIndex][i]);
				int y=y0+mzoom(NM->FireY[FIndex][i]);
				PlayAnimation(NANM,csp,x+(mapx<<5),y+(mapy<<4));
				AddPoint(x,mzoom(OB->RealY>>5)-scdy+SCDY+i,x+mzoom(NF->dx),y+mzoom(NF->dy),NULL,NF->FileID,NF->SpriteID,0,0);
			};
		};
	};
};
void ShowFires(OneObject* OB,int x0,int y0){
	if(!OB)return;
	AdvCharacter* ACR=OB->Ref.General->MoreCharacter;
	if(OB->Stage==ACR->ProduceStages)
	for(int k=0;k<2;k++)QShowFires(OB,x0,y0,k,PreFires[k],FiresAnm[k],NFiresAnm[k],k);
};
void FireObjectInfo::Erase(){
	free(FireSprite);
	free(FireStage);
};
void FireInfo::Erase(){
	for(int k=0;k<2;k++)
		if(Objs[k]){
			Objs[k]->Erase();
			free(Objs[k]);
		};
};
void ShowProducedShip(OneObject* Port,int CX,int CY);
bool AddSprRefl(int xg,int yg,int xc,int yc,int GP_File,int Sprite,int,int);
void DrawReflect();
void AddPointEx(short XL,short YL,short x,short y,OneObject* OB,word FileID,word SpriteID,word FileIDex,word SpriteIDex,int Param1,int Param2);
void ShowNewMonsters(){

#ifdef _USE3D
	GPS.EnableZBuffer( true );
#endif // _USE3D

	if(LMode)return;
	//DrawReflect();
	time1=GetTickCount();
	//InitVisual();
	ClearZBuffer();
	int x0=mapx<<5;
	int y0=mul3(mapy)<<3;
	int Lx1=smaplx<<5;
	int Ly1=mul3(smaply)<<3;
	int xx,yy;
	int mpdy=mapy<<4;
	int dxx=mapx<<5;
	int dyy=mapy<<4;
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB){
			if(OB->NewMonst&&!OB->Hidden){
				xx=(OB->RealX>>4)-x0;
				yy=(mul3(OB->RealY)>>6)-y0;
				int zz=yy;
				if(OB->LockType==1){
					int tmm=tmtmt&63;
					if(tmm>31)zz++;
				};
				if(Mode3D)zz-=OB->RZ+int(OB->OverEarth);
				if(xx>-256&&zz>-300&&xx<Lx1+256&&zz<Ly1+128){
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

							if(OB->Sdoxlo)AddOptPoint(ZBF_LO,xx,yy-256,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,NF->SpriteID,AV_TRANSPARENT);
							else AddPoint(xx,yy,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,NF->SpriteID,0,0);

							if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
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
								AddOptPoint(ZBF_LO,xx,yy-256,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,cspr,AV_TRANSPARENT);
							}else{
								if(Downpart){
									AddLoPoint((xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,cspr,0,0);
									if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,cspr,NM->WaterCheckDist1,NM->WaterCheckDist2);
									if(UpAnm)AddPoint(xx,yy+NM->UpperPartShift,(xx+UNF->dx)>>(5-Shifter),((zz+UNF->dy)>>(5-Shifter)),OB,UNF->FileID,cspr,0,0);
								}else{
									if(NM->LinearLength){
										int LL=NM->LinearLength>>1;
										int dx=(LL*TCos[OB->RealDir])>>8;
										int dy=(LL*TSin[OB->RealDir])>>9;
										AddLine(xx-dx,yy-dy,xx+dx,yy+dy,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,cspr,0,0);
										if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,cspr,NM->WaterCheckDist1,NM->WaterCheckDist2);
									}else{
										AddPoint(xx,yy,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,cspr,0,0);
										if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,cspr,NM->WaterCheckDist1,NM->WaterCheckDist2);
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
										AddPoint(xx+dx1,yy+dy1+pt[i3+3],xx+dx1-56,yy+dy1-dz-31,OB,2,(tmtmt+19*i)%54,0,0);
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
							if(OB->Sdoxlo>100){
								if(dir<ocM)
									AddOptPoint(ZBF_LO,xx,yy-256,(xx-NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,AV_TRANSPARENT);
								else{
									dir=oc1-dir;
									AddOptPoint(ZBF_LO,xx,yy-256,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,AV_TRANSPARENT);
								};
							}else{
								if(dir<ocM){
									if(Downpart){
										AddLoPoint((xx-NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0,0);
										if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
										if(UpAnm)AddPoint(xx,yy+NM->UpperPartShift,(xx-UNF->dx)>>(5-Shifter),((zz+UNF->dy)>>(5-Shifter)),OB,UNF->FileID,oc2-dir+NAM->Rotations*UNF->SpriteID+4096,0,0);
										NewFrame* NF=NM->Fist.Frames+csp;
										if(NM->BReflection&&NM->Fist.Enabled&&!OB->Sdoxlo){
											int ROC=(NM->Fist.Rotations-1)*2;
											if(ROC==octs){
												AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,oc2-dir/*+NAM->Rotations*NF->SpriteID*/,NM->WaterCheckDist1,NM->WaterCheckDist2);
											}else{
												AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,(oc2-dir)*ROC/octs/*+NAM->Rotations*NF->SpriteID*/,NM->WaterCheckDist1,NM->WaterCheckDist2);
											};
										};
									}else{
										if(NM->LinearLength){
											int LL=NM->LinearLength>>1;
											int dx=(LL*TCos[OB->RealDir])>>8;
											int dy=(LL*TSin[OB->RealDir])>>9;
											AddLine(xx-dx,yy-dy,xx+dx,yy+dy,(xx-NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0,0);
											if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
										}else{
											if(NAM->DoubleAnm)AddPointEx(xx,yy,(xx-NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,NF[NAM->NFrames].FileID,oc2-dir+NAM->Rotations*NF[NAM->NFrames].SpriteID+4096,0,0);
											else AddPoint(xx,yy,(xx-NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,0,0);
											if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
										};
										NewFrame* NF=NM->Fist.Frames+csp;
										if(NM->BReflection&&NM->Fist.Enabled&&!OB->Sdoxlo){
											int ROC=(NM->Fist.Rotations-1)*2;
											if(ROC==octs){
												AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,oc2-dir/*+NAM->Rotations*NF->SpriteID*/,NM->WaterCheckDist1,NM->WaterCheckDist2);
											}else{
												AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,(oc2-dir)*ROC/octs/*+NAM->Rotations*NF->SpriteID*/,NM->WaterCheckDist1,NM->WaterCheckDist2);
											};
										};
									};
								}else{
									dir=oc1-dir;
									if(Downpart){
										AddLoPoint((xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0,0);
										if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
										if(UpAnm)AddPoint(xx,yy+NM->UpperPartShift,(xx+UNF->dx)>>(5-Shifter),((zz+UNF->dy)>>(5-Shifter)),OB,UNF->FileID,oc2-dir+NAM->Rotations*UNF->SpriteID,0,0);
										NewFrame* NF=NM->Fist.Frames+csp;
										if(NM->BReflection&&NM->Fist.Enabled&&!OB->Sdoxlo){
											int ROC=(NM->Fist.Rotations-1)*2;
											if(ROC==octs){
												AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,oc2-dir/*+NAM->Rotations*NF->SpriteID*/+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
											}else{
												AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,(oc2-dir)*ROC/octs/*+NAM->Rotations*NF->SpriteID*/+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
											};
										};
									}else{
										if(NM->LinearLength){
											int LL=NM->LinearLength>>1;
											int dx=(LL*TCos[OB->RealDir])>>8;
											int dy=(LL*TSin[OB->RealDir])>>9;
											AddLine(xx-dx,yy-dy,xx+dx,yy+dy,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0,0);
											if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
										}else{
											if(NAM->DoubleAnm)AddPointEx(xx,yy,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,NF[NAM->NFrames].FileID,oc2-dir+NAM->Rotations*NF[NAM->NFrames].SpriteID,0,0);
											else AddPoint(xx,yy,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,0,0);
											if(!OB->LockType)AddSprRefl(dxx+xx,dyy+yy,-NF->dx,-NF->dy,NF->FileID,oc2-dir+NAM->Rotations*NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
										};
										NewFrame* NF=NM->Fist.Frames+csp;
										if(NM->BReflection&&NM->Fist.Enabled&&!OB->Sdoxlo){
											int ROC=(NM->Fist.Rotations-1)*2;
											if(ROC==octs){
												AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,oc2-dir/*+NAM->Rotations*NF->SpriteID*/+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
											}else{
												AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,(oc2-dir)*ROC/octs/*+NAM->Rotations*NF->SpriteID*/+4096,NM->WaterCheckDist1,NM->WaterCheckDist2);
											};
										};
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
										AddPoint(xx+dx1,yy+dy1+pt[i3+3],xx+dx1-56,yy+dy1-dz-31,OB,2,(tmtmt+19*i)%54,0,0);
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
										byte Rdir=(((OB->/*GraphDir*/RealDir+64+Rsesize)&255)*Rocts)>>8;
										NewFrame* NF=&NAM->Frames[csp];
										if(Rdir<RocM)
											AddOptPoint(ZBF_LO,0,0,(xx-NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,Roc2-Rdir+NAM->Rotations*NF->SpriteID+4096,256+512);
										else{
											Rdir=Roc1-Rdir;
											AddOptPoint(ZBF_LO,0,0,(xx+NF->dx)>>(5-Shifter),((zz+NF->dy)>>(5-Shifter)),OB,NF->FileID,Roc2-Rdir+NAM->Rotations*NF->SpriteID,256+512);
										};
									};
								};
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
								//if(LDir<0)LDir++;
								//if(RDir<0)RDir++;
								//if(char(OB->RealDir)>0){
								//	cspL=VES->NFrames-cspL-1;
								//	cspR=VES->NFrames-cspR-1;
								//};
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
											AddPoint(xx+dx,yy+dy,(xx+dx+VNFL->dx),(zz+dy+VNFL->dy),NULL,VNFL->FileID,((LDir*ROTS)>>8)+VNFL->SpriteID*ROTS,0,0);
										}else{
											//AddPoint(xx+dx,yy+dy,(xx+dx-VNFL->dx),(zz+dy+VNFL->dy),NULL,VNFL->FileID,((LDir*ROTS)>>8)+VNFL->SpriteID*ROTS+4096,0,0);
										};
										if(RDir>=0){
											AddPoint(xx+dx1,yy+dy1,(xx+dx1-VNFR->dx),(zz+dy1+VNFL->dy),NULL,VNFL->FileID,((RDir*ROTS)>>8)+VNFR->SpriteID*ROTS+4096,0,0);
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
				xx=(OB->RealX>>4)-x0;
				yy=(mul3(OB->RealY)>>6)-y0;
				int zz=yy;
				if(Mode3D)zz-=OB->RZ;
				NewMonster* NM=OB->newMons;
				//assert(NM);
				if(NM){
					if(NM->BReflection&&NM->Fist.Enabled){
						AddSprRefl(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,-NM->PicDx,-NM->PicDy,NM->Fist.Frames[0].FileID,NM->Fist.Frames[0].SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
					};
					int xx0=xx+NM->PicDx;
					int yy0=zz+NM->PicDy;
					int xx1=xx0+NM->PicLx-1;
					int yy1=yy0+NM->PicLy-1;
					int CSP;
					if(NM->Reflection){
						NewFrame* NF=NM->Reflection->Frames;
						AddOptPoint(ZBF_LO,0,0,xx0,yy0,NULL,NF->FileID,NF->SpriteID,256+512);
					};
					if(NM->FLAGS&&OB->LoLayer==&NM->StandLo){
						int xre=(OB->RealX>>4)-x0;
						int yre=(OB->RealY>>5)-y0;
						int Yre=yre-OB->RZ;
						int NF=NM->FLAGS->N;
						short* PT=NM->FLAGS->Points;
						for(int i=0;i<NF;i++){
							int i3=i<<2;
							AddPoint(xre,yre,xx0+PT[i3+1]-56,yy0+PT[i3+2]-31,OB,2,(tmtmt+i*19)%54,0,0);
						};
					};
					if(NM->Port){
						int px=(OB->WallX<<4)-(mapx<<5);
						int py=(OB->WallY<<3)-(mapy<<4)-GetHeight(PortBuiX<<4,PortBuiY<<4);
						int LL=NM->BuiDist<<3;
						NewAnimation* BUANM=NM->BuiAnm;
						if(BUANM){
							NewFrame* NF1=&BUANM->Frames[div(tmtmt,BUANM->NFrames).rem];
							//NewFrame* NF2=&BUANM->Frames[div(tmtmt+373,BUANM->NFrames).rem];
							//NewFrame* NF3=&BUANM->Frames[div(tmtmt+977,BUANM->NFrames).rem];
							//NewFrame* NF4=&BUANM->Frames[div(tmtmt+1077,BUANM->NFrames).rem];
							AddOptPoint(ZBF_LO,px,py,px+NF1->dx,py+NF1->dy,NULL,NF1->FileID,NF1->SpriteID,AV_TRANSPARENT);
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
							int xs=(OB->RealX>>4)+ANM->StartDx-x0;
							int yy=(mul3(OB->RealY)>>6)-y0;
							int ys=yy+ANM->StartDy;
							ANM_DX=ANM->StartDx;
							ANM_DY=ANM->StartDy;
							if(Mode3D)ys-=OB->RZ;
							for(int i=0;i<ANM->Parts;i++){
								NewFrame* NF=&ANM->Frames[i+CSP];
								AddHiPoint(xs+NF->dx,ys+NF->dy,OB,NF->FileID,NF->SpriteID,0,0);
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
							int xre=(OB->RealX>>4)-x0;
							int yre=(mul3(OB->RealY)>>6)-y0;
							int xs=xre+ANM->StartDx;
							ANM_DX=ANM->StartDx;
							ANM_DY=ANM->StartDy;
							//int yy=(mul3(OB->RealY)>>6)-y0;
							int ys=yre+ANM->StartDy;
							if(Mode3D){
								ys-=OB->RZ;
								//yre-=OB->RZ;
							};
							for(int i=0;i<ANM->Parts;i++){
								NewFrame* NF=&ANM->Frames[i+CSP];
								AddPoint(xre+NF->dx,yre+NM->AnmUpShift+NF->dy,xs+NF->dx,ys+NF->dy,OB,NF->FileID,NF->SpriteID,0,0);
								xs+=ANM->PartSize;
							};
                            if(NOPAUSE)OB->NewCurSprite+=ANM->Parts;;
						};
						
						ANM=OB->LoLayer;
						if(ANM&&ANM->Enabled){
							ANM_DX=ANM->StartDx;
							ANM_DY=ANM->StartDy;
							if(ANM->LineInfo){
								short* LINF=ANM->LineInfo;
								int nf=ANM->NFrames;
								int pos=0;
								for(int i=0;i<ANM->NFrames;i++){
									int xs=(OB->RealX>>4)+ANM->StartDx-x0;
									int yy=(mul3(OB->RealY)>>6)-y0;
									int ys=yy+ANM->StartDy;
									int ysz=ys;
									if(Mode3D)ys-=OB->RZ;
									int x1=LINF[ pos ];
									int y1=LINF[pos+1];
									int x2=LINF[pos+2];
									int y2=LINF[pos+3];
									NewFrame* NF=&ANM->Frames[i];
									if(x1==-10000){
										AddLoPoint(xs+NF->dx,ys+NF->dy,OB,NF->FileID,NF->SpriteID,0,0);
									}else
									if(x1==10000){
										AddHiPoint(xs+NF->dx,ys+NF->dy,OB,NF->FileID,NF->SpriteID,0,0);
									}else{
										AddLine(xs+x1,ysz+y1,xs+x2,ysz+y2,xs+NF->dx,ys+NF->dy,OB,NF->FileID,NF->SpriteID,0,0);
									};
									pos+=4;
								};
							}else{
								//if(CSP>=ANM->NFrames){
								//	OB->NewCurSprite=0;
								if(!OB->NewState)CSP=0;
								else CSP=OB->NewCurSprite;
								//};
								int xs=(OB->RealX>>4)+ANM->StartDx-x0;
								int yy=(mul3(OB->RealY)>>6)-y0;
								int ys=yy+ANM->StartDy;
								int ysz=ys;
								if(Mode3D)ys-=OB->RZ;
								int np=ANM->Parts-1;
								for(int i=0;i<ANM->Parts;i++){
									NewFrame* NF=&ANM->Frames[i+CSP];
									//if(i!=np)AddToVisual(yy-2048,xs+NF->dx,ys+NF->dy,NULL,NF->FileID,NF->SpriteID);
									//else 
							        AddLoPoint(xs+NF->dx,ys+NF->dy,OB,NF->FileID,NF->SpriteID,0,0);
									if(!OB->LockType)AddSprRefl(dxx+xs,dyy+ys,-NF->dx,-NF->dy,NF->FileID,NF->SpriteID,NM->WaterCheckDist1,NM->WaterCheckDist2);
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
											int xs=(OB->RealX>>4)+ANM_DX-x0;
											int yy=(mul3(OB->RealY)>>6)-y0;
											int ys=yy+ANM_DY;
											int ysz=ys;
											if(Mode3D)ys-=OB->RZ;
											AddPoint(xs+OAS->SortX,ysz+OAS->SortY,xs,ys,OB,GPID,OAS->SpriteID,0,0);
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
	ShowTrianglesOwerUnits();
	PreShowSprites();
#ifdef COSSACKS2
	void DrawAllComplexObjects();
	DrawAllComplexObjects();
#endif
	time2=GetTickCount()-time1;

#ifdef _USE3D
	GPS.EnableZBuffer( false );
#endif // _USE3D

	//ShowVisual();
};
void RunLeftVeslo(OneObject* OB,bool State){
	if(State&&OB->LeftVeslo>=128)OB->LeftVeslo-=128;
	if((!State)&&OB->LeftVeslo<128)OB->LeftVeslo+=128;
};
void RunRightVeslo(OneObject* OB,bool State){
	if(State&&OB->RightVeslo>=128)OB->RightVeslo-=128;
	if((!State)&&OB->RightVeslo<128)OB->RightVeslo+=128;
};
#define MinRot 16
void OneObject::ProcessNewMotion(){
	if(NewMonst){
		NewMonster* NWM=newMons;
		int dst;
		int FdestX=0;
		int FdestY=0;
		bool Fdest=false;
		byte StepDir=1;//0-back,1-stay,2-forward
		if(DestX>0){
			dst=Norma(RealX-DestX,RealY-DestY);
			if(dst<=NWM->MotionDist)DestX=-100;	
		    else{
				FdestX=div((DestX-RealX)<<6,dst).quot;
				FdestY=div((DestY-RealY)<<6,dst).quot;
				Fdest=true;
			};
		};
		if(Collision){
			int Ux=TSin[RealDir];
			int Uy=TCos[RealDir];
			int Csign=ForceX*Ux+ForceY*Uy;
			if(Csign>0)//Collision pushes forward
				StepDir=2;
			else StepDir=0;
		}else{
			FdestX+=ForceX;
			FdestY+=ForceY;
			if(FdestX||FdestY){
				char ndrr=char(GetDir(FdestX,FdestY));
				char dir1=char(ndrr-RealDir);
				if(abs(dir1)>MinRot){
					if(dir1>0)RealDir+=MinRot;
					else RealDir-=MinRot;
				}else RealDir=ndrr;
				StepDir=2;
			};
		};
		//has previous animation finished?
		if(NewAnm){
			if(NewAnm->NFrames-FrmDec>NewCurSprite){
				//still not finished
				if((MotionDir==2&&!StepDir)||
					(StepDir==2&&!MotionDir))MotionDir=1;
				if(MotionDir==0){
					RealX-=NWM->OneStepDX[RealDir];
					RealY-=NWM->OneStepDY[RealDir];
				}else{
					if(MotionDir==2){
						RealX+=NWM->OneStepDX[RealDir];
						RealY+=NWM->OneStepDY[RealDir];
					};
				};
				NewCurSprite+=FrmDec;
				APPLY_TIRING(this);
				//rando();
			}else{
				//animation finished
				MotionDir=StepDir;
				if(MotionDir!=1){
					//in motion
					LeftLeg=!LeftLeg;
					if(LeftLeg)NewAnm=&NWM->MotionL;
					else NewAnm=&NWM->MotionR;
					if(MotionDir==0){
						RealX-=NWM->OneStepDX[RealDir];
						RealY-=NWM->OneStepDY[RealDir];
					}else{
						RealX+=NWM->OneStepDX[RealDir];
						RealY+=NWM->OneStepDY[RealDir];
					};
					NewCurSprite=0;
				}else{
					NewAnm=&NWM->Stand;
					NewCurSprite=0;
				};
			};
		};
		x=RealX>>9;
		y=RealY>>9;
		if(StandTime<8)RZ=GetUnitHeight(RealX>>4,RealY>>4);
	};
};
//---------------------NEW MOTION ALGORITM----------------------//
// Flags & Variables in OneObject
//  
//   bool CanMove:1
//   bool ImpMotion:1;
//   bool NeedForceCalc:1;
//   byte RotCntr;
//   int BestNX;
//	 int BestNY;
//   int ForceX;
//   int ForceY;
void RotateMon(OneObject* OB,char angle){
	//OB->NothingTime=0;
	OB->RealDir=OB->GraphDir;
	OB->RealDir+=angle;
	OB->GraphDir+=angle;
	OB->UnBlockUnit();
	if(OB->AlwaysLock)OB->WeakBlockUnit();
};
void SetMonstersInCells(){
	ZMem(MCount,VAL_MAXCX*VAL_MAXCX);
	ZMem(TmpMC,VAL_MAXCX*VAL_MAXCX);
	memset(BLDList,0xFF,VAL_MAXCX*VAL_MAXCX*2);
	CleanNMSL();
	OneObject* OB;
	int ofst,ofst1,k;
	for(int i=0;i<MAXOBJECT;i++){
		OB=Group[i];
		if(OB&&!OB->Sdoxlo){
			if(OB->NewMonst){
				ofst=(OB->RealX>>11)+((OB->RealY>>11)<<VAL_SHFCX)+VAL_MAXCX+1;
				ofst1=ofst<<SHFCELL;
				if(ofst<VAL_MAXCX*VAL_MAXCX){
					k=MCount[ofst];
					if(k<MAXINCELL){
						MCount[ofst]++;
						TmpMC[ofst]++;
						SetNMSL(ofst1+k,OB->Index);
					};
				};
			}else{
				if(OB->NewBuilding){
					ofst=(OB->RealX>>11)+((OB->RealY>>11)<<VAL_SHFCX)+VAL_MAXCX+1;
					BLDList[ofst]=OB->Index;
				};
			};
		};
	}; 
};
//checking position for monster MID in  (x,y)
bool CheckPosition(int x,int y,int R,word MID){
	int cx=(x>>11);
	int cy=(y>>11);
	int ofs0=(cx)+((cy)<<VAL_SHFCX);
	int ofs1=ofs0<<SHFCELL;
	for(int dy=0;dy<3;dy++){
		for(int dx=0;dx<3;dx++){
			int NMon=MCount[ofs0];
            if(ofs0<MAXCIOFS){
			    for(int z=0;z<NMon;z++){
			    	word MD=GetNMSL(z+ofs1);
			    	if(MD!=MID){
			    		OneObject* OB=Group[MD];
			    		//if(OB&&Norma(OB->BestNX-x,OB->BestNY-y)<R+OB->Radius2)return false;
			    		if(OB&&(!OB->Sdoxlo)&&Norma(OB->RealX-x,OB->RealY-y)<R+OB->Radius2)return false;
			    	};
			    };
            };
			ofs0++;
			ofs1+=MAXINCELL;
		};
		ofs0+=VAL_MAXCX-3;
		ofs1+=(VAL_MAXCX-3)<<SHFCELL;
	};
	return true;
};
bool CheckBar(int x,int y,int Lx,int Ly);
bool CheckTerra(int x,int y,int Lx,int Media){
	int xs=(x-((Lx)<<7))>>8;
	int ys=(y-((Lx)<<7))>>8;
	if(MFIELDS[Media].CheckBar(xs,ys,Lx,Lx))return false;
	else return true;
};
int FORCEX;
int FORCEY;
void PushMonsters(int x,int y,int mcx,int mcy,int R,word MID){
	//if(Group[MID]->InMotion){
		FORCEX=0;
		FORCEY=0;
		return;
	//};
	int cx=(x>>11);
	int cy=(y>>11);
	byte nms=0xFF;
	OneObject* OBJE=NULL;
	if(MID!=0xFFFF){
		OBJE=Group[MID];
		if(OBJE)nms=OBJE->NMask;
	};
    FORCEX=0;
    FORCEY=0;
    int DFX,DFY;
	int ofs0=cx+(cy<<VAL_SHFCX);
	int ofs1=ofs0<<SHFCELL;
    int rr,dst1;
	for(int dy=0;dy<3;dy++){
		for(int dx=0;dx<3;dx++){
			int NMon=MCount[ofs0];
			for(int z=0;z<NMon;z++){
				word MD=GetNMSL(z+ofs1);
				if(MD!=MID){
					OneObject* OB=Group[MD];
					if(OB){
						//int dst=Norma(OB->BestNX-x,OB->BestNY-y);
						int dst=Norma(OB->RealX-x,OB->RealY-y);
                        rr=R+OB->Radius2+32;
						if(dst<rr){
							//dst=Norma(OB->RealX-mcx,OB->RealY-mcy);
							if(dst<4)dst=4;
                            dst1=dst>>2;
                            dst<<=2;
                            dst-=rr;
                            if(dst<dst1)dst=dst1;
                            DFX=div((OB->RealX-mcx)<<6,dst).quot;
                            DFY=div((OB->RealY-mcy)<<6,dst).quot;
							OB->NextForceX+=DFX;
							OB->NextForceY+=DFY;
                            FORCEX-=DFX;
                            FORCEY-=DFY;
							if(OBJE&&(!(OB->NMask&nms||OBJE->Zombi)))OBJE->AttackObj(OB->Index,1);
						};
					};
				};
			};
			ofs0++;
			ofs1+=MAXINCELL;
		};
		ofs0+=VAL_MAXCX-3;
		ofs1+=(VAL_MAXCX-3)<<SHFCELL;
	};
};
void ChkAnm(OneObject* OB){
	//assert(OB->NewAnm->Enabled);
	//assert(OB->NewState<4);
};
void TryToStand(OneObject* OB,bool rest){
#ifdef COSSACKS2
	//no rest for busy formation!!!
	if(rest&&OB->BrigadeID!=0xFFFF){
		if(CITY[OB->NNUM].Brigs[OB->BrigadeID].BOrder)rest=0;
	};
#endif //COSSACKS2
	rando();
	OB->StandTime++;
	OB->InMotion=false;
	OB->NothingTime++;
	OB->RealDir=OB->GraphDir;
	OB->CheckState();
	NewMonster* NMN=OB->newMons;
	if(OB->NewState>NAttTypes)OB->NewState=0;
	if(OB->LocalNewState>NAttTypes)OB->LocalNewState=0;
	OB->UnBlockUnit();
	if(OB->AlwaysLock)OB->WeakBlockUnit();
	if(OB->NewState!=OB->LocalNewState){
		if(OB->LocalNewState!=0){
			int n1=OB->LocalNewState;
			int n2=OB->NewState;
			if(n2&&NMN->TransMask[n1-1]&(1<<(n2-1))){
				OB->LocalNewState=n2;
				return;
			}else{
				if(n1==1&&n2==2){
					if(NMN->Trans01.Enabled){
						OB->NewAnm=&NMN->Trans01;
						OB->NewCurSprite=0;
						OB->LocalNewState=2;
						OB->InMotion=false;
						OB->BackMotion=false;
						return;
					};
				}else
				if(n1==2&&n2==1){
					if(NMN->Trans10.Enabled){
						OB->NewAnm=&NMN->Trans10;
						OB->NewCurSprite=0;
						OB->LocalNewState=1;
						OB->InMotion=false;
						OB->BackMotion=false;
						return;
					};
				}else
				if(n2==4){
					if(n1&&NMN->TransXMask&(1<<(n1-1))){
						OB->NewAnm=&NMN->TransX3;
						OB->NewCurSprite=0;
						OB->LocalNewState=n2;
						OB->InMotion=false;
						OB->BackMotion=false;
						return;
					};
				}else
				if(n1==4){
					if(n2&&NMN->TransXMask&(1<<(n2-1))){
						OB->NewAnm=&NMN->Trans3X;
						OB->NewCurSprite=0;
						OB->LocalNewState=n2;
						OB->InMotion=false;
						OB->BackMotion=false;
						return;
					};
				};
			};
			//try to transform in state 0
			NewAnimation* UA=&NMN->UAttack[OB->LocalNewState-1];
			if(UA->Enabled){
				OB->NewAnm=UA;
				OB->NewCurSprite=0;
				OB->LocalNewState=0;
				OB->InMotion=false;
				OB->BackMotion=false;
				return;
			}else{
				OB->NewAnm=&NMN->Stand;
				OB->NewCurSprite=0;
				OB->LocalNewState=0;
				OB->InMotion=false;
				OB->BackMotion=false;
				return;
			};
		}else{
			//now we can transform unit to state NewState
			NewAnimation* PA=&NMN->PAttack[OB->NewState-1];
			if(PA->Enabled){
				OB->NewAnm=PA;
				OB->NewCurSprite=0;
				OB->LocalNewState=OB->NewState;
				OB->InMotion=false;
				OB->BackMotion=false;
				return;
			}else{
				NewAnimation* AT=&NMN->PStand[OB->NewState-1];
				if(AT->Enabled){
					OB->NewAnm=AT;
					OB->NewCurSprite=0;
					OB->LocalNewState=OB->NewState;
					OB->InMotion=false;
					OB->BackMotion=false;
					return;
				}else{
					OB->NewAnm=&NMN->Stand;
					OB->NewCurSprite=0;
					OB->LocalNewState=OB->NewState;
					OB->InMotion=false;
					OB->BackMotion=false;
					return;
				};
			};
		};
	};
	if(OB->NewState){
		NewAnimation* AT=&NMN->PStand[OB->NewState-1];
		if(AT->Enabled){
			OB->NewAnm=AT;
			OB->NewCurSprite=0;
			OB->InMotion=false;
			OB->BackMotion=false;
			return;
		}else{
			OB->NewAnm=&NMN->Stand;
			OB->NewCurSprite=0;
			OB->InMotion=false;
			OB->BackMotion=false;
			return;
		};
	}else{
		//if(rest)
		if(OB->newMons->MotionStyle==7||OB->NewAnm&&OB->NewAnm->NFrames-FrmDec<=OB->NewCurSprite){
			if(rest&&rando()<512){
				if(rando()<16384&&NMN->Rest.Enabled)OB->NewAnm=&NMN->Rest;
				else{
					if(NMN->Rest1.Enabled)OB->NewAnm=&NMN->Rest1;
					else OB->NewAnm=&NMN->Stand;
				};
			}else OB->NewAnm=&NMN->Stand;
			OB->NewCurSprite=0;
			OB->InMotion=false;
			OB->BackMotion=false;
		};
		return;
	};
};
void OneObject::CheckState(){
	if(RType<16){
		if(newMons->CompxUnit){
			//NewState=newMons->CompxUnit->GoWithStage[RType];
			GroundState=newMons->CompxUnit->GoWithStage[RType];
		}else{
			switch(RType){
			case 0://wood
				NewState=2;
				GroundState=2;
				break;
			case 1:
			case 2:
				NewState=3;
				GroundState=3;
				break;
			case 3:
				NewState=4;
				GroundState=4;
			};
		};
	};
};
int GetPrecHeight(int x,int y);
//attempt to move back or forward NewDir
//if(Dirc)attempt to move back
//returns:
//-1 : land is locked there
//1  : motion succesfull
//0  : imposible to move because of other monsters

int TryToMove(OneObject* OB,byte NewDir,bool Dirc){
	rando();
	if(OB->LocalNewState!=OB->NewState){
		TryToStand(OB,0);
		return 0; 
	};
	if(Dirc&&OB->BackDelay){
		TryToStand(OB,false);
		return 1;
	};
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	byte RDIR=NewDir;
	int retval=0;
	if(Dirc){
		RDIR+=128;
		OB->BackDelay=20;
	};
	byte Media=OB->LockType;
	OB->CheckState();
	int nf=NMN->MotionL.NFrames>>SpeedSh;
	int odx=NMN->OneStepDX[RDIR]<<SpeedSh;
	int ody=NMN->OneStepDY[RDIR]<<SpeedSh;
	
	if(OB->NewState){
		odx=(odx*ADC->Rate[OB->NewState-1])>>4;
		ody=(ody*ADC->Rate[OB->NewState-1])>>4;
		OB->Speed=ADC->Rate[OB->NewState-1];
	}else{
		//bdx=(bdx*ADC->Rate[0])>>4;
		//bdy=(bdy*ADC->Rate[0])>>4;
		OB->Speed=16;//ADC->Rate[0];
	};
	int bdx=nf*odx;
	int bdy=nf*ody;
	bool Unlim=OB->UnlimitedMotion;
	if(!(Unlim||CheckTerra(OB->RealX+bdx,OB->RealY+bdy,OB->Lx,Media)))return -1;
	if(Unlim||CheckPosition(OB->RealX+bdx,OB->RealY+bdy,OB->Radius2,OB->Index))retval=1;
	else{
		retval=0;
		//if(rando()>4096){
		//	TryToStand(OB,false);
		//	return 0;
		//}
	};
	NewAnimation* GO=NULL;
	if(OB->MaxAIndex){
		if(Dirc){
			if(OB->LeftLeg){
				GO=&NMN->PMotionLB[OB->CurAIndex];
			}else{
				GO=&NMN->PMotionRB[OB->CurAIndex];
			};
			if(OB->CurAIndex)OB->CurAIndex--;
			else{
				OB->CurAIndex=OB->MaxAIndex;
				OB->LeftLeg=!OB->LeftLeg;
			};
		}else{
			if(OB->LeftLeg){
				GO=&NMN->PMotionL[OB->CurAIndex];
			}else{
				GO=&NMN->PMotionR[OB->CurAIndex];
			};
			if(OB->CurAIndex<OB->MaxAIndex)OB->CurAIndex++;
			else{
				OB->CurAIndex=0;
				OB->LeftLeg=!OB->LeftLeg;
			};
		};
	};
	if(OB->NewState&&!GO){
		if(Dirc){
			//Back motion
			GO=&NMN->PMotionLB[OB->NewState-1];
				if(GO->Enabled){
				if(!OB->LeftLeg)GO=&NMN->PMotionRB[OB->NewState-1];
			}else GO=NULL;
		}else{
			GO=&NMN->PMotionL[OB->NewState-1];
			if(GO->Enabled){
				if(!OB->LeftLeg)GO=&NMN->PMotionR[OB->NewState-1];
			}else GO=NULL;
		};
	};
	if(OB->LocalNewState&&!GO){
		NewAnimation* NANM=&NMN->UAttack[OB->LocalNewState-1];
		if(NANM->Enabled){
			OB->NewAnm=NANM;
			OB->NewCurSprite=0;
			OB->InMotion=false;
			OB->NewState=OB->GroundState;
			return false;
		};
	};
	if(!GO){
		if(Dirc){
			if(OB->LeftLeg)GO=&NMN->MotionLB;
			else GO=&NMN->MotionRB;
		}else{
			if(OB->LeftLeg)GO=&NMN->MotionL;
			else GO=&NMN->MotionR;
		};
		if(!GO->Enabled)GO=&NMN->Stand;
	};
	//OB->Speed=16;
	if(retval==0){
		//bdx>>=1;
		//bdy>>=1;
		//OB->Speed>>=1;

	};
	OB->UnBlockUnit();
	OB->BestNX=OB->RealX+bdx;
	OB->BestNY=OB->RealY+bdy;
	int dx1=odx;
	int dy1=ody;
	//int bx1=(OB->RealX>>4)+dx1;
	//int by1=(OB->RealY>>4)+dy1;
	//int h1=GetHeight(bx1,by1);
	//int h0=OB->RZ;
	//int sdy=dy1+((h0-h1));
	//OB->GraphDir=GetDir(dx1,sdy);
	//if(Dirc)OB->GraphDir+=128;
	//int sdy=bdy-div((GetHeight(OB->BestNX>>4,OB->BestNY>>4)-GetHeight(OB->RealX>>4,OB->RealY>>4))<<(5+4),24).quot;//div((GetPrecHeight(OB->BestNX,OB->BestNY)-GetPrecHeight(OB->RealX,OB->RealY))<<5,24).quot;
	//OB->GraphDir=GetDir(bdx,sdy);
	OB->RealDir=NewDir;
	OB->GraphDir=NewDir;
	OB->NewAnm=GO;
	if(!OB->MaxAIndex)OB->LeftLeg=!OB->LeftLeg;
	OB->RealVx=dx1>>SpeedSh;
	OB->RealVy=dy1>>SpeedSh;
	//assert(abs(dx1)<16*256);
	OB->RealX+=dx1;//((NMN->OneStepDX[RDIR]*OB->Speed)>>4)<<SpeedSh;
	OB->RealY+=dy1;//((NMN->OneStepDY[RDIR]*OB->Speed)>>4)<<SpeedSh;
	OB->InMotion=true;
	OB->NewCurSprite=FrmDec-1;;
	OB->BackMotion=Dirc;
	OB->NothingTime=0;
	int N=OB->NewAnm->NFrames;
	OB->StandTime=0;
	return retval;
	//}else return 0;
};
int NewTryToMove(OneObject* OB,byte NewDir,bool Dirc,int dx,int dy){
	rando();
	if(Dirc&&OB->BackDelay){
		TryToStand(OB,false);
		return 1;
	};
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	byte RDIR=NewDir;
	int retval=0;
	if(Dirc){
		RDIR+=128;
		OB->BackDelay=20;
	};
	byte Media=OB->LockType;
	OB->CheckState();
	int nf=NMN->MotionL.NFrames>>SpeedSh;
	int bdx=nf*dx;
	int bdy=nf*dy;
	NewAnimation* GO=NULL;
	if(OB->MaxAIndex){
		if(Dirc){
			if(OB->LeftLeg){
				GO=&NMN->PMotionLB[OB->CurAIndex];
			}else{
				GO=&NMN->PMotionRB[OB->CurAIndex];
			};
			if(OB->CurAIndex)OB->CurAIndex--;
			else{
				OB->CurAIndex=OB->MaxAIndex;
				OB->LeftLeg=!OB->LeftLeg;
			};
		}else{
			if(OB->LeftLeg){
				GO=&NMN->PMotionL[OB->CurAIndex];
			}else{
				GO=&NMN->PMotionR[OB->CurAIndex];
			};
			if(OB->CurAIndex<OB->MaxAIndex)OB->CurAIndex++;
			else{
				OB->CurAIndex=0;
				OB->LeftLeg=!OB->LeftLeg;
			};
		};
	};
	if(OB->NewState&&!GO){
		if(Dirc){
			//Back motion
			GO=&NMN->PMotionLB[OB->NewState-1];
				if(GO->Enabled){
				if(!OB->LeftLeg)GO=&NMN->PMotionRB[OB->NewState-1];
			}else GO=NULL;
		}else{
			GO=&NMN->PMotionL[OB->NewState-1];
			if(GO->Enabled){
				if(!OB->LeftLeg)GO=&NMN->PMotionR[OB->NewState-1];
			}else GO=NULL;
		};
	};
	if(OB->LocalNewState&&!GO){
		NewAnimation* NANM=&NMN->UAttack[OB->LocalNewState-1];
		if(NANM->Enabled){
			OB->NewAnm=NANM;
			OB->NewCurSprite=0;
			OB->InMotion=false;
			OB->LocalNewState=0;
			return false;
		};
	};
	if(!GO){
		if(Dirc){
			if(OB->LeftLeg)GO=&NMN->MotionLB;
			else GO=&NMN->MotionRB;
		}else{
			if(OB->LeftLeg)GO=&NMN->MotionL;
			else GO=&NMN->MotionR;
		};
		if(!GO->Enabled)GO=&NMN->Stand;
	};
	OB->Speed=16;
	OB->UnBlockUnit();
	OB->RealVx=dx;
	OB->RealVy=dy;
	//assert(abs(dx)<16*256);
	OB->NewAnm=GO;
	if(!OB->MaxAIndex)OB->LeftLeg=!OB->LeftLeg;
	OB->RealX+=dx<<SpeedSh;
	OB->RealY+=dy<<SpeedSh;
	OB->InMotion=true;
	OB->NewCurSprite=FrmDec-1;
	OB->BackMotion=Dirc;
	OB->NothingTime=0;
	//int N=OB->NewAnm->NFrames;
	OB->StandTime=0;
	return retval;
	//}else return 0;
};
void DamageUnitsInAngle(int x,int y,WeaponInSector* WS,byte Angle,OneObject* Sender,int AttType);
bool Create3DAnmObject(Weapon* Weap,int xs,int ys,int zs,
					                int xd,int yd,int zd,
									OneObject* OB);
void CreateFullPath(int x1,int y1,OneObject* OB);
void AddDestn(int x,int y);
extern int tmtmt;
//extern int RESRC[8][8];
void CalculateMotionX0(){
	if(!(tmtmt&31)){
		for(int i=0;i<MAXOBJECT;i++){
			OneObject* OB=Group[i];
			if(OB&&OB->NZalp<OB->newMons->MaxZalp)OB->NZalp++;
		};
	};
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB){
			NewMonster* NM=OB->newMons;
			if(OB->delay){
				if(NM->SlowRecharge){
					if(!OB->LocalOrder){
						int cf=OB->NewCurSprite>>SpeedSh;
						int nf=(OB->NewAnm->NFrames-1)>>SpeedSh;
						if(nf==cf){
							if(OB->NewState!=4){
								OB->NewState=4;
								TryToStand(OB,0);
							}else{
								NewAnimation* ANM=&NM->Attack[3];
								if(OB->NewAnm!=ANM){
									OB->NewAnm=ANM;
									OB->NewCurSprite=0;
									OB->InMotion=0;
									OB->BackMotion=0;
								};
							};
						};
					};
				}else{
					OB->delay--;
				};
			};
			if(OB->NewBuilding&&!OB->Sdoxlo){
		        //if(OB->Weap)OB->SearchVictim();
				if(OB->DstX&&OB->ImSelected&&OB->NNUM==MyNation)AddDestn(OB->DstX>>4,OB->DstY>>4);
				if(OB->LocalOrder)OB->LocalOrder->DoLink(OB);
				int HLife=OB->MaxLife>>1;
				if(OB->Life<HLife){
                
					bool DoFog=false;
					HLife>>=1;
					int Prob=NM->Fogging.WProb;
					if(OB->Life<HLife){
						if(rando()<Prob)DoFog=true;
					}else{
						Prob>>=1;
						if(rando()<Prob)DoFog=true;
					};
					if(DoFog&&NM->Fogging.NWeap&&OB->Stage==OB->NStages){
						Prob=(rando()*NM->Fogging.NWeap)>>15;
						Weapon* WP=WPLIST[NM->Fogging.Weap[Prob]];
						Prob=(rando()*NM->NDamPt)>>15;
						int xx,yy;
						OB->GetCornerXY(&xx,&yy);
						if(NM->NDamPt){
							xx+=NM->DamPtX[Prob];
							yy+=NM->DamPtY[Prob];
						};
						xx<<=4;
						yy<<=4;
						Create3DAnmObject(WP,xx,yy,GetHeight(xx,yy)+4,xx,yy,-1000,NULL);
					};
				};
			}else
			if(OB->NewMonst/*&&OB->NewCurSprite==OB->NewAnm->NFrames-1*/){
				OB->ForceX=OB->NextForceX;
				OB->ForceY=OB->NextForceY;
				OB->NextForceX=0;
				OB->NextForceY=0;
			
			};
			if(OB->Sdoxlo&&!OB->Hidden){
				//death processing
				NewMonster* NMN=OB->newMons;
				if(NMN->Death.Enabled){
					if(OB->NewAnm==&NMN->Death){
						OB->NewCurSprite+=FrmDec;
						if(OB->NewCurSprite>=NMN->Death.NFrames-FrmDec){
							if(NMN->DeathLie1.Enabled){
								OB->NewAnm=&NMN->DeathLie1;
								OB->NewCurSprite=0;
							}else{
								OB->DeletePath();
								OB->ClearOrders();
								DelObject(OB);
								Group[OB->Index]=NULL;
								OB=NULL;//died.	
							};
						};
					}else{
						OB->Sdoxlo++;
#ifdef DEADTIMER
                        if (OB->Sdoxlo > 80000) {
#else
                        if (OB->Sdoxlo > 200) {
#endif
							if(NMN->DeathLie2.Enabled){
								OB->NewAnm=&NMN->DeathLie2;
								OB->NewCurSprite=0;
							}else{
								OB->DeletePath();
								OB->ClearOrders();
								DelObject(OB);
								Group[OB->Index]=NULL;
								OB=NULL;//died.	
							};
						};
#ifdef DEADTIMER
                        if (OB && OB->Sdoxlo > 160000) {
#else
                        if (OB && OB->Sdoxlo > 400) {
#endif
							if(NMN->DeathLie2.Enabled){
								OB->NewAnm=&NMN->DeathLie3;
								OB->NewCurSprite=0;
							}else{
								OB->DeletePath();
								OB->ClearOrders();
								DelObject(OB);
								Group[OB->Index]=NULL;
								OB=NULL;//died.	
							};
						};
#ifdef DEADTIMER
                        if (OB && OB->Sdoxlo > 240000) {
#else
                        if (OB && OB->Sdoxlo > 600) {
#endif
							OB->DeletePath();
							OB->ClearOrders();
							DelObject(OB);
							Group[OB->Index]=NULL;
							OB=NULL;//died.	
						};
					};
					if(OB)OB->Sdoxlo++;
				};
			};
		};
	};
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i]; 
		if(OB&&OB->NewMonst&&!OB->Sdoxlo){
            OB->SearchVictim();
			NewMonster* NMN=OB->newMons;
			if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
				int FX=0;
				int FY=0;
				OB->BestNX=OB->RealX;
				OB->BestNY=OB->RealY;
				if(OB->DestX>0){
					//int uux=(OB->DestX-(OB->Lx<<7))>>8;
					//int uuy=(OB->DestY-(OB->Ly<<7))>>8;
					int dst=Norma(OB->RealX-OB->DestX,OB->RealY-OB->DestY);
					if(dst>NMN->StopDistance){
						FX=div((OB->DestX-OB->RealX)<<7,dst).quot;
						FY=div((OB->DestY-OB->RealY)<<7,dst).quot;
					}else OB->DestX=-100;
				};
				bool ExtForce=OB->ForceX||OB->ForceY;
				FX+=OB->ForceX;
				FY+=OB->ForceY;
				bool Try1=false;
uuuu:
				if(FX||FY){
					char BestDir=char(GetDir(FX,FY));
					int bdx=NMN->MotionL.NFrames*NMN->OneStepDX[byte(BestDir)];
					int bdy=NMN->MotionL.NFrames*NMN->OneStepDY[byte(BestDir)];
					if(!CheckTerra(OB->RealX+bdx,OB->RealY+bdy,OB->Lx,0)){
						if((!Try1)&&(OB->PathX||OB->DestX<0)){
							int OFX=FX;
							int OFY=FY;
							int xs0=(OB->RealX-((OB->Lx)<<7))>>8;
							int ys0=(OB->RealY-((OB->Lx)<<7))>>8;
							int xs1=(OB->RealX+bdx-((OB->Lx)<<7))>>8;
							int ys1=(OB->RealY+bdy-((OB->Lx)<<7))>>8;
							if(CheckBar(xs0,ys1,OB->Lx,OB->Lx))FY=0;
							if(CheckBar(xs1,ys0,OB->Lx,OB->Lx))FX=0;
							if(FX&&FY&&OB->DestX>0){
								if(abs(FX)>abs(FY))FY=0;
								else FX=0;
							};
							if(!(FX||FY)){
                                OB->DeletePath();
								//if(OB->DestX>0){
								//	if(OB->PathX&&OB->NIPoints){
								//		OB->CreatePrePath(OB->PathX[OB->NIPoints-1],OB->PathY[OB->NIPoints-1]);
								//	}else{
								//		CreateFullPath((OB->DestX-(OB->Lx<<7))>>8,(OB->DestY-(OB->Lx<<7))>>8,OB);
								//	};
								//};
							}else{;
								Try1=true;
								goto uuuu;
							};
						};
					};
					char ddir=OB->RealDir-BestDir;
					Try1=false;
                    if(ExtForce){
    					if(abs(ddir)>64){
                            BestDir=BestDir+128;
                            ddir=OB->RealDir-BestDir;
                            if(ddir<8){
                                OB->RealDir=BestDir;
                            }else{
                                if(ddir>0)OB->RealDir-=8;
                                else OB->RealDir+=8;
                            };
                            OB->GraphDir=OB->RealDir;
		    				int trmv=TryToMove(OB,OB->RealDir,true);
			    			if(trmv!=-1)Try1=true;
                            if(trmv==0){
                                PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
                                OB->NextForceX+=FORCEX;
                                OB->NextForceY+=FORCEY;
                            };
    
	    					/*if(!trmv){
		    					//monsterы задрали!свалите отсюда !
			    				int bdx=NMN->MotionL.NFrames*NMN->OneStepDX[OB->RealDir];
				    			int bdy=NMN->MotionL.NFrames*NMN->OneStepDY[OB->RealDir];
					    		PushMonsters(OB->RealX-bdx,OB->RealY-bdy,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
						    	TryToStand(OB,false);
							    Try1=true;
	    					}else{
    							if(trmv==1)Try1=true;
		    				};*/
			    		}else{
                            if(abs(ddir)<8)OB->RealDir=BestDir;
                            else{
                                if(ddir>0)OB->RealDir-=8;
                                else OB->RealDir+=8;
                            };
                            OB->GraphDir+=8;
    						int trmv=TryToMove(OB,OB->RealDir,false);
                            if(trmv==0){
                                PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
                                OB->NextForceX+=FORCEX;
                                OB->NextForceY+=FORCEY;
                            };
    						if(trmv!=-1)Try1=true;
	    				};
                    };
					if(!Try1){
						if(abs(ddir)<MinRot){
							OB->RealDir=BestDir;
							OB->GraphDir=OB->RealDir;
							int trmv=TryToMove(OB,OB->RealDir,false);
                            if(trmv==0){
                                PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
                                OB->NextForceX+=FORCEX;
                                OB->NextForceY+=FORCEY;
                            };
							/*if(!trmv){
								//monsterы задрали!свалите отсюда !
								int bdx=NMN->MotionL.NFrames*NMN->OneStepDX[OB->RealDir];
								int bdy=NMN->MotionL.NFrames*NMN->OneStepDY[OB->RealDir];
								PushMonsters(OB->RealX+bdx,OB->RealY+bdy,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
								TryToStand(OB,false);
								/*
								if(!TryToMove(OB,OB->RealDir-16)){
									if(!TryToMove(OB,OB->RealDir+16)){
										int bdx=NMN->MotionL.NFrames*NMN->OneStepDX[OB->RealDir];
										int bdy=NMN->MotionL.NFrames*NMN->OneStepDY[OB->RealDir];
										PushMonsters(OB->RealX+bdx,OB->RealY+bdy,OB->Radius2,OB->Index);
										OB->InMotion=false;
										OB->NewCurSprite=0;
										OB->NewAnm=&NMN->Stand;
									};
								};*/
							//}else{
							if(trmv==-1){
								//Стены тут понатыкали!
								if(OB->DestX>0){
									if(OB->PathX&&OB->NIPoints){
										OB->CreatePrePath(OB->PathX[OB->NIPoints-1],OB->PathY[OB->NIPoints-1]);
										TryToStand(OB,false);
									}else{
										CreateFullPath((OB->DestX-(OB->Lx<<7))>>8,(OB->DestY-(OB->Lx<<7))>>8,OB);
										TryToStand(OB,false);									
									};
								}else{
									TryToStand(OB,false);
								};
							};
							OB->RotCntr=0;
						}else{
							if(ddir>0){
								RotateMon(OB,-MinRot);//OB->RealDir-=MinRot;
                                TryToMove(OB,OB->RealDir,false);
								/*if(OB->RotCntr>1){
									if(TryToMove(OB,OB->RealDir,false)!=1){
										TryToStand(OB,false);
									}else{
										OB->InMotion=true;
										OB->NewCurSprite=0;
										if(OB->LeftLeg)OB->NewAnm=&NMN->MotionL;
										else OB->NewAnm=&NMN->MotionR;
										OB->LeftLeg=!OB->LeftLeg;
										//OB->NewAnm=&NMN->MotionL;
										OB->RotCntr=0;
									};
								}else{
									TryToStand(OB,false);
									//OB->RotCntr++;
								};*/
							}else{
								RotateMon(OB,MinRot);//OB->RealDir+=MinRot;
                                TryToMove(OB,OB->RealDir,false);
								/*if(OB->RotCntr>1){
									if(TryToMove(OB,OB->RealDir,false)!=1){
										TryToStand(OB,false);
									}else{
										OB->InMotion=true;
										OB->NewCurSprite=0;
										if(OB->LeftLeg)OB->NewAnm=&NMN->MotionL;
										else OB->NewAnm=&NMN->MotionR;
										OB->LeftLeg=!OB->LeftLeg;
										//OB->NewAnm=&NMN->MotionR;
										OB->RotCntr=0;
									};
								}else{
									TryToStand(OB,false);
									//OB->RotCntr++;
								};*/
							};	
						};
					};
				}else{
					TryToStand(OB,true);
				};
			}else{
				if(OB->InMotion){
                    //OB->ForceX=0;
                    //OB->ForceY=0;
					//OB->UnBlockUnit();
                    //if(OB->ForceX||OB->ForceY){
                        int FX=0;
				        int FY=0;
				        if(OB->DestX>0){
					        int dst=Norma(OB->RealX-OB->DestX,OB->RealY-OB->DestY);
					        if(dst>NMN->StopDistance){
						        FX=div((OB->DestX-OB->RealX)<<7,dst).quot;
						        FY=div((OB->DestY-OB->RealY)<<7,dst).quot;
					        }else OB->DestX=-100;
				        };
                        FX+=OB->ForceX;
                        FY+=OB->ForceY;
                        char Bdir=char(GetDir(FX,FY));
                        if(OB->BackMotion)Bdir+=char(128);
                        //Bdir+=128;
                        char ddir=OB->RealDir-Bdir;
                        if(abs(ddir)<8)OB->RealDir=Bdir;
                        else{
                            if(ddir>0)OB->RealDir-=8;
                            else OB->RealDir+=8;
                        };
                        int xx1=OB->RealX;
                        int yy1=OB->RealY;
                        OB->GraphDir=OB->RealDir;
                        if(OB->BackMotion){
                            xx1-=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
                            yy1-=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
                        }else{
                            xx1+=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
                            yy1+=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
                        };
                        if(CheckTerra(xx1,yy1,OB->Lx,0)){
                            OB->RealX=xx1;
                            OB->RealY=yy1;
                        };
                        if(CheckPosition(OB->RealX,OB->RealY,OB->Radius2,OB->Index))
                             PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
                    //}else{;
					//    if(OB->BackMotion){
				    //		OB->RealX-=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
				    //		OB->RealY-=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
				    //	}else{
				    //		OB->RealX+=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
				    //		OB->RealY+=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
				    //	};
                    //};
				};
				OB->NewCurSprite+=FrmDec;
				APPLY_TIRING(OB);
			};
			OB->x=(OB->RealX-((OB->Lx)<<7))>>8;
			OB->y=(OB->RealY-((OB->Lx)<<7))>>8;
			OB->RZ=GetHeight((OB->RealX-((OB->Lx)<<7))>>4,(OB->RealY-((OB->Lx)<<7))>>4);
			if(OB->LocalOrder){
				OB->LocalOrder->DoLink(OB);
			}else OB->PrioryLevel=0;;
		};
	};
};
void UnitLight(OneObject* OB);
void CorrectLockPosition(OneObject* OB);
void MotionHandler0(OneObject* OB){
	addrand(5);
	//if(OB->delay)OB->delay--;
	if(OB->PathDelay>=FrmDec)OB->PathDelay-=FrmDec;
	else OB->PathDelay=0;
	if(OB->BackDelay>=FrmDec)OB->BackDelay-=FrmDec;
	else OB->BackDelay=0;
	NewMonster* NMN=OB->newMons;
	int MRot=NMN->MinRotator;
	int MRot1=MRot<<1;
	if(MRot1>MinRot)MRot1=MinRot;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	//NMN->AdvChar=ADC;
	//if(NMN->WaterActive)SpotByUnit(OB->RealX,OB->RealY,35,OB->RealDir);
	if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
		if((!OB->UnlimitedMotion)&&MFIELDS[OB->LockType].CheckBar(OB->x,OB->y,OB->Lx,OB->Lx)){
			CorrectLockPosition(OB);
		};
		int FX=0;
		int FY=0;
		OB->BestNX=OB->RealX;
		OB->BestNY=OB->RealY;
		if(OB->DestX>0){
			//int uux=(OB->DestX-(OB->Lx<<7))>>8;
			//int uuy=(OB->DestY-(OB->Ly<<7))>>8;
			int dst=Norma(OB->RealX-OB->DestX,OB->RealY-OB->DestY);
			if(dst>NMN->StopDistance){
				FX=div((OB->DestX-OB->RealX)<<7,dst).quot;
				FY=div((OB->DestY-OB->RealY)<<7,dst).quot;
			}else OB->DestX=-100;
		};
		if(OB->UnlimitedMotion){
			OB->ForceX=0;
			OB->ForceY=0;
		};
		bool ExtForce=OB->ForceX||OB->ForceY;
		FX+=OB->ForceX;
		FY+=OB->ForceY;
		bool Try1=false;
uuuu:
		if(FX||FY){
			char BestDir=char(GetDir(FX,FY));
			int bdx=NMN->MotionL.NFrames*NMN->OneStepDX[byte(BestDir)];
			int bdy=NMN->MotionL.NFrames*NMN->OneStepDY[byte(BestDir)];
			if(!OB->UnlimitedMotion){
				if(!CheckTerra(OB->RealX+bdx,OB->RealY+bdy,OB->Lx,OB->LockType)){
					if((!Try1)&&(OB->PathX||OB->DestX<0)){
						int OFX=FX;
						int OFY=FY;
						int xs0=(OB->RealX-((OB->Lx)<<7))>>8;
						int ys0=(OB->RealY-((OB->Lx)<<7))>>8;
						int xs1=(OB->RealX+bdx-((OB->Lx)<<7))>>8;
						int ys1=(OB->RealY+bdy-((OB->Lx)<<7))>>8;
						if(CheckBar(xs0,ys1,OB->Lx,OB->Lx))FY=0;
						if(CheckBar(xs1,ys0,OB->Lx,OB->Lx))FX=0;
						if(FX&&FY&&OB->DestX>0){
							if(abs(FX)>abs(FY))FY=0;
							else FX=0;
						};
						if(!(FX||FY)){
                            OB->DeletePath();
						}else{;
							Try1=true;
							goto uuuu;
						};
					};
				};
			};
			char ddir=OB->RealDir-BestDir;
			Try1=false;
            if(ExtForce){
   				if(abs(ddir)>64){
                    BestDir=BestDir+128;
                    ddir=OB->RealDir-BestDir;
                    if(ddir<8){
                        OB->RealDir=BestDir;
                    }else{
                        if(ddir>0)OB->RealDir-=8;
                        else OB->RealDir+=8;
                    };
                    OB->GraphDir=OB->RealDir;
		   			int trmv=TryToMove(OB,OB->RealDir,true);
		    		if(trmv!=-1)Try1=true;
                    if(trmv==0){
                        PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
                        OB->NextForceX+=FORCEX;
                        OB->NextForceY+=FORCEY;
                    };
				}else{
					if(abs(ddir)<8)OB->RealDir=BestDir;
                    else{
                        if(ddir>0)OB->RealDir-=8;
                        else OB->RealDir+=8;
					};
					OB->GraphDir+=8;
    				int trmv=TryToMove(OB,OB->RealDir,false);
					if(trmv==0){
						PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
						OB->NextForceX+=FORCEX;
						OB->NextForceY+=FORCEY;
					};
    				if(trmv!=-1)Try1=true;
				};
			};
			if(!Try1){
				if(abs(ddir)<MRot1){
					OB->RealDir=BestDir;
					OB->GraphDir=OB->RealDir;
					int trmv=TryToMove(OB,OB->RealDir,false);
					if(trmv==0){
					PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
					OB->NextForceX+=FORCEX;
						OB->NextForceY+=FORCEY;
					};
					if(trmv==-1){
					//Стены тут понатыкали!
						if(OB->DestX>0){
							if(OB->PathDelay){
								byte CDir=OB->RealDir;
								byte ddr=16;
								int NN=NMN->MotionL.NFrames;
								byte olddir=OB->RealDir;
								for(int i=0;i<8;i++){
									byte CDR1=CDir-ddr;
									OB->RealDir=CDR1;
									OB->GraphDir=CDR1;
									int tmtrv=TryToMove(OB,OB->RealDir,false);
									if(tmtrv!=-1){
										goto hhh1;
									};
									CDR1=CDir+ddr;
									OB->RealDir=CDR1;
									OB->GraphDir=CDR1;
									tmtrv=TryToMove(OB,OB->RealDir,false);
									if(tmtrv!=-1){
										goto hhh1;
									};
									ddr+=16;
								};
								OB->RealDir=olddir;
								OB->GraphDir=olddir;
hhh1:;
							}else{
								if(OB->PathX&&OB->NIPoints){
									addrand(11);
									OB->CreatePrePath(OB->PathX[OB->NIPoints-1],OB->PathY[OB->NIPoints-1]);
									TryToStand(OB,false);
									if(AllowPathDelay)OB->PathDelay=32+(rando()&7);
								}else{
									addrand(12);
									CreateFullPath((OB->DestX-(OB->Lx<<7))>>8,(OB->DestY-(OB->Lx<<7))>>8,OB);
									TryToStand(OB,false);									
								};
							};
						}else{
							TryToStand(OB,false);
						};
					};
					OB->RotCntr=0;
				}else{
					if(ddir>0){
						RotateMon(OB,-MRot);//OB->RealDir-=MinRot;
						//	if(OB->RotCntr>1){
				//		if(TryToMove(OB,OB->RealDir,false)!=1){
							TryToStand(OB,false);
				//		}else{
				//			OB->InMotion=true;
				//			OB->NewCurSprite=0;
				//			if(OB->LeftLeg)OB->NewAnm=&NMN->MotionL;
				//			else OB->NewAnm=&NMN->MotionR;
				//			OB->LeftLeg=!OB->LeftLeg;
				//			//OB->NewAnm=&NMN->MotionL;
				//			OB->RotCntr=0;
				//		};
					}else{
						RotateMon(OB,MRot);//OB->RealDir+=MinRot;
				//		if(TryToMove(OB,OB->RealDir,false)!=1){
							TryToStand(OB,false);
				//		}else{
				//			OB->InMotion=true;
				//			OB->NewCurSprite=0;
				//			if(OB->LeftLeg)OB->NewAnm=&NMN->MotionL;
				//			else OB->NewAnm=&NMN->MotionR;
				//			OB->LeftLeg=!OB->LeftLeg;
				//			//OB->NewAnm=&NMN->MotionR;
				//		OB->RotCntr=0;
				//		};
					};	
				};
			};
		}else{
			TryToStand(OB,true);
		};
	}else{
		if(OB->InMotion){
			OB->UnBlockUnit();
			if(OB->BackMotion){
				OB->RealX-=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
				OB->RealY-=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			}else{
				//OB->RealX+=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
				//OB->RealY+=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
				OB->RealX+=(NMN->OneStepDX[OB->RealDir]);
				OB->RealY+=(NMN->OneStepDY[OB->RealDir]);
			};
		};
		OB->NewCurSprite+=FrmDec;
		APPLY_TIRING(OB);
	};
	OB->x=(OB->RealX-((OB->Lx)<<7))>>8;
	OB->y=(OB->RealY-((OB->Lx)<<7))>>8;
	if(OB->AlwaysLock)OB->WeakBlockUnit();
	if(OB->StandTime<8)OB->RZ=GetUnitHeight((OB->RealX-((OB->Lx)<<7))>>4,(OB->RealY-((OB->Lx)<<7))>>4);
	//if(OB->LocalOrder){
	//	OB->LocalOrder->DoLink(OB);
	//}else OB->PrioryLevel=0;
	//OB->SearchVictim();
};
void SWAP(short* x,short* y){
	short t=*x;
	(*x)=(*y);
	*y=t;
};
void SWAP(word* x,word* y){
	word t=*x;
	(*x)=(*y);
	*y=t;
};
void SWAP(int* x,int* y){
	int t=*x;
	(*x)=(*y);
	*y=t;
};
void SWAP(byte*x,byte* y){
	byte t=*x;
	(*x)=(*y);
	*y=t;
};
void SWAP(char* x,char* y){
	char t=*x;
	(*x)=(*y);
	*y=t;
};
void SWAP(NewAnimation** x,NewAnimation** y){
	NewAnimation* t=*x;
	(*x)=(*y);
	*y=t;
};
extern MotionField UnitsField;
void AttackObjLink(OneObject* OBJ);
bool RemoveFoolsInCell(int cell,int x,int y,int Lx,OneObject* MyObj){
	cell+=VAL_MAXCX+1;
	word MyMid=MyObj->Index;
	int mx1=x+Lx-1;
	int my1=y+Lx-1;
    int NMon=MCount[cell];
	if(!NMon)return false;
	int ofs1=cell<<SHFCELL;
	word MID;
	int cx=(x<<1)+Lx;
	int cy=(y<<1)+Lx;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&OB->Index!=MyMid&&!(OB->InMotion||OB->BlockInfo)){
				int olx=OB->Lx;
				int ox=((OB->x)<<1)+olx;
				int oy=((OB->y)<<1)+olx;
				int dx=abs(ox-cx);
				int dy=abs(oy-cy);
				if(!(dx>=(olx+Lx)||dy>=(olx+Lx))){
					if(OB->NMask&MyObj->NMask){
						//intersection
						if(OB->StandTime>20&&OB->Lx==MyObj->Lx){
							if(OB->NewAnm->NFrames-FrmDec<=OB->NewCurSprite){
								SWAP(&OB->x,&MyObj->x);
								SWAP(&OB->y,&MyObj->y);
								SWAP(&OB->RealX,&MyObj->RealX);
								SWAP(&OB->RealY,&MyObj->RealY);
								SWAP(&OB->RealVx,&MyObj->RealVx);
								SWAP(&OB->RealVy,&MyObj->RealVy);
								SWAP(&OB->BestNX,&MyObj->BestNX);
								SWAP(&OB->BestNY,&MyObj->BestNY);
								SWAP(&OB->RealDir,&MyObj->RealDir);
								SWAP(&OB->GraphDir,&MyObj->GraphDir);
								//SWAP(&OB->NewAnm,&MyObj->NewAnm);
								//SWAP(&OB->NewCurSprite,&MyObj->NewCurSprite);
								//SWAP(&OB->LocalNewState,&MyObj->LocalNewState);
								//SWAP(&OB->NewState,&MyObj->NewState);
								/*
								Order1* OR1=OB->LocalOrder;
								Order1* OR2=MyObj->LocalOrder;
								Group[OB->Index]=MyObj;
								Group[MyObj->Index]=OB;
								word id=OB->Index;
								OB->Index=MyObj->Index;
								MyObj->Index=id;
								OB->LocalOrder=OR2;
								MyObj->LocalOrder=OR1;
								id=OB->Serial;
								OB->Serial=MyObj->Serial;
								MyObj->Serial=id;
								id=
								*/
								OB->MoveStage=0;
								MyObj->MoveStage=0;
							};
							return true;
						};
						int rx=OB->RealX>>5;
						int ry=OB->RealY>>5;
						UnitsField.BClrBar(OB->x,OB->y,OB->Lx);
						bool NeedRemove=true;
						int lx2=olx<<2;
						for(int r=0;r<8&&NeedRemove;r++){
							char* xi=Rarr[r].xi;
							char* yi=Rarr[r].yi;
							int N=Rarr[r].N;
							for(int j=0;j<N&&NeedRemove;j++){
								int rx1=rx+xi[j];
								int ry1=ry+yi[j];
								int sx=(rx1-lx2)>>3;
								int sy=(ry1-lx2)>>3;
								if(!(UnitsField.CheckBar(sx,sy,olx,olx)||CheckBar(sx,sy,olx,olx))){
									ox=sx+sx+olx;
									oy=sy+sy+olx;
									dx=abs(ox-cx);
									dy=abs(oy-cy);
									if(dx>=(olx+Lx)||dy>=(olx+Lx)){
										OB->x=sx;
										OB->y=sy;
										OB->RealX=rx1<<5;
										OB->RealY=ry1<<5;
										if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,OB->Lx);
										NeedRemove=false;
									};
								};
							};
						};
						if(NeedRemove){
							if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,OB->Lx);
						};
					}else{
						if(MyObj->LocalOrder&&MyObj->LocalOrder->DoLink==&AttackObjLink&&
							MyObj->LocalOrder->info.MoveToObj.ObjIndex==OB->Index)return true;
						else if(MyObj->EnemyID==0xFFFF)MyObj->AttackObj(OB->Index,128+4,1);
					};
				};
			};
		};
	};
	return false;
};
void RemoveFools(int x,int y,int Lx,OneObject* MyMid){
	if(MyMid&&MyMid->EnemyID==0xFFFF&&MyMid->BrigadeID!=0xFFFF)return;
	int cell=(x>>3)+((y>>3)<<VAL_SHFCX);

	if(RemoveFoolsInCell(cell,x,y,Lx,MyMid))return;

	if(RemoveFoolsInCell(cell-1,x,y,Lx,MyMid))return;
	if(RemoveFoolsInCell(cell+1,x,y,Lx,MyMid))return;
	if(RemoveFoolsInCell(cell-VAL_MAXCX,x,y,Lx,MyMid))return;
	if(RemoveFoolsInCell(cell+VAL_MAXCX,x,y,Lx,MyMid))return;

	if(RemoveFoolsInCell(cell-VAL_MAXCX+1,x,y,Lx,MyMid))return;
	if(RemoveFoolsInCell(cell+VAL_MAXCX+1,x,y,Lx,MyMid))return;;
	if(RemoveFoolsInCell(cell-VAL_MAXCX-1,x,y,Lx,MyMid))return;
	if(RemoveFoolsInCell(cell+VAL_MAXCX-1,x,y,Lx,MyMid))return;
};
void SmartTryToMove(OneObject* OB,byte NewDir,bool Dirc){
	if(!OB->LockType){
		char BestDir=NewDir;
		if(Dirc)BestDir+=char(128);
		NewMonster* NMN=OB->newMons;
		int mrot=NMN->MinRotator;
		bool NeedToPush=false;
		char dirr;
		switch(OB->MoveStage){
		case 0:
			dirr=BestDir;
			OB->MoveStage=1;
			break;
		case 1:
			dirr=BestDir+mrot;
			OB->MoveStage=2;
			break;
		case 2:
			dirr=BestDir-mrot;
			OB->MoveStage=3;
			break;
		case 3:
			dirr=BestDir+mrot+mrot;
			OB->MoveStage=4;
			break;
		case 4:
			dirr=BestDir-mrot-mrot;
			OB->MoveStage=5;
			break;
		case 5:
			dirr=BestDir+mrot+mrot+mrot;
			OB->MoveStage=6;
			break;
		case 6:
			dirr=BestDir-mrot-mrot-mrot;
			OB->MoveStage=255;
			break;
		case 255:
			NeedToPush=true;
			dirr=BestDir;
			break;
		};
		int nst=NMN->MotionL.NFrames>>SpeedSh;
		int dx0=(NMN->OneStepDX[byte(dirr)]*nst)<<SpeedSh;
		int dy0=(NMN->OneStepDY[byte(dirr)]*nst)<<SpeedSh;
		int LX=OB->Lx;
		UnitsField.BClrBar(OB->x,OB->y,LX);
		int xfin=(OB->RealX+dx0-(LX<<7))>>8;
		int yfin=(OB->RealY+dy0-(LX<<7))>>8;
		if(UnitsField.CheckBar(xfin,yfin,LX,LX)||CheckBar(xfin,yfin,LX,LX)){
			TryToStand(OB,false);
			if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,LX);
			if(NeedToPush){
				RemoveFools(xfin,yfin,LX,OB);
			};
		}else{
			if(Dirc)dirr+=char(128);
			OB->RealDir=dirr;
			OB->GraphDir=dirr;
			NewMonster* NM=OB->newMons;
			int dx=NM->OneStepDX[byte(dirr)];
			int dy=NM->OneStepDY[byte(dirr)];
			NewTryToMove(OB,dirr,Dirc,dx,dy);
			if(!OB->UnlimitedMotion)UnitsField.BSetBar(xfin,yfin,LX);
			OB->x=xfin;
			OB->y=yfin;
			OB->MoveStage=0;
		};
	};
};
__forceinline void MotionHandler1(OneObject* OB){
	if(!OB->LocalOrder)OB->DestX=-1;
	addrand(OB->Index);
	//if(OB->delay)OB->delay--;
	if(OB->PathDelay>=FrmDec)OB->PathDelay-=FrmDec;
	else OB->PathDelay=0;
	//if(OB->BackDelay>=FrmDec)OB->BackDelay-=FrmDec;
	//else OB->BackDelay=0;
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	//NMN->AdvChar=ADC;
	if(NMN->WaterActive)SpotByUnit(OB->RealX,OB->RealY,35,OB->RealDir);
	if(OB->NewAnm->MoveBreak){
		if(OB->DestX>0)OB->NewCurSprite=OB->NewAnm->NFrames-FrmDec;
	}
	if(OB->NewAnm->CanBeBroken){
		if(OB->DestX>0||OB->NewState!=OB->LocalNewState){
			OB->NewCurSprite=OB->NewAnm->NFrames-FrmDec;
		};
	};
	bool TESTMOVE=(OB->BrigadeID==0xFFFF||OB->EnemyID!=0xFFFF);
	addrand(OB->NewCurSprite);
	if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
		APPLY_TIRING(OB);
		addrand(OB->DestX);
		if(OB->DestX>0){
			//assert(!(((OB->RealX-128)>>8)-OB->x));
			//assert(!(((OB->RealY-128)>>8)-OB->y));
			//assert(OB->BestNX==OB->RealX);
			//assert(OB->BestNY==OB->RealY);
			int dx=OB->DestX-OB->RealX;
			int dy=OB->DestY-OB->RealY;
			int Nrm=Norma(dx,dy);
			char BestDir=char(GetDir(dx,dy));
			char ddir=BestDir-OB->RealDir;
			int mrot=NMN->MinRotator;
			bool NeedToPush=false;
			if(abs(ddir)<=mrot){
				byte dirr;
				switch(OB->MoveStage){
				case 0:
					dirr=BestDir;
					OB->MoveStage=1;
					break;
				case 1:
					dirr=BestDir+mrot;
					OB->MoveStage=2;
					break;
				case 2:
					dirr=BestDir-mrot;
					OB->MoveStage=3;
					break;
				case 3:
					dirr=BestDir+mrot+mrot;
					OB->MoveStage=4;
					break;
				case 4:
					dirr=BestDir-mrot-mrot;
					OB->MoveStage=5;
					break;
				case 5:
					dirr=BestDir+mrot+mrot+mrot;
					OB->MoveStage=6;
					break;
				case 6:
					dirr=BestDir-mrot-mrot-mrot;
					OB->MoveStage=255;
					break;
				case 255:
					NeedToPush=true;
					dirr=BestDir;
					break;
				};
				
				int nf=NMN->MotionL.NFrames>>SpeedSh;
				int odx=NMN->OneStepDX[dirr]<<SpeedSh;
				int ody=NMN->OneStepDY[dirr]<<SpeedSh;
	
				if(OB->NewState){
					odx=(odx*ADC->Rate[OB->NewState-1])>>4;
					ody=(ody*ADC->Rate[OB->NewState-1])>>4;
				};
				int dx0=nf*odx;
				int dy0=nf*ody;
				/*
				int nst=NMN->MotionL.NFrames;
				int dxx=(NMN->OneStepDX[byte(dirr)]*RATE)>>4;
				int dyy=(NMN->OneStepDY[byte(dirr)]*RATE)>>4;
				int dx0=dxx*nst;
				int dy0=dyy*nst;
				*/
				int LX=OB->Lx;
				UnitsField.BClrBar(OB->x,OB->y,LX);
				int xrfin=OB->RealX+dx0;
				int yrfin=OB->RealY+dy0;
				int xfin=(xrfin-(LX<<7))>>8;
				int yfin=(yrfin-(LX<<7))>>8;
				bool OnTheWay=false;
				bool unlim=!OB->UnlimitedMotion;
				if((unlim)&&(TESTMOVE&&UnitsField.CheckBar(xfin,yfin,LX,LX)||(OnTheWay=CheckBar(xfin,yfin,LX,LX)))){
					if(OnTheWay&&(NeedToPush||!TESTMOVE)){
						if(OB->PathX&&OB->NIPoints){
							addrand(6);
							OB->CreatePrePath(OB->PathX[OB->NIPoints-1],OB->PathY[OB->NIPoints-1]);
							if(AllowPathDelay)OB->PathDelay=32+(rando()&7);
						}else{
							addrand(32);
							CreateFullPath((OB->DestX-(OB->Lx<<7))>>8,(OB->DestY-(OB->Lx<<7))>>8,OB);
							if(OB->PathX)OB->MoveStage=0;
						};						
					};
					addrand(7);
					TryToStand(OB,false);
					if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,LX);
					if(NeedToPush){
						RemoveFools(xfin,yfin,LX,OB);
					};
				}else{
					if(unlim){
						word MID=CheckMotionThroughEnemyAbility(OB,xrfin,yrfin);
						if(MID!=0xFFFF){
							addrand(11);
							TryToStand(OB,false);
							goto JJJ;
						};
					};
					OB->RealVx=0;
					OB->RealVy=0;
					//OB->BestNX=OB->RealX;
					//OB->BestNY=OB->RealY;
					TryToMove(OB,dirr,false);
					OB->RealDir=dirr;
					OB->GraphDir=dirr;
					if(OB->RealVx||OB->RealVy){
						OB->x=xfin;
						OB->y=yfin;
						//assert(OB->BestNX==xrfin);
						//assert(OB->BestNY==yrfin);
					};
					if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,LX);
					OB->MoveStage=0;
JJJ:;
				};
			}else{
				if(ddir>0)OB->RealDir+=mrot;
				else OB->RealDir-=mrot;
				OB->GraphDir=OB->RealDir;
			};
		}else{
			addrand(71);
			TryToStand(OB,true);
			OB->MoveStage=0;
		};
	}else{
		if(OB->InMotion){
			OB->UnBlockUnit();
			//if(OB->BackMotion){
			//	OB->RealX-=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
			//	OB->RealY-=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			//}else{
			//	OB->RealX+=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
			//	OB->RealY+=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			//};
			OB->RealX+=OB->RealVx<<SpeedSh;
			OB->RealY+=OB->RealVy<<SpeedSh;
			if(OB->AlwaysLock)OB->WeakBlockUnit();
		};
		OB->NewCurSprite+=FrmDec;
		APPLY_TIRING(OB);
	};
	if(OB->StandTime<2)OB->RZ=GetUnitHeight((OB->RealX-((OB->Lx)<<7))>>4,(OB->RealY-((OB->Lx)<<7))>>4);
	addrand(OB->Index);
};
void MotionHandler5(OneObject* OB){
	addrand(3);
	//if(OB->delay)OB->delay--;
	if(OB->PathDelay>=FrmDec)OB->PathDelay-=FrmDec;
	else OB->PathDelay=0;
	if(OB->BackDelay>=FrmDec)OB->BackDelay-=FrmDec;
	else OB->BackDelay=0;
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	//NMN->AdvChar=ADC;
	if(NMN->WaterActive)SpotByUnit(OB->RealX,OB->RealY,35,OB->RealDir);
	if(OB->NewAnm->MoveBreak){
		if(OB->DestX>0)OB->NewCurSprite=OB->NewAnm->NFrames-FrmDec;
	}
	if(OB->NewAnm->CanBeBroken){
		if(OB->DestX>0||OB->NewState!=OB->LocalNewState){
			OB->NewCurSprite=OB->NewAnm->NFrames-FrmDec;
		};
	};
	if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
		if(OB->DestX>0){
			int dx=OB->DestX-OB->RealX;
			int dy=OB->DestY-OB->RealY;
			int Nrm=Norma(dx,dy);
			char BestDir=char(GetDir(dx,dy));
			char ddir=BestDir-OB->RealDir;
			int mrot=NMN->MinRotator;
			bool NeedToPush=false;
			if(abs(ddir)<=mrot){
				char dirr;
				switch(OB->MoveStage){
				case 0:
					dirr=BestDir;
					OB->MoveStage=1;
					break;
				case 1:
					dirr=BestDir+mrot;
					OB->MoveStage=2;
					break;
				case 2:
					dirr=BestDir-mrot;
					OB->MoveStage=3;
					break;
				case 3:
					dirr=BestDir+mrot+mrot;
					OB->MoveStage=4;
					break;
				case 4:
					dirr=BestDir-mrot-mrot;
					OB->MoveStage=5;
					break;
				case 5:
					dirr=BestDir+mrot+mrot+mrot;
					OB->MoveStage=6;
					break;
				case 6:
					dirr=BestDir-mrot-mrot-mrot;
					OB->MoveStage=255;
					break;
				case 255:
					NeedToPush=true;
					dirr=BestDir;
					break;
				};
				int nst=NMN->MotionL.NFrames;
				int dxx=NMN->OneStepDX[byte(dirr)];
				int dyy=NMN->OneStepDY[byte(dirr)];
				int dx0=dxx*nst;
				int dy0=dyy*nst;
				int LX=OB->Lx;
				UnitsField.BClrBar(OB->x,OB->y,LX);
				int xrfin=OB->RealX+dx0;
				int yrfin=OB->RealY+dy0;
				int xfin=(xrfin-(LX<<7))>>8;
				int yfin=(yrfin-(LX<<7))>>8;
				bool OnTheWay=false;
				bool unlim=!OB->UnlimitedMotion;
				if((unlim)&&(UnitsField.CheckBar(xfin,yfin,LX,LX)||(OnTheWay=CheckBar(xfin,yfin,LX,LX)))){
					if(OnTheWay&&NeedToPush){
						if(OB->PathX&&OB->NIPoints){
							addrand(6);
							OB->CreatePrePath(OB->PathX[OB->NIPoints-1],OB->PathY[OB->NIPoints-1]);
							if(AllowPathDelay)OB->PathDelay=32+(rando()&7);
						}else{
							addrand(32);
							CreateFullPath((OB->DestX-(OB->Lx<<7))>>8,(OB->DestY-(OB->Lx<<7))>>8,OB);
						};
						OB->MoveStage=0;
					};
					addrand(7);
					TryToStand(OB,false);
					if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,LX);
					if(NeedToPush){
						RemoveFools(xfin,yfin,LX,OB);
					};
				}else{
					if(unlim){
						word MID=CheckMotionThroughEnemyAbility(OB,xrfin,yrfin);
						if(MID!=0xFFFF){
							addrand(11);
							TryToStand(OB,false);
							goto JJJ;
						};
					};
					TryToMove(OB,dirr,false);
					OB->RealDir=dirr;
					OB->GraphDir=dirr;
					if(!OB->UnlimitedMotion)UnitsField.BSetBar(xfin,yfin,LX);
					OB->x=xfin;
					OB->y=yfin;
					OB->MoveStage=0;
JJJ:;
				};
			}else{
				if(ddir>0)OB->RealDir+=mrot;
				else OB->RealDir-=mrot;
				OB->GraphDir=OB->RealDir;
				TryToMove(OB,OB->RealDir,false);
			};
		}else{
			addrand(71);
			TryToStand(OB,true);
			OB->MoveStage=0;
		};
	}else{
		if(OB->InMotion){
			OB->UnBlockUnit();
			//if(OB->BackMotion){
			//	OB->RealX-=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
			//	OB->RealY-=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			//}else{
			//	OB->RealX+=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
			//	OB->RealY+=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			//};
			OB->RealX+=OB->RealVx<<SpeedSh;
			OB->RealY+=OB->RealVy<<SpeedSh;
			if(OB->AlwaysLock)OB->WeakBlockUnit();
		};
		OB->NewCurSprite+=FrmDec;
		APPLY_TIRING(OB);
	};
	if(OB->StandTime<2)OB->RZ=GetUnitHeight((OB->RealX-((OB->Lx)<<7))>>4,(OB->RealY-((OB->Lx)<<7))>>4);
};
int GetPState(int dir){
	int d=((dir-64+8+1024)>>4);
	return d&15;
};
void MotionHandler4(OneObject* OB){
	addrand(3);
	//if(OB->delay)OB->delay--;
	if(OB->PathDelay>=FrmDec)OB->PathDelay-=FrmDec;
	else OB->PathDelay=0;
	if(OB->BackDelay>=FrmDec)OB->BackDelay-=FrmDec;
	else OB->BackDelay=0;
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	//NMN->AdvChar=ADC;
	if(OB->NewAnm->MoveBreak){
		if(OB->DestX>0)OB->NewCurSprite=OB->NewAnm->NFrames-FrmDec;
	}
	if(OB->NewAnm->CanBeBroken){
		if(OB->DestX>0||OB->NewState!=OB->LocalNewState){
			OB->NewCurSprite=OB->NewAnm->NFrames-FrmDec;
		};
	};

	if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
		int LX=OB->Lx;
		UnitsField.BClrBar(OB->x,OB->y,LX);
		OB->x=(OB->RealX-(LX<<7))>>8;
		OB->y=(OB->RealY-(LX<<7))>>8;
		UnitsField.BSetBar(OB->x,OB->y,LX);
//XX1:
		if(OB->DestX>0){
			int dx=OB->DestX-OB->RealX;
			int dy=OB->DestY-OB->RealY;
			int Nrm=Norma(dx,dy);
			//if(Nrm<32*16){
			//	OB->DestX=-1;
			//	OB->DestY=-1;
			//	OB->GroundState=0;
			//	return;
			//};
			NewMonster* NM=OB->newMons;
			char BestDir=char(GetDir(dx,dy));
			char ddir=BestDir-OB->RealDir;
			int OLDD=GetPState(OB->RealDir);
			int NEWD=GetPState(BestDir);
			if(OLDD!=NEWD){
				if(OB->LocalNewState==2){
					if(OB->NewAnm==&NM->PMotionL[1]){
						OB->RealDir+=16;
						OB->GraphDir=OB->RealDir;
						TryToStand(OB,0);
						return;
						//goto XX1;
					}else 
					if(OB->NewAnm==&NM->PMotionR[1]){
						TryToStand(OB,0);
						return;
						//goto XX1;
					}else{
						if(ddir>0){
							OB->NewAnm=&NM->PMotionL[1];
							OB->NewCurSprite=0;
							return;
						}else{
							OB->NewAnm=&NM->PMotionR[1];
							OB->NewCurSprite=0;
							OB->RealDir-=16;
							OB->GraphDir=OB->RealDir;
							return;
						};
					};
				}else{
					OB->NewState=2;
					OB->GroundState=2;
					TryToStand(OB,0);
					return;
				};
			}else{ 
				OB->RealDir=BestDir;
				if(OB->LocalNewState==2){
					OB->NewState=0;
					OB->GroundState=0;
					TryToStand(OB,0);
					return;
				}else{
					if(OB->LocalNewState==1){
						TryToMove(OB,OB->RealDir,false);
						return;
					}else{
						OB->NewState=1;
						OB->GroundState=1;
						TryToStand(OB,0);
						return;
					};
				};
			};
		}else{
			//OB->NewState=OB->GroundState;
			//addrand(71);
			TryToStand(OB,true);
			OB->MoveStage=0;
		};
	}else{
		if(OB->InMotion){
			OB->UnBlockUnit();
			//if(OB->BackMotion){
			//	OB->RealX-=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
			//	OB->RealY-=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			//}else{
			//	OB->RealX+=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
			//	OB->RealY+=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			//};
			OB->RealX+=OB->RealVx<<SpeedSh;
			OB->RealY+=OB->RealVy<<SpeedSh;
			if(OB->AlwaysLock)OB->WeakBlockUnit();
		};
		OB->NewCurSprite+=FrmDec;
		APPLY_TIRING(OB);
	};
	if(OB->StandTime<2)OB->RZ=GetUnitHeight((OB->RealX-((OB->Lx)<<7))>>4,(OB->RealY-((OB->Lx)<<7))>>4);
};
//for slow rotated objects

int CheckMotionAbility(OneObject* OB,char dirr,bool Remove){
	if(OB->UnlimitedMotion)return 0;
	NewMonster* NMN=OB->newMons;
	int nst=NMN->MotionL.NFrames;
	int dxx=NMN->OneStepDX[byte(dirr)];
	int dyy=NMN->OneStepDY[byte(dirr)];
	int dx0=dxx*nst;
	int dy0=dyy*nst;
	int LX=OB->Lx;
	UnitsField.BClrBar(OB->x,OB->y,LX);
	int xfin=(OB->RealX+dx0-(LX<<7))>>8;
	int yfin=(OB->RealY+dy0-(LX<<7))>>8;
	if(Remove){
		if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,LX);
		RemoveFools(xfin,yfin,LX,OB);
	}else{
		bool NewPath=false;
		if(UnitsField.CheckBar(xfin,yfin,LX,LX)||(NewPath=CheckBar(xfin,yfin,LX,LX))){
			if(NewPath)return -1;
			else return -2;
		};
	};
	return 0;
};

void MotionHandler3(OneObject* OB){
	//if(OB->delay)OB->delay--;
	if(OB->PathDelay>=FrmDec)OB->PathDelay-=FrmDec;
	else OB->PathDelay=0;
	if(OB->BackDelay>=FrmDec)OB->BackDelay-=FrmDec;
	else OB->BackDelay=0;
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	//NMN->AdvChar=ADC;
	if(NMN->WaterActive)SpotByUnit(OB->RealX,OB->RealY,35,OB->RealDir);
	if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
		if(OB->DestX>0){
			int dx=OB->DestX-OB->RealX;
			int dy=OB->DestY-OB->RealY;
			int Nrm=Norma(dx,dy);
			char BestDir=char(GetDir(dx,dy));
			char ddir=BestDir-OB->RealDir;
			int mrot=NMN->MinRotator;
			int ChRes,ChRes0;
			char NewBestDir;
			int LX=OB->Lx;
			UnitsField.BClrBar(OB->x,OB->y,LX);
			bool CanMove=true;
			bool NeedPush=false;
			bool NeedPath=false;
			if(ChRes0=CheckMotionAbility(OB,BestDir,false)){
				if(ChRes=CheckMotionAbility(OB,BestDir+16,false)){
					if(ChRes=CheckMotionAbility(OB,BestDir-16,false)){
						if(ChRes=CheckMotionAbility(OB,BestDir+32,false)){
							if(ChRes=CheckMotionAbility(OB,BestDir-32,false)){
								if(ChRes=CheckMotionAbility(OB,BestDir+48,false)){
									if(ChRes=CheckMotionAbility(OB,BestDir-48,false)){
										if(ChRes=CheckMotionAbility(OB,BestDir-54,false)){
											if(ChRes=CheckMotionAbility(OB,BestDir+54,false)){
												if(ChRes0==-1){
													NeedPath=true;
												}else{
													NeedPush=true;
												}
												CanMove=false;
											}else NewBestDir=BestDir+54;
										}else NewBestDir=BestDir-54;
									}else NewBestDir=BestDir-48;
								}else NewBestDir=BestDir+48;
							}else NewBestDir=BestDir-32;
						}else NewBestDir=BestDir+32;
					}else NewBestDir=BestDir-16;
				}else NewBestDir=BestDir+16;
			}else NewBestDir=BestDir;
			if(CanMove){
				OB->MoveStage=0;
				char ddir=NewBestDir-OB->RealDir;
				if(abs(ddir)<=mrot){
					if(NeedPath){
						if(OB->PathX&&OB->NIPoints){
							OB->CreatePrePath(OB->PathX[OB->NIPoints-1],OB->PathY[OB->NIPoints-1]);
							if(AllowPathDelay)OB->PathDelay=32+(rando()&7);
						}else{
							CreateFullPath((OB->DestX-(OB->Lx<<7))>>8,(OB->DestY-(OB->Lx<<7))>>8,OB);
						};
						OB->MoveStage=0;
						if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,LX);
						TryToStand(OB,false);
					}else if(NeedPush){
						if(OB->MoveStage>4)CheckMotionAbility(OB,NewBestDir,true);
						else OB->MoveStage++;
						if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,LX);
					};
					if(CanMove){
						int nst=NMN->MotionL.NFrames>>SpeedSh;
						int dxx=NMN->OneStepDX[byte(NewBestDir)]<<SpeedSh;
						int dyy=NMN->OneStepDY[byte(NewBestDir)]<<SpeedSh;
						int dx0=dxx*nst;
						int dy0=dyy*nst;
						int xfin=(OB->RealX+dx0-(LX<<7))>>8;
						int yfin=(OB->RealY+dy0-(LX<<7))>>8;
						NewTryToMove(OB,NewBestDir,false,dxx>>SpeedSh,dyy>>SpeedSh);
						OB->RealDir=NewBestDir;
						OB->GraphDir=NewBestDir;
						if(!OB->UnlimitedMotion)UnitsField.BSetBar(xfin,yfin,LX);
						OB->x=xfin;
						OB->y=yfin;
						OB->MoveStage=0;
					}else{
						if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,LX);
						TryToStand(OB,false);
					};
				}else{
					if(ddir>0)OB->RealDir+=mrot;
					else OB->RealDir-=mrot;
					OB->GraphDir=OB->RealDir;
					if(!OB->UnlimitedMotion)UnitsField.BSetBar(OB->x,OB->y,LX);
					TryToStand(OB,false);
				};
			};
		}else{
			if(!OB->LocalOrder){
				if(OB->NewState==1||OB->NewState==2)OB->NewState=0;
			};
			TryToStand(OB,true);
			OB->MoveStage=0;
		};
	}else{
		if(OB->InMotion){
			OB->UnBlockUnit();
			//if(OB->BackMotion){
			//	OB->RealX-=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
			//	OB->RealY-=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			//}else{
			//	OB->RealX+=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
			//	OB->RealY+=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			//};
			OB->RealX+=OB->RealVx<<SpeedSh;
			OB->RealY+=OB->RealVy<<SpeedSh;
			if(OB->AlwaysLock)OB->WeakBlockUnit();
		};
		OB->NewCurSprite+=FrmDec;
		APPLY_TIRING(OB);
	};
	OB->RZ=GetUnitHeight((OB->RealX-((OB->Lx)<<7))>>4,(OB->RealY-((OB->Lx)<<7))>>4);
};
void MotionHandler2(OneObject* OB){
	//if(OB->delay)OB->delay--;
	if(OB->PathDelay>=FrmDec)OB->PathDelay-=FrmDec;
	else OB->PathDelay=0;
	if(OB->BackDelay>=FrmDec)OB->BackDelay-=FrmDec;
	else OB->BackDelay=0;
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	//NMN->AdvChar=ADC;
	if(NMN->WaterActive)SpotByUnit(OB->RealX,OB->RealY,35,OB->RealDir);
	if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
		int FX=0;
		int FY=0;
		OB->BestNX=OB->RealX;
		OB->BestNY=OB->RealY;
		if(OB->DestX>0){
			//int uux=(OB->DestX-(OB->Lx<<7))>>8;
			//int uuy=(OB->DestY-(OB->Ly<<7))>>8;
			int dst=Norma(OB->RealX-OB->DestX,OB->RealY-OB->DestY);
			if(dst>NMN->StopDistance){
				FX=div((OB->DestX-OB->RealX)<<7,dst).quot;
				FY=div((OB->DestY-OB->RealY)<<7,dst).quot;
			}else OB->DestX=-100;
		};
		if(OB->UnlimitedMotion){
			OB->ForceX=0;
			OB->ForceY=0;
		};
		bool ExtForce=OB->ForceX||OB->ForceY;
		FX+=OB->ForceX;
		FY+=OB->ForceY;
		bool Try1=false;
uuuu:
		if(FX||FY){
			char BestDir=char(GetDir(FX,FY));
			int bdx=NMN->MotionL.NFrames*NMN->OneStepDX[byte(BestDir)];
			int bdy=NMN->MotionL.NFrames*NMN->OneStepDY[byte(BestDir)];
			if(!OB->UnlimitedMotion){
				if(!CheckTerra(OB->RealX+bdx,OB->RealY+bdy,OB->Lx,OB->LockType)){
					if((!Try1)&&(OB->PathX||OB->DestX<0)){
						int OFX=FX;
						int OFY=FY;
						int xs0=(OB->RealX-((OB->Lx)<<7))>>8;
						int ys0=(OB->RealY-((OB->Lx)<<7))>>8;
						int xs1=(OB->RealX+bdx-((OB->Lx)<<7))>>8;
						int ys1=(OB->RealY+bdy-((OB->Lx)<<7))>>8;
						if(CheckBar(xs0,ys1,OB->Lx,OB->Lx))FY=0;
						if(CheckBar(xs1,ys0,OB->Lx,OB->Lx))FX=0;
						if(FX&&FY&&OB->DestX>0){
							if(abs(FX)>abs(FY))FY=0;
							else FX=0;
						};
						if(!(FX||FY)){
                            OB->DeletePath();
						}else{;
							Try1=true;
							goto uuuu;
						};
					};
				};
			};
			char ddir=OB->RealDir-BestDir;
			Try1=false;
            if(ExtForce){
   				if(abs(ddir)>64){
                    BestDir=BestDir+128;
                    ddir=OB->RealDir-BestDir;
                    if(ddir<8){
                        OB->RealDir=BestDir;
                    }else{
                        if(ddir>0)OB->RealDir-=8;
                        else OB->RealDir+=8;
                    };
                    OB->GraphDir=OB->RealDir;
		   			int trmv=TryToMove(OB,OB->RealDir,true);
		    		if(trmv!=-1)Try1=true;
                    if(trmv==0){
                        PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
                        OB->NextForceX+=FORCEX;
                        OB->NextForceY+=FORCEY;
                    };
				}else{
					if(abs(ddir)<8)OB->RealDir=BestDir;
                    else{
                        if(ddir>0)OB->RealDir-=8;
                        else OB->RealDir+=8;
					};
					OB->GraphDir+=8;
    				int trmv=TryToMove(OB,OB->RealDir,false);
					if(trmv==0){
						PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
						OB->NextForceX+=FORCEX;
						OB->NextForceY+=FORCEY;
					};
    				if(trmv!=-1)Try1=true;
				};
			};
			if(!Try1){
				if(abs(ddir)<MinRot){
					OB->RealDir=BestDir;
					OB->GraphDir=OB->RealDir;
					int trmv=TryToMove(OB,OB->RealDir,false);
					if(trmv==0){
					PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
					OB->NextForceX+=FORCEX;
						OB->NextForceY+=FORCEY;
					};
					if(trmv==-1){
					//Стены тут понатыкали!
						if(OB->DestX>0){
							if(OB->PathX&&OB->NIPoints){
								OB->CreatePrePath(OB->PathX[OB->NIPoints-1],OB->PathY[OB->NIPoints-1]);
								TryToStand(OB,false);
								//OB->PathDelay=32+(rando()&7);
							}else{
								CreateFullPath((OB->DestX-(OB->Lx<<7))>>8,(OB->DestY-(OB->Lx<<7))>>8,OB);
								TryToStand(OB,false);									
							};
						}else{
							TryToStand(OB,false);
						};
					};
					OB->RotCntr=0;
				}else{
					if(ddir>0){
						RotateMon(OB,-MinRot);//OB->RealDir-=MinRot;
						//	if(OB->RotCntr>1){
						if(TryToMove(OB,OB->RealDir,false)!=1){
							TryToStand(OB,false);
						}else{
							OB->InMotion=true;
							OB->NewCurSprite=0;
							if(OB->LeftLeg)OB->NewAnm=&NMN->MotionL;
							else OB->NewAnm=&NMN->MotionR;
							OB->LeftLeg=!OB->LeftLeg;
							//OB->NewAnm=&NMN->MotionL;
							OB->RotCntr=0;
						};
					}else{
						RotateMon(OB,MinRot);//OB->RealDir+=MinRot;
						if(TryToMove(OB,OB->RealDir,false)!=1){
							TryToStand(OB,false);
						}else{
							OB->InMotion=true;
							OB->NewCurSprite=0;
							if(OB->LeftLeg)OB->NewAnm=&NMN->MotionL;
							else OB->NewAnm=&NMN->MotionR;
							OB->LeftLeg=!OB->LeftLeg;
							//OB->NewAnm=&NMN->MotionR;
						OB->RotCntr=0;
						};
					};	
				};
			};
		}else{
			TryToStand(OB,true);
		};
	}else{
		if(OB->InMotion){
			OB->UnBlockUnit();
			if(OB->BackMotion){
				OB->RealX-=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
				OB->RealY-=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			}else{
				OB->RealX+=(NMN->OneStepDX[OB->RealDir]*OB->Speed)>>4;
				OB->RealY+=(NMN->OneStepDY[OB->RealDir]*OB->Speed)>>4;
			};
		};
		OB->NewCurSprite+=FrmDec;
		APPLY_TIRING(OB);
	};
	OB->x=(OB->RealX-((OB->Lx)<<7))>>8;
	OB->y=(OB->RealY-((OB->Lx)<<7))>>8;
	OB->RZ=0;//GetHeight((OB->RealX-((OB->Lx)<<7))>>4,(OB->RealY-((OB->Lx)<<7))>>4);
	//if(OB->LocalOrder){
	//	OB->LocalOrder->DoLink(OB);
	//}else{
	//	OB->PrioryLevel=0;
	//	//OB->AlwaysLock=false;
	//};
	//OB->SearchVictim();
};
void CreateOrdersList(OneObject* OB,char* Str){
	char ccc[32];
	Str[0]=0;
	if(OB->LocalOrder){
		Order1* OR1=OB->LocalOrder;
		while(OR1){
			sprintf_s(ccc,sizeof(ccc)," %d",OR1->OrderType);
			strcat(Str,ccc);
			rando();
			OR1=OR1->NextOrder;
		};
	}else{
		strcpy(Str,"No orders.");
	};

};
char scs[16];
char* its1(int i){
	sprintf(scs," %d",i);
	return scs;
};
void AssignHint1(char* s,int time);
int DoLink_Time,SearchVictim_Time,CheckCapture_Time;
//void addrand(int i);
void OpenGates(byte NI);
void CloseGates(byte NI);
void MotionHandler4(OneObject* OB);
void MotionHandlerOfNewSheeps(OneObject* OB);
void addSpriteAnyway(int x,int y,SprGroup* SG,word id);
extern SprGroup SPECIAL;
extern int LastAddSpr;
extern byte BaloonState;
extern int FogMode;
void EliminateBuilding(OneObject* OB);
extern bool SpriteSuccess;
bool ViewFriendsInScenario=0;
int HUNGPROB=50;
int CheckNtIntegrity(byte NI);
void LongProcesses(){
	int T0=GetTickCount();
	//RunPF(10);
	int d=tmtmt&7;
	int d1=tmtmt&15;
	
//	StopPF(10);
	SearchVictim_Time=GetTickCount()-T0;
	T0=GetTickCount();
	//RunPF(11);
	bool LIGHT=FogMode&&BaloonState!=1&&(!NATIONS[NatRefTBL[MyNation]].Vision);
	byte Mask=NATIONS[NatRefTBL[MyNation]].NMask;
	byte MyNT=NatRefTBL[MyNation];
	bool sce=!((SCENINF.hLib==NULL)||ViewFriendsInScenario/*||SCENINF.StandartVictory*/);
	for(int i=0;i<8;i++){
		int N=NtNUnits[i];
		if(N){
			OpenGates(i);
			//word* units=NatList[i];
			//int NU=NtNUnits[i];
			//word* NL=NatList[i];
			//word* NL0=NatList[i];

			addrand(NU);
			//addrand(CheckNtIntegrity(i));
			//addrand(NL==NL0);
			for(int j=0;j<NtNUnits[i];j++){
				//assert(units[j]<8192);
				//addrand(CheckNtIntegrity(i));
				word UMID=NatList[i][j];
				//NL++;
				if(UMID!=0xFFFF){
					OneObject* OB=Group[UMID];
					addrand(UMID);
					addrand(j);
					if(OB){
						addname(OB->Index);
						if(!OB->Sdoxlo){
							if(OB->InFire&&OB->Life){
								if(OB->NewBuilding){
									int DL=OB->MaxLife>>11;
									if(DL<1)DL=1;
									if(OB->Life)OB->MakeDamage(DL,DL,NULL,255);
								}else{
									if(!(tmtmt&31)){
										if(!(OB->LockType||OB->newMons->Artilery)){
											int MIN=OB->MaxLife/10;
											if(OB->Life>MIN)OB->MakeDamage(1,1,NULL,255);
										};
									};
								};
							};
							if(OB->Nat->AI_Enabled){
								int use=OB->newMons->Usage;
								if(use!=TowerID){
									if((i&7)==d)OB->SearchVictim();
								}else{
									OB->SearchVictim();
								};
							}else{
								int use=OB->newMons->Usage;
								if(use==PeasantID){
									if((i&15)==d1)OB->SearchVictim();
								}else{
									OB->SearchVictim();
								};
							};
							int mm=i&31;
							if((tmtmt&31)==mm&&(OB->newMons->Capture||!(OB->Ready||OB->LocalOrder)))CheckCapture(OB);
							addrand(OB->RealX+OB->RealY);
							addname(OB->Index);
							Order1* OR1=OB->LocalOrder;
							//int orr=-1;
							//addrand(CheckNtIntegrity(i));
							if(OR1){
								//Order1 ORR=*OR1;
								addname(OB->Index);
								OR1->DoLink(OB);
								addrand(OB->LocalOrder!=NULL);
							}else{
								//addrand(OB->Index+40000);
								OB->PrioryLevel=0;
								if(OB->StandTime==48){
									addrand(OB->Index+50000);
									GotoFinePosition(OB);
								};
							};
							//addrand(CheckNtIntegrity(i));
							addname(OB->Index);
							//addrand(CheckNtIntegrity(i));
							if(OB->NewBuilding){
								if(OB->delay>=FrmDec)OB->delay-=FrmDec;
								else OB->delay=0;
								if(OB->TempDelay){
									int N=OB->newMons->NShotPt;
									for(int i=0;i<N;i++){
										if(OB->TempDelay[i]>FrmDec){
											OB->TempDelay[i]-=FrmDec;
										}else OB->TempDelay[i]=0;
									};
								};
							}else{
								switch(OB->newMons->MotionStyle){
								case 0:
									MotionHandler1(OB);
									addname(OB->Index);
									addrand(OB->Index+30000);
									break;
								case 1:
									MotionHandler3(OB);
									addrand(OB->Index+20000);
									break;
								case 2:
									//MotionHandler2(OB);
									PerformMotion2(OB);
									addrand(OB->Index+10000);
									break;
								case 3:
									{
										int T=OB->StandTime;
										if((T&15)==14)GotoFinePosition(OB);
										MotionHandler4(OB);
										addrand(OB->Index+5000);
									};
									break;
								case 4:
									MotionHandler5(OB);
									break;
								case 5:
									MotionHandlerOfNewSheeps(OB);
									break;
#ifdef COSSACKS2
								case 6:
									void MotionHandlerForComplexObjects(OneObject* OB);
									MotionHandlerForComplexObjects(OB);
									break;
								case 7:
									void MotionHandlerForSingleStepObjects(OneObject* OB);
									MotionHandlerForSingleStepObjects(OB);
									break;

#endif //COSSACKS2
								};
							};
							//addrand(CheckNtIntegrity(i));
							if(LIGHT){
								if(sce){
									if(OB->NNUM==MyNT)UnitLight(OB);
								}else if(OB->NMask&Mask)UnitLight(OB);
							};
							NewMonster* NM=OB->newMons;
							if(!NM->SlowRecharge){
								if(OB->delay>=FrmDec)OB->delay-=FrmDec;
								else OB->delay=0;
							}else{
								if(OB->delay){
									if(OB->StandTime>20&&!OB->LocalOrder){
										int cf=OB->NewCurSprite>>SpeedSh;
										int nf=(OB->NewAnm->NFrames-1)>>SpeedSh;
										if(nf==cf){
											if(OB->NewState!=4){
												OB->NewState=4;
												TryToStand(OB,0);
											}else{
												NewAnimation* ANM=&NM->Attack[3];
												if(OB->NewAnm!=ANM){
													if(OB->delay>ANM->NFrames)OB->delay-=ANM->NFrames;
													else OB->delay=0;
													OB->NewAnm=ANM;
													OB->NewCurSprite=0;
													OB->InMotion=0;
													OB->BackMotion=0;
												};
											};
										};
									};
								}else{
									if(OB->NewState==4){
										OB->NewState=OB->GroundState;
									};
								};
							};
							//addrand(CheckNtIntegrity(i));
							word HARCH=OB->Nat->Harch;
							if(!(HARCH||OB->NewBuilding||rando()>HUNGPROB)){
								if(!OB->newMons->NotHungry){
									if(OB->NNUM==MyNation)AssignHint1(VERYHUN,50,32);//"Очень кушать хочется. Охх...."
									OB->Die();
									OB->Nat->CITY->LastGolodTime=tmtmt;
								};
							};
							if(OB->NewBuilding){
								//if(OB->Weap)OB->SearchVictim();
								//if(OB->DstX&&OB->Selected&&OB->NNUM==MyNation)AddDestn(OB->DstX>>4,OB->DstY>>4);
								//if(OB->LocalOrder)OB->LocalOrder->DoLink(OB);else OB->PrioryLevel=0;
								int HLife=OB->MaxLife>>1;
								if(OB->Life<HLife){
									
									bool DoFog=false;
									HLife>>=1;
									int Prob=NM->Fogging.WProb;
									if(OB->Life<HLife){
										if(rando()<Prob)DoFog=true;
									}else{
										Prob>>=1;
										if(rando()<Prob)DoFog=true;
									};
									if(DoFog&&NM->Fogging.NWeap&&OB->Stage==OB->NStages){
										Prob=(rando()*NM->Fogging.NWeap)>>15;
										Weapon* WP=WPLIST[NM->Fogging.Weap[Prob]];
										Prob=(rando()*NM->NDamPt)>>15;
										int xx,yy;
										OB->GetCornerXY(&xx,&yy);
										if(NM->NDamPt){
											xx+=NM->DamPtX[Prob];
											yy+=NM->DamPtY[Prob];
										};
										xx<<=4;
										yy<<=4;
										Create3DAnmObject(WP,xx,yy,GetHeight(xx,yy)+4,xx,yy,-1000,NULL);
									};
								};
							};
							if(OB->DstX&&OB->ImSelected&GM(MyNation))AddDestn(OB->DstX>>4,OB->DstY>>4);
							//addrand(CheckNtIntegrity(i));
						}else if(!OB->Hidden){
							//death processing
							NewMonster* NMN=OB->newMons;
							if(OB->NewBuilding){
								OB->Sdoxlo++;
								if(OB->Sdoxlo>1000){
									OB->LoLayer=&NMN->DeathLie2;
								};
								if(OB->Sdoxlo>2000){
									EliminateBuilding(OB);
									DelObject(OB);
									Group[OB->Index]=NULL;
								};
							}else
							if(NMN->Death.Enabled){
								if(OB->Sdoxlo==1){
									OB->NewAnm=&NMN->Death;
									OB->NewCurSprite=0;
								};
								if(OB->NewAnm==&NMN->Death){
									OB->NewCurSprite+=FrmDec;
									if(OB->NewCurSprite>=NMN->Death.NFrames-FrmDec){
										if(OB->newMons->MeatTransformIndex!=0xFF){
											addSpriteAnyway(OB->RealX>>4,OB->RealY>>4,&SPECIAL,OB->newMons->MeatTransformIndex);
											if(SpriteSuccess){
												OneSprite* OS=Sprites+LastAddSpr;
												if(OS->Enabled){
													OS->NIndex=OB->NIndex;
													OS->Direction=OB->RealDir;
												};
											};
											OB->DeletePath();
											OB->ClearOrders();
											DelObject(OB);
											Group[OB->Index]=NULL;
											OB=NULL;//died.	
										}else
										if(NMN->DeathLie1.Enabled){
											OB->NewAnm=&NMN->DeathLie1;
											OB->NewCurSprite=0;
										}else{
											OB->DeletePath();
											OB->ClearOrders();
											DelObject(OB);
											Group[OB->Index]=NULL;
											OB=NULL;//died.	
										};
									};
								}else{
#ifdef _USE3D
									OB->Sdoxlo+=FrmDec;
									if(OB->Sdoxlo>24000){
#else									
									OB->Sdoxlo+=32;
#ifdef DEADTIMER
									if(OB->Sdoxlo>48000){
#else
                                    if (OB->Sdoxlo > 1200) {
#endif
#endif
										if(NMN->DeathLie2.Enabled){
											OB->NewAnm=&NMN->DeathLie2;
											OB->NewCurSprite=0;
										}else{
											//OneObject OBBX=*OB;
											OB->DeletePath();
											OB->ClearOrders();
											DelObject(OB);
											Group[OB->Index]=NULL;
											OB=NULL;//died.	
										};
									};
#ifndef _USE3D
#ifdef DEADTIMER
                                    if (OB && OB->Sdoxlo > 64000) {
#else
                                    if (OB && OB->Sdoxlo > 1600) {
#endif
										if(NMN->DeathLie2.Enabled){
											OB->NewAnm=&NMN->DeathLie3;
											OB->NewCurSprite=0;
										}else{
											OB->DeletePath();
											OB->ClearOrders();
											DelObject(OB);
											Group[OB->Index]=NULL;
											OB=NULL;//died.	
										};
									};
#ifdef DEADTIMER
                                    if (OB && OB->Sdoxlo > 80000) {
#else
                                    if (OB && OB->Sdoxlo > 2000) {
#endif
										OB->DeletePath();
										OB->ClearOrders();
										DelObject(OB);
										Group[OB->Index]=NULL;
										OB=NULL;//died.	
									};
#endif
								};
								if(OB)OB->Sdoxlo++;
							};
						};
					};
				};				
			};
			CloseGates(i);
		};
	};
//	StopPF(11);
	//RunPF(12);
	DoLink_Time=GetTickCount()-T0;
	T0=GetTickCount();
//	StopPF(12);
	CheckCapture_Time=GetTickCount()-T0;
	addrand(3321);
};
void CheckArmies(City*);

void addSpriteAnyway(int x,int y,SprGroup* SG,word id);
extern SprGroup SPECIAL;
extern bool SpriteSuccess;
extern int LastAddSpr;
extern byte BaloonState;
extern int FogMode;
void CalculateMotion(){
	
	//RunPF(0);
	LongProcesses();
//	StopPF(0);
	//RunPF(1);
	CheckUnits();
//	StopPF(1);
	//RunPF(2);
	
	
//	StopPF(2);
};

void CalculateMotionV2(){
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->NewMonst){
			OB->ForceX=OB->NextForceX;
			OB->ForceY=OB->NextForceY;
			OB->NextForceX=0;
			OB->NextForceY=0;
			if(OB->Sdoxlo){
				//death processing
				NewMonster* NMN=OB->newMons;
				if(NMN->Death.Enabled){
					if(OB->NewAnm==&NMN->Death){
						OB->NewCurSprite+=FrmDec;
					};
					if(OB->Sdoxlo<3&&OB->NewAnm!=&NMN->Death){
						OB->NewAnm=&NMN->Death;
						OB->NewCurSprite=0;
						OB->InMotion=false;
						OB->BackMotion=false;
					}else{
						if(OB->Sdoxlo>3&&OB->NewCurSprite>=NMN->Death.NFrames-FrmDec){
							//finally dead
							if(OB->newMons->MeatTransformIndex!=0xFF){
								addSpriteAnyway(OB->RealX>>4,OB->RealY>>4,&SPECIAL,OB->newMons->MeatTransformIndex);
								if(SpriteSuccess){
									OneSprite* OS=Sprites+LastAddSpr;
									if(OS->Enabled){
										OS->NIndex=OB->NIndex;
										OS->Direction=OB->RealDir;
									};
								};
								if(OB->PathX)free(OB->PathX);
								if(OB->PathY)free(OB->PathY);
								Group[OB->Index]=NULL;
								free(OB);
								OB=NULL;
							}else{
#ifdef DEADTIMER
								if(OB->Sdoxlo<40000)OB->NewAnm=&NMN->DeathLie1;
								else if(OB->Sdoxlo<120000)OB->NewAnm=&NMN->DeathLie2;
								else if(OB->Sdoxlo<240000)OB->NewAnm=&NMN->DeathLie3;
#else
                                if (OB->Sdoxlo < 100)OB->NewAnm = &NMN->DeathLie1;
                                else if (OB->Sdoxlo < 300)OB->NewAnm = &NMN->DeathLie2;
                                else if (OB->Sdoxlo < 600)OB->NewAnm = &NMN->DeathLie3;
#endif
								else{
									//erasing of the monster
									if(OB->PathX)free(OB->PathX);
									if(OB->PathY)free(OB->PathY);
									Group[OB->Index]=NULL;
									free(OB);
									OB=NULL;
								};
								if(OB)OB->NewCurSprite=0;
							};
						};
					};
					OB->Sdoxlo++;
				};
			};
		}else{
			//erasing of the monster
			//if(OB->PathX)free(OB->PathX);
			//if(OB->PathY)free(OB->PathY);
			//Group[OB->Index]=NULL;
			//free(OB);
			//OB=NULL;
		};
	};
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i]; 
		if(OB&&OB->NewMonst&&!OB->Sdoxlo){
			NewMonster* NMN=OB->newMons;
			if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
				int FX=0;
				int FY=0;
				OB->BestNX=OB->RealX;
				OB->BestNY=OB->RealY;
				if(OB->DestX>0){
					//int uux=(OB->DestX-(OB->Lx<<7))>>8;
					//int uuy=(OB->DestY-(OB->Ly<<7))>>8;
					int dst=Norma(OB->RealX-OB->DestX,OB->RealY-OB->DestY);
					if(dst>NMN->StopDistance){
						FX=div((OB->DestX-OB->RealX)<<8,dst).quot;
						FY=div((OB->DestY-OB->RealY)<<8,dst).quot;
					}else OB->DestX=-100;
				};
				FX+=OB->ForceX;
				FY+=OB->ForceY;
				bool Try1=false;
uuuu:
				if(FX||FY){
					char BestDir=char(GetDir(FX,FY));
					//need optimisation for* !:
					int bdx=NMN->MotionL.NFrames*NMN->OneStepDX[byte(BestDir)];
					int bdy=NMN->MotionL.NFrames*NMN->OneStepDY[byte(BestDir)];
					if(!CheckTerra(OB->RealX+bdx,OB->RealY+bdy,OB->Lx,OB->LockType)){
						if((!Try1)&&(OB->PathX||OB->DestX<0)){
							int OFX=FX;
							int OFY=FY;
							int xs0=(OB->RealX-(OB->Lx<<7))>>8;
							int ys0=(OB->RealY-(OB->Ly<<7))>>8;
							int xs1=(OB->RealX+bdx-(OB->Lx<<7))>>8;
							int ys1=(OB->RealY+bdy-(OB->Lx<<7))>>8;
							if(CheckBar(xs0,ys1,OB->Lx,OB->Lx))FY=0;
							if(CheckBar(xs1,ys0,OB->Lx,OB->Lx))FX=0;
							if(FX&&FY&&OB->DestX>0){
								if(abs(FX)>abs(FY))FY=0;
								else FX=0;
							};
							if(!(FX||FY)){
							//	FX=-OFX;
							//	FY=-OFY;
							};
							Try1=true;
							goto uuuu;
						};
					};
					char ddir=OB->RealDir-BestDir;
					if(abs(ddir)<MinRot){
						OB->RealDir=BestDir;
						OB->GraphDir=OB->RealDir;
						int trmv=TryToMove(OB,OB->RealDir,false);
						if(!trmv){
							int bdx=NMN->MotionL.NFrames*NMN->OneStepDX[OB->RealDir];
							int bdy=NMN->MotionL.NFrames*NMN->OneStepDY[OB->RealDir];
							PushMonsters(OB->RealX,OB->RealY,OB->RealX,OB->RealY,OB->Radius2,OB->Index);
                            OB->NextForceX+=FORCEX;
                            OB->NextForceY+=FORCEY;
							TryToStand(OB,false);
							/*
							if(!TryToMove(OB,OB->RealDir-16)){
								if(!TryToMove(OB,OB->RealDir+16)){
									int bdx=NMN->MotionL.NFrames*NMN->OneStepDX[OB->RealDir];
									int bdy=NMN->MotionL.NFrames*NMN->OneStepDY[OB->RealDir];
									PushMonsters(OB->RealX+bdx,OB->RealY+bdy,OB->Radius2,OB->Index);
									OB->InMotion=false;
									OB->NewCurSprite=0;
									OB->NewAnm=&NMN->Stand;
								};
							};*/
						}else{
							if(trmv==-1){
								if(OB->DestX>0){
									if(OB->PathX&&OB->NIPoints){
										OB->CreatePrePath(OB->PathX[OB->NIPoints-1],OB->PathY[OB->NIPoints-1]);
										TryToStand(OB,false);
										OB->PathDelay=32+(rando()&7);
									}else{
										addrand(23);
										CreateFullPath((OB->DestX-(OB->Lx<<7))>>8,(OB->DestY-(OB->Lx<<7))>>8,OB);
										TryToStand(OB,false);									
									};
								}else{
									TryToStand(OB,false);
								};
							};
						};
						OB->RotCntr=0;
					}else{
						if(ddir>0){
							RotateMon(OB,-MinRot);//OB->RealDir-=MinRot;
							if(OB->RotCntr>1){
								if(TryToMove(OB,OB->RealDir,false)!=1){
									TryToStand(OB,false);
								}else{
									OB->InMotion=true;
									OB->NewCurSprite=0;
									if(OB->LeftLeg)OB->NewAnm=&NMN->MotionL;
									else OB->NewAnm=&NMN->MotionR;
									OB->LeftLeg=!OB->LeftLeg;
									//OB->NewAnm=&NMN->MotionL;
									OB->RotCntr=0;
								};
							}else{
								TryToStand(OB,false);
							};
						}else{
							RotateMon(OB,MinRot);//OB->RealDir+=MinRot;
							if(OB->RotCntr>1){
								if(TryToMove(OB,OB->RealDir,false)!=1){
									TryToStand(OB,false);
								}else{
									OB->InMotion=true;
									OB->NewCurSprite=0;
									if(OB->LeftLeg)OB->NewAnm=&NMN->MotionL;
									else OB->NewAnm=&NMN->MotionR;
									OB->LeftLeg=!OB->LeftLeg;
									//OB->NewAnm=&NMN->MotionR;
									OB->RotCntr=0;
								};
							}else{
								TryToStand(OB,false);
							};
						};	
					};
				}else{
					TryToStand(OB,true);
				};
			}else{
				if(OB->InMotion){
					OB->UnBlockUnit();
					OB->RealX+=NMN->OneStepDX[OB->RealDir];
					OB->RealY+=NMN->OneStepDY[OB->RealDir];
				};
				OB->NewCurSprite+=FrmDec;
				APPLY_TIRING(OB);
			};
			OB->x=(OB->RealX-(OB->Lx<<7))>>8;
			OB->y=(OB->RealY-(OB->Lx<<7))>>8;
			OB->RZ=GetHeight((OB->RealX-(OB->Lx<<7))>>4,(OB->RealY-(OB->Lx<<7))>>4);
			if(OB->LocalOrder){
				OB->LocalOrder->DoLink(OB);
			};
		};
	};
};
//--------------------------------------------------------------//
int RoundX(int x){
	return (x>>8)<<8;
};
int RoundY(int y){
	return (div(y,8*16).quot)*(8*16);
};
void BSetPt(int x,int y);
void CreateAveragePlane(int x,int y,int r);
extern word LastObject;
bool CheckSpritesInArea(int x,int y,int r);
bool CheckBuildingsInArea(int x0,int y0,int x1,int y1,word* BLD,int Nb,bool Mine){
	if(x0==x1&&y0==y1)return true;
	int nx0=x0+y0;
	int ny0=y0-x0;
	int nx1=x1+y1;
	int ny1=y1-x1;
	int nxc=(nx0+nx1)>>1;
	int nyc=(ny0+ny1)>>1;
	int Rx=abs(nx1-nx0)>>1;
	int Ry=abs(ny1-ny0)>>1;
	int xc=(x0+x1)>>1;
	int yc=(y0+y1)>>1;
	if(Mine){
		for(int i=0;i<Nb;i++){
			OneObject* OB=Group[BLD[i]];
			if(OB){
				NewMonster* NM=OB->newMons;
				if(NM->Building&&NM->ProdType){
					if(Norma(OB->RealX-xc,OB->RealY-yc)<16*120)return false;
				};
			};
		};
	}else{
		for(int i=0;i<Nb;i++){
			OneObject* OB=Group[BLD[i]];
			if(OB){
				NewMonster* NM=OB->newMons;
				if(NM->Building){
					int bx0=OB->RealX+(NM->BuildX0<<4);
					int by0=OB->RealY+(NM->BuildY0<<4);
					int bx1=OB->RealX+(NM->BuildX1<<4);
					int by1=OB->RealY+(NM->BuildY1<<4);
					if(bx0!=bx1||by0!=by1){
						int bnx0=bx0+by0;
						int bny0=by0-bx0;
						int bnx1=bx1+by1;
						int bny1=by1-bx1;
						int bnxc=(bnx0+bnx1)>>1;
						int bnyc=(bny0+bny1)>>1;
						int bRx=abs(bnx1-bnx0)>>1;
						int bRy=abs(bny1-bny0)>>1;
						if(abs(bnxc-nxc)<bRx+Rx&&abs(bnyc-nyc)<bRy+Ry)return false;
					};
				};
			};
		};
	};
	return true;
};
bool CheckBuildingsForWalls(int x,int y){
	int nr=4;
    int nr1=nr+nr+1;
    int cx=(x>>11);
	int cy=(y>>11);

	int nxc=x+y;
	int nyc=y-x;

	int ofs0=(cx-nr)+((cy-nr)<<VAL_SHFCX)+VAL_MAXCX+1;
	int ofs1=ofs0<<SHFCELL;
	for(int dy=0;dy<nr1;dy++){
		for(int dx=0;dx<nr1;dx++){
            if(ofs0>=0&&ofs0<MAXCIOFS){
			    word Mon1=BLDList[ofs0];
				if(Mon1!=0xFFFF){
					OneObject* OB=Group[Mon1];
					if(OB){
						NewMonster* NM=OB->newMons;
						if(NM->Building){
							int bx0=OB->RealX+(NM->BuildX0<<4);
							int by0=OB->RealY+(NM->BuildY0<<4);
							int bx1=OB->RealX+(NM->BuildX1<<4);
							int by1=OB->RealY+(NM->BuildY1<<4);
							if(bx0!=bx1||by0!=by1){
								int bnx0=bx0+by0;
								int bny0=by0-bx0;
								int bnx1=bx1+by1;
								int bny1=by1-bx1;
								int bnxc=(bnx0+bnx1)>>1;
								int bnyc=(bny0+bny1)>>1;
								int bRx=abs(bnx1-bnx0)>>1;
								int bRy=abs(bny1-bny0)>>1;
								if(abs(bnxc-nxc)<bRx+2048&&abs(bnyc-nyc)<bRy+2048)return false;
							};
						};
					};

				};
            };
			ofs0++;
			ofs1+=MAXINCELL;
		};
		ofs0+=VAL_MAXCX-nr1;
		ofs1+=(VAL_MAXCX-nr1)<<SHFCELL;
	};
    return true;
};
bool CheckMonstersInArea(byte Mask,int x,int y,int r){
    int nr=(r>>11)+1;
    int nr1=nr+nr+1;
    int cx=(x>>11);
	int cy=(y>>11);
	int ofs0=(cx-nr)+((cy-nr)<<VAL_SHFCX);
	int ofs1=ofs0<<SHFCELL;
	for(int dy=0;dy<nr1;dy++){
		for(int dx=0;dx<nr1;dx++){
            if(ofs0>=0&&ofs0<MAXCIOFS){
			    int NMon=MCount[ofs0];
		     	for(int z=0;z<NMon;z++){
		    		word MD=GetNMSL(z+ofs1);
	    			OneObject* OB=Group[MD];
	    			if(OB&&OB->NewMonst&&(!(OB->NMask&Mask||OB->NNUM==7))&&Norma(OB->RealX-x,OB->RealY-y)<r)return false;
		    	};
            };
			ofs0++;
			ofs1+=MAXINCELL;
		};
		ofs0+=VAL_MAXCX-nr1;
		ofs1+=(VAL_MAXCX-nr1)<<SHFCELL;
	};
    return true;
};
void FindPositionForUnit(OneObject* OB){
	MotionField* MF=MFIELDS+OB->LockType;
	if(OB&&MF->CheckBar(OB->x-1,OB->y-1,3,3)){
		int x0=OB->x;
		int y0=OB->y;
		for(int rr=0;rr<60;rr++){
			char* xi=Rarr[rr].xi;
			char* yi=Rarr[rr].yi;
			int N=Rarr[rr].N;
			for(int j=0;j<N;j++){
				int xx=x0+xi[j];
				int yy=y0+yi[j];
				if(!MF->CheckBar(xx-1,yy-1,3,3)){
					OB->RealX=(xx<<8)+128;
					OB->RealY=(yy<<8)+128;
					OB->x=xx;
					OB->y=yy;
					OB->DestX=-1;
					OB->DestY=-1;
					if(OB->PathX)free(OB->PathX);
					if(OB->PathY)free(OB->PathY);
					OB->PathX=NULL;
					OB->PathY=NULL;
					OB->NIPoints=0;
					rr=60;
					j=N;
					OB->CPdestX=-1;
					OB->CPdestY=-1;
				};
			};
		};
	};
};
void GetAwayMonstersInArea(int x,int y,int r){
	SetMonstersInCells();
    int nr=(r>>11)+1;
    int nr1=nr+nr+1;
    int cx=(x>>11);
	int cy=(y>>11);
	int ofs0=(cx-nr)+((cy-nr)<<VAL_SHFCX);
	int ofs1=ofs0<<SHFCELL;
	for(int dy=0;dy<nr1;dy++){
		for(int dx=0;dx<nr1;dx++){
            if(ofs0>=0&&ofs0<MAXCIOFS){
			    int NMon=MCount[ofs0];
		     	for(int z=0;z<NMon;z++){
		    		word MD=GetNMSL(z+ofs1);
	    			OneObject* OB=Group[MD];
					if(OB&&OB->NewMonst&&Norma(OB->RealX-x,OB->RealY-y)<r){
						if(CheckBar(OB->x-1,OB->y-1,3,3)){
							int x0=OB->x;
							int y0=OB->y;
							for(int rr=0;rr<60;rr++){
								char* xi=Rarr[rr].xi;
								char* yi=Rarr[rr].yi;
								int N=Rarr[rr].N;
								for(int j=0;j<N;j++){
									int xx=x0+xi[j];
									int yy=y0+yi[j];
									if(!CheckBar(xx-1,yy-1,3,3)){
										OB->RealX=(xx<<8)+128;
										OB->RealY=(yy<<8)+128;
										OB->x=xx;
										OB->y=yy;
										OB->DestX=-1;
										OB->DestY=-1;
										if(OB->PathX)free(OB->PathX);
										if(OB->PathY)free(OB->PathY);
										OB->PathX=NULL;
										OB->PathY=NULL;
										OB->NIPoints=0;
										rr=60;
										j=N;
										OB->CPdestX=-1;
										OB->CPdestY=-1;
									};
								};
							};
						};
					};
		    	};
            };
			ofs0++;
			ofs1+=MAXINCELL;
		};
		ofs0+=VAL_MAXCX-nr1;
		ofs1+=(VAL_MAXCX-nr1)<<SHFCELL;
	};
};
int CheckMinePosition(NewMonster* NM,int* xi,int* yi,int r);
bool FindPortPlace(NewMonster* NM,int x,int y,int* BuiX,int* BuiY);
int CheckVLine(int x,int y,int Lx);
int CheckHLine(int x,int y,int Lx);
bool CheckSpritesInAreaNew(int x,int y,int r,bool Erase);
void EraseTreesInPoint(int x,int y);
int CreateBLDList(byte NI,word* BLD,int MaxBLD,int x0,int y0){
	int N=0;
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->NewBuilding){
			if(N<MaxBLD){
				BLD[N]=i;
				N++;
			};
		};
	};
	return N;
};
int CheckCreationAbility(byte NI,NewMonster* NM,int* x2i,int* y2i,word* BLD,int NBLD){
	word TotBLD[512];
	if(NM->Building&&NI!=0xFF&&!BLD){
		NBLD=CreateBLDList(NI,TotBLD,512,*x2i,*y2i);
		BLD=TotBLD;
	};
	int NBLD1=NBLD;
	byte Use=NM->Usage;
	if(NI!=0xFF&&NATIONS[NI].AI_Enabled&&Use!=SkladID)NBLD1=0;
	int x2=*x2i;
	int y2=*y2i;
	int xs=x2>>9;
	int ys=y2>>9;
	if(xs<=0||ys<=0||xs>=msx||ys>=msy)return -1;
	if(NI!=0xFF&&!CheckBuildPossibility(NI,x2>>4,y2>>4))return -1;
    if(NM->Building){
        int x,y,xm,ym;
        int ID=0;
	    int PicSX=NM->PicDx<<4;
	    int PicSY=Prop43(NM->PicDy)<<4;
	    xm=(x2+PicSX)>>8;
	    ym=(y2+PicSY)>>8;
        x=((xm+NM->CenterMX)<<8)+128;
        y=((ym+NM->CenterMY)<<8)+128;
        int r=(NM->BRadius+15)<<8;
		if(NM->ProdType){
			int xx=x2;
			int yy=y2;
            ID=CheckMinePosition(NM,&xx,&yy,r);
			*x2i=xx;
			*y2i=yy;
			x2=xx;
			y2=yy;
			xm=(x2+PicSX)>>8;
			ym=(y2+PicSY)>>8;
			x=((xm+NM->CenterMX)<<8)+128;
			y=((ym+NM->CenterMY)<<8)+128;
            if(ID==-1)return -1;
        }else{
			bool ETree=1;//(!NBLD1)||NM->StandAnywhere;
			for(int p=0;p<NBLD1;p++){
				OneObject* OB=Group[BLD[p]];
				if(OB&&OB->Ready&&Norma(OB->RealX-x,OB->RealY-y)<3000*16)ETree=true;
			};
			if(!CheckSpritesInAreaNew(x,y,r,ETree))return -1;
		};
        if(!CheckMonstersInArea(NATIONS[NI].NMask,x,y,r))return -1;
		if(!CheckBuildingsInArea(x+(NM->BuildX0<<4),y+(NM->BuildY0<<4),x+(NM->BuildX1<<4),y+(NM->BuildY1<<4),BLD,NBLD,NM->ProdType!=0))return -1;
        
        //checking locking information
        int np=NM->NCheckPt;
        byte* Cpx=NM->CheckX;
        byte* Cpy=NM->CheckY;
		int maxZ=-100000;
		int minZ=100000;
		if(NM->ProdType){
			int nn=0;
			//for(int i=0;i<np;i++){
			//	int xx=xm+Cpx[i];
			//	int yy=ym+Cpy[i];
			//	int z=GetHeight(xx<<4,yy<<4);
			//	if(z>maxZ)maxZ=z;
			//	if(z<minZ)minZ=z;
			//	if(CheckPt(xx,yy))nn++;
			//	if(nn>2)return -1;
			//};
		}else{
	        for(int i=0;i<np;i++){
				int xx=xm+Cpx[i];
				int yy=ym+Cpy[i];
				int z=GetHeight(xx<<4,yy<<4);
				if(z>maxZ)maxZ=z;
				if(z<minZ)minZ=z;
				if(CheckVLine(xx,yy-4,8))return -1;
				if(CheckHLine(xx-3,yy,6))return -1;
			};
        };
		if(NM->ProdType)maxZ=minZ;
		if(abs(maxZ-minZ)>50)return -1;
		if(NM->Port){
			if(!FindPortPlace(NM,x,y,&PortBuiX,&PortBuiY))return -1;
		};
        return ID;
    }else{
        int mLx=(NM->Radius2+3)>>7;
	    if(!mLx)mLx=1;
	    if(!CheckTerra(x2,y2,mLx,NM->LockType))return -1;
        if(CheckPosition(x2,y2,NM->Radius2+8,0xFFFF))return 0;
        else return -1;
    };
};
int CheckSmartCreationAbility(byte NI,NewMonster* NM,int* x2i,int* y2i){
	word TotBLD[512];
	int Nb=0;
	if(NM->Building)Nb=CreateBLDList(NI,TotBLD,512,*x2i,*y2i);
	int v=CheckCreationAbility(NI,NM,x2i,y2i,TotBLD,Nb);
	if(NM->Port)return v;
	if(v==-1){
		int x0=((*x2i)>>8)<<8;
		int y0=((*y2i)>>8)<<8;
		for(int r=1;r<10;r++){
			char* xi=Rarr[r].xi;
			char* yi=Rarr[r].yi;
			int N=Rarr[r].N;
			for(int j=0;j<N;j++){
				int xc=x0+(int(xi[j])<<8);
				int yc=y0+(int(yi[j])<<8);
				int xc0=xc;
				int yc0=yc;
				v=CheckCreationAbility(NI,NM,&xc,&yc,TotBLD,Nb);
				if(v!=-1){
					*x2i=xc;
					*y2i=yc;
					return v;
				};
			};
		};
		return -1;
	}else return v;
};
int CheckAISmartCreationAbility(byte NI,NewMonster* NM,int* x2i,int* y2i){
	word TotBLD[512];
	int Nb=0;
	if(NM->Building)Nb=CreateBLDList(NI,TotBLD,512,*x2i,*y2i);
	int v=CheckCreationAbility(NI,NM,x2i,y2i,TotBLD,Nb);
	if(NM->Port)return v;
	if(v==-1){
		int x0=((*x2i)>>8)<<8;
		int y0=((*y2i)>>8)<<8;
		for(int r=1;r<10;r++){
			char* xi=Rarr[r].xi;
			char* yi=Rarr[r].yi;
			int N=Rarr[r].N;
			for(int j=0;j<N;j++){
				int xc=x0+(int(xi[j])<<9);
				int yc=y0+(int(yi[j])<<9);
				v=CheckCreationAbility(NI,NM,&xc,&yc,TotBLD,Nb);
				if(v!=-1){
					*x2i=xc;
					*y2i=yc;
					return v;
				};
			};
		};
		return -1;
	}else return v;
};
int SmartCreationUnit(byte NI,int NIndex,int x,int y){
	int v=CheckAISmartCreationAbility(NI,NATIONS[NI].Mon[NIndex]->newMons,&x,&y);
	if(v!=-1){
		return NATIONS[NI].CreateNewMonsterAt(x,y,NIndex,false);
	}else return -1;
};

extern City CITY[8];
extern byte BaloonState;
extern byte CannonState;
extern byte XVIIIState;
int CV_time=-1;
int CV_Cost[8];
int CV_x=-1;
int CV_y=-1;
int CV_NIndex=-1;
int CV_NI=-1;
void GetUnitCost(byte NI,word NIndex,int* Cost,int x,int y){
	if(NI==CV_NI&&NIndex==CV_NIndex&&tmtmt==CV_time&&x==CV_x&&y==CV_y){
		memcpy(Cost,CV_Cost,8*4);
		return;
	};
	CV_NI=NI;
	CV_NIndex=NIndex;
	CV_x=x;
	CV_y=y;
	CV_time=tmtmt;

	Nation* NT=&NATIONS[NI];
	GeneralObject* GO=NT->Mon[NIndex];
	AdvCharacter* ADC=GO->MoreCharacter;
	NewMonster* NM=GO->newMons;
	for(int i=0;i<8;i++)Cost[i]=ADC->NeedRes[i];
	if(NM->CostPercent==100){
		memcpy(CV_Cost,Cost,8*4);
		return;
	};
	int NUNITS=CITY[NI].UnitAmount[NIndex];
	int Percent=NM->CostPercent;
	bool EXPENS=0;
	if(NM->Usage==PushkaID&&CannonState==2)EXPENS=1;
	int TIMES=1000;
	int LR=NM->LowCostRadius;
	if(Percent!=100){
		if(LR&&x>0&&y>0){
			//checking distance
			bool USUALCOST=0;
			int N=NtNUnits[NI];
			word* IDS=NatList[NI];
			for(int i=0;i<N;i++){
				word ID=IDS[i];
				OneObject* OB=Group[ID];
				if(OB&&OB->NIndex==NIndex&&!OB->Sdoxlo){
					int R=Norma((OB->RealX>>4)-x,(OB->RealY>>4)-y);
					if(R<LR){
						USUALCOST=1;
						break;
					};
				};
			};
			if(!USUALCOST)NUNITS=0;
		};
		for(int j=0;j<NUNITS;j++){
			if(TIMES<1000000){
				TIMES=(TIMES*Percent)/100;
				for(int i=0;i<8;i++){
					Cost[i]=div(Cost[i]*Percent,100).quot;
					if(Cost[i]<0)Cost[i]=10000000;
				};
			};
		};
	};
	if(EXPENS){
		for(int i=0;i<8;i++){
			Cost[i]*=10;
			if(Cost[i]<0)Cost[i]=10000000;
		};
	};
	memcpy(CV_Cost,Cost,8*4);
};
void GetUnitCost(byte NI,word NIndex,int* Cost,word Power){
	Nation* NT=&NATIONS[NI];
	GeneralObject* GO=NT->Mon[NIndex];
	AdvCharacter* ADC=GO->MoreCharacter;
	NewMonster* NM=GO->newMons;
	for(int i=0;i<8;i++)Cost[i]=ADC->NeedRes[i];
	if(NM->CostPercent==100)return;
	int NUNITS=CITY[NI].UnitAmount[NIndex];
	int Percent=NM->CostPercent;
	bool EXPENS=0;
	if(NM->Usage==PushkaID&&CannonState==2)EXPENS=1;
	int TIMES=1000;
	for(int j=0;j<Power;j++){
		if(TIMES<1000000){
			TIMES=(TIMES*Percent)/100;
			for(int i=0;i<8;i++){
				Cost[i]=(Cost[i]*Percent)/100;
				///if(EXPENS)Cost[i]*=10;
				if(Cost[i]<0)Cost[i]=10000000;
			};
		};
	};
	if(EXPENS){
		for(int i=0;i<8;i++){
			Cost[i]*=10;
			if(Cost[i]<0)Cost[i]=10000000;
		};
	};
};
extern int LastMx;
extern int LastMy;
bool CheckCostHint(byte NI,word NIndex){
	Nation* NT=&NATIONS[NI];
	GeneralObject* GO=NT->Mon[NIndex];
    AdvCharacter* ADC=GO->MoreCharacter;
	int Cost[8];
	GetUnitCost(NI,NIndex,Cost,LastMx,LastMy);
	char NRES=0;
	char CCC[128];
	CCC[0]=0;
    for(int i=0;i<8;i++){
        if(XRESRC(NI,i)<Cost[i]){
			if(CCC[0])strcat(CCC,",");
			strcat(CCC,RDS[i].Name);
			NRES++;
		};
	};
	if(CCC[0]){
		char CC1[200];
		if(NRES>1)sprintf(CC1,NETRESS,CCC);//"Not enough resources: %s"
		else sprintf(CC1,NETRES1,CCC);//"Not enough resource: %s"
		AssignHint1(CC1,70);
		return false;
	};
    return true;
};
bool CheckCost(byte NI,word NIndex,int x,int y){
	Nation* NT=&NATIONS[NI];
	GeneralObject* GO=NT->Mon[NIndex];
    AdvCharacter* ADC=GO->MoreCharacter;
	int Cost[8];
	GetUnitCost(NI,NIndex,Cost,x,y);
    for(int i=0;i<8;i++)
        if(XRESRC(NI,i)<Cost[i])return false;
    return true;
};
bool ApplyCost(byte NI,word NIndex,int x,int y){
	Nation* NT=&NATIONS[NI];
	GeneralObject* GO=NT->Mon[NIndex];
	AdvCharacter* ADC=GO->MoreCharacter;
	int Cost[8];
	GetUnitCost(NI,NIndex,Cost,x,y);
    if(!CheckCost(NI,NIndex,x,y))return false;
    for(int i=0;i<8;i++){
		AddXRESRC(NI,i,-Cost[i]);
		NT->ControlProduce(GO->Branch,i,-Cost[i]);
		if(GO->newMons->Building)NT->ResOnBuildings[i]+=Cost[i];
		else NT->ResOnUnits[i]+=Cost[i];
	};
    return true;
};

void GetDiscreta(int* x,int* y,int val){
	if(!val)return;
	int xx1=(*x)-val;
	int yy1=(*y);
	int p=(xx1+yy1)>>1;
	int q=(xx1-yy1)>>1;
	p=div(p+(val>>1),val).quot*val;
	q=div(q+(val>>1),val).quot*val;
	*x=p+q;
	*y=p-q;
};
extern MotionField UnitsField;
extern bool SubCost;
extern bool SpriteSuccess;
void FindUnitPosition(int* x,int *y,NewMonster* NM){
	int x0=*x;
	int y0=*y;
	int SH=9;
	if(NM->Usage==FastHorseID)SH++;
	for(int r=0;r<50;r++){
		char* xi=Rarr[r].xi;
		char* yi=Rarr[r].yi;
		int N=Rarr[r].N;
		if(rando()<16384){
			for(int j=0;j<N;j++){
				int x1=x0+(int(xi[j])<<SH);
				int y1=y0+(int(yi[j])<<SH);
				if(!CheckCreationAbility(0xFF,NM,&x1,&y1,NULL,0)){
					*x=x1;
					*y=y1;
					return;
				};
			};
		}else{
			for(int j=N-1;j>=0;j--){
				int x1=x0+(int(xi[j])<<SH);
				int y1=y0+(int(yi[j])<<SH);
				if(!CheckCreationAbility(0xFF,NM,&x1,&y1,NULL,0)){
					*x=x1;
					*y=y1;
					return;
				};
			};
		};
	};
};
void CHKS();
bool CheckSpritesInAreaFast(int x,int y,int r);
void CreateFields(byte NI,int x,int y,int n){
	Nation* NT=NATIONS+NI;
	NewMonster* NM=NT->Mon[n]->newMons;
	if(NM->ExField){
		int r0=NM->Radius1;
		int L=NM->MotionDist;
		int dr=NM->Radius2;
		for(int ix=-r0;ix<=r0;ix++)
			for(int iy=-r0;iy<=r0;iy++){
				int xx=x+((ix+iy)*L*16)+((int(rando())*dr)>>11);
				int yy=y+((ix-iy)*L*16)+((int(rando())*dr)>>11);
				//if(CheckSpritesInAreaFast(xx,yy,35*16)){
					NT->CreateNewMonsterAt(xx,yy,n,false);
				//};
			};
	}else{
		
		for(int ix=-3;ix<=3;ix++)
			for(int iy=-3;iy<=3;iy++){
				int xx=x+((ix+iy)*64*16);
				int yy=y+((ix-iy)*64*16);
				NT->CreateNewMonsterAt(xx,yy,n,false);
			};
	};
};
void SlowCreateFields(byte NI,int x,int y,int n){
	Nation* NT=NATIONS+NI;
	NewMonster* NM=NT->Mon[n]->newMons;
	if(NM->ExField){
		int r0=NM->Radius1;
		int L=NM->MotionDist;
		int dr=NM->Radius2;
		int RR=r0*L*16;
		for(int i=0;i<20;i++){
			int ix=((int(rando())*(RR+RR+1))>>15)-RR;
			int iy=((int(rando())*(RR+RR+1))>>15)-RR;
			int xx=x+(ix+iy);
			int yy=y+(ix-iy);
			NT->CreateNewMonsterAt(xx,yy,n,false);
		};

		/*
		for(int ix=-r0;ix<=r0;ix++)
			for(int iy=-r0;iy<=r0;iy++){
				int xx=x+((ix+iy)*L*16)+((int(rando())*dr)>>11);
				int yy=y+((ix-iy)*L*16)+((int(rando())*dr)>>11);
				if(rando()<80){
					NT->CreateNewMonsterAt(xx,yy,n,false);
				};
			};
		*/
	}else{
		for(int ix=-3;ix<=3;ix++)
			for(int iy=-3;iy<=3;iy++){
				int xx=x+((ix+iy)*64*16);
				int yy=y+((ix-iy)*64*16);
				if(rando()<80){
					NT->CreateNewMonsterAt(xx,yy,n,false);
				};
			};
	};
};
bool GroundBox=1;
extern byte NTex1[32];
extern byte NTex2[32];
void CreateGround(OneObject* G);
int GetTotalUnits(){
	int N=0;
	for(int i=0;i<MAXOBJECT;i++){
		if(Group[i])N++;
	};
	return N;
};
extern bool Loadingmap;
void CreateGrassGround(int x,int y);
void GenerateMiniMapSquare(int x0,int y0,int nx,int ny);
void GetAwayMonstersInArea(int x,int y,int r);
void RM_LoadNotObj(char* Name,int x,int y);
void RM_GetObjVector(char* Name,int* x,int* y);
extern bool ImmVis;
bool NoGround=0;
void WriteUnitAction(byte NI,word NIndex,int* R0,int* R1);
int Nation::CreateNewMonsterAt(int rx,int ry,int n,bool Anyway){
	addrand(rx);
	addrand(ry);
	addrand(n);
	addrand(Anyway);
	int maxx=(msx+2)<<(5+4);
	if(rx<1024||ry<1024||rx>maxx||ry>maxx)return -1;
	rando();
	int x=rx;
	int y=ry;
	int R0[6];
	int R1[6];
	for(int j=0;j<6;j++)R0[j]=XRESRC(NNUM,j);
	GeneralObject* GO=Mon[n];
	NewMonster* NM=GO->newMons;
	//if(!FindPortPlace(NM,x,y,&PortBuiX,&PortBuiY))return -1;
	AdvCharacter* ADC=GO->MoreCharacter;
	int ID=-1;
	if((!Anyway)){
		if((!CheckBuildPossibility(NNUM,rx>>4,ry>>4))||
        (!CheckCost(NNUM,n,rx>>4,ry>>4)))return -1;
		rando();
    };
	rando();
    if(NM->SpriteObject&&!NM->Wall){
		rando();
		ObjCharacter* OC=&COMPLEX.ObjChar[NM->Sprite];
		int sx=rx>>4;
		int sy=ry>>4;
		GetDiscreta(&sx,&sy,OC->Discret);
		//accurate check
		int cxx=sx>>4;
		int cyy=sy>>4;
		if(!CheckBar(cxx-4,cyy-3,8,15)){
			int H1=GetHeight(sx-64,sy);
			int H2=GetHeight(sx+64,sy);
			int H3=GetHeight(sx,sy-64);
			int H4=GetHeight(sx,sy+64);
			int Ha=(H1+H2+H3+H4)>>2;
			if(abs(H1-Ha)<40&&abs(H2-Ha)<32&&abs(H3-Ha)<32&&abs(H4-Ha)<32){
				int sid=NM->Sprite;
				if(NM->BRadius)sid+=rando()%NM->BRadius;
				addSprite(sx,sy,&COMPLEX,sid);
				if(SpriteSuccess){
					//RenewInfoMap(sx,sy);
					if(!Anyway)ApplyCost(NNUM,n,rx>>4,ry>>4);
					//CreateGrassGround(sx,sy);
				};
			};
		};
        return -1;
    };
	if(!(Anyway||NM->Wall)){
        ID=CheckCreationAbility(NNUM,GO->newMons,&x,&y,NULL,0);
        if(ID==-1)return -1;
		rando();
    };
	if(NM->Building&&!Anyway){
		ApplyCost(NNUM,n,rx>>4,ry>>4);
	};
	for(int j=0;j<6;j++)R1[j]=XRESRC(NNUM,j);
	if(NM->PieceName&&!NoGround){
		int x,y;
		RM_GetObjVector(NM->PieceName,&x,&y);
		ImmVis=1;
		RM_LoadNotObj(NM->PieceName,(rx>>4)-x,(ry>>4)-y);
		ImmVis=0;
	};
	if(NM->ResSubst){
		int xxx=x>>4;
		int yyy=y>>4;
		for(int i=0;i<MaxSprt;i++)if(Sprites[i].Enabled){
			OneSprite* OS=Sprites+i;
			if(OS->OC->IntResType<8&&Norma(OS->x-xxx,OS->y-yyy)<64){
				i=MaxSprt;
				byte rt=OS->OC->IntResType;
				if(NM->ProdType!=1<<rt){
					if(rt==IronID){
						n++;
					}else if(rt==CoalID){
						n+=2;
					}else if(rt==StoneID){
						n+=3;
					}else if(rt==FoodID){
						n+=4;
					}else if(rt==TreeID){
						n+=5;
					};
					GO=Mon[n];
					NM=GO->newMons;
					ADC=GO->MoreCharacter;
				};
			};
		};
	};
	int PicSX=NM->PicDx<<4;
	int PicSY=Prop43(NM->PicDy)<<4;
	if(NM->Building&&!NM->Wall){
		x=RoundX(rx+PicSX)-PicSX;
		y=RoundX(ry+PicSY)-PicSY;
	};
	int mLx=(GO->newMons->Radius2+3)>>7;
	//if(!mLx)mLx=1;
#ifdef COSSACKS2
	if(!(NM->LockType||NM->ComplexObjIndex!=0xFFFF))mLx=1;
#else
	if(!NM->LockType)mLx=1;
#endif
	//if(!CheckTerra(x,y,mLx))return -1;
	int i;
	for(i=0;i<MaxObj&&int(Group[i]);i++);
	if(i>=MaxObj)return -1;
	rando();
	if(n>=NMon) return -1;
	rando();
	if(i>=MAXOBJECT)MAXOBJECT=i+1;
	Group[i]=OBJECTS+i;
	memset(Group[i],0,sizeof OneObject);
	LastObject=i;
	//Cell8x8* CELL=&TCInf[NNUM][y>>11][x>>11];
	OneObject* G=Group[i];
	//AddOrderEffect(x>>9,y>>9,GO->BornSound);
	//CELL->UnitsAmount[GO->Kind]++;
	if(NM->Port){
		G->WallX=PortBuiX;
		G->WallY=PortBuiY;
		int LL=NM->BuiDist;
//		MFIELDS[1].BSetBar(PortBuiX-LL-1,PortBuiY-1,3);
//		MFIELDS[1].BSetBar(PortBuiX+LL-1,PortBuiY-1,3);
//		MFIELDS[1].BSetBar(PortBuiX-1,PortBuiY-LL-1,3);
//		MFIELDS[1].BSetBar(PortBuiX-1,PortBuiY+LL-1,3);
	};
	G->Usage=NM->Usage;
	if(G->Usage==ArcherID&&rando()<2600)G->Usage=GrenaderID;
	G->Guard=0xFFFF;
	G->GroundState=0;
	G->DoWalls=false;
	G->InPatrol=false;
	G->AddDamage=0;
	G->AddShield=0;
	G->StandGround=false;
	G->InArmy=false;
	G->Zombi=0;
	G->CurAIndex=0;
	G->MaxAIndex=NM->MaxAIndex;
	G->Ustage=0;
	G->NUstages=0;
	G->SingleUpgLevel=2;
	G->InternalLock=true;
	G->IFire=NULL;
	G->DefaultSettings(GO);
	G->AlwaysLock=false;
	G->LockType=NM->LockType;
	G->OverEarth=0;
	G->NewMonst=false;
	G->NewBuilding=false;
	G->Wall=false;
	G->NothingTime=0;
	G->Hidden=false;
	G->UnlimitedMotion=false;
	G->NoBuilder=false;
	G->DoNotCall=false;
	G->BackSteps=11;
	G->BackReserv=0;
	G->AddInside=0;
	G->PersonalDelay=100;
	G->BrigadeID=0xFFFF;
	G->BrIndex=0;
	G->AutoKill=false;
	G->StandGround=NM->AutoStandGround;
	G->NoSearchVictim=NM->AutoNoAttack;
#ifdef CONQUEST
	if(NM->CompxCraft){
		G->StageState=32768;
	};
	if(GO->newMons->NShotPt&&GO->newMons->ShotPtZ){
		G->TempDelay=znew(short,GO->newMons->NShotPt);
		memset(G->TempDelay,0,GO->newMons->NShotPt<<1);
	};
#endif
	if(GO->newMons->Building&&!NM->Wall)G->NewBuilding=true;
	else{
		if(NM->Wall)G->Wall=true;
		else G->NewMonst=true;
	};
	addrand(GO->newMons->Building);
	addrand(G->NewBuilding);
	addname(G->Index);
	//G->MadeSel=0;
	//G->MadeUnSel=0;
	G->ImSelected=0;
	G->NewState=0;
	G->LocalNewState=0;
	G->newMons=GO->newMons;
	G->RealX=x;
	G->RealY=y;
	G->DestX=-100;
	G->DestY=-100;
	G->RealVx=0;
	G->RealVy=0;
	G->RealDir=rando()&255;
	G->GraphDir=G->RealDir;
	G->Media=0;
	G->Nat=this;
	G->Ready=true;
	G->delay=0;
	G->MaxDelay=0;
	G->NearBase=0xFFFF;
	G->RStage=0;
	G->RType=0xFF;
	G->RAmount=0;
	G->NNUM=NNUM;
	G->NIndex=n;
	G->Index=i;
	G->BackMotion=0;
	Visuals* m;
	m=(Visuals*)Mon[n];
	G->Selected=false;
	G->Borg=false;
	G->Life=ADC->BirthLife;//ADC->Life;
	G->MaxLife=ADC->Life;
	if(NM->Nedorazvitoe)G->Life=NM->Life;
	G->Ref.Visual=m;
	//G->CrowdRef=NULL;
	G->LocalOrder=NULL;
	G->PrioryLevel=0;
	//G->InLineCom=NULL;
	//G->LineOffset=0;
	G->Sdoxlo=false;
	//G->Weap=Mon[n]->Weap;
	G->NMask=NMask;
	//G->Important=false;
	//G->EnemyDist=5000;
	G->Attack=false;
	G->EnemyID=0xFFFF;
	//G->Egoist=false;
	G->Height=0;
	G->SafeWall=false;
	G->NewAnm=&G->newMons->Stand;
	G->NewCurSprite=0;
	G->InMotion=false;
	G->RotCntr=0;
	G->BestNX=G->RealX;
	G->BestNY=G->RealY;
	G->NextForceX=0;
	G->NextForceY=0;
	G->LeftLeg=true;
	G->MotionDir=1;
	G->Radius1=GO->newMons->Radius1;
	G->Radius2=GO->newMons->Radius2;
	G->Lx=mLx;
	G->Ly=mLx;
	G->x=(G->RealX-(G->Lx<<7))>>8;
	G->y=(G->RealY-(G->Lx<<7))>>8;
	G->FireOwner=0xFF;
	//G->Weap=ADC->DamWeap[0];
	G->BlockInfo=0;
	G->AbRes=NM->ResConcentrator;
	G->Kills=0;
	G->NZalp=NM->MaxZalp;
	AddObject(G);
	if(!G->NewBuilding)CITY->Account+=NM->Ves;
	/*
	if(GO->newMons->PStand.Enabled){
		G->HasStatePA=true;
		G->NewState=0;
		G->LocalNewState=0;
	}else{
		G->HasStatePA=false;
		G->NewState=0;
		G->LocalNewState=0;
	};
	*/
	if(G->LockType==1&&!G->NewBuilding){
		int NewX=G->RealX;
		int NewY=G->RealY;
		int LX=G->Lx;
		int ULX=(LX)<<7;
		int NewLockX=(NewX-ULX)>>8;
		int NewLockY=(NewY-ULX)>>8;
		UnitsField.BSetBar(NewLockX,NewLockY,LX);
		G->LLock=true;
	}else{
		if(!(G->NewBuilding||G->Wall))UnitsField.BSetBar(G->x,G->y,G->Lx);
	};
	NProduced[G->NIndex]++;
	G->RZ=GetHeight(x>>4,y>>4);
	if(G->NewBuilding&&!NM->Wall){
		G->LoLayer=&GO->newMons->PMotionL[0];
		G->HiLayer=&GO->newMons->PMotionR[0];
		//CreateAveragePlane(x>>4,y>>4,GO->newMons->Radius2>>4);
		//G->RZ=GetHeight(x>>4,y>>4);
		G->Stage=0;
		if(NM->NBLockPt){
			int x0=(x+PicSX)>>8;
			int y0=(y+PicSY)>>8;
			for(i=0;i<NM->NBLockPt;i++){
				int xx=x0+NM->BLockX[i];
				int yy=y0+NM->BLockY[i];
				BSetPt(xx,yy);
				EraseTreesInPoint(xx,yy);
			};
			int xx1,yy1,xm,ym;
			int ID=0;
			int x2=rx;
			int y2=ry;
			xm=(x2+PicSX)>>8;
			ym=(y2+PicSY)>>8;
			xx1=((xm+NM->CenterMX)<<8)+128;
			yy1=((ym+NM->CenterMY)<<8)+128;
			int r=(NM->BRadius+2)<<8;
			GetAwayMonstersInArea(xx1,yy1,r);
		}else
		if(NM->NLockPt){
			int x0=(x+PicSX)>>8;
			int y0=(y+PicSY)>>8;
			for(int i=0;i<NM->NLockPt;i++){
				int xx=x0+NM->LockX[i];
				int yy=y0+NM->LockY[i];
				BSetPt(xx,yy);
				EraseTreesInPoint(xx,yy);
			};
		};
		G->Life=2;
        G->Ready=false;
        if(NM->ProdType){
            G->TakeResourceFromSprite(ID);
        };
	};
	if(NM->NBars){
		int bx0=(G->RealX>>4)+NM->PicDx;
		int by0=(G->RealY>>4)+(NM->PicDy<<1);
		int N=NM->NBars;
		for(i=0;i<N;i++){
			int pp=i*5;
			int XB0=NM->Bars3D[pp]+bx0;
			int YB0=(NM->Bars3D[pp+1]<<1)+by0;
			int L1=NM->Bars3D[pp+2];
			int L2=NM->Bars3D[pp+3];
			Add3DBar(XB0,YB0,XB0+L1+L2,YB0+L1-L2,NM->Bars3D[pp+4],G->Index,G->Index);
		};
	};
	NGidot++;
	if(G->NewBuilding){
		if(NM->NHideTri)SetTrianglesState(G->RealX>>4,((G->RealY-4)>>5)-G->RZ,NM->HideTriX,NM->HideTriY,NM->NHideTri,false);
		RenewInfoMap(G->RealX>>4,G->RealY>>4);
	}else CITY->RegisterNewUnit(G);
	//if(GroundBox)CreateGround(G);
	addname(G->Index);
	addrand(G->NoSearchVictim);
	if(!Loadingmap){
		CITY->EnumUnits();
		CITY->RefreshAbility();
	};
	WriteUnitAction(G->NNUM,G->NIndex,R0,R1);

#ifdef _USE3D
	if (NM->Monster3D) 
	{
		G->ExObj = new OneObjectEx( G );
	}
#endif // _USE3D
#ifdef NEWMORALE
	G->MaxMorale=G->Ref.General->MoreCharacter->StartMorale*10000;
	G->Morale=G->MaxMorale/2;
	if(G->Morale<33*10000)G->Morale=33*10000;
#endif //NEWMORALE
#ifdef GETTIRED
	G->GetTired=100*1000;
#endif //GETTIRED
#ifdef SIMPLEMANAGE
	if(NM->ArmAttack){
		G->ArmAttack=1;
		G->RifleAttack=1;
	};
#endif //SIMPLEMANAGE
#ifdef COSSACKS2
	G->CObjIndex=0xFFFF;
	if(NM->ComplexObjIndex!=0xFFFF){
		int CreateComplexObjectAt(int x,int y,word Type,byte NI,word Owner,byte StartDir);
		G->CObjIndex=CreateComplexObjectAt(x>>4,y>>4,NM->ComplexObjIndex,G->NNUM,G->Index,G->RealDir);
	};
	G->TotalPath=0;
	G->UnitSpeed=64;
	G->CurUnitSpeed=64;
#endif //COSSACKS2
	return G->Index;
};
void GenerateMiniMapSquare(int x0,int y0,int nx,int ny);
void CreateGround(OneObject* G){
	NewMonster* NM=G->newMons;
	if(NM->Building){
		byte Use=NM->Usage;
		if(Use==MelnicaID||Use==SkladID||Use==MineID)return;
		G->InFire=0;
		G->FireOwner=0xFF;
		GeneralObject* GO=G->Ref.General;
		byte tex1=NTex1[GO->NatID];
		byte tex2=NTex2[GO->NatID];
		int CX=G->RealX>>4;
		int CY=G->RealY>>4;
		int X0=CX+NM->BuildX0;
		int Y0=CY+NM->BuildY0;
		int X1=CX+NM->BuildX1;
		int Y1=CY+NM->BuildY1;
		int D=(Y1-Y0+X1-X0)>>1;
		int D2=D;
		int DD1=40;
		int DD2=0;
		if(!D){
			DD1=80;
			DD2=40;
		};
		if(tex2!=0xFF){
			NCurves=0;
			NTextures=1;
			TexList[0]=tex2;
			AddPointToCurve(X0-40,Y0,0,1);
			AddPointToCurve(X0+D,Y0+D2+40,0,1);
			AddPointToCurve(X1+40,Y1,0,1);
			AddPointToCurve(X1-D,Y1-D2-40,1,1);
		};
		if(tex1!=0xFF){
			NCurves=0;
			NTextures=1;
			TexList[0]=tex1;
			AddPointToCurve(X0,Y0,0,1);
			AddPointToCurve(X0+D,Y0+D2,0,1);
			AddPointToCurve(X1,Y1,0,1);
			AddPointToCurve(X1-D,Y1-D2,1,1);
		};
	};
};
void CreateGrassGround(int x,int y){
	y+=32;
	NCurves=0;
	NTextures=2;
	TexList[0]=7;
	TexList[1]=11;
	AddPointToCurve(x-100,y,0,1);
	AddPointToCurve(x,y-100,0,1);
	AddPointToCurve(x+100,y,0,1);
	AddPointToCurve(x,y+100,1,1);
};
void OneObject::GetCornerXY(int* x,int* y){
	NewMonster* NM=newMons;
	int PicSX=NM->PicDx<<4;
	int PicSY=Prop43(NM->PicDy)<<4;
	*x=(RealX+PicSX)>>8;
	*y=(RealY+PicSY)>>8;
};
void NewMonsterSendToLink(OneObject* OB);
bool ParkWaterNewMonster(OneObject* OB,int x,int y,byte Prio,byte OrdType);
#undef NewMonsterSendTo
void OneObject::NewMonsterSendTo(int x,int y,byte Prio,byte OrdType){
	bool OPT=OrdType>=128;
	OrdType&=127;
	NewMonster* NM=newMons;
	if(Ref.General->OFCR&&InArmy){
		return;
	};
	if(NM->Transport){
		bool CTR=CheckTransportOnParking(this,x>>8,y>>8);
		if(ParkWaterNewMonster(this,x,y,Prio,OrdType))return;
	};
	if(!NM->Transport){
		if(CheckOrderAbility())return;
	};
	if(NM->Building)return;
	if(PrioryLevel>Prio)return;
	Order1* Or1=CreateOrder(OrdType);
	if(!int(Or1))return;

	if(!(newMons->LockType==1||Sdoxlo||UnlimitedMotion||OPT||BlockInfo)){
		MotionField* MF=MFIELDS+newMons->LockType;
		if(MF->CheckPt(this->x,this->y)){
			FindPositionForUnit(this);			
		};
	};
	Or1->PrioryLevel=Prio&127;
	Or1->OrderType=12;
	Or1->OrderTime=0;
	Or1->DoLink=&NewMonsterSendToLink;
	Or1->info.MoveToXY.x=(x-((Lx)<<7))>>8;
	Or1->info.MoveToXY.y=(y-((Lx)<<7))>>8;
	Order1* LOR=LocalOrder;
	//LocalOrder=Or1;
	DestX=x;
	DestY=y;
	//PrioryLevel=Prio&127;
	UnBlockUnit();
	PrioryLevel=Or1->PrioryLevel;
	MakeOrderSound(this,2);
	StandTime=0;
	PathDelay=0;
	if(Or1->PrioryLevel==0)InPatrol=true;
};
#define NewMonsterSendTo(x,y,Prio,OrdType) NewMonsterSendTo(x,y,Prio,OrdType);addrand(33)
bool FindBestPosition(OneObject* OB,int* xd,int* yd,int R0);
void NewMonsterSendToLink(OneObject* OB){
	if(OB->PathDelay&&OB->StandTime>64){
		if(int(OB->LocalOrder)){
			OB->DeleteLastOrder();
			return;
		};
	};
	OB->PrioryLevel=OB->LocalOrder->PrioryLevel;
	OB->NewState=OB->GroundState;
	int xx1=OB->LocalOrder->info.MoveToXY.x;
	int yy1=OB->LocalOrder->info.MoveToXY.y;
	int Lx=OB->Lx;
	if((!OB->UnlimitedMotion)&&CheckBar(xx1-1,yy1-1,Lx+2,Lx+2)){
		if(FindBestPosition(OB,&xx1,&yy1,80)){
			OB->LocalOrder->info.MoveToXY.x=xx1;
			OB->LocalOrder->info.MoveToXY.y=yy1;
		};
	};
#ifdef COSSACKS2
	if(OB->DistTo(xx1,yy1)<=(OB->newMons->StopDistance>>8)){
#else
	if(OB->DistTo(xx1,yy1)<=(OB->newMons->StopDistance>>8)){
#endif
		if(int(OB->LocalOrder)){
			//OB->DeleteLastOrder();
			//if(OB->newMons->MotionStyle==3){
#ifdef COSSACKS2
				if(OB->newMons->MotionStyle==7||OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
#else
				if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
#endif
					if(OB->newMons->MotionStyle==3){
						OB->GroundState=0;
						OB->NewState=0;
					};
#ifdef COSSACKS2
					if(OB->newMons->MotionStyle!=7)TryToStand(OB,0);
#else
					TryToStand(OB,0);
#endif
					OB->DeleteLastOrder();
				};
			//}else{
			//	OB->DeleteLastOrder();
			//};
		};
	}else OB->CreatePath(xx1,yy1);
};
void NewMonsterPreciseSendToLink(OneObject* OB);
#undef NewMonsterPreciseSendTo
void OneObject::NewMonsterPreciseSendTo(int x,int y,byte Prio,byte OrdType){
	NewMonster* NM=newMons;
	if(NM->Transport){
		bool CTR=CheckTransportOnParking(this,x>>8,y>>8);
		if(ParkWaterNewMonster(this,x,y,Prio,OrdType))return;
	};
	if(!NM->Transport){
		if(CheckOrderAbility())return;
	};
	if(NM->Building)return;
	if(PrioryLevel>Prio)return;
#ifdef COSSACKS2
	void NewAttackPointLink(OneObject* OBJ);
	if(OrdType<2&&LocalOrder&&LocalOrder->DoLink==&NewAttackPointLink)OrdType=2;
#endif
	Order1* Or1=CreateOrder(OrdType);
	if(!int(Or1))return;
	if(!(newMons->LockType==1||Sdoxlo||UnlimitedMotion)){
		if(MFIELDS[newMons->LockType].CheckPt(this->x,this->y)){
			FindPositionForUnit(this);			
		};
	};
	Or1->PrioryLevel=Prio&127;
	Or1->OrderType=12;
	Or1->OrderTime=0;
	Or1->DoLink=&NewMonsterPreciseSendToLink;
	Or1->info.MoveToXY.x=x;
	Or1->info.MoveToXY.y=y;
	addrand(x);
	addrand(y);
	//assert(Or1->info.MoveToXY.x>1256);
	Order1* LOR=LocalOrder;
	//LocalOrder=Or1;
	DestX=x;
	DestY=y;
	//PrioryLevel=Prio&127;
	UnBlockUnit();
	//PrioryLevel=Or1->PrioryLevel;
	MakeOrderSound(this,2);
	StandTime=0;
	PathDelay=0;
	//assert(Or1->info.MoveToXY.x>1256);
	if(!(Prio&127)){
		if(newMons->PMotionL[0].Enabled&&!newMons->MaxAIndex)GroundState=1;
	}else{
		bool GS0=1;
		if(InArmy&&BrigadeID!=0xFFFF){
			Brigade* BR=Nat->CITY->Brigs+BrigadeID;
			if(BR->AttEnm)GS0=0;
		};
		if(GS0){
			GroundState=0;
			CheckState();
		};
	};
};
#define NewMonsterPreciseSendTo(x,y,Prio,OrdType) NewMonsterPreciseSendTo(x,y,Prio,OrdType);addrand(44)
void NewMonsterPreciseSendToLink(OneObject* OB){
	if(OB->PathDelay&&OB->StandTime>64){
		if(int(OB->LocalOrder)){
			OB->DeleteLastOrder();
			return;
		};
	};
	OB->PrioryLevel=OB->LocalOrder->PrioryLevel;
	OB->NewState=OB->GroundState;
	int xx2=OB->LocalOrder->info.MoveToXY.x;
	int yy2=OB->LocalOrder->info.MoveToXY.y;
	//assert(xx2>1256);
	int Lx=OB->Lx;
	int xx1=(xx2-(Lx<<7))>>8;
	int yy1=(yy2-(Lx<<7))>>8;
	addrand(xx1);
	addrand(xx2);
	//if((!OB->UnlimitedMotion)&&CheckBar(xx1-1,yy1-1,Lx+2,Lx+2)){
	//	if(FindBestPosition(OB,&xx1,&yy1,80)){
	//		OB->LocalOrder->info.MoveToXY.x=(xx1<<8)+128;
	//		OB->LocalOrder->info.MoveToXY.y=(yy1<<8)+128;
	//	};
	//};
	int dr=Norma(xx2-OB->RealX,yy2-OB->RealY);
	if(dr<1024){
		OB->DestX=xx2;
		OB->DestY=yy2;
	}else{
		OB->CreatePath(xx1,yy1);
#ifdef COSSACKS2
		int dr2=Norma(xx2-OB->DestX,yy2-OB->DestY);
		if(dr2<512){
			OB->DestX=xx2;
			OB->DestY=yy2;
		};
#endif
	};
	if(dr<120){
		OB->RealX=xx2;
		OB->RealY=yy2;
		if(xx1!=OB->x||yy1!=OB->y){
			UnitsField.BClrBar(OB->x,OB->y,Lx);
			//if(!UnitsField.CheckBar(xx1,yy1,Lx,Lx)){
			OB->x=xx1;
			OB->y=yy1;
			UnitsField.BSetBar(xx1,yy1,Lx);
		};
		TryToStand(OB,false);
		OB->DeleteLastOrder();
		OB->DestX=-1;
		OB->DestY=-1;
		return;
	};
};
void ProcessNewMonsters(){
	//RunPF(5);
	SetupSpot();
//	StopPF(5);
	//RunPF(4);
	SetMonstersInCells();
//	StopPF(4);
	//RunPF(14);
	CalculateMotion();
//	StopPF(14);
	return;
};
typedef  bool CHOBJ(OneObject* OB,int N);
void GetRect(OneObject* ZZ,int* x,int* y,int* Lx,int* Ly);
bool PInside(int x,int y,int x1,int y1,int xp,int yp);
word CheckCoorInGP(int x,int y);
word GoodSelectNewMonsters(byte NI,int xr,int yr,int xr1,int yr1,word *Collect,word* Ser,bool WRITE,CHOBJ* FN,int NN,int MAX,bool InForm,bool NotInForm){
	byte NIX=NatRefTBL[NI];
	int NSLC=0;
	if(abs(xr-xr1)<5&&abs(yr-yr1)<5){
		int xx=xr-(mapx<<5);
		int yy=yr-(mapy<<4);
		word IDX=CheckCoorInGP(xx>>(5-Shifter),yy>>(5-Shifter));
		if(IDX!=0xFFFF){
			OneObject* OB=Group[IDX];
			if(OB&&OB->newMons&&(!((OB->ImSelected&GM(NI))||OB->Hidden||OB->Sdoxlo||OB->UnlimitedMotion))&&OB->NNUM==NIX&&((!FN)||FN(OB,NN))){
				if((OB->BrigadeID!=0xFFFF&&InForm)||(OB->BrigadeID==0xFFFF&&NotInForm)){
					if(WRITE){
						Collect[NSLC]=OB->Index;
						Ser[NSLC]=OB->Serial;
						OB->ImSelected|=GM(NI);
					};
					NSLC++;
				};
			};
		};
		return NSLC;
	};

	int x,y,x1p,y1p,x1,y1;
	int xp=xr>>5;
	int yp=div24(yr);
	int xp1=xr1>>5;
	int yp1=div24(yr1);
	if(xp>xp1){
		x=xp1;
		x1=xp;
	}else{
		x1=xp1;
		x=xp;
	};
	if(yp>yp1){
		y=yp1;
		y1=yp;
	}else{
		y1=yp1;
		y=yp;
	};
	x1p=x1;
	y1p=y1;
	int mx=(msx>>2)-1;
	int my=(msy>>2)-1;
	int x0=(x>>2)-1;
	if(x0<0)x0=0;
	int y0=(y>>2)-1;
	if(y0<0)y0=0;
	if(x1&3)x1=(x1>>2)+2;
	else x1=(x1>>2)+1;
	if(y1&3)y1=(y1>>2)+2;
	else y1=(y1>>2)+1;
	if(x1>mx)x1=mx;
	if(y1>my)y1=my;
	//int mxm=(smaplx<<5)+64;
	//int mym=(smaply<<5)+64;
	x=xr;
	y=yr;
	x1p=xr1;
	y1p=yr1;
	bool UPresent=false;
	for(int ii=0;ii<MAXOBJECT;ii++){
		OneObject* OB=Group[ii];
		if(OB&&OB->newMons&&(!((OB->ImSelected&GM(NI))||OB->Hidden||OB->Sdoxlo||OB->UnlimitedMotion))&&OB->NNUM==NIX&&((!FN)||FN(OB,NN))){
			if((OB->BrigadeID!=0xFFFF&&InForm)||(OB->BrigadeID==0xFFFF&&NotInForm)){
				int xs,ys,xs1,ys1,LY;
				GetRect(OB,&xs,&ys,&xs1,&LY);
				ys1=ys+LY-1;
				xs1+=xs-1;
				if(PInside(x,y,x1p,y1p,xs,ys)||
				   PInside(x,y,x1p,y1p,xs1,ys)||
				   PInside(x,y,x1p,y1p,xs,ys1)||
				   PInside(x,y,x1p,y1p,xs1,ys1)||
				   PInside(xs,ys,xs1,ys1,x,y)||
				   PInside(xs,ys,xs1,ys1,x1p,y)||
				   PInside(xs,ys,xs1,ys1,x,y1p)||
				   PInside(xs,ys,xs1,ys1,x1p,y1p)){
					if(NSLC<MAX){
						if(WRITE){
							Collect[NSLC]=ii;
							Ser[NSLC]=OB->Serial;
							OB->ImSelected|=GM(NI);
						};
						NSLC++;
						if(!(OB->NewBuilding||OB->Wall))UPresent=true;
					}else assert(0);
				};
			};
		};
	};
	if(UPresent&&WRITE){
		for(int i=0;i<NSLC;i++){
			OneObject* OB=Group[Collect[i]];
			if(OB&&(OB->NewBuilding||OB->Wall)){
				Collect[i]=0xFFFF;
				OB->ImSelected&=~GM(NI);
			};
		};
	};
	return NSLC;
};
void DrawMarker(OneObject* OB);
void DrawMiniMarker(OneObject* OB);
void ShowNMHealth(OneObject* OB){
	if(MiniMode)DrawMiniMarker(OB);
	else DrawMarker(OB);
};
//-------------------------------------------------------//
//                                                       //
//                    ОБХОД ПРЕПЯТСТВИЙ                  //
//                                                       //
//-------------------------------------------------------//
//byte MapH[MAPSY*BMSX];//(x,y)-bit matrix(1point=16x12 pix)
//byte MapV[MAPSY*BMSX];//(y,x)-T bit matrix
const int Power2[24]={
		0x00000001,0x00000003,0x00000007,0x0000000F,
		0x0000001F,0x0000003F,0x0000007F,0x000000FF,
		0x000001FF,0x000003FF,0x000007FF,0x00000FFF,
		0x00001FFF,0x00003FFF,0x00007FFF,0x0000FFFF,
		0x0001FFFF,0x0003FFFF,0x0007FFFF,0x000FFFFF,
		0x001FFFFF,0x003FFFFF,0x007FFFFF,0x00FFFFFF};
void ClearMaps(){
	MFIELDS->ClearMaps();
	UnitsField.ClearMaps();
};
void BSetPt(int x,int y){
	MFIELDS->BSetPt(x,y);
};
void BClrPt(int x,int y){
	MFIELDS->BClrPt(x,y);
};

void BSetBar(int x,int y,int Lx){
	MFIELDS->BSetBar(x,y,Lx);
};
void BClrBar(int x,int y,int Lx){
	MFIELDS->BClrBar(x,y,Lx);
};
void BXorBar(int x,int y,int Lx){
	MFIELDS->BXorBar(x,y,Lx);
};
void BSetSQ(int x,int y,int Lx,int Ly){
	MFIELDS->BSetSQ(x,y,Lx,Ly);
};
void BClrSQ(int x,int y,int Lx,int Ly){
	MFIELDS->BClrSQ(x,y,Lx,Ly);
};
int CheckPt(int x,int y){
	return MFIELDS->CheckPt(x,y);
};
int CheckHLine(int x,int y,int Lx){
	return MFIELDS->CheckHLine(x,y,Lx);
};
int CheckVLine(int x,int y,int Lx){
	return MFIELDS->CheckVLine(x,y,Lx);
};
bool CheckBar(int x,int y,int Lx,int Ly){
	return MFIELDS->CheckBar(x,y,Lx,Ly);
};
byte NewCirc[16];

void SetLock(int x,int y,char val){
	if(val){
		MFIELDS->BSetBar(x<<1,y<<1,2);
	}else{
		MFIELDS->BClrBar(x<<1,y<<1,2);
	};
};
//------------------------------------------------//
//          KERNEL OF THE MOTION ENGINE           //
//            Search for the best way             //
//------------------------------------------------//
#define MaxP (4096)
word pxx[MaxP+160];
word pyy[MaxP+160];
word RVIS[MaxP+160];
word LVIS[MaxP+160];
int GetLAngle(int dx,int dy,int Angle);
int GetRAngle(int dx,int dy,int Angle);
//const int ndrr[9]={7,6,5,0,0,4,1,2,3};
//const nidrx[8]={0,1,1,1,0,-1,-1,-1};
//const nidry[8]={-1,-1,0,1,1,1,0,-1};



int div24(int y){
	return y>>4;
};
int Prop43(int y){
	//return div(y<<2,3).quot;
	return y<<1;
};
void CBar(int x,int y,int Lx,int Ly,byte c);
extern byte LockGrid;
extern bool VHMode;
void TopShow();
void LShow(){
	TopShow();

	if(LMode)return;	
	if(!LockGrid)return;
	int ox=mapx<<5;
	int oy=mapy<<5;
	
	MotionField* MFI=MFIELDS+LockGrid-1;
	
	/*
	if(LockGrid==1)MFI=&UnitsField;
	else MFI=&MFIELDS[LockGrid-2];
	*/

	OpenGates(MyNation);
	for(int ix=0;ix<smaplx<<1;ix++){
		int iy=0;
		bool nydone=true;
		do{
			int xx0=mapx+mapx+ix;
			int yy0=mapy+mapy+iy;
			bool res;
			if(VHMode)res=MFI->CheckHLine(xx0,yy0,1)!=0;
			else res=MFI->CheckVLine(xx0,yy0,1)!=0;
			int yy=smapy+(mul3(iy)<<2);
			int yy1=yy;
			if(Mode3D){
				yy1-=GetUnitHeight(ox+(ix<<4),oy+(iy<<4));
			};
			if(res){
				int dz1=0;
				int dz2=0;
				int dz3=0;
				int dz4=0;
				/*
				if(Mode3D){
					dz=GetHeight(ox+(ix<<4)+4,oy+(iy<<4)+4);
				};
				Xbar(smapx+(ix<<4)+4,smapy+(mul3(iy)<<2)-dz+2,2,2,254);
				dz=0;
				if(Mode3D){
					dz=GetHeight(ox+(ix<<4)+4,oy+(iy<<4)+12);
				};
				Xbar(smapx+(ix<<4)+4,smapy+(mul3(iy)<<2)-dz+6,2,2,254);
				dz=0;
				if(Mode3D){
					dz=GetHeight(ox+(ix<<4)+12,oy+(iy<<4)+4);
				};
				Xbar(smapx+(ix<<4)+12,smapy+(mul3(iy)<<2)-dz+2,2,2,254);
				dz=0;
				if(Mode3D){
					dz=GetHeight(ox+(ix<<4)+12,oy+(iy<<4)+12);
				};
				Xbar(smapx+(ix<<4)+12,smapy+(mul3(iy)<<2)-dz+6,2,2,254);
				*/
				if(Mode3D){
					dz1=GetUnitHeight(ox+(ix<<4),oy+(iy<<4));
					dz2=GetUnitHeight(ox+(ix<<4)+16,oy+(iy<<4));
					dz3=GetUnitHeight(ox+(ix<<4),oy+(iy<<4)+16);
					dz4=GetUnitHeight(ox+(ix<<4)+16,oy+(iy<<4)+16);
				};
				int xx=smapx+(ix<<4);
				DrawLine(xx,yy-dz1,xx+16,yy+8-dz4,254);
				DrawLine(xx,yy+8-dz3,xx+16,yy-dz2,254);
				if(!MFI->CheckPt(xx0+1,yy0))DrawLine(xx+16,yy-dz2,xx+16,yy+8-dz4,255);
				if(!MFI->CheckPt(xx0-1,yy0))DrawLine(xx,yy-dz1,xx,yy+8-dz3,255);
				if(!MFI->CheckPt(xx0,yy0-1))DrawLine(xx,yy-dz1,xx+16,yy-dz2,255);
				if(!MFI->CheckPt(xx0,yy0+1))DrawLine(xx,yy+8-dz3,xx+16,yy+8-dz4,255);
			};
#ifndef _USE3D
			if(yy1>WindY1)nydone=false;
#else
			if (yy1 > GPS.GetClipArea().GetBottom()) nydone = false;
#endif // _USE3D
			iy++;
		}while(nydone);
	};
	CloseGates(MyNation);	
};
extern int* DMap;
extern int  DMapSize;
extern byte TopTypeMode;
void TopShow(){
	if(LMode)return;
	if(!TopTypeMode)return;
	
	byte TopType=TopTypeMode-1;

	int ox=mapx<<5;
	int oy=mapy<<5;
	int nx=(smaplx>>1)+1;
	int stx=mapx>>1;
	int sty=mapy>>1;
	for(int ix=0;ix<nx;ix++){
		int iy=0;
		bool nydone=true;
		do{
			int xx0=stx+ix;
			int yy0=sty+iy;
			int yy=smapy+(mul3(iy)<<4);
			int yy1=yy;
			if(Mode3D){
				yy1-=GetHeight(ox+(ix<<6),oy+(iy<<6));
			};
			int top=GetTopRef(xx0+yy0*TopLx,TopType);
			if(top<0xFFFE){
				int dz1=0;
				int dz2=0;
				int dz3=0;
				int dz4=0;
				/*
				if(Mode3D){
					dz=GetHeight(ox+(ix<<4)+4,oy+(iy<<4)+4);
				};
				Xbar(smapx+(ix<<4)+4,smapy+(mul3(iy)<<2)-dz+2,2,2,254);
				dz=0;
				if(Mode3D){
					dz=GetHeight(ox+(ix<<4)+4,oy+(iy<<4)+12);
				};
				Xbar(smapx+(ix<<4)+4,smapy+(mul3(iy)<<2)-dz+6,2,2,254);
				dz=0;
				if(Mode3D){
					dz=GetHeight(ox+(ix<<4)+12,oy+(iy<<4)+4);
				};
				Xbar(smapx+(ix<<4)+12,smapy+(mul3(iy)<<2)-dz+2,2,2,254);
				dz=0;
				if(Mode3D){
					dz=GetHeight(ox+(ix<<4)+12,oy+(iy<<4)+12);
				};
				Xbar(smapx+(ix<<4)+12,smapy+(mul3(iy)<<2)-dz+6,2,2,254);
				*/
				int tc=0xD0+(top&31);
				if(Mode3D){
					dz1=GetHeight(ox+(ix<<6),oy+(iy<<6));
					dz2=GetHeight(ox+(ix<<6)+64,oy+(iy<<6));
					dz3=GetHeight(ox+(ix<<6),oy+(iy<<6)+64);
					dz4=GetHeight(ox+(ix<<6)+64,oy+(iy<<6)+64);
				};
				int xx=smapx+(ix<<6);
				DrawLine(xx,yy-dz1,xx+64,yy+32-dz4,tc);
				DrawLine(xx,yy+32-dz3,xx+64,yy-dz2,tc);
				if(xx0%3==1&&yy0%3==1){
					bool TCL=true;
					for(int dx=-1;dx<2;dx++)
						for(int dy=-1;dy<2;dy++){
							if(GetTopRef(xx0+dx+(yy0+dy)*TopLx,TopType)>=0xFFFE)TCL=false;
						};
						if(TCL){
							DrawLine(xx+1,yy-dz1,xx+64,yy+32-dz4-1,tc);
							DrawLine(xx+1,yy+32-dz3,xx+64,yy-dz2+1,tc);
							DrawLine(xx,yy-dz1+1,xx+64-1,yy+32-dz4,tc);
							DrawLine(xx,yy+32-dz3-1,xx+64-1,yy-dz2,tc);
						};
				};
				if(GetTopRef(xx0+1+yy0*TopLx,TopType)!=top)DrawLine(xx+63,yy-dz2,xx+63,yy+32-dz4,255);
				if(xx0>0&&GetTopRef(xx0-1+yy0*TopLx,TopType)!=top)DrawLine(xx,yy-dz1,xx,yy+32-dz3,255);
				if(GetTopRef(xx0+yy0*TopLx+TopLx,TopType)!=top)DrawLine(xx,yy+31-dz3,xx+63,yy+31-dz4,255);
				if(yy0>0&&GetTopRef(xx0+yy0*TopLx-TopLx,TopType)!=top)DrawLine(xx,yy-dz1,xx+63,yy-dz2,255);
				/*
				DWORD dat=InflMap[xx0+yy0*TopLx]&255;
				if(dat){
					char ccc[16];
					sprintf_s(ccc,sizeof(ccc),"%d",dat);
					ShowString(xx+32-6,yy-((dz1+dz2+dz3+dz4)>>2)-6+16,ccc,&fn10);
				};
				SafeCellInfo* SCI=GetCellInfo(xx0,yy0);
				if(SCI){
					char ccc[16];
					sprintf_s(ccc,sizeof(ccc),"%d",SCI->Index);
					ShowString(xx+32-6,yy-((dz1+dz2+dz3+dz4)>>2)-6+16,ccc,&fn8);
				};
				*/
			};
#ifndef _USE3D
			if(yy1>WindY1)nydone=false;
#else
			if (yy1 > GPS.GetClipArea().GetBottom()) nydone = false;
#endif // _USE3D
			iy++;
		}while(nydone);
	};
	if(DMap){
		oy>>=1;
		int NA=GetNAreas(TopType);
		for(int i=0;i<NA;i++){
			char cc[64];
			sprintf_s( cc,sizeof(cc),"Z%d:%d",i,DMap[i]);
			Area* AR=GetTopMap(i,TopType);
			ShowString((AR->x<<6)-ox,(AR->y<<5)-oy,cc,&BigYellowFont);
		};
	};
};
//------------------ATTACK POINT---------------------//
//it is used for the mortira,pushka(AI)
#define ATTP_ALLOWMOTION	1
#define ATTP_FINDPLACE		2
#define ATTP_IMMEDIATECHECK 4
#define ATTP_SINGLESHOT     8
#define ATTP_IMMEDIATEFINDPLACE 16
//return values:
//>0-ready
//-1-Too far or too near
//-2-prepiatsvia
int PredictShot(Weapon* Weap,int xs,int ys,int zs,int xd,int yd,int zd,word Index);
int PredictShotEx(Weapon* Weap,int xs,int ys,int zs,int xd,int yd,int zd,word Index);
int CheckDamageAbility(OneObject* OB,int x,int y,int z,byte Nation,int Soft){
	//1.Let us determine weapon
	int ox=OB->RealX>>4;
	int oy=OB->RealY>>4;
	int oz=OB->RZ+OB->newMons->SrcZPoint;
	int dst=Norma(x-ox,y-oy);
	if(Soft)dst-=48;
	int dstx=dst+((z-oz)<<1);
	if(dstx<0)dstx=0;
	int NeedState=-1;
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	bool NWATER=!OB->LockType;
	int DRMAX=0;
	if(Soft==3)DRMAX=280;
	for(int i=0;i<NAttTypes;i++){
		int wpk=NMN->WeaponKind[i];
		if(NWATER&&WeaponFlags[wpk]&1){
			if(dst>ADC->AttackRadius1[i]&&dstx<ADC->AttackRadius2[i]){
				NeedState=i;
			};
		}else{
			int R2=ADC->AttackRadius2[i];
			if(R2)R2+=DRMAX;
			if(dst>=ADC->AttackRadius1[i]&&dst<=R2){
				NeedState=i;
			};
		};
	};
	if(NeedState==-1)return -1;
	Weapon* WP=NMN->DamWeap[NeedState];
	if(!WP)return -1;
	int res=PredictShot(WP,ox,oy,oz,x,y,z,OB->Index);
	if(res==-1)return NeedState;
	if(res>=8192)return -2;
	OneObject* OBJ=Group[res];
	if(OBJ&&OBJ->NNUM!=Nation)return NeedState;
	return -2;
	
};
int FastCheckDamageAbility(OneObject* OB,int x,int y,int z,byte Nation,int Soft){
	//1.Let us determine weapon
	int ox=OB->RealX>>4;
	int oy=OB->RealY>>4;
	int oz=OB->RZ+OB->newMons->SrcZPoint;
	int dst=Norma(x-ox,y-oy);
	if(Soft)dst-=48;
	int dstx=dst+((z-oz)<<1);
	if(dstx<0)dstx=0;
	int NeedState=-1;
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	bool NWATER=!OB->LockType;
	int DRMAX=0;
	if(Soft==3)DRMAX=280;
	for(int i=0;i<NAttTypes;i++){
		int wpk=NMN->WeaponKind[i];
		if(NWATER&&WeaponFlags[wpk]&1){
			if(dst>ADC->AttackRadius1[i]&&dstx<ADC->AttackRadius2[i]){
				NeedState=i;
			};
		}else{
			int R2=ADC->AttackRadius2[i];
			if(R2)R2+=DRMAX;
			if(dst>=ADC->AttackRadius1[i]&&dst<=R2){
				NeedState=i;
			};
		};
	};
	if(NeedState==-1)return -1;
	Weapon* WP=NMN->DamWeap[NeedState];
	if(!WP)return -1;
	int res=PredictShot(WP,ox,oy,oz,x,y,z,OB->Index);
	if(res==-1)return NeedState;
	if(res>=8192)return -2;
	OneObject* OBJ=Group[res];
	if(OBJ&&OBJ->NNUM!=Nation)return NeedState;
	return -2;
	
};
int FindPlaceForAttack(OneObject* OB,int x,int y,int z,byte Nation,
					   int MaxSteps,int SearchMethod,int* Newx,int* Newy){
	int OldRX=OB->RealX;
	int OldRY=OB->RealY;
	int myx=OldRX>>4;
	int myy=OldRY>>4;
	int DX=x-myx;
	int DY=y-myy;
	int N=Norma(DX,DY);
	if(!(DX||DY))return 1;
	DX=(DX<<5)/N;
	DY=(DY<<5)/N;
	DX+=(rando()&15)-8;
	DY+=(rando()&15)-8;
	if(SearchMethod==-1){
		//move forward
		myx+=DX;
		myy+=DY;
		MaxSteps<<=1;
		for(int i=0;i<MaxSteps;i++){
			if(CheckPt(myx>>4,myy>>4)){
				OB->RealX=OldRX;
				OB->RealY=OldRY;
				return SearchMethod;
			}else{
				OB->RealX=(myx<<4);
				OB->RealY=(myy<<4);
				int r=CheckDamageAbility(OB,x,y,z,Nation,false);
				if(!r){
					OB->RealX=OldRX;
					OB->RealY=OldRY;
					*Newx=myx;
					*Newy=myy;
					return 0;
				};
			};
			myx+=DX;
			myy+=DY;
		};
	}else{
		//strafe
		int lfx=myx+DY;
		int lfy=myy-DX;
		int rfx=myx-DY;
		int rfy=myy+DX;
		bool LMove=true;
		bool RMove=true;
		for(int i=0;i<MaxSteps&&(LMove||RMove);i++){
			if(LMove){
				if(!CheckPt(lfx>>4,lfy>>4)){
					OB->RealX=lfx<<4;
					OB->RealY=lfy<<4;
					int r=CheckDamageAbility(OB,x,y,z,Nation,false);
					if(!r){
						OB->RealX=OldRX;
						OB->RealY=OldRY;
						*Newx=lfx;
						*Newy=lfy;
						return 0;
					};	
				}else LMove=false;
				lfx+=DY;
				lfy-=DX;
			};
			if(RMove){
				if(!CheckPt(rfx>>4,rfy>>4)){
					OB->RealX=rfx<<4;
					OB->RealY=rfy<<4;
					int r=CheckDamageAbility(OB,x,y,z,Nation,false);
					if(!r){
						OB->RealX=OldRX;
						OB->RealY=OldRY;
						*Newx=rfx;
						*Newy=rfy;
						return 0;
					};	
				}else RMove=false;
				rfx+=DY;
				rfy-=DX;
			};
		};
	};
	OB->RealX=OldRX;
	OB->RealY=OldRY;
	return SearchMethod;
};
//only for pushka and mortira
void CreateRazbros(OneObject* OBJ,Coor3D* C3D);
bool MoveAwayInSector(int x,int y,int r0,int r1,char Dir,int ddir);
void AI_AttackPointLink(OneObject* OBJ){
	if(OBJ->delay)return;
	int dax=OBJ->LocalOrder->info.AttackXY.ox;
	int day=OBJ->LocalOrder->info.AttackXY.oy;
	int x=OBJ->LocalOrder->info.AttackXY.x;
	int y=OBJ->LocalOrder->info.AttackXY.y;
	int z=OBJ->LocalOrder->info.AttackXY.z;
	int myx=OBJ->RealX>>4;
	int myy=OBJ->RealY>>4;
	int RRot=OBJ->newMons->MinRotator;
	int rot2=RRot<<1;
	if(rot2>16)rot2=16;
	if(Norma(dax-myx,day-myy)<32){
		//rotation
		char Ndir=char(GetDir(x-myx,y-myy));
		char ddir=Ndir-OBJ->RealDir;
		if(abs(ddir)<rot2){
			//can make shot now
			//1.let us determine method of attack
			int r=CheckDamageAbility(OBJ,x,y,z,OBJ->NNUM,true);
			if(r<0){
				OBJ->DeleteLastOrder();
				return;
			};
			NewMonster* NMN=OBJ->newMons;
			if(NMN->Usage!=SupMortID&&NMN->Usage!=MortiraID){
				if(MoveAwayInSector(OBJ->RealX,OBJ->RealY,16*16,200*16,OBJ->RealDir,33))return;
			};
			//2.checking of resources
			bool AllowShot=true;
			AdvCharacter* ADC=OBJ->Ref.General->MoreCharacter;
			if(NMN->NShotRes){
				for(int k=0;k<NMN->NShotRes;k++){
					if(XRESRC(OBJ->NNUM,NMN->ShotRes[k+k])<NMN->ShotRes[k+k+1])AllowShot=false;
				};
				if(AllowShot){
					for(int k=0;k<NMN->NShotRes;k++){
						AddXRESRC(OBJ->NNUM,NMN->ShotRes[k+k],-NMN->ShotRes[k+k+1]);
						OBJ->Nat->AddResource(byte(NMN->ShotRes[k+k]),-NMN->ShotRes[k+k+1]);
						OBJ->Nat->ResOnLife[NMN->ShotRes[k+k]]+=NMN->ShotRes[k+k+1];
					};
				};
			};
			//3.source point
			OBJ->delay=ADC->AttackPause[r];
			OBJ->MaxDelay=div(int(OBJ->delay)*OBJ->PersonalDelay,100).quot;
			OBJ->delay=OBJ->MaxDelay;
			addrand(OBJ->delay);
			Weapon* WP=NMN->DamWeap[r];
			//pubuxxxxxxxxx...
			if(WP){
				//----------
				NewAnimation* UNAM=NMN->Attack+r;
				int uocts=(UNAM->Rotations-1)*2;
				int uoc2=UNAM->Rotations-1;
				if(!uocts)uocts=1;
				int usesize=div(255,uocts<<1).quot;
				byte udir=(((OBJ->RealDir+64+usesize)&255)*uocts)>>8;
				NewFrame* NF=&UNAM->Frames[0];
				//----------
				NewAnimation* NAM=WP->NewAnm;
				int octs=(NAM->Rotations-1)*2;
				int oc2=NAM->Rotations-1;
				if(!octs)octs=1;
				int sesize=div(255,octs<<1).quot;
				byte dir=(((OBJ->RealDir+64+sesize)&255)*octs)>>8;
				int x0,y0,z0,x1,y1,z1;
				if(udir<uoc2){
					int udir1=uoc2-udir;
					x0=(OBJ->RealX>>4)-(UNAM->ActivePtX[udir1]+NF->dx);
					y0=(OBJ->RealY>>4)+Prop43(UNAM->ActivePtY[udir1]+NF->dy+NMN->SrcZPoint);
				}else{
					int udir1=uoc2-uocts+udir;
					x0=(OBJ->RealX>>4)+(UNAM->ActivePtX[udir1]+NF->dx);
					y0=(OBJ->RealY>>4)+Prop43(UNAM->ActivePtY[udir1]+NF->dy+NMN->SrcZPoint);
				};
				Coor3D C3D;
				C3D.x=x;
				C3D.y=y;
				C3D.z=z;
				CreateRazbros(OBJ,&C3D);
				z0=OBJ->RZ+NMN->SrcZPoint;
				if(udir<uoc2){
					udir=uoc2-udir;
					x1=C3D.x;
					y1=C3D.y;
					z1=C3D.z+NMN->DstZPoint;
					Create3DAnmObject(WP,x0,y0,z0,x1,y1,z1,OBJ,OBJ->NewState-1,0xFFFF);
				}else{
					udir=uoc2-uocts+udir;
					x1=C3D.x;
					y1=C3D.y;
					z1=C3D.z+NMN->DstZPoint;
					Create3DAnmObject(WP,x0,y0,z0,x1,y1,z1,OBJ,OBJ->NewState-1,0xFFFF);
				};
			};
			if(OBJ->LocalOrder->info.AttackXY.wep!=255)OBJ->DeleteLastOrder();
			return;
		}else{
			//Need to rotate
			if(ddir>0)OBJ->RealDir+=RRot;
			else OBJ->RealDir-=RRot;
			OBJ->GraphDir=OBJ->RealDir;
			OBJ->DeletePath();
			OBJ->DestX=-1;
			OBJ->DestY=-1;
		};
	}else{
		OBJ->CreatePath(dax>>4,day>>4);
	};
};

bool OneObject::AttackPoint(int x,int y,int z,byte Times,byte Flags,byte OrdType,byte Prio){
	int ImmCanDam=CheckDamageAbility(this,x,y,z,NNUM,false);
	if((delay&&Times!=255)||LocalOrder&&LocalOrder->DoLink==&AI_AttackPointLink)return false;
	if((Flags&ATTP_IMMEDIATECHECK)&&ImmCanDam<0)return false;
	int r=0;
	int Newx=-1;
	int Newy=-1;
	if(ImmCanDam<0)r=FindPlaceForAttack(this,x,y,z,NNUM,30,ImmCanDam,&Newx,&Newy);
	else{
		Newx=RealX>>4;
		Newy=RealY>>4;
	};
	if((ImmCanDam<0)&&(Flags&ATTP_IMMEDIATEFINDPLACE)&&r)return false;
	Order1* OR1=CreateOrder(OrdType);
	if(!OR1)return false;
	OR1->info.AttackXY.ox=Newx;
	OR1->info.AttackXY.oy=Newy;
	OR1->DoLink=&AI_AttackPointLink;
	OR1->OrderType=177;
	OR1->OrderTime=0;
	OR1->info.AttackXY.x=x;
	OR1->info.AttackXY.y=y;
	OR1->info.AttackXY.z=z;
	OR1->info.AttackXY.wep=Times;
	return true;
};
//Special for pushek
int GetEnemyDifference(int cell,byte Mask){
	int NHim=0;
	int NMy=0;
	cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	if(!NMon)return NULL;
	int ofs1=cell<<SHFCELL;
	word MID;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB){
				if(OB->NMask&Mask)NMy++;
				else NHim++;
			};
		};
	};
	if(NMy>5||NHim<3||NHim<=NMy)return -1;
    return NHim-NMy;
};

//bool PushUnitsInCell(int cell,int x0,int y0
//bool PushUnitsAway(OnObject* OB){
//
//};
void SearchEnemyForAIArtillery(OneObject* OB){
	if(OB->delay)return;
	//if(rando()<20000)return;
    int cell=((OB->RealY>>11)<<VAL_SHFCX)+(OB->RealX>>11);
    NewMonster* NM=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
    int rr=ADC->MaxR_Attack;
	int rx0=(OB->RealX>>4)+64;
	int ry0=(OB->RealY>>4)+64;
	int x0=OB->RealX>>11;
	int y0=OB->RealY>>11;
	EnemyInfo* EIN=GNFO.EINF[OB->NNUM];
	//search for towers
	DangerInfo* DF=EIN->DINF;
	int xxx=OB->RealX>>4;
	int yyy=OB->RealY>>4;
	for(int p=0;p<EIN->NDINF;p++){
		if(Norma(DF->x-xxx,DF->y-yyy)<rr){
			OB->AttackPoint(DF->x,DF->y,DF->z+32,1,ATTP_IMMEDIATEFINDPLACE,1,128+64);
			if(OB->LocalOrder&&OB->LocalOrder->DoLink==&AI_AttackPointLink)return;
		};
		DF++;
	};
	//-----------------
    int rx1=(rr>>7)+1;
	int EnX=-1;
	int EnY=-1;
    int mindist=10000000;
    int rx2=rx1+rx1+1;
    int stcell;
	int MaxDif=-1;
    for(int i=0;i<64;i++){
        int dx=((rando()*rx2)>>15)-rx1;
        int dy=((rando()*rx2)>>15)-rx1;
		int x1=rx0+(dx<<7);
		int y1=ry0+(dy<<7);
		if(Norma(x1-rx0,y1-ry0)<rr){
			stcell=cell+dx+(dy<<VAL_SHFCX);
	        if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
		        if(NPresence[stcell]){
			        int N=GetEnemyDifference(stcell,OB->NMask);
				    if(N>MaxDif){
						MaxDif=N;
						EnX=x1;
						EnY=y1;
					};
				 };
			 };
        };
    };
    if(MaxDif>5){
		OB->AttackPoint(EnX,EnY,GetHeight(EnX,EnY)+32,1,ATTP_IMMEDIATEFINDPLACE,1,128+64);
	}else{
		//search of wall to erase
		if(rando()<4096){
			rr-=510;
			for(int p=0;p<EIN->NEnWalls;p++){
				int xx1=(int(EIN->WallsX[p])<<6)+32;
				int yy1=(int(EIN->WallsY[p])<<6)+32;

				if(Norma(xx1-xxx,yy1-yyy)<rr){
					OB->AttackPoint(xx1,yy1,GetHeight(xx1,yy1)+5,1,ATTP_IMMEDIATEFINDPLACE,1,128+64);
					if(OB->LocalOrder&&OB->LocalOrder->DoLink==&AI_AttackPointLink)return;
				};
				DF++;
			};
		};
	};
};
void SearchEnemyForMortira(OneObject* OB){
	if(OB->delay)return;
	//if(rando()<20000)return;
    int cell=((OB->RealY>>11)<<VAL_SHFCX)+(OB->RealX>>11);
    NewMonster* NM=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
    int rr=ADC->MaxR_Attack+400;
	int rx0=(OB->RealX>>4)+64;
	int ry0=(OB->RealY>>4)+64;
	int x0=OB->RealX>>11;
	int y0=OB->RealY>>11;
	int xxx=OB->RealX>>4;
	int yyy=OB->RealY>>4;
	EnemyInfo* EIN=GNFO.EINF[OB->NNUM];
	if(rando()<8192){
		rr-=510;
		for(int p=0;p<EIN->NEnWalls;p++){
			int xx1=(int(EIN->WallsX[p])<<6)+32;
			int yy1=(int(EIN->WallsY[p])<<6)+32;
			if(Norma(xx1-xxx,yy1-yyy)<rr){
				OB->AttackPoint(xx1,yy1,GetHeight(xx1,yy1)+5,1,ATTP_IMMEDIATEFINDPLACE,1,128+64);
				if(OB->LocalOrder&&OB->LocalOrder->DoLink==&AI_AttackPointLink)return;
			};
		};
	};
	rr-=390;
	//search for towers
	DangerInfo* DF=EIN->DINF;
	for(int p=0;p<EIN->NDINF;p++){
		if(Norma(DF->x-xxx,DF->y-yyy)<rr){
			OB->AttackPoint(DF->x,DF->y,DF->z+32,1,ATTP_IMMEDIATECHECK,1,128+64);
			if(OB->LocalOrder&&OB->LocalOrder->DoLink==&AI_AttackPointLink)return;
		};
		DF++;
	};
	//-----------------
    int rx1=(rr>>7)+1;
	int EnX=-1;
	int EnY=-1;
    int mindist=10000000;
    int rx2=rx1+rx1+1;
    int stcell;
	int MaxDif=-1;
    for(int i=0;i<64;i++){
        int dx=((rando()*rx2)>>15)-rx1;
        int dy=((rando()*rx2)>>15)-rx1;
		int x1=rx0+(dx<<7);
		int y1=ry0+(dy<<7);
		if(Norma(x1-rx0,y1-ry0)<rr){
			stcell=cell+dx+(dy<<VAL_SHFCX);
	        if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
		        if(NPresence[stcell]){
			        int N=GetEnemyDifference(stcell,OB->NMask);
				    if(N>MaxDif){
						MaxDif=N;
						EnX=x1;
						EnY=y1;
					};
				 };
			 };
        };
    };
    if(MaxDif>5){
		OB->AttackPoint(EnX,EnY,GetHeight(EnX,EnY)+32,1,ATTP_IMMEDIATECHECK,1,128+64);
	};
};
void AIArtilleryAgainstTowers(OneObject* OB){
	if(OB->delay)return;
	//if(rando()<20000)return;
    int cell=((OB->RealY>>11)<<VAL_SHFCX)+(OB->RealX>>11);
    NewMonster* NM=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
    int rr=ADC->MaxR_Attack+900;
	int rx0=(OB->RealX>>4)+64;
	int ry0=(OB->RealY>>4)+64;
	int x0=OB->RealX>>11;
	int y0=OB->RealY>>11;
	//search for towers
	EnemyInfo* EIN=GNFO.EINF[OB->NNUM];
	DangerInfo* DF=EIN->DINF;
	int xxx=OB->RealX>>4;
	int yyy=OB->RealY>>4;
	for(int p=0;p<EIN->NDINF;p++){
		if(Norma(DF->x-xxx,DF->y-yyy)<rr){
			OB->AttackPoint(DF->x,DF->y,DF->z+32,1,ATTP_IMMEDIATEFINDPLACE,1,128+64);
			if(OB->LocalOrder&&OB->LocalOrder->DoLink==&AI_AttackPointLink)return;
		};
		DF++;
	};
};
//---------------------ATTACK------------------------//
//Атаковать объект с заданным индексом
void AttackObjLink(OneObject* OBJ);
void BuildObjLink(OneObject* OBJ);
void ProduceObjLink(OneObject* OBJ);
void WaterAttackLink(OneObject* OBJ);
bool OneObject::AttackObj(word OID,int Prio){
	return AttackObj(OID,Prio,0);
};
bool OneObject::AttackObj(word OID,int Prio,byte OrdType){
	return AttackObj(OID,Prio,OrdType,0);
};
void B_BitvaLink(Brigade* BR);
void GrenaderSupermanLink(OneObject* OBJ);
void EraseBrigade(Brigade* BR);
void SetAttState(Brigade* BR,bool Val);
void PanicUnitLink(OneObject* OBJ);
void LeaveMineLink(OneObject* OB);
void PerformUpgradeLink(OneObject* OBJ);
bool OneObject::AttackObj(word OID,int Prio1,byte OrdType,word NTimes){
	if(NewBuilding&&LocalOrder){
		if(LocalOrder->DoLink==&LeaveMineLink||LocalOrder->DoLink==&PerformUpgradeLink)return false;
		if(!Attack)Prio1=128+1;
	};
	if(Sdoxlo||Hidden||UnlimitedMotion||!Ready)return false;
	if(CheckOrderAbility())return false;
	if((!(Nat->AI_Enabled||StopBuildMode))&&LocalOrder&&LocalOrder->DoLink==&ProduceObjLink)return false;
	//assert(OID!=0xFFFF);
	byte Prio;
	if(Prio1==254){
		Prio=16+128;
		StandGround=0;
	}else Prio=Prio1;
	//if((Prio&127)>=16&&!InArmy)StandGround=0;
	if(newMons->MaxInside&&!newMons->DamWeap[0])return false;
	OneObject* EOB=Group[OID];
	if(!EOB){
		//DeleteLastOrder();
		return 0;
	};
	if(EOB->Sdoxlo){
		//DeleteLastOrder();
		return 0;
	};
#ifdef COSSACKS2
	if(newMons->Usage==PushkaID){
		NewAttackPoint(EOB->RealX>>4,EOB->RealY>>4,Prio1,0,0);
		return true;
	};
#endif //COSSACKS2
#ifdef SIMPLEMANAGE
	if(newMons->ArmAttack){
		if(Prio<128&&!(ArmAttack||RifleAttack))return false;
	};
#endif //SIMPLEMANAGE
	if(NMask&EOB->NMask&&!(newMons->Priest||newMons->Shaman))return false;
	if(!CheckAttAbility(this,OID)){
		if(OID&&(!(LockType==1||NewBuilding||newMons->Capture))&&EOB->newMons->Capture){
			NewMonsterSmartSendTo(EOB->RealX>>4,EOB->RealY>>4,0,0,128,0);
		};
		return false;
	};
	bool STRELOK=false;
	Brigade* BR=NULL;
	byte Use=newMons->Usage==GrenaderID;
	if(InArmy&&(Use==GrenaderID||Use==ArcherID)&&(EOB->NewBuilding||EOB->Wall)){
		BR=CITY[NNUM].Brigs+BrigadeID;
		SetAttState(BR,0);
		EraseBrigade(BR);
	};
	if((!Zombi)&&InArmy&&Prio1!=15+128&&!(newMons->Artilery||EOB->NewBuilding)){
		BR=CITY[NNUM].Brigs+BrigadeID;
#ifdef COSSACKS2
		if(!RifleAttack){
#else
		if(!BR->Strelki){
#endif

			/*
			if(BR->AttEnm){
				if(BR->BOrder&&BR->BOrder->BLink!=&B_BitvaLink){
					if(Norma(RealX-EOB->RealX,RealY-EOB->RealY)<100*16)BR->Bitva();
					return true;
				};
			};
			*/
			if(Prio1==254){
				return true;
			}else{
				if(BR->BOrder){
					if(BR->BOrder->BLink!=&B_BitvaLink){
						if(BR->AttEnm){
							if(Norma(RealX-EOB->RealX,RealY-EOB->RealY)<100*16)BR->Bitva();
							return true;
						}else return true;
					};
				}else{
					BR->Bitva();
					return true;
				};
			};
		}else{
			if(BR->BOrder&&(BR->BOrder->Prio&127))return true;
			STRELOK=1;
		};
	};
	if(NoSearchVictim&&Prio<128)return true;
	NoSearchVictim=false;
	//if(CheckOrderAbility())return false;
	//if(LocalOrder&&LocalOrder->DoLink==&ProduceObjLink)return false;
	//assert(OID!=0xFFFF);
	//if(newMons->MaxInside)return false;
	//if(!CheckAttAbility(this,OID))return false;
	//if(Prio>=16)StandGround=false;
	//	WAttackObj(OID,Prio);
	//	return;
	//};
	//if(Media==2){
	//	FlyAttack(OID,Prio);
	//	return;
	//};
	OneObject* OB=Group[OID];
	if((!OB)||(OB->UnlimitedMotion&&!(OB->LocalOrder&&OB->LocalOrder->DoLink==&PanicUnitLink)))return false;
	if(Prio<PrioryLevel/*&&!Attack*/)return false;
	if(Prio<5&&EnemyID!=0xFFFF){
		OneObject* EOB=Group[EnemyID];
		if(EOB){
			int dist1=Norma(OB->RealX-RealX,OB->RealY-RealY);
			int dist2=Norma(EOB->RealX-RealX,EOB->RealY-RealY);
			if(dist1<=Ref.General->MoreCharacter->MinR_Attack||dist2<=dist1)return false;
		};
	};
    //if(Prio<16&&Prio==PrioryLevel)return false;
	if(!Ready)return false;
	//if(Weap)return;
	//ClearOrders();
	if(RAmount){
		RAmount=0;
		RType=0xFF;
		GroundState=0;
	};
	if(!LocalOrder)Attack=false;
	if(!int(OB)||OB->Sdoxlo)return false;
	//Important=true;
	if(Attack){
		if(!(LocalOrder&&LocalOrder->DoLink==&GrenaderSupermanLink)){
			EnemyID=OID;
			EnemySN=OB->Serial;
			PrioryLevel=Prio&127;
			LocalOrder->PrioryLevel=PrioryLevel;
			//SearchSupport(OID);
#ifdef COSSACKS2
			if(RifleAttack)
#endif
			StandTime=0;
			return true;
		};
	};
	//SearchSupport(OID);
#ifdef NEWMORALEPRIEST
    if (newMons->Shaman) {
        bool OKK = 1;
        if (newMons->MoraleHealer) {
            if (OB->Morale >= OB->MaxMorale)OKK = 0;
        }
        else {
            if (OB->Life >= OB->MaxLife)OKK = 0;
        }
        if (!(NMask & OB->NMask))OKK = 0;
        if (!OKK) {
            OKK = 1;
            if (NMask & OB->NMask)OKK = 0;
        };
        if (!OKK)return false;
    }
    else
        if (newMons->Priest) {
            if (newMons->MoraleHealer) {
                if (OB->Morale >= OB->MaxMorale)return false;
            }
            else {
                if (OB->Life >= OB->MaxLife)return false;
            }
            if (!(NMask & OB->NMask))return false;
        }
        else {
            if (NMask & OB->NMask)return false;
        };
#else
    if (newMons->Shaman) {
        bool OKK = 1;
        if (OB->Life >= OB->MaxLife)OKK = 0;
        if (!(NMask & OB->NMask))OKK = 0;
        if (!OKK) {
            OKK = 1;
            if (NMask & OB->NMask)OKK = 0;
        };
        if (!OKK)return false;
    }
    else
        if (newMons->Priest) {
            if (OB->Life >= OB->MaxLife)return false;
            if (!(NMask & OB->NMask))return false;
        }
        else {
            if (NMask & OB->NMask)return false;
        };
#endif
	if(InPatrol&&OrdType==0)
		OrdType=1;
	if(newMons->ShotDir){
		if(OrdType==0)OrdType=1;
	};
	Order1* Or1=CreateOrder(OrdType);
	if(!int(Or1))return false;
	Or1->PrioryLevel=Prio&127;
	Or1->OrderType=3;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=AttackLink;
	if(newMons->NMultiWp)Or1->DoLink=&WaterAttackLink;
	Or1->info.BuildObj.ObjX=OB->x;
	Or1->info.BuildObj.ObjY=OB->y;
	Or1->info.BuildObj.SN=NTimes;
	Or1->info.BuildObj.AttMethod=0xFF;
	EnemyID=OID;
	EnemySN=OB->Serial;
	Attack=true;
	//if(int(InLineCom))FreeAsmLink();
	if(Prio>=16)MakeOrderSound(this,1);
	RType=0xFF;
	//PrioryLevel=Prio&127;
	//if(CrowdRef)CrowdRef->AddToAttackQueue(OID);
	//AlwaysLock=true;
	PathDelay=0;
#ifdef COSSACKS2
	if(StandGround){
		AttackObjLink(this);
		if(EnemyID==0xFFFF)return false;
	};
#endif //COSSACKS2
	if(STRELOK){
		if(PrioryLevel==0){
			//if(!BR->BOrder){
				int N=BR->NMemb;
				word* mem=BR->Memb;
				word* sns=BR->MembSN;
				for(int i=NBPERSONAL;i<N;i++){
					if(mem[i]!=0xFFFF){
						OneObject* OB=Group[mem[i]];
						if(OB!=this&&OB->EnemyID==0xFFFF){
							OB->AttackObj(OID,Prio,OrdType,NTimes);
						};
					};
				};
				for(int i=0;i<NBPERSONAL;i++){
					if(mem[i]!=0xFFFF){
						OneObject* OB=Group[mem[i]];
						if(OB){
							if(OB->EnemyID!=0xFFFF){
								OneObject* OBJ=Group[OB->EnemyID];
								if(OBJ&&Norma(OB->RealX-OBJ->RealX,OB->RealY-OBJ->RealY)>80*16)OBJ->ClearOrders();
							}else OB->ClearOrders();
						};
					};
				};
			//};
		};
	};
#ifdef COSSACKS2
	UnitSpeed=64;
	if(RifleAttack)
#endif
	StandTime=0;
	return true;
};	
void NewAttackPointLink(OneObject* OBJ);
bool OneObject::NewAttackPoint(int x,int y,int Prio1,byte OrdType,word NTimes){
	if(Sdoxlo||Hidden||UnlimitedMotion)return false;
	if(CheckOrderAbility())return false;
	if(LocalOrder){
		if(LocalOrder->DoLink==&ProduceObjLink)return false;
		//if(LocalOrder->DoLink==NewAttackPointLink){
		//	if(NewAnm&&NewAnm->ActiveFrame)return false;
		//};

	}
	//assert(OID!=0xFFFF);
	byte Prio;
	if(Prio1==254){
		Prio=16+128;
		StandGround=0;
	}else Prio=Prio1;
	//if((Prio&127)>=16&&!InArmy)StandGround=0;
	if(newMons->MaxInside)return false;
	
	if(Prio<PrioryLevel/*&&!Attack*/)return false;
	
	if(!Ready)return false;
	//if(Weap)return;
	//ClearOrders();
	if(RAmount){
		RAmount=0;
		RType=0xFF;
		GroundState=0;
	};
	
	Order1* Or1=CreateOrder(OrdType);
	if(!int(Or1))return false;
	Or1->PrioryLevel=Prio&127;
	Or1->OrderType=76;//Атака
	Or1->OrderTime=0;
	Or1->DoLink=&NewAttackPointLink;
	
	Or1->info.BuildObj.ObjX=x>>4;
	Or1->info.BuildObj.ObjY=y>>4;
	Or1->info.BuildObj.SN=NTimes;
	Or1->info.BuildObj.AttMethod=0xFF;
	DstX=x<<4;
	DstY=y<<4;
	EnemyID=0xFFFF;
	EnemySN=0xFFFF;
	Attack=0;
	//if(int(InLineCom))FreeAsmLink();
	if(Prio>=16)MakeOrderSound(this,1);
	RType=0xFF;
	if(OrdType==0||OrdType==1)PrioryLevel=Prio&127;
	//if(CrowdRef)CrowdRef->AddToAttackQueue(OID);
	//AlwaysLock=true;
	PathDelay=0;
	StandTime=0;
	
	return true;
};	
int GetTopDistance(int xa,int ya,int xb,int yb,int LT);
bool SetDestUnit(OneObject* OBJ,OneObject* OB,short OLDX,short OLDY,short NEWX,short NEWY){
#ifdef COSSACKS2
	if(GetTopDistance((OBJ->RealX+512)>>10,(OBJ->RealY+512)>>10,(OB->RealX+512)>>10,(OB->RealY+512)>>10,OBJ->LockType)>4096)return false;
	int xd=NEWX;
	int yd=NEWY;
	FindBestPosition(OBJ,&xd,&yd,40);
	NEWX=xd;
	NEWY=yd;
#endif //COSSACKS2
	if(!OB->newMons->Artilery){
		int dr=Norma(NEWX-OBJ->x,NEWY-OBJ->y);
		int dr1=Norma(NEWX-OBJ->CPdestX,NEWY-OBJ->CPdestY);
		bool renew=0; 
		if(dr>500){
			if(dr1>30)renew=1;
		}else if(dr>100){
			if(dr1>30)renew=1;
		}else if(dr>50){
			if(dr1>20)renew=1;
		}else if(dr>30&&dr1>10)renew=1;
		if(dr>30){
			if(renew){
				OBJ->PathDelay=0;
				OBJ->CreatePath(NEWX,NEWY);
			}else OBJ->CreatePath(OBJ->CPdestX,OBJ->CPdestY);
			return true;
		};
	};
	/*
	if(OBJ->newMons->MaxAttRange<150){
		OBJ->CreatePath(OB->x,OB->y);
		return;
	};
	*/
	if(!OBJ->PathX){
#ifdef COSSACKS2
		OBJ->DestX=NEWX<<8;//OB->RealX;
		OBJ->DestY=NEWY<<8;//OB->RealY;
#else
		OBJ->DestX=OB->RealX;
		OBJ->DestY=OB->RealY;
#endif
		OBJ->LocalOrder->info.BuildObj.ObjX=NEWX;
		OBJ->LocalOrder->info.BuildObj.ObjY=NEWY;
	}else{
		int dis=Norma(OBJ->x-NEWX,OBJ->y-NEWY);
		int dis1=Norma(NEWX-OLDX,NEWY-OLDY);
		int dds=0;
		if(dis>100)dds=20;
		else if(dis>50)dds=10;
		else if(dis>30)dds=5;
		else if(dis>10)dds=3;
		else dds=2;
		if(dis1>=dds){
			OBJ->DeletePath();
			//OBJ->DestX=OB->RealX;
			//OBJ->DestY=OB->RealY;
			OBJ->CreatePath(NEWX,NEWY);
			OBJ->LocalOrder->info.BuildObj.ObjX=NEWX;
			OBJ->LocalOrder->info.BuildObj.ObjY=NEWY;
		}else OBJ->CreatePath(OLDX,OLDY);
	};
	return true;
};

bool Create3DAnmObject(Weapon* Weap,int xs,int ys,int zs,
					                int xd,int yd,int zd,
									OneObject* OB);
extern bool ShotRecommended;
void CreateRazbros(OneObject* OBJ,Coor3D* C3D){
	AdvCharacter* ADC=OBJ->Ref.General->MoreCharacter;
	if(!ADC->Razbros)return;
	int R=Norma(((OBJ->RealX)>>4)-C3D->x,(OBJ->RealY>>4)-C3D->y)>>5;
	C3D->x+=div((int(rando()>>5)-512)*R*ADC->Razbros,32000).quot;
	C3D->y+=div((int(rando()>>5)-512)*R*ADC->Razbros,32000).quot;
};
int CheckPointForDamageAbility(OneObject* OBJ,int x,int y,int z){
	int dst=Norma((OBJ->RealX>>4)-x,(OBJ->RealY>>4)-y);
#ifdef COSSACKS2
	int D=(int(OBJ->RZ+OBJ->newMons->SrcZPoint-z)<<2);
	if(D>0)D=0;
	int dst1=dst-D;
#else //COSSACKS2
	int dst1=dst-(int(OBJ->RZ+OBJ->newMons->SrcZPoint-z)<<1);
#endif //COSSACKS2
	NewMonster* NMN=OBJ->newMons;
	AdvCharacter* ADC=OBJ->Ref.General->MoreCharacter;
    if(dst<ADC->AttackRadius1[0]||dst1>ADC->AttackRadius2[0])return -1;
    int xx,yy,x0,y0;
    OBJ->GetCornerXY(&xx,&yy);
    xx<<=8;
    yy<<=8;
    int mindis=1000000;
    int bsp=0;
    int np=NMN->NShotPt;
    int xx1=x<<4;
    int yy1=y<<4;
	int zzz=NMN->SrcZPoint;
	int DZZ=OBJ->RZ;
    for(int i=0;i<np;i++){
        x0=xx+(NMN->ShotPtX[i]<<4);
        y0=yy+(Prop43(NMN->ShotPtY[i])<<4);
        dst=Norma(x0-xx1,y0-yy1);
        if(dst<mindis){
            mindis=dst;
            bsp=i;
		};
    };
    x0=xx+(NMN->ShotPtX[bsp]<<4);
    y0=yy+(Prop43(NMN->ShotPtY[bsp])<<4)+(zzz<<5);
	Weapon* Weap=NMN->DamWeap[0];
    if(Weap){
		int wx0=x0>>4;
		int wy0=(y0>>4)+6;
		int wz0=3+DZZ+zzz;
		int WRes=PredictShot(Weap,wx0,wy0,wz0,x,y,z,OBJ->Index);
		if(WRes!=-1) return -1;
		return 0;
	};
	return -1;
};
bool TestUnitsInSectorForGrenade(OneObject* GRE);
bool AttGroundMod=0;
void NewAttackPointLink(OneObject* OBJ){
	AttGroundMod=1;
	int DX=OBJ->DestX;
	int DY=OBJ->DestY;
	AttackObjLink(OBJ);
	OBJ->DestX=DX;
	OBJ->DestY=DY;
#ifdef COSSACKS2
	if(OBJ->newMons->Usage!=PushkaID){
		OBJ->DestX=-1;
		OBJ->DestY=-1;
	};
#endif
	AttGroundMod=0;
};
extern int ADDRANGE;
extern int ADDPAUSE;
bool Create3DAnmObjectEX(Weapon* Weap,int xs,int ys,int zs1,
					                int xd,int yd,int zd,
									OneObject* OB,byte AttType,word DestObj,
									int vx,int vy,int vz,int GRDZ);
/*
int CheckBuildingDamagePoint(word NIndex,int dx,int dy){
	if(NMN->ShotDir){
		int xx,yy;
		OBJ->GetCornerXY(&xx,&yy);
		xx<<=4;
		yy<<=4;
		int N=NMN->NShotPt;
		int dst=Norma(OBJ->RealX-OB->RealX,OBJ->RealY-OB->RealY)>>4;
		if(dst<=OBJ->Ref.General->MoreCharacter->MinR_Attack||
			dst>OBJ->Ref.General->MoreCharacter->MaxR_Attack){
			OBJ->DeleteLastOrder();
			return;
		};
			for(int i=0;i<N;i++){
				if(!OBJ->TempDelay[i]){
					int x=xx+NMN->ShotPtX[i];
					int y=NMN->ShotPtY[i];
					int z=NMN->ShotPtZ[i];
					int dz=NMN->ShotPtYG[i]-y-z;
					y=yy+((y+z)<<1);
					z+=OBJ->RZ;
					char dir=NMN->ShotDir[i];
					byte diff=NMN->ShotDiff[i];
					int minr=NMN->ShotMinR[i];
					int maxr=NMN->ShotMaxR[i];

					int rx=OB->RealX>>4;
					int ry=OB->RealY>>4;
						
					int dst=Norma(x-rx,y-ry);
					char rdir=char(GetDir(rx-x,ry-y));
					int ddir=abs(char(rdir-dir));
					if(dst>=minr&&dst<=maxr&&ddir<diff){
						//can shot
						Weapon* Weap=NMN->DamWeap[NMN->ShotAttType[i]];
						if(Weap){
};
*/
int TestCapture(OneObject* OBJ);
void AttackObjLink(OneObject* OBJ){
	OBJ->PrioryLevel=OBJ->LocalOrder->PrioryLevel;
	byte prl=OBJ->PrioryLevel;
	NewMonster* NMN=OBJ->newMons;
	int RMAX=100000000;
	if(OBJ->LockType==1&&OBJ->Nat->AI_Enabled&&NMN->AI_PreferredAttR_Max){
		RMAX=NMN->AI_PreferredAttR_Max;
	};
	AdvCharacter* ADC=OBJ->Ref.General->MoreCharacter;
#ifndef COSSACKS2
	if(OBJ->LockType!=1)OBJ->DestX=-1;
#endif
	//OBJ->UnBlockUnit();
	word OID=OBJ->EnemyID;
	word OSN=OBJ->EnemySN;
	if(OID>=ULIMIT&&!AttGroundMod){
		OBJ->DeleteLastOrder();
		return;
	};
	OneObject TMPOBJ;
	OneObject* OB;
#ifdef SIMPLEMANAGE
	bool ARMONLY=OBJ->ArmAttack&&!(OBJ->RifleAttack||AttGroundMod);
	bool SHOTONLY=OBJ->RifleAttack&&!(OBJ->ArmAttack||AttGroundMod);
#else //SIMPLEMANAGE
	bool ARMONLY=OBJ->ArmAttack;
#endif //SIMPLEMANAGE
	
	if(AttGroundMod){
		memset(&TMPOBJ,0,sizeof OneObject);
		OB=&TMPOBJ;
		TMPOBJ.RealX=OBJ->DstX;
		TMPOBJ.RealY=OBJ->DstY;
		TMPOBJ.x=OBJ->DstX>>8;
		TMPOBJ.y=OBJ->DstY>>8;
		TMPOBJ.RZ=GetHeight(OBJ->RealX>>4,OBJ->RealY>>4);
		TMPOBJ.Index=0xFFFF;
		TMPOBJ.Serial=0xFFFF;
		TMPOBJ.Ref.General=NATIONS[7].Mon[0];
		TMPOBJ.newMons=NATIONS[7].Mon[0]->newMons;
		TMPOBJ.NIndex=7;
		OID=0xFFFF;
		OSN=0xFFFF;
	}else OB=Group[OID];
	if(NMN->CompxUnit&&NMN->CompxUnit->CanTakeExRes){
		if(OB&&OB->Sdoxlo&&OB->Serial==OSN&&OB->newMons->MeatTransformIndex!=0xFF){
			ObjCharacter* OCR=SPECIAL.ObjChar+OB->newMons->MeatTransformIndex;
			int rt=OCR->ResType;
			if(rt>=0&&rt<16){
				if(NMN->CompxUnit->TakeResStage[rt]){
					OBJ->DeleteLastOrder();
					for(int i=0;i<60;i++)OBJ->TakeResource(OB->RealX>>4,OB->RealY>>4,rt,16+128,1);
					OBJ->NewMonsterSendTo(OB->RealX,OB->RealY,16+128,1);
					return;
				};
			};
		};
	};
	if(((!OB)||(OB->Sdoxlo>8)||OSN!=OB->Serial)||OBJ->TurnOff){
		if(OBJ->PrioryLevel>=16&&OBJ->NNUM==MyNation&&OBJ->TurnOff){
			char ccc[128];
			sprintf_s(ccc,sizeof(ccc),GetTextByID("CSNOGL"),OBJ->Ref.General->Message);
			AssignHint1(ccc,50);
		};
		OBJ->DeleteLastOrder();
		return;
	};
	if(ARMONLY&&OB->NewBuilding)ARMONLY=0;
#ifdef SIMPLEMANAGE
	if(SHOTONLY&&OB->NewBuilding)SHOTONLY=0;
#endif //SIMPLEMANAGE
	if(OBJ->newMons->Shaman&&!(OBJ->NMask&OB->NMask))ARMONLY=1;
	if(NMN->Priest){
		if(!OB->NMask&OBJ->NMask){
			OBJ->DeleteLastOrder();
			return;
		};
	}else{
		if(!NMN->Shaman){
			if(OB->NMask&OBJ->NMask){
				OBJ->DeleteLastOrder();
				return;
			};
		};
	};
	
	
	byte maska=OB->newMons->MathMask;
	if(AttGroundMod)maska=0xFF;
	bool killins=0;
	if(OB->newMons->CanBeKilledInside){
		if(NMN->CanFire){
			if(OB->InFire&&OB->NInside)killins=1;
		}else if(OB->NInside)killins=1;
	};
	short OLDX=OBJ->LocalOrder->info.BuildObj.ObjX;
	short OLDY=OBJ->LocalOrder->info.BuildObj.ObjY;
	word NTIMES=OBJ->LocalOrder->info.BuildObj.SN;
	short NEWX=OB->RealX>>8;
	short NEWY=OB->RealY>>8;
	//--------------BUILDING ATTACK------------
    if(OBJ->NewBuilding){
		if(OBJ->delay){ 
			//OBJ->delay--;
			return;
		};
		if(NMN->ShotDir){
			if(!OBJ->NInside){
				OBJ->DeleteLastOrder();
				return;
			};
			bool UseEvro=0;
			if(OBJ->Inside){
				word MID=OBJ->Inside[0];
				if(MID!=0xFFFF){
					OneObject* IOB=Group[MID];
					if(IOB)UseEvro=IOB->newMons->EuropeanMask;
				};
			};
			int xx,yy;
			OBJ->GetCornerXY(&xx,&yy);
			xx<<=4;
			yy<<=4;
			int N=NMN->NShotPt;
			int dst=Norma(OBJ->RealX-OB->RealX,OBJ->RealY-OB->RealY)>>4;
			if(dst<=OBJ->Ref.General->MoreCharacter->MinR_Attack||
				dst>OBJ->Ref.General->MoreCharacter->MaxR_Attack){
				OBJ->DeleteLastOrder();
				return;
			};
			int ATTTP=OBJ->newMons->EuropeanMask;
			if(ATTTP==100)UseEvro=0;

			bool COMPSEARCH=OB->NewBuilding;
			bool DELAYPROBLEM=0;
			for(int i=0;i<N;i++){
				if(!OBJ->TempDelay[i]){
					int x=xx+NMN->ShotPtX[i];
					int y=NMN->ShotPtY[i];
					int z=NMN->ShotPtZ[i];
					int dz=NMN->ShotPtYG[i]-y-z;
					y=yy+((y+z)<<1);
					z+=OBJ->RZ;
					char dir=NMN->ShotDir[i];
					byte diff=NMN->ShotDiff[i];
					int minr=NMN->ShotMinR[i];
					int maxr=NMN->ShotMaxR[i];

					int rx=OB->RealX>>4;
					int ry=OB->RealY>>4;
						
					int dst=Norma(x-rx,y-ry);
					char rdir=char(GetDir(rx-x,ry-y));
					int ddir=abs(char(rdir-dir));
					if(dst>=minr&&dst<=maxr&&ddir<diff){
						//can shot
						int sat=NMN->ShotAttType[i];
						if(((!UseEvro)&&sat!=ATTTP)||(UseEvro&&sat==ATTTP)){
							Weapon* Weap=NMN->DamWeap[sat];
							if(Weap){
								Coor3D C3D;
								word DestObj=0xFFFF;
								if(OB->GetDamagePoint(OBJ->x,OBJ->y,&C3D,0))DestObj=OID;
								int wx0=x;
								int wy0=y;
								int wz0=z;
								int wx1=C3D.x;
								int wy1=C3D.y;
								int wz1=C3D.z;
								int WRes=-1;
								/*
								if(!NMN->SlowRecharge){
									WRes=PredictShot(Weap,wx0,wy0,wz0,wx1,wy1,wz1,OBJ->Index);
								};
								if(WRes!=-1){
									char atts[128];
									//something on the way...
									if(WRes>8192){
										//ground
										if(OBJ->NNUM==MyNation){
											sprintf(atts,MESHPOV,OBJ->newMons->Message);//"%s : Не могу стрелять. Мешает поверхность земли."
											AssignHint1(atts,48);
										};
									}else{
										OneObject* OBX=Group[WRes];
										if(OBX){
											if(ShotRecommended&&!(OBX->NMask&OBJ->NMask))goto DoShotA;
											if(OBJ->NNUM==MyNation){
												sprintf(atts,MESHOBJ,OBJ->newMons->Message,OBX->newMons->Message);//"%s : Не могу стрелять. Мешает %s"
												AssignHint1(atts,48);
											};
										};
									};
									OBJ->delay=100;
									addname(OBJ->Index);
									if(OBJ->delay>OBJ->MaxDelay)OBJ->delay=OBJ->MaxDelay;
									addrand(OBJ->delay);
									OBJ->DeleteLastOrder();
								};
								*/
	DoShotA:;
                                bool AllowShot=true;
								if(NMN->NShotRes){
									for(int k=0;k<NMN->NShotRes;k++){
										if(XRESRC(OBJ->NNUM,NMN->ShotRes[k+k])<NMN->ShotRes[k+k+1])AllowShot=false;
									};
									if(AllowShot){
										for(int k=0;k<NMN->NShotRes;k++){
											AddXRESRC(OBJ->NNUM,NMN->ShotRes[k+k],-NMN->ShotRes[k+k+1]);
											OBJ->Nat->AddResource(byte(NMN->ShotRes[k+k]),-NMN->ShotRes[k+k+1]);
											OBJ->Nat->ResOnLife[NMN->ShotRes[k+k]]+=NMN->ShotRes[k+k+1];
										};
									}else{
										if(OBJ->NNUM==MyNation&&OBJ->PrioryLevel>=16){
											char ccc[150];
											sprintf_s(ccc,sizeof(ccc),GetTextByID("CSNORS"),OBJ->Ref.General->Message);
											AssignHint1(ccc,64);
										};
										OBJ->DeleteLastOrder();
										return;
									};
								};	
		
								if(AllowShot){
									CreateRazbros(OBJ,&C3D);
									Create3DAnmObjectEX(Weap,wx0,wy0,wz0,wx1,wy1,wz1,OBJ,NMN->ShotAttType[i],DestObj,0,0,0,dz);
									//determining the pause
									if(OBJ->NInside){
										OBJ->delay=ADC->AttackPause[0];
										OBJ->delay=(int(OBJ->delay)*OBJ->PersonalDelay)/(100*OBJ->NInside);
										OBJ->MaxDelay=OBJ->delay;
										OBJ->TempDelay[i]=OBJ->newMons->ShotDelay[i];
									}else{
										OBJ->delay=ADC->AttackPause[0];
										OBJ->delay=(int(OBJ->delay)*OBJ->PersonalDelay)/100;
										OBJ->MaxDelay=OBJ->delay;
										OBJ->DeleteLastOrder();
										return;
									};
									if(NTIMES&&OBJ->LocalOrder){
										OBJ->LocalOrder->info.BuildObj.SN--;
										if(NTIMES==1){
											OBJ->DeleteLastOrder();
											if(OBJ->LocalOrder)OBJ->LocalOrder->DoLink(OBJ);
											return;
										};
									};
									return;
								};
                               };
							OBJ->DeleteLastOrder();
							if(OBJ->LocalOrder)OBJ->LocalOrder->DoLink(OBJ);
							return;
						};
					};
				}else{
					if(COMPSEARCH){
						int x=xx+NMN->ShotPtX[i];
						int y=NMN->ShotPtY[i];
						int z=NMN->ShotPtZ[i];
						int dz=NMN->ShotPtYG[i]-y-z;
						y=yy+((y+z)<<1);
						z+=OBJ->RZ;
						char dir=NMN->ShotDir[i];
						byte diff=NMN->ShotDiff[i];
						int minr=NMN->ShotMinR[i];
						int maxr=NMN->ShotMaxR[i];

						int rx=OB->RealX>>4;
						int ry=OB->RealY>>4;
							
						int dst=Norma(x-rx,y-ry);
						char rdir=char(GetDir(rx-x,ry-y));
						int ddir=abs(char(rdir-dir));
						if(dst>=minr&&dst<=maxr&&ddir<diff){
							DELAYPROBLEM=1;
						};
					};
				};
			};
			if(!DELAYPROBLEM){
				OBJ->DeleteLastOrder();
				if(OBJ->LocalOrder)OBJ->LocalOrder->DoLink(OBJ);
			};
			return;
		};
        
        if(!NMN->NShotPt){
            OBJ->DeleteLastOrder();
            return;
        };
        int dst=(Norma(OBJ->RealX-OB->RealX,OBJ->RealY-OB->RealY)>>4);
#ifdef COSSACKS2
		int D=OBJ->RZ-OB->RZ+int(OBJ->newMons->SrcZPoint)-int(OB->newMons->SrcZPoint);
		if(D>0)D=0;
		int dst1=dst-(D<<2);
#else //COSSACKS2
		int dst1=dst-(int(OBJ->RZ+OBJ->newMons->SrcZPoint-(OB->RZ+OB->newMons->SrcZPoint))<<1);
#endif //COSSACKS2
        if(dst<ADC->AttackRadius1[0]||dst1>ADC->AttackRadius2[0]){
            OBJ->DeleteLastOrder();
            return;
        };
        int xx,yy,x0,y0;
        OBJ->GetCornerXY(&xx,&yy);
        xx<<=8;
        yy<<=8;
        int mindis=1000000;
        int bsp=0;
        int np=NMN->NShotPt;
        int xx1=OB->RealX;
        int yy1=OB->RealY;
		int zzz=NMN->SrcZPoint;
		int DZZ=OBJ->RZ;
        for(int i=0;i<np;i++){
            x0=xx+(NMN->ShotPtX[i]<<4);
            y0=yy+(Prop43(NMN->ShotPtY[i])<<4);
            dst=Norma(x0-xx1,y0-yy1);
            if(dst<mindis){
                mindis=dst;
                bsp=i;
            };
        };
        x0=xx+(NMN->ShotPtX[bsp]<<4);
        y0=yy+(Prop43(NMN->ShotPtY[bsp])<<4)+(zzz<<5);
		Weapon* Weap=NMN->DamWeap[0];
        if(Weap){
			Coor3D C3D;
			word DestObj=0xFFFF;
			if(OB->GetDamagePoint(OBJ->x,OBJ->y,&C3D,0))DestObj=OID;
			int wx0=x0>>4;
			int wy0=(y0>>4)+6;
			int wz0=3+DZZ+zzz;
			int wx1=C3D.x;
			int wy1=C3D.y;
			int wz1=C3D.z;
			int WRes=PredictShot(Weap,wx0,wy0,wz0,wx1,wy1,wz1,OBJ->Index);
			if(WRes!=-1){
				char atts[128];
				//something on the way...
				if(WRes>8192){
					//ground
					if(OBJ->NNUM==MyNation){
						sprintf(atts,MESHPOV,OBJ->newMons->Message);//"%s : Не могу стрелять. Мешает поверхность земли."
						AssignHint1(atts,48);
					};
				}else{
					OneObject* OBX=Group[WRes];
					if(OBX){
						if(ShotRecommended&&!(OBX->NMask&OBJ->NMask))goto DoShot0;
						if(OBJ->NNUM==MyNation){
							sprintf(atts,MESHOBJ,OBJ->newMons->Message,OBX->newMons->Message);//"%s : Не могу стрелять. Мешает %s"
							AssignHint1(atts,48);
						};
					};
				};
				OBJ->delay=100;
				addname(OBJ->Index);
				if(OBJ->delay>OBJ->MaxDelay)OBJ->delay=OBJ->MaxDelay;
				addrand(OBJ->delay);
				OBJ->DeleteLastOrder();
				return;
			};
        DoShot0:;
			bool AllowShot=true;
			if(NMN->NShotRes){
				for(int k=0;k<NMN->NShotRes;k++){
					if(XRESRC(OBJ->NNUM,NMN->ShotRes[k+k])<NMN->ShotRes[k+k+1])AllowShot=false;
				};
				if(AllowShot){
					for(int k=0;k<NMN->NShotRes;k++){
						AddXRESRC(OBJ->NNUM,NMN->ShotRes[k+k],-NMN->ShotRes[k+k+1]);
						OBJ->Nat->AddResource(byte(NMN->ShotRes[k+k]),-NMN->ShotRes[k+k+1]);
						OBJ->Nat->ResOnLife[NMN->ShotRes[k+k]]+=NMN->ShotRes[k+k+1];
					};
				}else{
					if(OBJ->NNUM==MyNation&&OBJ->PrioryLevel>=16){
						char ccc[150];
						sprintf_s(ccc,sizeof(ccc),GetTextByID("CSNORS"),OBJ->Ref.General->Message);
						AssignHint1(ccc,64);
					};
					OBJ->DeleteLastOrder();
					return;
				};
			};

			if(AllowShot){
				CreateRazbros(OBJ,&C3D);
				Create3DAnmObject(Weap,wx0,wy0,wz0,wx1,wy1,wz1,OBJ,0,DestObj);
				OBJ->delay=ADC->AttackPause[0];
				addrand(OBJ->delay);
				OBJ->MaxDelay=div(int(OBJ->delay)*OBJ->PersonalDelay,100).quot;
				OBJ->delay=OBJ->MaxDelay;
				addrand(OBJ->delay);
				if(NTIMES&&OBJ->LocalOrder){
					OBJ->LocalOrder->info.BuildObj.SN--;
					if(NTIMES==1){
						OBJ->DeleteLastOrder();
						return;
					};
				};
			};
		};
                        return;
                    };
	//--------------END BUILDING ATTACK------------
	addname(OBJ->Index);
	addrand(OBJ->NewState);
	addrand(OBJ->delay);
	addrand(OBJ->NewCurSprite);

	int nst=OBJ->NewState;
	bool DELAY;
	bool CANSETDELAY=1;
	bool NEEDSLOWRECHARGE=0;
#ifdef CONQUEST
	DELAY=OBJ->delay!=0;
	if(DELAY){
		if(NMN->NoWaitMask&(1<<(nst-1)))DELAY=0;
		CANSETDELAY=0;
	};
#else
	DELAY=OBJ->delay;
#endif
	if(DELAY&&NMN->SlowRecharge){
		NEEDSLOWRECHARGE=1;
	};
	if(nst&&!DELAY){
		//attack animation to shoot
		NewAnimation* Anm=&NMN->Attack[OBJ->NewState-1];
		addrand(Anm->Enabled);
		if(Anm->Enabled){
			addname(OBJ->Index);
			int af=Anm->ActiveFrame;
			if(!af)af=1;
			if(FrmDec==2&&(af&1))af++;
			if(OBJ->NewAnm==Anm&&OBJ->NewCurSprite==af){
				if(NMN->Usage==PushkaID&&OBJ->NewState==2&&prl<10){
					if(!TestUnitsInSectorForGrenade(OBJ)){
						OBJ->DeleteLastOrder();
						return;
					};
				};
				//checking the distance
				int x0=OB->RealX;
				int y0=OB->RealY;
				if(AttGroundMod){
					int R=Norma(x0-OBJ->RealX,y0-OBJ->RealY);
					x0=OBJ->RealX+((int(TCos[OBJ->RealDir])*R)>>8);
					y0=OBJ->RealY+((int(TSin[OBJ->RealDir])*R)>>8);
					OB->RealX=x0;
					OB->RealY=y0;
				};
				char EnDir=char(GetDir(x0-OBJ->RealX,y0-OBJ->RealY));
				char ddir=EnDir-OBJ->RealDir;
				OBJ->RealDir=EnDir;//BUG!!!!
				OBJ->GraphDir=OBJ->RealDir;
				int dr1=ADC->AttackRadius1[OBJ->NewState-1];
				int dr2=ADC->AttackRadius2[OBJ->NewState-1]+
					NMN->AttackRadiusAdd[OBJ->NewState-1];
				int DPAUS=100;
				if(OBJ->NewState/*==1*/&&AttGroundMod){
					dr2=(ADDRANGE*dr2)/100;
					DPAUS=ADDPAUSE;
				};
				int MyZ=OBJ->RZ+NMN->SrcZPoint;
				int EnZ=OB->RZ+OB->newMons->DstZPoint;
				int dst=(Norma(x0-OBJ->RealX,y0-OBJ->RealY)>>4);
				int dst1=dst-OB->newMons->AddShotRadius;
				if(dr2>RMAX)dr2=RMAX;
				byte Wpk=NMN->WeaponKind[OBJ->NewState-1];
				if(WeaponFlags[Wpk]&1)dst1-=((MyZ-EnZ)<<1);
				//int angl=0;
				//if(dst>0)angl=div((EnZ-MyZ)<<6,dst).quot;
				//dst-=(MyZ-EnZ)>>1;
#ifdef COSSACKS2
				{
#else
				if(dst>=dr1&&dst1<=dr2||AttGroundMod){
#endif
					Weapon* WP=NMN->DamWeap[OBJ->NewState-1];
					if(CANSETDELAY){
						if(NMN->MaxZalp&&WP){
							if(OBJ->NZalp>0)OBJ->NZalp--;
							else{
								OBJ->delay=ADC->AttackPause[OBJ->NewState-1];
								OBJ->NZalp=NMN->MaxZalp;
							};
						}else OBJ->delay=ADC->AttackPause[OBJ->NewState-1];
						addrand(OBJ->delay);
						addrand(OBJ->NewState);
						if(OBJ->delay){
							OBJ->MaxDelay=div(int(OBJ->delay)*OBJ->PersonalDelay,100).quot;
							OBJ->MaxDelay=(int(OBJ->MaxDelay)*DPAUS)/100;
							OBJ->delay=OBJ->MaxDelay;
							addrand(OBJ->delay);
						};
					};
					
					//pubuxxxxxxxxx...
					if(WP){
						//----------
						NewAnimation* UNAM=OBJ->NewAnm;
						int NR=UNAM->Rotations;
						int NWP=1;
						int SWS=-NR;
						if(UNAM->DoubleShot){
							NWP=1+UNAM->DoubleShot;
						};
						for(int q=0;q<NWP;q++){
							SWS+=NR;
							Coor3D C3D;
							word DestObj=0xFFFF;
							int x0,y0,z0,x1,y1,z1,uocts,uoc2;
							byte udir;	

							if(NR&1||!NR){
								int csp=OBJ->NewCurSprite;
								uocts=(UNAM->Rotations-1)*2;
								uoc2=UNAM->Rotations-1;
								if(!uocts)uocts=1;
								int usesize=div(255,uocts<<1).quot;
								udir=(((OBJ->RealDir+64+usesize)&255)*uocts)>>8;
								NewFrame* NF=&UNAM->Frames[csp];
								if(udir<uoc2){
									int udir1=uoc2-udir;
									x0=(OBJ->RealX>>4)-(UNAM->ActivePtX[udir1+SWS]+NF->dx);
									y0=(OBJ->RealY>>4)+Prop43(UNAM->ActivePtY[udir1+SWS]+NF->dy+NMN->SrcZPoint);
								}else{
									int udir1=uoc2-uocts+udir;
									x0=(OBJ->RealX>>4)+(UNAM->ActivePtX[udir1+SWS]+NF->dx);
									y0=(OBJ->RealY>>4)+Prop43(UNAM->ActivePtY[udir1+SWS]+NF->dy+NMN->SrcZPoint);
								};
							}else{
								int csp=OBJ->NewCurSprite;
								uocts=NR;
								uoc2=NR;
								if(!uocts)uocts=1;
								int usesize=128/uocts;
								udir=((OBJ->RealDir-64+usesize)&255)/NR;
								NewFrame* NF=&UNAM->Frames[csp];
								x0=(OBJ->RealX>>4)+(UNAM->ActivePtX[udir+SWS]+NF->dx);
								y0=(OBJ->RealY>>4)+Prop43(UNAM->ActivePtY[udir+SWS]+NF->dy+NMN->SrcZPoint);
							};
							//----------
							NewAnimation* NAM=WP->NewAnm;
							int octs=(NAM->Rotations-1)*2;
							int oc2=NAM->Rotations-1;
							if(!octs)octs=1;
							int sesize=div(255,octs<<1).quot;
							byte dir=(((OBJ->RealDir+64+sesize)&255)*octs)>>8;
							if(OB->GetDamagePoint(OBJ->x,OBJ->y,&C3D,0))DestObj=OID;
							DestObj=0xFFFF;//temp
							if(OB->Wall)DestObj=OID;
							//OBJ->BlockUnit();
							
							z0=OBJ->RZ+NMN->SrcZPoint;
							x1=C3D.x;
							y1=C3D.y;
							z1=C3D.z;
							int WRes=PredictShot(WP,x0,y0,z0,x1,y1,z1,OBJ->Index);
							if(WRes!=-1){
								char atts[128];
								//something on the way...
								if(WRes>=ULIMIT){
									//ground
									if(OBJ->NNUM==MyNation){
										sprintf(atts,MESHPOV,OBJ->newMons->Message);
										AssignHint1(atts,48);
									};
								}else{
									if(WRes==OB->Index)goto DoShot;
									OneObject* OBX=Group[WRes];
									if(OBX){
										if(ShotRecommended&&!(OBX->NMask&OBJ->NMask))goto DoShot;
										if(OBJ->NNUM==MyNation){
											sprintf(atts,MESHOBJ,OBJ->newMons->Message,OBX->newMons->Message);
											AssignHint1(atts,48);
										};
									};
								};
								//attempt to move
								if(!OBJ->StandGround){
									int dxx=y1-y0;
									int dyy=x0-x1;
									int nn=Norma(dxx,dyy);
									dxx=(dxx*20)/nn;
									dyy=(dyy*20)/nn;
									bool LAllow=true;
									bool RAllow=true;
									int XL=x0;
									int YL=y0;
									int XR=x0;
									int YR=y0;
									for(int j=0;j<10&&(LAllow||RAllow);j++){
										XL+=dxx;
										YL+=dyy;
										XR-=dxx;
										YR-=dyy;
										if(CheckPt(XL>>4,YL>>4))LAllow=false;
										if(CheckPt(XR>>4,YR>>4))RAllow=false;
										if(LAllow&&PredictShot(WP,XL,YL,z0,x1,y1,z1,OBJ->Index)==-1){
											OBJ->NewMonsterSendTo((XL+dxx)<<4,(YL+dyy)<<4,128+16,1);
											return;
										};
										if(RAllow&&PredictShot(WP,XR,YR,z0,x1,y1,z1,OBJ->Index)==-1){
											OBJ->NewMonsterSendTo((XR-dxx)<<4,(YR-dyy)<<4,128+16,1);
											return;
										};
										if(CheckPt(XL>>4,YL>>4))LAllow=false;
										if(CheckPt(XR>>4,YR>>4))RAllow=false;
									};
									if(CANSETDELAY){
										OBJ->delay=16;
										addname(OBJ->Index);
										if(OBJ->delay>OBJ->MaxDelay)OBJ->delay=OBJ->MaxDelay;
										addrand(OBJ->delay);
									};
									OBJ->DeleteLastOrder();
									return;
								}else{
									OBJ->DeleteLastOrder();
									return;
								};
							};
DoShot:;					
							bool AllowShot=true;
							if(!AllowShot)return;
							//shot occurs there
							if(udir<uoc2){
								udir=uoc2-udir;
								CreateRazbros(OBJ,&C3D);
								x1=C3D.x;
								y1=C3D.y;
								z1=C3D.z;//+NMN->DstZPoint;
								Create3DAnmObject(WP,x0,y0,z0,x1,y1,z1,OBJ,OBJ->NewState-1,DestObj);
								if(NMN->WSECT&&OBJ->NewState-1==NMN->WSECT->AttIndex){
									DamageUnitsInAngle(OBJ->RealX>>4,OBJ->RealY>>4,NMN->WSECT,OBJ->RealDir,OBJ,OBJ->NewState-1);
								};
								//CreateExObjD(OBJ->Weap,(OBJ->RealX>>2)-((UNAM->ActivePtX[udir]+NF->dx)<<2),(OBJ->RealY>>2)+((UNAM->ActivePtY[udir]+NF->dy)<<2),OBJ->RealDir,0,0,NULL);
							}else{
								udir=uoc2-uocts+udir;
								CreateRazbros(OBJ,&C3D);
								x1=C3D.x;
								y1=C3D.y;
								z1=C3D.z;//+NMN->DstZPoint;
								addrand(78);
								Create3DAnmObject(WP,x0,y0,z0,x1,y1,z1,OBJ,OBJ->NewState-1,DestObj);
								if(NMN->WSECT&&OBJ->NewState-1==NMN->WSECT->AttIndex){
									DamageUnitsInAngle(OBJ->RealX>>4,OBJ->RealY>>4,NMN->WSECT,OBJ->RealDir,OBJ,OBJ->NewState-1);
								};
								//CreateExObjD(OBJ->Weap,(OBJ->RealX>>2)+((UNAM->ActivePtX[udir]+NF->dx)<<2),(OBJ->RealY>>2)+((UNAM->ActivePtY[udir]+NF->dy)<<2),OBJ->RealDir,0,0,NULL);
							};
							if(NMN->SingleShot&(1<<(OBJ->NewState-1)))NTIMES=1;
							if(NTIMES&&OBJ->LocalOrder){
								OBJ->LocalOrder->info.BuildObj.SN--;
								if(NTIMES==1){
									OBJ->DeleteLastOrder();
									return;
								};
							};
						};
	                 }else{
#ifdef NEWMORALEPRIEST
                         if (NMN->Priest || (NMN->Shaman && OB->NMask & OBJ->NMask)) {
                             if (NMN->MoraleHealer) {
                                 if (OB->Morale < OB->MaxMorale) {
                                     if (NMN->UnitMoralist) OB->Morale += (OBJ->Ref.General->MoreCharacter->MaxDamage[0]) * 10000;
                                     if (NMN->FormationMoralist) OB->PriestMorale = (OBJ->Ref.General->MoreCharacter->MaxDamage[0]) * 10000;
                                     //lokalnimoral=1000000;
                                     //OB->MaxMorale=lokalnimoral;
                                     if (OB->Morale >= OB->MaxMorale) OB->Morale = OB->MaxMorale;
                                 }
                                 else {
                                     OBJ->DeleteLastOrder();
                                     return;
                                 };
                             }
                             else {
                                 if (OB->Life < OB->MaxLife) {
                                     OB->Life += OBJ->Ref.General->MoreCharacter->MaxDamage[0];
                                     if (OB->Life >= OB->MaxLife) {
                                         OB->Life = OB->MaxLife;
                                         OB->InFire = 0;
                                         OB->FireOwner = 0xFF;
                                         OB->FiringStage = 0;
                                     };
                                 }
                                 else {
                                     OBJ->DeleteLastOrder();
                                     return;
                                 };
                             }
                         }
#else
                         if (NMN->Priest || (NMN->Shaman && OB->NMask & OBJ->NMask)) {
                             if (OB->Life < OB->MaxLife) {
                                 OB->Life += OBJ->Ref.General->MoreCharacter->MaxDamage[0];;
                                 if (OB->Life >= OB->MaxLife) {
                                     OB->Life = OB->MaxLife;
                                     OB->InFire = 0;
                                     OB->FireOwner = 0xFF;
                                     OB->FiringStage = 0;
                                 };
                             }
                             else {
                                 OBJ->DeleteLastOrder();
                                 return;
                             };
                         }
                         #endif
                         else{
	                        OB->MakeDamage(0,ADC->MaxDamage[OBJ->NewState-1],OBJ,OBJ->NewState-1);
							AddEffect(OBJ->RealX>>4,(OBJ->RealY>>5)-OBJ->RZ,NMN->StrikeSoundID);
							if(OB->newMons->Usage!=PushkaID)OB->AttackObj(OBJ->Index,2);
							//OBJ->BlockUnit();
						};
						if(NMN->SingleShot&(1<<(OBJ->NewState-1)))NTIMES=1;
						if(NTIMES&&OBJ->LocalOrder){
							OBJ->LocalOrder->info.BuildObj.SN--;
							if(NTIMES==1){
								OBJ->DeleteLastOrder();
								return;
							};
						};
                    };
					return;
				};
			};
		};
	};
	if(OBJ->NewCurSprite>=OBJ->NewAnm->NFrames-FrmDec||(OBJ->NewAnm->CanBeBroken&&OBJ->NewAnm!=&NMN->Attack[OBJ->NewState-1])){
		if(NMN->ArmAttack&&NMN->NShotRes){
			int n=NMN->NShotRes;
			for(int i=0;i<n;i++){
				int rid=NMN->ShotRes[i+i];
				int ram=NMN->ShotRes[i+i+1];
				if(XRESRC(OBJ->NNUM,rid)<ram)ARMONLY=1;
			};
		};
	
		addname(OBJ->Index);
		if(DELAY){
			NewAnimation* Anm=&NMN->Attack[OBJ->NewState-1];
			if(Anm->Enabled){
				addname(OBJ->Index);
				if(OBJ->NewAnm==Anm){
					if(NMN->SlowRecharge){
						int ns=OBJ->NewState;
						if(ns){
							if(ns==4||WeaponFlags[NMN->WeaponKind[ns-1]]&1){
								OBJ->NewState=4;
								OBJ->LocalNewState=4;
							}else TryToStand(OBJ,false);
						}else TryToStand(OBJ,false);
					}else TryToStand(OBJ,false);
				};
			};
		}else{
			if(NMN->SlowRecharge){
				NewAnimation* Anm=&NMN->Attack[OBJ->NewState-1];
				if(Anm->Enabled&&OBJ->NewAnm==Anm){
					int ns=OBJ->NewState;
					if(ns){
						if(ns==4||WeaponFlags[NMN->WeaponKind[ns-1]]&1){
							OBJ->NewState=4;
							OBJ->LocalNewState=4;
						}else TryToStand(OBJ,false);
					}else TryToStand(OBJ,false);
				}else TryToStand(OBJ,false);
			};
		};
		if(!(OBJ->NewCurSprite>=OBJ->NewAnm->NFrames-FrmDec||(OBJ->NewAnm->CanBeBroken&&OBJ->NewAnm!=&NMN->Attack[OBJ->NewState-1])))return;
		
		//animation has finished and now we can analyse all
		//determining the need state
		int x0=OB->RealX;
		int y0=OB->RealY;
		int dst=Norma(x0-OBJ->RealX,y0-OBJ->RealY)>>4;
		//assert(dst);
		int MyZ=OBJ->RZ+NMN->SrcZPoint;
		int EnZ=OB->RZ+OB->newMons->DstZPoint;
		int dst1=dst;
#ifdef COSSACKS2
		int D=MyZ-EnZ;
		if(D>0)D=0;
		int dstx=dst-((MyZ-EnZ)<<2);
#else
		int dstx=dst-((MyZ-EnZ)<<1);
#endif
		//dst+=(EnZ-MyZ)>>1;
		int NeedState=-1;
		addrand(1);
#ifdef SIMPLEMANAGE
		if(SHOTONLY&&!killins){
			NeedState=1;
		}else
#endif //SIMPLEMANAGE
		if(ARMONLY&&!killins){
			NeedState=0;
			if(!NMN->AttackMask[0]&maska){
				OBJ->DeleteLastOrder();
				return;
			};
		}else{
			for(int i=0;i<NAttTypes;i++){
				int wpk=NMN->WeaponKind[i];
				if(WeaponFlags[wpk]&1){
					if(dst1>=ADC->DetRadius1[i]&&dstx<=ADC->DetRadius2[i]&&((NMN->AttackMask[i]&maska)||(killins&&NMN->DamWeap[i]))){
						NeedState=i;
					};
				}else{
					if(dst>=ADC->DetRadius1[i]&&dst<=ADC->DetRadius2[i]&&((NMN->AttackMask[i]&maska)||(killins&&NMN->DamWeap[i]))){
						NeedState=i;
					};
				};
			};
			if(OBJ->delay&&NMN->NoWaitMask){
				byte wpmask=NMN->NoWaitMask;
				for(int i=0;i<NAttTypes;i++)if(wpmask&(1<<i)){
					int wpk=NMN->WeaponKind[i];
					if(WeaponFlags[wpk]&1){
						if(dst1>=ADC->DetRadius1[i]&&dstx<=ADC->DetRadius2[i]&&((NMN->AttackMask[i]&maska)||(killins&&NMN->DamWeap[i]))){
							NeedState=i;
						};
					}else{
						if(dst>=ADC->DetRadius1[i]&&dst<=ADC->DetRadius2[i]&&((NMN->AttackMask[i]&maska)||(killins&&NMN->DamWeap[i]))){
							NeedState=i;
						};
					};
				};
			};
		};
		if(NeedState==-1){
			addrand(1);
			//if(OBJ->delay){
			OBJ->DeleteLastOrder();
			//if(!OBJ->StandGround)OBJ->NewMonsterSmartSendTo(OB->RealX>>4,OB->RealY>>4,0,0,OBJ->PrioryLevel,1);
			if(!TestCapture(OB))OBJ->NewMonsterSmartSendTo(OB->RealX>>4,OB->RealY>>4,0,0,OBJ->PrioryLevel,1);
			return;
			//}else{
			//	SetDestUnit(OBJ,OB,OLDX,OLDY,NEWX,NEWY);
			//	return;
			//};
			//search the attack with max. radius
			//int maxr=0;
			//for(int i=0;i<NAttTypes;i++){
			//	if(ADC->DetRadius2[i]>maxr){
			//		NeedState=i;
			//		maxr=ADC->DetRadius2[i];
			//	};
			//};
		};
		int wpk=NMN->WeaponKind[NeedState];
		if(WeaponFlags[wpk]&1){
			dst=dstx;
			if(NEEDSLOWRECHARGE){
				if(OBJ->LocalNewState!=4){
					OBJ->NewState=4;
					return;
				}else{
					NewAnimation* NRE=&NMN->Attack[3];
					if(OBJ->NewAnm!=NRE){
						if(OBJ->delay>NRE->NFrames)OBJ->delay-=NRE->NFrames;
						else OBJ->delay=0;
						OBJ->NewAnm=&NMN->Attack[3];
						OBJ->NewCurSprite=0;
						OBJ->InMotion=0;
						OBJ->BackMotion=0;
#ifdef COSSACKS2
						OBJ->DestX=-1;
#endif
						return;
					}else{
						if(OBJ->delay>NRE->NFrames){
							OBJ->delay-=NRE->NFrames;
							OBJ->NewAnm=&NMN->Attack[3];
							OBJ->NewCurSprite=0;
							OBJ->InMotion=0;
							OBJ->BackMotion=0;
#ifdef COSSACKS2
							OBJ->DestX=-1;
#endif

							//OB->delay-=NRE->NFrames;
							return;
						};
						OBJ->delay=0;
					};
				};
			};
		};
		//checking distance
		NewAnimation* NAMO=&NMN->PMotionL[NeedState];
		bool AMove=NAMO->Enabled;//&&!NMN->MaxAIndex;
		int MRot=NMN->MinRotator;
		int MRot1=MRot+(MRot>>1);
		if(MRot1>16)MRot1=16;
		int DR2=0;
		if(/*NeedState==0&&*/AttGroundMod){
			DR2=((ADDRANGE-100)*ADC->AttackRadius2[NeedState])/100;
		};
		DR2+=OB->newMons->AddShotRadius;
		if(dst1>=ADC->AttackRadius1[NeedState]&&dst<ADC->AttackRadius2[NeedState]+DR2&&dst<RMAX){
			addrand(1);
			//can attack at once
			//checking angle of rotation
			if(OBJ->LockType==1)OBJ->DestX=-1;
			char EnDir=char(GetDir(x0-OBJ->RealX,y0-OBJ->RealY));
			char ddir=EnDir-OBJ->RealDir;
			bool NewArt=OBJ->newMons->MotionStyle==3;
			if(NewArt){
				int enmd=((EnDir+8+512)>>4)&15;
				int pusd=((OBJ->RealDir+8+512)>>4)&15;
				if(enmd==pusd){
					ddir=0;
					//OBJ->RealDir=EnDir;
					if(OBJ->NewState==2){
						OBJ->NewState=0;
						OBJ->GroundState=0;
						TryToStand(OBJ,0);
						return;
					};
				}else{
					if(OBJ->LocalNewState==2){
						if(OBJ->NewAnm==&NMN->PMotionL[1]){
							OBJ->RealDir+=16;
							OBJ->GraphDir=OBJ->RealDir;
							TryToStand(OB,0);
							return;//
						}else 
						if(OBJ->NewAnm==&NMN->PMotionR[1]){
							TryToStand(OB,0);
							return;//
						}else{
							if(ddir>0){
								OBJ->NewAnm=&NMN->PMotionL[1];
								OBJ->NewCurSprite=0;
#ifdef COSSACKS2
								OBJ->DestX=-1;
#endif
								return;
							}else{
								OBJ->NewAnm=&NMN->PMotionR[1];
								OBJ->NewCurSprite=0;
								OBJ->RealDir-=16;
								OBJ->GraphDir=OBJ->RealDir;
#ifdef COSSACKS2
								OBJ->DestX=-1;
#endif
								return;
							};
						};
					}else{
						OBJ->NewState=2;
						OBJ->GroundState=2;
						TryToStand(OB,0);
						return;
					};
				};
			};
			if(abs(ddir)<MRot1){
				addrand(4);
				OBJ->RealDir=EnDir;
				OBJ->GraphDir=EnDir;
				if(OBJ->LocalNewState==NeedState+1||(OBJ->LockType==1&&OBJ->NewState==NeedState+1)){
					addrand(3);
					if(!DELAY){
						addrand(2);
						//checking attack ability
						Weapon* WP;
						if(OBJ->LockType==1)WP=NMN->DamWeap[OBJ->NewState-1];
						else WP=NMN->DamWeap[OBJ->LocalNewState-1];
						if(WP){
							Coor3D C3D;
							word DestObj=0xFFFF;
							if(OB->GetDamagePoint(OBJ->x,OBJ->y,&C3D,0))DestObj=OID;
							if(OB->Wall)DestObj=OID;
							//OBJ->BlockUnit();
							int x0,y0,z0,x1,y1,z1;
							x0=(OBJ->RealX>>4);
							y0=(OBJ->RealY>>4);
							z0=OBJ->RZ+NMN->SrcZPoint;
							x1=C3D.x;
							y1=C3D.y;
							z1=C3D.z;
							int WRes=OBJ->LocalNewState==3?-1:PredictShotEx(WP,x0,y0,z0,x1,y1,z1,OBJ->Index);
							if(WRes!=-1){
								if(OBJ->NewCurSprite==0){
									if(WRes<0xFFFF){
										OneObject* OB=Group[WRes];
										if(OB&&OB->NewBuilding&&OB->NInside&&!(OBJ->NMask&OB->NMask))goto DoShot1;
									};
									OBJ->NewAnm=&NMN->PStand[OBJ->LocalNewState-1];
									OBJ->DeleteLastOrder();
#ifdef COSSACKS2
									OBJ->DestX=-1;
#endif
									return;
								};
								char atts[128];
								//something on the way...
								if(WRes>=ULIMIT){
									//ground
									if(OBJ->NNUM==MyNation){
										sprintf(atts,MESHPOV,OBJ->newMons->Message);
										AssignHint1(atts,48);
									};
								}else{
									if(WRes==OB->Index)goto DoShot1;
									OneObject* OBX=Group[WRes];
									if(OBX){
										if(ShotRecommended&&!(OBX->NMask&OBJ->NMask))goto DoShot1;
										if(OBJ->NNUM==MyNation){
											sprintf(atts,MESHOBJ,OBJ->newMons->Message,OBX->newMons->Message);
											AssignHint1(atts,48);
										};
									};
								};
								//attempt to move
								addrand(2);
								if(!OBJ->StandGround){
									addrand(1);
									int dxx=y1-y0;
									int dyy=x0-x1;
									int nn=Norma(dxx,dyy);
									dxx=(dxx*20)/nn;
									dyy=(dyy*20)/nn;
									bool LAllow=true;
									bool RAllow=true;
									int XL=x0;
									int YL=y0;
									int XR=x0;
									int YR=y0;
									for(int j=0;j<10&&(LAllow||RAllow);j++){
										XL+=dxx;
										YL+=dyy;
										XR-=dxx;
										YR-=dyy;
										if(CheckPt(XL>>4,YL>>4))LAllow=false;
										if(CheckPt(XR>>4,YR>>4))RAllow=false;
										if(LAllow&&PredictShot(WP,XL,YL,z0,x1,y1,z1,OBJ->Index)==-1){
											OBJ->NewMonsterSendTo((XL+dxx)<<4,(YL+dyy)<<4,128+16,1);
											return;
										};
										if(RAllow&&PredictShot(WP,XR,YR,z0,x1,y1,z1,OBJ->Index)==-1){
											OBJ->NewMonsterSendTo((XR-dxx)<<4,(YR-dyy)<<4,128+16,1);
											return;
										};
										if(CheckPt(XL>>4,YL>>4))LAllow=false;
										if(CheckPt(XR>>4,YR>>4))RAllow=false;
									};
									if(CANSETDELAY){
										OBJ->delay=20;
										OBJ->MaxDelay=20;
										if(OBJ->delay>OBJ->MaxDelay)OBJ->delay=OBJ->MaxDelay;
										addrand(OBJ->delay);
									};
									OBJ->DeleteLastOrder();
									return;
								}else{
									OBJ->DeleteLastOrder();
									return;
								};
								if(CANSETDELAY){
									OBJ->delay=20;
									OBJ->MaxDelay=20;
									addname(OBJ->Index);
								};
							};
DoShot1:;
							if((NMN->ResAttType==OBJ->LocalNewState-1||NMN->ResAttType1==OBJ->LocalNewState-1)&&NMN->NShotRes){
								bool AllowShot=true;
								for(int k=0;k<NMN->NShotRes;k++){
									if(XRESRC(OBJ->NNUM,NMN->ShotRes[k+k])<NMN->ShotRes[k+k+1])AllowShot=false;
								};
								if(AllowShot){
									for(int k=0;k<NMN->NShotRes;k++){
										AddXRESRC(OBJ->NNUM,NMN->ShotRes[k+k],-NMN->ShotRes[k+k+1]);
										OBJ->Nat->AddResource(byte(NMN->ShotRes[k+k]),-NMN->ShotRes[k+k+1]);
										OBJ->Nat->ResOnLife[NMN->ShotRes[k+k]]+=NMN->ShotRes[k+k+1];
									};
								}else{
									//OBJ->delay=50;
									//OBJ->MaxDelay=50;
									//addname(OBJ->Index);
									if(OBJ->NNUM==MyNation&&OBJ->PrioryLevel>=16){
										char ccc[150];
										sprintf_s(ccc,sizeof(ccc),GetTextByID("CSNORS"),OBJ->Ref.General->Message);
										AssignHint1(ccc,64,32);
									};
									OBJ->DeleteLastOrder();
									return;
								};
							};
						};
						if(!DELAY){
							//attacks animation begins there
							if(OBJ->LockType==1)OBJ->NewAnm=&NMN->Attack[OBJ->NewState-1];
							else OBJ->NewAnm=&NMN->Attack[OBJ->LocalNewState-1];
							OBJ->InMotion=false;
							OBJ->BackMotion=false;
							OBJ->NewCurSprite=0;
#ifdef COSSACKS2
							OBJ->DestX=-1;
#endif
							return;
							//}else{
							//	addname(OBJ->Index);
							//	TryToStand(OBJ,0);
							//};
						};
					};
				}else{
					OBJ->NewState=NeedState+1;
					return;
				};
			}else if(!NewArt){
				addrand(5);
				if(!AMove){
					addrand(6);
					//if(abs(ddir)>MRot1){
					if(abs(ddir)>36){
						OBJ->NewState=OBJ->GroundState;
						addname(OBJ->Index);
						TryToStand(OBJ,false);
					};
					if(ddir>0)RotateMon(OBJ,MRot);//OBJ->RealDir+=16;
					else RotateMon(OBJ,-MRot);//OBJ->RealDir-=16;
				}else{
					addrand(31);
					if(ddir>0)RotateMon(OBJ,MRot);//OBJ->RealDir+=16;
					else RotateMon(OBJ,-MRot);//OBJ->RealDir-=16;
				};
			};
		}else{
			addrand(334);
			if(OBJ->StandGround){
				addrand(112);
				OBJ->DeleteLastOrder();
				return;
			};
			if(dst>ADC->AttackRadius2[NeedState]+DR2||dst>=RMAX){
				addrand(223);
				Order1* OR1=OB->LocalOrder;
				if(OBJ->Zombi&&OB->InMotion&&(OB->EnemyID==0xFFFF)){
					addrand(122);
					int dde=GetDir(OB->RealX-OBJ->RealX,OB->RealY-OBJ->RealY);
					char ddr=OB->RealDir-dde;
					if(abs(ddr)<64){
						addrand(132);
						OBJ->DeleteLastOrder();
						return;
					};
				};
				if(AMove){
					addrand(26);
					if(OBJ->NewState!=NeedState+1){
						addrand(289);
						OBJ->NewState=NeedState+1;
					}else{
						addrand(244);
						if(!SetDestUnit(OBJ,OB,OLDX,OLDY,NEWX,NEWY)){
							OBJ->DeleteLastOrder();
							return;
						};
					};
				}else{
					addrand(44);
					if(OBJ->NewState!=OBJ->GroundState){
						addrand(234);
						OBJ->NewState=OBJ->GroundState;
						addname(OBJ->Index);
						TryToStand(OBJ,false);
					}else{
						addrand(1123);
						if(!SetDestUnit(OBJ,OB,OLDX,OLDY,NEWX,NEWY)){
							OBJ->DeleteLastOrder();
							return;
						};
					};
				};
				return;
			}else{
				addrand(55);
				if(!OBJ->BackSteps){
					if(OBJ->BackReserv<32){
						addrand(33);
						OBJ->BackReserv++;
						OBJ->PrioryLevel=0;
						addname(OBJ->Index);
						TryToStand(OBJ,false);
						OBJ->DestX=-1;
						OBJ->DestY=-1;
						//OBJ->SearchVictim();
						return;
					}else{
						OBJ->BackSteps=11;
						OBJ->BackReserv=0;
					};
				};
				char EnDir=char(GetDir(x0-OBJ->RealX,y0-OBJ->RealY));
				int dds=ADC->AttackRadius1[NeedState]-dst1;
				if(dds>16){
					char Endr=EnDir+128;
					char ddir=Endr-OBJ->RealDir;
					if(abs(ddir)<MRot1){
						OBJ->RealDir=Endr;
						OBJ->GraphDir=Endr;
						SmartTryToMove(OBJ,OBJ->RealDir,false);
						OBJ->BackSteps--;
						return;
					}else{
						if(ddir>0)RotateMon(OBJ,MRot);
						else RotateMon(OBJ,-MRot);
						return;
					};
				}else{
					char Endr=EnDir+128;
					char ddir1=Endr-OBJ->RealDir;
					char ddir=EnDir-OBJ->RealDir;
					if(abs(ddir1)<abs(ddir)){
						//forward
						if(abs(ddir1)<MRot1){
							OBJ->RealDir=Endr;
							OBJ->GraphDir=Endr;
							SmartTryToMove(OBJ,OBJ->RealDir,false);
							OBJ->BackSteps--;
							return;
						}else{
							if(ddir1>0)RotateMon(OBJ,MRot);
							else RotateMon(OBJ,-MRot);
							return;
						};
					}else{
						//back
						if(abs(ddir)<MRot1){
							OBJ->RealDir=EnDir;
							OBJ->GraphDir=EnDir;
							SmartTryToMove(OBJ,OBJ->RealDir,true);
							OBJ->BackSteps--;
							return;
						}else{
							if(ddir>0)RotateMon(OBJ,MRot);
							else RotateMon(OBJ,-MRot);
							return;
						};
					};
					if(abs(ddir)<MRot1){
						OBJ->RealDir=EnDir;
						OBJ->GraphDir=EnDir;
						SmartTryToMove(OBJ,OBJ->RealDir,false);
						return;
					}else{
						if(ddir>0)RotateMon(OBJ,MRot);
						else RotateMon(OBJ,-MRot);
						return;
					};
				};
			};
		};
	};
};
void MakeOneShotLink(OneObject* OBJ){
	int cf=OBJ->NewCurSprite>>SpeedSh;
	int ns=OBJ->NewState;
	if(!ns){
		OBJ->DeleteLastOrder();
		OBJ->NewState=4;
		OBJ->LocalNewState=4;
		OBJ->StandTime=20;
		return;
	};
	NewMonster* NM=OBJ->newMons;
	NewAnimation* NMA=&NM->Attack[ns-1];
	if(OBJ->NewAnm!=NMA){
		OBJ->DeleteLastOrder();
		OBJ->NewState=4;
		OBJ->LocalNewState=4;
		OBJ->StandTime=20;
		return;
	};
	int hf=NMA->ActiveFrame>>SpeedSh;;
	
	if(cf>hf+1){
		OBJ->DeleteLastOrder();
		OBJ->NewState=4;
		OBJ->LocalNewState=4;
		OBJ->StandTime=20;
		return;
	};
	
	if(cf>=hf){
		Weapon* WP=NM->DamWeap[OBJ->NewState-1];
		if(WP){
			NewAnimation* UNAM=OBJ->NewAnm;
			int csp=OBJ->NewCurSprite;
			int uocts=(UNAM->Rotations-1)*2;
			int uoc2=UNAM->Rotations-1;
			if(!uocts)uocts=1;
			int usesize=div(255,uocts<<1).quot;
			byte udir=(((OBJ->RealDir+64+usesize)&255)*uocts)>>8;
			NewFrame* NF=&UNAM->Frames[csp];
			//----------
			NewAnimation* NAM=WP->NewAnm;
			int octs=(NAM->Rotations-1)*2;
			int oc2=NAM->Rotations-1;
			if(!octs)octs=1;
			int sesize=div(255,octs<<1).quot;
			byte dir=(((OBJ->RealDir+64+sesize)&255)*octs)>>8;
			int x0,y0,z0,x1,y1,z1;
			if(udir<uoc2){
				int udir1=uoc2-udir;
				x0=(OBJ->RealX>>4)-(UNAM->ActivePtX[udir1]+NF->dx);
				y0=(OBJ->RealY>>4)+Prop43(UNAM->ActivePtY[udir1]+NF->dy+NM->SrcZPoint);
			}else{
				int udir1=uoc2-uocts+udir;
				x0=(OBJ->RealX>>4)+(UNAM->ActivePtX[udir1]+NF->dx);
				y0=(OBJ->RealY>>4)+Prop43(UNAM->ActivePtY[udir1]+NF->dy+NM->SrcZPoint);
			};						
			z0=OBJ->RZ+NM->SrcZPoint;
			x1=int(OBJ->LocalOrder->info.AttackXY.x)<<2;
			y1=int(OBJ->LocalOrder->info.AttackXY.y)<<2;
			z1=int(OBJ->LocalOrder->info.AttackXY.z);
			if(udir<uoc2){
				udir=uoc2-udir;
				Create3DAnmObject(WP,x0,y0,z0,x1,y1,z1,OBJ,OBJ->NewState-1,0xFFFF);
			}else{
				udir=uoc2-uocts+udir;
				Create3DAnmObject(WP,x0,y0,z0,x1,y1,z1,OBJ,OBJ->NewState-1,0xFFFF);
			};
			OBJ->delay=OBJ->Ref.General->MoreCharacter->AttackPause[OBJ->NewState-1];
			OBJ->MaxDelay=div(int(OBJ->delay)*OBJ->PersonalDelay,100).quot;
			OBJ->delay=OBJ->MaxDelay;

			OBJ->DeleteLastOrder();
			OBJ->NewState=4;
			OBJ->LocalNewState=4;
			OBJ->StandTime=20;
		};
	};
};
void MakeOneShot(OneObject* OB,int xd,int yd,int zd){
	Order1* OR1=OB->CreateOrder(1);
	OR1->info.AttackXY.ox=OB->Index;
	OR1->info.AttackXY.x=xd>>2;
	OR1->info.AttackXY.y=yd>>2;
	OR1->info.AttackXY.z=zd;
	OR1->DoLink=&MakeOneShotLink;
	OR1->PrioryLevel=255;
	OB->PrioryLevel=255;
	MakeOneShotLink(OB);
};
void NewMonsterSmartSendToLink(OneObject* OBJ);
int SmartLink=0x153829;
void __stdcall CDGINIT_INIT1(){
	SmartLink=int(&NewMonsterSmartSendToLink)+0x7654391;
};
void WaterAttackLink(OneObject* OBJ){
	OBJ->PrioryLevel=OBJ->LocalOrder->PrioryLevel;
	int NTimes=OBJ->LocalOrder->info.BuildObj.SN;
	bool ai=OBJ->Nat->AI_Enabled;
	NewMonster* NMN=OBJ->newMons;
	if(!NMN->NMultiWp){
		OBJ->DeleteLastOrder();
		return;
	};
	AdvCharacter* ADC=OBJ->Ref.General->MoreCharacter;
	OBJ->DestX=-1;
	//OBJ->UnBlockUnit();
	word OID=OBJ->EnemyID;
	word OSN=OBJ->EnemySN;
	if(OID>8192){
		OBJ->DeleteLastOrder();
		return;
	};
	OneObject* OB=Group[OID];
	if(!OB||OB->Sdoxlo||OBJ->TurnOff||OSN!=OB->Serial){
		if(OBJ->PrioryLevel>=16&&OBJ->NNUM==MyNation&&OBJ->TurnOff){
			char ccc[128];
			sprintf_s(ccc,sizeof(ccc),GetTextByID("CSNOGL"),OBJ->Ref.General->Message);
			AssignHint1(ccc,50,32);
		};
		OBJ->DeleteLastOrder();
		return;
	};
	int dst=Norma(OBJ->RealX-OB->RealX,OBJ->RealY-OB->RealY)>>4;
	//detecting attack method
	int AttMethod=-1;
	for(int i=0;i<NAttTypes;i++){
		if(ADC->DetRadius1[i]<=dst&&ADC->DetRadius2[i]>=dst)AttMethod=i;
	};
	if(AttMethod!=-1){
		byte ATM=OBJ->LocalOrder->info.BuildObj.AttMethod;
		if(ATM!=0xFF){
			if(!OB->NewBuilding)AttMethod=ATM;
		};
		//if(OB->StandTime>5)dst+=90;
		//checking actual distance
		if(dst>=ADC->AttackRadius1[AttMethod]&&dst<=ADC->AttackRadius2[AttMethod]){
			/*if(ai&&OBJ->delay>30&&(((tmtmt/70>>7)%6)==3)&&dst>ADC->AttackRadius2[AttMethod]-400){
				int ux=OBJ->x;
				int uy=OBJ->y;
				int DX=OB->x>ux?1:-1;
				int DY=OB->x>uy?1:-1;
				int LX=OBJ->Lx;
				UnitsField.BClrBar(ux,uy,LX);
				if((UnitsField.CheckBar(ux+DX,uy,LX,LX)||MFIELDS[1].CheckBar(ux+DX,uy,LX,LX))&&
					(UnitsField.CheckBar(ux,uy+DY,LX,LX)||MFIELDS[1].CheckBar(ux,uy+DY,LX,LX))){
					UnitsField.BSetBar(ux,uy,LX);
				}else{
					UnitsField.BSetBar(ux,uy,LX);
					OBJ->CreatePath(OB->x,OB->y);
				};
			}else*/{
				OBJ->DeletePath();
				//checking rotation
				char endir=char(GetDir(OB->RealX-OBJ->RealX,OB->RealY-OBJ->RealY));
				char RBort=OBJ->RealDir-64;
				char LBort=OBJ->RealDir+64;
				if(NMN->ShotForward){
					RBort=OBJ->RealDir;
					LBort=RBort;
				};
				char dL=LBort-endir;
				char dR=RBort-endir;
				bool MakeShot=false;
				int ShotX=0;
				int ShotY=0;
				if(abs(dL)<=abs(dR)){
					//shot by the left side
					if(abs(endir-LBort)<8){
						//pubuhhhhhh!!!!!!!!!!
						if(!OBJ->delay){
							bool AllowShot=true;
							if(NMN->NShotRes){
								for(int k=0;k<NMN->NShotRes;k++){
									if(XRESRC(OBJ->NNUM,NMN->ShotRes[k+k])<NMN->ShotRes[k+k+1])AllowShot=false;
								};
								if(AllowShot){
									for(int k=0;k<NMN->NShotRes;k++){
										AddXRESRC(OBJ->NNUM,NMN->ShotRes[k+k],-NMN->ShotRes[k+k+1]);
										OBJ->Nat->AddResource(byte(NMN->ShotRes[k+k]),-NMN->ShotRes[k+k+1]);
										OBJ->Nat->ResOnLife[NMN->ShotRes[k+k]]+=NMN->ShotRes[k+k+1];
									};
								}else{
									if(OBJ->NNUM==MyNation&&OBJ->PrioryLevel>=16){
										char ccc[150];
										sprintf_s(ccc,sizeof(ccc),GetTextByID("CSNORS"),OBJ->Ref.General->Message);
										AssignHint1(ccc,64,32);
									};
									OBJ->DeleteLastOrder();
									return;
								};
							};
							if(AllowShot){
								if(NMN->ShotForward){
									int pus=(int(NMN->NMultiWp)*int(rando()))>>15;
									int vx=NMN->MultiWpX[pus];
									int vy=NMN->MultiWpY[pus];
									int Dang=(OBJ->RealDir+192)&255;
									int SIN=TSin[Dang];
									int COS=TCos[Dang];
									ShotX=((vx*COS-vy*SIN)>>8);
									ShotY=((vx*SIN+vy*COS)>>8);
									if(NMN->MaxZalp&&OBJ->NZalp){
										OBJ->NZalp--;
									}else{
										OBJ->NZalp=NMN->MaxZalp;
										OBJ->delay=NMN->AttackPause[AttMethod];
										OBJ->MaxDelay=div(int(OBJ->delay)*OBJ->PersonalDelay,100).quot;
										OBJ->delay=OBJ->MaxDelay;
									};
									MakeShot=true;
								}else{
									int pus=(int(NMN->NMultiWp)*int(rando()))>>15;
									int vx=NMN->MultiWpX[pus];
									int vy=NMN->MultiWpY[pus];
									int Dang=(OBJ->RealDir+128)&255;
									int SIN=TSin[Dang];
									int COS=TCos[Dang];
									ShotX=((vx*COS+vy*SIN)>>8);
									ShotY=((vx*SIN-vy*COS)>>8);
									if(NMN->MaxZalp&&OBJ->NZalp){
										OBJ->NZalp--;
									}else{
										OBJ->NZalp=NMN->MaxZalp;
										OBJ->delay=NMN->AttackPause[AttMethod];
										OBJ->MaxDelay=div(int(OBJ->delay)*OBJ->PersonalDelay,100).quot;
										OBJ->delay=OBJ->MaxDelay;
									};
									MakeShot=true;
								};
							};
						};
					}else{
						if(dL>0)OBJ->RealDir-=2;
						else OBJ->RealDir+=2;
						OBJ->GraphDir=OBJ->RealDir;
					};
                    }else{
					//shot by the right side
					if(abs(dR)<8){
						//pubuhhhhhh!!!!!!!!!!
						if(!OBJ->delay){
							bool AllowShot=true;
							if(NMN->NShotRes){
								for(int k=0;k<NMN->NShotRes;k++){
									if(XRESRC(OBJ->NNUM,NMN->ShotRes[k+k])<NMN->ShotRes[k+k+1])AllowShot=false;
								};
							};
							if(AllowShot){
								for(int k=0;k<NMN->NShotRes;k++){
									AddXRESRC(OBJ->NNUM,NMN->ShotRes[k+k],-NMN->ShotRes[k+k+1]);
									OBJ->Nat->AddResource(byte(NMN->ShotRes[k+k]),-NMN->ShotRes[k+k+1]);
									OBJ->Nat->ResOnLife[NMN->ShotRes[k+k]]+=NMN->ShotRes[k+k+1];
								};
								int pus=(int(NMN->NMultiWp)*int(rando()))>>15;
								int vx=NMN->MultiWpX[pus];
								int vy=NMN->MultiWpY[pus];
								int Dang=(OBJ->RealDir+128)&255;
								int SIN=TSin[Dang];
								int COS=TCos[Dang];
								ShotX=((vx*COS-vy*SIN)>>8);
								ShotY=((vx*SIN+vy*COS)>>8);
								if(NMN->MaxZalp&&OBJ->NZalp){
									OBJ->NZalp--;
								}else{
									OBJ->NZalp=NMN->MaxZalp;
									OBJ->delay=NMN->AttackPause[AttMethod];
									OBJ->MaxDelay=div(int(OBJ->delay)*OBJ->PersonalDelay,100).quot;
									OBJ->delay=OBJ->MaxDelay;
								};
								MakeShot=true;
							}else{
								//OBJ->delay=20;
								if(OBJ->NNUM==MyNation&&OBJ->PrioryLevel>=16){
									char ccc[150];
									sprintf_s(ccc,sizeof(ccc),GetTextByID("CSNORS"),OBJ->Ref.General->Message);
									AssignHint1(ccc,64,32);
								};
								OBJ->DeleteLastOrder();
								return;
							};
						};
					}else{
						if(dR>0)OBJ->RealDir-=2;
						else OBJ->RealDir+=2;
						OBJ->GraphDir=OBJ->RealDir;
					};
				};
				if(MakeShot){
					Weapon* WP=NMN->DamWeap[AttMethod];
					if(WP){
						Coor3D C3D;
						word DestObj=0xFFFF;
						if(OB->GetDamagePoint(OBJ->x,OBJ->y,&C3D,0))DestObj=OID;
						if(OB->Wall)DestObj=OID;
						CreateRazbros(OBJ,&C3D);
						addrand(76);
						Create3DAnmObject(WP,(OBJ->RealX>>4)+ShotX,(OBJ->RealY>>4)+ShotY,/*OBJ->RZ*/+NMN->MultiWpZ,
							C3D.x,C3D.y,C3D.z,OBJ,AttMethod,0xFFFF);
						if(NTimes==1){
							OBJ->DeleteLastOrder();
						}else{
							if(NTimes>1&&OBJ->LocalOrder)OBJ->LocalOrder->info.BuildObj.SN--;
						};
					};
				};
			};
		}else{
			if(dst>NMN->AttackRadius2[AttMethod]){
				if(NTimes>0){
					OBJ->DeleteLastOrder();
					return;
				};
				if(!OBJ->StandGround){
					OBJ->CreatePath(OB->x,OB->y);
				}else OBJ->DeleteLastOrder();
				//OBJ->DeleteLastOrder();
			}else{
				OBJ->DeleteLastOrder();
			};
		};
	}else OBJ->DeleteLastOrder();
};
//-----------------------Search for the enemy------------------------
word GetNewEnemy(int xr,int yr,byte NI){
	NI=NatRefTBL[NI];
	byte nms=1<<NI;
	OneObject* OBX=NULL;
	for(int ii=0;ii<MAXOBJECT;ii++){
		OneObject* OB=Group[ii];
		if(OB&&(!( (nms&OB->NMask) || OB->Sdoxlo))){
			int xs,ys,xs1,ys1,LY;
			GetRect(OB,&xs,&ys,&xs1,&LY);
			ys1=ys+LY-1;
			xs1+=xs-1;
			if(PInside(xs,ys,xs1,ys1,xr,yr)){
				if((!OBX)||OBX->NewBuilding){
					OBX=Group[ii];
				};
			};
		};
	};
	if(OBX)return OBX->Index;
	else return 0xFFFF;
};
word AdvancedGetNewEnemyInCell(int cx,int cy,int xr,int yr,byte EMask){
	if(cx<0||cy<0||cx>=VAL_MAXCX||cy>=VAL_MAXCX)return 0xfFFF;
	int cell=VAL_MAXCX+1+cx+(cy<<VAL_SHFCX);
	cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	int ofs1=cell<<SHFCELL;
	word MID;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&(!(OB->NMask&EMask))&&!OB->Sdoxlo){
				int xs,ys,xs1,ys1,LY;
				GetRect(OB,&xs,&ys,&xs1,&LY);
				ys1=ys+LY-1;
				xs1+=xs-1;
				if(OB->InMotion&&(!OB->LockType)){
					int dx=OB->newMons->OneStepDX[byte(OB->RealDir)]>>2;
					int dy=OB->newMons->OneStepDY[byte(OB->RealDir)]>>2;
					for(int j=0;j<3;j++){
						if(PInside(xs,ys,xs1,ys1,xr,yr))return MID;
						xs-=dx;
						ys-=dy;
						xs1-=dx;
						ys1-=dy;
					};
				};
				if(PInside(xs,ys,xs1,ys1,xr,yr))return MID;
			};
		};
	};
	MID=BLDList[cell];
	if(MID!=0xFFFF){
		OneObject* OB=Group[MID];
		if(OB&&(!(OB->NMask&EMask))&&!OB->Sdoxlo){
			int xs,ys,xs1,ys1,LY;
			GetRect(OB,&xs,&ys,&xs1,&LY);
			ys1=ys+LY-1;
			xs1+=xs-1;
			if(PInside(xs,ys,xs1,ys1,xr,yr))return MID;
		};
	};
	//search for wall
	int xx1=cx<<1;
	int yy1=cy<<1;
	for(int dx=0;dx<2;dx++)for(int dy=0;dy<2;dy++){
		int LI=GetLI(xx1+dx,yy1+dy);	
		WallCell* WCL=WRefs[LI];
		if(WCL){
			word MID=WCL->OIndex;
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&(!(OB->NMask&EMask))&&!OB->Sdoxlo){
					int xs,ys,xs1,ys1,LY;
					GetRect(OB,&xs,&ys,&xs1,&LY);
					ys1=ys+LY-1;
					xs1+=xs-1;
					if(PInside(xs,ys,xs1,ys1,xr,yr))return MID;
				};
			};
		};
	};
	return 0xFFFF;
};
word CheckCoorInGP(int x,int y);
word AdvancedGetNewEnemy(int rex,int rey,int xr,int yr,byte NI){
	int scx=(rex>>(5-Shifter))-(mapx<<Shifter);
	int scy=(rey>>(6-Shifter))-(mapy<<(Shifter-1))-GetHeight(rex,rey);
	int sx=rex-(mapx<<Shifter);
	byte nms=NATIONS[NI].NMask;

	word ID=CheckCoorInGP(scx,scy);
	if(ID!=0xFFFF){
		OneObject* OB=Group[ID];
		if(OB&&(!OB->Sdoxlo)&&!(OB->NMask&nms))return ID;
	};

	int x0=rex>>7;
	int y0=rey>>7;
	for(int ix=-2;ix<3;ix++){
		for(int iy=-2;iy<3;iy++){
			word MID=AdvancedGetNewEnemyInCell(x0+ix,y0+iy,xr,yr,nms);
			if(MID!=0xFFFF)return MID;
		};
	};
	return 0xFFFF;
};
word GetNewFriend(int xr,int yr,byte NI){
	byte nms=1<<NI;
	for(int ii=0;ii<MAXOBJECT;ii++){
		OneObject* OB=Group[ii];
		if(OB&&OB->NNUM==NI&&!OB->Sdoxlo){
			int xs,ys,xs1,ys1,LY;
			GetRect(OB,&xs,&ys,&xs1,&LY);
			ys1=ys+LY-1;
			xs1+=xs-1;
			if(PInside(xs,ys,xs1,ys1,xr,yr))return ii;
		};
	};
	return 0xFFFF;
};
word AdvancedGetNewFriendInCell(int cx,int cy,int xr,int yr,byte NI){
	if(cx<0||cy<0||cx>=VAL_MAXCX||cy>=VAL_MAXCX)return 0xfFFF;
	int cell=VAL_MAXCX+1+cx+(cy<<VAL_SHFCX);
	cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	int ofs1=cell<<SHFCELL;
	word MID;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&OB->NNUM==NI&&!OB->Sdoxlo){
				int xs,ys,xs1,ys1,LY;
				GetRect(OB,&xs,&ys,&xs1,&LY);
				ys1=ys+LY-1;
				xs1+=xs-1;
				if(PInside(xs,ys,xs1,ys1,xr,yr))return MID;
			};
		};
	};
	MID=BLDList[cell];
	if(MID!=0xFFFF){
		OneObject* OB=Group[MID];
		if(OB&&OB->NNUM==NI&&!OB->Sdoxlo){
			int xs,ys,xs1,ys1,LY;
			GetRect(OB,&xs,&ys,&xs1,&LY);
			ys1=ys+LY-1;
			xs1+=xs-1;
			if(PInside(xs,ys,xs1,ys1,xr,yr))return MID;
		};
	};
	int xx1=cx<<1;
	int yy1=cy<<1;
	for(int dx=0;dx<2;dx++)for(int dy=0;dy<2;dy++){
		int LI=GetLI(xx1+dx,yy1+dy);	
		WallCell* WCL=WRefs[LI];
		if(WCL){
			word MID=WCL->OIndex;
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&OB->NNUM==NI&&!OB->Sdoxlo){
					int xs,ys,xs1,ys1,LY;
					GetRect(OB,&xs,&ys,&xs1,&LY);
					ys1=ys+LY-1;
					xs1+=xs-1;
					if(PInside(xs,ys,xs1,ys1,xr,yr))return MID;
				};
			};
		};
	};
	return 0xFFFF;
};
word AdvancedGetNewFriend(int rex,int rey,int xr,int yr,byte NI){
	NI=NatRefTBL[NI];
	int x0=rex>>7;
	int y0=rey>>7;
	for(int ix=-2;ix<3;ix++){
		for(int iy=-2;iy<3;iy++){
			word MID=AdvancedGetNewFriendInCell(x0+ix,y0+iy,xr,yr,NI);
			if(MID!=0xFFFF)return MID;
		};
	};
	return 0xFFFF;
};
word GetDoorsInCell(int cx,int cy,int xr,int yr,byte NI){
	if(cx<0||cy<0||cx>=VAL_MAXCX||cy>=VAL_MAXCX)return 0xfFFF;
	int cell=VAL_MAXCX+1+cx+(cy<<VAL_SHFCX);
	cell+=VAL_MAXCX+1;
    byte mask=NATIONS[NI].NMask;
	word MID=BLDList[cell];
	if(MID!=0xFFFF){
		OneObject* OB=Group[MID];
		if(OB&&(!(OB->NMask&mask))&&!OB->Sdoxlo){
			NewMonster* NM=OB->newMons;
			if(NM->Doors){
				int x0=(OB->RealX>>4)+NM->PicDx;
				int y0=(OB->RealY>>5)+NM->PicDy-OB->RZ;
				ExRect* DR=NM->Doors;
				int j4=0;
				for(int j=0;j<DR->NRects;j++){
					int xL=x0+DR->Coor[j4  ];
					int yL=y0+DR->Coor[j4+1];
					int xR=x0+DR->Coor[j4+2];
					int yR=y0+DR->Coor[j4+3];
					if(xr>=xL&&xr<=xR&&yr>=yL&&yr<=yR)return MID;
					j4+=4;
				};
			};
		};
	};
	return 0xFFFF;
};
word GetDoorsOfEnemy(int rex,int rey,int xr,int yr,byte NI){
	NI=NatRefTBL[NI];
	int x0=rex>>7;
	int y0=rey>>7;
	for(int ix=-5;ix<5;ix++){
		for(int iy=-5;iy<5;iy++){
			word MID=GetDoorsInCell(x0+ix,y0+iy,xr,yr,NI);
			if(MID!=0xFFFF)return MID;
		};
	};
	return 0xFFFF;
};
//------------------Block&UnBlock---------------------//
void OneObject::WeakBlockUnit(){
	if(BlockInfo)return;
	/*
	if(!CheckPt(x,y)){
		BlockInfo|=1;
		BSetPt(x,y);
	};
	if(!CheckPt(x,y-1)){
		BlockInfo|=2;
		BSetPt(x-1,y-1);
	};
	if(!CheckPt(x,y+1)){
		BlockInfo|=4;
		BSetPt(x,y+1);
	};
	if(!CheckPt(x-1,y)){
		BlockInfo|=8;
		BSetPt(x+1,y);
	};
	if(!CheckPt(x-1,y)){
		BlockInfo|=16;
		BSetPt(x-1,y);
	};
	return;
	//--------------------
	*/
	/*if(!CheckPt(x-1,y-1)){
		BlockInfo|=1;
		BSetPt(x-1,y-1);
	};
	if(!CheckPt(x,y-1)){
		BlockInfo|=2;
		BSetPt(x,y-1);
	};
	if(!CheckPt(x+1,y-1)){
		BlockInfo|=4;
		BSetPt(x+1,y-1);
	};
	if(!CheckPt(x-1,y)){
		BlockInfo|=8;
		BSetPt(x-1,y);
	};*/
	if(!CheckPt(x,y)){
		BlockInfo|=16;
		BSetPt(x,y);
	};
	/*
	if(!CheckPt(x+1,y)){
		BlockInfo|=32;
		BSetPt(x+1,y);
	};
	if(!CheckPt(x-1,y+1)){
		BlockInfo|=64;
		BSetPt(x-1,y+1);
	};
	if(!CheckPt(x,y+1)){
		BlockInfo|=128;
		BSetPt(x,y+1);
	};
	if(!CheckPt(x+1,y+1)){
		BlockInfo|=256;
		BSetPt(x+1,y+1);
	};*/
};
void OneObject::BlockUnit(){
	if(BlockInfo)return;
	/*
	if(!CheckPt(x,y)){
		BlockInfo|=1;
		BSetPt(x,y);
	};
	if(!CheckPt(x,y-1)){
		BlockInfo|=2;
		BSetPt(x-1,y-1);
	};
	if(!CheckPt(x,y+1)){
		BlockInfo|=4;
		BSetPt(x,y+1);
	};
	if(!CheckPt(x-1,y)){
		BlockInfo|=8;
		BSetPt(x+1,y);
	};
	if(!CheckPt(x-1,y)){
		BlockInfo|=16;
		BSetPt(x-1,y);
	};
	return;
	//--------------------
	*/
	if(!CheckPt(x-1,y-1)){
		BlockInfo|=1;
		BSetPt(x-1,y-1);
	};
	if(!CheckPt(x,y-1)){
		BlockInfo|=2;
		BSetPt(x,y-1);
	};
	if(!CheckPt(x+1,y-1)){
		BlockInfo|=4;
		BSetPt(x+1,y-1);
	};
	if(!CheckPt(x-1,y)){
		BlockInfo|=8;
		BSetPt(x-1,y);
	};
	if(!CheckPt(x,y)){
		BlockInfo|=16;
		BSetPt(x,y);
	};
	if(!CheckPt(x+1,y)){
		BlockInfo|=32;
		BSetPt(x+1,y);
	};
	if(!CheckPt(x-1,y+1)){
		BlockInfo|=64;
		BSetPt(x-1,y+1);
	};
	if(!CheckPt(x,y+1)){
		BlockInfo|=128;
		BSetPt(x,y+1);
	};
	if(!CheckPt(x+1,y+1)){
		BlockInfo|=256;
		BSetPt(x+1,y+1);
	};
};
void OneObject::UnBlockUnit(){
	if(!BlockInfo)return;
	/*if(BlockInfo&1)BClrPt(x,y);
	if(BlockInfo&2)BClrPt(x,y-1);
	if(BlockInfo&4)BClrPt(x,y+1);
	if(BlockInfo&8)BClrPt(x+1,y);
	if(BlockInfo&16)BClrPt(x-1,y);
	return;*/
	if(BlockInfo&1)BClrPt(x-1,y-1);
	if(BlockInfo&2)BClrPt(x  ,y-1);
	if(BlockInfo&4)BClrPt(x+1,y-1);
	if(BlockInfo&8)BClrPt(x-1,y);
	if(BlockInfo&16)BClrPt(x  ,y);
	if(BlockInfo&32)BClrPt(x+1,y);
	if(BlockInfo&64)BClrPt(x-1,y+1);
	if(BlockInfo&128)BClrPt(x  ,y+1);
	if(BlockInfo&256)BClrPt(x+1,y+1);
	BlockInfo=0;
};
void OneObject::DeletePath(){
	if(PathX){
		free(PathX);
		free(PathY);
		PathX=NULL;
		PathY=NULL;
        CPdestX=-1;
		NIPoints=0;
		CurIPoint=0;

	};
};
//Creation the building
bool OneObject::FindPoint(int* x1,int* y1,byte Flags){
	NewMonster* NM=newMons;
	char* px;
	char* py;
	int Npt=0;
	if(Flags&FP_FIND_WORKPOINT){
		px=NM->BuildPtX;
		py=NM->BuildPtY;
		Npt=NM->NBuildPt;
	};
	if(Flags&FP_FIND_DAMPOINT){
		px=NM->DamPtX;
		py=NM->DamPtY;
		Npt=NM->NDamPt;
	};
	if(Flags&FP_CONCENTRATION){
		px=NM->ConcPtX;
		py=NM->ConcPtY;
		Npt=NM->NConcPt;
	};
	if(!Npt)return false;
	if(Flags&FP_NEAREST_POINT){
		int xx=*x1;
		int yy=*y1;
		int dist=10000;
		int dis;
		int PicSX=NM->PicDx<<4;
		int PicSY=Prop43(NM->PicDy)<<4;
		int x0=(RealX+PicSX)>>8;
		int y0=(RealY+PicSY)>>8;
		int x2,y2,x3,y3;
		if(Flags&FP_UNLOCKED_POINT){
			for(int i=0;i<Npt;i++){
				x2=x0+px[i];
				y2=y0+py[i];
				dis=Norma(x2-xx,y2-yy);
				if(dis<dist&&!CheckBar(x2-1,y2-1,3,3)){
					dist=dis;
					x3=x2;
					y3=y2;
				};
			};
		}else{
			for(int i=0;i<Npt;i++){
				x2=x0+px[i];
				y2=y0+py[i];
				dis=Norma(x2-xx,y2-yy);
				if(dis<dist){
					dist=dis;
					x3=x2;
					y3=y2;
				};
			};
		};
		if(dist<10000){
			*x1=x3;
			*y1=y3;
			return true;
		};
	};
	return false;
};
void CmdFieldBar(byte NI,word n);
void CreateFields(byte NI,int x,int y,int n);
void InvitePeasant(OneObject* Mine);
extern bool EditMapMode;
void OneObject::NextStage(){
    int i;
	if(!NewBuilding||Life==MaxLife)return;
	NewMonster* NM=newMons;
	AdvCharacter* ACR=Ref.General->MoreCharacter;
	if(Stage<ACR->ProduceStages){
		Stage++;
		Life=div(Stage*MaxLife,ACR->ProduceStages).quot;
		int nn=div(Stage<<2,ACR->ProduceStages).quot;
		LoLayer=&NM->PMotionL[nn];
		HiLayer=&NM->PMotionR[nn];
		NewCurSprite=0;
		if(Stage==ACR->ProduceStages){
			if(NM->NBLockPt){
				int xx,yy;
				this->GetCornerXY(&xx,&yy);
				for(i=0;i<NM->NBLockPt;i++){
					BClrPt(xx+NM->BLockX[i],yy+NM->BLockY[i]);
				};
				for(i=0;i<NM->NLockPt;i++){
					BSetPt(xx+NM->LockX[i],yy+NM->LockY[i]);
				};
			};
			Ready=true;
			LoLayer=&NM->StandLo;
			HiLayer=&NM->StandHi;
            NewAnm=&NM->Work;
            NewCurSprite=0;
			byte Usage=newMons->Usage;
			if(Usage==TowerID)GNFO.AddTow(this);
#ifdef CONQUEST
			if(Usage==MelnicaID){
				if(Nat->PACount[NIndex])CreateFields(NNUM,RealX,RealY,Nat->PAble[NIndex][0]);
			};
#else
			if(Usage==MelnicaID&&(!Nat->AI_Enabled)&&Nat->CITY->ReadyAmount[NIndex]==0){
				if(Nat->PACount[NIndex])CreateFields(NNUM,RealX,RealY,Nat->PAble[NIndex][0]);
			};
#endif
			if(Usage==MineID&&!EditMapMode){
				addrand(6);
				InvitePeasant(this);
			};
			if(NNUM==MyNation){
				char ccc[200];
#ifdef _USE3D
				ccc[0] = 0;
				int msgLen = strlen( Ref.General->Message );
				if (msgLen < 100) 
#endif // _USE3D
				sprintf_s(ccc,sizeof(ccc),BLDBLD,Ref.General->Message);
				AssignHint1(ccc,100);
				LastActionX=RealX>>4;
				LastActionY=RealY>>4;
			};
			Nat->CITY->Account+=newMons->Ves;
		};
	}else{
		Life+=20;
		LoLayer=&NM->StandLo;
		HiLayer=&NM->StandHi;
        NewAnm=&NM->Work;
		//NewCurSprite=0;
		if(Life>=MaxLife){
			Life=MaxLife;
			InFire=0;
			FireOwner=0xFF;
			FiringStage=0;
		};
		//Ready=true;
		if(newMons->Usage==TowerID&&NNUM==0)GNFO.AddTow(this);
	};
};
void OneObject::ClearBuildPt(){
	NewMonster* NM=newMons;
	char* px=NM->BuildPtX;
	char* py=NM->BuildPtY;
	int Npt=NM->NBuildPt;
	if(!Npt)return;
	int PicSX=NM->PicDx<<4;
	int PicSY=Prop43(NM->PicDy)<<4;
	int x0=(RealX+PicSX)>>8;
	int y0=(RealY+PicSY)>>8;
	for(int i=0;i<Npt;i++)BClrPt(x0+px[i],y0+py[i]);
};
void BuildObjLink(OneObject* OBJ);
void TakeResLink(OneObject* OB);
void UnConstructBuilduingLink(OneObject* OBJ);
bool OneObject::BuildObj(word OID,int Prio,bool LockPoint,byte OrdType){
	addrand(OID);
	if(CheckOrderAbility())return false;
	if(Prio<PrioryLevel&&Prio<100)return false;
    if(LocalOrder&&LocalOrder->DoLink==&BuildObjLink&&LocalOrder->info.BuildObj.ObjIndex==OID)return true;
	if(LocalOrder&&LocalOrder->DoLink==&UnConstructBuilduingLink)return false;
	NewMonster* NM=newMons;
	if(!NM->Work.Enabled)return false;
	OneObject* OB=Group[OID];
	if(!int(OB)||OB->Sdoxlo)return false;
	if(!OB->NewBuilding)return false ;
	if(!(OB->NMask&NMask))return false;
	if(OB->Life==OB->MaxLife)return false;
	int xx=x;
	int yy=y;
	if(OB->FindPoint(&xx,&yy,FP_NEAREST_POINT|FP_FIND_WORKPOINT|FP_UNLOCKED_POINT)){
		if(OrdType==0||OrdType==2)NewMonsterSmartSendTo(xx<<4,yy<<4,0,0,Prio,OrdType);
		if(OrdType==2&&LocalOrder&&LocalOrder->DoLink==&TakeResLink){
			DeleteLastOrder();
			OrdType=1;
		};
		//Important=true;
		Order1* Or1;
		if(!OrdType)Or1=CreateOrder(2);
		else Or1=CreateOrder(OrdType);
		if(!int(Or1))return false;
		Or1->OrderType=89;//Build
		Or1->OrderTime=0;
		Or1->DoLink=&BuildObjLink;
		Or1->info.BuildObj.ObjIndex=OID;
		Or1->info.BuildObj.ObjX=xx;
		Or1->info.BuildObj.ObjY=yy;
		Or1->info.BuildObj.SN=OB->Serial;
		Or1->PrioryLevel=Prio&127;
		if(LockPoint)BSetPt(xx,yy);
		//PrioryLevel=Prio&127;	
		//PrioryLevel=LocalOrder->PrioryLevel;
		if(OrdType==1)NewMonsterSmartSendTo(xx<<4,yy<<4,0,0,Prio,1);
		addrand(OID);
		return true;
	};
	return false;
};
void InvitePeasant(OneObject* Mine){
	if(!SCENINF.hLib){
		if(Mine->Nat->AI_Enabled)return;
	};
	word PLIST[64];
	int Ri[64];
	int NP=Mine->Ref.General->newMons->MaxInside;
	if(NP>64)NP=64;
	byte NI=Mine->NNUM;
	int NN=0;
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&(!OB->Sdoxlo)&&OB->NNUM==NI&&OB->Ref.General->newMons->Peasant&&
			((OB->LocalOrder&&OB->LocalOrder->DoLink==&BuildObjLink&&OB->LocalOrder->info.BuildObj.ObjIndex==Mine->Index)||
			(!OB->LocalOrder))){
			int R=Norma(OB->RealX-Mine->RealX,OB->RealY-Mine->RealY)>>4;
			addrand(i);
			addrand(R);
			if(R<512){
				addrand(i);
				if(NN<64){
					PLIST[NN]=i;
					Ri[NN]=R;
					if(!OB->LocalOrder)R+=500;
					NN++;
				};
			};
		};
	};
	addrand(NN);
	UNISORT.CheckSize(NN);
	for(int i=0;i<NN;i++){
		UNISORT.Parms[i]=Ri[i];
		UNISORT.Uids[i]=PLIST[i];
		addrand(Ri[i]);
		addrand(PLIST[i]);
	};
	UNISORT.NUids=NN;
	UNISORT.Sort();
	for(int i=0;i<NN&&i<NP;i++)Group[UNISORT.Uids[i]]->GoToMine(Mine->Index,128+16);
};
void TakeResLink(OneObject* OBJ);
void InviteAI_Peasants(OneObject* Mine){
	if(Mine->Nat->CITY->FreePS>Mine->Nat->CITY->Nat->MIN_PBRIG)return;
	word PLIST[512];
	int Ri[512];
	int NP=Mine->Ref.General->newMons->MaxInside+Mine->AddInside-Mine->NInside;
	if(NP<0)NP=0;
	if(NP>64)NP=64;
	byte NI=Mine->NNUM;
	int NN=0;
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&(!OB->Sdoxlo)&&OB->NNUM==NI&&OB->Ref.General->newMons->Peasant&&
			((OB->LocalOrder&&OB->LocalOrder->DoLink==&TakeResLink)||(!OB->LocalOrder))){
			int R=Norma(OB->RealX-Mine->RealX,OB->RealY-Mine->RealY)>>4;
			addrand(i);
			addrand(R);
			if(R<2048){
				addrand(i);
				if(NN<512){
					PLIST[NN]=i;
					Ri[NN]=R;
					NN++;
				};
			};
		};
	};
	addrand(NN);
	UNISORT.CheckSize(NN);
	for(int i=0;i<NN;i++){
		UNISORT.Parms[i]=Ri[i];
		UNISORT.Uids[i]=PLIST[i];
		addrand(Ri[i]);
		addrand(PLIST[i]);
	};
	UNISORT.NUids=NN;
	UNISORT.Sort();
	for(int i=0;i<NN&&i<NP;i++){
		//Group[UNISORT.Uids[i]]->Serial+=35;
		OneObject* PS=Group[UNISORT.Uids[i]];
		if(PS){
			if(PS->BrigadeID!=0xFFFF){
				Brigade* BR=PS->Nat->CITY->Brigs+PS->BrigadeID;
				Brigade* BRD=&PS->Nat->CITY->InMines;
				if(BR!=BRD){
					BR->RemoveOne(PS->BrIndex,BRD);
				};
			};
			PS->DoNotCall=1;
			PS->GoToMine(Mine->Index,128+127);
		};
	};
};
void GoToMineLink(OneObject* OB);
void BuildObjLink(OneObject* OBJ){
	addrand(OBJ->Index);
	if(OBJ->NewBuilding){
		OBJ->DeleteLastOrder();
		return;
	};
	OBJ->PrioryLevel=OBJ->LocalOrder->PrioryLevel;
	if(OBJ->NewCurSprite<OBJ->NewAnm->NFrames-FrmDec)return;
	NewMonster* NM=OBJ->newMons;
	word OID=OBJ->LocalOrder->info.BuildObj.ObjIndex;
	int ObjX=OBJ->LocalOrder->info.BuildObj.ObjX;
	int ObjY=OBJ->LocalOrder->info.BuildObj.ObjY;
	word OSN=OBJ->LocalOrder->info.BuildObj.SN;
	OBJ->UnBlockUnit();
	OneObject* OB=Group[OID];
	//OBJ->Important=true;
	Order1* Or1;
	if((!OB)||OB->Sdoxlo||OB->Life==OB->MaxLife||OSN!=OB->Serial||(OB->LocalOrder&&OB->LocalOrder->DoLink==&UnConstructBuilduingLink)){
		if(OB&&OB->NNUM==MyNation&&OB->Life==OB->MaxLife)AddUnlimitedEffect(OB->RealX>>4,(OB->RealY>>5)-OB->RZ,OB->newMons->BornSoundID);
		if(OBJ->LocalOrder){
			OBJ->DeleteLastOrder();
			OBJ->DeletePath();
			if(OB&&OB->Life==OB->MaxLife&&OB->newMons->Usage==MineID){
				int N=OB->NInside;
				int MID=OB->Index;
				for(int i=0;i<MAXOBJECT;i++){
					OneObject* MOB=Group[i];
					if(MOB&&MOB->LocalOrder&&MOB->LocalOrder->DoLink==&GoToMineLink&&MOB->LocalOrder->info.BuildObj.ObjIndex==MID)N++;
				};
				if(N<OB->AddInside+OB->Ref.General->MoreCharacter->MaxInside)OBJ->GoToMine(OB->Index,128+16,1);
			};
			return;
		};
	};

	int dst=Norma(OBJ->x-ObjX,OBJ->y-ObjY);
	if(dst<7){
		if(CheckPt(ObjX,ObjY)){
			if(OB->FindPoint(&ObjX,&ObjY,FP_NEAREST_POINT|FP_FIND_WORKPOINT|FP_UNLOCKED_POINT)){
				OBJ->LocalOrder->info.BuildObj.ObjX=ObjX;
				OBJ->LocalOrder->info.BuildObj.ObjY=ObjY;
				return;
			}else{
				OBJ->DeleteLastOrder();
				OBJ->DeletePath();
				return;
			};

		};
	};
	if(dst<=1){
		OBJ->DestX=-1;
		OBJ->DeletePath();
		/*if(dst==1&&OBJ->NothingTime<5){
			OBJ->DestX=(ObjX<<8);
			OBJ->DestY=(ObjY<<8);
		}else{;*/
			//we can build now
			char dir=char(GetDir(OB->RealX-OBJ->RealX,OB->RealY-OBJ->RealY));
			char ddir=dir-OBJ->RealDir;
			if(abs(ddir)<16){
				OBJ->RealDir=dir;
				OBJ->GraphDir=OBJ->RealDir;
				OBJ->NewAnm=&NM->Work;
				OBJ->NewCurSprite=0;
				OBJ->InMotion=false;
				OBJ->BackMotion=false;
				OBJ->WeakBlockUnit();
				OB->NextStage();
			}else{
				if(ddir>0)RotateMon(OBJ,16);
				else RotateMon(OBJ,-16);
				TryToStand(OBJ,false);
			};
		//};
	}else{
		if(CheckPt(ObjX,ObjY)){
			ObjX=OBJ->x;
			ObjY=OBJ->y;
			if(OB->FindPoint(&ObjX,&ObjY,FP_NEAREST_POINT|FP_FIND_WORKPOINT|FP_UNLOCKED_POINT)){
				OBJ->LocalOrder->info.BuildObj.ObjX=ObjX;
				OBJ->LocalOrder->info.BuildObj.ObjY=ObjY;
			}else{
				Or1=OBJ->LocalOrder->NextOrder;
				OBJ->FreeOrdBlock(OBJ->LocalOrder);
				OBJ->LocalOrder=Or1;
				//OBJ->Important=false;
				TryToStand(OBJ,true);
				return;
			};
		};
		if(OBJ->NothingTime>8){
			if(OBJ->CheckBlocking()){
				OBJ->DeleteBlocking();
			}else OBJ->CreatePath(ObjX,ObjY);
		}else OBJ->CreatePath(ObjX,ObjY);
	};
};
//x,y-coordinates of point on the 2D plane (unit:pix)
//returnfs index of building,otherwise 0xFFFF
word DetermineBuilding(int x,int y,byte NMask){
	int xx=x>>4;
	int yy=y>>4;
	int xr=x<<4;
	int yr=y<<4;
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB&&OB->NewBuilding&&(OB->NMask&NMask)){
			NewMonster* NM=OB->newMons;
			int rr=NM->PicLx<<4;
			if(Norma(xr-OB->RealX,yr-OB->RealY)<rr){
				//precise checking
				int PicSX=NM->PicDx<<4;
				int PicSY=Prop43(NM->PicDy)<<4;
				int x0=(OB->RealX+PicSX)>>8;
				int y0=(OB->RealY+PicSY)>>8;
				byte* px=NM->CheckX;
				byte* py=NM->CheckY;
				int Npt=NM->NCheckPt;
				for(int j=0;j<Npt;j++)
					if(x0+px[j]==xx&&y0+py[j]==yy) return i;
			};
		};
	};
	return 0xFFFF;
};
bool OneObject::CheckBlocking(){
	return (CheckHLine(x-1,y-1,3)||CheckHLine(x-1,y+1,3)||
		CheckPt(x-1,y)||CheckPt(x+1,y));
};
void DeleteBlockLink(OneObject* OBJ);
void OneObject::DeleteBlocking(){
	Order1* OR1=GetOrdBlock();
	if(!OR1)return;//torba
	//PrioryLevel=0;
	DeletePath();
	OR1->OrderType=100;
	OR1->DoLink=&DeleteBlockLink;
	if(!CheckPt(x,y+2)){
		OR1->info.DelBlock.EndX=x;
		OR1->info.DelBlock.EndY=y+2;
		if(CheckPt(x,y+1)){
			OR1->info.DelBlock.LockX=x;
			OR1->info.DelBlock.LockY=y+1;
		}else{
			OR1->info.DelBlock.LockX=-1;
			OR1->info.DelBlock.LockY=-1;
		};
	}else
	if(!CheckPt(x,y-2)){
		OR1->info.DelBlock.EndX=x;
		OR1->info.DelBlock.EndY=y-2;
		if(CheckPt(x,y-1)){
			OR1->info.DelBlock.LockX=x;
			OR1->info.DelBlock.LockY=y-1;
		}else{
			OR1->info.DelBlock.LockX=-1;
			OR1->info.DelBlock.LockY=-1;
		};
	}else
	if(!CheckPt(x-2,y)){
		OR1->info.DelBlock.EndX=x-2;
		OR1->info.DelBlock.EndY=y;
		if(CheckPt(x-1,y)){
			OR1->info.DelBlock.LockX=x-1;
			OR1->info.DelBlock.LockY=y;
		}else{
			OR1->info.DelBlock.LockX=-1;
			OR1->info.DelBlock.LockY=-1;
		};
	}else
	if(!CheckPt(x+2,y)){
		OR1->info.DelBlock.EndX=x+2;
		OR1->info.DelBlock.EndY=y;
		if(CheckPt(x+1,y)){
			OR1->info.DelBlock.LockX=x+1;
			OR1->info.DelBlock.LockY=y;
		}else{
			OR1->info.DelBlock.LockX=-1;
			OR1->info.DelBlock.LockY=-1;
		};
	};
	NothingTime=0;
	OR1->NextOrder=LocalOrder;
	LocalOrder=OR1;
};
void DeleteBlockLink(OneObject* OBJ){
	OBJ->PrioryLevel=OBJ->LocalOrder->PrioryLevel;
	if(OBJ->NewCurSprite<OBJ->NewAnm->NFrames-FrmDec)return;
	int lox=OBJ->LocalOrder->info.DelBlock.LockX;
	int loy=OBJ->LocalOrder->info.DelBlock.LockY;
	int enx=OBJ->LocalOrder->info.DelBlock.EndX;
	int eny=OBJ->LocalOrder->info.DelBlock.EndY;
	if(lox>0)BClrPt(lox,loy);
	BClrPt(enx,eny);
	OBJ->DestX=enx<<8;
	OBJ->DestY=eny<<8;
	int RATE=16;
	if(OBJ->NewState){
		RATE=OBJ->newMons->Rate[OBJ->NewState-1];
	};
	int RR=((OBJ->newMons->OneStepDX[0]*OBJ->NewAnm->NFrames*RATE)>>5)+8;
	if(OBJ->NothingTime>9||Norma(OBJ->RealX-OBJ->DestX,OBJ->RealY-OBJ->DestY)<RR){
		Order1* OR1=OBJ->LocalOrder;
		OBJ->LocalOrder=OR1->NextOrder;
		OBJ->FreeOrdBlock(OR1);
		OBJ->NothingTime=0;
		OBJ->DeletePath();
	};
};
//-----------Inverse references from (x,y)->Index----------//
/*word MonInd[MAPSX*MAPSX];//512k
#define MAXMI (MAPSX*MAPSY)
inline word GetMI(int x,int y){
	__asm{
		mov		ebx,y
		shl		ebx,9
		add		ebx,x
		cmp		ebx,0
		jl		nona
		cmp		ebx,MAXMI
		jge		nona
		shl		ebx,1
		mov		ax,[eax]
		jmp		done
nona:	mov		ax,0xFFFF
done:
	};
	return;
};
inline void SetMI(int x,int y,word MI){
	__asm{
		mov		eax,y
		shl		eax,9
		add		eax,x
		cmp		eax,0
		jl		nona
		cmp		eax,MAXMI
		jge		nona
		shl		eax,1
		mov		bx,MI
		mov		[eax],bx
nona:
	};
	return;
};*/
int ExplMedia;
bool DamageInCell(int cell,int x,int y,int r,word Damage,OneObject* Sender,word Attr){
	if(cell<0||cell>=VAL_MAXCX*VAL_MAXCX)return 0;
	int NMon=MCount[cell];
	if(!NMon)return 0;
	int ofs1=cell<<SHFCELL;
	word MID;
	bool DAM=0;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB){
				NewMonster* NM=OB->newMons;
				int rr=int(NM->EMediaRadius+NM->ExplRadius)<<4;
				int dr=Norma(OB->RealX-x,OB->RealY-y);
				if(OB&&dr<r+rr){
					//Unit is in danger !
					if(dr<rr)ExplMedia=NM->ExplosionMedia;
#ifdef COSSACKS2
					if(!(Sender&&Sender->NMask&OB->NMask&&Norma(Sender->RealX-OB->RealX,Sender->RealY-OB->RealY)<150*16&&Sender->newMons->FriendlyFire))
#endif //COSSACKS2
					{
						OB->MakeDamage(0,Damage,Sender,byte(Attr));
						DAM=1;
					};
				};
			};
		};
	};
	return DAM;
};
bool MakeRoundDamage(int x,int y,int r,word Damage,OneObject* Sender,word Attr){
	int cx=(x>>11);
	int cy=(y>>11);
	int cell=cx+(cy<<VAL_SHFCX)+VAL_MAXCX+1;
	ExplMedia=-1;
	return 
	DamageInCell(cell-VAL_MAXCX-1,x,y,r,Damage,Sender,Attr)||
	DamageInCell(cell-VAL_MAXCX  ,x,y,r,Damage,Sender,Attr)||
	DamageInCell(cell-VAL_MAXCX+1,x,y,r,Damage,Sender,Attr)||
	DamageInCell(cell-1      ,x,y,r,Damage,Sender,Attr)||
	DamageInCell(cell        ,x,y,r,Damage,Sender,Attr)||
	DamageInCell(cell+1      ,x,y,r,Damage,Sender,Attr)||
	DamageInCell(cell+VAL_MAXCX-1,x,y,r,Damage,Sender,Attr)||
	DamageInCell(cell+VAL_MAXCX  ,x,y,r,Damage,Sender,Attr)||
	DamageInCell(cell+VAL_MAXCX+1,x,y,r,Damage,Sender,Attr);
};
bool OneObject::GetDamagePoint(int xo,int yo,Coor3D* dp,int Precise){
	NewMonster* NM=newMons;
	if(NM->NBars){
		int bx0=(RealX>>4)+NM->PicDx;
		int by0=(RealY>>4)+(NM->PicDy<<1);
		int N=(int(rando())*int(NM->NBars))>>15;
		int pp=N*5;
		int XB0=NM->Bars3D[pp]+bx0;
		int YB0=(NM->Bars3D[pp+1]<<1)+by0;
		int L1=NM->Bars3D[pp+2];
		int L2=NM->Bars3D[pp+3];
		dp->x=XB0+((L1+L2)>>1);
		dp->y=YB0+((L1-L2)>>1);
		dp->z=GetHeight(dp->x,dp->y)+(NM->Bars3D[pp+4]>>1);
	}else{
		dp->x=RealX>>4;
		dp->y=RealY>>4;
		dp->z=RZ+NM->DstZPoint;
	};
	x+=(rando()*Precise)>>15;
	y+=(rando()*Precise)>>15;
	return NewBuilding;
};

//Type:
//0 - single order (previous orders will be erased)
//1 - add order to the head of link
//2 - add order to the tile of link
Order1* OneObject::CreateOrder(byte Type){
	if(UnlimitedMotion)return NULL;
	Order1* OR1=GetOrdBlock();
	Order1* OR2;
	if(!OR1)return NULL;
	switch(Type){
	case 1:
		OR1->NextOrder=LocalOrder;
		LocalOrder=OR1;
		break;
	case 2:
		OR1->NextOrder=NULL;
		if(LocalOrder){
			OR2=LocalOrder;
			while(OR2->NextOrder)OR2=OR2->NextOrder;
			OR2->NextOrder=OR1;
		}else LocalOrder=OR1;
		break;
	default:
		ClearOrders();
		LocalOrder=OR1;
		OR1->NextOrder=NULL;
	};
	return OR1;
};
void NewMonsterSmartSendToLink(OneObject* OBJ);
int GetOrderKind(ReportFn* RF);
#undef DeleteLastOrder
void NewAttackPointLink(OneObject* OBJ);
void MakeOneShotLink(OneObject* OBJ);
void PanicUnitLink(OneObject* OBJ);
extern bool InClOrders;
void OneObject::DeleteLastOrder(){
	BackMotion=0;
	DeletePath();
	DestX=-1;
	Order1* OR1=LocalOrder;
	if(!OR1)return;
	if(OR1->DoLink==&PanicUnitLink){
		UnlimitedMotion=0;
	};
	if(OR1->DoLink==&AttackObjLink||OR1->DoLink==&WaterAttackLink){
		Attack=false;
		if(newMons->SlowRecharge){
			int ns=NewState;
			if(ns&&WeaponFlags[newMons->WeaponKind[ns-1]]&1&&NewAnm==&newMons->Attack[ns-1]){
				if(EnemyID!=0xFFFF){
					OneObject* EOB=Group[EnemyID];
					if(EOB&&EOB->Sdoxlo>8){
						MakeOneShot(this,EOB->RealX>>4,EOB->RealY>>4,EOB->RZ+40);
					};
				};
				if(LocalOrder&&LocalOrder->DoLink!=&MakeOneShotLink){
					NewState=4;
					LocalNewState=4;
					StandTime=20;
				};
				
			};
		};
		EnemyID=0xFFFF;
		EnemySN=0xFFFF;
	};
	if(OR1->DoLink==NewAttackPointLink){
		DstX=-1;
		DstY=-1;
	};
	if(LocalOrder){
		//addrand(GetOrderKind(LocalOrder->DoLink));
	};
	LocalOrder=OR1->NextOrder;
	FreeOrdBlock(OR1);
	//search for patrol commands
	bool Patr=false;
	bool Atta=false;
	OR1=LocalOrder;
	while(OR1){
		if(OR1->DoLink==&AttackObjLink)Atta=true;
		else
		if(OR1->DoLink==&NewMonsterSmartSendToLink||OR1->DoLink==NewMonsterSendToLink&&
		   OR1->PrioryLevel<5)Patr=true;
		OR1=OR1->NextOrder;
	};
	Attack=Atta;
	InPatrol=Patr;
	UnBlockUnit();
#ifdef COSSACKS2
	if((!InClOrders)&&LocalOrder&&LocalOrder->DoLink&&LocalOrder->OrderType==12)LocalOrder->DoLink(this);
	int GetOrderKind(ReportFn* RF);
	if(LocalOrder){
		//int okn=GetOrderKind(LocalOrder->DoLink);
		//assert(okn!=-1);
	};
#endif //COSSACKS2
};
//#define DeleteLastOrder() DeleteLastOrder();addrand(0)
OneObject* SearchEnemyInCell(int cell,byte nmask,byte mmask,byte Priest){
    cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	if(!NMon)return NULL;
	int ofs1=cell<<SHFCELL;
	word MID;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&!(OB->Sdoxlo||(OB->AI_Guard&&!OB->Attack))){
				if(Priest==3){
					if(OB->newMons->Capture&&TestCapture(OB)==0)return OB;
				}else
				if(Priest==2&&(!(OB->NMask&nmask))&&OB->newMons->Capture&&TestCapture(OB)==0){
					return OB;
				}else{
					if((!(OB->NMask&nmask))&&OB->newMons->MathMask&mmask){
#ifdef NEWMORALEPRIEST
                        if (Priest == 1) {
                            if (OB->Morale < OB->MaxMorale)return OB;
                        }
                        else if (Priest == 4) {
                            if (OB->Life < OB->MaxLife)return OB;
                        }
                        else return OB;
#else
                        if (Priest == 1) {
                            if (OB->Life < OB->MaxLife)return OB;
                        }
                        else return OB;
#endif
					};
				};
			};
		};
	};
    return NULL;
};
OneObject* SearchEnemyInCell3(int cell,byte nmask,byte mmask,byte Priest){
    cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	if(!NMon)return NULL;
	int ofs1=cell<<SHFCELL;
	word mons[64];
	int NMONS=0;
	word MID;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&!OB->Sdoxlo){
				if((!(OB->NMask&nmask))&&OB->newMons->MathMask&mmask&&NMONS<64){
					mons[NMONS]=MID;
					NMONS++;
				};
			};
		};
	};
	if(NMONS){
		return Group[mons[(NMONS*int(rando()))>>15]];
	};
    return NULL;
};
OneObject* SearchEnemyGroupInCell(int cell,byte nmask,byte mmask){
    cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	if(NMon<3)return NULL;
	int ofs1=cell<<SHFCELL;
	word MID;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&OB->NMask&nmask&&OB->newMons->MathMask&mmask&&!OB->LockType==1)return OB;
		};
	};
    return NULL;
};
bool CheckVisibility(int x1,int y1,int x2,int y2,word MyID);
void SearchEnemyForAIArtillery(OneObject* OB);
void AIArtilleryAgainstTowers(OneObject* OB);
void SearchEnemyForMortira(OneObject* OB);
void EraseBrigade(Brigade* BR);
int TestCapture(OneObject* OBJ);
void SearchVictimForAI_Archer(OneObject* OB){
	int x0=OB->RealX>>11;
	int y0=OB->RealY>>11;
	NewMonster* NM=OB->newMons;
	int rr=NM->VisRange;
	int rx1=(rr>>11)+1;
	int rx2=rx1+rx1+1;
	byte nmask=~OB->NMask;
	OneObject* DestObj=NULL;
	int stcell;
	int NSR=6;
	int RMIN=rr+100;
	for(int i=0;i<6;i++){
		int dx=x0+((rando()*rx2)>>15)-rx1;
		int dy=y0+((rando()*rx2)>>15)-rx1;
		if(dx>=0&&dy>0&&dx<VAL_MAXCX-2&&dy<VAL_MAXCX-2){
			stcell=dx+(dy<<VAL_SHFCX);
			word BMID=BLDList[stcell+VAL_MAXCX+1];
			if(BMID!=0xFFFF){
				OneObject* BOB=Group[BMID];
				if(BOB&&(!BOB->Sdoxlo)&&BOB->NMask&nmask){
					int r=Norma(BOB->RealX-OB->RealX,BOB->RealY-OB->RealY)>>4;
					if(r<RMIN){
						RMIN=r;
						DestObj=BOB;
					};
				};
			}else{
				int wcell=GetLI(dx+dx,dy+dy);
				for(int vx=0;vx<2;vx++)
					for(int vy=0;vy<2;vy++){
						int wcl=wcell+vx+(vy<<TopSH);
						WallCell* WCL=WRefs[wcl];
						if(WCL){
							if(WCL->NI==0&&WCL->OIndex!=0xFFFF){
								OneObject* BOB=Group[WCL->OIndex];
								if(BOB&&!BOB->Sdoxlo){
									int r=Norma(BOB->RealX-OB->RealX,BOB->RealY-OB->RealY)>>4;
									if(r<RMIN){
										RMIN=r;
										DestObj=BOB;
									};
								};
							};
						};
					};
			};
		};
    };
	if(DestObj)OB->AttackObj(DestObj->Index,16+128,0,0);
};
int GetUnitActivity(OneObject* OB);
bool CheckVictimInAngle(int x,int y,WeaponInSector* WS,byte Angle,byte Mask);
void OneObject::SearchVictim(){
#ifdef SIMPLEMANAGE
	if(PeaceMode||AI_Guard||NoSearchVictim)return;
#else //SIMPLEMANAGE
	if(PeaceMode||NoSearchVictim||AI_Guard)return;
#endif //SIMPLEMANAGE
	int ac1=GetUnitActivity(this);
	if(ac1){
		if(newMons->Peasant||newMons->Usage==TowerID||newMons->Artilery||Media)return;
	};
	NewMonster* NM=newMons;
	if(NM->Archer&&Nat->AI_Enabled){
		return;
	};
	if(NM->WSECT){
#ifdef COSSACKS2
		if(!RifleAttack)return;
#endif //COSSACKS2
		int R=NM->WSECT->RMax;
		NM->WSECT->RMax=NM->VisRange>>4;
		if((!LocalOrder)&&PrioryLevel<15&&LocalNewState!=0&&(!delay)&&NewAnm->NFrames-FrmDec<=NewCurSprite&&CheckVictimInAngle(RealX>>4,RealY>>4,NM->WSECT,RealDir,NMask)){
			NewAttackPoint((RealX>>4)+TCos[RealDir],(RealY>>4)+TSin[RealDir],128+16,0,1);
		};
		NM->WSECT->RMax=R;
		return;
	};
	int minR=int(Ref.General->MoreCharacter->MinR_Attack)<<4;
	int REALX=RealX;
	int REALY=RealY;
	int rr=NM->VisRange;
	int VRS=NM->VisRangeShift;
#ifdef COSSACKS2
	if(BrigadeID!=0xFFFF){
		rr+=RZ<<(1+4);
		if(StandGround&&!RifleAttack){
			rr=120*16;
			VRS=0;
		};
	};
#endif //COSSACKS2
	if(NM->VisRangeShift){
		REALX+=(int(TCos[RealDir])*VRS)>>8;
		REALY+=(int(TSin[RealDir])*VRS)>>8;

	};
	if(LockType==1&&Nat->AI_Enabled&&EnemyID==0xFFFF){
		if(delay)return;
		
		bool Kill=false;
		/*
		if(EnemyID!=0xFFFF){
			OneObject* OB=Group[EnemyID];
			if(OB&&EnemySN==OB->Serial){
				if(OB->NewBuilding)Kill=true;
				else if(delay||OB->LockType)Kill=false;
			}else Kill=true;
		}else Kill=true;
		if(!Kill)return;
		*/
		//search victim for AI Battleship
		int x0=REALX>>11;
		int y0=REALY>>11;
		int maxx=msx>>2;
		int DY=TopLx>>1;
		//int cell=((y0)<<VAL_SHFCX)+(x0>11);

		
		if(NM->Usage==GaleraID)rr=NM->AttackRadius2[1];
		int rx1=(rr>>11)+1;
		byte MMASK=NMask;
		byte nmask=~NMask;
		byte mmask=NM->KillMask;
		OneObject* DestObj=NULL;
		int mindist=10000000;
		int dist;
		int rx2=rx1+rx1+1;
		int stcell;
		for(int i=0;i<6;i++){
			int dx=x0+((rando()*rx2)>>15)-rx1;
			int dy=y0+((rando()*rx2)>>15)-rx1;
			if(dx>0&&dx<maxx&&dy>=0&&dy<maxx){
				stcell=dx+(dy<<VAL_SHFCX);
				byte BPT=NPresence[stcell];
				if((BPT&nmask)&&!((BPT&MMASK)||
					(NPresence[stcell-1]&MMASK)||(NPresence[stcell+1]&MMASK)||
					(NPresence[stcell-DY]&MMASK)||(NPresence[stcell+DY]&MMASK))){
					OneObject*OB=SearchEnemyGroupInCell(stcell,nmask,mmask);
					if(OB){
						dist=Norma(OB->RealX-REALX,OB->RealY-REALY);
						if(dist>minR&&dist<mindist){
							mindist=dist;
							DestObj=OB;
						};
					};
				};
			};
		};
		if(DestObj){
			if(LocalOrder)AttackObj(DestObj->Index,128+16,1,1);
			else AttackObj(DestObj->Index,128+16,1,4);
		};
		return;
	};
	addname(Index);
	if(Zombi){
		if(newMons->Usage==PushkaID){
			SearchEnemyForAIArtillery(this);
			return;
		};
		if(newMons->Usage==MortiraID){
			SearchEnemyForMortira(this);
			return;
		};
		if(newMons->Usage==SupMortID&&GNFO.EINF[NNUM]&&!delay){
			int BX,BY;
			word MID=GNFO.EINF[NNUM]->SearchBestEnemyAndPlaceForSupermortira(this,&BX,&BY);
			if(MID!=0xFFFF){
				if(BrigadeID!=0xFFFF){
					Nat->CITY->UnRegisterNewUnit(this);
					Zombi=true;
				};
				if(BX!=REALX){
					NewMonsterSmartSendTo(BX>>4,BY>>4,0,0,128+64,0);
					delay=16;
					MaxDelay=16;
				}else{
					OneObject* OB=Group[MID];
					if(OB)AttackPoint(OB->RealX>>4,OB->RealY>>4,OB->RZ+32,1,ATTP_IMMEDIATECHECK,0,128+64);
				};
			}else{
				//need to connect to nearest brigade
				if(BrigadeID==0xFFFF){
					//search for the nearest army
					AI_Army* BestArmy=NULL;
					int MinTopDist=1000000;
					int BestFinalTop=0xFFFF;
					int MyTop=GetTopology(REALX>>4,REALY>>4);
					if(MyTop<0xFFFE){
						int TOPSTART=MyTop*GetNAreas();
						AI_Army* ARM=Nat->CITY->ARMS;
						for(int i=0;i<MaxArm;i++){
							if(ARM->Enabled&&(ARM->Spec==0||ARM->Spec==1)){		
								int top=ARM->TopPos;
								if(top<0xFFFE){
									int topdst=GetLinksDist(top+TOPSTART);
									if(top==MyTop)topdst=0;
									if(topdst<MinTopDist){
										MinTopDist=topdst;
										BestFinalTop=top;
										BestArmy=ARM;
									};
								};
							};
							ARM++;
						};
						if(BestFinalTop!=0xFFFF){
							int NextTop=GetMotionLinks(TOPSTART+BestFinalTop);
							if(NextTop==BestFinalTop||MinTopDist==0){
								//we can connect now!
								City* CT=Nat->CITY;
								int BID=CT->GetFreeBrigade();
								if(BID!=-1){
									Brigade* BR=CT->Brigs+BID;
									BR->Enabled=true;
									BR->AddObject(this);
									BestArmy->AddBrigade(BR);
									BR->DeleteAll();
									EraseBrigade(BR);
									BestArmy->Parad();
								};
							}else{
								//move to next topology zone
								if(NextTop<0xFFFE){
									Area* AR=GetTopMap(NextTop);
									NewMonsterSendTo(int(AR->x)<<10,int(AR->y)<<10,128+16,0);
								};
							};
						};
					};
					delay=50;
					MaxDelay=50;
				};
			};
			//AIArtilleryAgainstTowers(this);
			return;
		};
		int cell=((REALY>>11)<<VAL_SHFCX)+(REALX>>11);
	    NewMonster* NM=newMons;
	    int rr=NM->VisRange;
		int rx1=6;
		byte nmask=~NMask;
	    byte mmask=NM->KillMask;
		OneObject* DestObj=NULL;
		int mindist=10000000;
		int dist;
		int rx2=rx1+rx1+1;
		int stcell;

		for(int i=0;i<6;i++){
			int dx=((rando()*rx2)>>15)-rx1;
			int dy=((rando()*rx2)>>15)-rx1;
			stcell=cell+dx+(dy<<VAL_SHFCX);
			if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
				if(NPresence[stcell]&nmask){
					OneObject*OB=SearchEnemyInCell(stcell,~nmask,mmask,3);
					if(OB){
						dist=Norma(OB->RealX-REALX,OB->RealY-REALY);
						if(dist<mindist&&dist>minR){
							mindist=dist;
							DestObj=OB;
						};
					};
				};
			};
        };
		if(DestObj){
			int t0=GetTopology(REALX>>4,REALY>>4);
			if(t0>=0xFFFE)return;
			int t1=GetTopology(DestObj->RealX>>4,DestObj->RealY>>4);
			if(t0==t1||GetLinksDist(t1*GetNAreas()+t0)<40)AttackObj(DestObj->Index,1);
		};
		return;
	};
	
    int cell=((REALY>>11)<<VAL_SHFCX)+(REALX>>11);
    
    int rx1=(rr>>11)+1;
    byte nmask=~NMask;
	byte Priest=NM->Priest;
#ifdef NEWMORALEPRIEST
    if (NM->Shaman) {
        if (rando() < 4096) {
            if (NM->MoraleHealer) Priest = 1;
            else Priest = 4;
        };
    };
    if (Priest) {
        nmask = NMask;
        if (NM->MoraleHealer) Priest = 1;
        else Priest = 4;
    }
    else {
        if (!(NM->Capture || LockType == 1 || NewBuilding))Priest = 2;
    };
#else
    if (NM->Shaman) {
        if (rando() < 4096) {
            Priest = 1;
        };
    };
    if (Priest) {
        nmask = NMask;
        Priest = 1;
    }
    else {
        if (!(NM->Capture || LockType == 1 || NewBuilding))Priest = 2;
    };
#endif
    byte mmask=NM->KillMask;
    OneObject* DestObj=NULL;
    int mindist=10000000;
    int dist;
#ifdef CONQUEST
	if(TempDelay&&NInside){
		word VICTIMS[1024];
		int NVICT=0;
		int rx2=rx1+rx1+1;
        int stcell=cell-rx1-(rx1<<VAL_SHFCX);
        byte* bpt=NPresence+stcell;
        for(int nx=0;nx<rx2;nx++){
            for(int ny=0;ny<rx2;ny++){
                if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
                    if(bpt[0]&nmask){
                        OneObject*OB=SearchEnemyInCell3(stcell,~nmask,mmask,Priest);
                        if(OB){
							dist=Norma(OB->RealX-REALX,OB->RealY-REALY);
							if(dist>minR&&NVICT<1024){
								VICTIMS[NVICT]=OB->Index;
								NVICT++;
							};
                        };
                    };
                };
                stcell++;
                bpt++;
            };
            stcell+=VAL_MAXCX-rx2;
            bpt+=VAL_MAXCX-rx2;
        };
		if(NVICT){
			DestObj=Group[VICTIMS[(NVICT*int(rando()))>>15]];
			mindist=100;
		};

	}else
	if(rx1<=10){
		if(rando()>8192)return;
#else
    if(rx1<=5){
#endif
		addname(Index);
        //short range search
        int rx2=rx1+rx1+1;
        int stcell=cell-rx1-(rx1<<VAL_SHFCX);
        byte* bpt=NPresence+stcell;
        for(int nx=0;nx<rx2;nx++){
            for(int ny=0;ny<rx2;ny++){
                if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
                    if(bpt[0]&nmask){
                        OneObject*OB=SearchEnemyInCell(stcell,~nmask,mmask,Priest);
                        if(OB){
                            dist=Norma(OB->RealX-REALX,OB->RealY-REALY);
                            if(dist<mindist&&dist>minR){
                                mindist=dist;
                                DestObj=OB;
								addname(OB->Index);
								addrand(OB->RealX);
								addrand(OB->RealY);
                            };
                        };
                    };
                };
                stcell++;
                bpt++;
            };
               stcell+=VAL_MAXCX-rx2;
               bpt+=VAL_MAXCX-rx2;
        };
    }else{
		//near nose search
		if(Ref.General->MoreCharacter->MinR_Attack<140&&!(StandTime>16&&rando()>6000)){
			addname(Index);
			int rxx=1;
			int rx2=rxx+rxx+1;
			int stcell=cell-rxx-(rxx<<VAL_SHFCX);
			byte* bpt=NPresence+stcell;
			for(int nx=0;nx<rx2;nx++){
				for(int ny=0;ny<rx2;ny++){
					if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
						if(bpt[0]&nmask){
							OneObject*OB=SearchEnemyInCell(stcell,~nmask,mmask,Priest);
							if(OB){
								dist=Norma(OB->RealX-REALX,OB->RealY-REALY);
								if(dist<mindist&&dist>minR){
									mindist=dist;
									DestObj=OB;
									addname(OB->Index);
									addrand(OB->RealX);
									addrand(OB->RealY);
								};
							};
						};
					};
				};
				stcell++;
				bpt++;
           };
           stcell+=VAL_MAXCX-rx2;
           bpt+=VAL_MAXCX-rx2;
		};
		if(!DestObj){
			addname(Index);
	        //long range search
			int rx2=rx1+rx1+1;
			int stcell;
			int NSR=6;
			if(NM->Usage==TowerID)NSR=20;
			if(TempDelay)NSR=50;
			for(int i=0;i<6;i++){
				int dx=((rando()*rx2)>>15)-rx1;
				int dy=((rando()*rx2)>>15)-rx1;
				stcell=cell+dx+(dy<<VAL_SHFCX);
				if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
					rando();
					if(NPresence[stcell]&nmask){
						rando();
						OneObject*OB=SearchEnemyInCell(stcell,~nmask,mmask,Priest);
						if(OB){
							rando();
							dist=Norma(OB->RealX-REALX,OB->RealY-REALY);
							if(dist<mindist&&dist>minR){
								mindist=dist;
								DestObj=OB;
								addname(OB->Index);
								addrand(OB->RealX);
								addrand(OB->RealY);
							};
						};
					};
				};
            };
        };
    };
	//if(!DestObj&&!(newMons->Capture||NewBuilding||LockType)){
	//};
	//addname(Index);
	//if(DestObj)addname(DestObj->Index);
    if(DestObj&&mindist<rr){
		int ac2=GetUnitActivity(DestObj);
		if(ac2!=1||DestObj->NMask&NMask){
			addrand(DestObj->Index);
			if(!CheckVisibility(DestObj->RealX,DestObj->RealY,REALX,REALY,Index))return;
			addrand(DestObj->Index);	
			AttackObj(DestObj->Index,1);
		};
	}else addrand(Index+10000);
};
int GetTopDistance(int xa,int ya,int xb,int yb);
word SearchVictim(OneObject* OBJ,int R0,int R1){
    int cell=((OBJ->RealY>>11)<<VAL_SHFCX)+(OBJ->RealX>>11);
    NewMonster* NM=OBJ->newMons;
    int rr=R1<<4;
	int rmin=R0<<4;
    int rx1=(R1>>7)+1;
    byte nmask=~OBJ->NMask;
    byte mmask=NM->KillMask;
    OneObject* DestObj=NULL;
    int mindist=10000000;
    int dist;
    //short range search
    int rx2=rx1+rx1+1;
    int stcell=cell-rx1-(rx1<<VAL_SHFCX);
    byte* bpt=NPresence+stcell;
	int RealX=OBJ->RealX;
	int RealY=OBJ->RealY;
    for(int nx=0;nx<rx2;nx++){
        for(int ny=0;ny<rx2;ny++){
            if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
                if(bpt[0]&nmask){
                    OneObject*OB=SearchEnemyInCell(stcell,~nmask,mmask,0);
                    if(OB){
                        dist=Norma(OB->RealX-RealX,OB->RealY-RealY);
                        if(dist>rmin&&dist<mindist){
                            mindist=dist;
                            DestObj=OB;
                        };
                    };
                };
            };
            stcell++;
            bpt++;
        };
        stcell+=VAL_MAXCX-rx2;
        bpt+=VAL_MAXCX-rx2;
    };
    if(DestObj&&mindist<rr){
		if(GetTopDistance(OBJ->RealX>>10,OBJ->RealY>>10,DestObj->RealX>>10,DestObj->RealY>>10)<20)
			return DestObj->Index;
	};
	return 0xFFFF;
};
void SetUnlimitedLink(OneObject* OB){
	OB->UnlimitedMotion=true;
	OB->DeleteLastOrder();
};
void OneObject::SetOrderedUnlimitedMotion(byte OrdType){
	if(CheckOrderAbility())return;
	Order1* Or1=CreateOrder(OrdType);
	if(!int(Or1))return;
	Or1->OrderType=57;
	Or1->OrderTime=0;
	Or1->PrioryLevel=0;
	Or1->DoLink=&SetUnlimitedLink;	
};
bool FINDNEAREMPTY(int* x,int* y){
	int xx=*x;
	int yy=*y;
	for(int r=1;r<30;r++){
		int N=Rarr[r].N;
		char* dx=Rarr[r].xi;
		char* dy=Rarr[r].yi;
		for(int p=0;p<N;p++){
			if(!CheckBar(xx+dx[p]-4,yy+dy[p]-4,9,9)){
				*x=xx+dx[p];
				*y=yy+dy[p];
				return 1;
			};
		};
	};
	return 0;
};
void ClearUnlimitedLink(OneObject* OB){
	Order1* OR1=OB->LocalOrder->NextOrder;
	if(OB->StandTime<5&&!OR1)return;
	if(CheckBar(OB->x-1,OB->y-1,3,3)){
		int xx=OB->x;
		int yy=OB->y;
		if(FINDNEAREMPTY(&xx,&yy)){
			OB->UnlimitedMotion=0;
			OB->NewMonsterSendTo(xx<<8,yy<<8,128+16,1+128);
			OB->UnlimitedMotion=1;
			return;
		};
	};
	OB->UnlimitedMotion=false;
	word GID=OB->LocalOrder->info.BuildObj.ObjIndex;
	OB->DeleteLastOrder();
	OB->PrioryLevel=0;
	OB->NextForceX=OB->newMons->OneStepDX[OB->RealDir];
	OB->NextForceY=OB->newMons->OneStepDY[OB->RealDir];
	if(GID!=0xFFFF&&GID<SCENINF.NUGRP){
		UnitsGroup* UG=SCENINF.UGRP+GID;
		CheckDynamicalPtr(UG->IDS);
		UG->Allocate(UG->N+1);
		UG->IDS[UG->N]=OB->Index;
		UG->SNS[UG->N]=OB->Serial;
		UG->N++;
		CheckDynamicalPtr(UG->IDS);
		if(UG->NMASK)OB->NMask=UG->NMASK;
	};
};
void OneObject::ClearOrderedUnlimitedMotion(byte OrdType,word GroupID){
	if(CheckOrderAbility())return;
	Order1* Or1=CreateOrder(OrdType);
	if(!int(Or1))return;
	Or1->OrderType=57;
	Or1->OrderTime=0;
	Or1->PrioryLevel=0;
	Or1->info.BuildObj.ObjIndex=GroupID;
	Or1->DoLink=&ClearUnlimitedLink;	
};
void OneObject::ClearOrderedUnlimitedMotion(byte OrdType){
	ClearOrderedUnlimitedMotion(OrdType,0xFFFF);
};
OneObject* SearchCapturers(int cell,byte mmask){
    cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	if(!NMon)return NULL;
	int ofs1=cell<<SHFCELL;
	word MID;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB){
				NewMonster* NM=OB->newMons;
				if(!(OB->NMask&mmask||NM->Capture||NM->CantCapture||OB->NNUM==7))return OB;
			};
		};
	};
    return NULL;
};
OneObject* SearchProtectors(int cell,byte mmask){
    cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	if(!NMon)return NULL;
	int ofs1=cell<<SHFCELL;
	word MID;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&(OB->NMask&mmask)&&!(OB->newMons->Capture||OB->Sdoxlo))return OB;
		};
	};
    return NULL;
};
bool CheckProtectors_Walls(int x,int y,byte mask){
	int LI=GetLI(x<<1,y<<1);
	for(int iy=0;iy<2;iy++){
		for(int ix=0;ix<2;ix++){
			WallCell* WC=WRefs[LI];
			if(WC&&NATIONS[WC->NI].NMask&mask)return true;
			LI++;
		};
		LI+=TopLx-2;
	};
	return false;
};
int GetProtectors(int cell,byte mmask){
    cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	if(!NMon)return NULL;
	int ofs1=cell<<SHFCELL;
	word MID;
	int N=0;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&(OB->NMask&mmask)&&!(OB->newMons->Capture||OB->Sdoxlo))N++;
		};
	};
    return N;
};
word GetNearestDefender(OneObject* OBJ){
	int cell=((OBJ->RealY>>11)<<VAL_SHFCX)+(OBJ->RealX>>11);
	int CELL0=cell;
	int rx1=3;
    int rx2=rx1+rx1+1;
    int stcell=cell-rx1-(rx1<<VAL_SHFCX);
	byte NMASK=OBJ->NMask;
	byte* bpt=NPresence+stcell;
	for(int nx=0;nx<rx2;nx++){
		for(int ny=0;ny<rx2;ny++){
			if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
				if(bpt[0]&NMASK){
					OneObject* EOB=SearchProtectors(stcell,NMASK);
					if(EOB)return EOB->Index;
				};
			};
			stcell++;
			bpt++;
		};
		stcell+=VAL_MAXCX-rx2;
		bpt+=VAL_MAXCX-rx2;
	};
	return 0xFFFF;
};
int TestCapture(OneObject* OBJ){
	if(!(OBJ->newMons->Capture||!(OBJ->Ready||OBJ->LocalOrder)))return -1;
	switch(CaptState){
	case 1:
		if(OBJ->newMons->Peasant)return -1;
		break;
	case 2:
		if(OBJ->newMons->Peasant||OBJ->newMons->Usage==CenterID||OBJ->newMons->Usage==MineID)return -1;
		break;
	case 3:
		if(!OBJ->newMons->Artilery)return -1;
		break;
	};
	int cell=((OBJ->RealY>>11)<<VAL_SHFCX)+(OBJ->RealX>>11);

	int X0=OBJ->RealX>>10;
	int Y0=OBJ->RealY>>10;

	int CELL0=cell;
    NewMonster* NM=OBJ->newMons;
    byte nmask=~OBJ->NMask;
	byte NMASK=OBJ->NMask;
    OneObject* DestObj=NULL;
    int mindist=10000000;
	int rx1=2;
	//X0-=rx1+rx1;
	//Y0-=rx1+rx1;
	bool Capture=true;
    int rx2=rx1+rx1+1;
    int stcell=cell-rx1-(rx1<<VAL_SHFCX);
    byte* bpt=NPresence+stcell;
	//byte MyMask=OBJ->NMask;
	if(!(OBJ->Wall&&OBJ->Life<OBJ->MaxLife/3)){
		rx1=3;
		rx2=rx1+rx1+1;
		stcell=CELL0-rx1-(rx1<<VAL_SHFCX);
		//checking help
		bpt=NPresence+stcell;
		for(int nx=0;nx<rx2&&Capture;nx++){
			for(int ny=0;ny<rx2&&Capture;ny++){
				if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
					if(bpt[0]&NMASK){
						OneObject*OB=SearchProtectors(stcell,NMASK);
						if(OB&&!OB->newMons->Capture)Capture=false;
						//if(CheckProtectors_Walls(X0+nx+nx,Y0+ny+ny,MyMask))Capture=false;
					};
				};
				stcell++;
				bpt++;
			};
			stcell+=VAL_MAXCX-rx2;
			bpt+=VAL_MAXCX-rx2;
		};
    };
	if(Capture)return 0;
	else return -1;
};
extern int LastAttackDelay;
extern short AlarmSoundID;
extern int LastAttackX;
extern int LastAttackY;
extern int AlarmDelay;
void CheckCapture(OneObject* OBJ){
	switch(CaptState){
	case 1:
		if(OBJ->newMons->Peasant)return;
		break;
	case 2:
		if(OBJ->newMons->Peasant||OBJ->newMons->Usage==CenterID||OBJ->newMons->Usage==MineID)return;
		break;
	case 3:
		if(!(OBJ->newMons->Artilery||OBJ->Wall))return;
		break;
	};
	int cell=((OBJ->RealY>>11)<<VAL_SHFCX)+(OBJ->RealX>>11);

	//int X0=OBJ->RealX>>10;
	//int Y0=OBJ->RealY>>10;

	int CELL0=cell;
    NewMonster* NM=OBJ->newMons;
    byte nmask=~OBJ->NMask;
	byte NMASK=OBJ->NMask;
    OneObject* DestObj=NULL;
    int mindist=10000000;
    int dist;
	int rx1=2;

	//X0-=rx1+rx1;
	//Y0-=rx1+rx1;

	bool Capture=false;
    int rx2=rx1+rx1+1;
    int stcell=cell-rx1-(rx1<<VAL_SHFCX);
	byte CapNation;
    byte* bpt=NPresence+stcell;
	int NCapt=0;
	OneObject* CAPUNIT=NULL;
	//byte MyMask=OBJ->NMask;
    for(int nx=0;nx<rx2;nx++){
        for(int ny=0;ny<rx2;ny++){
            if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
                if(bpt[0]&nmask){
                    OneObject*OB=SearchCapturers(stcell,NMASK);
                    if(OB&&!OB->newMons->Capture){
                        dist=Norma(OB->RealX-OBJ->RealX,OB->RealY-OBJ->RealY);
                        if(dist<250*16){
							Capture=true;
							CapNation=OB->NNUM;
							NCapt++;
							CAPUNIT=OB;
						};
                    };
                };
            };
            stcell++;
            bpt++;
        };
        stcell+=VAL_MAXCX-rx2;
        bpt+=VAL_MAXCX-rx2;
    };
	int npro=0;
	if(Capture){
		int ac1=GetUnitActivity(OBJ);
		if(ac1==1&&!CAPUNIT)return;
		if(CAPUNIT){
			int ac2=GetUnitActivity(CAPUNIT);
			if(ac1==1){
				CAPUNIT->Die();
				return;
			};
		};
	};
	if(Capture&&!(OBJ->Wall&&OBJ->Life<OBJ->MaxLife/3)){
		if(!OBJ->newMons->Peasant){
			OBJ->delay=100;
			OBJ->MaxDelay=100;
			addname(OBJ->Index);
		};
		rx1=3;
		rx2=rx1+rx1+1;
		stcell=CELL0-rx1-(rx1<<VAL_SHFCX);
		//checking help
		bpt=NPresence+stcell;
		for(int nx=0;nx<rx2;nx++){
			for(int ny=0;ny<rx2;ny++){
				if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
					if(bpt[0]&NMASK){
						OneObject*OB=SearchProtectors(stcell,NMASK);
						if(OB&&!OB->newMons->Capture){
							Capture=false;
							npro++;
						};
						//if(CheckProtectors_Walls(X0+nx+nx,Y0+ny+ny,MyMask))Capture=false;
					};
				};
				stcell++;
				bpt++;
			};
			stcell+=VAL_MAXCX-rx2;
			bpt+=VAL_MAXCX-rx2;
		};
		if(OBJ->Nat->AI_Enabled&&OBJ->newMons->Artilery){
			if(npro&&(NCapt>=npro&&npro==1)||(NCapt>3&&npro==2)||(NCapt>7&&npro==3)||(NCapt>10&&npro==4)){
				OBJ->Die();
				return;
			};
		};
    };
	if(Capture){
		byte OldNat=OBJ->NNUM;
		bool Easy=CITY[OldNat].Difficulty<3;
		if(OBJ->UnlimitedMotion&&!OBJ->Hidden){
			OBJ->Die();
			return;
		};
		if(OBJ->AutoKill||OBJ->Wall){
			int idx=OBJ->Index;
			DestructBuilding(OBJ);
			if(Group[idx]&&OBJ->Sdoxlo)return;
		}else{
			/*
			if(NATIONS[OldNat].AI_Enabled){
				int ttt=tmtmt&63;
				if(OBJ->NewBuilding&&ttt>16)DestructBuilding(OBJ);
				else{
					byte use=OBJ->newMons->Usage;
					if(use==PeasantID&&!Easy){
						DestructBuilding(OBJ);
					}else{
						if((use==SupMortID&&ttt>30)||(use==PushkaID&&ttt>20)||
							(use==MortiraID&&ttt>8)||(use==PeasantID&&ttt>35)){
								DestructBuilding(OBJ);
							return;
						};
					};
				};
			}else{
			*/
				if(NATIONS[CapNation].AI_Enabled){
					if(OBJ->newMons->Usage==PeasantID&&!Easy){
						DestructBuilding(OBJ);
						return;
					};
				};
			//};
		};
		NATIONS[OldNat].NKilled[OBJ->NIndex]++;
		OBJ->Nat->CITY->Account-=OBJ->newMons->Ves*5;
		CITY[CapNation].Account+=OBJ->newMons->Ves*5;
		if(OBJ->NNUM==MyNation&&OBJ->NewBuilding){
			char ccc[200];
			sprintf_s(ccc,sizeof(ccc),CAPBLD,OBJ->Ref.General->Message);
			AssignHint1(ccc,100,32);
		};
		if(OBJ->NNUM==MyNation){
			LastActionX=OBJ->RealX>>4;
			LastActionY=OBJ->RealY>>4;
			LastAttackX=OBJ->RealX;
			LastAttackY=OBJ->RealY;
			AlarmDelay=60;
		};
		if(OBJ->newMons->Usage==PeasantID)OBJ->ClearOrders();
		//if(OBJ->NewBuilding&&(!OBJ->Ready)&&OBJ->Stage>=OBJ->Ref.General->MoreCharacter->ProduceStages)
		//	OBJ->Ready=true;
		DelObject(OBJ);
		OBJ->Nat->CITY->UnRegisterNewUnit(OBJ);
		int oldst=OBJ->Stage;
		int oldmax=OBJ->Ref.General->MoreCharacter->ProduceStages;
		OBJ->Ref.General=NATIONS[CapNation].Mon[OBJ->NIndex];
		OBJ->Nat=&NATIONS[CapNation];
		OBJ->NMask=OBJ->Nat->NMask;
		OBJ->Serial^=1;
		OBJ->Selected=0;
		OBJ->ImSelected=0;
		OBJ->Zombi=false;
		
		if(OBJ->NewBuilding&&OldNat==MyNation){
			LastAttackDelay=900;
			if(AlarmSoundID!=-1){
				AddEffect((mapx<<5)+500,(mapy<<4)+300,AlarmSoundID);
				LastActionX=OBJ->RealX;
				LastActionY=OBJ->RealY;
				AlarmDelay=60;
			};
		};
		OBJ->NNUM=CapNation;
		OBJ->Nat->CITY->RegisterNewUnit(OBJ);
		AddObject(OBJ);
		if(OBJ->NewBuilding)
			OBJ->Stage=(oldst*int(OBJ->Ref.General->MoreCharacter->ProduceStages))/oldmax;
		if(OBJ->NInside){
			if(!CaptState){
				word* Uni=OBJ->Inside;
				for(int j=0;j<OBJ->NInside;j++){
					word MID=Uni[j];
					if(MID!=0xFFFF){
						OneObject* OB=Group[MID];
						if(OB){
							OB->Nat->CITY->Account-=OB->newMons->Ves*5;
							CITY[CapNation].Account+=OB->newMons->Ves*5;
							if(OB->newMons->Usage==PeasantID)OB->ClearOrders();
							DelObject(OB);
							OB->Nat->CITY->UnRegisterNewUnit(OB);
							OB->Ref.General=NATIONS[CapNation].Mon[OB->NIndex];
							OB->Nat=&NATIONS[CapNation];
							OB->NMask=OB->Nat->NMask;
							OB->Serial^=1;
							OB->Selected=false;
							OB->ImSelected=false;
							OB->Zombi=false;
							byte OldNat=OB->NNUM;
							OB->NNUM=CapNation;
							OB->Nat->CITY->RegisterNewUnit(OB);
							AddObject(OB);
							if(OB->LocalOrder)OB->Ready=1;
							OB->ClearOrders();
						};
					};
				};
			}else DestructBuilding(OBJ);
		};
	};
};
int GetAmountOfProtectors(OneObject* OBJ){
	int cell=((OBJ->RealY>>11)<<VAL_SHFCX)+(OBJ->RealX>>11);
	int CELL0=cell;
	int rx1=5;
    int rx2=rx1+rx1+1;
    int stcell=cell-rx1-(rx1<<VAL_SHFCX);
	int N=0;
	byte NMASK=OBJ->NMask;
	byte* bpt=NPresence+stcell;
	for(int nx=0;nx<rx2;nx++){
		for(int ny=0;ny<rx2;ny++){
			if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
				if(bpt[0]&NMASK){
					N+=GetProtectors(stcell,NMASK);
				};
			};
			stcell++;
			bpt++;
		};
		stcell+=VAL_MAXCX-rx2;
		bpt+=VAL_MAXCX-rx2;
	};
	return N;
};
extern int HISPEED;
void PlayAnimation(NewAnimation* NA,int Frame,int x,int y){
	if(NA->SoundID==-1)return;
	int HF=NA->HotFrame;
	Frame/=FrmDec;
	HF/=FrmDec;
	if(Frame!=HF)return;
	AddEffect(x,y,NA->SoundID);

};
bool CheckPlaceForShar(int x,int y){
	if(CheckBar((x>>4)-8,(y>>4)-8,17,17))return false;
	int h=GetHeight(x,y);
	if(abs(GetHeight(x-60,y)-h)>10||abs(GetHeight(x+60,y)-h)>10||abs(GetHeight(x,y-60)-h)>10||abs(GetHeight(x,y+60)-h)>10)return false;
	//check monsters
	return CheckMonstersInArea(0xFF,x<<4,y<<4,150<<4)&&CheckSpritesInArea(x<<4,y<<4,150<<4);
};
void HandleShar(Nation* NT){
	if(NT->SharAllowed){
		addrand(NT->NNUM);
		if(NT->SharPlaceFound){
			addrand(NT->NNUM);
			if(NT->SharStage==0xFF){
				addrand(NT->NNUM);
				//processing and drawing <shar> in motion.
				//1.processing
				int x=NT->SharX>>4;
				int y=NT->SharY>>4;
				if(y>(msy<<5))y=msy<<5;
				int z=GetHeight(x,y);
				addrand(x);
				addrand(y);
				int dz=(NT->SharZ>>4)-z;
				addrand(dz);
				addrand(z);
				bool AllowClip=false;
				if(dz<100){
					NT->SharAx=0;
					NT->SharAy=0;
					NT->SharAz=64;
				}else{
					NT->Vision=true;
					NT->SharAz=(250-dz)>>5;
					if(NT->SharAz>64)NT->SharAz=64;
					if(NT->SharAz<-64)NT->SharAz=-64;
					//free motion;
					if(rando()<60){
						do{
							NT->SharAx=(rando()&64)-32;
						}while(abs(NT->SharAx)<16);
						do{
							NT->SharAy=(rando()&64)-32;
						}while(abs(NT->SharAy)<16);
					};
					AllowClip=true;
				};
				//Friction
				int fx=-NT->SharVx>>7;
				int fy=-NT->SharVy>>7;
				int fz=-NT->SharVz>>7;
				if(AllowClip){
					if(x<1200)fx+=64;
					if(y<1200)fy+=64;
					if(x>(msx<<5)-1200)fx-=64;
					if(y>(msy<<5)-1200)fy-=64;
				};
				NT->SharVx+=NT->SharAx+fx;
				NT->SharVy+=NT->SharAy+fy;
				NT->SharVz+=NT->SharAz+fz;
				NT->SharX+=NT->SharVx>>7;
				NT->SharY+=NT->SharVy>>7;
				NT->SharZ+=NT->SharVz>>7;
			}else{
				int frm=0;
				if(NT->SharStage>180)frm=(NT->SharStage-180)>>1;
				NT->SharStage++;
				int dx=mapx<<5;
				int dy=mapy<<4;
				int x=NT->SharX>>4;
				int y=NT->SharY>>4;
				int z=GetHeight(x,y);
				if(frm>=SharBuild->NFrames-FrmDec){
					BClrBar((NT->SharX>>8)-3,(NT->SharY>>8)-3,7);
					NT->SharStage=0xFF;
					NT->SharZ=z<<4;
					NT->SharVx=0;
					NT->SharVy=0;
					NT->SharVz=0;
					NT->SharAx=0;
					NT->SharAy=0;
					NT->SharAz=0;
				};
			};
		}else{
			int xx=(NT->SharX>>4)+((int(rando())*NT->SearchRadius)>>15);
			int yy=(NT->SharY>>4)+((int(rando())*NT->SearchRadius)>>15);
			if(CheckPlaceForShar(xx,yy)){
				NT->SharPlaceFound=true;
				NT->SharStage=0;
				NT->SharX=xx<<4;
				NT->SharY=yy<<4;
				BSetBar((NT->SharX>>8)-3,(NT->SharY>>8)-3,7);
				addrand(xx+yy);
			}else NT->SearchRadius+=10;
		};
	};
};
void DrawShar(Nation* NT){
	if(NT->SharAllowed){
		if(NT->SharPlaceFound){
			if(NT->SharStage==0xFF){
				//processing and drawing <shar> in motion.
				//1.processing
				int x=NT->SharX>>4;
				int y=NT->SharY>>4;
				if(y>(msy<<5))y=msy<<5;
				int z=GetHeight(x,y);
				int dz=(NT->SharZ>>4)-z;
				bool AllowClip=false;
				
				//2.drawing
				if(Shar){
					int dx=mapx<<5;
					int dy=mapy<<4;
					int x=NT->SharX>>4;
					int y=NT->SharY>>4;
					int z=NT->SharZ>>4;
					int vx=NT->SharVx>>5;
					int vy=NT->SharVy>>5;
					z+=int(sqrt(vx*vx+vy*vy));
					int z0=GetHeight(x,y);
					int dz=(z-z0)>>1;
					x-=dx;
					y=(y>>1)-dy-z;
					int maxxx=smaplx<<5;
					int maxyy=smaply<<4;
					CurDrawNation=NT->NNUM;
					if(x>-128&&y>-160&&x<maxxx+128&&y<maxyy+160){
						NewFrame* NF=&Shar->Frames[0];
						AddPoint(x,y+z,x+NF->dx,y+NF->dy,NULL,NF->FileID,NF->SpriteID,0,0);
					};
					//shadow drawing
					x=(NT->SharX>>4)-dz;
					y=(NT->SharY>>4)-(dz>>1);
					z=GetHeight(x,y);
					x-=dx;
					y=(y>>1)-dy-z;
					if(x>-128&&y>-160&&x<maxxx+128&&y<maxyy+160){
						NewFrame* NF=&Shar->Frames[1];
						CurDrawNation=NT->NNUM;
						AddPoint(x,y+z,x+NF->dx,y+NF->dy,NULL,NF->FileID,NF->SpriteID,0,0);
					};
				};
			}else{
				int frm=0;
				if(NT->SharStage>180)frm=(NT->SharStage-180)>>1;
				int dx=mapx<<5;
				int dy=mapy<<4;
				int x=NT->SharX>>4;
				int y=NT->SharY>>4;
				int z=GetHeight(x,y);
				x-=dx;
				y=(y>>1)-dy-z;
				int maxxx=smaplx<<5;
				int maxyy=smaply<<4;
				if(x>-128&&y>-160&&x<maxxx+128&&y<maxyy+160){
					NewFrame* NF=&SharBuild->Frames[frm];
					CurDrawNation=NT->NNUM;
					AddPoint(x,y,x+NF->dx,y+NF->dy,NULL,NF->FileID,NF->SpriteID,0,0);
				};
			};
		};
	};
};
BlockBars::BlockBars(){
	BC=NULL;
	NBars=0;
	MaxBars=0;
};
void BlockBars::Clear(){
	if(BC)free(BC);
	NBars=0;
	MaxBars=0;
	BC=NULL;
};
BlockBars::~BlockBars(){
	Clear();
};
bool BlockBars::Add(word x,word y){
	if(NBars){
		DWORD DT=x+(y<<16);
		DWORD* SDAT=(DWORD*)BC;
		for(int i=0;i<NBars;i++)if(SDAT[i]==DT)return false;
	};
	if(NBars>=MaxBars){
		MaxBars+=128;
		BC=(BlockCell*)realloc(BC,MaxBars*sizeof(BlockCell));
	};
	BC[NBars].x=x;
	BC[NBars].y=y;
	NBars++;
	return true;
};
bool BlockBars::FastAdd(word x,word y){
	if(NBars>=MaxBars){
		MaxBars+=128;
		BC=(BlockCell*)realloc(BC,MaxBars*sizeof(BlockCell));
	};
	BC[NBars].x=x;
	BC[NBars].y=y;
	NBars++;
	return true;
};
bool BlockBars::Delete(word x,word y){
	int i;
	if(NBars){
		DWORD DT=x+(y<<16);
		DWORD* SDAT=(DWORD*)BC;
		for(i=0;i<NBars&&SDAT[i]!=DT;i++);
		if(i>=NBars)return false;
		if(i<NBars-1){
			memcpy(SDAT+i,SDAT+i+1,(NBars-i-1)<<2);
		};
		NBars--;
		return false;
	};
	return false;
};
void MoveAwayInCell(int cell,int x,int y){
    cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	if(!NMon)return;
	int ofs1=cell<<SHFCELL;
	word MID;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB){
				int dist=Norma(OB->RealX-x,OB->RealY-y);
                if(dist<364*16){
					int dx=OB->RealX-x;
					int dy=OB->RealY-y;
					if(dx||dy){
						dx=(350*16*dx)/dist;
						dy=(350*16*dy)/dist;
						if(!CheckBar(((x+dx)>>8)-2,((y+dy)>>8)-2,4,4)){
							OB->NewMonsterSendTo(x+dx,y+dy,128,1);
						};
					};
				};
			};
		};
	};
};
void MoveAway(int x,int y){
	int cell=((y>>11)<<VAL_SHFCX)+(x>>11);
	int CELL0=cell;
	int rx1=3;
	bool Capture=false;
    int rx2=rx1+rx1+1;
    int stcell=cell-rx1-(rx1<<VAL_SHFCX);
    for(int nx=0;nx<rx2;nx++){
        for(int ny=0;ny<rx2;ny++){
            if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
				MoveAwayInCell(stcell,x,y);
            };
            stcell++;
        };
        stcell+=VAL_MAXCX-rx2;
    };
};
bool MoveAwaySectorInCell(int cell,int x,int y,int r0,int r1,char Dir,int ddir){
    cell+=VAL_MAXCX+1;
    int NMon=MCount[cell];
	if(!NMon)return false;
	int ofs1=cell<<SHFCELL;
	word MID;
	bool Pushed=false;
	for(int i=0;i<NMon;i++){
		MID=GetNMSL(ofs1+i);
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB){
				int dist=Norma(OB->RealX-x,OB->RealY-y);
                if(dist<r1&&dist>r0){
					int dx=OB->RealX-x;
					int dy=OB->RealY-y;
					if(dx||dy){
						char ndir=char(GetDir(dx,dy));
						if(abs(ndir-Dir)<ddir){
							if(OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec){
								int r=TryToMove(OB,OB->RealDir,false);
								if(r==-1)TryToMove(OB,OB->RealDir,true);
								/*
								dx=(int(TCos[OB->RealDir])<<3);
								dy=(int(TSin[OB->RealDir])<<3);
								
								if(!CheckBar(((x+dx)>>8)-2,((y+dy)>>8)-2,4,4)){
									OB->NewMonsterSendTo(OB->RealX+dx,OB->RealY+dy,128+64,1);
								}else{
									OB->RealDir+=32;
									OB->GraphDir=OB->RealDir;
								};
								*/
							};
							Pushed=true;
						};
					};
				};
			};
		};
	};
	return Pushed;
};
bool MoveAwayInSector(int x,int y,int r0,int r1,char Dir,int ddir){
	int cell=((y>>11)<<VAL_SHFCX)+(x>>11);
	int CELL0=cell;
	int rx1=2;
	bool Capture=false;
    int rx2=rx1+rx1+1;
    int stcell=cell-rx1-(rx1<<VAL_SHFCX);
	bool Pushed=false;
    for(int nx=0;nx<rx2;nx++){
        for(int ny=0;ny<rx2;ny++){
            if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
				if(MoveAwaySectorInCell(stcell,x,y,r0,r1,Dir,ddir))Pushed=true;
            };
            stcell++;
        };
        stcell+=VAL_MAXCX-rx2;
    };
	return Pushed;
};
#define SectRAD (450*16)
#define SectMIN (32*16)
void GetUnitsInSector(int cell,int x,int y,char IniDir,int* My,int* Enemy,byte Mask){
	cell+=VAL_MAXCX+1;
	if(cell>=0&&cell<VAL_MAXCX*VAL_MAXCX){
	    int NMon=MCount[cell];
		if(!NMon)return;
		int ofs1=cell<<SHFCELL;
		word MID;
		bool Pushed=false;
		for(int i=0;i<NMon;i++){
			MID=GetNMSL(ofs1+i);
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB){
					int dx=OB->RealX-x;
					int dy=OB->RealY-y;
					int dist=Norma(dx,dy);
		            if(dist<SectRAD&&dist>SectMIN){
						if(dx||dy){
							char ndir=char(GetDir(dx,dy));
							if(abs(ndir-IniDir)<28){
								if(OB->NMask&Mask)(*My)++;
								else (*Enemy)++;
							};
						};
					 };
				};
			};
		};
	};
};
bool TestUnitsInSectorForGrenade(OneObject* GRE){
	int x=GRE->RealX;
	int y=GRE->RealY;
	int cell=((y>>11)<<VAL_SHFCX)+(x>>11);
	int CELL0=cell;
	int rx1=4;
	bool Capture=false;
    int rx2=rx1+rx1+1;
    int stcell=cell-rx1-(rx1<<VAL_SHFCX);
	bool Pushed=false;
	int NMY=0;
	int NHIM=0;
    for(int nx=0;nx<rx2;nx++){
        for(int ny=0;ny<rx2;ny++){
            if(stcell>=0&&stcell<VAL_MAXCX*VAL_MAXCX){
				GetUnitsInSector(stcell,x,y,GRE->RealDir,&NMY,&NHIM,GRE->NMask);
            };
            stcell++;
        };
        stcell+=VAL_MAXCX-rx2;
    };
	return NMY<9&&(NMY+NMY+NMY<=NHIM);
};
int GetTopology1(int x,int y){
	int xc=x>>2;
	int yc=y>>2;
	word tr=SafeTopRef(xc,yc);
	if(tr<0xFFFE)return tr;
	else return 0xFFFF;
};
int GetWTopology1(int x,int y,byte LTP){
	int xc=x>>2;
	int yc=y>>2;
	if(xc<0||yc<0||xc>=TopLx||yc>=TopLy)return 0xFFFF;
	word tr=GetTopRef(xc+(yc<<TopSH),LTP);
	if(tr<0xFFFE)return tr;
	else return 0xFFFF;
};
bool TopFindBestPosition(OneObject* OB,int* xd,int* yd,int R0,int Top,byte LTP){
	MotionField* MFI=MFIELDS+OB->LockType;
	if(!OB)return false;
	if(!MFI->CheckBar((*xd)-1,(*yd)-1,OB->Lx+1,OB->Lx+1))return true;
	int Lx=OB->Lx;
	int R1=R0;
	int bx=*xd;
	int by=*yd;
	int bdist=100000;
	int xxx=bx-1;
	int yyy=by-1;
	int LL=2;
	int bx1,by1,dstn;
	int x=OB->x;
	int y=OB->y;
	while(R1){
		for(int i=0;i<=LL;i++)if(!MFI->CheckBar(xxx+i-1,yyy-1,Lx,Lx)){
			bx1=xxx+i;
			by1=yyy;
			dstn=Norma(bx1-x,by1-y);
			if(dstn<bdist&&GetWTopology1(bx1,by1,LTP)==Top){
				bx=bx1;
				by=by1;
				bdist=dstn;
			};
		};
		for(int i=0;i<=LL;i++)if(!MFI->CheckBar(xxx+i-1,yyy+LL-1,Lx,Lx)){
			bx1=xxx+i;
			by1=yyy+LL;
			dstn=Norma(bx1-x,by1-y);
			if(dstn<bdist&&GetWTopology1(bx1,by1,LTP)==Top){
				bx=bx1;
				by=by1;
				bdist=dstn;
			};
		};
		for(int i=0;i<LL-1;i++)if(!MFI->CheckBar(xxx-1,yyy+i,Lx,Lx)){
			bx1=xxx;
			by1=yyy+i+1;
			dstn=Norma(bx1-x,by1-y);
			if(dstn<bdist&&GetWTopology1(bx1,by1,LTP)==Top){
				bx=bx1;
				by=by1;
				bdist=dstn;
			};
		};
		for(int i=0;i<LL-1;i++)if(!MFI->CheckBar(xxx+LL-1,yyy+i,Lx,Lx)){
			bx1=xxx+LL;
			by1=yyy+i+1;
			dstn=Norma(bx1-x,by1-y);
			if(dstn<bdist&&GetWTopology1(bx1,by1,LTP)==Top){
				bx=bx1;
				by=by1;
				bdist=dstn;
			};
		};
		if(bdist<100000){
			*xd=bx;
			*yd=by;
			return true;
		};
		R1--;
		LL+=2;
		xxx--;
		yyy--;
	};
	return false;
};
bool CheckDirectWay(int x0,int y0,int x1,int y1){
	int bx=x0<<10;
	int by=y0<<10;
	int Lx=x1-x0;
	int Ly=y1-y0;
	int r=Norma(Lx,Ly);
	int N=(r>>6)+1;
	Lx=(Lx<<10)/N;
	Ly=(Ly<<10)/N;
	for(int i=0;i<N;i++){
		bx+=Lx;
		by+=Ly;
		if(CheckPt(bx>>14,by>>14))return false;
	};
	return true;
};
int FindSuperSmartBestPosition(OneObject* OB,int* cx,int* cy,int dx,int dy,word Top,byte LTP){
	MotionField* MF=MFIELDS;;
	//if(OB) MF=MFIELDS+OB->LockType;
	MF=MFIELDS+LTP;

	int i;
	int LDX=-2;
	int LLX=4;
	if(OB&&OB->LockType){
		LDX=-7;
		LLX=14;
	};
	int x0=(*cx)<<8;
	int y0=(*cy)<<8;
	int N=(Norma(dx,dy)>>5)+1;
	int DX=(dx<<8)/N;
	int DY=(dy<<8)/N;
	int mx=msx>>1;
	if(LTP!=1){
		for(i=0;i<N;i++){
			x0+=DX;
			y0+=DY;
			int tx=x0>>(8+6);
			int ty=y0>>(8+6);
			if(tx<=0||ty<=0||tx>=mx||ty>=mx)goto NextSm1;
			int ofs=tx+TopLx*ty;
			if(GetTopRef(ofs,LTP)>=0xFFFE)goto NextSm1;
		};
NextSm1:;
		bool Prolong=true;
		if(i!=N){
			x0-=DX;
			y0-=DY;
			Prolong=false;
		};
		N=i;
		if(!MF->CheckBar((x0>>(8+4))+LDX,(y0>>(8+4))+LDX,LLX,LLX)){
			*cx=x0>>8;
			*cy=y0>>8;
			//assert(!CheckBar((*cx>>4)-1,(*cy>>4)-1,3,3));
			return 1;
		};
		int xx=x0;
		int yy=y0;
		for(int i=0;i<N;i++){
			x0-=DX;
			y0-=DY;
			if(!MF->CheckBar((x0>>(8+4))+LDX,(y0>>(8+4))+LDX,LLX,LLX)){
				*cx=x0>>8;
				*cy=y0>>8;
				//assert(!CheckBar((*cx>>4)-1,(*cy>>4)-1,3,3));
				return 2;
			};
		};
		if(Prolong){
			x0=xx;
			y0=yy;
			for(i=0;i<5;i++){
				x0+=DX;
				y0+=DY;
				int tx=x0>>(8+6);
				int ty=y0>>(8+6);
				if(tx<=0||ty<=0||tx>=mx||ty>=mx)goto NextSm2;
				int ofs=tx+TopLx*ty;
				if(GetTopRef(ofs,LTP)>=0xFFFE)goto NextSm2;
				if(!MF->CheckBar((x0>>(8+4))+LDX,(y0>>(8+4))+LDX,LLX,LLX)){
					*cx=x0>>8;
					*cy=y0>>8;
					//assert(!CheckBar((*cx>>4)-1,(*cy>>4)-1,3,3));
					return 3;
				};
			};
		};
	};
NextSm2:;
	if(!OB) return false;
	int ccx=(*cx)>>4;
	int ccy=(*cy)>>4;
	if(TopFindBestPosition(OB,&ccx,&ccy,60,Top,LTP)){
		*cx=ccx<<4;
		*cy=ccy<<4;
		//assert(!CheckBar((*cx>>4)-1,(*cy>>4)-1,3,3));
		return 4;
	};	
	return false;
};
int GetWTopology(int x,int y,byte LTP){
	if(!GetNAreas(LTP))return 0xFFFF;
	int xc=x>>6;
	int yc=y>>6;
	if(xc<0||yc<0||xc>=TopLx||yc>=TopLy)return 0xFFFF;
	word tr=GetTopRef(xc+(yc<<TopSH),LTP);
	if(tr<0xFFFE)return tr;
	for(int i=0;i<10;i++){
		char* xi=Rarr[i].xi;
		char* yi=Rarr[i].yi;
		int N=Rarr[i].N;
		for(int j=0;j<N;j++){
			int xx=xc+xi[j];
			int yy=yc+yi[j];
			if(xx<0||yy<0||xx>=TopLx||yy>=TopLy)tr=0xFFFF;
			else tr=GetTopRef(xx+(yy<<TopSH),LTP);
			if(tr<0xFFFE)return tr;
		};
	};
	return 0xFFFF;
};
int GetWTopology(int* x,int* y,byte LTP){
	int xc=(*x)>>6;
	int yc=(*y)>>6;
	if(xc<0||yc<0||xc>=TopLx||yc>=TopLy)return 0xFFFF;
	word tr=GetTopRef(xc+(yc<<TopSH),LTP);
	if(tr<0xFFFE)return tr;
	for(int i=0;i<10;i++){
		char* xi=Rarr[i].xi;
		char* yi=Rarr[i].yi;
		int N=Rarr[i].N;
		for(int j=0;j<N;j++){
			int xx=xc+xi[j];
			int yy=yc+yi[j];
			if(xx<0||yy<0||xx>=TopLx||yy>=TopLy)tr=0xFFFF;
			else tr=GetTopRef(xx+(yy<<TopSH),LTP);
			if(tr<0xFFFE){
				*x=(xx<<6)+32;
				*y=(yy<<6)+32;
				return tr;
			};
		};
	};
	return 0xFFFF;
};
void NewMonsterSmartSendToLink(OneObject* OBJ){
#ifdef _USE3D
	//---definitions----
	void CreateWayNet(int x0,int y0,int Z1,int Z2,int *x,int *y,int& N,word UType,int DR);
	bool CheckIfRoadZone(int Zone);
	//------------------
	Order1* OR1=OBJ->LocalOrder;
	OBJ->PrioryLevel=OR1->PrioryLevel;
	int x=OR1->info.SmartSend.x;
	int y=OR1->info.SmartSend.y;
	int dx=OR1->info.SmartSend.dx;
	int dy=OR1->info.SmartSend.dy;
	int NextX=OR1->info.SmartSend.NextX;
	int NextY=OR1->info.SmartSend.NextY;
	int NextTop=OR1->info.SmartSend.NextTop;
	byte LTP=OBJ->LockType;
#ifdef COSSACKS2
	OBJ->UnitSpeed=64;
#endif
	int FinalTop=GetWTopology(&x,&y,LTP);

	if(FinalTop<0xFFFE){
		//searching for the next point
		int CTOP=GetWTopology(OBJ->RealX>>4,OBJ->RealY>>4,LTP);
		if(CTOP<0xFFFE){
			int NA=GetNAreas();
			//checking if there is road on the way to final point
			int NTP=CTOP;
			if(!CheckIfRoadZone(CTOP)){
				/*
				int NTP0;
				do{
					NTP0=NTP;
					NTP=GetMotionLinks(FinalTop+NA*NTP,LTP);
					if(NTP<0xFFFE&&NTP!=FinalTop&&CheckIfRoadZone(NTP)){
						Area* AR=GetTopMap(NTP,LTP);
						OBJ->NewMonsterSmartSendTo(AR->x<<6,AR->y<<6,0,0,OR1->PrioryLevel,1);
						return;
					};
				}while(NTP<0xFFFE&&NTP!=FinalTop&&NTP!=NTP0);
				*/
			};
			int NTOP=GetMotionLinks(FinalTop+NA*CTOP,LTP);
			if(NTOP<0xFFFE){
				if(CheckIfRoadZone(NTOP)&&CheckIfRoadZone(CTOP)){
					int xx[8];
					int yy[8];
					int NP=8;
						CreateWayNet(OBJ->RealX>>4,OBJ->RealY>>4,CTOP,NTOP,xx,yy,NP,OBJ->NIndex,0);
					for(int p=NP-1;p>=0;p--){
#ifdef COSSACKS2
						OBJ->UnitSpeed=24*4;
#endif //COSSACKS2
						if(dx>32)dx=32;
						if(dx<-32)dx=-32;
						if(dy>32)dy=32;
						if(dy<-32)dy=-32;
						OBJ->NewMonsterSendTo((xx[p]+dx)<<4,(yy[p]+dy)<<4,OR1->PrioryLevel,1);
					};
					return;
				};
			};
		};
	};
	if(NextTop==0xFFFF){
		NextTop=GetWTopology(OBJ->RealX>>4,OBJ->RealY>>4,LTP);
		if(NextTop==0xFFFF){
			OBJ->DeleteLastOrder();
			return;
		};
	}

	if(CheckTopDirectWay(OBJ->RealX>>4,OBJ->RealY>>4,x+dx,y+dy,OBJ->LockType)){
		int prior=OR1->PrioryLevel;
		OBJ->DeleteLastOrder();
		OBJ->NewMonsterSendTo((x+dx)<<4,(y+dy)<<4,prior,1);
		return;
	};
	if(NextTop==0xFFFF){
		NextTop=GetWTopology(OBJ->RealX>>4,OBJ->RealY>>4,LTP);
		if(NextTop==0xFFFF){
			OBJ->DeleteLastOrder();
			return;
		};
	}else{
		//if(Norma(x-(OBJ->RealX>>4),y-(OBJ->RealY>4))>64){
		//	OBJ->NewMonsterSendTo(NextX<<4,NextY<<4,OR1->PrioryLevel,1);
		//	return;
		//};
	};
	if(FinalTop>=0xFFFE){
		OBJ->DeleteLastOrder();
		return;
	};
	OR1->info.SmartSend.x=x;
	OR1->info.SmartSend.y=y;
	int NA=GetNAreas(LTP);
	word NextNextTop=GetMotionLinks(FinalTop+NA*NextTop,LTP);
	if(NextNextTop==FinalTop||FinalTop==NextTop){
		int prio=OR1->PrioryLevel;
		OBJ->DeleteLastOrder();
		if(!FindSuperSmartBestPosition(OBJ,&x,&y,dx,dy,FinalTop,LTP)){
			OBJ->DeleteLastOrder();
			return;
		};
		OBJ->NewMonsterSendTo(x<<4,y<<4,prio,1);
		return;
	};
	if(NextNextTop!=0xFFFF){
		//atttempt to optomise way
		int MaxPre=3;
		int cox=OBJ->RealX>>4;
		int coy=OBJ->RealY>>4;
		do{
			int Next2=GetMotionLinks(FinalTop+NA*NextNextTop,LTP);
			if(Next2!=0xFFFF){
				Area* AR2=GetTopMap(Next2);
				int nx=(int(AR2->x)<<6)+32;
				int ny=(int(AR2->y)<<6)+32;
				int rx=nx+dx;
				int ry=ny+dy;
				if(CheckTopDirectWay(cox,coy,rx,ry,OBJ->LockType)){
					NextNextTop=Next2;
					MaxPre--;
				}else MaxPre=0;
			}else MaxPre=0;
		}while(MaxPre);
		Area* NextAr=GetTopMap(NextNextTop,LTP);
		//------------------------
		int nx=(int(NextAr->x)<<6)+32;
		int ny=(int(NextAr->y)<<6)+32;
		int rx=nx+dx;
		int ry=ny+dy;
		//if(CheckBar((rx>>4)-2,(ry>>4)-2,4,4)){
			//search for better place
		int xx0=nx;
		int yy0=ny;
		if(!FindSuperSmartBestPosition(OBJ,&xx0,&yy0,dx,dy,NextNextTop,LTP)){
			OBJ->DeleteLastOrder();
			return;
		};
		rx=xx0;
		ry=yy0;
		//};
		OR1->info.SmartSend.NextX=rx;
		OR1->info.SmartSend.NextY=ry;
		OR1->info.SmartSend.NextTop=NextNextTop;
		OBJ->NewMonsterSendTo(rx<<4,ry<<4,OR1->PrioryLevel,1);
		return;
	}else{
		OBJ->DeleteLastOrder();
		return;
	};

#else //COSACKS2
	Order1* OR1=OBJ->LocalOrder;
	OBJ->PrioryLevel=OR1->PrioryLevel;
	int x=OR1->info.SmartSend.x;
	int y=OR1->info.SmartSend.y;
	int dx=OR1->info.SmartSend.dx;
	int dy=OR1->info.SmartSend.dy;
	int NextX=OR1->info.SmartSend.NextX;
	int NextY=OR1->info.SmartSend.NextY;
	int NextTop=OR1->info.SmartSend.NextTop;
	byte LTP=OBJ->LockType;

	if(CheckTopDirectWay(OBJ->RealX>>4,OBJ->RealY>>4,x+dx,y+dy,OBJ->LockType)){
		int prior=OR1->PrioryLevel;
		OBJ->DeleteLastOrder();
		OBJ->NewMonsterSendTo((x+dx)<<4,(y+dy)<<4,prior,1);
		return;
	};
	if(NextTop==0xFFFF){
		NextTop=GetWTopology(OBJ->RealX>>4,OBJ->RealY>>4,LTP);
		if(NextTop==0xFFFF){
			OBJ->DeleteLastOrder();
			return;
		};
	}else{
		//if(Norma(x-(OBJ->RealX>>4),y-(OBJ->RealY>4))>64){
		//	OBJ->NewMonsterSendTo(NextX<<4,NextY<<4,OR1->PrioryLevel,1);
		//	return;
		//};
	};
	int FinalTop=GetWTopology(&x,&y,LTP);
	if(FinalTop==0xFFFF){
		OBJ->DeleteLastOrder();
		return;
	};
	OR1->info.SmartSend.x=x;
	OR1->info.SmartSend.y=y;
	int NA=GetNAreas(LTP);
	word NextNextTop=GetMotionLinks(FinalTop+NA*NextTop,LTP);
	if(NextNextTop==FinalTop||FinalTop==NextTop){
		int prio=OR1->PrioryLevel;
		OBJ->DeleteLastOrder();
		if(!FindSuperSmartBestPosition(OBJ,&x,&y,dx,dy,FinalTop,LTP)){
			OBJ->DeleteLastOrder();
			return;
		};
		OBJ->NewMonsterSendTo(x<<4,y<<4,prio,1);
		return;
	};
	if(NextNextTop!=0xFFFF){
		//atttempt to optomise way
		int MaxPre=3;
		int cox=OBJ->RealX>>4;
		int coy=OBJ->RealY>>4;
		do{
			int Next2=GetMotionLinks(FinalTop+NA*NextNextTop,LTP);
			if(Next2!=0xFFFF){
				Area* AR2=GetTopMap(Next2);
				int nx=(int(AR2->x)<<6)+32;
				int ny=(int(AR2->y)<<6)+32;
				int rx=nx+dx;
				int ry=ny+dy;
				if(CheckTopDirectWay(cox,coy,rx,ry,OBJ->LockType)){
					NextNextTop=Next2;
					MaxPre--;
				}else MaxPre=0;
			}else MaxPre=0;
		}while(MaxPre);
		Area* NextAr=GetTopMap(NextNextTop,LTP);
		//------------------------
		int nx=(int(NextAr->x)<<6)+32;
		int ny=(int(NextAr->y)<<6)+32;
		int rx=nx+dx;
		int ry=ny+dy;
		//if(CheckBar((rx>>4)-2,(ry>>4)-2,4,4)){
			//search for better place
		int xx0=nx;
		int yy0=ny;
		if(!FindSuperSmartBestPosition(OBJ,&xx0,&yy0,dx,dy,NextNextTop,LTP)){
			OBJ->DeleteLastOrder();
			return;
		};
		rx=xx0;
		ry=yy0;
		//};
		OR1->info.SmartSend.NextX=rx;
		OR1->info.SmartSend.NextY=ry;
		OR1->info.SmartSend.NextTop=NextNextTop;
		OBJ->NewMonsterSendTo(rx<<4,ry<<4,OR1->PrioryLevel,1);
		return;
	}else{
		OBJ->DeleteLastOrder();
		return;
	};
#endif //COSSACKS2
};
#undef NewMonsterSmartSendTo
void FindPositionForUnit(OneObject* OB);
void OneObject::NewMonsterSmartSendTo(int x,int y,int dx,int dy,byte Prio,byte OrdType){
	if(!GetNAreas())return;
	if(NewBuilding||!Ready)return;
	int RR=Norma(dx,dy);
#ifdef HASH_TOP
	int start=GetTopRef((RealX>>10)+(RealY>>10)*TopLx,LockType);
	int fin=GetTopRef((x>>6)+(y>>6)*TopLx,LockType);
	int TD;
	if(start==fin) TD=0;
		else TD=GetMotionLinks(start*GetNAreas(LockType)+fin,LockType);//GetTopDistance(x>>6,y>>6,RealX>>10,RealY>>10);
#else
	int TD=GetTopDistance(x>>6,y>>6,RealX>>10,RealY>>10);
#endif //HASH_TOP

#ifndef COSSACKS2
	if(TD<35){
			if(!(Prio&127)){
			if(newMons->PMotionL[0].Enabled&&!newMons->MaxAIndex)GroundState=1;
		}else{
			GroundState=0;
			CheckState();
		};
		NewMonsterSendTo((x+dx)<<4,(y+dy)<<4,Prio,OrdType);
		return;
	};
#endif
	

	if(RR>128){
		int dx1=(dx*120)/RR;
		int dy1=(dy*90)/RR;
		NewMonsterSmartSendTo(x+dx-dx1,y+dy-dy1,dx1,dy1,Prio,OrdType);
		return;
	};
	/*
	if(BrigadeID!=0xFFFF){
		Brigade* BR=Nat->CITY->Brigs+BrigadeID;
		if(BR->WarType){
			BR->HumanGlobalSendTo(x,y,LastDirection,Prio,OrdType);
			return;
		};
	};
	*/
	if(x<64)x=64;
	if(y<64)y=64;
	if(Prio<PrioryLevel)return;
	word Top=GetWTopology(&x,&y,LockType);
	if(Top==0xFFFF)return;
	Order1* OR1=CreateOrder(OrdType&127);
	if(!OR1)return;

	if(!(newMons->LockType||Sdoxlo||UnlimitedMotion)){
		if(CheckPt(this->x,this->y)&&!OrdType&128){
			FindPositionForUnit(this);			
		};
	};
	OrdType&=127;


	OR1->DoLink=(ReportFn*)(SmartLink-0x7654391);//&NewMonsterSmartSendToLink;
	OR1->OrderType=12;
	OR1->PrioryLevel=Prio&127;
	OR1->info.SmartSend.x=x;
	OR1->info.SmartSend.y=y;
	OR1->info.SmartSend.dx=dx;
	OR1->info.SmartSend.dy=dy;
	OR1->info.SmartSend.NextX=0xFFFF;
	OR1->info.SmartSend.NextY=0xFFFF;
	OR1->info.SmartSend.NextTop=0xFFFF;
	if(OrdType==0||OrdType==1)PrioryLevel=Prio&127;
	if(OR1->PrioryLevel==0)InPatrol=true;
	if(!(Prio&127)){
		if(newMons->PMotionL[0].Enabled&&!newMons->MaxAIndex)GroundState=1;
	}else{
		GroundState=0;
		CheckState();
	};
};
#define NewMonsterSmartSendTo(x,y,dx,dy,Prio,OrdType) NewMonsterSmartSendTo(x,y,dx,dy,Prio,OrdType);addrand(55)
int CurTR=2;
int PreMID=0xFFFF;
#define DLR 3
int CheckPointForDamageAbility(OneObject* OBJ,int x,int y,int z);
void ProcessSelectedTower(){
	return;
	if(NSL[MyNation]==1){
		word MID=Selm[MyNation][0];
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&OB->newMons->Usage==TowerID){
				if(PreMID!=MID)CurTR=2;
				PreMID=MID;
				int N=Rarr[CurTR].N;
				char* xi=Rarr[CurTR].xi;
				char* yi=Rarr[CurTR].yi;
				int nn=0;
				int xc=OB->RealX>>10;
				int yc=OB->RealY>>10;
				int mpdx=mapx<<5;
				int mpdy=mapy<<4;
				int MLX=smaplx<<5;
				int MLY=smaply<<4;
				for(int p=0;p<N;p++){
					int xx=((xc+int(xi[p]))<<6)+32;
					int yy=((yc+int(yi[p]))<<6)+32;
					int zz=GetHeight(xx,yy);
					if(!CheckPointForDamageAbility(OB,xx,yy,zz+32)){
						nn++;
						int xs=xx-mpdx;
						int ys=(yy>>1)-mpdy-zz;
						if(xs>0&&ys>0&&xs<MLX&&ys<MLY){
							int ys1=((yy-DLR)>>1)-mpdy-GetHeight(xx-DLR,yy-DLR);
							int ys2=((yy-DLR)>>1)-mpdy-GetHeight(xx+DLR,yy-DLR);
							int ys3=((yy+DLR)>>1)-mpdy-GetHeight(xx-DLR,yy+DLR);
							int ys4=((yy+DLR)>>1)-mpdy-GetHeight(xx+DLR,yy+DLR);
							DrawLine(xs-DLR,ys1,xs+DLR,ys4,255);
							DrawLine(xs-DLR,ys3,xs+DLR,ys2,255);
						};
					};
				};
				if(nn||CurTR<12)CurTR++;else CurTR=2;
			};
		};
	};
};
//------------------------smart motion-----------------------
void CorrectTopPoint(int* cx,int* cy,int dx,int dy){
	if(!(dx||dy))return;
	int NR=Norma(dx,dy);
	int x0=(*cx)<<4;
	int y0=(*cy)<<4;
	int N=(NR>>6)+1;
	int sx=(dx<<4)/N;
	int sy=(dy<<4)/N;
	for(int i=0;i<N;i++){
		x0+=sx;
		y0+=sy;
		int tx=x0>>10;
		int ty=y0>>10;
		if(tx>0&&ty>0&&tx<TopLx&&ty<TopLy){
			int ofs=tx+(ty<<TopSH);
			if(GetTopRef(ofs)>=0xFFFE){
				x0-=sx;
				y0-=sy;
				*cx=x0>>4;
				*cy=y0>>4;
				return;
			};
		};
	};
};
void CorrectBlocking(int* cx,int* cy){
	if(!CheckBar(((*cx)>>4)-2,((*cy)>>4)-2,4,4))return;
	int top=GetTopology(*cx,*cy);
	if(top>=0xFFFE){
		//need to find free point with the same topology
		int tcx=(*cx)>>6;
		int tcy=(*cy)>>6;
		for(int r=1;r<10;r++){
			char* xi=Rarr[r].xi;
			char* yi=Rarr[r].yi;
			int N=Rarr[r].N;
			for(int j=0;j<N;j++){
				int tx=tcx+xi[j];
				int ty=tcy+yi[j];
				if(tx>0&&ty>0&&tx<TopLx&&ty<TopLy){
					int ofs=tx+(ty<<TopSH);
					if(GetTopRef(ofs)==top){
						if(!CheckBar((tx<<2),(ty<<2),4,4)){
							*cx=(tx<<6)+32;
							*cy=(ty<<6)+32;
						};
					};

				};
			};
		};
	};
};
bool CheckTopDirectWay(int x0,int y0,int x1,int y1,byte TopType){
	x0<<=8;
	y0<<=8;
	x1<<=8;
	y1<<=8;
	int dx=x1-x0;
	int dy=y1-y0;
	int N=(Norma(dx,dy)>>(8+6))+1;
	dx/=N;
	dy/=N;
	for(int i=0;i<N;i++){
		x0+=dx;
		y0+=dy;
		int tx=x0>>(8+6);
		int ty=y0>>(8+6);
		if(tx>0&&ty>0&&tx<TopLx&&ty<TopLy){
			if(GetTopRef(tx+(ty<<TopSH),TopType)>=0xFFFE)return false;
		}else return false;

	};
	return true;
};
void OneObject::FindNextSmartPoint(){
	bm_NextX=-1;
	bm_NextY=-1;
	int x=bm_DestX;
	int y=bm_DestY;
	int FinalTop=GetTopology(&x,&y);
	if(FinalTop>=0xFFFE)return;
	int MyX=RealX>>4;
	int MyY=RealY>>4;
	int MyTop=GetTopology(&MyX,&MyY);
	if(MyTop>=0xFFFE)return;
	if(MyTop==FinalTop){
		bm_NextX=bm_DestX;
		bm_NextY=bm_DestY;
		return;
	};
	int MMTP=MyTop*GetNAreas();
	int NextTop=GetMotionLinks(MMTP+FinalTop);
	if(NextTop>=0xFFFE)return;
	if(NextTop==FinalTop){
		int cx=bm_DestX;
		int cy=bm_DestY;
		CorrectTopPoint(&cx,&cy,bm_dx,bm_dy);
		CorrectBlocking(&cx,&cy);
		return;
	};
	Area* AR=GetTopMap(NextTop);
	int NxX=(int(AR->x)<<6)+32;
	int NxY=(int(AR->y)<<6)+32;
	CorrectTopPoint(&NxX,&NxY,bm_dx,bm_dy);
	bm_NextX=NxX;
	bm_NextY=NxY;
	bm_NextTop=NextTop;
	if(CheckTopDirectWay(MyX,MyY,NxX,NxY,LockType)){
		//attempt to optimise way
		int MaxPre=3;
		int NA=GetNAreas();
		while(MaxPre&&NextTop!=FinalTop){
			NextTop=GetMotionLinks(NextTop*NA+FinalTop);
			if(NextTop<0xFFFE){
				if(NextTop==FinalTop){
					NxX=bm_DestX;
					NxY=bm_DestY;
				}else{
					Area* AR=GetTopMap(NextTop);
					NxX=(int(AR->x)<<6)+32;
					NxY=(int(AR->y)<<6)+32;
				};
				CorrectTopPoint(&NxX,&NxY,bm_dx,bm_dy);
				if(CheckTopDirectWay(MyX,MyY,bm_dx,bm_dy,LockType)){
					bm_NextX=NxX;
					bm_NextY=NxY;
					bm_NextTop=NextTop;
				}else{
					int cx=bm_NextX;
					int cy=bm_NextY;
					CorrectBlocking(&cx,&cy);
					bm_NextX=cx;
					bm_NextY=cy;
					return;
				};
			};
		};
	};
	int cx=bm_NextX;
	int cy=bm_NextY;
	CorrectBlocking(&cx,&cy);
	bm_NextX=cx;
	bm_NextY=cy;
};
void OneObject::CreateSmartPath(int x,int y,int dx,int dy){
	if(x==bm_DestX&&y==bm_DestY&&dx==bm_DestY&&dy==bm_DestY){

	}else{

	};
};
int EnumUnitsInCell(int cell,int x,int y,int r,word Type,byte Nation){
	int N=0;
	if(cell>0){
		cell+=VAL_MAXCX+1;
		if(cell>=VAL_MAXCX*VAL_MAXCX)return 0;
		int NMon=MCount[cell];
		if(!NMon)return NULL;
		int ofs1=cell<<SHFCELL;
		word MID;
		for(int i=0;i<NMon;i++){
			MID=GetNMSL(ofs1+i);
			if(MID!=0xFFFF){
				OneObject* OB=Group[MID];
				if(OB&&(!(OB->Guard!=0xFFFF||OB->Sdoxlo))&&OB->BrigadeID==0xFFFF&&OB->NNUM==Nation&&OB->NIndex==Type&&Norma(OB->RealX-x,OB->RealY-y)<r)N++;
			};
		};
	};
    return N;
};
int EnumUnitsInRound(int x,int y,int r,word Type,byte Nation){
    int rx1=(r>>11)+1;
	int N=0;
	int x0=x>>11;
	int y0=y>>11;
	int maxx=msx>>2;
	for(int R=0;R<=rx1;R++){
		char* xi=Rarr[R].xi;
		char* yi=Rarr[R].yi;
		int NN=Rarr[R].N;
		for(int p=0;p<NN;p++){
			int xx=x0+xi[p];
			int yy=y0+yi[p];
			if(xx>=0&&yy>=0&&xx<maxx&&yy<maxx){
				int cell=xx+(yy<<VAL_SHFCX);
				N+=EnumUnitsInCell(cell,x,y,r,Type,Nation);
			};
		};
	};
	return N;
};
int CheckShipDirection(char Dir){
	word MinD=127;
	char BestD=0;
	if(abs(Dir-32)<MinD){
		BestD=32;
		MinD=abs(Dir-32);
	};
	if(abs(Dir+32)<MinD){
		BestD=-32;
		MinD=abs(Dir+32);
	};
	if(abs(Dir-96)<MinD){
		BestD=96;
		MinD=abs(Dir-86);
	};
	if(abs(Dir+96)<MinD){
		BestD=-96;
		MinD=abs(Dir+96);
	};
	char D=Dir-BestD;
	if(abs(D)<4)return 0;
	if(D>0)return -2;
	return 2;
};
void RotateShipAndDieLink(OneObject* OBJ){
	char DD=CheckShipDirection(OBJ->RealDir);
	if(DD==0){
		OBJ->ClearOrders();
		OBJ->MaxDelay=2000;
		OBJ->delay=2000;
		OBJ->Die();
		return;
	};
	OBJ->RealDir+=DD;
	OBJ->GraphDir=OBJ->RealDir;
};
void RotateShipAndDie(OneObject* OBJ){
	Order1* OR1=OBJ->CreateOrder(0);
	OR1->DoLink=&RotateShipAndDieLink;
	OR1->PrioryLevel=255;
	OBJ->PrioryLevel=255;
	OBJ->RealVx=0;
	OBJ->RealVy=0;
	OBJ->Speed=0;
	OBJ->DestX=-1;
	OBJ->DestY=-1;
	NewMonster* NM=OBJ->newMons;
	if(NM->Destruct.NWeap){
		FogRec* FR=&NM->Destruct;
		if(FR->NWeap){
			for(int j=0;j<FR->NWeap;j++){
				int xp=(OBJ->RealX>>4)+(rando()%180)-90;
				int yp=(OBJ->RealY>>4)+(rando()%180)-90;
				int nw=j;//(rando()*FR->NWeap)>>15;
				addrand(71);
				Create3DAnmObject(WPLIST[FR->Weap[nw]],xp,yp,GetHeight(xp,yp)+60,xp,yp,1000,NULL,0,0xFFFF);
			};
		};
	};
};
///DEBUG
char LASTFILE[128];
int LastLine;
byte NatRefTBL[8]={0,1,2,3,4,5,6,7};
void DoNormalTBL(){
	for(int i=0;i<8;i++)NatRefTBL[i]=i;
};
void ProcessGuard(){
	if((tmtmt&31)==5){
		for(int i=0;i<MAXOBJECT;i++){
			OneObject* OB=Group[i];
			if(OB&&(!OB->Sdoxlo)){
				word GUID=OB->Guard;
				if(GUID!=0xFFFF){
					OneObject* GUOB=Group[GUID];
					if(GUOB&&!GUOB->Sdoxlo){
						int R=Norma(OB->RealX-GUOB->RealX,OB->RealY-GUOB->RealY)>>4;
						if(GUOB->NewBuilding&&R>250){
							int vx=(GUOB->RealX-OB->RealX)>>4;
							int vy=(GUOB->RealY-OB->RealY)>>4;
							int NR=Norma(vx,vy);
							vx=(150*vx)/NR;
							vy=(150*vy)/NR;
							OB->NewMonsterSmartSendTo((GUOB->RealX>>4)-vx,(GUOB->RealY>>4)-vy,0,0,16+128,0);
							//OB->StandGround=1;
						};
						if((!GUOB->NewBuilding)&&R>150){
							int vx=(GUOB->RealX-OB->RealX)>>4;
							int vy=(GUOB->RealY-OB->RealY)>>4;
							int NR=Norma(vx,vy);
							vx=(64*vx)/NR;
							vy=(64*vy)/NR;
							OB->NewMonsterSmartSendTo((GUOB->RealX>>4)-vx,(GUOB->RealY>>4)-vy,0,0,16+128,0);
							//OB->StandGround=1;

						};
					}else{
						OB->Guard=0xFFFF;
					};
				};
			};
		};
	};
};
typedef void AnglActor(OneObject* OB,WeaponInSector* WS,byte Angle,int param);
bool CheckPtInSector(WeaponInSector* WS,byte Angle,int dx,int dy){
	int R=Norma(dx,dy);
	if(R<WS->RMin||R>WS->RMax)return false;
	char DD=GetDir(dx,dy)-Angle;
	int dang=abs(int(DD));
	if(dang>WS->Angle)return false;
	return true;
};
int GetPtDamageInSector(WeaponInSector* WS,byte Angle,int dx,int dy){
	int R=Norma(dx,dy);
	if(R<WS->RMin||R>WS->RMax)return false;
	char DD=GetDir(dx,dy)-Angle;
	int dang=abs(int(DD));
	if(dang>WS->Angle)return 0;
	int DAM=WS->MaxDamage-(R-WS->RMin)*(WS->MaxDamage-WS->MinDamage)/(WS->RMax-WS->RMin);
	return DAM*(100-dang*(100-WS->AnglFactor)/WS->Angle)/100;
};
void ActInSector(int x,int y,WeaponInSector* WS,byte Angle,AnglActor* ANGA,int param){
	int iy;
	int MAX=WS->RMax;
	if(MAX>1600)MAX=1600;
	int RM=(MAX+256)>>7;
	int RM2=RM+RM;
	int LRM=RM2+1;
	byte DamBuff[4096];
	//memset(DamBuff,0,(LRM*LRM));
	int X0=(x>>7)-RM;
	int Y0=(y>>7)-RM;
	int ofs=0;
	for(iy=0;iy<LRM;iy++){
		for(int ix=0;ix<LRM;ix++){
			DamBuff[ofs]=CheckPtInSector(WS,Angle,((ix+X0)<<7)-x,((iy+Y0)<<7)-y);
			ofs++;
		};
	};
	ofs=0;
	for(iy=0;iy<RM2;iy++){
		for(int ix=0;ix<RM2;ix++){
			if(DamBuff[ofs]||DamBuff[ofs+1]||DamBuff[ofs+LRM]||DamBuff[ofs+LRM+1]){
				int cx=X0+ix+1;
				int cy=Y0+iy+1;
				if(cx>=0&&cy>=0&&cx<VAL_MAXCX&&cy<VAL_MAXCX){
					int cell=cx+(cy<<VAL_SHFCX);
					int NMon=MCount[cell];
					if(NMon){
						int ofs1=cell<<SHFCELL;
						word MID;
						for(int i=0;i<NMon;i++){
							MID=GetNMSL(ofs1+i);
							if(MID!=0xFFFF){
								OneObject* OB=Group[MID];
								if(OB&&(!OB->Sdoxlo)){
									ANGA(OB,WS,Angle,param);
								};
							};
						};
					};
				};
			};
			ofs++;
		};
		ofs++;
	};
};
struct AngParam1{
	int NEnemy;
	int NFriends;
	byte NMask;
};
void CheckUnitsInAngle(OneObject* OB,WeaponInSector* WS,byte Angle,int param){
	AngParam1* v=(AngParam1*)param;
	if(OB->NMask&v->NMask)v->NFriends++;
	else v->NEnemy++;
};
bool CheckVictimInAngle(int x,int y,WeaponInSector* WS,byte Angle,byte Mask){
	AngParam1 APR;
	APR.NMask=Mask;
	APR.NEnemy=0;
	APR.NFriends=0;
	ActInSector(x,y,WS,Angle,&CheckUnitsInAngle,int(&APR));
	return APR.NEnemy>3&&APR.NEnemy>APR.NFriends*2;

};
struct AngParam2{
	OneObject* Sender;
	int AttType;
	int x,y;
};
void DamageInAngle(OneObject* OB,WeaponInSector* WS,byte Angle,int param){
	AngParam2* v=(AngParam2*)param;
	int Dam=GetPtDamageInSector(WS,Angle,(OB->RealX>>4)-v->x,(OB->RealY>>4)-v->y);
	if(Dam){
		OB->MakeDamage(Dam,Dam,v->Sender,v->AttType);
	};
};
void DamageUnitsInAngle(int x,int y,WeaponInSector* WS,byte Angle,OneObject* Sender,int AttType){
	AngParam2 APR;
	APR.Sender=Sender;
	APR.AttType=AttType;
	APR.x=x;
	APR.y=y;
	ActInSector(x,y,WS,Angle,&DamageInAngle,int(&APR));
};
NewMonster::~NewMonster(){
	//for(int i=0;i<NAttTypes;i++){
	//	if(DamWeap[i])delete(DamWeap[i]);
	//};
	if(CompxCraft){
		for(int i=0;i<MaxAStages;i++){
			if(CompxCraft->Stages[i].ExplCoor)free(CompxCraft->Stages[i].ExplCoor);
			for(int j=0;j<2;j++){
				if(CompxCraft->Stages[i].FireX[j])free(CompxCraft->Stages[i].FireX[j]);
				if(CompxCraft->Stages[i].FireY[j])free(CompxCraft->Stages[i].FireY[j]);
			};
		};
		delete(CompxCraft);
	};
	if(CompxUnit)delete(CompxUnit);
	if(ShotRes)free(ShotRes);
	if(LockX)free(LockX);
	if(LockY)free(LockY);
	if(CheckX)free(CheckX);
	if(CheckY)free(CheckY);
	if(BLockX)free(BLockX);
	if(BLockY)free(BLockY);
	if(Doors)free(Doors);
	if(PieceName)free(PieceName);

	if(BuildPtX)free(BuildPtX);
	if(BuildPtY)free(BuildPtY);
	if(ConcPtX)free(ConcPtX);
	if(ConcPtY)free(ConcPtY);
	if(CraftPtX)free(CraftPtX);
	if(CraftPtY)free(CraftPtY);
	if(DamPtX)free(DamPtX);
	if(DamPtY)free(DamPtY);
	if(BornPtX)free(BornPtX);
	if(BornPtY)free(BornPtY);
	if(ShotPtX)free(ShotPtX);
	if(ShotPtY)free(ShotPtY);
	if(ShotPtYG)free(ShotPtYG);
	if(ShotPtZ)free(ShotPtZ);
	if(ShotDelay)free(ShotDelay);
	if(ShotAttType)free(ShotAttType);
	if(ShotDir)free(ShotDir);
	if(ShotDiff)free(ShotDiff);
	if(ShotMinR)free(ShotMinR);
	if(ShotMaxR)free(ShotMaxR);
	for(int i=0;i<2;i++){
		if(FireX[i])free(FireX[i]);
		if(FireY[i])free(FireY[i]);
	};
	if(Bars3D)free(Bars3D);
	if(VesloX)free(VesloX);
	if(VesloY)free(VesloY);
	if(MultiWpX)free(MultiWpX);
	if(MultiWpY)free(MultiWpY);
	if(HideTriX)free(HideTriX);
	if(HideTriY)free(HideTriY);
	if(WavePoints)free(WavePoints);
	if(FLAGS)free(FLAGS);
	if(WSECT)free(WSECT);
	if(Fogging.Weap)free(Fogging.Weap);
	if(Destruct.Weap)free(Destruct.Weap);
	if(Fire.Weap)free(Fire.Weap);
	if(MD_File)free(MD_File);
	if(Name)free(Name);

#ifdef _USE3D
	delete Monster3D;
#endif // _USE3D
};
NewAnimation::~NewAnimation(){
	if(ActivePtX)free(ActivePtX);
	if(ActivePtY)free(ActivePtY);
	if(LineInfo)free(LineInfo);
	if(Frames)delete[](Frames);
	memset(this,0,sizeof*this);
};
//-----------------IMPLEMENTATION OF SINGLE STEP-BY-STEP MOTION-----------------//
int BSpeed=12;
void MotionHandlerForSingleStepObjects(OneObject* OB){
#ifdef COSSACKS2
	if(!OB->LocalOrder)OB->DestX=-1;
	if(OB->PathDelay>=FrmDec)OB->PathDelay-=FrmDec;
	else OB->PathDelay=0;
	NewMonster* NMN=OB->newMons;
	AdvCharacter* ADC=OB->Ref.General->MoreCharacter;
	if(NMN->WaterActive)SpotByUnit(OB->RealX,OB->RealY,35,OB->RealDir);
	bool CANNOWMOVE=OB->InMotion||OB->NewCurSprite>=OB->NewAnm->NFrames-FrmDec;
	if(!CANNOWMOVE){
		if(OB->NewAnm->MoveBreak){
			if(OB->DestX>0)CANNOWMOVE=1;
		}
		if(OB->NewAnm->CanBeBroken){
			if(OB->DestX>0||OB->NewState!=OB->LocalNewState)CANNOWMOVE=1;
		};
	};
	if(CANNOWMOVE){
		APPLY_TIRING(OB);
		if(OB->DestX>0){
			OB->StandTime=0;
			int R=NMN->MotionDist;
			int RInFrame=R;
#ifdef GETTIRED
			if(OB->BrigadeID!=0xFFFF&&CITY[NatRefTBL[OB->NNUM]].Brigs[OB->BrigadeID].IsTired){				
				int T=OB->GetTired;
				if(T<5000)R-=(R>>2);
			}
#endif
			if(OB->CurUnitSpeed!=OB->UnitSpeed){
				OB->CurUnitSpeed=OB->UnitSpeed;
				/*
				int Sp14=OB->UnitSpeed>>1;
				if(OB->CurUnitSpeed<Sp14)OB->CurUnitSpeed=Sp14;
				else OB->CurUnitSpeed+=(tmtmt&1)==0;
				*/
			};
			if(OB->CurUnitSpeed!=64){
				R=(R*int(OB->CurUnitSpeed))>>6;
			};
			//checking TEXTURE
			int TX=(OB->RealX+256)>>9;
			int TY=(OB->RealY+256)>>9;
			int ofs=TX+TY*VertInLine;
			if(ofs>=0&&ofs<MaxPointIndex){
				int T=TexMap[ofs];
				if(TexFlags[T]&(256*64))R=(R*BSpeed)>>4;
			};
			int st=OB->NewState;
			int AutoSpeedAnm=16;
			NewAnimation* GOANM=NULL;
			AutoSpeedAnm=NMN->Rate[0];
			if(st){
				int RAT=NMN->Rate[st-1];
				R=(R*RAT)>>4;
				int SPD=(int(OB->CurUnitSpeed)*RAT)>>4;
				if(AutoSpeedAnm>16){
					if(SPD>65){
						//R=(R*AutoSpeedAnm)>>4;
						RInFrame=(RInFrame*AutoSpeedAnm)>>4;
						GOANM=&NMN->PMotionL[0];
					}else{
						GOANM=&NMN->MotionL;
					};
				}else{
					RInFrame=(RInFrame*RAT)>>4;
				};
			}else{
				if(AutoSpeedAnm>16){
					if(OB->CurUnitSpeed>65){
						//R=(R*AutoSpeedAnm)>>4;
						RInFrame=(RInFrame*AutoSpeedAnm)>>4;
						GOANM=&NMN->PMotionL[0];
					}else{
						GOANM=&NMN->MotionL;
					};
				}
			};
			int RS=R<<SpeedSh;
			OB->InMotion=1;
			int dx=OB->DestX-OB->RealX;
			int dy=OB->DestY-OB->RealY;
			int Nrm=Norma(dx,dy);
			bool FixEnd=0;
			if(Nrm<RS){
				RS=Nrm;
				FixEnd=1;
			};
			if(Nrm){
				char BestDir=char(GetDir(dx,dy));
				char ddir=BestDir-OB->RealDir;
				if(abs(ddir)>32){
					//OB->CurUnitSpeed>>=1;
				};
				int mr=NMN->MinRotator;
				if(abs(int(ddir))<mr)OB->RealDir=BestDir;
				else{
					if(ddir<0)OB->RealDir-=mr;
					else OB->RealDir+=mr;

				};
				OB->GraphDir=OB->RealDir;
				word ENMID=0xFFFF;
				if(OB->LocalNewState==OB->NewState){
					if(GOANM)
						OB->NewAnm=GOANM;
					else{
						if(OB->NewState)OB->NewAnm=&NMN->PMotionL[OB->NewState-1];
						else OB->NewAnm=&NMN->MotionL;
					};
					if(!FixEnd){
						OB->RealX+=(RS*TCos[OB->RealDir])>>8;
						OB->RealY+=(RS*TSin[OB->RealDir])>>8;
					}else{
						OB->RealX=OB->DestX;
						OB->RealY=OB->DestY;
					};
					OB->TotalPath+=RS;
					UnitsField.BClrBar(OB->x,OB->y,OB->Lx);
					OB->x=(OB->RealX-((OB->Lx)<<7))>>8;
					OB->y=(OB->RealY-((OB->Lx)<<7))>>8;
					if(UnitsField.CheckBar(OB->x-1,OB->y-1,OB->Lx+2,OB->Lx+2)){
						ENMID=CheckMotionThroughEnemyAbility(OB,OB->RealX,OB->RealY);
					};
					UnitsField.BSetBar(OB->x,OB->y,OB->Lx);
					if(ENMID!=0xFFFF){
						OB->AttackObj(ENMID,128+15);
					};
				}else{
					TryToStand(OB,0);
					CANNOWMOVE=0;
				};
			}else{
				TryToStand(OB,0);
				CANNOWMOVE=0;
			}
			//BlockIncFrames=1;
			if(CANNOWMOVE){
				if(OB->NewAnm->NFrames)OB->NewCurSprite=(OB->TotalPath/RInFrame)%OB->NewAnm->NFrames;
				else OB->NewCurSprite=0;
#ifdef _USE3D
				void AddSmokeOnHorseStep(OneObject* OB);
				AddSmokeOnHorseStep(OB);
#endif//_USE3D				
			}else{
				OB->NewCurSprite+=FrmDec;
				if(OB->NewAnm->NFrames)OB->NewCurSprite=OB->NewCurSprite%OB->NewAnm->NFrames;
				else OB->NewCurSprite=0;
			}
		}else{
			if(!(OB->LocalOrder&&OB->LocalOrder->OrderType==12)){
				OB->InMotion=0;
				TryToStand(OB,1);
				OB->CurUnitSpeed=0;
			};
		};
	}else{
		OB->NewCurSprite+=FrmDec;
		if(OB->NewAnm->NFrames)OB->NewCurSprite=OB->NewCurSprite%OB->NewAnm->NFrames;
		else OB->NewCurSprite=0;
		OB->CurUnitSpeed=0;
	};
	if(OB->StandTime<2)OB->RZ=GetUnitHeight((OB->RealX-((OB->Lx)<<7))>>4,(OB->RealY-((OB->Lx)<<7))>>4);
#endif //COSSACKS2
};
int GetDamFall(int x,int x0,int Dam);
int GetDamage(word ShooterID,word VictimID,int AttType){
	if(ShooterID==0xFFFF||VictimID==0xFFFF)return 0;
	OneObject* Victim=Group[VictimID];
	if(!Victim)return 0;
	OneObject* Shooter=Group[ShooterID];
	if(!Shooter)return 0;
	int R=Norma(Shooter->RealX-Victim->RealX,Shooter->RealY-Victim->RealY)>>4;
	if(R>Shooter->Ref.General->MoreCharacter->AttackRadius2[AttType])return 0;
	int DAM0=Shooter->Ref.General->MoreCharacter->MaxDamage[AttType];
	DAM0=GetDamFall(R,Shooter->newMons->DamageDecr[AttType],DAM0);
	return Victim->MakeDamage(DAM0,DAM0,Shooter,AttType,0);
};