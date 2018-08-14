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
const char * tplCPPHeader = {
	"//--------------------------------------" EOL
	"// GraphStudioNext code export for C++ and MS Visual Studio" EOL
	"//--------------------------------------" EOL
	EOL
	"#include <DShow.h>" EOL
	"#include <atlbase.h>" EOL
	"#include <initguid.h>" EOL
	EOL
	"BOOL hrcheck(HRESULT hr, const WCHAR* errtext)" EOL
	"{" EOL
	"    if (hr >= S_OK) return TRUE;" EOL
	"    WCHAR szErr[MAX_ERROR_TEXT_LEN];" EOL
	"    DWORD res = AMGetErrorText(hr, szErr, MAX_ERROR_TEXT_LEN);" EOL
	"    if (res)" EOL
	"        wprintf(L\"Error %x: %ls\\r\\n%ls\\r\\n\", hr, errtext, szErr);" EOL
	"    else" EOL
	"        wprintf(L\"Error %x: %ls\\r\\n\", hr, errtext);" EOL
	"    return FALSE;" EOL
	"}" EOL
	EOL
	"#define CHECK_HR(hr, msg) if (!hrcheck(hr, msg)) return hr;" EOL
	EOL
	"CComPtr<IPin> GetPin(IBaseFilter *pFilter, LPCOLESTR pinname)" EOL
	"{" EOL
	"    CComPtr<IEnumPins>    pEnum;" EOL
	"    CComPtr<IPin>         pPin;" EOL
	EOL
	"    HRESULT hr = pFilter->EnumPins(&pEnum);" EOL
	"    if (FAILED(hr)) return NULL;" EOL
	EOL
	"    while (pEnum->Next(1, &pPin, 0) == S_OK)" EOL
	"    {" EOL
	"        PIN_INFO pinfo;" EOL
	"        pPin->QueryPinInfo(&pinfo);" EOL
	"        BOOL found = !wcscmp(pinname, pinfo.achName);" EOL
	"         if (pinfo.pFilter) pinfo.pFilter->Release();" EOL
	"         if (found) return pPin;" EOL
	"    }" EOL
	"    printf(\"Pin not found!\\r\\n\");" EOL
	"    return NULL;" EOL
	"}" EOL
	EOL
};

//######################################
// Start of the BuildGraph function
// $args - 
//######################################
const char * tplCPPBuildGraphStart = {
	"HRESULT BuildGraph(IGraphBuilder *pGraph$args)" EOL
	"{" EOL
	"    HRESULT hr = S_OK;" EOL
	EOL
	"    //graph builder" EOL
	"    CComPtr<ICaptureGraphBuilder2> pBuilder;" EOL
	"    hr = pBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder2);" EOL
	"    CHECK_HR(hr, L\"Can't create Capture Graph Builder\");" EOL
	"    hr = pBuilder->SetFiltergraph(pGraph);" EOL
	"    CHECK_HR(hr, L\"Can't SetFiltergraph\");" EOL
	EOL
};

//######################################
// End of the BuildGraph function
//######################################
const char * tplCPPBuildGraphEnd = {
	"    return S_OK;" EOL
	"}" EOL
	EOL
};

//######################################
// Define CLSID for a custom DirectShow filter
// $clsname - name for CLSID value
// $guid - GUID digits
// $xguid - GUID as a sequence of 0x.. values
// $file - name of file containing the filter
//######################################
const char * tplCPPDefineGUID = {
	"// $guid" EOL
	"DEFINE_GUID($clsname, $xguid);" EOL
	EOL
};

//######################################
// Create a DirectShow filter
// $name - name of the filter
// $var - variable to hold IBaseFilter
// $clsname - name of CLSID value for this filter
//######################################
const char * tplCPPCreateFilter = {
	"    //add $name" EOL
	"    CComPtr<IBaseFilter> $var;" EOL
	"    hr = $var.CoCreateInstance($clsname);" EOL
	"    CHECK_HR(hr, L\"Can't create $name\");" EOL
	"    hr = pGraph->AddFilter($var, L\"$name\");" EOL
	"    CHECK_HR(hr, L\"Can't add $name to graph\");" EOL
	EOL
};

