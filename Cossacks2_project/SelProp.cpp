/* Properties of the selected objects viewering and *
 * corresponding mouse handling                     */
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "mode.h"
#include "Nature.h"
#include "fonts.h"
#include "walls.h"
#include "Megapolis.h"
#include "GSound.h"
#include "IconTool.h"
#include "GP_Draw.h"
#include "ConstStr.h"
#include "GSINC.H"
#include "ActiveScenary.h"
#include <assert.h>
#include <math.h>
void EconomyInterface(IconSet* ISET);
void ClearEconomy();
extern int UNGROUP_ICON;
extern int DOGROUP_ICON;
extern int GroupIconPos;
bool CheckGroupPossibility(byte NI);
bool CheckUnGroupPossibility(byte NI);
IconSet  PrpPanel;
IconSet  AblPanel;
IconSet  UpgPanel;
IconSet  BrigPanel;
IconSet  GeneralPanel;
extern byte   WeaponFlags[32];
extern int MessageUX;
extern int MessageUY;
extern int SGenIcon;
extern int PanelY;
extern int IconLx;
void GetMonCost(byte NI,word NIndex,char* st);
void AssignHintLo(char* s,int time);
extern short WeaponIcn [32];
extern short ProtectIcn[32];
char* GetSprResourceName(OneObject* OB);
#define MaxO 500
void AssignHint1(char* s,int time);
bool SelSoundDobe;
bool SelSoundReady;
int SelSoundType;
int CreateRZone(int x,int y,int lx,int ly,HandlePro* HPro,HandlePro* RHPro,int Index,char* Hint);
int PrpX;
int PrpY;
int PrpNx;
int PrpNy;
int AblX;
int AblY;
int AblNx;
int AblNy;
static bool PropClean=false;
static bool AbltClean=false;
int IconLx;
int IconLy;
int AddIconLx;
int AddIconLy;
extern short FearIcons[NFEARSUBJ];
extern char* FearTxt[NFEARSUBJ];
extern int FILLFORM_ICON;
extern int ZasevIconX;
extern int ZasevIconY;
extern int ZasevIconID;
int FillF_Pos=12;
struct MonInf{
	GeneralObject* RF;
	int Life;
	int MaxLife;
	int Magic;
	int MaxMag;
	int N;
	int Pause;
	int MaxPause;
	word Last;
	word ID;
};
struct AblInf{
	Nation* NT;
	word OInd;
	word Kind;
	word UPIND;
	char HotKey;
	char IconIndex;
};
MonInf MList[MaxO];
AblInf AList[MaxO];
word ArmList[MaxO];
int GetProgress(word ID,int* MaxProgress);
int GetAmount(word ID);
int SubIcon;
static GeneralObject* LastGO;
static Nation* LastNT;
static word LastAmount;
static word LastID;
extern int curptr;
extern int curdx;
extern int curdy;

//uniq properties
bool GetCoord;
UniqMethood* UNIM;
void InitPrpBar(){
	PrpNx=4;
	PrpNy=2;
	PrpX=10;
	PrpY=(msy>>1)+miniy+16;
	AblX=PrpX;
	AblY=PrpY+PrpNy*(IconLy+2)+16;
	AblNx=PrpNx;
	AblNy=6;
	SubIcon=-1;
};
void ResFon(int x,int y,int Lx,int Ly){

};
void ScrCopy(int x,int y,int Lx,int Ly){
	if(!bActive)return;
	//if(DDebug){
//
//		return;
//	};
	int x1=(x&(65535-3));
	int lx=((x+Lx+4)&(65535-3))-x1;
	int StDy=RSCRSizeX-lx;
	int FONDy=SCRSizeX-lx;
	int DsOf=int(RealScreenPtr)+x1+y*RSCRSizeX;
	int SrOf=int(ScreenPtr)+x1+y*SCRSizeX;
	int lx1=lx>>2;
	__asm{
		push	esi
		push	edi
		cld
		mov		eax,Ly
		mov		esi,SrOf
		mov		edi,DsOf
		mov		ebx,StDy
jj1:	mov		ecx,lx1
		rep		movsd
		add		esi,FONDy
		add		edi,ebx
		dec		eax	
		jnz		jj1
	};
};
int NINF,NABL,NARMINF;
void GetUnitCost(byte NI,word NIndex,int* Cost,int x,int y);
extern int LastMx;
extern int LastMy;
void GetMonCost(byte NI,word NIndex,char* st){
	Nation* NT=&NATIONS[NI];
	GeneralObject* GO=NT->Mon[NIndex];
	char uu[128];
	bool AddSp=false;
	uu[0]=0;
	AdvCharacter* AC=GO->MoreCharacter;
	int Cost[8];
	GetUnitCost(NI,NIndex,Cost,LastMx,LastMy);
	strcat(st," -");
	for(int i=0;i<8;i++){
		if(AC->NeedRes[i]){
			//if(AddSp)strcat(st," -");
			sprintf(uu," %s: %d",RDS[i].Name,Cost[i]);
			strcat(st,uu);
			AddSp=true;
		};
	};
};
void GeneralObject::GetMonsterCostString(char* st){
	st[0]=0;
};
void Nation::GetUpgradeCostString(char* st,word UI){
	char uu[128];
	bool AddSp=false;
	for(int i=0;i<8;i++){
		if(UPGRADE[UI]->Cost[i]){
			if(AddSp)strcat(st," ");
			sprintf(uu,"%s:%d",RDS[i].Name,int(UPGRADE[UI]->Cost[i]));
			strcat(st,uu);
			AddSp=true;
		};
	};
};
word SELSET[32];
int NSelSet=0;
void HPSEL(int i){
	if(GetKeyState(VK_CONTROL)&0x8000){
		CmdChooseUnSelType(MyNation,i);
	}else{
		if(GetKeyState(VK_SHIFT)&0x8000){
			if(NSelSet<32){
				SELSET[NSelSet]=i;
				NSelSet++;
			};
		}else{
			CmdChooseSelType(MyNation,i);
		};
	};
};
void SelectUnits(GAMEOBJ* Units, bool Add);
void TEST(int i) {
    OneObject* OB1 = Group[i];
    //OB1->Selected=true;
    //OB1->ImSelected=true;
    //SelectUnits(OB1->Index, 1);
}
void HPSELBRIG(int i){
	if(GetKeyState(VK_CONTROL)&0x8000){
		CmdChooseUnSelBrig(MyNation,i);
	}else{
		if(GetKeyState(VK_SHIFT)&0x8000){
			if(NSelSet<32){
				SELSET[NSelSet]=i+32768;
				NSelSet++;
			};
		}else{
			CmdChooseSelBrig(MyNation,i);
		};
	};
};
bool IsItInSelSet(word ID){
	for(int i=0;i<NSelSet;i++)if(SELSET[i]==ID)return true;
	return false;
};
void FREEORD(int i){
	CmdErseBrigs(MyNation);
};
void MSTANDGR(int i){
	CmdMakeStandGround(MyNation);
};
void CSTANDGR(int i){
	CmdCancelStandGround(MyNation);
};
extern int Zone1X;
extern int Zone1Y;
extern int Zone1LX;
extern int Zone1LY;
extern int BrigPnX;
extern int BrigPnY;
extern int BrigNx;
extern int BrigNy;
extern int PreviewBrig;
bool FullBrigMode=false;
bool GeneralsMode = false;
void OLIHPRO(int i){
	FullBrigMode=!FullBrigMode;
};
void OLIHPRO1(int i) {
    GeneralsMode = !GeneralsMode;
};
void SELBRIG(int i){
	if(GetKeyState(VK_SHIFT)&0x8000){
		CmdSelBrig(MyNation,1,i);
	}else{
		CmdSelBrig(MyNation,0,i);
		//FullBrigMode=false;
	};
};
void SELMONSTER(int i) {
    if (GetKeyState(VK_SHIFT) & 0x8000) {
        CmdRememSelection(MyNation,i);
    }
    else {
        CmdRememSelection(MyNation,i);
        //FullBrigMode=false;
    };
};
void CmdSetSrVictim(byte NI,byte val);
void PreBrig(int i){
	PreviewBrig=i;
};
void EATTACK_Pro(int i){
	CmdSetSrVictim(MyNation,0);
};
void DATTACK_Pro(int i){
	CmdSetSrVictim(MyNation,1);
};

void CmdSetShotMyUnitsState(byte NI,byte val);
void CmdSetArmAttackState(byte NI,byte val);
void CmdSetStopBuildState(byte NI,byte val);
void CmdKartech(byte NI);
void CmdPreKartech(byte NI);
void CmdFishing(byte NI,byte val);
void CmdAutozasev(byte NI,byte val);
void SHOTMYUNITS_Pro(int i){
	CmdSetShotMyUnitsState(MyNation,1);
};
void KEEPMYUNITS_Pro(int i){
	CmdSetShotMyUnitsState(MyNation,0);
};
void ARMATTACK_Pro(int i){
	CmdSetArmAttackState(MyNation,1);
};
#ifdef SIMPLEMANAGE
void SETATTSTATE_Pro(int i){
	CmdSetArmAttackState(MyNation,i);
};
#endif //SIMPLEMANAGE
void ANYATTACK_Pro(int i){
	CmdSetArmAttackState(MyNation,0);
};
void STOPBUILD_Pro(int i){
	CmdSetStopBuildState(MyNation,i);
};
void KARTECH_Pro(int i){
	CmdKartech(MyNation);
};
void PREKARTECH_Pro(int i){
	CmdPreKartech(MyNation);
};
void FISHING_Pro(int i){
	CmdFishing(MyNation,i);
};
void ZASEV_Pro(int i){
	CmdAutozasev(MyNation,i);
};
void CmdShotLine(byte NI,int i);
void ENBLINE(int i){
	CmdShotLine(MyNation,i);
};
void CmdThrowGrenade(byte,int);
void ThrowGrenade(int i){
	CmdThrowGrenade(MyNation,i);

};
extern int PATROL_ICON;
extern bool PATROLMODE;
void DOPATROL(int i){
	PATROLMODE=1;
};
void PATROL_Pro(int i){
};
void GOATTA_Pro(int i){
};
void CmdMakeReformation(byte,word,byte);
void REFORMA(int i){
	CmdMakeReformation(MyNation,i&65535,i>>16);
};
void CmdFillFormation(byte NI,word BrigadeID);
void FILLFORM(int i){
	CmdFillFormation(MyNation,i);
};
bool GoAndAttackMode=0;
void GONATTACK(int i){
	GoAndAttackMode=1;
};
extern City CITY[8];
extern int GOAT_Icon;
extern int GOAT_PX;
extern int GOAT_PY;
extern int HISTORY_Icon;
void ShowHistory();
void HISTORY_Call(int i){
	ShowHistory();
};
extern bool GoAndAttackMode;
extern char** NatNames;
char* GetTextByID(char* ID);
extern int STOP_ICON;
bool BreefUInfo=0;
extern int ATTGR_ICON;
extern int AttGrPos;
void ATTGR_PRO(int p);
void CmdDoGroup(byte);
void CmdUnGroup(byte);
void COM_DOGROUP(int i){
	CmdDoGroup(MyNation);
};
void COM_UNGROUP(int i){
	CmdUnGroup(MyNation);
};
int GetPanicProbability(OneObject* OB,int FearID);
char* FEAR_TXT[6]={NULL,NULL,NULL,NULL,NULL,NULL};
char* STOPBUILD_OFF_TXT=NULL;
char* STOPBUILD_ON_TXT=NULL;
extern int STOPBUILD_OFF;
extern int STOPBUILD_ON;
extern int STOPBUILD_X;
extern int STOPBUILD_Y;
extern int KARTECH_ICON;
extern int KARTECH_X;
extern int KARTECH_Y;
extern int PREKARTECH_ICON;
extern int PREKARTECH_X;
extern int PREKARTECH_Y;
extern int FISHING_ON_ICON;
extern int FISHING_OFF_ICON;
extern int FISHING_X;
extern int FISHING_Y;

