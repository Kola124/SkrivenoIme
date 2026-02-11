#ifndef DIALOGS_USER
#define DIALOGS_API __declspec(dllexport)
#else
#define DIALOGS_API __declspec(dllimport)
#endif
#define MAXDLG 1024
DIALOGS_API int GetSound(char* Name);
DIALOGS_API char* GetTextByID(char*);

#ifndef _USE3D
class DIALOGS_API SQPicture{
public:
	word* PicPtr;
	SQPicture();
	SQPicture(char* Name);
	void LoadPicture(char* Name);
	void Draw(int x,int y);
	void DrawTransparent(int x,int y);
	void Xlat(byte* tbl){
		if(PicPtr){
			int N=PicPtr[0]*PicPtr[1];
			byte* data=(byte*)(PicPtr+2);
			for(int i=0;i<N;i++)data[i]=tbl[data[i]];
		};
	};
	~SQPicture();
	int GetLx(){
		if(!PicPtr)return 0;
		return PicPtr[0];
	};
	int GetLy(){
		if(!PicPtr)return 0;
		return PicPtr[1];
	};

	void SetLx( int lX ){
		if(!PicPtr)return;
		PicPtr[0] = lX;
	};
	void SetLy( int lY ){
		if(!PicPtr)return;
		PicPtr[1] = lY;
	};
	byte* GetPixelData()
	{
		if (!PicPtr) return 0;
		return (byte*)(PicPtr + 2);
	}
	void SetPixelData( void* dta )
	{
		PicPtr = (word*)dta;
	}
};

#endif // !_USE3D

#ifdef _USE3D
#include "GP_Draw.h"
class DIALOGS_API SQPicture : public GPBitmap{
public:
	SQPicture();
	SQPicture(char* Name);
	~SQPicture();
	
	void LoadPicture(char* Name);
	void DrawTransparent(int x,int y);
	void Draw(int x,int y);

	void Xlat(byte* tbl){
		if(GetPixelData()){
			int N=GetLx()*GetLy();
			byte* data=(byte*)GetPixelData();
			for(int i=0;i<N;i++)data[i]=tbl[data[i]];
		};
	};
	
	int GetLx() const; 
	int GetLy() const; 
	void SetLx( int lX );
	void SetLy( int lY );
private:
};
#endif // _USE3D

class DIALOGS_API SimpleDialog;
typedef bool VCall(SimpleDialog* SD);
typedef bool VCallXY(SimpleDialog* SD,int x,int y,int ActiveID);
class DialogsSystem;
class DIALOGS_API SimpleDialog;
class DIALOGS_API VScrollBar;
class DIALOGS_API SimpleDialog{
public:
	bool Enabled:1;
	bool Active:1;
	bool PrevMouseOver:1;
	bool MouseOver:1;
	bool NeedToDraw:1;
	bool MouseOverActive:1;
	bool IsActive:1;//drop-down panel is active
	bool Visible:1;
	bool AllocHint:1;
	bool NeedRedrawAll=0;
	short MouseSound=0;
	short ClickSound=0;
	int x=0,y=0,x1=0,y1=0;
	int UserParam=0;
	char* Hint=nullptr; 
	char* AllocPtr=nullptr;
	SimpleDialog* Parent=nullptr;
	SimpleDialog* Child=nullptr;
	VCall* OnClick=nullptr;
	VCall* OnDraw=nullptr;
	VCall* OnActivate=nullptr;
	VCall* OnLeave=nullptr;
	VCall* OnKeyDown=nullptr;
	VCall* OnMouseOver=nullptr;
	VCall* Refresh=nullptr;
	VCall* Destroy=nullptr;
	VCall* OnUserClick=nullptr;
	VCallXY* MouseOverActiveZone=nullptr;
	VCall* OnDrawActive=nullptr; 
	VCallXY* OnNewClick=nullptr;
	VScrollBar* ParentSB=nullptr;
	DialogsSystem* ParentDS=nullptr;
	SimpleDialog();
	void AssignSound(int ID,int USAGE);
	void AssignSound(char* Name,int USAGE);
};
#define CLICK_SOUND  0x1234
#define MOUSE_SOUND  0x4321
#define OK_SOUND     0x5421
#define CANCEL_SOUND 0x7513

