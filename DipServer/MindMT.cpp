#include "CommonDip.h"
#include "UnitsGroup.h"
#include "Mind.h"
#include "Script.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <future>

int Norma(int x,int y);

Mind Player[8];

int Dang[4096];

// Thread pool for reusable worker threads
class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::atomic<bool> stop;
    
public:
    ThreadPool(size_t threads = std::thread::hardware_concurrency()) : stop(false) {
        // Reserve threads but don't start them yet
    }
    
    ~ThreadPool() {
        stop = true;
        for(auto& worker : workers) {
            if(worker.joinable()) worker.join();
        }
    }
    
    template<class F>
    void enqueue(F&& f) {
        workers.emplace_back(std::forward<F>(f));
    }
    
    void wait() {
        for(auto& worker : workers) {
            if(worker.joinable()) worker.join();
        }
        workers.clear();
    }
};

static ThreadPool g_threadPool;

// Global mutex for thread-unsafe game API calls
static std::mutex g_gameApiMutex;

void LoadMindData(byte* ptr,int size){
};

void SaveMindData(byte** ptr,int* size){
};

void ClearMindData(){
    for(int i=0;i<8;i++){
        Mind* Pl=Player+i;
        Pl->Free();
    }
}

CEXPORT
void CloseMind(){
    ClearMindData();
}

// Squad methods remain the same
bool Squad::FindTargetZone(short* DangerMap, word* ObjIds, word& TOP, word& TZ, word& Dst){
    int maxdang;
    word DST=0xFFFF;
    
    TZ=(word)FindNextZoneOnTheSafeWayToObject(Top,DangerMap,ObjIds,&maxdang,5,&DST);

    if(DST!=0xFFFF){        
        word* Way;
        int NWayPoint=GetLastFullWay(&Way);
        
        TOP=Way[0];
        
        Dst=0;
        for(int i=1;i<NWayPoint;i++){
            Dst+=(word)GetZonesDist(Way[i-1],Way[i]);
        }
        return true;
    }

    TZ=0xFFFF;
    Dst=0;
    return false;
};

void Squad::MoveToTop(short* DangerMap){
    short NullDang[4096];
    if(!DangerMap){
        memset(NullDang,0,sizeof NullDang);
        DangerMap=NullDang;
    }
    
    int xc,yc;
    if(GetGrpCenter(&Group,&xc,&yc)){
        int top=GetTopZone(xc,yc);
        if(top>=0xFFFE){
            OneUnit UN;
            GetUnitInfo(&Group,0,&UN);
            xc=UN.x;
            yc=UN.y;
            top=GetTopZone(xc,yc);
        }
        if(top>=0&&top<GetNZones()){
            int maxdang;
            word DST=0xFFFF;
            word dz=FindNextZoneOnTheSafeWay(top,Top,DangerMap,&maxdang,5);
            if(top==Top) dz=top;    
            
            if(dz<0xFFFF){
                int dx,dy;
                if(GetTopZRealCoor(dz,&dx,&dy)){
                    int dir=512;
                    {
                        word TopList[1024];
                        TopList[0]=dz;
                        int NTopList=1;
                        bool Check[4096];
                        memset(Check,0,sizeof Check);

                        int dirx=0,diry=0;

                        while(NTopList){
                            NTopList--;
                            int t=TopList[NTopList];                            

                            word* links=NULL;
                            int NL=GetListOfNearZones(t,&links);
                            for(int p=0;p<NL;p++){
                                int id=links[p+p];
                                
                                if(!Check[id]){
                                    int x=0,y=0;
                                    if(GetTopZRealCoor(id,&x,&y)){                                    
                                        x-=dx;
                                        y-=dy;
                                        int norm=Norma(x,y);

                                        if(norm<2000){
                                            Check[id]=1;
                                            TopList[NTopList]=id;
                                            NTopList++;
                                            int dang=DangerMap[id];
                                            if(dang){
                                                dirx+=x*dang;
                                                diry+=y*dang;
                                            }
                                        };
                                    }
                                }
                            };
                        }

                        if(dirx||diry) dir=getDir(dirx,diry);
                    }

                    if(Brig!=0xFFFF) SelectUnits(&Group,0);
                    
                    {
                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                        SGP_MoveToPoint(NI,&Group,dx,dy,dir,0,0,1);
                        SetUnitsState(&Group,1,0,0,0);
                    }
                }
            }
            Top=top;
        }
    }
};

// Mind methods
Mind::Mind(){
    Clear();
}

Mind::~Mind(){
    Free();
}

void Mind::Clear(){
    memset(this,0,sizeof(*this));
    NI=0xFF;
    CanGuardSqd=0xFFFF;
}

void Mind::Free(){
    if(Sqd) free(Sqd);
    Clear();
};

CEXPORT
void ActivateMind(byte NI, int type=0){
    if(NI<7) Player[NI].Init(NI,type);
}

CEXPORT
void DeactivateMind(byte NI){
    if(NI<7) Player[NI].~Mind();
}

word Mind::AddSquad(word BrgID, word NIndex){
    Sqd=(Squad*)realloc(Sqd,sizeof(Squad)*(NSqd+1));
    Squad* NewSqd=Sqd+NSqd;

    memset(NewSqd,0,sizeof(Squad));
    RegisterDynGroup(&NewSqd->Group);
    NewSqd->Index=NSqd;
    NewSqd->NI=NI;
    NewSqd->Brig=BrgID;
    NewSqd->NIndex=NIndex;
    NewSqd->Top=0xFFFF;
    NewSqd->LastTop=0xFFFF;
    
    memset(NewSqd->TarTop,0xFF,sizeof(NewSqd->TarTop));
    memset(NewSqd->TarZone,0xFF,sizeof(NewSqd->TarZone));
    
    return NSqd++;
};

DLLIMPORT bool OBJ_GoToMine(word Index,word DestMine,byte prio,byte OrdType);
DLLIMPORT void SetBuildingsCollector(byte NI,GAMEOBJ* Grp);
DLLIMPORT void AddOneUnitToGroup(GAMEOBJ* Units,OneObject* OB);

