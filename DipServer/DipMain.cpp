#include "CommonDip.h"
#include "Mind.h"
#include "Economic.h"
extern CBattle Battle;
DiplomacySystem DIPS;
//Economic Auto_Economic;
//extern Economic Auto_Economic;

int  DIP_SimpleBuilding::CurIndex=0;
CIMPORT
void ActivateDipDialog(char* request);
CIMPORT
void SendDipCommand(char* Data,int size);

void PerformBattleCommads(char* Data,int size);
CEXPORT
void PerformDipCommand(char* Data,int size){
	DIPS.PerformCommand(Data,size);
	PerformBattleCommads(Data,size);
	//Auto_Economic.PerformCommand(Data,size);
};

void LoadMindData(byte* ptr,int size);
void SaveMindData(byte** ptr,int* size);
void ClearMindData();

byte* tmpptr=NULL;
#define SAVEAR(field,array,type)\
	tmpptr=(byte*)realloc(tmpptr,sz+4);\
	((int*)(tmpptr+sz))[0]=DIPS.field;\
	sz+=4;\
	if(DIPS.field){\
		tmpptr=(byte*)realloc(tmpptr,sz+DIPS.field*sizeof(type));\
		memcpy(tmpptr+sz,DIPS.array,DIPS.field*sizeof(type));\
		sz+=DIPS.field*sizeof(type);\
	};

#define SAFE_SAVEAR(field, array, type) \
    do { \
        /* First realloc for the count field */ \
        byte* temp1 = (byte*)realloc(tmpptr, sz + 4); \
        if (temp1 == NULL) { \
            free(tmpptr); \
            tmpptr = NULL; \
            sz = 0; \
            break; \
        } \
        tmpptr = temp1; \
        ((int*)(tmpptr + sz))[0] = DIPS.field; \
        sz += 4; \
        \
        /* Second realloc for the array data if needed */ \
        if (DIPS.field > 0) { \
            size_t array_size = DIPS.field * sizeof(type); \
            byte* temp2 = (byte*)realloc(tmpptr, sz + array_size); \
            if (temp2 == NULL) { \
                free(tmpptr); \
                tmpptr = NULL; \
                sz = 0; \
                break; \
            } \
            tmpptr = temp2; \
            memcpy(tmpptr + sz, DIPS.array, array_size); \
            sz += array_size; \
        } \
    } while (0)

CEXPORT
void SaveAllDipData(byte** ptr, int* size) {
    int sz = 4;
    tmpptr = (byte*)malloc(sz);
    if (tmpptr == NULL) {
        *size = 0;
        *ptr = NULL;
        return;
    }

    *((int*)tmpptr) = DIPS.NDIPS;

    for (int i = 0; i < DIPS.NDIPS; i++) {
        int data_size = DIPS.DIPS[i]->GetSaveData(NULL, 0);

        byte* temp = (byte*)realloc(tmpptr, sz + data_size);
        if (temp == NULL) {
            free(tmpptr);
            *size = 0;
            *ptr = NULL;
            return;
        }
        tmpptr = temp;

        DIPS.DIPS[i]->GetSaveData(tmpptr + sz, 1);
        sz += data_size;
    }

    // Safe SAVEAR calls with proper error handling
    SAFE_SAVEAR(NWalk, WID, word);
    SAFE_SAVEAR(NWalk, WSN, word);
    SAFE_SAVEAR(NFirers, FIRERS, FiringGroup);
    SAFE_SAVEAR(NKillers, KILLERS, KillersGroup);
    SAFE_SAVEAR(NTomahawks, TOMAHAWKS, TomahawkGroup);
    SAFE_SAVEAR(DMSize, DangerMap, short);
    SAFE_SAVEAR(NZOO, ZOO, ZooGroup);
    SAFE_SAVEAR(NBZOO, BZOO, ZooBirthZone);
    SAFE_SAVEAR(NCannons, CANNONS, CannonGroup);
    SAFE_SAVEAR(NStorms, STORMS, StormGroup);

    // Safe realloc for StartPopulation
    byte* temp = (byte*)realloc(tmpptr, sz + sizeof(DIPS.StartPopulation));
    if (temp == NULL) {
        free(tmpptr);
        *size = 0;
        *ptr = NULL;
        return;
    }
    tmpptr = temp;
    memcpy(tmpptr + sz, DIPS.StartPopulation, sizeof(DIPS.StartPopulation));
    sz += sizeof(DIPS.StartPopulation);

    SaveMindData(&tmpptr, &sz);

    *size = sz;
    *ptr = tmpptr;
};
CEXPORT
void FreeSaveDipData(byte* ptr){
	if(tmpptr)free(tmpptr);
	tmpptr=NULL;
};
CEXPORT
void ClearAllDipData(){
	for(int i=0;i<DIPS.NDIPS;i++){
		DIPS.DIPS[i]->Free();
		delete(DIPS.DIPS[i]);
	};
	if(DIPS.DIPS)free(DIPS.DIPS);
	DIPS.DIPS=NULL;
	DIPS.NDIPS=0;
	DIPS.BusyPageIndex=-1;
	if(DIPS.FIRERS)free(DIPS.FIRERS);
	if(DIPS.WID)free(DIPS.WID);
	if(DIPS.WSN)free(DIPS.WSN);
	if(DIPS.KILLERS)free(DIPS.KILLERS);
	if(DIPS.TOMAHAWKS)free(DIPS.TOMAHAWKS);
	//if(DIPS.DangerMap)free(DIPS.DangerMap);
	if(DIPS.ZOO)free(DIPS.ZOO);
	if(DIPS.BZOO)free(DIPS.BZOO);
	if(DIPS.CANNONS)free(DIPS.CANNONS);
	if(DIPS.STORMS)free(DIPS.STORMS);
	memset(&DIPS,0,sizeof DIPS);
	ClearMindData();
};
#define LOADAR(field, array, type) \
    do { \
        DIPS.field = ((int*)(ptr + sz))[0]; \
        sz += 4; \
        if (DIPS.field > 0) { \
            DIPS.array = (type*)malloc(DIPS.field * sizeof(type)); \
            if (DIPS.array == NULL) { \
                /* Handle allocation failure */ \
                DIPS.field = 0; \
                /* Optionally: log error, throw exception, or return early */ \
            } else { \
                memcpy(DIPS.array, ptr + sz, DIPS.field * sizeof(type)); \
                sz += DIPS.field * sizeof(type); \
            } \
        } else { \
            DIPS.array = NULL; \
        } \
    } while (0)
