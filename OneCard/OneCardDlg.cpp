
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
    ON_WM_SIZE()
END_MESSAGE_MAP()


// COneCardDlg 메시지 처리기

BOOL COneCardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    m_game.StartGame();

	return TRUE;
}

void COneCardDlg::OnPaint()
{
    CPaintDC dc(this);

    // 1. 배경 그리기
    CRect rect;
    GetClientRect(&rect);
    dc.FillSolidRect(rect, RGB(0, 80, 0));

    // OnSize에서 미리 계산해 둔 위치 정보를 사용합니다.
    // 2. 카드 덱(Deck) 더미 그리기
    DrawCard(&dc, IDB_CARD_BACK, m_deckRect);

    // 3. 공개된 카드(Open Pile) 그리기
    //    GetOpenCard는 실패할 수 있으므로 방어 코드를 추가합니다.
    Card openCard = m_game.GetOpenCard();
    if (openCard.resourceID != -1) // 유효한 카드일 경우에만 그립니다.
    {
        DrawCard(&dc, openCard.resourceID, m_openCardRect);
    }

    // --- 4. 상대방(컴퓨터) 패 그리기 ---
    const auto& comCards = m_game.GetComHand();
    if (!comCards.empty())
    {
        // 컴퓨터 패의 위치를 그릴 때마다 계산합니다.
        int comCardCount = comCards.size();
        int overlap = m_cardSize.cx / 4;
        int margin = rect.Width() / 50;

        for (int i = 0; i < comCardCount; ++i)
        {
            CRect cardRect(0, 0, m_cardSize.cx, m_cardSize.cy);
            // m_openCardRect 오른쪽에 배치합니다.
            cardRect.OffsetRect(m_openCardRect.right + margin + i * overlap, margin);
            DrawCard(&dc, IDB_CARD_BACK, cardRect); // 컴퓨터 카드는 뒷면으로 그립니다.
        }
    }

    // --- 5. 내(플레이어) 패 그리기 ---
    const auto& myCards = m_game.GetPlayerHand();
    if (!myCards.empty())
    {
        // 플레이어 패의 위치를 그릴 때마다 계산합니다.
        int myCardCount = myCards.size();
        int overlap = m_cardSize.cx / 4;
        int totalHandWidth = (myCardCount - 1) * overlap + m_cardSize.cx;
        int startX = (rect.Width() - totalHandWidth) / 2; // 중앙 정렬
        int margin = rect.Width() / 50;

        for (int i = 0; i < myCardCount; ++i)
        {
            CRect cardRect(0, 0, m_cardSize.cx, m_cardSize.cy);
            cardRect.OffsetRect(startX + i * overlap, rect.Height() - m_cardSize.cy - margin);
            DrawCard(&dc, myCards[i].resourceID, cardRect);
        }
    }
}

void COneCardDlg::DrawCard(CDC* pDC, int resourceID, CRect destRect)
{
    CImage image;
    bool bLoaded = false; // 로드 성공 여부를 추적할 플래그

    // Win32 API를 사용하여 "PNG" 타입의 리소스를 직접 찾습니다.
    HRSRC hRes = ::FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(resourceID), _T("PNG"));
    if (hRes != NULL)
    {
        DWORD len = ::SizeofResource(AfxGetInstanceHandle(), hRes);
        HGLOBAL hResData = ::LoadResource(AfxGetInstanceHandle(), hRes);
        if (hResData != NULL)
        {
            // 리소스 데이터에 대한 포인터를 얻습니다.
            void* pResData = ::LockResource(hResData);
            if (pResData != NULL)
            {
                // --- 핵심 수정 부분 ---
                // 1. CreateStreamOnHGlobal이 사용할 수 있는 새로운 HGLOBAL 메모리를 할당합니다.
                HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, len);
                if (hGlobal != NULL)
                {
                    // 2. 쓰기 가능한 메모리 포인터를 얻습니다.
                    void* pBuffer = ::GlobalLock(hGlobal);
                    if (pBuffer != NULL)
                    {
                        // 3. 리소스 데이터를 새로 할당한 메모리에 복사합니다.
                        memcpy(pBuffer, pResData, len);
                        ::GlobalUnlock(hGlobal);

                        // 4. 이제 안전하게 스트림을 생성할 수 있습니다.
                        IStream* pStream = NULL;
                        // fDeleteOnRelease를 TRUE로 설정하여 스트림이 닫힐 때 hGlobal 메모리가 자동으로 해제되게 합니다.
                        if (SUCCEEDED(::CreateStreamOnHGlobal(hGlobal, TRUE, &pStream)))
                        {
                            if (SUCCEEDED(image.Load(pStream)))
                            {
                                bLoaded = true; // 최종 로드 성공!
                            }
                            pStream->Release();
                        }
                    }

                    // 만약 스트림 생성/로드 실패 시, 우리가 직접 할당한 메모리를 해제해야 합니다.
                    if (!bLoaded)
                    {
                        ::GlobalFree(hGlobal);
                    }
                }
                // --- 핵심 수정 끝 ---
            }
        }
    }

    // 최종적으로 로드 성공 여부를 확인하고 이미지를 그립니다.
    if (bLoaded && !image.IsNull())
    {
        image.Draw(pDC->GetSafeHdc(), destRect);
    }
    else
    {
        // 로드 실패 시 디버그 '출력' 창에 메시지를 표시합니다.
        CString str;
        str.Format(_T("!!! 리소스 ID %d, 타입 'PNG' 로드 최종 실패 !!!"), resourceID);
        TRACE(str);

        // (선택) 실패했음을 화면에 표시
        pDC->Rectangle(destRect);
        pDC->TextOut(destRect.left + 30, destRect.top + 40, L"X");
    }
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

void COneCardDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    if (cx == 0 || cy == 0) return; // 창이 최소화되는 등 크기가 0이면 계산하지 않음

    // 1. 새 카드 크기 계산 (창 높이의 1/4 크기로 설정, 가로세로 비율 유지)
    //    - 일반적인 트럼프 카드 비율: 가로:세로 ≈ 2.5 : 3.5 ≈ 0.714
    m_cardSize.cy = cy / 4; // 카드 높이를 창 높이의 25%로 설정
    m_cardSize.cx = static_cast<int>(m_cardSize.cy * 0.714);

    // 2. 덱과 공개된 카드 위치 계산 (상단 배치)
    int margin = cx / 50; // 창 너비의 2%를 여백으로 사용
    m_deckRect.SetRect(margin, margin, margin + m_cardSize.cx, margin + m_cardSize.cy);
    m_openCardRect.SetRect(m_deckRect.right + margin, margin,
        m_deckRect.right + margin + m_cardSize.cx, margin + m_cardSize.cy);

    // 5. 모든 계산이 끝났으므로 화면을 갱신하라고 윈도우에 알림
    Invalidate();
}