char* KARTECH_TXT=NULL;
char* PREKARTECH_TXT=NULL;
char* FISHING_ON_TXT=NULL;
char* FISHING_OFF_TXT=NULL;
char* DONTMAKEZASEV=NULL;
char* MAKEZASEV=NULL;
char* NOARMATTACK=NULL;
char* NORIFLEATTACK=NULL;
char* RIFLEATTACK=NULL;
#define IC_DISTX 55
#define IC_ADISTX 55
#define IC_DISTY 19
byte FCLR[8]={39,39,126,126,194,194,208,208};
int FEARMAX=30;
int GetFScaled(int x){
 	int v=50+atan(float(FEARMAX)*log(float(x)/30.0)/60.0)*31.83;
	if(v<0)v=0;
	if(v>100)v=100;
	return v;
};
extern int HintTime;
extern char HintStrLo[256];
extern bool WasLoHint;
#ifdef GETTIRED
void CBar(int x,int y,int Lx,int Ly,byte c);
void ShowTiringInfo(Brigade* BR,int x,int y,int LX,int LY);
void ShowTiringInfo(OneObject* OB,int x,int y,int LX,int LY){
	if(OB->BrigadeID!=0xFFFF){
		ShowTiringInfo(CITY[OB->NNUM].Brigs+OB->BrigadeID,x,y,LX,LY);
		return;
	};
	x+=20;
	y+=39;
	Xbar(x-1,y-1,LX+2,LY+2,clrYello);
	LX=(LX*(OB->GetTired/100))/1000;
#ifndef EW
	CBar(x,y,LX,LY,clrGreen);
#else
    CBar(x,y,LX,LY,clrYello);
#endif
};
void ShowTiringInfo(Brigade* BR,int x,int y,int LX,int LY){
	x+=20;
	y+=39;
	word* IDS=BR->Memb;
	word* SNS=BR->MembSN;
	int N=BR->NMemb;
	int T=0;
	int NT=0;
	//int MINT=100*10000;
	for(int i=3;i<N;i++){
		word MID=IDS[i];
		if(MID!=0xFFFF){
			OneObject* OB=Group[MID];
			if(OB&&OB->Serial==SNS[i]){
				T+=OB->GetTired/1000;
				NT++;
				//if(MINT>OB->GetTired)MINT=OB->GetTired;
			};
		};
	};
	if(NT){
		T/=NT;
		Xbar(x-1,y-1,LX+2,LY+2,clrYello);
		LX=(LX*T)/100;
		CBar(x,y,LX,LY,clrGreen);
	};
};
#else //GETTIRED
#define ShowTiringInfo(OB,x,y,LX,LY)
#endif //GETTIRED
bool AllowArt=0;
bool RejectInterface=0;
typedef bool SubstInterfFunc(word IDX);
SubstInterfFunc* CheckInterface=NULL;
void ShowProp(){
	RejectInterface=0;
	//return;
	RunPF(6,"ShowProp");
	byte c;
	NINF=0;
	NARMINF=0;
	InitZones();
    PrpPanel.ClearIconSet();
    AblPanel.ClearIconSet();
	UpgPanel.ClearIconSet();
	BrigPanel.ClearIconSet();
    GeneralPanel.ClearIconSet();
	word MID;
	OneObject* OBJ;
	GeneralObject* GO;
	MonInf* MI;
	int i,j,k;
	//ResFon(PrpX,PrpY,PrpNx*IconLx,PrpNy*IconLy);
	word Nsel=ImNSL[MyNation];
	word* SMon=ImSelm[MyNation];
	if(!STOPBUILD_OFF_TXT){
		STOPBUILD_OFF_TXT=GetTextByID("STOPBUILD_OFF");
		STOPBUILD_ON_TXT=GetTextByID("STOPBUILD_ON");
		KARTECH_TXT=GetTextByID("KARTECH_TXT");
		PREKARTECH_TXT=GetTextByID("PREKARTECH_TXT");
		FISHING_ON_TXT=GetTextByID("FISHING_ON_TXT");
		FISHING_OFF_TXT=GetTextByID("FISHING_OFF_TXT");
		DONTMAKEZASEV=GetTextByID("DONTMAKEZASEV");
		MAKEZASEV=GetTextByID("MAKEZASEV");
		NOARMATTACK=GetTextByID("NOARMATTACK");
		NORIFLEATTACK=GetTextByID("NORIFLEATTACK");
		RIFLEATTACK=GetTextByID("RIFLEATTACK");
	};
	if(Nsel){
		if(SelSoundReady){
			if(SMon[0]!=0xFFFF){
				OneObject* OB=Group[SMon[0]];
				if(OB){
					if(SelSoundType)AddOrderEffect(OB->x>>1,OB->y>>1,OB->Ref.General->OrderSound);
					else AddOrderEffect(OB->x>>1,OB->y>>1,OB->Ref.General->ClickSound);
					SelSoundReady=false;
				};
			};
		};
		bool StandGround=false;
		bool NotStandGround=false;						
		bool UnitsPres=false;
		bool SrVictim=false;
		bool NoSrVictim=false;
		bool PrTowers=0;
		bool PrBuildings=0;
		bool SupportsFriendlyFire=0;
		bool CanKillFriend=0;
		bool CantKillFriend=0;
		bool SupportsArmAttack=0;
		bool ArmAttack=0;
		bool AnyAttack=0;
		bool StopBuild=0;
		bool SupportsStopBuild=0;
		bool SupportsKartech=0;
		bool SupportsFishing=0;
		bool FishingOn=0;
		bool CanMakeZasev=0;
		bool AutoZasevActive=0;
#ifdef SIMPLEMANAGE
		bool RifleAttack=0;
#endif //SIMPLEMANAGE
		word CurBrig=0xFFFE;
		//RunPF(22);
		int ST=Nsel/100;
		if(!ST)ST=1;
		for(i=0;i<Nsel;i++){
			MID=SMon[i];
			if(MID!=0xFFFF){
				OBJ=Group[MID];
				if(int(OBJ)&&!OBJ->Sdoxlo){
					NewMonster* NM=OBJ->newMons;
					if(i==0){
						if(OBJ->Ready&&NM->Usage==MelnicaID){
							CanMakeZasev=1;
							AutoZasevActive=!OBJ->AutoZasev;
						};
						if(NM->CanStopBuild){
							StopBuild=OBJ->StopBuildMode;
							SupportsStopBuild=1;
						};
						if(NM->FishSpeed){
							SupportsFishing=1;
							FishingOn=!OBJ->StopBuildMode;
						};
					};
					if(NM->FriendlyFire){
						SupportsFriendlyFire=1;
						if(OBJ->FriendlyFire)CanKillFriend=1;
						else CantKillFriend=1;
					};
					if(NM->ArmAttack){
						SupportsArmAttack=1;
						if(OBJ->ArmAttack)ArmAttack=1;
						else AnyAttack=1;
#ifdef SIMPLEMANAGE
						if(OBJ->RifleAttack)RifleAttack=1;
#endif //SIMPLEMANAGE
					};
					if(NM->Usage==TowerID)PrTowers=1;
					else if(OBJ->NewBuilding)PrBuildings=1;
					if(CurBrig==0xFFFE){
						CurBrig=OBJ->BrigadeID;
					}else{
						if(CurBrig!=OBJ->BrigadeID)CurBrig=0xFFFF;
					};
					if(OBJ->newMons->CanStandGr)UnitsPres=true;
					if(OBJ->StandGround){
						StandGround=true;
					}else{
						NotStandGround=true;
					};
					if(OBJ->newMons->WSECT)SupportsKartech=1;
					if(OBJ->NoSearchVictim) NoSrVictim=true;else SrVictim=true;
					GO=OBJ->Ref.General;
					int MyID=OBJ->NIndex;
					if(OBJ->InArmy&&NARMINF<MaxO){
						word id=OBJ->BrigadeID;
						for(k=0;k<NARMINF&&ArmList[k]!=id;k++);
						if(k>=NARMINF){
							ArmList[NARMINF]=id;
							NARMINF++;
						};
					}else{
						for( j=0;j<NINF;j++){
								if(GO==MList[j].RF)break;
						};
						MI=&(MList[j]);
						if(j>=NINF){
							for(int q=0;q<NINF;q++){
								if(MList[q].ID>MyID){
									for(int p=NINF;p>q;p--)
										MList[p]=MList[p-1];
									j=q;
									MI=MList+q;
									q=NINF;
								};
							};
							MI->Life=0;
							MI->MaxLife=0;
							MI->MaxMag=0;
							MI->N=0;
							NINF++;
							MI->Last=0;
							MI->ID=0;
							MI->MaxPause=0;
							MI->Pause=0;
						};
						MI->RF=GO;
						MI->Life+=OBJ->Life;
						MI->MaxLife+=OBJ->MaxLife;
						MI->MaxPause+=OBJ->MaxDelay;
						MI->Pause+=OBJ->delay;
						//MI->Magic+=OBJ->Magic;
						MI->N++;
							if(MI->N==1&&!OBJ->Ready)MI->Last=MID;
						if(OBJ->Ready)MI->Last=MID;
						MI->ID=OBJ->NIndex;
					};
				};
			};
		};
//		StopPF(22);
		//RunPF(23);
		int PN=PrpNx*PrpNy;
		if(NINF>PN)NINF=PN;
		if(NINF==1&&CheckInterface){
			if(RejectInterface=CheckInterface(MList[0].Last))return;
		};
		int	xx=0;
		int x1=PrpX;
		int y1=PrpY;
		//OneSlide* OC;
		//char str[32];
		for( i=0;i<NINF;i++){
			MI=&(MList[i]);
			OBJ=Group[MI->Last];
			NewMonster* NM=OBJ->newMons;
			char sxt[512];
			GetChar(MI->RF,sxt);
	        OneIcon* OI=PrpPanel.AddIcon(NM->IconFileID,NM->IconID,0xD0+(NatRefTBL[MyNation]<<2),sxt);
		    OI->AssignIntVal(MI->N);
			OI->AssignHealth(MI->Life,MI->MaxLife);
			if((!OBJ->Ready)&&OBJ->NUstages)
            OI->AssignStage(OBJ->Ustage,OBJ->NUstages);
			OI->AssignLeft(&HPSEL,OBJ->NIndex);
			if(NINF==1&&i==0&&!NARMINF){
				if(BreefUInfo){
					AssignHint(OBJ->Ref.General->Message,5);
				}else{
					char ccc[130];
					sprintf_s(ccc,sizeof(ccc),"%s (%s)",OBJ->Ref.General->Message,NatNames[OBJ->Ref.General->NatID]);
					AssignHint(ccc,5);
				};
			}else{
				if(NSelSet){
					if(GetKeyState(VK_SHIFT)&0x8000){
						if(IsItInSelSet(OBJ->NIndex)){
							OI->SelectIcon();
						};
					}else{
						if(!IsItInSelSet(OBJ->NIndex)){
							CmdChooseUnSelType(MyNation,OBJ->NIndex);
						};
					};
				};
			};
		};
		for( i=0;i<NARMINF;i++){
			int bid=ArmList[i];
			if(bid!=0xFFFF){
				City* CT=NATIONS[NatRefTBL[MyNation]].CITY;
				Brigade* BR=CT->Brigs+bid;
				GeneralObject* GO=NATIONS[MyNation].Mon[BR->MembID];
				NewMonster* NM=GO->newMons;
				OneIcon* OI=PrpPanel.AddIcon(NM->IconFileID,NM->IconID,0xD0+(NatRefTBL[MyNation]<<2),ARMYSTR);//"Army"
				int N=0;
				int Life=0;
				int MaxLife=0;
				int NU=BR->NMemb;
				for(int j=NBPERSONAL;j<NU;j++){
					word MID=BR->Memb[j];
					if(MID!=0xFFFF){
						OneObject* OB=Group[MID];
						if(OB&&OB->Serial==BR->MembSN[j]){
							N++;
							Life+=OB->Life;
							MaxLife+=OB->MaxLife;
						};
					};
				};
				if(N){
					OI->AssignIntVal(N);
				};
				if(MaxLife){
					OI->AssignHealth(Life,MaxLife);
				};
				OI->AssignLeft(&HPSELBRIG,bid);
				OI->AssignIntParam(bid+1);
				if(NSelSet){
					if(GetKeyState(VK_SHIFT)&0x8000){
						if(IsItInSelSet(bid+32768)){
							OI->SelectIcon();
						};
					}else{
						if(!IsItInSelSet(bid+32768))CmdChooseUnSelBrig(MyNation,bid);
					};
				};
			};
		};
		if(NARMINF==1&&!NINF){
			int bid=ArmList[0];
			if(bid!=0xFFFF){
				City* CT=NATIONS[NatRefTBL[MyNation]].CITY;
				Brigade* BR=CT->Brigs+bid;

                char ccc[130];
                sprintf_s(ccc,sizeof(ccc), "Formation: %s (%s)", OBJ->Ref.General->Message, NatNames[OBJ->Ref.General->NatID]);
                AssignHint(ccc, 5);
//#ifdef COSSACKS2
#ifdef NEWMORALE
				if(BR->NKills&&!(HintTime&&WasLoHint)){
					char str[128];
					sprintf(str,KILLUNI,BR->NKills);//"Lñøû:%d"
					ShowString(Zone1X+10,PanelY+Zone1Y-20,str,&SmallWhiteFont);
				};
#endif
//#endif //COSSACKS2
				GeneralObject* GO=NATIONS[NatRefTBL[MyNation]].Mon[BR->MembID];
				NewMonster* NM=GO->newMons;
				//Xbar(Zone1X,PanelY+Zone1Y+1,Zone1LX,Zone1LY,0xD0+(MyNation<<2));
				//Xbar(Zone1X-1,PanelY+Zone1Y,Zone1LX+2,Zone1LY+2,0xD0+(MyNation<<2)+1);
				MonInf* MI=&(MList[0]);
				char str[128];
				int DYY=-7;
				int x1=Zone1X+IconLx+4+6-4;
				int y1=PanelY+Zone1Y+6+DYY;
				AdvCharacter* ADC=GO->MoreCharacter;
				int NN=0;
				for(int i=0;i<NAttTypes;i++){
					if(ADC->MaxDamage[i]&&NN<2){
						int wk=ADC->WeaponKind[i];
						GPS.ShowGP(x1+NN*IC_ADISTX+1,y1+1,1,WeaponIcn[wk],0);
						int dm=ADC->MaxDamage[i];
#ifdef COSSACKS2
						int ADM=0;
						if(GO->newMons->SkillDamageMask&(1<<i)){
							int SC=GO->newMons->SkillDamageFormationBonusStep;
							int V;
							if(SC)V=(BR->NKills/SC)*SC;
							else V=BR->NKills;
							ADM=V*int(GO->newMons->SkillDamageFormationBonus)/100;
						};
						if(WeaponFlags[wk]&4)ADM+=BR->AddDamage;
						if(ADM){
							sprintf(str,"%d+%d",dm,ADM);
						}else{
							sprintf(str,"%d",dm);
						};
						ShowString(x1+NN*IC_ADISTX+IC_DISTY,y1+5,str,&SmallWhiteFont);
#else //COSSACKS2
						if((WeaponFlags[wk]&4)&&BR->AddDamage){
							if(BR->AddDamage>=0)sprintf(str,"%d+%d",dm,BR->AddDamage);
							else sprintf(str,"%d-%d",dm,-BR->AddDamage);
							ShowString(x1+NN*IC_ADISTX+IC_DISTY,y1+5,str,&SmallWhiteFont);
						}else{
							sprintf(str,"%d",dm);
							ShowString(x1+NN*IC_ADISTX+IC_DISTY,y1+5,str,&SmallWhiteFont);
						};
#endif //COSSACKS2
						NN++;
					};
				};
				y1+=21;
				GPS.ShowGP(x1+1,y1+1,1,SGenIcon,0);
				if(BR->AddShield){
					if(BR->AddShield>=0)sprintf(str,"%d+%d",ADC->Shield,BR->AddShield);
					else sprintf(str,"%d-%d",ADC->Shield,-BR->AddShield);
				}else{
					sprintf(str,"%d",ADC->Shield);
				};
				ShowString(x1+21,y1+12+DYY,str,&SmallWhiteFont);
				y1=PanelY+Zone1Y+IconLx+10+9+DYY-2;
				x1=Zone1X+4;
				NN=0;
				for( i=0;i<16;i++){
					if(ADC->Protection[i]&&NN<6){
						int ix=x1+(NN>>1)*IC_DISTX;
						int iy=y1+(NN&1)*IC_DISTY;
						GPS.ShowGP(ix+3,iy+1,1,ProtectIcn[i],0);
						int dm=ADC->Protection[i];
						sprintf(str,"%d",dm);
						ShowString(ix+22,iy+5,str,&SmallWhiteFont);
						NN++;
					};
				};
				ShowTiringInfo(OBJ,x1,y1+36-8,120,4);
#ifdef NEWMORALE
				void DrawNewMoraleInterface(int x,int y,OneObject* OB);
				DrawNewMoraleInterface(x1,y1+36-8,OBJ);
#else //NEWMORALE
#ifdef COSSACKS2
				int FFY0=y1+36;
				if(!FEAR_TXT[0]){
					FEAR_TXT[0]=GetTextByID("FEAR_TXT0");
					FEAR_TXT[1]=GetTextByID("FEAR_TXT1");
					FEAR_TXT[2]=GetTextByID("FEAR_TXT2");
					FEAR_TXT[3]=GetTextByID("FEAR_TXT3");
					FEAR_TXT[4]=GetTextByID("FEAR_TXT4");
					FEAR_TXT[5]=GetTextByID("FEAR_TXT5");
				};
				FFY0+=5;
				for( i=0;i<NFEARSUBJ;i++){
					int F=GetPanicProbability(OBJ,i)*1000/32768;
					int V=GetFScaled(F);
					int DLX=77;
					int L=DLX*V/100;
					int c=FCLR[V*8/101];
					void CBar(int x,int y,int Lx,int Ly,byte c);
					void Xbar(int x,int y,int lx,int ly,byte c);
					int x0=x1+4+(i%2)*(DLX+4);
					int y0=FFY0+(i/2)*15;
					for(int j=0;j<8;j++){
						if((j+1)*DLX/8<DLX-L)CBar(x0+j*DLX/8,y0,(DLX*(j+1)/8)-(DLX*j/8),13,FCLR[7-j]);
						else break;
						
					};
					CBar(x0+j*DLX/8,y0,DLX-L-1-(DLX*j/8),13,FCLR[7-j]);

					sprintf(str,"%s",FEAR_TXT[i]);
					ShowString(x1+4+(i%2)*(DLX+4),FFY0+(i/2)*15,str,&fn10);
					//FFY0+=12;
				};
#endif //COSSACKS2
#endif //NEWMORALE
				if(BR->WarType){
					OrderDescription* ODE=ElementaryOrders+BR->WarType-1;
					if(ODE->GroupID!=0xFF){
						SingleGroup* FGD=FormGrp.Grp+ODE->GroupID;
						for(int j=0;j<FGD->NCommon;j++){
							OneIcon* OI=AblPanel.AddIconFixed(0,OrderDesc[FGD->IDX[j]].IconID,OrderDesc[FGD->IDX[j]].IconPos);
							OI->CreateHint(OrderDesc[FGD->IDX[j]].Message);
							OI->AssignLeft(&REFORMA,BR->ID+(int(FGD->IDX[j])<<16));
						};
					};
					OneIcon* OI=AblPanel.AddIconFixed(0,FILLFORM_ICON,FillF_Pos);
					OI->AssignLeft(&FILLFORM,BR->ID);
					OI->CreateHint(GetTextByID("FILLFRM"));
					if(GO->newMons->Artpodgotovka&&(AllowArt||!SCENINF.hLib)){
						OneIcon* OI=AblPanel.AddIconFixed(0,ATTGR_ICON,AttGrPos);
						OI->CreateHint(ATGHINT);
						OI->AssignLeft(&ATTGR_PRO,0);
					};
#ifdef COSSACKS2
					if(GO->newMons->MaxGrenadesInFormation){
						extern int GRENADE_X;
						extern int GRENADE_Y;
						extern int GRENADE_ICON;
						OI=AblPanel.AddIconFixed(0,GRENADE_ICON,GRENADE_X+GRENADE_Y*12);
						OI->AssignLeft(&ThrowGrenade,BR->ID);
						static char* GRENADE_HINT=GetTextByID("GRENADE_HINT");
						OI->AssignHint(GRENADE_HINT);
						OI->AssignIntVal(BR->NGrenades);
						if(BR->GrenDelay){
							int N=int(NATIONS[BR->CT->NI].Mon[BR->MembID]->newMons->GrenadeRechargeTime)<<3;
							if(BR->GrenDelay)OI->AssignHealth(N-BR->GrenDelay,N);
						};
					};
					int NA=ODE->NActualLines;
					if(GO->newMons->ArmAttack){
						int IDXS[3]={-1,-1,-1};
						bool LENB[3]={false,false,false};
						bool LPRS[3]={false,false,false};
						int FL=ODE->FirstActualLine;
						int bp=NBPERSONAL;
						int CP=0;
						for(int p=0;p<NA;p++){
							int NU=ODE->LineNU[p+FL];
							if(NU){
								if(CP<3){
									IDXS[CP]=p;
									for(int q=0;q<NU;q++){
										if(bp<BR->NMemb){
											word ID=BR->Memb[bp];
											if(ID!=0xFFFF){
												OneObject* OB=Group[ID];
												if(OB&&OB->Serial==BR->MembSN[bp]){
													LPRS[CP]=true;
#ifdef SIMPLEMANAGE
													if(OB->RifleAttack)LENB[CP]=true;
#else
													if(!OB->NoSearchVictim)LENB[CP]=true;
#endif
												};
											};
											bp++;
										};
									};
								};
								CP++;
							};
							if(CP<=3&&CP>2){
								for(int p=0;p<3;p++){
									if(LPRS[p]&&IDXS[p]!=-1){
										int GetShotLinePos(int p);
										char* GetShotLineHint(int p);
										int GetShotLineIcon(int p);
										OneIcon* OI=AblPanel.AddIconFixed(0,GetShotLineIcon(p),GetShotLinePos(p));
										OI->CreateHint(GetShotLineHint(p));
										if(LENB[p]){
											OI->AssignLeft(&ENBLINE,1*8*1024+int(BR->ID)*8+IDXS[p]);
											OI->Selected=1;
										}else{
											OI->AssignLeft(&ENBLINE,int(BR->ID)*8+IDXS[p]);
											OI->Selected=0;
										};
									};
								};
							};
						};
					};
#endif //COSSACKS2
				};
				if(NM->ShowDelay){
					int L=0;
					int LMAX=(Zone1LY+25);
					int MaxPause=0;
					int Pause=0;
					for(int i=0;i<BR->NMemb;i++){
						word ID=BR->Memb[i];
						word SN=BR->MembSN[i];
						if(ID!=0xFFFF){
							OneObject* OBJ=Group[ID];
							if(OBJ){
								MaxPause+=OBJ->MaxDelay;
								Pause+=OBJ->delay;
							};
						};
					};
					if(MaxPause){
						L=div(LMAX*Pause,MaxPause).quot;
					}; 
					if(L<LMAX){
						Vline(Zone1X+Zone1LX-6,PanelY+Zone1Y+1+L,PanelY+Zone1Y+LMAX-2,clrGreen);
						Vline(Zone1X+Zone1LX-5,PanelY+Zone1Y+1+L,PanelY+Zone1Y+LMAX-2,clrGreen);
						Vline(Zone1X+Zone1LX-4,PanelY+Zone1Y+1+L,PanelY+Zone1Y+LMAX-2,clrGreen);
						Vline(Zone1X+Zone1LX-3,PanelY+Zone1Y+1+L,PanelY+Zone1Y+LMAX-2,clrGreen);
						//Vline(Zone1X/*+Zone1LX-1*/+3,PanelY+Zone1Y+1+L,PanelY+Zone1Y+Zone1LY-2,clrGreen);
						//Vline(Zone1X/*+Zone1LX-1*/+4,PanelY+Zone1Y+1+L,PanelY+Zone1Y+Zone1LY-2,clrGreen);
					};
				};
			};
		};
		c=0xD0+(NatRefTBL[MyNation]<<2);
		if(NARMINF&&!NINF){
			OneIcon* OI=AblPanel.AddIconFixed(0,FREE_ORDER_ICON,12*2);
			OI->AssignColor(c);
			OI->CreateHint(SFREORD);//"¦ð¸ÿº¸ªøª¹ þª¨ ô."
			OI->AssignLeft(&FREEORD,0);
		};
	 //PrpPanel.DrawIconSet(PrpX+3,PrpY+3,PrpNx,PrpNy,0);
		if(NARMINF<=12&&!NINF){
			if(CheckUnGroupPossibility(MyNation)){
				OneIcon* OI=AblPanel.AddIconFixed(0,UNGROUP_ICON,GroupIconPos);
				OI->CreateHint(GetTextByID("CM_UNGRP"));
				OI->AssignLeft(&COM_UNGROUP,0);
			}else if(CheckGroupPossibility(MyNation)){
				OneIcon* OI=AblPanel.AddIconFixed(0,DOGROUP_ICON,GroupIconPos);
				OI->CreateHint(GetTextByID("CM_DOGRP"));
				OI->AssignLeft(&COM_DOGROUP,0);
			};
		};
		if(UnitsPres&&(NINF||NARMINF)){
			if(NINF+NARMINF<=24){
				OneIcon* OI=AblPanel.AddIconFixed(0,STAND_GROUND_ICON,12*2+1);
				OI->CreateHint(STANDGR);//"Stand ground"
				OI->AssignColor(c);
				if(NARMINF==1&&!NINF){
					int bid=ArmList[0];
					if(bid!=0xFFFF){
						City* CT=NATIONS[NatRefTBL[MyNation]].CITY;
						Brigade* BR=CT->Brigs+bid;
						if(BR->WarType){
							if(BR->BrigDelay||StandGround)OI->AssignHealth(BRIGDELAY-BR->BrigDelay,BRIGDELAY);
						};
					};
				};
				if(StandGround)OI->SelectIcon();
				OI->AssignLeft(&MSTANDGR,0);
				OI=AblPanel.AddIconFixed(0,NOT_STAND_GROUND_ICON,12*2+2);
				if(NotStandGround)OI->SelectIcon();
				OI->CreateHint(NOSTAND);//"Cancel stand ground"
				OI->AssignColor(c);
				OI->AssignLeft(&CSTANDGR,0);

				OI=AblPanel.AddIconFixed(0,GOAT_Icon,GOAT_PX+GOAT_PY*12);
				OI->CreateHint(GONATT);
				OI->AssignColor(c);
				OI->AssignLeft(&GONATTACK,0);
				if(GoAndAttackMode)OI->SelectIcon();
			};
			
			if(UnitsPres&&NINF&&NINF+NARMINF<=12){
				OneIcon* OI=AblPanel.AddIconFixed(0,PATROL_ICON,14);
				OI->CreateHint(HPATROL);
				//OI->AssignColor(c);
				OI->AssignLeft(&DOPATROL,0);
			};
		};
		if((PrTowers&&!PrBuildings)||(UnitsPres&&(NINF||NARMINF)&&NINF+NARMINF<=24)||SupportsKartech){

#ifdef SIMPLEMANAGE
			if(!SupportsArmAttack){
				OneIcon* OI=AblPanel.AddIconFixed(0,EATT_Icon,EATT_X+EATT_Y*12);
				if(SrVictim){
					OI->SelectIcon();
					OI->CreateHint(DATTACK);
					OI->AssignLeft(&DATTACK_Pro,0);
				}else{
					OI->CreateHint(EATTACK);
					OI->AssignLeft(&EATTACK_Pro,0);
				};
			};
#else //SIMPLEMANAGE
			OneIcon* OI=AblPanel.AddIconFixed(0,EATT_Icon,EATT_X+EATT_Y*12);
			OI->CreateHint(EATTACK);
			OI->AssignLeft(&EATTACK_Pro,0);
			if(SrVictim)OI->SelectIcon();
			OI=AblPanel.AddIconFixed(0,DATT_Icon,DATT_X+DATT_Y*12);
			OI->CreateHint(DATTACK);
			OI->AssignLeft(&DATTACK_Pro,0);
			if(NoSrVictim)OI->SelectIcon();
#endif //SIMPLEMANAGE
		};
		if(SupportsFriendlyFire){
#ifndef SIMPLEMANAGE
			OneIcon* OI=AblPanel.AddIconFixed(0,SHOTMYUNITS_ICON,SHOTMYUNITS_X+SHOTMYUNITS_Y*12);
			OI->CreateHint(SHOTMYUNITS);
			OI->AssignLeft(&SHOTMYUNITS_Pro,0);
			if(CanKillFriend)OI->SelectIcon();
			OI=AblPanel.AddIconFixed(0,KEEPMYUNITS_ICON,KEEPMYUNITS_X+KEEPMYUNITS_Y*12);
			OI->CreateHint(KEEPMYUNITS);
			OI->AssignLeft(&KEEPMYUNITS_Pro,0);
			if(CantKillFriend)OI->SelectIcon();
#endif //SIMPLEMANAGE
		};
		if(SupportsArmAttack){
#ifdef SIMPLEMANAGE
			OneIcon* OI=AblPanel.AddIconFixed(0,ARMATTACK_ICON,ARMATTACK_X+ARMATTACK_Y*12);
			if(ArmAttack){
				OI->SelectIcon();
				OI->CreateHint(NOARMATTACK);
				OI->AssignLeft(&SETATTSTATE_Pro,0);
			}else{
				OI->CreateHint(ARMATTACK);
				OI->AssignLeft(&SETATTSTATE_Pro,1);
			};
			OI=AblPanel.AddIconFixed(0,EATT_Icon,EATT_X+EATT_Y*12);
			if(RifleAttack){
				OI->SelectIcon();
				OI->CreateHint(NORIFLEATTACK);
				OI->AssignLeft(&SETATTSTATE_Pro,128);
			}else{
				OI->CreateHint(RIFLEATTACK);
				OI->AssignLeft(&SETATTSTATE_Pro,129);
			};

#else //SIMPLEMANAGE
			OneIcon* OI=AblPanel.AddIconFixed(0,ARMATTACK_ICON,ARMATTACK_X+ARMATTACK_Y*12);
			OI->CreateHint(ARMATTACK);
			OI->AssignLeft(&ARMATTACK_Pro,0);
			if(ArmAttack)OI->SelectIcon();
			OI=AblPanel.AddIconFixed(0,ANYATTACK_ICON,ANYATTACK_X+ANYATTACK_Y*12);
			OI->CreateHint(ANYATTACK);
			OI->AssignLeft(&ANYATTACK_Pro,0);
			if(AnyAttack)OI->SelectIcon();
#endif //SIMPLEMANAGE
		};
		
		if(SupportsStopBuild&&PrpPanel.NIcons<12){
			if(StopBuild){
				OneIcon* OI=AblPanel.AddIconFixed(0,STOPBUILD_OFF,STOPBUILD_X+STOPBUILD_Y*12);
				OI->CreateHint(STOPBUILD_OFF_TXT);
				OI->AssignLeft(&STOPBUILD_Pro,0);
			}else{
				OneIcon* OI=AblPanel.AddIconFixed(0,STOPBUILD_ON,STOPBUILD_X+STOPBUILD_Y*12);
				OI->CreateHint(STOPBUILD_ON_TXT);
				OI->AssignLeft(&STOPBUILD_Pro,1);
			};
		};
		if(SupportsKartech&&NINF==1){
			OneIcon* OI=AblPanel.AddIconFixed(0,KARTECH_ICON,KARTECH_X+KARTECH_Y*12);
			OI->CreateHint(KARTECH_TXT);
			OI->AssignLeft(&KARTECH_Pro,0);
			OI=AblPanel.AddIconFixed(0,PREKARTECH_ICON,PREKARTECH_X+PREKARTECH_Y*12);
			OI->CreateHint(PREKARTECH_TXT);
			OI->AssignLeft(&PREKARTECH_Pro,0);
		};
		if(SupportsFishing&&NINF==1){
			if(FishingOn){
				OneIcon* OI=AblPanel.AddIconFixed(0,FISHING_OFF_ICON,FISHING_X+FISHING_Y*12);
				OI->CreateHint(FISHING_OFF_TXT);
				OI->AssignLeft(&FISHING_Pro,1);
			}else{
				OneIcon* OI=AblPanel.AddIconFixed(0,FISHING_ON_ICON,FISHING_X+FISHING_Y*12);
				OI->CreateHint(FISHING_ON_TXT);
				OI->AssignLeft(&FISHING_Pro,0);
			};
		};
		if(CanMakeZasev&&NINF==1){
			if(AutoZasevActive){
				OneIcon* OI=AblPanel.AddIconFixed(0,ZasevIconID,ZasevIconX+ZasevIconY*12);
				OI->CreateHint(DONTMAKEZASEV);
				OI->AssignLeft(&ZASEV_Pro,1);
			}else{
				OneIcon* OI=AblPanel.AddIconFixed(0,ZasevIconID,ZasevIconX+ZasevIconY*12);
				OI->MoreSprite=3;
				OI->CreateHint(MAKEZASEV);
				OI->AssignLeft(&ZASEV_Pro,0);
			};
		};
//		StopPF(23);
	};
	int N=BrigNx*(BrigNy-1);
	int P=0;
	Brigade* BR0=CITY[NatRefTBL[MyNation]].Brigs;
	for( j=0;j<N;j++){
		if(BR0[j].Enabled)P++;
	};
	if(P){
		OneIcon* OI=BrigPanel.AddIconFixed(0,ORDERS_LIST_ICON,0);
		OI->AssignLeft(&OLIHPRO,0);
        int brojac = 0;
		if(FullBrigMode){
			for(int j=0;j<N;j++){
				Brigade* BR=BR0+j;
				if(BR->Enabled){
					bool UNFND=true;
					bool SEL=false;
					int NU=0;
					for(int i=NBPERSONAL;i<BR->NMemb;i++){
						word MID=BR->Memb[i];
						if(MID!=0xFFFF){
							OneObject* OB=Group[MID];
							if(OB&&OB->Serial==BR->MembSN[i]){
								UNFND=false;
								SEL=OB->Selected&GM(MyNation);
								NU++;
							};
						};
					};
					if(!UNFND){
						GeneralObject* GO=NATIONS[NatRefTBL[MyNation]].Mon[BR->MembID];
						NewMonster* NM=GO->newMons;
						OI=BrigPanel.AddIconFixed(NM->IconFileID,NM->IconID,brojac+BrigNx);
						OI->AssignLeft(&SELBRIG, j);
						OI->AssignIntVal(NU);
						OI->AssignMoveOver(&PreBrig,BR->ID);
						OI->AssignIntParam(j+1);
						if(SEL)OI->SelectIcon();
                        brojac++;
					};
				};
			};
		};
	};
    //SHOWING GENERALS IN TAB, P1 IS COUNTER THAT MAKES SURE THERE IS ATLEAST ONE, N IS MATRIX 5X6, THIS CHECKS IF UNIT IS DIPLOMAT AND ADDS HIM TO COUNTER IF HE IS
    //USAGE 0x22 is diplomat to use instead officer in some cases
    int P1 = 0;
    for(i=0;i< N;i++){
		OneObject* OB=Group[i];
        if (OB && OB->newMons->Officer && OB->NNUM == NatRefTBL[MyNation])P1++;
	};
    if (P1) {
        OneIcon* OI = GeneralPanel.AddIconFixed(0, ORDERS_LIST_ICON, 0);
        OI->AssignLeft(&OLIHPRO1, 0);
        int brojac = 0;
        if (GeneralsMode) {
            for (int j = 0; j < N; j++) {
                OneObject* OB1 = Group[j];
                Brigade* BR = BR0 + j;
                bool UNFND = true;
                bool SEL = false;
                int NU = 0;
                if (OB1 && OB1->newMons->Officer && OB1->NNUM == NatRefTBL[MyNation]) {
                    //OneObject* OB1 = Group[j];
                    //if (BR->Enabled) UNFND = true;
                    SEL = OB1->Selected & GM(MyNation);
                    UNFND = false;
                } 
                //SEL = OB1->Selected & GM(MyNation);
                NU++;
                if (!UNFND) {
                    NewMonster* NM = OB1->newMons;
                    //GeneralObject* GO = NATIONS[NatRefTBL[MyNation]].Mon[BR->MembID];
                    //NewMonster* NM = GO->newMons;
                    OI = GeneralPanel.AddIconFixed(NM->IconFileID, NM->IconID, brojac + BrigNx);
                    OI->AssignLeft(&TEST, j);
                    //OI->AssignIntVal(NU);
                    //OI->AssignMoveOver(&PreBrig, j);
                    OI->AssignIntParam(brojac + 1);
                    if(SEL)OI->SelectIcon();
                    brojac++;
                };
            };
        };
    }
	/*
	if(NATIONS[MyNation].NHistory){
		OneIcon* OI=BrigPanel.AddIconFixed(0,HISTORY_Icon,1);
		OI->AssignLeft(&HISTORY_Call,0);
	};
	*/
	if(NSelSet&&!(GetKeyState(VK_SHIFT)&0x8000))NSelSet=0;
	StopPF(6);
};

