/////////////////////////////////////////////////////////////////////////////
// Name:        dragimag.cpp
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "main.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "sample.xpm"
    #include "sample2.xpm"
#endif





// main program

// VRow
VRow::VRow( const wxString& path, VList *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
	: wxPanel( parent, id, pos, size, wxNO_BORDER )
{
	m_flag = 0;
	SetBackgroundColour(wxColour("#cdcfd2"));
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_owner = parent;
	m_vpath = path;

    wxStaticBox *sb = new wxStaticBox( this, wxID_ANY, m_vpath,
                           wxDefaultPosition, wxDefaultSize );
	this->Bind(wxEVT_PAINT, &VRow::OnPaint, this);
	this->Bind(wxEVT_ERASE_BACKGROUND, &VRow::OnEraseBackground, this);
	this->Bind(wxEVT_ENTER_WINDOW, &VRow::OnEnterEvent, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &VRow::OnLeaveEvent, this);
	this->Bind(wxEVT_LEFT_DOWN, &VRow::OnLeftClickEvent, this);
	this->Bind(wxEVT_LEFT_UP, &VRow::OnLeftClickEvent, this);
}

VRow::~VRow()
{
}

void VRow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	bool isMouseon = CC_BIT_IS(m_flag, CC_VROW_FLAG_MOUSEON);
	bool isSelected = IsSelected();
	wxSize size = this->GetSize();
	wxRect rect(2, 2, size.x - 4, size.y - 4);
	wxPaintDC paintDC(this);
    PrepareDC(paintDC);

	paintDC.SetBackground(this->GetParent()->GetBackgroundColour());
	paintDC.Clear();

	if(isSelected || isMouseon)
	{
		paintDC.SetPen(wxPen(*wxBLUE, 4));
		paintDC.SetBrush(*wxWHITE);
	}
	else
	{
		paintDC.SetPen(*wxWHITE);
		paintDC.SetBrush(this->GetBackgroundColour());
	}

	paintDC.DrawRoundedRectangle(rect, 5);
	paintDC.SetBrush(wxNullBrush);
	paintDC.SetPen(wxNullPen);
}

void VRow::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void VRow::OnEnterEvent(wxMouseEvent& event)
{
	CC_BIT_SET(m_flag, CC_VROW_FLAG_MOUSEON, true);
	this->Refresh();
}

void VRow::OnLeaveEvent(wxMouseEvent& event)
{
	CC_BIT_SET(m_flag, CC_VROW_FLAG_MOUSEON, false);
	this->Refresh();
}

void VRow::OnLeftClickEvent( wxMouseEvent &event )
{
	if(event.LeftDown())
	{
		bool isSelected = this->IsSelected();
		SetSelected(!isSelected);
		this->Refresh();
		m_owner->SetSelected(this);
	}
	m_owner->SetFocus();
}

