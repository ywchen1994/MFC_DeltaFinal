
// MFC_DeltaFinal.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CMFC_DeltaFinalApp: 
// �аѾ\��@�����O�� MFC_DeltaFinal.cpp
//

class CMFC_DeltaFinalApp : public CWinApp
{
public:
	CMFC_DeltaFinalApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CMFC_DeltaFinalApp theApp;