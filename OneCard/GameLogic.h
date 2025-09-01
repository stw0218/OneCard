// GameLogic.h
#pragma once
#include <vector>
#include <string>
#include <optional>

enum class CardSuit
{
	ERR = -1,
	CLUB = 0,
	DIAMOND,
	HEART,
	SPADE,
	JOKER_BLACK,
	JOKER_RED,
};

enum class TurnResult {
    SUCCESS,                // 일반적인 턴 성공
    INVALID_PLAY,           // 낼 수 없는 카드를 냄
    REQUIRE_SUIT_CHOICE,    // 7을 내서 모양을 선택해야 함
    GAME_OVER_WIN,
    GAME_OVER_LOSE
};

struct Card
{
    // 클로버(0), 다이아(1), 하트(2), 스페이드(3)
    CardSuit suit;
    // 1(A), 2, 3, ..., 11(J), 12(Q), 13(K)
    int rank;
    // 화면에 그릴 때 사용할 리소스 ID
    int resourceID;
};

class CGameLogic
{
public:
    CGameLogic();
    ~CGameLogic();

    // --- 게임 진행 함수 ---
    virtual void StartGame();   // 게임 시작 (카드 덱 생성, 셔플, 분배)
    TurnResult PlayCard(int playerCardIndex); // 플레이어가 카드를 냄
    virtual TurnResult DrawCard();
    TurnResult ComTurn();     // 컴퓨터의 턴을 진행
    void SetForcedSuit(CardSuit suit);      // 7 카드를 낸 후, Dlg가 선택한 모양을 설정하는 함수
    int GetAttackStack() const;     // Dlg가 현재 공격 스택을 화면에 표시할 수 있도록 값을 알려주는 함수

    // --- 게임 상태 정보 함수 ---
    const std::vector<Card>& GetPlayerHand() const; // 플레이어의 카드 목록을 반환
    const std::vector<Card>& GetComHand() const;    // 컴퓨터의 카드 목록을 반환
    Card GetOpenCard() const;                // 공개된 카드(맨 위)를 반환
    bool IsPlayerTurn() const; // 현재 플레이어 턴인지 확인하는 함수

protected:
    TurnResult CheckGameOver();
    void ReshuffleDeck();

    std::vector<Card> m_deck;         // 전체 카드 덱
    std::vector<Card> m_playerHand;   // 플레이어의 패
    std::vector<Card> m_comHand;      // 컴퓨터의 패
    std::vector<Card> m_openPile;     // 공개된 카드 더미
    bool m_isPlayerTurn; // 현재 플레이어의 턴인지 여부
    int m_attackStack;                     // 누적된 공격 카드 수
    std::optional<CardSuit> m_forcedSuit;  // 7로 인해 강제된 카드 모양 (optional: 값이 없을 수도 있음)
    std::optional<Card> m_attackCard;
};