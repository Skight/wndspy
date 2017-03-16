#define INC_GLOBALVAR_CPP
#include "WndSpy.h"
#include "GlobalVar.h"

HINSTANCE g_hInstance;
HINSTANCE_LIBRARYS g_hInst={NULL,NULL,NULL,NULL};
HACCEL g_hAccel;

HWND g_hwnd, g_hwndBK, g_hwndTC=NULL;

#ifdef WNDSPY_MAINDLGTOOLTIP
HWND g_hwndTTMain;
#endif

HWND g_hwndTTFinder, g_hwndTTCalc, g_hwndTTGetIcon;
HICON g_hiconFinder, g_hiconCalc, g_hiconGetIcon;
HWND g_hwndTT[C_MAIN_TAB_PAGES];

HWND g_hwnd_TaskModal=NULL;
HWND g_hwnd_IconExtr=NULL;
HWND g_hwnd_AboutWS=NULL;
HWND g_hwnd_AppSetting=NULL;
HWND g_hwnd_SpyTarget=NULL;
HWND g_hwnd_EffectOption=NULL;
HWND g_hwndTool[C_TOOLS]={0};

DLGHDR g_TabDlgHdr = {0};

//////////////////////////////////////////////////////////////////////////

WS_APP_OPTIONS g_option = {0};

SHORT g_siWinVer=0;
BOOL g_isHookSignal;
BOOL g_IsFinding=FALSE;
BOOL g_IsIdle=TRUE;

DWORD g_dwBitFlag=0L;
//DWORD g_dwBuf=0L;
HANDLE g_hTarget=NULL;
//HMENU g_hPopMenu=NULL;

SPY_WININFOEX g_spyinfo_SWIex = {0};
SPY_PROCESSINFO g_spyinfo_SPI = {0};

TCHAR g_szLabelIdCtrl[LABEL_WINID_LEN]={0};
TCHAR g_szLabelIdMenu[LABEL_WINID_LEN]={0};
LPCTSTR g_szFormat=NULL;
const TCHAR *g_szVarFormat[]= {TEXT("%08X"), TEXT("0x%08X")};
TCHAR g_szSpyDllName[MAX_PATH]={0};
TCHAR g_strBuf[MAX_STRBUF_LEN];

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	TCHAR g_debugStr[1024*10];
#endif
//////////////////////////////////////////////////////////////////////////
LPFGETMODULEFILENAMEEX lpfGetModuleFileNameEx=NULL;
LPFSETLAYEREDWINDOWATTRIBUTES lpfSetLayeredWindowAttributes=NULL;


LPFSPYLIB_INITDATA lpfSpyLibInitHookData=NULL;
LPFSPYLIB_HOOKGETWNDINFO lpfSpyLibHookGetWndInfoEx=NULL;
LPFSPYLIB_BASEGETWNDINFO lpfSpyLibBaseGetWndInfoEx=NULL;
LPFSPYLIB_READWNDINFO lpfSpyLibReadWndInfoEx=NULL;
LPFSPYLIB_HOOKGETPSW lpfSpyLibHookGetPSW=NULL;

#ifdef WS_LIB_MAP_MSG_PARAM
LPFSPYLIB_CLEARHOOK lpfSpyLibClearHook=NULL;
LPFSPYLIB_GETDATAFROMSTR lpfGetDataFromStr=NULL;
LPFSPYLIB_MAPDATATOTARGET lpfSpyMapDataToTarget=NULL;
#endif
//////////////////////////////////////////////////////////////////////////

