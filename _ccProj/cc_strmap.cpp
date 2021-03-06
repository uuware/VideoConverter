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

#include "cc_strmap.h"
#include "cc_util.h"

ccStr::ccStr()
{
}

ccStr::~ccStr()
{
}

int ccStr::indexOf(const wxString& str, const wxString& s, bool isIgnorecase, size_t nStart)
{
    size_t len1 = str.length();
    size_t len2 = s.length();
	if(len1 - nStart < len2)
	{
		return -1;
	}
	if(len2 == 0)
	{
		return nStart;
	}
	if(!isIgnorecase)
	{
		return str.find(s, nStart);
	}

	const_iterator i1 = str.begin() + nStart;
    const_iterator end1 = str.end();
    const_iterator i2 = s.begin();
    const_iterator end2 = s.end();

	for ( ; i1 <= end1 - len2; ++i1 )
    {
		const_iterator i1_loop = i1;
		const_iterator i2_loop = i2;
		for ( ; i1_loop != end1 && i2_loop != end2; ++i1_loop, ++i2_loop )
		{
			if ( wxTolower(*i1_loop) != wxTolower(*i2_loop) )
			{
				break;
			}
		}
		if(i2_loop == end2)
		{
			return nStart;
		}
		nStart++;
	}
	return -1;
}

int ccStr::lastIndexOf(const wxString& str, const wxString& s, bool isIgnorecase, size_t nLastStart)
{
    size_t len1 = str.length();
    size_t len2 = s.length();
	if(nLastStart == 0)
	{
		nLastStart = len1;
	}
	if(nLastStart > len1 || nLastStart < len2)
	{
		return -1;
	}
	if(len2 == 0)
	{
		return nLastStart;
	}
	if(!isIgnorecase)
	{
		return str.rfind(s, nLastStart);
	}

	const_iterator i1 = str.begin() + nLastStart - len2;
    const_iterator end1 = str.begin() + nLastStart;
    const_iterator i2 = s.begin();
    const_iterator end2 = s.end();

	nLastStart -= len2;
	for ( ; i1 > str.begin(); --i1 )
    {
		const_iterator i1_loop = i1;
		const_iterator i2_loop = i2;
		for ( ; i1_loop != end1 && i2_loop != end2; ++i1_loop, ++i2_loop )
		{
			if ( wxTolower(*i1_loop) != wxTolower(*i2_loop) )
			{
				break;
			}
		}
		if(i2_loop == end2)
		{
			return nLastStart;
		}
		nLastStart--;
	}
	return -1;
}

bool ccStr::StartsWith(const wxString& sSrc, const wxString& prefix, bool isIgnorecase)
{
	if(sSrc.length() < prefix.length())
	{
		return false;
	}
	if(!isIgnorecase)
	{
		return sSrc.StartsWith(prefix);
	}
	return (sSrc.Mid(0, prefix.length()).CmpNoCase(prefix) == 0);
}
bool ccStr::EndsWith(const wxString& sSrc, const wxString& suffix, bool isIgnorecase)
{
	if(sSrc.length() < suffix.length())
	{
		return false;
	}
	if(!isIgnorecase)
	{
		return sSrc.EndsWith(suffix);
	}
    int start = sSrc.length() - suffix.length();
	return (sSrc.Mid(start).CmpNoCase(suffix) == 0);
}

bool ccStr::Splite(const wxString& str, const wxString& sep, wxString& left, wxString& right)
{
	int pos = str.find(sep);
	if(pos < 0)
	{
		left = str;
		return false;
	}

	left = str.Left(pos);
	right = str.Mid(pos + 1);
	return true;
}

int ccStr::Splite(const wxString& str, const wxString& sep, wxArrayString& result)
{
	int pos = str.find(sep);
	while(pos >= 0)
	{
		pos += sep.length();
		int pos2 = str.find(sep, pos);
		if(pos2 > pos)
		{
			result.Add(str.substr(pos, pos2 - pos));
		}
		else
		{
			result.Add(str.substr(pos));
			break;
		}

		pos = pos2;
	}
	return result.size();
}