int GetRLCStrWidth(char* str,lpRLCFont lpf);
extern byte PlayGameMode;
char* ARTCAPS[8]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
void ShowTextDiscription(){
	if(PlayGameMode==1)return;
	RunPF(4,"ShowTextDescription");
	StopPF(4);
    if(NINF==1){
		//Xbar(Zone1X,PanelY+Zone1Y+1,Zone1LX,Zone1LY,0xD0+(MyNation<<2));
		//Xbar(Zone1X-1,PanelY+Zone1Y,Zone1LX+2,Zone1LY+2,0xD0+(MyNation<<2)+1);
		MonInf* MI=&(MList[0]);
		OneObject* OBJ=Group[MI->Last];
		if(!OBJ)return;
		RunPF(4,"ShowTextDescription");
		if(OBJ->Kills&&!(HintTime&&WasLoHint)){
			char str[128];
			sprintf(str,KILLUNI,OBJ->Kills);//"Lñøû:%d"
			ShowString(Zone1X+10,PanelY+Zone1Y-20,str,&SmallWhiteFont);
		};
		if(OBJ->Ready){
			int DYY=-7;
			char str[128];
			int x1=Zone1X+IconLx+4+6-4;
			int y1=PanelY+Zone1Y+6+DYY;
			AdvCharacter* ADC=OBJ->Ref.General->MoreCharacter;
			NewMonster* NM=OBJ->newMons;
			int NN=0;
			int EFFICACY=1000;
			if(NM->ShotPtZ){
				int NS=0;
				int MAXS=OBJ->Ref.General->MoreCharacter->MaxInside;
				for(int i=0;i<OBJ->NInside;i++){
					word MID=OBJ->Inside[i];
					if(MID!=0xFFFF){
						OneObject* IOB=Group[MID];
						if(IOB){
							byte USE=IOB->newMons->Usage;
							if(USE==StrelokID||USE==HorseStrelokID||USE==ArcherID)NS++;
						};
					};
				};
				if(MAXS){
					EFFICACY=(NS*1000)/MAXS;
				};
			};
			byte skm=NM->SkillDamageMask;
			for(int i=0;i<NAttTypes;i++){
				if(ADC->MaxDamage[i]&&(!(WeaponFlags[NM->WeaponKind[i]]&64))&&NN<2){
					GPS.ShowGP(x1+NN*IC_ADISTX+1,y1+1,1,WeaponIcn[ADC->WeaponKind[i]],0);
					int DMIN=NM->MinDamage[i];
					int dm=ADC->MaxDamage[i];
					
					dm=DMIN+(((dm-DMIN)*EFFICACY)/1000);
					int adm=0;
					if(skm&(1<<i)){
						adm=(int(OBJ->Kills)*int(NM->SkillDamageBonus))/10;
					};

					//dm=(dm+(dm>>1))>>1;
					if(OBJ->AddDamage+adm>0)sprintf(str,"%d+%d",dm,OBJ->AddDamage+adm);
					else if(OBJ->AddDamage+adm<0)sprintf(str,"%d-%d",dm,-OBJ->AddDamage+adm);
					else sprintf(str,"%d",dm);
					ShowString(x1+NN*IC_ADISTX+IC_DISTY,y1+5,str,&SmallWhiteFont);
					NN++;
				};
			};
			sprintf(str,"%d",int(OBJ->Life));
			int LLL=GetRLCStrWidth(str,&SmallWhiteFont);
			ShowString(Zone1X+(IconLx-LLL>>1)+6,PanelY+Zone1Y+IconLx+1+6+DYY,str,&SmallWhiteFont);
			y1+=21;
			GPS.ShowGP(x1+1,y1+1,1,SGenIcon,0);
			if(OBJ->AddShield>0)sprintf(str,"%d+%d",ADC->Shield,OBJ->AddShield);
			else if(OBJ->AddShield<0)sprintf(str,"%d-%d",ADC->Shield,-OBJ->AddShield);
			else sprintf(str,"%d",ADC->Shield);
			ShowString(x1+21,y1+12+DYY,str,&SmallWhiteFont);
			y1=PanelY+Zone1Y+IconLx+10+9+DYY-2;
			x1=Zone1X+4;
			NN=0;
			if(!NM->Building){
				for(int i=0;i<16;i++){
					if(ADC->Protection[i]&&NN<6){
						int ix=x1+(NN>>1)*IC_DISTX;
						int iy=y1+(NN&1)*IC_DISTY;
						GPS.ShowGP(ix+3,iy+1,1,ProtectIcn[i],0);
						int dm=ADC->Protection[i];
						//  ;-)  //dm=((dm+1)*100)>>8;
						sprintf(str,"%d",dm);
						ShowString(ix+22,iy+5,str,&SmallWhiteFont);
						NN++;
					};
				};
			};
			int xf=x1;
			int yf=y1+IC_DISTY*2;
			int nf=0;
			ShowTiringInfo(OBJ,x1,y1+36-8,120,4);
#ifdef NEWMORALE
			void DrawNewMoraleInterface(int x,int y,OneObject* OB);
			DrawNewMoraleInterface(x1,y1+36-8,OBJ);
#else //NEWMORALE
#ifdef COSSACKS2
				int FFY0=y1+36;
				if(!FEAR_TXT[0]){
					FEAR_TXT[0]=GetTextByID("FEAR_TXT0");
					FEAR_TXT[1]=GetTextByID("FEAR_TXT1");
					FEAR_TXT[2]=GetTextByID("FEAR_TXT2");
					FEAR_TXT[3]=GetTextByID("FEAR_TXT3");
					FEAR_TXT[4]=GetTextByID("FEAR_TXT4");
					FEAR_TXT[5]=GetTextByID("FEAR_TXT5");
				};
				FFY0+=5;
				for(int i=0;i<NFEARSUBJ;i++){
					int F=GetPanicProbability(OBJ,i)*1000/32768;
					int V=GetFScaled(F);
					int DLX=77;
					int L=DLX*V/100;
					int c=FCLR[V*8/101];
					void CBar(int x,int y,int Lx,int Ly,byte c);
					void Xbar(int x,int y,int lx,int ly,byte c);
					int x0=x1+4+(i%2)*(DLX+4);
					int y0=FFY0+(i/2)*15;
					for(int j=0;j<8;j++){
						if((j+1)*DLX/8<DLX-L)CBar(x0+j*DLX/8,y0,(DLX*(j+1)/8)-(DLX*j/8),13,FCLR[7-j]);
						else break;
						
					};
					CBar(x0+j*DLX/8,y0,DLX-L-1-(DLX*j/8),13,FCLR[7-j]);

					sprintf(str,"%s",FEAR_TXT[i]);
					ShowString(x1+4+(i%2)*(DLX+4),FFY0+(i/2)*15,str,&fn10);
					//FFY0+=12;
				};
#endif //COSSACKS2
#endif //NEWMORALE
			/*
			for(int i=0;i<NFEARSUBJ;i++)if(FearIcons[i]!=-1&&NM->FearFactor[i]){
				int F=(GetPanicProbability(OBJ,i)+1)*1000/32768;
				sprintf(str,"%d.%d%%",FEAR_TXT[i],F/10,F%10);
				GPS.ShowGP(xf+3,yf,1,FearIcons[i],0);
				ShowString(xf+23,yf+3,str,&SmallWhiteFont);
				xf+=IC_DISTX;
				nf++;
				if(nf>2){
					xf=x1;
					yf+=IC_DISTY;
					nf=0;
				};
			};
			int FFY0=y1+36;
			if(!FEAR_TXT[0]){
				FEAR_TXT[0]=GetTextByID("FEAR_TXT0");
				FEAR_TXT[1]=GetTextByID("FEAR_TXT1");
				FEAR_TXT[2]=GetTextByID("FEAR_TXT2");
				FEAR_TXT[3]=GetTextByID("FEAR_TXT3");
				FEAR_TXT[4]=GetTextByID("FEAR_TXT4");
				FEAR_TXT[5]=GetTextByID("FEAR_TXT5");
			};
			for(int i=0;i<NFEARSUBJ;i++)if(NM->FearFactor[i]){
				int F=(GetPanicProbability(OBJ,i)+1)*1000/32768;
				sprintf(str,"%s: %d.%d%%",FEAR_TXT[i],F/10,F%10);
				ShowString(x1+2,FFY0,str,&fn10);
				FFY0+=12;
			};
			*/
			if(NM->FishAmount){
				sprintf(str,FISHSTR,OBJ->Ustage,ADC->FishAmount);//"¦vñð:%d/%d"
				ShowString(x1+3,y1,str,&fn10);
				y1+=12;
			};
			if(NM->Farm){
				Nation* NT=OBJ->Nat;
				/*
				City* CT=NT->CITY;
				sprintf(str,"T¸õóþ ¯õ¨ü:%d",CT->ReadyAmount[OBJ->NIndex]);
				ShowString(x1,y1,str,&fn10);
				y1+=12;
				sprintf(str,"=ºöýþ:%d",div(NT->NGidot,UnitsPerFarm).quot+1);
				ShowString(x1,y1,str,&fn8);
				y1+=12;
				*/
				sprintf(str,HABITAN,NT->NGidot);//"+ñøªðªõûõù:%d"
				ShowString(x1+3,y1,str,&fn10);
				y1+=12;
				sprintf(str,MAXHABI,NT->NFarms);//"¦ðú¸.þñøªðªõûõù:%d"
				ShowString(x1+3,y1,str,&fn10);
				y1+=12;

			};
			if(NM->ArtDepo){
				if(!ARTCAPS[0]){
					ARTCAPS[0]=GetTextByID("__ARTCAP0");
					ARTCAPS[1]=GetTextByID("__ARTCAP2");
					ARTCAPS[2]=GetTextByID("__ARTCAP1");
					ARTCAPS[3]=GetTextByID("__ARTCAP3");
				};
				Nation* NT=OBJ->Nat;
				for(int i=0;i<4;i++){
					if(ARTCAPS[i][0]!='_'&&NM->ArtCap[i]){
						sprintf(str,ARTCAPS[i],NT->NArtUnits[i],NM->ArtCap[i]*NT->NArtdep);//"+ñøªðªõûõù:%d"
						ShowString(x1+3,y1,str,&fn10);
						y1+=12;
					};
				};

			};
			if(ADC->MaxInside){
#ifdef COSSACKS2
				int NINS=OBJ->NInside;
				int NMAX=OBJ->Ref.General->MoreCharacter->MaxInside+OBJ->AddInside;
				if(NINS>NMAX){
					sprintf(str,INSISTR,NMAX);
					sprintf(str+strlen(str),"+%d",NINS-NMAX);
					ShowString(x1+3,y1,str,&SmallRedFont);
				}else{
					sprintf(str,INSISTR,NINS);
					ShowString(x1+3,y1,str,&fn10);
				};
#else
				sprintf(str,INSISTR,OBJ->NInside);
				ShowString(x1+3,y1,str,&fn10);
#endif
				y1+=12;
				if(OBJ->AddInside){
					sprintf(str,MAXINSI,int(ADC->MaxInside),OBJ->AddInside);//"¦ðú¸.Týºª¨ø:%d+%d"
					ShowString(x1+3,y1,str,&fn10);
					y1+=12;
				}else{
					sprintf(str,MAXINS1,int(ADC->MaxInside));//"¦ðú¸.Týºª¨ø:%d"
					ShowString(x1+3,y1,str,&fn10);
					y1+=12;
				};
				char* RNAME=GetSprResourceName(OBJ);
				if(RNAME){
					ShowString(x1+3,y1,RNAME,&fn10);
					y1+=12;
				};
			};
			/*
			if(OBJ->Repair){
				ShowString(x1,y1,"¦õüþýªýøú",&fn10);
				y1+=16;
			};
			*/
			if(OBJ->newMons->ShowDelay){
				int L=0;
				int LMAX=(Zone1LY+25);
				if(MI->MaxPause){
					L=div(LMAX*MI->Pause,MI->MaxPause).quot;
				}; 
				if(L<LMAX){
					Vline(Zone1X+Zone1LX-5,PanelY+Zone1Y+1+L,PanelY+Zone1Y+LMAX-2,clrGreen);
					Vline(Zone1X+Zone1LX-4,PanelY+Zone1Y+1+L,PanelY+Zone1Y+LMAX-2,clrGreen);
					Vline(Zone1X+Zone1LX-3,PanelY+Zone1Y+1+L,PanelY+Zone1Y+LMAX-2,clrGreen);
					Vline(Zone1X+Zone1LX-2,PanelY+Zone1Y+1+L,PanelY+Zone1Y+LMAX-2,clrGreen);
					//Vline(Zone1X/*+Zone1LX-1*/+3,PanelY+Zone1Y+1+L,PanelY+Zone1Y+Zone1LY-2,clrGreen);
					//Vline(Zone1X/*+Zone1LX-1*/+4,PanelY+Zone1Y+1+L,PanelY+Zone1Y+Zone1LY-2,clrGreen);
				};
			};
		};
		StopPF(4);
	};
//	StopPF(35);
};
extern bool BuildMode;
extern OneSlide* OSB;
extern byte blx;
extern byte bly;
extern word BuildingID;
extern Nation* BNat;
void CmdUnProduceObj(byte NI,word Type);
void RHPR(int i){
	AblInf* AI=&AList[i];
	Nation* NT=AI->NT;
	word j=AI->OInd;
	GeneralObject* GO=NT->Mon[j];
	if(!GO->newMons->Building){
		if(GetKeyState(VK_SHIFT)&0x8000){
			CmdUnProduceObj(MyNation,j);
			CmdUnProduceObj(MyNation,j);
			CmdUnProduceObj(MyNation,j);
			CmdUnProduceObj(MyNation,j);
			CmdUnProduceObj(MyNation,j);
		}else CmdUnProduceObj(MyNation,j);
	};
};
void CmdFieldBar(byte NI,word n);
bool CheckCostHint(byte NI,word NIndex);
int GetTotalUnits();
extern bool NOPAUSE;
void HPR(int i){
	//if(!NOPAUSE)return;
	if(GetTotalUnits()>LULIMIT){
		AssignHint1(UNILIMIT,50,32);
		return;
	};
	AblInf* AI=&AList[i];
	Nation* NT=AI->NT;
	word j=AI->OInd;
	GeneralObject* GO=NT->Mon[j];
	NewMonster* NM=GO->newMons;
	if(NM->SpriteObject&&!NM->Wall){
		CmdFieldBar(MyNation,j);
		return;
	};
	if(NM->Building||NM->Wall){
		if(NM->Wall||CheckCostHint(NatRefTBL[MyNation],j)){
			BuildMode=true;
			BuildingID=j;
			BNat=NT;
		};
	}else{
		if(CheckCostHint(NatRefTBL[MyNation],j)){
			if(GetKeyState(VK_SHIFT)&0x8000){
				CmdProduceObj(MyNation,j);
				CmdProduceObj(MyNation,j);
				CmdProduceObj(MyNation,j);
				CmdProduceObj(MyNation,j);
				CmdProduceObj(MyNation,j);
			}else CmdProduceObj(MyNation,j);
		};
	}
};
void HPR1(int i){
	//if(!NOPAUSE)return;
	Nation* NT=&NATIONS[MyNation];
	GeneralObject* GO=NT->Mon[i];
	NewMonster* NM=GO->newMons;
	if(NM->Building){
		BuildMode=true;
		BuildingID=i;
		BNat=NT;
	}else{
		if(GetKeyState(VK_SHIFT)&0x8000){
			CmdProduceObj(MyNation,i);
			CmdProduceObj(MyNation,i);
			CmdProduceObj(MyNation,i);
			CmdProduceObj(MyNation,i);
			CmdProduceObj(MyNation,i);
		}else CmdProduceObj(MyNation,i);
	}
};
void DoUPG(int i){
	AblInf* AI=&AList[i];
	Nation* NT=AI->NT;
	word j=AI->OInd;
	CmdPerformUpgrade(MyNation,j);
};
void DoUPG1(int i){
	CmdPerformUpgrade(MyNation,i);
};
void EnterIn(int i){
	if(i==0xFFFF)SubIcon=-1;
	WIcon* IC=NATIONS[MyNation].wIcons[i];
	if(!IC->SubList){
		SubIcon=-1;
		return;
	};
	SubIcon=i;
};
void OrderMove(int i,int x,int y){
	CmdSendToXY(MyNation,x,y,512);
};
void OrderAttack(int i,int x,int y){
	//word MID=GetOwner((x<<5)+16,(y<<5)+16);
	/*if(MID!=0xFFFF){
		OneObject* OB=Group[MID];
		byte Msk=1<<MyNation;
		if(OB->NMask&Msk)CmdAttackToXY(MyNation,x,y);
		else CmdAttackObj(MyNation,MID);
	}else CmdAttackToXY(MyNation,x,y);
	*/
};
void OrderPatrol(int i,int x,int y){
	CmdPatrol(MyNation,x,y);
};
void CmdSetRprState(byte NI,byte State);
void OrderRepair(int i,int x,int y){
	CmdSetRprState(MyNation,1);
};
void OrderNucAtt(int i,int x,int y){
	CmdNucAtt(MyNation,x,y);
};
void OrderSetDst(int i,int x,int y){
	CmdSetDst(MyNation,x,y);
};
void SelPage1(int par){
	NewMonster* NM=(NewMonster*)par;
	NM->CurrPage=0;
};
void SelPage2(int par){
	NewMonster* NM=(NewMonster*)par;
	NM->CurrPage=1;
};
void SelPage3(int par){
	NewMonster* NM=(NewMonster*)par;
	NM->CurrPage=2;
};
word MaxMagic;
extern int MousDX[32];
extern int MousDY[32];
CEXPORT
void SetCurPtr(int v);
void UNIPARAM(int i){
	SetCurPtr(2);
	//curdx=16;
	//curdy=15;
	curdx=MousDX[curptr];
	curdy=MousDY[curptr];
	GetCoord=true;
	UNIM=NULL;
	switch(i){
	case 1://Move to xy
		UNIM=&OrderMove;
		break;
	case 2://Attack near (x,y)
		UNIM=&OrderAttack;
		break;
	case 5://Patrol
		UNIM=&OrderPatrol;
		break;
	case 6://Repair
		UNIM=&OrderRepair;
		break;
	case 7:
		//UNIM=&OrderGetResource;
		break;
	case 8:
		//UNIM=&OrderUnload;
		break;
	case 9:
		//UNIM=&OrderAttPt;
		break;
	case 10:
		UNIM=&OrderNucAtt;
		break;
	case 11:
		UNIM=&OrderSetDst;
		break;
	};
};
void NOPARAM(int i){
	UNIM=NULL;
	switch(i){
	case 3://Stop
		CmdStop(MyNation);
		break;
	case 4://Stand Ground
		CmdStandGround(MyNation);
		break;
	case 5:
		CmdSetRprState(MyNation,1);
		break;
	case 6:
		CmdSetRprState(MyNation,0);
		break;
	case 7://Build wall
		//SetBuildWallMode();
		break;
	};
};
void OrderPointN(int i,int x,int y){
	
};
void OrderPoint0(int i,int x,int y){
	OrderPointN(0,x,y);
};
void OrderPoint1(int i,int x,int y){
	OrderPointN(1,x,y);
};
void OrderPoint2(int i,int x,int y){
	OrderPointN(2,x,y);
};
void OrderPoint3(int i,int x,int y){
	OrderPointN(3,x,y);
};
void OrderPoint4(int i,int x,int y){
	OrderPointN(4,x,y);
};
void OrderPoint5(int i,int x,int y){
	OrderPointN(5,x,y);
};
void OrderPoint6(int i,int x,int y){
	OrderPointN(6,x,y);
};
void OrderPoint7(int i,int x,int y){
	OrderPointN(7,x,y);
};
void OrderPoint8(int i,int x,int y){
	OrderPointN(8,x,y);
};
void OrderPoint9(int i,int x,int y){
	OrderPointN(9,x,y);
};
void OrderPoint10(int i,int x,int y){
	OrderPointN(10,x,y);
};
void OrderPoint11(int i,int x,int y){
	OrderPointN(11,x,y);
};
void STRANGEWEAPON(int i){
	//if(MaxMagic<NATIONS[MyNation].WMagic[i])return;
	SetCurPtr(3);
	//curdx=16;
	//curdy=16;
	curdx=MousDX[curptr];
	curdy=MousDY[curptr];
	GetCoord=true;
	UNIM=NULL;

	switch(i){
	case 0:
		UNIM=&OrderPoint0;
		break;
	case 1:
		UNIM=&OrderPoint1;
		break;
	case 2:
		UNIM=&OrderPoint2;
		break;
	case 3:
		UNIM=&OrderPoint3;
		break;
	case 4:
		UNIM=&OrderPoint4;
		break;
	case 5:
		UNIM=&OrderPoint5;
		break;
	case 6:
		UNIM=&OrderPoint6;
		break;
	case 7:
		UNIM=&OrderPoint7;
		break;
	case 9:
		UNIM=&OrderPoint9;
		break;
	case 10:
		UNIM=&OrderPoint10;
		break;
	};
};
bool CreateInsideList(IconSet* IS,byte NI);
bool ECOSHOW;
extern byte PlayGameMode;
int ShowUniqAbility(){
	RunPF(1,"ShowUniqAbility");
	StopPF(1);
	if(PlayGameMode==1)return 0;
	ECOSHOW=false;
	char sxt[128];
	if(MList[0].Last>LULIMIT)return false;
	GeneralObject* GO=MList[0].RF;
	OneObject* OB=Group[MList[0].Last];
	Nation* NT=OB->Nat;
	if(NT!=LastNT||MList[0].Last!=LastID||GO!=LastGO)SubIcon=-1;
	LastNT=NT;
	LastID=MList[0].Last;
	//MaxMagic=MList[0].MaxMag;
	LastGO=GO;
	int NABL;
	WIcon* SIC;
	if(SubIcon==-1)NABL=GO->NIcons;
	else{
		SIC=NT->wIcons[SubIcon];
		NABL=SIC->Param1;
		if(!SIC->SubList){
			SubIcon=-1;
			NABL=GO->NIcons;
		};
	};
    CreateInsideList(&AblPanel,MyNation);
	if(!NABL)return 0;
	RunPF(1,"ShowUniqAbility");
	WIcon* MI;
	int IREF;
	//char str[32];
	int spr,fid; //p,q,
	//Visuals* VS;
    OneIcon* OI;
	for(int i=0;i<NABL;i++){
		bool DrawIt=true;
		if(SubIcon==-1){
			MI=NT->wIcons[GO->IRefs[i]];
			IREF=GO->IRefs[i];
		}else{
			MI=NT->wIcons[SIC->SubList[i]];
			IREF=SIC->SubList[i];
		};
		switch(MI->Kind){
		case 0:
			//directory entry
            OI=AblPanel.AddIcon(MI->FileID,MI->Spr,0x1B,MI->Message);
            OI->AssignLeft(&EnterIn,IREF);
			break;
		case 1://uniq without param
            OI=AblPanel.AddIcon(MI->FileID,MI->Spr,0x1B,MI->Message);
            OI->AssignLeft(&NOPARAM,MI->Param2);
			break;
		case 2://uniq with param
            OI=AblPanel.AddIcon(MI->FileID,MI->Spr,0x1B,MI->Message);
            OI->AssignLeft(&UNIPARAM,MI->Param2);
			break;
		case 3://perform upgrade
			{
				/*
				SimpleUTP* SU=NT->UPG.utp[MI->Param2];
				if(SU->Enabled){
                    strcpy(sxt,SU->Message);
					strcat(sxt," (");
					NT->GetUpgradeCostString(sxt,MI->Param2);
					strcat(sxt,")");
                    OI=AblPanel.AddIcon(MI->FileID,MI->Spr,240,sxt);
                    OI->AssignLeft(&DoUPG1,MI->Param2);
				}else DrawIt=false;
				*/
			};
			break;
		case 4://produce object
			GO=NT->Mon[MI->Param2];
			if(GO->Enabled){
				if(!MI->FileID){
					fid=GO->newMons->IconFileID;
					spr=GO->newMons->IconID;
				}else{
					fid=MI->FileID;
					spr=MI->Spr;
				};
                GetChar(GO,sxt);
                OI=AblPanel.AddIcon(MI->FileID,MI->Spr);
				OI->AssignColor(0xD0+(NatRefTBL[MyNation]<<2));
				OI->CreateHint(sxt);
                OI->AssignLeft(&HPR1,MI->Param2);
			}else DrawIt=false;
			break;
		case 5://strange weapon
			//if(NT->SWP[MI->Param1].Enabled){
			//	ShowRLCItem(x1,y1,&MImage[MI->FileID],MI->Spr,MyNation);
			//	CreateZone(x1,y1,IconLx,IconLy,&STRANGEWEAPON,MI->Param2,MI->Message);
			//}else DrawIt=false;
			break;
		};
	};
	StopPF(1);
	if(GO->ExtMenu)return NABL;
	return -1;
};
bool ShowCommonAbl(){
	RunPF(2,"ShowCommonAbility");
	StopPF(2);
	bool isAir=false;
	bool isWater=false;
	bool isLand=false;

    OneIcon* OI;
	//char sxt[128];
	if(!(isAir||isLand||isWater))return false;
	RunPF(2,"ShowCommonAbility");
	Nation* NT=&NATIONS[MyNation];
	word NIcons;
	word* Icons=NULL;
	if(isAir&&!(isLand||isWater)){
		NIcons=NT->NAmenus;
		Icons=NT->Amenus;
	};
	if(isLand&&!(isAir||isWater)){
		NIcons=NT->NLmenus;
		Icons=NT->Lmenus;
	};
	if(isWater&&!(isAir||isLand)){
		NIcons=NT->NWmenus;
		Icons=NT->Wmenus;
	};
	if(!Icons){
		NIcons=NT->NCmenus;
		Icons=NT->Cmenus;
	};
	
	if(NT!=LastNT||LastID!=7777)SubIcon=-1;
	LastID=7777;
	LastNT=NT;
	int NABL;
	WIcon* SIC;
	if(SubIcon==-1)NABL=NIcons;
	else{
		SIC=NT->wIcons[SubIcon];
		NABL=SIC->Param1;
		if(!SIC->SubList){
			SubIcon=-1;
			NABL=NIcons;
		};
	};
	int AN=AblNx*AblNy;
	if(NABL>AN)NABL=AN;
	int xx=0;
	int x1=AblX;
	int y1=AblY;
	//OneSlide* OC;
	WIcon* MI;
	int IREF;
	//char str[32];
	//int p,q,spr,fid;
	//Visuals* VS;
	//MaxMagic=0;
	for(int i=0;i<NABL;i++){
		if(SubIcon==-1){
			MI=NT->wIcons[Icons[i]];
			IREF=Icons[i];
		}else{
			MI=NT->wIcons[SIC->SubList[i]];
			IREF=SIC->SubList[i];
		};
		switch(MI->Kind){
		case 0:
			//directory entry
            OI=AblPanel.AddIcon(MI->FileID,MI->Spr,0x1B,MI->Message);
            OI->AssignLeft(&EnterIn,IREF);
			break;
		case 1://uniq without param
            OI=AblPanel.AddIcon(MI->FileID,MI->Spr,0x1B,MI->Message);
            OI->AssignLeft(&NOPARAM,MI->Param2);//IREF);
			break;
		case 2://uniq with param
            OI=AblPanel.AddIcon(MI->FileID,MI->Spr,0x1B,MI->Message);
            OI->AssignLeft(&NOPARAM,MI->Param2);
			break;
		case 3:
			break;
		case 4:
			break;
		case 5://strange weapon
            OI=AblPanel.AddIcon(MI->FileID,MI->Spr,0x1C,MI->Message);
            OI->AssignLeft(&STRANGEWEAPON,MI->Param2);
			break;
		};
	};
	StopPF(2);
	return true;

};
bool CheckCost(byte NI,word NIndex,int x,int y);
extern byte BaloonState;
extern byte CannonState;
extern byte XVIIIState;
extern bool EditMapMode;
extern int LastMx;
extern int LastMy;

