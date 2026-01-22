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
#include "TopoGraf.h"
#include "HashTop.h"
#include <CrtDbg.h>
#include "fonts.h"
#include "LoadSave.h"

//MediaTop GTOP[2];

Radio Rarr[RRad];
void CreateRadio(){
	for(int ix=-RRad;ix<=RRad;ix++){
		for(int iy=-RRad;iy<=RRad;iy++){
			int r=int(sqrt(ix*ix+iy*iy));
			if(r<RRad)Rarr[r].N++;
		};
	};
	for(int i=0;i<RRad;i++){
		if(Rarr[i].N){
			Rarr[i].xi=znew(char,Rarr[i].N);
			Rarr[i].yi=znew(char,Rarr[i].N);
			Rarr[i].N=0;
		};
	};
	for(int ix=-RRad;ix<=RRad;ix++){
		for(int iy=-RRad;iy<=RRad;iy++){
			int r=int(sqrt(ix*ix+iy*iy));
			if(r<RRad){
				int N=Rarr[r].N;
				Rarr[r].xi[N]=ix;
				Rarr[r].yi[N]=iy;
				Rarr[r].N++;
			};
		};
	};
}
void ClearRadio(){
	for(int i=0;i<RRad;i++){
		free(Rarr[i].xi);
		free(Rarr[i].yi);
	};
};

void CreateWTopMap();
void CreateLinkInfo();

word* TopRef;
Area* TopMap;
int   NAreas;
int MaxArea;
word* MotionLinks;
word* LinksDist;
//#define MinNorm (7<<(ADDSH-1))

bool AddArea(short x,short y,byte Sliv){
	int MinNorm;
#ifndef HASH_TOP
	if(GetNAreas()>1016)return false;
	switch(ADDSH){
	case 1:
		MinNorm=7;
		break;
	case 2:
		MinNorm=10;
		break;
	case 3:
		MinNorm=15;
		break;
	};
#else 
	switch(ADDSH){
	case 1:
		MinNorm=4;
		break;
	case 2:
		MinNorm=7;
		break;
	case 3:
		MinNorm=10;
		break;
	};	
#endif //HASH_TOP
	if(Sliv!=2){
		int NA=GetNAreas();
		for(int i=0;i<NA;i++){
			Area* AR=GetTopMap(i);
			if(Norma(AR->x-x,AR->y-y)<=MinNorm){
				if(Sliv){
					AR->x=(AR->x+x)>>1;
					AR->y=(AR->y+y)>>1;
				};
				return false;
			};
		};
	};
	if(GetNAreas()>=MaxArea){
		MaxArea+=120;
		TopMap=(Area*)realloc(TopMap,MaxArea* sizeof(Area));
	};
	Area* AR=GetTopMap(NAreas);
	AR->x=x;
	AR->y=y;
	AR->Importance=0;
	AR->NTrees=0;
	AR->NStones=0;
	AR->MinesIdx=NULL;
	AR->NLinks=0;
	AR->NMines=0;
	AR->MaxLink=6;
	AR->Link=znew(word,AR->MaxLink<<1);
	NAreas++;
	return true;
};
int GetLinkScale(int N1,int N2);
void AddLink(int N1,int N2){
	Area* AR=GetTopMap(N1);
	int N=AR->NLinks;
	for(int i=0;i<N;i++){
		if(AR->Link[i+i]==N2)return;
	};
	if(AR->NLinks==AR->MaxLink){
		AR->MaxLink+=4;
		AR->Link=(word*)realloc(AR->Link,AR->MaxLink<<2);
	};
	N=AR->NLinks;
	AR->Link[N+N]=N2;
	Area* A2=GetTopMap(N2);
	int SC=GetLinkScale(N1,N2);
	AR->Link[N+N+1]=(Norma(AR->x-A2->x,AR->y-A2->y)*SC)>>4;
	AR->NLinks++;
};
extern int MAXSPR;
void EraseAreas();
bool GetTCStatus(int x,int y){
	int xxx=x<<2;
	int yyy=y<<2;
	if(!MFIELDS->CheckBar(xxx,yyy,4,4))return true;
	else{
		//accurate check
		if( ( (!CheckPt(xxx+1,yyy))||(!CheckPt(xxx+2,yyy+1))||(!CheckPt(xxx+3,yyy+2)) )||
			( (!CheckPt(xxx,yyy+1))||(!CheckPt(xxx+1,yyy+2))||(!CheckPt(xxx+2,yyy+3)) )||
			( (!CheckPt(xxx+2,yyy))||(!CheckPt(xxx+1,yyy+1))||(!CheckPt(xxx,yyy+2  )) )||
			( (!CheckPt(xxx+3,yyy+1))||(!CheckPt(xxx+2,yyy+2))||(!CheckPt(xxx+1,yyy+3)) )
		  )return true;
	};
	return false;
};
void CheckBright();
void ResearchIslands();
void CreateCostPlaces();
void StopDynamicalTopology();
bool ProcessMessages();
void PreCreateTopLinks();

void CreateAreas(){
	int i;

#ifdef HASH_TOP
	for( i=0;i<NMFIELDS;i++){
		HashTable[i].CreateAreas();
	}
	return;
#endif //HASH_TOP

	StopDynamicalTopology();
	EraseAreas();
	MaxArea=200;
	rpos=0;
	TopMap=znew(Area,MaxArea);
	NAreas=0;
	//creating ref. map
	memset(TopRef,0xFFFF,TopLx*TopLx*2);
	int mmx=msx>>1;
	int mmy=msy>>1;
	for(int x=0;x<mmx;x++)
		for(int y=0;y<mmy;y++){
			if(GetTCStatus(x,y))TopRef[x+y*TopLx]=0xFFFE;
		};
	/*
	for(x=0;x<mmx;x++)
		for(int y=0;y<mmy;y++){
			if(x>0&&y>0&&x<mmx-1&&y<mmx-1&&!MFIELDS[1].CheckBar(x<<2,y<<2,4,4)){
				int ofs=x+y*TopLx;
				TopRef[ofs]=0xFFFD;
				if(TopRef[ofs-1]==0xFFFE)TopRef[ofs-1]=0xFFFF;
				if(TopRef[ofs+1]==0xFFFE)TopRef[ofs+1]=0xFFFF;
				if(TopRef[ofs-TopLx]==0xFFFE)TopRef[ofs-TopLx]=0xFFFF;
				if(TopRef[ofs+TopLx]==0xFFFE)TopRef[ofs+TopLx]=0xFFFF;
				if(TopRef[ofs-TopLx]==0xFFFE)TopRef[ofs-TopLx]=0xFFFF;
				if(TopRef[ofs-TopLx+1]==0xFFFE)TopRef[ofs-TopLx+1]=0xFFFF;
				if(TopRef[ofs-TopLx-1]==0xFFFE)TopRef[ofs-TopLx-1]=0xFFFF;
				if(TopRef[ofs+TopLx+1]==0xFFFE)TopRef[ofs+TopLx+1]=0xFFFF;
				if(TopRef[ofs+TopLx-1]==0xFFFE)TopRef[ofs+TopLx-1]=0xFFFF;
			};
		};
	for(x=0;x<mmx;x++)
		for(int y=0;y<mmy;y++){
			int ofs=x+y*TopLx;
			if(TopRef[ofs]==0xFFFD)TopRef[ofs]=0xFFFE;
		};
	*/
	int N1=0;
	PreCreateTopLinks();
	//assert(_CrtCheckMemory());
	for( i=0;i<MAXSPR;i++){
		OneSprite* OS=&Sprites[i];
		if(OS->Enabled){
			SprGroup* SG=OS->SG;
			ObjCharacter* OC=&SG->ObjChar[OS->SGIndex];
			if(SG==&TREES&&OS->SGIndex==SG->NSpr-1)AddArea(OS->x>>6,OS->y>>6,2);
		};
	};
	/*
	for( i=0;i<MAXSPR;i++){
		OneSprite* OS=&Sprites[i];
		if(OS->Enabled){
			SprGroup* SG=OS->SG;
			ObjCharacter* OC=&SG->ObjChar[OS->SGIndex];
			if(OC->IntResType<8)AddArea(OS->x>>6,OS->y>>6,0);
		};
	};
	*/
	int V0=rando();
	do{
		int VX=(rando()+V0)&32767;
		if(VX<1024)V0+=rando();
		int x=((int(rando()+V0)&32767)*(mmx-4))>>15;
		int y=((int(rando()+V0)&32767)*(mmy-4))>>15;
		bool Empty=true;
		for(int dx=0;dx<=2&&Empty;dx++)
			for(int dy=0;dy<=2&&Empty;dy++){
				if(TopRef[x+dx+(y+dy)*TopLx]!=0xFFFE)Empty=false;
			};
		if(Empty){
			if(AddArea(x+1,y+1,0))N1=0;
			else N1++;
		};
	}while(N1<500);
	ProcessMessages();
	//assert(_CrtCheckMemory());
	//creating linking between areas
	int NA=GetNAreas();
	for( i=0;i<NA;i++){
		Area* AR=GetTopMap(i);
		TopRef[AR->x+int(AR->y)*TopLx]=i;
	}
	bool Change=false;
	//word TempTop[TopLx*TopLy];
	Change=true;
	for(i=1;i<RRad&&Change;i++){
		//memcpy(TempTop,TopRef,sizeof(TopRef));
		Change=false;
		char* xi=Rarr[i].xi;
		char* yi=Rarr[i].yi;
		int N=Rarr[i].N;
		for(int j=0;j<NA;j++){
			Area* AR=GetTopMap(j);
			int xx=AR->x;
			int yy=AR->y;
			for(int k=0;k<N;k++){
				int x=xx+xi[k];
				int y=yy+yi[k];
				if(x>0&&y>0&&x<mmx&&y<mmy){
					int ofst=x+y*TopLx;
					word ZZ=GetTopRef(ofst	  );
					word ZU=GetTopRef(ofst-TopLx);
					word ZD=GetTopRef(ofst+TopLx);
					word ZL=GetTopRef(ofst-1    );
					word ZR=GetTopRef(ofst+1    );
					if(ZZ==0xFFFE){
						if(ZU==j||ZD==j||ZL==j||ZR==j||
							GetTopRef(ofst-TopLx-1)==j||
							GetTopRef(ofst-TopLx+1)==j||
							GetTopRef(ofst+TopLx-1)==j||
							GetTopRef(ofst+TopLx+1)==j){
							TopRef[ofst]=j;
							Change=true;
							ZZ=j;
						};
					};
					if(ZZ<0xFFFE){
						if(ZU!=ZZ&&ZU<0xFFFE){
							AddLink(ZZ,ZU);
							AddLink(ZU,ZZ);
						};
						if(ZD!=ZZ&&ZD<0xFFFE){
							AddLink(ZZ,ZD);
							AddLink(ZD,ZZ);
						};
						if(ZL!=ZZ&&ZL<0xFFFE){
							AddLink(ZZ,ZL);
							AddLink(ZL,ZZ);
						};
						if(ZR!=ZZ&&ZR<0xFFFE){
							AddLink(ZZ,ZR);
							AddLink(ZR,ZZ);
						};
					};
				};
			};
		};
	};
	ProcessMessages();
	//creating resource references
	for( i=0;i<MAXSPR;i++){
		OneSprite* OS=&Sprites[i];
		SprGroup* SG=OS->SG;
		ObjCharacter* OC=&SG->ObjChar[OS->SGIndex];
		if(OC->IntResType<8){
			byte imp=0;
			int ort=OC->IntResType;
			if(ort==GoldID)imp=1;
			else if(ort==IronID)imp=2;
			else if(ort==CoalID)imp=4;
			int xx=OS->x>>6;
			int yy=OS->y>>6;
			int ofs=xx+yy*TopLx;
			word TZ=GetTopRef(ofs);
			if(TZ<4096){
				Area* AR=GetTopMap(TZ);
				AR->NMines++;
				AR->Importance|=imp;
			};
		};
	};
	for( i=0;i<NA;i++){
		Area* AR=GetTopMap(i);
		if(AR->NMines){
			AR->MinesIdx=znew(word,AR->NMines);
			AR->NMines=0;
		}
	};
	for( i=0;i<MAXSPR;i++){
		OneSprite* OS=&Sprites[i];
		SprGroup* SG=OS->SG;
		ObjCharacter* OC=&SG->ObjChar[OS->SGIndex];
		if(OC->IntResType<8){
			int xx=OS->x>>6;
			int yy=OS->y>>6;
			int ofs=xx+yy*TopLx;
			word TZ=GetTopRef(ofs);
			if(TZ<4096){
				Area* AR=GetTopMap(TZ);
				AR->MinesIdx[AR->NMines]=i;
				AR->NMines++;
			};
		};
	};
	ProcessMessages();
	MotionLinks=znew(word,NA*NA);
	LinksDist=znew(word,NA*NA);
	memset(MotionLinks,0xFFFF,NA*NA*2);
	memset(LinksDist,0xFFFF,NA*NA*2);
	CreateLinkInfo();
	//assert(_CrtCheckMemory());
	CreateStrategyInfo();
	CreateWTopMap();
	ResearchIslands();
	CreateCostPlaces();
};

