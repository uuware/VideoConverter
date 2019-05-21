/////////////////////////////////////////////////////////////////////////////
// Name:        ccSkinFrame.h
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CC_APP_CTL_
#define _WX_CC_APP_CTL_

#include <wx/log.h>
#include "wx/image.h"
#include "wx/graphics.h"
#include "wx/dcgraph.h"
#include <wx/filefn.h> 
#include <wx/xml/xml.h>
#include <wx/dcbuffer.h>
#include <wx/datetime.h>
#include <wx/rawbmp.h>
#include <wx/bmpbuttn.h>

#include "cc_strmap.h"
#include "cc_util.h"
#include "cc_control.h"

class ccLog: public wxLog
{
public:
	static long m_nFileMaxSize;
	static int m_nFileMaxCount;

	ccLog();
	virtual ~ccLog();

protected:
	virtual void DoLogText(const wxString& msg);
	void DoCheckSize();

	wxCriticalSection m_critsect;
	wxString m_tcLogFile;
	wxFile m_logger;
};

#define CC_MENUID_SKIN_START 20100
#define CC_MENUID_LANG_START 20200
class ccApp: public wxApp
{
public:
    ccApp();
    ~ccApp();

	static void SetApp(wxString vendorName, wxString appName);
	static ccStrMap* GetLang();
	static ccStrMap* GetIni();
	static ccDrawSkin* GetDrawSkin();
	static wxArrayString* GetLangNames();
	static wxArrayString* GetSkinNames();

    virtual bool OnInit();
    virtual int OnExit();
	static void test();

protected:
	//flage check for first instance of App
	static int _nIsConstructored;
	static ccStrMap* m_lang;
	static ccStrMap* m_ini;
	static wxArrayString* m_langnames;
	static ccDrawSkin* m_drawSkin;
};

#endif
    // _WX_CC_APP_CTL_
