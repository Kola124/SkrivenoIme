/*               Fast raster graphics routine
 *
 *  This module presents several useful procedures for fast output,
 * using RLC graphics format (see the discription below). Before the
 * wor you must call SetRlcWindow to set properly the screen
 * capabilities.After this you mast call SetScreenPtr to set the
 * output distanation. Uou can use LoadRLC to load the set of RLC
 * pictures into the RLCTable structure. Then you can draw the
 * picture on the screen wit the procedure ShowRLCItem
 */

#ifndef FASTDRAW_USER
#define FASTDRAW_API __declspec(dllexport)
#else
#define FASTDRAW_API __declspec(dllimport)
#endif

#ifndef _USE3D
extern FASTDRAW_API int WindX;
extern FASTDRAW_API int WindY;
extern FASTDRAW_API int WindLx;
extern FASTDRAW_API int WindLy;
extern FASTDRAW_API int WindX1;
extern FASTDRAW_API int WindY1;
#endif // !_USE3D

extern FASTDRAW_API int ScrWidth;
extern FASTDRAW_API int ScrHeight;
extern FASTDRAW_API int BytesPerPixel;
extern FASTDRAW_API void* ScreenPtr;
extern FASTDRAW_API void* RealScreenPtr;

struct RLCHeader
{
    short SizeX = 0;
    short SizeY = 0;
};
typedef RLCHeader* lpRLCHeader;
struct xRLCTable
{
    int Size = 0;
    int sign = 0;
    int SCount = 0;
    int OfsTable[32768000]; //int OfsTable[32768000];
};
typedef xRLCTable* RLCTable;
typedef RLCTable* lpRLCTable;
class FASTDRAW_API RLCFont
{
public:
    int FirstSymbol;
    int LastSymbol;
    int ShadowGP;
    RLCTable RLC;
    short ShadowDx;
    short ShadowDy;
    byte Options;
    
    RLCFont();
    RLCFont(char* Name);
    RLCFont(int GP_Index);
    
    // ADD THESE TWO:
    RLCFont(const RLCFont& other);
    RLCFont& operator=(const RLCFont& other);
    
    void SetGPIndex(int n);
    void SetRedColor();
    void SetWhiteColor();
    void SetBlackColor();
    void SetColorTable(int n);
    void SetStdShadow() {
        ShadowGP = int(RLC);
        ShadowDx = 1;
        ShadowDy = 1;
    };
    void SetShadowGP(int n, int dx, int dy) {
        ShadowGP = n;
        ShadowDx = dx;
        ShadowDy = dy;
    };
    ~RLCFont();

#ifdef _USE3D
private:
    DWORD			color;

public:
    void			SetColor(DWORD clr) { color = clr; }
    DWORD			GetColor() const { return color; }
#endif //_USE3D
};
typedef RLCFont* lpRLCFont;
CEXPORT void SetRLCWindow(int x, int y, int lx, int ly, int slx);
void SetScreenPtr(void);
int GetRLCWidth(RLCTable lpr, byte n);
CEXPORT
int GetRLCHeight(RLCTable lpr, byte n);
/*      Showing the RLC picture
 *
 *  This procedure needs window parameters to be initializated.
 */
FASTDRAW_API void ShowRLC(int x, int y, void* PicPtr);
//          Loading RLC file
FASTDRAW_API bool LoadRLC(LPCSTR lpFileName, RLCTable* RLCtbl);
//       Showing RLC item
FASTDRAW_API void ShowRLCItem(int x, int y, lpRLCTable lprt, int n, byte nt);
FASTDRAW_API void ShowRLCItemShadow(int x, int y, lpRLCTable lprt, int n);
FASTDRAW_API void ClearScreen();
FASTDRAW_API void ShowString(int x, int y, LPCSTR lps, lpRLCFont lpf);
FASTDRAW_API void RegisterRLCFont(lpRLCFont lrf, RLCTable lpr, int fir);
FASTDRAW_API void LoadPalettes();
FASTDRAW_API void StoreWindow();
FASTDRAW_API void RestoreWindow();
FASTDRAW_API void SetWind(int x, int y, int Lx, int Ly);
FASTDRAW_API int GetRLCStrWidth(char* str, lpRLCFont lpf);
FASTDRAW_API void CheckFontColor(lpRLCFont lpf);
FASTDRAW_API void LoadOptionalTable(int n, char* Name);
#define SIGNBYTE ''
FASTDRAW_API int GetRLCWidthUNICODE(RLCTable lpr, byte* strptr, int* L);
/*
 *                   Graphics routine
 *                 RLC-format interface
 *
 *RLC - description
 *
 *  unsigned int    Horisontal size of the picture
 *  unsigned short  Vertical size of the sub-picture
 *
 *    byte          Number of RLE substrings
 *
 *      byte        Length of the substring
 *
 *  byte[...]       Colors table
 */