// VList
VList::VList( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
	: wxScrolledWindow( parent, id, pos, size, wxVSCROLL )
{
	this->SetScrollRate(-1, 10);
	SetBackgroundColour(parent->GetBackgroundColour());
	//SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_owner = parent;
	m_selectedRow = NULL;

	this->Bind(wxEVT_SIZE, &VList::OnResize, this);
}

VList::~VList()
{
}

void VList::AddRow(VRow* vRow)
{
	m_List.Append(vRow);
	ReLayout();
}

void VList::RemoveRow(VRow* vRow)
{
	m_List.remove(vRow);
	ReLayout();
	this->RemoveChild(vRow);
	delete vRow;
}

void VList::RemoveRowAll()
{
	wxList::iterator iter;
	for (iter = m_List.begin(); iter != m_List.end(); ++iter)
	{
		VRow* vRow = (VRow*)*iter;
		this->RemoveChild(vRow);
		delete vRow;
	}
	m_List.clear();
}

int VList::GetRowCount()
{
	return m_List.size();
}

wxString VList::GetPath(int index)
{
	if(index < 0 || index > GetRowCount())
	{
		return wxEmptyString;
	}
	VRow* vRow = (VRow*)m_List[index];
	return vRow->GetPath();
}

void VList::ReLayout()
{
	bool hasBarY = this->HasScrollbar(wxVERTICAL);
	int barW = hasBarY ? ccUtil::GetScrollBarWidth() : 0;
	int nTop = 3;
	wxSize size = this->GetSize();
	// let's iterate over the list in STL syntax
	wxList::iterator iter;
	for (iter = m_List.begin(); iter != m_List.end(); ++iter)
	{
		VRow* vRow = (VRow*)*iter;
		wxSize size2 = vRow->GetSize();
		vRow->SetSize(1, nTop, size.x - barW - 2, size2.y);
		nTop += size2.y;
	}
	nTop = CC_MAX(size.y, nTop - 3);
	this->SetVirtualSize(-1, nTop);
	//SetScrollbars(1, 1, size.x - barW, nTop, 0, 0);
}

void VList::OnResize(wxSizeEvent& event)
{
	ReLayout();
}

void VList::SetSelected(VRow* vRow)
{
	wxList::iterator iter;
	for (iter = m_List.begin(); iter != m_List.end(); ++iter)
	{
		VRow* vRow2 = (VRow*)*iter;
		if(vRow2 != vRow)
		{
			vRow2->SetSelected(false);
		}
	}
	if(!vRow->IsSelected())
	{
		vRow->SetSelected(true);
	}
	m_selectedRow = vRow;
}

// VPlay
VPlay::VPlay( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
	: wxPanel( parent, id, pos, size, wxNO_BORDER )
{
	SetBackgroundColour(parent->GetBackgroundColour());
	//SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_owner = parent;
	m_play = NULL;

	this->Bind(wxEVT_SIZE, &VPlay::OnResize, this);
}

VPlay::~VPlay()
{
}

void VPlay::OnResize(wxSizeEvent& event)
{
	if(m_play != NULL)
	{
		wxSize size = this->GetSize();
		m_play->SetSize(0, size.y/4, size.x, size.y/2);
	}
}

wxPanel* VPlay::GetPlayPanel(bool isReset)
{
	if(isReset || m_play == NULL)
	{
		if(m_play != NULL)
		{
			this->RemoveChild(m_play);
			delete m_play;
			m_play = NULL;
		}
		m_play = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
		//post a event
		this->SendSizeEvent(wxSEND_EVENT_POST);
	}
	return m_play;
}

// MainFrame
MainFrame::MainFrame()
: ccSkinFrame( (wxFrame *)NULL, wxID_ANY, wxT("视频转换软件"),
          wxDefaultPosition, wxSize(900,600) )
{
    SetIcon(wxICON(mainicon));
	m_skin = new ccDrawSkin();
	this->AddAutoDestroy(m_skin);

	m_skin->LoadSkin(wxT(".\\skins\\skin01.skin"));
	this->SetDrawSkin(m_skin);

    wxMenu *file_menu = new wxMenu();
	this->AddAutoDestroy(file_menu);

	wxString defSkin = ccApp::GetIni()->Get(wxT("Main.Skin"), wxT("skin01"));
    submSkin = new wxMenu();
	file_menu->AppendSubMenu( submSkin, ccApp::GetLang()->Get(wxT("Lang.S_Skins"), wxT("Skins")) );
	wxArrayString* skinsArr = ccApp::GetSkinNames();
	for(int i = 0; i < (int)skinsArr->size(); i++)
	{
		wxString sItem = skinsArr->Item(i);
		wxString sItem2 = ccApp::GetLang()->Get(wxT("Lang.S_skinname_") + sItem, wxT("Skin(") + sItem + wxT(")"));
		wxMenuItem* item = new wxMenuItem(file_menu, CC_MENUID_SKIN_START + i, sItem2, wxT(""), wxITEM_CHECK);
		submSkin->Append( item );
		if(defSkin == sItem)
		{
			item->Check(true);
		}
	}

	wxString defLang = ccApp::GetIni()->Get(wxT("Main.Lang"), wxT("en_US"));
    submLang = new wxMenu();
	file_menu->AppendSubMenu( submLang, ccApp::GetLang()->Get(wxT("Lang.S_Language"), wxT("Language")) );
	wxArrayString* pArr = ccApp::GetLangNames();
	for(int i = 0; i < (int)pArr->size(); i++)
	{
		wxString sItem = pArr->Item(i);
		wxString sItem2 = ccApp::GetLang()->Get(wxT("Lang.S_langname_") + sItem, wxT("Language(") + sItem + wxT(")"));
		wxMenuItem* item = new wxMenuItem(file_menu, CC_MENUID_LANG_START + i, sItem2, wxT(""), wxITEM_CHECK);
		submLang->Append( item );
		if(defLang == sItem)
		{
			item->Check(true);
		}
	}

    file_menu->AppendSeparator();
	itemAbout = new wxMenuItem(file_menu, wxNewId(), wxT("&About"));
    file_menu->Append(itemAbout);
	itemExit = new wxMenuItem(file_menu, wxNewId(), wxT("E&xit"));
    file_menu->Append( itemExit );
	file_menu->Bind( wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnMenuCmd, this );

	this->SetTopMenu(file_menu);
    //SetMenuBar( menu_bar );

#if wxUSE_STATUSBAR
    //CreateStatusBar(2);
	//MainStatusBar* statbar = new MainStatusBar(this, wxSTB_DEFAULT_STYLE);
	//this->SetStatusBar(statbar);
    //int widths[] = { -1, 100 };
    //SetStatusWidths( 2, widths );
	//SetStatusText(wxT("准备就绪。"));
	//m_frameStatusBar->SetBackgroundColour(this->GetBackgroundColour());
#endif // wxUSE_STATUSBAR

	ccStatusBar* statbar = new ccStatusBar(this);
	this->SetStatusPanel(statbar);
	new ccLabel(statbar, wxID_ANY, wxT("准备就绪。"), wxPoint(3, 2), wxSize(80, 18));
	m_chkShutdown = new ccCheckbox( statbar, wxID_ANY, wxT("转换完成后关闭电脑"), wxPoint(200, 2), wxSize(140, 18) );

	int nBtnTop = 3;
	int nBtnLeft = 5;
	m_btnAddFile = new ccSkinButton(m_client, wxNewId(), wxT("添加文件"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnAddFile);
	m_btnAddFile->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);
	nBtnLeft += m_btnAddFile->GetSize().x + 5;

	m_btnPlay = new ccSkinButton(m_client, wxNewId(), wxT("播放"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnPlay);
	m_btnPlay->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);
	nBtnLeft += m_btnPlay->GetSize().x + 5;

	m_btnEdit = new ccSkinButton(m_client, wxNewId(), wxT("编辑"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnEdit);
	m_btnEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);
	nBtnLeft += m_btnEdit->GetSize().x + 5;

	m_btnRemove = new ccSkinButton(m_client, wxNewId(), wxT("移除"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnRemove);
	m_btnRemove->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);
	nBtnLeft += m_btnRemove->GetSize().x + 5;

	m_btnRemoveall = new ccSkinButton(m_client, wxNewId(), wxT("移除所有"), wxPoint(nBtnLeft, nBtnTop), wxSize(120, 30));
	this->AddSkinButton(m_btnRemoveall);
	m_btnRemoveall->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);

	m_btnFormat = new ccSkinButton(m_client, wxNewId(), wxT("设置"), wxPoint(nBtnLeft, nBtnTop), wxSize(80, 22));
	this->AddSkinButton(m_btnFormat);
	m_btnFormat->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);

	m_btnOutpath = new ccSkinButton(m_client, wxNewId(), wxT("打开"), wxPoint(nBtnLeft, nBtnTop), wxSize(80, 22));
	this->AddSkinButton(m_btnOutpath);
	m_btnOutpath->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);

	m_btnStart = new ccSkinButton(m_client, wxNewId(), wxT("开始转换"), wxPoint(nBtnLeft, nBtnTop), wxSize(155, 46));
	this->AddSkinButton(m_btnStart);
	m_btnStart->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnClick), NULL, this);

	m_vList = new VList(m_client, wxNewId(), wxDefaultPosition, wxDefaultSize);
	m_vList->SetBackgroundColour( wxColour("#485464") );

	m_vPlay = new VPlay(m_client, wxNewId(), wxDefaultPosition, wxDefaultSize);
	m_vPlay->SetBackgroundColour( *wxWHITE );

	m_labFormat = new ccLabel(m_client, wxID_ANY, wxT("输出格式："), wxDefaultPosition, wxSize(80, 22));
	m_labOutpath = new ccLabel(m_client, wxID_ANY, wxT("输出目录："), wxDefaultPosition, wxSize(80, 22));

	m_txtFormat = new wxTextCtrl( m_client, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
	m_txtFormat->SetBackgroundColour(wxColour("#4e5a6c"));
	m_txtOutpath = new wxTextCtrl( m_client, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
	m_txtOutpath->SetBackgroundColour(wxColour("#4e5a6c"));

	wxButton* btn2 = new wxButton(m_vPlay, wxID_ANY, "Close", wxPoint(10, 1));
	btn2->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnQuit), NULL, this);

	ccSkinButton* btn = new ccSkinButton(m_vPlay, wxID_ANY, "Sample", wxPoint(10, 40), wxSize(100, 22));
	btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::OnQuit), NULL, this);
	this->AddSkinButton(btn);

	//for trigger resize, must after set ccDrawSkin
	SetMinSize(wxSize(900, 600));
	CentreOnScreen();
}

