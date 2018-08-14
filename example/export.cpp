//--------------------------------------
// GraphStudioNext code export for C++ and MS Visual Studio
//--------------------------------------

#include <DShow.h>
#include <atlbase.h>
#include <initguid.h>

BOOL hrcheck(HRESULT hr, const WCHAR* errtext)
{
    if (hr >= S_OK) return TRUE;
    WCHAR szErr[MAX_ERROR_TEXT_LEN];
    DWORD res = AMGetErrorText(hr, szErr, MAX_ERROR_TEXT_LEN);
    if (res)
        wprintf(L"Error %x: %ls\r\n%ls\r\n", hr, errtext, szErr);
    else
        wprintf(L"Error %x: %ls\r\n", hr, errtext);
    return FALSE;
}

#define CHECK_HR(hr, msg) if (!hrcheck(hr, msg)) return hr;

CComPtr<IPin> GetPin(IBaseFilter *pFilter, LPCOLESTR pinname)
{
    CComPtr<IEnumPins>    pEnum;
    CComPtr<IPin>         pPin;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr)) return NULL;

    while (pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_INFO pinfo;
        pPin->QueryPinInfo(&pinfo);
        BOOL found = !wcscmp(pinname, pinfo.achName);
         if (pinfo.pFilter) pinfo.pFilter->Release();
         if (found) return pPin;
    }
    printf("Pin not found!\r\n");
    return NULL;
}

// {B98D13E7-55DB-4385-A33D-09FD1BA26338}
DEFINE_GUID(CLSID_GSN_LAVSplitterSource, 0xB98D13E7, 0x55DB, 0x4385, 0xA3, 0x3D, 0x09, 0xFD, 0x1B, 0xA2, 0x63, 0x38);

// {E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}
DEFINE_GUID(CLSID_GSN_LAVAudioDecoder, 0xE8E73B6B, 0x4CB3, 0x44A4, 0xBE, 0x99, 0x4F, 0x7B, 0xCB, 0x96, 0xE4, 0x91);

// {EE30215D-164F-4A92-A4EB-9D4C13390F9F}
DEFINE_GUID(CLSID_GSN_LAVVideoDecoder, 0xEE30215D, 0x164F, 0x4A92, 0xA4, 0xEB, 0x9D, 0x4C, 0x13, 0x39, 0x0F, 0x9F);

// {79376820-07D0-11CF-A24D-0020AFD79767}
DEFINE_GUID(CLSID_GSN_DirectSoundAudioRenderer, 0x79376820, 0x07D0, 0x11CF, 0xA2, 0x4D, 0x00, 0x20, 0xAF, 0xD7, 0x97, 0x67);

// {B87BEB7B-8D29-423F-AE4D-6582C10175AC}
DEFINE_GUID(CLSID_GSN_VideoMixingRenderer, 0xB87BEB7B, 0x8D29, 0x423F, 0xAE, 0x4D, 0x65, 0x82, 0xC1, 0x01, 0x75, 0xAC);

