
// A_StarDlg.h : header file
//

#pragma once

#include "AStar.h"
#include "afxwin.h"

// CA_StarDlg dialog
class CA_StarDlg : public CDialog
{
// Construction
public:
	CA_StarDlg(CWnd* pParent = NULL);	// standard constructor
    ~CA_StarDlg();
    void FindCallback();

// Dialog Data
	enum { IDD = IDD_A_STAR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    BOOL PreTranslateMessage( MSG* pMsg );


    struct MAPINFO 
    {
        int nRow;
        int nColumn;
        NODE start;
        NODE end;
        vector<NODE> vecBlock;
    };

    MAPINFO m_mapinfo;

    COLORREF m_brush_background;
    COLORREF m_brush_barrier;
    COLORREF m_brush_start;
    COLORREF m_brush_end;
    COLORREF m_brush_open;
    COLORREF m_brush_close;
    COLORREF m_brush_path;


    int m_nBaseX;
    int m_nBaseY;
    int m_nWidth;
    int m_nHeight;
    int m_deltaX;
    int m_deltaY;
    CDC * m_pdc;
    HDC m_hdc;
    
    bool m_bFirstShowMap;

    bool m_bPause;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

    afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CAStar m_AStar;

    afx_msg void OnClickedButton();
    void DrawSquare(COLORREF &m_brush_normal, const NODE *node);
    CStatic m_picCtrl;
    afx_msg void OnClickedPicture();
    void PointToCoord(CPoint &point, int &x, int &y);
    void SetSquareProperty(int x, int y);
    void ShowMap();

    void DrawBackGround();
    void DrawBarrier();
    void DrawStartSquare();
    void DrawEndSquare();

    afx_msg void OnClickedPause();
};
