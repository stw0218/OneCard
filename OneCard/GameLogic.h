// GameLogic.h
#pragma once
#include <vector>
#include <string>

enum class CardSuit
{
	ERR = -1,
	CLUB = 0,
	DIAMOND,
	HEART,
	SPADE
};

enum class GameStatus {
    PLAYING,
    PLAYER_WIN,
    PLAYER_LOSE
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
    void StartGame();   // 게임 시작 (카드 덱 생성, 셔플, 분배)
    GameStatus PlayCard(int playerCardIndex); // 플레이어가 카드를 냄
    GameStatus DrawCard();    // 플레이어가 덱에서 카드를 뽑음
    GameStatus ComTurn();     // 컴퓨터의 턴을 진행

    // --- 게임 상태 정보 함수 ---
    const std::vector<Card>& GetPlayerHand() const; // 플레이어의 카드 목록을 반환
    const std::vector<Card>& GetComHand() const;    // 컴퓨터의 카드 목록을 반환
    Card GetOpenCard() const;                // 공개된 카드(맨 위)를 반환
    bool IsPlayerTurn() const; // 현재 플레이어 턴인지 확인하는 함수

private:
    GameStatus CheckGameOver(); // 게임 종료 조건을 확인하는 내부 함수

    std::vector<Card> m_deck;         // 전체 카드 덱
    std::vector<Card> m_playerHand;   // 플레이어의 패
    std::vector<Card> m_comHand;      // 컴퓨터의 패
    std::vector<Card> m_openPile;     // 공개된 카드 더미
    bool m_isPlayerTurn; // 현재 플레이어의 턴인지 여부
};