inline int wxSafeIsspace(wxChar ch) { return (ch < 127) && wxIsspace(ch); }
inline int wxSafeIstab(wxChar ch) { return (ch < 127) && (ch == 0x9); }
inline int wxSafeIscrln(wxChar ch) { return (ch < 127) && (ch == 0x10 || ch == 0x13); }
// trims spaces (in the sense of isspace) from left or right side
wxString& ccStr::Trim(wxString& sSrc, bool bTrimSpace, bool bTrimTab, bool bTrimCrln, bool bFromLeft, bool bFromRight)
{
    // first check if we're going to modify the string at all
    if ( !sSrc.empty() )
    {
        if ( bFromRight )
        {
            // find last non-space character
            reverse_iterator psz = sSrc.rbegin();
            while ( (psz != sSrc.rend()) && (
				(bTrimSpace && wxSafeIsspace(*psz)) ||
				(bTrimTab && wxSafeIstab(*psz)) ||
				(bTrimCrln && wxSafeIscrln(*psz))
				) )
                ++psz;

			if(psz != sSrc.rbegin())
			{
				// truncate at trailing space start
				sSrc.erase(psz.base(), sSrc.end());
			}
        }
        if ( bFromLeft )
        {
            // find first non-space character
            iterator psz = sSrc.begin();
            while ( (psz != sSrc.end()) && (
				(bTrimSpace && wxSafeIsspace(*psz)) ||
				(bTrimTab && wxSafeIstab(*psz)) ||
				(bTrimCrln && wxSafeIscrln(*psz))
				) )
                ++psz;

			if(psz != sSrc.begin())
			{
				// fix up data and length
				sSrc.erase(sSrc.begin(), psz);
			}
        }
    }

    return sSrc;
}

void ccStr::test()
{
#ifdef _DEBUG
	wxString s1 = wxT("			  asd\tfa fdasdfaf\t\n\t   \t\n");
	ccStr::Trim(s1, true, true, true, true, true);
	wxLogDebug(wxT("Trim:[%s]"), s1);

	wxString s2 = wxT("abcCdeGeh");
	bool b1 = ccStr::StartsWith(s2, wxT("AbCcd"), true);
	bool b2 = ccStr::StartsWith(s2, wxT("AbCcd"), false);
	bool b3 = ccStr::EndsWith(s2, wxT("EgeH"), true);
	bool b4 = ccStr::EndsWith(s2, wxT("EgeH"), false);
	wxLogDebug(wxT("StartsWith:%d, %d; EndsWith:%d, %d;"), b1, b2, b3, b4);
#endif
}

//===========================================================ccStrMap
///////////////////////////////////////////////////////////////////////////////
// constructors/destructor
ccStrMap::ccStrMap(bool isSort, bool isReturnKeyIfNoFound, bool isIgnorecase) : m_KeySort(isSort)
{
	_Flags = 0;
	CC_BIT_SET(_Flags, CC_FLAG_KEYARR_RETURNKEY, isReturnKeyIfNoFound);
	CC_BIT_SET(_Flags, CC_FLAG_KEYARR_IGNORECASE, isIgnorecase);
	CC_BIT_SET(_Flags, CC_FLAG_KEYARR_SORT, isSort);
}

ccStrMap::~ccStrMap()
{
}

void ccStrMap::Clear()
{
	if(m_KeyArr.size() > 0)
	{
		CC_BIT_ON(_Flags, CC_FLAG_KEYARR_DIRTY);
	}
	m_KeyArr.clear();
	m_KeySort.clear();
}

wxStringToStringHashMap::const_iterator ccStrMap::find(const wxString& key) const
{
	if(CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_IGNORECASE))
	{
		return m_KeyArr.find(key.Upper());
	}
	return m_KeyArr.find(key);
}

