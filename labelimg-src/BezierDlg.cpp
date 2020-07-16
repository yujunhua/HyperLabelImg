// BezierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Bezier.h"
#include "BezierDlg.h"
#include <fstream>
#include <iostream>
#include<Common\CommonWin.h>
#include <stdio.h>
#include "atlimage.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "resource.h"
#include "CLM\clmInter.h"
#include <math.h>


using namespace std;
using namespace cv;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBezierDlg dialog

CBezierDlg::CBezierDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBezierDlg::IDD, pParent)
	, m_beye1(TRUE)
	, m_beye2(TRUE)
{
	mBInited = false;

	m_bMouseMove = false;
	m_leftBtnDown = false;
	m_bKeyOper = false;

	m_bDeleteFile = false;
	//�ƶ����ο�
	mBMovingRect = false;

	m_nowMultiShow = 1.0;

	//�������ʼλ��
	mStartPt.x = 0;
	mStartPt.y = 0;
	//����ƶ�λ��
	mNowPt.x = 0;
	mNowPt.y = 0;
	//�µĳ�ʼ��λ��
	mInitPt.x = 0;
	mInitPt.y = 0;
	m_currentMark = -1;
	mBMovingEdge = false;
	
}

void CBezierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBezierDlg)
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_LIST1, mListBox);
	DDX_Check(pDX, IDC_EYE1, m_beye1);
	DDX_Check(pDX, IDC_EYE2, m_beye2);
}

BEGIN_MESSAGE_MAP(CBezierDlg, CDialog)
	//{{AFX_MSG_MAP(CBezierDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()


	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BTN_SAVE, &CBezierDlg::OnBnClickedBtnSave)
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOMMAND()

	

	ON_BN_CLICKED(IDC_BUTTON_IN, &CBezierDlg::OnBnClickedButtonIn)
	ON_WM_MOVING()
	ON_BN_CLICKED(IDC_BTN_FOLDER, &CBezierDlg::OnBnClickedBtnFolder)
	ON_LBN_SELCHANGE(IDC_LIST1, &CBezierDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BTN_DEL, &CBezierDlg::OnBnClickedBtnDel)
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CBezierDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_currentMark = -1;



	CRect rect;
	GetClientRect(rect);


	//�ؼ��㣬��������һ��
	//m_points.push_back(cv::Point(500, 500));
	//m_points.push_back(cv::Point(750, 500));
	//m_points.push_back(cv::Point(750, 550));
	//m_points.push_back(cv::Point(500, 550));


	CLM_DoInit();
	mBInited = true;

	ShowMax();
	return TRUE;  // return TRUE  unless you set the focus to a control
}



