extern int menu_x_off;
extern int menu_y_off;

bool SingleOptions(){
	if(MPL_WaitingGame(1,1)){
		StartIGame(1);
		return true;
	}else return false;
};
int MM_ProcessSinglePlayer(){
	
	LoadFog(1);
	LocalGP BTNS("Interface\\Single_Player");	

	SQPicture MnPanel("Interface\\Background_Single_Player.bmp");
#ifdef SCREENFIX
	DialogsSystem MMenu(menu_x_off, menu_y_off);
#else
    DialogsSystem MMenu(0, 0);
#endif
	LocalGP HFONT("rom10");
	RLCFont hfnt(HFONT.GPID);
	hfnt.SetWhiteColor();
    if (!window_mode)
    {
        menu_hint_x = 18 + menu_x_off;
        menu_hint_y = 701 + menu_y_off;
    }
    else {
        menu_hint_x = 18;
        menu_hint_y = 701;
    }
	MMenu.HintFont=&hfnt;
#ifdef SCREENFIX
	MMenu.HintY= menu_hint_y;
	MMenu.HintX= menu_hint_x;
#else
    MMenu.HintY = 18;
    MMenu.HintX = 701;
#endif
	int Dy=27;
	Picture* PIC=MMenu.addPicture(NULL,0,0,&MnPanel,&MnPanel,&MnPanel);
	

	//LoadPlayerData();//();
	//LoadFog(2);
	LoadPlayerData();//();
	//LoadFog(2);
	//SlowUnLoadPalette("2\\agew_1.pal");
	
	ItemChoose=-1;
	int pp=1;
	KeyPressed=false;
	LastKey=0;
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
		//if((ItemChoose==1||ItemChoose==2)&&!CAMPAGINS.NCamp)ItemChoose=-1;
/*
#ifdef TUTORIAL_DEMOVERSION
	if(ItemChoose==2)ItemChoose=-1;
#endif 
*/
	}while(ItemChoose==-1);
	if(ItemChoose==3){
//		SlowUnLoadPalette("1\\agew_1.pal");
		SingleOptions();
	};
	if(ItemChoose==1){
//		SlowUnLoadPalette("1\\agew_1.pal");
		ProcessCampagins(-1);
	};
//#ifndef TUTORIAL_DEMOVERSION
	if(ItemChoose==2){
//		SlowUnLoadPalette("1\\agew_1.pal");
		SelectSingleMission();
	};
//#endif
	if(ItemChoose==5){
//		SlowUnLoadPalette("1\\agew_1.pal");
	};
	return ItemChoose;
};