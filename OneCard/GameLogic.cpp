// GameLogic.cpp
#include "pch.h"
#include "GameLogic.h"
#include "Resource.h"
#include <algorithm> // std::shuffle
#include <random>    // std::shuffle

CGameLogic::CGameLogic()
{
	m_isPlayerTurn = true; // ���� ���� �� �÷��̾ ���� ����
	m_attackStack = 0;
}

CGameLogic::~CGameLogic()
{
    // ... �Ҹ��� ...
}

void CGameLogic::StartGame()
{
	// �Լ� ���� �� ��� ���͸� �����ϰ� ���ϴ�.
	m_deck.clear();
	m_playerHand.clear();
	m_comHand.clear();
	m_openPile.clear();

	int resourceIDs[4][13] = {
		{IDB_CLUB_ACE, IDB_CLUB_2, IDB_CLUB_3, IDB_CLUB_4, IDB_CLUB_5, IDB_CLUB_6,
		IDB_CLUB_7, IDB_CLUB_8, IDB_CLUB_9, IDB_CLUB_10, IDB_CLUB_JACK, IDB_CLUB_QUEEN, IDB_CLUB_KING},

		{IDB_DIAMOND_ACE, IDB_DIAMOND_2, IDB_DIAMOND_3, IDB_DIAMOND_4, IDB_DIAMOND_5, IDB_DIAMOND_6,
		IDB_DIAMOND_7, IDB_DIAMOND_8, IDB_DIAMOND_9, IDB_DIAMOND_10, IDB_DIAMOND_JACK, IDB_DIAMOND_QUEEN, IDB_DIAMOND_KING},

		{IDB_HEART_ACE, IDB_HEART_2, IDB_HEART_3, IDB_HEART_4, IDB_HEART_5, IDB_HEART_6,
		IDB_HEART_7, IDB_HEART_8, IDB_HEART_9, IDB_HEART_10, IDB_HEART_JACK, IDB_HEART_QUEEN, IDB_HEART_KING},

		{IDB_SPADE_ACE, IDB_SPADE_2, IDB_SPADE_3, IDB_SPADE_4, IDB_SPADE_5, IDB_SPADE_6,
		IDB_SPADE_7, IDB_SPADE_8, IDB_SPADE_9, IDB_SPADE_10, IDB_SPADE_JACK, IDB_SPADE_QUEEN, IDB_SPADE_KING},
	};

    // 1. ��� ī��(52��)�� m_deck�� ����
	for (int i = 0; i < 4; ++i)
	{
		CardSuit suit = static_cast<CardSuit>(i);
		for (int rank = 1; rank <= 13; ++rank)
		{
			m_deck.push_back({ suit, rank, resourceIDs[i][rank - 1] });
		}
	}

	// ��Ŀ 2�� �߰�
	m_deck.push_back({ CardSuit::JOKER_BLACK, 20, IDB_JOKER_BLACK });
	m_deck.push_back({ CardSuit::JOKER_RED, 21, IDB_JOKER_RED });

	m_attackStack = 0;
	m_forcedSuit.reset();
    m_attackCard.reset();

    // 2. �� ����
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_deck.begin(), m_deck.end(), g);

    // 3. �÷��̾�� ��ǻ�Ϳ��� ī�� 7�徿 �й�
	m_playerHand.clear();
	m_comHand.clear();
	for (int i = 0; i < 7; ++i) {
		m_playerHand.push_back(m_deck.back());
		m_deck.pop_back();
		m_comHand.push_back(m_deck.back());
		m_deck.pop_back();
	}

    // 4. ù ��° ������ ī�� ���� (Ư�� ī�尡 �ƴ� ������ �ݺ�)
    m_openPile.clear();
    while (true)
    {
        Card firstCard = m_deck.back();
        m_deck.pop_back();

        // A(1), 2, 7, ��Ŀ(20, 21)�� �ƴϸ� ���� �߰��ϰ� ���� Ż��
        if (firstCard.rank != 1 && firstCard.rank != 2 && firstCard.rank != 7 &&
            firstCard.rank != 20 && firstCard.rank != 21)
        {
            m_openPile.push_back(firstCard);
            break;
        }
        else
        {
            // Ư�� ī���̸� ���� �� �Ʒ��� �ٽ� �ְ� ���� (�Ǵ� �׳� ����)
            m_deck.insert(m_deck.begin(), firstCard);
        }
    }

	// 5. �÷��̾ ���� ����
	m_isPlayerTurn = true;
}

