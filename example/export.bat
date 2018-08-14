::--------------------------------------
:: GraphStudioNext code export for DScmd
::--------------------------------------

@echo off
chcp 65001>nul

set CLSID_LAVSplitterSource={B98D13E7-55DB-4385-A33D-09FD1BA26338}
set CLSID_LAVAudioDecoder={E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}
set CLSID_LAVVideoDecoder={EE30215D-164F-4A92-A4EB-9D4C13390F9F}
set CLSID_DirectSoundAudioRenderer={79376820-07D0-11CF-A24D-0020AFD79767}
set CLSID_VideoMixingRenderer={6BC1CFFA-8FC1-4261-AC22-CFB4CC38DB50}

DScmd.exe ^
-graph ^
%CLSID_LAVSplitterSource%;src=bbb_360p_10sec.mp4,^
%CLSID_LAVAudioDecoder%,^
%CLSID_LAVVideoDecoder%,^
%CLSID_DirectSoundAudioRenderer%,^
%CLSID_VideoMixingRenderer%^
!0:2,0:1,1:3,2:4 ^
-winCaption "GraphStudioNext Export" ^
-i