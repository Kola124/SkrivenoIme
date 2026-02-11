int PanelGP=-1;
extern int GenPnX;
extern DialogsSystem GLOBHINT;
void ShowRMap(){
	if(PlayGameMode==1)return;
	int x=smapx+smaplx*32-140;
	if(MiniMode)x=smapx+(smaplx<<4)-140;
	int y=smapy;
	
	int ResBackW = 256;

	int ResPanelW = 716;
	int x0=RealLx-ResPanelW;
	int RPMButX=x0-65;
	int RPLButX=x0-65*2;

	int x1 = x0+119;
	int dx0 = 110;

	if(PanelGP==-1) PanelGP=GPS.PreLoadGPImage("Interf2\\ResPanel");
	
	int N=(RealLx-ResPanelW)/ResBackW+1;
	for(int i=0;i<N;i++){
		GPS.ShowGP(i*ResBackW,0,PanelGP,1,MyNation);
	};

	GPS.ShowGP(x0,0,PanelGP,0,MyNation);
	GPS.ShowGP(RPMButX,0,PanelGP,2,MyNation);
    GPS.ShowGP(RPMButX - 65, 0, PanelGP, 2, MyNation);
#ifdef _USE3D
	GPS.FlushBatches();
#endif
	int MON[8];	
	GetCorrectMoney(NatRefTBL[MyNation],MON);	
	for(int i=0;i<6;i++){
		char gg[100];
		sprintf(gg,"%d",MON[i]);
		//int x;
		int ii=i;
		if(i==FoodID)//GetResID("FOOD"))
			ii-=2;
		else
		if(i==GoldID)//GetResID("GOLD"))
			ii+=2;			
		if(MON[i]<10000000){
			ShowString(x1+ii*dx0-GetRLCStrWidth(gg,&YellowFont),7,gg,&YellowFont);
		}else
			ShowString(x1+ii*dx0-GetRLCStrWidth(RES_LOT_MONEY,&YellowFont),7, RES_LOT_MONEY, &YellowFont);
	};

    //POPULATION COUNTER
    Nation* NT = NATIONS + NatRefTBL[MyNation];
    char str[128];
    int y1 = 38;
    int x2 = GenPnX+45;
    if (NT->NGidot >= NT->NFarms) {
        sprintf(str, HABITAN, NT->NGidot);//"+ñøªðªõûõù:%d"
        if ( NT->NFarms != 0) ShowString(x2, y1, str, &RedFont);
        else ShowString(x2, y1, str, &YellowFont);
        sprintf(str, MAXHABI, NT->NFarms);//"¦ðú¸.þñøªðªõûõù:%d"
        ShowString(x2, y1 + 12, str, &YellowFont);
    }else {
        sprintf(str, HABITAN, NT->NGidot);//"+ñøªðªõûõù:%d"
        ShowString(x2, y1, str, &YellowFont);
        sprintf(str, MAXHABI, NT->NFarms);//"¦ðú¸.þñøªðªõûõù:%d"
        ShowString(x2, y1 + 12, str, &YellowFont);
    }
    
#ifdef _USE3D
	GPS.FlushBatches();
#endif
	GLOBHINT.MarkToDraw();
	GLOBHINT.ProcessDialogs();
	return;
};