TurnResult CGameLogic::PlayCard(int cardIndex)
{
    std::vector<Card>& currentHand = m_isPlayerTurn ? m_playerHand : m_comHand;

    if (cardIndex < 0 || cardIndex >= currentHand.size())
        return TurnResult::INVALID_PLAY;

    Card playedCard = currentHand[cardIndex];
    Card topCard = GetOpenCard();
    bool isPlayedCardJoker = playedCard.suit == CardSuit::JOKER_BLACK || playedCard.suit == CardSuit::JOKER_RED;
    bool isValidPlay = false;
    if (m_attackStack > 0) // ���ݹ޴� ��Ȳ
    {
        // optional�� ���� �ִ���, �� ���� ī�尡 �����Ǿ� �ִ��� Ȯ��
        if (m_attackCard.has_value())
        {
            CardSuit attackSuit = m_attackCard.value().suit;

            // 1-1. ���� ��Ŀ�� ������ ���� �� ���� (� ī�嵵 �� �� ����)
            if (attackSuit == CardSuit::JOKER_RED) {
                isValidPlay = false;
            }
            // 1-2. �� ��Ŀ�� ������ �ٸ� ��Ŀ�θ� ��� ����
            else if (attackSuit == CardSuit::JOKER_BLACK) {
                if (isPlayedCardJoker) isValidPlay = true;
            }
            // 1-3. �Ϲ� ī��(A, 2)�� ����
            else {
                // 3(���), 2/A(�ݰ�), ��Ŀ(�ݰ�) ī��� ��� ����
                if (playedCard.rank == 3 || playedCard.rank == 1 || playedCard.rank == 2 || isPlayedCardJoker) {
                    isValidPlay = true;
                }
            }
        }
    }
    else // �Ϲ� ��Ȳ
    {
        // ��Ŀ�� ������ ����, �Ǵ� ������ ���, �Ǵ� �⺻ ��Ģ(���� ���/����)
        if (isPlayedCardJoker ||
            (m_forcedSuit.has_value() && playedCard.suit == m_forcedSuit.value()) ||
            (!m_forcedSuit.has_value() && (playedCard.suit == topCard.suit || playedCard.rank == topCard.rank)))
        {
            isValidPlay = true;
        }
    }

    if (!isValidPlay) {
        return TurnResult::INVALID_PLAY;
    }

    // --- 3. ī�� �̵� ---
    m_openPile.push_back(playedCard);
    currentHand.erase(currentHand.begin() + cardIndex); // currentHand���� ī�� ����
    m_forcedSuit.reset();

    // --- 4. Ư�� ȿ�� �� �� ó�� ---
    bool keepTurn = false;
    switch (playedCard.rank)
    {
    case 1:  m_attackStack += 3; m_attackCard = playedCard; break;
    case 2:  m_attackStack += 2; m_attackCard = playedCard; break;
    case 3:
        m_attackStack = 0;
        m_attackCard.reset();
        break;
    case 7:
        // 7�� �� ����� �÷��̾��� ���� ��� ������ �ʿ��մϴ�.
        // ��ǻ�ʹ� ������ ����� �����ؾ� �մϴ� (AI ���� �߰� �ʿ�).
        // �ϴ� �� �Լ��� ȣ���ߴٴ� ���� ����� �̹� �����Ǿ��ٰ� �����մϴ�.
        break;
    case 11: // J (Jack)
    case 13: // K (King)
        keepTurn = true;
        break;
    case 20: // Black Joker
        m_attackStack += 5;  m_attackCard = playedCard; break;
    case 21: // Red Joker
        m_attackStack += 10; m_attackCard = playedCard; break;
    }

    // ���� �ѱ��� ����
    if (!keepTurn) {
        // 7�� �� �÷��̾�� ��� ������ ���� ���� �����ؾ� ��
        if (playedCard.rank == 7 && m_isPlayerTurn) {
            return TurnResult::REQUIRE_SUIT_CHOICE;
        }
        m_isPlayerTurn = !m_isPlayerTurn;
    }

    return CheckGameOver();
}

TurnResult CGameLogic::DrawCard()
{
    std::vector<Card>& currentHand = m_isPlayerTurn ? m_playerHand : m_comHand;

    // 1. ���� ���� ó��
    if (m_attackStack > 0)
    {
        for (int i = 0; i < m_attackStack; ++i) {
            if (m_deck.empty()) {
                ReshuffleDeck(); // ���� ������� ������
                // ������ �Ŀ��� ���� ����ִٸ� (���� ī�尡 �����ٸ�) �ߴ�
                if (m_deck.empty()) break;
            }
            currentHand.push_back(m_deck.back());
            m_deck.pop_back();
        }
        m_attackStack = 0;
        m_attackCard.reset();
    }
    // 2. �Ϲ� ��Ȳ ó��
    else
    {
        if (m_deck.empty()) {
            ReshuffleDeck(); // ���� ������� ������
        }
        // ������ �Ŀ��� ���� ��������� ī�带 ���� �� �����Ƿ� �ϸ� �ѱ�
        if (m_deck.empty()) {
            m_isPlayerTurn = !m_isPlayerTurn;
            return CheckGameOver();
        }
        currentHand.push_back(m_deck.back());
        m_deck.pop_back();
    }

    // 3. ī�带 �̰� ���� ���� ���� ������� �ѱ�ϴ�.
    m_isPlayerTurn = !m_isPlayerTurn;
    return CheckGameOver();
}

