UniversalButton* IPX;
UniversalButton* TCP;
UniversalButton* TCPDIRECT;
UniversalButton* INET;

int SPX = 444;
int SPY = 336;
int SPW = 470;
int SPdX = 12;

bool MMChooseNameEx(SimpleDialog* SD){
	int x1 = SPX+SPW;
	IPX->x=SPX+menu_x_off;
	IPX->x1=x1 + menu_x_off;
	TCP->x=SPX + menu_x_off;
	TCP->x1=x1 + menu_x_off;
	TCPDIRECT->x=SPX + menu_x_off;
	TCPDIRECT->x1=x1 + menu_x_off;
	INET->x=SPX + menu_x_off;
	INET->x1=x1 + menu_x_off;

	SD->x=SPX-SPdX + menu_x_off;
	SD->x1=SPX+SPW+SPdX + menu_x_off;
	
	NameChoose=SD->UserParam;
	Lpressed=false;
	return false;
};

int MPL_ChooseConnection(){
	if(InternetGame) return 0;
	LoadFog(1);	
	
	LocalGP BTN("Interf2\\Buttons");	
	LocalGP TB2("Interf2\\TextBox2");
	int Set2[30] = {5,6,7,8,9, 5,6,7,8,9, 5,6,7,8,9,
				    5,6,7,8,9, 5,6,7,8,9, 5,6,7,8,9};
	LocalGP TB3("Interf2\\TextBox3");
	int Set3[30] = {0,1,2,3,4, 5,6,7,8,9, 5,6,7,8,9,
				    0,1,2,3,4, 5,6,7,8,9, 5,6,7,8,9};

	LocalGP HFONT("rom10");
	RLCFont hfnt(HFONT.GPID);

	LocalGP BTNS("Interface\\Type_connection");
	//SQPicture Back("Interface\\Background_Type_Connection.bmp");
	SQPicture Back("Interf2\\Background_Type_Connection.bmp");

	// Fonts
	RLCFont* FScreen = &fon40y5;
	RLCFont* FWindow = &fon16y5;
	RLCFont* FButton = &fon18y5;
	RLCFont* FWhite = &fon18w;
	RLCFont* FYellow = &fon18y3;
	RLCFont* FHint = &fon13y2;

//#define FRANCEVERSION
#ifndef FRANCEVERSION
	SQPicture IBACK("Interface\\Background_Type_Connection_Crop.bmp");
#endif
	
	DialogsSystem MENU(menu_x_off, menu_y_off);
	//int dx=((RealLx-GPS.GetGPWidth(BTNS.GPID,0))>>1)-85;
	//int dy=(RealLx-GPS.GetGPWidth(BTNS.GPID,0))>>1;
	MENU.addPicture(NULL,0,0,&Back,&Back,&Back);
#ifndef FRANCEVERSION
	//MENU.addPicture(NULL,72,222,&IBACK,&IBACK,&IBACK);
#endif
	//char* IPTXT=GetTextByID("IP:");
	//int L=GetRLCStrWidth(IPTXT,&YellowFont);
	//MENU.addGPPicture(NULL,dx,dy,BTNS.GPID,0);
	int dy1=0;
	int dy2=0;
	int dy3=0;
#ifdef FRANCEVERSION
	//dy1=-2;
	//dy2=24;
	//dy3=-3;
#endif

	// Hint
    if (!window_mode)
    {
        menu_hint_x = 513 + menu_x_off;
        menu_hint_y = 745 + menu_y_off;
    }
    else {
        menu_hint_x = 513;
        menu_hint_y = 745;
    }

	MENU.HintFont=FHint;
	MENU.HintX= menu_hint_x;
	MENU.HintY= menu_hint_y;

	IPX=MENU.addUniversalButton(SPX,SPY,SPW,GetTextByID("INTF_PROT_B0"),TB3.GPID,Set3,0,0,0,FWhite,FWhite,FYellow);
	TCP=MENU.addUniversalButton(SPX,SPY+26,SPW,GetTextByID("INTF_PROT_B1"),TB3.GPID,Set3,0,0,0,FWhite,FWhite,FYellow);
	TCPDIRECT=MENU.addUniversalButton(SPX,SPY+26*2,SPW,GetTextByID("INTF_PROT_B2"),TB3.GPID,Set3,0,0,0,FWhite,FWhite,FYellow);
	INET=MENU.addUniversalButton(SPX,SPY+26*3,SPW,GetTextByID("INTF_PROT_B3"),TB3.GPID,Set3,0,0,0,FWhite,FWhite,FYellow);

	IPX->Hint=GetTextByID("INTF5_HINT20");
	TCP->Hint=GetTextByID("INTF5_HINT20");
	TCPDIRECT->Hint=GetTextByID("INTF5_HINT20");
	INET->Hint=GetTextByID("INTF5_HINT20");

	IPX->FontDy-=1;
	TCP->FontDy-=1;
	TCPDIRECT->FontDy-=1;
	INET->FontDy-=1;

	int x1 = IPX->x1;

	IPX->UserParam=0;
	IPX->OnUserClick=&MMChooseNameEx;
	IPX->Tiling=0;
	IPX->ActiveDY-=4;	
	
	TCP->UserParam=1;
	TCP->OnUserClick=&MMChooseNameEx;
	TCP->Tiling=0;
	TCP->ActiveDY-=1;
	
	TCPDIRECT->UserParam=2;
	TCPDIRECT->OnUserClick=&MMChooseNameEx;
	TCPDIRECT->Tiling=0;
	TCPDIRECT->ActiveDY-=1;
	
	INET->UserParam=3;
	INET->OnUserClick=&MMChooseNameEx;
	INET->Tiling=0;
	INET->ActiveDY-=1;
	INET->Visible=0;
	INET->Enabled=0;

	

#ifdef FRANCEVERSION
	GP_Button* GOA=MENU.addGP_Button(NULL,97,279+52+26-6,BTNS.GPID,20,22);
	GOA->UserParam=100;
	GOA->OnUserClick=&MMChooseName;
#endif

	int ButY=501;
	GP_TextButton* Join=MENU.addGP_TextButton(NULL,447,ButY,GetTextByID("INTF_JOIN"),BTN.GPID,1,FButton,FButton);
	GP_TextButton* Create=MENU.addGP_TextButton(NULL,614,ButY,GetTextByID("INTF_CREATE"),BTN.GPID,1,FButton,FButton);
	GP_TextButton* Cancel=MENU.addGP_TextButton(NULL,781,ButY,GetTextByID("INTF_CANCEL"),BTN.GPID,1,FButton,FButton);	
	
	Join->Hint=GetTextByID("INTF5_HINT18");
	Create->Hint=GetTextByID("INTF5_HINT17");
	Cancel->Hint=GetTextByID("INTF5_HINT19");

	Join->FontDy -= 1;
	Create->FontDy -= 1;
	Cancel->FontDy -= 1;

	Join->UserParam=mcmJoin;	
	Create->UserParam=mcmHost;
	Cancel->UserParam=mcmCancel;

	Join->OnUserClick=&MMItemChoose;	
	Create->OnUserClick=&MMItemChoose;
	Cancel->OnUserClick=&MMItemChoose;

	// Text
	TextButton* T0=MENU.addTextButton(NULL,512,18,GetTextByID("INTF_PROT_T0"),FScreen,FScreen,FScreen,1);
	TextButton* T1=MENU.addTextButton(NULL,680,291,GetTextByID("INTF_PROT_T1"),FWindow,FWindow,FWindow,1);

	TextButton* IP=MENU.addTextButton(NULL,447,451,GetTextByID("INTF_PROT_IP"),FYellow,FYellow,FYellow,0);

	UniversalButton* PIP=MENU.addUniversalButton(695,449,221,"",TB2.GPID,Set2,0,0,0,FWhite,FWhite,FYellow);
	PIP->Enabled=0;
	InputBox* IBIP=MENU.addInputBox(NULL,700,448,IPADDR,64,200,26,FYellow,FWhite);
	//IBIP->

	ItemChoose=-1;	
	CurProtocol=1;
	switch(CurProtocol){
		case 0: MMChooseNameEx(IPX); IPX->State=1; break;
		case 1: MMChooseNameEx(TCP); TCP->State=1; break;
		case 2: MMChooseNameEx(TCPDIRECT); TCPDIRECT->State=1; break;
		//case 3: MMChooseNameEx(INET); INET->State=1; break;
		default: MMChooseNameEx(TCP); TCP->State=1;		
	};

	int pp=1;
	
	StartMixing();
	do{
		
		ProcessMessages();
		MENU.ProcessDialogs();
		ProcessMixing();
		MENU.RefreshView();
		
		if(NameChoose==0){
			CurProtocol=0;
		}
		if(NameChoose==1){
			CurProtocol=1;
		}
		if(NameChoose==2){
			CurProtocol=2;
			PIP->Visible=0;
			IBIP->Enabled=1;
			IBIP->Active=1;			
		}else{
			PIP->Visible=1;
			IBIP->Enabled=0;
		};
		if(NameChoose==3){
			CurProtocol=3;
		}

#ifdef FRANCEVERSION
		if(NameChoose==100){
			//GOA->PassiveFrame=21;
			CurProtocol=100;
		}else{
			//GOA->PassiveFrame=22;
			GOA->State=0;
		};
#endif
		if(pp){
//			SlowLoadPalette("1\\agew_1.pal");
			pp=0;
		};
	}while(ItemChoose==-1);
	if(CurProtocol==100&&ItemChoose!=mcmCancel){
		TOTALEXIT=1;
		CurProtocol=0;
		ItemChoose=mcmCancel;
	};
//	SlowUnLoadPalette("1\\agew_1.pal");
	return ItemChoose;
};