#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//define for version
#define FILEVER           3, 0, 1, 0
#define STRFILEVER       "3, 0, 1, 0"
#define PRODUCTVER       FILEVER
#define STRPRODUCTVER    STRFILEVER
#define FILENAME         "sample"
#ifdef _DEBUG
#define APPNAME          "sample[uuware.com](debug)"
#else
#define APPNAME          "sample[uuware.com]"
#endif
//Ver 2.01 build 20080325
#define APPVERSION       "1.00"
#define APPBUILD         "build 20130115"

#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128

#define IDI_ICON_MAIN           102
#define IDI_ICON_COPY           103
#define IDI_ICON_CUT            104
#define IDI_ICON_PASTE          105
#define IDI_ICON_ATTACH         106
#define IDI_ICON_MOVE1          107
#define IDI_ICON_MOVE2          108
#define IDI_ICON_DELETE         110
#define IDI_ICON_EXIT           111
#define IDI_ICON_FONT           112
#define IDI_ICON_LOCK           113
#define IDI_ICON_SAVE           114
#define IDI_ICON_REFRESH        115
#define IDI_ICON_PRINT          116
#define IDI_ICON_CLEAR          117
#define IDI_ICON_NEWWIN         118
#define IDI_ICON_COLOR          119
#define IDI_ICON_NOTE           120
#define IDI_ICON_REDO           121
#define IDI_ICON_UNDO           122
#define IDI_ICON_CAPW           123
#define IDI_ICON_CAPS           124
#define IDI_ICON_OPTION         125
#define IDI_ICON_IE             126
#define IDI_ICON_STOP           127
#define IDI_ICON_GO             128
#define IDI_ICON_FOLDER         129
#define IDI_ICON_PAGE           130
#define IDI_ICON_KEY            131
#define IDI_ICON_SETBACK        132
#define IDI_ICON_NOTEMIN        133
#define IDI_ICON_NOTEMIN2       134
#define IDI_ICON_NOTEORG        135
#define IDI_ICON_CALENDAR       136

#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS

#define _APS_NO_MFC			130
#define _APS_NEXT_RESOURCE_VALUE	129
#define _APS_NEXT_COMMAND_VALUE		32771
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		110
#endif
#endif
