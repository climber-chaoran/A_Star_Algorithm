
// A_Star.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CA_StarApp:
// See A_Star.cpp for the implementation of this class
//

class CA_StarApp : public CWinAppEx
{
public:
	CA_StarApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CA_StarApp theApp;