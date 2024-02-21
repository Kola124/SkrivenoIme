/*				  ������ �������� ��������
 *			����������� �������������� ��������.*/
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mgraph.h"
#include "mouse.h"
#include "menu.h"
#include "MapDiscr.h"
#include "walls.h"
#include "Nucl.h"
#include <math.h>
#include "GSound.h"
#include "3DGraph.h"
#include "3DMapEd.h"
#include "MapSprites.h"
#include "NewMon.h"
#include "assert.h"
#include "GP_Draw.h"
#include "ZBuffer.h"
#include "3DBars.h"
#define MaxExpl 8192
#define ExMask (MaxExpl-1)
#define WEPSH 14
void PlayAnimation(NewAnimation* NA,int Frame,int x,int y);
extern byte AlphaR[65536];
extern byte AlphaW[65536];
void ShowRLCItemGrad(int x,int y,lpRLCTable lprt,int n,byte* Pal);
extern word FlyMops[256][256];
int NUCLUSE[4];
bool EUsage[MaxExpl];
word LastAnmIndex;
AnmObject* GAnm[MaxExpl];
short TSin[257];
short TCos[257];
short TAtg[257];
word ID_FIRE;
word ID_FIREX;
word ID_FIRE1;
word ID_MAGW;
word ID_EXPL1;
word ID_EXPL2;
word ID_GLASSFLY;
word ID_GLASSBROKEN;
word ID_FLYFIRE;
word ID_MAGEXP;
word ID_FOG;
word ID_FOG1;
NewAnimation** FiresAnm[2]={NULL,NULL};
NewAnimation** PreFires[2]={NULL,NULL};
NewAnimation** PostFires[2]={NULL,NULL};
int            NFiresAnm[2];
void FreeFiresAnm(){
	for(int i=0;i<2;i++){
		if(FiresAnm[i])free(FiresAnm[i]);
		if(PreFires[i])free(PreFires[i]);
		if(PostFires[i])free(PostFires[i]);
	};
};
int  NWeaponIDS;
char* WeaponIDS[32];
int GetWeaponType(char* Name){
	for(int i=0;i<NWeaponIDS;i++){
		if(!strcmp(WeaponIDS[i],Name))return i;
	};
	return -1;
};
int mul3(int);
int nEused;
word LastReq;
short randoma[8192];
word rpos;
void InitExplosions(){
	for(int i=0;i<MaxExpl;i++)GAnm[i]=new AnmObject;
	for(int i=0;i<257;i++){
		TSin[i]=short(256*sin(2*3.1415927*i/256));
		TCos[i]=short(256*cos(2*3.1415927*i/256));
		TAtg[i]=short(128*atan2(i,256)/3.1415927);
	};
	memset(&EUsage,0,MaxExpl);
	LastReq=0;
	nEused=0;
	ResFile rf=RReset("random.lst");
	RBlockRead(rf,randoma,16384);
	RClose(rf);
//	for(int i=0;i<8192;i++){
//		randoma[i]=rand();
//	};
	rpos=0;
	//ResFile rf=RRewrite("random.lst");
	//RBlockWrite(rf,randoma,16384);
	//RClose(rf);

};
//short rando(){
//	rpos++;
//	rpos&=8191;
//	return randoma[rpos];
//};
//void addrand(int i){;
//	rpos+=i;
//	rpos&=8191;
//};
void CloseExu(int i){
	if(EUsage[i]){
		EUsage[i]=false;
		nEused--;
	};
};
void CloseExplosions(){
	for(int i=0;i<MaxExpl;i++)if(GAnm[i]){
		if(GAnm[i])delete(GAnm[i]);
		GAnm[i]=NULL;
	};
};
bool Create3DAnmObject(Weapon* Weap,int xs,int ys,int zs,
					                int xd,int yd,int zd,
									OneObject* OB);
extern int ExplMedia;
extern byte TexMedia[256];
extern byte* WaterDeep;
char WpnChar[228];
char* AOChar(AnmObject* AO){
	return WpnChar;
	int Index=div(int(AO)-int(GAnm),sizeof AnmObject).quot;
	if(AO->Sender){
		sprintf(WpnChar,"ANM:%d,Weap=%d,Sender=%d,x=%d,y=%d,z=%d,Frame=%d",Index,AO->Weap->MyIndex,
			AO->Sender->Index,AO->x,AO->y,AO->z,AO->Frame);
	}else{
		sprintf(WpnChar,"ANM:%d,Weap=%d,Sender=??,x=%d,y=%d,z=%d,Frame=%d",Index,AO->Weap->MyIndex,
			AO->x,AO->y,AO->z,AO->Frame);
	};
	return WpnChar;
};
bool Create3DAnmObjectEX(Weapon* Weap,int xs,int ys,int zs1,
					                int xd,int yd,int zd,
									OneObject* OB,byte AttType,word DestObj,
									int vx,int vy,int vz,int GRDZ);
