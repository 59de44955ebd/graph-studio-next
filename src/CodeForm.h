//-----------------------------------------------------------------------------
//
//	GraphStudioNext
//
//	Author : Valentin Schmidt
//
//-----------------------------------------------------------------------------
#pragma once
#include "afxcmn.h"


//-----------------------------------------------------------------------------
//
//	CCodeForm class
//
//-----------------------------------------------------------------------------
class CCodeForm : public CGraphStudioModelessDialog
	
{
protected:
	DECLARE_DYNAMIC(CCodeForm)
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	GraphStudio::TitleBar	title;
    CButton         btn_copy;
    CButton         btn_save;
	CComboBox		combo_lang;
    CEdit			edit_code;
    CFont			font_code;
    
public:
	CCodeForm(CWnd* pParent = NULL);
		
	virtual ~CCodeForm();
	virtual CRect GetDefaultRect() const;
	
	// Dialog Data
	enum { IDD = IDD_DIALOG_CODE };

    BOOL DoCreateDialog(CWnd* parent);
    void OnInitialize();
    	
	void OnSize(UINT nType, int cx, int cy);

	void OnRefresh();
	
	afx_msg void OnClickedButtonCopy();
    afx_msg void OnClickedButtonSave();
};