void ClearLinkInfo(){
	int NA=GetNAreas();
	memset(MotionLinks,0xFFFF,NA*NA*2);
	memset(LinksDist,0xFFFF,NA*NA*2);
	for(int i=0;i<NA;i++){
		Area* AR=GetTopMap(i);
		int k=AR->NLinks;
		for(int j=0;j<k;j++){
			int ofst=AR->Link[j+j]+i*NA;
			MotionLinks[ofst]=AR->Link[j+j];
			LinksDist[ofst]=AR->Link[j+j+1];
		};
	};
};
int OneIteration(){
#ifdef HASH_TOP
	return 0;
#endif
	int NA=GetNAreas();
	int N=NA*NA;
	int ofs=0;
	int NChanges=0;
	for(int i=0;i<NA;i++)
		for(int j=0;j<NA;j++){
			if(i!=j){
				Area* AR=GetTopMap(i);
				int N=AR->NLinks;
				word* LINK=AR->Link;
				int CurDis=GetLinksDist(ofs);
				for(int k=0;k<N;k++){
					word N2=LINK[k+k];
					word adds=LINK[k+k+1];
					if(N2!=j){
						int ofs1=j+N2*NA;
						int dst=GetLinksDist(ofs1);
						if(dst!=0xFFFF){
							if(dst+adds<CurDis){
								LinksDist[ofs]=dst+adds;
								MotionLinks[ofs]=N2;
								NChanges++;
							};
						};
					};
				};
			};
			ofs++;
		};
	return NChanges;
};
void CreateRoadsNet();
extern bool MiniMade;
bool ProcessMessages();
bool NeedProcessTop;
void CreateLinkInfo(){
	ClearLinkInfo();
	int N;
	int p=0;
	do{
		ProcessMessages();
		N=OneIteration();
		p++;
	}while(N&&p<3);
	//CreateRoadsNet();
	NeedProcessTop=1;
	MiniMade=false;
};
void InitAreas(){
	NAreas=0;
};
void EraseAreas(){
	int i;

#ifdef HASH_TOP
	for(int i=0;i<NMFIELDS;i++){
		HashTable[i].EraseAreas();
	}
#endif //HASH_TOP
	int NA=GetNAreas();
	if(NA){
		for(int i=0;i<NA;i++){
			Area* AR1=GetTopMap(i);
			if(AR1->MaxLink)free(AR1->Link);
			if(AR1->NMines)free(AR1->MinesIdx);
		};
		free(TopMap);
		free(MotionLinks);
		free(LinksDist);
		NAreas=0;
		MaxArea=0;
		TopMap=NULL;
	};
	if(!WNAreas)return;
	for(int i=0;i<WNAreas;i++){
		Area* AR1=&WTopMap[i];
		if(AR1->MaxLink)free(AR1->Link);
		if(AR1->NMines)free(AR1->MinesIdx);
	};
	delete[](WTopMap);
	free(WMotionLinks);
	free(WLinksDist);
	WNAreas=0;
	WMaxArea=0;
	WTopMap=NULL;
};
extern word CurZone;
#define xTopLx (256<<2)
void DrawAreas(int xs,int ys,int x0,int y0,int Lx,int Ly){
	return;
	int topof=int(TopRef)+(x0+y0*TopLx)*2;
	int scrof=int(ScreenPtr)+xs+ys*ScrWidth;
	int adds=ScrWidth-Lx;
	int addt=(TopLx-Lx)<<1;
	__asm{
		push	esi
		push	edi
		pushf
		cld
		mov		esi,topof
		mov		edi,scrof
		mov		dx,CurZone
ppp:
		mov		ecx,Lx
ppp1:
		lodsw
		cmp		ax,0xFFFE
		jae		NoDraw
		
		cmp		ax,[esi]
		jne		DoDraw
		cmp		ax,[esi-4]
		jne		DoDraw
		cmp		ax,[esi+xTopLx+xTopLx-2]
		jne		DoDraw
		cmp		ax,[esi-xTopLx-xTopLx-2]
		je		NoDraw
		
DoDraw: cmp		dx,ax
		jne		Draw1
		mov		byte ptr [edi],0xFF
		jmp		NoDraw
Draw1:
		and		al,15
		add		al,0xD0
		mov		byte ptr [edi],al
NoDraw:	inc		edi
		dec		ecx
		jnz		ppp1
		add		edi,adds
		add		esi,addt
		dec		Ly
		jnz		ppp

		popf
		pop		edi
		pop		esi
	};
	
	/*
	if(CurZone<0xFFFE){
		for(int i=0;i<GetNAreas();i++){
			if(i!=CurZone){
				int j=MotionLinks[i*GetNAreas()+CurZone];
				if(j!=0xFFFF){
					Area* AR=GetNAreas()+i;
					int xx=int(AR->x)-x0;
					int yy=int(AR->y)-y0;
					Area* AR1=GetNAreas()+j;
					int xx1=int(AR1->x)-x0;
					int yy1=int(AR1->y)-y0;
					//if(xx>=0&&yy>=0&&xx<Lx&&yy<Ly&&xx1>=0&&yy1>=0&&xx1<Lx&&yy1<Ly)
						DrawLine(xx+xs,yy+ys,xx1+xs,yy1+ys,254);
				};

			};
		};
	};
	*/
};
void CreateStrategyInfo(){
	int NA=GetNAreas();
	if(!NA)return;
	for(int i=0;i<NA;i++){
		memset(&GetTopMap(i)->SINF[0],0,8*sizeof StrategyInfo);
	};
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		if(OB){
			int xx=OB->RealX>>10;
			int yy=OB->RealY>>10;
			int ofs=xx+(yy<<TopSH);
			if(ofs>=0&&ofs<MaxTop){
				word TR=GetTopRef(ofs);
				if(TR<0xFFFE){
					Area* AR=GetTopMap(TR);
					StrategyInfo* STINF=&AR->SINF[OB->NNUM];
					byte USAGE=OB->newMons->Usage;
					switch(USAGE){
					case MelnicaID:
						STINF->BuildInfo|=SI_Meln;
						break;
					case FarmID:
						STINF->BuildInfo|=SI_Farm;
						break;
					case CenterID:
						STINF->BuildInfo|=SI_Center;
						break;
					case SkladID:
						STINF->BuildInfo|=SI_Sklad;
						break;
					case TowerID:
						STINF->BuildInfo|=SI_Tower;
						STINF->NTowers++;
						break;
					case MineID:
						STINF->BuildInfo|=SI_Shahta;
						break;
					//case FastHorceID:
					case HardHorceID:
					case LightInfID:
						if(STINF->NShortRange<255)STINF->NShortRange++;
						break;
					case HorseStrelokID:
					case StrelokID:
						if(STINF->NLongRange<255)STINF->NLongRange++;
						break;
					case ArcherID:
					case GrenaderID:
						if(STINF->NLongRange<255)STINF->NLongRange++;
						break;
					case ArtDepoID:
						STINF->BuildInfo|=SI_ArtDepo;
						break;
					case PeasantID:
						if(STINF->NPeasants<255)STINF->NPeasants++;
						break;
					case MortiraID:
						if(STINF->NMortir<255)STINF->NMortir++;
						break;
					case PushkaID:
						if(STINF->NPushek<255)STINF->NPushek++;
						break;
					};
				};
			};
		};
	};
	for(int i=0;i<NA;i++){
		Area*AR= GetTopMap(i);
		int N=AR->NLinks;
		StrategyInfo* ST=AR->SINF;
		if(N){
			word* Lin=AR->Link;
			for(int j=0;j<N;j++){
				Area* AR2=GetTopMap(Lin[j]);
				StrategyInfo* ST2=AR2->SINF;
				for(int k=0;k<8;k++){
					if(ST[k].BuildInfo&(~SI_Near)){
						ST2[k].BuildInfo|=SI_Near;
					};
				};
			};
		};
	};
};
/*
void RemakeGroupOfAreas(word* Grp,int Na){
	//clear old links&distances
	for(int i=0;i<Na;i++){
		int ID=Grp[i];
		Area* AR=GetTopMap(+ID;
		int k=AR->NLinks;
		for(int j=0;j<k;j++){
			int ofst=int(AR->Link[j+j])+ID*GetNAreas();
			MotionLinks[ofst]=0xFFFF;
			LinksDist[ofst]=0xFFFF;
		};
	};
	//clear
	int mmx=msx>>1;
	int mmy=msy>>1;
	for(int i=0;i<Na;i++){
		word ID=Grp[i];
		int minr=1000;
		Area* AR=GetTopMap(+ID;
		int x0=AR->x;
		int y0=AR->y;
		int Nclr=0;
		for(int r=0;r<RRad&&r<minr+3;r++){
			char* xi=Rarr[r].xi;
			char* yi=Rarr[r].yi;
			int N=Rarr[r].N;
			for(int j=0;j<N;j++){
				int x1=x0+xi[j];
				int y1=y0+yi[j];
				if(x1>0&&y1>0&&x1<mmx&&y1<mmy){
					int ofst=x1+(y1<<TopSH);
					if(TopRef[ofst]==ID){
						TopRef[ofst]=0xFFFE;
						minr=r;
					};
				};
			};
		};
		//start point
		int ofst=x0+(y0<<TopSH);
		if(TopRef[ofst]==0xFFFE)TopRef[ofst]=ID;
		//Erase links
		AR->NLinks=0;
	};
	//filling areas and creating new links
	bool Change=true;
	for(i=1;i<RRad&&Change;i++){
		Change=false;
		char* xi=Rarr[i].xi;
		char* yi=Rarr[i].yi;
		int N=Rarr[i].N;
		for(int m=0;m<Na;m++){
			int j=Grp[m];
			int xx=GetTopMap([j].x;
			int yy=GetTopMap([j].y;
			for(int k=0;k<N;k++){
				int x=xx+xi[k];
				int y=yy+yi[k];
				if(x>0&&y>0&&x<mmx&&y<mmy){
					int ofst=x+y*TopLx;
					word ZZ=TopRef[ofst	  ];
					word ZU=TopRef[ofst-TopLx];
					word ZD=TopRef[ofst+TopLx];
					word ZL=TopRef[ofst-1    ];
					word ZR=TopRef[ofst+1    ];
					if(ZZ==0xFFFE){
						if(ZU==j||ZD==j||ZL==j||ZR==j||
							(ZU!=0xFFFF&&ZL!=0xFFFF&&TopRef[ofst-TopLx-1]==j)||
							(ZU!=0xFFFF&&ZR!=0xFFFF&&TopRef[ofst-TopLx+1]==j)||
							(ZD!=0xFFFF&&ZL!=0xFFFF&&TopRef[ofst+TopLx-1]==j)||
							(ZD!=0xFFFF&&ZR!=0xFFFF&&TopRef[ofst+TopLx+1]==j)){
							TopRef[ofst]=j;
							Change=true;
							ZZ=j;
						};
					};
					if(ZZ<0xFFFE){
						if(ZU!=ZZ&&ZU<0xFFFE){
							AddLink(ZZ,ZU);
							AddLink(ZU,ZZ);
						};
						if(ZD!=ZZ&&ZD<0xFFFE){
							AddLink(ZZ,ZD);
							AddLink(ZD,ZZ);
						};
						if(ZL!=ZZ&&ZL<0xFFFE){
							AddLink(ZZ,ZL);
							AddLink(ZL,ZZ);
						};
						if(ZR!=ZZ&&ZR<0xFFFE){
							AddLink(ZZ,ZR);
							AddLink(ZR,ZZ);
						};
					};
				};
			};
		};
	};
};
*/
bool WasOnlyOpen;
word NChAreas;
word MaxChAreas;
word* ChAreas;
//procesing variables
int CurIStart;
int TmpChanges;
word* TmpMLinks;
word* TmpMDist;
int MinChX;
int MaxChX;
int MinChY;
int MaxChY;
int GetTopology(int x,int y, byte LockType=0);
void InitTopChange(){
	NeedProcessTop=false;
	MaxChAreas=0;
	NChAreas=0;
	ChAreas=NULL;
	TmpMLinks = NULL;
	TmpMDist  = NULL;
	CurIStart = 0;
	MinChX=100000;
	MaxChX=0;
	MinChY=100000;
	MaxChY=0;
};
void ClearTopChange(){
	if(ChAreas)free(ChAreas);
	if(TmpMLinks){
		free(TmpMLinks);
		free(TmpMDist);
	};
	TmpMLinks=NULL;
	TmpMDist=NULL;
	InitTopChange();
	WasOnlyOpen=true;
};
void AddChTop(word TopID){
	for(int i=0;i<NChAreas;i++)if(ChAreas[i]==TopID)return;
	if(NChAreas>=MaxChAreas){
		MaxChAreas+=32;
		ChAreas=(word*)realloc(ChAreas,MaxChAreas<<1);
	};
	ChAreas[NChAreas]=TopID;
	NChAreas++;
};
void AddSurroundingAreas(word TopID){
	if(!GetNAreas())return;
	AddChTop(TopID);
	Area* AR=GetTopMap(TopID);
	int N=AR->NLinks;
	for(int i=0;i<N;i++)AddChTop(AR->Link[i<<1]);
};
void DynamicalLockTopCell(int x,int y){
	NeedProcessTop=true;
	word topo=GetTopology(x<<6,y<<6);
	if(topo<0xFFFE){
		AddSurroundingAreas(topo);
	};
	int ofst=x+(y<<TopSH);
	TopRef[ofst]=0xFFFF;
	if(x<MinChX)MinChX=x;
	if(x>MaxChX)MaxChX=x;
	if(y<MinChY)MinChY=y;
	if(y>MaxChY)MaxChY=y;
	WasOnlyOpen=false;
};
void DynamicalUnLockTopCell(int x,int y){
	NeedProcessTop=true;
	word topo=GetTopology(x<<6,y<<6);
	if(topo<0xFFFE){
		AddSurroundingAreas(topo);
	};
	int ofst=x+(y<<TopSH);
	TopRef[ofst]=0xFFFE;
	if(x<MinChX)MinChX=x;
	if(x>MaxChX)MaxChX=x;
	if(y<MinChY)MinChY=y;
	if(y>MaxChY)MaxChY=y;
};
void OnePartIteration(int Nitr){
	if(!TmpMLinks)return;
	int NA=GetNAreas();
	int N=NA*NA;
	int ofs=CurIStart*NA;
	if(!CurIStart)TmpChanges=0;
	for(int i=CurIStart;i<NA;i++){
		for(int j=0;j<NA;j++){
			if(i!=j){
				Area* AR=GetTopMap(i);
				int N=AR->NLinks;
				word* LINK=AR->Link;
				int CurDis=TmpMDist[ofs];
				//int CurDis=LinksDist[ofs];//
				for(int k=0;k<N;k++){
					word N2=LINK[k+k];
					word adds=LINK[k+k+1];
					if(N2!=j&&adds!=0xFFFF){
						int ofs1=j+N2*NA;
						int dst=TmpMDist[ofs1];
						//int dst=LinksDist[ofs1];//
						if(dst!=0xFFFF){
							if(dst+adds<CurDis){
								int ddd=dst+adds;
								TmpMDist[ofs]=ddd;
								//LinksDist[ofs]=ddd;//
								TmpMLinks[ofs]=N2;
								//MotionLinks[ofs]=N2;//
								if(ddd<GetLinksDist(ofs)){
									LinksDist[ofs]=ddd;
									MotionLinks[ofs]=N2;
								};
								TmpChanges++;
							};
						};
					};
				};
			};
			ofs++;
		};
		CurIStart=i;
		Nitr--;
		if(!Nitr)return;
	};
	//COUNTER=TmpChanges;
	if(!TmpChanges){
		memcpy(MotionLinks,TmpMLinks,NA*NA*2);
		memcpy(LinksDist,TmpMDist,NA*NA*2);
		free(TmpMLinks);
		free(TmpMDist);
		TmpMLinks=NULL;
		TmpMDist=NULL;
		CurIStart=0;
	}else{
		CurIStart=0;
	};
};
void LimitedClearBadLinks(word* AList,int Na,word* MLinks,word* MDist){
	
	int Change;
	int p=0;
	int NA=GetNAreas();
	do{
		Change=0;
		for(int i=0;i<Na;i++){
			int Ai=AList[i];
			Area* ARi=GetTopMap(Ai);
			int N=ARi->NLinks;
			word* LINK=ARi->Link;
			int ofs0=int(AList[i])*NA;
			for(int j=0;j<Na;j++){
				if(i!=j){
					int Aj=AList[j];
					int ofs=ofs0+Aj;
					int CurDis=MDist[ofs];
					if(CurDis!=0xFFFF){
						for(int k=0;k<N;k++){
							word N2=LINK[k+k];
							//word adds=LINK[k+k+1];
							if(N2!=Aj){
								int ofs1=Aj+N2*NA;
								int dst=TmpMDist[ofs1];
								if(dst==0xFFFF){
									MLinks[ofs]=0xFFFF;
									MDist[ofs]=0xFFFF;
									Change++;
								};
							};
						};
					};
				};

			};
		};
		p++;
	}while(Change&&p<100);
};
void TotalClearBadLinks(word* MLinks,word* MDist){
	
	int Change;
	int p=0;
	int NA=GetNAreas();
	do{
		Change=0;
		for(int i=0;i<NA;i++){
			int Ai=i;
			Area* ARi=GetTopMap(Ai);
			int N=ARi->NLinks;
			word* LINK=ARi->Link;
			int ofs0=int(i)*NA;
			for(int j=0;j<NA;j++){
				if(i!=j){
					int Aj=j;
					int ofs=ofs0+Aj;
					int CurDis=MDist[ofs];
					if(CurDis!=0xFFFF){
						for(int k=0;k<N;k++){
							word N2=LINK[k+k];
							//word adds=LINK[k+k+1];
							if(N2!=Aj){
								int ofs1=Aj+N2*NA;
								int dst=TmpMDist[ofs1];
								if(dst==0xFFFF){
									MLinks[ofs]=0xFFFF;
									MDist[ofs]=0xFFFF;
									Change++;
								};
							};
						};
					};
				};

			};
		};
		p++;
	}while(Change&&p<100);
};
void LimitedSetNewLinks(word* AList,int Na,word* MLinks,word* MDist){
	
	int NA=GetNAreas();

	byte* aropt=znew(byte,NA);
	memset(aropt,0,NA);
	for(int i=0;i<Na;i++)aropt[AList[i]]=1;
	
	int Change;
	int p=0;
	do{
		Change=0;
		for(int i=0;i<Na;i++){
			int Ai=AList[i];
			Area* ARi=GetTopMap(Ai);
			int N=ARi->NLinks;
			word* LINK=ARi->Link;
			int ofs0=int(AList[i])*NA;
			for(int j=0;j<Na;j++){
				if(i!=j){
					int Aj=AList[j];
					int ofs=ofs0+Aj;
					int CurDis=MDist[ofs];
					for(int k=0;k<N;k++){
						word N2=LINK[k+k];
						if(aropt[N2]){
							word adds=LINK[k+k+1];
							if(N2!=Aj&&adds!=0xFFFF){
								int ofs1=Aj+N2*NA;
								int dst=MDist[ofs1];
								if(dst!=0xFFFF){
									if(dst+adds<CurDis){
										int ddd=dst+adds;
										MDist[ofs]=ddd;
										MLinks[ofs]=N2;
										if(ddd<LinksDist[ofs]){
											LinksDist[ofs]=ddd;
											MotionLinks[ofs]=N2;
										};
										Change++;
									};
								};
							};
						};
					};
				};
			};
		};
		p++;
	}while(Change&&p<100);
	free(aropt);
};
void FastReduceWays(word* MLinks,word* MDist,word* StartChange,int NStartChange){
	word Changes[1000];
	word PrevChanges[1000];
	memcpy(PrevChanges,StartChange,NStartChange<<1);
	int NPrev=NStartChange;
	int Change=0;
	int p=0;
	int NChanges;
	int NA=GetNAreas();
	do{
		memcpy(Changes,PrevChanges,NPrev<<1);
		NChanges=NPrev;
		NPrev=0;
		//extend changes
		byte ChMap[1000];
		memset(ChMap,0,NA);
		byte PrevMap[1000];
		memset(PrevMap,0,NA);
		for(int i=0;i<NChanges;i++)ChMap[Changes[i]]=1;
		int N1=NChanges;
		for(int i=0;i<N1;i++){
			Area* AR=GetTopMap(Changes[i]);
			int N=AR->NLinks;
			word* ids=AR->Link;
			for(int j=0;j<N;j++){
				word id=ids[j];
				if(!ChMap[id]){
					ChMap[id]=1;
					Changes[NChanges]=id;
					NChanges++;
				};
			};
		};
		//processing
		Change=0;
		for(int i=0;i<NChanges;i++){
			int Ai=Changes[i];
			Area* ARi=GetTopMap(Ai);
			int N=ARi->NLinks;
			word* LINK=ARi->Link;
			int ofs0=int(Changes[i])*NA;
			for(int j=0;j<NChanges;j++){
				if(i!=j){
					int Aj=Changes[j];
					int ofs=ofs0+Aj;
					int CurDis=MDist[ofs];
					for(int k=0;k<N;k++){
						word N2=LINK[k+k];
						//if(aropt[N2]){
							word adds=LINK[k+k+1];
							if(N2!=Aj&&adds!=0xFFFF){
								int ofs1=Aj+N2*NA;
								int dst=MDist[ofs1];
								if(dst!=0xFFFF){
									if(dst+adds<CurDis){
										int ddd=dst+adds;
										MDist[ofs]=ddd;
										MLinks[ofs]=N2;
										//if(ddd<LinksDist[ofs]){
										//	LinksDist[ofs]=ddd;
										//	MotionLinks[ofs]=N2;
										//};
										Change++;
										if(!PrevMap[Ai]){
											PrevChanges[NPrev]=Ai;
											PrevMap[Ai]=1;
											NPrev++;
										};
										if(!PrevMap[Aj]){
											PrevChanges[NPrev]=Aj;
											PrevMap[Aj]=1;
											NPrev++;
										};
									};
								};
							};
						//};
					};
				};
			};
		};
		p++;
	}while(Change&&p<200);
};
void OnePartIterationNew(int Nitr);
void RemakeGroupOfAreas(word* Grp,int Na);
int NeedITR=100;
void ResearchIslands();
void ProcessDynamicalTopology(){
	if(rando()<100)ResearchIslands();
	if(NChAreas){
		RemakeGroupOfAreas(ChAreas,NChAreas);		
		NChAreas=0;
		NeedITR=3200;
	};
	if(MinChX<=MaxChX){
		word LimAreas[200];
		int xc=(MaxChX+MinChX)>>1;	
		int yc=(MaxChY+MinChY)>>1;
		word NLim=0;
		int NA=GetNAreas();
		for(int i=0;i<NA;i++){
			Area* AR1=GetTopMap(i);
			if(Norma(xc-int(AR1->x),yc-int(AR1->y))<70){
				LimAreas[NLim]=i;
				NLim++;
			};
		};
		/*
		for(int i=0;i<NLim;i++){
			int Ai=LimAreas[i];
			int ofs0=Ai*GetNAreas();
			for(int j=0;j<NLim;j++){
				int ofs=ofs0+LimAreas[j];
				MotionLinks[ofs]=0xFFFF;
				LinksDist[ofs]=0xFFFF;
			};
		};
		*/
		
		for(int i=0;i<NA;i++){
			Area* AR=GetTopMap(i);
			int k=AR->NLinks;
			for(int j=0;j<k;j++){
				int ofst=int(AR->Link[j+j])+i*NA;
				MotionLinks[ofst]=AR->Link[j+j];
				LinksDist[ofst]=AR->Link[j+j+1];
				//ofst=int(AR->Link[j+j])*GetNAreas()+i;
				//MotionLinks[ofst]=AR->Link[j+j];
				//LinksDist[ofst]=AR->Link[j+j+1];
			};
		};
		
		LimitedSetNewLinks(LimAreas,NLim,MotionLinks,LinksDist);
	};
	MinChX=100000;
	MaxChX=0;
	MinChY=100000;
	MaxChY=0;
	//WasOnlyOpen=true;
	OnePartIterationNew(NeedITR);
	if(!TmpChanges)NeedITR=1600;
	else int NeedITR=4500;
};

