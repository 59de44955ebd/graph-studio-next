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
const char * tplLuaHeader = {
	"----------------------------------------" EOL
	"-- GraphStudioNext code export for LuaJIT and extension 'directshow'" EOL
	"----------------------------------------" EOL
	EOL
	"local directshow = require('directshow')" EOL
	"local ffi = require('ffi')" EOL
	"local winapi = require('winapi')" EOL
	"require('winapi.windowclass')" EOL
	EOL
	"local function check_hr (hr, msg)" EOL
	"    if hr<0 then" EOL
	"        print(msg)" EOL
	"        directshow.forget()" EOL
	"        os.exit()" EOL
	"    end" EOL
	"end" EOL
	EOL
	"-- Create a window" EOL
	"local win = winapi.Window{" EOL
	"    title = 'GraphStudioNext Export'," EOL
	"    autoquit = true," EOL
	"    visible = false" EOL
	"}" EOL
	EOL
	"function win:on_resized()" EOL
	"    -- Resize video to window" EOL
	"    directshow.set_rect(0, 0, win.client_w, win.client_h)" EOL
	"end" EOL
	EOL
};

//######################################
// 
//######################################
const char * tplLuaBuildGraphStart = {
	EOL
	"local hr" EOL
	EOL
	"-- Initialize DirectShow" EOL
	"directshow.init()" EOL
	EOL
	"-- Create filter graph" EOL
	"directshow.graph_start()" EOL
	EOL
};

//######################################
// Define CLSID for a custom DirectShow filter
// $clsname - name for CLSID value
// $guid - GUID digits
// $file - name of file containing the filter
//######################################
const char * tplLuaDefineGUID = {
	"local $clsname = '$guid'" EOL
};

//######################################
// Create a DirectShow filter
// $name - name of the filter
// $clsname - name of CLSID value for this filter
//######################################
const char * tplLuaCreateFilter = {
	"hr = directshow.graph_add_filter($clsname)" EOL
	"check_hr(hr, 'Can\\'t add $name to graph')" EOL
	EOL
};

//######################################
// Create a DirectShow source filter
// $name - name of the filter
// $clsname - name of CLSID value for this filter
// $filename - variable holding name of file to open
//######################################
const char * tplLuaCreateSourceFilter = {
	"hr = directshow.graph_add_source('$filename', $clsname)" EOL
	"check_hr(hr, 'Can\\'t add $name to graph')" EOL
	EOL
};

//######################################
// Create a DirectShow sink filter
// $name - name of the filter
// $clsname - name of CLSID value for this filter
// $filename - variable holding name of file to open
//######################################
const char * tplLuaCreateSinkFilter = {
	"hr = directshow.graph_add_dest('$filename', $clsname)" EOL
	"check_hr(hr, 'Can\\'t add $name to graph')" EOL
	EOL
};

//######################################
// Connect two filters directly
// $names - names of connecting filters
// $pair - indexes of connecting filters as "a, b"
//######################################
const char * tplLuaConnect = {
	"-- Connect $names" EOL
	"hr = directshow.graph_connect($pair)" EOL
	"check_hr(hr, 'Can\\'t connect $names')" EOL
	EOL
};

//######################################
// Main
// $args - 
//######################################
const char * tplLuaMain = {
	"-- Finalize graph, passing options" EOL
	"directshow.graph_finalize({" EOL
	"    windowstyle = winapi.WS_CHILD," EOL
	"    hwnd = tonumber(ffi.cast('int32_t', win.hwnd))," EOL
	"    keepaspectratio = true," EOL
	"    paused = true" EOL
	"})" EOL
	EOL
	"-- Timer to check for DirectShow events" EOL
	"win:settimer(0.1, function ()" EOL
	"    evt, evtName, p1, p2 = directshow.get_event()" EOL
	"    if evt~=nil then " EOL
	"        --print(evtName, p1, p2)" EOL
	"        if evtName=='EC_COMPLETE' then" EOL
	"            win:close()" EOL
	"        end" EOL
	"    end" EOL
	"end," EOL
	"1)" EOL
	EOL
	"-- Show the window" EOL
	"win:show()" EOL
	EOL
	"-- Start playing" EOL
	"directshow.play()" EOL
	EOL
	"-- Start window message loop" EOL
	"winapi.MessageLoop()" EOL
	EOL
	"directshow.forget()" EOL
};

