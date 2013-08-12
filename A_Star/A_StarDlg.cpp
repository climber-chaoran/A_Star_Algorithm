
// A_StarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "A_Star.h"
#include "A_StarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CA_StarDlg dialog




CA_StarDlg::CA_StarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CA_StarDlg::IDD, pParent) 

{
    m_brush_background  = (RGB(255, 255, 255));
    m_brush_barrier     = (RGB(1, 0, 0));
    m_brush_start       = (RGB(255, 0, 0));
    m_brush_end         = (RGB(1, 255, 0));

    m_brush_open        = (RGB(0, 0, 255));
    m_brush_close       = (RGB(254, 255, 0));
    m_brush_path        = (RGB(254, 0, 255));

    m_bFirstShowMap     = true;
    m_bPause            = false;

	m_hIcon             = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CA_StarDlg::~CA_StarDlg()
{
    DeleteObject(m_hdc);
}

void CA_StarDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PICTURE, m_picCtrl);
}

BEGIN_MESSAGE_MAP(CA_StarDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    
    ON_BN_CLICKED(IDC_BUTTON2, &CA_StarDlg::OnClickedButton)
    ON_STN_CLICKED(IDC_PICTURE, &CA_StarDlg::OnClickedPicture)
    ON_BN_CLICKED(BTN_PAUSE, &CA_StarDlg::OnClickedPause)
END_MESSAGE_MAP()


// CA_StarDlg message handlers


int CallBack(void *p)
{
    if (p == NULL)
        return 0;

    CA_StarDlg *pDlg = (CA_StarDlg*)p;
    pDlg->FindCallback();
    return 1;
}


BOOL CA_StarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    m_nBaseX = 20;
    m_nBaseY = 20;
    m_nWidth = 600;
    m_nHeight = 600;

    m_mapinfo.nRow = 15;
    m_mapinfo.nColumn = 15;

    m_deltaX = m_nWidth / m_mapinfo.nColumn;
    m_deltaY = m_nHeight / m_mapinfo.nRow;
    m_pdc = m_picCtrl.GetDC();
    m_hdc = CreateCompatibleDC(m_pdc->GetSafeHdc());
    HBITMAP hbmScreen = CreateCompatibleBitmap(m_pdc->GetSafeHdc(), m_nWidth, m_nHeight);
    SelectObject(m_hdc, hbmScreen);
    DeleteObject(hbmScreen);

    NODE node[] = {NODE(4, 5), NODE(5, 5), NODE(6, 5), NODE(7, 5), NODE(8, 5),
                   NODE(4, 11), NODE(5, 11), NODE(6, 11), NODE(7, 11), NODE(8, 11),
                   NODE(6, 6), NODE(6, 7), NODE(6, 8), NODE(6, 9), NODE(6, 10) };
    int nBlockSize = sizeof(node)/sizeof(node[0]);
    for (int i = 0; i < nBlockSize; i++)
    {
        m_mapinfo.vecBlock.push_back(node[i]);
    }

    m_mapinfo.start = NODE(0,8);
    m_mapinfo.end = NODE(14,8);

    ShowMap();

    ((CButton*)(GetDlgItem(RAD_BLOCK)))->SetCheck(BST_CHECKED);

    CString cs;
    cs += L"f值在左上角；g值在左下角；h值在右下角。\r\n\r\n";
    cs += L"红色方块为起点；\r\n\r\n";
    cs += L"绿色方块为终点；\r\n\r\n";
    cs += L"蓝色方块为OPEN队列中的点；\r\n\r\n";
    cs += L"黄色方块为CLOSE队列中的点；\r\n\r\n";
    cs += L"紫色方块为最短路径！\r\n\r\n\r\n";
    cs += L"可先在下面选择要设置的方块，然后点击左边图形中的方块来进行设置。\r\n\rn";
    GetDlgItem(CTL_DES)->SetWindowText(cs);

    ::SetWindowPos(m_picCtrl.GetSafeHwnd(), HWND_NOTOPMOST, 
        m_nBaseX, m_nBaseY, m_nWidth-1, m_nHeight-1, SWP_NOZORDER);

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CA_StarDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        ShowMap();
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CA_StarDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CA_StarDlg::PreTranslateMessage( MSG* pMsg )
{
    if (WM_KEYDOWN== pMsg->message && pMsg->wParam == VK_ESCAPE)
    {
        return FALSE;
    }
    return   CDialog::PreTranslateMessage(pMsg);  
}

unsigned int __stdcall FindPath(void *p)
{
    CA_StarDlg *pDlg = (CA_StarDlg*)p;
    pDlg->m_AStar.FindPath();
    pDlg->GetDlgItem(BTN_FIND)->EnableWindow();
    return 1;
}

void CA_StarDlg::OnClickedButton()
{
    GetDlgItem(BTN_FIND)->EnableWindow(FALSE);
    m_AStar.SetRange(m_mapinfo.nRow, m_mapinfo.nColumn);
    m_AStar.SetStartAndEnd(m_mapinfo.start, m_mapinfo.end);
    m_AStar.SetBlock(m_mapinfo.vecBlock);
    m_AStar.SetCallBack(CallBack, this);
    m_AStar.SetDiagonal(BST_CHECKED == ((CButton*)GetDlgItem(CHK_DIAGONAL))->GetCheck());

    unsigned int n;
    _beginthreadex(NULL, 0, FindPath, this, 1, &n);

    // TODO: Add your control notification handler code here
}

void CA_StarDlg::FindCallback()
{
    Sleep(300);
    while (m_bPause)
    {
        Sleep(1);
    }
    ShowMap();
}

void CA_StarDlg::DrawSquare(COLORREF &brush, const NODE *node)
{
    RECT rect;
    CBrush br(brush);
    
    rect.left = m_deltaX*node->nX+1;
    rect.top = m_deltaY*node->nY+1;
    rect.right = rect.left + m_deltaX-2;
    rect.bottom = rect.top + m_deltaY-2;
    FillRect(m_hdc, &rect, (HBRUSH)br.GetSafeHandle());

    if (0 == (brush & 1))
    {
        CString csF;
        CString csG;
        CString csH;
        csF.Format(L"%d", node->f);
        csG.Format(L"%d", node->g);
        csH.Format(L"%d", node->h);
        rect.left += 2;
        rect.right -= 2;
        rect.top += 2;
        rect.bottom -= 2;

        SetBkColor(m_hdc, brush);
        DrawTextEx(m_hdc, csF.GetBuffer(), csF.GetLength(), &rect, DT_TOP | DT_LEFT, NULL);
        DrawTextEx(m_hdc, csG.GetBuffer(), csG.GetLength(), &rect, DT_SINGLELINE | DT_BOTTOM | DT_LEFT, NULL);
        DrawTextEx(m_hdc, csH.GetBuffer(), csH.GetLength(), &rect, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT, NULL);
    }
}

void CA_StarDlg::OnClickedPicture()
{
    // TODO: Add your control notification handler code here
    m_AStar.ClearAllList();

    CPoint point;
    GetCursorPos(&point);
    int x, y;
    PointToCoord(point, x, y);

    SetSquareProperty(x, y);
}

void CA_StarDlg::PointToCoord(CPoint &point, int &x, int &y)
{
    m_picCtrl.ScreenToClient(&point);
    x = point.x/(m_nWidth/m_mapinfo.nColumn);
    y = point.y/(m_nHeight/m_mapinfo.nRow);
}

void CA_StarDlg::SetSquareProperty(int x, int y)
{
    bool bFind = false;
    unsigned int i = 0;
    for ( ; i < m_mapinfo.vecBlock.size(); i++)
    {
        if (m_mapinfo.vecBlock[i].nX == x && m_mapinfo.vecBlock[i].nY == y)
        {
            bFind = true;
            break;
        }
    }

    if (BST_CHECKED == ((CButton*)(GetDlgItem(RAD_START)))->GetCheck())
    {
        if (bFind)
            return;
        if (m_mapinfo.end.nX   == x && m_mapinfo.end.nY   == y)
            return;
        m_mapinfo.start = NODE(x, y);
    }
    else if (BST_CHECKED == ((CButton*)(GetDlgItem(RAD_END)))->GetCheck())
    {
        if (bFind)
            return;
        if (m_mapinfo.start.nX == x && m_mapinfo.start.nY == y)
            return;
        m_mapinfo.end = NODE(x, y);
    }
    else
    {
        if ((m_mapinfo.start.nX == x && m_mapinfo.start.nY == y) ||
            (m_mapinfo.end.nX   == x && m_mapinfo.end.nY   == y))
        {
            return;
        }
        if (bFind)
            m_mapinfo.vecBlock.erase(m_mapinfo.vecBlock.begin() + i);
        else
            m_mapinfo.vecBlock.push_back(NODE(x, y));
    }

    ShowMap();
}

void CA_StarDlg::ShowMap()
{
    DrawBackGround();
    DrawBarrier();

    const vector<NODE>* open = m_AStar.GetOpenVector();
    vector<NODE>::const_iterator it = open->begin();
    for ( ; it != open->end(); it++)
        DrawSquare(m_brush_open, &(*it));

    const vector<NODE>* close = m_AStar.GetCloseVector();
    it = close->begin();
    for ( ; it != close->end(); it++)
        DrawSquare(m_brush_close, &(*it));

    if (m_AStar.IsFindPath())
    {
        const vector<NODE>* path = m_AStar.GetPathVector();
        vector<NODE>::const_iterator it = path->begin();
        for ( ; it != path->end(); it++)
            DrawSquare(m_brush_path, &(*it));
    }

    DrawStartSquare();
    DrawEndSquare();
    ::BitBlt(m_pdc->GetSafeHdc(), 0, 0, m_nWidth, m_nHeight, m_hdc, 0, 0, SRCCOPY);
}

void CA_StarDlg::DrawBackGround()
{
    for (int x = 0; x < m_mapinfo.nColumn; x++)
    {
        for (int y = 0; y < m_mapinfo.nRow; y++)
        {
            DrawSquare(m_brush_background, &NODE(x, y));
        }
    }
}

void CA_StarDlg::DrawBarrier()
{
    for (unsigned int i = 0; i < m_mapinfo.vecBlock.size(); i++)
    {
        DrawSquare(m_brush_barrier, &m_mapinfo.vecBlock[i]);
    }
}

void CA_StarDlg::DrawStartSquare()
{
    DrawSquare(m_brush_start, &m_mapinfo.start);
}

void CA_StarDlg::DrawEndSquare()
{
    DrawSquare(m_brush_end, &m_mapinfo.end);
}
void CA_StarDlg::OnClickedPause()
{
    m_bPause = !m_bPause;
    if (!m_bPause)
    {
        SetDlgItemText(BTN_PAUSE, L"暂停");
    }
    else
    {
        SetDlgItemText(BTN_PAUSE, L"继续");
    }
}
