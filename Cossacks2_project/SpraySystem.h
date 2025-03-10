#ifndef SpraySystem_def
#define SpraySystem_def

#include "SquareObject.h"

// ������� ������ �������� � Cell
#define SquadLx 256
#define SquadLy 256

#define SquadSH 8

// ������� ���� ����� � ���������
#define SprayLx 32
#define SprayLy 32

#define SpraySH 5

#define SprayNx (SquadLx>>SpraySH)
#define SprayNy (SquadLy>>SpraySH)

#define SprayNSH (SquadSH-SpraySH)

#define SprayN SprayNx*SprayNy

struct SprayCell{	
	byte R,G,B,A; // ����������� ��������	
	byte r,g,b,a; // current

	bool Process();
};

class  SpraySquad{
public:
	void Init();
	SprayCell Cells[SprayN];
	
	bool Process();
};

class SpraySystem{
public:
	int SquadNx;
	int SquadNy;
	int SquadNSH;
	int SquadN;

	void Clear(){ memset(this,0,sizeof(*this)); }
	SpraySystem(){ Clear(); }	
	void Free(){
		if(Squads){
			for(int i=0;i<SquadN;i++) if(Squads[i]){
				free(Squads[i]);
			}
			free(Squads);
		}
		if(DXSquare) free(DXSquare);
		Clear();
	}
	void Init(){
		SquadNx=(8192<<ADDSH)>>SquadSH;
		SquadNy=(8192<<ADDSH)>>SquadSH;
		SquadNSH=13+ADDSH-SquadSH;
		SquadN=SquadNx*SquadNy;
		if(Squads) Free();
		Squads=(SpraySquad**)malloc(sizeof(SpraySquad*)*SquadN);
	}

	SpraySquad** Squads;//SquadN

	bool AddDust(int x, int y, byte r, byte g, byte b, byte a);
	int GetSprayDust(int x,int y);

	SquareObject* DXSquare;

	void Process();
	void Draw();

};

extern SpraySystem SpraySys;

void InitSpraySystem();
void UnLoadSpraySystem();
void AddOneDust(int x, int y);
void DrawDust();

#endif //SpraySystem_def