//######################################
//
//######################################
HRESULT getGraphCodeLua (IFilterGraph * pGraph, string * code) {
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

	char szFilterOutName[MAX_FILTER_NAME];

	WCHAR wFilterName[MAX_FILTER_NAME];

	char szFileName[MAX_PATH];

	char guid[GUID_LEN];

	std::string code_clsids("-- Define filter CLSIDs" EOL);
	std::string code_filters_add("-- Add filters to graph" EOL);
	std::string code_filters_connect;

	//######################################
	// Create filter list with reverse order
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
	for (int i = 0; i < (int)filterList.size(); i++)
	{
		pFilter = filterList[i];

		// get filter name
		getFilterName(pFilter, wFilterName);
		wcstombs(szFilterName, wFilterName, MAX_FILTER_NAME);

		// convert to varName (remove spaces etc)
		strcpy_s(szFilterNameCompact, MAX_FILTER_NAME, szFilterName);
		removeDisallowed(szFilterNameCompact);

		//######################################
		// define CLSID
		//######################################
		pFilter->GetClassID(&clsid);

		guid_to_string(clsid, guid);
		str = regex_replace(tplLuaDefineGUID, regex("\\$guid"), guid);
		str = regex_replace(str, regex("\\$clsname"), string("CLSID_").append(szFilterNameCompact));

		code_clsids.append(str);

		//######################################
		// add filter
		//######################################

		BOOL bFilterAdded = FALSE;

		// check if IFileSourceFilter
		CComQIPtr<IFileSourceFilter, &IID_IFileSourceFilter> pSource(pFilter);
		if (pSource) {
			WCHAR * src;
			AM_MEDIA_TYPE mt;
			hr = pSource->GetCurFile(&src, &mt);
			if (SUCCEEDED(hr)) {
				std::wcstombs(szFileName, src, MAX_PATH);
				
				string tmp(szFileName);
				escapeBackSlashes(tmp);
				
				str = regex_replace(tplLuaCreateSourceFilter, regex("\\$name"), szFilterName);		
				str = regex_replace(str, regex("\\$clsname"), string("CLSID_").append(szFilterNameCompact));
				str = regex_replace(str, regex("\\$filename"), tmp.c_str());
				
				code_filters_add.append(str);				
			}
			bFilterAdded = TRUE;
		}

		if (!bFilterAdded) {
			// check if IFileSinkFilter
			CComQIPtr<IFileSinkFilter, &IID_IFileSinkFilter> pSink(pFilter);
			if (pSink) {
				WCHAR * dst;
				AM_MEDIA_TYPE mt;
				hr = pSink->GetCurFile(&dst, &mt);
				if (SUCCEEDED(hr)) {
					std::wcstombs(szFileName, dst, MAX_PATH);

					string tmp(szFileName);
					escapeBackSlashes(tmp);

					str = regex_replace(tplLuaCreateSinkFilter, regex("\\$name"), szFilterName);
					str = regex_replace(str, regex("\\$clsname"), string("CLSID_").append(szFilterNameCompact));
					str = regex_replace(str, regex("\\$filename"), tmp.c_str());
				}
				bFilterAdded = TRUE;
			}
		}

		if (!bFilterAdded) {
			// neither source nor sink
			str = regex_replace(tplLuaCreateFilter, regex("\\$name"), szFilterName);
			str = regex_replace(str, regex("\\$clsname"), string("CLSID_").append(szFilterNameCompact));

			code_filters_add.append(str);
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
								
								// get name of output pin's filter
								getFilterName(pinInfoConnect.pFilter, wFilterName);
								wcstombs(szFilterOutName, wFilterName, MAX_FILTER_NAME);
								
								char pair[9];
								sprintf_s(pair, 9, "%d, %d", i+1, j+1);
								str = regex_replace(tplLuaConnect, regex("\\$pair"), pair);

								char names[MAX_FILTER_NAME*2 + 6];
								sprintf_s(names, MAX_FILTER_NAME * 2 + 6, "%s and %s", szFilterName, szFilterOutName);
								str = regex_replace(str, regex("\\$names"), names);
						
								code_filters_connect.append(str);
							}
						}
					}
				}
			}
		}
	}
	
	//######################################
	//
	//######################################
	code->append(tplLuaHeader);
	code->append(code_clsids);
	code->append(tplLuaBuildGraphStart);
	code->append(code_filters_add);
	code->append(code_filters_connect);
	code->append(tplLuaMain);

	return S_OK;
}
