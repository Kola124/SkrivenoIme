class WallCharacter{
public:
    char* Name = {0};
    word RIndex=0;
    word NBuild=0;
    word Ndamage=0;
    short dx=0,dy=0;
	word GateFile=0;
	short GateDx=0;
	short GateDy=0;
	short UpgradeGateIcon=0;
	short OpenGateIcon=0;
	short CloseGateIcon=0;
	short UpgradeTower1=0;
	short UpgradeTower2=0;
    word  GateCost[8] = {0};
	short TexRadius=0;
	short NTex=0;
	short TexPrec=0;
	short* Tex=nullptr;
	short OpenGateSoundID=0;
	short CloseGateSoundID=0;
	byte  OpenKeyFrame=0;
	byte  CloseKeyFrame=0;
};
class WallCluster;
class WallCell{
public:
    short x=0;
    short y=0;
    byte DirMask=0;
    byte Type=0;
    byte NI=0;
    byte Stage=0;
    byte MaxStage=0;
    word Health=0;
    word MaxHealth=0;
    byte Sprite=0;
    byte SprBase=0;
	word ClusterIndex=0;
	word OIndex=0;
	word GateIndex=0;
	int Locks=0;
    bool Visible=0;
    bool CheckPosition();
	int GetLockStatus();
	void SetLockStatus();
    bool StandOnLand(WallCluster* WC);
	void CreateLocking(WallCluster* WC);
	void Landing(WallCluster* WC);
    void ClearLocking();
};
class WallSystem;
class WallCluster{
public:
    byte Type=0;
    int NCornPt=0;
    word* CornPt=nullptr;
    int NCells=0;
    WallCell* Cells = {0};
    WallSystem* WSys={0};
    short LastX=0;
    short LastY=0;
    short FinalX=0;
    short FinalY=0;
    NewMonster* NM = {0};
	word  NIndex=0;
	byte  NI=0;
    bool  WallLMode=0;
//------------------//
    WallCluster();
    ~WallCluster();
    void SetSize(int N);
    void ConnectToPoint(short x,short y);
	void ConnectToPoint(short x,short y,bool Vis);
    void UndoSegment();
    void SetPreviousSegment();
    void KeepSegment();
    void CreateSprites();
    void AddPoint(short x,short y,bool Vis);
    void Preview();
    void MiniPreview();
    void ViewMiniWall();
    void View();
	int  CreateData(word* Data,word Health);
	void CreateByData(word* Data);
	void SendSelectedToWork(byte NI,byte OrdType);
};
class WallSystem{
public:
    int NClusters=0;
    WallCluster** WCL;
//-------------------//
    WallSystem();
    ~WallSystem();
     void AddCluster(WallCluster*);
     void WallSystem::Show();
};
void WallHandleDraw();
void WallHandleMouse();
void SetWallBuildMode(byte NI,word NIndex);
void LoadAllWalls();
inline int GetLI(int x,int y){
    return x+(y<<(VAL_SHFCX+1));
};
extern int MaxLI;
extern int MaxLIX;
extern int MaxLIY;
extern int NChar;
extern WallCharacter WChar[32];
//extern WallCell* WRefs[MAXCX*MAXCY*4];
extern WallCell** WRefs;
extern WallSystem WSys;
void DetermineWallClick(int x,int y);
void SetLife(WallCell* WC,int Health);
//gates
#define NGOpen 9
class Gate{
public:
	short x=0;
	short y=0;
	byte NI=0;
	byte NMask=0;
	byte State=0;
	byte delay=0;
	byte Locked=0;
	byte CharID=0;
};
extern Gate* Gates;
extern int NGates;
extern int MaxGates;
void SetupGates();
void InitGates();
int AddGate(short x,short y,byte NI);
void DelGate(int ID);