void StopDynamicalTopology(){
	if(TmpMLinks)free(TmpMLinks);
	if(TmpMDist)free(TmpMDist);
	TmpMLinks=NULL;
	TmpMDist=NULL;
	NeedProcessTop=false;
	NChAreas=0;
	if(MaxChAreas)free(ChAreas);
	ChAreas=NULL;
	CurIStart=0;
	TmpChanges=0;
	MaxChAreas=0;
};

//------------------Roads tecnology------------------
class Road;
class Road{
public:
	Road();
	~Road();
	int MaxSegs;
	int NSegs;
	int CheckSumm;
	word* Segs;
	void Erase();
	void CreateCheckSumm();
	void CheckSize(int N);
	void CreateRandomWay(int Start,int Finish,int N);
	void EraseCrossing();
	void ErasePart(int Start,int Finish);
	bool Connect(int StartIndex,int EndIndex);
	void Simplify();
	//void CheckDanger();
	void CreateMostSafeWay();
	bool Compare(Road* ROAD);
	void View(int x,int y,byte c);
};
Road::Road(){
	MaxSegs=0;
	NSegs=0;
	Segs=NULL;
};
Road::~Road(){
	if(Segs)free(Segs);
	MaxSegs=0;
	NSegs=0;
	Segs=NULL;
};
void Road::Erase(){
	NSegs=0;
};
void Road::CheckSize(int N){
	if(N>=MaxSegs){
		MaxSegs+=64;
		Segs=(word*)realloc(Segs,MaxSegs<<1);
	};
};
void Road::CreateRandomWay(int Start,int Finish,int N){
	NSegs=0;
	int NA=GetNAreas();
	if(GetLinksDist(Start+Finish*NA)==0xFFFF)return;
	CheckSize(1);
	Segs[0]=Start;
	NSegs=1;
	int PreTop=-1;
	for(int i=0;i<N;i++){
		word Top=0xFFFF;
		while(Top==0xFFFF){
			Top=(int(rando())*NA)>>15;
			if(GetLinksDist(Start+Top*NA)==0xFFFF||PreTop==Top||Top==Start||Top==Finish)Top=0xFFFF;
		};
		CheckSize(NSegs+1);
		Segs[NSegs]=Top;
		NSegs++;
		Connect(NSegs-2,NSegs-1);
		PreTop=Top;
	};
	CheckSize(NSegs+1);
	Segs[NSegs]=Finish;
	NSegs++;
	Connect(NSegs-2,NSegs-1);
	EraseCrossing();
	Simplify();
	CreateCheckSumm();
};
bool Road::Connect(int Start,int End){
	int N=1;
	int CTop=Segs[Start];
	int ETop=Segs[End];
	bool Change=false;
	int NA=GetNAreas();
	do{
		CTop=GetMotionLinks(ETop+CTop*NA);
		N++;
	}while(CTop!=ETop&&CTop!=0xFFFF);
	if(CTop==0xFFFF)return false;
	if(N!=(End-Start+1)){
		//Need to expand
		CheckSize(NSegs+N-End+Start-1);
		memmove(Segs+Start+N-1,Segs+End,(NSegs-End)<<1);
		Change=true;
		NSegs+=N-End+Start-1;
	};
	CTop=Segs[Start];
	int ps=Start+1;
	do{
		CTop=GetMotionLinks(ETop+CTop*NA);
		if(Segs[ps]!=CTop){
			Segs[ps]=CTop;
			Change=true;
		};
		ps++;
	}while(CTop!=ETop&&CTop!=0xFFFF);
	return Change;
};
void Road::ErasePart(int Start,int Finish){
	if(Segs&&Finish<NSegs-1){
		memmove(Segs+Start,Segs+Finish+1,(NSegs-Finish-1)<<1);
	};
	NSegs-=Finish-Start+1;
};
void Road::EraseCrossing(){
	word NCross[512];
	assert(GetNAreas()<512);
STRT:
	memset(NCross,0xFFFF,GetNAreas()<<1);
	for(int i=0;i<NSegs;i++){
		int sg=Segs[i];
		if(NCross[sg]!=0xFFFF){
			//erasing
			ErasePart(NCross[sg]+1,i);
			goto STRT;
		}else NCross[sg]=i;
	};
};
void Road::CreateCheckSumm(){
	CheckSumm=NSegs<<19;
	for(int i=0;i<NSegs;i++){
		CheckSumm+=Segs[i]<<(i&15);
	};
};
void Road::Simplify(){
	CreateCheckSumm();
	int PreSumm=CheckSumm;
	do{
		PreSumm=CheckSumm;
		for(int i=0;i<NSegs-4;i++){
			Connect(i,i+4);
		};
		CreateCheckSumm();
	}while(CheckSumm!=PreSumm);
};
void Road::View(int x,int y,byte c){
	int prevx=-1;
	int prevy=-1;
	for(int i=0;i<NSegs-1;i++){
		Area* AR=GetTopMap(Segs[i]);
		if(prevx!=-1){
			DrawLine(prevx,prevy,x+AR->x,y+AR->y,c);
		};
		prevx=x+AR->x;
		prevy=y+AR->y;
	};
};
//-------------------testing---------------------

