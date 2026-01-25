#include "CommonDip.h"


void DIP_SimpleBuilding::InitThisTribe2(char* FileID, char* Dip, char* GDance, char* ZDance){
	InitThisTribe("",Dip,FileID);	
	RegisterUnits(&Actors,GDance);
	UseLikeSkarb=(Actors.Type==0);	
	NStartActors=GetNUnits(&Actors);
	if(NStartActors){
		OneUnit OU;
		if(GetUnitInfo(&Actors,0,&OU)){;
			ActorsType=OU.NIndex;
		};
	};
	RegisterZone(&DanceZone,ZDance);
#ifdef AC_ADDON
	RegisterDynGroup(&Friends);
#endif AC_ADDON
}

void DIP_SimpleBuilding::InitThisTribe(char* Preffix, char* FirstGroupName, char* CommonID) {
    char CCC[1024];
#ifdef AC_ADDON
    EasyTalk = 0;
#endif // AC_ADDON
    Busy = 0;
    Owner = 7;
    DipProposition = 0;
    CounsilNI = 7;
    DIPPAR = NULL;
    RepType = -1;
    DefType = -1;
    MaxRep = 0;
    MaxDef = 0;
    AddDefAttack = 0;
    AddDefShield = 0;
    DanceStage = 0;
    ActorsType = -1;
    NDipPar = 0;
    strcpy(TellNo, "$TELLNO");
    strcpy(CongratID, "$WECAN$");
    memset(ReSellCost, 0, sizeof(ReSellCost));
    CurrentStateMessage[0] = 0;

    if (RegisterUnits(&CentralGroup, FirstGroupName)) {
        char cc[256];

        // Zero-terminate and validate string operations
        snprintf(cc, sizeof(cc), "$ACTORS-%s%s", CommonID, Preffix);
        cc[sizeof(cc) - 1] = '\0';
        RegisterUnits(&Actors, cc);
        UseLikeSkarb = Actors.Type == 0;

        snprintf(cc, sizeof(cc), "$DZONE-%s%s", CommonID, Preffix);
        cc[sizeof(cc) - 1] = '\0';
        RegisterZone(&DanceZone, cc);
        DanceStage = 0;

        RegisterDynGroup(&Repairers);
        OneUnit OU;
        if (GetUnitInfo(&CentralGroup, 0, &OU)) {
            SGP_RepairBuilding(&Repairers, 0, OU.Index);
        }

        RegisterDynGroup(&Defenders);
        RegisterDynGroup(&ComeInside);
        if (GetUnitInfo(&CentralGroup, 0, &OU)) {
            SGP_ComeIntoBuilding(&ComeInside, 0, OU.Index);
        }

        NStartActors = GetNUnits(&Actors);
        if (NStartActors) {
            OneUnit OU;
            if (GetUnitInfo(&Actors, 0, &OU)) {
                ActorsType = OU.NIndex;
            }
        }

        snprintf(cc, sizeof(cc), "Dip\\%s", CommonID);
        cc[sizeof(cc) - 1] = '\0';

        // Replace unsafe strcpy with safer alternative
        size_t len = strlen(cc);
        if (len >= 2) {
            strcpy(cc + len - 2, ".dat");
        }
        else {
            // Handle error: string too short
            strcat(cc, ".dat");
        }

        GFILE* F = Gopen(cc, "r");
        if (F) {
            bool good = true;
            do {
                char str[1024];
                good = ReadWinString(F, str, sizeof(str) - 1);
                str[sizeof(str) - 1] = '\0'; // Ensure null termination
                if (good) {
                    char com[128] = { 0 };

                    // Safe sscanf with length limitation
                    if (sscanf(str, "%127s", com) == 1) {
                        if (strcmp(com, "$PSB") == 0) {
                            DipComParams DPP = { 0 };
                            int z = sscanf(str, "%127s %127s %127s %127s %d %d %d %d %d %d %127s",
                                com, DPP.MessageID, DPP.HintID, DPP.ProcessID,
                                &DPP.Cost[0], &DPP.Cost[1], &DPP.Cost[2],
                                &DPP.Cost[3], &DPP.Cost[4], &DPP.Cost[5], DPP.ComID);

                            if (z == 11) {
                                // Ensure all strings are null-terminated
                                DPP.MessageID[sizeof(DPP.MessageID) - 1] = '\0';
                                DPP.HintID[sizeof(DPP.HintID) - 1] = '\0';
                                DPP.ProcessID[sizeof(DPP.ProcessID) - 1] = '\0';
                                DPP.ComID[sizeof(DPP.ComID) - 1] = '\0';

                                // Safe realloc with null check
                                DipComParams* temp = (DipComParams*)realloc(DIPPAR, (NDipPar + 1) * sizeof(DipComParams));
                                if (temp == NULL) {
                                    MissErrorMessage("Memory error", "Failed to allocate memory for DIPPAR");
                                    break;
                                }
                                DIPPAR = temp;

                                char* s = strstr(str, DPP.ComID);
                                if (s) {
                                    size_t comIDLen = strlen(DPP.ComID);
                                    if (strlen(s) > comIDLen) {
                                        strncpy(DPP.Params, s + comIDLen + 1, sizeof(DPP.Params) - 1);
                                        DPP.Params[sizeof(DPP.Params) - 1] = '\0';
                                    }
                                    else {
                                        DPP.Params[0] = '\0';
                                    }
                                }
                                else {
                                    DPP.Params[0] = '\0';
                                }

                                memcpy(&DIPPAR[NDipPar], &DPP, sizeof(DipComParams));
                                NDipPar++;

                                RegisterDynGroup(&DestGroup);
                                BigZone.Type = 0;
                                UnitsCenter(&BigZone, &CentralGroup, 800);
                                UnitsCenter(&VeryBigZone, &CentralGroup, 750);
                            }
                            else {
                                snprintf(CCC, sizeof(CCC), "%s : Invalid string : %s", cc, str);
                                MissErrorMessage("Dip error!", CCC);
                            }
                        }
                        else if (strcmp(com, "$IFTELLNO$") == 0) {
                            char tempTellNo[128] = { 0 }; // Initialize and zero-terminate
                            if (sscanf(str, "%127s %127s", com, tempTellNo) == 2) {
                                strncpy(TellNo, tempTellNo, sizeof(TellNo) - 1);
                                TellNo[sizeof(TellNo) - 1] = '\0';
                            }
                        }
                        else if (strcmp(com, "$CONGRAT") == 0) {
                            char tempCongrat[128] = { 0 }; // Initialize and zero-terminate
                            if (sscanf(str, "%127s %127s", com, tempCongrat) == 2) {
                                strncpy(CongratID, tempCongrat, sizeof(CongratID) - 1);
                                CongratID[sizeof(CongratID) - 1] = '\0';
                            }
                        }
                        else if (strcmp(com, "$FIRSTMEET") == 0) {
                            char tempFirstMeet[128] = { 0 }; // Initialize and zero-terminate
                            if (sscanf(str, "%127s %127s", com, tempFirstMeet) == 2) {
                                strncpy(FirstMeetID, tempFirstMeet, sizeof(FirstMeetID) - 1);
                                FirstMeetID[sizeof(FirstMeetID) - 1] = '\0';
                            }
                        }
                        else if (strcmp(com, "$RESELL") == 0) {
                            char tempReSell[128] = { 0 }; // Initialize and zero-terminate
                            if (sscanf(str, "%127s %127s", com, tempReSell) == 2) {
                                strncpy(ReSellID, tempReSell, sizeof(ReSellID) - 1);
                                ReSellID[sizeof(ReSellID) - 1] = '\0';
                            }
                        }
                        else if (strcmp(com, "$STARTPRICE") == 0) {
                            if (sscanf(str, "%127s %d %d %d %d %d %d", com,
                                &ReSellCost[0], &ReSellCost[1], &ReSellCost[2],
                                &ReSellCost[3], &ReSellCost[4], &ReSellCost[5]) != 7) {
                                // Handle parsing error
                            }
                        }
                        else if (strcmp(com, "$NAME") == 0) {
                            char NID[128] = { 0 }; // Initialize and zero-terminate
                            if (sscanf(str, "%127s %127s", com, NID) == 2) {
                                int idx = 0;
                                bool doit = true;
                                do {
                                    char CCC[128];
                                    snprintf(CCC, sizeof(CCC), NID, idx);
                                    CCC[sizeof(CCC) - 1] = '\0';
                                    char* id = GetTextByID(CCC);
                                    if (id[0] != '$') {
                                        idx++;
                                    }
                                    else {
                                        doit = false;
                                    }
                                } while (doit && idx < 1000);

                                if (idx > 0) {
                                    snprintf(NameOfTribe, sizeof(NameOfTribe), NID, CurIndex % idx);
                                }
                                else {
                                    snprintf(NameOfTribe, sizeof(NameOfTribe), NID, 0);
                                }
                                NameOfTribe[sizeof(NameOfTribe) - 1] = '\0';
                                CurIndex++;
                            }
                        }
                        else if (strcmp(com, "$PIX") == 0) {
                            char tempPix[128] = { 0 }; // Initialize and zero-terminate
                            int st = 0;
                            int npix = 0;
                            if (sscanf(str, "%127s %127s %d %d", com, tempPix, &st, &npix) == 4) {
                                strncpy(gpPix, tempPix, sizeof(gpPix) - 1);
                                gpPix[sizeof(gpPix) - 1] = '\0';
                                if (npix > 0) {
                                    gpidx = st + (CurIndex % npix);
                                }
                                else {
                                    gpidx = -1;
                                }
                            }
                        }
                        else if (strcmp(com, "$REPAIR") == 0) {
                            char cc[128] = { 0 }; // Initialize and zero-terminate
                            int st = 0;
                            if (sscanf(str, "%127s %127s %d", com, cc, &st) == 3) {
                                GAMEOBJ UTP;
                                RegisterUnitType(&UTP, cc);
                                if (UTP.Type == 'UTYP') {
                                    RepType = static_cast<short>(UTP.Index);
                                    MaxRep = st;
                                }
                            }
                        }
                        else if (strcmp(com, "$DEFEND") == 0 || strcmp(com, "$DEFENCE") == 0) {
                            char cc[128] = { 0 }; // Initialize and zero-terminate
                            int max = 0, ada = 0, ads = 0;
                            if (sscanf(str, "%127s %127s %d %d %d", com, cc, &max, &ada, &ads) == 5) {
                                GAMEOBJ UTP;
                                RegisterUnitType(&UTP, cc);
                                if (UTP.Type == 'UTYP') {
                                    DefType = static_cast<short>(UTP.Index);
                                    MaxDef = max;
                                    AddDefAttack = ada;
                                    AddDefShield = ads;
                                }
                            }
                        }
                    }
                }
            } while (good);
            Gclose(F);
        }
        else {
            snprintf(CCC, sizeof(CCC), "Unable to open : %s", cc);
            CCC[sizeof(CCC) - 1] = '\0';
            MissErrorMessage("Dip error!", CCC);
        }
    }
}
int DIP_SimpleBuilding::GetAmountOfPropositions(){
	return NDipPar;
};
DWORD DIP_SimpleBuilding::GetPossibility(int Index,char* Message,char* Hint){
	strcpy(Message,GetTextByID(DIPPAR[Index].MessageID));
	strcpy(Hint,GetTextByID(DIPPAR[Index].HintID));
	return Index;
};
extern DiplomacySystem DIPS;
void DIP_SimpleBuilding::PerformPossibility(DWORD Code){
	bool success=0;
	char* cod=DIPPAR[Code].ComID;
	if(!strcmp(cod,"@@@PROD")){
		//produce units
		ProcessID=1;
		//sending command
		word BUF[32];
		BUF[0]=0;//standart possibility
		BUF[1]=Index;//tribe index
		BUF[2]=word(Code);//possibility code
		SendDipCommand((char*)BUF,6);
	}else
	if(!strcmp(cod,"@@@RAZVEDKA")){
		ProcessID=2;
		//sending command
		word BUF[32];
		BUF[0]=0;//standart possibility
		BUF[1]=Index;//tribe index
		BUF[2]=word(Code);//possibility code
		SendDipCommand((char*)BUF,6);
	}else
	if(!strcmp(cod,"@@@FIRERS")){
		ProcessID=3;
		//sending command
		word BUF[32];
		BUF[0]=0;//standart possibility
		BUF[1]=Index;//tribe index
		BUF[2]=word(Code);//possibility code
		SendDipCommand((char*)BUF,6);
	}else
	if(!strcmp(cod,"@@@STORMS")){
		ProcessID=4;
		//sending command
		word BUF[32];
		BUF[0]=0;//standart possibility
		BUF[1]=Index;//tribe index
		BUF[2]=word(Code);//possibility code
		SendDipCommand((char*)BUF,6);
	}else
	if(!strcmp(cod,"@@@KILLERS")){
		ProcessID=5;
		//sending command
		word BUF[32];
		BUF[0]=0;//standart possibility
		BUF[1]=Index;//tribe index
		BUF[2]=word(Code);//possibility code
		SendDipCommand((char*)BUF,6);
	}else
	if(!strcmp(cod,"@@@TOMAHAWKS")){
		ProcessID=6;
		//sending command
		word BUF[32];
		BUF[0]=0;//standart possibility
		BUF[1]=Index;//tribe index
		BUF[2]=word(Code);//possibility code
		SendDipCommand((char*)BUF,6);
	};
	if(success){
		Busy=1;
		strcpy(CurrentStateMessage,DIPPAR[Code].ProcessID);
	};
};
void DIP_SimpleBuilding::PerformCommand(char* Data,int size){
	if(Busy){
		if(Owner==GetMyNation()){
			CreateAbilityPage();
			ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
		};
		return;
	};
	word* wData=(word*)Data;
	switch(wData[0]){
	case 0://perform possibility (produce units...)
		{
			int Code=wData[2];
			char* cod=DIPPAR[Code].ComID;
			if(!strcmp(cod,"@@@PROD")){
				//produce units
				int N;
				char ID[64];
				int z=sscanf(DIPPAR[Code].Params,"%d%s",&N,ID);
				if(z==2){
					//checking for cost
					for(int i=0;i<6;i++)if(GetResource(Owner,i)<DIPPAR[Code].Cost[i]){
						if(Owner==GetMyNation())ShowNoMoney();
						return;
					};
					for(int i=0;i<6;i++)SetResource(Owner,i,GetResource(Owner,i)-DIPPAR[Code].Cost[i]);
					RegisterUnitType(&info.Prod.UnitType,ID);
					info.Prod.Amount=N;
					info.Prod.Stage=0;
					info.Prod.StartTime=GetGlobalTime();
#ifdef AC_ADDON
					byte speed;
					z=sscanf(DIPPAR[Code].Params,"%d%s%d",&N,ID,&speed);
					info.Prod.Speed=3;
					if(z>2&&speed<8) info.Prod.Speed=speed;
#endif AC_ADDON
					ProcessID=1+1024;
					Busy=1;
					strcpy(CurrentStateMessage,DIPPAR[Code].ProcessID);
				}else{
					ProcessID=-1;//error
					char Temp[128];
					sprintf(Temp,"error: bad string: %s",DIPPAR[Code].Params);
					strcpy(CurrentStateMessage,Temp);
				};
			}else
			if(!strcmp(cod,"@@@RAZVEDKA")){
				int N;
				char ID[64];
				int z=sscanf(DIPPAR[Code].Params,"%d%s",&N,ID);
				if(z==2){
					//checking for cost
					for(int i=0;i<6;i++)if(GetResource(Owner,i)<DIPPAR[Code].Cost[i]){
						if(Owner==GetMyNation())ShowNoMoney();
						return;
					};
					for(int i=0;i<6;i++)SetResource(Owner,i,GetResource(Owner,i)-DIPPAR[Code].Cost[i]);
					RegisterUnitType(&info.Prod.UnitType,ID);
					info.Prod.Amount=N;
					info.Prod.Stage=0;
					info.Prod.StartTime=GetGlobalTime();
#ifdef AC_ADDON
					byte speed;
					z=sscanf(DIPPAR[Code].Params,"%d%s%d",&N,ID,&speed);
					info.Prod.Speed=3;
					if(z>2&&speed<8) info.Prod.Speed=speed;
#endif AC_ADDON
					ProcessID=2+1024;
					Busy=1;
					strcpy(CurrentStateMessage,DIPPAR[Code].ProcessID);
				}else{
					ProcessID=-1;//error
					char Temp[128];
					sprintf(Temp,"error: bad string: %s",DIPPAR[Code].Params);
					strcpy(CurrentStateMessage,Temp);
				};
			}else
			if(!strcmp(cod,"@@@FIRERS")){
				//produce units
				int N;
				char ID[64];
				int z=sscanf(DIPPAR[Code].Params,"%d%s",&N,ID);
				if(z==2){
					//checking for cost
					for(int i=0;i<6;i++)if(GetResource(Owner,i)<DIPPAR[Code].Cost[i]){
						if(Owner==GetMyNation())ShowNoMoney();
						return;
					};
					for(int i=0;i<6;i++)SetResource(Owner,i,GetResource(Owner,i)-DIPPAR[Code].Cost[i]);
					RegisterUnitType(&info.Prod.UnitType,ID);
					info.Prod.Amount=N;
					info.Prod.Stage=0;
					info.Prod.StartTime=GetGlobalTime();
#ifdef AC_ADDON
					byte speed;
					z=sscanf(DIPPAR[Code].Params,"%d%s%d",&N,ID,&speed);
					info.Prod.Speed=3;
					if(z>2&&speed<8) info.Prod.Speed=speed;
#endif AC_ADDON
					ProcessID=3+1024;
					Busy=1;
					strcpy(CurrentStateMessage,DIPPAR[Code].ProcessID);
				}else{
					ProcessID=-1;//error
					char Temp[128];
					sprintf(Temp,"error: bad string: %s",DIPPAR[Code].Params);
					strcpy(CurrentStateMessage,Temp);
				};
			}else
			if(!strcmp(cod,"@@@STORMS")){
				//produce units
				int N;
				char ID[64];
				int z=sscanf(DIPPAR[Code].Params,"%d%s",&N,ID);
				if(z==2){
					//checking for cost
					for(int i=0;i<6;i++)if(GetResource(Owner,i)<DIPPAR[Code].Cost[i]){
						if(Owner==GetMyNation())ShowNoMoney();
						return;
					};
					for(int i=0;i<6;i++)SetResource(Owner,i,GetResource(Owner,i)-DIPPAR[Code].Cost[i]);
					RegisterUnitType(&info.Prod.UnitType,ID);
					info.Prod.Amount=N;
					info.Prod.Stage=0;
					info.Prod.StartTime=GetGlobalTime();
#ifdef AC_ADDON
					byte speed;
					z=sscanf(DIPPAR[Code].Params,"%d%s%d",&N,ID,&speed);
					info.Prod.Speed=3;
					if(z>2&&speed<8) info.Prod.Speed=speed;
#endif AC_ADDON
					ProcessID=4+1024;
					Busy=1;
					strcpy(CurrentStateMessage,DIPPAR[Code].ProcessID);
				}else{
					ProcessID=-1;//error
					char Temp[128];
					sprintf(Temp,"error: bad string: %s",DIPPAR[Code].Params);
					strcpy(CurrentStateMessage,Temp);
				};
			}else
			if(!strcmp(cod,"@@@KILLERS")){
				//produce units
				int N;
				char ID[64];
				int z=sscanf(DIPPAR[Code].Params,"%d%s",&N,ID);
				if(z==2){
					//checking for cost
					for(int i=0;i<6;i++)if(GetResource(Owner,i)<DIPPAR[Code].Cost[i]){
						if(Owner==GetMyNation())ShowNoMoney();
						return;
					};
					for(int i=0;i<6;i++)SetResource(Owner,i,GetResource(Owner,i)-DIPPAR[Code].Cost[i]);
					RegisterUnitType(&info.Prod.UnitType,ID);
					info.Prod.Amount=N;
					info.Prod.Stage=0;
					info.Prod.StartTime=GetGlobalTime();
#ifdef AC_ADDON
					byte speed;
					z=sscanf(DIPPAR[Code].Params,"%d%s%d",&N,ID,&speed);
					info.Prod.Speed=3;
					if(z>2&&speed<8) info.Prod.Speed=speed;
#endif AC_ADDON
					ProcessID=5+1024;
					Busy=1;
					strcpy(CurrentStateMessage,DIPPAR[Code].ProcessID);
				}else{
					ProcessID=-1;//error
					char Temp[128];
					sprintf(Temp,"error: bad string: %s",DIPPAR[Code].Params);
					strcpy(CurrentStateMessage,Temp);
				};
			}else
			if(!strcmp(cod,"@@@TOMAHAWKS")){
				//produce units
				int N;
				char ID[64];
				int z=sscanf(DIPPAR[Code].Params,"%d%s",&N,ID);
				if(z==2){
					//checking for cost
					for(int i=0;i<6;i++)if(GetResource(Owner,i)<DIPPAR[Code].Cost[i]){
						if(Owner==GetMyNation())ShowNoMoney();
						return;
					};
					for(int i=0;i<6;i++)SetResource(Owner,i,GetResource(Owner,i)-DIPPAR[Code].Cost[i]);
					RegisterUnitType(&info.Prod.UnitType,ID);
					info.Prod.Amount=N;
					info.Prod.Stage=0;
					info.Prod.StartTime=GetGlobalTime();
#ifdef AC_ADDON
					byte speed;
					z=sscanf(DIPPAR[Code].Params,"%d%s%d",&N,ID,&speed);
					info.Prod.Speed=3;
					if(z>2&&speed<8) info.Prod.Speed=speed;
#endif AC_ADDON
					ProcessID=6+1024;
					Busy=1;
					strcpy(CurrentStateMessage,DIPPAR[Code].ProcessID);
				}else{
					ProcessID=-1;//error
					char Temp[128];
					sprintf(Temp,"error: bad string: %s",DIPPAR[Code].Params);
					strcpy(CurrentStateMessage,Temp);
				};
			};
		};
		break;
	case 1://agreement to cowork (first time), if button "yes" pressed
		{
			Owner=byte(wData[2]);
			if(Owner==GetMyNation()){
				CreateAbilityPage();
				ActivateDipDialog("LW_new&|LW_file&Dip//tmp.cml");
			};
			CleanGroup(&CentralGroup);
			CleanGroup(&Actors);
			SetGroupNMASK(&CentralGroup,128|GetNatNMASK(Owner));
			SetGroupNMASK(&Actors,128|GetNatNMASK(Owner));
			word Shaman=wData[3];
			if(Shaman!=0xFFFF){
				word BID=0xFFFF;
				int N=GetNUnits(&CentralGroup);
				OneUnit OU;
				for(int i=0;i<N;i++){
					GetUnitInfo(&CentralGroup,i,&OU);
					if(OU.Index!=0xFFFF)BID=OU.Index;
				};
				ChangeUnitColor(Shaman,7);
				if(BID!=0xFFFF)InviteUnitIntoBuilding(BID,Shaman,0);

			};
		};
		break;
	case 2://agreement to resell itself
		{
			//checking for price
			byte NI=byte(wData[2]);
			bool good=1;
			for(int i=0;i<6;i++)if(GetResource(NI,i)<ReSellCost[i])good=0;
			if(!good){
				if(NI==GetMyNation()){
					if(Owner==GetMyNation())ShowNoMoney();
				};
			}else{
				for(int i=0;i<6;i++){
					SetResource(NI,i,GetResource(NI,i)-ReSellCost[i]);
					ReSellCost[i]<<=1;
				};
				Owner=NI;
				if(Owner==GetMyNation()){
					CreateAbilityPage();
					ActivateDipDialog("LW_new&|LW_file&Dip//tmp.cml");
				};
				CleanGroup(&CentralGroup);
				CleanGroup(&Actors);
				SetGroupNMASK(&CentralGroup,128|GetNatNMASK(Owner));
				SetGroupNMASK(&Actors,128|GetNatNMASK(Owner));
				word Shaman=wData[3];
				if(Shaman!=0xFFFF){
					word BID=0xFFFF;
					int N=GetNUnits(&CentralGroup);
					OneUnit OU;
					for(int i=0;i<N;i++){
						GetUnitInfo(&CentralGroup,i,&OU);
						if(OU.Index!=0xFFFF)BID=OU.Index;
					};
					ChangeUnitColor(Shaman,7);
					if(BID!=0xFFFF)InviteUnitIntoBuilding(BID,Shaman,0);
				};
			};
		};
		break;
	};
	if(Busy){
		if(Owner==GetMyNation()){
			//CreateAbilityPage();
			DIPS.CreateContactsList(1);
			ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
		};
		return;
	};
};
void DIP_SimpleBuilding::PerformProduceUnitsCommend(){
	int Stage=info.Prod.Stage;
	if(Stage==0){
		//need to order units;
		EraseGroup(&DestGroup);
		byte speed=3;
#ifdef AC_ADDON
		speed=info.Prod.Speed;
#endif AC_ADDON
		for(int i=0;i<info.Prod.Amount;i++)
			ProduceUnitFast(&CentralGroup,&info.Prod.UnitType,&DestGroup,speed);
		info.Prod.Stage=1;
	}else
	if(Stage>0){
		//units under construction
		int NU=GetTotalAmount0(&DestGroup);
		SetGroupStartNMASK(&DestGroup,128|GetNatNMASK(Owner));
		if(NU>=info.Prod.Amount||GetGlobalTime()-info.Prod.StartTime>4000){
			//repainting units
			CleanGroup(&DestGroup);
			SelectUnits(&DestGroup,0);
			if(Owner<7){
				SelChangeNation(7,Owner);//units are ready for you
				SetGroupNMASK(&DestGroup,GetNatNMASK(Owner));
			};
			Busy=0;
			ProcessID=0;
			if(DIP->BusyPageIndex==Index){
				CreateAbilityPage();
				ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
			};
		};
		ProcessDance();
	};
};
void DIP_SimpleBuilding::PerformStorms(){
	int Stage=info.Prod.Stage;
	if(Stage==0){
		//need to order units;
		EraseGroup(&DestGroup);
		byte speed=3;
#ifdef AC_ADDON
		speed=info.Prod.Speed;
#endif AC_ADDON
		for(int i=0;i<info.Prod.Amount;i++)
			ProduceUnitFast(&CentralGroup,&info.Prod.UnitType,&DestGroup,speed);
		info.Prod.Stage=1;
	}else
	if(Stage>0){
		//units under construction
		int NU=GetTotalAmount0(&DestGroup);
		SetGroupStartNMASK(&DestGroup,128|GetNatNMASK(Owner));
		if(NU>=info.Prod.Amount||GetGlobalTime()-info.Prod.StartTime>4000){
			CleanGroup(&DestGroup);
			int N=GetNUnits(&DestGroup);
			SetGroupNMASK(&DestGroup,GetNatNMASK(Owner)|128);
			DIP->AddStorm(&DestGroup,Owner);
			CleanGroup(&DestGroup);
			Busy=0;
			ProcessID=0;
			if(DIP->BusyPageIndex==Index){
				CreateAbilityPage();
				ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
			};
		};
		ProcessDance();
	};
};