//######################################
// Create a filter by display name
// $name - name of the filter
// $var - variable holding IBaseFilter
// $displayname - display name of the filter
//######################################
//const char * tplCPPCreateFilterByName = {
//	"    //add $name" EOL
//	"    CComPtr<IBaseFilter> $var = CreateFilter(L\"$displayname\");" EOL
//	EOL
//};

//######################################
// Create a filter by name and category
// $name - name of the filter
// $var - variable holding IBaseFilter
// $category - CLSID of this filter's category
//######################################
//const char * tplCPPCreateFilterByNameCat = {
//	"    //add $name" EOL
//	"    CComPtr<IBaseFilter> $var = CreateFilterByName(L\"$name\", $category);" EOL
//	EOL
//};

//######################################
// Create a DMO filter
// $name - name of the filter
// $var - variable to hold IBaseFilter
// $clsname - name of CLSID value for this DMO
// $dmocat - name of CLSID value for DMO category
//######################################
//const char * tplCPPCreateDMO = {
//	"    //add $name" EOL
//	"    CComPtr<IBaseFilter> $var;" EOL
//	"    hr = $var.CoCreateInstance(CLSID_DMOWrapperFilter);" EOL
//	"    CHECK_HR(hr, L\"Can't create DMO Wrapper\");" EOL
//	"    CComQIPtr<IDMOWrapperFilter, &IID_IDMOWrapperFilter> $var_wrapper($var);" EOL
//	"    if (!$var_wrapper)" EOL
//	"        CHECK_HR(E_NOINTERFACE, L\"Can\'t get IDMOWrapperFilter\");" EOL
//	"    hr = $var_wrapper->Init($clsname, $dmocat);" EOL
//	"    CHECK_HR(hr, L\"DMO Wrapper Init failed\");" EOL
//	EOL
//};

//######################################
// Set source file to IFileSourceFilter
// $srcvar - variable to hold IFileSourceFilter
// $var - variable holding IBaseFilter
// $filename - variable holding name of file to open
//######################################
const char * tplCPPSetSourceFile = {
	"    //set source filename" EOL
	"    CComQIPtr<IFileSourceFilter, &IID_IFileSourceFilter> $srcvar($var);" EOL
	"    if (!$srcvar)" EOL
	"        CHECK_HR(E_NOINTERFACE, L\"Can't get IFileSourceFilter\");" EOL
	"    hr = $srcvar->Load($filename, NULL);" EOL
	"    CHECK_HR(hr, L\"Can't load file\");" EOL
	EOL
};

//######################################
// Set destination file to IFileSinkFilter
// $dstvar - variable to hold IFileSinkFilter
// $var - variable holding IBaseFilter
// $filename - variable holding name of file to open
//######################################
const char * tplCPPSetDestFile = {
	"    //set destination filename" EOL
	"    CComQIPtr<IFileSinkFilter, &IID_IFileSinkFilter> $dstvar($var);" EOL
	"    if (!$dstvar)" EOL
	"        CHECK_HR(E_NOINTERFACE, L\"Can't get IFileSinkFilter\");" EOL
	"    hr = $dstvar->SetFileName($filename, NULL);" EOL
	"    CHECK_HR(hr, L\"Can't set filename\");" EOL
	EOL
};

//######################################
// Connect two filters
// $pair - names of connecting filters
// $majortype - major media type of connection
// $var1, $var2 - variables holding IBaseFilter of connecting filters
//######################################
//const char * tplCPPConnect = {
//	"    //connect $pair" EOL
//	"    hr = pBuilder->RenderStream(NULL, &$majortype, $var1, NULL, $var2);" EOL
//	"    CHECK_HR(hr, L\"Can't connect $pair\");" EOL
//	EOL
//};

//######################################
// Connect two filters directly
// $pair - names of connecting filters
// $var1, $var2 - variables holding IBaseFilter of connecting filters
// $pin1, $pin2 - names of connecting pins
//######################################
const char * tplCPPConnectDirect = {
	"    //connect $pair" EOL
	"    hr = pGraph->ConnectDirect(GetPin($var1, L\"$pin1\"), GetPin($var2, L\"$pin2\"), NULL);" EOL
	"    CHECK_HR(hr, L\"Can't connect $pair\");" EOL
	EOL
};

