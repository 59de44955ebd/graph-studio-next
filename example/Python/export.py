#---------------------------------------
# GraphStudioNext code export for Python
# --------------------------------------

import ctypes
from ctypes import *
from ctypes.wintypes import *
from comtypes import client
from comtypes.gen.DirectShowLib import *
from win32 import constants as c

client.GetModule('.\DirectShow.tlb')
client.GetModule('qedit.dll') # DexterLib
quartz = client.GetModule('quartz.dll')

WNDPROCTYPE = WINFUNCTYPE(c_int, HWND, c_uint, WPARAM, LPARAM)

EC_COMPLETE = 1
EC_USERABORT = 2
EC_ERRORABORT = 3

class WNDCLASSEX(Structure):
    _fields_ = [('cbSize', c_uint),
                ('style', c_uint),
                ('lpfnWndProc', WNDPROCTYPE),
                ('cbClsExtra', c_int),
                ('cbWndExtra', c_int),
                ('hInstance', HANDLE),
                ('hIcon', HANDLE),
                ('hCursor', HANDLE),
                ('hBrush', HANDLE),
                ('lpszMenuName', LPCWSTR),
                ('lpszClassName', LPCWSTR),
                ('hIconSm', HANDLE)]

def WindowProc (hWnd, msg, wParam, lParam):

	if msg == c.WM_SIZE:
		# Resize video to window
		rect = RECT()
		rectPtr = pointer(rect)
		windll.user32.GetClientRect(hWnd, rectPtr)
		videoWindow.SetWindowPosition(0, 0, rect.right, rect.bottom)

	elif msg == c.WM_TIMER:
		while True:
			try:
				ev,p1,p2 = mediaEvent.GetEvent(0)
				if ev == EC_COMPLETE or ev == EC_USERABORT:
					print('Done.')
					windll.user32.PostQuitMessage(0)
				elif ev == EC_ERRORABORT:
					print('An error occured: HRESULT=%x' % p1)
					mediaControl.Stop()
					windll.user32.PostQuitMessage(0)
			except:
				break

	elif msg == c.WM_DESTROY:
		windll.user32.PostQuitMessage(0)

	else:
		return windll.user32.DefWindowProcW(hWnd, msg, wParam, lParam)

	return 0

def GetPin (filter, pinName):
	enum = filter.EnumPins()
	while True:
		pin , fetched = enum.Next(1)
		if fetched:
			pinInfo = pin.QueryPinInfo()
			if pinName in ''.join(map(chr, pinInfo.achName)):
				return pin
		else:
			pin.Release()
			break
	print("Pin '%s' not found!" % pinName)
	return None

# define CLSIDs
CLSID_FilterGraph = '{E436EBB3-524F-11CE-9F53-0020AF0BA770}'
CLSID_LAVSplitterSource = '{B98D13E7-55DB-4385-A33D-09FD1BA26338}'
CLSID_ffdshowVideoDecoder = '{04FE9017-F873-410E-871E-AB91661A4EF7}'
CLSID_LAVAudioDecoder = '{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}'
CLSID_VideoMixingRenderer = '{B87BEB7B-8D29-423F-AE4D-6582C10175AC}'
CLSID_DirectSoundAudioRenderer = '{79376820-07D0-11CF-A24D-0020AFD79767}'

