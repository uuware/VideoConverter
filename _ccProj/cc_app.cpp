/////////////////////////////////////////////////////////////////////////////
// Name:        ccSkinFrame.cpp
// Purpose:     Frame without Border and owerr draw background
// Author:      uuware
// Modified by:
// Created:     2016/1/15
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "cc_app.h"

int ccLog::m_nFileMaxCount = 10;
long ccLog::m_nFileMaxSize = 1024 * 1000;
ccLog::ccLog() : wxLog()
{
	m_tcLogFile = ccUtil::GetStartupPath(true, true);
	m_tcLogFile.append(wxT(".log"));
	m_logger.Open(m_tcLogFile, wxFile::write_append);
}

ccLog::~ccLog()
{
	m_logger.Close();
}

void ccLog::DoCheckSize()
{
	long pos = (long)m_logger.Length();
	if(pos >= m_nFileMaxSize)
	{
		for(int n = m_nFileMaxCount - 1; n >= 1; n--)
		{
			wxString cFileFrom = wxString::Format(wxT("%s.%02d"), m_tcLogFile.c_str(), n - 1);
			wxString cFileTo = wxString::Format(wxT("%s.%02d"), m_tcLogFile.c_str(), n);
			if(wxFileExists(cFileFrom))
			{
				::wxRenameFile(cFileFrom, cFileTo);
			}
		}

		m_logger.Close();
		wxString cFileTo = wxString::Format(wxT("%s.00"), m_tcLogFile.c_str());
		::wxRenameFile(m_tcLogFile, cFileTo);
		m_logger.Open(m_tcLogFile, wxFile::write_append);
	}
	if(pos == 0 || pos >= m_nFileMaxSize)
	{
		//big-endian:0xFEFF, little-endian:0xFFFE, uft8:0xEFBBBF
		//const byte fBom[4] = {0xef, 0xbb, 0xbf, 0};
		//m_logger.Write(fBom, 3); // write BOM

		size_t count=0;
		const char* bom(wxConvAuto::GetBOMChars(wxBOM_UTF8, &count));
		m_logger.Write(bom, count); // write BOM
		wxString msg = wxString::Format(wxT("Log File(UTF-8, COMPILED:%s %s)\r\n"), wxT(__DATE__), wxT(__TIME__));
		m_logger.Write(msg, wxConvUTF8);
		m_logger.Flush();
	}
}

void ccLog::DoLogText(const wxString& msg)
{
	//for any wxLog
	wxLog *oldlogger = wxLog::SetActiveTarget(NULL);
#ifdef _DEBUG
    wxMessageOutputDebug().Output(msg);
#endif
	wxCriticalSectionLocker locker(m_critsect);
	DoCheckSize();
	m_logger.Write(msg, wxConvUTF8);
	m_logger.Write(wxT("\r\n"), wxConvUTF8);
	//m_logger.Flush(); //so slowly!
	wxLog::SetActiveTarget(oldlogger);
}

int ccApp::_nIsConstructored = 0;
ccStrMap* ccApp::m_lang = NULL;
ccStrMap* ccApp::m_ini = NULL;
ccDrawSkin* ccApp::m_drawSkin = NULL;
wxArrayString* ccApp::m_langnames = NULL;

ccApp::ccApp()
{
}

ccApp::~ccApp()
{
	if(_nIsConstructored != 1)
	{
		return;
	}
	_nIsConstructored = 2;

	if(m_drawSkin != NULL)
	{
		delete m_drawSkin;
		m_drawSkin = NULL;
	}
	if(m_ini != NULL)
	{
		delete m_ini;
		m_ini = NULL;
	}
	if(m_lang != NULL)
	{
		delete m_lang;
		m_lang = NULL;
	}
	if(m_langnames != NULL)
	{
		delete m_langnames;
		m_langnames = NULL;
	}
}

bool ccApp::OnInit()
{
	if ( !wxApp::OnInit() )
	{
		return false;
	}
	if(_nIsConstructored != 0)
	{
		//We get here if the framework is used incorrectly, as more than one instance
		// of a App derived class is started.
		return true;
	}
	_nIsConstructored = 1;

	ccLog *logger = new ccLog();
	wxLog *oldlogger = wxLog::SetActiveTarget(logger);
	if(oldlogger != NULL)
	{
		delete oldlogger;
	}

	wxLog::SetTimestamp(wxT("%Y-%m-%d %H:%M:%S"));
	//wxLog::SetLogLevel(wxLOG_Debug);
	wxLogMessage("App Start.");

#if defined(__WXMAC__)
	// Disable window animation
	wxSystemOptions::SetOption( wxMAC_WINDOW_PLAIN_TRANSITION, 1 );
#endif
	return true;
}

