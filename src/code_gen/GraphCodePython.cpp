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
const char * tplPythonHeader = {
	"#---------------------------------------" EOL
	"# GraphStudioNext code export for Python" EOL
	"# --------------------------------------" EOL
	EOL
	"import ctypes" EOL
	"from ctypes import *" EOL
	"from ctypes.wintypes import *" EOL
	"from comtypes import client" EOL
	"from comtypes.gen.DirectShowLib import *" EOL
	"from win32 import constants as c" EOL
	EOL
	"client.GetModule('.\\DirectShow.tlb')" EOL
	"client.GetModule('qedit.dll') # DexterLib" EOL
	"quartz = client.GetModule('quartz.dll')" EOL
	EOL
	"WNDPROCTYPE = WINFUNCTYPE(c_int, HWND, c_uint, WPARAM, LPARAM)" EOL
	EOL
	"EC_COMPLETE = 1" EOL
	"EC_USERABORT = 2" EOL
	"EC_ERRORABORT = 3" EOL
	EOL
	"class WNDCLASSEX(Structure):" EOL
	"    _fields_ = [('cbSize', c_uint)," EOL
	"                ('style', c_uint)," EOL
	"                ('lpfnWndProc', WNDPROCTYPE)," EOL
	"                ('cbClsExtra', c_int)," EOL
	"                ('cbWndExtra', c_int)," EOL
	"                ('hInstance', HANDLE)," EOL
	"                ('hIcon', HANDLE)," EOL
	"                ('hCursor', HANDLE)," EOL
	"                ('hBrush', HANDLE)," EOL
	"                ('lpszMenuName', LPCWSTR)," EOL
	"                ('lpszClassName', LPCWSTR)," EOL
	"                ('hIconSm', HANDLE)]" EOL
	EOL
	"def WindowProc (hWnd, msg, wParam, lParam):" EOL
	EOL
	"	if msg == c.WM_SIZE:" EOL
	"		# Resize video to window" EOL
	"		rect = RECT()" EOL
	"		rectPtr = pointer(rect)" EOL
	"		windll.user32.GetClientRect(hWnd, rectPtr)" EOL
	"		videoWindow.SetWindowPosition(0, 0, rect.right, rect.bottom)" EOL
	EOL
	"	elif msg == c.WM_TIMER:" EOL
	"		while True:" EOL
	"			try:" EOL
	"				ev,p1,p2 = mediaEvent.GetEvent(0)" EOL
	"				if ev == EC_COMPLETE or ev == EC_USERABORT:" EOL
	"					print('Done.')" EOL
	"					windll.user32.PostQuitMessage(0)" EOL
	"				elif ev == EC_ERRORABORT:" EOL
	"					print('An error occured: HRESULT=%x' % p1)" EOL
	"					mediaControl.Stop()" EOL
	"					windll.user32.PostQuitMessage(0)" EOL
	"			except:" EOL
	"				break" EOL
	EOL
	"	elif msg == c.WM_DESTROY:" EOL
	"		windll.user32.PostQuitMessage(0)" EOL
	EOL
	"	else:" EOL
	"		return windll.user32.DefWindowProcW(hWnd, msg, wParam, lParam)" EOL
	EOL
	"	return 0" EOL
	EOL
	"def GetPin (filter, pinName):" EOL
	"	enum = filter.EnumPins()" EOL
	"	while True:" EOL
	"		pin , fetched = enum.Next(1)" EOL
	"		if fetched:" EOL
	"			pinInfo = pin.QueryPinInfo()" EOL
	"			if pinName in ''.join(map(chr, pinInfo.achName)):" EOL
	"				return pin" EOL
	"		else:" EOL
	"			pin.Release()" EOL
	"			break" EOL
	"	print(\"Pin '%s' not found!\" % pinName)" EOL
	"	return None" EOL
	EOL
	"# define CLSIDs" EOL
	"CLSID_FilterGraph = '{E436EBB3-524F-11CE-9F53-0020AF0BA770}'" EOL
};

//######################################
// Start of the BuildGraph function
// $args - 
//######################################
const char * tplPythonBuildGraphStart = {
	EOL
	"def BuildGraph (graph$args):" EOL
	EOL
};

//######################################
// Define CLSID for a custom DirectShow filter
// $clsname - name for CLSID value
// $guid - GUID digits
//######################################
const char * tplPythonDefineGUID = {
	"$clsname = '$guid'" EOL
};

//######################################
// Create a DirectShow filter
// $name - name of the filter
// $var - variable to hold IBaseFilter
// $clsname - name of CLSID value for this filter
//######################################
const char * tplPythonCreateFilter = {
	"    # Add $name" EOL
	"    $var = client.CreateObject($clsname, interface = IBaseFilter)" EOL
	"    graph.AddFilter($var, '$name')" EOL
	EOL
};

//######################################
// Set source file to IFileSourceFilter
// $srcvar - variable to hold IFileSourceFilter
// $var - variable holding IBaseFilter
// $filename - variable holding name of file to open
//######################################
const char * tplPythonSetSourceFile = {
	"    # Set source filename" EOL
	"    $srcvar = $var.QueryInterface(IFileSourceFilter)" EOL
	"    $srcvar.Load($filename, None)" EOL
	EOL
};

//######################################
// Set destination file to IFileSinkFilter
// $dstvar - variable to hold IFileSinkFilter
// $var - variable holding IBaseFilter
// $filename - variable holding name of file to open
//######################################
const char * tplPythonSetDestFile = {
	"    # Set destination filename" EOL
	"    $dstvar = $var.QueryInterface(IFileSinkFilter)" EOL
	"    $dstvar.SetFileName($filename, None)" EOL
	EOL
};

