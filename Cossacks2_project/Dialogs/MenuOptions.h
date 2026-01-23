bool ProcessMenuOptions(){
	LoadFog(1);
//	LoadPalette("1\\agew_1.pal");
	
	// Controls
	LocalGP BTN("Interf2\\Buttons");
	LocalGP CMB("Interf2\\Combo");
	LocalGP SCR("Interf2\\Scroll");
	LocalGP CHK("Interf2\\Checker");
	LocalGP TB2("Interf2\\Textbox2");

	LocalGP BTNS("Interf2\\Options");
	
	// Fonts
	RLCFont* FScreen = &fon40y5;
	RLCFont* FWindow = &fon16y5;
	RLCFont* FButton = &fon18y5;
	RLCFont* FWhite = &fon18w;
	RLCFont* FYellow = &fon18y3;
	RLCFont* FHint = &fon13y2;
	
	/*
	int GPID;
	GPID=GPS.PreLoadGPImage("FontM18");
	RLCFont MainWhite(GPID);
	MainWhite.SetColorTable(1);
	MainWhite.SetStdShadow();
	RLCFont MainYellow(GPID);
	MainYellow.SetColorTable(3);
	*/

	// Main menu	
#ifdef SCREENFIX
	DialogsSystem MMenu(menu_x_off, menu_y_off);
#else
    DialogsSystem MMenu(0, 0);
#endif
    if (!window_mode)
    {
        menu_hint_x = 513 + menu_x_off;
        menu_hint_y = 745 + menu_y_off;
    }
    else {
        menu_hint_x = 513;
        menu_hint_y = 745;
    }
	// Hint
	MMenu.HintFont=FHint;
#ifdef SCREENFIX
    MMenu.HintY = menu_hint_y;
    MMenu.HintX = menu_hint_x;
#else
    MMenu.HintX = 513;
    MMenu.HintY = 745;
#endif
	
	//int dx=((RealLx-GPS.GetGPWidth(BTNS.GPID,1))>>1)-85-125;
	//int dy=(RealLy-GPS.GetGPHeight(BTNS.GPID,1))>>1;
	//MMenu.addGPPicture(NULL,dx,dy,BTNS.GPID,0);
	//MMenu.addGPPicture(NULL,dx,dy,BTNS.GPID,1);
	
	// Background	
	SQPicture MnPanel("Interf2\\Background_options.bmp");
	Picture* PIC=MMenu.addPicture(NULL,0,0,&MnPanel,&MnPanel,&MnPanel);
	
	// Text
	TextButton* T0=MMenu.addTextButton(NULL,512,18,GetTextByID("INTF_OPT_TITLE0"),FScreen,FScreen,FScreen,1);
	TextButton* T1=MMenu.addTextButton(NULL,680,217,GetTextByID("INTF_OPT_TITLE1"),FWindow,FWindow,FWindow,1);	
	
	int SecX = 443;
	int SecDx = 27;
	int SecY = 258;
	TextButton* VO=MMenu.addTextButton(NULL,SecX,SecY,GetTextByID("INTF_OPT_VO"),FWhite,FWhite,FWhite,0);
	TextButton* AO=MMenu.addTextButton(NULL,SecX,SecY+SecDx*2,GetTextByID("INTF_OPT_AO"),FWhite,FWhite,FWhite,0);
	TextButton* GO=MMenu.addTextButton(NULL,SecX,SecY+SecDx*6,GetTextByID("INTF_OPT_GO"),FWhite,FWhite,FWhite,0);
	
	int SubSecX = SecX+10;
	TextButton* VO0=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*1,GetTextByID("INTF_OPT_VO_0"),FYellow,FYellow,FYellow,0);
	TextButton* AO0=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*3,GetTextByID("INTF_OPT_AO_0"),FYellow,FYellow,FYellow,0);
	TextButton* AO1=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*4,GetTextByID("INTF_OPT_AO_1"),FYellow,FYellow,FYellow,0);
	TextButton* AO2=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*5,GetTextByID("INTF_OPT_AO_2"),FYellow,FYellow,FYellow,0);
	TextButton* GO0=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*7,GetTextByID("INTF_OPT_GO_0"),FYellow,FYellow,FYellow,0);
	TextButton* GO1=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*8,GetTextByID("INTF_OPT_GO_1"),FYellow,FYellow,FYellow,0);
	TextButton* GO2=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*9,GetTextByID("INTF_OPT_GO_2"),FYellow,FYellow,FYellow,0);
	TextButton* GO3=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*10,GetTextByID("INTF_OPT_GO_3"),FYellow,FYellow,FYellow,0);
	
	// Buttons
	GP_TextButton* OkBtn=MMenu.addGP_TextButton(NULL,472,577,GetTextByID("INTF_ACCEPT"),BTN.GPID,0,FButton,FButton);
	GP_TextButton* ExitBtn=MMenu.addGP_TextButton(NULL,704,577,GetTextByID("INTF_CANCEL"),BTN.GPID,0,FButton,FButton);

	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	OkBtn->Hint=GetTextByID("INTF5_HINT0");
	
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	ExitBtn->Hint=GetTextByID("INTF5_HINT1");

	OkBtn->FontDy -= 1;
	ExitBtn->FontDy -= 1;

