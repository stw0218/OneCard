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
    // Ŭ�ι�(0), ���̾�(1), ��Ʈ(2), �����̵�(3)
    CardSuit suit;
    // 1(A), 2, 3, ..., 11(J), 12(Q), 13(K)
    int rank;
    // ȭ�鿡 �׸� �� ����� ���ҽ� ID
    int resourceID;
};

class CGameLogic
{
public:
    CGameLogic();
    ~CGameLogic();

    // --- ���� ���� �Լ� ---
    void StartGame();   // ���� ���� (ī�� �� ����, ����, �й�)
    GameStatus PlayCard(int playerCardIndex); // �÷��̾ ī�带 ��
    GameStatus DrawCard();    // �÷��̾ ������ ī�带 ����
    GameStatus ComTurn();     // ��ǻ���� ���� ����

    // --- ���� ���� ���� �Լ� ---
    const std::vector<Card>& GetPlayerHand() const; // �÷��̾��� ī�� ����� ��ȯ
    const std::vector<Card>& GetComHand() const;    // ��ǻ���� ī�� ����� ��ȯ
    Card GetOpenCard() const;                // ������ ī��(�� ��)�� ��ȯ
    bool IsPlayerTurn() const; // ���� �÷��̾� ������ Ȯ���ϴ� �Լ�

private:
    GameStatus CheckGameOver(); // ���� ���� ������ Ȯ���ϴ� ���� �Լ�

    std::vector<Card> m_deck;         // ��ü ī�� ��
    std::vector<Card> m_playerHand;   // �÷��̾��� ��
    std::vector<Card> m_comHand;      // ��ǻ���� ��
    std::vector<Card> m_openPile;     // ������ ī�� ����
    bool m_isPlayerTurn; // ���� �÷��̾��� ������ ����
};