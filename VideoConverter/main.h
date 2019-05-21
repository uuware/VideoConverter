/////////////////////////////////////////////////////////////////////////////
// Name:        dragimag.h
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DRAGIMAGSAMPLE_
#define _WX_DRAGIMAGSAMPLE_

// derived classes

class MainFrame;
class MainApp;
class VRow;
class VList;
class VPlay;

// MainFrame
#define DIRECT_R   1
#define DIRECT_B   2
#define DIRECT_L   3
#define DIRECT_T   4

#define CC_VROW_FLAG_SELECTED   0x0001
#define CC_VROW_FLAG_MOUSEON   0x0002

class VRow: public wxPanel
{
public:
    VRow( const wxString& path, VList *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    ~VRow();

	bool IsSelected()
	{
		return CC_BIT_IS(m_flag, CC_VROW_FLAG_SELECTED);
	}
	void SetSelected(bool isSelected)
	{
		bool oldIsSelected = IsSelected();
		CC_BIT_SET(m_flag, CC_VROW_FLAG_SELECTED, isSelected);
		if(oldIsSelected != isSelected)
		{
			this->Refresh();
		}
	}
	wxString GetPath()
	{
		return m_vpath;
	}

private:
    VList *m_owner;
	unsigned int m_flag;
	wxString m_vpath;

	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnEraseBackground(wxEraseEvent& event);
	virtual void OnEnterEvent(wxMouseEvent& event);
	virtual void OnLeaveEvent(wxMouseEvent& event);
	virtual void OnLeftClickEvent(wxMouseEvent& event);
};

class VList: public wxScrolledWindow
{
public:
    VList( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    ~VList();

    void AddRow(VRow* vRow);
    void RemoveRow(VRow* vRow);
    void RemoveRowAll();
    void ReLayout();
	void SetSelected(VRow* vRow);

    int GetRowCount();
	wxString GetPath(int index);

private:
    wxWindow *m_owner;
	VRow* m_selectedRow;
    wxList m_List; // A list of vrow

	void OnResize(wxSizeEvent& WXUNUSED(event));
};

class VPlay: public wxPanel
{
public:
    VPlay( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    ~VPlay();
    wxPanel* GetPlayPanel(bool isReset);

private:
    wxWindow *m_owner;
    wxPanel *m_play;

	void OnResize(wxSizeEvent& WXUNUSED(event));
};

class MainFrame: public ccSkinFrame
{
public:
    MainFrame();
    ~MainFrame();

protected:
	ccDrawSkin* m_skin;
	wxMenuItem *itemSkin1;
	wxMenuItem *itemSkin2;
	wxMenuItem *itemSkin3;
	wxMenuItem* itemAbout;
	wxMenuItem* itemExit;
	ccSkinButton* m_btnAddFile;
	ccSkinButton* m_btnPlay;
	ccSkinButton* m_btnEdit;
	ccSkinButton* m_btnRemove;
	ccSkinButton* m_btnRemoveall;
	ccSkinButton* m_btnFormat;
	ccSkinButton* m_btnOutpath;
	ccSkinButton* m_btnStart;
	VList* m_vList;
	VPlay* m_vPlay;
	wxStaticText* m_labFormat;
	wxStaticText* m_labOutpath;
	wxTextCtrl* m_txtFormat;
	wxTextCtrl* m_txtOutpath;
	wxCheckBox* m_chkShutdown;

	//void OnPaint(wxPaintEvent& event);
	//void OnEraseBackground( wxEraseEvent &event );
	void OnResize(wxSizeEvent& WXUNUSED(event));

    void OnAbout( wxCommandEvent &event );
    void OnMenuSkin( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnClick( wxCommandEvent &event );

    int turn_one(const wxString sPath, const wxString sDesPath);
};

// MainApp
class MainApp: public wxApp
{
public:
    MainApp();
    virtual bool OnInit();
    virtual int OnExit();

protected:
};

DECLARE_APP(MainApp)

#endif
    // _WX_DRAGIMAGSAMPLE_
