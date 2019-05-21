/////////////////////////////////////////////////////////////////////////////
// Name:        ImgFrame.cpp
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

#include "cc_util.h"

ccXml::ccXml()
{
	m_flag = _CCXML_TRIMSPACE_ | _CCXML_TRIMTAB_ | _CCXML_TRIMCRLN_;
}

ccXml::~ccXml()
{
}

bool ccXml::IsTrim()
{
	return CC_BIT_HAS(m_flag, _CCXML_TRIMSPACE_);
}

void ccXml::SetTrim(bool value)
{
	CC_BIT_SET(m_flag, _CCXML_TRIMSPACE_, value);
}

bool ccXml::IsTrimTab()
{
	return CC_BIT_HAS(m_flag, _CCXML_TRIMTAB_);
}

void ccXml::SetTrimTab(bool value)
{
	CC_BIT_SET(m_flag, _CCXML_TRIMTAB_, value);
}

bool ccXml::IsTrimCrln()
{
	return CC_BIT_HAS(m_flag, _CCXML_TRIMCRLN_);
}

void ccXml::SetTrimCrln(bool value)
{
	CC_BIT_SET(m_flag, _CCXML_TRIMCRLN_, value);
}

long ccXml::GetSize(const wxString& sXmlPath)
{
	//wxStringToNumHashMap::const_iterator it = m_hasHeader.find(sXmlPath);
    //if ( it != m_hasHeader.end() )
	//{
	//	return it->second;
	//}
	if(m_hasHeader.find(sXmlPath) != m_hasHeader.end())
	{
		return m_hasHeader[sXmlPath];
	}
	wxString sXmlPath2 = sXmlPath;
	sXmlPath2.Replace(wxT("[0]/"), wxT("/"));
	if(m_hasHeader.find(sXmlPath2) != m_hasHeader.end())
	{
		return m_hasHeader[sXmlPath2];
	}
	return -1;
}

wxString ccXml::GetValue(const wxString& sXmlPath)
{
	if(m_hasXml.find(sXmlPath) != m_hasXml.end())
	{
		return m_hasXml[sXmlPath];
	}
	wxString sXmlPath2 = sXmlPath;
	sXmlPath2.Replace(wxT("[0]/"), wxT("/"));
	if(m_hasXml.find(sXmlPath2) != m_hasXml.end())
	{
		return m_hasXml[sXmlPath2];
	}
	return wxT("");
}

int ccXml::GetValue(const wxString& sXmlPath, int notFound)
{
	if(m_hasXml.find(sXmlPath) != m_hasXml.end())
	{
		return wxAtoi(m_hasXml[sXmlPath]);
	}
	wxString sXmlPath2 = sXmlPath;
	sXmlPath2.Replace(wxT("[0]/"), wxT("/"));
	if(m_hasXml.find(sXmlPath2) != m_hasXml.end())
	{
		return wxAtoi(m_hasXml[sXmlPath2]);
	}
	return notFound;
}

wxString ccXml::GetAttribute(const wxString& sXmlPath, const wxString& sName)
{
	wxString path = sXmlPath + wxT("[:]") + sName;
	if(m_hasXml.find(path) != m_hasXml.end())
	{
		return m_hasXml[path];
	}
	path.Replace(wxT("[0]/"), wxT("/"));
	if(m_hasXml.find(path) != m_hasXml.end())
	{
		return m_hasXml[path];
	}
	return wxT("");
}

bool ccXml::LoadAttribute(wxXmlNode *child, wxString& sParent)
{
	wxXmlAttribute* attr = child->GetAttributes();
	while(attr != NULL)
	{
		wxLogDebug("parent:%s, name:%s, value:%s", sParent, attr->GetName(), attr->GetValue());
		m_hasXml[sParent + wxT("[:]") + attr->GetName()] = attr->GetValue();
		attr = attr->GetNext();
	}
	return true;
}