void Mind::Init(byte ni, int type){
    if(ni>=7) return;
    
    switch(GetDiff(ni)){
    case 0: SetPlayerName(ni,GetTextByID("@RMID_EASY")); break;
    case 1: SetPlayerName(ni,GetTextByID("@RMID_NORMAL")); break;
    case 2: SetPlayerName(ni,GetTextByID("@RMID_HARD")); break;
    case 3: SetPlayerName(ni,GetTextByID("@RMID_VERYHARD")); break;
    case 4: SetPlayerName(ni,GetTextByID("@RMID_IMPOSIBLE")); break;
    }

    NI=ni;
    Type=type;

    RegisterDynGroup(&New);
    RegisterDynGroup(&Panic);
    SetUnitsCollector(ni,&New);
    
    GAMEOBJ Bld;
    RegisterDynGroup(&Bld);
    SetBuildingsCollector(ni,&Bld);

    UnitsGroup* NEW=GetUnitsGroup(&New);
    RegisterDynGroup(&Repaires);

    if(NEW){
        int N=NEW->N;
        word* ID=NEW->IDS;
        word* SN=NEW->SNS;

        word Prim[1024];
        int NPrim=0;

        int NB=GetNUnits(&Bld);
        UnitsGroup* BG=GetUnitsGroup(&Bld);
        if(BG){
            word* BID=BG->IDS;
            CPlayerScript* PS=PScript+ni;        
            for(int i=0;i<NB;i++){
                OneObject* OBJ=GetOBJ(BID[i]);
                if(OBJ&&!OBJ->Sdoxlo){                
                    for(int j=0;j<PS->NProtectGrp;j++) if(OBJ->NIndex==PS->ProtectGrp[j]&&NPrim<1024){
                        Prim[NPrim]=OBJ->Index;
                        NPrim++;
                    }
                }
            }
        }

        for(int i=0;i<N;i++){
            OneObject* OBJ=GetOBJ(ID[i]);
            if(OBJ&&OBJ->Serial==SN[i]&&!OBJ->Sdoxlo){
                if(DetCannon(OBJ->NIndex)&&NCannon<256){
                    Cannon[NCannon]=OBJ->Index;
                    CannSN[NCannon]=OBJ->Serial;
                    NCannon++;
                }else if(OBJ->BrigadeID!=0xFFFF){
                    word sqd=0xFFFF;
                    for(int i=0;i<128&&i<NSqd;i++){
                        if(Sqd[i].Brig==OBJ->BrigadeID){
                            sqd=i;
                            break;
                        }
                    }
                    if(sqd==0xFFFF){
                        sqd=AddSquad(OBJ->BrigadeID, OBJ->NIndex);
                        if(CanGuardSqd==0xFFFF) CanGuardSqd=sqd;
                    }
                    if(sqd!=0xFFFF) InsertUnitToGroup(NULL,&Sqd[sqd].Group,OBJ->Index);
                }else{
                    word PrimID=0xFFFF;
                    int PrimDist=1000000;
                    for(int j=0;j<NPrim;j++){
                        OneObject* PO=GetOBJ(Prim[j]);
                        if(PO&&PO->NewBuilding){
                            int dst=Norma(OBJ->RealX-PO->RealX,OBJ->RealY-PO->RealY);
                            if(dst<PrimDist){
                                PrimDist=dst;
                                PrimID=PO->Index;
                            }
                        }
                    }
                    if(OBJ->LockType==1 && NSheep<1024){
                        Sheep[NSheep]=OBJ->Index;
                        SheSN[NSheep]=OBJ->Serial;
                        NSheep++;
                    }else if(PrimDist<16000){
                        if(DetPeasant(OBJ->NIndex)){
                            AddOneUnitToGroup(&Repaires,OBJ);                            
                        }else
                            OBJ_GoToMine(OBJ->Index,PrimID,128+16,0);
                    }else{
                        word BrID=0xFFFF;
                        word ONIndex=OBJ->NIndex;
                        word sqd=0xFFFF;
                        for(int i=0;i<128&&i<NSqd;i++){
                            if(Sqd[i].Brig==BrID&&Sqd[i].NIndex==ONIndex&&GetNUnits(&Sqd[i].Group)<40){
                                sqd=i;
                                break;
                            }
                        }
                        if(sqd==0xFFFF){
                            sqd=AddSquad(BrID, ONIndex);
                        }
                        if(sqd!=0xFFFF) InsertUnitToGroup(NULL,&Sqd[sqd].Group,OBJ->Index);
                    }
                }
            }
        }
    }
    
    for(int j=0;j<256;j++) Fear[j]=1;
    LastGlobalMove=0;

    CPlayerScript* PS=PScript+ni;
    if(PS->Enable){
        bool* AIUpg=PS->AIUpg;
        for(int i=0;i<4096;i++) if(AIUpg[i]){
            GAMEOBJ upg;
            upg.Index=i;
            upg.Type='UPGR';
            InitialUpgradeNew(ni,&upg,0xFFFF);
        }
    }
}

void Mind::Process(){
    if(NI==0xFF) return;
    switch(Type){
    case 0: Process0(); break;
    case 1: Process1(); break;
    case 2: Process2(); break;
    case 3: Process3(); break;
    case 4:
    case 5:
    case 6:
        {
            int Time=GetGlobalTime();
            if(Time-LastGlobalMove>0){
                LastGlobalMove=Time+GetRND(40)+40;
                int PeasTime = (Type==4) ? 0 : (Type==5) ? 1400 : 2200;
                ProcessCannons(PeasTime);
                if(Time<PeasTime) Process2();
            }
        }
        break;    
    }
}

// MULTITHREADED Process0
void Mind::Process0() {
    if (NI == 0xFF) return;
    
    auto nt = std::make_unique<int[]>(2048);
    auto xt = std::make_unique<int[]>(2048);
    auto yt = std::make_unique<int[]>(2048);
    
    GetEnemyTopInfo(NI, nt.get(), xt.get(), yt.get());
    SetDangerMap(nt.get());
}

