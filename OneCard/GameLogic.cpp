// GameLogic.cpp
#include "pch.h"
#include "GameLogic.h"
#include "Resource.h"
#include <algorithm> // std::shuffle
#include <random>    // std::shuffle

CGameLogic::CGameLogic()
{
	m_isPlayerTurn = true; // ���� ���� �� �÷��̾ ���� ����
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

    // 4. ù ��° ������ ī�� ����
	m_openPile.clear();
	m_openPile.push_back(m_deck.back());
	m_deck.pop_back();
	// 5. �÷��̾ ���� ����
	m_isPlayerTurn = true;
}

bool CGameLogic::PlayCard(int playerCardIndex)
{
	// 1. �ε��� ��ȿ�� �˻�
	if (playerCardIndex < 0 || playerCardIndex >= m_playerHand.size())
		return false;
	const Card& playedCard = m_playerHand[playerCardIndex];
	const Card& topCard = m_openPile.back();
	// 2. ī�尡 ��ȿ���� �˻� (���� ���� �Ǵ� ���� ����)
	if (playedCard.suit == topCard.suit || playedCard.rank == topCard.rank)
	{
		// 3. ī�� �÷���
		m_openPile.push_back(playedCard);
		m_playerHand.erase(m_playerHand.begin() + playerCardIndex);
		m_isPlayerTurn = false; // �� ����
		return true;
	}
	return false; // ��ȿ���� ���� ī��
}

void CGameLogic::DrawCard()
{
	if (m_deck.empty()) return; // ���� ������� �ƹ��͵� �� ��
	if (!m_isPlayerTurn) return; // �÷��̾� ���� �ƴϸ� �� ��
	// 1. ������ ī�� �� �� �̱�
	m_playerHand.push_back(m_deck.back());
	m_deck.pop_back();
	m_isPlayerTurn = false; // �� ����
}

void CGameLogic::ComTurn()
{
	if (m_isPlayerTurn) return; // ��ǻ�� ���� �ƴ�
	const Card& topCard = m_openPile.back();
	// 1. �� �� �ִ� ī�� ã��
	auto it = std::find_if(m_comHand.begin(), m_comHand.end(), [&](const Card& card) {
		return card.suit == topCard.suit || card.rank == topCard.rank;
	});
	if (it != m_comHand.end())
	{
		// 2. ī�� �÷���
		m_openPile.push_back(*it);
		m_comHand.erase(it);
	}
	else
	{
		// 3. �� ī�尡 ������ �̱�
		if (!m_deck.empty())
		{
			m_comHand.push_back(m_deck.back());
			m_deck.pop_back();
		}
	}
	m_isPlayerTurn = true; // �÷��̾� ������ ����
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