class DIALOGS_API Picture:public SimpleDialog{
public:
	SQPicture* PassivePicture=nullptr;
	SQPicture* ActivePicture=nullptr;
	SQPicture* DisabledPicture=nullptr;
	bool Transparent:1;
    Picture() : SimpleDialog() {};
};
class DIALOGS_API RLCPicture:public SimpleDialog{
public:
	RLCTable* PassivePicture=nullptr;
	byte ppic=0;
	RLCTable* ActivePicture=nullptr;
	byte apic=0;
	RLCTable* DisabledPicture=nullptr;
	byte dpic=0;
};
class DIALOGS_API GPPicture:public SimpleDialog{
public:
	int dx=0,dy=0;
	int FileID=0;
	int SpriteID=0;
	int Nation=0;
};
class DIALOGS_API TextMessage{
	char* Message=nullptr;
	RLCFont* Font=nullptr;
	byte Align=0;
};
class DIALOGS_API TextButton:public SimpleDialog{
public:
	char* Message=nullptr;
	RLCFont* ActiveFont=nullptr;
	RLCFont* PassiveFont=nullptr;
	RLCFont* DisabledFont=nullptr;
	int	xc=0;
	int yc=0;
	int ActiveDX=0;
	int ActiveDY=0;
	byte Align=0;
};
class DIALOGS_API GP_TextButton:public TextButton{
public:
	char* Message=nullptr;
	RLCFont* ActiveFont=nullptr;
	RLCFont* PassiveFont=nullptr;
	int FontDy=0;
	int FontDx=0;
	bool Center=0;
	int GP_File=0;
	int Sprite=0;
	int Sprite1=0;
	int Nx=0;
	int OneLx=0;
};
class DIALOGS_API UniversalButton:public TextButton{
public:
	char* Message=nullptr;
	RLCFont* ActiveFont=nullptr;
	RLCFont* PassiveFont=nullptr;
	RLCFont* SelectedFont=nullptr;
	int  FontDy=0;
	int  FontDx=0;
	bool Center=0;
	int  GP_File=0;
    int  SpritesSet[30] = {0};
	int  Group=0;
	int  State=0;
	bool Tiling=0;
};

class DIALOGS_API VideoButton:public SimpleDialog{
public:
	int GP_on=0;
	int GP_off=0;
	int N_on=0;
	int N_off=0;
	int CurSprite=0;
	int Stage=0;
	int LastTime=0;
};
class DIALOGS_API VScrollBar:public SimpleDialog{
public:
	int SMaxPos=0,SPos=0;
	int sbx=0,sby=0,sblx=0,sbly=0;
	int LastTime=0;
	byte mkly=0,btnly=0;
	bool Zaxvat=0;
	SQPicture* btn_up0=nullptr;
	SQPicture* btn_up1=nullptr;
	SQPicture* btn_up1p=nullptr;
	SQPicture* btn_dn0=nullptr;
	SQPicture* btn_dn1=nullptr;
	SQPicture* btn_dn1p=nullptr;
	SQPicture* sbar0=nullptr;
	SQPicture* sbar1=nullptr;
	SQPicture* marker=nullptr;
	int OnesDy=0;
	void SetMaxPos(int i){SMaxPos=i;};
	int GetMaxPos(){return SMaxPos;};
	int GetPos(){return SPos;};
	//----------GP_ScrollBar------------//
	int GP_File=0;
	int ScrollIndex=0;
	int LineIndex=0;
	int ScrDx=0,ScrDy=0;
	int ScrLx=0,ScrLy=0;
	int LineLx=0;
	int LineLy=0;
	//----------
	int StartGP_Spr=0;
};
class DIALOGS_API BpxTextButton:public TextButton{
public:
	SQPicture* PassivePicture=nullptr;
	SQPicture* ActivePicture=nullptr;
	SQPicture* DisabledPicture=nullptr;
};