/*

	���ܣ��ؼ����Ӧ�� �ƶ���ͨ·
*/
void CBezierDlg::DrawPoint(int start,int end,CDC* pDC, bool bTong)
{
	//���ݵ�ǰ�ƶ�״̬����ֵ �����б�

	int disX = mNowPt.x - mStartPt.x;
	int disY = mNowPt.y - mStartPt.y;
	
	// //��tmpNowPts��������ʾ��m_points��Ϊԭʼ���ݱ��ֲ���
	vector<Point2f> tmpNowPts;
	char szBuf[256];
	if (mBMovingEdge)
	{	//�ƶ����Ǳ߽磬���ҵ���֮����ĵ㣬������Ե��ƶ�
		int span = 6 * m_nowMultiShow;
		if (span > 15) span = 15;
		//�����ƶ�X����Y
		sprintf(szBuf, "%d %d", disX,disY);
		if (abs(disX) > abs(disY))
		{
			disY = 0;
			for (int m = 0; m < m_points.size(); ++m)
			{
				Point2f tp = m_points[m];
				sprintf(szBuf,"%d", mNowPt.x - tp.x*m_nowMultiShow - disX);
				if (abs(mNowPt.x - tp.x*m_nowMultiShow - disX) < span)
				{
					tmpNowPts.push_back(cv::Point2f(m_points[m].x*m_nowMultiShow+disX,
						m_points[m].y*m_nowMultiShow));
				}
				else
				{
					//��������
					tmpNowPts.push_back(cv::Point2f(m_points[m].x*m_nowMultiShow,
						m_points[m].y*m_nowMultiShow));
				}
			}
		}
		else
		{
			disX = 0;

			for (int m = 0; m < m_points.size(); ++m)
			{
				Point2f tp = m_points[m];
				if (abs(mNowPt.y - tp.y*m_nowMultiShow - disY) < span)
				{
					tmpNowPts.push_back(cv::Point2f(m_points[m].x*m_nowMultiShow,
						m_points[m].y*m_nowMultiShow+disY));
				}
				else
				{
					tmpNowPts.push_back(cv::Point2f(m_points[m].x*m_nowMultiShow,
						m_points[m].y*m_nowMultiShow ));
				}
			}
		}
		

	}
	else
	{
		if (!mBMovingRect)
		{
			disX = 0;
			disY = 0;
		}


		for (int k = 0; k < m_points.size(); ++k)
		{
			tmpNowPts.push_back(cv::Point2f(m_points[k].x*m_nowMultiShow + disX, 
				m_points[k].y*m_nowMultiShow + disY));
		}
	}

	


	
	pDC->MoveTo(tmpNowPts[start].x, tmpNowPts[start].y);
	for(int i = start; i<=end; i++)
	{
		//���Ʊʵ���ɫ
			CPen penStroke(PS_SOLID,2,0x007700);
			CPen *ppenPrevious=pDC->SelectObject(&penStroke);
			pDC->LineTo(tmpNowPts[i].x, tmpNowPts[i].y);

			if(bTong)
			if(i == end)
				pDC->LineTo(tmpNowPts[start].x, tmpNowPts[start].y);

			pDC->SelectObject(ppenPrevious);

			//���Ʊʵ���ɫ
			CPen penStroke1(PS_SOLID,2,0x00FFFF);
		    CPen penStroke2(PS_SOLID,2,0x0000FF);

			CPen *ppenPrevious2;
			if(m_currentMark == i)
			{
				ppenPrevious2 = pDC->SelectObject(&penStroke2);
			}
			else
			{
				ppenPrevious2 = pDC->SelectObject(&penStroke1);
			}
			

			pDC->SetBkMode(TRANSPARENT);
		
			cv::Point2d pt = cv::Point(tmpNowPts[i].x, tmpNowPts[i].y);
			CPoint tmpPts[4];
			float scale = m_nowMultiShow*0.8;
			if (scale < 1) scale = 1.0;
			if (scale > 2.0) scale = 2.0;
			int spanD = 7* scale;

			tmpPts[0].x = pt.x - spanD;
			tmpPts[0].y = pt.y - spanD;

			tmpPts[1].x = pt.x + spanD;
			tmpPts[1].y = pt.y - spanD;
			tmpPts[2].x = pt.x + spanD;
			tmpPts[2].y = pt.y + spanD;
			tmpPts[3].x = pt.x - spanD;
			tmpPts[3].y = pt.y + spanD;
		
			
			 pDC->SelectStockObject(NULL_BRUSH);
			 pDC->Ellipse(tmpPts[0].x,tmpPts[0].y,tmpPts[2].x,tmpPts[2].y);

		
			pDC->SelectObject(ppenPrevious2);
	}
	
}
/*

���ܣ���������Ҫ��  ��Ϊ�ڽ���΢���ؼ����ʱ�򣬷�ֹ��˸�� ����˫����������
  ���� ����������ͼƬ��Ȼ����ƹؼ��㣬���Ƶ��������������������
  
*/
void CBezierDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

	}
	else
	{
		CDialog::OnPaint();

		CDC * pDC = GetDC();
		int iCanvaW = 1560;
		if(!m_nowImg.empty() && pDC)
		{

			   CImage ImageBk;
			   ImageBk.Create(iCanvaW, 960, 24);
				CImage Image;
				cv::Mat dst = m_nowImg.clone();
				cv::resize(dst, dst, cv::Size(m_nowImg.cols*m_nowMultiShow, m_nowImg.rows*m_nowMultiShow));


				Image.Create(dst.cols,dst.rows,24);
			
				int i;	int j;

				for (i = 0; i < Image.GetHeight(); i++)
				{
					int step = dst.step[0]*i;
					int jump = 0;
					for (j = 0; j < Image.GetWidth(); j++)
					{
						byte *pByte = (byte *)Image.GetPixelAddress(j,i);
						pByte[0] = (unsigned char)dst.data[step+jump+0];
						pByte[1] = (unsigned char)dst.data[step+jump+1];
						pByte[2] = (unsigned char)dst.data[step+jump+2];
		
						jump+=3;
					}
				}


				CDC   MemDC;   //���ȶ���һ����ʾ�豸���� 
				CBitmap   MemBitmap;//����һ��λͼ���� 

				pDC->SetBkMode(TRANSPARENT);
				//���������Ļ��ʾ���ݵ��ڴ���ʾ�豸 
				MemDC.CreateCompatibleDC(pDC); 
				//��ʱ�����ܻ�ͼ����Ϊû�еط���   ^_^ 
				//���潨��һ������Ļ��ʾ���ݵ�λͼ������λͼ�Ĵ�С������ô��ڵĴ�С 
				MemBitmap.CreateCompatibleBitmap(pDC, iCanvaW,960);
			
				//��λͼѡ�뵽�ڴ���ʾ�豸�� 
				//ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ�� 
				CBitmap  *pOldBit=MemDC.SelectObject(&MemBitmap); 

				ImageBk.Draw(MemDC.m_hDC, 0, 0, iCanvaW,960);
				if (mBMovingRect || m_currentMark >=0 || mBMovingEdge)
				{//�����ƶ��� ���� �ƶ��������£��������治��Ҫ�ƶ�
					//�ƶ��߽�
					Image.Draw(MemDC.m_hDC, mInitPt.x, mInitPt.y,Image.GetWidth(), Image.GetHeight());
				}
				else
				Image.Draw(MemDC.m_hDC, mInitPt.x + mNowPt.x - mStartPt.x, mInitPt.y + mNowPt.y - mStartPt.y, 
					Image.GetWidth(),Image.GetHeight());

				char szBuf[256];
				sprintf(szBuf,"%d", m_points.size());
				if (m_points.size() == 68)
				{
					//������
					DrawPoint(0, 16, &MemDC);
					//��üë
					DrawPoint(17, 21, &MemDC);
					//��üë
					DrawPoint(22, 26, &MemDC);
					//����
					DrawPoint(27, 30, &MemDC);
					//���»���
					DrawPoint(31, 35, &MemDC);
					//����
					DrawPoint(36, 41, &MemDC);
					//����
					DrawPoint(42, 47, &MemDC);
					//���������
					DrawPoint(48, 59, &MemDC);

					//��� ������
					DrawPoint(60, 67, &MemDC);
				}
				else if(m_points.size() > 0)
				{
					//������
					DrawPoint(0,3,&MemDC,true);
				}
				//���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ 
				pDC->BitBlt(0,0 ,
					iCanvaW, 960,&MemDC,0,0,SRCCOPY);


				MemDC.SelectObject(pOldBit);
				//��ͼ��ɺ������ 
				MemBitmap.DeleteObject(); 
				MemDC.DeleteDC();

				ReleaseDC(pDC);

		}

	}
}

void CBezierDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	mStartPt = point;
	mNowPt = point;
	m_leftBtnDown = true;
	mBMovingRect = false;
	mBMovingEdge = false;//�ȳ�ʼ��
	// TODO: Add your message handler code here and/or call default
	m_currentMark = -1;

	// //��tmpNowPts��������ʾ��m_points��Ϊԭʼ���ݱ��ֲ���
	vector<Point2f> tmpNowPts;
	for (int k = 0; k < m_points.size(); ++k)
	{
		tmpNowPts.push_back(cv::Point2f(m_points[k].x*m_nowMultiShow, m_points[k].y*m_nowMultiShow));
	}

	    //����������˵��ѡ���˹ؼ���
		double x, y;
		double t=80* m_nowMultiShow;
		for(int i = 0; i < tmpNowPts.size(); i++)
		{
			x = tmpNowPts[i].x - point.x, y = tmpNowPts[i].y - point.y;
			 x*=x; y*=y;
			if(x + y < t)
			{	
				m_currentMark = i;		
			}

		}

		//���϶���Ϊס������ж�������
		if (m_currentMark == -1)
		{
			vector<Point2f> tmpPolyPts;
			for (int k = 0; k < min(16, (int)m_points.size()); ++k)
			{
				tmpPolyPts.push_back(cv::Point2f(m_points[k].x*m_nowMultiShow, m_points[k].y*m_nowMultiShow));
			}

			int span = m_nowMultiShow * 9;
			if (span > 20) span = 20;
			//�ж��Ƿ��ƶ�����
			int iDis = cv::pointPolygonTest(tmpPolyPts, cv::Point(point.x, point.y), true);
			if (iDis > span)
			{
				mBMovingRect = true;
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
			}
			else
			{
				
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
				if (iDis < span && iDis > -span)
				{
					mBMovingEdge = true;
				}
			}
		}
	
		
		
	
	   CDialog::OnLButtonDown(nFlags, point);

}

void CBezierDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//���е����ݽ��г�ʼ�����߸���
	if ( m_leftBtnDown )
	{
		//˵�������ƶ��㣬��������ͼƬ���ƶ�
		if (m_currentMark < 0 && !mBMovingRect && !mBMovingEdge)
		{
			//��ԭʼλ�ý��б���
			mInitPt.x = mInitPt.x + mNowPt.x - mStartPt.x;
			mInitPt.y = mInitPt.y + mNowPt.y - mStartPt.y;
		}


		if (mBMovingEdge)
		{
			int disX = mNowPt.x - mStartPt.x;
			int disY = mNowPt.y - mStartPt.y;

			char szBuf[256];
			if (mBMovingEdge)
			{	//�ƶ����Ǳ߽磬���ҵ���֮����ĵ㣬������Ե��ƶ�
				int span = 9 * m_nowMultiShow;
				if (span > 15) span = 18;
				////�����ƶ�X����Y
				sprintf(szBuf, "%d %d", disX, disY);
				if (abs(disX) > abs(disY))
				{
					disY = 0;
					for (int m = 0; m < m_points.size(); ++m)
					{
						Point2f tp = m_points[m];
						sprintf(szBuf, "%d", mNowPt.x - tp.x*m_nowMultiShow - disX);
						if (abs(mNowPt.x - tp.x*m_nowMultiShow - disX) < span)
						{
							m_points[m] = cv::Point2f((m_points[m].x*m_nowMultiShow + disX)/ m_nowMultiShow,
								m_points[m].y);
						}
						
					}
				}
				else
				{
					disX = 0;

					for (int m = 0; m < m_points.size(); ++m)
					{
						Point2f tp = m_points[m];
						if (abs(mNowPt.y - tp.y*m_nowMultiShow - disY) < span)
						{
							m_points[m] = cv::Point2f(m_points[m].x,
								(m_points[m].y*m_nowMultiShow + disY)/ m_nowMultiShow);
						}
						
					}
				}


			}
		}
		else if ( mBMovingRect)
		{
			for (int k = 0; k < m_points.size(); ++k)
			{
				m_points[k].x += (mNowPt.x - mStartPt.x)/m_nowMultiShow;
				m_points[k].y += (mNowPt.y - mStartPt.y) / m_nowMultiShow;
			}
			
		}
	}


	mNowPt.x = 0;
	mNowPt.y = 0;
	mStartPt.x = 0;
	mStartPt.y = 0;
	mBMovingRect = false;
	mBMovingEdge = false;
	m_leftBtnDown = false;
	// TODO: Add your message handler code here and/or call default
	m_bMouseMove = false;
	m_currentMark = -1;

	CDialog::OnLButtonUp(nFlags, point);
}

