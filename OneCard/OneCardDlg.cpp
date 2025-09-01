
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
    ON_WM_LBUTTONDOWN()
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
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

    CRect rect;
    GetClientRect(&rect);

    // 1. 메모리 DC와 메모리 비트맵을 생성합니다. (보이지 않는 스케치북)
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);

    CBitmap memBitmap;
    memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

    // 2. 메모리 DC에 비트맵을 선택하여 그릴 준비를 합니다.
    CBitmap* pOldBmp = memDC.SelectObject(&memBitmap);

    // ===================================================================
    // ★★★ 모든 그리기 코드는 이제 'dc'가 아닌 'memDC'에 그려야 합니다. ★★★
    // ===================================================================

    // 1. 배경 그리기 (memDC에 그립니다)
    memDC.FillSolidRect(rect, RGB(0, 80, 0));

    // 2. 덱, 공개된 카드 그리기 (memDC에 그립니다)
    DrawCard(&memDC, IDB_CARD_BACK, m_deckRect);
    Card openCard = m_game.GetOpenCard();
    if (openCard.resourceID != -1)
    {
        DrawCard(&memDC, openCard.resourceID, m_openCardRect);
    }

    // 3. 컴퓨터 패 그리기 (memDC에 그립니다)
    const auto& comCards = m_game.GetComHand();
    if (!comCards.empty())
    {
        int comCardCount = comCards.size();
        int overlap = m_cardSize.cx * OVERLAP_CARD_RATIO;
        int margin = rect.Width() / 50;

        for (int i = 0; i < comCardCount; ++i)
        {
            CRect cardRect(0, 0, m_cardSize.cx, m_cardSize.cy);
            cardRect.OffsetRect(m_openCardRect.right + margin + i * overlap, margin);
            DrawCard(&memDC, IDB_CARD_BACK, cardRect);
        }
    }

    // 4. 플레이어 패 그리기 (memDC에 그립니다)
    const auto& myCards = m_game.GetPlayerHand();
    if (!myCards.empty())
    {
        int myCardCount = myCards.size();
        int overlap = m_cardSize.cx * OVERLAP_CARD_RATIO;
        int totalHandWidth = (myCardCount - 1) * overlap + m_cardSize.cx;
        int startX = (rect.Width() - totalHandWidth) / 2;
        int margin = rect.Width() / 50;

        for (int i = 0; i < myCardCount; ++i)
        {
            CRect cardRect(0, 0, m_cardSize.cx, m_cardSize.cy);
            cardRect.OffsetRect(startX + i * overlap, rect.Height() - m_cardSize.cy - margin);
            DrawCard(&memDC, myCards[i].resourceID, cardRect);
        }
    }

    // ===================================================================
    // ★★★ 모든 그리기가 끝났습니다. ★★★
    // ===================================================================

    // 3. 메모리 DC에 완성된 그림을 실제 화면 DC로 한 번에 복사합니다.
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    // 4. 사용이 끝난 GDI 객체들을 정리합니다.
    memDC.SelectObject(pOldBmp);
    memBitmap.DeleteObject();
    memDC.DeleteDC();
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

void COneCardDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // 1. 플레이어 턴이 아니면 아무런 행동도 하지 않음
    if (!m_game.IsPlayerTurn())
    {
        CDialogEx::OnLButtonDown(nFlags, point);
        return;
    }

    GameStatus status = GameStatus::PLAYING; // 턴의 결과를 저장할 변수

    // 2. 플레이어의 카드를 클릭했는지 먼저 확인
    int clickedCardIndex = GetClickedCardIndex(point);
    if (clickedCardIndex != -1)
    {
        // 2-1. 카드를 클릭했다면, 카드 내는 로직 실행
        status = m_game.PlayCard(clickedCardIndex);
    }
    // 3. 카드를 클릭하지 않았다면, 덱을 클릭했는지 확인
    else if (m_deckRect.PtInRect(point))
    {
        // 3-1. 덱을 클릭했다면, 카드 뽑는 로직 실행
        status = m_game.DrawCard();
    }

    // 4. 어떤 행동이라도 했다면 (상태가 바뀌었다면) 화면 갱신 및 후속 처리
    if (status != GameStatus::PLAYING || clickedCardIndex != -1 || m_deckRect.PtInRect(point))
    {
        Invalidate(); // 화면을 다시 그림

        // 게임 오버 상태인지 확인하고 처리
        ProcessGameStatus(status);

        // 게임이 계속되고, 턴이 컴퓨터에게 넘어갔다면 컴퓨터 턴 타이머 설정
        if (status != GameStatus::PLAYER_WIN && status != GameStatus::PLAYER_LOSE && !m_game.IsPlayerTurn())
        {
            SetTimer(TIMER_COM_TURN, 1000, NULL);
        }
    }

    CDialogEx::OnLButtonDown(nFlags, point);
}

void COneCardDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TIMER_COM_TURN)
    {
        // 1. 타이머를 즉시 종료하여 반복적으로 실행되는 것을 막습니다. (매우 중요)
        KillTimer(TIMER_COM_TURN);

        // 2. 컴퓨터의 턴 로직을 실행합니다.
        m_game.ComTurn();

        // 3. 컴퓨터가 행동한 결과를 화면에 그리도록 갱신을 요청합니다.
        Invalidate();
    }

    CDialogEx::OnTimer(nIDEvent);
}

void COneCardDlg::ProcessGameStatus(GameStatus status)
{
    CString message;
    bool bGameOver = false;

    switch (status)
    {
    case GameStatus::PLAYER_WIN:
        message = _T("승리했습니다! 새 게임을 시작합니다.");
        bGameOver = true;
        break;
    case GameStatus::PLAYER_LOSE:
        message = _T("패배했습니다. 새 게임을 시작합니다.");
        bGameOver = true;
        break;
    case GameStatus::PLAYING:
        // 게임이 진행 중이면 아무것도 하지 않음
        break;
    }

    if (bGameOver)
    {
        AfxMessageBox(message);
        m_game.StartGame(); // 게임 로직 초기화
        Invalidate();       // 화면을 완전히 새로 그림
    }
}

// 마우스 클릭 좌표를 기반으로 플레이어의 몇 번째 카드가 클릭되었는지 반환하는 함수
// 클릭된 카드가 없으면 -1을 반환
int COneCardDlg::GetClickedCardIndex(CPoint point)
{
    const auto& myCards = m_game.GetPlayerHand();
    if (myCards.empty()) return -1;

    // --- OnPaint와 동일한 카드 위치 계산 로직 ---
    CRect clientRect;
    GetClientRect(&clientRect);

    CSize cardSize;
    cardSize.cy = clientRect.Height() / 4;
    cardSize.cx = static_cast<int>(cardSize.cy * 0.714);

    int overlap = cardSize.cx / 4;
    int myCardCount = myCards.size();
    int totalHandWidth = (myCardCount - 1) * overlap + cardSize.cx;
    int startX = (clientRect.Width() - totalHandWidth) / 2;
    int margin = clientRect.Width() / 50;
    // --- 계산 로직 끝 ---

    // ★★★ 중요: 맨 위에 그려진 카드(오른쪽 카드)부터 역순으로 검사해야 합니다. ★★★
    for (int i = myCardCount - 1; i >= 0; --i)
    {
        CRect cardRect(0, 0, cardSize.cx, cardSize.cy);
        cardRect.OffsetRect(startX + i * overlap, clientRect.Height() - cardSize.cy - margin);

        // 마지막 카드(맨 오른쪽)는 전체 영역을 검사합니다.
        if (i == myCardCount - 1)
        {
            if (cardRect.PtInRect(point))
            {
                return i; // i번째 카드 클릭됨
            }
        }
        else // 겹쳐진 카드들은 보이는 부분(왼쪽 overlap 만큼)만 검사합니다.
        {
            CRect visibleRect = cardRect;
            visibleRect.right = visibleRect.left + overlap; // 보이는 영역만 사각형으로 만듦
            if (visibleRect.PtInRect(point))
            {
                return i; // i번째 카드 클릭됨
            }
        }
    }

    return -1; // 아무 카드도 클릭되지 않음
}
BOOL COneCardDlg::OnEraseBkgnd(CDC* pDC)
{
    //return CDialogEx::OnEraseBkgnd(pDC);
    return TRUE;
}