typedef void procDrawBoxElement(int x,int y,int Lx,int Ly,int Index,byte Active,int param);
#define MaxColumn 16
class DIALOGS_API ComplexBox:public SimpleDialog{
public:
	procDrawBoxElement* DRAWPROC=nullptr;
	int N=0;
	int CurrentItem=0;
	int TopStart=0;
	int NOnScr=0;
	int OneLy=0;
	int GP_Index=0;
	int StSprite=0;
	int M_OvrItem=0;
	int FirstVisItem=0;
	int param=0;
	VScrollBar* VS=nullptr;
};
class DIALOGS_API Canvas:public SimpleDialog{
public:
	int BottomY=0;
	int L=0;
	int MaxL=0;
	VScrollBar* VS=nullptr;
	byte* DrawData=nullptr;
	void AddLine(int x,int y,int x1,int y1,byte c);
	void AddSprite(int x,int y,int GPidx,int Sprite);
	void AddBar(int x,int y,int Lx,int Ly,byte c);
	void AddRect(int x,int y,int Lx,int Ly,byte c);
	void AddText(int x,int y,char* Text,RLCFont* Font);
	void AddCText(int x,int y,char* Text,RLCFont* Font);
	void CheckSize(int sz);
};
class DIALOGS_API CustomBox:public SimpleDialog{
public:
	procDrawBoxElement* DRAWPROC=nullptr;
	int param=0;
};
struct ListBoxItem{
	char* Message=nullptr;
	int Param1=0;
	byte Flags=0;
	ListBoxItem* NextItem=nullptr; 
};
class DIALOGS_API ListBox:public SimpleDialog{
public:
	ListBoxItem* FirstItem=nullptr;
	ListBoxItem* LastItem=nullptr;
	SQPicture* ItemPic=nullptr;
	RLCFont* AFont=nullptr;
	RLCFont* PFont=nullptr;
	RLCFont* DFont=nullptr;
	byte ny=0;
	byte oneLy=0;
	int	 oneLx=0;
	int NItems=0;
	int CurItem=0;
	int FLItem=0;
	int GP_File=0;
	int Sprite=0;
	int FontDy=0;
	int FontDx=0;
	int M_Over=0;
	int CurMouseOver=0;
	VScrollBar* VS=nullptr;
	ListBoxItem* GetItem(int i);
	void AddItem(char* str,int info);
	void AddStaticItem(char* str,int info);
	void ClearItems();
	void SetFirstPos(int i);
	void SetCurrentItem(int i);
};
class DIALOGS_API RLCListBox:public SimpleDialog{
public:
	//RLCTable Items;
	int GPIndex=0;
	byte* Choosed=nullptr;
	int NItems=0;
	byte BackColor=0;
	byte SelColor=0;
	int  XPos=0;
	int  MaxXpos=0;
	bool Done=0;
};
class DIALOGS_API InputBox:public SimpleDialog{
public:
	char* Str=nullptr;
    size_t CursPos=0;
	int totdx=0;
	int StrMaxLen=0;
	RLCFont* Font=nullptr;
	RLCFont* AFont=nullptr;
	SQPicture* Pict=nullptr;
	bool Centering=0;
	bool Anonim=0;
};
class DIALOGS_API DialogsSystem;
class DIALOGS_API CheckBox:public SimpleDialog{
public:
	DialogsSystem* DS=nullptr;
	SQPicture* OnPic=nullptr;
	SQPicture* OffPic=nullptr;
	char* Message=nullptr;
    unsigned int State:2;
	bool Transparent:1;
	bool Central:1;
	int GroupIndex=0;
	RLCFont* Font=nullptr;
	RLCFont* AFont=nullptr;
	//GP
	short GP_File=0;
	short Sprite0=0;
	short Sprite1=0;
	short Sprite2=0;
	short Sprite3=0;
};
class DIALOGS_API ColoredBar:public SimpleDialog{
public:
	byte color=0;
	byte Style=0;
};
class DIALOGS_API ChatViewer:public SimpleDialog{
public:
	char*** Mess=nullptr;
	char*** Names=nullptr;
	int* NChats=nullptr;
	int ChatDY=0;
	int MaxLx=0;
	int OneLy=0;
	int ScrNy=0;
};
struct LineInfo{
	bool NeedFormat=0;
	word LineSize=0;
	word NSpaces=0;
	int  Offset=0;
	int  NextOffset=0;
	word LineWidth=0;
};
class DIALOGS_API TextViewer:public SimpleDialog{
public:
	char* TextPtr=nullptr;
	int TextSize=0;
	int NLines=0;
	int Line=0;
	int PageSize=0;
	int Lx=0;
	word SymSize=0;
	RLCFont* Font=nullptr;
	void GetNextLine(LineInfo*);
	void CreateLinesList();
	char** LinePtrs=nullptr;
	word*  LineSize=nullptr;
	word*  LineWidth=nullptr;
	bool*  NeedFormat=nullptr;
	word*  NSpaces=nullptr;
	VScrollBar* VS=nullptr;
	void AssignScroll(VScrollBar* SB);
	void LoadFile(char* Name);
};
class DIALOGS_API BPXView:public SimpleDialog{
public:
	byte* Ptr=nullptr;
	int OneLx=0;
	int OneLy=0;
	int Nx=0,Ny=0;
	int RealNy=0;
	int PosY=0;
	int ChoosedPos=0;
	int DECLX=0;
	byte* Choosed=nullptr;
	bool Done=0;
	bool EnableSelection=0;
	VScrollBar* VSC=nullptr;
	byte Mode=0;
};
class DIALOGS_API WinComboBox;
class DIALOGS_API WinComboBox:public SimpleDialog{
public:
	RLCFont* ActiveFont=nullptr;
	RLCFont* PassiveFont=nullptr;
	int ListX0=0;
	int ListY0=0;
	int ListX1=0;
	int ListY1=0;
	char** Lines=nullptr;
	int CurLine=0;
	int ActiveLine=0;
	int NLines=0;
	char* Message=nullptr;
	WinComboBox** Group=nullptr;
	int LastChooseTime=0;
	int NBox=0;
	int CurBox=0;
	void AddLine(char* Text);
	void Clear();
};
class DIALOGS_API ComboBox:public SimpleDialog{
public:
	RLCFont* ActiveFont=nullptr;
	RLCFont* PassiveFont=nullptr;
	int HdrLy=0;
	int FontDy=0;
	int FontDx=0;
	int OneLy=0;
	int OneLx=0;
	int OneDx=0;
	int OneDy=0;
	int HiLy=0;
	int LoLy=0;
	int NLines=0;
	int DropX=0;
	int DropY=0;
	int LightIndex=0;
	int GP_File=0;
	int UpPart=0;
	int Center=0;
	int DownPart=0;
	int UpLy=0;
	byte BackColor=0;
	byte BorderColor=0;
	char** Lines=nullptr;
	int CurLine=0;
	//--------new--------
	VScrollBar* VS=nullptr;
	int  MaxLY=0;
	int  YPos=0;
	int  DLX=0;
	//--------ruler(new!!)------
	bool rulermode=0;
	int MinDeal=0;

