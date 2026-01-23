CIMPORT void GoHomeAnyway();

extern int menu_x_off;
extern int menu_y_off;
extern int menu_hint_x;
extern int menu_hint_y;

int MM_ProcessMultiPlayer(){	
	GoHomeAnyway();
    if (!window_mode)
    {
        menu_hint_x = 18 + menu_x_off;
        menu_hint_y = 701 + menu_y_off;
    }
    else {
        menu_hint_x = 18;
        menu_hint_y = 701;
    }
	LocalGP BTNS("Interface\\Multi_Player");
	LocalGP HFONT("rom10");
	RLCFont hfnt(HFONT.GPID);
	hfnt.SetWhiteColor();

	//SQPicture MnPanel("Interface\\Background_Multi_Player.bmp");
	SQPicture MnPanel("Interface\\Background_Multi_Player.bmp");
	
#ifdef SCREENFIX
	DialogsSystem MMenu(menu_x_off, menu_y_off);
	MMenu.HintFont=&hfnt;
	MMenu.HintY= menu_hint_y;
	MMenu.HintX= menu_hint_x;
#else
    DialogsSystem MMenu(0, 0);
    MMenu.HintFont = &hfnt;
    MMenu.HintY = 18;
    MMenu.HintX = 701;
#endif

	int Dy=110;
	Picture* PIC=MMenu.addPicture(NULL,0,0,&MnPanel,&MnPanel,&MnPanel);
	GP_Button* DeathM=MMenu.addGP_Button(NULL,76,140+Dy,BTNS.GPID,0,1);
	DeathM->UserParam=1;
	DeathM->OnUserClick=&MMItemChoose;
	DeathM->Hint=GetTextByID("MDEATHM");
	DeathM->AssignSound(GETS("@MOUSESOUND"),MOUSE_SOUND);
	GP_Button* HistBatt=MMenu.addGP_Button(NULL,76,140+82+Dy,BTNS.GPID,2,3);
	HistBatt->UserParam=2;
	HistBatt->OnUserClick=&MMItemChoose;
	HistBatt->Hint=GetTextByID("MHISTBATT");
	HistBatt->AssignSound(GETS("@MOUSESOUND"),MOUSE_SOUND);
	GP_Button* Back=MMenu.addGP_Button(NULL,76,140+82*2+Dy,BTNS.GPID,4,5);
	Back->UserParam=5;
	Back->OnUserClick=&MMItemChoose;
	Back->Hint=GetTextByID("MBACK");
	Back->AssignSound(GETS("@MOUSESOUND"),MOUSE_SOUND);
	//SlowUnLoadPalette("2\\agew_1.pal");
	ItemChoose=-1;
	UnPress();
	Lpressed=0;
	LastKey=0;
	KeyPressed=0;
	int pp=1;
	do{
		ProcessMessages();
		if(KeyPressed&&LastKey==27){
			ItemChoose=5;
			KeyPressed=0;
		};
		MMenu.ProcessDialogs();
		MMenu.RefreshView();
		if(pp){
//			SlowLoadPalette("1\\agew_1.pal");
			pp=0;
		};
#ifdef TUTORIAL_DEMOVERSION
		if(ItemChoose==2)ItemChoose=-1;
#else
		if(ItemChoose==2&&!WARS.NWars)ItemChoose=-1;
#endif
	}while(ItemChoose==-1);
//	SlowUnLoadPalette("1\\agew_1.pal");
	if(ItemChoose==1){
		processMultiplayer();
		if(TOTALEXIT)return mcmCancel;
	};
	if(ItemChoose==2){
		if(WARS.NWars)processBattleMultiplayer();
		//ProcessWars();
	};
	return ItemChoose;
};


bool ProcessNewInternetLogin();

extern char ACCESS[16];