CEXPORT
void LoadAllDipData(byte* ptr, int size) {
    ClearAllDipData();
    int sz = 4;

    if (size < 4) {
        // Handle invalid data
        return;
    }

    DIPS.NDIPS = ((int*)ptr)[0];

    // Allocate DIPS array with proper error checking
    if (DIPS.NDIPS > 0) {
        DIPS.DIPS = (BasicDiploRelation**)malloc(DIPS.NDIPS * sizeof(BasicDiploRelation*));
        if (DIPS.DIPS == NULL) {
            DIPS.NDIPS = 0;
            return; // Allocation failed
        }
    }
    else {
        DIPS.DIPS = NULL;
    }

    for (int i = 0; i < DIPS.NDIPS; i++) {
        if (sz + 1 > size) break; // Check bounds

        byte tp = ptr[sz];
        sz++;

        if (sz + 4 > size) break; // Check bounds

        int data_size = ((DWORD*)(ptr + sz))[0];
        sz += 4;

        if (sz + data_size > size) break; // Check bounds

        if (tp == 1) {
            DIPS.DIPS[i] = new DIP_SimpleBuilding;
            if (DIPS.DIPS[i] != NULL) {
                DIPS.DIPS[i]->SetSaveData(ptr + sz, data_size);
                DIPS.DIPS[i]->DIP = &DIPS;
            }
            sz += data_size - 5;
        }
    }

    // Safe LOADAR calls
    LOADAR(NWalk, WID, word);
    LOADAR(NWalk, WSN, word);
    LOADAR(NFirers, FIRERS, FiringGroup);
    LOADAR(NKillers, KILLERS, KillersGroup);
    LOADAR(NTomahawks, TOMAHAWKS, TomahawkGroup);
    LOADAR(DMSize, DangerMap, short);
    LOADAR(NZOO, ZOO, ZooGroup);
    LOADAR(NBZOO, BZOO, ZooBirthZone);
    LOADAR(NCannons, CANNONS, CannonGroup);
    LOADAR(NStorms, STORMS, StormGroup);

    // Safe memcpy for StartPopulation
    if (sz + sizeof(DIPS.StartPopulation) <= size) {
        memcpy(DIPS.StartPopulation, ptr + sz, sizeof(DIPS.StartPopulation));
    }
    sz += sizeof(DIPS.StartPopulation);

    LoadMindData(tmpptr, sz);

    if (DIPS.NDIPS) {
        if (DIPS.CreateContactsList(1)) {
            ActivateDipDialog("LW_file&Dip/tmp.cml");
        }
    }
};
CEXPORT
void InitDipForThisMap(){
	DIPS.ResearchMap();
	//Auto_Economic.ResearchMap();
};
CEXPORT
void ProcessDipRelations(){
	DIPS.Process();
	//Auto_Economic.Process();
	for(int i=0;i<7;i++) Player[i].Process();
	Battle.Process();
};