void DIP_SimpleBuilding::PerformKillers(){
	int Stage=info.Prod.Stage;
	if(Stage==0){
		//need to order units;
		EraseGroup(&DestGroup);
		byte speed=3;
#ifdef AC_ADDON
		speed=info.Prod.Speed;
#endif AC_ADDON
		for(int i=0;i<info.Prod.Amount;i++)
			ProduceUnitFast(&CentralGroup,&info.Prod.UnitType,&DestGroup,speed);
		info.Prod.Stage=1;
	}else
	if(Stage>0){
		//units under construction
		int NU=GetTotalAmount0(&DestGroup);
		SetGroupStartNMASK(&DestGroup,128|GetNatNMASK(Owner));
		if(NU>=info.Prod.Amount||GetGlobalTime()-info.Prod.StartTime>4000){
			CleanGroup(&DestGroup);
			int N=GetNUnits(&DestGroup);
			SetGroupNMASK(&DestGroup,GetNatNMASK(Owner)|128);
			DIP->AddKillers(&DestGroup,Owner);
			CleanGroup(&DestGroup);
			Busy=0;
			ProcessID=0;
			if(DIP->BusyPageIndex==Index){
				CreateAbilityPage();
				ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
			};
		};
		ProcessDance();
	};
};

void DIP_SimpleBuilding::PerformTomahawks(){
	int Stage=info.Prod.Stage;
	if(Stage==0){
		//need to order units;
		EraseGroup(&DestGroup);
		byte speed=3;
#ifdef AC_ADDON
		speed=info.Prod.Speed;
#endif AC_ADDON
		for(int i=0;i<info.Prod.Amount;i++)
			ProduceUnitFast(&CentralGroup,&info.Prod.UnitType,&DestGroup,speed);
		info.Prod.Stage=1;
	}else
	if(Stage>0){
		//units under construction
		int NU=GetTotalAmount0(&DestGroup);
		SetGroupStartNMASK(&DestGroup,128|GetNatNMASK(Owner));
		if(NU>=info.Prod.Amount||GetGlobalTime()-info.Prod.StartTime>4000){
			CleanGroup(&DestGroup);
			int N=GetNUnits(&DestGroup);
			SetGroupNMASK(&DestGroup,GetNatNMASK(Owner)|128);
			DIP->AddTomahawks(&DestGroup,Owner,0,0,0);
			CleanGroup(&DestGroup);
			Busy=0;
			ProcessID=0;
			if(DIP->BusyPageIndex==Index){
				CreateAbilityPage();
				ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
			};
		};
		ProcessDance();
	};
};