	void AssignScroll(DialogsSystem* DSS,VScrollBar** SCR,int GPFile,int Sprite,int MaxLy);
	void AddLine(char* Text);
	void AddComplexLine(char* Text);
	void CreateRuler(int MinDeal,int NDeals); 
	void Clear();
};
class DIALOGS_API GP_Button:public SimpleDialog{
public:
	int GP_File=0;
	int ActiveFrame=0;
	int PassiveFrame=0;
};
struct OnePage{
	int x=0,y=0,x1=0,y1=0;
	int Index=0;
};
class DIALOGS_API GP_PageControl:public SimpleDialog{
public:
	int GP_File=0;
	int CurPage=0;
	OnePage* Pages=nullptr;
	int NPages=0;
	void AddPage(int x0,int y0,int x1,int y1,int Index);
};
class DIALOGS_API BorderEx:public SimpleDialog{
public:
	int ymid=0;
	byte Style=0;
};
class DIALOGS_API CustomBorder:public SimpleDialog{
public:
	int GP=0;
    int BOR_N[8] = {0};
    int BOR_A[8] = {0};
	int FILL_N=0;
	int FILL_A=0;
};
class DIALOGS_API DialogsSystem{
public:
	short OkSound=0;
	short CancelSound=0;
	short UserClickSound=0;
	int HintX=0,HintY=0;
	char* Hint=nullptr;
	char DefaultHint[1024];
	int	BaseX=0,BaseY=0;
	SimpleDialog* DSS[MAXDLG];
	int NDial=0;
	DialogsSystem(int x,int y);
	DialogsSystem();
	~DialogsSystem();
	RLCFont* Active=nullptr;
	RLCFont* Passive=nullptr;
	RLCFont* Disabled=nullptr;
	RLCFont* Message=nullptr;
	RLCFont* HintFont=nullptr;
	int ActiveX=0,ActiveY=0,ActiveX1=0,ActiveY1=0,ActiveID=0;
	SimpleDialog* ActiveParent=nullptr;
	bool CenteredHint=0;
	void ProcessDialogs();
	void MarkToDraw();
	void RefreshView();
	void CloseDialogs();
	void SetFonts(RLCFont* Active,
				  RLCFont* Passive,
				  RLCFont* Disabled,
				  RLCFont* Message);
	Picture* addPicture(SimpleDialog* Parent,int x,int y,
						SQPicture* Active,
						SQPicture* Passive,
						SQPicture* Disabled);
	GPPicture* addGPPicture(SimpleDialog* Parent,
									   int dx,int dy,int FileID,int SpriteID);
	RLCPicture* addRLCPicture(SimpleDialog* Parent,int x,int y,
						RLCTable* Active,byte apic,
						RLCTable* Passive,byte ppic,
						RLCTable* Disabled,byte dpic);
	TextMessage* addTextMessage(SimpleDialog* Parent,int x,int y,char* str,RLCFont* Font,byte Align);
	TextMessage* addsTextMessage(SimpleDialog* Parent,int x,int y,char* str);
	TextButton* addTextButton(SimpleDialog* Parent,int x,int y,char* str,
						RLCFont* Active,
						RLCFont* Passive,
						RLCFont* Disabled,
						byte Align);//==0-left, ==1-center,  ==2-right
	TextButton* addsTextButton(SimpleDialog* Parent,int x,int y,char* str);
	GP_TextButton* addGP_TextButton(SimpleDialog* Parent,int x,int y,char* str,
						int GP_File,int Sprite,RLCFont* Active,RLCFont* Passive);
	GP_TextButton* addGP_TextButtonLimited(SimpleDialog* Parent,int x,int y,char* str,
						int GP_File,int SpriteActive,int SpritePassive,int Lx,RLCFont* Active,RLCFont* Passive);
	GP_Button* addGP_Button(SimpleDialog* Parent,int x,int y,int GP_File,int Active,int Passsive);
	BpxTextButton* addBpxTextButton(SimpleDialog* Parent,int x,int y,char* str,
						RLCFont* Active,
						RLCFont* Passive,
						RLCFont* Disabled,
						SQPicture* pActive,
						SQPicture* pPassive,
						SQPicture* pDisabled);
	VScrollBar* addVScrollBar(SimpleDialog* Parent,int x,int y,int MaxPos,int Pos,
						SQPicture* btn_up0,
						SQPicture* btn_up1,
						SQPicture* btn_up1p,
						SQPicture* btn_dn0,
						SQPicture* btn_dn1,
						SQPicture* btn_dn1p,
						SQPicture* sbar0,
						SQPicture* sbar1,
						SQPicture* marker);
	VScrollBar* addHScrollBar(SimpleDialog* Parent,int x,int y,int MaxPos,int Pos,
						SQPicture* btn_up0,
						SQPicture* btn_up1,
						SQPicture* btn_up1p,
						SQPicture* btn_dn0,
						SQPicture* btn_dn1,
						SQPicture* btn_dn1p,
						SQPicture* sbar0,
						SQPicture* sbar1,
						SQPicture* marker);
	ListBox* addListBox(SimpleDialog* Parent,int x,int y,int Ny,
						SQPicture* ItemPic,
						RLCFont* AFont,
						RLCFont* PFont,
						VScrollBar* VS);
	ListBox* addListBox(SimpleDialog* Parent,
						int x,int y,int Ny,int Lx,int Ly,
						RLCFont* AFont,
						RLCFont* PFont,
						VScrollBar* VS);
	ListBox* addGP_ListBox(SimpleDialog* Parent,int x,int y,int Ny,
						int GP_File,int Sprite,int Ly,
						RLCFont* AFont,
						RLCFont* PFont,
						VScrollBar* VS);
	ComplexBox* addComplexBox(int x,int y,int Ny,int OneLy,
						procDrawBoxElement* PDRAW,int GP_File,int Spr);
	CustomBox* addCustomBox(int x,int y,int Lx,int Ly,procDrawBoxElement* PDRAW);
	InputBox* addInputBox(SimpleDialog* Parent,int x,int y,char* str,int Len,SQPicture* Panel,RLCFont* RFont,RLCFont* AFont);
	InputBox* addInputBox(SimpleDialog* Parent,int x,int y,char* str,int Len,int Lx,int Ly,RLCFont* RFont,RLCFont* AFont,bool Centering);
	InputBox* DialogsSystem::addInputBox(SimpleDialog* Parent,
									 int x,int y,char* str,
									 int Len,
									 int Lx,int Ly,
									 RLCFont* RFont,
									 RLCFont* AFont);
	CheckBox* addCheckBox(SimpleDialog* Parent,int x,int y,char* Message,
						int group,bool State,
						SQPicture* OnPict,
						SQPicture* OffPict,
						RLCFont* Font,
						RLCFont* AFont);
	CheckBox* DialogsSystem::addGP_CheckBox(SimpleDialog* Parent,
									 int x,int y,char* message,RLCFont* a_font,RLCFont* p_font,
									 int group,bool State,
									 int GP,int active,int passive,int mouseover);
	SimpleDialog* addViewPort(int x,int y,int Nx,int Ny);
	ColoredBar* addViewPort2(int x,int y,int Nx,int Ny,byte Color);
	ColoredBar* addColoredBar(int x,int y,int Nx,int Ny,byte c);
	TextViewer* addTextViewer(SimpleDialog* Parent,int x,int y,int Lx,int Ly,char* TextFile,RLCFont* TFont); 
	BPXView* addBPXView(SimpleDialog* Parent,int x,int y,int OneLx,int OneLy,int Nx,int Ny,int RealNy,byte* Ptr,VScrollBar* VSC);
	RLCListBox* addRLCListBox(SimpleDialog* Parent,int x,int y,int Lx,int Ly,int GPIndex,byte BGColor,byte SelColor);
	ComboBox* addComboBox(SimpleDialog* Parent,int x,int y,int Lx,int Ly,int LineLy,
											byte BackColor,byte BorderColor,
											RLCFont* ActiveFont,RLCFont* PassiveFont,
											char* Contence);
	ComboBox* addGP_ComboBox(SimpleDialog* Parent,int x,int y,int GP_File,
											int UpPart,int Center,int DownPart,
											RLCFont* ActiveFont,RLCFont* PassiveFont,
											char* Contence);
	ComboBox* addGP_ComboBoxDLX(SimpleDialog* Parent,int x,int y,int LX,int GP_File,
											int UpPart,int Center,int DownPart,
											RLCFont* ActiveFont,RLCFont* PassiveFont,
											char* Contence);
	WinComboBox* addWinComboBox(SimpleDialog* Parent,char* Message,int x,int y,int Lx,int Ly,
								int ListX,int ListY,int ListLx,int ListLy,
								RLCFont* ActiveFont,RLCFont* PassiveFont,
								WinComboBox** Group,int NInGroup,int CurBox);
	GP_PageControl* addPageControl(SimpleDialog* Parent,int x,int y,int GF_File,int FirstIndex);
	VScrollBar* addGP_ScrollBar(SimpleDialog* Parent,int x,int y,
								int MaxPos,int Pos,int GP_File,
								int ScrIndex,int LineIndex,int ScrDx,int ScrDy);
	VScrollBar* addGP_ScrollBarL(SimpleDialog* Parent,int x,int y,
								int MaxPos,int Pos,int GP_File,
								int ScrIndex,int LineLx,int LineLy,int ScrDx,int ScrDy);
	VScrollBar* addNewGP_VScrollBar(SimpleDialog* Parent,int x,int y,int Ly,
								int MaxPos,int Pos,int GP_File,int Sprite);
	VideoButton* addVideoButton(SimpleDialog* Parent,int x,int y,int GP1,int GP2);
	CustomBorder* addCustomBorder(int x,int y,int x1,int y1,int gp,int* bn,int* ba,int fill_n,int fill_a);
	GP_TextButton* addStdGP_TextButton(int x,int y,int Lx,char* str,
						int GP_File,int Sprite,RLCFont* Active,RLCFont* Passive);
	ChatViewer* addChatViewer(SimpleDialog* Parent,int x,int y,int Ny,int OneLy,int OneLx
		,char*** Mess,char*** Name,int* Nchats);
	SimpleDialog* addClipper(int x0,int y0,int x1,int y1);
	Canvas* AddCanvas(int x,int y,int Lx,int Ly);
	BorderEx* addBorder(int x,int y,int x1,int y1,int Ymid,byte Style);
	//--------------------New style elements---------------//
	UniversalButton* addUniversalButton(int x,int y,int Lx,char* str,int GP_File,
						int* SprSet,int Group,int NowSelected,bool tiling,
						RLCFont* Active,RLCFont* Passive,RLCFont* Selected);
	UniversalButton* addTextureStrip(int x,int y,int Lx,int GP_File,int L,int C1,int C2,int C3,int R,bool Tiling);
};
//extern char* SoundID[MaxSnd];
int SearchStr(char** Res,char* s,int count);
#define mcmExit		0xFF
#define mcmSingle	0xF1
#define mcmMulti	0xF2
#define mcmVideo	0xF3
#define mcmResume	0xF4
#define mcmOk		0xF5
#define mcmCancel	0xF6
#define mcmLoad		0xF7
#define mcmSave		0xF8
#define mcmHost		0xF9
#define mcmJoin		0xFA
#define mcmRefresh	0xFB
#define mcmEdit		0xFC
#define mcmAll		0xFD
#define mcmGraph	0xE0
#define mcmSound	0xE1
#define mcmSpeed	0xE2
#define mcmSnWeapon 0xE3
#define mcmSnBirth	0xE4
#define mcmSnOrder	0xE5
#define mcmSnAttack 0xE6
#define mcmOptions	0xE7
#define mcmHelp		0xE8