//clear one part
void ccStrMap::Clear(const wxString& keyStartWith)
{
	if(m_KeyArr.size() < 1)
	{
		return;
	}

	for(size_t j = m_KeySort.Count(); j >= 0; j--)
	{
		wxString key = m_KeySort.Item(j);
		if(ccStr::StartsWith(key, keyStartWith, CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_IGNORECASE)))
		{
			CC_BIT_ON(_Flags, CC_FLAG_KEYARR_DIRTY);
			m_KeyArr.erase(GetIsIgnoreCase() ? key.Upper() : key);
			m_KeySort.RemoveAt(j);
		}
	}
}

//clear one
bool ccStrMap::Remove(const wxString& key)
{
	//wxStringToStringHashMap::const_iterator it = find(key);
	//m_KeyArr.erase cannot be const_iterator
	wxStringToStringHashMap::iterator it = 
		CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_IGNORECASE) ? m_KeyArr.find(key.Upper()) : m_KeyArr.find(key);
	if(it != m_KeyArr.end())
	{
		CC_BIT_ON(_Flags, CC_FLAG_KEYARR_DIRTY);
		m_KeyArr.erase(it);
		m_KeySort.Remove(key);
		return true;
	}
	return false;
}

int ccStrMap::LoadFromString(const wxString& sTxt)
{
	Clear();
	wxString head;
	size_t nLen = sTxt.length();
	size_t pos1 = 0;
	size_t pos2 = 0;
	int nCnt = 0;
	for ( size_t n = 0; n < nLen; n++ )
	{
		const wxChar c = sTxt[n];
		if ( c == '\0' || c == '\n' || c == '\r' )
		{
			if(pos1 != n)
			{
				if(sTxt[pos1] == '#' || sTxt[pos1] == ';')
				{
				}
				else if(sTxt[pos1] == '[' && sTxt[n - 1] == ']')
				{
					head = sTxt.substr(pos1 + 1, n - pos1 - 2);
					head.append(wxT("."));
				}
				else
				{
					if(pos2 <= pos1)
					{
						pos2 = n + 1;
					}
					wxString key = sTxt.substr(pos1, pos2 - pos1 - 1);
					if(!head.IsEmpty())
					{
						key.insert(0, head);
					}
					wxString value = pos2 >= n ? wxT("") : sTxt.substr(pos2, n - pos2);
					//if(pos2 >= n)
					//{
					//	key.append(wxT("="));
					//}
					value.Replace(wxT("\\r"), wxT("\r"));
					value.Replace(wxT("\\n"), wxT("\n"));
					value.Replace(wxT("\\\\"), wxT("\\"));
					//wxLogDebug(wxT("pos:[%d, %d], k:%s, v:%s"), pos1, pos2, key, value);
					Set(key, value);
					nCnt++;
				}
			}
			pos1 = n + 1;
			pos2 = 0;
		}
		else if ( c == '=' )
		{
			if(pos2 == 0)
			{
				pos2 = n + 1;
			}
		}
	}

	CC_BIT_OFF(_Flags, CC_FLAG_KEYARR_DIRTY);
	return nCnt;
}

int ccStrMap::Load(const wxString& filepath, const wxMBConv& conv)
{
	wxString sTxt;
	bool ret = ccUtil::fRead(filepath, sTxt, conv);
	//wxFile file(filepath, wxFile::read);
	//if(file.IsOpened())
	//{
	//	file.ReadAll(&sTxt, conv);
	//	file.Close();
	if(ret)
	{
		return LoadFromString(sTxt);
	}
	return 0;
}

void ccStrMap::SaveToString(wxString& sTxt) const
{
	wxString head;
	size_t m_nCount = m_KeySort.Count();
	for(size_t j = 0; j < m_nCount; j++)
	{
		wxString key = m_KeySort.Item(j);
		wxString value = this->Get(key);
		if(!head.IsEmpty() && ccStr::StartsWith(key, head, true))
		{
			key = key.substr(head.length() + 1);
		}
		else
		{
			int nInd = key.find(wxT("."));
			if(nInd != wxNOT_FOUND)
			{
				head = key.substr(0, nInd);
				key = key.substr(nInd + 1);
				sTxt.append(wxT("[")).append(head).append(wxT("]\r\n"));
			}
		}

		//if(!key.EndsWith(wxT("=")))
		//{
			sTxt.append(key);
			value.Replace(wxT("\\"), wxT("\\\\"));
			value.Replace(wxT("\n"), wxT("\\n"));
			value.Replace(wxT("\r"), wxT("\\r"));
			sTxt.append(wxT("=")).append(value);
		//}
		//else
		//{
		//	sTxt.append(key.substr(0, key.length() - 1));
		//}
		sTxt.append(wxT("\r\n"));
	}
}

