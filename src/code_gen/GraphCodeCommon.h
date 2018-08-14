#pragma once

#define GUID_LEN 39
#define XGUID_LEN 75
#define MAX_VAR_NAME 12

void getFilterName(IBaseFilter * pFilter, WCHAR * wFilterName);
void guid_to_string(GUID clsid, char * guidStr);
void guid_to_xguid(GUID clsid, char * guidStr);
void removeDisallowed(char* source);
void escapeBackSlashes(std::string &s);
