#include "CommonDip.h"

#ifndef NOTMULTITHREAD
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

// Add these to your DiplomacySystem class definition
std::mutex walkerMutex;
std::mutex stormMutex;
std::mutex cannonMutex;
std::mutex firerMutex;
std::mutex killerMutex;
std::mutex tomahawkMutex;

#endif

DiplomacySystem::DiplomacySystem(){
	memset(this,0,sizeof(*this));

	memset(AIX,0,7*sizeof(int));
	memset(AIY,0,7*sizeof(int));
	for(int i=0;i<7;i++) AI_set[i]=false;
	FriendAI=false;
};

extern DiplomacySystem DIPS;

#ifdef AC_ADDON
CEXPORT
void AddDipFriends(word ID, GAMEOBJ* GFriends){	
	if(ID<DIPS.NDIPS && GFriends){
		DIP_SimpleBuilding* dip=(DIP_SimpleBuilding*)DIPS.DIPS[ID];
		CopyUnits(GFriends,&dip->Friends,0,10000,1,0);
	}
}

CEXPORT
void SetDipTalkRadius(word ID, int R){
	if(ID<DIPS.NDIPS && R>500){
		DIP_SimpleBuilding* dip=(DIP_SimpleBuilding*)DIPS.DIPS[ID];
		UnitsCenter(&dip->BigZone,&dip->CentralGroup,R);		
	}
}

CEXPORT
void SetDipEasyTalk(word ID, bool State){
	if(ID<DIPS.NDIPS){
		DIP_SimpleBuilding* dip=(DIP_SimpleBuilding*)DIPS.DIPS[ID];
		dip->EasyTalk=State;		
	}
}

CEXPORT
word AddDipCenter(char* FileID, char* Dip, char* GDance, char* ZDance){
	char FID[128];
	sprintf(FID,"%s00",FileID);	
	return DIPS.AddDipCenter(FID, Dip, GDance, ZDance);
}
#endif AC_ADDON

word DiplomacySystem::AddDipCenter(char* FileID, char* Dip, char* GDance, char* ZDance){
	DIPS=(BasicDiploRelation**)realloc(DIPS,4*NDIPS+4);
	DIPS[NDIPS]=new DIP_SimpleBuilding;
	DIPS[NDIPS]->InitThisTribe2(FileID, Dip, GDance, ZDance);
	DIPS[NDIPS]->Index=NDIPS;
	DIPS[NDIPS]->DIP=this;
	NDIPS++;
	return NDIPS-1;
};

void DiplomacySystem::ResearchMap(){ 
	memset(StartPopulation,0,sizeof StartPopulation);
	BusyPageIndex=-1;
	NWalk=0;
	WSN=NULL;
	WID=NULL;
	int N=GetAmountOfGroups();
	for(int i=0;i<N;i++){
		char* GName=GetGroupName(i);
		if(GName[0]=='$'&&GName[1]=='D'&&GName[2]=='I'&&GName[3]=='P'&&GName[4]=='-'){
			//group for diplomacy
			char sname[256];
			strcpy(sname,GName+5);
			sname[strlen(sname)-5]=0;
			if(sname[0]=='S'&&sname[1]=='B'&&sname[2]=='L'&&sname[3]=='D'){
				DIPS=(BasicDiploRelation**)realloc(DIPS,4*NDIPS+4);
				DIPS[NDIPS]=new DIP_SimpleBuilding;
				DIPS[NDIPS]->InitThisTribe(GName+strlen(GName)-5,GName,sname);
				DIPS[NDIPS]->Index=NDIPS;
				DIPS[NDIPS]->DIP=this;
				NDIPS++;
			};
		}else if(GName[0]=='$'&&GName[1]=='U'&&GName[2]=='N'&&GName[3]=='I'&&GName[4]=='-'){
			char ccc[128];
			strcpy(ccc,GName);
			ccc[1]='Z';
			ccc[2]='O';
			ccc[3]='O';
			GAMEOBJ UNI;
			GAMEOBJ ZON;
			if(RegisterUnits(&UNI,GName)){
				RegisterZone(&ZON,ccc);
				ZooBirthZone ZBZ;
				memset(&ZBZ,0,sizeof ZBZ);
				ZBZ.Zone=ZON;
				int NU=GetNUnits(&UNI);
				OneUnit OU;
				int nur=0;
				if(NU){
					for(int p=0;p<NU;p++){
						if(GetUnitInfo(&UNI,p,&OU)){
							ZBZ.NIndex[nur]=OU.NIndex;
							nur++;
						};
					};
					if(nur){
						for(int i=nur;i<8;i++)ZBZ.NIndex[i]=ZBZ.NIndex[i%nur];
						ZBZ.Type=ZBZ.NIndex[0];
						AddZooBirthZone(&ZBZ);
						ZooGroup ZG;
						memset(&ZG,0,sizeof ZG);
						ZG.Type=ZBZ.NIndex[0];
						ZG.Group=UNI;
						ZG.MaxAmount=2+GetRND(4);
						ZG.MotionFrequency=700;
						if(GetGrpCenter(&UNI,&ZG.xc,&ZG.yc)){
							AddNewZooGroup(&ZG);
						};
					};
				};
			};
		};
	};
	GAMEOBJ UTP;
	UTP.Type='UTYP';
	UTP.Serial=0;
	memset(StartPopulation,0,sizeof StartPopulation);
	for(int i=0;i<1024;i++){
		OneUnit OU;
		if(GetUnitGlobalInfo(i,&OU)){
			if(OU.NIndex<2048)StartPopulation[OU.NIndex]++;
		};
	};
	for(int i=0;i<2048;i++)if(StartPopulation[i]){
		StartPopulation[i]>>=2;
		if(!StartPopulation[i])StartPopulation[i]=1;
	};
	DangerMap=znew(short,GetNZones());
	DMSize=GetNZones();
	memset(DangerMap,0,GetNZones()<<1);
};
void DiplomacySystem::PerformCommand(char* Data,int size){
	word* wData=(word*)Data;
	if(wData[0]<1024){
		//standart possibility[DIP_SimpleBuilding]
		DIPS[wData[1]]->PerformCommand(Data,size);
	};
};

#pragma warning(disable : 4035)
int Norma(int x, int y) {
	__asm{
		mov		eax,x
		cmp		eax,0
		jge		ggg1
		neg		eax
ggg1:	mov		ebx,y
		cmp		ebx,0
		jge		ggg2
		neg		ebx
ggg2:	mov		ecx,eax
		cmp		eax,ebx
		ja		ggg3
		mov		ecx,ebx
ggg3:	add		ecx,eax
		add		ecx,ebx
		shr		ecx,1
		mov		eax,ecx
	};
};
#pragma warning(default : 4035)

void SetZone(GAMEOBJ* Zone, int X, int Y, int R){
	Zone->Index = X;
	Zone->Serial = Y;
	Zone->Type='@   '-0x202020+R;
}

extern DiplomacySystem DIPS;
CEXPORT
bool SetAIForDiplo(byte nat, int x, int y){
	return DIPS.SetAI(nat,x,y);
}

bool DiplomacySystem::SetAI(byte nat, int x, int y){
	if(nat>=7) return false;
	if(!AI_set[nat]){
		AIX[nat]=x;
		AIY[nat]=y;
		AI_set[nat]=true;
	}
	return true;
};

