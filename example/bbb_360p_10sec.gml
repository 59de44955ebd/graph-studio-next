<?xml version="1.0" encoding="utf-8"?>
<graph name="Unnamed Graph">
    <filter name="Default DirectSound Device" index="0" clsid="{79376820-07D0-11CF-A24D-0020AFD79767}" clock="1">
        <ipersiststream encoding="base64" data="MAAwADAAMAAwADAAMAAwADAAMAAwACAAAAAAAAAAAAAAAAAAAAAAAA=="/>
    </filter>
    <filter name="Video Renderer" index="1" clsid="{B87BEB7B-8D29-423F-AE4D-6582C10175AC}">
        <ipersiststream encoding="base64" data="MAAwADAAMAAwADAAMAAwADAAMAAwACAAiAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA="/>
    </filter>
    <filter name="LAV Audio Decoder" index="2" clsid="{E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491}"/>
    <filter name="LAV Video Decoder" index="3" clsid="{EE30215D-164F-4A92-A4EB-9D4C13390F9F}"/>
    <filter name="F:\TMP\graph-studio-next\Win32\Release\äöü.mp4" index="4" clsid="{B98D13E7-55DB-4385-A33D-09FD1BA26338}">
        <ifilesourcefilter source="bbb_360p_10sec.mp4"/>
    </filter>
    <connect out="F:\TMP\graph-studio-next\Win32\Release\bbb_360p_10sec.mp4/Video" in="LAV Video Decoder/Input" outFilterIndex="4" outPinId="Video" outPinName="Video" outPinIndex="0" inFilterIndex="3" inPinId="In" inPinName="Input" inPinIndex="0" direct="true">
        <mediaType type="MEDIATYPE_Video / MEDIASUBTYPE_AVC1" sampleSize="1" fixedSizeSamples="false" temporalCompression="true" majorType="{73646976-0000-0010-8000-00AA00389B71}" subType="{31435641-0000-0010-8000-00AA00389B71}"/>
        <format type="FORMAT_MPEG2_VIDEO" formatType="{E06D80E3-DB46-11CF-B4D1-00805F6CBBEA}" encoding="base64" data="AAAAAAAAAACAAgAAaAEAAAAAAAAAAAAAgAIAAGgBAAAixgcAAAAAAJtbBgAAAAAAAAAAAAAAAAAQAAAACQAAAAAAAAAAAAAAKAAAAIACAABoAQAAAQAYAEFWQzEAjAoAAAAAAAAAAAAAAAAAAAAAAAAAAAAdAAAAQgAAAB4AAAAEAAAAABUnQuAeqRgUBf8uANQYBBrbCte98BAABCjeCcg="/>
    </connect>
    <connect out="F:\TMP\graph-studio-next\Win32\Release\bbb_360p_10sec.mp4/Audio" in="LAV Audio Decoder/Input" outFilterIndex="4" outPinId="Audio" outPinName="Audio" outPinIndex="1" inFilterIndex="2" inPinId="In" inPinName="Input" inPinIndex="0" direct="true">
        <mediaType type="MEDIATYPE_Audio / MEDIASUBTYPE_ADTS" sampleSize="256000" fixedSizeSamples="true" temporalCompression="false" majorType="{73647561-0000-0010-8000-00AA00389B71}" subType="{53544441-0000-0010-8000-00AA00389B71}"/>
        <format type="FORMAT_WaveFormatEx" formatType="{05589F81-C356-11CE-BF01-00AA0055595A}" encoding="base64" data="QUQCACJWAAACIwAAAQAAAAIAE5A="/>
    </connect>
    <connect out="LAV Audio Decoder/Output" in="Default DirectSound Device/Audio Input pin (rendered)" outFilterIndex="2" outPinId="Out" outPinName="Output" outPinIndex="0" inFilterIndex="0" inPinId="Audio Input pin (rendered)" inPinName="Audio Input pin (rendered)" inPinIndex="0" direct="true">
        <mediaType type="MEDIATYPE_Audio / MEDIASUBTYPE_IEEE_FLOAT" sampleSize="8" fixedSizeSamples="true" temporalCompression="false" majorType="{73647561-0000-0010-8000-00AA00389B71}" subType="{00000003-0000-0010-8000-00AA00389B71}"/>
        <format type="FORMAT_WaveFormatEx" formatType="{05589F81-C356-11CE-BF01-00AA0055595A}" encoding="base64" data="AwACACJWAAAQsQIACAAgAAAA"/>
    </connect>
    <connect out="LAV Video Decoder/Output" in="Video Renderer/VMR Input0" outFilterIndex="3" outPinId="Out" outPinName="Output" outPinIndex="0" inFilterIndex="1" inPinId="VMR Input0" inPinName="VMR Input0" inPinIndex="0" direct="true">
        <mediaType type="MEDIATYPE_Video / MEDIASUBTYPE_NV12" sampleSize="345600" fixedSizeSamples="true" temporalCompression="false" majorType="{73646976-0000-0010-8000-00AA00389B71}" subType="{3231564E-0000-0010-8000-00AA00389B71}"/>
        <format type="FORMAT_VIDEOINFO2" formatType="{F72A76A0-EB0A-11D0-ACE4-0000C0CC16BA}" encoding="base64" data="AAAAAAAAAACAAgAAaAEAAAAAAAAAAAAAgAIAAGgBAAAAAAAAAAAAAJtbBgAAAAAAAAAAAAAAAAAQAAAACQAAAAAAAAAAAAAAKAAAAIACAABoAQAAAQAMAE5WMTIARgUAAAAAAAAAAAAAAAAAAAAAAA=="/>
    </connect>
</graph>
