bool ProcessGameOptions(){
    if (!window_mode)
    {//Calculate offsets for centering menu in fullscreen mode
        menu_x_off = (screen_width - 1024) / 2;
        menu_y_off = (screen_height - 768) / 2;
        menu_hint_x = 18 + menu_x_off;
        menu_hint_y = 701 + menu_y_off;
    }
	// Graphics
	LocalGP BTNS("Interf2\\G_Load_Game");	
	LocalGP MGP("Interf2\\Options_for_Game");

	LocalGP CMB("Interf2\\Combo2");
	LocalGP SCR("Interf2\\Scroll2");
	
	LocalGP HFONT("rom10");
	RLCFont hfnt(HFONT.GPID);
	hfnt.SetWhiteColor();
	LoadPointer("cur_mn.rlc");
	//SQPicture MnPanel("Interface\\Background_options.bmp");
	int dx=-63+6+((RealLx-484)>>1);
	int dy=-166+((RealLy-425)>>1);
	DialogsSystem MMenu(dx,dy);
	MMenu.HintFont=&hfnt;
    if (!window_mode)
    {
        menu_hint_x = 18 + menu_x_off;
        menu_hint_y = 701 + menu_y_off;
    }
    else {
        menu_hint_x = 18;
        menu_hint_y = 701;
    }
	MMenu.HintY= menu_hint_y;
	MMenu.HintX= menu_hint_x;
	
	// Fonts
	RLCFont* FWin = &fon16y5;
	RLCFont* FABut = &fon18y5;
	RLCFont* FPBut = &fon18y5;
	RLCFont* FText = &fon18y3;
	RLCFont* FWhite = &fon18w;
	RLCFont* FYellow = &fon18y3;

	//int Dx=((RealLx-GPS.GetGPWidth(BTNS.GPID,1))>>1)-85-125;
	//int Dy=(RealLy-GPS.GetGPHeight(BTNS.GPID,1))>>1;	
	//MMenu.addGPPicture(NULL,Dx,Dy,MGP.GPID,0);
	//MMenu.addGPPicture(NULL,dx,dy,BTNS.GPID,1);

#ifndef SPEEDFIX
    if (FPSTime > 100)FPSTime = 100;
    if (FPSTime < 0)FPSTime = 0;
#endif
	//game speed (#3)
	if(ScrollSpeed<2)ScrollSpeed=2;

	int X = -60;
	int Y = -40;

	GPS.ShowGP(X+dx+63,Y+dy+166,MGP.GPID,0,0);
	GPPicture* PIC=MMenu.addGPPicture(NULL,X+63,Y+166,MGP.GPID,1);	
	
	int SecX = X+443-317;//150;
	int SecDx = 27;
	int SecY = Y+258-15;//244;
	TextButton* VO=MMenu.addTextButton(NULL,SecX,SecY,GetTextByID("INTF_OPT_VO"),FWhite,FWhite,FWhite,0);
	TextButton* AO=MMenu.addTextButton(NULL,SecX,SecY+SecDx*2,GetTextByID("INTF_OPT_AO"),FWhite,FWhite,FWhite,0);
	TextButton* GO=MMenu.addTextButton(NULL,SecX,SecY+SecDx*6,GetTextByID("INTF_OPT_GO"),FWhite,FWhite,FWhite,0);
	
	int DY=27;
	int SubSecX = SecX+10;
	TextButton* VO0=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*1,GetTextByID("INTF_OPT_VO_0"),FYellow,FYellow,FYellow,0);
	TextButton* AO0=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*3,GetTextByID("INTF_OPT_AO_0"),FYellow,FYellow,FYellow,0);
	TextButton* AO1=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*4,GetTextByID("INTF_OPT_AO_1"),FYellow,FYellow,FYellow,0);
	TextButton* AO2=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*5,GetTextByID("INTF_OPT_AO_2"),FYellow,FYellow,FYellow,0);
	TextButton* GO0=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*7,GetTextByID("INTF_OPT_GO_0"),FYellow,FYellow,FYellow,0);
	TextButton* GO1=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*8,GetTextByID("INTF_OPT_GO_1"),FYellow,FYellow,FYellow,0);
	TextButton* GO2=MMenu.addTextButton(NULL,SubSecX,SecY+SecDx*9,GetTextByID("INTF_OPT_GO_2"),FYellow,FYellow,FYellow,0);

	int ComX = X+299;
	int ComY = Y+271;
	ComboBox* VMode=MMenu.addGP_ComboBox(NULL,ComX,ComY-2,CMB.GPID,0,9,0,&WhiteFont,&YellowFont,NULL);
	ComboBox* MUS=MMenu.addGP_ComboBox(NULL,ComX,ComY+2*DY/*323*/,CMB.GPID,0,9,0,&WhiteFont,&YellowFont,NULL);
	ComboBox* FMode=MMenu.addGP_ComboBox(NULL,ComX,ComY+6*DY/*432*/,CMB.GPID,0,9,0,&WhiteFont,&YellowFont,NULL);

	int X0=X+618-317;
	int LX=290+2;
	int ScrDy=8;
	
	VScrollBar* SVL=MMenu.addGP_ScrollBarL(NULL,X0,ScrDy+1+ComY+3*DY,100,WarSound,SCR.GPID,0,LX,12,0,0);
	VScrollBar* MVL=MMenu.addGP_ScrollBarL(NULL,X0,ScrDy+ComY+4*DY,100,MidiSound,SCR.GPID,0,LX,12,0,0);	
	VScrollBar* SSP=MMenu.addGP_ScrollBarL(NULL,X0,ScrDy+1+ComY+7*DY,8,ScrollSpeed-2,SCR.GPID,0,LX,12,0,0);