MainFrame::~MainFrame()
{
}

void _GetSelFilter(wxString& strfilter)
{
	strfilter.Append(_T("Common media formats|*.avi;*.wmv;*.wmp;*.wm;*.asf;*.rm;*.ram;*.rmvb;*.ra;*.mpg;*.mpeg;*.mpe;*.m1v;*.m2v;*.mpv2;"));
	strfilter.Append(_T("*.mp2v;*.dat;*.mp4;*.m4v;*.m4p;*.vob;*.ac3;*.dts;*.mov;*.qt;*.mr;*.3gp;*.3gpp;*.3g2;*.3gp2;*.swf;*.ogg;*.wma;*.wav;"));
	strfilter.Append(_T("*.mid;*.midi;*.mpa;*.mp2;*.mp3;*.m1a;*.m2a;*.m4a;*.aac;*.mkv;*.ogm;*.m4b;*.tp;*.ts;*.tpr;*.pva;*.pss;*.wv;*.m2ts;*.evo;"));
	strfilter.Append(_T("*.rpm;*.realpix;*.rt;*.smi;*.smil;*.scm;*.aif;*.aiff;*.aifc;*.amr;*.amv;*.au;*.acc;*.dsa;*.dsm;*.dsv;*.dss;*.pmp;*.smk;*.flic|"));
	strfilter.Append(_T("Windows Media Video(*.avi;*wmv;*wmp;*wm;*asf)|*.avi;*.wmv;*.wmp;*.wm;*.asf|"));
	strfilter.Append(_T("Windows Media Audio(*.wma;*wav;*aif;*aifc;*aiff;*mid;*midi;*rmi)|*.wma;*.wav;*.aif;*.aifc;*.aiff;*.mid;*.midi;*.rmi|"));
	strfilter.Append(_T("Real(*.rm;*ram;*rmvb;*rpm;*ra;*rt;*rp;*smi;*smil;*.scm)|*.rm;*.ram;*.rmvb;*.rpm;*.ra;*.rt;*.rp;*.smi;*.smil;*.scm|"));
	strfilter.Append(_T("MPEG Video(*.mpg;*mpeg;*mpe;*m1v;*m2v;*mpv2;*mp2v;*dat;*mp4;*m4v;*m4p;*m4b;*ts;*tp;*tpr;*pva;*pss;*.wv;)|"));
	strfilter.Append(_T("*.mpg;*.mpeg;*.mpe;*.m1v;*.m2v;*.mpv2;*.mp2v;*.dat;*.mp4;*.m4v;*.m4p;*.m4b;*.ts;*.tp;*.tpr;*.pva;*.pss;*.wv;|"));
	strfilter.Append(_T("MPEG Audio(*.mpa;*mp2;*m1a;*m2a;*m4a;*aac;*.m2ts;*.evo)|*.mpa;*.mp2;*.m1a;*.m2a;*.m4a;*.aac;*.m2ts;*.evo|"));
	strfilter.Append(_T("DVD(*.vob;*ifo;*ac3;*dts)|*.vob;*.ifo;*.ac3;*.dts|MP3(*.mp3)|*.mp3|CD Tracks(*.cda)|*.cda|"));
	strfilter.Append(_T("Quicktime(*.mov;*qt;*mr;*3gp;*3gpp;*3g2;*3gp2)|*.mov;*.qt;*.mr;*.3gp;*.3gpp;*.3g2;*.3gp2|"));
	strfilter.Append(_T("Flash Files(*.flv;*swf;*.f4v)|*.flv;*.swf;*.f4v|Playlist(*.smpl;*.asx;*m3u;*pls;*wvx;*wax;*wmx;*mpcpl)|*.smpl;*.asx;*.m3u;*.pls;*.wvx;*.wax;*.wmx;*.mpcpl|"));
	strfilter.Append(_T("Others(*.ivf;*au;*snd;*ogm;*ogg;*fli;*flc;*flic;*d2v;*mkv;*pmp;*mka;*smk;*bik;*ratdvd;*roq;*drc;*dsm;*dsv;*dsa;*dss;*mpc;*divx;*vp6;*.ape;*.flac;*.tta;*.csf)"));
	strfilter.Append(_T("|*.ivf;*.au;*.snd;*.ogm;*.ogg;*.fli;*.flc;*.flic;*.d2v;*.mkv;*.pmp;*.mka;*.smk;*.bik;*.ratdvd;*.roq;*.drc;*.dsm;*.dsv;*.dsa;*.dss;*.mpc;*.divx;*.vp6;*.ape;*.amr;*.flac;*.tta;*.csf|"));
	strfilter.Append(_T("All Files(*.*)|*.*||"));
}

