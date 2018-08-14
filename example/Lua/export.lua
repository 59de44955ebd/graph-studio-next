----------------------------------------
-- GraphStudioNext Export for LuaJIT and extension 'directshow'
----------------------------------------

local directshow = require('directshow')
local ffi = require('ffi')
local winapi = require('winapi')
require('winapi.windowclass')

local function check_hr (hr, msg)
    if hr<0 then
        directshow.forget()
        print(msg..'\n\n(Hit the <Enter> key to quit)\n')
        io.read()
        os.exit()
    end
end

-- Create a window
local win = winapi.Window{
    title = 'GraphStudioNext Export',
    autoquit = true,
    visible = false
}

function win:on_resized()
    --Resize video to window
    directshow.set_rect(0, 0, win.client_w, win.client_h)
end

-- Define filter CLSIDs
local CLSID_LAVSplitterSource = '{B98D13E7-55DB-4385-A33D-09FD1BA26338}'
local CLSID_LAVVideoDecoder = '{EE30215D-164F-4A92-A4EB-9D4C13390F9F}'
local CLSID_LAVAudioDecoder = '{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}'
local CLSID_VideoMixingRenderer = '{B87BEB7B-8D29-423F-AE4D-6582C10175AC}'
local CLSID_DirectSoundAudioRenderer = '{79376820-07D0-11CF-A24D-0020AFD79767}'
local hr


-- Initialize DirectShow
directshow.init()

-- Create filter graph
directshow.graph_start()

-- Add filters to graph
hr = directshow.graph_add_source('bbb_360p_10sec.mp4', CLSID_LAVSplitterSource)
check_hr(hr, 'Can\'t add LAV Splitter Source to graph')

hr = directshow.graph_add_filter(CLSID_LAVVideoDecoder)
check_hr(hr, 'Can\'t add LAV Video Decoder to graph')

hr = directshow.graph_add_filter(CLSID_LAVAudioDecoder)
check_hr(hr, 'Can\'t add LAV Audio Decoder to graph')

hr = directshow.graph_add_filter(CLSID_VideoMixingRenderer)
check_hr(hr, 'Can\'t add Video Mixing Renderer to graph')

hr = directshow.graph_add_filter(CLSID_DirectSoundAudioRenderer)
check_hr(hr, 'Can\'t add DirectSound Audio Renderer to graph')

-- Connect LAV Splitter Source and LAV Video Decoder
hr = directshow.graph_connect(1, 2)
check_hr(hr, 'Can\'t connect LAV Splitter Source and LAV Video Decoder')

-- Connect LAV Splitter Source and LAV Audio Decoder
hr = directshow.graph_connect(1, 3)
check_hr(hr, 'Can\'t connect LAV Splitter Source and LAV Audio Decoder')

-- Connect LAV Video Decoder and Video Mixing Renderer
hr = directshow.graph_connect(2, 4)
check_hr(hr, 'Can\'t connect LAV Video Decoder and Video Mixing Renderer')

-- Connect LAV Audio Decoder and DirectSound Audio Renderer
hr = directshow.graph_connect(3, 5)
check_hr(hr, 'Can\'t connect LAV Audio Decoder and DirectSound Audio Renderer')

-- Finalize graph, passing options
directshow.graph_finalize({
    windowstyle = winapi.WS_CHILD,
    hwnd=tonumber(ffi.cast('int32_t', win.hwnd)),
    keepaspectratio=true,
    paused=true
})

-- Timer to check for DirectShow events
win:settimer(0.1, function ()
    evt, evtName, p1, p2 = directshow.get_event()
    if evt~=nil then 
        --print(evtName, p1, p2)
        if evtName=='EC_COMPLETE' then
            win:close()
        end
    end
end,
1)

-- Show the window
win:show()

-- Start playing
directshow.play()

-- Start window message loop
winapi.MessageLoop()

directshow.forget()
