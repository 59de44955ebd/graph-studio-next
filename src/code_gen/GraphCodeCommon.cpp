#include "stdafx.h"

#include <DShow.h>
#include <atlbase.h>

//#include <regex>
//
//#include <map>
//#include <vector>
//#include <cctype>
//#include <cstring>
//
//#include "time_utils.h"
//#include "object_details.h"
//
//#include "dbg.h"

#define GUID_LEN 39
#define XGUID_LEN 75
#define MAX_VAR_NAME 12

//######################################
// Returns either registry name or local name of filter
//######################################
void getFilterName (IBaseFilter * pFilter, WCHAR * wFilterName) {
	CString name;
	GUID clsid;
	pFilter->GetClassID(&clsid);
	if (SUCCEEDED(GraphStudio::GetObjectName(clsid, name))) {
		wcscpy_s(wFilterName, MAX_FILTER_NAME, name.GetBuffer());
	}
	else {
		FILTER_INFO filterInfo;
		pFilter->QueryFilterInfo(&filterInfo);
		if (filterInfo.pGraph) filterInfo.pGraph->Release();
		wcscpy_s(wFilterName, MAX_FILTER_NAME, filterInfo.achName);
	}
}

//######################################
// -> "{B98D13E7-55DB-4385-A33D-09FD1BA26338}"
//######################################
void guid_to_string (GUID clsid, char * guidStr) {
	sprintf_s(guidStr, GUID_LEN, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		clsid.Data1,
		clsid.Data2,
		clsid.Data3,
		clsid.Data4[0],
		clsid.Data4[1],
		clsid.Data4[2],
		clsid.Data4[3],
		clsid.Data4[4],
		clsid.Data4[5],
		clsid.Data4[6],
		clsid.Data4[7]
	);
}

//######################################
// -> "0xB98D13E7, 0x55DB, 0x4385, 0xA3, 0x3D, 0x09, 0xFD, 0x1B, 0xA2, 0x63, 0x38"
//######################################
void guid_to_xguid (GUID clsid, char * guidStr) {
	sprintf_s(guidStr, XGUID_LEN, "0x%08X, 0x%04X, 0x%04X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X",
		clsid.Data1,
		clsid.Data2,
		clsid.Data3,
		clsid.Data4[0],
		clsid.Data4[1],
		clsid.Data4[2],
		clsid.Data4[3],
		clsid.Data4[4],
		clsid.Data4[5],
		clsid.Data4[6],
		clsid.Data4[7]
	);
}

//######################################
//
//######################################
void removeDisallowed(char* source) {
	char* i = source;
	char* j = source;
	while (*j != 0){
		*i = *j++;
		//if (*i != ' ' && *i != '-' && *i != '+' && *i != ':' && *i != '*') i++;

		// only ASCII letters
		if ((*i >= 65 && *i <= 90) || (*i >= 97 && *i <= 122)) i++;

	}
	*i = 0;
}

//######################################
// 
//######################################
void escapeBackSlashes(std::string &s) {
	for (unsigned i = 0; i < s.size(); ++i)
		if (s[i] == '\\') s.insert(i++, "\\");
}