bool processMultiplayer(){
	GoHomeAnyway();
	byte AddrBuf[128];
	memset(AddrBuf,0,128);
	int crs=0;
RetryConn:;
	if(IEMMOD)goto REINCONN;
	crs=MPL_ChooseConnection();
	if(TOTALEXIT)return 0;
	if(crs==mcmCancel)return 0;
	
	if(CurProtocol==3){
		if(!ProcessNewInternetLogin())return false;
REINCONN:;
		int r=ProcessInternetConnection(1);
		if(!r)return 0;
		if(r==2)crs=10;//Join
		if(r==1)crs=11;//Host DM
		if(r==3)crs=13;//Host BT
	}else{
		if(!CommEnterName())return 0;
	};
	DoNewInet=0;
	if(CurProtocol>2)DoNewInet=1;
	if(!DoNewInet){
		if(!lpDirectPlay3A){
			CreateMultiplaterInterface();
			if(!lpDirectPlay3A)return 0;
		};
		LPDIRECTPLAYLOBBYA	lpDPlayLobbyA = NULL;
		LPDIRECTPLAYLOBBY2A	lpDPlayLobby2A = NULL;
		if FAILED(DirectPlayLobbyCreate(NULL, &lpDPlayLobbyA, NULL, NULL, 0)) return 0;
			// get ANSI DirectPlayLobby2 interface
		HRESULT hr = lpDPlayLobbyA->QueryInterface(IID_IDirectPlayLobby2A, (LPVOID *) &lpDPlayLobby2A);
		if FAILED(hr)return 0;
				// don't need DirectPlayLobby interface anymore
		lpDPlayLobbyA->Release();
		lpDPlayLobbyA = NULL;
		DPCOMPOUNDADDRESSELEMENT	addressElements[3];
			DWORD sz=128;
		char* cc="";
		switch(CurProtocol){
		case 0://IPX
			addressElements[0].guidDataType = DPAID_ServiceProvider;
			addressElements[0].dwDataSize = sizeof(GUID);
			addressElements[0].lpData = (LPVOID) &DPSPGUID_IPX;
			lpDPlayLobby2A->CreateCompoundAddress(addressElements,1,AddrBuf,&sz);
			/*
			memcpy(AddrBuf,&DPAID_TotalSize,16);
			AddrBuf[16]=56;
			memcpy(AddrBuf+20,&DPAID_ServiceProvider,16);
			AddrBuf[36]=16;
			memcpy(AddrBuf+40,&DPSPGUID_IPX,16);
			*/
			break;
		case 1://TCP/IP
			IPADDR[0]=0;
		case 3:
		case 2:
			addressElements[0].guidDataType = DPAID_ServiceProvider;
			addressElements[0].dwDataSize = sizeof(GUID);
			addressElements[0].lpData = (LPVOID) &DPSPGUID_TCPIP;
			addressElements[1].guidDataType = DPAID_INet;
			addressElements[1].dwDataSize = strlen(IPADDR)+1;
			addressElements[1].lpData = (LPVOID) IPADDR;
			lpDPlayLobby2A->CreateCompoundAddress(addressElements,2,AddrBuf,&sz);
			/*
			memcpy(AddrBuf,&DPAID_ServiceProvider,16);
			AddrBuf[16]=16;
			memcpy(AddrBuf+20,&DPSPGUID_TCPIP,16);
			memcpy(AddrBuf+36,&DPAID_INet,16);
			AddrBuf[40]=1;
			AddrBuf[41]=0;
			*/
			break;
		};
		lpDPlayLobby2A->Release();
		CloseMPL();
		CreateMultiplaterInterface();
		HRESULT HR=lpDirectPlay3A->InitializeConnection(AddrBuf,0);
		if(FAILED(HR))goto RetryConn;
	}else{
		CloseMPL();
		CreateMultiplaterInterface();
	};
	switch(crs){
	case mcmHost:
		if(CreateNamedSession(PlName,0,GMMAXPL))WaitingHostGame(0);
		break;
	case mcmJoin:
		MPL_JoinGame(0);
		break;
	case 11://Inet Host(Deathmatch)
		PlayerMenuMode=1;
			if(CreateSession(GlobalRIF.Name,GlobalRIF.Nick, 0,DoNewInet,GlobalRIF.MaxPlayers)){
				NeedToPerformGSC_Report=1;
                udp_hole_puncher.Init(GlobalRIF.udp_server, GlobalRIF.port, GlobalRIF.udp_interval,GlobalRIF.player_id, ACCESS);
				WaitingHostGame(0);
				NeedToPerformGSC_Report=0;
				if(PlayerMenuMode==1){
					//need to leave the room there
					LeaveGSCRoom();
					goto REINCONN;
				}else{
					char* PLAYERS[8];
					int Profiles[8];
					char NAT[8][32];
					char* Nations[8];
					int Teams[8];
					int Colors[8];
					for(int i=0;i<NPlayers;i++){
						PLAYERS[i]=PINFO[i].name;
						sprintf(NAT[i],"%d",PINFO[i].NationID+48);
						Nations[i]=NAT[i];
						Profiles[i]=PINFO[i].ProfileID;
						Teams[i]=PINFO[i].GroupID;
						Colors[i]=PINFO[i].ColorID;
					};
					StartGSCGame("",PINFO[0].MapName,NPlayers,Profiles,Nations,Teams,Colors);
					NeedToReportInGameStats=1;
					LastTimeReport_tmtmt=0;
				};
			}else{ 
				LeaveGSCRoom();
				goto REINCONN;
			};
		break;
	case 13:
        PlayerMenuMode = 1;
        goto REINCONN;
        break;
	case 10://Inet Join(Deathmatch)
			PlayerMenuMode=1;
			strcpy(IPADDR,GlobalRIF.RoomIP);
			if(!FindSessionAndJoin(ROOMNAMETOCONNECT, GlobalRIF.Nick, DoNewInet, GlobalRIF.port)){
				LeaveGSCRoom();
				WaitWithMessage(GetTextByID("ICUNJ"));
			}else{
				WaitingJoinGame(GMTYPE);
			};
			if(PlayerMenuMode==1){
				LeaveGSCRoom();
				goto REINCONN;
			}else{
				char* PLAYERS[8];
				int Profiles[8];
				char NAT[8][32];
				char* Nations[8];
				int Teams[8];
				int Colors[8];
				for(int i=0;i<NPlayers;i++){
					PLAYERS[i]=PINFO[i].name;
					sprintf(NAT[i],"%d",PINFO[i].NationID+48);
					Nations[i]=NAT[i];
					Profiles[i]=PINFO[i].ProfileID;
					Teams[i]=PINFO[i].GroupID;
					Colors[i]=PINFO[i].ColorID;
				};
				StartGSCGame("",PINFO[0].MapName,NPlayers,Profiles,Nations,Teams,Colors);
				NeedToReportInGameStats=1;
				LastTimeReport_tmtmt=0;
			};
		break;
	};
	return 1;
};