CIMPORT bool DetArcher(word NIndex);
#ifdef NOTMULTITHREAD
void DiplomacySystem::Process(){
	//if(!FriendAI)
	{
		//FriendAI=true;
		for(int i=0;i<NDIPS;i++){
			int x,y;
			if(DIPS[i]->Owner==7&&GetGrpCenter(&DIPS[i]->CentralGroup,&x,&y)){
				byte clr=0xFF;
				int MinR=10000;
				for(int c=0;c<7;c++)if(AI_set[c]){
					int r=Norma(AIX[c]-x,AIY[c]-y);
					if(r<MinR){
						MinR=r;
						clr=c;
					}
				}
				if(clr!=0xFF){
					word Data[8];
					Data[0]=1;// Передать племя AI
					Data[1]=i;//Index of tribe
					Data[2]=clr;//color of player
					Data[3]=0xFFFF;//Index of shaman
					SendDipCommand((char*)Data,8);
				}
			}
		}
	}
	for(int i=0;i<NDIPS;i++)DIPS[i]->ProcessTribe();
	//processing walking units
	OneUnit OU;
	for(int i=0;i<NWalk;i++){
		bool erase=1;
		if(GetUnitGlobalInfo(WID[i],&OU)){
			if(WSN[i]==OU.Serial){
				erase=0;
				if(!OU.Busy){
					int xx=OU.x+GetRND(8192)-4096;
					int yy=OU.y+GetRND(8192)-4096;
					OBJ_SmartSendTo(WID[i],xx,yy,0,0,128+16,0);
				};
			};
		};
		if(erase){
			if(i<NWalk-1){
				memcpy(WID+i,WID+i+1,(NWalk-i-1)<<1);
				memcpy(WSN+i,WSN+i+1,(NWalk-i-1)<<1);
			};
			i--;
			NWalk--;
		};
	};
	// processing storms
	for(int i=0;i<NStorms;i++){
		StormGroup* Grp=STORMS+i;
		GAMEOBJ* Group=&Grp->Group;
		GAMEOBJ* StrGrp=&Grp->StrGrp;
		int top=0xFFFF;
		DeleteHiddenUnits(StrGrp);
		int N=CleanGroup(Group)+CleanGroup(StrGrp);
		if(N){
			int T0=GetGlobalTime();
			
			if(Grp->StormID!=0xFFFF){
				// штурм
				OneUnit u;
				GetUnitGlobalInfo(Grp->StormID,&u);

				int xc,yc;
				if(GetGrpCenter(&Grp->Group,&xc,&yc)){
					Grp->Top=GetTopZone(xc,yc);
				}

				if(u.Index!=0xFFFF && u.Serial==Grp->StormSN){
					// процесс штурма
					int x=u.x, y=u.y;
					GetBuildingEntryPoint(&x, &y, u.Index, 64);
					
					GAMEOBJ Z;
					Z.Type='@   '-'   '+1000;
					Z.Index=x;
					Z.Serial=y;

					int NIn=0;
					GetBuildingOposit(u.Index,NIn);
					if(NIn>8){						
						NIn>>=1;
						if(Grp->Difficulty<3) NIn>>=1;
					}
					NIn|=1;

					int NStr=GetNUnits(StrGrp);
					NIn-=NStr;
					
					if(NIn>0){
						//Grp->StormID=0xFFFF;												
						N=GetNUnits(Group);
						//OneUnit su;
						for(int id=0;id<N&&id<NIn;id++){
							//GetUnitInfo(Group,id,&su);
							//if(su.Index!=0xFFFF){
								//InviteUnitIntoBuilding(OU.Index,u.Index,0);												
								//InsertUnitToGroup(Group,StrGrp,id);
								//DeleteUnitFromGroup(Group,id);								
							RemoveNearestUnit(Group,StrGrp,x,y,1000);								
							//}
						}
					}

					AttackEnemyInZone2(Group,&Z,Grp->Owner);					

				}else{
					// штурм удался
					if(u.Index!=0xFFFF) DieUnit(u.Index);
					Grp->StormID=0xFFFF;
					RemoveGroup(StrGrp,Group);
					Grp->LastTop=0xFFFF;
				}
				
			}else{
				// подготовка к штурму
				if(/*CheckIfNotBusy(&Grp->Group)||*/T0-Grp->LastMoveTime>100){
					Grp->LastMoveTime=T0;
					
					RemoveGroup(StrGrp,Group);

					int xc,yc;
					//if(Group->Index==15083) SelectUnits(Group,0);
					if(GetGrpCenter(Group,&xc,&yc)){
						int top=GetTopZone(xc,yc);
						Grp->Top=top;
						if(top>=0xFFFE){
							OneUnit UN;
							GetUnitInfo(Group,0,&UN);
							xc=UN.x;
							yc=UN.y;
							top=GetTopZone(xc,yc);
						}
						if(top>=0&&top<GetNZones()){
							byte Owner=Grp->Owner;
							short Dang[4096];
							int Fear[256];
							word IDS[4096];
							for(int j=0;j<256;j++)Fear[j]=3;
							Fear[1]=10;
							if(Grp->Difficulty>1)
								CreateDangerMapForStormHard(Owner,Dang,GetNZones(),Fear,2);
							else
								CreateDangerMapForStorm(Owner,Dang,GetNZones(),Fear,2);
							if(Grp->LastTop!=0xFFFF) Dang[Grp->LastTop]+=4000;
							
							//*
							int DM[2048];
							for(int d=0;d<2048;d++) DM[d]=Dang[d];
							SetDangerMap(DM);
							//*/

							bool find=false;
							CannonGroup* Can=NULL;	// cannon to defend
							int Force=2000;
							//memset(IDS,0xFF,sizeof(IDS));
							for(int f=NCannons-1;f>=0;f--){
								CannonGroup* can=CANNONS+f;
								if(can->Owner==Grp->Owner){
									int t=can->Top;
									int f=can->DefForce;
									if(t<0xFFFE){
										if(t==top){
											Can=can;
											find=true;
											break;
										}
										if(f<Force){
											Can=can;
											Force=f;
											find=true;
										}
									}
								}
							}

							word DST=0xFFFF;
							int zf=0xFFFF;

							word StrmNIndex=0xFFFF;
							int n=GetNUnits(Group);
							for(int i=0;i<n;i++){
								OneUnit OU;
								if(GetUnitInfo(Group,i,&OU)){
									StrmNIndex=OU.NIndex;
									break;
								}
							}

							int maxdang;
							if(!find){							
								if(Grp->Difficulty>1){
									int NSt=N<<(4-Grp->Difficulty);
									//if(Grp->Difficulty==2) NSt<<=1;
									CreateTopListForStormHard(IDS,Owner,NSt,StrmNIndex);
								}else{
									CreateTopListForStorm(IDS,Owner,StrmNIndex);
								}
								zf=FindNextZoneOnTheSafeWayToObject(top,Dang,IDS,&maxdang,3,&DST);
							}else{
								zf=FindNextZoneOnTheSafeWay(top,Can->Top,Dang,&maxdang,3);
							}
							if(zf>=0xFFFE){
								if(Grp->Difficulty>1){
									Grp->Difficulty=0;
								}else{
									//CreateTopListForStorm(IDS,Owner);
									
									//find=true;
									//CreateTopListEnArmy(IDS,Owner,N>>4);
									//zf=FindNextZoneOnTheSafeWayToObject(top,Dang,IDS,&maxdang,3,&DST);
									
									if(DetArcher(StrmNIndex)&&(GetRND(4)&1)) AddFirers(Group,Grp->Owner);
										else AddKillers(Group,Grp->Owner,0);
								}
							}
							
							if(zf<0xFFFE){
								int tx=-1;
								int ty=-1;
								
								OneUnit OU;
								OU.Index=0xFFFF;
								if(DST!=0xFFFF&&GetUnitGlobalInfo(DST,&OU)){								
									tx=OU.x;
									ty=OU.y;
								}
								
								int r=GetTopDist(xc,yc,tx,ty);
								
								if(r<25){
									int n=GetNUnits(Group);
									for(int i=0;i<n;i++){
										OneUnit OU;
										if(GetUnitInfo(Group,i,&OU)){
											int rr=GetTopDist(OU.x,OU.y,tx,ty);
											if(rr<r){
												r=rr;
											}											
										}
									}
								}
								
								if(r<12){
									if(!find){
										Grp->StormID=OU.Index;
										Grp->StormSN=OU.Serial;										
										SGP_ComeIntoBuilding(StrGrp,0,OU.Index);

										/*
										int MaxSt=3+GetRND(8);
										
										int N=CleanGroup(&Grp->Group);
										OneUnit u;
										for(int id=0;id<N&&id<MaxSt;id++){
											GetUnitInfo(&Grp->Group,id,&u);
											if(u.Index!=0xFFFF){
												InviteUnitIntoBuilding(OU.Index,u.Index,0);												
												InsertUnitToGroup(&Grp->Group,&Grp->StrGrp,id);
												DeleteUnitFromGroup(&Grp->Group,id);
											}
										}
										*/
									}else{
										GAMEOBJ Z;									
										Z.Index=xc;
										Z.Serial=yc;
										Z.Type='@   '-'   '+1000;
										AttackEnemyInZone2(Group,&Z,Owner);
									}

								}else{
									GAMEOBJ Z;
									Z.Index=xc;
									Z.Serial=yc;
									Z.Type='@   '-'   '+600;
									if(!find){
										if(zf!=top&&r>3){
											int x,y;
											bool Attack=false;
											if(r<30){
												GetBuildingEntryPoint(&x, &y, OU.Index, 64);
											}else{
												word* Way;
												int NWayPoint=GetLastFullWay(&Way);
												if(NWayPoint>2) GetTopZRealCoor(Way[NWayPoint-3],&x,&y);
													else GetTopZRealCoor(zf,&x,&y);
												
												Attack=AttackEnemyInZone2(&Grp->Group,&Z,Owner);
											}
											if(!Attack){
												SGP_MoveToPoint(Owner,Group,x,y,512,0,0,32);
											}
										}
									}else{
										Z.Type='@   '-'   '+1400;
										AttackEnemyInZone2(&Grp->Group,&Z,Owner);
									}
								}
							}else{
								GAMEOBJ Z;									
								Z.Index=xc;
								Z.Serial=yc;
								Z.Type='@   '-'   '+1000;
								AttackEnemyInZone2(&Grp->Group,&Z,Owner);
								//AddStorm(NULL,0);
							}							
							Grp->LastTop=top;
						};
					};
				};
			};
		}
	}
	// processing cannons
	for(int i=0;i<NCannons;i++){
		CannonGroup* Grp=CANNONS+i;
		GAMEOBJ* Group=&Grp->Group;
		int top=0xFFFF;
		int N=CleanGroup(Group);		
		if(N){
			int xc,yc;
			if(GetGrpCenter(Group,&xc,&yc)){
				
				GAMEOBJ Z;
				SetZone(&Z,xc,yc,1000);
				Grp->DefForce=GetAlliesForce(&Z,Grp->Owner);

				top=GetTopZone(xc,yc);
				if(top>=0&&top<GetNZones()){

					for(int j=0;j<NCannons;j++) if(j!=i){
						CannonGroup* Allies=CANNONS+j;
						if(GetNUnits(&Allies->Group)){
							int dist=GetZonesDist(top,Allies->Top);
							if(dist<10){
								RemoveGroup(&Allies->Group,Group);
							}
						}
					}
				
					if(CheckIfNotBusy(Group)){
						int ax=Grp->ArtDepoX;
						int ay=Grp->ArtDepoY;
						byte Owner=Grp->Owner;

						short Dang[4096];
						int Fear[256];
						for(int j=0;j<256;j++)Fear[j]=3;
						CreateDangerMapWithoutPeasants(Owner,Dang,GetNZones(),Fear,4);
						if(Grp->LastTop!=0xFFFF) Dang[Grp->LastTop]+=4000;

						/*
						int DM[2048];
						memset(DM,0,sizeof DM);
						int NZ=GetNZones();
						for(int i=0;i<NZ;i++) DM[i]=Dang[i];
						SetDangerMap(DM);
						*/

						word IDS[2048];
						CreateNonFiredEnemyBuildingsTopList(IDS,Owner);
						int maxdang;
						word DST=0xFFFF;
						
						int zf=FindNextZoneOnTheSafeWayToObject(top,Dang,IDS,&maxdang,3,&DST);
						int dx,dy;
						if(DST!=0xFFFF&&GetTopZRealCoor(zf,&dx,&dy)){
							/*
							word* Way;
							int NWayPoint=GetLastFullWay(&Way);
							if(NWayPoint>2) GetTopZRealCoor(Way[NWayPoint-3],&x,&y);
								else GetTopZRealCoor(zf,&x,&y);
							*/
							word* WAY;
							int NStep=GetLastFullWay(&WAY);
							int DanTop=0xFFFF;
							for(int s=0;s<NStep;s++){
								int DT=WAY[s];
								if(Dang[DT]>1000){
									DanTop=DT;
								}
							}

							int destnorm=10000;
							int dnx=-1;
							int dny=-1;
							if(DanTop!=0xFFFF){								
								if(GetTopZRealCoor(DanTop,&dnx,&dny)){
									destnorm=Norma(xc-dnx,yc-dny);
								};
							}else{
								OneUnit u;
								GetUnitGlobalInfo(DST,&u);
								if(u.Index!=0xFFFF){
									destnorm=Norma(xc-u.x,yc-u.y);
								}
							}

							int artnorm=Norma(xc-ax,yc-ay);
							
							if(artnorm>1000 && destnorm<5600){
								//OBJ_AttackObj(ID,DST,128+16,0);
								if(DanTop!=0xFFFF && dnx!=-1){
									ArtAttackGroud(Owner,Group,dnx,dny);
								}else{
									ArtAttackObject(Owner,Group,DST);
								}
							}else{											
								SGP_MoveToPoint(Owner,Group,dx,dy,512,200,0,32);

								Grp->LastTop=top;
							}
						}
					};					
				}
			}
		}
		Grp->Top=top;
	}
	//processing firers
	for(int i=0;i<NFirers;i++)if(CleanGroup(&FIRERS[i].Group)){
		FiringGroup* Grp=FIRERS+i;
		GAMEOBJ* Group=&Grp->Group;
		int NU=CleanGroup(Group);
		if(NU){
			//if(Group->Index==4995) SelectUnits(Group,0);
			int T0=GetGlobalTime();
			word BID=Grp->CurrentFiringBuilding;
			word SNS=Grp->FBSN;
			OneUnit OU;
			GetUnitGlobalInfo(BID,&OU);
			if(OU.Index!=0xFFFF&&OU.Serial==SNS&&T0-Grp->LastMoveTime>100){
				Grp->LastMoveTime=T0;
				if(OU.Index==0xFFFF || OU.NI==Grp->Owner || CheckIfBuildingIsFired(BID)){
					int xc,yc;
					if(GetGrpCenter(&Grp->Group,&xc,&yc)){
						GAMEOBJ Z;
						Z.Index=xc;
						Z.Serial=yc;
						Z.Type='@   '-'   '+1200;
						AttackEnemyInZone2(&Grp->Group,&Z,Grp->Owner);
					}
					Grp->CurrentFiringBuilding=0xFFFF;
				}else{
					GrpAttackObject(Grp->Owner,&Grp->Group,BID);
				}
			}else{
				if(/*CheckIfNotBusy(&Grp->Group)||*/T0-Grp->LastMoveTime>100){
					Grp->LastMoveTime=T0;

					SetToInternalResourcesZero(Group);
					int xc,yc;
					if(GetGrpCenter(&Grp->Group,&xc,&yc)){
						int top=GetTopZone(xc,yc);
						if(top>=0xFFFE){
							OneUnit UN;
							GetUnitInfo(Group,0,&UN);
							xc=UN.x;
							yc=UN.y;
							top=GetTopZone(xc,yc);
						}
						if(top>=0&&top<GetNZones()){
							byte Owner=FIRERS[i].Owner;
							short Dang[4096];
							int Fear[256];
							word IDS[4096];
							for(int j=0;j<256;j++)Fear[j]=3;
							Fear[1]=10;
							CreateDangerMapForFire(Owner,Dang,GetNZones(),Fear,3);
							if(Grp->LastTop!=0xFFFF) Dang[Grp->LastTop]+=4000;

							/*
							int DM[2048];
							for(int d=0;d<2048;d++) DM[d]=Dang[d];
							SetDangerMap(DM);
							*/
							
							
							bool find=false;
							/*
							memset(IDS,0xFF,sizeof(IDS));
							for(int f=0;f<NCannons;f++){
								CannonGroup* Grp=CANNONS+f;
								
								int top=Grp->Top;
								if(top<0xFFFE){
									IDS[top]=top;
									find=true;
								}
							}*/

							if(!find) CreateNonFiredEnemyBuildingsTopList(IDS,Owner);

							int maxdang;
							word DST=0xFFFF;
							int zf=FindNextZoneOnTheSafeWayToObject(top,Dang,IDS,&maxdang,3,&DST);
							if(DST!=0xFFFF&&zf<0xFFFE){
								int tx=-1;
								int ty=-1;							
								if(!find){								
									if(GetUnitGlobalInfo(DST,&OU)){
										tx=OU.x;
										ty=OU.y;
									}
								}else{
									GetTopZRealCoor(DST,&tx,&ty);
								}
								
								if(tx!=-1){
									int r=GetTopDist(xc,yc,tx,ty);
									
									if(r<30){
										GrpAttackObject(Owner,&Grp->Group,DST);
										Grp->CurrentFiringBuilding=OU.Index;
										Grp->FBSN=OU.Serial;										
									}else{
										GAMEOBJ Z;
										Z.Index=xc;
										Z.Serial=yc;
										Z.Type='@   '-'   '+600;
										if(/*!AttackEnemyInZone2(&Grp->Group,&Z,Owner)&&*/zf!=top&&r>3){
											int x,y;
											
											word* Way;
											int NWayPoint=GetLastFullWay(&Way);
											if(NWayPoint>2) GetTopZRealCoor(Way[NWayPoint-3],&x,&y);
												else GetTopZRealCoor(zf,&x,&y);
											
											SGP_MoveToPoint(Owner,Group,x,y,512,128-GetRND(256),128-GetRND(256),32);

											Grp->LastTop=top;
										}
									};
								};
							}else{
								AddTomahawks(Group,Grp->Owner,0,0);
							};
						};
					};
				};
			}
		};
	};
	for(int i=0;i<NKillers;i++){
		int T0=GetGlobalTime();
		OneUnit OU;		
		KillersGroup* Grp=KILLERS+i;
		GAMEOBJ* Group=&Grp->Group;
		int NUnits=CleanGroup(Group);		
		if(NUnits&&(/*CheckIfNotBusy(Group)||*/T0-KILLERS[i].LastMoveTime>100)){
			
			Grp->LastMoveTime=T0;

			int xc,yc;
			if(GetGrpCenter(&KILLERS[i].Group,&xc,&yc)){
				int top=GetTopZone(xc,yc);
				if(top>=0xFFFE){
					OneUnit UN;
					GetUnitInfo(Group,0,&UN);
					xc=UN.x;
					yc=UN.y;
					top=GetTopZone(xc,yc);
				}
				if(top>=0&&top<GetNZones()){
					byte Owner=KILLERS[i].Owner;
					short Dang[4096];
					int Fear[256];
					word IDS[4096];
					for(int j=0;j<256;j++)Fear[j]=5;  // indeyci =32, evrope =4
					Fear[1]=10;					
					CreateDangerMap(Owner,Dang,GetNZones(),Fear,3);
					if(KILLERS[i].LastTop!=0xFFFF) Dang[KILLERS[i].LastTop]+=4000;

					int MinPS=NUnits>>3;
					if(MinPS>12) MinPS=12;
					CreatePeasantsTopList(IDS,Owner,MinPS,Grp->SeakMine);
					int maxdang;
					word DST=0xFFFF;
					int zf=FindNextZoneOnTheSafeWayToObject(top,Dang,IDS,&maxdang,3,&DST);
					bool FindPeasant=true;
					/*
					if(DST==0xFFFF){
						CreateTopListEnArmy(IDS,Owner,1);
						zf=FindNextZoneOnTheSafeWayToObject(top,Dang,IDS,&maxdang,5,&DST);
						FindPeasant=false;
					}
					*/
					if(DST!=0xFFFF){
						if(GetUnitGlobalInfo(DST,&OU)){							
							GAMEOBJ Z;
							Z.Index=xc;
							Z.Serial=yc;
							Z.Type='@   '-'   '+1500;
							int r=GetTopDist(xc,yc,OU.x,OU.y);
							if(r<12){
								//SGP_MoveToPoint(7,Group,OU.x,OU.y,512,0,0,1);
								//CancelAllPurposes(&KILLERS[i].Group);
								Z.Index=OU.x;
								Z.Serial=OU.y;
								if(OU.Building){
									//int x,y;
									//GetBuildingEntryPoint(&x,&y,OU.Index,32);
									Z.Type='@   '-'   '+800;
									if(!AttackEnemyInZone2(&KILLERS[i].Group,&Z,Owner)) SGP_MoveToPoint(7,Group,OU.x,OU.y,512,0,0,0);
								}else{
									if(FindPeasant) AttackEnemyPeasantsInZone(&KILLERS[i].Group,&Z,OU.NI);
										else AttackEnemyInZone2(&KILLERS[i].Group,&Z,Owner);
								}
								//KILLERS[i].LastMoveTime=T0;
							}else{
								//int NArm;
								//AttackEnemyInZone2(&KILLERS[i].Group,&Z,Owner)
								//if(!Attack6(&KILLERS[i].Group,&Z,Owner,NArm)){
									SetToInternalResourcesZero(Group);
									
									int x,y;
									if(r<25){
										x=OU.x;
										y=OU.y;
									}else{										
										word* Way;
										int NWayPoint=GetLastFullWay(&Way);
										if(NWayPoint>2) GetTopZRealCoor(Way[NWayPoint-3],&x,&y);
											else GetTopZRealCoor(zf,&x,&y);
									}
									
									SGP_MoveToPoint(Owner,Group,x,y,512,128-GetRND(256),128-GetRND(256),32);
									
									KILLERS[i].LastMoveTime=T0+15;
									KILLERS[i].LastTop=top;
								//}
							};
						};
					}else{
						AddStorm(Group,Grp->Owner,2);
					}
				};
			};
		};
	};
	for(int i=0;i<NTomahawks;i++){
		int T0=GetGlobalTime();
		TomahawkGroup* Grp=TOMAHAWKS+i;
		int NMen = CleanGroup(&Grp->Group);
		GAMEOBJ* Group=&Grp->Group;
		if(NMen){
			// add friends
			/*
			for(int j=0;j<NTomahawks;j++)if(i!=j){
				TomahawkGroup* Friend=TOMAHAWKS+j;
				if(Friend->Owner==Grp->Owner&&GetNUnits(&Friend->Group)&&Norma(Grp->x-Friend->x,Grp->y-Friend->y)<1000){
					OneUnit FU;
					GetUnitInfo(&Friend->Group
					RemoveGroup(&Friend->Group, Group);
					CleanGroup(&Friend->Group);
				}
			}
			*/
			// thinking
			if(/*CheckIfNotBusy(&Grp->Group)||*/T0-Grp->LastMoveTime>100){

				Grp->LastMoveTime=T0;

				int xc,yc;
				if(GetGrpCenter(&Grp->Group,&xc,&yc)){
					Grp->x=xc;
					Grp->y=yc;
					int top=GetTopZone(xc,yc);
					if(top>=0xFFFE){
						OneUnit UN;
						GetUnitInfo(&Grp->Group,0,&UN);
						xc=UN.x;
						yc=UN.y;
						top=GetTopZone(xc,yc);
					}
					if(top>=0&&top<GetNZones()){
						byte Owner=Grp->Owner;
						short Dang[4096];
						int Fear[256];
						word IDS[4096];
						for(int j=0;j<256;j++)Fear[j]=2;
						Fear[1]=100;
						//CreateDangerMap(Owner,Dang,GetNZones(),Fear,3);
						CreateDangerMapForTom(Owner,Dang,GetNZones(),Fear,3);						

						
						int DD[4096];
						for(int j=0;j<GetNZones();j++) DD[j]=Dang[j];
						SetDangerMap(DD);
						

						int maxdang;
						
						// looking to enemy force						
						GAMEOBJ Zone;
						SetZone(&Zone,xc,yc,Grp->MaxAttR+300); //400
						int Force=GetGroupForce(&Grp->Group);
						int AliForce=-GetEnemyForce(&Zone,Owner);
						
						bool attacking=true;	// нападаем, отступаем
						bool moving=true;		// перейти в новую топ-зону
						bool thinktwice=false;	// думать часче
						bool retreat=false;		// полное отступление

						// поиск щита
						if(Grp->ShieldID>=0xFFFE||!GetNUnits(&STORMS[Grp->ShieldID].Group)){
							memset(IDS,0xFF,sizeof(IDS));
							for(int i=0;i<NStorms;i++){
								StormGroup* GRP=STORMS+i;
								if(GRP->Owner==Grp->Owner){
									int ST=GRP->Top;
									if(GetNUnits(&GRP->Group)+GetNUnits(&GRP->StrGrp)>0 && ST>=0 && ST<0xFFFE){
										IDS[ST]=i;									
									}
								}
							}
							word DST=0xFFFF;
							int zf=FindNextZoneOnTheSafeWayToObject(top,Dang,IDS,&maxdang,5,&DST);
							if(DST!=0xFFFF) Grp->ShieldID=DST;
								else Grp->ShieldID=0xFFFF;
						}
						word DST=0xFFFF;
						if(Grp->ShieldID!=0xFFFF) DST=STORMS[Grp->ShieldID].Top;
						bool findstm=(DST!=0xFFFF);
						
						if(findstm){
							//Force += GetGroupForce(&Grp->Group);
							int sx,sy;
							if(GetTopZRealCoor(DST,&sx,&sy)){
								if(Norma(sx-xc,sy-yc)<800) DST=0xFFFF;
							}
						}

						//word Builds[128];
						//int NBuilds=0;
						//bool building=MakeListOfEnBuildingInZone(Owner,&Zone,Builds,&NBuilds);
						
						SetZone(&Zone,xc,yc,Grp->MaxAttR+200);
						SetUnitsState(Group,1,0,0,0);
						word BID=0xFFFF;
						if(T0-Grp->AttackTime>0&&(AliForce>=Force||GetRND(100)>30)) BID=MakeOneShotToBuild(&Zone,&Grp->Group,Owner);
						bool building=false;
						if(BID!=0xFFFF){
							OneObject* BOB=GetOBJ(BID);
							if(BOB&&!(BOB->Sdoxlo||BOB->NNUM==7)){
								building=true;
							}
						};

						if(building&&Grp->Strelok) moving=false;
						
						int NIn=0;
						if(building){
							if(GetBuildingOposit(BID,NIn)){
								//if(!NIn) building=false;
								AliForce -= NIn*5;
								NIn>>=1;
								NIn++;
							}
						}
						
						if(building&&NIn<NMen){							
							if(isUnitAbsorber(BID)){
								int NStr=GetRND(GetDiff(Grp->Owner))+1;							

								int N=CleanGroup(Group);
								OneUnit u;
								for(int id=0;id<N&&NStr>0;id++){
									GetUnitInfo(Group,id,&u);
									bool ru=(GetRND(300)==0);
									if(u.Index!=0xFFFF&&(u.ChargeProgress||ru)){
										InviteUnitIntoBuilding(BID,u.Index,0);												
										//InsertUnitToGroup(&Grp->Group,&Grp->StrGrp,id);
										DeleteUnitFromGroup(Group,id);
										if(ru) ru=0;
										NStr--;
									}
								}
							}else{
								building=false;
							}
						}								

						if(moving){
							if(AliForce>Force-(Force>>3)){
								
								if(Grp->Strelok){
									SetUnitsState(Group,1,0,1,0);
								}else{
									SetUnitsState(Group,1,0,1,0);
								}
								
								SetZone(&Zone,xc,yc,Grp->MaxAttR+200);
								if(AttackEnemyInZone2(&Grp->Group,&Zone,Owner)){
									//moving=false;
									Grp->LastThinkTime=T0;
									/*
									if(T0-Grp->AttackTime>0&&Grp->Strelok){
										Grp->AttackTime=T0+196;
									}
									*/
								}
							}else{
								// recession
								if(T0-Grp->LastThinkTime>100){
									thinktwice=true;
									attacking=false;
									if(building) retreat=true;
								}else{
									SetUnitsState(Group,1,0,1,0);
									if(!Grp->Strelok) AttackEnemyInZone2(&Grp->Group,&Zone,Owner);
									moving=false;
								}
							}
						}
						
						if(Grp->LastTop!=0xFFFF&&!thinktwice) Dang[Grp->LastTop]+=4000;
						//if(attacking&&Grp->LastTop!=0xFFFF) Dang[Grp->LastTop]+=4000;

						// looking for nearest firest
						//bool find=false;	// поиск топ-зоны для прикрытия
						/*
						memset(IDS,0xFF,sizeof(IDS));
						for(int f=0;f<NFirers;f++){
							FiringGroup* SGrp=FIRERS+f;
							int xc,yc;
							if(SGrp->Owner==Owner&&GetNUnits(&SGrp->Group)){								
								int t=SGrp->LastTop;
								if(t!=0xFFFF){
									IDS[t]=f;
									find=true;
								}
							}
						}*/
						
						
						if(thinktwice&&T0-Grp->AttackTime>0){
							SetZone(&Zone,xc,yc,Grp->MaxAttR+500);
							if(Grp->Strelok) SetUnitsState(Group,1,0,0,0);
								else SetUnitsState(Group,1,0,0,1);
							int NArm;
							bool at=AttackByTomahawks(&Grp->Group,&Zone,Owner,NArm);
							
							if(!at){
								Grp->LastMoveTime=T0;
							}
						}

						bool SearchEnemy=false;
						if(moving){
								
							//int r=GetTopDist(xc,yc,tx,ty);

							//if(!find){
								if(attacking && T0-Grp->AttackTime>0){
									CreateTopListEnArmy(IDS,Owner,NMen>>4);
									SearchEnemy=true;
								}else{
									if(DST==0xFFFF){
										CreateFriendBuildingsTopList(IDS,Owner);
										IDS[top]=0xFFFF;
										if(Grp->LastTop!=0xFFFF) IDS[Grp->LastTop]=0xFFFF;
									}
								}
							//}
							
							int zf;
							if(DST==0xFFFF||attacking) zf=FindNextZoneOnTheSafeWayToObject(top,Dang,IDS,&maxdang,5,&DST);
								else zf=DST;

							Grp->LastMoveTime=T0;

							if(DST!=0xFFFF&&zf!=top){
								int dx,dy;
								//bool AttEnm=false;
								
								/*
								if(SearchEnemy){
									OneUnit OU;
									GetUnitGlobalInfo(DST,&OU);
									if(OU.Index!=0xFFFF){
										int r=Norma(xc-OU.x,yc-OU.y);
										if(r<Grp->MaxAttR+300) thinktwice=true;
									}
								}
								*/

								if(GetTopZRealCoor(zf,&dx,&dy)){
									if(GetGroupCharge(Group)||!attacking){
										if(!thinktwice){
											word* Way;
											int NWayPoint=GetLastFullWay(&Way);
											if(NWayPoint>2) GetTopZRealCoor(Way[NWayPoint-3],&dx,&dy);						
											//if(NWayPoint<5) thinktwice=true;
										}

										SGP_MoveToPoint(Owner,Group,dx,dy,512,128-GetRND(256),128-GetRND(256),32);
										//ClearAZones();
										//CreateAZone("Move",dx,dy,128);
										
									}
									if(moving&&attacking) Grp->LastTop=top;
										else Grp->LastTop=0xFFFF;
									
									/*
										if(Grp->ShieldID!=0xFFFF){
										StormGroup* STR=STORMS+Grp->ShieldID;
									}
									*/
									
									/*
									if(findstm){
										int sx,sy;
										if(GetTopZRealCoor(DST,&sx,&sy)){
											if(Norma(sx-xc,sy-yc)>1000) retreat=false;
										}
									}
									*/
									if(thinktwice){ 
										Grp->LastMoveTime=T0-62;
									}else
									if(retreat && !findstm && T0-Grp->AttackTime>0){
										Grp->AttackTime=T0+256;
									}else 
										Grp->LastMoveTime=T0;
								}

							}else{
								if(Grp->Strelok){
									SetUnitsState(Group,1,0,0,0);
								}else{
									SetUnitsState(Group,1,0,1,0);
								}								
								AttackEnemyInZone2(&Grp->Group,&Zone,Owner);								
							}
						}else{
							Grp->LastMoveTime=T0;
						}

						/*
						if(thinktwice&&T0-Grp->AttackTime>0){
							SetZone(&Zone,xc,yc,Grp->MaxAttR+500);
							if(Grp->Strelok) SetUnitsState(Group,1,0,0,0);
								else SetUnitsState(Group,1,0,0,1);
							int NArm;
							bool at=AttackByTomahawks(&Grp->Group,&Zone,Owner,NArm);
							
							if(!at){
								Grp->LastMoveTime=T0;
							}
						}
						*/

						if(!moving) Grp->LastMoveTime=T0-60;					

					}
				}
			}
		}
	}
	for(int i=0;i<6;i++)SetResource(7,i,1000000);
	ProcessZoology();
};
#else
static std::mutex g_gameApiMutex;