int DetonationX=-1;
int DetonationY=-1;
int DetonationForce=-1;
void ExplodeAnmObject(AnmObject* AO,bool Landing){
	addrand(AO->ASerial);
	addrand(AO->x);
	addrand(AO->y);
	Weapon* Weap=AO->Weap;
	if(Weap->DetonationForce){
		DetonationForce=Weap->DetonationForce;
		DetonationX=AO->x>>WEPSH;
		DetonationY=AO->y>>WEPSH;
	};
	if(AO->Damage){
		int Damage=AO->Damage;
		if(Weap->FullParent)Damage=Weap->Damage;
		if(AO->Weap->Radius){
			int Damage=AO->Damage;
			if(AO->NTimes>0){
				Damage-=int(Weap->Times-AO->NTimes)*Weap->DamageDec/100;
				if(Damage<=0)Damage=1;
			};
			MakeRoundDamage(AO->x>>(WEPSH-4),AO->y>>(WEPSH-4),
			AO->Weap->Radius,Damage,AO->Sender,AO->AttType);
		};
		if(AO->DestObj!=0xFFFF){
			OneObject* OB=Group[AO->DestObj];
			if(OB&&OB->Serial==AO->DestSN){
				ExplMedia=OB->newMons->ExplosionMedia;
				addrand(7);
				addrand(ExplMedia);
				addrand(AO->Damage);
				addrand(OB->Life);
				addrand(AO->AttType);
				OB->MakeDamage(0,AO->Damage,AO->Sender,AO->AttType);
			};
		};
	};
	if(ExplMedia==-1){
		//water analysing
		int wx=AO->x>>(WEPSH+5);
		int wy=((AO->y>>(WEPSH+1))-GetHeight(AO->x>>WEPSH,AO->y>>WEPSH))>>4;
		if(wx>=1&&wy>=1&&wx<(MaxWX)-1&&wy<(MAPSY>>1)-1){
			int ofsw=wx+wy*(MaxWX);
			int z1=(int(WaterDeep[ofsw])+int(WaterDeep[ofsw+1])+int(WaterDeep[ofsw+(MaxWX)])+int(WaterDeep[ofsw+(MaxWX)+1]))>>2;
			if(z1>128)ExplMedia=1;
		};
		if(ExplMedia==-1){
			//texture analysing
			int tx=AO->x>>WEPSH;
			tx=div(tx+TriUnit,TriUnit+TriUnit).quot;
			int ty=AO->y>>WEPSH;
			if(tx&1)ty=div(ty,TriUnit+TriUnit).quot;
			else ty=div(ty+TriUnit,TriUnit+TriUnit).quot;
			if(tx<0)tx=0;
			if(ty<0)ty=0;
			if(tx>=msx)tx=msx-1;
			if(ty>=msy)ty=msy-1;
			ExplMedia=TexMedia[TexMap[tx+ty*(MaxTH+1)]];
		};
	};
	ChildWeapon* CWP=NULL;
	int cwtp=-1;
	if(ExplMedia!=-1){
		for(int j=0;j<Weap->NCustomEx;j++){
			ChildWeapon* CW=Weap->CustomEx+j;
			if(CW->Type==ExplMedia){
				CWP=CW;
				cwtp=j;
			};
		};
		if(!CWP)CWP=&Weap->Default;
	};

	if(CWP){
		int rnd=rando();
		int NChild=CWP->MinChild+(((CWP->MaxChild-CWP->MinChild)*rnd)>>15);
		//if(Weap->FullParent){
		for(int i=0;i<NChild;i++){
			bool ok=Landing;
			Weapon* BWpn=NULL;
			int natm=20;
			do{
				BWpn=CWP->Child[(rando()*CWP->NChildWeapon)>>15];
				if(BWpn->Propagation!=7)ok=1;
				natm--;
			}while(natm&&!ok);
			addrand(79);
			if(BWpn)Create3DAnmObjectEX(BWpn,AO->x>>WEPSH,AO->y>>WEPSH,AO->z>>WEPSH,
						AO->xd>>WEPSH,AO->yd>>WEPSH,AO->zd>>WEPSH,AO->Sender,AO->AttType,AO->DestObj,AO->vx,AO->vy,AO->vz,AO->GraphDZ);
		};
	};
	/*
	}else{
		for(int i=0;i<NChild;i++){
			Weapon* BWpn=CWP->Child[(rando()*CWP->NChildWeapon)>>15];
			Create3DAnmObject(BWpn,AO->x>>WEPSH,AO->y>>WEPSH,AO->z>>WEPSH,
				AO->xd>>WEPSH,AO->yd>>WEPSH,AO->zd>>WEPSH,NULL);
		};
	};
	*/
	if(Weap->NSyncWeapon&&!(ExplMedia!=-1&&CWP!=&Weap->Default)){
		for(int i=0;i<Weap->NSyncWeapon;i++){
			Weapon* BWpn=Weap->SyncWeapon[i];
			addrand(78);
			Create3DAnmObjectEX(BWpn,AO->x>>WEPSH,AO->y>>WEPSH,AO->z>>WEPSH,
				AO->xd>>WEPSH,AO->yd>>WEPSH,AO->zd>>WEPSH,AO->Sender,AO->AttType,AO->DestObj,AO->vx,AO->vy,AO->vz,AO->GraphDZ);
		};
	};
	DetonationForce=-1;
	DetonationX=-1;
	DetonationY=-1;
};
void ExplodeAnmObject(AnmObject* AO){
	ExplodeAnmObject(AO,0);
};
int ConvScrY(int x,int y);
void ShowRLCItemDark(int x,int y,lpRLCTable lprt,int n);
extern bool NOPAUSE;
void MiniShowExpl();
void ShowExpl(){
	if(LMode){
		MiniShowExpl();
		return;
	};
	int ScrX=mapx<<5;
	int ScrY=mul3(mapy<<5)>>2;
	int ScrX1=(mapx+smaplx)<<5;
	int ScrY1=mul3((mapy+smaply)<<5)>>2;
	for(int i=0;i<MaxExpl;i++)if(EUsage[i]){
		if(EUsage[i]){
			AnmObject* AO=GAnm[i];
			Weapon* Weap=AO->Weap;
			int xs=(AO->x>>WEPSH);
			int ys0=mul3(AO->y>>4)>>(WEPSH-2);
			int ys=ys0-(AO->z>>WEPSH);
			int DZ=AO->GraphDZ;
			int xs1=xs;
			xs=xs1;
			NewAnimation* NANI=Weap->NewAnm;
			//Visualization
			if(xs>=ScrX&&ys>=ScrY&&xs<=ScrX1&&ys<=ScrY1){
				//determining the direction
				if(AO->Frame>NANI->NFrames-1)AO->Frame=NANI->NFrames-1;
				NewFrame* NF=&NANI->Frames[AO->Frame];
				PlayAnimation(NANI,AO->Frame,xs,ys);
				int NDir=((NANI->Rotations-1)<<1);
				int spr;
				xs-=ScrX-smapx;
				ys-=ScrY-smapy;
				ys0-=ScrY-smapy;
				int oxs=xs;
				//int oys=ys;
				if(NDir){
					double angl;
					bool bdir;
					if(!(AO->vx||AO->vy)){
						angl=atan2(AO->xd-AO->x,AO->yd-AO->y);
						bdir=AO->xd-AO->x>0;
					}else{
						angl=atan2(-AO->vx,AO->vy-Prop43(AO->vz));
						bdir=AO->vx>0;
					};
					//angl+=(3.14152927/NDir);
					if(angl>=3.1415297)angl-=3.14152927*2;
					if(angl<0)angl=-angl;
					spr=angl*double(NDir)/3.14152927;
					if(spr>=NDir)spr=NDir-1;
					spr=(spr+1)>>1;
					if(bdir){
						spr+=4096;
						xs-=NF->dx;
					}else{
						xs+=NF->dx;
					};
				}else{
					spr=0;
					xs+=NF->dx;
				};
				int zz=GetHeight(AO->x>>WEPSH,AO->y>>WEPSH);
				int zz1=(AO->z>>WEPSH)-zz;
				ys+=NF->dy;
				spr+=NF->SpriteID*NANI->Rotations;
				if(Weap->HiLayer)ys0+=300;
				//it is visible!
				switch(Weap->Transparency){
				case 1://DARK
					//GPS.ShowGPDark(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0);
					AddOptPoint(ZBF_NORMAL,oxs,ys0+DZ,xs,ys,NULL,NF->FileID,spr,AV_DARK|AV_GRADIENT);
					break;
				case 2://WHITE
					//GPS.ShowGPMutno(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0);
					AddOptPoint(ZBF_NORMAL,oxs,ys0+DZ,xs,ys,NULL,NF->FileID,spr,AV_WHITE|AV_GRADIENT);
					break;
				case 3://RED
					//GPS.ShowGPFired(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0);
					//break;
				case 4://BRIGHT
				case 5://YELLOW
				case 6://ALPHAR
					//GPS.ShowGPGrad(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0,AlphaR);
					//break;
				case 7://ALPHAW
					//GPS.ShowGPGrad(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0,AlphaW);
					//break;
				default:
					//GPS.ShowGP(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0);
					AddOptPoint(ZBF_NORMAL,oxs,ys0+DZ,xs,ys,NULL,NF->FileID,spr,0);
					break;
				};
			};
		};
	};
};
void ProcessFlyBodies();
int TreeHeight=128;
void ProcessExpl(){
	if(!NOPAUSE)return;
	ProcessFlyBodies();
	//int ScrX=mapx<<5;
	//int ScrY=mul3(mapy<<5)>>2;
	//int ScrX1=(mapx+smaplx)<<5;
	//int ScrY1=mul3((mapy+smaply)<<5)>>2;
	//if(!NOPAUSE)return;
	for(int i=0;i<MaxExpl;i++)if(EUsage[i]){
		if(EUsage[i]){
			AnmObject* AO=GAnm[i];
			Weapon* Weap=AO->Weap;
			NewAnimation* NANI=Weap->NewAnm;
			if(Weap->NTileWeapon){
				int tpp=Weap->TileProbability;
				int vdx=div(AO->vx,tpp).quot;
				int vdy=div(AO->vy,tpp).quot;
				int vdz=div(AO->vz,tpp).quot;
				int xxx=AO->x;
				int yyy=AO->y;
				int zzz=AO->z;
				for(int j=0;j<tpp;j++){
					Weapon* WP=Weap->TileWeapon[(rando()*Weap->NTileWeapon)>>15];
					addrand(77);
					Create3DAnmObject(WP,xxx>>WEPSH,yyy>>WEPSH,zzz>>WEPSH,
						AO->xd>>WEPSH,AO->yd>>WEPSH,AO->zd>>WEPSH,NULL);
					xxx-=vdx;
					yyy-=vdy;
					zzz-=vdz;
				};
			};
			AO->vz+=AO->az;
			AO->x+=AO->vx;
			AO->y+=AO->vy;
			AO->z+=AO->vz;
			addrand(AO->x+AO->y+AO->z+AO->vx+AO->vy+AO->vz);
			int dis=abs(AO->x-AO->xd)+abs(AO->y-AO->yd)+abs(AO->z-AO->zd);
			int wprp=Weap->Propagation;
			if((wprp==3||wprp==5)&&dis<65536*4){
				ExplodeAnmObject(AO,1);
				CloseExu(i);
				goto UUU2;
			}else{
				int ssx=AO->x>>WEPSH;
				int ssy=AO->y>>WEPSH;
				int zz=GetHeight(ssx,ssy);
				int zz0=zz;
				int wpt=Weap->Propagation;
				int BHi=0;
				if(wpt>=2&&wpt<=5){
					BHi=GetBar3DHeight(ssx,ssy);
					zz+=BHi;
				};
#ifdef COSSACKS2
				int DH=Weap->DamageHeight;
				int AOZ=(AO->z>>WEPSH);
				if(DH&&zz>AOZ-DH-TreeHeight){
#else
				if(Weap->DamageHeight&&zz>(AO->z>>WEPSH)-Weap->DamageHeight){
#endif
					//damage under spot
					if(AO->Damage){
						int Damage=AO->Damage;
						//if(Weap->FullParent)Damage=Weap->Damage;
						if(AO->NTimes>0){
							Damage-=int(Weap->Times-AO->NTimes)*Weap->DamageDec/100;
							if(Damage<=0)Damage=1;
						};
#ifdef COSSACKS2
						bool DetectSpriteInPoint(int x,int y);
						bool EXPL=0;
						
							if((zz>AOZ-DH&&MakeRoundDamage(AO->x>>(WEPSH-4),AO->y>>(WEPSH-4),
								AO->Weap->Radius,Damage,AO->Sender,AO->AttType))||
								(EXPL=DetectSpriteInPoint(AO->x>>(WEPSH),AO->y>>(WEPSH)))){
#else
						bool EXPL=0;
						if(MakeRoundDamage(AO->x>>(WEPSH-4),AO->y>>(WEPSH-4),
							AO->Weap->Radius,Damage,AO->Sender,AO->AttType)){
#endif
							if(Weap->Propagation==8||EXPL){
								ExplodeAnmObject(AO);
								CloseExu(i);
								goto UUU2;
							};
						};
						if(AO->DestObj!=0xFFFF){
							OneObject* OB=Group[AO->DestObj];
							if(OB&&OB->Serial==AO->DestSN){
								ExplMedia=OB->newMons->ExplosionMedia;
								if(Weap->DetonationForce){
									DetonationForce=Weap->DetonationForce;
									DetonationX=AO->x>>WEPSH;
									DetonationY=AO->y>>WEPSH;
								};
								OB->MakeDamage(0,AO->Damage,AO->Sender,AO->AttType);
								if(Weap->Propagation==8){
									ExplodeAnmObject(AO);
									CloseExu(i);
								};
								DetonationForce=-1;
								DetonationX=-1;
								DetonationY=-1;
							};
						};
					};
				};
				if(zz>(AO->z>>WEPSH)){
					//Collision with surface
					if(BHi){
						int IDI=GetBar3DOwner(ssx,ssy);
						if(AO->Sender&&IDI!=-1&&IDI==AO->Sender->Index){
							zz=zz0;
							BHi=0;
							goto UUU1;
						}else{
							OneObject* OB=Group[IDI];
							if(OB){
								AO->DestObj=IDI;
								AO->DestSN=OB->Serial;
							};
							ExplodeAnmObject(AO);
							CloseExu(i);
						};
					}else{
						ExplodeAnmObject(AO,1);
						CloseExu(i);
					};
				}else{
UUU1:/*
				if(FrmDec==2){
					if(AO->NTimes==1&&(AO->Frame>>SpeedSh)==(Weap->HotFrame>>SpeedSh)){
						ExplodeAnmObject(AO);
					};
				}else{
				*/
						if(AO->NTimes==1&&AO->Frame==Weap->HotFrame){
							ExplodeAnmObject(AO);
						};
					//};
					if(AO->Frame>=NANI->NFrames-FrmDec){
						if(AO->NTimes==1){
							CloseExu(i);
						}else{
							if(AO->NTimes>0)AO->NTimes--;
							AO->Frame=-1;
						};
					};
				};
			};
			AO->Frame++;//=FrmDec;
UUU2:;
		};
	};
};
void MoveAway(int x,int y);
bool TraceObjectsInLine(int xs,int ys,int zs,int* xD,int* yD,int* zD,int damage,OneObject* Sender,byte AttType);
bool Create3DAnmObjectEX(Weapon* Weap,int xs,int ys,int zs1,
					                int xd,int yd,int zd,
									OneObject* OB,byte AttType,word DestObj,
									int vx,int vy,int vz,int GRDZ){
	if(!Weap)return false;
	if(AttType>=NAttTypes)AttType=0;
	if(Weap->SoundID!=-1){
		AddEffect(xs,(ys>>1)-zs1,Weap->SoundID);
	};
	int zs;
	int hig=GetHeight(xs,ys);
	if(zs1<hig)zs=hig+1;
	else zs=zs1;
	short i=LastReq;
	while(EUsage[i]&&i<MaxExpl)i++;	
	if(i>=MaxExpl){
		i=0;
		while(EUsage[i]&&i<LastReq)i++;
	};
	LastAnmIndex=-1;
	if(EUsage[i])return 0;
	LastAnmIndex=i;
	LastReq=(i+1)&ExMask;
	EUsage[i]=true;
	nEused++;
	AnmObject* AO=GAnm[i];
	AO->ASerial=rando();
	AO->AttType=AttType;
	AO->GraphDZ=GRDZ;
	addrand(AttType);
	if(OB&&Weap->FullParent){
		int nn=OB->newMons->MaxInside;
		if(OB->newMons->ShotPtZ&&nn){
			int N=OB->NInside;
			int ns=0;
			for(int i=0;i<N;i++){
				word MID=OB->Inside[i];
				if(MID!=0xFFFF){
					OneObject* OBX=Group[MID];
					if(OBX&&!OBX->Sdoxlo){
						byte use=OBX->newMons->Usage;
						if(use==StrelokID)ns++;
					};
				};
			};
			int min=OB->newMons->MinDamage[AttType];
			int max=OB->Ref.General->MoreCharacter->MaxDamage[AttType];
			AO->Damage=min+(((max-min)*ns)/nn);
		}else AO->Damage=OB->Ref.General->MoreCharacter->MaxDamage[AttType];
	}else AO->Damage=Weap->Damage;
	AO->Weap=Weap;
	AO->Sender=OB;
	AO->DestObj=0xFFFF;
	AO->DestSN=0xFFFF;
	if(DestObj!=0xFFFF&&Weap->FullParent){
		OneObject* OB=Group[DestObj];
		if(OB){
			AO->DestObj=DestObj;
			AO->DestSN=OB->Serial;
		};
	};
	AO->az=-Weap->Gravity*2000*FrmDec*FrmDec;
	int SPEED=Weap->Speed<<SpeedSh;
	int time,dist;
	switch(Weap->Propagation){
	case 0://STAND
		AO->x=xs<<WEPSH;
		AO->y=ys<<WEPSH;
		AO->z=zs<<WEPSH;
		AO->vx=0;
		AO->vy=0;
		AO->vz=0;
		AO->NTimes=Weap->Times;
		if(AO->Weap->HotFrame==0xFF){
			AO->xd=xd<<WEPSH;
			AO->yd=yd<<WEPSH;
			AO->zd=zd<<WEPSH;
			AO->Frame=0;
			ExplodeAnmObject(AO);
		};
		break;
	case 1://SLIGHTUP
		AO->x=xs<<WEPSH;
		AO->y=ys<<WEPSH;
		AO->z=zs<<WEPSH;
		AO->vx=0;
		AO->vy=0;
		AO->vz=SPEED<<WEPSH;
		AO->NTimes=Weap->Times;
		if(AO->Weap->HotFrame==0xFF){
			AO->xd=xd<<WEPSH;
			AO->yd=yd<<WEPSH;
			AO->zd=zd<<WEPSH;
			AO->Frame=0;
			ExplodeAnmObject(AO);
		};
		break;
	case 7://REFLECT
		{
			vx>>=16;
			vy>>=16;
			vz>>=16;
			int angl=0;
			if(vx||vy||vz){
				int zL=GetHeight(xs-32,ys);
				int zR=GetHeight(xs+32,ys);
				int zU=GetHeight(xs,ys-32);
				int zD=GetHeight(xs,ys+32);
				int nx=zL-zR;
				int ny=zD-zU;
				int nz=32;
				int norma=sqrt(nx*nx+ny*ny+nz*nz);
				int vn=2*(vx*nx+vy*ny+vz*nz)/norma;
				int vv=sqrt(vx*vx+vy*vy+vz*vz);
				vx-=(vn*nx)/norma;
				vy-=(vn*ny)/norma;
				vz-=(vn*nz)/norma;
				angl=(vn<<5)/vv;
			};
			vx=(vx*Weap->Speed)<<10;
			vy=(vy*Weap->Speed)<<10;
			vz=(vz*Weap->Speed)<<10;
			AO->z=zs<<WEPSH;
			AO->x=xs<<WEPSH;
			AO->y=ys<<WEPSH;
			AO->vx=vx;
			AO->vy=vy;
			AO->vz=vz;
			AO->NTimes=Weap->Times;
			AO->xd=0;
			AO->yd=0;
			AO->zd=0;
			if(abs(angl)>26){
				AO->xd=xd<<WEPSH;
				AO->yd=yd<<WEPSH;
				AO->zd=zd<<WEPSH;
				AO->Frame=0;
				ExplodeAnmObject(AO);
				CloseExu(i);
			};
		};
		break;
	case 6:
	case 2://RANDOM
		AO->x=xs<<WEPSH;
		AO->y=ys<<WEPSH;
		AO->z=zs<<WEPSH;
		AO->vx=((rando()-16384)*SPEED);
		AO->vy=((rando()-16384)*SPEED);
		AO->vz=((rando())*SPEED)>>1;
		AO->NTimes=Weap->Times;
		break;
	case 8:
		{
			AO->x=xs<<WEPSH;
			AO->y=ys<<WEPSH;
			AO->z=zs<<WEPSH;
			dist=sqrt((xs-xd)*(xs-xd)+(ys-yd)*(ys-yd));
			time=div(dist,SPEED).quot;
			AO->NTimes=Weap->Times;
			if(!time)time=1;
			AO->vx=div((xd-xs)<<WEPSH,time+1).quot;
			AO->vy=div((yd-ys)<<WEPSH,time+1).quot;
			AO->vz=div((zd-zs)<<WEPSH,time+1).quot-((AO->az*(time+2))>>1);
			int vx=AO->vx>>8;
			int vy=AO->vy>>8;
			int vz=AO->vz>>8;
			int v=sqrt(vx*vx+vy*vy+vz*vz);
			int d=((int(rando()>>7)-128)*v)>>7;
			d=(d*Weap->RandomAngle)/100;
			AO->vx+=d<<8;
			d=((int(rando()>>7)-128)*v)>>7;
			d=(d*Weap->RandomAngle)/100;
			AO->vy+=d<<8;
			d=((int(rando()>>7)-128)*v)>>7;
			d=(d*Weap->RandomAngle)/100;
			AO->vz+=d<<4;
		};
		break;
	case 3://FLY
		AO->x=xs<<WEPSH;
		AO->y=ys<<WEPSH;
		AO->z=zs<<WEPSH;
		dist=sqrt((xs-xd)*(xs-xd)+(ys-yd)*(ys-yd));
		time=div(dist,SPEED).quot;
		AO->NTimes=Weap->Times;
		if(time){
			AO->vx=div((xd-xs)<<WEPSH,time+1).quot;
			AO->vy=div((yd-ys)<<WEPSH,time+1).quot;
			AO->vz=div((zd-zs)<<WEPSH,time+1).quot-((AO->az*(time+2))>>1);
		}else{
#ifdef CONQUEST
			if(!TraceObjectsInLine(xs,ys,zs,&xd,&yd,&zd,AO->Damage,AO->Sender,AO->AttType)){
				if(DestObj!=0xFFFF&&AO->Sender){
					OneObject* OOO=Group[DestObj];
					if(OOO&&OOO->NewBuilding&&!OOO->Sdoxlo){
						int RR=Norma(xd-xs,yd-ys)-OOO->newMons->AddShotRadius;
						if(RR<0)RR=0;
#ifdef COSSACKS2
						int GetDamFall(int x,int x0,int Dam);
						int damage=GetDamFall(RR,AO->Sender->newMons->DamageDecr[AO->AttType],AO->Damage);
#else
						int damage=int(float(AO->Damage)*exp(-0.693147*float(RR)/float(AO->Sender->newMons->DamageDecr[AO->AttType])));
#endif
						if(Weap->DetonationForce){
							DetonationForce=Weap->DetonationForce;
							DetonationX=AO->x>>WEPSH;
							DetonationY=AO->y>>WEPSH;
						};
						OOO->MakeDamage(damage,damage,AO->Sender,AO->AttType);
						DetonationForce=-1;
						DetonationX=-1;
						DetonationY=-1;
					};
				};
			};
			//ExplodeAnmObject(AO);
			AO->vx=0;
			AO->vy=0;
			AO->vz=0;
			AO->x=xd<<WEPSH;
			AO->y=yd<<WEPSH;
			AO->z=zd<<WEPSH;
			AO->xd=xd<<WEPSH;
			AO->yd=yd<<WEPSH;
			AO->zd=zd<<WEPSH;
			AO->Frame=0;
			CloseExu(i);
#else
			AO->vx=0;
			AO->vy=0;
			AO->vz=0;
			AO->x=xd<<WEPSH;
			AO->y=yd<<WEPSH;
			AO->z=zd<<WEPSH;
			AO->xd=xd<<WEPSH;
			AO->yd=yd<<WEPSH;
			AO->zd=zd<<WEPSH;
			AO->Frame=0;
			ExplodeAnmObject(AO);
			CloseExu(i);
#endif
			return true;
		};
		break;
	case 4://IMMEDIATE
		AO->x=xd<<WEPSH;
		AO->y=yd<<WEPSH;
		AO->z=zd<<WEPSH;
		AO->vx=0;
		AO->vy=0;
		AO->vz=0;
		AO->NTimes=Weap->Times;
		break;
	case 5://ANGLE
		{
			int rxy=int(sqrt((xd-xs)*(xd-xs)+(yd-ys)*(yd-ys)));
			int t=zd-zs-int(double(rxy)*tan(Weap->Speed*3.1415/180));
			if(t>=0||!AO->az){
				EUsage[i]=false;
				return false;
			};
			t=4*sqrt(double(t<<(WEPSH-3))/AO->az);
			AO->vx=div((xd-xs)<<WEPSH,t).quot;
			AO->vy=div((yd-ys)<<WEPSH,t).quot;
			int vxy=sqrt(double(AO->vx)*double(AO->vx)+double(AO->vy)*double(AO->vy));
			AO->vz=int(double(vxy)*tan(Weap->Speed*3.1415/180));
			AO->x=xs<<WEPSH;
			AO->y=ys<<WEPSH;
			AO->z=zs<<WEPSH;
			AO->NTimes=Weap->Times;
			//if(AO->Damage>200){
			//MoveAway(xd<<4,yd<<4);
			//};
		};
		break;
	default:
		assert(0);
	};
	AO->xd=xd<<WEPSH;
	AO->yd=yd<<WEPSH;
	AO->zd=zd<<WEPSH;
	AO->Frame=0;
	return true;
};
bool Create3DAnmObject(Weapon* Weap,int xs,int ys,int zs1,
					                int xd,int yd,int zd,
									OneObject* OB,byte AttType,word DestObj){
	return Create3DAnmObjectEX(Weap,xs,ys,zs1,xd,yd,zd,OB,AttType,DestObj,0,0,0,0);
};
bool Create3DAnmObject(Weapon* Weap,int xs,int ys,int zs1,
					                int xd,int yd,int zd,
									OneObject* OB){
	return Create3DAnmObjectEX(Weap,xs,ys,zs1,xd,yd,zd,OB,0,0xFFFF,0,0,0,0);
};
void ChildWeapon::InitChild(){
	MinChild=0;
	MaxChild=0;
	NChildWeapon=0;
	for(int i=0;i<4;i++){
		Child[i]=NULL;
	};
};
bool CheckWpn(Weapon* WP){
	int wpt=WP->Propagation;
	if(wpt>=3&&wpt<=5)return true;
	return false;
};
Weapon* ReturnFlyChild(Weapon* Weap){
	if(CheckWpn(Weap))return Weap;
	ChildWeapon* CWP=&Weap->Default;
	for(int i=0;i<CWP->NChildWeapon;i++){
		Weapon* CW=CWP->Child[i];
		if(CheckWpn(CW))return CW;
	};
	for(int i=0;i<Weap->NSyncWeapon;i++){
		Weapon* SW=Weap->SyncWeapon[i];
		if(CheckWpn(SW))return SW;
	};
	return NULL;
};
int CheckWpPoint(int x,int y,int z,word Index){
	int zg=GetBar3DHeight(x,y);
	int zL=GetUnitHeight(x,y);
	if(zg<20)zL-=45;
	if(zg+zL<=z)return -1;
	if(zg){
		word Own=GetBar3DOwner(x,y);
		if(Own!=Index)return Own;
		else if(zL<z)return -1;

	};
	return 0xFFFF;
};
int CheckWpLine(int xs,int ys,int zs,int xd,int yd,int zd,word Index){
	int v=Norma(xd-xs,yd-ys);
	if(!v)return -1;
	int N=(v>>4)+1;
	int vx=div((xd-xs)<<WEPSH,N).quot;
	int vy=div((yd-ys)<<WEPSH,N).quot;
	int vz=div((zd-zs)<<WEPSH,N).quot;
	xs<<=WEPSH;
	ys<<=WEPSH;
	zs<<=WEPSH;
	for(int i=0;i<N;i++){
		int res=CheckWpPoint(xs>>WEPSH,ys>>WEPSH,zs>>WEPSH,Index);
		if(res!=-1)return res;
		xs+=vx;
		ys+=vy;
		zs+=vz;
	};
	return -1;
};
bool ShotRecommended;
int PredictShot(Weapon* Weap,int xs,int ys,int zs,int xd,int yd,int zd,word Index){
	Weapon* WP=ReturnFlyChild(Weap);
	if(WP){
		int x,y,z,xf,yf,zf,vx,vy,vz,g;
		g=-WP->Gravity*2000*FrmDec*FrmDec;
		int SPEED=WP->Speed<<SpeedSh;
		x=xs<<WEPSH;
		y=ys<<WEPSH;
		z=zs<<WEPSH;
		xf=xd<<WEPSH;
		yf=yd<<WEPSH;
		zf=zd<<WEPSH;
		int N=0;
		int dist,time;
		bool NoCheck=1;
		switch(WP->Propagation){
		case 3://FLY
			dist=sqrt((xs-xd)*(xs-xd)+(ys-yd)*(ys-yd));
			time=div(dist,SPEED).quot;
			vx=div((xd-xs)<<WEPSH,time+1).quot;
			vy=div((yd-ys)<<WEPSH,time+1).quot;
			vz=div((zd-zs)<<WEPSH,time+1).quot-((g*(time+2))>>1);
			if(!time)NoCheck=0;
			break;
		case 4://IMMEDIATE
			vx=xf-x;
			vy=yf-y;
			vz=zf-z;
			NoCheck=0;
			break;
		case 5://ANGLE
			{
				int rxy=int(sqrt((xd-xs)*(xd-xs)+(yd-ys)*(yd-ys)));
				int t1=zd-zs-int(double(rxy)*tan(WP->Speed*3.1415/180));
				if(t1>=0)return 0xFFFE;
				int t=4*sqrt(double(t1<<(WEPSH-3))/g);
				vx=div((xd-xs)<<WEPSH,t).quot;
				vy=div((yd-ys)<<WEPSH,t).quot;
				int vxy=sqrt(double(vx)*double(vx)+double(vy)*double(vy));
				vz=int(double(vxy)*tan(WP->Speed*3.1415/180));
			};
			break;
		};
		if(NoCheck)return -1;
		//process checking
		if(abs(vx>>WEPSH)+abs(vy>>WEPSH)+abs(vz>>WEPSH)<200)ShotRecommended=true;
		else ShotRecommended=false;
		int x0=x;
		int y0=y;
		int z0=z;
		int dst=0;
		do{
			vz+=g;
			int x1=x0+vx;
			int y1=y0+vy;
			int z1=z0+vz;
			int LRes=CheckWpLine(x0>>WEPSH,y0>>WEPSH,z0>>WEPSH,x1>>WEPSH,y1>>WEPSH,z1>>WEPSH,Index);
			if(LRes!=-1)return LRes;
			x0=x1;y0=y1;z0=z1;
			dst=Norma(x0-xf,y0-yf);
		}while(dst>(4<<WEPSH));
	};
	return -1;
};
bool TraceObjectsInLine(int xs,int ys,int zs,int* xD,int* yD,int* zD,int damage,OneObject* Sender,byte AttType){
	bool Friend=0;
	if(Sender&&Sender->newMons->FriendlyFire&&!Sender->FriendlyFire)Friend=1;
	int cx=-1;
	int cy=-1;

	int dx=*xD-xs;
	int dy=*yD-ys;
	int dz=*zD-zs;

	int Len=sqrt(dx*dx+dy*dy+dz*dz);
	if(!Len)return false;
	int N=(Len>>4)+1;
	int N2=N+N;
	int MinR=10000;
	word BestID=0xFFFF;
	int SMID=0xFFFF;
	byte MASK=0;
	if(Sender){
		SMID=Sender->Index;
		MASK=Sender->NMask;
	};
	int NOID=0xFFFF;
	if(Sender)NOID=Sender->Index;
	for(int i=0;i<N2&&MinR;i++){
		int xx=(dx*i)/N;
		int yy=(dy*i)/N;
		int zz=zs+(dz*i)/N;
		int H=GetBar3DHeight(xs+xx,ys+yy);
		int H0=GetHeight(xs+xx,ys+yy);
#ifdef COSSACKS2
		bool DetectSpriteInPoint(int x,int y);
		if(i*Len/N>180&&DetectSpriteInPoint(xs+xx,ys+yy))return false;
#endif //COSSACKS2
		if(H+H0>zz&&H){
			int ID=GetBar3DOwner(xs+xx,ys+yy);
			if(ID>=0&&ID<0xFFFF&&ID!=NOID){
				BestID=ID;
				MinR=0;
			};
		};
		int ccx=(xs+xx)>>7;
		int ccy=(ys+yy)>>7;
		if(cx!=ccx||cy!=ccy){
			cx=ccx;
			cy=ccy;
			if(cx>=0&&cy>=0&&cx<VAL_MAXCX-1&&cy<VAL_MAXCX-1){
				int cell=1+VAL_MAXCX+(cy<<VAL_SHFCX)+cx;
				int NMon=MCount[cell];
				if(NMon){
					int ofs1=cell<<SHFCELL;
					word MID;
					for(int i=0;i<NMon;i++){
						MID=GetNMSL(ofs1+i);
						if(MID!=0xFFFF&&MID!=SMID){
							OneObject* OB=Group[MID];
							if(OB&&!OB->Sdoxlo){
								int ux=OB->RealX>>4;
								int uy=OB->RealY>>4;
								int R=Norma(ux-xs,uy-ys);
								int dz=zz-OB->RZ;
								int minR=60;
								bool MyUnit=0;
								if(OB->NMask&MASK){
									minR=150;
									MyUnit=1;
								};
								if(dz>0&&dz<90&&R>minR&&R<MinR){
									int r=abs((ux-xs)*yy-(uy-ys)*xx)/Len;
									if(MyUnit&&R<250)r<<=1;
									if(r<OB->newMons->UnitRadius){
										BestID=MID;
										MinR=R;
									};
								};
							};
						};
					};
				};
			};
		};
	};
	if(BestID!=0xFFFF){
		OneObject* OB=Group[BestID];
		if(OB&&((OB->NewBuilding&&OB->NInside)||!OB->NewBuilding)){
			*xD=(OB->RealX>>4);
			*yD=(OB->RealY>>4);
			if(Sender&&AttType<NAttTypes){
				int RR=Norma(*xD-xs,*yD-ys);
#ifdef COSSACKS2
				int GetDamFall(int x,int x0,int Dam);
				damage=GetDamFall(RR,Sender->newMons->DamageDecr[AttType],damage);
#else
				damage=int(float(damage)*exp(-0.693147*float(RR)/float(Sender->newMons->DamageDecr[AttType])));
#endif
			};
			if(Friend){
				if(!(Sender->NMask&OB->NMask))OB->MakeDamage(damage,damage,Sender,AttType);
			}else{
				OB->MakeDamage(damage,damage,Sender,AttType);
			};
		};
		return true;
	};
	return false;
};
int CheckFriendlyUnitsInLine(int xs,int ys,int zs,int* xD,int* yD,int* zD,byte MASK){
	int cx=-1;
	int cy=-1;

	int dx=*xD-xs;
	int dy=*yD-ys;
	int dz=*zD-zs;

	int Len=sqrt(dx*dx+dy*dy+dz*dz);
	if(!Len)return -1;
	int N=(Len>>4)+1;
	int N2=N+N;
	int MinR=10000;
	word BestID=0xFFFF;
	int SMID=0xFFFF;
	for(int i=0;i<N2;i++){
		int xx=(dx*i)/N;
		int yy=(dy*i)/N;
		int zz=zs+(dz*i)/N;
		int ccx=(xs+xx)>>7;
		int ccy=(ys+yy)>>7;
		if(cx!=ccx||cy!=ccy){
			cx=ccx;
			cy=ccy;
			if(cx>=0&&cy>=0&&cx<VAL_MAXCX-1&&cy<VAL_MAXCX-1){
				int cell=1+VAL_MAXCX+(cy<<VAL_SHFCX)+cx;
				int NMon=MCount[cell];
				if(NMon){
					int ofs1=cell<<SHFCELL;
					word MID;
					for(int i=0;i<NMon;i++){
						MID=GetNMSL(ofs1+i);
						if(MID!=0xFFFF&&MID!=SMID){
							OneObject* OB=Group[MID];
							if(OB&&!OB->Sdoxlo){
								int ux=OB->RealX>>4;
								int uy=OB->RealY>>4;
								int R=Norma(ux-xs,uy-ys);
								int dz=zz-OB->RZ;
								int minR=60;
								bool MyUnit=0;
								if(OB->NMask&MASK){
									minR=150;
									MyUnit=1;
								};
								if(dz>0&&dz<90&&R>minR&&R<MinR){
									int r=abs((ux-xs)*yy-(uy-ys)*xx)/Len;
									r=r-(r>>2);
									if(MyUnit&&R<250)r<<=1;
									if(r<OB->newMons->UnitRadius){
										BestID=MID;
										MinR=R;
									};
								};
							};
						};
					};
				};
			};
		};
	};
	if(BestID!=0xFFFF){
		OneObject* OB=Group[BestID];
		if(OB->NMask&MASK)return BestID;
	};
	return -1;
};
int PredictShotEx(Weapon* Weap,int xs,int ys,int zs,int xd,int yd,int zd,word Index){
	OneObject* OBJ=Group[Index];
	if(OBJ->newMons->FriendlyFire&&!OBJ->FriendlyFire){
		int r=CheckFriendlyUnitsInLine(xs,ys,zs,&xd,&yd,&zd,OBJ->NMask);
		if(r==-1){
			return PredictShot(Weap,xs,ys,zs,xd,yd,zd,Index);
		}else return r;
	}else return PredictShot(Weap,xs,ys,zs,xd,yd,zd,Index);
};
void MiniShowExpl(){
	int ScrX=mapx<<5;
	int ScrY=mapy<<4;
	int ScrX1=(mapx+smaplx)<<5;
	int ScrY1=(mapy+smaply)<<4;
	for(int i=0;i<MaxExpl;i++)if(EUsage[i]){
		if(EUsage[i]){
			AnmObject* AO=GAnm[i];
			Weapon* Weap=AO->Weap;
			int xs=(AO->x>>WEPSH);
			int ys0=mul3(AO->y>>4)>>(WEPSH-2);
			int ys=ys0-(AO->z>>WEPSH);
			int DZ=AO->GraphDZ>>2;
			int xs1=xs;
			xs=xs1;
			NewAnimation* NANI=Weap->NewAnm;
			//Visualization
			if(xs>=ScrX&&ys>=ScrY&&xs<=ScrX1&&ys<=ScrY1){
				//determining the direction
				if(AO->Frame>NANI->NFrames-1)AO->Frame=NANI->NFrames-1;
				NewFrame* NF=&NANI->Frames[AO->Frame];
				PlayAnimation(NANI,AO->Frame,xs,ys);
				int NDir=((NANI->Rotations-1)<<1);
				int spr;
				xs-=ScrX-smapx;
				ys-=ScrY-smapy;
				ys0-=ScrY-smapy;
				int oxs=xs;
				//int oys=ys;
				if(NDir){
					double angl;
					bool bdir;
					if(!(AO->vx||AO->vy)){
						angl=atan2(AO->xd-AO->x,AO->yd-AO->y);
						bdir=AO->xd-AO->x>0;
					}else{
						angl=atan2(-AO->vx,AO->vy-Prop43(AO->vz));
						bdir=AO->vx>0;
					};
					//angl+=(3.14152927/NDir);
					if(angl>=3.1415297)angl-=3.14152927*2;
					if(angl<0)angl=-angl;
					spr=angl*double(NDir)/3.14152927;
					if(spr>=NDir)spr=NDir-1;
					spr=(spr+1)>>1;
					if(bdir){
						spr+=4096;
						xs-=NF->dx;
					}else{
						xs+=NF->dx;
					};
				}else{
					spr=0;
					xs+=NF->dx;
				};
				xs>>=2;
				ys>>=2;
				oxs>>=2;
				ys0>>=2;
				DZ>>=2;
				int zz=GetHeight(AO->x>>WEPSH,AO->y>>WEPSH);
				int zz1=((AO->z>>WEPSH)-zz)>>2;
				ys+=NF->dy>>2;
				spr+=NF->SpriteID*NANI->Rotations;
				if(Weap->HiLayer)ys0+=300;
				//it is visible!
				switch(Weap->Transparency){
				case 1://DARK
					//GPS.ShowGPDark(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0);
					AddOptPoint(ZBF_NORMAL,oxs,ys0+DZ,xs,ys,NULL,NF->FileID,spr,AV_DARK|AV_GRADIENT);
					break;
				case 2://WHITE
					if(GP_L_IDXS[NF->FileID]){
						GPS.SetWhiteFont(GP_L_IDXS[NF->FileID]);
					};
					//GPS.ShowGPMutno(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0);
					AddOptPoint(ZBF_NORMAL,oxs,ys0+DZ,xs,ys,NULL,NF->FileID,spr,AV_WHITE|AV_GRADIENT);
					break;
				case 3://RED
					//GPS.ShowGPFired(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0);
					//break;
				case 4://BRIGHT
				case 5://YELLOW
				case 6://ALPHAR
					//GPS.ShowGPGrad(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0,AlphaR);
					//break;
				case 7://ALPHAW
					//GPS.ShowGPGrad(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0,AlphaW);
					//break;
					if(GP_L_IDXS[NF->FileID]){
						GPS.SetWhiteFont(GP_L_IDXS[NF->FileID]);
					};
				default:
					//GPS.ShowGP(smapx+xs-ScrX,smapy+ys-ScrY,NF->FileID,spr,0);
					AddOptPoint(ZBF_NORMAL,oxs,ys0+DZ,xs,ys,NULL,NF->FileID,spr,0);
					break;
				};
			};
		};
	};
};
//-----------------------FLYING BODY PHYSICS------------------//

struct OneFlyBody{
	word Index;
	word Serial;
	byte Stage;
	int X,Y,Z,Vx,Vy,Vz;
};
#define MaxFlyBod 128
OneFlyBody FBODS[MaxFlyBod];
int NFlyBodies=0;
void AddFlyBody(word Index,int vx,int vy,int vz){
	if(NFlyBodies<MaxFlyBod){
		for(int i=0;i<NFlyBodies;i++)if(FBODS[i].Index==Index)return;
		OneObject* OB=Group[Index];
		if(OB){
			FBODS[NFlyBodies].Index=Index;
			FBODS[NFlyBodies].Serial=OB->Serial;
			FBODS[NFlyBodies].X=OB->RealX<<4;
			FBODS[NFlyBodies].Y=OB->RealY<<4;
			FBODS[NFlyBodies].Z=(GetHeight(OB->RealX>>4,OB->RealY>>4)+1)<<8;
			FBODS[NFlyBodies].Vx=vx;
			FBODS[NFlyBodies].Vy=vy;
			FBODS[NFlyBodies].Vz=vz;
			FBODS[NFlyBodies].Stage=0;
			NFlyBodies++;
		};
	};
};
void ProcessFlyBodies(){
	OneFlyBody* FB=FBODS;
	for(int i=0;i<NFlyBodies;i++){
		bool valid=0;
		OneObject* OB=Group[FB->Index];
		if(OB&&OB->Serial==FB->Serial){
			FB->X+=FB->Vx;
			FB->Y+=FB->Vy;
			FB->Z+=FB->Vz;
			FB->Vz-=1000;
			int z=GetHeight(FB->X>>8,FB->Y>>8);
			if(z<(FB->Z>>8)){
				valid=1;
			};
			OB->RealX=FB->X>>4;
			OB->RealY=FB->Y>>4;
			OB->RZ=FB->Z>>8;
		};
		if(valid){
			FB++;
		}else{
			if(i<NFlyBodies-1)memcpy(FBODS+i,FBODS+i+1,(NFlyBodies-i-1)*sizeof OneFlyBody);
			i--;
			NFlyBodies--;
		};
	};
};
void DetonateUnit(OneObject* OB,int CenterX,int CenterY,int Force){
	int DX=(OB->RealX>>4)-CenterX;
	int DY=(OB->RealY>>4)-CenterY;
	int N=int(sqrt(DX*DX+DY*DY));
	if(N){
		DX=(DX*Force*20)/N/(20+N);
		DY=(DY*Force*20)/N/(20+N);
		int DZ=Force*20/(20+N);
		AddFlyBody(OB->Index,DX,DY,DZ);
	};
};
void InitBodies(){
	NFlyBodies=0;
	int DetonationX=-1;
	DetonationY=-1;
	DetonationForce=-1;
};
bool DetectSpriteInPointInCell(int cell,int x,int y){
    if(cell<0||cell>=VAL_SPRSIZE)return false;
	int* CEL=SpRefs[cell];
	int   NCEL=NSpri[cell];
	if(!(CEL&&NCEL))return false;
	for(int i=0;i<NCEL;i++){
		OneSprite* OS=&Sprites[CEL[i]];
		//assert(OS->Enabled);
		ObjCharacter* OC=OS->OC;
		if(OC->ResType!=0xFE){
			int dr=OC->ShieldRadius;
			//if(OC->IntResType>8)dr=0;
			if(Norma(OS->x-x,OS->y-y)<dr&&OC->ShieldProbability>(rando()%100))
				return true;
        };
    };
    return false;
};
bool DetectSpriteInPoint(int x,int y){
    int nr=1;
    int nr1=nr+nr+1;
    int cell=(x>>7)-nr+(((y>>7)-nr)<<SprShf);
    for(int ix=0;ix<nr1;ix++)
        for(int iy=0;iy<nr1;iy++)
            if(DetectSpriteInPointInCell(cell+ix+(iy<<SprShf),x,y))return true;
    return false;
};