
struct MineRec{
	char* Name=nullptr;
	int Ng=0,Ni=0,Nc=0;
};
struct ResRec{
	char* Name=nullptr;
    int RES[8] = {0};
};
struct PlRec{
	int NPlayers=0;
	char* name=nullptr;
};
struct StyleRec{
	char* Name=nullptr;
	char* Style=nullptr;
	int NPl=0;
	int AI_Style=0;
	PlRec* Players=nullptr;
};
class RandomMapDesc{
public:
	int NMINES=0;
	int MINES_DEF=0;
	MineRec* MINES=nullptr;
	int NRES=0;
	int RES_DEF=0;
	ResRec* RES=nullptr;
	int NRelief=0;
	int Relief_DEF=0;
	char** Relief=nullptr;
	int NSTY=0;
	int STY_DEF=0;
	StyleRec* STY=nullptr;
	RandomMapDesc();
	~RandomMapDesc();
	void Close();
	void Load(char* Name);
};
class GlobalProgress{
public:
	int NWeights=0;
    int StageID[64] = {0};
    int StageWeight[64] = { 0 };
    int StagePositions[64] = {0};
	int CurStage=0;
	int CurPosition=0;
	int MaxPosition=0;
	void Setup();
	void AddPosition(int ID,int Weight,int Max);
	void SetCurrentStage(int ID);
	void SetCurrentPosition(int Pos);
	int GetCurProgress();
	GlobalProgress();
};
extern GlobalProgress GPROG;