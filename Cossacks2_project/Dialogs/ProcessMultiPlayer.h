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
	
#ifndef STEAM
	// ============================================================================
	// DIRECTPLAY VERSION (Original)
	// ============================================================================
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

#else
	// ============================================================================
	// STEAM VERSION (New)
	// ============================================================================
	
	int crs=0;
RetryConn_Steam:;
	if(IEMMOD)goto REINCONN_Steam;
	
	crs = MPL_ChooseConnection();
	if(TOTALEXIT) return 0;
	if(crs == mcmCancel) return 0;
	
	// Handle internet mode (protocol 3 = new internet)
	if(CurProtocol == 3) {
		if(!ProcessNewInternetLogin()) return false;
REINCONN_Steam:;
		int r = ProcessInternetConnection(1);
		if(!r) return 0;
		if(r == 2) crs = 10;  // Join
		if(r == 1) crs = 11;  // Host Deathmatch
		if(r == 3) crs = 13;  // Host Battle
	} else {
		if(!CommEnterName()) return 0;
	}
	
	// With Steam, always use new internet protocol
	DoNewInet = 1;
    extern bool IPCORE_INIT;
    extern bool NETWORK_INIT;
	// Initialize Steam networking if needed
	if(!IPCORE_INIT) {
		CloseMPL();
		CreateMultiplaterInterface();
		if(!NETWORK_INIT) goto RetryConn_Steam;
	}
	
	// No DirectPlay address setup needed with Steam!
	// Steam handles all network configuration automatically
	
	switch(crs) {
	case mcmHost:
		// Host a regular game (LAN/local)
		if(CreateNamedSession(PlName, 0, GMMAXPL)) {
			WaitingHostGame(0);
		}
		break;
		
	case mcmJoin:
		// Join a regular game
		MPL_JoinGame(0);
		break;
		
	case 11: // Internet Host (Deathmatch)
		PlayerMenuMode = 1;
		
		if(CreateSession(GlobalRIF.Name, GlobalRIF.Nick, 0, DoNewInet, GlobalRIF.MaxPlayers)) {
			NeedToPerformGSC_Report = 1;
			
			// NO HOLE PUNCHING NEEDED WITH STEAM!
			// Steam handles all NAT traversal automatically
			// udp_hole_puncher.Init() is not needed
			
			WaitingHostGame(0);
			NeedToPerformGSC_Report = 0;
			
			if(PlayerMenuMode == 1) {
				// Need to leave the room
				LeaveGSCRoom();
				goto REINCONN_Steam;
			} else {
				// Game started successfully
				char* PLAYERS[8];
				int Profiles[8];
				char NAT[8][32];
				char* Nations[8];
				int Teams[8];
				int Colors[8];
				
				for(int i = 0; i < NPlayers; i++) {
					PLAYERS[i] = PINFO[i].name;
					sprintf(NAT[i], "%d", PINFO[i].NationID + 48);
					Nations[i] = NAT[i];
					Profiles[i] = PINFO[i].ProfileID;
					Teams[i] = PINFO[i].GroupID;
					Colors[i] = PINFO[i].ColorID;
				}
				
				StartGSCGame("", PINFO[0].MapName, NPlayers, Profiles, Nations, Teams, Colors);
				NeedToReportInGameStats = 1;
				LastTimeReport_tmtmt = 0;
			}
		} else {
			LeaveGSCRoom();
			goto REINCONN_Steam;
		}
		break;
		
	case 13: // Host Battle
		PlayerMenuMode = 1;
		goto REINCONN_Steam;
		break;
		
	case 10: // Internet Join (Deathmatch)
		PlayerMenuMode = 1;
		
		// With Steam, IPADDR contains the lobby ID instead of IP address
		// Format: "lobby_123456789012345"
		strcpy(IPADDR, GlobalRIF.RoomIP);
		
		if(!FindSessionAndJoin(ROOMNAMETOCONNECT, GlobalRIF.Nick, DoNewInet, GlobalRIF.port)) {
			LeaveGSCRoom();
			WaitWithMessage(GetTextByID("ICUNJ"));
		} else {
			WaitingJoinGame(GMTYPE);
		}
		
		if(PlayerMenuMode == 1) {
			LeaveGSCRoom();
			goto REINCONN_Steam;
		} else {
			// Game joined successfully
			char* PLAYERS[8];
			int Profiles[8];
			char NAT[8][32];
			char* Nations[8];
			int Teams[8];
			int Colors[8];
			
			for(int i = 0; i < NPlayers; i++) {
				PLAYERS[i] = PINFO[i].name;
				sprintf(NAT[i], "%d", PINFO[i].NationID + 48);
				Nations[i] = NAT[i];
				Profiles[i] = PINFO[i].ProfileID;
				Teams[i] = PINFO[i].GroupID;
				Colors[i] = PINFO[i].ColorID;
			}
			
			StartGSCGame("", PINFO[0].MapName, NPlayers, Profiles, Nations, Teams, Colors);
			NeedToReportInGameStats = 1;
			LastTimeReport_tmtmt = 0;
		}
		break;
	}
	
	return 1;