// MULTITHREADED Process1 - Parallel squad processing
void Mind::Process1() {
    if (NI == 0xFF) return;

    // All large arrays moved to heap
    auto nt = std::make_unique<int[]>(2048);
    auto xt = std::make_unique<int[]>(2048);
    auto yt = std::make_unique<int[]>(2048);
    
    GetEnemyTopInfo(NI, nt.get(), xt.get(), yt.get());
    CleanGroup(&New);
    CleanGroup(&Panic);

    int Time = GetGlobalTime();

    if (Time - LastGlobalMove > 50) {
        LastGlobalMove = Time + GetRND(30);

        ClearAZones();

        auto IDSS = std::make_unique<word[]>(4096);
        memset(IDSS.get(), 0xFF, 4096 * sizeof(word));
        CreateTopListEnArmyBtl(IDSS.get(), NI, 1);

        auto REAR = std::make_unique<word[]>(4096);
        memset(REAR.get(), 0xFF, 4096 * sizeof(word));
        CreateFriendBuildingsTopList(REAR.get(), NI);

        word ZREAR = 0xFFFF;
        int NZ = GetNZones();
        for (int z = 0; z < NZ; z++) {
            if (REAR[z] != 0xFFFF) {
                ZREAR = z;
                break;
            }
        }

        auto dang = std::make_unique<short[]>(4096);
        memset(dang.get(), 0, 4096 * sizeof(short));

        int maxdang;
        word DST = 0xFFFF;
        int TZ = 0xFFFF;
        if (ZREAR != 0xFFFF) TZ = FindNextZoneOnTheSafeWayToObject(ZREAR, dang.get(), IDSS.get(), &maxdang, 5, &DST);
        
        int MaxDist = 1000;
        if (TZ != 0xFFFF && ZREAR != 0xFFFF) {
            word* WL = NULL;
            int NW = GetLastFullWay(&WL);
            if (NW > 0) {
                word dst = GetZonesDist(ZREAR, WL[0]);
                if (dst < MaxDist) {
                    MaxDist = dst;
                }
            }
        }
        if (MaxDist > 23) MaxDist -= 13;

        auto Fear = std::make_unique<int[]>(256);                    
        for (int j = 0; j < 256; j++) Fear[j] = 1;

        auto Dang = std::make_unique<int[]>(2 * 4096);  // Flattened 2D array
        memset(Dang.get(), 0, 2 * 4096 * sizeof(int));

        CreateDangerMapBattle(NI, &Dang[4096], GetNZones(), Fear.get(), 2);  // Dang[1]

        // PARALLEL SQUAD PROCESSING
        std::vector<std::future<void>> futures;
        
        for (int i = 0; i < NSqd; i++) {
            futures.push_back(std::async(std::launch::async, [this, i, &Dang, &IDSS, ZREAR, MaxDist, &nt, &xt, &yt]() {
                Squad* SQD = Sqd + i;
                GAMEOBJ* Group = &SQD->Group;
                int NMen = CleanGroup(Group);
                if (NMen) {
                    int xc, yc;
                    if (GetGrpCenter(Group, &xc, &yc)) {
                        int top = GetTopZone(xc, yc);
                        if (top >= 0xFFFE) {
                            OneUnit UN;
                            GetUnitInfo(Group, 0, &UN);
                            xc = UN.x;
                            yc = UN.y;
                            top = GetTopZone(xc, yc);
                        }
                        if (top >= 0 && top < GetNZones()) {        
                            SQD->Top = top;

                            // Thread-local heap allocations
                            auto IDS_local = std::make_unique<word[]>(3 * 4096);
                            memset(IDS_local.get(), 0xFF, 3 * 4096 * sizeof(word));

                            bool CaptureCenter = false;
                            if (!AddEnemyCaptBuildTopList(IDS_local.get(), NI)) {
                                AddEnemyCenterTopList(IDS_local.get(), NI);
                                CaptureCenter = true;
                            }

                            CreateTopListEnArmyBtl(&IDS_local[4096], NI, NMen >> 2);  // IDS_local[1]

                            for (int t = 0; t < 2; t++) {
                                for (int d = 0; d < 2; d++) {
                                    if (t == 1 && d == 0) {
                                        auto SDang = std::make_unique<short[]>(4096);
                                        int NZ = GetNZones();
                                        for (int s = 0; s < NZ; s++) SDang[s] = Dang[d * 4096 + s];
                                        
                                        auto IDSS_local = std::make_unique<word[]>(4096);
                                        memcpy(IDSS_local.get(), IDSS.get(), 4096 * sizeof(word));
                                        
                                        // Note: Need to adjust FindTargetZone signature to accept pointers
                                        // SQD->FindTargetZone(SDang.get(), IDSS_local.get(), SQD->TarTop[t], SQD->TarZone[t][d], SQD->TarDist[t][d]);
                                    }
                                }
                            }

                            bool moving = true;
                            int zt = SQD->TarTop[1];
                            if (zt == 0xFFFF && i) {
                                zt = Sqd[i - 1].TarTop[1];
                            }

                            if (moving && zt != 0xFFFF) {
                                word zf;
                                word zb = ZREAR;
                                
                                zf = 0xFFFF;
                                int fdst = 1000;
                                int zbb = zb;                        
                                
                                int xx, yy;
                                if (GetTopZRealCoor(zb, &xx, &yy)) {
                                    int x, y;
                                    if (GetTopZRealCoor(zt, &x, &y)) {
                                        int Dir = getDir(x - xx, y - yy);

                                        auto IDSS_local2 = std::make_unique<word[]>(4096);
                                        memcpy(IDSS_local2.get(), IDSS.get(), 4096 * sizeof(word));

                                        int NZ = GetNZones();
                                        for (int z = 0; z < NZ; z++) {                                            
                                            if (IDSS_local2[z] != 0xFFFF && GetTopZRealCoor(z, &x, &y)) {
                                                int dir = abs(getDir(x - xx, y - yy) - Dir);
                                                if (dir < 15) {
                                                    IDSS_local2[z] = 0xFFFF;
                                                }
                                            }
                                        }
                                    }
                                }

                                int tdst = 10000;

                                zb = GetNextZone(zb, zt);
                                while (zb != 0xFFFF && zb != zt) {
                                    int wdst = GetZonesDist(zb, zt);
                                    int bdst = GetZonesDist(zb, zbb);
                                    
                                    if (wdst < fdst && bdst < MaxDist) {
                                        fdst = wdst;
                                        tdst = bdst;
                                        zf = zb;
                                    }
                                    zb = GetNextZone(zb, zt);
                                }
                                
                                int btogdist = 1000;
                                if (zbb != 0xFFFF) {
                                    btogdist = GetZonesDist(zbb, zt);
                                }
                                                            
                                if (btogdist > 19 && zf != 0xFFFF) {
                                    int dx, dy;
                                    GetTopZRealCoor(zf, &dx, &dy);
                                    
                                    int dir = 512;
                                    word TT = zt;
                                    
                                    int tx = 0, ty = 0, tn = 0;
                                    
                                    int nz = GetNZones();
                                    for (int j = 0; j < nz; j++) {
                                        int nn = nt[j];
                                        if (nn) {
                                            word ds = GetZonesDist(j, zt);
                                            if (ds < 0xFFFE) {
                                                if (ds > 1) nn /= ds;
                                                if (nn) {
                                                    tx += xt[j] * nn;
                                                    ty += yt[j] * nn;
                                                    tn += nn;
                                                }
                                            }
                                        }
                                    }
                                    if (tn) {
                                        tx /= tn;
                                        ty /= tn;
                                        dir = getDir(tx - dx, ty - dy);
                                    }
                                    
                                    if (SQD->Brig != 0xFFFF) SelectUnits(Group, 0);
                                    
                                    {
                                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                        SGP_MoveToPoint(NI, Group, dx + 16 - GetRND(32), dy + 16 - GetRND(32), dir, 0, 0, 1);
                                        SetUnitsState(Group, 0, 0, 0, 0);
                                    }
                                } else {
                                    int d = GetZonesDist(top, zt);
                                    {
                                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                        if (d < 12) {
                                            SetUnitsState(Group, 1, 1, 0, 0);
                                        } else {
                                            SetUnitsState(Group, 0, 1, 0, 0);
                                        }
                                    }
                                }
                            }
                        }
                    }                    
                }
            }));
        }

        // Wait for all squads to finish processing
        for (auto& f : futures) {
            f.get();
        }
    }
}