//Road ROADS[20];
//int NROADS=0;
//extern int COUNTER;

void TestRoads(){
/*	
	//COUNTER=GetTickCount();
	//NROADS=0;
	int NAttempts=0;
	int Start=TopRef[10+TopLx*10];
	int Fin=TopRef[220+TopLx*220];
	//StartPoint(AI_Nation);
	int AINT=1+((int(rando())*6)>>15);
	int NAI=NtNUnits[
	if(NROADS<2500){
		ROADS[NROADS].CreateRandomWay(Start,Fin,3);
		if(NROADS){
			int SUM=ROADS[NROADS].CheckSumm;
			bool Present=false;
			for(int k=0;k<NROADS;k++){
				if(SUM==ROADS[k].CheckSumm)Present=true;;
			};
			if(!Present)NROADS++;
		}else NROADS++;
	};
	//COUNTER=GetTickCount()-COUNTER;
*/	
};
extern int tmtmt;
void ViewRoads(int x,int y){
	/*
	TestRoads();
	if(NROADS){
		int ci=tmtmt%NROADS;
		ROADS[ci].View(x,y,255);
	};
	*/
};
//Water topology

word* WTopRef;
Area* WTopMap;
int   WNAreas;
int WMaxArea;
word* WMotionLinks;
word* WLinksDist;
#define MinWNorm (8<<(ADDSH-1))
bool AddWArea(short x,short y){
	for(int i=0;i<WNAreas;i++){
		if(Norma(WTopMap[i].x-x,WTopMap[i].y-y)<=MinWNorm){
			return false;
		};
	};
	if(WNAreas>=WMaxArea){
		WMaxArea+=120;
		WTopMap=(Area*)realloc(WTopMap,WMaxArea* sizeof(Area));
	};
	Area* AR=WTopMap+WNAreas;
	AR->x=x;
	AR->y=y;
	AR->Importance=0;
	AR->NTrees=0;
	AR->NStones=0;
	AR->MinesIdx=NULL;
	AR->NLinks=0;
	AR->NMines=0;
	AR->MaxLink=6;
	AR->Link=znew(word,AR->MaxLink<<1);
	WNAreas++;
	return true;
};
void AddWLink(int N1,int N2){
	Area* AR=&WTopMap[N1];
	int N=AR->NLinks;
	for(int i=0;i<N;i++){
		if(AR->Link[i+i]==N2)return;
	};
	if(AR->NLinks==AR->MaxLink){
		AR->MaxLink+=4;
		AR->Link=(word*)realloc(AR->Link,AR->MaxLink<<2);
	};
	N=AR->NLinks;
	AR->Link[N+N]=N2;
	AR->Link[N+N+1]=Norma(AR->x-WTopMap[N2].x,AR->y-WTopMap[N2].y);
	AR->NLinks++;
};
int WOneIteration(){
	int N=WNAreas*WNAreas;
	int ofs=0;
	int NChanges=0;
	for(int i=0;i<WNAreas;i++)
		for(int j=0;j<WNAreas;j++){
			if(i!=j){
				Area* AR=WTopMap+i;
				int N=AR->NLinks;
				word* LINK=AR->Link;
				int CurDis=WLinksDist[ofs];
				for(int k=0;k<N;k++){
					word N2=LINK[k+k];
					word adds=LINK[k+k+1];
					if(N2!=j){
						int ofs1=j+N2*WNAreas;
						int dst=WLinksDist[ofs1];
						if(dst!=0xFFFF){
							if(dst+adds<CurDis){
								WLinksDist[ofs]=dst+adds;
								WMotionLinks[ofs]=N2;
								NChanges++;
							};
						};
					};
				};
			};
			ofs++;
		};
	return NChanges;
};
void ClearWLinkInfo(){
	memset(WMotionLinks,0xFFFF,WNAreas*WNAreas*2);
	memset(WLinksDist,0xFFFF,WNAreas*WNAreas*2);
	for(int i=0;i<WNAreas;i++){
		Area* AR=WTopMap+i;
		int k=AR->NLinks;
		for(int j=0;j<k;j++){
			int ofst=AR->Link[j+j]+i*WNAreas;
			WMotionLinks[ofst]=AR->Link[j+j];
			WLinksDist[ofst]=AR->Link[j+j+1];
		};
	};
};
void CreateWLinkInfo(){
	ClearWLinkInfo();
	int N;
	int p=0;
	do{
		ProcessMessages();
		N=WOneIteration();
		p++;
	}while(N&&p<500);
};
void CreateWTopMap(){

	WNAreas=0;
	memset(WTopRef,0xFFFF,WTopLx*WTopLx*2);
	for(int ix=0;ix<WTopLx;ix++)
		for(int iy=0;iy<WTopLx;iy++){
			if(!MFIELDS[1].CheckBar((ix<<WCLSH),(iy<<WCLSH),WCLLX,WCLLX)){
				WTopRef[ix+(iy<<WTopSH)]=0xFFFE;
			};
		};
	int mmx=msx>>(WCLSH-1);
	int mmy=msy>>(WCLSH-1);
	int N1=0;
	/*
	do{
		int x=(int(rando())*(mmx-4))>>15;
		int y=(int(rando())*(mmy-4))>>15;
		bool Empty=true;
		for(int dx=0;dx<=2&&Empty;dx++)
			for(int dy=0;dy<=2&&Empty;dy++){
				if(WTopRef[x+dx+(y+dy)*WTopLx]!=0xFFFE)Empty=false;
			};
		if(Empty){
			if(AddWArea(x+1,y+1))N1=0;
			else N1++;
		};
	}while(N1<50);
	*/
	N1=0;
	int N2=0;
	do{
		int x=(int(rando())*(mmx-4))>>15;
		int y=(int(rando())*(mmy-4))>>15;
		if(x>4&&y>4&&x<WTopLx-4&&y<WTopLx-4){
			if(WTopRef[x+y*WTopLx]==0xFFFE&&WTopRef[x+3+y*WTopLx]==0xFFFE
				&&WTopRef[x-3+y*WTopLx]==0xFFFE&&WTopRef[x+(y+3)*WTopLx]==0xFFFE
				&&WTopRef[x+(y-3)*WTopLx]==0xFFFE&&WTopRef[x+3+(y+3)*WTopLx]==0xFFFE
				&&WTopRef[x-3+(y+3)*WTopLx]==0xFFFE&&WTopRef[(x+3)+(y-3)*WTopLx]==0xFFFE
				&&WTopRef[x-3+(y-3)*WTopLx]==0xFFFE){
				N2=0;
				if(AddWArea(x+1,y+1))N1=0;
				else N1++;
			}else N2++;
		}else N2++;
	}while(N1<50&&N2<5000);
	//splitting...
	for(int i=0;i<WNAreas;i++)WTopRef[WTopMap[i].x+int(WTopMap[i].y)*WTopLx]=i;
	bool Change=false;
	//word TempTop[TopLx*TopLy];
	Change=true;
	for(int i=1;i<RRad&&Change;i++){
		//memcpy(TempTop,TopRef,sizeof(TopRef));
		Change=false;
		char* xi=Rarr[i].xi;
		char* yi=Rarr[i].yi;
		int N=Rarr[i].N;
		for(int j=0;j<WNAreas;j++){
			int xx=WTopMap[j].x;
			int yy=WTopMap[j].y;
			for(int k=0;k<N;k++){
				int x=xx+xi[k];
				int y=yy+yi[k];
				if(x>0&&y>0&&x<mmx&&y<mmy){
					int ofst=x+y*WTopLx;
					word ZZ=WTopRef[ofst	  ];
					word ZU=WTopRef[ofst-WTopLx];
					word ZD=WTopRef[ofst+WTopLx];
					word ZL=WTopRef[ofst-1    ];
					word ZR=WTopRef[ofst+1    ];
					if(ZZ==0xFFFE){
						if(ZU==j||ZD==j||ZL==j||ZR==j||
							WTopRef[ofst-WTopLx-1]==j||
							WTopRef[ofst-WTopLx+1]==j||
							WTopRef[ofst+WTopLx-1]==j||
							WTopRef[ofst+WTopLx+1]==j){
							WTopRef[ofst]=j;
							Change=true;
							ZZ=j;
						};
					};
					if(ZZ<0xFFFE){
						if(ZU!=ZZ&&ZU<0xFFFE){
							AddWLink(ZZ,ZU);
							AddWLink(ZU,ZZ);
						};
						if(ZD!=ZZ&&ZD<0xFFFE){
							AddWLink(ZZ,ZD);
							AddWLink(ZD,ZZ);
						};
						if(ZL!=ZZ&&ZL<0xFFFE){
							AddWLink(ZZ,ZL);
							AddWLink(ZL,ZZ);
						};
						if(ZR!=ZZ&&ZR<0xFFFE){
							AddWLink(ZZ,ZR);
							AddWLink(ZR,ZZ);
						};
					};
				};
			};
		};
	};
	ProcessMessages();
	if(WNAreas){
		WMotionLinks=znew(word,WNAreas*WNAreas);
		WLinksDist=znew(word,WNAreas*WNAreas);
		memset(WMotionLinks,0xFFFF,WNAreas*WNAreas*2);
		memset(WLinksDist,0xFFFF,WNAreas*WNAreas*2);
		CreateWLinkInfo();
	}else{
		WMotionLinks=NULL;
		WLinksDist=NULL;
	};
//	GTOP[0].LinksDist=LinksDist;
///	GTOP[0].MotionLinks=MotionLinks;
///	GTOP[0].GetNAreas()=GetNAreas();
///	GTOP[0].TopMap=TopMap;
///	GTOP[0].TopRef=TopRef;
///	GTOP[1].LinksDist=WLinksDist;
///	GTOP[1].MotionLinks=WMotionLinks;
///	GTOP[1].GetNAreas()=WNAreas;
///	GTOP[1].TopMap=WTopMap;
///	GTOP[1].TopRef=WTopRef;
};
void DrawAreas1(int xs,int ys,int x0,int y0,int Lx,int Ly){
	xs--;
	ys--;
	Lx>>=(WCLSH-2);
	Ly>>=(WCLSH-2);
	for(int ix=0;ix<Lx;ix++)
		for(int iy=0;iy<Ly;iy++){
			word tid=WTopRef[ix+(iy<<WTopSH)];
			if(tid<0xFFFF){
				if(tid==0xFFFE)tid=0;
				else tid=(tid&31)+0xD0;
				int ofs1=int(ScreenPtr)+xs+(ix<<(WCLSH-2))+((ys+(iy<<(WCLSH-2)))*ScrWidth);
				((byte*)ofs1)[0]=tid;
				((byte*)ofs1)[ScrWidth+1]=tid;
			};
		};
};
extern word TopAreasDanger[2048];
extern word WTopNear[64];
extern int NWTopNear;
void CBar(int x,int y,int Lx,int Ly,byte c);
extern int NIslands;
#define MaxIsl 64
extern int IslandX[MaxIsl];
extern int IslandY[MaxIsl];