const SYMBOL_TABLE_ITEM ST_WndMsg[] =
{
Make_SymbolTableItem(ACM_OPEN),
Make_SymbolTableItem(ACM_PLAY),
Make_SymbolTableItem(ACM_STOP),
Make_SymbolTableItem(BM_CLICK),
Make_SymbolTableItem(BM_GETCHECK),
Make_SymbolTableItem(BM_GETIMAGE),
Make_SymbolTableItem(BM_GETSTATE),
Make_SymbolTableItem(BM_SETCHECK),
Make_SymbolTableItem(BM_SETIMAGE),
Make_SymbolTableItem(BM_SETSTATE),
Make_SymbolTableItem(BM_SETSTYLE),
Make_SymbolTableItem(CB_ADDSTRING),
Make_SymbolTableItem(CB_DELETESTRING),
Make_SymbolTableItem(CB_DIR),
Make_SymbolTableItem(CB_FINDSTRING),
Make_SymbolTableItem(CB_FINDSTRINGEXACT),
Make_SymbolTableItem(CB_GETCOUNT),
Make_SymbolTableItem(CB_GETCURSEL),
Make_SymbolTableItem(CB_GETDROPPEDCONTROLRECT),
Make_SymbolTableItem(CB_GETDROPPEDSTATE),
Make_SymbolTableItem(CB_GETDROPPEDWIDTH),
Make_SymbolTableItem(CB_GETEDITSEL),
Make_SymbolTableItem(CB_GETEXTENDEDUI),
Make_SymbolTableItem(CB_GETHORIZONTALEXTENT),
Make_SymbolTableItem(CB_GETITEMDATA),
Make_SymbolTableItem(CB_GETITEMHEIGHT),
Make_SymbolTableItem(CB_GETLBTEXT),
Make_SymbolTableItem(CB_GETLBTEXTLEN),
Make_SymbolTableItem(CB_GETLOCALE),
Make_SymbolTableItem(CB_GETMINVISIBLE),
Make_SymbolTableItem(CB_GETTOPINDEX),
Make_SymbolTableItem(CB_INSERTSTRING),
Make_SymbolTableItem(CB_LIMITTEXT),
Make_SymbolTableItem(CB_RESETCONTENT),
Make_SymbolTableItem(CB_SELECTSTRING),
Make_SymbolTableItem(CB_SETCURSEL),
Make_SymbolTableItem(CB_SETDROPPEDWIDTH),
Make_SymbolTableItem(CB_SETEDITSEL),
Make_SymbolTableItem(CB_SETEXTENDEDUI),
Make_SymbolTableItem(CB_SETHORIZONTALEXTENT),
Make_SymbolTableItem(CB_SETITEMDATA),
Make_SymbolTableItem(CB_SETITEMHEIGHT),
Make_SymbolTableItem(CB_SETLOCALE),
Make_SymbolTableItem(CB_SETTOPINDEX),
Make_SymbolTableItem(CB_SETMINVISIBLE),
Make_SymbolTableItem(CB_SHOWDROPDOWN),
Make_SymbolTableItem(DM_GETDEFID),
Make_SymbolTableItem(DM_REPOSITION),
Make_SymbolTableItem(DM_SETDEFID),
Make_SymbolTableItem(EM_CANUNDO),
Make_SymbolTableItem(EM_CHARFROMPOS),
Make_SymbolTableItem(EM_EMPTYUNDOBUFFER),
Make_SymbolTableItem(EM_FMTLINES),
Make_SymbolTableItem(EM_GETFIRSTVISIBLELINE),
Make_SymbolTableItem(EM_GETLIMITTEXT),
Make_SymbolTableItem(EM_GETLINE),
Make_SymbolTableItem(EM_GETLINECOUNT),
Make_SymbolTableItem(EM_GETMARGINS),
Make_SymbolTableItem(EM_GETMODIFY),
Make_SymbolTableItem(EM_GETPASSWORDCHAR),
Make_SymbolTableItem(EM_GETRECT),
Make_SymbolTableItem(EM_GETSEL),
Make_SymbolTableItem(EM_GETTHUMB),
Make_SymbolTableItem(EM_GETWORDBREAKPROC),
Make_SymbolTableItem(EM_LIMITTEXT),
Make_SymbolTableItem(EM_LINEFROMCHAR),
Make_SymbolTableItem(EM_LINEINDEX),
Make_SymbolTableItem(EM_LINELENGTH),
Make_SymbolTableItem(EM_LINESCROLL),
Make_SymbolTableItem(EM_POSFROMCHAR),
Make_SymbolTableItem(EM_REPLACESEL),
Make_SymbolTableItem(EM_REDO),
Make_SymbolTableItem(EM_SCROLL),
Make_SymbolTableItem(EM_SCROLLCARET),
Make_SymbolTableItem(EM_SETLIMITTEXT),
Make_SymbolTableItem(EM_SETMARGINS),
Make_SymbolTableItem(EM_SETMODIFY),
Make_SymbolTableItem(EM_SETPASSWORDCHAR),
Make_SymbolTableItem(EM_SETREADONLY),
Make_SymbolTableItem(EM_SETRECT),
Make_SymbolTableItem(EM_SETRECTNP),
Make_SymbolTableItem(EM_SETSEL),
Make_SymbolTableItem(EM_UNDO),
Make_SymbolTableItem(HDM_DELETEITEM),
Make_SymbolTableItem(HDM_GETITEM),
Make_SymbolTableItem(HDM_GETITEMCOUNT),
Make_SymbolTableItem(HDM_INSERTITEM),
Make_SymbolTableItem(HDM_LAYOUT),
Make_SymbolTableItem(HDM_SETITEM),
Make_SymbolTableItem(HKM_GETHOTKEY),
Make_SymbolTableItem(HKM_SETHOTKEY),
Make_SymbolTableItem(HKM_SETRULES),
Make_SymbolTableItem(LB_ADDFILE),
Make_SymbolTableItem(LB_ADDSTRING),
Make_SymbolTableItem(LB_DELETESTRING),
Make_SymbolTableItem(LB_DIR),
Make_SymbolTableItem(LB_FINDSTRING),
Make_SymbolTableItem(LB_FINDSTRINGEXACT),
Make_SymbolTableItem(LB_GETANCHORINDEX),
Make_SymbolTableItem(LB_GETCARETINDEX),
Make_SymbolTableItem(LB_GETCOUNT),
Make_SymbolTableItem(LB_GETCURSEL),
Make_SymbolTableItem(LB_GETHORIZONTALEXTENT),
Make_SymbolTableItem(LB_GETITEMDATA),
Make_SymbolTableItem(LB_GETITEMHEIGHT),
Make_SymbolTableItem(LB_GETITEMRECT),
Make_SymbolTableItem(LB_GETLOCALE),
Make_SymbolTableItem(LB_GETSEL),
Make_SymbolTableItem(LB_GETSELCOUNT),
Make_SymbolTableItem(LB_GETSELITEMS),
Make_SymbolTableItem(LB_GETTEXT),
Make_SymbolTableItem(LB_GETTEXTLEN),
Make_SymbolTableItem(LB_GETTOPINDEX),
Make_SymbolTableItem(LB_INSERTSTRING),
Make_SymbolTableItem(LB_ITEMFROMPOINT),
Make_SymbolTableItem(LB_RESETCONTENT),
Make_SymbolTableItem(LB_SELECTSTRING),
Make_SymbolTableItem(LB_SELITEMRANGE),
Make_SymbolTableItem(LB_SELITEMRANGEEX),
Make_SymbolTableItem(LB_SETANCHORINDEX),
Make_SymbolTableItem(LB_SETCARETINDEX),
Make_SymbolTableItem(LB_SETCOLUMNWIDTH),
Make_SymbolTableItem(LB_SETCOUNT),
Make_SymbolTableItem(LB_SETCURSEL),
Make_SymbolTableItem(LB_SETHORIZONTALEXTENT),
Make_SymbolTableItem(LB_SETITEMDATA),
Make_SymbolTableItem(LB_SETITEMHEIGHT),
Make_SymbolTableItem(LB_SETLOCALE),
Make_SymbolTableItem(LB_SETSEL),
Make_SymbolTableItem(LB_SETTOPINDEX),
Make_SymbolTableItem(LVM_APPROXIMATEVIEWRECT),
Make_SymbolTableItem(LVM_ARRANGE),
Make_SymbolTableItem(LVM_CREATEDRAGIMAGE),
Make_SymbolTableItem(LVM_DELETEALLITEMS),
Make_SymbolTableItem(LVM_DELETECOLUMN),
Make_SymbolTableItem(LVM_DELETEITEM),
Make_SymbolTableItem(LVM_EDITLABEL),
Make_SymbolTableItem(LVM_ENSUREVISIBLE),
Make_SymbolTableItem(LVM_FINDITEM),
Make_SymbolTableItem(LVM_GETBKCOLOR),
Make_SymbolTableItem(LVM_GETCALLBACKMASK),
Make_SymbolTableItem(LVM_GETCOLUMN),
Make_SymbolTableItem(LVM_GETCOLUMNORDERARRAY),
Make_SymbolTableItem(LVM_GETCOLUMNWIDTH),
Make_SymbolTableItem(LVM_GETCOUNTPERPAGE),
Make_SymbolTableItem(LVM_GETEDITCONTROL),
Make_SymbolTableItem(LVM_GETEXTENDEDLISTVIEWSTYLE),
Make_SymbolTableItem(LVM_GETHOTCURSOR),
Make_SymbolTableItem(LVM_GETHOTITEM),
Make_SymbolTableItem(LVM_GETIMAGELIST),
Make_SymbolTableItem(LVM_GETISEARCHSTRING),
Make_SymbolTableItem(LVM_GETITEM),
Make_SymbolTableItem(LVM_GETITEMCOUNT),
Make_SymbolTableItem(LVM_GETITEMPOSITION),
Make_SymbolTableItem(LVM_GETITEMRECT),
Make_SymbolTableItem(LVM_GETITEMSPACING),
Make_SymbolTableItem(LVM_GETITEMSTATE),
Make_SymbolTableItem(LVM_GETITEMTEXT),
Make_SymbolTableItem(LVM_GETNEXTITEM),
Make_SymbolTableItem(LVM_GETORIGIN),
Make_SymbolTableItem(LVM_GETOUTLINECOLOR),
Make_SymbolTableItem(LVM_GETSELECTEDCOUNT),
Make_SymbolTableItem(LVM_GETSTRINGWIDTH),
Make_SymbolTableItem(LVM_GETSUBITEMRECT),
Make_SymbolTableItem(LVM_GETTEXTBKCOLOR),
Make_SymbolTableItem(LVM_GETTEXTCOLOR),
Make_SymbolTableItem(LVM_GETTOPINDEX),
Make_SymbolTableItem(LVM_GETVIEWRECT),
Make_SymbolTableItem(LVM_INSERTCOLUMN),
Make_SymbolTableItem(LVM_INSERTGROUP),
Make_SymbolTableItem(LVM_INSERTITEM),
Make_SymbolTableItem(LVM_MOVEGROUP),
Make_SymbolTableItem(LVM_REDRAWITEMS),
Make_SymbolTableItem(LVM_SCROLL),
Make_SymbolTableItem(LVM_SETBKCOLOR),
Make_SymbolTableItem(LVM_SETCALLBACKMASK),
Make_SymbolTableItem(LVM_SETCOLUMN),
Make_SymbolTableItem(LVM_SETCOLUMNORDERARRAY),
Make_SymbolTableItem(LVM_SETCOLUMNWIDTH),
Make_SymbolTableItem(LVM_SETEXTENDEDLISTVIEWSTYLE),
Make_SymbolTableItem(LVM_SETHOTCURSOR),
Make_SymbolTableItem(LVM_SETHOTITEM),
Make_SymbolTableItem(LVM_SETICONSPACING),
Make_SymbolTableItem(LVM_SETIMAGELIST),
Make_SymbolTableItem(LVM_SETITEM),
Make_SymbolTableItem(LVM_SETITEMCOUNT),
Make_SymbolTableItem(LVM_SETITEMPOSITION),
Make_SymbolTableItem(LVM_SETITEMSTATE),
Make_SymbolTableItem(LVM_SETITEMTEXT),
Make_SymbolTableItem(LVM_SETOUTLINECOLOR),
Make_SymbolTableItem(LVM_SETTEXTBKCOLOR),
Make_SymbolTableItem(LVM_SETTEXTCOLOR),
Make_SymbolTableItem(LVM_UPDATE),
Make_SymbolTableItem(PBM_DELTAPOS),
Make_SymbolTableItem(PBM_GETPOS),
Make_SymbolTableItem(PBM_GETRANGE),
Make_SymbolTableItem(PBM_SETPOS),
Make_SymbolTableItem(PBM_SETRANGE),
Make_SymbolTableItem(PBM_SETRANGE32),
Make_SymbolTableItem(PBM_SETSTEP),
Make_SymbolTableItem(PBM_STEPIT),
Make_SymbolTableItem(SB_GETBORDERS),
Make_SymbolTableItem(SB_GETPARTS),
Make_SymbolTableItem(SB_GETRECT),
Make_SymbolTableItem(SB_GETTEXT),
Make_SymbolTableItem(SB_GETTEXTLENGTH),
Make_SymbolTableItem(SB_SETMINHEIGHT),
Make_SymbolTableItem(SB_SETPARTS),
Make_SymbolTableItem(SB_SETTEXT),
Make_SymbolTableItem(SB_SIMPLE),
Make_SymbolTableItem(SBM_ENABLE_ARROWS),
Make_SymbolTableItem(SBM_GETPOS),
Make_SymbolTableItem(SBM_GETRANGE),
Make_SymbolTableItem(SBM_GETSCROLLINFO),
Make_SymbolTableItem(SBM_SETPOS),
Make_SymbolTableItem(SBM_SETRANGE),
Make_SymbolTableItem(SBM_SETRANGEREDRAW),
Make_SymbolTableItem(SBM_SETSCROLLINFO),
Make_SymbolTableItem(STM_GETICON),
Make_SymbolTableItem(STM_GETIMAGE),
Make_SymbolTableItem(STM_SETICON),
Make_SymbolTableItem(STM_SETIMAGE),
Make_SymbolTableItem(TB_ADDBITMAP),
Make_SymbolTableItem(TB_ADDBUTTONS),
Make_SymbolTableItem(TB_ADDSTRING),
Make_SymbolTableItem(TB_AUTOSIZE),
Make_SymbolTableItem(TB_BUTTONCOUNT),
Make_SymbolTableItem(TB_BUTTONSTRUCTSIZE),
Make_SymbolTableItem(TB_CHANGEBITMAP),
Make_SymbolTableItem(TB_CHECKBUTTON),
Make_SymbolTableItem(TB_COMMANDTOINDEX),
Make_SymbolTableItem(TB_CUSTOMIZE),
Make_SymbolTableItem(TB_DELETEBUTTON),
Make_SymbolTableItem(TB_ENABLEBUTTON),
Make_SymbolTableItem(TB_GETBITMAP),
Make_SymbolTableItem(TB_GETBITMAPFLAGS),
Make_SymbolTableItem(TB_GETBUTTON),
Make_SymbolTableItem(TB_GETBUTTONSIZE),
Make_SymbolTableItem(TB_GETBUTTONTEXT),
Make_SymbolTableItem(TB_GETDISABLEDIMAGELIST),
Make_SymbolTableItem(TB_GETHOTIMAGELIST),
Make_SymbolTableItem(TB_GETIMAGELIST),
Make_SymbolTableItem(TB_GETITEMRECT),
Make_SymbolTableItem(TB_GETRECT),
Make_SymbolTableItem(TB_GETROWS),
Make_SymbolTableItem(TB_GETSTATE),
Make_SymbolTableItem(TB_GETSTYLE),
Make_SymbolTableItem(TB_GETTEXTROWS),
Make_SymbolTableItem(TB_GETTOOLTIPS),
Make_SymbolTableItem(TB_HIDEBUTTON),
Make_SymbolTableItem(TB_INDETERMINATE),
Make_SymbolTableItem(TB_INSERTBUTTON),
Make_SymbolTableItem(TB_ISBUTTONCHECKED),
Make_SymbolTableItem(TB_ISBUTTONHIDDEN),
Make_SymbolTableItem(TB_ISBUTTONINDETERMINATE),
Make_SymbolTableItem(TB_ISBUTTONPRESSED),
Make_SymbolTableItem(TB_LOADIMAGES),
Make_SymbolTableItem(TB_MOVEBUTTON),
Make_SymbolTableItem(TB_PRESSBUTTON),
Make_SymbolTableItem(TB_SETBITMAPSIZE),
Make_SymbolTableItem(TB_SETBUTTONSIZE),
Make_SymbolTableItem(TB_SETBUTTONWIDTH),
Make_SymbolTableItem(TB_SETCMDID),
Make_SymbolTableItem(TB_SETDISABLEDIMAGELIST),
Make_SymbolTableItem(TB_SETHOTIMAGELIST),
Make_SymbolTableItem(TB_SETIMAGELIST),
Make_SymbolTableItem(TB_SETINDENT),
Make_SymbolTableItem(TB_SETMAXTEXTROWS),
Make_SymbolTableItem(TB_SETPARENT),
Make_SymbolTableItem(TB_SETROWS),
Make_SymbolTableItem(TB_SETSTATE),
Make_SymbolTableItem(TB_SETSTYLE),
Make_SymbolTableItem(TB_SETTOOLTIPS),
Make_SymbolTableItem(TCM_ADJUSTRECT),
Make_SymbolTableItem(TCM_DELETEALLITEMS),
Make_SymbolTableItem(TCM_DELETEITEM),
Make_SymbolTableItem(TCM_DESELECTALL),
Make_SymbolTableItem(TCM_GETEXTENDEDSTYLE),
Make_SymbolTableItem(TCM_GETCURFOCUS),
Make_SymbolTableItem(TCM_GETCURSEL),
Make_SymbolTableItem(TCM_GETIMAGELIST),
Make_SymbolTableItem(TCM_GETITEM),
Make_SymbolTableItem(TCM_GETITEMCOUNT),
Make_SymbolTableItem(TCM_GETITEMRECT),
Make_SymbolTableItem(TCM_GETROWCOUNT),
Make_SymbolTableItem(TCM_GETTOOLTIPS),
Make_SymbolTableItem(TCM_HIGHLIGHTITEM),
Make_SymbolTableItem(TCM_INSERTITEM),
Make_SymbolTableItem(TCM_REMOVEIMAGE),
Make_SymbolTableItem(TCM_SETCURFOCUS),
Make_SymbolTableItem(TCM_SETCURSEL),
Make_SymbolTableItem(TCM_SETIMAGELIST),
Make_SymbolTableItem(TCM_SETITEM),
Make_SymbolTableItem(TCM_SETITEMEXTRA),
Make_SymbolTableItem(TCM_SETITEMSIZE),
Make_SymbolTableItem(TCM_SETPADDING),
Make_SymbolTableItem(TCM_SETTOOLTIPS),
Make_SymbolTableItem(TTM_ACTIVATE),
Make_SymbolTableItem(TTM_ADDTOOL),
Make_SymbolTableItem(TTM_DELTOOL),
Make_SymbolTableItem(TTM_ENUMTOOLS),
Make_SymbolTableItem(TTM_GETCURRENTTOOL),
Make_SymbolTableItem(TTM_GETDELAYTIME),
Make_SymbolTableItem(TTM_GETMARGIN),
Make_SymbolTableItem(TTM_GETMAXTIPWIDTH),
Make_SymbolTableItem(TTM_GETTEXT),
Make_SymbolTableItem(TTM_GETTIPBKCOLOR),
Make_SymbolTableItem(TTM_GETTIPTEXTCOLOR),
Make_SymbolTableItem(TTM_GETTITLE),
Make_SymbolTableItem(TTM_GETTOOLCOUNT),
Make_SymbolTableItem(TTM_GETTOOLINFO),
Make_SymbolTableItem(TTM_POP),
Make_SymbolTableItem(TTM_POPUP),
Make_SymbolTableItem(TTM_SETDELAYTIME),
Make_SymbolTableItem(TTM_SETMARGIN),
Make_SymbolTableItem(TTM_SETMAXTIPWIDTH),
Make_SymbolTableItem(TTM_SETTIPBKCOLOR),
Make_SymbolTableItem(TTM_SETTIPTEXTCOLOR),
Make_SymbolTableItem(TTM_SETTITLE),
Make_SymbolTableItem(TTM_SETTOOLINFO),
Make_SymbolTableItem(TTM_UPDATETIPTEXT),
Make_SymbolTableItem(TTM_WINDOWFROMPOINT),
Make_SymbolTableItem(TVM_DELETEITEM),
Make_SymbolTableItem(TVM_EDITLABEL),
Make_SymbolTableItem(TVM_ENDEDITLABELNOW),
Make_SymbolTableItem(TVM_ENSUREVISIBLE),
Make_SymbolTableItem(TVM_EXPAND),
Make_SymbolTableItem(TVM_GETBKCOLOR),
Make_SymbolTableItem(TVM_GETTEXTCOLOR),
Make_SymbolTableItem(TVM_GETCOUNT),
Make_SymbolTableItem(TVM_GETEDITCONTROL),
Make_SymbolTableItem(TVM_GETIMAGELIST),
Make_SymbolTableItem(TVM_GETINDENT),
Make_SymbolTableItem(TVM_GETISEARCHSTRING),
Make_SymbolTableItem(TVM_GETITEM),
Make_SymbolTableItem(TVM_GETITEMRECT),
Make_SymbolTableItem(TVM_GETNEXTITEM),
Make_SymbolTableItem(TVM_GETVISIBLECOUNT),
Make_SymbolTableItem(TVM_INSERTITEM),
Make_SymbolTableItem(TVM_SELECTITEM),
Make_SymbolTableItem(TVM_SETIMAGELIST),
Make_SymbolTableItem(TVM_SETINDENT),
Make_SymbolTableItem(TVM_SETITEM),
Make_SymbolTableItem(TVM_SORTCHILDREN),
Make_SymbolTableItem(TVM_SETBKCOLOR),
Make_SymbolTableItem(TVM_SETTEXTCOLOR),
Make_SymbolTableItem(UDM_GETACCEL),
Make_SymbolTableItem(UDM_GETBASE),
Make_SymbolTableItem(UDM_GETBUDDY),
Make_SymbolTableItem(UDM_GETPOS),
Make_SymbolTableItem(UDM_GETRANGE),
Make_SymbolTableItem(UDM_SETACCEL),
Make_SymbolTableItem(UDM_SETBASE),
Make_SymbolTableItem(UDM_SETBUDDY),
Make_SymbolTableItem(UDM_SETPOS),
Make_SymbolTableItem(UDM_SETRANGE),
Make_SymbolTableItem(WM_ACTIVATE),
Make_SymbolTableItem(WM_ACTIVATEAPP),
Make_SymbolTableItem(WM_APPCOMMAND),
Make_SymbolTableItem(WM_CANCELJOURNAL),
Make_SymbolTableItem(WM_CANCELMODE),
Make_SymbolTableItem(WM_CAPTURECHANGED),
Make_SymbolTableItem(WM_CHANGEUISTATE),
Make_SymbolTableItem(WM_CHAR),
Make_SymbolTableItem(WM_CHARTOITEM),
Make_SymbolTableItem(WM_CHILDACTIVATE),
Make_SymbolTableItem(WM_CLEAR),
Make_SymbolTableItem(WM_CLOSE),
Make_SymbolTableItem(WM_COMMAND),
Make_SymbolTableItem(WM_COMPACTING),
Make_SymbolTableItem(WM_COMPAREITEM),
Make_SymbolTableItem(WM_CONTEXTMENU),
Make_SymbolTableItem(WM_COPY),
Make_SymbolTableItem(WM_COPYDATA),
Make_SymbolTableItem(WM_CTLCOLOR),
Make_SymbolTableItem(WM_CTLCOLORBTN),
Make_SymbolTableItem(WM_CTLCOLORDLG),
Make_SymbolTableItem(WM_CTLCOLOREDIT),
Make_SymbolTableItem(WM_CTLCOLORLISTBOX),
Make_SymbolTableItem(WM_CTLCOLORSCROLLBAR),
Make_SymbolTableItem(WM_CTLCOLORSTATIC),
Make_SymbolTableItem(WM_CUT),
Make_SymbolTableItem(WM_DEADCHAR),
Make_SymbolTableItem(WM_DELETEITEM),
Make_SymbolTableItem(WM_DESTROY),
Make_SymbolTableItem(WM_DEVICECHANGE),
Make_SymbolTableItem(WM_DEVMODECHANGE),
Make_SymbolTableItem(WM_DISABLEMODAL),
Make_SymbolTableItem(WM_DISPLAYCHANGE),
Make_SymbolTableItem(WM_DRAWITEM),
Make_SymbolTableItem(WM_DROPFILES),
Make_SymbolTableItem(WM_ENABLE),
Make_SymbolTableItem(WM_ENDSESSION),
Make_SymbolTableItem(WM_ENTERIDLE),
Make_SymbolTableItem(WM_ENTERMENULOOP),
Make_SymbolTableItem(WM_ENTERSIZEMOVE),
Make_SymbolTableItem(WM_ERASEBKGND),
Make_SymbolTableItem(WM_EXITHELPMODE),
Make_SymbolTableItem(WM_EXITMENULOOP),
Make_SymbolTableItem(WM_EXITSIZEMOVE),
Make_SymbolTableItem(WM_FLOATSTATUS),
Make_SymbolTableItem(WM_FONTCHANGE),
Make_SymbolTableItem(WM_GETDLGCODE),
Make_SymbolTableItem(WM_GETFONT),
Make_SymbolTableItem(WM_GETHOTKEY),
Make_SymbolTableItem(WM_GETICON),
Make_SymbolTableItem(WM_GETMINMAXINFO),
Make_SymbolTableItem(WM_GETOBJECT),
Make_SymbolTableItem(WM_GETTEXT),
Make_SymbolTableItem(WM_GETTEXTLENGTH),
Make_SymbolTableItem(WM_HELP),
Make_SymbolTableItem(WM_HOTKEY),
Make_SymbolTableItem(WM_HSCROLL),
Make_SymbolTableItem(WM_IDLEUPDATECMDUI),
Make_SymbolTableItem(WM_IME_CHAR),
Make_SymbolTableItem(WM_IME_COMPOSITION),
Make_SymbolTableItem(WM_IME_COMPOSITIONFULL),
Make_SymbolTableItem(WM_IME_CONTROL),
Make_SymbolTableItem(WM_IME_ENDCOMPOSITION),
Make_SymbolTableItem(WM_IME_KEYDOWN),
Make_SymbolTableItem(WM_IME_KEYUP),
Make_SymbolTableItem(WM_IME_NOTIFY),
Make_SymbolTableItem(WM_IME_SELECT),
Make_SymbolTableItem(WM_IME_SETCONTEXT),
Make_SymbolTableItem(WM_IME_STARTCOMPOSITION),
Make_SymbolTableItem(WM_INITIALUPDATE),
Make_SymbolTableItem(WM_INITMENU),
Make_SymbolTableItem(WM_INITMENUPOPUP),
Make_SymbolTableItem(WM_INPUTLANGCHANGE),
Make_SymbolTableItem(WM_INPUTLANGCHANGEREQUEST),
Make_SymbolTableItem(WM_KEYDOWN),
Make_SymbolTableItem(WM_KEYUP),
Make_SymbolTableItem(WM_KICKIDLE),
Make_SymbolTableItem(WM_KILLFOCUS),
Make_SymbolTableItem(WM_LBUTTONDBLCLK),
Make_SymbolTableItem(WM_LBUTTONDOWN),
Make_SymbolTableItem(WM_LBUTTONUP),
Make_SymbolTableItem(WM_MBUTTONDBLCLK),
Make_SymbolTableItem(WM_MBUTTONDOWN),
Make_SymbolTableItem(WM_MBUTTONUP),
Make_SymbolTableItem(WM_MDIACTIVATE),
Make_SymbolTableItem(WM_MDICASCADE),
Make_SymbolTableItem(WM_MDICREATE),
Make_SymbolTableItem(WM_MDIDESTROY),
Make_SymbolTableItem(WM_MDIGETACTIVE),
Make_SymbolTableItem(WM_MDIICONARRANGE),
Make_SymbolTableItem(WM_MDIMAXIMIZE),
Make_SymbolTableItem(WM_MDINEXT),
Make_SymbolTableItem(WM_MDIREFRESHMENU),
Make_SymbolTableItem(WM_MDIRESTORE),
Make_SymbolTableItem(WM_MDISETMENU),
Make_SymbolTableItem(WM_MDITILE),
Make_SymbolTableItem(WM_MEASUREITEM),
Make_SymbolTableItem(WM_MENUCHAR),
Make_SymbolTableItem(WM_MENUCOMMAND),
Make_SymbolTableItem(WM_MENUDRAG),
Make_SymbolTableItem(WM_MENUGETOBJECT),
Make_SymbolTableItem(WM_MENURBUTTONUP),
Make_SymbolTableItem(WM_MENUSELECT),
Make_SymbolTableItem(WM_MOUSEACTIVATE),
Make_SymbolTableItem(WM_MOUSEHOVER),
Make_SymbolTableItem(WM_MOUSELEAVE),
Make_SymbolTableItem(WM_MOUSEMOVE),
Make_SymbolTableItem(WM_MOUSEWHEEL),
Make_SymbolTableItem(WM_MOVE),
Make_SymbolTableItem(WM_MOVING),
Make_SymbolTableItem(WM_NCACTIVATE),
Make_SymbolTableItem(WM_NCCALCSIZE),
Make_SymbolTableItem(WM_NCCREATE),
Make_SymbolTableItem(WM_NCDESTROY),
Make_SymbolTableItem(WM_NCHITTEST),
Make_SymbolTableItem(WM_NCLBUTTONDBLCLK),
Make_SymbolTableItem(WM_NCLBUTTONDOWN),
Make_SymbolTableItem(WM_NCLBUTTONUP),
Make_SymbolTableItem(WM_NCMBUTTONDBLCLK),
Make_SymbolTableItem(WM_NCMBUTTONDOWN),
Make_SymbolTableItem(WM_NCMBUTTONUP),
Make_SymbolTableItem(WM_NCMOUSELEAVE),
Make_SymbolTableItem(WM_NCMOUSEHOVER),
Make_SymbolTableItem(WM_NCMOUSEMOVE),
Make_SymbolTableItem(WM_NCPAINT),
Make_SymbolTableItem(WM_NCRBUTTONDBLCLK),
Make_SymbolTableItem(WM_NCRBUTTONDOWN),
Make_SymbolTableItem(WM_NCRBUTTONUP),
Make_SymbolTableItem(WM_NCXBUTTONDBLCLK),
Make_SymbolTableItem(WM_NCXBUTTONDOWN),
Make_SymbolTableItem(WM_NCXBUTTONUP),
Make_SymbolTableItem(WM_NEXTDLGCTL),
Make_SymbolTableItem(WM_NEXTMENU),
Make_SymbolTableItem(WM_NOTIFY),
Make_SymbolTableItem(WM_NOTIFYFORMAT),
Make_SymbolTableItem(WM_PAINT),
Make_SymbolTableItem(WM_PAINTICON),
Make_SymbolTableItem(WM_PALETTECHANGED),
Make_SymbolTableItem(WM_PALETTEISCHANGING),
Make_SymbolTableItem(WM_PARENTNOTIFY),
Make_SymbolTableItem(WM_PASTE),
Make_SymbolTableItem(WM_POPMESSAGESTRING),
Make_SymbolTableItem(WM_POWER),
Make_SymbolTableItem(WM_POWERBROADCAST),
Make_SymbolTableItem(WM_PRINT),
Make_SymbolTableItem(WM_PRINTCLIENT),
Make_SymbolTableItem(WM_QUERY3DCONTROLS),
Make_SymbolTableItem(WM_QUERYAFXWNDPROC),
Make_SymbolTableItem(WM_QUERYCENTERWND),
Make_SymbolTableItem(WM_QUERYDRAGICON),
Make_SymbolTableItem(WM_QUERYENDSESSION),
Make_SymbolTableItem(WM_QUERYNEWPALETTE),
Make_SymbolTableItem(WM_QUERYOPEN),
Make_SymbolTableItem(WM_QUERYUISTATE),
Make_SymbolTableItem(WM_QUEUE_SENTINEL),
Make_SymbolTableItem(WM_QUEUESYNC),
Make_SymbolTableItem(WM_QUIT),
Make_SymbolTableItem(WM_RBUTTONDBLCLK),
Make_SymbolTableItem(WM_RBUTTONDOWN),
Make_SymbolTableItem(WM_RBUTTONUP),
Make_SymbolTableItem(WM_RECALCPARENT),
Make_SymbolTableItem(WM_RENDERALLFORMATS),
Make_SymbolTableItem(WM_RENDERFORMAT),
Make_SymbolTableItem(WM_SETCURSOR),
Make_SymbolTableItem(WM_SETFOCUS),
Make_SymbolTableItem(WM_SETFONT),
Make_SymbolTableItem(WM_SETHOTKEY),
Make_SymbolTableItem(WM_SETICON),
Make_SymbolTableItem(WM_SETMESSAGESTRING),
Make_SymbolTableItem(WM_SETREDRAW),
Make_SymbolTableItem(WM_SETTEXT),
Make_SymbolTableItem(WM_SETTINGCHANGE),
Make_SymbolTableItem(WM_SHOWWINDOW),
Make_SymbolTableItem(WM_SIZE),
Make_SymbolTableItem(WM_SIZECHILD),
Make_SymbolTableItem(WM_SIZEPARENT),
Make_SymbolTableItem(WM_SIZING),
Make_SymbolTableItem(WM_SPOOLERSTATUS),
Make_SymbolTableItem(WM_STYLECHANGED),
Make_SymbolTableItem(WM_STYLECHANGING),
Make_SymbolTableItem(WM_SYNCPAINT),
Make_SymbolTableItem(WM_SYSCHAR),
Make_SymbolTableItem(WM_SYSCOLORCHANGE),
Make_SymbolTableItem(WM_SYSCOMMAND),
Make_SymbolTableItem(WM_SYSDEADCHAR),
Make_SymbolTableItem(WM_SYSKEYDOWN),
Make_SymbolTableItem(WM_SYSKEYUP),
Make_SymbolTableItem(WM_TCARD),
Make_SymbolTableItem(WM_THEMECHANGED),
Make_SymbolTableItem(WM_TIMECHANGE),
Make_SymbolTableItem(WM_TIMER),
Make_SymbolTableItem(WM_UNDO),
Make_SymbolTableItem(WM_UNICHAR),
Make_SymbolTableItem(WM_UNINITMENUPOPUP),
Make_SymbolTableItem(WM_UPDATEUISTATE),
Make_SymbolTableItem(WM_USERCHANGED),
Make_SymbolTableItem(WM_VKEYTOITEM),
Make_SymbolTableItem(WM_VSCROLL),
Make_SymbolTableItem(WM_WINDOWPOSCHANGED),
Make_SymbolTableItem(WM_WINDOWPOSCHANGING),
Make_SymbolTableItem(WM_WININICHANGE),
Make_SymbolTableItem(WM_XBUTTONDBLCLK),
Make_SymbolTableItem(WM_XBUTTONDOWN),
Make_SymbolTableItem(WM_XBUTTONUP),
End_SymbolTable()
};

