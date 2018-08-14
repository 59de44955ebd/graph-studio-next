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
const char * tplCSharpHeader = {
	"//--------------------------------------" EOL
	"// GraphStudioNext code export for C# and DirectShowLib" EOL
	"//--------------------------------------" EOL
	EOL
	"using System;" EOL
	"using System.Collections.Generic;" EOL
	"using System.Runtime.InteropServices;" EOL
	"using System.Runtime.InteropServices.ComTypes;" EOL
	"using DirectShowLib;" EOL
	EOL
	"namespace GraphCode" EOL
	"{" EOL
	"    class Program" EOL
	"    {" EOL
	EOL
	"        static void checkHR(int hr, string msg)" EOL
	"        {" EOL
	"            if (hr < 0)" EOL
	"            {" EOL
	"                Console.WriteLine(msg);" EOL
	"                DsError.ThrowExceptionForHR(hr);" EOL
	"            }" EOL
	"        }" EOL
	EOL
	"        static IPin GetPin(IBaseFilter pFilter, string pinname)" EOL
	"        {" EOL
	"            IEnumPins pEnum;" EOL
	"            IntPtr pPin = Marshal.AllocCoTaskMem(4);" EOL
	EOL
	"            int hr = pFilter.EnumPins(out pEnum);" EOL
	"            checkHR(hr, \"Can't enumerate pins\");" EOL
	EOL
	"            IPin[] pins = new IPin[1];" EOL
	"            while (pEnum.Next(1, pins, pPin) == 0)" EOL
	"            {" EOL
	"                PinInfo pinfo;" EOL
	"                pins[0].QueryPinInfo(out pinfo);" EOL
	"                bool found = (pinname == pinfo.name);" EOL
	"                DsUtils.FreePinInfo(pinfo);" EOL
	"                if (found) return pins[0];" EOL
	"            }" EOL
	"            checkHR(-1, \"Pin not found\");" EOL
	"            return null;" EOL
	"        }" EOL
	EOL
};

//######################################
// Start of the BuildGraph function
// $args - 
//######################################
const char * tplCSharpBuildGraphStart = {
	EOL
	"        static void BuildGraph(IGraphBuilder pGraph$args)" EOL
	"        {" EOL
	"            int hr = 0;" EOL
	EOL
	"            //graph builder" EOL
	"            ICaptureGraphBuilder2 pBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();" EOL
	"            hr = pBuilder.SetFiltergraph(pGraph);" EOL
	"            checkHR(hr, \"Can't SetFiltergraph\");" EOL
	EOL
};

//######################################
// End of the BuildGraph function
//######################################
const char * tplCSharpBuildGraphEnd = {
	"        }" EOL
	EOL
};

//######################################
// Define CLSID for a custom DirectShow filter
// $clsname - name for CLSID value
// $guid - GUID digits
//######################################
const char * tplCSharpDefineGUID = {
	"            Guid $clsname = new Guid(\"$guid\");" EOL
};

//######################################
// Create a DirectShow filter
// $name - name of the filter
// $var - variable to hold IBaseFilter
// $clsname - name of CLSID value for this filter
//######################################
const char * tplCSharpCreateFilter = {
    "            //add $name" EOL
    "            IBaseFilter $var = (IBaseFilter)Activator.CreateInstance(Type.GetTypeFromCLSID($clsname));" EOL
    "            hr = pGraph.AddFilter($var, \"$name\");" EOL
    "            checkHR(hr, \"Can't add $name to graph\");" EOL
	EOL
};

//######################################
// Set source file to IFileSourceFilter
// $srcvar - variable to hold IFileSourceFilter
// $var - variable holding IBaseFilter
// $filename - variable holding name of file to open
//######################################
const char * tplCSharpSetSourceFile = {
	"            //set source filename" EOL
	"            IFileSourceFilter $srcvar = $var as IFileSourceFilter;" EOL
	"            if ($srcvar == null)" EOL
	"                checkHR(unchecked((int)0x80004002), \"Can't get IFileSourceFilter\");" EOL
	EOL
	"            hr = $srcvar.Load($filename, null);" EOL
	"            checkHR(hr, \"Can't load file\");" EOL
	EOL
};

//######################################
// Set destination file to IFileSinkFilter
// $dstvar - variable to hold IFileSinkFilter
// $var - variable holding IBaseFilter
// $filename - variable holding name of file to open
//######################################
const char * tplCSharpSetDestFile = {
	"            //set destination filename" EOL
	"            IFileSinkFilter $dstvar = $var as IFileSinkFilter;" EOL
	"            if ($dstvar == null)" EOL
	"                checkHR(unchecked((int)0x80004002), \"Can't get IFileSinkFilter\");" EOL
	"            hr = $dstvar.SetFileName($filename, null);" EOL
	"            checkHR(hr, \"Can't set filename\");" EOL
	EOL
};