void CBezierDlg::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (m_leftBtnDown)
	{
		mNowPt = point;
		if (m_currentMark >= 0)
		{//�ƶ��ؼ���
			m_points[m_currentMark].x = mNowPt.x/ m_nowMultiShow;
			m_points[m_currentMark].y = mNowPt.y/ m_nowMultiShow;
		}


		CRect tRect;
		GetClientRect(tRect);
		RedrawWindow(tRect);
	}
	
	if (mBMovingRect)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));

	}
	else
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}


	CDialog::OnMouseMove(nFlags, point);
}






/*

��һ���ļ������е�ֵ���й�0

*/
void CBezierDlg::DoGetPtsAndDraw(CString fileName)
{
	mInitPt.x = 0;
	mInitPt.y = 0;

	mStartPt.x = 0;
	mStartPt.y = 0;

	mNowPt.x = 0;
	mNowPt.y = 0;


	m_points.resize(2);
	//Ĭ��ֵ�� 4���ǵ�
	m_points[0].x = 300;
	m_points[0].y = 700;
	m_points[1].x = 400;
	m_points[1].y = 450;
	//m_points[2].x = 200;
	//m_points[2].y = 200;
	//m_points[3].x = 10;
	//m_points[3].y = 200;


	m_nowMultiShow = 1.0;
	   m_fileName=fileName; //�ļ�����������FilePathName��
	   m_ptsName = m_fileName;

	   m_srcImg = cv::imread(m_fileName.GetBuffer(0));
	   //���ͼƬ����
	   if (m_srcImg.cols > 1500)
	   {
		   cv::resize(m_srcImg, m_srcImg,Size(1500, m_srcImg.rows*1500/ m_srcImg.cols));
		   cv::imwrite(m_fileName.GetBuffer(0), m_srcImg);
	   }
	   else if (m_srcImg.cols < 600)
	   {
		   cv::resize(m_srcImg, m_srcImg, Size(800, m_srcImg.rows * 800 / m_srcImg.cols));
		   cv::imwrite(m_fileName.GetBuffer(0), m_srcImg);
	   }


	   m_nowImg = m_srcImg.clone();
	   Invalidate();
	
		int index = m_ptsName.Replace(".jpg",".pts");
		if(index == 0)
		{
			index = m_ptsName.Replace(".png",".pts");
			if(index == 0)
			{
				index = m_ptsName.Replace(".bmp",".pts");
			}
		}

		ifstream locations(m_ptsName.GetBuffer(0), ios_base::in);
		if(locations.is_open())
		{
			vector<cv::Point2f> tmpPts;
			//�������忴�ܶ�ȡ�����ٵ�
			tmpPts.resize(120);
			string line;
			int index = 0;
			// The main file contains the references to other files
			char szInfo[256];
			while (!locations.eof())
			{ 
				getline(locations, line);
				if (line.empty()) break;
				stringstream lineStream(line);

				int indexF = line.find(" ");
				string str1;
				lineStream >> str1;
				strcpy(szInfo,str1.c_str());
				tmpPts[index].x = atof(str1.c_str());
				lineStream >> str1;
				strcpy(szInfo, str1.c_str());
				tmpPts[index].y = atof(str1.c_str());
				sprintf(szInfo,"%.2f %.2f", tmpPts[index].x, tmpPts[index].y);

				
				index++;
			}
			m_points.resize(index);
			for (int k = 0; k < index; ++k)
				m_points[k] = tmpPts[k];
			
		}
		else
		{
			//������ü�����������ܴ�������
			if (((CButton*)GetDlgItem(IDC_CHECKFACE))->GetCheck())
			{
				if (DoDetectMarksInPic(m_nowImg))
				{
					m_points = GetMainPts();
				}
			}
			
		
		}
		

}
void CBezierDlg::OnBnClickedBtnImport()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString FilePathName;
    CFileDialog dlg(TRUE, //TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
        NULL, 
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        (LPCTSTR)_TEXT("pic Files (*.jpg)|*.jpg|(*.bmp)|*.bmp|(*.png)|*.png|All Files (*.*)|*.*||"),
        NULL);
    if(dlg.DoModal()==IDOK)
    {
		DoGetPtsAndDraw(dlg.GetPathName());
		
	}
}