extern word* TopIslands;
extern word* NearWater;
void DrawForces(int xs,int ys,int x0,int y0,int Lx,int Ly){
	xs--;
	ys--;
	int NA=GetNAreas();
	for(int i=0;i<NA;i++){
		char cc[10];
		sprintf_s( cc,sizeof(cc),"%d",TopIslands[i]);
		ShowString(GetTopMap(i)->x+xs-3,GetTopMap(i)->y+ys-3,cc,&fn10);
		int w=NearWater[i];
		if(w!=0xFFFF){
			ShowString(WTopMap[w].x+xs-3,WTopMap[w].y+ys-3,cc,&fn8);
		};
	};
	for(int i=0;i<NIslands;i++){
		CBar(IslandX[i]+xs-3,IslandY[i]+ys-3,6,6,0xD0+4*i);
	};
	/*
	for(int i=0;i<NWTopNear;i++){
		Area* AR=WTopMap+WTopNear[i];
		CBar(xs+AR->x-1,ys+AR->y-1,3,3,clrYello);
	};
	*/
	/*
	for(int i=0;i<WNAreas;i++){
		if(TopAreasDanger[i]){
			char cc[20];
			sprintf_s( cc,sizeof(cc),"%d",TopAreasDanger[i]);
			ShowString(WTopMap[i].x+xs-4,WTopMap[i].y+ys-5,cc,&fn8);
		};
	};
	*/
};



