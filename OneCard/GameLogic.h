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
    SUCCESS,                // �Ϲ����� �� ����
    INVALID_PLAY,           // �� �� ���� ī�带 ��
    REQUIRE_SUIT_CHOICE,    // 7�� ���� ����� �����ؾ� ��
    GAME_OVER_WIN,
    GAME_OVER_LOSE
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
    virtual void StartGame();   // ���� ���� (ī�� �� ����, ����, �й�)
    TurnResult PlayCard(int playerCardIndex); // �÷��̾ ī�带 ��
    virtual TurnResult DrawCard();
    TurnResult ComTurn();     // ��ǻ���� ���� ����
    void SetForcedSuit(CardSuit suit);      // 7 ī�带 �� ��, Dlg�� ������ ����� �����ϴ� �Լ�
    int GetAttackStack() const;     // Dlg�� ���� ���� ������ ȭ�鿡 ǥ���� �� �ֵ��� ���� �˷��ִ� �Լ�

    // --- ���� ���� ���� �Լ� ---
    const std::vector<Card>& GetPlayerHand() const; // �÷��̾��� ī�� ����� ��ȯ
    const std::vector<Card>& GetComHand() const;    // ��ǻ���� ī�� ����� ��ȯ
    Card GetOpenCard() const;                // ������ ī��(�� ��)�� ��ȯ
    bool IsPlayerTurn() const; // ���� �÷��̾� ������ Ȯ���ϴ� �Լ�

protected:
    TurnResult CheckGameOver();
    void ReshuffleDeck();

    std::vector<Card> m_deck;         // ��ü ī�� ��
    std::vector<Card> m_playerHand;   // �÷��̾��� ��
    std::vector<Card> m_comHand;      // ��ǻ���� ��
    std::vector<Card> m_openPile;     // ������ ī�� ����
    bool m_isPlayerTurn; // ���� �÷��̾��� ������ ����
    int m_attackStack;                     // ������ ���� ī�� ��
    std::optional<CardSuit> m_forcedSuit;  // 7�� ���� ������ ī�� ��� (optional: ���� ���� ���� ����)
    std::optional<Card> m_attackCard;
};