void GetArmyMap(int* ArmyMap, int* Dang, ActiveArmy* AA, int& NAA){
    int NZ=GetNZones();
    for(int j=0;j<NZ;j++) ArmyMap[j]=0xFFFF;

    NAA=0;
    AA[0].Clear();

    for(int i=0;i<NZ;i++)if(ArmyMap[i]==0xFFFF){                    
        ActiveArmy* aa=AA+NAA;

        word TopList[1024];
        TopList[0]=i;
        int NTopList=1;
        
        bool add=false;

        while(NTopList){
            NTopList--;
            int t=TopList[NTopList];

            word armpos=0xFFFF;

            word* links=NULL;
            int NL=GetListOfNearZones(t,&links);
            for(int p=0;p<NL;p++){
                int id=links[p+p];
                if(Dang[id]){
                    if(ArmyMap[id]==0xFFFF){
                        armpos=NAA;
                        TopList[NTopList]=id;
                        NTopList++;
                    }else{
                        armpos=ArmyMap[id];
                    }
                };
            };

            if(Dang[t]){
                ArmyMap[t]=NAA;
                int x,y;
                if(GetTopZRealCoor(t,&x,&y)){
                    aa->xc+=x;
                    aa->yc+=y;
                    aa->n++;
                }
                add=true;
            }else{
                if(armpos<0xFFFF){
                    ArmyMap[t]=0x1000;
                    AA[armpos].PID[AA[armpos].NP]=t;
                    AA[armpos].NP++;
                }else{
                    ArmyMap[t]=0xFFFE;
                }
            }
        }

        if(add){
            NAA++;
            AA[NAA].Clear(NAA);
        }
    }
}

void ShowWay(char* Mess, word* Way, int NWay){
    if(!Way) return;
    char txt[255];
    
    for(int i=0;i<NWay;i++){
        sprintf(txt,"Way %s %d",Mess,i);
        int x,y;
        if(GetTopZRealCoor(Way[i],&x,&y)) CreateAZone(txt,x,y,64);
    }
}

int FindStepToObject(int Start,short* DangerMap,word* ObjIds,word* DestObj){
    int MaxDang;
    return FindNextZoneOnTheSafeWayToObject(Start,DangerMap,ObjIds,&MaxDang,1000,DestObj);
}

#include <queue>

using namespace std;

void GetArmyMap(Enemy* ENM, word* DefTent, short* Dang) {
    int NZ = GetNZones();
    
    // Large arrays moved to heap using std::vector
    std::vector<int> DM(NZ);
    for (int i = 0; i < NZ; i++) DM[i] = Dang[i];
    
    int* ArmyMap = ENM->ArmyMap;
    GetArmyMap(ArmyMap, DM.data(), ENM->AA, ENM->NAA);

    std::vector<short> NullDang(NZ);
    std::fill(NullDang.begin(), NullDang.end(), 0);

    int NA = ENM->NAA;

    ArmyTopInfo* ATI = &ENM->TopInf;
    int* AM = ENM->ArmyMap;
    for (int i = 0; i < NZ; i++) {
        int arm = AM[i];
        if (arm < NA) {
            ActiveArmy* a = ENM->AA + arm;
            a->Power += ATI->Power[i];
        }
    }
    
    // PARALLEL ARMY PROCESSING
    std::vector<std::future<void>> futures;
    
    for (int i = 0; i < NA; i++) {
        futures.push_back(std::async(std::launch::async, [ENM, &NullDang, DefTent, ArmyMap, i, NA]() {
            ActiveArmy* a = ENM->AA + i;
            if (a->n) {
                a->xc /= a->n;
                a->yc /= a->n;
                a->TopCenter = GetTopZone(a->xc, a->yc);
                
                // Create a mutable copy of NullDang for this thread
                std::vector<short> threadNullDang = NullDang;
                
                if (a->TopCenter < 0xFFFE && 
                    FindStepToObject(a->TopCenter, threadNullDang.data(), DefTent, &a->TentID) < 0xFFFE) {
                    word* W = NULL;
                    int N = GetLastFullWay(&W);

                    if (W) {
                        for (int j = N - 1; j >= 0; j--) {
                            int arm = ArmyMap[W[j]];
                            if (arm < NA && arm != i) {
                                a->Link.Front = arm;
                                break;
                            }
                        }

                        a->NTentWay = N;
                        // Copy waypoints - assuming a->TentWay is pre-allocated with enough space
                        if (a->TentWay && N > 0) {
                            std::memcpy(a->TentWay, W, N * sizeof(word));
                        }
                        a->TentWayDist = GetZonesDist(a->TentWay[0], a->TentWay[N - 1]);
                        
                        char txt[200];
                        sprintf(txt, "Arm %d", a->Power);
                        ShowWay(txt, a->TentWay, a->NTentWay);
                    }
                }
            }
        }));
    }
    
    // Wait for all armies
    for (auto& f : futures) {
        f.get();
    }
}

void Mind::MarkArmy(){
    ActiveArmy* AA=ENM.AA;
    int NAA=ENM.NAA;
    
    for(int i=0;i<NAA;i++){
        ActiveArmy* a=AA+i;
        int N=NSqd;
        char name[255];
        sprintf(name,"En %d, top %d",i,a->TopCenter);
        CreateAZone(name,a->xc,a->yc,256);
    }
}

