/////////////////////////////////////////////////////////////////////////////
// Name:        ImgFrame.h
// Purpose:     wxDragImage sample
// Author:      Julian Smart
// Modified by:
// Created:     28/2/2000
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CC_UTIL_CTL_
#define _WX_CC_UTIL_CTL_

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

#include "cc_strmap.h"

#define CC_MAX(a, b) ((a) >= (b) ? (a) : (b))
#define CC_MIN(a, b) ((a) <= (b) ? (a) : (b))
#define CC_ABS(a) ((a) >= 0 ? (a) : -(a))
#define CC_RANGE(x, lower, upper) (((x) >= (lower) && (x) <= (upper)))
#define CC_RANGE_LESS(x, lower, upper) (((x) > (lower) && (x) < (upper)))

//CC_BIT_IS(VAR, BIT1|BIT2|BIT3): return true if has BIT1 and BIT2 and BIT3
#define CC_BIT_IS(VAR, BIT)				( ((VAR) & (BIT)) == (BIT) )
//CC_BIT_HAS(VAR, BIT1|BIT2|BIT3): return true if has any BIT1 or BIT2 or BIT3
#define CC_BIT_HAS(VAR, BIT)			( ((VAR) & (BIT)) != 0 )
#define CC_BIT_SET(VAR, BIT, VALUE)		if(VALUE){(VAR) |= (BIT);} else {(VAR) &= ~(BIT);}
#define CC_BIT_ON(VAR, BIT)				( (VAR) |= (BIT) )
#define CC_BIT_OFF(VAR, BIT)			( (VAR) &= ~(BIT) )

//not delete point save to this Map!
WX_DECLARE_STRING_HASH_MAP(void* , ccPointMap);
//not delete point save to this List!
//WX_DECLARE_LIST_2(void, ccPointList, ccPointNode, class WXDLLIMPEXP_BASE);

#define _CCXML_TRIMSPACE_ 0x0001
#define _CCXML_TRIMTAB_ 0x0002
#define _CCXML_TRIMCRLN_ 0x0004

/// Length of the byte-order-mark (BOM) which consists of the three bytes 0xEF, 0xBB and 0xBF
/// in UTF-8 encoding.
#define nBOMLen 3
/// Length of the byte-order-mark (BOM) which consists of the two bytes 0xFF and 0xFE in
/// in UTF-16 encoding.
#define nU16BOMLen 2
static unsigned char szBOM[nBOMLen] = {0xEF, 0xBB, 0xBF};
static unsigned char szU16BOM[nU16BOMLen] = {0xFF, 0xFE};

#define nBOMLen_UTF32LE 4
static unsigned char szBOM_UTF32LE[nBOMLen_UTF32LE] = {0xFF, 0xFE, 0x00, 0x00};
#define nBOMLen_UTF32BE 4
static unsigned char szBOM_UTF32BE[nBOMLen_UTF32BE] = {0x00, 0x00, 0xFE, 0xFF};
#define nBOMLen_UTF16LE 2
static unsigned char szBOM_UTF16LE[nBOMLen_UTF16LE] = {0xFF, 0xFE};
#define nBOMLen_UTF16BE 2
static unsigned char szBOM_UTF16BE[nBOMLen_UTF16BE] = {0xFE, 0xFF};
#define nBOMLen_UTF8 3
static unsigned char szBOM_UTF8[nBOMLen_UTF8] = {0xEF, 0xBB, 0xBF};
#define nBOMLen_UTF7 5
static unsigned char szBOM_UTF7[nBOMLen_UTF7] = {0x2B, 0x2F, 0x76, 0x38, 0x2D};

// derived classes
class ccXml: public wxObject
{
public:
    ccXml();
    ~ccXml();

	bool IsTrim();
	void SetTrim(bool value);
	bool IsTrimTab();
	void SetTrimTab(bool value);
	bool IsTrimCrln();
	void SetTrimCrln(bool value);
	bool Load(const wxString& sXmlPath);
	void Clear();
	//if not found, return -1
	//GetCount(wxT("/root/sub1[2]/sub2"));
	long GetSize(const wxString& sXmlPath);
	wxString GetValue(const wxString& sXmlPath);
	int GetValue(const wxString& sXmlPath, int notFound);
	wxString GetRootName()
	{
		return m_rootName;
	}
	wxString GetAttribute(const wxString& sXmlPath, const wxString& sName);
	wxString Dump();

protected:
	wxStringToStringHashMap m_hasXml;
	wxStringToNumHashMap m_hasHeader;
	unsigned long m_flag;
	wxString m_rootName;