int ccApp::OnExit()
{
	wxLogMessage("App End.");
	if(m_ini != NULL)
	{
		wxString m_tcFile = m_ini->GetPath();
		if(m_tcFile.IsEmpty())
		{
			m_tcFile = ccUtil::GetStartupPath(true, true);
			m_tcFile.append(wxT(".ini"));
		}
		m_ini->Save(m_tcFile, wxConvUTF8, wxBOM_UTF8);
		m_ini->SetIsDirty(false);
	}
    return 0;
}

void ccApp::SetApp(wxString vendorName, wxString appName)
{
	wxTheApp->SetVendorName(vendorName);
	wxTheApp->SetAppName(appName);
}

wxArrayString* ccApp::GetLangNames()
{
	if(m_langnames == NULL)
	{
		m_langnames = new wxArrayString();

		wxString pfile = ccUtil::GetStartupReleatedPath(wxT("langs"));
		wxArrayString mfiles;
		if(ccUtil::dFiles(pfile, mfiles, wxT("*.lang"), wxDIR_FILES|wxDIR_HIDDEN))
		{
			for (size_t i = 0; i < mfiles.size(); i++)
			{
				wxString key = mfiles[i];
				int nInd = key.rfind(wxT("."));
				if(nInd != wxNOT_FOUND)
				{
					m_langnames->Add(key.substr(0, nInd));
				}
			}
		}
	}
	return m_langnames;
}

wxArrayString* ccApp::GetSkinNames()
{
	static wxArrayString m_skinnames;
	static int flag = 0;
	if(flag == 0)
	{
		flag = 1;
		wxString pfile = ccUtil::GetStartupReleatedPath(wxT("skins"));
		wxArrayString mfiles;
		if(ccUtil::dFiles(pfile, mfiles, wxT("*.skin"), wxDIR_FILES|wxDIR_HIDDEN))
		{
			for (size_t i = 0; i < mfiles.size(); i++)
			{
				wxString key = mfiles[i];
				int nInd = key.rfind(wxT("."));
				if(nInd != wxNOT_FOUND)
				{
					m_skinnames.Add(key.substr(0, nInd));
				}
			}
		}
	}
	return &m_skinnames;
}

ccStrMap* ccApp::GetLang()
{
	if(m_lang == NULL)
	{
		m_lang = new ccStrMap(false, true, true);

		//current lang
		ccStrMap* mapini = ccApp::GetIni();
		wxString lang = mapini->Get(wxT("Main.Lang"), wxT("en_US"));

		wxString pfile = ccUtil::GetStartupReleatedPath(wxT("langs"));
		wxString pfile2 = pfile;
		pfile2.append(lang).append(wxT(".lang"));
		if(!ccUtil::fExist(pfile2))
		{
			mapini->Set(wxT("Main.Lang"), wxT("en_US"));
			pfile2 = pfile + wxT("en_US.lang");
		}
		m_lang->Load(pfile2, wxConvUTF8);
		m_lang->SetPath(pfile2);
	}
	return m_lang;
}

ccStrMap* ccApp::GetIni()
{
	if(m_ini == NULL)
	{
		m_ini = new ccStrMap(false, false, true);
		wxString m_tcFile = ccUtil::GetStartupPath(true, true);
		m_tcFile.append(wxT(".ini"));
		m_ini->Load(m_tcFile, wxConvUTF8);
		m_ini->SetPath(m_tcFile);
	}
	return m_ini;
}

ccDrawSkin* ccApp::GetDrawSkin()
{
	if(m_drawSkin == NULL)
	{
		m_drawSkin = new ccDrawSkin();
		wxString m_tcFile = ccUtil::GetStartupPath(true, true);
		m_tcFile.append(wxT(".ini"));
		m_ini->Load(m_tcFile, wxConvUTF8);
		m_ini->SetPath(m_tcFile);
	}
	return m_drawSkin;
}

void ccApp::test()
{
#ifdef _DEBUG
	wxArrayString* langnames = ccApp::GetLangNames();
	for (size_t i = 0; i < langnames->size(); i++)
	{
		wxString key = langnames->Item(i);
		wxLogDebug(wxT("key:[%s]"), key);
	}
	ccStrMap* ini = ccApp::GetIni();
	ini->Dump();
	ccStrMap* lang = ccApp::GetLang();
	lang->Dump();
#endif
}