//######################################
// Main function
// $args - 
//######################################
const char * tplCPPMain = {
	"int main(int argc, char* argv[])" EOL
	"{" EOL
	"    CoInitialize(NULL);" EOL
	"    CComPtr<IGraphBuilder> pGraph;" EOL
	"    pGraph.CoCreateInstance(CLSID_FilterGraph);" EOL
	EOL
	"    printf(\"Building graph...\\r\\n\");" EOL
	"    HRESULT hr = BuildGraph(pGraph$args);" EOL
	"    if (hrcheck(hr, L\"Can't BuildGraph\")) {" EOL
	"        CComQIPtr<IMediaControl, &IID_IMediaControl> pMediaControl(pGraph);" EOL
	"        hr = pMediaControl->Run();" EOL
	"        if (hrcheck(hr, L\"Can't run the graph\")) {" EOL
	"            printf(\"Running\");" EOL
	"            CComQIPtr<IMediaEvent, &IID_IMediaEvent> pMediaEvent(pGraph);" EOL
	"            BOOL stop = FALSE;" EOL
	"            MSG msg;" EOL
	"            while (!stop)" EOL
	"            {" EOL
	"                long ev = 0, p1 = 0, p2 = 0;" EOL
	"                Sleep(50);" EOL
	"                printf(\".\");" EOL
	"                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))" EOL
	"                    DispatchMessage(&msg);" EOL
	"                while (pMediaEvent->GetEvent(&ev, &p1, &p2, 0) == S_OK)" EOL
	"                {" EOL
	"                    if (ev == EC_COMPLETE || ev == EC_USERABORT)" EOL
	"                    {" EOL
	"                        printf(\"\\r\\nDone!\\r\\n\");" EOL
	"                        stop = TRUE;" EOL
	"                    }" EOL
	"                    else if (ev == EC_ERRORABORT)" EOL
	"                    {" EOL
	"                        printf(\"An error occured: HRESULT=%x\\r\\n\", p1);" EOL
	"                        pMediaControl->Stop();" EOL
	"                        stop = TRUE;" EOL
	"                    }" EOL
	"                    pMediaEvent->FreeEventParams(ev, p1, p2);" EOL
	"                }" EOL
	"            }" EOL
	"        }" EOL
	"    }" EOL
	"    CoUninitialize();" EOL
	"    return 0;" EOL
	"}" EOL
	EOL
};

