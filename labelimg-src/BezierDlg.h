// BezierDlg.h : header file
//

#if !defined(AFX_BEZIERDLG_H__78532247_8543_44EB_AE32_706C9CD0D646__INCLUDED_)
#define AFX_BEZIERDLG_H__78532247_8543_44EB_AE32_706C9CD0D646__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
/*
      �������������� ��Ҫ�ṩ��ʾ68��չʾ���ܣ�΢������(���������Ҽ�����΢��)��
	  �Զ�����pts�ļ��ڵ�ǰ·���£�
	  ���Դ��ļ��У�������ͼƬ

	  ����д�ıȽ����ң����������������Ҿ���ɾ�������ظ�����
	  
*/
// CBezierDlg dialog
#include <opencv2\core.hpp>
#include "afxwin.h"


using namespace std;

class CBezierDlg : public CDialog
{
// Construction
public:

	CBezierDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL mBInited;
// Dialog Data
	//{{AFX_DATA(CBezierDlg)
	enum { IDD = IDD_BEZIER_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBezierDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	void ShowMax();
// Implementation

public:
	BOOL m_beye1;
	BOOL m_beye2;
private:

	CPaintDC * pDC;
	int  m_currentMark;
	std::vector<cv::Point2f> m_points;
	CString m_fileName;
	CString m_ptsName;
	bool m_bDeleteFile;
	bool m_bMouseMove;
	bool m_leftBtnDown;
	bool m_bKeyOper;
	float m_nowMultiShow;//��������ֻ����ʾ������

	cv::Mat m_srcImg;
	cv::Mat m_nowImg;

	CPoint mInitPt;

	CPoint mStartPt;
	CPoint mNowPt;
	BOOL mBMovingRect;//
	BOOL mBMovingEdge;//�϶��߽�
private:
	afx_msg void OnBnClickedBtnImport();
	afx_msg void OnBnClickedBtnSave();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);


	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	
	void DrawPoint(int start,int end,CDC * pDc,bool bTong =false);
	
	void DoGetPtsAndDraw(CString fileName);

	//���ļ��У�������������ļ�
	void ParseFiles(CString foldername, std::vector<std::string>& outfiles);

	// Generated message map functions
	//{{AFX_MSG(CBezierDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	void DrawThePicPoint(cv::Mat clmResult, int posX, int posY, int dstW);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



public:
	afx_msg void OnBnClickedButtonIn();
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedBtnFolder();
	CListBox mListBox;
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedBtnDel();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEZIERDLG_H__78532247_8543_44EB_AE32_706C9CD0D646__INCLUDED_)