//sample:  /aaaa/bbbb[0]/ccc[1]/ddd
bool ccXml::LoadSub(wxXmlNode *child, wxString& sParent)
{
	long nCnt = 1;
	wxString sCurrent = sParent + wxT("/") + child->GetName();
	if(m_hasHeader.find(sCurrent) != m_hasHeader.end())
	{
		nCnt = m_hasHeader[sCurrent] + 1;
	}
	m_hasHeader[sCurrent] = nCnt;
	if(nCnt > 1)
	{
		sCurrent = wxString::Format(wxT("%s/%s[%d]"), sParent, child->GetName(), nCnt);
	}

	LoadAttribute(child, sCurrent);
	if(child->GetChildren())
	{
		wxXmlNode *childSub = child->GetChildren();
		while (childSub)
		{
			if(childSub->GetType() == wxXML_TEXT_NODE)
			{
				wxString sCont = childSub->GetContent();
				if(IsTrim() || IsTrimTab() || IsTrimCrln())
				{
					ccStr::Trim(sCont, IsTrim(), IsTrimTab(), IsTrimCrln(), true, true);
				}
				m_hasXml[sCurrent] = sCont;
				LoadAttribute(childSub, sCurrent);
			}
			else if(childSub->GetType() != wxXML_COMMENT_NODE)
			{
				LoadSub(childSub, sCurrent);
			}
			childSub = childSub->GetNext();
		}
	}
	else
	{
		wxString sCont = child->GetContent();
		if(IsTrim() || IsTrimTab() || IsTrimCrln())
		{
			ccStr::Trim(sCont, IsTrim(), IsTrimTab(), IsTrimCrln(), true, true);
		}
		m_hasXml[sCurrent] = sCont;
	}
	return true;
}

//topl, top, topr, left, right, bottoml, bottom, bottomr
bool ccXml::Load(const wxString& sXmlPath)
{
	if(!wxFileExists(sXmlPath))
	{
		return false;
	}

	wxXmlDocument doc;
	if(!doc.Load(sXmlPath))
	{
		return false;
	}

	m_rootName = doc.GetRoot()->GetName();
	wxString sParent = wxT("");
	return LoadSub(doc.GetRoot(), sParent);
}

wxString ccXml::Dump()
{
	wxString sRet;
	wxStringToStringHashMap::const_iterator it;
	for( it = m_hasXml.begin(); it != m_hasXml.end(); ++it )
	{
		wxString key = it->first;
		wxString value = it->second;
		// do something useful with key and value
		sRet += key;
		sRet += wxT("=");
		sRet += value;
		sRet += wxT("\r\n");
	}

    //wxFileOutputStream stream(wxT(".\\skins_out.xml"));
    //if (stream.IsOk())
	//{
    //    stream.Write(sRet.mb_str(), sRet.length());
	//}
	//stream.Close();
	return sRet;
}

void ccXml::Clear()
{
	m_hasXml.clear();
	m_hasHeader.clear();
	m_rootName.Empty();
}

ccUtil::ccUtil()
{
}

ccUtil::~ccUtil()
{
}

bool ccUtil::dFiles(const wxString& sPath, wxArrayString& result, const wxString fileSpec, UINT flag)
{
	wxDir dir(sPath);
	if (!dir.IsOpened())
	{
		return false;
	}
	wxString filename;
	bool cont = dir.GetFirst(&filename, fileSpec, flag);
	while (cont)
	{
		result.Add(filename);
		cont = dir.GetNext(&filename);
	}
	return true;
}

bool ccUtil::fRead(const wxString& sPath, wxString& data, const wxMBConv& conv)
{
	bool ret = false;
	if(!wxFileExists(sPath))
	{
		return false;
	}
	//wxFFile file(sPath, wxT("rb"));
	wxFile file(sPath, wxFile::read);
	if ( file.IsOpened() )
	{
		ssize_t length = file.Length();
		wxCHECK_MSG( (wxFileOffset)length == file.Length(), false, wxT("huge file not supported") );

		wxCharBuffer buf(length);
		char* p = buf.data();
		for ( ;; )
		{
			static const ssize_t READSIZE = 4096;

			ssize_t nread = file.Read(p, length > READSIZE ? READSIZE : length);
			if ( nread == wxInvalidOffset )
			{
				break;
			}

			p += nread;
			if ( length <= nread )
			{
				ret = true;
				break;
			}

			length -= nread;
		}

		*p = 0;
		p = buf.data();
		if(length >= nBOMLen_UTF8 && !memcmp(p, szBOM_UTF8, nBOMLen_UTF8))
		{
			p += nBOMLen_UTF8;
			data = wxString(p, wxConvUTF8);
		}
		else if(length >= nBOMLen_UTF32BE && !memcmp(p, szBOM_UTF32BE, nBOMLen_UTF32BE))
		{
			p += nBOMLen_UTF32BE;
			data = wxString(p, wxMBConvUTF32BE());
		}
		else if(length >= nBOMLen_UTF32LE && !memcmp(p, szBOM_UTF32LE, nBOMLen_UTF32LE))
		{
			p += nBOMLen_UTF32LE;
			data = wxString(p, wxMBConvUTF32LE());
		}
		else if(length >= nBOMLen_UTF16BE && !memcmp(p, szBOM_UTF16BE, nBOMLen_UTF16BE))
		{
			p += nBOMLen_UTF16BE;
			data = wxString(p, wxMBConvUTF16BE());
		}
		else if(length >= nBOMLen_UTF16LE && !memcmp(p, szBOM_UTF16LE, nBOMLen_UTF16LE))
		{
			p += nBOMLen_UTF16LE;
			data = wxString(p, wxMBConvUTF16LE());
		}
		else if(length >= nBOMLen_UTF7 && !memcmp(p, szBOM_UTF7, nBOMLen_UTF7))
		{
			p += nBOMLen_UTF7;
			data = wxString(p, wxConvUTF7);
		}
		//wxString strTmp(p, conv);
		//data.swap(strTmp);
		data = wxString(p, conv);
	}
	file.Close();
	return ret;
}