const SYMBOL_TABLE_ITEM ST_SysCursors[] =
{
Make_SymbolTableItem(IDC_ARROW),
Make_SymbolTableItem(IDC_IBEAM),
Make_SymbolTableItem(IDC_WAIT),
Make_SymbolTableItem(IDC_CROSS),
Make_SymbolTableItem(IDC_UPARROW),
Make_SymbolTableItem(IDC_SIZEALL),
Make_SymbolTableItem(IDC_SIZENWSE),
Make_SymbolTableItem(IDC_SIZENESW),
Make_SymbolTableItem(IDC_SIZENS),
Make_SymbolTableItem(IDC_SIZEWE),
Make_SymbolTableItem(IDC_HAND),
Make_SymbolTableItem(IDC_APPSTARTING),
Make_SymbolTableItem(IDC_HELP),
Make_SymbolTableItem(IDC_NO),
End_SymbolTable()
};


const SYMBOL_TABLE_ITEM ST_BrushColors[] =
{
Make_SymbolTableItem(COLOR_SCROLLBAR),
{(LONG)COLOR_BACKGROUND,TEXT("COLOR_BACKGROUND(COLOR_DESKTOP)")},
Make_SymbolTableItem(COLOR_ACTIVECAPTION),
Make_SymbolTableItem(COLOR_INACTIVECAPTION),
Make_SymbolTableItem(COLOR_MENU),
Make_SymbolTableItem(COLOR_WINDOW),
Make_SymbolTableItem(COLOR_WINDOWFRAME),
Make_SymbolTableItem(COLOR_MENUTEXT),
Make_SymbolTableItem(COLOR_WINDOWTEXT),
Make_SymbolTableItem(COLOR_CAPTIONTEXT),
Make_SymbolTableItem(COLOR_ACTIVEBORDER),
Make_SymbolTableItem(COLOR_INACTIVEBORDER),
Make_SymbolTableItem(COLOR_APPWORKSPACE),
Make_SymbolTableItem(COLOR_HIGHLIGHT),
Make_SymbolTableItem(COLOR_HIGHLIGHTTEXT),
Make_SymbolTableItem(COLOR_BTNFACE),
Make_SymbolTableItem(COLOR_BTNSHADOW),
Make_SymbolTableItem(COLOR_GRAYTEXT),
Make_SymbolTableItem(COLOR_BTNTEXT),
End_SymbolTable()
};