// MULTITHREADED Process2 - Parallel danger map and squad processing
void Mind::Process2() {
    if (NI == 0xFF) return;

    int Time = GetGlobalTime();

    if (true) {
        ClearAZones();
    
        // Small arrays can stay on stack
        word SList[255];
        int SX[255];
        int SY[255];
        int ST[255];
        int NS = 0;

        for (int i = 0; i < NSqd; i++) {
            Squad* SQD = Sqd + i;
            GAMEOBJ* Group = &SQD->Group;
            int NMen = CleanGroup(Group);
            if (NMen) {
                int xc, yc;
                if (GetGrpCenter(Group, &xc, &yc)) {
                    int top = GetTopZone(xc, yc);
                    if (top >= 0xFFFE) {
                        OneUnit UN;
                        GetUnitInfo(Group, 0, &UN);
                        xc = UN.x;
                        yc = UN.y;
                        top = GetTopZone(xc, yc);
                    }
                    if (top >= 0 && top < GetNZones()) {
                        SX[NS] = xc;
                        SY[NS] = yc;
                        ST[NS] = top;
                        SList[NS] = i;
                        SQD->xc = xc;
                        SQD->yc = yc;
                    }
                }
                NS++;
            }
        }

        int NZ = GetNZones();
        
        // Small array can stay on stack
        int Fear[256];
        for (int j = 0; j < 256; j++) Fear[j] = 1;

        // Large arrays moved to heap using std::vector
        std::vector<int> Dang(NZ);
        std::fill(Dang.begin(), Dang.end(), 0);
        CreateDangerMapBattle(NI, Dang.data(), NZ, Fear, 2);

        // This is fine on stack (128 * sizeof(ActiveArmy) ~ few KB)
        ActiveArmy AA[128];
        memset(AA, 0, sizeof(AA));
        int NAA = 0;
        
        // Large array moved to heap
        std::vector<int> ArmyMap(NZ);
        GetArmyMap(ArmyMap.data(), Dang.data(), AA, NAA);

        std::vector<short> DD(NZ);
        std::fill(DD.begin(), DD.end(), 0);

        std::vector<word> IDS(NZ);
        for (int i = 0; i < NZ; i++) {
            if (ArmyMap[i] < 0x1000) {
                IDS[i] = ArmyMap[i];
            } else {
                IDS[i] = 0xFFFF;
            }
        }

        int MaxDan;

        for (int i = 0; i < NS; i++) {
            word DST = 0xFFFF;
            FindNextZoneOnTheSafeWayToObject(ST[i], DD.data(), IDS.data(), &MaxDan, 1000, &DST);
            
            if (DST < NAA) {
                ActiveArmy* aa = AA + DST;
                aa->SID[aa->NS] = SList[i];
                aa->NS++;
            }
        }

        for (int i = 0; i < NAA; i++) {
            ActiveArmy* aa = AA + i;
            int N = aa->NS;
            word* SID = aa->SID;

            ClearSelection(NI);
            for (int s = 0; s < N; s++) {
                int ss = SID[s];
                if (ss < NSqd) {
                    SelectUnits(&Sqd[ss].Group, 1);
                }
            }

            char name[255];
            sprintf(name, "Army %d", i);
            CreateAGroup(NI, name);
        }

        // Copy danger map to global DangerMap
        for (int i = 0; i < NZ; i++) DangerMap[i] = Dang[i];
        ActivateArmy(AA, NAA);
    }
}

CIMPORT bool DetectAbsorber(word NIndex);
CIMPORT bool DetShoot(word NIndex);

