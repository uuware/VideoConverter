/////////////////////////////////////////////////////////////////////////////
// Name:        ccSkinFrame.h
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CC_CONTROL_CTL_
#define _WX_CC_CONTROL_CTL_

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
#include "cc_app.h"

class ccSkinFrame;
class ccSkinButtonBase;
class ccSkinButton;
class ccSkinTool;

#define _CC_ccDrawSkin_CONVERT_DISABLED_ 100

#define _CC_ccDrawSkin_NORMAL_ 0
#define _CC_ccDrawSkin_MOUSEON_ 1
#define _CC_ccDrawSkin_PRESSED_ 2
#define _CC_ccDrawSkin_DISABLED_ 3
// derived classes
class ccDrawSkin: public wxObject
{
public:
    ccDrawSkin();
    ~ccDrawSkin();

	static wxBitmap Scale(wxBitmap& bitmapOrg, int width, int height);
	static bool ChangeImageColour(wxImage& srcImage, wxColour & srcColour, wxColour & dstColour);
	static bool OverlayImage(wxImage * background, wxImage * foreground,
						  wxImage * mask, int xoff, int yoff);
	static void PasteSubImage( wxImage * background, wxImage * foreground, int xoff, int yoff );
	static void PasteSubImage( wxImage& background, wxImage& foreground, int xoff, int yoff, int toW, int toH);
	static bool GetSubImage( const wxImage & Src, const wxRect &rect, wxImage& Dst );
	static bool SkinRegion(wxRegion& region, const wxImage& image, int tolerance);
	static bool SkinRegion(wxRegion& region, wxBitmap& bitmap, int tolerance);