//////////////////////////////////////////////////////////////////////////

/*
 * Class styles
 */

const SYMBOL_TABLE_ITEM ST_ClassStyle[] =
{
Make_SymbolTableItem(CS_VREDRAW),
Make_SymbolTableItem(CS_HREDRAW),
Make_SymbolTableItem(CS_DBLCLKS),
Make_SymbolTableItem(CS_OWNDC),
Make_SymbolTableItem(CS_CLASSDC),
Make_SymbolTableItem(CS_PARENTDC),
Make_SymbolTableItem(CS_NOCLOSE),
Make_SymbolTableItem(CS_SAVEBITS),
Make_SymbolTableItem(CS_BYTEALIGNCLIENT),
Make_SymbolTableItem(CS_BYTEALIGNWINDOW),
Make_SymbolTableItem(CS_GLOBALCLASS),
Make_SymbolTableItem(CS_IME),
Make_SymbolTableItem(CS_DROPSHADOW),
End_SymbolTable()
};



/*
 * Window Styles
 */
const SYMBOL_TABLE_ITEM ST_WindowStyle[] =
{
Make_SymbolTableItem(WS_POPUP),
Make_SymbolTableItem(WS_CHILD),
Make_SymbolTableItem(WS_MINIMIZE),
Make_SymbolTableItem(WS_VISIBLE),
Make_SymbolTableItem(WS_DISABLED),
Make_SymbolTableItem(WS_CLIPSIBLINGS),
Make_SymbolTableItem(WS_CLIPCHILDREN),
Make_SymbolTableItem(WS_MAXIMIZE),
Make_SymbolTableItem(WS_BORDER),
Make_SymbolTableItem(WS_DLGFRAME),
Make_SymbolTableItem(WS_VSCROLL),
Make_SymbolTableItem(WS_HSCROLL),
Make_SymbolTableItem(WS_SYSMENU),
Make_SymbolTableItem(WS_THICKFRAME),
Link_SymbolTable(LINK_RESETSTATE_ISEX, NULL),
Make_SymbolTableItem(WS_EX_DLGMODALFRAME),
Make_SymbolTableItem(WS_EX_NOPARENTNOTIFY),
Make_SymbolTableItem(WS_EX_TOPMOST),
Make_SymbolTableItem(WS_EX_ACCEPTFILES),
Make_SymbolTableItem(WS_EX_TRANSPARENT),
Make_SymbolTableItem(WS_EX_MDICHILD),
Make_SymbolTableItem(WS_EX_TOOLWINDOW),
Make_SymbolTableItem(WS_EX_WINDOWEDGE),
Make_SymbolTableItem(WS_EX_CLIENTEDGE),
Make_SymbolTableItem(WS_EX_CONTEXTHELP),
Make_SymbolTableItem(WS_EX_RIGHT),
Make_SymbolTableItem(WS_EX_RTLREADING),
Make_SymbolTableItem(WS_EX_LEFTSCROLLBAR),
Make_SymbolTableItem(WS_EX_CONTROLPARENT),
Make_SymbolTableItem(WS_EX_STATICEDGE),
Make_SymbolTableItem(WS_EX_APPWINDOW),
Make_SymbolTableItem(WS_EX_LAYERED),
Make_SymbolTableItem(WS_EX_NOINHERITLAYOUT),
Make_SymbolTableItem(WS_EX_LAYOUTRTL),
Make_SymbolTableItem(WS_EX_COMPOSITED),
Make_SymbolTableItem(WS_EX_NOACTIVATE),
End_SymbolTable()
};

