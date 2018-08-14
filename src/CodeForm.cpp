//-----------------------------------------------------------------------------
//
//	GraphStudioNext
//
//	Author : Valentin Schmidt
//
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "code_gen/GraphCode.h"

//-----------------------------------------------------------------------------
//
//	CCodeForm class
//
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CCodeForm, CGraphStudioModelessDialog)
BEGIN_MESSAGE_MAP(CCodeForm, CGraphStudioModelessDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_COPYTEXT, &CCodeForm::OnClickedButtonCopy)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CCodeForm::OnClickedButtonSave)
    ON_CBN_SELCHANGE(IDC_COMBO_LANG, &CCodeForm::OnRefresh)
END_MESSAGE_MAP()

LPCTSTR	LangNames[] =
{
	_T("BAT"),
	_T("C++"),
	_T("C#"),
	_T("Lua")
};
int LangNamesCount = sizeof(LangNames)/sizeof(LangNames[0]);

//-----------------------------------------------------------------------------
//
//	CCodeForm class
//
//-----------------------------------------------------------------------------

CCodeForm::CCodeForm(CWnd* pParent)	:
	CGraphStudioModelessDialog(CCodeForm::IDD, pParent)
{	
}

CCodeForm::~CCodeForm()
{
}

void CCodeForm::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TITLEBAR, title);
	DDX_Control(pDX, IDC_EDIT_CODE, edit_code);
}

void CCodeForm::OnSize(UINT nType, int cx, int cy)
{
	// resize our controls along...
	CRect		rc, rc2;
	GetClientRect(&rc);
	
	if (IsWindow(edit_code)) {
        title.GetClientRect(&rc2);
        title.SetWindowPos(NULL, 0, 0, rc.Width(), rc2.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);

        edit_code.SetWindowPos(NULL, 0, rc2.Height(), rc.Width(), rc.Height() - rc2.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);

		combo_lang.GetWindowRect(&rc2);
        combo_lang.SetWindowPos(NULL, rc.Width()-4-rc2.Width(), 5, rc2.Width(), rc2.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);

        title.Invalidate();
        edit_code.Invalidate();
	}
}

BOOL CCodeForm::DoCreateDialog(CWnd* parent)
{
	BOOL ret = Create(IDD, parent);
	if (!ret) return FALSE;

    // prepare titlebar
	title.ModifyStyle(0, WS_CLIPCHILDREN);
	title.ModifyStyleEx(0, WS_EX_CONTROLPARENT);

    CRect	rc;
	rc.SetRect(0, 0, 60, 23);
    btn_copy.Create(_T("&Copy"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, rc, &title, IDC_BUTTON_COPYTEXT);
    btn_copy.SetWindowPos(NULL, 4, 4, rc.Width(), rc.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);
    btn_copy.SetFont(GetFont());

    btn_save.Create(_T("&Save"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, rc, &title, IDC_BUTTON_SAVE);
    btn_save.SetWindowPos(NULL, 8 + rc.Width(), 4, rc.Width(), rc.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);
    btn_save.SetFont(GetFont());

    rc.SetRect(0, 0, 150, 23);
    combo_lang.Create(WS_TABSTOP | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, rc, &title, IDC_COMBO_LANG);
    combo_lang.SetFont(GetFont());

	// Force a second resize to give the combo box a chance to position itself once it's fully created
	// Would not be an issue if the setup above was done in OnInitDialog...
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE);		// resize down to zero
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER);		// resize down to zero
	RestorePosition();								// then restore position or set default position

	OnInitialize();

	return TRUE;
};

void CCodeForm::OnInitialize()
{
	if(GraphStudio::HasFont(_T("Consolas")))
        GraphStudio::MakeFont(font_code, _T("Consolas"), 10, false, false);
    else
        GraphStudio::MakeFont(font_code, _T("Courier New"), 10, false, false);
	edit_code.SetFont(&font_code);

	combo_lang.ResetContent();
	for (int i=0; i<LangNamesCount; i++) {
		combo_lang.AddString(LangNames[i]);
	}
	combo_lang.SetCurSel(1); // CPP LangNamesCount - 1
}

CRect CCodeForm::GetDefaultRect() const 
{
	return CRect(50, 200, 650, 600);
}

// CCodeForm message handlers

void CCodeForm::OnRefresh()
{

	HRESULT hr = E_FAIL;
	std::string code("");
		
	// get lang index
	int lang = combo_lang.GetCurSel();
	switch (lang){
		case 0:
			hr = getGraphCodeBAT(view->graph.gb, &code);
			break;
		case 1:
			hr = getGraphCodeCPP(view->graph.gb, &code);
			break;
		case 2:
			hr = getGraphCodeCSharp(view->graph.gb, &code);
			break;
		case 3:
			hr = getGraphCodeLUA(view->graph.gb, &code);
			break;
	}
	
	//if (SUCCEEDED(hr)){
		CA2W w_code(code.c_str());
		edit_code.SetWindowText(w_code);
	//}

//	// gnerate the report
//	CGraphReportGenerator graphReportGenerator(&view->graph, view->render_params.use_media_info);
//	CString report = graphReportGenerator.GetReport(level);
//
//	edit_report.SetWindowText(report);
}

void CCodeForm::OnClickedButtonCopy()
{
	// copy the content to the clipboard
	CString		text;

	edit_code.GetWindowText(text);

    DSUtil::SetClipboardText(this->GetSafeHwnd(), text);
}

void CCodeForm::OnClickedButtonSave()
{
	CString	filter;
	CString	ext;
	CString	filename;

	int lang = combo_lang.GetCurSel();
	switch (lang){
		case 0:
			ext = _T(".bat");
			filter = _T("BAT Files (*.bat,*.cmd)|*.bat;*.cmd|All Files (*.*)|*.*|");
			break;
		case 1:
			ext = _T(".cpp");
			filter = _T("C++ Files (*.cpp)|*.cpp|All Files (*.*)|*.*|");
			break;
		case 2:
			ext = _T(".cs");
			filter = _T("C# Files (*.cs)|*.cs|All Files (*.*)|*.*|");
			break;
		case 3:
			ext = _T(".lua");
			filter = _T("Lua Files (*.lua)|*.lua|All Files (*.*)|*.*|");
			break;
	}

	CFileDialog dlg(FALSE,_T("Code"),NULL,OFN_OVERWRITEPROMPT|OFN_ENABLESIZING|OFN_PATHMUSTEXIST,filter);
    INT_PTR ret = dlg.DoModal();

	filename = dlg.GetPathName();
	if (ret == IDOK)
    {
		CPath path(filename);
		if (path.GetExtension() == _T(""))
        {
			path.AddExtension(ext);
			filename = CString(path);
		}

        CFile file(filename, CFile::modeCreate|CFile::modeWrite);
        
        CString	text;
        edit_code.GetWindowText(text);
        CT2CA outputText(text, CP_UTF8);
        file.Write(outputText, (DWORD) ::strlen(outputText));
    }
}
