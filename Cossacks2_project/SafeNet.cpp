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
class NetSample{
public:
	word* Danger;
	word* Pretty;
	int NZ;
	int LastUpdate;
	void CreateDiversantMap(byte NI);
	void CreateGrenadersMap(byte NI);
	NetSample();
	~NetSample();
};
class SafeNet{
public:
	NetSample Diversant;
	NetSample Grenader;
	/*
	int LastUpdate;
	byte NI;
	int NZ;
	word* CellDanger;
	word* CellPretty;
	*/
	word FindNextCell(int F,int Start,NetSample* Net);
	word FindWayTo(int F,int Start,int Fin,NetSample* Net);
	//void CreateMaps();
	SafeNet();
	~SafeNet();

};
NetSample::NetSample(){
	NZ=0;
	Danger=NULL;
	Pretty=NULL;
	LastUpdate=0;
};
NetSample::~NetSample(){
	if(Danger)free(Danger);
	if(Pretty)free(Pretty);
	NZ=0;
	Danger=NULL;
	Pretty=NULL;
};
SafeNet::SafeNet(){
};
SafeNet::~SafeNet(){
};
extern int tmtmt;
#ifndef AIFIX
void NetSample::CreateDiversantMap(byte NI) {
    int NA = GetNAreas();
    if (tmtmt - LastUpdate < 20 && NZ == NA)return;
    if (NZ != NA) {
        Danger = (word*)realloc(Danger, 2 * NA);
        Pretty = (word*)realloc(Pretty, 2 * NA);
        NZ = NA;
    };
    int N2 = 2 * NA;
    word addDam[2048];
    memset(Danger, 0, N2);
    memset(Pretty, 0, N2);
    memset(addDam, 0, N2);
    byte Mask = 1 << NI;
    for (int i = 0; i < MAXOBJECT; i++) {
        OneObject* OB = Group[i];
        if (OB && !(OB->Sdoxlo || OB->NMask & Mask)) {
            NewMonster* NM = OB->newMons;
            byte Use = NM->Usage;
            int Pret = 0;
            int Dang = 0;
            if (NM->Capture) {
                if (NM->Building) {
                    if (NM->Producer)Pret = 40;
                    else {
                        switch (Use) {
                        case CenterID:
                            Pret = 40;
                            break;
                        case MelnicaID:
                            Pret = 20;
                            break;
                        case SkladID:
                            Pret = 0;
                            break;
                        default:
                            Pret = 20;
                        };
                    };
                };
            }
            else {
                switch (Use) {
                case HardHorceID:
                    Dang = 3;
                    break;
                case FastHorseID:
                    Dang = 2;
                    break;
                case PeasantID:
                    Pret = 1;
                    break;
                default:
                    if (OB->AddDamage > 5)Dang = 2;
                    else Dang = 1;
                };
            };
            if (Dang || Pret) {
                int cx = OB->RealX >> 10;
                int cy = OB->RealY >> 10;
                int ofs = cx + (cy << TopSH);
                if (ofs >= 0 || ofs < TopLx * TopLy) {
                    word Top = GetTopRef(ofs);
                    if (Top < 0xFFFE) {
                        Danger[Top] += Dang;
                        Pretty[Top] += Pret;
                    };
                };
            };
        };
    };

    for (int i = 0; i < NA; i++) {
        if (Danger[i] > 2) {
            Area* AR = GetTopMap(i);
            int NL = AR->NLinks;
            for (int j = 0; j < NL; j++) {
                int id = AR->Link[j + j];
                int dam = Danger[id];
                if (dam) {
                    addDam[i] += (dam >> 1) + (dam >> 2);
                };
            };
        };
    };
    for (int i = 0; i < NA; i++)Danger[i] += addDam[i];
};
#else
void NetSample::CreateDiversantMap(byte NI) {
    const int NA = GetNAreas();
    if (tmtmt - LastUpdate < 20 && NZ == NA) return;

    // Safely reallocate memory
    word* newDanger = (word*)realloc(Danger, 2 * NA * sizeof(word));
    word* newPretty = (word*)realloc(Pretty, 2 * NA * sizeof(word));
    if (!newDanger || !newPretty) {
        free(Danger);
        free(Pretty);
        Danger = Pretty = NULL;
        NZ = 0;
        return;
    }
    Danger = newDanger;
    Pretty = newPretty;
    NZ = NA;

    // Initialize arrays
    memset(Danger, 0, NA * sizeof(word));
    memset(Pretty, 0, NA * sizeof(word));

    // Temporary damage accumulator
    word* addDam = (word*)calloc(NA, sizeof(word));
    if (!addDam) return;

    const byte Mask = 1 << NI;

    // Process objects
    for (int i = 0; i < MAXOBJECT; i++) {
        OneObject* OB = Group[i];
        if (!OB || OB->Sdoxlo || (OB->NMask & Mask)) continue;

        NewMonster* NM = OB->newMons;
        byte Use = NM->Usage;
        int Pret = 0, Dang = 0;

        if (NM->Capture) {
            if (NM->Building) {
                Pret = (NM->Producer) ? 40 :
                    (Use == CenterID) ? 40 :
                    (Use == MelnicaID) ? 20 : 20;
            }
        }
        else {
            switch (Use) {
            case HardHorceID: Dang = 3; break;
            case FastHorseID: Dang = 2; break;
            case PeasantID: Pret = 1; break;
            default: Dang = (OB->AddDamage > 5) ? 2 : 1;
            }
        }

        if (Dang || Pret) {
            const int cx = OB->RealX >> 10;
            const int cy = OB->RealY >> 10;
            const int ofs = cx + (cy << TopSH);

            if (ofs >= 0 && ofs < TopLx * TopLy) {
                const word Top = GetTopRef(ofs);
                if (Top < 0xFFFE) {
                    Danger[Top] += Dang;
                    Pretty[Top] += Pret;
                }
            }
        }
    }

    // Propagate danger to adjacent areas
    for (int i = 0; i < NA; i++) {
        if (Danger[i] > 2) {
            Area* AR = GetTopMap(i);
            for (int j = 0; j < AR->NLinks; j++) {
                const word id = AR->Link[j * 2];
                addDam[i] += (Danger[id] >> 1) + (Danger[id] >> 2);
            }
        }
    }

    // Merge accumulated danger
    for (int i = 0; i < NA; i++) {
        Danger[i] += addDam[i];
    }

    free(addDam);
    LastUpdate = tmtmt;
}
#endif
void NetSample::CreateGrenadersMap(byte NI){
	int NA=GetNAreas();
	if(tmtmt-LastUpdate<20&&NZ==NA)return;
	if(NZ!=NA){
		Danger=(word*)realloc(Danger,2*NA);
		Pretty=(word*)realloc(Pretty,2*NA);
		NZ=NA;
	};
	int N2=2*NA;
	word addDam[2048];
	memset(Danger,0,N2);
	memset(Pretty,0,N2);
	memset(addDam,0,N2);
	byte Mask=1<<NI;
	for(int i=0;i<MAXOBJECT;i++){
		OneObject* OB=Group[i];
		int Pret=0;
		int Dang=0;
		if(OB&&(!OB->Sdoxlo)&&!(OB->NMask&Mask)){
			NewMonster* NM=OB->newMons;
			byte Use=NM->Usage;
			if(OB->Wall){
				if(!(OB->Ready&&OB->SafeWall)){
					int cx=OB->RealX>>10;
					int cy=OB->RealY>>10;
					if(cx>0&&cx<TopLx-1&&cy>0&&cy<TopLy-1){
						int ofs=cx+(cy<<TopSH);
						word Top=GetTopRef(ofs+1);
						if(Top<0xFFFE)Pretty[Top]+=10;
						Top=GetTopRef(ofs-1);
						if(Top<0xFFFE)Pretty[Top]+=10;
						Top=GetTopRef(ofs-TopLx);
						if(Top<0xFFFE)Pretty[Top]+=10;
						Top=GetTopRef(ofs+TopLx);
						if(Top<0xFFFE)Pretty[Top]+=10;
					};
				};
			}else{
				if(Use==TowerID)Pret=20;
				else
				if(Use==StrelokID||Use==HorseStrelokID||Use==HardHorceID||Use==FastHorseID)Dang=3;
				else{
					if(NM->Capture){
						Pret=5;
					};
					if(Use==LightInfID)Dang=1;
				};
				if(Dang||Pret){
					int cx=OB->RealX>>10;
					int cy=OB->RealY>>10;
					int ofs=cx+(cy<<TopSH);
					if(ofs>=0||ofs<TopLx*TopLy){
						word Top=GetTopRef(ofs);
						if(Top<0xFFFE){
							Danger[Top]+=Dang;
							Pretty[Top]+=Pret;
						};
					};
				};
			};
		};
	};
};
#define MAXAR 700
#ifndef AIFIX
word SafeNet::FindNextCell(int F, int Cell, NetSample* Net) {
    int NZ = Net->NZ;
    if (Cell < NZ) {
        int F0 = F >> 2;
        int F1 = F >> 1;
        int F2 = F0 + F1;
        int F3 = F + F0;
        word* CellDanger = Net->Danger;
        word* CellPretty = Net->Pretty;
        word AddPrett[2048];
        memcpy(AddPrett, CellPretty, NZ << 1);
        int NA = GetNAreas();
        for (int i = 0; i < NA; i++) {
            word cdn = CellDanger[i];
            if (cdn <= F1 && cdn > 3) {
                AddPrett[i] += 20;
            };
        };
        if (AddPrett[Cell])return Cell;
        byte CantGo[MAXAR];
        for (int i = 0; i < NA; i++) {
            word dang = CellDanger[i];
            CantGo[i] = dang > F1;
        };
        for (int i = 0; i < NA; i++) {
            if (CantGo[i] == 1) {
                Area* AR = GetTopMap(i);
                int NL = AR->NLinks;
                for (int j = 0; j < NL; j++) {
                    word id = AR->Link[j + j];
                    if (!CantGo[id])CantGo[id] = 2;
                };
            };
        };

        //now we are ready to find a way

        word DistArr[2048];
        word DistID[2048];
        byte Checked[2048];

        memset(DistArr, 0, NA << 1);
        memset(Checked, 0, NA);
        word BoundCells[200];
        int NBound = 0;
        word NewBound[200];
        int NNewB = 0;

        NBound = 1;
        BoundCells[0] = Cell;
        DistArr[Cell] = 1;
        word LastPrettyID = 0xFFFF;
        do {
            NNewB = 0;
            for (int i = 0; i < NBound; i++)Checked[BoundCells[i]] = 1;
            for (int i = 0; i < NBound; i++) {
                int stp = BoundCells[i];
                Area* BA = GetTopMap(stp);
                int N = BA->NLinks;
                int L0 = DistArr[i];
                for (int j = 0; j < N; j++) {
                    word id = BA->Link[j + j];
                    if (!(CantGo[id] || Checked[id] == 1)) {
                        int d = L0 + BA->Link[j + j + 1];
                        if (DistArr[id]) {
                            if (d < DistArr[id]) {
                                DistID[id] = stp;
                                DistArr[id] = d;
                                if (AddPrett[id]) {
                                    LastPrettyID = id;
                                };
                            };
                        }
                        else {
                            DistID[id] = stp;
                            DistArr[id] = d;
                            if (AddPrett[id]) {
                                LastPrettyID = id;
                            };
                        };
                        if (!Checked[id]) {
                            Checked[id] = 2;
                            NewBound[NNewB] = id;
                            NNewB++;
                        };
                    };
                };
            };
            memcpy(BoundCells, NewBound, NNewB << 1);
            NBound = NNewB;
        } while (LastPrettyID == 0xFFFF && NNewB);
        if (LastPrettyID != 0xFFFF) {
            int PreCell = LastPrettyID;
            while (LastPrettyID != Cell) {
                PreCell = LastPrettyID;
                LastPrettyID = DistID[LastPrettyID];
            };
            return PreCell;
        };
        return 0xFFFF;
    };
    return 0xFFFF;
};
#else
word SafeNet::FindNextCell(int F, int Cell, NetSample* Net) {
    // Validate inputs
    if (!Net || Cell >= Net->NZ || Net->NZ == 0) {
        return 0xFFFF;  // Invalid input or empty network
    }

    const int NA = GetNAreas();
    if (NA != Net->NZ) {
        return 0xFFFF;  // Topology mismatch
    }

    const int F1 = F >> 1;  // Danger threshold

    // Allocate and initialize prettiness modifier array
    word* AddPrett = (word*)calloc(NA, sizeof(word));
    if (!AddPrett) {
        return 0xFFFF;  // Memory allocation failed
    }

    // 1. Enhance prettiness near danger zones
    memcpy(AddPrett, Net->Pretty, NA * sizeof(word));
    for (int i = 0; i < NA; i++) {
        if (Net->Danger[i] <= F1 && Net->Danger[i] > 3) {
            AddPrett[i] += 20;  // Bonus for moderately dangerous areas
        }
    }

    // 2. Early exit if current cell is acceptable
    if (AddPrett[Cell] > 0) {
        free(AddPrett);
        return Cell;
    }

    // 3. Mark blocked areas (2-pass approach)
    byte* CantGo = (byte*)calloc(NA, 1);
    if (!CantGo) {
        free(AddPrett);
        return 0xFFFF;
    }

    // First pass: mark directly dangerous areas
    for (int i = 0; i < NA; i++) {
        CantGo[i] = (Net->Danger[i] > F1) ? 1 : 0;
    }

    // Second pass: mark areas adjacent to danger zones
    for (int i = 0; i < NA; i++) {
        if (CantGo[i] == 1) {
            Area* AR = GetTopMap(i);
            for (int j = 0; j < AR->NLinks; j++) {
                const word neighbor = AR->Link[j * 2];
                if (CantGo[neighbor] == 0) {
                    CantGo[neighbor] = 2;  // Mark as adjacent to danger
                }
            }
        }
    }

    // 4. BFS Setup
    word* DistArr = (word*)calloc(NA, sizeof(word));
    word* DistID = (word*)calloc(NA, sizeof(word));
    byte* Checked = (byte*)calloc(NA, 1);
    if (!DistArr || !DistID || !Checked) {
        free(AddPrett); free(CantGo); free(DistArr); free(DistID); free(Checked);
        return 0xFFFF;
    }

    word BoundCells[256];
    word NewBound[256];
    int NBound = 1;
    BoundCells[0] = Cell;
    DistArr[Cell] = 1;  // Distance to self is 1

    // 5. BFS Execution
    word BestCell = 0xFFFF;
    int BestScore = -1;

    do {
        int NNewB = 0;

        // Mark current boundary as processed
        for (int i = 0; i < NBound; i++) {
            Checked[BoundCells[i]] = 1;
        }

        // Process each cell in current boundary
        for (int i = 0; i < NBound; i++) {
            const word current = BoundCells[i];
            Area* AR = GetTopMap(current);
            const int currentDist = DistArr[current];

            // Evaluate all neighbors
            for (int j = 0; j < AR->NLinks; j++) {
                const word neighbor = AR->Link[j * 2];
                const int linkCost = AR->Link[j * 2 + 1];

                // Skip if blocked or already processed
                if (CantGo[neighbor] || Checked[neighbor]) {
                    continue;
                }

                const int newDist = currentDist + linkCost;

                // Update if found a better path
                if (DistArr[neighbor] == 0 || newDist < DistArr[neighbor]) {
                    DistArr[neighbor] = newDist;
                    DistID[neighbor] = current;

                    // Track best candidate
                    if (AddPrett[neighbor] > 0) {
                        const int score = AddPrett[neighbor] * 1000 - newDist;  // Prettier and closer = better
                        if (score > BestScore) {
                            BestScore = score;
                            BestCell = neighbor;
                        }
                    }

                    // Add to next boundary if not already there
                    if (!Checked[neighbor]) {
                        NewBound[NNewB++] = neighbor;
                        Checked[neighbor] = 2;  // Mark as in queue
                    }
                }
            }
        }

        // Prepare next iteration
        memcpy(BoundCells, NewBound, NNewB * sizeof(word));
        NBound = NNewB;
    } while (NBound > 0 && BestCell == 0xFFFF);

    // 6. Backtrack to find next step
    word Result = 0xFFFF;
    if (BestCell != 0xFFFF) {
        // Trace back to find first step from origin
        word current = BestCell;
        while (DistID[current] != Cell && DistID[current] != 0xFFFF) {
            current = DistID[current];
        }
        Result = current;
    }

    // Cleanup
    free(AddPrett); free(CantGo); free(DistArr); free(DistID); free(Checked);
    return Result;
}
#endif
#ifndef AIFIX
word SafeNet::FindWayTo(int F, int Cell, int Fin, NetSample* Net) {
    //CreateMaps();
    if (Cell < Net->NZ) {
        word* CellDanger = Net->Danger;
        int F0 = F >> 2;
        int F1 = F >> 1;
        int F2 = F0 + F1;
        int F3 = F + F0;
        byte CantGo[MAXAR];
        int NA = GetNAreas();
        for (int i = 0; i < NA; i++) {
            word dang = CellDanger[i];
            CantGo[i] = dang > F1;
        };

        //now we are ready to find a way

        word DistArr[2048];
        word DistID[2048];
        byte Checked[2048];

        memset(DistArr, 0, NA << 1);
        memset(Checked, 0, NA);
        word BoundCells[200];
        int NBound = 0;
        word NewBound[200];
        int NNewB = 0;

        NBound = 1;
        BoundCells[0] = Cell;
        DistArr[Cell] = 1;
        word LastPrettyID = 0xFFFF;
        do {
            NNewB = 0;
            for (int i = 0; i < NBound; i++)Checked[BoundCells[i]] = 1;
            for (int i = 0; i < NBound; i++) {
                int stp = BoundCells[i];
                Area* BA = GetTopMap(stp);
                int N = BA->NLinks;
                int L0 = DistArr[i];
                for (int j = 0; j < N; j++) {
                    word id = BA->Link[j + j];
                    if (!(CantGo[id] || Checked[id] == 1)) {
                        int d = L0 + BA->Link[j + j + 1];
                        if (DistArr[id]) {
                            if (d < DistArr[id]) {
                                DistID[id] = stp;
                                DistArr[id] = d;
                                if (id == Fin) {
                                    LastPrettyID = id;
                                };
                            };
                        }
                        else {
                            DistID[id] = stp;
                            DistArr[id] = d;
                            if (id == Fin) {
                                LastPrettyID = id;
                            };
                        };
                        if (!Checked[id]) {
                            Checked[id] = 2;
                            NewBound[NNewB] = id;
                            NNewB++;
                        };
                    };
                };
            };
            memcpy(BoundCells, NewBound, NNewB << 1);
            NBound = NNewB;
        } while (LastPrettyID == 0xFFFF && NNewB);
        if (LastPrettyID != 0xFFFF) {
            int PreCell = LastPrettyID;
            while (LastPrettyID != Cell) {
                PreCell = LastPrettyID;
                LastPrettyID = DistID[LastPrettyID];
            };
            return PreCell;
        };
        return 0xFFFF;
    };
    return 0xFFFF;
};
#else
word SafeNet::FindWayTo(int F, int Cell, int Fin, NetSample* Net) {
    if (Cell >= Net->NZ || Fin >= Net->NZ) return 0xFFFF;
    if (Cell == Fin) return Fin;  // Already at destination

    const int F1 = F >> 1;
    const int NA = GetNAreas();

    // Mark dangerous areas
    byte* CantGo = (byte*)calloc(NA, 1);
    if (!CantGo) return 0xFFFF;

    for (int i = 0; i < NA; i++) {
        CantGo[i] = (Net->Danger[i] > F1) ? 1 : 0;
    }

    // BFS setup
    word* DistArr = (word*)calloc(NA, sizeof(word));
    word* DistID = (word*)calloc(NA, sizeof(word));
    byte* Checked = (byte*)calloc(NA, 1);
    if (!DistArr || !DistID || !Checked) {
        free(CantGo); free(DistArr); free(DistID); free(Checked);
        return 0xFFFF;
    }

    word BoundCells[256];
    word NewBound[256];
    int NBound = 1, NNewB = 0;
    BoundCells[0] = Cell;
    DistArr[Cell] = 1;

    // BFS loop
    word LastID = 0xFFFF;
    do {
        NNewB = 0;
        for (int i = 0; i < NBound; i++) Checked[BoundCells[i]] = 1;

        for (int i = 0; i < NBound; i++) {
            const word stp = BoundCells[i];
            Area* BA = GetTopMap(stp);
            const int L0 = DistArr[stp];

            for (int j = 0; j < BA->NLinks; j++) {
                const word id = BA->Link[j * 2];
                if (CantGo[id] || Checked[id]) continue;

                const int d = L0 + BA->Link[j * 2 + 1];
                if (!DistArr[id] || d < DistArr[id]) {
                    DistArr[id] = d;
                    DistID[id] = stp;
                    if (id == Fin) LastID = id;
                    if (!Checked[id]) NewBound[NNewB++] = id;
                }
            }
        }

        memcpy(BoundCells, NewBound, NNewB * sizeof(word));
        NBound = NNewB;
    } while (LastID == 0xFFFF && NBound > 0);

    // Backtrack path
    word Result = 0xFFFF;
    if (LastID != 0xFFFF) {
        while (DistID[LastID] != Cell) {
            LastID = DistID[LastID];
        }
        Result = LastID;
    }

    free(CantGo); free(DistArr); free(DistID); free(Checked);
    return Result;
}
#endif
SafeNet SAFNET[8];
word GetNextSafeCell(byte NI,int F,int start,int Fin){
	SAFNET[NI].Diversant.CreateDiversantMap(NI);
	return SAFNET[NI].FindWayTo(F,start,Fin,&SAFNET[NI].Diversant);
};
word GetNextDivCell(byte NI,int F,int Start){
	SAFNET[NI].Diversant.CreateDiversantMap(NI);
	return SAFNET[NI].FindNextCell(F,Start,&SAFNET[NI].Diversant);
};
word GetNextGreCell(byte NI,int F,int Start){
	SAFNET[NI].Grenader.CreateGrenadersMap(NI);
	return SAFNET[NI].FindNextCell(F,Start,&SAFNET[NI].Grenader);
};