void CBezierDlg::OnBnClickedBtnSave()
{

		UpdateData(TRUE);
		char szBuf[256];
		sprintf(szBuf, "%s", m_ptsName.GetBuffer(0));
		fstream locations(m_ptsName.GetBuffer(0), ios_base::out);
		locations << m_srcImg.rows << " " << m_srcImg.cols << " ";

		locations << int(m_beye1) << " " << m_points[0].x << " " << m_points[0].y << " ";
		locations << int(m_beye2) << " " << m_points[1].x << " " << m_points[1].y << "\n";

		//locations << "}" << "\n";



}


BOOL CBezierDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(!m_leftBtnDown)
	{
		if(m_bKeyOper)
		{
			m_bKeyOper = false;return 1;
		}
		else
		{
			return CDialog::OnEraseBkgnd(pDC);
		}
	}
	else
	{
		return 1;
	}
}



void CBezierDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nID == 61488)
	{
		//ȫ����Ϣ
		ShowMax();
		Invalidate();
	}
	CDialog::OnSysCommand(nID, lParam);
}



BOOL CBezierDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == 0x53&& 
			(pMsg->lParam == 0x401f0001
				|| pMsg->lParam == 0x1f0001))
		{
			//AfxMessageBox("ctrl s");
			OnBnClickedBtnSave();
		}
		char szBuf[256];
		sprintf(szBuf, "%x %x\n", pMsg->wParam, pMsg->lParam);
		OutputDebugString(szBuf);
	}
	if(pMsg->message == WM_KEYUP)
	{
			int span = 1;
			switch (pMsg->wParam)
			{
			case VK_UP:
				{
					if(m_currentMark > 0)
					{
						m_points[m_currentMark].y-=span;
						m_bKeyOper = true;
						
						RedrawWindow();
					}
				}
				break;
			case VK_DOWN:
				if(m_currentMark > 0)
					{
						m_points[m_currentMark].y+=span;
						m_bKeyOper = true;
						CRect tRect;
						GetClientRect(tRect);
						RedrawWindow(tRect);
					}
				break;

			default:
				break;
			}
			

		
		
	}

	if(pMsg->wParam == VK_UP
				|| pMsg->wParam == VK_DOWN
				|| pMsg->wParam == VK_LEFT
				|| pMsg->wParam == VK_RIGHT)
			{
				if(m_currentMark > 0)
				{
					return 1;
				}
			}

	return CDialog::PreTranslateMessage(pMsg);
}






void CBezierDlg::DrawThePicPoint(cv::Mat clmResult, int posX, int posY, int dstW)
{

	//��ʹ��ǰ�����ԭ�����Ƶ�ͼƬȥ��������ɾȥ����������
	CDC *pDC = GetDC();

	int biaoWidht = dstW;
	cv::Mat dst;
	if (clmResult.cols > dstW)
	{
		float ratio = clmResult.cols*1.0 / biaoWidht;
		int height = clmResult.rows / ratio;

		resize(clmResult, dst, cv::Size(biaoWidht, height));
	}
	else
	{
		dst = clmResult.clone();
	}




	CImage Image;
	Image.Create(dst.cols, dst.rows, 24);
	int i;
	int j;
	for (i = 0; i < Image.GetHeight(); i++)
	{
		int step = dst.step[0] * i;
		int jump = 0;
		for (j = 0; j < Image.GetWidth(); j++)
		{
			byte *pByte = (byte *)Image.GetPixelAddress(j, i);

			if (dst.channels() != 1)
			{
				pByte[0] = (unsigned char)dst.data[step + jump + 0];
				pByte[1] = (unsigned char)dst.data[step + jump + 1];
				pByte[2] = (unsigned char)dst.data[step + jump + 2];

				jump += 3;
			}
			else
			{
				pByte[0] = (unsigned char)dst.data[step + jump + 0];
				pByte[1] = (unsigned char)dst.data[step + jump + 0];
				pByte[2] = (unsigned char)dst.data[step + jump + 0];

				jump += 1;
			}

		}
	}

	Image.Draw(pDC->m_hDC, posX, posY);
	Image.Destroy();

	ReleaseDC(pDC);
}