void DIP_SimpleBuilding::PerformFiring(){
	int Stage=info.Prod.Stage;
	if(Stage==0){
		//need to order units;
		EraseGroup(&DestGroup);
		byte speed=3;
#ifdef AC_ADDON
		speed=info.Prod.Speed;
#endif AC_ADDON
		for(int i=0;i<info.Prod.Amount;i++)
			ProduceUnitFast(&CentralGroup,&info.Prod.UnitType,&DestGroup,speed);
		info.Prod.Stage=1;
	}else
	if(Stage>0){
		//units under construction
		int NU=GetTotalAmount0(&DestGroup);
		SetGroupStartNMASK(&DestGroup,128|GetNatNMASK(Owner));
		if(NU>=info.Prod.Amount||GetGlobalTime()-info.Prod.StartTime>4000){
			CleanGroup(&DestGroup);
			int N=GetNUnits(&DestGroup);
			SetGroupNMASK(&DestGroup,GetNatNMASK(Owner)|128);
			DIP->AddFirers(&DestGroup,Owner);
			CleanGroup(&DestGroup);
			Busy=0;
			ProcessID=0;
			if(DIP->BusyPageIndex==Index){
				CreateAbilityPage();
				ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
			};
		};
		ProcessDance();
	};
};
void DIP_SimpleBuilding::PerformRazvedka(){
	int Stage=info.Prod.Stage;
	if(Stage==0){
		//need to order units;
		EraseGroup(&DestGroup);
		byte speed=3;
#ifdef AC_ADDON
		speed=info.Prod.Speed;
#endif AC_ADDON
		for(int i=0;i<info.Prod.Amount;i++)
			ProduceUnitFast(&CentralGroup,&info.Prod.UnitType,&DestGroup,speed);
		info.Prod.Stage=1;
	}else
	if(Stage>0){
		//units under construction
		int NU=GetTotalAmount0(&DestGroup);
		SetGroupStartNMASK(&DestGroup,128|GetNatNMASK(Owner));
		if(NU>=info.Prod.Amount||GetGlobalTime()-info.Prod.StartTime>4000){
			CleanGroup(&DestGroup);
			int N=GetNUnits(&DestGroup);
			OneUnit OU;
			SetGroupNMASK(&DestGroup,GetNatNMASK(Owner));
			for(int i=0;i<N;i++){
				if(GetUnitInfo(&DestGroup,i,&OU)){
					DIP->AddWalker(OU.Index);
				};
			};
			CleanGroup(&DestGroup);
			Busy=0;
			ProcessID=0;
			if(DIP->BusyPageIndex==Index){
				CreateAbilityPage();
				ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
			};
		};
		ProcessDance();
	};
};
void DIP_SimpleBuilding::ProcessTribe(){

	//checking if main building is damaged
	word MainID=0xFFFF;
	bool MainBusy=0;
	bool NeedActors=0;
	bool NeedRepair=0;
	CleanGroup(&CentralGroup);
	OneUnit OU;
	if(GetUnitInfo(&CentralGroup,0,&OU)){
		MainID=OU.Index;
		MainBusy=OU.Busy!=0;
	};
	if(UseLikeSkarb){
		if(MainID!=0xFFFF&&!AlreadyUsedLikeSkarb){
			int MAXR=0;
			if(OU.NI!=7){
				for(int i=0;i<6;i++){
					int R=GetResource(OU.NI,GOLD);
					SetResource(OU.NI,GOLD,R+DiploCost[i]);
					if(DiploCost[i]!=0)MAXR=DiploCost[i];
				};
				if(OU.NI==GetMyNation()){
					char cc[512];
					sprintf_s(cc,sizeof(cc),GetTextByID(FirstMeetID),MAXR);
					AssignHint(cc,300);
				};
				AlreadyUsedLikeSkarb=1;
			};
		};
		return;
	};
	word NMask=128;
	if(CounsilNI<7) NMask=(1<<CounsilNI);
	NMask|=GetNatNMASK(Owner);
	SetGroupNMASK(&Actors   ,NMask);
	SetGroupNMASK(&Defenders,NMask);
	SetGroupNMASK(&Repairers,NMask);
#ifdef AC_ADDON	
	SetGroupNMASK(&Friends,NMask);
#endif AC_ADDON
	SetAI_Guard(&Actors,1);
	SetAI_Guard(&CentralGroup,1);
	CleanGroup(&Actors);
	int DAC=NStartActors-GetNUnits(&Actors);
	if(DAC>0&&ActorsType!=-1){
		GAMEOBJ RTP;
		RTP.Type='UTYP';
		RTP.Serial=0;
		RTP.Index=ActorsType;
		if(!MainBusy){
			if(DAC>6)ProduceUnitFast(&CentralGroup,&RTP,&Actors,0);
			else ProduceUnitFast(&CentralGroup,&RTP,&Actors,7);
		};
		NeedActors=1;
	};
	int N=GetNUnits(&CentralGroup);
	if(N){
		if(RepType!=-1){
			if(MainID!=0xFFFF){
				GAMEOBJ RTP;
				if(OU.Life<OU.MaxLife)NeedRepair=1;
				if(NeedRepair){
					//need to repair
					RTP.Type='UTYP';
					RTP.Serial=0;
					RTP.Index=RepType;
					CleanGroup(&Repairers);
					if(GetNUnits(&Repairers)<MaxRep){
//						OneUnit OU;
						if(!MainBusy)ProduceUnitFast(&CentralGroup,&RTP,&Repairers,0);
					};
					if(GetRND(100)<30)SGP_RepairBuilding(&Repairers,0,MainID);
				};
				if(NeedActors||NeedRepair||GetNUnits(&Defenders))
					if(!AttackEnemyInZone2(&Actors,&VeryBigZone,7)){
						ProcessDance();
						ProcessDance();
					};
				if(NeedActors||NeedRepair){
					if(DefType!=-1){
						RTP.Type='UTYP';
						RTP.Serial=0;
						RTP.Index=DefType;
						CleanGroup(&Defenders);
						if(GetNUnits(&Defenders)<MaxDef){
							if(!MainBusy)ProduceUnitFast(&CentralGroup,&RTP,&Defenders,0);
						};
						if(!AttackEnemyInZone2(&Defenders,&VeryBigZone,7)){
							CopyUnits(&Defenders,&ComeInside,0,1000,1,1);
							ProcessDance();
						};
					};
					int ND=GetNUnits(&Defenders);
					for(int j=0;j<ND;j++){
						OneUnit OU;
						if(GetUnitInfo(&Defenders,j,&OU)){
							OU.AddDamage=AddDefAttack;
							OU.AddShield=AddDefShield;
							SetUnitInfo(&OU);
						};
					};
				}else{
					if(GetRND(100)<1){
						CopyUnits(&Repairers,&ComeInside,0,1000,1,1);
						CopyUnits(&Defenders,&ComeInside,0,1000,1,1);
						ProcessDance();
					};
				};
			}else{
				Owner=7;
				return;
			};
		};
	}else{
		Owner=7;
		return;
	};
	if(MainID!=0xFFFF){
		word* IDS;
		int N=GetInsideList(MainID,&IDS);
		if(N){
			word MID=IDS[0];
			if(MID!=0xFFFF){
				DieUnit(MID);
			};
		};
	};
	
	if(Busy){
		switch(ProcessID){
		case 1024+1://produce units
			PerformProduceUnitsCommend();
			break;
		case 1024+2:
			PerformRazvedka();
			break;
		case 1024+3:
			PerformFiring();
			break;
		case 1024+4:
			PerformStorms();
			break;
		case 1024+5:
			PerformKillers();
			break;
		case 1024+6:
			PerformTomahawks();
			break;
		};
	}else{
		//seeking for diplomat
		//AddUnitsInZoneToGroup(&VeryBigZone,&DestGroup,0xFF,DiplomatID);
		//int N=GetNUnits(&DestGroup);
//		OneUnit OU;
		//for(int p=0;p<N;p++){
		//	if(GetUnitInfo(&DestGroup,p,&OU)){
		//		OU.NMask|=(1<<Owner);
		//		SetUnitInfoNew(&OU);
		//	};
		//};
		if(DipProposition){
			//if(GetGlobalTime()-StartRelTime>500){
			//	DipProposition=0;
			//	CounsilNI=7;
			//};
			EraseGroup(&DestGroup);
			if(BigZone.Type){
				int Usage=DiplomatID;
#ifdef AC_ADDON
				if(EasyTalk) Usage=0;
#endif AC_ADDON
				AddUnitsInZoneToGroup(&BigZone,&DestGroup,0xFF,Usage);
				int N=GetNUnits(&DestGroup);
				if(!N)DipProposition=0;
			};
		}else{
			EraseGroup(&DestGroup);
			if(BigZone.Type){
				int Usage=DiplomatID;
#ifdef AC_ADDON
				if(EasyTalk) Usage=0;
#endif AC_ADDON
				AddUnitsInZoneToGroup(&BigZone,&DestGroup,0xFF,Usage);
				int N=GetNUnits(&DestGroup);
				OneUnit OU;
				if(!N)DipProposition=0;
				for(int p=0;p<N;p++){
					if(GetUnitInfo(&DestGroup,p,&OU)){
						if(OU.NI!=7&&OU.NI!=Owner){
							if(Owner==7){
								DipProposition=1;
								//CounsilNI=OU.NI;
								StartRelTime=GetGlobalTime();
								if(OU.NI==GetMyNation()){
									GAMEOBJ Zone;
									UnitsCenter(&Zone,&CentralGroup,512);
									SetStartPoint(&Zone);
									//now nobody owns this tribe
									DString DST;
									DST.ReadFromFile("Dip\\InviteMask.cml");
									DST.Replace("$CONTXT$",GetTextByID("$CONTXT$"));
									DST.Replace("$GPVOG$",gpPix);
									DST.Replace("$FRAME$",gpidx);
									DST.Replace("$PROPOSE$",GetTextByID(FirstMeetID));
									DST.Replace("$YESTXT$",GetTextByID("$YESTXT$"));
									DST.Replace("$NOTXT$",GetTextByID("$NOTXT$"));
									DST.Replace("$YESCODE$","%d&%d&%d",Index,OU.NI,OU.Index);
									DST.Replace("$NOCODE$","%d&%d&%d",Index,OU.NI,OU.Index);
									DST.WriteToFile("Dip\\tmp.cml");
									ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
									DIP->BusyPageIndex=-1;
								};
							}else{
								DipProposition=1;
								//CounsilNI=OU.NI;
								StartRelTime=GetGlobalTime();
								if(OU.NI==GetMyNation()){
									char tmp[128];
									GAMEOBJ Zone;
									UnitsCenter(&Zone,&CentralGroup,512);
									SetStartPoint(&Zone);
									//now nobody owns this tribe
									DString DST;
									DST.ReadFromFile("Dip\\ResellMask.cml");
									DST.Replace("$CONTXT$",GetTextByID("$CONTXT$"));
									DST.Replace("$GPVOG$",gpPix);
									DST.Replace("$FRAME$",gpidx);
									DST.Replace("$PROPOSE$",GetTextByID(ReSellID),GetMoneyString(ReSellCost,tmp));
									DST.Replace("$YESTXT$",GetTextByID("$YESTXT$"));
									DST.Replace("$NOTXT$",GetTextByID("$NOTXT$"));
									DST.Replace("$YESCODE$","%d&%d&%d",Index,OU.NI,OU.Index);
									DST.Replace("$NOCODE$","%d&%d&%d",Index,OU.NI,OU.Index);
									DST.WriteToFile("Dip\\tmp.cml");
									ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
									DIP->BusyPageIndex=-1;
								};
							};
						};
					};
				};
			};
		};
	};
};
extern DiplomacySystem DIPS;
void DIP_SimpleBuilding::CreateAbilityPage(){
	DString DST;
	DString One;
	if(Owner!=GetMyNation()){
		DST.ReadFromFile("Dip\\WaitMask.cml");
		DST.Replace("$CONTXT$",GetTextByID("$CONTXT$"));
		DST.Replace("$GPVOG$",gpPix);
		DST.Replace("$FRAME$",gpidx);
		DST.Replace("$PROPOSE$",GetTextByID("$DIPREBEL"));
		DST.WriteToFile("Dip\\tmp.cml");
		DIP->BusyPageIndex=-1;
		return;
	};
	if(Busy){
		DST.ReadFromFile("Dip\\WaitMask.cml");
		DST.Replace("$CONTXT$",GetTextByID("$CONTXT$"));
		DST.Replace("$GPVOG$",gpPix);
		DST.Replace("$FRAME$",gpidx);
		DST.Replace("$PROPOSE$",GetTextByID(CurrentStateMessage));
		DST.WriteToFile("Dip\\tmp.cml");
		DIP->BusyPageIndex=Index;
		return;
	};
	DST.ReadFromFile("Dip\\Propositions.cml");
	DST.Replace("$CONTXT$",GetTextByID("$CONTXT$"));
	DST.Replace("$GPVOG$",gpPix);
	DST.Replace("$FRAME$",gpidx);
	DST.Replace("$PROPOSE$",GetTextByID(CongratID));
	for(int i=0;i<NDipPar;i++){
		One.ReadFromFile("Dip\\PropString.cml");
		One.Replace("$IDX$",i+1);
		One.Replace("$IDXX$",i);
		char MSS[512];
		char STR[512];
		strcpy(MSS,GetTextByID(DIPPAR[i].MessageID));
		char* RESS[6]={"$W","$G","$S","$F","$I","$C"};
		for(int p=0;p<6;p++){
			char* s=strstr(MSS,RESS[p]);
			if(s){
				s[0]='%';
				s[1]='d';
				sprintf(STR,MSS,DIPPAR[i].Cost[p]);
				strcpy(MSS,STR);
			};
		};
		One.Replace("$PROPOSE$",MSS);
		One.Replace("$PERFORM$","%d&%d&%d",Index,i,GetMyNation());
		DST+=One;
	};
	DST.WriteToFile("Dip\\tmp.cml");
	DIP->BusyPageIndex=-1;
};
void CreateRadialPosition(int* x,int* y,int N,int R1,int R2,int Phase1,int Phase2){
	if(!N)return;
	for(int i=0;i<N;i++){
		int R,ph;
		if(i&1){
			R=R1;
			ph=Phase1;
		}else{
			R=R2;
			ph=Phase2;
		};
		x[i]=(GetTCos(ph+(i<<8)/N)*R)>>8;
		y[i]=(GetTSin(ph+(i<<8)/N)*R)>>8;
	};
};
void DIP_SimpleBuilding::ProcessDance(){
	int N=GetNUnits(&Actors);
	if(N){
		int x,y,R;
		if(GetZoneCoor(&DanceZone,&x,&y)){
			GetZoneR(&DanceZone,&R);
			int ang=256/N;
			bool busy=0;
			OneUnit OU;
			for(int i=0;i<N;i++){
				GetUnitInfo(&Actors,i,&OU);
				if(OU.Busy&&!OU.Attack)busy=1;
			};
			if(!busy){
				DanceStage++;
				int xi[512];
				int yi[512];
				int r1,r2,p1,p2;
				if(DanceStage==1){
					r1=R;r2=R;p1=0;p2=0;
				}else{
					int st=(DanceStage-1)%4;
					switch(st){
					case 0:
						r1=R-64;
						r2=R+64;
						p1=0;
						p2=0;
						break;
					case 1:
						r1=R-64;
						r2=R+64;
						p1=64;
						p2=-64;
						break;
					case 2:
						r1=R+64;
						r2=R-64;
						p1=0;
						p2=0;
						break;
					case 3:
						r1=R+64;
						r2=R-64;
						p1=-64;
						p2=64;
						break;
					};
				};
				CreateRadialPosition(xi,yi,N,r1,r2,p1,p2);
				for(int i=0;i<N;i++){
					GetUnitInfo(&Actors,i,&OU);
					if(OU.Index!=0xFFFF){
						if(!OU.Busy)OBJ_SendTo(OU.Index,xi[i]+x,yi[i]+y,128+16,0);
					};
				};
			};
		};
	};
};
void DIP_SimpleBuilding::ShowNoMoney(){
	DString DST;
	DST.ReadFromFile("Dip\\NoMoney.cml");
	DST.Replace("$CONTXT$",GetTextByID("$CONTXT$"));
	DST.Replace("$GPVOG$",gpPix);
	DST.Replace("$FRAME$",gpidx);
	DST.Replace("$PROPOSE$",GetTextByID("$NOMONEY$"));
	DST.Replace("$GOBACK$",GetTextByID("$GOBACK$"));
	DST.Replace("$TRIBEINFO$",Index);
	DST.WriteToFile("Dip\\tmp.cml");
	ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
	DIP->BusyPageIndex=-1;
};