bool ccStrMap::Save(const wxString& filepath, const wxMBConv& conv, wxBOM bom) const
{
	wxFile file(filepath, wxFile::write);
	if(file.IsOpened())
	{
		wxString sTxt;
		SaveToString(sTxt);
		//file.Write(sTxt, conv);
		//file.Close();
		bool ret = ccUtil::fWrite(filepath, sTxt, conv, false, bom);
		return ret;
	}
	return false;
}

void ccStrMap::SetPath(const wxString& filepath)
{
	m_LastPath = filepath;
}

wxString ccStrMap::GetPath() const
{
	return m_LastPath;
}

int ccStrMap::Add(const ccStrMap& keyArr, const wxString& keyStartWith, const wxString& preKeyTurnTo)
{
	int nCnt = 0;
	for(size_t j = keyArr.m_KeySort.Count(); j >= 0; j--)
	{
		wxString key = keyArr.m_KeySort.Item(j);
		if(keyStartWith.IsEmpty() || ccStr::StartsWith(key, keyStartWith, CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_IGNORECASE)))
		{
			CC_BIT_ON(_Flags, CC_FLAG_KEYARR_DIRTY);

			wxString value = keyArr.Get(key);
			if(!preKeyTurnTo.IsEmpty())
			{
				key.insert(0, preKeyTurnTo);
			}
			Set(key, value);
			nCnt++;
		}
	}
	return nCnt;
}

int ccStrMap::Add(const wxStringToStringHashMap& keyArr, const wxString& keyStartWith, const wxString& preKeyTurnTo)
{
	int nCnt = 0;
	wxStringToStringHashMap::const_iterator it;
    for( it = keyArr.begin(); it != keyArr.end(); ++it )
    {
        wxString key = it->first;
		if(keyStartWith.IsEmpty() || ccStr::StartsWith(key, keyStartWith, CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_IGNORECASE)))
		{
			CC_BIT_ON(_Flags, CC_FLAG_KEYARR_DIRTY);

			wxString value = it->second;
			if(!preKeyTurnTo.IsEmpty())
			{
				key.insert(0, preKeyTurnTo);
			}
			Set(key, value);
			nCnt++;
		}
    }
	return nCnt;
}

const wxStringToStringHashMap* ccStrMap::GetMap()
{
	return &m_KeyArr;
}

bool ccStrMap::Exist(const wxString& key) const
{
	wxStringToStringHashMap::const_iterator it = find(key);
	if(it != m_KeyArr.end())
	{
		return true;
	}
	return false;
}

bool ccStrMap::Exist(const wxString& key, wxString& getvalue) const
{
	wxStringToStringHashMap::const_iterator it = find(key);
	if(it != m_KeyArr.end())
	{
		getvalue = it->second;
		return true;
	}
	return false;
}

wxString ccStrMap::Get(const wxString& key) const
{
	wxStringToStringHashMap::const_iterator it = find(key);
	if(it != m_KeyArr.end())
	{
		return it->second;
	}

	if(CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_LOGNOTFOUNT))
	{
		wxLogDebug(wxT("not found key:%s"), key);
	}
	if(CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_RETURNKEY))
	{
		wxString ret = key;
		ret.append(wxT("[NOFOUND]"));
		return ret;
	}
	return wxEmptyString;
}

wxString ccStrMap::Get(const wxString& key, const wxString& defaultValue) const
{
	wxStringToStringHashMap::const_iterator it = find(key);
	if(it != m_KeyArr.end())
	{
		return it->second;
	}
	if(CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_LOGNOTFOUNT))
	{
		wxLogDebug(wxT("not found key:%s"), key);
	}
	return defaultValue;
}