bool ccUtil::fWrite(const wxString& sPath, const wxString& data, const wxMBConv& conv, bool isAppend, wxBOM bom)
{
	bool ret = false;
	//wxFFile file(sPath, wxT("w"));
	wxFile file(sPath, isAppend ? wxFile::write_append : wxFile::write);
	if ( file.IsOpened() )
	{
		if(bom == wxBOM_UTF8)
		{
			file.Write(szBOM_UTF8, nBOMLen_UTF8);
			ret = file.Write(data, wxConvUTF8);
		}
		else if(bom == wxBOM_UTF32BE)
		{
			file.Write(szBOM_UTF32BE, nBOMLen_UTF32BE);
			ret = file.Write(data, wxMBConvUTF32BE());
		}
		else if(bom == wxBOM_UTF32LE)
		{
			file.Write(szBOM_UTF32LE, nBOMLen_UTF32LE);
			ret = file.Write(data, wxMBConvUTF32LE());
		}
		else if(bom == wxBOM_UTF16BE)
		{
			file.Write(szBOM_UTF16BE, nBOMLen_UTF16BE);
			ret = file.Write(data, wxMBConvUTF16BE());
		}
		else if(bom == wxBOM_UTF16LE)
		{
			file.Write(szBOM_UTF16LE, nBOMLen_UTF16LE);
			ret = file.Write(data, wxMBConvUTF16LE());
		}
		else
		{
			ret = file.Write(data, conv);
		}
	}
	file.Close();
	return ret;
}

wxString ccUtil::GetComparePath(const wxString& sAbsolute, const wxString& ReleatedPath)
{
	if(ReleatedPath.StartsWith(wxT("/")) || ReleatedPath.StartsWith(wxT("\\")) || ReleatedPath.substr(1, 2) == wxT(":\\"))
	{
		return ReleatedPath;
	}

	wxString sRet = sAbsolute;
	if(!sRet.EndsWith(wxT("/")) && !sRet.EndsWith(wxT("\\")))
	{
		sRet.Append(wxFILE_SEP_PATH);
	}

	return sRet + ReleatedPath;
}

wxString ccUtil::GetAbsoluteFullPath(const wxString& sFilePath)
{
	wxFileName file;
	file.Assign(sFilePath);
	file.MakeAbsolute();
	return file.GetFullPath();
}

wxString ccUtil::GetAbsolutePathOnly(const wxString& sFilePath)
{
	wxFileName file;
	file.Assign(sFilePath);
	file.MakeAbsolute();
	return file.GetPath();
}

wxString ccUtil::GetStartupReleatedPath(const wxString& ReleatedPath)
{
	if(ReleatedPath.StartsWith(wxT("/")) || ReleatedPath.StartsWith(wxT("\\")) || ReleatedPath.substr(1, 2) == wxT(":\\"))
	{
		return ReleatedPath;
	}

	wxString sRet = ccUtil::GetStartupPath(false, false);
#ifdef _DEBUG
	if(ccUtil::dExist(sRet + ReleatedPath))
	{
		return sRet + ReleatedPath;
	}
#endif

	sRet.append(wxT(".."));
	sRet.append(wxFILE_SEP_PATH);
	sRet.append(ReleatedPath);
	if(!sRet.EndsWith(wxT("/")) && !sRet.EndsWith(wxT("\\")))
	{
		sRet.Append(wxFILE_SEP_PATH);
	}
	return sRet;
}