void HandleDipRequest(char* com,ParsedRQ* Request,ParsedRQ* Result){
};
void HandleBattleCml(char* com,char** params,int npr,ParsedRQ* Result);
void Handle_dipcomm(char* com,char** params,int npr,ParsedRQ* Result){
	if(!strcmp(com,"dipyes")){
		if(npr){
			int idx=atoi(params[0]);
			int clr=atoi(params[1]);
			BasicDiploRelation* BDR=DIPS.DIPS[idx];
			if(BDR->Owner==7&&clr==GetMyNation()){
				word Data[8];
				Data[0]=1;
				Data[1]=idx;//Index of tribe
				Data[2]=word(clr);//color of player
				Data[3]=atoi(params[2]);//Index of shaman
                SendDipCommand((char*)Data, sizeof(Data));
				DIP_SimpleBuilding* DSB=(DIP_SimpleBuilding*)DIPS.DIPS[Data[1]];
				DString DST;
				DST.ReadFromFile("Dip\\WaitMask.cml");
				DST.Replace("$CONTXT$",GetTextByID("$CONTXT$"));
				DST.Replace("$GPVOG$",DSB->gpPix);
				DST.Replace("$FRAME$",DSB->gpidx);
				DST.Replace("$PROPOSE$",GetTextByID("$LETMETHINK$"));
				DST.WriteToFile("Dip\\tmp.cml");
				Result->Clear();
				Result->Parse("LW_file&Dip/tmp.cml");
				DSB->DIP->BusyPageIndex=-1;
			};
		};
	}else
	if(!strcmp(com,"dipno")){
		int idx=atoi(params[0]);
		int clr=atoi(params[1]);
		BasicDiploRelation* BDR=DIPS.DIPS[idx];
		if(BDR->Owner==7&&clr==GetMyNation()){
			DIP_SimpleBuilding* DSB=(DIP_SimpleBuilding*)DIPS.DIPS[idx];
			DString DST;
			DST.ReadFromFile("Dip\\WaitMask.cml");
			DST.Replace("$CONTXT$",GetTextByID("$CONTXT$"));
			DST.Replace("$GPVOG$",DSB->gpPix);
			DST.Replace("$FRAME$",DSB->gpidx);
			DST.Replace("$PROPOSE$",GetTextByID(DSB->TellNo));	
			DST.WriteToFile("Dip\\tmp.cml");
			Result->Clear();
			Result->Parse("LW_file&Dip/tmp.cml");
			DSB->DIP->BusyPageIndex=-1;
		};
	}else
	if(!strcmp(com,"dipresell")){
		if(npr){
			int idx=atoi(params[0]);
			int clr=atoi(params[1]);
			BasicDiploRelation* BDR=DIPS.DIPS[idx];
			if(clr==GetMyNation()){
				word Data[8];
				Data[0]=2;
				Data[1]=idx;//Index of tribe
				Data[2]=word(clr);//color of player
				Data[3]=atoi(params[2]);//Index of shaman
                SendDipCommand((char*)Data, sizeof(Data));
				DIP_SimpleBuilding* DSB=(DIP_SimpleBuilding*)DIPS.DIPS[Data[1]];
				DString DST;
				DST.ReadFromFile("Dip\\WaitMask.cml");
				DST.Replace("$CONTXT$",GetTextByID("$CONTXT$"));
				DST.Replace("$GPVOG$",DSB->gpPix);
				DST.Replace("$FRAME$",DSB->gpidx);
				DST.Replace("$PROPOSE$",GetTextByID("$LETMETHINK$"));
				DST.WriteToFile("Dip\\tmp.cml");
				Result->Clear();
				Result->Parse("LW_file&Dip/tmp.cml");
				DSB->DIP->BusyPageIndex=-1;
			};
		};
	}else
	if(!strcmp(com,"cnlist")){
		DIPS.CreateContactsList(1);
		Result->Clear();
		Result->Parse("LW_file&Dip/tmp.cml");
		DIPS.BusyPageIndex=-1;
	}else
	if(!strcmp(com,"cnlist1")){
		DIPS.CreateContactsList(0);
		Result->Clear();
		Result->Parse("LW_file&Dip/tmp.cml");
		DIPS.BusyPageIndex=-1;
	}else
	if(!strcmp(com,"showtribe")){
		if(npr){
			int idx=atoi(params[0]);
			if(idx<DIPS.NDIPS){
				DIP_SimpleBuilding* DSB=(DIP_SimpleBuilding*)DIPS.DIPS[idx];
				DSB->CreateAbilityPage();
				GAMEOBJ Zone;
				UnitsCenter(&Zone,&DSB->CentralGroup,512);
				SetStartPoint(&Zone);
				Result->Clear();
				Result->Parse("LW_file&Dip/tmp.cml");
			};
		};
	}else if(!strcmp(com,"perform")){
		if(npr>1){
			int idx=atoi(params[0]);
			int pidx=atoi(params[1]);
			int ni=atoi(params[2]);
			if(idx<DIPS.NDIPS){
				DIP_SimpleBuilding* DSB=((DIP_SimpleBuilding*)DIPS.DIPS[idx]);
				if(ni==DSB->Owner){
					if(DSB->Busy&&DSB->Owner==GetMyNation()){
						DSB->CreateAbilityPage();
						ActivateDipDialog("LW_file&Dip/tmp.cml");
					}else DSB->PerformPossibility(pidx);
				}else{
					if(ni==GetMyNation()){
						DSB->CreateAbilityPage();
						Result->Clear();
						Result->Parse("LW_file&Dip/tmp.cml");
					};
				};
			};
		};
	};
	HandleBattleCml(com,params,npr,Result);
	//Auto_Economic.EconomicDipComm(com,params,npr,Result);
	
};