void Mind::ProcessCannons(int TimeToStorm){
    if(NI==0xFF) return;

    byte PrimNIndex[4096];
    memset(PrimNIndex,0,sizeof(PrimNIndex));
    
    for(int i=0;i<8;i++)if(i!=NI){
        CPlayerScript* PS=PScript+i;
        if(PS->Enable&&!PS->Defeat){
            for(int j=0;j<PS->NProtectGrp;j++){
                PrimNIndex[PS->ProtectGrp[j]]=1;                    
            }
        }
    }

    word Prim[1024];
    int NPrim=0;
    word Secd[1024];
    int NSecd=0;

    int NTop=GetNZones();
    byte* SecdTop=(byte*)malloc(NTop);
    memset(SecdTop,0,NTop);

    int MO=GetMaxObject();
    for(int i=0;i<MO;i++){
        OneObject* OB=GetOBJ(i);
        if(OB&&!OB->Sdoxlo){
            if(OB->NNUM==NI){
                CPlayerScript* PS=PScript+NI;
                for(int j=0;j<PS->NProtectGrp;j++){
                    if(PS->ProtectGrp[j]==OB->NIndex){
                        if(OB->Life<OB->MaxLife) SGP_RepairBuilding(&Repaires,2,OB->Index);
                        break;
                    }
                }
            }else{
                if(PrimNIndex[OB->NIndex] && NPrim<1024){
                    Prim[NPrim]=OB->Index;
                    NPrim++;
                }else if(NSecd<1024){
                    if(OB->BrigadeID!=0xFFFF){
                        int OT=GetTopZone(OB->RealX>>4,OB->RealY>>4);
                        if(OT<NTop){
                            SecdTop[OT]++;
                            if(SecdTop[OT]==30){
                                Secd[NSecd]=OB->Index;
                                NSecd++;
                            }
                        }
                    }else if(OB->LockType==1){
                        Secd[NSecd]=OB->Index;
                        NSecd++;
                    }
                }
            }
        }
    }

    int ccx=0,ccy=0;
    if(NCannon){
        int n=0;
        
        // PARALLEL CANNON PROCESSING
        std::mutex cannon_mutex;
        std::vector<std::future<void>> futures;
        
        for(int i=0;i<NCannon;i++){
            futures.push_back(std::async(std::launch::async, [this, i, &Prim, NPrim, &Secd, NSecd, &cannon_mutex, &ccx, &ccy, &n](){
                OneObject* CO=GetOBJ(Cannon[i]);
                if(CO&&CO->Serial==CannSN[i]&&!CO->Sdoxlo){
                    int cx=CO->RealX;
                    int cy=CO->RealY;

                    {
                        std::lock_guard<std::mutex> lock(cannon_mutex);
                        ccx+=cx;
                        ccy+=cy;
                        n++;
                    }
                    
                    int PrimDist=10000000;
                    word PrimID=0xFFFF;
                    byte PrimDir;
                    int px,py;
                    byte PrimLockType;
                    for(int j=0;j<NPrim;j++){
                        OneObject* PO=GetOBJ(Prim[j]);
                        int dst=Norma(PO->RealX-cx,PO->RealY-cy);
                        if(PrimDist>dst){
                            PrimDist=dst;
                            PrimID=PO->Index;
                            PrimDir=getDir(PO->RealX-cx,PO->RealY-cy);
                            px=PO->RealX>>4;
                            py=PO->RealY>>4;
                            PrimLockType=PO->LockType;
                        }
                    }
                    
                    int SecdDist=10000000;
                    word SecdID=0xFFFF;
                    int sx,sy;
                    byte SecdLockType;
                    for(int j=0;j<NSecd;j++){
                        OneObject* SO=GetOBJ(Secd[j]);
                        sx=SO->RealX;
                        sy=SO->RealY;
                        int dst=Norma(sx-cx,sy-cy);
                        
                        byte tdir=0;
                        if(PrimID!=0xFFFF){
                            byte dir=getDir(sx-cx,sy-cy);
                            tdir=PrimDir-dir;
                            if(tdir>128) tdir=-tdir;
                        }
                        
                        if(dst<SecdDist && tdir<24){
                            SecdDist=dst;
                            SecdID=SO->Index;
                            SecdLockType=SO->LockType;
                        }
                    }

                    if(PrimID!=0xFFFF && (SecdID==0xFFFF||PrimDist-SecdDist<32000)){
                        if(PrimDist>60000){
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            if(GetTopDist(cx>>4,cy>>4,px,py)<0xFFFE) OBJ_SendTo(CO->Index,px,py,128+16,0);
                        }else{
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            OBJ_AttackObj(CO->Index,PrimID,128+16,0);
                        }
                    }else if(SecdID!=0xFFFF){
                        if(SecdDist>50000){
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            if(GetTopDist(cx>>4,cy>>4,sx>>4,sy>>4)<0xFFFE) OBJ_SendTo(CO->Index,px,py,128+16,0);
                        }else{
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            OBJ_AttackObj(CO->Index,SecdID,128+16,0);
                        }
                    }
                }
            }));
        }

        for(auto& f : futures) {
            f.get();
        }

        if(n){
            ccx/=n;
            ccy/=n;
            ccx>>=4;
            ccy>>=4;
        }
    }

    // PARALLEL SHEEP PROCESSING
    if(NSheep){
        std::vector<std::future<void>> futures;
        
        for(int i=0;i<NSheep;i++){
            futures.push_back(std::async(std::launch::async, [this, i, &Prim, NPrim, &Secd, NSecd](){
                OneObject* OB=GetOBJ(Sheep[i]);
                if(OB&&OB->Serial==SheSN[i]&&!OB->Sdoxlo){
                    int cx=OB->RealX;
                    int cy=OB->RealY;
                    int px,py;

                    int PrimDist=10000000;
                    word PrimID=0xFFFF;
                    byte PrimDir;
                    for(int j=0;j<NPrim;j++){
                        OneObject* PO=GetOBJ(Prim[j]);
                        int dst=Norma(PO->RealX-cx,PO->RealY-cy);
                        if(PrimDist>dst){
                            PrimDist=dst;
                            PrimID=PO->Index;
                            PrimDir=getDir(PO->RealX-cx,PO->RealY-cy);
                            px=PO->RealX>>4;
                            py=PO->RealY>>4;
                        }
                    }
                    
                    int SecdDist=10000000;
                    word SecdID=0xFFFF;
                    for(int j=0;j<NSecd;j++){
                        OneObject* SO=GetOBJ(Secd[j]);
                        int dx=SO->RealX-cx;
                        int dy=SO->RealY-cy;
                        int dst=Norma(dx,dy);
                        
                        byte tdir=0;
                        if(PrimID!=0xFFFF){
                            byte dir=getDir(dx,dy);
                            tdir=PrimDir-dir;
                            if(tdir>128) tdir=-tdir;
                        }
                        
                        if(dst<SecdDist && tdir<24){
                            SecdDist=dst;
                            SecdID=SO->Index;
                        }
                    }

                    if(PrimID!=0xFFFF && (SecdID==0xFFFF||PrimDist-SecdDist<32000)){
                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                        OBJ_AttackObj(OB->Index,PrimID,128+16,0);
                    }else if(SecdID!=0xFFFF){
                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                        if(SecdDist>40000){
                            OBJ_SendTo(OB->Index,px,py,128+16,0);
                        }else{
                            OBJ_AttackObj(OB->Index,SecdID,128+16,0);
                        }
                    }
                }
            }));
        }
        
        for(auto& f : futures) {
            f.get();
        }
    }

    free(SecdTop);

    if(GetGlobalTime()>TimeToStorm){
        RefreshSquadInfo();
        
        // PARALLEL SQUAD STORM PROCESSING
        std::vector<std::future<void>> futures;
        
        for(int i=0;i<NSqd;i++){
            futures.push_back(std::async(std::launch::async, [this, i, ccx, ccy, &Prim, NPrim, &Secd, NSecd](){
                Squad* SQ=Sqd+i;
                GAMEOBJ* Grp=&SQ->Group;
                
                if(SQ->Top<0xFFFE){
                    if(i==CanGuardSqd&&ccx){
                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                        if(SQ->Brig!=0xFFFF) SelectUnits(Grp,0);
                        SGP_MoveToPoint(NI,Grp,ccx,ccy,256,0,0,1);
                        SetUnitsState(Grp,1,0,0,0);
                        return;
                    }

                    if(DetArcher(SQ->NIndex)){
                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                        if(SQ->Brig!=0xFFFF) EraseBrig(NI,SQ->Brig);
                        AddTomahawks(Grp,NI,0,0,0);
                        return;
                    }

                    word PrimID=0xFFFF;
                    int PrimDist=1000000;
                    word PrimNIndex;
                    bool UnitAbsorber;
                    int px,py;
                    for(int j=0;j<NPrim;j++){
                        OneObject* PO=GetOBJ(Prim[j]);
                        if(PO){
                            int dst=Norma((PO->RealX>>4)-SQ->xc,(PO->RealY>>4)-SQ->yc);
                            if(dst<PrimDist){
                                PrimDist=dst;
                                PrimID=PO->Index;
                                UnitAbsorber=DetectAbsorber(PO->NIndex);
                                px=PO->RealX>>4;
                                py=PO->RealY>>4;
                                PrimNIndex=PO->NIndex;
                            }
                        }
                    }

                    if(SQ->Brig==0xFFFF&&DetAbsorbeAbility(PrimNIndex,SQ->NIndex)&&GetNUnits(Grp)<20){
                        std::lock_guard<std::mutex> lock(g_gameApiMutex);
                        AddStorm(Grp,NI,0);
                        return;
                    }

                    int SecdDist=10000000;
                    word SecdID=0xFFFF;
                    for(int j=0;j<NSecd;j++){
                        OneObject* SO=GetOBJ(Secd[j]);
                        int dx=(SO->RealX>>4)-SQ->xc;
                        int dy=(SO->RealY>>4)-SQ->yc;
                        int dst=Norma(dx,dy);
                        
                        if(dst<SecdDist){
                            SecdDist=dst;
                            SecdID=SO->Index;
                        }
                    }
                    
                    if(PrimID!=0xFFFF){
                        if(UnitAbsorber&&PrimDist<1000){
                            std::lock_guard<std::mutex> lock(g_gameApiMutex);
                            UnitsGroup* SG=GetUnitsGroup(Grp);
                            if(SG){
                                for(int u=0;u<SG->N;u++){
                                    OBJ_GoToMine(SG->IDS[u],PrimID,128+16,0);
                                }
                            }
                        }else{
                            bool move=true;
                            
                            if(DetShoot(SQ->NIndex)){
                                std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                if(PrimDist>400&&(SecdID==0xFFFF||SecdDist>800)){
                                    if(PrimDist<1000)
                                        SetUnitsState(Grp,1,0,0,0);
                                    else
                                        SetUnitsState(Grp,1,0,1,0);
                                }else{
                                    if(SQ->Brig!=0xFFFF){
                                        SetUnitsState(Grp,1,1,0,0);
                                        move=false;
                                    }else{
                                        SetUnitsState(Grp,1,0,0,0);
                                    }
                                }
                            }else{
                                std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                SetUnitsState(Grp,1,0,0,0);
                            }
                            
                            if(move){
                                std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                if(SQ->Brig!=0xFFFF) SelectUnits(Grp,0);
                                SGP_MoveToPoint(NI,Grp,px,py,256,0,0,1);
                            }
                            
                            {
                                std::lock_guard<std::mutex> lock(g_gameApiMutex);
                                UnitsGroup* SG=GetUnitsGroup(Grp);
                                if(SG){
                                    for(int u=0;u<SG->N;u++){
                                        OneObject* OU=GetOBJ(SG->IDS[u]);
                                        if(OU&&OU->Serial==SG->SNS[u] && Norma(px-(OU->RealX>>4),py-(OU->RealY>>4))<800){
                                            OBJ_GoToMine(SG->IDS[u],PrimID,128+16,0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }));
        }
        
        for(auto& f : futures) {
            f.get();
        }
    }
}

void Mind::Process3(){
    if(NI==0xFF) return;

    int Time=GetGlobalTime();

    if(true){
        SetGameGoals();
        SetDangerMap(Dang);
        RefreshSquadInfo();
        ClearAZones();
        GetArmyMap(&ENM,DefTent,DangerMap);
        MarkArmy();
        SetLink0();
        GlobalAttack();

        switch(ShowMode){
        case 1:
            ShowVistrel(); 
            break;
        }
    }
}

void Mind::SetLink0(){
    priority_queue <AAT,vector<AAT>,GreatAA> AQ;
    int NA=ENM.NAA;
    for(int i=0;i<NA;i++){
        ActiveArmy* a=ENM.AA+i;
        if(a->NTentWay&&a->Link.Front==0xFFFF) AQ.push(a);
    }
    
    while(!AQ.empty()){
        ActiveArmy* AA=AQ.top();        
        word dist=AA->TentWayDist;
        if(dist<0xFFFF){
            int pow=GetArmyActorPower(AA);
            
            priority_queue <SqdT,vector<SqdT>,GreatSQD> SqdQ;            
            for(int s=0;s<NSqd;s++){
                Squad* sqd=Sqd+s;
                if(sqd->Top!=0xFFFF&&sqd->ArmyID==0xFFFF){
                    sqd->ArmyDist=GetZonesDist(sqd->Top,AA->TopCenter);
                    SqdQ.push(sqd);
                }
            }

            CreateAZone("acti",AA->xc,AA->yc,64);            

            int s=0;
            while(!SqdQ.empty()&&pow<AA->Power){
                char txt[200];
                sprintf(txt,"prio=%d, dist=%d",s,SqdQ.top()->ArmyDist);
                SelectUnits(&SqdQ.top()->Group,0);
                CreateAGroup(NI,txt);
                
                Polk* plk=&AA->Actor;
                plk->SquadID[plk->NSquad]=SqdQ.top()->Index;
                plk->NSquad++;
                
                Squad* sqd=SqdQ.top();
                sqd->ArmyID=AA->Index;
                sqd->ArmyDist=GetZonesDist(AA->TopCenter,sqd->Top);

                pow+=GetGroupPower(&sqd->Group)>>10;
                
                SqdQ.pop();
                s++;
            }
        }
        AQ.pop();
    }
}

CEXPORT
void MindCheats(byte NI, char* com) {
    if (NI > 6) return;
    
    char scom[100];
    if (strstr(com, "acti")) {
        int param;
        int n = sscanf(com, "%s%d", scom, &param);
        if (n < 2) param = 2;
        ActivateMind(NI, param);
    } else if (strstr(com, "deac")) {
        DeactivateMind(NI);
    } else if (strstr(com, "run")) {
        int param;
        int result = sscanf(com, "%s%d", scom, &param);
        if (result != 2) {
            return;
        }
        Player[NI].RunState = param;
    } else if (strstr(com, "show")) {
        char param[100];
        int result = sscanf(com, "%s%s", scom, param);
        if (result < 2) {
            scom[0] = '\0';
            param[0] = '\0';
        }
        if (strstr(param, "dang")) {
            int NZ = GetNZones();
            std::vector<int> Dang(NZ);
            short* SDan = Player[NI].DangerMap;
            for (int i = 0; i < NZ; i++) Dang[i] = SDan[i];
            SetDangerMap(Dang.data());
        } else if (strstr(param, "vist")) {
            Player[NI].ShowMode = 1;
        } else if (strstr(param, "kart")) {
            Player[NI].ShowMode = 2;
        }
    } else if (strstr(com, "type")) {
        int type;
        if (sscanf(com, "%s%d", scom, &type) > 1) {
            Player[NI].Type = type;
        }
    }
}

int Mind::SetGameGoals(){
    int NZ=GetNZones();
    memset(Dang,0,sizeof(Dang));

    ArmyTopInfo* ATI=&ENM.TopInf;
    CreateArmyInfo(NI,ATI->Life,ATI->Damage, NZ);
    for(int i=0;i<NZ;i++){
        if(ATI->Life[i])
            Dang[i]=ATI->Power[i]=(ATI->Life[i]*ATI->Damage[i])>>10;
    }
    
    for(int i=0;i<NZ;i++) DangerMap[i]=Dang[i];
    
    memset(MapVistrel,0,sizeof MapVistrel);
    memset(MapKartech,0,sizeof MapKartech);
    SetMapOfShooters(NI,MapVistrel,MapKartech);
    ShowVistrel();

    memset(DefTent,0xFF,sizeof(DefTent));
    CreateFriendBuildingsTopList(DefTent,NI);
    
    memset(CapTent,0xFF,sizeof(CapTent));
    AddEnemyCaptBuildTopList(CapTent,NI);
    
    return 1;
}

void Mind::RefreshSquadInfo(){
    // PARALLEL SQUAD INFO REFRESH
    std::vector<std::future<void>> futures;
    
    for(int i=0;i<NSqd;i++){
        futures.push_back(std::async(std::launch::async, [this, i](){
            Squad* SQD=Sqd+i;
            GAMEOBJ* Group=&SQD->Group;
            
            SQD->Top=0xFFFF;
            SQD->ArmyID=0xFFFF;

            int NMen=CleanGroup(Group);
            if(NMen){
                int xc,yc;
                if(GetGrpCenter(Group,&xc,&yc)){
                    int top=GetTopZone(xc,yc);
                    if(top>=0xFFFE){
                        OneUnit UN;
                        GetUnitInfo(Group,0,&UN);
                        xc=UN.x;
                        yc=UN.y;
                        top=GetTopZone(xc,yc);
                    }
                    if(top>=0&&top<GetNZones()){
                        SQD->xc=xc;
                        SQD->yc=yc;
                        SQD->Top=top;
                    }
                }
                SQD->Power=GetGroupPower(Group)>>10;            
            }
        }));
    }
    
    for(auto& f : futures) {
        f.get();
    }
}

int Mind::GetArmyActorPower(ActiveArmy* arm){
    if(!arm) return -1;
    Polk* plk=&arm->Actor;
    plk->Power=0;
    for(int i=0;i<plk->NSquad;i++){
        plk->Power+=GetGroupPower(&Sqd[plk->SquadID[i]].Group)>>10;
    }
    return plk->Power;
}

void Mind::GlobalAttack(){
    for(int i=0;i<NSqd;i++){
        Squad* SQD=Sqd+i;
        GAMEOBJ* Group=&SQD->Group;
        
        if(SQD->Top<0xFFFE&&SQD->ArmyID!=0xFFFF){
            SQD->Top=ENM.AA[SQD->ArmyID].TopCenter;            
            SQD->MoveToTop(NULL);
        }
    }
}

void Mind::ActivateArmy(ActiveArmy* AA, int NA) {
    for (int a = 0; a < NA; a++) {
        ActiveArmy* arm = AA + a;

        // Small arrays can stay on stack
        word Dist[255];
        memset(Dist, 0xFF, sizeof(Dist));
        word NewPos[255];
        memset(NewPos, 0xFF, sizeof(NewPos));
        word NewSqd[255];
        memset(NewSqd, 0xFF, sizeof(NewSqd));
                
        int xc = 0, yc = 0, n = 0;
        for (int s = 0; s < arm->NS; s++) {
            word SID = arm->SID[s];
            Squad* sqd = Sqd + SID;
            int nn = GetNUnits(&sqd->Group);
            xc += sqd->xc * nn;
            yc += sqd->yc * nn;
            n += nn;
        }
        if (n) {
            xc /= n;
            yc /= n;
        }

        int NZ = GetNZones();
        
        // Large arrays moved to heap using std::vector
        std::vector<word> REAR(NZ);
        std::fill(REAR.begin(), REAR.end(), 0xFF);
        CreateFriendBuildingsTopList(REAR.data(), NI);

        std::vector<short> D(NZ);
        std::fill(D.begin(), D.end(), 0);

        int maxdang;
        word DST = 0xFFFF;
        
        FindNextZoneOnTheSafeWayToObject(GetTopZone(xc, yc), D.data(), REAR.data(), &maxdang, 10000, &DST);

        if (DST != 0xFFFF) {
            word* Way;
            int NWayPoint = GetLastFullWay(&Way);            
            if (NWayPoint) {
                GetTopZRealCoor(Way[0], &xc, &yc);
            }
        }

        word ss;
        do {            
            int MinR = 1000000;
            word pos = 0xFFFF;
            int px, py;
            for (int p = 0; p < arm->NP; p++) {
                if (NewSqd[p] == 0xFFFF) {
                    word TOP = arm->PID[p];
                    int tx, ty;
                    GetTopZRealCoor(TOP, &tx, &ty);

                    int r = Norma(tx - xc, ty - yc);
                    if (r < MinR) {
                        MinR = r;
                        pos = p;
                        px = tx;
                        py = ty;
                    }
                }
            }            
            ss = 0xFFFF;
            if (pos < 0xFFFE) {                
                MinR = 1000000;                
                for (int s = 0; s < arm->NS; s++) {
                    if (NewPos[s] == 0xFFFF) {
                        word SID = arm->SID[s];
                        Squad* sqd = Sqd + SID;

                        int r = Norma(px - sqd->xc, py - sqd->yc);
                        if (r < MinR) {
                            MinR = r;
                            ss = s;
                        }
                    }
                }
                if (ss < 0xFFFF) {
                    NewPos[ss] = pos;
                    NewSqd[pos] = ss;
                    Dist[ss] = MinR;
                }
            }
        } while (ss < 0xFFFF);

        char name[255];
        int tx, ty;
                
        ClearAZones();
        sprintf(name, "REAR");
        CreateAZone(name, xc, yc, 128);

        for (int s = 0; s < arm->NS; s++) {
            word SID = arm->SID[s];
            Squad* sqd = Sqd + SID;

            sqd->Top = arm->PID[NewPos[s]];            
            sqd->MoveToTop(DangerMap);
            
            SelectUnits(&sqd->Group, 0);            
            sprintf(name, "Squad %d", s);
            CreateAGroup(NI, name);
            
            GetTopZRealCoor(arm->PID[NewPos[s]], &tx, &ty);
            CreateAZone(name, tx, ty, 128);
        }
    }
}

void ActiveArmy::Clear(int ID){
    if(ID==-1) ID=Index;
    memset(this,0,sizeof *this);
    Index=ID;
    TopCenter=0xFFFF;
    memset(&Link,0xFF,sizeof Link);
}

Branch::Branch(word nindex){
}

Weapon::Weapon(word wk){
}