wxString ccUtil::GetStartupPath(bool withFilename, bool withFileext)
{
	//wxFileName exeFile(stdp.GetExecutablePath());
	//strAppPath = exeFile.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
	wxString path = wxStandardPaths::Get().GetExecutablePath();;
	if(withFilename)
	{
		if(withFileext)
		{
			return path;
		}
		int nInd = path.rfind(wxT("."));
		if(nInd == wxNOT_FOUND)
		{
			return path;
		}
		return path.substr(0, nInd);
	}
	path = GetAbsolutePathOnly(path);
	if(!path.EndsWith(wxT("/")) && !path.EndsWith(wxT("\\")))
	{
		path.Append(wxFILE_SEP_PATH);
	}
	return path;
}

wxRect ccUtil::RectFromString(const wxString& sRect)
{
	wxString sRect2 = sRect;
	wxString left = sRect2.BeforeFirst(',');
	sRect2 = sRect2.AfterFirst(',');

	wxString top = sRect2.BeforeFirst(',');
	sRect2 = sRect2.AfterFirst(',');

	wxString right = sRect2.BeforeFirst(',');
	sRect2 = sRect2.AfterFirst(',');

	wxString bottom = sRect2.BeforeFirst(',');
	sRect2 = sRect2.AfterFirst(',');

	wxRect rect(wxAtoi(left), wxAtoi(top), wxAtoi(right), wxAtoi(bottom));
	return rect;
}

int ccUtil::GetRandom(int min, int max, bool init_srand)
{
	//Intializes random number generator
	static int srand_flg = 0;
	if(srand_flg == 0 || init_srand)
	{
		srand_flg = 1;
		srand((unsigned int)wxGetLocalTimeMillis().GetLo());
	}

	int n = ((rand() % (int)(((max) + 1) - (min))) + (min));
	return n;
}

wxString ccUtil::GetRandomString(const wxString& sChars, int nMinLen, int nMaxLen)
{
	wxString sChars2 = sChars;
	if((int)sChars2.length() < 1)
	{
		sChars2 = wxT("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz");
	}
	//// initialize random numbers generator
	int nMaxLenReal = ccUtil::GetRandom(nMinLen, nMaxLen, true);
	if(nMaxLenReal <= nMinLen && nMaxLenReal < nMaxLen)
	{
		nMaxLenReal++;
	}
	wxString strPass;
	int nPosLast = -1;
	for(int i = 0; i < nMaxLenReal; i++)
	{
		int nPos = ccUtil::GetRandom(0, sChars2.length() - 1);
		if(nPos == nPosLast)
		{
			nPos = ccUtil::GetRandom(0, sChars2.length() - 1);
		}
		nPosLast = nPos;
		strPass.Append(sChars2.at(nPos));
	}
	return strPass;
}

wxString ccUtil::GetSysLanguage(int langDefault)
{
	int nLang = wxLocale::GetSystemLanguage();
	if(nLang == wxLANGUAGE_UNKNOWN)
	{
		nLang = langDefault;
	}
	wxLocale loc;
	loc.Init(nLang, 0);
	const wxLanguageInfo *info = wxLocale::GetLanguageInfo(nLang);
	if(info != NULL)
	{
		return info->CanonicalName;
	}
	return wxT("en_US");
}


void ccUtil::test()
{
#ifdef _DEBUG
	wxString lang = ccUtil::GetSysLanguage();
	wxLogDebug(wxT("GetSysLanguage:[%s]"), lang);
#endif
}
/*
bool LoadDataFromResource(char*& t_data, DWORD& t_dataSize, const wxString& t_name)
{
	bool     r_result    = false;
	HGLOBAL  a_resHandle = 0;
	HRSRC    a_resource;

	a_resource = FindResource(0, t_name.wchar_str(), RT_RCDATA);

	if(0 != a_resource)
	{
		a_resHandle = LoadResource(NULL, a_resource);
		if (0 != a_resHandle)
		{
			t_data = (char*)LockResource(a_resHandle);
			t_dataSize = SizeofResource(NULL, a_resource);
			r_result = true;
		}
	}

	return r_result;
}
*/