void MainFrame::OnClick( wxCommandEvent &event )
{
	int nId = event.GetId();
	if(nId == m_btnAddFile->GetId())
	{
		wxString filter;
		_GetSelFilter(filter);
		wxFileDialog* OpenDialog = new wxFileDialog(
			this, _("Choose a file to open"), wxEmptyString, wxEmptyString, filter, wxFD_OPEN, wxDefaultPosition);
		// Creates a "open file" dialog with 4 file types
		if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
		{
			wxImage frame;
			wxString docPath = OpenDialog->GetPath();
			VRow* row = new VRow(docPath, m_vList, wxNewId(), wxPoint(0, 0), wxSize(500, 78));
			m_vList->AddRow(row);
		}

		// Clean up after ourselves
		OpenDialog->Destroy();
	}
	else if(nId == m_btnPlay->GetId())
	{
		int cnt = m_vList->GetRowCount();
		if(cnt < 1)
		{
			(void)wxMessageBox( wxT("必须先添加文件才可以开始播放。"), wxT("错误提示"), wxICON_STOP | wxOK );
			return;
		}
		wxString sPath = m_vList->GetPath(0);
		if(!ccUtil::fExist(sPath))
		{
			(void)wxMessageBox( wxT("选择的文件不存在，请重新选择。"), wxT("错误提示"), wxICON_STOP | wxOK );
			return;
		}

		void* handle = (void *)m_vPlay->GetPlayPanel(true)->GetHandle();
	}
	else if(nId == m_btnEdit->GetId())
	{
	}
	else if(nId == m_btnRemove->GetId())
	{
	}
	else if(nId == m_btnRemoveall->GetId())
	{
		m_vList->RemoveRowAll();
	}
	else if(nId == m_btnStart->GetId())
	{
		int cnt = m_vList->GetRowCount();
		if(cnt < 1)
		{
			(void)wxMessageBox( wxT("必须先添加文件才可以开始转换。"), wxT("错误提示"), wxICON_STOP | wxOK );
			return;
		}

		wxString sDesPath = m_txtOutpath->GetValue();
		if(sDesPath.IsEmpty())
		{
			(void)wxMessageBox( wxT("必须先选择输出目录才可以开始转换。"), wxT("错误提示"), wxICON_STOP | wxOK );
			return;
		}
		if(!ccUtil::dExist(sDesPath))
		{
			ccUtil::dCreate(sDesPath);
			if(!ccUtil::dExist(sDesPath))
			{
				(void)wxMessageBox( wxT("设置的输出目录创建失败，请选择正确的输出路径。"), wxT("错误提示"), wxICON_STOP | wxOK );
				return;
			}
		}
		for(int n = 0; n < cnt; n++)
		{
			wxString sPath = m_vList->GetPath(n);
			if(!ccUtil::fExist(sPath))
			{
				(void)wxMessageBox( wxT("选择的文件不存在，请重新选择。"), wxT("错误提示"), wxICON_STOP | wxOK );
				return;
			}
			//turn_one(sPath, sDesPath);
		}
	}
}