long ccStrMap::Get(const wxString& key, long nValue) const
{
	wxStringToStringHashMap::const_iterator it = find(key);
	if(it != m_KeyArr.end())
	{
		it->second.ToLong(&nValue);
		return nValue;
	}
	if(CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_LOGNOTFOUNT))
	{
		wxLogDebug(wxT("not found key:%s"), key);
	}
	return nValue;
}

bool ccStrMap::GetBool(const wxString& key, bool bValue) const
{
	wxStringToStringHashMap::const_iterator it = find(key);
	if(it != m_KeyArr.end())
	{
		long nValue = 0;
		it->second.ToLong(&nValue);
		return (nValue == 1);
	}

	if(CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_LOGNOTFOUNT))
	{
		wxLogDebug(wxT("not found key:%s"), key);
	}
	return bValue;
}

void ccStrMap::Set(const wxString& key, const wxString& value)
{
	m_KeyArr[GetIsIgnoreCase() ? key.Upper() : key] = value;
	if(m_KeySort.Index(key, false) == wxNOT_FOUND)
	{
		m_KeySort.Add(key);
	}
}

void ccStrMap::Set(const wxString& key, long dwValue)
{
	wxString value = wxString::Format(wxT("%ld"), dwValue);
	Set(key, value);
}

void ccStrMap::SetBool(const wxString& key, bool bValue)
{
	wxString value = bValue ? wxString(wxT("1")) : wxString(wxT("0"));
	Set(key, value);
}

wxString ccStrMap::GetFormat(const wxString& key, ...) const
{
	wxStringToStringHashMap::const_iterator it = find(key);
	if(it != m_KeyArr.end())
	{
		wxString value = it->second;
		va_list argList;
		va_start(argList, key);
		wxString ret = wxString::FormatV(value, argList);
		va_end(argList);
		return ret;
	}

	if(CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_LOGNOTFOUNT))
	{
		wxLogDebug(wxT("not found key:%s"), key);
	}
	if(CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_RETURNKEY))
	{
		wxString ret = key;
		ret.append(wxT("[NOFOUND]"));
		return ret;
	}
	return wxEmptyString;
}

wxString ccStrMap::GetFormatDefaultValue(const wxString& defaultValue, const wxString& key, ...) const
{
	wxStringToStringHashMap::const_iterator it = find(key);
	if(it != m_KeyArr.end())
	{
		wxString value = it->second;
		va_list argList;
		va_start(argList, key);
		wxString ret = wxString::FormatV(value, argList);
		va_end(argList);
		return ret;
	}

	if(CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_LOGNOTFOUNT))
	{
		wxLogDebug(wxT("not found key:%s"), key);
	}
	return defaultValue;
}

void ccStrMap::SetIsDirty(bool isDirty)
{
	//_bIsDirty = isDirty;
	CC_BIT_SET(_Flags, CC_FLAG_KEYARR_DIRTY, isDirty);
}

bool ccStrMap::GetIsDirty() const
{
	return CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_DIRTY);
}

bool ccStrMap::GetIsEmpty() const
{
	return (m_KeyArr.size() < 1);
}

//return cc::Str(key).Append(_T("[NOFOUND]")); if not found
void ccStrMap::SetReturnNoFound(bool isReturnKey)
{
	CC_BIT_SET(_Flags, CC_FLAG_KEYARR_RETURNKEY, isReturnKey);
}

bool ccStrMap::GetIsSort() const
{
	return CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_SORT);
}

bool ccStrMap::GetIsIgnoreCase() const
{
	return CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_IGNORECASE);
}

//void ccStrMap::SetIsIgnoreCase(bool isIgnoreCase)
//{
//	CC_BIT_SET(_Flags, CC_FLAG_KEYARR_IGNORECASE, isIgnoreCase);
//}

void ccStrMap::SetIsLogNotFound(bool isLogNotFound)
{
	CC_BIT_SET(_Flags, CC_FLAG_KEYARR_LOGNOTFOUNT, isLogNotFound);
}

bool ccStrMap::GetIsLogNotFound() const
{
	return CC_BIT_HAS(_Flags, CC_FLAG_KEYARR_LOGNOTFOUNT);
}