#endif // USE_STEAM_NETWORKING
}

void processBattleMultiplayer(){
TryConnection:;

#ifndef STEAM
	// ============================================================================
	// DIRECTPLAY VERSION (Original)
	// ============================================================================
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
	if FAILED(lpDirectPlay3A->InitializeConnection(AddrBuf,0)) return;
	switch(crs){
	case mcmHost:
		if(CreateNamedSession(PlName,BTLID+1,2))WaitingHostGame(BTLID+1);
		break;
	case mcmJoin:
		MPL_JoinGame(BTLID+1);
		break;
	};

#else
	// ============================================================================
	// STEAM VERSION (New)
	// ============================================================================
	
	int crs;
	crs = MPL_ChooseConnection();
	if (TOTALEXIT) return;
	if (crs == mcmCancel) return;
	
	// Special mode handling (if CurProtocol == 3, redirect to regular multiplayer)
	if (CurProtocol == 3) {
		IEMMOD = 1;
		processMultiplayer();
		if (TOTALEXIT) return;
		IEMMOD = 0;
		return;
	}
    extern bool IPCORE_INIT;
    extern bool NETWORK_INIT;
	// Get player name
	if (!CommEnterName()) return;
	
	// Check battle ID
	if (BTLID == -1) goto TryConnection;
	
	// Initialize Steam networking if not already done
	if (!IPCORE_INIT) {
		DoNewInet = 1;  // Force new internet protocol
		CreateMultiplaterInterface();
		if (!NETWORK_INIT) return;
	}
	
	// With Steam, we don't need protocol selection or address setup
	// Steam handles all network connectivity automatically
	
	// CurProtocol is ignored with Steam - it always uses Steam P2P
	// IPADDR is only used if we need to join a specific lobby by ID
	
	switch (crs) {
	case mcmHost:
		// Create a battle game (2 players, battle mode)
		{
			char sessionName[128];
			sprintf_s(sessionName, sizeof(sessionName), "[BATTLE]_%s", PlName);
			
			if (CreateNamedSession(PlName, BTLID + 1, 2)) {
				// Session created successfully
				WaitingHostGame(BTLID + 1);
			}
		}
		break;
		
	case mcmJoin:
		// Join a battle game
		MPL_JoinGame(BTLID + 1);
		break;
	}

#endif // USE_STEAM_NETWORKING
}

// ============================================================================
// Helper function for Steam battle game creation
// ============================================================================

#ifdef STEAM

// This replaces the DirectPlay address setup for Steam
bool Steam_SetupBattleConnection(int battleID)
{
	// With Steam, no address setup is needed
	// Steam P2P connection is established automatically through the lobby
	
	// Just verify Steam is ready
	if (!SteamUser() || !SteamMatchmaking() || !SteamNetworking())
	{
		return false;
	}
	
	// Set battle-specific rich presence
	char battleInfo[64];
	sprintf_s(battleInfo, sizeof(battleInfo), "Battle %d", battleID);
	SteamFriends()->SetRichPresence("status", battleInfo);
	
	return true;
}

#endif // USE_STEAM_NETWORKING