const SYMBOL_TABLE_ITEM ST_TopWnd[] =
{
Make_SymbolTableItem(WS_MINIMIZEBOX),
Make_SymbolTableItem(WS_MAXIMIZEBOX),
End_SymbolTable()
};

const SYMBOL_TABLE_ITEM ST_ChildWnd[] =
{
Make_SymbolTableItem(WS_GROUP),
Make_SymbolTableItem(WS_TABSTOP),
End_SymbolTable()
};

/*
*DialogStyles
*/

const SYMBOL_TABLE_ITEM ST_Dialog[] =
{
Make_SymbolTableItem(DS_ABSALIGN),
Make_SymbolTableItem(DS_SYSMODAL),
Make_SymbolTableItem(DS_3DLOOK),
Make_SymbolTableItem(DS_FIXEDSYS),
Make_SymbolTableItem(DS_NOFAILCREATE),
Make_SymbolTableItem(DS_LOCALEDIT),
Make_SymbolTableItem(DS_SETFONT),
Make_SymbolTableItem(DS_MODALFRAME),
Make_SymbolTableItem(DS_NOIDLEMSG),
Make_SymbolTableItem(DS_SETFOREGROUND),
Make_SymbolTableItem(DS_CONTROL),
Make_SymbolTableItem(DS_CENTER),
Make_SymbolTableItem(DS_CENTERMOUSE),
Make_SymbolTableItem(DS_CONTEXTHELP),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
 * Edit Control Styles
 */
const SYMBOL_TABLE_ITEM ST_Edit[] =
{
Make_SymbolTableItem(ES_CENTER),
Make_SymbolTableItem(ES_RIGHT),
Make_SymbolTableItem(ES_MULTILINE),
Make_SymbolTableItem(ES_UPPERCASE),
Make_SymbolTableItem(ES_LOWERCASE),
Make_SymbolTableItem(ES_PASSWORD),
Make_SymbolTableItem(ES_AUTOVSCROLL),
Make_SymbolTableItem(ES_AUTOHSCROLL),
Make_SymbolTableItem(ES_NOHIDESEL),
Make_SymbolTableItem(ES_OEMCONVERT),
Make_SymbolTableItem(ES_READONLY),
Make_SymbolTableItem(ES_WANTRETURN),
Make_SymbolTableItem(ES_NUMBER),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
 * RichEdit Control Styles
 */
const SYMBOL_TABLE_ITEM ST_RichEdit[] =
{
#ifdef ES_EX_NOCALLOLEINIT
#undef ES_EX_NOCALLOLEINIT
#endif
#define ES_EX_NOCALLOLEINIT	0x01000000

Make_SymbolTableItem(ES_NOOLEDRAGDROP),
Make_SymbolTableItem(ES_DISABLENOSCROLL),
Make_SymbolTableItem(ES_SUNKEN),
Make_SymbolTableItem(ES_SAVESEL),
Make_SymbolTableItem(ES_SELFIME),
Make_SymbolTableItem(ES_NOIME),
Make_SymbolTableItem(ES_VERTICAL),
Link_SymbolTable(LINK_KEEPSTATE_ISEX,NULL),
Make_SymbolTableItem(ES_EX_NOCALLOLEINIT),
Link_SymbolTable(LINK_KEEPSTATE,ST_Edit)
};

/*
 * Button Control Styles
 */
const SYMBOL_TABLE_ITEM ST_Button[] =
{
Make_SymbolTableItem(BS_OWNERDRAW),
Make_SymbolTableItem(BS_PUSHBOX),
Make_SymbolTableItem(BS_AUTORADIOBUTTON),
Make_SymbolTableItem(BS_USERBUTTON),
Make_SymbolTableItem(BS_GROUPBOX),
Make_SymbolTableItem(BS_AUTO3STATE),
Make_SymbolTableItem(BS_3STATE),
Make_SymbolTableItem(BS_AUTOCHECKBOX),
Make_SymbolTableItem(BS_DEFPUSHBUTTON),
Make_SymbolTableItem(BS_CHECKBOX),
Make_SymbolTableItem(BS_RADIOBUTTON),
Make_SymbolTableItem(BS_LEFTTEXT),
Make_SymbolTableItem(BS_ICON),
Make_SymbolTableItem(BS_BITMAP),
Make_SymbolTableItem(BS_VCENTER),
Make_SymbolTableItem(BS_CENTER),
Make_SymbolTableItem(BS_LEFT),
Make_SymbolTableItem(BS_RIGHT),
Make_SymbolTableItem(BS_TOP),
Make_SymbolTableItem(BS_BOTTOM),
Make_SymbolTableItem(BS_PUSHLIKE),
Make_SymbolTableItem(BS_MULTILINE),
Make_SymbolTableItem(BS_NOTIFY),
Make_SymbolTableItem(BS_FLAT),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};


/*
*Static Control Styles
*/
const SYMBOL_TABLE_ITEM ST_Static[] =
{
Make_SymbolTableItem(SS_ETCHEDFRAME),
Make_SymbolTableItem(SS_ETCHEDVERT),
Make_SymbolTableItem(SS_ETCHEDHORZ),
Make_SymbolTableItem(SS_ENHMETAFILE),
Make_SymbolTableItem(SS_BITMAP),
Make_SymbolTableItem(SS_OWNERDRAW),
Make_SymbolTableItem(SS_LEFTNOWORDWRAP),
Make_SymbolTableItem(SS_SIMPLE),
Make_SymbolTableItem(SS_USERITEM),
Make_SymbolTableItem(SS_WHITEFRAME),
Make_SymbolTableItem(SS_GRAYFRAME),
Make_SymbolTableItem(SS_BLACKFRAME),
Make_SymbolTableItem(SS_WHITERECT),
Make_SymbolTableItem(SS_GRAYRECT),
Make_SymbolTableItem(SS_ICON),
Make_SymbolTableItem(SS_CENTER),
Make_SymbolTableItem(SS_RIGHT),
Make_SymbolTableItem(SS_BLACKRECT),
Make_SymbolTableItem(SS_REALSIZECONTROL),
Make_SymbolTableItem(SS_NOPREFIX),
Make_SymbolTableItem(SS_NOTIFY),
Make_SymbolTableItem(SS_CENTERIMAGE),
Make_SymbolTableItem(SS_RIGHTJUST),
Make_SymbolTableItem(SS_REALSIZEIMAGE),
Make_SymbolTableItem(SS_SUNKEN),
Make_SymbolTableItem(SS_EDITCONTROL),
Make_SymbolTableItem(SS_WORDELLIPSIS),
Make_SymbolTableItem(SS_ENDELLIPSIS),
Make_SymbolTableItem(SS_PATHELLIPSIS),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};


/*
*Listbox Styles
*/
const SYMBOL_TABLE_ITEM ST_Listbox[] =
{
Make_SymbolTableItem(LBS_NOTIFY),
Make_SymbolTableItem(LBS_SORT),
Make_SymbolTableItem(LBS_NOREDRAW),
Make_SymbolTableItem(LBS_MULTIPLESEL),
Make_SymbolTableItem(LBS_OWNERDRAWFIXED),
Make_SymbolTableItem(LBS_OWNERDRAWVARIABLE),
Make_SymbolTableItem(LBS_HASSTRINGS),
Make_SymbolTableItem(LBS_USETABSTOPS),
Make_SymbolTableItem(LBS_NOINTEGRALHEIGHT),
Make_SymbolTableItem(LBS_MULTICOLUMN),
Make_SymbolTableItem(LBS_WANTKEYBOARDINPUT),
Make_SymbolTableItem(LBS_EXTENDEDSEL),
Make_SymbolTableItem(LBS_DISABLENOSCROLL),
Make_SymbolTableItem(LBS_NODATA),
Make_SymbolTableItem(LBS_NOSEL),
Make_SymbolTableItem(LBS_COMBOBOX),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
* ComboBox (and Ex) styles
*/

const SYMBOL_TABLE_ITEM ST_ComboBox[] =
{
Make_SymbolTableItem(CBS_DROPDOWNLIST),
Make_SymbolTableItem(CBS_SIMPLE),
Make_SymbolTableItem(CBS_DROPDOWN),
Make_SymbolTableItem(CBS_OWNERDRAWFIXED),
Make_SymbolTableItem(CBS_OWNERDRAWVARIABLE),
Make_SymbolTableItem(CBS_AUTOHSCROLL),
Make_SymbolTableItem(CBS_OEMCONVERT),
Make_SymbolTableItem(CBS_SORT),
Make_SymbolTableItem(CBS_HASSTRINGS),
Make_SymbolTableItem(CBS_NOINTEGRALHEIGHT),
Make_SymbolTableItem(CBS_DISABLENOSCROLL),
Make_SymbolTableItem(CBS_UPPERCASE),
Make_SymbolTableItem(CBS_LOWERCASE),
Link_SymbolTable(LINK_CTRLEX_ISEX, CBEM_GETEXTENDEDSTYLE),
Make_SymbolTableItem(CBES_EX_CASESENSITIVE),
Make_SymbolTableItem(CBES_EX_NOEDITIMAGEINDENT),
Make_SymbolTableItem(CBES_EX_NOSIZELIMIT),
Make_SymbolTableItem(CBES_EX_PATHWORDBREAKPROC),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};


/*
* ScrollBar Styles
*/

const SYMBOL_TABLE_ITEM ST_ScrollBar[]=
{
Make_SymbolTableItem(SBS_SIZEGRIP),
Make_SymbolTableItem(SBS_SIZEBOX),
Make_SymbolTableItem(SBS_VERT|SBS_RIGHTALIGN),
Make_SymbolTableItem(SBS_VERT|SBS_LEFTALIGN),
Make_SymbolTableItem(SBS_BOTTOMALIGN),
Make_SymbolTableItem(SBS_TOPALIGN),
Make_SymbolTableItem(SBS_VERT),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
* SysPager Styles
*/

const SYMBOL_TABLE_ITEM ST_Pager[]=
{
Make_SymbolTableItem(PGS_HORZ),
Make_SymbolTableItem(PGS_AUTOSCROLL),
Make_SymbolTableItem(PGS_DRAGNDROP),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
* SysListView32 styles
*/
const SYMBOL_TABLE_ITEM ST_ListView[]=
{
Make_SymbolTableItem(LVS_LIST),
Make_SymbolTableItem(LVS_REPORT),
Make_SymbolTableItem(LVS_SMALLICON),
Make_SymbolTableItem(LVS_SINGLESEL),
Make_SymbolTableItem(LVS_SHOWSELALWAYS),
Make_SymbolTableItem(LVS_SORTASCENDING),
Make_SymbolTableItem(LVS_SORTDESCENDING),
Make_SymbolTableItem(LVS_SHAREIMAGELISTS),
Make_SymbolTableItem(LVS_NOLABELWRAP),
Make_SymbolTableItem(LVS_AUTOARRANGE),
Make_SymbolTableItem(LVS_EDITLABELS),
Make_SymbolTableItem(LVS_OWNERDRAWFIXED),
Make_SymbolTableItem(LVS_ALIGNLEFT),
Make_SymbolTableItem(LVS_OWNERDATA),
Make_SymbolTableItem(LVS_NOSCROLL),
Make_SymbolTableItem(LVS_NOCOLUMNHEADER),
Make_SymbolTableItem(LVS_NOSORTHEADER),
Link_SymbolTable(LINK_CTRLEX_ISEX, LVM_GETEXTENDEDLISTVIEWSTYLE),
Make_SymbolTableItem(LVS_EX_GRIDLINES),
Make_SymbolTableItem(LVS_EX_SUBITEMIMAGES),
Make_SymbolTableItem(LVS_EX_CHECKBOXES),
Make_SymbolTableItem(LVS_EX_TRACKSELECT),
Make_SymbolTableItem(LVS_EX_HEADERDRAGDROP),
Make_SymbolTableItem(LVS_EX_FULLROWSELECT),
Make_SymbolTableItem(LVS_EX_ONECLICKACTIVATE),
Make_SymbolTableItem(LVS_EX_TWOCLICKACTIVATE),
Make_SymbolTableItem(LVS_EX_FLATSB),
Make_SymbolTableItem(LVS_EX_REGIONAL),
Make_SymbolTableItem(LVS_EX_INFOTIP),
Make_SymbolTableItem(LVS_EX_UNDERLINEHOT),
Make_SymbolTableItem(LVS_EX_UNDERLINECOLD),
Make_SymbolTableItem(LVS_EX_MULTIWORKAREAS),
Make_SymbolTableItem(LVS_EX_LABELTIP),
Make_SymbolTableItem(LVS_EX_BORDERSELECT),
Make_SymbolTableItem(LVS_EX_DOUBLEBUFFER),
Make_SymbolTableItem(LVS_EX_HIDELABELS),
Make_SymbolTableItem(LVS_EX_SINGLEROW),
Make_SymbolTableItem(LVS_EX_SNAPTOGRID),
Make_SymbolTableItem(LVS_EX_SIMPLESELECT),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
* SysTreeView32 styles
*/
const SYMBOL_TABLE_ITEM ST_TreeView[]=
{
Make_SymbolTableItem(TVS_HASBUTTONS),
Make_SymbolTableItem(TVS_HASLINES),
Make_SymbolTableItem(TVS_LINESATROOT),
Make_SymbolTableItem(TVS_EDITLABELS),
Make_SymbolTableItem(TVS_DISABLEDRAGDROP),
Make_SymbolTableItem(TVS_SHOWSELALWAYS),
Make_SymbolTableItem(TVS_RTLREADING),
Make_SymbolTableItem(TVS_NOTOOLTIPS),
Make_SymbolTableItem(TVS_CHECKBOXES),
Make_SymbolTableItem(TVS_TRACKSELECT),
Make_SymbolTableItem(TVS_SINGLEEXPAND),
Make_SymbolTableItem(TVS_INFOTIP),
Make_SymbolTableItem(TVS_FULLROWSELECT),
Make_SymbolTableItem(TVS_NOSCROLL),
Make_SymbolTableItem(TVS_NONEVENHEIGHT),
Make_SymbolTableItem(TVS_NOHSCROLL),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
* SysTabControl32 styles
*/
const SYMBOL_TABLE_ITEM ST_TabControl[]=
{
Make_SymbolTableItem(TCS_FOCUSNEVER),			//0x8000
Make_SymbolTableItem(TCS_TOOLTIPS),				//0x4000
Make_SymbolTableItem(TCS_OWNERDRAWFIXED),		//0x2000
Make_SymbolTableItem(TCS_FOCUSONBUTTONDOWN),	//0x1000
Make_SymbolTableItem(TCS_RAGGEDRIGHT),			//0x0800
Make_SymbolTableItem(TCS_FIXEDWIDTH),			//0x0400
Make_SymbolTableItem(TCS_MULTILINE),			//0x0200
Make_SymbolTableItem(TCS_BUTTONS),				//0x0100
Make_SymbolTableItem(TCS_VERTICAL|TCS_RIGHT),	//0x0082
Make_SymbolTableItem(TCS_VERTICAL),				//0x0080
Make_SymbolTableItem(TCS_HOTTRACK),				//0x0040
Make_SymbolTableItem(TCS_FORCELABELLEFT),		//0x0020
Make_SymbolTableItem(TCS_FORCEICONLEFT),		//0x0010
Make_SymbolTableItem(TCS_FLATBUTTONS),			//0x0008
Make_SymbolTableItem(TCS_MULTISELECT),			//0x0004
Make_SymbolTableItem(TCS_BOTTOM),				//0x0002
Make_SymbolTableItem(TCS_SCROLLOPPOSITE),		//0x0001
Link_SymbolTable(LINK_CTRLEX_ISEX, TCM_GETEXTENDEDSTYLE),
Make_SymbolTableItem(TCS_EX_REGISTERDROP),		//0x00000002
Make_SymbolTableItem(TCS_EX_FLATSEPARATORS),	//0x00000001
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};


/*
* This section lists common control styles. 
Except where noted, these styles apply to header controls, 
toolbar controls, and status windows. and...Rebar...maybe more...
*/
const SYMBOL_TABLE_ITEM ST_HeaderStatusToolbar[]=
{
Make_SymbolTableItem(CCS_VERT),				//0x00000080L
Make_SymbolTableItem(CCS_NODIVIDER),		//0x00000040L
Make_SymbolTableItem(CCS_ADJUSTABLE),		//0x00000020L
Make_SymbolTableItem(CCS_NOPARENTALIGN),	//0x00000008L
Make_SymbolTableItem(CCS_NORESIZE),			//0x00000004L
Make_SymbolTableItem(CCS_BOTTOM),			//0x00000003L
Make_SymbolTableItem(CCS_NOMOVEY),			//0x00000002L
Make_SymbolTableItem(CCS_TOP),				//0x00000001L
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
* SysHeader32 styles
*/
const SYMBOL_TABLE_ITEM ST_Header[]=
{
//SysHeader32
Make_SymbolTableItem(HDS_FULLDRAG),			//0x0080
Make_SymbolTableItem(HDS_DRAGDROP),			//0x0040
Make_SymbolTableItem(HDS_HIDDEN),			//0x0008
Make_SymbolTableItem(HDS_HOTTRACK),			//0x0004
Make_SymbolTableItem(HDS_BUTTONS),			//0x0002
Make_SymbolTableItem(HDS_FLAT),				//0x0200
Make_SymbolTableItem(HDS_FILTERBAR),		//0x0100
Link_SymbolTable(LINK_RESETSTATE, ST_HeaderStatusToolbar)
};

/*
*msctls_statusbar32  styles
*/
const SYMBOL_TABLE_ITEM ST_Statusbar[]=
{
Make_SymbolTableItem(SBARS_TOOLTIPS), //0x0800
Make_SymbolTableItem(SBARS_SIZEGRIP), //0x0100
Link_SymbolTable(LINK_RESETSTATE, ST_HeaderStatusToolbar)
};

/*
* ToolbarWindow32 styles
*/
const SYMBOL_TABLE_ITEM ST_Toolbar[]=
{
Make_SymbolTableItem(TBSTYLE_TRANSPARENT),
Make_SymbolTableItem(TBSTYLE_REGISTERDROP),
Make_SymbolTableItem(TBSTYLE_CUSTOMERASE),
Make_SymbolTableItem(TBSTYLE_LIST),
Make_SymbolTableItem(TBSTYLE_FLAT),
Make_SymbolTableItem(TBSTYLE_ALTDRAG),
Make_SymbolTableItem(TBSTYLE_WRAPABLE),
Make_SymbolTableItem(TBSTYLE_TOOLTIPS),
Make_SymbolTableItem(BTNS_WHOLEDROPDOWN),
Make_SymbolTableItem(BTNS_SHOWTEXT),
Make_SymbolTableItem(BTNS_NOPREFIX),
Make_SymbolTableItem(BTNS_AUTOSIZE),
Make_SymbolTableItem(BTNS_DROPDOWN),
Make_SymbolTableItem(BTNS_GROUP),
Make_SymbolTableItem(BTNS_CHECK),
Make_SymbolTableItem(BTNS_SEP),
Link_SymbolTable(LINK_CTRLEX_ISEX, TB_GETEXTENDEDSTYLE),
Make_SymbolTableItem(TBSTYLE_EX_DRAWDDARROWS),
Make_SymbolTableItem(TBSTYLE_EX_MIXEDBUTTONS),
Make_SymbolTableItem(TBSTYLE_EX_HIDECLIPPEDBUTTONS),
Make_SymbolTableItem(TBSTYLE_EX_DOUBLEBUFFER),
Link_SymbolTable(LINK_RESETSTATE, ST_HeaderStatusToolbar)
};

/*
* ReBarWindow32 styles
*/
const SYMBOL_TABLE_ITEM ST_ReBar[]=
{
Make_SymbolTableItem(RBS_DBLCLKTOGGLE),
Make_SymbolTableItem(RBS_VERTICALGRIPPER),
Make_SymbolTableItem(RBS_AUTOSIZE),
Make_SymbolTableItem(RBS_REGISTERDROP),
Make_SymbolTableItem(RBS_FIXEDORDER),
Make_SymbolTableItem(RBS_BANDBORDERS),
Make_SymbolTableItem(RBS_VARHEIGHT),
Make_SymbolTableItem(RBS_TOOLTIPS),
Link_SymbolTable(LINK_RESETSTATE, ST_HeaderStatusToolbar)
};

/*
* tooltips_class32 styles
*/
const SYMBOL_TABLE_ITEM ST_Tooltip[]=
{
Make_SymbolTableItem(TTS_ALWAYSTIP),
Make_SymbolTableItem(TTS_NOPREFIX),
Make_SymbolTableItem(TTS_NOANIMATE),
Make_SymbolTableItem(TTS_NOFADE),
Make_SymbolTableItem(TTS_BALLOON),
Make_SymbolTableItem(TTS_CLOSE),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};


/*
* msctls_trackbar32 styles
*/
const SYMBOL_TABLE_ITEM ST_Trackbar[]=
{
Make_SymbolTableItem(TBS_DOWNISLEFT),		//0x0400
Make_SymbolTableItem(TBS_REVERSED),			//0x0200
Make_SymbolTableItem(TBS_TOOLTIPS),			//0x0100
Make_SymbolTableItem(TBS_NOTHUMB),			//0x0080
Make_SymbolTableItem(TBS_FIXEDLENGTH),		//0x0040
Make_SymbolTableItem(TBS_ENABLESELRANGE),	//0x0020
Make_SymbolTableItem(TBS_NOTICKS),			//0x0010
Make_SymbolTableItem(TBS_BOTH),				//0x0008
Make_SymbolTableItem(TBS_VERT|TBS_LEFT),	//0x0006
Make_SymbolTableItem(TBS_TOP),				//0x0004
Make_SymbolTableItem(TBS_VERT),				//0x0002
Make_SymbolTableItem(TBS_AUTOTICKS),		//0x0001
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};


/*
* msctls_updown32 styles
*/
const SYMBOL_TABLE_ITEM ST_Updown[]=
{
Make_SymbolTableItem(UDS_HOTTRACK),		//0x0100
Make_SymbolTableItem(UDS_NOTHOUSANDS),	//0x0080
Make_SymbolTableItem(UDS_HORZ),			//0x0040
Make_SymbolTableItem(UDS_ARROWKEYS),	//0x0020
Make_SymbolTableItem(UDS_AUTOBUDDY),	//0x0010
Make_SymbolTableItem(UDS_ALIGNLEFT),	//0x0008
Make_SymbolTableItem(UDS_ALIGNRIGHT),	//0x0004
Make_SymbolTableItem(UDS_SETBUDDYINT),	//0x0002
Make_SymbolTableItem(UDS_WRAP),			//0x0001
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};


/*
*msctls_progress32  styles
*/
const SYMBOL_TABLE_ITEM ST_Progress[]=
{
Make_SymbolTableItem(PBS_SMOOTH),
Make_SymbolTableItem(PBS_VERTICAL),
Make_SymbolTableItem(PBS_MARQUEE),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
* SysAnimate32 styles
*/
const SYMBOL_TABLE_ITEM ST_Animate[]=
{
Make_SymbolTableItem(ACS_CENTER),
Make_SymbolTableItem(ACS_TRANSPARENT),
Make_SymbolTableItem(ACS_AUTOPLAY),
Make_SymbolTableItem(ACS_TIMER),
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};


/*
* SysMonthCal32 styles
*/
const SYMBOL_TABLE_ITEM ST_MonthCal[]=
{
Make_SymbolTableItem(MCS_DAYSTATE),			//0x0001
Make_SymbolTableItem(MCS_MULTISELECT),		//0x0002
Make_SymbolTableItem(MCS_WEEKNUMBERS),		//0x0004
Make_SymbolTableItem(MCS_NOTODAYCIRCLE),	//0x0008
Make_SymbolTableItem(MCS_NOTODAY),			//0x0010
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
*SysDateTimePick32  styles
*/
const SYMBOL_TABLE_ITEM ST_DateTime[]=
{
Make_SymbolTableItem(DTS_RIGHTALIGN),		//0x0020
Make_SymbolTableItem(DTS_APPCANPARSE),		//0x0010
Make_SymbolTableItem(DTS_TIMEFORMAT),		//0x0009
Make_SymbolTableItem(DTS_LONGDATEFORMAT),	//0x0004
Make_SymbolTableItem(DTS_SHOWNONE),			//0x0002
Make_SymbolTableItem(DTS_UPDOWN),			//0x0001
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

/*
*MDIClient  styles
*/
const SYMBOL_TABLE_ITEM ST_MDIClient[]=
{
Make_SymbolTableItem(MDIS_ALLCHILDSTYLES),	//0x0001
Link_SymbolTable(LINK_RESETSTATE,ST_WindowStyle)
};

//////////////////////////////////////////////////////////////////////////

const SYMBOL_TABLE_ITEM ST_KnownClassList[] =
{
{KWC_DLG,        TEXT("#32770")},
{KWC_BTN,        TEXT("Button")},
{KWC_BTN,        TEXT("TButton")},
{KWC_EDIT,       TEXT("Edit")}, 
{KWC_EDIT,       TEXT("TEdit")},
{KWC_STATIC,     TEXT("Static")},
{KWC_LISTBOX,    TEXT("ListBox")},
{KWC_LISTBOX,    TEXT("TListBox")},
{KWC_LISTBOX,    TEXT("ComboLBox")},
{KWC_SCROLL,     TEXT("ScrollBar")},
{KWC_COMBO,      TEXT("ComboBox")},
{KWC_COMBO,      TEXT("TComboBox")},
{KWC_COMBOEX,    TEXT("ComboBoxEx32")},
{KWC_LISTVIEW,   TEXT("SysListView32")},
{KWC_LISTVIEW,   TEXT("TListView")},
{KWC_TREEVIEW,   TEXT("SysTreeView32")},
{KWC_TREEVIEW,   TEXT("TTreeView")},
{KWC_TAB,        TEXT("SysTabControl32")},
{KWC_TAB,        TEXT("TPageControl")},
{KWC_HEADER,     TEXT("SysHeader32")},
{KWC_TOOLBAR,    TEXT("ToolbarWindow32")},
{KWC_REBAR,      TEXT("ReBarWindow32")},
{KWC_TOOLTIP,    TEXT("tooltips_class32")},
{KWC_STATUS,     TEXT("msctls_statusbar32")},
{KWC_STATUS,     TEXT("TStatusBar")},
{KWC_TRACKBAR,   TEXT("msctls_trackbar32")},
{KWC_UPDOWN,     TEXT("msctls_updown32")},
{KWC_PROGRESS,   TEXT("msctls_progress32")},
{KWC_HOTKEY,     TEXT("msctls_hotkey32")},
{KWC_ANIMATE,    TEXT("SysAnimate32")},
{KWC_MONTHCAL,   TEXT("SysMonthCal32")},
{KWC_DATETIME,   TEXT("SysDateTimePick32")},
{KWC_REDIT,      TEXT("RichEdit")},
{KWC_REDIT,      TEXT("RichEdit20A")},
{KWC_REDIT,      TEXT("RichEdit20W")},
{KWC_REDIT,      TEXT("RichEdit50W")},
{KWC_IPADDRESS,  TEXT("SysIPAddress32")},
{KWC_PAGER,      TEXT("SysPager")},
{KWC_MENU,       TEXT("#32768")},
{KWC_MDI,        TEXT("MDIClient")},
{0,             (LPTSTR)NULL}
//Extend point...
};


const SYMBOL_TABLE_ITEM ST_ClassStyleList[]=
{
{(LONG)ST_WindowStyle, TEXT("Generic Window")},	//0
{(LONG)ST_Dialog,	   TEXT("Dialog")},			//1
{(LONG)ST_Button,      TEXT("Button")},			//2
{(LONG)ST_Edit,        TEXT("Edit")},				//3
{(LONG)ST_RichEdit,    TEXT("RichEdit")},			//4
{(LONG)ST_ComboBox,    TEXT("ComboBox")},			//5
{(LONG)ST_ComboBox,    TEXT("ComboBoxEx")},		//6
{(LONG)ST_Header,      TEXT("Header")},			//7
{(LONG)ST_WindowStyle, TEXT("HotKey")},			//8
{(LONG)ST_Listbox,     TEXT("List Box")},			//9
{(LONG)ST_ListView,    TEXT("List-View")},		//10
{(LONG)ST_Pager,       TEXT("Pager")},			//11
{(LONG)ST_Progress,    TEXT("Progress Bar")},		//12
{(LONG)ST_TreeView,    TEXT("Tree-View")},		//13
{(LONG)ST_TabControl,  TEXT("Tab Control")},		//14
{(LONG)ST_Toolbar,     TEXT("ToolBar")},			//15
{(LONG)ST_ReBar,       TEXT("ReBar")},			//16
{(LONG)ST_ScrollBar,   TEXT("Scroll Bar")},		//17
{(LONG)ST_Statusbar,   TEXT("Status Bar")},		//18
{(LONG)ST_Static,      TEXT("Static")},			//19
{(LONG)ST_Tooltip,     TEXT("ToolTip")},			//20
{(LONG)ST_Trackbar,    TEXT("TrackBar")},			//21
{(LONG)ST_Updown,      TEXT("Up-Down")},			//22
{(LONG)ST_WindowStyle, TEXT("IPAddress")},		//23
{(LONG)ST_Animate,     TEXT("Animation")},		//24
{(LONG)ST_DateTime,    TEXT("DateTimePicker")},	//25
{(LONG)ST_MonthCal,    TEXT("Month Calendar")},	//26
{(LONG)ST_WindowStyle, TEXT("Menu")},				//27
{(LONG)ST_MDIClient,   TEXT("MDIClient")},		//28
End_SymbolTable() 
//can point to other data for extending...
};



//////////////////////////////////////////////////////////////////////////
