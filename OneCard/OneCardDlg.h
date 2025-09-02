
// OneCardDlg.h: 헤더 파일
//

#pragma once
#include "GameLogic.h"
#include "Cheat.h"
#include <afxcoll.h>

constexpr int DELAY_COM_TURN = 100; // 컴퓨터 턴 지연 시간 (밀리초)
constexpr double OVERLAP_CARD_RATIO = 0.20; // 카드 간격 비율

constexpr int TIMER_COM_TURN = 1;
#define TIMER_FLAG_CHECK 2
#define WM_APP_SHOW_FLAG_DIALOG (WM_APP + 1)


// COneCardDlg 대화 상자
class COneCardDlg : public CDialogEx
{
// 생성입니다.
public:
	COneCardDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	virtual ~COneCardDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ONECARD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

private:
	void DrawCard(CDC* pDC, int resourceID, CRect destRect);
	int GetClickedCardIndex(CPoint point); // 클릭된 카드의 인덱스를 찾는 함수
	void ProcessGameStatus(TurnResult status);

	CCheat m_game;
	int m_nHighlightedCardIndex; // 마우스가 올라간 카드의 인덱스 (-1: 없음)
	CArray<CImage*> m_cardImages; // 모든 CImage 객체를 실제로 저장하는 배열
	CMap<int, int, int, int> m_mapCardIDtoIndex; // 리소스 ID를 m_cardImages 배열의 인덱스로 변환해주는 맵

	// UI 레이아웃 관리를 위한 멤버 변수
	CSize m_cardSize; // 계산된 카드의 크기 (폭, 높이)
	CRect m_deckRect; // 덱의 위치와 크기
	CRect m_openCardRect; // 공개된 카드의 위치와 크기
	CArray<CRect> m_comCardRects; // 상대방 카드들의 위치와 크기 배열
	CArray<CRect> m_playerCardRects; // 내 카드들의 위치와 크기 배열
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnShowDialog(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
};