	virtual bool LoadSkin(const wxString& sXmlPath);
	static bool DrawText(wxMemoryDC& memDC, const wxString& textOrig, const wxRect& rect, int alignment = wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	virtual bool DrawFrame(ccSkinFrame* ccSkinFrame, wxBitmap& bitmap, const wxBitmap& titleIcon);
	virtual bool DrawButton(ccSkinButtonBase* ccSkinButton, wxBitmap& bitmap, int nType);
	//static bool FillImage(wxMemoryDC& dc, const wxBitmap& bitmap, int nStartW, int nStepW, int nEndW, int nStartH, int nStepH, int nEndH);
	virtual bool DrawTool(wxColour backColour, wxSize size, const wxString& sXmlTagName, wxBitmap& bitmapNormal, wxBitmap& bitmapMouseon, wxBitmap& bitmapPressed, wxBitmap& bitmapDisabled);
	virtual wxString GetValue(const wxString& sXmlPath);
	virtual int GetValue(const wxString& sXmlPath, int notFound);
	int GetTopWidth()
	{
		return m_nTop;
	}
	int GetBottomWidth()
	{
		return m_nBottom;
	}
	int GetLeftRightWidth()
	{
		return m_nLeftRight;
	}
	bool IsLoaded()
	{
		return m_isLoaded;
	}
    void SetBackColour(const wxColour& colour)
    {
		m_backColour = colour;
    }
    wxColour GetBackColour() const
	{
		return m_backColour;
	}
    void SetTransparentColour(const wxColour& colour)
    {
		m_transparentColor = colour;
    }
    wxColour GetTransparentColour() const
	{
		return m_transparentColor;
	}
    //int GetFrameRadius() const
	//{
	//	return m_frameRadius;
	//}
	static bool DrawShape(wxRegion& bitmapRegion, const wxBitmap& bitmap);

protected:
    wxWindowList m_target;
	ccPointMap m_mapImg;
	ccXml xml;
	wxString m_xmlBasePath;
	wxImage m_imgBack[8];
	int m_nTop;
	int m_nBottom;
	int m_nLeftRight;
	bool m_isLoaded;
	wxColour m_backColour;
	wxColour m_transparentColor;
	wxColour m_titleColor;

	//int m_frameRadius;
	//int m_frameIconsize;
	int m_frameIconLeft;
	int m_frameIconTop;
	int m_frameTitleTop;

	wxImage m_imgTopLeft;
	wxImage m_imgTopTitleLeft;
	wxImage m_imgTopTitle;
	wxImage m_imgTopTitleRight;
	wxImage m_imgTopCenter;
	wxImage m_imgTopRight;
	wxImage m_imgBottomLeft;
	wxImage m_imgBottomCenter;
	wxImage m_imgBottomRight;
	wxImage m_imgLeft;
	wxImage m_imgRight;

#define _CC_ccDrawSkin_BTN_NORMAL_ 0
#define _CC_ccDrawSkin_BTN_MOUSEON_ 1
#define _CC_ccDrawSkin_BTN_PRESSED_ 2
#define _CC_ccDrawSkin_BTN_DISABLED_ 3
	wxImage m_ccSkinButtonLeft[4];
	wxImage m_ccSkinButtonCenter[4];
	wxImage m_ccSkinButtonRight[4];
	wxImage m_imgMinimize[4];
	wxImage m_imgMaximize[4];
	wxImage m_imgClose[4];
	wxImage m_imgTopmenu[4];

	virtual bool LoadImg(const wxString& sXmlPath, const wxString& sRect, wxImage& Dst);
};

// derived classes
class ccLabel: public wxStaticText
{
public:
    ccLabel(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    ~ccLabel();

protected:

	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnEraseBackground(wxEraseEvent& event);
};

// derived classes
class ccCheckbox: public wxCheckBox
{
public:
    ccCheckbox(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    ~ccCheckbox();

protected:

	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnEraseBackground(wxEraseEvent& event);
};

class ccClient: public wxScrolledWindow
{
public:
    ccClient( ccSkinFrame *parent, wxWindowID, const wxPoint &pos, const wxSize &size, long style = wxNO_BORDER );
    ~ccClient();

private:
    ccSkinFrame *m_owner;
};

class ccStatusBar: public wxPanel
{
public:
    ccStatusBar( wxWindow *parent, int height = 19, long style = wxNO_BORDER );
    ~ccStatusBar();

private:
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnEraseBackground(wxEraseEvent& event);
	virtual void OnResize(wxSizeEvent& event);
};

// derived classes
class ccSkinFrame: public wxFrame
{
public:
	ccSkinFrame()
	{
		m_ccDrawSkin = NULL;
	}
    ccSkinFrame(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize);
    ~ccSkinFrame();
	void SetDrawSkin(ccDrawSkin* ccDrawSkin);
	void SetTopMenu(wxMenu *topMenu);
	void AddAutoDestroy(wxObject* pObj)
	{
		m_lstDestroy.Append(pObj);
	}
    void AddSkinButton(ccSkinButton* imgBtn);
    void RemoveSkinButton(ccSkinButton* imgBtn);
	void UpdateSkin();
    void AddConfigControl(const wxString& cfgName, wxWindow* ctl, bool autoCfg, bool autoLang)
	{
		wxString value;
		if(autoCfg)
		{
			m_cfgControl[cfgName] = ctl;
			if(ccApp::GetIni()->Exist(cfgName, value))
			{
				ctl->SetLabel(value);
			}
		}
		if(autoLang)
		{
			m_langControl[cfgName] = ctl;
			if(ccApp::GetLang()->Exist(cfgName, value))
			{
				ctl->SetLabel(value);
			}
		}
	};
	void Config(bool isFromIni, ccStrMap& keyArr, const wxString& sParentHead)
	{
	};

    ccClient* GetClient() const { return m_client; }
    void SetClient(ccClient* client)
	{
		if(m_client != NULL)
		{
			delete m_client;
			m_client = NULL;
		}
		m_client = client;
	}

    virtual ccStatusBar* GetStatusPanel() const { return m_statusbar; }
    virtual void SetStatusPanel(ccStatusBar* statusbar)
	{
		if(m_statusbar != NULL)
		{
			delete m_statusbar;
			m_statusbar = NULL;
		}
		m_statusbar = statusbar;
	}
    const wxBitmap* GetBackBitmap() const { return m_bitmapBack; }

protected:
	ccPointMap m_cfgControl;
	ccPointMap m_langControl;
	ccClient* m_client;
	ccStatusBar* m_statusbar;
	wxList m_lstDestroy;
	wxList m_lstSkinBtn;