int MainFrame::turn_one(const wxString sPath, const wxString sDesPath)
{
	return 0;
}

void MainFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
    Close( true );
}

void MainFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
    (void)wxMessageBox( wxT("wxDragImage demo\n")
        wxT("Julian Smart (c) 2000"),
        wxT("About wxDragImage Demo"),
        wxICON_INFORMATION | wxOK );
}

void MainFrame::OnMenuLang( wxCommandEvent &event )
{
	int nid = event.GetId() - CC_MENUID_LANG_START;
	wxArrayString* pLang = ccApp::GetLangNames();
	if(nid >= 0 && nid < (int)pLang->size())
	{
		wxString sItem = pLang->Item(nid);
		for(int i = 0; i < (int)submLang->GetMenuItemCount(); i++)
		{
			wxMenuItem* item = submLang->FindItemByPosition(i);
			item->Check(i == nid ? true : false);
		}

		wxString pfile = ccUtil::GetStartupReleatedPath(wxT("langs"));
		pfile.append(sItem).append(wxT(".lang"));
		if(ccUtil::fExist(pfile))
		{
			ccApp::GetLang()->Load(pfile, wxConvUTF8);
			ccApp::GetLang()->SetPath(pfile);
			ccApp::GetIni()->Set(wxT("Main.Lang"), sItem);
			this->SendSizeEvent(wxSEND_EVENT_POST);
		}
		else
		{
			(void)wxMessageBox( pfile, wxT("error Demo"), wxICON_STOP | wxOK );
		}
		this->Refresh(true);
	}
}

