//--------------------------------------
// GraphStudioNext code export for C# and DirectShowLib
//--------------------------------------

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using DirectShowLib;

namespace GraphCode
{
    class Program
    {

        static void checkHR(int hr, string msg)
        {
            if (hr < 0)
            {
                Console.WriteLine(msg);
                DsError.ThrowExceptionForHR(hr);
            }
        }

        static IPin GetPin(IBaseFilter pFilter, string pinname)
        {
            IEnumPins pEnum;
            IntPtr pPin = Marshal.AllocCoTaskMem(4);

            int hr = pFilter.EnumPins(out pEnum);
            checkHR(hr, "Can't enumerate pins");

            IPin[] pins = new IPin[1];
            while (pEnum.Next(1, pins, pPin) == 0)
            {
                PinInfo pinfo;
                pins[0].QueryPinInfo(out pinfo);
                bool found = (pinname == pinfo.name);
                DsUtils.FreePinInfo(pinfo);
                if (found) return pins[0];
            }
            checkHR(-1, "Pin not found");
            return null;
        }


        static void BuildGraph(IGraphBuilder pGraph, string srcFile1)
        {
            int hr = 0;

            //graph builder
            ICaptureGraphBuilder2 pBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();
            hr = pBuilder.SetFiltergraph(pGraph);
            checkHR(hr, "Can't SetFiltergraph");

            Guid CLSID_GSN_LAVSplitterSource = new Guid("{B98D13E7-55DB-4385-A33D-09FD1BA26338}");
            Guid CLSID_GSN_LAVAudioDecoder = new Guid("{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}");
            Guid CLSID_GSN_LAVVideoDecoder = new Guid("{EE30215D-164F-4A92-A4EB-9D4C13390F9F}");
            Guid CLSID_GSN_DirectSoundAudioRenderer = new Guid("{79376820-07D0-11CF-A24D-0020AFD79767}");
            Guid CLSID_GSN_VideoMixingRenderer = new Guid("{B87BEB7B-8D29-423F-AE4D-6582C10175AC}");

            //add LAV Splitter Source
            IBaseFilter pLAVSplitterSource = (IBaseFilter)Activator.CreateInstance(Type.GetTypeFromCLSID(CLSID_GSN_LAVSplitterSource));
            hr = pGraph.AddFilter(pLAVSplitterSource, "LAV Splitter Source");
            checkHR(hr, "Can't add LAV Splitter Source to graph");

            //set source filename
            IFileSourceFilter pLAVSplitterSource_src = pLAVSplitterSource as IFileSourceFilter;
            if (pLAVSplitterSource_src == null)
                checkHR(unchecked((int)0x80004002), "Can't get IFileSourceFilter");

            hr = pLAVSplitterSource_src.Load(srcFile1, null);
            checkHR(hr, "Can't load file");

            //add LAV Audio Decoder
            IBaseFilter pLAVAudioDecoder = (IBaseFilter)Activator.CreateInstance(Type.GetTypeFromCLSID(CLSID_GSN_LAVAudioDecoder));
            hr = pGraph.AddFilter(pLAVAudioDecoder, "LAV Audio Decoder");
            checkHR(hr, "Can't add LAV Audio Decoder to graph");

            //add LAV Video Decoder
            IBaseFilter pLAVVideoDecoder = (IBaseFilter)Activator.CreateInstance(Type.GetTypeFromCLSID(CLSID_GSN_LAVVideoDecoder));
            hr = pGraph.AddFilter(pLAVVideoDecoder, "LAV Video Decoder");
            checkHR(hr, "Can't add LAV Video Decoder to graph");

            //add DirectSound Audio Renderer
            IBaseFilter pDirectSoundAudioRenderer = (IBaseFilter)Activator.CreateInstance(Type.GetTypeFromCLSID(CLSID_GSN_DirectSoundAudioRenderer));
            hr = pGraph.AddFilter(pDirectSoundAudioRenderer, "DirectSound Audio Renderer");
            checkHR(hr, "Can't add DirectSound Audio Renderer to graph");

            //add Video Mixing Renderer
            IBaseFilter pVideoMixingRenderer = (IBaseFilter)Activator.CreateInstance(Type.GetTypeFromCLSID(CLSID_GSN_VideoMixingRenderer));
            hr = pGraph.AddFilter(pVideoMixingRenderer, "Video Mixing Renderer");
            checkHR(hr, "Can't add Video Mixing Renderer to graph");

            //connect LAV Splitter Source and LAV Video Decoder
            hr = pGraph.ConnectDirect(GetPin(pLAVSplitterSource, "Video"), GetPin(pLAVVideoDecoder, "Input"), null);
            checkHR(hr, "Can't connect LAV Splitter Source and LAV Video Decoder");

            //connect LAV Splitter Source and LAV Audio Decoder
            hr = pGraph.ConnectDirect(GetPin(pLAVSplitterSource, "Audio"), GetPin(pLAVAudioDecoder, "Input"), null);
            checkHR(hr, "Can't connect LAV Splitter Source and LAV Audio Decoder");

            //connect LAV Audio Decoder and DirectSound Audio Renderer
            hr = pGraph.ConnectDirect(GetPin(pLAVAudioDecoder, "Output"), GetPin(pDirectSoundAudioRenderer, "Audio Input pin (rendered)"), null);
            checkHR(hr, "Can't connect LAV Audio Decoder and DirectSound Audio Renderer");

            //connect LAV Video Decoder and Video Mixing Renderer
            hr = pGraph.ConnectDirect(GetPin(pLAVVideoDecoder, "Output"), GetPin(pVideoMixingRenderer, "VMR Input0"), null);
            checkHR(hr, "Can't connect LAV Video Decoder and Video Mixing Renderer");

        }

        static void Main(string[] args)
        {
            try
            {
                IGraphBuilder pGraph = (IGraphBuilder)new FilterGraph();
                Console.WriteLine("Building graph...");

                BuildGraph(pGraph, @"bbb_360p_10sec.mp4");
                Console.WriteLine("Running...");

                IMediaControl pMediaControl = (IMediaControl)pGraph;
                IMediaEvent pMediaEvent = (IMediaEvent)pGraph;

                int hr = pMediaControl.Run();
                checkHR(hr, "Can't run the graph");

                bool stop = false;
                while (!stop)
                {
                    System.Threading.Thread.Sleep(50);
                    Console.Write(".");
                    EventCode ev;
                    IntPtr p1, p2;
                    System.Windows.Forms.Application.DoEvents();
                    while (pMediaEvent.GetEvent(out ev, out p1, out p2, 0) == 0)
                    {
                        if (ev == EventCode.Complete || ev == EventCode.UserAbort)
                        {
                            Console.WriteLine("Done!");
                            stop = true;
                        }
                        else if (ev == EventCode.ErrorAbort)
                        {
                            Console.WriteLine("An error occured: HRESULT={0:X}", p1);
                            pMediaControl.Stop();
                            stop = true;
                        }
                        pMediaEvent.FreeEventParams(ev, p1, p2);
                    }
                }
            }
            catch (COMException ex)
            {
                Console.WriteLine("COM error: " + ex.ToString());
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: " + ex.ToString());
            }
        }
    }
}

