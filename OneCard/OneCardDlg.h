
// OneCardDlg.h: 헤더 파일
//

#pragma once
#include "GameLogic.h"

#define DELAY_COM_TURN 100 // 컴퓨터 턴 지연 시간 (밀리초)
#define OVERLAP_CARD_RATIO 0.20 // 카드 간격 비율

#define TIMER_COM_TURN 1


// COneCardDlg 대화 상자
class COneCardDlg : public CDialogEx
{
// 생성입니다.
public:
	COneCardDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ONECARD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private:
	void DrawCard(CDC* pDC, int resourceID, CRect destRect);
	int GetClickedCardIndex(CPoint point); // 클릭된 카드의 인덱스를 찾는 함수
	void ProcessGameStatus(TurnResult status);

	CGameLogic m_game;
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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