void processBattleMultiplayer(){
TryConnection:;
	byte AddrBuf[128];
	memset(AddrBuf,0,128);
	int crs;
	crs=MPL_ChooseConnection();
	if(TOTALEXIT)return;
	if(crs==mcmCancel)return;
	if(CurProtocol==3){
		IEMMOD=1;
		processMultiplayer();
		if(TOTALEXIT)return;
		IEMMOD=0;
		return;
	};
	if(!CommEnterName())return;
	
	//if(crs==mcmHost){
	//	BTLID=ProcessWars();
	//};

	if(BTLID==-1)goto TryConnection;
	if(!lpDirectPlay3A){
		DoNewInet=0;
		CreateMultiplaterInterface();
		if(!lpDirectPlay3A)return;
	};

	LPDIRECTPLAYLOBBYA	lpDPlayLobbyA = NULL;
	LPDIRECTPLAYLOBBY2A	lpDPlayLobby2A = NULL;
	if FAILED(DirectPlayLobbyCreate(NULL, &lpDPlayLobbyA, NULL, NULL, 0)) return;
	// get ANSI DirectPlayLobby2 interface
	HRESULT hr = lpDPlayLobbyA->QueryInterface(IID_IDirectPlayLobby2A, (LPVOID *) &lpDPlayLobby2A);
	if FAILED(hr)return;

	// don't need DirectPlayLobby interface anymore
	lpDPlayLobbyA->Release();
	lpDPlayLobbyA = NULL;
	DPCOMPOUNDADDRESSELEMENT	addressElements[3];
	DWORD sz=128;
	char* cc="";
	switch(CurProtocol){
	case 0://IPX
		addressElements[0].guidDataType = DPAID_ServiceProvider;
		addressElements[0].dwDataSize = sizeof(GUID);
		addressElements[0].lpData = (LPVOID) &DPSPGUID_IPX;
		lpDPlayLobby2A->CreateCompoundAddress(addressElements,1,AddrBuf,&sz);
		break;
	case 1://TCP/IP
		IPADDR[0]=0;
	case 3:
	case 2:
		addressElements[0].guidDataType = DPAID_ServiceProvider;
		addressElements[0].dwDataSize = sizeof(GUID);
		addressElements[0].lpData = (LPVOID) &DPSPGUID_TCPIP;
		addressElements[1].guidDataType = DPAID_INet;
		addressElements[1].dwDataSize = strlen(IPADDR)+1;
		addressElements[1].lpData = (LPVOID) IPADDR;
		lpDPlayLobby2A->CreateCompoundAddress(addressElements,2,AddrBuf,&sz);
		break;

	};
	lpDPlayLobby2A->Release();
	CloseMPL();
	CreateMultiplaterInterface();
	if FAILED(lpDirectPlay3A->InitializeConnection(AddrBuf
		/*lplpConnectionBuffer[CurProtocol]*/,0)) return;
	switch(crs){
	case mcmHost:
		if(CreateNamedSession(PlName,BTLID+1,2))WaitingHostGame(BTLID+1);
		break;
	case mcmJoin:
		MPL_JoinGame(BTLID+1);
		break;
	};
};