int DIP_SimpleBuilding::GetSaveData(byte* buf,bool write){
	int sz=5;
	if(write){
		buf[0]=1;
	};
	if(write)memcpy(buf+sz,&Owner,((byte*)&DIP)-((byte*)&Owner));
	sz+=((byte*)&DIP)-((byte*)&Owner);
	if(write)memcpy(buf+sz,&NDipPar,((byte*)&StartRelTime)-((byte*)&NDipPar)+4);
	sz+=((byte*)&StartRelTime)-((byte*)&NDipPar)+4;
	if(NDipPar&&write)memcpy(buf+sz,DIPPAR,NDipPar*sizeof DipComParams);
	sz+=NDipPar*sizeof DipComParams;
	if(write)((DWORD*)(buf+1))[0]=sz;
	return sz;
};
void DIP_SimpleBuilding::SetSaveData(byte* buf,int size){
	int sz=0;
	memcpy(&Owner,buf+sz,((byte*)&DIP)-((byte*)&Owner));
	sz+=((byte*)&DIP)-((byte*)&Owner);
	memcpy(&NDipPar,buf+sz,((byte*)&StartRelTime)-((byte*)&NDipPar)+4);
	sz+=((byte*)&StartRelTime)-((byte*)&NDipPar)+4;
	if(NDipPar){
		DIPPAR=(DipComParams*)malloc(NDipPar*sizeof DipComParams);
        if (DIPPAR != 0) {
            memcpy(DIPPAR, buf + sz, NDipPar * sizeof DipComParams);
        }
	};
};
void DIP_SimpleBuilding::Init(){
	memset(this,0,((byte*)&DIP)-((byte*)&Owner));
	DIP=NULL;
    if (DIPPAR != 0) {
        memset(&DIPPAR, 0, ((byte*)&StartRelTime) - ((byte*)&DIPPAR) + 4);
    }
};
void DIP_SimpleBuilding::Free(){
	if(DIPPAR)free(DIPPAR);
	Init();
};
CEXPORT void CheckDipBuilding(byte NI,int Index){
	for(int i=0;i<DIPS.NDIPS;i++)if(DIPS.DIPS[i]->Owner==NI){
		OneUnit OU;
		if(GetUnitInfo(&DIPS.DIPS[i]->CentralGroup,0,&OU)&&OU.Index==Index){
			((DIP_SimpleBuilding*)DIPS.DIPS[i])->CreateAbilityPage();
			ActivateDipDialog("LW_new&|LW_file&Dip/tmp.cml");
		};
	};
};