#ifndef SPEEDFIX
#ifndef EW
    VScrollBar* HB = MMenu.addGP_ScrollBarL(NULL, X0, ScrDy + ComY + 8 * DY, 100, 100 - FPSTime, SCR.GPID, 0, LX, 12, 0, 0);
#else
    VScrollBar* HB = MMenu.addGP_ScrollBarL(NULL, X0, ScrDy + ComY + 8 * DY, 100, 100 - FPSTime, SCR.GPID, 0, LX, 12, 0, 0);
#endif
#endif

	int ButY = Y+527;
	GP_TextButton* OkBtn=MMenu.addGP_TextButton(NULL,X+476-337,ButY,GetTextByID("INTF_ACCEPT"),BTNS.GPID,1,FABut, FPBut);
	GP_TextButton* ExitBtn=MMenu.addGP_TextButton(NULL,X+708-337,ButY,GetTextByID("INTF_CANCEL"),BTNS.GPID,1,FABut, FPBut);	

	OkBtn->FontDy += 3;
	ExitBtn->FontDy += 3;
	OkBtn->FontDx += 8;
	ExitBtn->FontDx += 8;

	// Text
	MMenu.addTextButton(NULL,X+680-317,Y+217-15,GetTextByID("INTF_OPT_TITLE1"),FWin,FWin,FWin,1);	

	VMode->Center-=5;
	VMode->FontDy-=2;
	VMode->OneDy-=1;
	MUS->Center-=5;
	MUS->FontDy-=2;
	MUS->OneDy-=1;
	FMode->Center-=5;
	FMode->FontDy-=2;
	FMode->OneDy-=1;

	GPPicture* P1=MMenu.addGPPicture(NULL,240,317+11,BTNS.GPID,15);
	GPPicture* P2=MMenu.addGPPicture(NULL,240,343+11,BTNS.GPID,16);
	GPPicture* P3=MMenu.addGPPicture(NULL,240,447+11,BTNS.GPID,17);
	GPPicture* P4=MMenu.addGPPicture(NULL,240,473+11,BTNS.GPID,18);
	P1->Visible=0;
	P2->Visible=0;
	P3->Visible=0;
	P4->Visible=0;
	
	OkBtn->UserParam=mcmOk;
	OkBtn->OnUserClick=&MMItemChoose;
	
	ExitBtn->UserParam=mcmExit;
	ExitBtn->OnUserClick=&MMItemChoose;
	//scroll speed (#4)
	
	//sound volume (#1)
	MidiSound=GetCDVolume();
	
	//music volume (#2)
	
	VMode->CurLine=100;
	for(int i=0;i<NModes;i++){
		char cc[30];
		sprintf_s( cc,sizeof(cc),"%dx%d",ModeLX[i],ModeLY[i]);
		VMode->AddLine(cc);
		if(RealLx==ModeLX[i]&&RealLy==ModeLY[i])VMode->CurLine=i;
	};
	if(VMode->CurLine==100)VMode->CurLine=0;
	
	FMode->AddLine(SLOWMOD);
	FMode->AddLine(FASTMOD);
	FMode->CurLine=exFMode;

	
	MUS->AddLine(GetTextByID("NOCD"));
	//MUS->AddLine(GetTextByID("NATMUS"));
	MUS->AddLine(GetTextByID("RNDMUS"));
	MUS->CurLine=PlayMode!=0;

	ItemChoose=-1;
	//SVL->SPos=WarSound;
	//MVL->SPos=MidiSound;
	//int pp=1;
	int PrevPlayMode=MUS->CurLine!=0;
	KeyPressed=0;
	int ppp=PlayMode;
	int StartTime=GetTickCount();
	do{
#ifdef _USE3D
		void ProcessScreen();
		ProcessScreen();
#endif //_USE3D
		if(GetTickCount()-StartTime>10000&&NPlayers>1){
			ItemChoose=mcmCancel;
			ShowOptMessage("#LOOKOPT",0);
			//SpecCmd=199;
			//IAmLeft();
		};
		//P1->Visible=P1->MouseOver;
		//P2->Visible=P2->MouseOver;
		//P3->Visible=P3->MouseOver;
		//P4->Visible=P4->MouseOver;
		ProcessMessages();
		StdKeys();
		MMenu.MarkToDraw();
		MMenu.ProcessDialogs();
		MMenu.RefreshView();
		SetCDVolume(MVL->SPos);
		if(PrevPlayMode!=(MUS->CurLine!=0)){
			PlayMode=MUS->CurLine!=0;
			if(MUS->CurLine)PlayRandomTrack();
			else StopPlayCD();
			PrevPlayMode=MUS->CurLine!=0;
		};
		//if(pp){
		//	SlowLoadPalette("2\\agew_1.pal");
		//	pp=0;
		//};
	}while(ItemChoose==-1);
	if(ItemChoose==mcmOk){
#ifndef SPEEDFIX
        FPSTime = 100 - HB->SPos;
#endif
		ScrollSpeed=SSP->SPos+2;
		exFMode=FMode->CurLine;
		if(RealLx!=ModeLX[VMode->CurLine]||RealLy!=ModeLY[VMode->CurLine])SetGameDisplayModeAnyway(ModeLX[VMode->CurLine],ModeLY[VMode->CurLine]);
		WarSound=SVL->SPos;
		MidiSound=MVL->SPos;
		PlayMode=MUS->CurLine!=0;
	}else{
		PlayMode=ppp;
		if((PlayMode!=0)!=PrevPlayMode){
			if(PlayMode)PlayRandomTrack();
			else StopPlayCD();
		};
	};
	return false;
};