//----------------NEW!!!--------Dynamical zones remaking-----------
int SearchID(word* Set,int N,word ID){
	for(int i=0;i<N;i++)if(Set[i]==ID)return i;
	return -1;
};
bool Stop=0;
void EraseAllLinks(int Strat,int Fin);
struct OLDLINK{
	int  NAr;
	word* Links[2048];
	word NLinks[2048];
	word LinkBuf[40000];
	int LBPOS;
};
OLDLINK* OLN=NULL;
void SymmetrizeLinks(){
	int NA=GetNAreas();
	for(int i=0;i<NA;i++){
		Area* AR=GetTopMap(i);
		for(int j=0;j<AR->NLinks;j++){
			int L=AR->Link[j+j];
			AddLink(L,i);
			AddLink(i,L);
		};
	};
};
void RemakeGroupOfAreas(word* Grp,int Na){
	if(!Na)return;
	byte* LSET=znew(byte,Na*Na);
	memset(LSET,0,Na*Na);
	for(int q=0;q<Na;q++){
		int ID=Grp[q];
		Area* AR=GetTopMap(ID);
		int NL=AR->NLinks;
		for(int j=0;j<NL;j++){
			int ID1=AR->Link[j+j];
			int qq=SearchID(Grp,Na,ID1);
			if(qq!=-1){
				LSET[qq+q*Na]=1;
				LSET[qq*Na+q]=1;
			};
		};
	};
	OLN=new OLDLINK;
	OLN->LBPOS=0;
	int NA=GetNAreas();
	for(int q=0;q<NA;q++){
		Area* AR=GetTopMap(q);
		OLN->NLinks[q]=AR->NLinks;
		OLN->Links[q]=OLN->LinkBuf+OLN->LBPOS;
		OLN->LBPOS+=AR->NLinks;
		assert(OLN->LBPOS<40000);
		for(int j=0;j<AR->NLinks;j++){
			OLN->Links[q][j]=AR->Link[j+j];
		};
	};
	//clear old links&distances
	/*
	for(int i=0;i<Na;i++){
		int ID=Grp[i];
		Area* AR=GetTopMap(+ID;
		int k=AR->NLinks;
		for(int j=0;j<k;j++){
			int ofst=int(AR->Link[j+j])+ID*GetNAreas();
			MotionLinks[ofst]=0xFFFF;
			LinksDist[ofst]=0xFFFF;
		};
	};
	*/
	//clear
	int mmx=msx>>1;
	int mmy=msy>>1;
	for(int i=0;i<Na;i++){
		word ID=Grp[i];
		int minr=1000;
		Area* AR=GetTopMap(ID);
		int x0=AR->x;
		int y0=AR->y;
		int Nclr=0;
		for(int r=0;r<RRad&&r<minr+3;r++){
			char* xi=Rarr[r].xi;
			char* yi=Rarr[r].yi;
			int N=Rarr[r].N;
			for(int j=0;j<N;j++){
				int x1=x0+xi[j];
				int y1=y0+yi[j];
				if(x1>0&&y1>0&&x1<mmx&&y1<mmy){
					int ofst=x1+(y1<<TopSH);
					if(GetTopRef(ofst)==ID){
						TopRef[ofst]=0xFFFE;
						minr=r;
					};
				};
			};
		};
		//start point
		int ofst=x0+(y0<<TopSH);
		if(GetTopRef(ofst)==0xFFFE)TopRef[ofst]=ID;
		//Erase links
		AR->NLinks=0;
	};
	//filling areas and creating new links
	bool Change=true;
	for(int i=1;i<RRad&&Change;i++){
		Change=false;
		char* xi=Rarr[i].xi;
		char* yi=Rarr[i].yi;
		int N=Rarr[i].N;
		for(int m=0;m<Na;m++){
			int j=Grp[m];
			Area* AR=GetTopMap(j);
			int xx=AR->x;
			int yy=AR->y;
			for(int k=0;k<N;k++){
				int x=xx+xi[k];
				int y=yy+yi[k];
				if(x>0&&y>0&&x<mmx&&y<mmy){
					int ofst=x+y*TopLx;
					word ZZ=GetTopRef(ofst	    );
					word ZU=GetTopRef(ofst-TopLx);
					word ZD=GetTopRef(ofst+TopLx);
					word ZL=GetTopRef(ofst-1    );
					word ZR=GetTopRef(ofst+1    );
					if(ZZ==0xFFFE){
						if(ZU==j||ZD==j||ZL==j||ZR==j||
							(ZU!=0xFFFF&&ZL!=0xFFFF&&GetTopRef(ofst-TopLx-1)==j)||
							(ZU!=0xFFFF&&ZR!=0xFFFF&&GetTopRef(ofst-TopLx+1)==j)||
							(ZD!=0xFFFF&&ZL!=0xFFFF&&GetTopRef(ofst+TopLx-1)==j)||
							(ZD!=0xFFFF&&ZR!=0xFFFF&&GetTopRef(ofst+TopLx+1)==j)){
							TopRef[ofst]=j;
							Change=true;
							ZZ=j;
						};
					};
					if(ZZ<0xFFFE){
						if(ZU!=ZZ&&ZU<0xFFFE){
							AddLink(ZZ,ZU);
							AddLink(ZU,ZZ);
						};
						if(ZD!=ZZ&&ZD<0xFFFE){
							AddLink(ZZ,ZD);
							AddLink(ZD,ZZ);
						};
						if(ZL!=ZZ&&ZL<0xFFFE){
							AddLink(ZZ,ZL);
							AddLink(ZL,ZZ);
						};
						if(ZR!=ZZ&&ZR<0xFFFE){
							AddLink(ZZ,ZR);
							AddLink(ZR,ZZ);
						};
					};
				};
			};
		};
	};
	SymmetrizeLinks();
	//setting new links
	//determining erased links
	for(int q=0;q<Na;q++){
		int ID=Grp[q];
		Area* AR=GetTopMap(ID);
		int NL=AR->NLinks;
		for(int j=0;j<NL;j++){
			int ID1=AR->Link[j+j];
			int qq=SearchID(Grp,Na,ID1);
			if(qq!=-1){
				LSET[qq+q*Na]=2;
				LSET[qq*Na+q]=2;
			};
		};
	};
	int NBadLinks=0;
	word BadLinks[1024];
	//word BadL1[100000];
	//word BadL2[100000];
	//int NBadL=0;
	for(int q=0;q<Na;q++){
		for(int p=0;p<q;p++){
			if(LSET[q*Na+p]==1&&NBadLinks<512){
				BadLinks[NBadLinks+NBadLinks]=Grp[q];
				BadLinks[NBadLinks+NBadLinks+1]=Grp[p];
				NBadLinks++;
			};
		};
	};
	free(LSET);
	int NBAD=0;
	/*
	for(q=0;q<GetNAreas();q++){
		int ML_Q=q*GetNAreas();
		for(int p=0;p<GetNAreas();p++){
			int L_QP=LinksDist[ML_Q+p];
			if(L_QP<0xFFFE){
				int ML_P=p*GetNAreas();
				for(int t=0;t<NBadLinks;t++){
					int C=BadLinks[t+t];
					int D=BadLinks[t+t+1];
					int L_CD=LinksDist[C+D*GetNAreas()];
					int L_QC=q==C?0:LinksDist[ML_Q+C];
					int L_QD=q==D?0:LinksDist[ML_Q+D];
					int L_CP=p==C?0:LinksDist[p+C*GetNAreas()];
					int L_DP=p==D?0:LinksDist[p+D*GetNAreas()];
					int D_L=L_QP-L_CD;
					if(D_L==L_QC+L_DP||D_L==L_QD+L_CP){
						//bad link
						MotionLinks[ML_Q+p]=0xFFFF;
						//MotionLinks[ML_P+q]=0xFFFF;
						NBAD++;
					};
				};
			};
		};
	};
	*/
	for(int t=0;t<NBadLinks;t++){
		int C=BadLinks[t+t];
		int D=BadLinks[t+t+1];
		EraseAllLinks(C,D);
		EraseAllLinks(D,C);
	};
	
	int NN=NA*NA;
	for(int q=0;q<NN;q++)if(GetLinksDist(q)==0xFFFF)MotionLinks[q]=0xFFFF;
	//Stop=1;
	free(OLN);
};
void OnePartIterationNew(int Nitr){
#ifdef HASH_TOP
	return;
#endif
	//if(!TmpMLinks)return;
	//if(Stop)return;
	int NA=GetNAreas();
	int N=NA*NA;
	int ofs=CurIStart*NA;
	if(!CurIStart)TmpChanges=0;
	for(int i=CurIStart;i<NA;i++){
		for(int j=0;j<NA;j++){
			if(i!=j){
				Area* AR=GetTopMap(i);
				int N=AR->NLinks;
				word* LINK=AR->Link;
				int CurDis=GetLinksDist(ofs);
				//int CurDis=LinksDist[ofs];//
				for(int k=0;k<N;k++){
					word N2=LINK[k+k];
					word adds=LINK[k+k+1];
					if(N2!=j&&adds!=0xFFFF){
						int ofs1=j+N2*NA;
						int dst=GetLinksDist(ofs1);
						//int dst=LinksDist[ofs1];//
						if(dst!=0xFFFF){
							if(dst+adds<CurDis){
								int ddd=dst+adds;
								LinksDist[ofs]=ddd;
								//LinksDist[ofs]=ddd;//
								MotionLinks[ofs]=N2;
								//MotionLinks[ofs]=N2;//
								TmpChanges++;
							};
						};
					};
				};
				Nitr--;
			};
			ofs++;
		};
		CurIStart=i;
		Nitr--;
		if(Nitr<=0)return;
	};
	if(CurIStart==NA-1)CurIStart=0;
};
void EraseLinks(int X,int A){
	int N=OLN->NLinks[A];
	word* LINKS=OLN->Links[A];
	int NA=GetNAreas();
	for(int j=0;j<N;j++){
		int W=LINKS[j];
		int ofs=W*NA+X;
		int Next=GetMotionLinks(ofs);
		if(Next==A){
			LinksDist[ofs]=0xFFFF;
			EraseLinks(X,W);
		};
	};
};
void EraseAllLinks(int Start,int Fin){
	int NA=GetNAreas();
	int NS=Start*NA;
	LinksDist[Fin+NS]=0xFFFF;
	EraseLinks(Fin,Start);
	for(int j=0;j<NA;j++){
		int Next=GetMotionLinks(NS+j);
		if(Next==Fin){
			LinksDist[j+NS]=0xFFFF;
			EraseLinks(j,Start);
		};
	};
	LinksDist[Start*NA+Fin]=0xFFFF;
};