	ccSkinTool* toolMin;
	ccSkinTool* toolMax;
	ccSkinTool* toolMaxMax;
	ccSkinTool* toolClose;
	ccSkinTool* toolMenu;
	// same for control of close/max/min
	int m_nToolWidth;
	wxPoint m_oldMousePos;
	wxRect m_oldWinRect;
	int m_DirectMode;
	wxCursor* m_cursorNS;
	wxCursor* m_cursorWE;
	wxCursor* m_cursorNESW;
	wxCursor* m_cursorNWSE;
	ccDrawSkin* m_ccDrawSkin;
	wxBitmap* m_bitmapBack;
    wxMenu *m_topMenu;
	wxBitmap m_titleIcon;

	int m_controloxTopside;
	int m_controloxRightside;
	int m_controloxSplitwidth;
	int m_frameLeft;
	int m_frameTop;
	int m_frameRight;
	int m_frameBottom;
	int m_titleIconSize;

	virtual void OnResize(wxSizeEvent& event);
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnEraseBackground(wxEraseEvent& event);
	virtual void OnCaptureLost(wxMouseCaptureLostEvent& event);
	//virtual void OnEnterEvent(wxMouseEvent& event);
	virtual void OnLeaveEvent(wxMouseEvent& event);
	virtual void OnLeftDownEvent(wxMouseEvent& event);
	virtual void OnLeftUpEvent(wxMouseEvent& event);
	virtual void OnMotionEvent(wxMouseEvent& event);
	virtual void OnLeftDoubleClickEvent(wxMouseEvent& event);

	virtual void OnBoxMin(wxCommandEvent& event);
	virtual void OnBoxMax(wxCommandEvent& event);
	virtual void OnBoxClose(wxCommandEvent& event);
	virtual void OnBoxMenu(wxCommandEvent& event);

	virtual void GetBackBitmap(bool isReCreate);
#if wxUSE_STATUSBAR
    virtual void PositionStatusBar();
#endif // wxUSE_STATUSBAR
};

// derived classes
class ccSkinButtonBase: public wxButton
{
public:
	ccSkinButtonBase() : wxButton()
	{
		m_ccDrawSkin = NULL;
	};
    ccSkinButtonBase(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize);
    ~ccSkinButtonBase();
	void SetDrawSkin(ccDrawSkin* ccDrawSkin) { m_ccDrawSkin = ccDrawSkin; }
	void UpdateSkin();

protected:
	ccDrawSkin* m_ccDrawSkin;
	wxBitmap m_bitmapBack;

	virtual void OnResize(wxSizeEvent& event);
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnEraseBackground(wxEraseEvent& event);
	virtual void OnEnterEvent(wxMouseEvent& event);
	virtual void OnLeaveEvent(wxMouseEvent& event);
	virtual void OnLeftDownEvent(wxMouseEvent& event);
	virtual void OnLeftUpEvent(wxMouseEvent& event);
	virtual void OnMotionEvent(wxMouseEvent& event);
	virtual void OnCaptureLost(wxMouseCaptureLostEvent& event);
	virtual void OnLeftDoubleClickEvent(wxMouseEvent& event);

	virtual void doDrawBack(int nType);
	virtual void doOnResize();
    //wxDECLARE_DYNAMIC_CLASS(ccSkinButton);
    //wxDECLARE_EVENT_TABLE();
};

// derived classes
class ccSkinButton: public ccSkinButtonBase
{
public:
	ccSkinButton() : ccSkinButtonBase()
	{
	};
    ccSkinButton(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize);
    ~ccSkinButton();

protected:
	virtual void doDrawBack(int nType);

};

// derived classes
class ccSkinTool: public ccSkinButtonBase
{
public:
	ccSkinTool() : ccSkinButtonBase()
	{
	};
    ccSkinTool(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize);
    ~ccSkinTool();
	void SetBackImage(const wxBitmap& bitmapNormal, const wxBitmap& bitmapMouseon, const wxBitmap& bitmapPressed, const wxBitmap& bitmapDisabled);

protected:
	wxBitmap m_bitmapNormal;
	wxBitmap m_bitmapMouseon;
	wxBitmap m_bitmapPressed;
	wxBitmap m_bitmapDisabled;

	virtual void doDrawBack(int nType);
	virtual void doOnResize();
};

#endif
    // _WX_CC_CONTROL_CTL_