int ccStrMap::GetSize() const
{
	return m_KeyArr.size();
}

void ccStrMap::Dump(const TCHAR* info) const
{
#ifdef _DEBUG
	wxString sTxt;
	SaveToString(sTxt);
	wxLogDebug(wxT("%s[ccStrMap::Dump(), Size:%d, Dump:%s]"), info, m_KeyArr.size(), sTxt);
#endif
}

void ccStrMap::test()
{
#ifdef _DEBUG
	wxLogDebug(wxT("ccStrMap::test()"));
	ccStrMap map(true);
	map.Set(wxT("a55"), wxT("v5"));
	map.Set(wxT("z21"), wxT("v2"));
	map.Set(wxT("a31"), wxT("va3"));
	map.Set(wxT("a21"), wxT("va2"));
	map.Set(wxT("z51"), wxT("vz5"));
	map.Set(wxT("z31"), wxT("vz3"));
	map.Set(wxT("a11"), wxT("va1"));
	//map.Set(wxT("Main.XXX"), wxT("en-日本語US"));
	//map.Set(wxT("Main.XXX2"), wxT("en-中国语US"));
	//map.Save(m_tcFile, wxConvUTF8);
	//map.SetIsDirty(false);
	//map.Save(wxT("c:\\temp\\aaa.ini"));
	//map.Dump();

	//ccStrMap map2(false);
	//map2.Load(wxT("Lang\\ja-JP.ini"));
	//map2.Dump();

	wxString s1 = wxT("			  asd\tfa fdasdfaf\t\n\t   \t\n");
	ccStr::Trim(s1, true, true, true, true, true);
	wxLogDebug(wxT("Trim:[%s]"), s1);
    wxASSERT_MSG( s1==wxT("asd\tfa fdasdfaf"), wxT("invalid Trim") );

	wxString ss00 = wxT("abcdefg abcdefg");
	wxString ss01 = wxT("DeF");
	int nn01 = ccStr::indexOf(ss00, ss01, true, 1);
    wxASSERT_MSG( nn01==3, wxT("invalid indexOf") );

	int nn01_2 = ccStr::indexOf(ss00, ss01, true, 7);
    wxASSERT_MSG( nn01_2==11, wxT("invalid indexOf") );

	wxString ss02 = wxT("AbC");
	int nn02 = ccStr::indexOf(ss00, ss02, true, 0);
    wxASSERT_MSG( nn02==0, wxT("invalid indexOf") );

	wxString ss03 = wxT("AbC");
	int nn03 = ccStr::indexOf(ss00, ss03, true, 11);
    wxASSERT_MSG( nn03==-1, wxT("invalid indexOf") );

	wxString ss04 = wxT("EfGh");
	int nn04 = ccStr::indexOf(ss00, ss04, true, 0);
    wxASSERT_MSG( nn04==-1, wxT("invalid indexOf") );


	wxString ss10 = wxT("abcdefg abcdefg");
	wxString ss11 = wxT("DeF");
	int mm11 = ccStr::lastIndexOf(ss10, ss11, true, 0);
    wxASSERT_MSG( mm11==11, wxT("invalid lastIndexOf") );
	int mm11_2 = ccStr::lastIndexOf(ss10, ss11, true, 7);
    wxASSERT_MSG( mm11_2==3, wxT("invalid lastIndexOf") );

	wxString ss12 = wxT("EfG");
	int mm12 = ccStr::lastIndexOf(ss10, ss12, true, 0);
    wxASSERT_MSG( mm12==12, wxT("invalid lastIndexOf") );

	wxString ss13 = wxT("EfG");
	int mm13 = ccStr::lastIndexOf(ss10, ss13, true, 5);
    wxASSERT_MSG( mm13==-1, wxT("invalid lastIndexOf") );

	wxString ss14 = wxT("EfGh");
	int mm14 = ccStr::lastIndexOf(ss10, ss14, true, 0);
    wxASSERT_MSG( mm14==-1, wxT("invalid lastIndexOf") );
#endif
}