//######################################
// Connect two filters directly
// $pair - names of connecting filters
// $var1, $var2 - variables holding IBaseFilter of connecting filters
// $pin1, $pin2 - names of connecting pins
//######################################
const char * tplCSharpConnectDirect = {
	"            //connect $pair" EOL
	"            hr = pGraph.ConnectDirect(GetPin($var1, \"$pin1\"), GetPin($var2, \"$pin2\"), null);" EOL
	"            checkHR(hr, \"Can't connect $pair\");" EOL	
	EOL
};

//######################################
// Main function
// $args - 
//######################################
const char * tplCSharpMain = {
	"        static void Main(string[] args)" EOL
	"        {" EOL
	"            try" EOL
	"            {" EOL
	"                IGraphBuilder pGraph = (IGraphBuilder)new FilterGraph();" EOL
	"                Console.WriteLine(\"Building graph...\");" EOL
	EOL
	"                BuildGraph(pGraph$args);" EOL
	"                Console.WriteLine(\"Running...\");" EOL
	EOL
	"                IMediaControl pMediaControl = (IMediaControl)pGraph;" EOL
	"                IMediaEvent pMediaEvent = (IMediaEvent)pGraph;" EOL
	EOL
	"                int hr = pMediaControl.Run();" EOL
	"                checkHR(hr, \"Can't run the graph\");" EOL
	EOL
	"                bool stop = false;" EOL
	"                while (!stop)" EOL
	"                {" EOL
	"                    System.Threading.Thread.Sleep(50);" EOL
	"                    Console.Write(\".\");" EOL
	"                    EventCode ev;" EOL
	"                    IntPtr p1, p2;" EOL
	"                    System.Windows.Forms.Application.DoEvents();" EOL
	"                    while (pMediaEvent.GetEvent(out ev, out p1, out p2, 0) == 0)" EOL
	"                    {" EOL
	"                        if (ev == EventCode.Complete || ev == EventCode.UserAbort)" EOL
	"                        {" EOL
	"                            Console.WriteLine(\"Done!\");" EOL
	"                            stop = true;" EOL
	"                        }" EOL
	"                        else if (ev == EventCode.ErrorAbort)" EOL
	"                        {" EOL
	"                            Console.WriteLine(\"An error occured: HRESULT={0:X}\", p1);" EOL
	"                            pMediaControl.Stop();" EOL
	"                            stop = true;" EOL
	"                        }" EOL
	"                        pMediaEvent.FreeEventParams(ev, p1, p2);" EOL
	"                    }" EOL
	"                }" EOL
	"            }" EOL
	"            catch (COMException ex)" EOL
	"            {" EOL
	"                Console.WriteLine(\"COM error: \" + ex.ToString());" EOL
	"            }" EOL
	"            catch (Exception ex)" EOL
	"            {" EOL
	"                Console.WriteLine(\"Error: \" + ex.ToString());" EOL
	"            }" EOL
	"        }" EOL
	"    }" EOL
	"}" EOL
	EOL
};

//######################################
//
//######################################
HRESULT getGraphCodeCSharp (IFilterGraph * pGraph, string * code) {
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
		str = regex_replace(tplCSharpDefineGUID, regex("\\$guid"), guid);
		str = regex_replace(str, regex("\\$clsname"), string("CLSID_GSN_").append(szFilterNameCompact));

		code_clsids.append(str);

		//######################################
		// add filter
		//######################################
		str = regex_replace(tplCSharpCreateFilter, regex("\\$name"), szFilterName);
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
				srcFiles.push_back(tmp);

				sprintf_s(fileVarName, MAX_VAR_NAME, "srcFile%u", (unsigned int)srcFiles.size());

				str = regex_replace(tplCSharpSetSourceFile, regex("\\$srcvar"), string("p").append(szFilterNameCompact).append("_src"));
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
				dstFiles.push_back(tmp);

				sprintf_s(fileVarName, MAX_VAR_NAME, "dstFile%u", (unsigned int)dstFiles.size());

				str = regex_replace(tplCSharpSetDestFile, regex("\\$dstvar"), string("p").append(szFilterNameCompact).append("_sink"));
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
						str = regex_replace(tplCSharpConnectDirect, regex("\\$pair"), pair);

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
		str.append(", string ");
		str.append(fileVarName);
	}
	for (unsigned int i = 0; i < dstFiles.size(); i++) {
		sprintf(fileVarName, "dstFile%d", i + 1);
		str.append(", string ");
		str.append(fileVarName);
	}

	code_build_start = std::regex_replace(tplCSharpBuildGraphStart, regex("\\$args"), str);

	//######################################
	//Main
	//######################################
	std::string args;
	for (unsigned int i = 0; i<srcFiles.size(); i++) {
		args.append(", @\"").append(srcFiles[i].c_str()).append("\"");
	}
	for (unsigned int i = 0; i<dstFiles.size(); i++) {
		args.append(", @\"").append(dstFiles[i].c_str()).append("\"");
	}

	str = regex_replace(tplCSharpMain, regex("\\$args"), args);
	code_main.append(str);

	code->append(tplCSharpHeader);
	code->append(code_build_start);
	code->append(code_clsids).append(EOL);
	code->append(code_filters_add);
	code->append(code_filters_connect);
	code->append(tplCSharpBuildGraphEnd);
	code->append(code_main);

	return S_OK;
}