void ProcessStormRange(DiplomacySystem* dipSystem, int start, int end, int T0) {
    StormGroup* STORMS = dipSystem->STORMS;
    CannonGroup* CANNONS = dipSystem->CANNONS;
    int NCannons = dipSystem->NCannons;
    
    for(int i = start; i < end; i++) {
        StormGroup* Grp = STORMS + i; 
        GAMEOBJ* Group = &Grp->Group;
        GAMEOBJ* StrGrp = &Grp->StrGrp;
        
        DeleteHiddenUnits(StrGrp);
        int N = CleanGroup(Group) + CleanGroup(StrGrp);
        if(!N) continue;

        if(Grp->StormID != 0xFFFF) {
            OneUnit u;
            GetUnitGlobalInfo(Grp->StormID, &u);

            int xc, yc;
            if(GetGrpCenter(&Grp->Group, &xc, &yc)) {
                Grp->Top = GetTopZone(xc, yc);
            }

            if(u.Index != 0xFFFF && u.Serial == Grp->StormSN) {
                int x = u.x, y = u.y;
                GetBuildingEntryPoint(&x, &y, u.Index, 64);
                
                GAMEOBJ Z;
                Z.Type = '@   ' - '   ' + 1000;
                Z.Index = x;
                Z.Serial = y;

                int NIn = 0;
                GetBuildingOposit(u.Index, NIn);
                if(NIn > 8) {
                    NIn >>= 1;
                    if(Grp->Difficulty < 3) NIn >>= 1;
                }
                NIn |= 1;

                int NStr = GetNUnits(StrGrp);
                NIn -= NStr;
                
                if(NIn > 0) {
                    N = GetNUnits(Group);
                    for(int id = 0; id < N && id < NIn; id++) {
                        RemoveNearestUnit(Group, StrGrp, x, y, 1000);
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                    AttackEnemyInZone2(Group, &Z, Grp->Owner);
                }
            } else {
                if(u.Index != 0xFFFF) {
                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                    DieUnit(u.Index);
                }
                Grp->StormID = 0xFFFF;
                RemoveGroup(StrGrp, Group);
                Grp->LastTop = 0xFFFF;
            }
        } else {
            if(T0 - Grp->LastMoveTime > 150) {
                Grp->LastMoveTime = T0;
                RemoveGroup(StrGrp, Group);

                int xc, yc;
                if(GetGrpCenter(Group, &xc, &yc)) {
                    int top = GetTopZone(xc, yc);
                    Grp->Top = top;
                    
                    if(top >= 0xFFFE) {
                        OneUnit UN;
                        GetUnitInfo(Group, 0, &UN);
                        xc = UN.x;
                        yc = UN.y;
                        top = GetTopZone(xc, yc);
                    }
                    
                    if(top>=0&&top<GetNZones()){
                        byte Owner=Grp->Owner;
                        
                        // Large arrays moved to heap using std::vector
                        std::vector<short> Dang(4096);
                        std::vector<int> Fear(256);
                        std::vector<word> IDS(4096);
                        
                        for(int j=0;j<256;j++) Fear[j]=3;
                        Fear[1]=10;
                        
                        if(Grp->Difficulty>1)
                            CreateDangerMapForStormHard(Owner,Dang.data(),GetNZones(),Fear.data(),2);
                        else
                            CreateDangerMapForStorm(Owner,Dang.data(),GetNZones(),Fear.data(),2);
                        
                        if(Grp->LastTop!=0xFFFF) Dang[Grp->LastTop]+=4000;

                        bool find=false;
                        CannonGroup* Can=NULL;
                        int Force=2000;
                        for(int f=NCannons-1;f>=0;f--){
                            CannonGroup* can=CANNONS+f;
                            if(can->Owner==Grp->Owner){
                                int t=can->Top;
                                int f=can->DefForce;
                                if(t<0xFFFE){
                                    if(t==top){
                                        Can=can;
                                        find=true;
                                        break;
                                    }
                                    if(f<Force){
                                        Can=can;
                                        Force=f;
                                        find=true;
                                    }
                                }
                            }
                        }

                        word DST=0xFFFF;
                        int zf=0xFFFF;

                        word StrmNIndex=0xFFFF;
                        int n=GetNUnits(Group);
                        for(int i=0;i<n;i++){
                            OneUnit OU;
                            if(GetUnitInfo(Group,i,&OU)){
                                StrmNIndex=OU.NIndex;
                                break;
                            }
                        }

                        int maxdang;
                        if(!find){
                            if(Grp->Difficulty>1){
                                int NSt=N<<(4-Grp->Difficulty);
                                CreateTopListForStormHard(IDS.data(),Owner,NSt,StrmNIndex);
                            }else{
                                CreateTopListForStorm(IDS.data(),Owner,StrmNIndex);
                            }
                            zf=FindNextZoneOnTheSafeWayToObject(top,Dang.data(),IDS.data(),&maxdang,3,&DST);
                        }else{
                            zf=FindNextZoneOnTheSafeWay(top,Can->Top,Dang.data(),&maxdang,3);
                        }
                        
                        if(zf>=0xFFFE){
                            if(Grp->Difficulty>1){
                                Grp->Difficulty=0;
                            }else{
                                if(DetArcher(StrmNIndex)&&(GetRND(4)&1)) {
                                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                    dipSystem->AddFirers(Group,Grp->Owner);
                                } else {
                                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                    dipSystem->AddKillers(Group,Grp->Owner,0);
                                }
                            }
                        }
                        
                        if(zf<0xFFFE){
                            int tx=-1;
                            int ty=-1;
                            
                            OneUnit OU;
                            OU.Index=0xFFFF;
                            if(DST!=0xFFFF&&GetUnitGlobalInfo(DST,&OU)){
                                tx=OU.x;
                                ty=OU.y;
                            }
                            
                            int r=GetTopDist(xc,yc,tx,ty);
                            
                            if(r<25){
                                int n=GetNUnits(Group);
                                for(int i=0;i<n;i++){
                                    OneUnit OU;
                                    if(GetUnitInfo(Group,i,&OU)){
                                        int rr=GetTopDist(OU.x,OU.y,tx,ty);
                                        if(rr<r){
                                            r=rr;
                                        }
                                    }
                                }
                            }
                            
                            if(r<12){
                                if(!find){
                                    Grp->StormID=OU.Index;
                                    Grp->StormSN=OU.Serial;
                                    
                                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                    SGP_ComeIntoBuilding(StrGrp,0,OU.Index);
                                }else{
                                    GAMEOBJ Z;
                                    Z.Index=xc;
                                    Z.Serial=yc;
                                    Z.Type='@   '-'   '+1000;
                                    
                                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                    AttackEnemyInZone2(Group,&Z,Owner);
                                }
                            }else{
                                GAMEOBJ Z;
                                Z.Index=xc;
                                Z.Serial=yc;
                                Z.Type='@   '-'   '+600;
                                
                                if(!find){
                                    if(zf!=top&&r>3){
                                        int x,y;
                                        bool Attack=false;
                                        if(r<30){
                                            GetBuildingEntryPoint(&x, &y, OU.Index, 64);
                                        }else{
                                            word* Way;
                                            int NWayPoint=GetLastFullWay(&Way);
                                            if(NWayPoint>2) GetTopZRealCoor(Way[NWayPoint-3],&x,&y);
                                            else GetTopZRealCoor(zf,&x,&y);
                                            
                                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                            Attack=AttackEnemyInZone2(&Grp->Group,&Z,Owner);
                                        }
                                        if(!Attack){
                                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                            SGP_MoveToPoint(Owner,Group,x,y,512,0,0,32);
                                        }
                                    }
                                }else{
                                    Z.Type='@   '-'   '+1400;
                                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                    AttackEnemyInZone2(&Grp->Group,&Z,Owner);
                                }
                            }
                        }else{
                            GAMEOBJ Z;
                            Z.Index=xc;
                            Z.Serial=yc;
                            Z.Type='@   '-'   '+1000;
                            
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            AttackEnemyInZone2(&Grp->Group,&Z,Owner);
                        }
                        Grp->LastTop=top;
                    }
                }
            }
        }
    }
}

void ProcessCannonRange(DiplomacySystem* dipSystem, int start, int end) {
    CannonGroup* CANNONS = dipSystem->CANNONS;
    int NCannons = dipSystem->NCannons;
    
    for(int i = start; i < end; i++) { 
        CannonGroup* Grp = CANNONS + i;
        GAMEOBJ* Group = &Grp->Group;
        int N = CleanGroup(Group);
        if(!N) continue;

        int xc, yc;
        if(GetGrpCenter(Group, &xc, &yc)) {
            GAMEOBJ Z;
            SetZone(&Z, xc, yc, 1000);
            Grp->DefForce = GetAlliesForce(&Z, Grp->Owner);

            int top = GetTopZone(xc, yc);
            if(top>=0&&top<GetNZones()){
                for(int j=0;j<NCannons;j++) if(j!=i){
                    CannonGroup* Allies=CANNONS+j;
                    if(GetNUnits(&Allies->Group)){
                        int dist=GetZonesDist(top,Allies->Top);
                        if(dist<10){
                            RemoveGroup(&Allies->Group,Group);
                        }
                    }
                }
            
                if(CheckIfNotBusy(Group)){
                    int ax=Grp->ArtDepoX;
                    int ay=Grp->ArtDepoY;
                    byte Owner=Grp->Owner;

                    // Large arrays moved to heap using std::vector
                    std::vector<short> Dang(4096);
                    std::vector<int> Fear(256);
                    for(int j=0;j<256;j++) Fear[j]=3;
                    CreateDangerMapWithoutPeasants(Owner,Dang.data(),GetNZones(),Fear.data(),4);
                    if(Grp->LastTop!=0xFFFF) Dang[Grp->LastTop]+=4000;

                    std::vector<word> IDS(2048);
                    CreateNonFiredEnemyBuildingsTopList(IDS.data(),Owner);
                    int maxdang;
                    word DST=0xFFFF;
                    
                    int zf=FindNextZoneOnTheSafeWayToObject(top,Dang.data(),IDS.data(),&maxdang,3,&DST);
                    int dx,dy;
                    if(DST!=0xFFFF&&GetTopZRealCoor(zf,&dx,&dy)){
                        word* WAY;
                        int NStep=GetLastFullWay(&WAY);
                        int DanTop=0xFFFF;
                        for(int s=0;s<NStep;s++){
                            int DT=WAY[s];
                            if(Dang[DT]>1000){
                                DanTop=DT;
                            }
                        }

                        int destnorm=10000;
                        int dnx=-1;
                        int dny=-1;
                        if(DanTop!=0xFFFF){
                            if(GetTopZRealCoor(DanTop,&dnx,&dny)){
                                destnorm=Norma(xc-dnx,yc-dny);
                            }
                        }else{
                            OneUnit u;
                            GetUnitGlobalInfo(DST,&u);
                            if(u.Index!=0xFFFF){
                                destnorm=Norma(xc-u.x,yc-u.y);
                            }
                        }

                        int artnorm=Norma(xc-ax,yc-ay);
                        word getDir(int dx, int dy);

                        if(artnorm>1000 && destnorm<5600){
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            
                            bool canFire = true;
                            
                            // Check for grapeshot friendly fire
                            if(DanTop!=0xFFFF && dnx!=-1){
                                // Calculate direction to grapeshot target
                                int dirToTarget = getDir(dnx - xc, dny - yc);
                                
                                // Count units in 30-degree cone (±15 degrees from center)
                                int totalUnitsInCone = 0;
                                int friendlyUnitsInCone = 0;
                                
                                // Check area within grapeshot range (100 units from target)
                                int MO = GetMaxObject();
                                for(int obj = 0; obj < MO; obj++){
                                    OneObject* OB = GetOBJ(obj);
                                    if(OB && !OB->Sdoxlo && !OB->NewBuilding && OB->LockType == 0){
                                        int ux = OB->RealX >> 4;
                                        int uy = OB->RealY >> 4;
                                        
                                        // Check if unit is near the grapeshot impact area
                                        int distToImpact = Norma(ux - dnx, uy - dny);
                                        if(distToImpact < 1000){
                                            // Calculate angle from cannon to this unit
                                            int dirToUnit = getDir(ux - xc, uy - yc);
                                            int angleDiff = abs(dirToUnit - dirToTarget);
                                            if(angleDiff > 128) angleDiff = 256 - angleDiff;
                                            
                                            // Check if unit is within 30-degree cone (15 degrees each side)
                                            if(angleDiff < 22){
                                                totalUnitsInCone++;
                                                if(OB->NNUM == Owner){
                                                    friendlyUnitsInCone++;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            // Check for friendly buildings in line of fire
                            if(canFire && DST!=0xFFFF){
                                OneUnit targetUnit;
                                GetUnitGlobalInfo(DST, &targetUnit);
                                if(targetUnit.Index != 0xFFFF){
                                    // Calculate direction to target
                                    int tx = targetUnit.x;
                                    int ty = targetUnit.y;
                                    int dirToTarget = getDir(tx - xc, ty - yc);
                                    
                                    // Check all zones along the path for friendly buildings
                                    int MO = GetMaxObject();
                                    for(int obj = 0; obj < MO; obj++){
                                        OneObject* OB = GetOBJ(obj);
                                        if(OB && !OB->Sdoxlo && OB->NewBuilding && OB->NNUM == Owner){
                                            int bx = OB->RealX >> 4;
                                            int by = OB->RealY >> 4;
                                            
                                            // Check if building is roughly in line of fire
                                            int dirToBuilding = getDir(bx - xc, by - yc);
                                            int angleDiff = abs(dirToBuilding - dirToTarget);
                                            if(angleDiff > 128) angleDiff = 256 - angleDiff;
                                            
                                            // If building is within 15 degrees of firing line
                                            if(angleDiff < 15){
                                                int distToBuilding = Norma(bx - xc, by - yc);
                                                int distToTarget = Norma(tx - xc, ty - yc);
                                                
                                                // If building is between cannon and target
                                                if(distToBuilding < distToTarget && distToBuilding < 5000){
                                                    int perpDist = distToBuilding * sin(angleDiff * 3.14159f / 128.0f);
                                                    
                                                    // If building is very close to line of fire (within 200 units)
                                                    if(perpDist < 200){
                                                        canFire = false;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            
                            // Fire only if safe
                            if(canFire){
                                if(DanTop!=0xFFFF && dnx!=-1){
                                    ArtAttackGroud(Owner,Group,dnx,dny);
                                }else{
                                    ArtAttackObject(Owner,Group,DST);
                                }
                            }else{
                                // Move closer instead of firing
                                SGP_MoveToPoint(Owner,Group,dx,dy,512,200,0,32);
                                Grp->LastTop=top;
                            }
                        }else{
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            SGP_MoveToPoint(Owner,Group,dx,dy,512,200,0,32);
                            Grp->LastTop=top;
                        }
                    }
                }
            }
        }
        Grp->Top = 0xFFFF;
    }
}

void ProcessFirerRange(DiplomacySystem* dipSystem, int start, int end, int T0) {
    FiringGroup* FIRERS = dipSystem->FIRERS;
    
    for(int i = start; i < end; i++) { 
        if(!CleanGroup(&FIRERS[i].Group)) continue;
        FiringGroup* Grp = FIRERS + i;
        GAMEOBJ* Group = &Grp->Group;
        int NU = CleanGroup(Group);
        if(!NU) continue;

        word BID = Grp->CurrentFiringBuilding;
        word SNS = Grp->FBSN;
        OneUnit OU;
        GetUnitGlobalInfo(BID, &OU);
        
        if(OU.Index != 0xFFFF && OU.Serial == SNS && T0 - Grp->LastMoveTime > 150) {
            Grp->LastMoveTime = T0;
            if(OU.Index == 0xFFFF || OU.NI == Grp->Owner || CheckIfBuildingIsFired(BID)) {
                int xc, yc;
                if(GetGrpCenter(&Grp->Group, &xc, &yc)) {
                    GAMEOBJ Z;
                    Z.Index = xc;
                    Z.Serial = yc;
                    Z.Type = '@   ' - '   ' + 1200;
                    
                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                    AttackEnemyInZone2(&Grp->Group, &Z, Grp->Owner);
                }
                Grp->CurrentFiringBuilding = 0xFFFF;
            } else {
                std::lock_guard<std::mutex> lock(g_gameApiMutex);
                GrpAttackObject(Grp->Owner, &Grp->Group, BID);
            }
        } else {
            if(T0-Grp->LastMoveTime>150){
                Grp->LastMoveTime=T0;

                SetToInternalResourcesZero(Group);
                int xc,yc;
                if(GetGrpCenter(&Grp->Group,&xc,&yc)){
                    int top=GetTopZone(xc,yc);
                    if(top>=0xFFFE){
                        OneUnit UN;
                        GetUnitInfo(Group,0,&UN);
                        xc=UN.x;
                        yc=UN.y;
                        top=GetTopZone(xc,yc);
                    }
                    if(top>=0&&top<GetNZones()){
                        byte Owner=FIRERS[i].Owner;
                        
                        // Large arrays moved to heap using std::vector
                        std::vector<short> Dang(4096);
                        std::vector<int> Fear(256);
                        std::vector<word> IDS(4096);
                        
                        for(int j=0;j<256;j++) Fear[j]=3;
                        Fear[1]=10;
                        CreateDangerMapForFire(Owner,Dang.data(),GetNZones(),Fear.data(),3);
                        if(Grp->LastTop!=0xFFFF) Dang[Grp->LastTop]+=4000;

                        bool find=false;
                        if(!find) CreateNonFiredEnemyBuildingsTopList(IDS.data(),Owner);

                        int maxdang;
                        word DST=0xFFFF;
                        int zf=FindNextZoneOnTheSafeWayToObject(top,Dang.data(),IDS.data(),&maxdang,3,&DST);
                        if(DST!=0xFFFF&&zf<0xFFFE){
                            int tx=-1;
                            int ty=-1;
                            if(!find){
                                if(GetUnitGlobalInfo(DST,&OU)){
                                    tx=OU.x;
                                    ty=OU.y;
                                }
                            }else{
                                GetTopZRealCoor(DST,&tx,&ty);
                            }
                            
                            if(tx!=-1){
                                int r=GetTopDist(xc,yc,tx,ty);
                                
                                if(r<30){
                                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                    GrpAttackObject(Owner,&Grp->Group,DST);
                                    Grp->CurrentFiringBuilding=OU.Index;
                                    Grp->FBSN=OU.Serial;
                                }else{
                                    GAMEOBJ Z;
                                    Z.Index=xc;
                                    Z.Serial=yc;
                                    Z.Type='@   '-'   '+600;
                                    if(zf!=top&&r>3){
                                        int x,y;
                                        
                                        word* Way;
                                        int NWayPoint=GetLastFullWay(&Way);
                                        if(NWayPoint>2) GetTopZRealCoor(Way[NWayPoint-3],&x,&y);
                                        else GetTopZRealCoor(zf,&x,&y);
                                        
                                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                        SGP_MoveToPoint(Owner,Group,x,y,512,128-GetRND(256),128-GetRND(256),32);
                                        Grp->LastTop=top;
                                    }
                                }
                            }
                        }else{
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            dipSystem->AddTomahawks(Group,Grp->Owner,0,0);
                        }
                    }
                }
            }
        }
    }
}

void ProcessKillerRange(KillersGroup* KILLERS, int start, int end, int T0) {
    for(int i = start; i < end; i++) {
        OneUnit OU;
        KillersGroup* Grp = KILLERS + i;
        GAMEOBJ* Group = &Grp->Group;
        int NUnits = CleanGroup(Group);
        if(!NUnits) continue;
        
        if(T0 - KILLERS[i].LastMoveTime <= 150) continue;
        
        Grp->LastMoveTime = T0;

        int xc, yc;
        if(GetGrpCenter(&KILLERS[i].Group, &xc, &yc)) {
            int top = GetTopZone(xc, yc);
            if(top >= 0xFFFE) {
                OneUnit UN;
                GetUnitInfo(Group, 0, &UN);
                xc = UN.x;
                yc = UN.y;
                top = GetTopZone(xc, yc);
            }
            if(top>=0&&top<GetNZones()){
                byte Owner=KILLERS[i].Owner;
                
                // Large arrays moved to heap using std::vector
                std::vector<short> Dang(4096);
                std::vector<int> Fear(256);
                std::vector<word> IDS(4096);
                
                for(int j=0;j<256;j++) Fear[j]=5;
                Fear[1]=10;
                
                CreateDangerMap(Owner,Dang.data(),GetNZones(),Fear.data(),3);
                if(KILLERS[i].LastTop!=0xFFFF) Dang[KILLERS[i].LastTop]+=4000;

                int MinPS=NUnits>>3;
                if(MinPS>12) MinPS=12;
                CreatePeasantsTopList(IDS.data(),Owner,MinPS,Grp->SeakMine);
                int maxdang;
                word DST=0xFFFF;
                int zf=FindNextZoneOnTheSafeWayToObject(top,Dang.data(),IDS.data(),&maxdang,3,&DST);
                bool FindPeasant=true;

                if(DST!=0xFFFF){
                    if(GetUnitGlobalInfo(DST,&OU)){
                        GAMEOBJ Z;
                        Z.Index=xc;
                        Z.Serial=yc;
                        Z.Type='@   '-'   '+1500;
                        int r=GetTopDist(xc,yc,OU.x,OU.y);
                        if(r<12){
                            Z.Index=OU.x;
                            Z.Serial=OU.y;
                            if(OU.Building){
                                Z.Type='@   '-'   '+800;
                                std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                if(!AttackEnemyInZone2(&KILLERS[i].Group,&Z,Owner)) {
                                    SGP_MoveToPoint(7,Group,OU.x,OU.y,512,0,0,0);
                                }
                            }else{
                                std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                if(FindPeasant) AttackEnemyPeasantsInZone(&KILLERS[i].Group,&Z,OU.NI);
                                else AttackEnemyInZone2(&KILLERS[i].Group,&Z,Owner);
                            }
                        }else{
                            SetToInternalResourcesZero(Group);
                            
                            int x,y;
                            if(r<25){
                                x=OU.x;
                                y=OU.y;
                            }else{
                                word* Way;
                                int NWayPoint=GetLastFullWay(&Way);
                                if(NWayPoint>2) GetTopZRealCoor(Way[NWayPoint-3],&x,&y);
                                else GetTopZRealCoor(zf,&x,&y);
                            }
                            
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            SGP_MoveToPoint(Owner,Group,x,y,512,128-GetRND(256),128-GetRND(256),32);
                            
                            KILLERS[i].LastMoveTime=T0+15;
                            KILLERS[i].LastTop=top;
                        }
                    }
                }else{
                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                    AddStorm(Group,Grp->Owner,2);
                }
            }
        }
    }
}

void ProcessTomahawkRange(DiplomacySystem* dipSystem, int start, int end, int T0) {
    for(int i = start; i < end; i++) {
        TomahawkGroup* TOMAHAWKS = dipSystem->TOMAHAWKS;
        StormGroup* STORMS = dipSystem->STORMS;
        int NStorms = dipSystem->NStorms;
        TomahawkGroup* Grp = TOMAHAWKS + i;
        int NMen = CleanGroup(&Grp->Group);
        if(!NMen) continue;
        
        GAMEOBJ* Group = &Grp->Group;
        
        if(T0 - Grp->LastMoveTime <= 150) continue;
        
        Grp->LastMoveTime = T0;

        int xc, yc;
        if(GetGrpCenter(&Grp->Group, &xc, &yc)) {
            Grp->x = xc;
            Grp->y = yc;
            int top = GetTopZone(xc, yc);
            if(top >= 0xFFFE) {
                OneUnit UN;
                GetUnitInfo(&Grp->Group, 0, &UN);
                xc = UN.x;
                yc = UN.y;
                top = GetTopZone(xc, yc);
            }
            if(top>=0&&top<GetNZones()){
                byte Owner=Grp->Owner;
                
                // Large arrays moved to heap using std::vector
                std::vector<short> Dang(4096);
                std::vector<int> Fear(256);
                std::vector<word> IDS(4096);
                
                for(int j=0;j<256;j++) Fear[j]=2;
                Fear[1]=100;
                CreateDangerMapForTom(Owner,Dang.data(),GetNZones(),Fear.data(),3);

                int maxdang;
                
                GAMEOBJ Zone;
                SetZone(&Zone,xc,yc,Grp->MaxAttR+300);
                int Force=GetGroupForce(&Grp->Group);
                int AliForce=-GetEnemyForce(&Zone,Owner);
                
                bool attacking=true;
                bool moving=true;
                bool thinktwice=false;
                bool retreat=false;

                // Shield search
                if(Grp->ShieldID>=0xFFFE||!GetNUnits(&STORMS[Grp->ShieldID].Group)){
                    // Initialize IDS vector with 0xFF
                    std::fill(IDS.begin(), IDS.end(), 0xFF);
                    for(int i=0;i<NStorms;i++){
                        StormGroup* GRP=STORMS+i;
                        if(GRP->Owner==Grp->Owner){
                            int ST=GRP->Top;
                            if(GetNUnits(&GRP->Group)+GetNUnits(&GRP->StrGrp)>0 && ST>=0 && ST<0xFFFE){
                                IDS[ST]=i;
                            }
                        }
                    }
                    word DST=0xFFFF;
                    int zf=FindNextZoneOnTheSafeWayToObject(top,Dang.data(),IDS.data(),&maxdang,5,&DST);
                    if(DST!=0xFFFF) Grp->ShieldID=DST;
                    else Grp->ShieldID=0xFFFF;
                }
                word DST=0xFFFF;
                if(Grp->ShieldID!=0xFFFF) DST=STORMS[Grp->ShieldID].Top;
                bool findstm=(DST!=0xFFFF);
                
                if(findstm){
                    int sx,sy;
                    if(GetTopZRealCoor(DST,&sx,&sy)){
                        if(Norma(sx-xc,sy-yc)<800) DST=0xFFFF;
                    }
                }

                SetZone(&Zone,xc,yc,Grp->MaxAttR+200);
                {
                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                    SetUnitsState(Group,1,0,0,0);
                }
                
                word BID=0xFFFF;
                if(T0-Grp->AttackTime>0&&(AliForce>=Force||GetRND(100)>30)) {
                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                    BID=MakeOneShotToBuild(&Zone,&Grp->Group,Owner);
                }
                
                bool building=false;
                if(BID!=0xFFFF){
                    OneObject* BOB=GetOBJ(BID);
                    if(BOB&&!(BOB->Sdoxlo||BOB->NNUM==7)){
                        building=true;
                    }
                }

                if(building&&Grp->Strelok) moving=false;
                
                int NIn=0;
                if(building){
                    if(GetBuildingOposit(BID,NIn)){
                        AliForce -= NIn*5;
                        NIn>>=1;
                        NIn++;
                    }
                }
                
                if(building&&NIn<NMen){
                    if(isUnitAbsorber(BID)){
                        int NStr=GetRND(GetDiff(Grp->Owner))+1;

                        int N=CleanGroup(Group);
                        OneUnit u;
                        for(int id=0;id<N&&NStr>0;id++){
                            GetUnitInfo(Group,id,&u);
                            bool ru=(GetRND(300)==0);
                            if(u.Index!=0xFFFF&&(u.ChargeProgress||ru)){
                                std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                InviteUnitIntoBuilding(BID,u.Index,0);
                                DeleteUnitFromGroup(Group,id);
                                if(ru) ru=0;
                                NStr--;
                            }
                        }
                    }else{
                        building=false;
                    }
                }

                if(moving){
                    if(AliForce>Force-(Force>>3)){
                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                        if(Grp->Strelok){
                            SetUnitsState(Group,1,0,1,0);
                        }else{
                            SetUnitsState(Group,1,0,1,0);
                        }
                        
                        SetZone(&Zone,xc,yc,Grp->MaxAttR+200);
                        if(AttackEnemyInZone2(&Grp->Group,&Zone,Owner)){
                            Grp->LastThinkTime=T0;
                        }
                    }else{
                        if(T0-Grp->LastThinkTime>100){
                            thinktwice=true;
                            attacking=false;
                            if(building) retreat=true;
                        }else{
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            SetUnitsState(Group,1,0,1,0);
                            if(!Grp->Strelok) AttackEnemyInZone2(&Grp->Group,&Zone,Owner);
                            moving=false;
                        }
                    }
                }
                
                if(Grp->LastTop!=0xFFFF&&!thinktwice) Dang[Grp->LastTop]+=4000;

                if(thinktwice&&T0-Grp->AttackTime>0){
                    SetZone(&Zone,xc,yc,Grp->MaxAttR+500);
                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                    if(Grp->Strelok) SetUnitsState(Group,1,0,0,0);
                    else SetUnitsState(Group,1,0,0,1);
                    int NArm;
                    bool at=AttackByTomahawks(&Grp->Group,&Zone,Owner,NArm);
                    
                    if(!at){
                        Grp->LastMoveTime=T0;
                    }
                }

                bool SearchEnemy=false;
                if(moving){
                    if(attacking && T0-Grp->AttackTime>0){
                        CreateTopListEnArmy(IDS.data(),Owner,NMen>>4);
                        SearchEnemy=true;
                    }else{
                        if(DST==0xFFFF){
                            CreateFriendBuildingsTopList(IDS.data(),Owner);
                            IDS[top]=0xFFFF;
                            if(Grp->LastTop!=0xFFFF) IDS[Grp->LastTop]=0xFFFF;
                        }
                    }
                    
                    int zf;
                    if(DST==0xFFFF||attacking) zf=FindNextZoneOnTheSafeWayToObject(top,Dang.data(),IDS.data(),&maxdang,5,&DST);
                    else zf=DST;

                    Grp->LastMoveTime=T0;

                    if(DST!=0xFFFF&&zf!=top){
                        int dx,dy;
                        
                        if(GetTopZRealCoor(zf,&dx,&dy)){
                            if(GetGroupCharge(Group)||!attacking){
                                if(!thinktwice){
                                    word* Way;
                                    int NWayPoint=GetLastFullWay(&Way);
                                    if(NWayPoint>2) GetTopZRealCoor(Way[NWayPoint-3],&dx,&dy);
                                }

                                std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                SGP_MoveToPoint(Owner,Group,dx,dy,512,128-GetRND(256),128-GetRND(256),32);
                            }
                            
                            if(moving&&attacking) Grp->LastTop=top;
                            else Grp->LastTop=0xFFFF;
                            
                            if(thinktwice){ 
                                Grp->LastMoveTime=T0-62;
                            }else if(retreat && !findstm && T0-Grp->AttackTime>0){
                                Grp->AttackTime=T0+256;
                            }else{
                                Grp->LastMoveTime=T0;
                            }
                        }
                    }else{
                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                        if(Grp->Strelok){
                            SetUnitsState(Group,1,0,0,0);
                        }else{
                            SetUnitsState(Group,1,0,1,0);
                        }
                        AttackEnemyInZone2(&Grp->Group,&Zone,Owner);
                    }
                }else{
                    Grp->LastMoveTime=T0;
                }

                if(!moving) Grp->LastMoveTime=T0-60;
            }
        }
    }
}

void DiplomacySystem::Process() {
    int T0 = GetGlobalTime();
    unsigned int numThreads = std::thread::hardware_concurrency();
    if(numThreads == 0) numThreads = 4;
    
    // AI assignment (keep as single-threaded)
    for(int i = 0; i < NDIPS; i++) {
        int x, y;
        if(DIPS[i]->Owner == 7 && GetGrpCenter(&DIPS[i]->CentralGroup, &x, &y)) {
            byte clr = 0xFF;
            int MinR = 10000;
            for(int c = 0; c < 7; c++) {
                if(AI_set[c]) {
                    int r = Norma(AIX[c] - x, AIY[c] - y);
                    if(r < MinR) {
                        MinR = r;
                        clr = c;
                    }
                }
            }
            if(clr != 0xFF) {
                word Data[8];
                Data[0] = 1;
                Data[1] = i;
                Data[2] = clr;
                Data[3] = 0xFFFF;
                SendDipCommand((char*)Data, 8);
            }
        }
    }
    
    // Process tribes (sequential)
    for(int i = 0; i < NDIPS; i++) {
        DIPS[i]->ProcessTribe();
    }
    
    // Process walking units (sequential)
    OneUnit OU;
    for(int i = 0; i < NWalk; i++) {
        bool erase = 1;
        if(GetUnitGlobalInfo(WID[i], &OU)) {
            if(WSN[i] == OU.Serial) {
                erase = 0;
                if(!OU.Busy) {
                    int xx = OU.x + GetRND(8192) - 4096;
                    int yy = OU.y + GetRND(8192) - 4096;
                    std::lock_guard<std::mutex> lock(g_gameApiMutex);
                    OBJ_SmartSendTo(WID[i], xx, yy, 0, 0, 128 + 16, 0);
                }
            }
        }
        if(erase) {
            if(i < NWalk - 1) {
                memcpy(WID + i, WID + i + 1, (NWalk - i - 1) << 1);
                memcpy(WSN + i, WSN + i + 1, (NWalk - i - 1) << 1);
            }
            i--;
            NWalk--;
        }
    }
    
    // MULTITHREADED PROCESSING
    
    // Process storms in parallel
    if(NStorms > 0) {
        std::vector<std::thread> threads;
        int stormsPerThread = (NStorms + numThreads - 1) / numThreads;
        
        for(unsigned int t = 0; t < numThreads; t++) {
            int start = t * stormsPerThread;
            int end = (start + stormsPerThread < NStorms) ? (start + stormsPerThread) : NStorms;
            if(start < end) {
                threads.emplace_back(ProcessStormRange, this, start, end, T0);
            }
        }
        
        for(auto& thread : threads) {
            thread.join();
        }
    }
    
    // Process cannons in parallel
    if(NCannons > 0) {
        std::vector<std::thread> threads;
        int cannonsPerThread = (NCannons + numThreads - 1) / numThreads;
        
        for(unsigned int t = 0; t < numThreads; t++) {
            int start = t * cannonsPerThread;
            int end = (start + cannonsPerThread < NCannons) ? (start + cannonsPerThread) : NCannons;
            if(start < end) {
                threads.emplace_back(ProcessCannonRange, this, start, end);
            }
        }
        
        for(auto& thread : threads) {
            thread.join();
        }
    }
    
    // Process firers in parallel
    if(NFirers > 0) {
        std::vector<std::thread> threads;
        int firersPerThread = (NFirers + numThreads - 1) / numThreads;
        
        for(unsigned int t = 0; t < numThreads; t++) {
            int start = t * firersPerThread;
            int end = (start + firersPerThread < NFirers) ? (start + firersPerThread) : NFirers;
            if(start < end) {
                threads.emplace_back(ProcessFirerRange, this, start, end, T0);
            }
        }
        
        for(auto& thread : threads) {
            thread.join();
        }
    }
    
    // Process killers in parallel
    if(NKillers > 0) {
        std::vector<std::thread> threads;
        int killersPerThread = (NKillers + numThreads - 1) / numThreads;
        
        for(unsigned int t = 0; t < numThreads; t++) {
            int start = t * killersPerThread;
            int end = (start + killersPerThread < NKillers) ? (start + killersPerThread) : NKillers;
            if(start < end) {
                threads.emplace_back(ProcessKillerRange, KILLERS, start, end, T0);
            }
        }
        
        for(auto& thread : threads) {
            thread.join();
        }
    }
    
    // Process tomahawks in parallel
    if(NTomahawks > 0) {
        std::vector<std::thread> threads;
        int tomahawksPerThread = (NTomahawks + numThreads - 1) / numThreads;
        
        for(unsigned int t = 0; t < numThreads; t++) {
            int start = t * tomahawksPerThread;
            int end = (start + tomahawksPerThread < NTomahawks) ? (start + tomahawksPerThread) : NTomahawks;
            if(start < end) {
                threads.emplace_back(ProcessTomahawkRange, this, start, end, T0);
            }
        }
        
        for(auto& thread : threads) {
            thread.join();
        }
    }
    
    // Resource setting (sequential)
    for(int i = 0; i < 6; i++) {
        SetResource(7, i, 1000000);
    }
    
    ProcessZoology();
}
#endif
bool DiplomacySystem::CreateContactsList(bool first){
	DString DST;
	DString One;
	if(first){
		DST.ReadFromFile("Dip\\ContactsList.cml");
		DST.Replace("$TRIBELIST$",GetTextByID("$TRIBELIST$"));
	};
	DST.Add("<FRM1>\n");
	int pp=0;
	for(int i=0;i<NDIPS;i++)if(DIPS[i]->Owner==GetMyNation()){
		DIP_SimpleBuilding* DSB=(DIP_SimpleBuilding*)DIPS[i];
		One.ReadFromFile("Dip\\ContactsListLine.cml");
		char ccc[128];
		strcpy(ccc,GetTextByID(DSB->NameOfTribe));
		if(DSB->Busy)strcat(ccc,"[B]");
		One.Replace("$TRIBELIST$",ccc);
		One.Replace("$IDX$",pp+1);
		One.Replace("$IDXX$",pp);
		One.Replace("$TRIBEINFO$",i);
		DST+=One;
		pp++;
	};
	DST.Add("\n<FRM1>");
	DST.WriteToFile("Dip\\tmp.cml");
	BusyPageIndex=-1;
	return pp!=0;
};
void DiplomacySystem::AddWalker(word ID){
	if(ID!=0xFFFF){
		OneUnit OU;
		if(GetUnitGlobalInfo(ID,&OU)){
			WID=(word*)realloc(WID,NWalk*2+2);
			WSN=(word*)realloc(WSN,NWalk*2+2);
			WID[NWalk]=ID;
			WSN[NWalk]=OU.Serial;
			NWalk++;
		};
	};
};
void DiplomacySystem::AddCannon(GAMEOBJ* Src,byte Owner){
	int N=CleanGroup(Src);
	if(N){
		OneUnit u;
		GetUnitInfo(Src,0,&u);
		if(u.Index!=0xFFFF){
			CANNONS=(CannonGroup*) realloc(CANNONS,sizeof(CannonGroup)*(NCannons+1));
			CannonGroup* Grp=CANNONS+NCannons;

			Grp->ArtDepoX=u.x;
			Grp->ArtDepoY=u.y;
			
			GAMEOBJ* Group=&Grp->Group;
			RegisterDynGroup(Group);
			RemoveGroup(Src,Group);
			Grp->Owner=Owner;

			int top=0xFFFF;
			int xc,yc;
			if(GetGrpCenter(Group,&xc,&yc)){
				top=GetTopZone(xc,yc);
			}
			Grp->Top=top;
			Grp->LastTop=0xFFFF;
			
			Grp->DefForce=0;

			NCannons++;
		}
	}
};
void DiplomacySystem::AddStorm(GAMEOBJ* Src, byte Owner, int Diff){
	if(!(Src&&Src->Type=='UNIT'&&CleanGroup(Src))) return;
	STORMS=(StormGroup*)realloc(STORMS,(NStorms+1)*sizeof(StormGroup));
	StormGroup* Grp=STORMS+NStorms;

	Grp->Owner=Owner;
	RegisterDynGroup(&Grp->Group);
	CopyUnits(Src,&Grp->Group,0,10000,0,1);
	RegisterDynGroup(&Grp->StrGrp);
	Grp->StormID=0xFFFF;
	
	Grp->Top=0xFFFF;
	Grp->LastTop=0xFFFF;

	Grp->LastMoveTime=0;

	Grp->CannonGuard=false;

	Grp->Difficulty=Diff;

	NStorms++;
};
void DiplomacySystem::AddFirers(GAMEOBJ* Src,byte Owner){
	if(!(Src->Type=='UNIT'&&CleanGroup(Src))) return;
	FIRERS=(FiringGroup*)realloc(FIRERS,(NFirers+1)*sizeof FiringGroup);
	RegisterDynGroup(&FIRERS[NFirers].Group);
	CopyUnits(Src,&FIRERS[NFirers].Group,0,10000,0,1);
	FIRERS[NFirers].Owner=Owner;
	FIRERS[NFirers].CurrentFiringBuilding=0xFFFF;
	FIRERS[NFirers].LastMoveTime=GetGlobalTime();
	FIRERS[NFirers].LastThinkTime=GetGlobalTime();

	FIRERS[NFirers].LastTop=0xFFFF;
	NFirers++;
};
void DiplomacySystem::AddKillers(GAMEOBJ* Src,byte Owner, bool SeakMine){
	if(!(Src->Type=='UNIT'&&CleanGroup(Src))) return;
	KILLERS=(KillersGroup*)realloc(KILLERS,(NKillers+1)*sizeof KillersGroup);	
	//SetUnitsState(Src,1,0,1,0);
	RegisterDynGroup(&KILLERS[NKillers].Group);	
	CopyUnits(Src,&KILLERS[NKillers].Group,0,10000,0,1);
	KILLERS[NKillers].Owner=Owner;
	KILLERS[NKillers].LastMoveTime=GetGlobalTime();
	KILLERS[NKillers].LastThinkTime=GetGlobalTime();

	KILLERS[NKillers].LastTop=0xFFFF;
	KILLERS[NKillers].SeakMine=SeakMine;
	NKillers++;
};
void DiplomacySystem::AddTomahawks(GAMEOBJ* Src,byte Owner,word Base,int DriftRadius, int CriticalMass){
	if(!(Src->Type=='UNIT'&&CleanGroup(Src))) return;
	TOMAHAWKS=(TomahawkGroup*)realloc(TOMAHAWKS,(NTomahawks+1)* sizeof TomahawkGroup);
	TomahawkGroup* Grp=TOMAHAWKS+NTomahawks;
	RegisterDynGroup(&Grp->Group);

	FilterUnitsByCategory(Src,&Grp->Group,2);
	if(GetNUnits(&Grp->Group)) Grp->Strelok=1;
		else Grp->Strelok=0;
	RemoveGroup(Src,&Grp->Group);
	
	GetGrpCenter(&Grp->Group,&Grp->x,&Grp->y);
	Grp->Owner=Owner;
	Grp->Base=Base;
	Grp->DRadius=DriftRadius;
	Grp->CMass=CriticalMass;
	Grp->LastMoveTime=GetGlobalTime();
	Grp->LastThinkTime=GetGlobalTime();
	
	SetUnitsState(&Grp->Group,1,0,0,1);
	Grp->AttackTime=GetGlobalTime();
	Grp->ShieldID=0xFFFF;

	OneUnit OU;
	GetUnitInfo(&Grp->Group,0,&OU);
	if(OU.Index!=0xFFFF){
		Grp->MaxAttR=GetMaxAttRadius(OU.Index);
	}else{
		Grp->MaxAttR=500;
	}

	Grp->LastTop=0xFFFF;
	NTomahawks++;
};
extern DiplomacySystem DIPS;
CEXPORT 
void AddStorm(GAMEOBJ* Src, byte Owner, int Diff=2){
	DIPS.AddStorm(Src,Owner,Diff);
}
CEXPORT 
void AddCannon(GAMEOBJ* Src, byte Owner){
	DIPS.AddCannon(Src,Owner);
}
CEXPORT 
void AddTomahawks(GAMEOBJ* Src,byte Owner,word Base,int DriftRadius, int CriticalMass){
	DIPS.AddTomahawks(Src,Owner,Base,DriftRadius,CriticalMass);
}
CEXPORT
void AddFirers(GAMEOBJ* Src,byte Owner){
	DIPS.AddFirers(Src,Owner);
};
CEXPORT
void AddPsKillers(GAMEOBJ* Src,byte Owner,bool SeakMine=true){
	DIPS.AddKillers(Src,Owner,SeakMine);
};
void DiplomacySystem::AddNewZooGroup(ZooGroup* ZGrp){
	ZOO=(ZooGroup*)realloc(ZOO,(NZOO+1)*sizeof ZooGroup);
	ZOO[NZOO]=*ZGrp;
	NZOO++;
};
void DiplomacySystem::AddZooBirthZone(ZooBirthZone* BZone){
	BZOO=(ZooBirthZone*)realloc(BZOO,(NBZOO+1)*sizeof ZooBirthZone);
	BZOO[NBZOO]=*BZone;
	NBZOO++;
};
CEXPORT
bool AddAnimals(GAMEOBJ* Units,GAMEOBJ* Zone){
	GAMEOBJ UNI=*Units;
	GAMEOBJ ZON=*Zone;
	if(UNI.Type!='UNIT'||!(ZON.Type=='ZONE'||(ZON.Type&0xFF000000)==('@   '-0x202020))) return false;
	ZooBirthZone ZBZ;
	memset(&ZBZ,0,sizeof ZBZ);
	ZBZ.Zone=ZON;
	int NU=GetNUnits(&UNI);
	OneUnit OU;
	int nur=0;
	if(NU){
		for(int p=0;p<NU;p++){
			if(GetUnitInfo(&UNI,p,&OU)){
				ZBZ.NIndex[nur]=OU.NIndex;
				nur++;
			};
		};
		if(nur){
			for(int i=nur;i<8;i++)ZBZ.NIndex[i]=ZBZ.NIndex[i%nur];
			ZBZ.Type=ZBZ.NIndex[0];
			DIPS.AddZooBirthZone(&ZBZ);
			ZooGroup ZG;
			memset(&ZG,0,sizeof ZG);
			ZG.Type=ZBZ.NIndex[0];			
			DIPS.StartPopulation[ZG.Type]+=1+(NU>>2);
			ZG.Group=UNI;
			ZG.MaxAmount=GetRND(6)+2;
			ZG.MotionFrequency=GetRND(400)+500;
			if(GetGrpCenter(&UNI,&ZG.xc,&ZG.yc)){
				DIPS.AddNewZooGroup(&ZG);
			};
		};
	};
	return true;
}
#ifdef NOTMULTITHREAD
void DiplomacySystem::ProcessZoology(){
	if(DMSize!=GetNZones()){
		free(DangerMap);
		DangerMap=znew(short,GetNZones());
		DMSize=GetNZones();
		memset(DangerMap,0,GetNZones()<<1);
	};
	if(GetRND(100)<10){
		//processing birth
		if(NBZOO){
			int RZoo=GetRND(NBZOO);
			bool born=0;
			ZooBirthZone* ZBZ=BZOO+RZoo;
			int zx=0;
			int zy=0;
			GetZoneCoor(&ZBZ->Zone,&zx,&zy);
			if(zx&&!CheckLandLocking(zx,zy,0,0)){
				GAMEOBJ UTP;
				UTP.Serial=0;
				UTP.Type='UTYP';
				for(int i=0;i<8&&!born;i++){
					UTP.Index=ZBZ->NIndex[i];
					if(UTP.Index<2048){
						int N=GetTotalAmount1(&UTP,7);
						if(N<StartPopulation[UTP.Index]){
							born=1;
							ZooGroup* ZG;
							//search for free zoo group
							for(int i=0;i<NZOO;i++){
								if(!GetTotalAmount0(&ZOO[i].Group))break;
							};
							bool newz=0;
							ZooGroup ZG0;
							if(i<NZOO){
								ZG=ZOO+i;
								memset(&ZG->xc,0,sizeof(ZooGroup)-sizeof(GAMEOBJ));
							}else{
								memset(&ZG0,0,sizeof ZG0);
								RegisterDynGroup(&ZG0.Group);
								newz=1;
								ZG=&ZG0;
							};
							ZG->Attract=1;
							ZG->MaxAmount=GetRND(6)+2;
							ZG->MotionFrequency=GetRND(400)+500;
							//ZG->MaxAmount=3;
							//ZG->MotionFrequency=500;
							ZG->LastMotionTime=GetGlobalTime();
							ZG->Type=UTP.Index;//ZBZ->Type;
							GAMEOBJ ODIN;
							RegisterFormation(&ODIN,"#ODIN");
							CreateObject0(&ZG->Group,&ODIN,&UTP,7,&ZBZ->Zone,GetRND(256));
							GetGrpCenter(&ZG->Group,&ZG->xc,&ZG->yc);
							if(newz)AddNewZooGroup(ZG);
						};
					};
				};
			};
		};
	};
	{
		//processing motion of animals
		ZooGroup* ZG=ZOO;
		int NTZ=GetNZones();
		if(GetRND(100)<10){
			//fear&danger map
			int Fear[256];
			for(int i=0;i<256;i++)Fear[i]=32;
			CreateDangerMap(7,DangerMap,NTZ,Fear,3);
		};
		word IDXS[4096];
		word TYPE[4096];
		memset(IDXS,0xFF,GetNZones()<<1);
		//list of purposes
		for(int i=0;i<NZOO;i++){//if(GetRND(100)<30){
			if(!ZG->Empty){
				int N=GetTotalAmount0(&ZG->Group);
				if(!N)ZG->Empty=1;
				if(!ZG->Empty){
					GetGrpCenter(&ZG->Group,&ZG->xc,&ZG->yc);
					if(N<ZG->MaxAmount){
						int tz=GetTopZone(ZG->xc,ZG->yc);
						if(tz<NTZ){
							IDXS[tz]=i;
							TYPE[tz]=ZG->Type;
						};
					};
				};
			};
			ZG++;
		};
		ZG=ZOO;
		int CTIME=GetGlobalTime();
		for(int i=0;i<NZOO;i++){
			if(CheckIfNotBusy(&ZG->Group)){
				if(!ZG->Empty){
					if(ZG->Attract){
						//otstal ot stada
						int tz=GetTopZone(ZG->xc,ZG->yc);
						if(tz<NTZ){
							int IDX=IDXS[tz];
							IDXS[tz]=0xFFFF;
							int DNG;
							word DST;
							int tzn=FindNextZoneOnTheSafeWayToLimitedObject(tz,DangerMap,IDXS,TYPE,ZG->Type,&DNG,0,&DST);
							IDXS[tz]=IDX;
							if(tzn<NTZ){
								ZooGroup* ZG1=ZOO+DST;
								if(Norma(ZG->xc-ZG1->xc,ZG->yc-ZG1->yc)<1000){
									//uniting with other group
									CopyUnits(&ZG->Group,&ZG1->Group,0,1000,1,1);
									ZG->Empty=1;
									ZG1->Attract=0;
								}else{
									int x,y;
									GetTopZRealCoor(tzn,&x,&y);
									RemoveAttackingUnits(&ZG->Group);
									SGP_MakeOneStepTo(7,&ZG->Group,x,y,GetRND(256),0);
								};
							}else{
								int tz=GetTopZone(ZG->xc,ZG->yc);
								if(tz<NTZ){
									word* List;
									int N=GetListOfNearZones(tz,&List);
									if(N){
										int tz1=List[GetRND(N)<<1];
										int x,y;
										GetTopZRealCoor(tz1,&x,&y);
										//SGP_MakeOneStepTo(7,&ZG->Group,x,y,GetRND(256),0);
										int N=GetNUnits(&ZG->Group);
										OneUnit OU;
										for(int p=0;p<N;p++){
											if(GetUnitInfo(&ZG->Group,p,&OU)){
												word ID=0xFFFF;
												word SN;
												GetTargetObject(OU.Index, &ID, &SN);
												if(ID!=0xFFFF) OBJ_SendTo(OU.Index,x+GetRND(400)-200,y+GetRND(400)-200,1+128,0);
												else{
													ZG->Attract=0;
													break;
												}
											};
										};
										ZG->LastMotionTime=CTIME;
									};
								};
							};
						};
					}else{						
						// free motion
						if(CTIME-ZG->LastMotionTime>ZG->MotionFrequency){
							// group							
							int tz=GetTopZone(ZG->xc,ZG->yc);
							if(tz<NTZ){
								word* List;
								int N=GetListOfNearZones(tz,&List);
								if(N){
									//searching for 2 most dangerous zones
									int MAXD1=0;
									int MAXD2=0;
									int Z1=-1,Z2=-1;
									for(int p=0;p<N;p++){
										int tz=List[p+p];
										int D=DangerMap[tz];
										if(D>MAXD1){
											MAXD2=MAXD1;
											Z2=Z1;
											MAXD1=D;
											Z1=tz;
										}else{
											if(D>MAXD2){
												MAXD2=D;
												Z2=tz;
											};
										};
									};
									int tz1=-1;
									int nac=0;
									do{
										tz1=List[GetRND(N)<<1];
										nac++;
									}while((tz1==Z1||tz1==Z2)&&nac<20);
									int x,y;
									GetTopZRealCoor(tz1,&x,&y);
									//SGP_MakeOneStepTo(7,&ZG->Group,x,y,GetRND(256),0);
									int N=GetNUnits(&ZG->Group);
									OneUnit OU;
									for(int p=0;p<N;p++){
										if(GetUnitInfo(&ZG->Group,p,&OU)){
											OBJ_SendTo(OU.Index,x+GetRND(400)-200,y+GetRND(400)-200,1+128,0);
										};
									};
									ZG->LastMotionTime=CTIME;
									ZG->MotionFrequency=GetRND(300)+900;
								};
							};
						}else{
							// personal motion
							int N=GetNUnits(&ZG->Group);
							OneUnit OU;
							for(int p=0;p<N;p++){
								if(GetUnitInfo(&ZG->Group,p,&OU)&&!OU.Busy&&!GetRND(30)){									
									OBJ_SendTo(OU.Index,OU.x+GetRND(400)-200,OU.y+GetRND(400)-200,1+128,0);
								};
							};
						}
					};
				};
			};
			ZG++;
		};
	};
};
#else
void DiplomacySystem::ProcessZoology() {
    if (DMSize != GetNZones()) {
        free(DangerMap);
        DangerMap = znew(short, GetNZones());
        DMSize = GetNZones();
        memset(DangerMap, 0, GetNZones() << 1);
    };
    
    if (GetRND(100) < 10) {
        // processing birth
        if (NBZOO) {
            int RZoo = GetRND(NBZOO);
            bool born = 0;
            ZooBirthZone* ZBZ = BZOO + RZoo;
            int zx = 0;
            int zy = 0;
            GetZoneCoor(&ZBZ->Zone, &zx, &zy);
            if (zx && !CheckLandLocking(zx, zy, 0, 0)) {
                GAMEOBJ UTP;
                UTP.Serial = 0;
                UTP.Type = 'UTYP';
                for (int i = 0; i < 8 && !born; i++) {
                    UTP.Index = ZBZ->NIndex[i];
                    if (UTP.Index < 2048) {
                        int N = GetTotalAmount1(&UTP, 7);
                        if (N < StartPopulation[UTP.Index]) {
                            born = 1;
                            ZooGroup* ZG;
                            // search for free zoo group
                            for (int i = 0; i < NZOO; i++) {
                                if (!GetTotalAmount0(&ZOO[i].Group)) break;
                            };
                            bool newz = 0;
                            ZooGroup ZG0;
                            if (i < NZOO) {
                                ZG = ZOO + i;
                                memset(&ZG->xc, 0, sizeof(ZooGroup) - sizeof(GAMEOBJ));
                            } else {
                                memset(&ZG0, 0, sizeof ZG0);
                                RegisterDynGroup(&ZG0.Group);
                                newz = 1;
                                ZG = &ZG0;
                            };
                            ZG->Attract = 1;
                            ZG->MaxAmount = GetRND(6) + 2;
                            ZG->MotionFrequency = GetRND(400) + 500;
                            // ZG->MaxAmount=3;
                            // ZG->MotionFrequency=500;
                            ZG->LastMotionTime = GetGlobalTime();
                            ZG->Type = UTP.Index; // ZBZ->Type;
                            GAMEOBJ ODIN;
                            RegisterFormation(&ODIN, "#ODIN");
                            CreateObject0(&ZG->Group, &ODIN, &UTP, 7, &ZBZ->Zone, GetRND(256));
                            GetGrpCenter(&ZG->Group, &ZG->xc, &ZG->yc);
                            if (newz) AddNewZooGroup(ZG);
                        };
                    };
                };
            };
        };
    };
    
    {
        // processing motion of animals
        ZooGroup* ZG = ZOO;
        int NTZ = GetNZones();
        if (GetRND(100) < 10) {
            // fear&danger map
            std::vector<int> Fear(256);
            for (int i = 0; i < 256; i++) Fear[i] = 32;
            CreateDangerMap(7, DangerMap, NTZ, Fear.data(), 3);
        };
        
        // Large arrays moved to heap using std::vector
        std::vector<word> IDXS(4096);
        std::vector<word> TYPE(4096);
        
        // Initialize IDXS with 0xFF
        std::fill(IDXS.begin(), IDXS.end(), 0xFF);
        
        // list of purposes
        for (int i = 0; i < NZOO; i++) { // if(GetRND(100)<30){
            if (!ZG->Empty) {
                int N = GetTotalAmount0(&ZG->Group);
                if (!N) ZG->Empty = 1;
                if (!ZG->Empty) {
                    GetGrpCenter(&ZG->Group, &ZG->xc, &ZG->yc);
                    if (N < ZG->MaxAmount) {
                        int tz = GetTopZone(ZG->xc, ZG->yc);
                        if (tz < NTZ) {
                            IDXS[tz] = i;
                            TYPE[tz] = ZG->Type;
                        };
                    };
                };
            };
            ZG++;
        };
        
        ZG = ZOO;
        int CTIME = GetGlobalTime();
        for (int i = 0; i < NZOO; i++) {
            if (CheckIfNotBusy(&ZG->Group)) {
                if (!ZG->Empty) {
                    if (ZG->Attract) {
                        // otstal ot stada
                        int tz = GetTopZone(ZG->xc, ZG->yc);
                        if (tz < NTZ) {
                            int IDX = IDXS[tz];
                            IDXS[tz] = 0xFFFF;
                            int DNG;
                            word DST;
                            int tzn = FindNextZoneOnTheSafeWayToLimitedObject(tz, DangerMap, IDXS.data(), TYPE.data(), ZG->Type, &DNG, 0, &DST);
                            IDXS[tz] = IDX;
                            if (tzn < NTZ) {
                                ZooGroup* ZG1 = ZOO + DST;
                                if (Norma(ZG->xc - ZG1->xc, ZG->yc - ZG1->yc) < 1000) {
                                    // uniting with other group
                                    CopyUnits(&ZG->Group, &ZG1->Group, 0, 1000, 1, 1);
                                    ZG->Empty = 1;
                                    ZG1->Attract = 0;
                                } else {
                                    int x, y;
                                    GetTopZRealCoor(tzn, &x, &y);
                                    RemoveAttackingUnits(&ZG->Group);
                                    SGP_MakeOneStepTo(7, &ZG->Group, x, y, GetRND(256), 0);
                                };
                            } else {
                                int tz = GetTopZone(ZG->xc, ZG->yc);
                                if (tz < NTZ) {
                                    word* List;
                                    int N = GetListOfNearZones(tz, &List);
                                    if (N) {
                                        int tz1 = List[GetRND(N) << 1];
                                        int x, y;
                                        GetTopZRealCoor(tz1, &x, &y);
                                        // SGP_MakeOneStepTo(7,&ZG->Group,x,y,GetRND(256),0);
                                        int N = GetNUnits(&ZG->Group);
                                        OneUnit OU;
                                        for (int p = 0; p < N; p++) {
                                            if (GetUnitInfo(&ZG->Group, p, &OU)) {
                                                word ID = 0xFFFF;
                                                word SN;
                                                GetTargetObject(OU.Index, &ID, &SN);
                                                if (ID != 0xFFFF) OBJ_SendTo(OU.Index, x + GetRND(400) - 200, y + GetRND(400) - 200, 1 + 128, 0);
                                                else {
                                                    ZG->Attract = 0;
                                                    break;
                                                }
                                            };
                                        };
                                        ZG->LastMotionTime = CTIME;
                                    };
                                };
                            };
                        };
                    } else {
                        // free motion
                        if (CTIME - ZG->LastMotionTime > ZG->MotionFrequency) {
                            // group
                            int tz = GetTopZone(ZG->xc, ZG->yc);
                            if (tz < NTZ) {
                                word* List;
                                int N = GetListOfNearZones(tz, &List);
                                if (N) {
                                    // searching for 2 most dangerous zones
                                    int MAXD1 = 0;
                                    int MAXD2 = 0;
                                    int Z1 = -1, Z2 = -1;
                                    for (int p = 0; p < N; p++) {
                                        int tz = List[p + p];
                                        int D = DangerMap[tz];
                                        if (D > MAXD1) {
                                            MAXD2 = MAXD1;
                                            Z2 = Z1;
                                            MAXD1 = D;
                                            Z1 = tz;
                                        } else {
                                            if (D > MAXD2) {
                                                MAXD2 = D;
                                                Z2 = tz;
                                            };
                                        };
                                    };
                                    int tz1 = -1;
                                    int nac = 0;
                                    do {
                                        tz1 = List[GetRND(N) << 1];
                                        nac++;
                                    } while ((tz1 == Z1 || tz1 == Z2) && nac < 20);
                                    int x, y;
                                    GetTopZRealCoor(tz1, &x, &y);
                                    // SGP_MakeOneStepTo(7,&ZG->Group,x,y,GetRND(256),0);
                                    int N = GetNUnits(&ZG->Group);
                                    OneUnit OU;
                                    for (int p = 0; p < N; p++) {
                                        if (GetUnitInfo(&ZG->Group, p, &OU)) {
                                            OBJ_SendTo(OU.Index, x + GetRND(400) - 200, y + GetRND(400) - 200, 1 + 128, 0);
                                        };
                                    };
                                    ZG->LastMotionTime = CTIME;
                                    ZG->MotionFrequency = GetRND(300) + 900;
                                };
                            };
                        } else {
                            // personal motion
                            int N = GetNUnits(&ZG->Group);
                            OneUnit OU;
                            for (int p = 0; p < N; p++) {
                                if (GetUnitInfo(&ZG->Group, p, &OU) && !OU.Busy && !GetRND(30)) {
                                    OBJ_SendTo(OU.Index, OU.x + GetRND(400) - 200, OU.y + GetRND(400) - 200, 1 + 128, 0);
                                };
                            };
                        }
                    };
                };
            };
            ZG++;
        };
    };
}
#endif