void MainFrame::OnMenuSkin( wxCommandEvent &event )
{
	int nid = event.GetId() - CC_MENUID_SKIN_START;
	wxArrayString* pSkin = ccApp::GetSkinNames();
	if(nid >= 0 && nid < (int)pSkin->size())
	{
		wxString sItem = pSkin->Item(nid);
		for(int i = 0; i < (int)submSkin->GetMenuItemCount(); i++)
		{
			wxMenuItem* item = submSkin->FindItemByPosition(i);
			item->Check(i == nid ? true : false);
		}

		wxString pfile = ccUtil::GetStartupReleatedPath(wxT("skins"));
		pfile.append(sItem).append(wxT(".skin"));
		if(ccUtil::fExist(pfile))
		{
			m_skin->LoadSkin(pfile);
			this->SetDrawSkin(m_skin);
			ccApp::GetIni()->Set(wxT("Main.Skin"), sItem);
			this->SendSizeEvent(wxSEND_EVENT_POST);
		}
		else
		{
			(void)wxMessageBox( pfile, wxT("error Demo"), wxICON_STOP | wxOK );
		}
		this->Refresh(true);
	}
}

void MainFrame::OnMenuCmd( wxCommandEvent &event )
{
	int nid = event.GetId();
	if(nid >= CC_MENUID_LANG_START && nid < CC_MENUID_LANG_START + (int)submLang->GetMenuItemCount())
	{
		OnMenuLang(event);
		return;
	}
	else if(nid >= CC_MENUID_SKIN_START && nid < CC_MENUID_SKIN_START + (int)submSkin->GetMenuItemCount())
	{
		OnMenuSkin(event);
		return;
	}
	else if(nid == itemAbout->GetId())
	{
		this->OnAbout(event);
	}
	else if(nid == itemExit->GetId())
	{
		this->OnQuit(event);
	}
}