def BuildGraph (graph, srcFile1):

    # Add LAV Splitter Source
    pLAVSplitterSource = client.CreateObject(CLSID_LAVSplitterSource, interface = IBaseFilter)
    graph.AddFilter(pLAVSplitterSource, 'LAV Splitter Source')

    # Set source filename
    pLAVSplitterSource_src = pLAVSplitterSource.QueryInterface(IFileSourceFilter)
    pLAVSplitterSource_src.Load(srcFile1, None)

    # Add ffdshow Video Decoder
    pffdshowVideoDecoder = client.CreateObject(CLSID_ffdshowVideoDecoder, interface = IBaseFilter)
    graph.AddFilter(pffdshowVideoDecoder, 'ffdshow Video Decoder')

    # Add LAV Audio Decoder
    pLAVAudioDecoder = client.CreateObject(CLSID_LAVAudioDecoder, interface = IBaseFilter)
    graph.AddFilter(pLAVAudioDecoder, 'LAV Audio Decoder')

    # Add Video Mixing Renderer
    pVideoMixingRenderer = client.CreateObject(CLSID_VideoMixingRenderer, interface = IBaseFilter)
    graph.AddFilter(pVideoMixingRenderer, 'Video Mixing Renderer')

    # Add DirectSound Audio Renderer
    pDirectSoundAudioRenderer = client.CreateObject(CLSID_DirectSoundAudioRenderer, interface = IBaseFilter)
    graph.AddFilter(pDirectSoundAudioRenderer, 'DirectSound Audio Renderer')

    # Connect LAV Splitter Source and ffdshow Video Decoder
    graph.ConnectDirect(GetPin(pLAVSplitterSource, 'Video'), GetPin(pffdshowVideoDecoder, 'In'), None)

    # Connect LAV Splitter Source and LAV Audio Decoder
    graph.ConnectDirect(GetPin(pLAVSplitterSource, 'Audio'), GetPin(pLAVAudioDecoder, 'Input'), None)

    # Connect ffdshow Video Decoder and Video Mixing Renderer
    graph.ConnectDirect(GetPin(pffdshowVideoDecoder, 'Out'), GetPin(pVideoMixingRenderer, 'VMR Input0'), None)

    # Connect LAV Audio Decoder and DirectSound Audio Renderer
    graph.ConnectDirect(GetPin(pLAVAudioDecoder, 'Output'), GetPin(pDirectSoundAudioRenderer, 'Audio Input pin (rendered)'), None)

# Create a window
hInst = windll.kernel32.GetModuleHandleA(0)
wclassName = 'GraphStudioNextExport'
wname = 'GraphStudioNext Export'

wndClass = WNDCLASSEX()
wndClass.cbSize = sizeof(WNDCLASSEX)
wndClass.style = c.CS_HREDRAW | c.CS_VREDRAW
wndClass.lpfnWndProc = WNDPROCTYPE(WindowProc)
wndClass.hInstance = hInst
wndClass.hBrush = windll.gdi32.GetStockObject(c.BLACK_BRUSH)
wndClass.lpszClassName = wclassName

windll.user32.RegisterClassExA(byref(wndClass))

hWnd = windll.user32.CreateWindowExA(0,wclassName,wname,
		c.WS_OVERLAPPEDWINDOW | c.WS_CAPTION,
		c.CW_USEDEFAULT, c.CW_USEDEFAULT, c.CW_USEDEFAULT, c.CW_USEDEFAULT,
		0,0,hInst,0)

if not hWnd:
	print('Failed to create window')
	exit(0)

graph = client.CreateObject(CLSID_FilterGraph, interface=IFilterGraph)

print('Building graph...')
BuildGraph(graph, 'F:\\TMP\\graph-studio-next-enhanced\\example\\Python\\bbb_360p_10sec.mp4')

mediaControl = graph.QueryInterface(quartz.IMediaControl)
mediaEvent = graph.QueryInterface(quartz.IMediaEvent)

videoWindow = graph.QueryInterface(quartz.IVideoWindow)
videoWindow.Owner = hWnd
videoWindow.WindowStyle = c.WS_CHILD|c.WS_CLIPCHILDREN|c.WS_CLIPSIBLINGS

# Create a timer to check for DirectShow events
windll.user32.SetTimer(hWnd, 1, 50, None)

# Show the window
windll.user32.ShowWindow(hWnd, c.SW_SHOW)
windll.user32.UpdateWindow(hWnd)

# Run the graph
mediaControl.Run()
print('Running...');

# Start window message loop
msg = MSG()
lpmsg = pointer(msg)
while windll.user32.GetMessageA(lpmsg, 0, 0, 0) != 0:
	windll.user32.TranslateMessage(lpmsg)
	windll.user32.DispatchMessageA(lpmsg)

# Clean up
del mediaControl
del mediaEvent
del videoWindow
del graph