#ifndef SPEEDFIX
    if (FPSTime > 100)FPSTime = 100;
    if (FPSTime < 0)FPSTime = 0;
#endif
	//game speed (#3)
	if(ScrollSpeed<2)ScrollSpeed=2;
	int X0=619+2;
	int LX=290+2;
	int DY=10;

	VScrollBar* SVL=MMenu.addGP_ScrollBarL(NULL,X0,366+DY,100,WarSound,SCR.GPID,0,LX,12,0,0);
	VScrollBar* MVL=MMenu.addGP_ScrollBarL(NULL,X0,392+DY,100,MidiSound,SCR.GPID,0,LX,12,0,0); //music volume (#2)
	VScrollBar* SSP=MMenu.addGP_ScrollBarL(NULL,X0,474+DY,8,ScrollSpeed-2,SCR.GPID,0,LX,12,0,0); //sound volume (#1)		
#ifndef SPEEDFIX	
    VScrollBar* HB = MMenu.addGP_ScrollBarL(NULL, X0, 500 + DY, 100, 100 - FPSTime, SCR.GPID, 0, LX, 12, 0, 0);
#endif
	
	SVL->Hint=GetTextByID("INTF5_HINT4");
	MVL->Hint=GetTextByID("INTF5_HINT5");
	SSP->Hint=GetTextByID("INTF5_HINT7");
#ifndef SPEEDFIX
    HB->Hint = GetTextByID("INTF5_HINT8");