void MainFrame::OnResize(wxSizeEvent& event)
{
	ccSkinFrame::OnResize(event);

    const wxSize size = m_client->GetSize();
	int nLeft = size.x * 0.7;
	m_vList->SetSize(0, m_btnAddFile->GetSize().y + 6, nLeft, size.y - 100);
	m_vPlay->SetSize(nLeft, m_btnAddFile->GetSize().y + 6, size.x - nLeft, size.y - 100);

	m_labFormat->SetPosition(wxPoint(5, size.y - 55));
	m_labOutpath->SetPosition(wxPoint(5, size.y - 30));
	m_txtFormat->SetSize(90, size.y - 55, nLeft - 90, 22);
	m_txtOutpath->SetSize(90, size.y - 30, nLeft - 90, 22);

	m_btnFormat->SetPosition(wxPoint(nLeft + 5, size.y - 55));
	m_btnOutpath->SetPosition(wxPoint(nLeft + 5, size.y - 30));
	m_btnStart->SetPosition(wxPoint(nLeft + 5 + 90, size.y - 55));
}

IMPLEMENT_APP(MainApp)

//-----------------------------------------------------------------------------
// MainApp
//-----------------------------------------------------------------------------
MainApp::MainApp()
{
}

bool MainApp::OnInit()
{
	if ( !ccApp::OnInit() )
	{
		return false;
	}

	//ccStr::test();
	//ccStrMap::test();
	ccUtil::test();
	ccApp::test();

	wxString vendorName = wxT("vendorName");
	wxString appName = wxT("appName");
	ccApp::SetApp(vendorName, appName);
   ::wxInitAllImageHandlers();
   //wxFileSystem::AddHandler(new wxZipFSHandler);

#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif

    MainFrame *frame = new MainFrame();

#if 0
    // Under Motif or GTK, this demonstrates that
    // wxScreenDC only gets the root window content.
    // We need to be able to copy the overall content
    // for full-screen dragging to work.
    int w, h;
    wxDisplaySize(& w, & h);
    wxBitmap bitmap(w, h);

    wxScreenDC dc;
    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);
    memDC.Blit(0, 0, w, h, & dc, 0, 0);
    memDC.SelectObject(wxNullBitmap);
    m_background = bitmap;
#endif

    frame->Show( true );

    return true;
}

int MainApp::OnExit()
{
    return ccApp::OnExit();
}