void CBezierDlg::OnBnClickedButtonIn()
{

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString FilePathName;
	CFileDialog dlg(TRUE, //TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("pic Files (*.jpg)|*.jpg|(*.bmp)|*.bmp|(*.png)|*.png|All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		DoGetPtsAndDraw(dlg.GetPathName());
	
	}
}




void CBezierDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	CDialog::OnMoving(fwSide, pRect);

	CRect rect;
	GetWindowRect(rect);

	
	// TODO: �ڴ˴������Ϣ����������
}





int truncateUint8(int value)
{
	if (value < 0) value = 0;
	if (value > 255) value = 255;
	return value;
}

void CBezierDlg::ParseFiles(CString foldername, std::vector<std::string>& outfiles)
{

	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	strcpy(szFind, foldername);
	strcat(szFind, "\\*.*");
	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{

			}
		}
		else
		{
			CString strTmp = foldername + "/";
			strTmp += FindFileData.cFileName;
			if(strTmp.Find(".jpg") != -1)
			outfiles.push_back(strTmp.GetBuffer(0));
		}


		if (!FindNextFile(hFind, &FindFileData))
			break;
		//
	}

	FindClose(hFind);
	
}


void CBezierDlg::OnBnClickedBtnFolder()
{
	TCHAR szBuffer[MAX_PATH] = { 0 };
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = NULL;
	bi.pszDisplayName = szBuffer;
	bi.lpszTitle = _T("������ѡ�ļ���Ŀ¼:");
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (NULL == idl)
	{
		return;
	}
	SHGetPathFromIDList(idl, szBuffer);

	//�����ļ�������ļ�
	vector<std::string> files;
	ParseFiles(szBuffer, files);

	mListBox.ResetContent();
	for (int k = 0; k < files.size(); ++k)
	{
		mListBox.AddString(files[k].c_str());
	}
}


void CBezierDlg::OnLbnSelchangeList1()
{
	int iCurSel = mListBox.GetCurSel();
	//�õ���ǰѡ������
	if (LB_ERR != iCurSel)
	{
		CString strTmp;
		mListBox.GetText(iCurSel, strTmp);

		//ѡ��֮��һ�й�0
		DoGetPtsAndDraw(strTmp);

		//�õ���ǰѡ����ַ���
		SetWindowText(strTmp);
	}
									
	
}


void CBezierDlg::OnBnClickedBtnDel()
{
	int iCurSel = mListBox.GetCurSel();
	//�õ���ǰѡ������
	if (LB_ERR != iCurSel)
	{
		CString strTmp;
		mListBox.GetText(iCurSel, strTmp);
		DeleteFile(strTmp);
		mListBox.DeleteString(iCurSel);
	}
}


BOOL CBezierDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (zDelta > 0)
	{
		m_nowMultiShow += 0.1;
	}
	else
	{
		m_nowMultiShow -= 0.1;
		if (m_nowMultiShow < 1) m_nowMultiShow = 1.0;

	}
	//���߱�������ˢ��
	m_bKeyOper = true;
	CRect tRect;
	GetClientRect(tRect); 
	tRect.SetRect(tRect.left,tRect.top,tRect.Width()-220,tRect.Height());
	RedrawWindow(tRect);
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CBezierDlg::ShowMax()
{
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	MoveWindow(0, 0, cx, cy);
	GetDlgItem(IDC_CHECK1)->MoveWindow(cx - 230, 5, 50, 40);
	GetDlgItem(IDC_BTN_FOLDER)->MoveWindow(cx - 160, 5, 70, 40);
	GetDlgItem(IDC_BUTTON_IN)->MoveWindow(cx - 90, 5, 70, 40);

	GetDlgItem(IDC_LIST1)->MoveWindow(cx - 240, 55, 220, cy - 190);
	GetDlgItem(IDC_BTN_DEL)->MoveWindow(cx - 120, cy - 130, 110, 40);
	

}
void CBezierDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);


	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)
	{
		if (mBInited)
		{
			ShowMax();
		}
	}
}