//######################################
//
//######################################
HRESULT getGraphCodeCPP (IFilterGraph * pGraph, string * code) {
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

	char fileVarName[MAX_VAR_NAME]; // srcFile<N>, dstFile<N>

	char szPinName[MAX_PIN_NAME];

	vector<string> srcFiles;
	vector<string> dstFiles;

	char szFileName[MAX_PATH];

	char guid[GUID_LEN];
	char xguid[XGUID_LEN];

	std::string code_clsids;
	std::string code_build_start;
	std::string code_filters_add;
	std::string code_filters_connect;
	std::string code_main;

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
		str = regex_replace(tplCPPDefineGUID, regex("\\$guid"), guid);
		str = regex_replace(str, regex("\\$clsname"), string("CLSID_GSN_").append(szFilterNameCompact));
		guid_to_xguid(clsid, xguid);
		str = regex_replace(str, regex("\\$xguid"), xguid);

		code_clsids.append(str);

		//######################################
		// add filter
		//######################################
		str = regex_replace(tplCPPCreateFilter, regex("\\$name"), szFilterName);
		str = regex_replace(str, regex("\\$var"), string("p").append(szFilterNameCompact));
		str = regex_replace(str, regex("\\$clsname"), string("CLSID_GSN_").append(szFilterNameCompact));
		code_filters_add.append(str);

		// check if IFileSourceFilter
		CComQIPtr<IFileSourceFilter, &IID_IFileSourceFilter> pSource(pFilter);
		if (pSource) {
			WCHAR * src; // OLECHAR is wchar_t since about 1995.
			AM_MEDIA_TYPE mt;
			hr = pSource->GetCurFile(&src, &mt);
			if (SUCCEEDED(hr)) {

				// convert to char*, and escape backslahes
				wcstombs(szFileName, src, MAX_PATH);
				string tmp(szFileName);
				escapeBackSlashes(tmp);
				srcFiles.push_back(tmp);

				sprintf_s(fileVarName, MAX_VAR_NAME, "srcFile%u", (unsigned int)srcFiles.size());

				str = regex_replace(tplCPPSetSourceFile, regex("\\$srcvar"), string("p").append(szFilterNameCompact).append("_src"));
				str = regex_replace(str, regex("\\$var"), string("p").append(szFilterNameCompact));
				str = regex_replace(str, regex("\\$filename"), string(fileVarName));

				code_filters_add.append(str);
			}
		}

		// check if IFileSinkFilter
		CComQIPtr<IFileSinkFilter, &IID_IFileSinkFilter> pSink(pFilter);
		if (pSink) {
			WCHAR * dst;
			AM_MEDIA_TYPE mt;
			hr = pSink->GetCurFile(&dst, &mt);
			if (SUCCEEDED(hr)) {

				// convert to char*, and escape backslahes
				wcstombs(szFileName, dst, MAX_PATH);
				string tmp(szFileName);
				escapeBackSlashes(tmp);
				dstFiles.push_back(tmp);

				sprintf_s(fileVarName, MAX_VAR_NAME, "dstFile%u", (unsigned int)dstFiles.size());

				str = regex_replace(tplCPPSetDestFile, regex("\\$dstvar"), string("p").append(szFilterNameCompact).append("_sink"));
				str = regex_replace(str, regex("\\$var"), string("p").append(szFilterNameCompact));
				str = regex_replace(str, regex("\\$filename"), string(fileVarName));

				code_filters_add.append(str);
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

						// get name of output pin's filter
						getFilterName(pinInfoConnect.pFilter, wFilterName);
						wcstombs(szFilterOutName, wFilterName, MAX_FILTER_NAME);

						//connect A and B
						char pair[MAX_FILTER_NAME*2 + 6];

						sprintf_s(pair, MAX_FILTER_NAME * 2 + 6, "%s and %s", szFilterName, szFilterOutName);
						str = regex_replace(tplCPPConnectDirect, regex("\\$pair"), pair);

						strcpy_s(szFilterNameCompact, MAX_FILTER_NAME, szFilterName);
						removeDisallowed(szFilterNameCompact);

						str = regex_replace(str, regex("\\$var1"), string("p").append(szFilterNameCompact));

						strcpy_s(szFilterNameCompact, MAX_FILTER_NAME, szFilterOutName);
						removeDisallowed(szFilterNameCompact);

						str = regex_replace(str, regex("\\$var2"), string("p").append(szFilterNameCompact));

						wcstombs(szPinName, pinInfo.achName, MAX_PIN_NAME);
						str = regex_replace(str, regex("\\$pin1"), szPinName);

						wcstombs(szPinName, pinInfoConnect.achName, MAX_PIN_NAME);
						str = regex_replace(str, regex("\\$pin2"), szPinName);

						code_filters_connect.append(str);

						pinInfo.pFilter->Release();
					}
				}
			}
		}
	}

	//######################################
	// BuildGraphStart
	//######################################

	// build args string
	str.clear();
	for (unsigned int i = 0; i < srcFiles.size(); i++) {
		sprintf(fileVarName, "srcFile%d", i + 1);
		str.append(", LPCOLESTR ");
		str.append(fileVarName);
	}
	for (unsigned int i = 0; i < dstFiles.size(); i++) {
		sprintf(fileVarName, "dstFile%d", i + 1);
		str.append(", LPCOLESTR ");
		str.append(fileVarName);
	}

	code_build_start = std::regex_replace(tplCPPBuildGraphStart, regex("\\$args"), str);

	//######################################
	//Main
	//######################################
	std::string args;
	for (unsigned int i = 0; i<srcFiles.size(); i++) {
		args.append(", L\"").append(srcFiles[i].c_str()).append("\"");
	}
	for (unsigned int i = 0; i<dstFiles.size(); i++) {
		args.append(", L\"").append(dstFiles[i].c_str()).append("\"");
	}

	str = regex_replace(tplCPPMain, regex("\\$args"), args);
	code_main.append(str);

	code->append(tplCPPHeader);
	code->append(code_clsids);
	code->append(code_build_start);
	code->append(code_filters_add);
	code->append(code_filters_connect);
	code->append(tplCPPBuildGraphEnd);
	code->append(code_main);

	return S_OK;
}