#endif

	ComboBox* VMode=MMenu.addGP_ComboBox(NULL,619,284,CMB.GPID,0,9,0,FWhite,FYellow,NULL);
	ComboBox* FMode=MMenu.addGP_ComboBox(NULL,619,448,CMB.GPID,0,9,0,FWhite,FYellow,NULL);
	ComboBox* MUS=MMenu.addGP_ComboBox(NULL,619,340,CMB.GPID,0,9,0,FWhite,FYellow,NULL);

	VMode->Hint=GetTextByID("INTF5_HINT2");
	FMode->Hint=GetTextByID("INTF5_HINT6");
	MUS->Hint=GetTextByID("INTF5_HINT3");

	MidiSound=GetCDVolume();

	//VMode->CurLine=100;
	VMode->Center-=5;
	VMode->FontDy-=2;
	VMode->OneDy-=1;
	//VMode->FontDx-=6;

	GP_Button* REC=MMenu.addGP_Button(NULL,619,525,CHK.GPID,1,0);
	REC->OnUserClick=&MMItemChoose;
	REC->UserParam=77;
	
	int TB2Set[30] = {5,6,7,8,9, 5,6,7,8,9, 5,6,7,8,9, 
				      5,6,7,8,9, 5,6,7,8,9, 5,6,7,8,9};
	UniversalButton* EDITPAN=MMenu.addUniversalButton(649,525,267,"",TB2.GPID,TB2Set,0,0,0,FYellow,FYellow,FYellow);
	
	EDITPAN->Hint=GetTextByID("INTF5_HINT9");
	EDITPAN->Enabled=0;

	InputBox* IBREC=MMenu.addInputBox(NULL,659,524,RECFILE,120,248,26,FYellow,FWhite);
	IBREC->Hint=GetTextByID("INTF5_HINT10");

	FMode->AddLine(GetTextByID("SLOWMOD"));
	FMode->AddLine(GetTextByID("FASTMOD"));
	FMode->Center-=5;
	FMode->FontDy-=2;
	FMode->OneDy-=1;
	
	
	MUS->AddLine(GetTextByID("NOCD"));
	//MUS->AddLine(GetTextByID("NATMUS"));
	MUS->AddLine(GetTextByID("RNDMUS"));
	//MUS->CurLine=100;
	MUS->Center-=5;
	MUS->FontDy-=2;
	MUS->OneDy-=1;
	//MUS->FontDx-=6;
	
	for(int i=0;i<NModes;i++){
		char cc[30];
		sprintf_s( cc,sizeof(cc),"%dx%d",ModeLX[i],ModeLY[i]);
		VMode->AddLine(cc);
		if(exRealLx==ModeLX[i]&&exRealLy==ModeLY[i])VMode->CurLine=i;
	};
	if(VMode->CurLine==100)VMode->CurLine=0;
	bool RECSTATE=RecordMode;
	FMode->CurLine=exFMode;
	MUS->CurLine=PlayMode!=0;
	ItemChoose=-1;
	int pp=1;
	KeyPressed=0;
	int PrevPlayMode=MUS->CurLine!=0;
	int ppp=PlayMode;
	
	StartMixing();
	do{
		if(RECSTATE){
			EDITPAN->Visible = 0;
			//EDITPAN->Enabled=1;
			IBREC->Enabled=1;
			REC->ActiveFrame=1;
			REC->PassiveFrame=1;
			IBREC->Active=1;
		}else{
			EDITPAN->Visible = 1;
			//EDITPAN->Enabled=0;
			IBREC->Enabled=0;
			REC->ActiveFrame=0;
			REC->PassiveFrame=0;
			IBREC->Active=0;
		};		
		//P1->Visible=P1->MouseOver;
		//P2->Visible=P2->MouseOver;
		//P3->Visible=P3->MouseOver;
		//P4->Visible=P4->MouseOver;
		ProcessMessages();
		StdKeys();
		MMenu.MarkToDraw();
		MMenu.ProcessDialogs();
		ProcessMixing();
		MMenu.RefreshView();
		if(ItemChoose==77){
			RECSTATE=!RECSTATE;
			ItemChoose=-1;
		};
		if(pp){
			//SlowLoadPalette("1\\agew_1.pal");
			pp=0;
		};
		SetCDVolume(MVL->SPos);
		if(PrevPlayMode!=(MUS->CurLine!=0)){
			PlayMode=MUS->CurLine!=0;
			if(MUS->CurLine)PlayRandomTrack();
			else StopPlayCD();
			PrevPlayMode=MUS->CurLine!=0;
		};
	}while(ItemChoose==-1);
	
	if(ItemChoose==mcmOk){
#ifndef SPEEDFIX
        FPSTime = 100 - HB->SPos;
#endif
		ScrollSpeed=SSP->SPos+2;
		exRealLx=ModeLX[VMode->CurLine];
		exRealLy=ModeLY[VMode->CurLine];
		WarSound=SVL->SPos;
		MidiSound=MVL->SPos;
		PlayMode=MUS->CurLine!=0;
		exFMode=FMode->CurLine;
		RecordMode=RECSTATE;
	}else{
		PlayMode=ppp!=0;
		if((PlayMode!=0)!=PrevPlayMode){
			if(PlayMode)PlayRandomTrack();
			else StopPlayCD();
		};
	};
//	SlowUnLoadPalette("1\\agew_1.pal");
	return false;
};