bool PAGE1PRESENT=0;
bool PAGE2PRESENT=0;
bool PAGE3PRESENT=0;
extern NewMonster* LASTNM;

void CmdChangeNPID(byte,word);
extern bool RejectInterface;

/// THIS SHOWS ABILITES OF BUIDLINGS AND UNITS LEFT BOTTOM WINDOW
void ShowAbility(){
	LASTNM=NULL;
	PAGE1PRESENT=0;
	PAGE2PRESENT=0;
	PAGE3PRESENT=0;
	if(NARMINF||RejectInterface)return;
	char sxt[128];
	RunPF(0,"ShowAbility");
	StopPF(0);
	//InitZones();
	NABL=0;
	//word MID;
	OneObject* OBJ;
	GeneralObject* GO;
	AblInf* MI;
	Nation* NT;
	word s;
	int i,j,k,p,q,r;
	//if(NINF)ResFon(AblX,AblY,AblNx*IconLx,AblNy*IconLy);
	if(!NINF){
		//ResFon(AblX,AblY,AblNx*IconLx,AblNy*IconLy);
		//ScrCopy(PrpX,PrpY,PrpNx*IconLx,PrpNy*IconLy);
		return;
	};
	int nability=0;
	if(NINF==1){
		//RunPF(28);
		nability=ShowUniqAbility();
//		StopPF(28);
		if(nability==-1)return;
	};
	if(NINF>1)return;
	AblPanel.Page=0;

	if(NINF){
		OBJ=Group[MList[0].Last];
		if(OBJ&&OBJ->Ready){
			NewMonster* NM=OBJ->newMons;
			AblPanel.Page=NM->CurrPage;
			LASTNM=NM;
		};
	};
	if(ShowCommonAbl())return;
	RunPF(0,"ShowAbility");
	//calculating monsters could be produced
	char NATID[5];
	int NPID=CITY[NatRefTBL[MyNation]].NationalPeasantID;
	if(NPID!=0xFFFF&&!PlayGameMode){
		char* S=NATIONS->Mon[NPID]->MonsterID;
		int L=strlen(S);
		strcpy(NATID,S+L-4);
	}else{
		NATID[0]=0;
	};
	bool CANDO1=1;
	if(NATID[0]){
		CANDO1=strstr(OBJ->Ref.General->MonsterID,NATID);
	};
	for( i=0;i<NINF;i++){
		OBJ=Group[MList[i].Last];
		bool OKK=1;
#ifndef EW
        if (OBJ && OBJ->newMons->Peasant && !(EditMapMode || PlayGameMode)) {
            if (NPID == 0xFFFF) {
                CITY[NatRefTBL[MyNation]].NationalPeasantID = OBJ->NIndex;
                CmdChangeNPID(NatRefTBL[MyNation], OBJ->NIndex);
            }
            else {
                if (NPID != OBJ->NIndex)OKK = 0;
            };
        };
#endif // !EW
		bool CANDO=1;
		if(NATID[0]){
			CANDO=CANDO1;
		};
		if(OKK&&CANDO&&OBJ->Ready){
			NT=OBJ->Nat;
			k=OBJ->Index;
			p=OBJ->NIndex;
			q=NT->PACount[p];
			for(r=0;r<q;r++){
				s=NT->PAble[p][r];	
				GeneralObject* GO=NT->Mon[s];
				if(GO->Enabled){
					bool okk=1;
					if(GO->StageMask){
						byte m=GO->StageMask;
						word s=OBJ->StageState;
						for(int i=0;i<5;i++){
							if(m&1){
								byte s1=s&7;
								if(s1!=2)okk=0;
							};
							m>>=1;
							s>>=3;
						};
					};
					if(okk){
						for( j=0;j<NABL;j++){
							if(AList[j].OInd==s)break;
						};
						AList[j].OInd=s;
						AList[j].NT=NT;
						AList[j].Kind=0;
						AList[j].HotKey=NT->AHotKey[p][r];
						AList[j].IconIndex=NT->AIndex[p][r];	
						if(j>=NABL)NABL=j+1;
					};
				};
			};
		};
	};
	//calculate upgrades
	for( i=0;i<NINF&&CANDO1;i++){
		OBJ=Group[MList[i].Last];
		if(OBJ->Ready){
			NT=OBJ->Nat;
			k=OBJ->Index;
			p=OBJ->NIndex;
			GeneralObject* GO=NT->Mon[p];
			q=GO->NUpgrades;
			for(r=0;r<q;r++){
				s=GO->Upg[r];	
				NewUpgrade* UPG=NT->UPGRADE[s];
				if((UPG->ManualEnable)||((!UPG->ManualDisable)&&(UPG->Enabled||(UPG->Gray&&!UPG->Done)))){
					NewUpgrade* NUPG=NT->UPGRADE[s];
					bool OKK=false;
					if(NUPG->Individual||NUPG->StageUp){
						if(NUPG->Individual&&OBJ->SingleUpgLevel==NUPG->Level)OKK=true;
//#ifdef CONQUEST
						if(NUPG->StageUp){
							byte st=(OBJ->StageState>>((NUPG->CtgUpgrade-24)*3))&7;
							if(st!=2)OKK=true;
						};
//#endif
					}else OKK=true;
					if(NUPG->StageMask){
						byte m=NUPG->StageMask;
						word st=OBJ->StageState;
						for(int i=0;i<5;i++){
							if(m&1){
								byte cs=st&7;
								if(cs!=2)OKK=false;
							};
							m >>=1;
							st>>=3;
						};
					};
					for( j=0;j<NABL;j++){
						if(AList[j].Kind==1&&AList[j].OInd==s)break;
					};
					if(OKK){
						AList[j].OInd=s;
						AList[j].NT=NT;
						AList[j].Kind=1;
						AList[j].HotKey=0;
						AList[j].IconIndex=-1;
						if(j>=NABL)NABL=j+1;
					};
				};
			};
		};
	};
	//show abilities
	int AN=AblNx*AblNy;
	if(NABL>AN)NABL=AN;
	int xx=nability;
	int x1=AblX+(IconLx+AddIconLx)*(xx&3);
	int y1=AblY+(IconLy+AddIconLy)*(xx>>2);
	xx=xx&3;
	//OneSlide* OC;
    OneIcon* OI;
    int GA,GP;
	//char str[512];
	int MaxProgr;

	bool P1Present=0;
	bool P2Present=0;
	bool P3Present=0;

	//RunPF(30);
	for( i=0;i<NABL&&CANDO1;i++){
		MI=&(AList[i]);
		switch(MI->Kind){
		case 0:
			GO=(MI->NT)->Mon[MI->OInd];
			if(!((BaloonState&&GO->Options&2)||
			 (CannonState==1&&GO->Options&1)||
			 (XVIIIState&&GO->Options&4))){
				p=GO->newMons->IconFileID;
				q=GO->newMons->IconID;
				if(!GO->newMons->Building){
					GA=GetAmount(MI->OInd);
					GP=GetProgress(MI->OInd,&MaxProgr);
				}else{
					GA=0;
					GP=0;
				};
				GetChar(GO,sxt);
				GetMonCost(NatRefTBL[MyNation],MI->OInd,sxt);
				OI=AblPanel.AddIconFixed(p,q,MI->IconIndex);
				OI->AssignColor(0xD0+(NatRefTBL[MyNation]<<2));
				OI->CreateHint(sxt);
				OI->AssignHealth(GP,MaxProgr);
				OI->AssignLeft(&HPR,i);
				OI->AssignRight(&RHPR,i);
				OI->AssignIntVal(GA);
				Nation* NT=NATIONS+NatRefTBL[MyNation];
				if(!CheckCost(NatRefTBL[MyNation],MI->OInd,LastMx,LastMy))OI->AssignRedPulse();
				if(GO->newMons->ArtSet&&NT->NArtUnits[GO->newMons->ArtSet-1]>=NT->NArtdep*GO->newMons->NInArtDepot){
					OI->AssignRedPulse();
				};
			};
			break;
		case 1://upgrade caps
			{
				NewUpgrade* SU=MI->NT->UPGRADE[MI->OInd];
				if(SU->ManualEnable||(!(SU->ManualDisable||(BaloonState&&SU->Options&2)||
				 (CannonState==1&&SU->Options&1)||
				 (XVIIIState&&SU->Options&4)))){
					sxt[0]=0;
					MI->NT->GetUpgradeCostString(sxt,MI->OInd);
		            OI=AblPanel.AddIconFixed(SU->IconFileID,SU->IconSpriteID,SU->IconPosition);
					OI->AssignColor(0xD0+(NatRefTBL[MyNation]<<2));
					OI->AssignHealth(SU->CurStage,SU->NStages);
					OI->AssignLevel(SU->Level);
					if(SU->Gray&&!(SU->Enabled||SU->Done))OI->Disable();
					bool NEWHINT=0;
					if(OBJ&&(OBJ->InFire||(OBJ->Life<2000&&OBJ->NewBuilding))){
						if(SU->CtgUpgrade>=24&&SU->CtgUpgrade<=28){
							NEWHINT=1;
							OI->CreateHint(GetTextByID("MUSTREPAIR"));
							OI->AssignRedPulse();
						};
					};
					if(!NEWHINT){
						OI->AssignLeft(&DoUPG,i);
						OI->CreateHint(SU->Message);
						OI->CreateHintLo(sxt);
						bool dis=0;
						for(int p=0;p<6;p++){
							if(XRESRC(NatRefTBL[MyNation],p)<SU->Cost[p])dis=1;
						};
						if(dis)OI->AssignRedPulse();
					};
				};
			};
			break;
		};
	};
//	StopPF(30);
	//pages icons
	AblPanel.Page=0;
	if(NINF){
		OBJ=Group[MList[0].Last];
		if(OBJ&&OBJ->Ready){
			NewMonster* NM=OBJ->newMons;
			if(NM->Page1Icon!=0xFFFF||NM->Page2Icon!=0xFFFF||NM->Page3Icon!=0xFFFF){
				if(NM->Page1Icon!=0xFFFF){
					OneIcon* OI=AblPanel.AddIconFixed(0,NM->Page1Icon,0);
					if(NM->CurrPage==0)OI->AssignIcon(0,NM->Page1IconON);
					OI->AssignLeft(&SelPage1,int(NM));
					if(NM->Page1Hint)OI->CreateHint(NM->Page1Hint);
					else OI->CreateHint("");
					PAGE1PRESENT=1;
				};
				if(NM->Page2Icon!=0xFFFF){
					OneIcon* OI=AblPanel.AddIconFixed(0,NM->Page2Icon,12);
					if(NM->CurrPage==1)OI->AssignIcon(0,NM->Page2IconON);
					OI->AssignLeft(&SelPage2,int(NM));
					if(NM->Page2Hint)OI->CreateHint(NM->Page2Hint);
					else OI->CreateHint("");
					PAGE2PRESENT=1;
				};
				if(NM->Page3Icon!=0xFFFF){
					OneIcon* OI=AblPanel.AddIconFixed(0,NM->Page3Icon,24);
					if(NM->CurrPage==2)OI->AssignIcon(0,NM->Page3IconON);
					OI->AssignLeft(&SelPage3,int(NM));
					if(NM->Page3Hint)OI->CreateHint(NM->Page3Hint);
					else OI->CreateHint("");
					PAGE3PRESENT=1;
				};
			};
		};
	};
    //AblPanel.DrawIconSet(AblX+3,AblY+3,AblNx,AblNy,0);
	StopPF(0);
};
void PropCopy(){
	int AY=NABL>>2;
	if(NABL&3)AY++;
	int PY=NINF>>2;
	if(NINF&3)PY++;
	if(AY>8)AY=0;
	if(PY>8)PY=0;
	AY=4;
	PY=2;
	if(PY)ScrCopy(PrpX,PrpY,PrpNx*(IconLx+AddIconLx),PY*(IconLy+AddIconLy));
	if(AY)ScrCopy(AblX,AblY,AblNx*(IconLx+AddIconLx),AY*(IconLy+AddIconLy));
};
//-------------------MESSAGES-------------------//
void LoadConstStr();
lpCHAR* GMessIDS=NULL;
lpCHAR* GMessage=NULL;
int NMess=0;
int MaxMess=0;
void ClearMessages(){
	for(int i=0;i<NMess;i++){
		free(GMessIDS[i]);
		free(GMessage[i]);
	};
	if(GMessIDS)free(GMessIDS);
	if(GMessage)free(GMessage);
};
void DosToWin(char*);
void normstr(char* str);
extern bool ProtectionMode;
void LoadMessagesFromFile(char* Name){
    if (!strstr(Name, ".dat"))
    {
        ProtectionMode = 1;
    }
    GFILE* f = Gopen(Name, "r");
    ProtectionMode = 0;
    if (!f)
    {
        return;
    }

    int z, z1;
    char IDN[128];
    char STR[4096];
    do
    {
        z = Gscanf(f, "%s", IDN);
        if (z == 1)
        {
            if (NMess >= MaxMess)
            {
                MaxMess += 256;
                GMessIDS = (lpCHAR*)realloc(GMessIDS, MaxMess * 4);
                GMessage = (lpCHAR*)realloc(GMessage, MaxMess * 4);
            }

            Ggetch(f);

            STR[0] = 0;
            int cc = 0;
            z1 = 0;
            int nn = 0;
            while (!(cc == 0x0A || cc == EOF))
            {
                cc = Ggetch(f);
                if (!(cc == 0x0A || cc == EOF))
                {
                    STR[nn] = cc;
                    nn++;
                }
            }
            STR[nn] = 0;

            GMessIDS[NMess] = new char[strlen(IDN) + 1];
            GMessage[NMess] = new char[strlen(STR) + 1];

            strcpy(GMessIDS[NMess], IDN);
            strcpy(GMessage[NMess], STR);

            NMess++;
        }
    } while (z == 1);

    Gclose(f);
};
void LoadMessages(){
	LoadMessagesFromFile("Comment.txt");
	TGSCFindInfo* FD=GSFILES.gFindFirst("Missions\\miss_*.txt");
	if(FD){
		char cc[128];
		sprintf_s( cc,sizeof(cc),"Missions\\%s",FD->m_FileName);
		LoadMessagesFromFile(cc);
		bool good=false;
		do{
			good=GSFILES.gFindNext(FD);
			if(good){
				sprintf_s( cc,sizeof(cc),"Missions\\%s",FD->m_FileName);
				LoadMessagesFromFile(cc);
			};
		}while(good);
	};
	FD=GSFILES.gFindFirst("Text\\*.txt");
	if(FD){
		char cc[128];
		sprintf_s( cc,sizeof(cc),"Text\\%s",FD->m_FileName);
		LoadMessagesFromFile(cc);
		bool good=false;
		do{
			good=GSFILES.gFindNext(FD);
			if(good){
				sprintf_s( cc,sizeof(cc),"Text\\%s",FD->m_FileName);
				LoadMessagesFromFile(cc);
			};
		}while(good);
	};
	LoadMessagesFromFile("moretx.dat");
	LoadConstStr();
};
CEXPORT
char* GetTextByID(char* ID){
	for(int i=0;i<NMess;i++)if(!strcmp(GMessIDS[i],ID))return GMessage[i];
	return ID;
};
int GetTextIDByID(char* ID){
	for(int i=0;i<NMess;i++)if(!strcmp(GMessIDS[i],ID))return i;
	return -1;
};
