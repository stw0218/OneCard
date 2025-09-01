
// OneCardDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "OneCard.h"
#include "OneCardDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COneCardDlg 대화 상자



COneCardDlg::COneCardDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ONECARD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COneCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COneCardDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &COneCardDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COneCardDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// COneCardDlg 메시지 처리기

BOOL COneCardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void COneCardDlg::OnPaint()
{
    CPaintDC dc(this); // 그리기를 위한 DC(Device Context)를 얻습니다.

    // 1. 배경 그리기 (단색 또는 이미지)
    CRect rect;
    GetClientRect(&rect);
    dc.FillSolidRect(rect, RGB(0, 80, 0)); // 어두운 녹색 배경

    // 2. 카드 덱(Deck) 더미 그리기
    DrawCard(&dc, IDB_CARD_BACK, CPoint(50, 50)); // 카드 뒷면 그리기

    // 3. 공개된 카드(Open Pile) 그리기
    // gameState.getOpenCard() 같은 함수로 현재 공개된 카드 ID를 가져옵니다.
    int topCardID = GetTopCardResourceID();
    DrawCard(&dc, topCardID, CPoint(150, 50));

    // 4. 상대방(컴퓨터) 패 그리기
    // 상대방 카드 수만큼 뒷면을 겹쳐서 그립니다.
    int comCardCount = GetComCardCount();
    for (int i = 0; i < comCardCount; ++i) {
        DrawCard(&dc, IDB_CARD_BACK, CPoint(300 + i * 20, 50));
    }

    // 5. 내(플레이어) 패 그리기
    // 내가 가진 카드를 모두 앞면으로 겹쳐서 그립니다.
    std::vector<int> myCards = GetMyCardResourceIDs();
    for (int i = 0; i < myCards.size(); ++i) {
        DrawCard(&dc, myCards[i], CPoint(50 + i * 40, 400));
    }

    // CDialogEx::OnPaint()는 호출하지 않거나, 그리기를 시작하기 전에 호출합니다.
    // CDialogEx::OnPaint(); 
}

void COneCardDlg::DrawCard(CDC* pDC, int bitmapID, CPoint pos)
{
    CBitmap bmp;
    bmp.LoadBitmap(bitmapID);

    BITMAP bmpInfo;
    bmp.GetBitmap(&bmpInfo);

    CDC memDC;
    memDC.CreateCompatibleDC(pDC);

    CBitmap* pOldBmp = memDC.SelectObject(&bmp);
    pDC->BitBlt(pos.x, pos.y, bmpInfo.bmWidth, bmpInfo.bmHeight, &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldBmp);
    memDC.DeleteDC();
    bmp.DeleteObject();
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR COneCardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COneCardDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

void COneCardDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

BOOL COneCardDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