// -------- Topology extern function --------------

#ifdef HASH_TOP //------------- HASH_TOP --------------------

word GetLinksDist(int Ofs, byte TopType){
	HashTop* HT=HashTable[TopType].GetHashTop(Ofs);
	if(HT){
		return HT->LD;
	}	
	return 0xFFFF;
}

word GetMotionLinks(int Ofs, byte TopType){
	HashTop* HT=HashTable[TopType].GetHashTop(Ofs);
	if(HT){
		int ML=HT->ML;
		if(ML==8191)ML=0xFFFF;
		return ML;
	}
	return 0xFFFF;
}

word GetTopRef(int Ofs, byte TopType){
	/*if(TopType==0) return TopRef[Ofs];
	else if(TopType==1) return WTopRef[Ofs];
	else */if(TopType<NMFIELDS) return HashTable[TopType].TopRef[Ofs];
	else return 0xFFFF;
}

Area* GetTopMap(int Ofs, byte TopType){
	/*if(TopType==1){ if(WTopMap) return WTopMap+Ofs; }
	else if(TopType==0) return TopMap+Ofs;
	else */if(TopType<NMFIELDS) return HashTable[TopType].TopMap+Ofs;
	return NULL;
}

int GetNAreas(byte TopType){
	/*if(TopType==1) return WNAreas;
	else if(TopType==0) return NAreas;
	else*/ if(TopType<NMFIELDS) return HashTable[TopType].NAreas;
	return 0;
}

#else //------------- HASH_TOP --------------------

word GetTopRef(int Ofs, byte TopType){
	if(Ofs<0||Ofs>=MaxTop)return 0xFFFF;
	if(TopType==1) return WTopRef[Ofs];
		else return TopRef[Ofs];
}
int NAR0=0;
int NAR02=0;
int NAR1=0;
int NAR12=0;
word GetLinksDist(int Ofs, byte TopType){
	if(TopType==1){
		if(WNAreas!=NAR1){
			NAR12=WNAreas*WNAreas;
			NAR1=WNAreas;
		};
		if(Ofs<0||Ofs>NAR12)return 0xFFFF;
		else return WLinksDist[Ofs];
	}else{
		if(NAreas!=NAR0){
			NAR02=NAreas*NAreas;
			NAR0=NAreas;
		};
		if(Ofs<0||Ofs>NAR02)return 0xFFFF;
		else return LinksDist[Ofs];
	};
}

word GetMotionLinks(int Ofs, byte TopType){
	if(TopType==1){
		if(WNAreas!=NAR1){
			NAR12=WNAreas*WNAreas;
			NAR1=WNAreas;
		};
		if(Ofs<0||Ofs>NAR12)return 0xFFFF;
		else return WMotionLinks[Ofs];
	}else{
		if(NAreas!=NAR0){
			NAR02=NAreas*NAreas;
			NAR0=NAreas;
		};
		if(Ofs<0||Ofs>NAR02)return 0xFFFF;
		else return MotionLinks[Ofs];
	};
}

Area* GetTopMap(int Ofs, byte TopType){
	if(TopType==1){ if(WTopMap) return WTopMap+Ofs; }
		else return TopMap+Ofs;
	return NULL;
}

int GetNAreas(byte TopType){
	if(TopType==1) return WNAreas;
		else return NAreas;
}

#endif //------------- HASH_TOP --------------------


// --- Save-Load *.sav ---
extern bool NeedProcessTop;
extern bool WasOnlyOpen;
extern word NChAreas;
extern word MaxChAreas;
extern word* ChAreas;
//procesing variables
extern int CurIStart;
extern int TmpChanges;
extern word* TmpMLinks;
extern word* TmpMDist;
extern int MinChX;
extern int MaxChX;
extern int MinChY;
extern int MaxChY;

