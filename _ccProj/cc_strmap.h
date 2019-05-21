/////////////////////////////////////////////////////////////////////////////
// Name:        ImgFrame.h
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CC_STRMAP_CTL_
#define _WX_CC_STRMAP_CTL_

#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/xml/xml.h>
#include <wx/wfstream.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/settings.h>
#include <wx/fileconf.h>
#include <wx/ffile.h>
#include <wx/log.h>
#include <wx/image.h>
#include <wx/graphics.h>
#include <wx/filefn.h>
#include <wx/dir.h>

// derived classes
class ccStr: public wxString
{
public:
    ccStr();
	ccStr(const ccStr& stringSrc) : wxString(stringSrc) { }
	ccStr(const wxString& stringSrc) : wxString(stringSrc) { }
    ~ccStr();

	// look for [str + nStart, str + length) beginning after nStart
	static int indexOf(const wxString& str, const wxString& s, bool isIgnorecase = false, size_t nStart = 0);
	// look for [str, str + length) beginning before nLastStart(0 means from last)
	static int lastIndexOf(const wxString& str, const wxString& s, bool isIgnorecase = false, size_t nLastStart = 0);
	static bool Splite(const wxString& str, const wxString& sep, wxString& left, wxString& right);
	static int Splite(const wxString& str, const wxString& sep, wxArrayString& result);
	static wxString& Trim(wxString& sSrc, bool bTrimSpace, bool bTrimTab, bool bTrimCrln, bool bFromLeft, bool bFromRight);
	static bool StartsWith(const wxString& sSrc, const wxString& prefix, bool isIgnorecase = false);
	static bool EndsWith(const wxString& sSrc, const wxString& sPre, bool isIgnorecase = false);

  // overloaded assignment
  // from another wxString
	ccStr& operator=(const wxString& stringSrc)
	{
		if ( this != (ccStr*)&stringSrc )
		{
			wxSTRING_INVALIDATE_CACHE();

			this->Clear();
			this->Append(stringSrc);
		}

		return *this;
	}

protected:

public:
	static void test();
};

#define CC_FLAG_KEYARR_START				0x0001
#define CC_FLAG_KEYARR_DIRTY				CC_FLAG_KEYARR_START
#define CC_FLAG_KEYARR_RETURNKEY			CC_FLAG_KEYARR_START <<1
#define CC_FLAG_KEYARR_IGNORECASE			CC_FLAG_KEYARR_START <<2
#define CC_FLAG_KEYARR_LOGNOTFOUNT			CC_FLAG_KEYARR_START <<3
#define CC_FLAG_KEYARR_SORT					CC_FLAG_KEYARR_START <<4
class ccStrMap : public wxObject
{
public:
	ccStrMap(bool isSort = false, bool isReturnKeyIfNoFound = false, bool isIgnorecase = false);
	virtual ~ccStrMap();

	void Clear();
	//clear one pare
	void Clear(const wxString& keyStartWith);
	bool Remove(const wxString& key);
	int LoadFromString(const wxString& sTxt);
	int Load(const wxString& filepath, const wxMBConv& conv = wxConvUTF8);
	void SaveToString(wxString& sTxt) const;
	bool Save(const wxString& filepath, const wxMBConv& conv = wxConvUTF8, wxBOM bom = wxBOM_None) const;
	//cannot be self!
	int Add(const ccStrMap& keyArr, const wxString& keyStartWith = wxEmptyString, const wxString& preKeyTurnTo = wxEmptyString);
	int Add(const wxStringToStringHashMap& keyArr, const wxString& keyStartWith = wxEmptyString, const wxString& preKeyTurnTo = wxEmptyString);
	//int Add(unsigned int nResID, const TCHAR *resType, const wxMBConv& conv = wxConvAuto());
	//int Add(const TCHAR *resName, const TCHAR *resType, const wxMBConv& conv = wxConvAuto());
	const wxStringToStringHashMap* GetMap();
	bool Exist(const wxString& key) const;
	bool Exist(const wxString& key, wxString& getvalue) const;
	//key:like:Main.title,if not found,return wxString(key).Append(_T("[NOFOUND]")) of _bIsReturnKey or wxString()
	wxString Get(const wxString& key) const;
	wxString Get(const wxString& key, const wxString& defaultValue) const;
	long Get(const wxString& key, long nValue) const;
	bool GetBool(const wxString& key, bool bValue) const;
	void Set(const wxString& key, const wxString& value);
	void Set(const wxString& key, long dwValue);
	void SetBool(const wxString& key, bool bValue);
	wxString GetFormat(const wxString& key, ...) const;
	wxString GetFormatDefaultValue(const wxString& defaultValue, const wxString& key, ...) const;
	void SetIsDirty(bool isDirty);
	bool GetIsDirty() const;
	bool GetIsEmpty() const;
	//for lang, log not exist key
	void SetReturnNoFound(bool isReturnKey);
	bool GetIsSort() const;
	bool GetIsIgnoreCase() const;
	//void SetIsIgnoreCase(bool isIgnoreCase);
	bool GetIsLogNotFound() const;
	void SetIsLogNotFound(bool isLogNotFound);
	int GetSize() const;
	void SetPath(const wxString& filepath);
	wxString GetPath() const;

	void Dump(const TCHAR* info = NULL) const;

protected:
	DWORD _Flags;
	wxStringToStringHashMap m_KeyArr;
	wxArrayString m_KeySort;
	wxString m_LastPath;

	wxStringToStringHashMap::const_iterator find(const wxString& key) const;

public:
	static void test();
};

#endif
    // _WX_CC_STRMAP_CTL_
