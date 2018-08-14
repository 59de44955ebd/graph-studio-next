#include "stdafx.h"

#include <DShow.h>
#include <atlbase.h>
#include <deque>
#include <regex>
#include "GraphCodeCommon.h"

#define EOL "\r\n"

//######################################
// Template strings
//######################################

//######################################
// Header
//######################################
const char * tplBATHeader = {
	"::--------------------------------------" EOL
	":: GraphStudioNext code export for DScmd" EOL
	"::--------------------------------------" EOL
	EOL
	"@echo off" EOL
	//"chcp 1252>nul" EOL
	"chcp 65001>nul" EOL
	EOL
};

//######################################
// Define CLSID for a custom DirectShow filter
// $clsname - name for CLSID value
// $guid - GUID digits
// $file - name of file containing the filter
//######################################
const char * tplBATDefineGUID = {
	"set $clsname=$guid" EOL
};

//######################################
//
//######################################
const char * tplBATMain = {
	"DScmd.exe ^" EOL
	"-graph ^" EOL
	"$filters^" EOL
	"!$connect ^" EOL
	"-winCaption \"GraphStudioNext Export\" ^" EOL
	"-i"
};

//######################################
//
//######################################
HRESULT getGraphCodeBAT(IFilterGraph * pGraph, std::string * code) {
	HRESULT hr;

	deque<CComPtr<IBaseFilter>> filterList;
	
	CComPtr<IEnumFilters> pEnumFilters;
	CComPtr<IBaseFilter> pFilter;

	PIN_INFO pinInfo;
	PIN_INFO pinInfoConnect;

	CComPtr<IEnumPins> pEnumPins;
	CComPtr<IPin> pPin;

	GUID clsid;
	string str;

	char szFilterName[MAX_FILTER_NAME];
	char szFilterNameCompact[MAX_FILTER_NAME];
	WCHAR wFilterName[MAX_FILTER_NAME];
	char szFileName[MAX_PATH];
	char guid[GUID_LEN];

	std::string code_clsids;
	std::string code_filters_add;
	std::string code_filters_connect;

	BOOL bSupportedRenderer = FALSE;

	//######################################
	// Create filter list with reverse ordner
	//######################################
	hr = pGraph->EnumFilters(&pEnumFilters);
	if (FAILED(hr)) return hr;
	while (pEnumFilters->Next(1, &pFilter, 0) == S_OK)
	{
		filterList.push_front(pFilter);
	}

	//######################################
	// 
	//######################################
	if (filterList.size())
	for (int i = 0; i < (int)filterList.size(); i++)
	{
		pFilter = filterList[i];

		pFilter->GetClassID(&clsid);

		if (clsid == CLSID_VideoMixingRenderer) clsid = CLSID_VideoRendererDefault;
		if (clsid == CLSID_VideoRendererDefault || clsid == CLSID_VideoMixingRenderer9 || clsid == CLSID_EnhancedVideoRenderer) {
			bSupportedRenderer = TRUE;
		}

		// get filter name
		getFilterName(pFilter, wFilterName);
		std::wcstombs(szFilterName, wFilterName, MAX_FILTER_NAME);

		// convert to varName (remove spaces etc)
		strcpy_s(szFilterNameCompact, MAX_FILTER_NAME, szFilterName);
		removeDisallowed(szFilterNameCompact);

		//######################################
		// define CLSID
		//######################################

		guid_to_string(clsid, guid);
		str = regex_replace(tplBATDefineGUID, regex("\\$guid"), guid);
		str = regex_replace(str, regex("\\$clsname"), string("CLSID_").append(szFilterNameCompact));

		code_clsids.append(str);

		//######################################
		// add filter
		//######################################

		if (!code_filters_add.empty()) code_filters_add.append(",^" EOL);
		code_filters_add.append(string("%CLSID_").append(szFilterNameCompact).append("%"));

		// check if IFileSourceFilter
		CComQIPtr<IFileSourceFilter, &IID_IFileSourceFilter> pSource(pFilter);
		if (pSource) {
			WCHAR * src;
			AM_MEDIA_TYPE mt;
			hr = pSource->GetCurFile(&src, &mt);
			if (SUCCEEDED(hr)) {
				std::wcstombs(szFileName, src, MAX_PATH);
				code_filters_add.append(string(";src=").append(szFileName));
			}
		}

		// check if IFileSinkFilter
		CComQIPtr<IFileSinkFilter, &IID_IFileSinkFilter> pSink(pFilter);
		if (pSink) {
			WCHAR * dst;
			AM_MEDIA_TYPE mt;
			hr = pSink->GetCurFile(&dst, &mt);
			if (SUCCEEDED(hr)) {
				std::wcstombs(szFileName, dst, MAX_PATH);
				code_filters_add.append(string(";dest=").append(szFileName));
			}
		}

		//######################################
		// enum filter pins
		//######################################
		HRESULT hr = pFilter->EnumPins(&pEnumPins);

		if (SUCCEEDED(hr))
		while (pEnumPins->Next(1, &pPin, 0) == S_OK)
		{

			// check if connected
			CComPtr<IPin> pPinConnect;
			hr = pPin->ConnectedTo(&pPinConnect);
			if (SUCCEEDED(hr)) {

				pPin->QueryPinInfo(&pinInfo);
				if (pinInfo.pFilter) pinInfo.pFilter->Release();

				if (pinInfo.dir == PINDIR_OUTPUT) {

					// get the connected target pin and its filter
					pPinConnect->QueryPinInfo(&pinInfoConnect);
					if (pinInfoConnect.pFilter) {

						// find target filter in list
						for (int j = 0; j < (int)filterList.size(); j++)
						{
							if (j != i && filterList[j] == pinInfoConnect.pFilter)
							{
								if (!code_filters_connect.empty()) code_filters_connect.append(",");
								char pair[8];
								sprintf_s(pair, 8, "%d:%d", i, j);
								code_filters_connect.append(pair);
							}
						}
					}
				}
			}
		}
	}

	code->append(tplBATHeader);
	code->append(code_clsids);
	code->append(EOL);

	str = regex_replace(tplBATMain, regex("\\$filters"), code_filters_add);
	str = regex_replace(str, regex("\\$connect"), code_filters_connect);
	code->append(str);

	if (!bSupportedRenderer) code->append(" -noWindow" EOL);

	return S_OK;
}
