#pragma once

HRESULT getGraphCodeBAT(IFilterGraph * pGraph, std::string * code);
HRESULT getGraphCodeCPP(IFilterGraph * pGraph, std::string * code);
HRESULT getGraphCodeCSharp(IFilterGraph * pGraph, std::string * code);
HRESULT getGraphCodeLua(IFilterGraph * pGraph, std::string * code);
HRESULT getGraphCodePython(IFilterGraph * pGraph, std::string * code);