// ��ǻ�� �� ���� (AI)
TurnResult CGameLogic::ComTurn()
{
    if (m_isPlayerTurn) return TurnResult::INVALID_PLAY; // ��ǻ�� ���� �ƴϸ� ��� ��ȯ

    const Card topCard = GetOpenCard();
    int bestCardIndex = -1;
    bool keepTurn = false; // J, K�� ���� �� ���� �������� �����ϴ� �÷���

    // --- 1. AI: � ī�带 ���� ���� ---
    // 1-1. ���ݹ޴� ��Ȳ�� ��
    if (m_attackStack > 0)
    {
        // 3(���) ī�带 �ֿ켱���� ã��
        for (int i = 0; i < m_comHand.size(); ++i) {
            if (m_comHand[i].rank == 3) {
                bestCardIndex = i;
                break;
            }
        }
        // ��� ī�尡 ������, �ݰ��� �� �ִ� ���� ī�带 ã��
        if (bestCardIndex == -1) {
            for (int i = 0; i < m_comHand.size(); ++i) {
                if (m_comHand[i].rank == 1 || m_comHand[i].rank == 2 ||
                    m_comHand[i].suit == CardSuit::JOKER_BLACK || m_comHand[i].suit == CardSuit::JOKER_RED) {
                    bestCardIndex = i;
                    break;
                }
            }
        }
    }
    // 1-2. �Ϲ� ��Ȳ�� ��
    else
    {
        for (int i = 0; i < m_comHand.size(); ++i)
        {
            const Card& comCard = m_comHand[i];
            bool isJoker = comCard.suit == CardSuit::JOKER_BLACK || comCard.suit == CardSuit::JOKER_RED;

            // �� �� �ִ� ī������ Ȯ��
            if (isJoker ||
                (m_forcedSuit.has_value() && comCard.suit == m_forcedSuit.value()) ||
                (!m_forcedSuit.has_value() && (comCard.suit == topCard.suit || comCard.rank == topCard.rank)))
            {
                bestCardIndex = i;
                break; // ���� ���� ã�� ī�带 �� (AI �ܼ�ȭ)
            }
        }
    }

    // 2. �� ī�尡 �ִ� ���
    if (bestCardIndex != -1)
    {
        return PlayCard(bestCardIndex);
    }
    // 3. �� ī�尡 ���� ���
    else
    {
        return DrawCard();
    }
}

const std::vector<Card>& CGameLogic::GetPlayerHand() const
{
	return m_playerHand;
}

const std::vector<Card>& CGameLogic::GetComHand() const
{
	return m_comHand;
}

Card CGameLogic::GetOpenCard() const
{
	if (!m_openPile.empty())
	{
		return m_openPile.back();
	}
	// ���� �� �⺻�� �Ǵ� ������ ��Ÿ���� ī�� ��ȯ
	return Card{ CardSuit::ERR, -1, -1 };
}

bool CGameLogic::IsPlayerTurn() const
{
	return m_isPlayerTurn;
}

TurnResult CGameLogic::CheckGameOver()
{
	if (m_playerHand.empty() || m_comHand.size() >= 20)
	{
		return TurnResult::GAME_OVER_WIN;
	}
	if (m_comHand.empty() || m_playerHand.size() >= 20)
	{
		return TurnResult::GAME_OVER_LOSE;
	}
	return TurnResult::SUCCESS;
}

// 7�� ���� �� Dlg�� ȣ�����ִ� �Լ�
void CGameLogic::SetForcedSuit(CardSuit suit)
{
	m_forcedSuit = suit;
	m_isPlayerTurn = false; // ���� ���� �ѱ�
}

int CGameLogic::GetAttackStack() const
{
	return m_attackStack;
}

void CGameLogic::ReshuffleDeck()
{
    // 1. ������ ī�� ���̿� ���� ī�尡 �ִ��� Ȯ�� (1�� ���ϸ� ���� �� ����)
    if (m_openPile.size() <= 1)
    {
        return; // �������� ī�尡 ����
    }

    // 2. �� �� ī�带 ���� �����մϴ�.
    Card topCard = m_openPile.back();
    m_openPile.pop_back();

    // 3. ������ ī����� m_openPile���� m_deck���� ��� �ű�ϴ�.
    //    m_deck�� ���� m_openPile�� ���ۺ��� �������� ��� ��Ҹ� �����մϴ�.
    m_deck.insert(m_deck.end(), m_openPile.begin(), m_openPile.end());

    // 4. ���� m_openPile�� ���ϴ�.
    m_openPile.clear();

    // 5. �����ߴ� �� �� ī�带 �ٽ� m_openPile�� �ֽ��ϴ�.
    m_openPile.push_back(topCard);

    // 6. ���� ä���� ���� �ٽ� �����ϴ�.
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_deck.begin(), m_deck.end(), g);

    // TRACE ��ũ�δ� ����� ����� �� '���' â�� �޽����� �����༭
    // �������� ���� �Ͼ���� Ȯ���ϱ� �����ϴ�.
    TRACE("--- Deck has been reshuffled. ---\n");
}