	bool LoadSub(wxXmlNode *child, wxString& sParent);
	bool LoadAttribute(wxXmlNode *child, wxString& sParent);
};

// derived classes
class ccUtil: public wxObject
{
public:
    ccUtil();
    ~ccUtil();

	static bool fExist(const wxString& sPath) { return wxFileExists(sPath); };
	//wxConvUTF8
	static bool fRead(const wxString& sPath, wxString& data, const wxMBConv& conv = wxConvAuto());
	static bool fWrite(const wxString& sPath, const wxString& data, const wxMBConv& conv = wxConvAuto(), bool isAppend = false, wxBOM bom = wxBOM_None);
	static bool fRemove(const wxString& sPath) { return wxRemoveFile(sPath); };
	static bool fWritable(const wxString& sPath) { return wxIsWritable(sPath); };
	static wxULongLong fSize(const wxString& sPath) { return wxFileName::GetSize(sPath); };
	static bool dExist(const wxString& sPath) { return wxDirExists(sPath); };
	static bool dRemove(const wxString& sPath) { return wxRmdir(sPath); };
	static bool dCreate(const wxString& sPath) { return wxFileName::Mkdir(sPath); };
	static bool dCreateAll(const wxString& sPath) { return wxFileName::Mkdir(sPath, 0777, wxPATH_MKDIR_FULL); };
	static bool dWritable(const wxString& sPath) { return wxIsWritable(sPath); };
	static bool dFiles(const wxString& sPath, wxArrayString& result, const wxString fileSpec = wxT("*.*"), UINT flag = wxDIR_DEFAULT);

	static wxString CreateTempFileName(const wxString& prefix, wxFile *fileTemp = NULL)
	{
		return wxFileName::CreateTempFileName(prefix, fileTemp);
	}

	// Construct full path with name and ext
	static wxString GetComparePath(const wxString& sAbsolute, const wxString& ReleatedPath);
	// Construct full path with name and ext
	static wxString GetAbsoluteFullPath(const wxString& sFilePath);
	static wxString GetAbsolutePathOnly(const wxString& sFilePath);
	static wxString GetStartupPath(bool withFilename = true, bool withFileext = true);
	static wxString GetStartupReleatedPath(const wxString& ReleatedPath);
	static wxString GetFilename(const wxString& sFilePath)
	{
		return wxFileNameFromPath(sFilePath);
	}
	static wxString GetParentPath(const wxString& sFilePath)
	{
		if(sFilePath.EndsWith('\\') || sFilePath.EndsWith('/'))
		{
			wxString sFilePath2 = sFilePath.substr(0, sFilePath.Len() - 1);
			return wxPathOnly(sFilePath2);
		}
		return wxPathOnly(sFilePath);
	}

	//String like:left,top,right,bottom
	static wxRect RectFromString(const wxString& sRect);

	static int GetScrollBarWidth()
	{
		return wxSystemSettingsNative::GetMetric(wxSYS_HSCROLL_Y);
	}
	static int GetScrollBarHeight()
	{
		return wxSystemSettingsNative::GetMetric(wxSYS_VSCROLL_X);
	}

	static int GetRandom(int min, int max, bool init_srand = false);
	static wxString GetRandomString(const wxString& sChars, int nMinLen, int nMaxLen);

	//int langid=wxLANGUAGE_FRENCH; 
	//int langid=wxLANGUAGE_ENGLISH_UK; 
	static wxString GetSysLanguage(int langDefault = wxLANGUAGE_ENGLISH_US);
	static void test();

protected:
};

/*
// derived classes
class ccLang: public wxObject
{
public:
    ccLang();
    ~ccLang();
	static bool Load(const wxString& sLangFile, const wxString& sLanguage);
	static bool Write();
	static bool Write(const wxString& sLangFile);
	static wxString GetLang(const wxString& sKey, const wxString& sDefault = wxEmptyString);
	static bool GetLang(const wxString& sKey, bool nDefault);
	static int GetLang(const wxString& sKey, int nDefault);
	static double GetLang(const wxString& sKey, double nDefault);
	static void SetLang(const wxString& sKey, const wxString& sValue);
	static bool GetLangArray(wxArrayString& arr);
	static void Clear();

protected:
	static wxString m_LangFile;

};
*/

#endif
    // _WX_CC_UTIL_CTL_