void LS_SaveTopology(SaveBuf* SB){
	xBlockWrite(SB,&NAreas,4);
	for(int j=0;j<NAreas;j++){
		Area Ar1=TopMap[j];
		xBlockWrite(SB,&Ar1,sizeof Area);
		Ar1.MaxLink=Ar1.NLinks;
		if(Ar1.NMines)xBlockWrite(SB,Ar1.MinesIdx,Ar1.NMines<<1);
		if(Ar1.NLinks)xBlockWrite(SB,Ar1.Link,Ar1.NLinks<<2);
	};
	xBlockWrite(SB,MotionLinks,NAreas*NAreas*2);
	xBlockWrite(SB,LinksDist,NAreas*NAreas*2);
	xBlockWrite(SB,TopRef,2*TopLx*TopLy);	
	//saving dynamical topology
	xBlockWrite(SB,&NeedProcessTop,1);
	xBlockWrite(SB,&WasOnlyOpen,1);
	xBlockWrite(SB,&NChAreas,2);
	xBlockWrite(SB,&MaxChAreas,2);
	if(NChAreas)xBlockWrite(SB,ChAreas,NChAreas<<1);
	xBlockWrite(SB,&CurIStart,4);
	xBlockWrite(SB,&TmpChanges,4);
	xBlockWrite(SB,&MinChX,4);
	xBlockWrite(SB,&MaxChX,4);
	xBlockWrite(SB,&MinChY,4);
	xBlockWrite(SB,&MaxChY,4);
	byte t=0;
	if(TmpMLinks){
		t=1;
		xBlockWrite(SB,&t,1);
		xBlockWrite(SB,TmpMLinks,NAreas*NAreas*2);
		xBlockWrite(SB,TmpMDist,NAreas*NAreas*2);
	}else xBlockWrite(SB,&t,1);

};
void CreateRoadsNet();
void ClearLinkInfo();
void LS_LoadTopology(SaveBuf* SB){
	ClearLinkInfo();
	xBlockRead(SB,&NAreas,4);
	MaxArea=NAreas;
	TopMap=new Area[NAreas];
	MotionLinks=znew(word,NAreas*NAreas);
	LinksDist=znew(word,NAreas*NAreas);
	for(int j=0;j<NAreas;j++){
		Area* Ar1=TopMap+j;
		xBlockRead(SB,Ar1,sizeof Area);
		if(Ar1->NMines)Ar1->MinesIdx=znew(word,Ar1->NMines);
		else Ar1->MinesIdx=NULL;
		if(Ar1->MaxLink)Ar1->Link=znew(word,Ar1->MaxLink<<1);
		else Ar1->Link=NULL;
		if(Ar1->NMines)xBlockRead(SB,Ar1->MinesIdx,Ar1->NMines<<1);
		if(Ar1->NLinks)xBlockRead(SB,Ar1->Link,Ar1->NLinks<<2);
	};
	xBlockRead(SB,MotionLinks,2*NAreas*NAreas);
	xBlockRead(SB,LinksDist,2*NAreas*NAreas);
	xBlockRead(SB,TopRef,2*TopLx*TopLy);
	//loading dynamical topology
	xBlockRead(SB,&NeedProcessTop,1);
	xBlockRead(SB,&WasOnlyOpen,1);
	xBlockRead(SB,&NChAreas,2);
	xBlockRead(SB,&MaxChAreas,2);
	if(MaxChAreas)ChAreas=znew(word,MaxChAreas);
	if(NChAreas)xBlockRead(SB,ChAreas,NChAreas<<1);
	xBlockRead(SB,&CurIStart,4);
	xBlockRead(SB,&TmpChanges,4);
	xBlockRead(SB,&MinChX,4);
	xBlockRead(SB,&MaxChX,4);
	xBlockRead(SB,&MinChY,4);
	xBlockRead(SB,&MaxChY,4);
	byte t;
	xBlockRead(SB,&t,1);
	if(t){
		TmpMLinks=znew(word,NAreas* NAreas);
		TmpMDist=znew(word,NAreas* NAreas);
		xBlockRead(SB,TmpMLinks,NAreas*NAreas*2);
		xBlockRead(SB,TmpMDist,NAreas*NAreas*2);
	};
	//CreateRoadsNet();
};

// --- Save-Load *.m3d ---
void CreateTotalLocking();
void EraseAreas();
void SaveTopology(ResFile f1){
	EraseAreas();
	rando();//!!
	CreateTotalLocking();
	int i='1POT';
	RBlockWrite(f1,&i,4);
	i=4+4+NAreas*sizeof(Area)+4*NAreas*NAreas+2*TopLx*TopLy;
	for(int j=0;j<NAreas;j++){
		Area* Ar1=TopMap+j;
		i+=(Ar1->NMines+(Ar1->NLinks<<1))<<1;
	};
	RBlockWrite(f1,&i,4);
	RBlockWrite(f1,&NAreas,4);
	for(int j=0;j<NAreas;j++){
		Area Ar1=TopMap[j];
		RBlockWrite(f1,&Ar1,sizeof Area);
		Ar1.MaxLink=Ar1.NLinks;
		if(Ar1.NMines)RBlockWrite(f1,Ar1.MinesIdx,Ar1.NMines<<1);
		if(Ar1.NLinks)RBlockWrite(f1,Ar1.Link,Ar1.NLinks<<2);
	};
	RBlockWrite(f1,MotionLinks,NAreas*NAreas*2);
	RBlockWrite(f1,LinksDist,NAreas*NAreas*2);
	RBlockWrite(f1,TopRef,2*TopLx*TopLy);
};
void CreateRoadsNet();
/*
void LoadTopology(ResFile f1){
	EraseAreas();
	int i;
	RBlockRead(f1,&NAreas,4);
	MaxArea=NAreas;
	TopMap=new Area[NAreas];
	MotionLinks=znew(word,NAreas*NAreas);
	LinksDist=znew(word,NAreas*NAreas);
	for(int j=0;j<NAreas;j++){
		Area* Ar1=TopMap+j;
		RBlockRead(f1,Ar1,sizeof Area);
		if(Ar1->NMines)Ar1->MinesIdx=znew(word,Ar1->NMines);
		else Ar1->MinesIdx=NULL;
		if(Ar1->NLinks)Ar1->Link=znew(word,Ar1->NLinks);
		else Ar1->Link=NULL;
		if(Ar1->NMines)RBlockRead(f1,Ar1->MinesIdx,Ar1->NMines<<1);
		if(Ar1->NLinks)RBlockRead(f1,Ar1->Link,Ar1->NLinks<<1);
	};
	RBlockRead(f1,MotionLinks,2*NAreas*NAreas);
	RBlockRead(f1,LinksDist,2*NAreas*NAreas);
	RBlockRead(f1,TopRef,2*TopLx*TopLy);
	//CreateRoadsNet();
};
*/
void SaveWTopology(ResFile f1){
	int i='WPOT';
	RBlockWrite(f1,&i,4);
	i=4+4+WNAreas*sizeof(Area)+4*WNAreas*WNAreas+2*TopLx*TopLy;
	for(int j=0;j<WNAreas;j++){
		Area* Ar1=WTopMap+j;
		i+=(Ar1->NMines+(Ar1->NLinks<<1))<<1;
	};
	RBlockWrite(f1,&i,4);
	RBlockWrite(f1,&WNAreas,4);
	for(int j=0;j<WNAreas;j++){
		Area Ar1=WTopMap[j];
		RBlockWrite(f1,&Ar1,sizeof Area);
		Ar1.MaxLink=Ar1.NLinks;
		if(Ar1.NMines)RBlockWrite(f1,Ar1.MinesIdx,Ar1.NMines<<1);
		if(Ar1.NLinks)RBlockWrite(f1,Ar1.Link,Ar1.NLinks<<2);
	};
	RBlockWrite(f1,WMotionLinks,WNAreas*WNAreas*2);
	RBlockWrite(f1,WLinksDist,WNAreas*WNAreas*2);
	RBlockWrite(f1,WTopRef,2*TopLx*TopLy);
};
void LoadTopology1(ResFile f1){
	EraseAreas();
//	int i;
	RBlockRead(f1,&NAreas,4);
	MaxArea=NAreas;
	TopMap=new Area[NAreas];
	MotionLinks=znew(word,NAreas*NAreas);
	LinksDist=znew(word,NAreas*NAreas);
	for(int j=0;j<NAreas;j++){
		Area* Ar1=TopMap+j;
		RBlockRead(f1,Ar1,sizeof Area);
		if(Ar1->NMines)Ar1->MinesIdx=znew(word,Ar1->NMines);
		else Ar1->MinesIdx=NULL;
		if(Ar1->NLinks)Ar1->Link=znew(word,Ar1->MaxLink<<1);
		else Ar1->Link=NULL;
		if(Ar1->NMines)RBlockRead(f1,Ar1->MinesIdx,Ar1->NMines<<1);
		if(Ar1->NLinks)RBlockRead(f1,Ar1->Link,Ar1->NLinks<<2);
	};
	RBlockRead(f1,MotionLinks,2*NAreas*NAreas);
	RBlockRead(f1,LinksDist,2*NAreas*NAreas);
	RBlockRead(f1,TopRef,2*TopLx*TopLy);
	//CreateRoadsNet();
};
void ResearchIslands();
void LoadWTopology1(ResFile f1){
//	int i;
	RBlockRead(f1,&WNAreas,4);
	WTopMap=new Area[WNAreas];
	WMotionLinks=znew(word,WNAreas*WNAreas);
	WLinksDist=znew(word,WNAreas*WNAreas);
	for(int j=0;j<WNAreas;j++){
		Area* Ar1=WTopMap+j;
		RBlockRead(f1,Ar1,sizeof Area);
		if(Ar1->NMines)Ar1->MinesIdx=znew(word,Ar1->NMines);
		else Ar1->MinesIdx=NULL;
		if(Ar1->NLinks)Ar1->Link=znew(word,Ar1->MaxLink<<1);
		else Ar1->Link=NULL;
		if(Ar1->NMines)RBlockRead(f1,Ar1->MinesIdx,Ar1->NMines<<1);
		if(Ar1->NLinks)RBlockRead(f1,Ar1->Link,Ar1->NLinks<<2);
	};
	RBlockRead(f1,WMotionLinks,2*WNAreas*WNAreas);
	RBlockRead(f1,WLinksDist,2*WNAreas*WNAreas);
	RBlockRead(f1,WTopRef,2*TopLx*TopLy);
	ResearchIslands();
};

// --- Init ---
void ClearTopology(){
	/*NOCRASH(*/memset(TopRef,0,TopLx*TopLx*2);//)
	/*NOCRASH(*/memset(WTopRef,0,WTopLx*WTopLx*2);//)
};
void SetupTopology(){
	TopRef=znew(word,TopLx*TopLx);
	//szz=TopLx*TopLx*2;
	//ARRSZ+=szz;
	memset(TopRef,0,TopLx*TopLx*2);
	WTopRef=znew(word,WTopLx*WTopLx);
	//szz=WTopLx*WTopLx*2;
	//ARRSZ+=szz;
	memset(WTopRef,0,WTopLx*WTopLx*2);

#ifdef HASH_TOP
	for(int i=0;i<NMFIELDS;i++){
		HashTable[i].SetUp(i);
	}
#endif //HASH_TOP

};

void FreeTopology(){
	free(TopRef);
	TopRef=NULL;
	free(WTopRef);
	WTopRef=NULL;
#ifdef HASH_TOP
	for(int i=0;i<NMFIELDS;i++){
		HashTable[i].Free();
	}
#endif //HASH_TOP
};