//######################################
// Connect two filters directly
// $pair - names of connecting filters
// $var1, $var2 - variables holding IBaseFilter of connecting filters
// $pin1, $pin2 - names of connecting pins
//######################################
const char * tplPythonConnectDirect = {
	"    # Connect $pair" EOL
	"    graph.ConnectDirect(GetPin($var1, '$pin1'), GetPin($var2, '$pin2'), None)" EOL
	EOL
};

//######################################
// Main function
// $args - 
//######################################
const char * tplPythonMain = {
	"# Create a window" EOL
	"hInst = windll.kernel32.GetModuleHandleA(0)" EOL
	"wclassName = 'GraphStudioNextExport'" EOL
	"wname = 'GraphStudioNext Export'" EOL
	EOL
	"wndClass = WNDCLASSEX()" EOL
	"wndClass.cbSize = sizeof(WNDCLASSEX)" EOL
	"wndClass.style = c.CS_HREDRAW | c.CS_VREDRAW" EOL
	"wndClass.lpfnWndProc = WNDPROCTYPE(WindowProc)" EOL
	"wndClass.hInstance = hInst" EOL
	"wndClass.hBrush = windll.gdi32.GetStockObject(c.BLACK_BRUSH)" EOL
	"wndClass.lpszClassName = wclassName" EOL
	EOL
	"windll.user32.RegisterClassExA(byref(wndClass))" EOL
	EOL
	"hWnd = windll.user32.CreateWindowExA(0,wclassName,wname," EOL
	"		c.WS_OVERLAPPEDWINDOW | c.WS_CAPTION," EOL
	"		c.CW_USEDEFAULT, c.CW_USEDEFAULT, c.CW_USEDEFAULT, c.CW_USEDEFAULT," EOL
	"		0,0,hInst,0)" EOL
	EOL
	"if not hWnd:" EOL
	"	print('Failed to create window')" EOL
	"	exit(0)" EOL
	EOL
	"graph = client.CreateObject(CLSID_FilterGraph, interface=IFilterGraph)" EOL
	EOL
	"print('Building graph...')" EOL
	"BuildGraph(graph$args)" EOL
	EOL
	"mediaControl = graph.QueryInterface(quartz.IMediaControl)" EOL
	"mediaEvent = graph.QueryInterface(quartz.IMediaEvent)" EOL
	EOL
	"videoWindow = graph.QueryInterface(quartz.IVideoWindow)" EOL
	"videoWindow.Owner = hWnd" EOL
	"videoWindow.WindowStyle = c.WS_CHILD|c.WS_CLIPCHILDREN|c.WS_CLIPSIBLINGS" EOL
	EOL
	"# Create a timer to check for DirectShow events" EOL
	"windll.user32.SetTimer(hWnd, 1, 50, None)" EOL
	EOL
	"# Show the window" EOL
	"windll.user32.ShowWindow(hWnd, c.SW_SHOW)" EOL
	"windll.user32.UpdateWindow(hWnd)" EOL
	EOL
	"# Run the graph" EOL
	"mediaControl.Run()" EOL
	"print('Running...');" EOL
	EOL
	"# Start window message loop" EOL
	"msg = MSG()" EOL
	"lpmsg = pointer(msg)" EOL
	"while windll.user32.GetMessageA(lpmsg, 0, 0, 0) != 0:" EOL
	"	windll.user32.TranslateMessage(lpmsg)" EOL
	"	windll.user32.DispatchMessageA(lpmsg)" EOL
	EOL
	"# Clean up" EOL
	"del mediaControl" EOL
	"del mediaEvent" EOL
	"del videoWindow" EOL
	"del graph" EOL
};

//######################################
//
//######################################
HRESULT getGraphCodePython (IFilterGraph * pGraph, string * code) {
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
		str = regex_replace(tplPythonDefineGUID, regex("\\$guid"), guid);
		str = regex_replace(str, regex("\\$clsname"), string("CLSID_").append(szFilterNameCompact));

		code_clsids.append(str);

		//######################################
		// add filter
		//######################################
		str = regex_replace(tplPythonCreateFilter, regex("\\$name"), szFilterName);
		str = regex_replace(str, regex("\\$var"), string("p").append(szFilterNameCompact));
		str = regex_replace(str, regex("\\$clsname"), string("CLSID_").append(szFilterNameCompact));
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

				str = regex_replace(tplPythonSetSourceFile, regex("\\$srcvar"), string("p").append(szFilterNameCompact).append("_src"));
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

				str = regex_replace(tplPythonSetDestFile, regex("\\$dstvar"), string("p").append(szFilterNameCompact).append("_sink"));
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
						str = regex_replace(tplPythonConnectDirect, regex("\\$pair"), pair);

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
		str.append(", ");
		str.append(fileVarName);
	}
	for (unsigned int i = 0; i < dstFiles.size(); i++) {
		sprintf(fileVarName, "dstFile%d", i + 1);
		str.append(", ");
		str.append(fileVarName);
	}

	code_build_start = std::regex_replace(tplPythonBuildGraphStart, regex("\\$args"), str);

	//######################################
	//Main
	//######################################
	std::string args;
	for (unsigned int i = 0; i<srcFiles.size(); i++) {
		args.append(", '").append(srcFiles[i].c_str()).append("'");
	}
	for (unsigned int i = 0; i<dstFiles.size(); i++) {
		args.append(", '").append(dstFiles[i].c_str()).append("'");
	}

	str = regex_replace(tplPythonMain, regex("\\$args"), args);
	code_main.append(str);

	code->append(tplPythonHeader);
	code->append(code_clsids);
	code->append(code_build_start);
	code->append(code_filters_add);
	code->append(code_filters_connect);
	code->append(code_main);

	return S_OK;
}