HRESULT BuildGraph(IGraphBuilder *pGraph, LPCOLESTR srcFile1)
{
    HRESULT hr = S_OK;

    //graph builder
    CComPtr<ICaptureGraphBuilder2> pBuilder;
    hr = pBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder2);
    CHECK_HR(hr, L"Can't create Capture Graph Builder");
    hr = pBuilder->SetFiltergraph(pGraph);
    CHECK_HR(hr, L"Can't SetFiltergraph");

    //add LAV Splitter Source
    CComPtr<IBaseFilter> pLAVSplitterSource;
    hr = pLAVSplitterSource.CoCreateInstance(CLSID_GSN_LAVSplitterSource);
    CHECK_HR(hr, L"Can't create LAV Splitter Source");
    hr = pGraph->AddFilter(pLAVSplitterSource, L"LAV Splitter Source");
    CHECK_HR(hr, L"Can't add LAV Splitter Source to graph");

    //set source filename
    CComQIPtr<IFileSourceFilter, &IID_IFileSourceFilter> pLAVSplitterSource_src(pLAVSplitterSource);
    if (!pLAVSplitterSource_src)
        CHECK_HR(E_NOINTERFACE, L"Can't get IFileSourceFilter");
    hr = pLAVSplitterSource_src->Load(srcFile1, NULL);
    CHECK_HR(hr, L"Can't load file");

    //add LAV Audio Decoder
    CComPtr<IBaseFilter> pLAVAudioDecoder;
    hr = pLAVAudioDecoder.CoCreateInstance(CLSID_GSN_LAVAudioDecoder);
    CHECK_HR(hr, L"Can't create LAV Audio Decoder");
    hr = pGraph->AddFilter(pLAVAudioDecoder, L"LAV Audio Decoder");
    CHECK_HR(hr, L"Can't add LAV Audio Decoder to graph");

    //add LAV Video Decoder
    CComPtr<IBaseFilter> pLAVVideoDecoder;
    hr = pLAVVideoDecoder.CoCreateInstance(CLSID_GSN_LAVVideoDecoder);
    CHECK_HR(hr, L"Can't create LAV Video Decoder");
    hr = pGraph->AddFilter(pLAVVideoDecoder, L"LAV Video Decoder");
    CHECK_HR(hr, L"Can't add LAV Video Decoder to graph");

    //add DirectSound Audio Renderer
    CComPtr<IBaseFilter> pDirectSoundAudioRenderer;
    hr = pDirectSoundAudioRenderer.CoCreateInstance(CLSID_GSN_DirectSoundAudioRenderer);
    CHECK_HR(hr, L"Can't create DirectSound Audio Renderer");
    hr = pGraph->AddFilter(pDirectSoundAudioRenderer, L"DirectSound Audio Renderer");
    CHECK_HR(hr, L"Can't add DirectSound Audio Renderer to graph");

    //add Video Mixing Renderer
    CComPtr<IBaseFilter> pVideoMixingRenderer;
    hr = pVideoMixingRenderer.CoCreateInstance(CLSID_GSN_VideoMixingRenderer);
    CHECK_HR(hr, L"Can't create Video Mixing Renderer");
    hr = pGraph->AddFilter(pVideoMixingRenderer, L"Video Mixing Renderer");
    CHECK_HR(hr, L"Can't add Video Mixing Renderer to graph");

    //connect LAV Splitter Source and LAV Video Decoder
    hr = pGraph->ConnectDirect(GetPin(pLAVSplitterSource, L"Video"), GetPin(pLAVVideoDecoder, L"Input"), NULL);
    CHECK_HR(hr, L"Can't connect LAV Splitter Source and LAV Video Decoder");

    //connect LAV Splitter Source and LAV Audio Decoder
    hr = pGraph->ConnectDirect(GetPin(pLAVSplitterSource, L"Audio"), GetPin(pLAVAudioDecoder, L"Input"), NULL);
    CHECK_HR(hr, L"Can't connect LAV Splitter Source and LAV Audio Decoder");

    //connect LAV Audio Decoder and DirectSound Audio Renderer
    hr = pGraph->ConnectDirect(GetPin(pLAVAudioDecoder, L"Output"), GetPin(pDirectSoundAudioRenderer, L"Audio Input pin (rendered)"), NULL);
    CHECK_HR(hr, L"Can't connect LAV Audio Decoder and DirectSound Audio Renderer");

    //connect LAV Video Decoder and Video Mixing Renderer
    hr = pGraph->ConnectDirect(GetPin(pLAVVideoDecoder, L"Output"), GetPin(pVideoMixingRenderer, L"VMR Input0"), NULL);
    CHECK_HR(hr, L"Can't connect LAV Video Decoder and Video Mixing Renderer");

    return S_OK;
}

int main(int argc, char* argv[])
{
    CoInitialize(NULL);
    CComPtr<IGraphBuilder> pGraph;
    pGraph.CoCreateInstance(CLSID_FilterGraph);

    printf("Building graph...\r\n");
    HRESULT hr = BuildGraph(pGraph, L"bbb_360p_10sec.mp4");
    if (hrcheck(hr, L"Can't BuildGraph")) {
        CComQIPtr<IMediaControl, &IID_IMediaControl> pMediaControl(pGraph);
        hr = pMediaControl->Run();
        if (hrcheck(hr, L"Can't run the graph")) {
            printf("Running");
            CComQIPtr<IMediaEvent, &IID_IMediaEvent> pMediaEvent(pGraph);
            BOOL stop = FALSE;
            MSG msg;
            while (!stop)
            {
                long ev = 0, p1 = 0, p2 = 0;
                Sleep(50);
                printf(".");
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                    DispatchMessage(&msg);
                while (pMediaEvent->GetEvent(&ev, &p1, &p2, 0) == S_OK)
                {
                    if (ev == EC_COMPLETE || ev == EC_USERABORT)
                    {
                        printf("\r\nDone!\r\n");
                        stop = TRUE;
                    }
                    else if (ev == EC_ERRORABORT)
                    {
                        printf("An error occured: HRESULT=%x\r\n", p1);
                        pMediaControl->Stop();
                        stop = TRUE;
                    }
                    pMediaEvent->FreeEventParams(ev, p1, p2);
                }
            }
        }
    }
    CoUninitialize();
    return 0;
}

