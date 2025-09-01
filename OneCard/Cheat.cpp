#include "pch.h"
#include "Cheat.h"
#include <algorithm>
#include <random>

void CCheat::SetCheatPlayer()
{
	if (cheatHand == 1) {
		p_cheatHand = &m_comHand;
	}
	else {
		p_cheatHand = &m_playerHand;
	}
}

void CCheat::StartGame()
{
	CGameLogic::StartGame();

	SetCheatPlayer();

    // ��� �÷��̾��� ī�带 �ٽ� ����
	auto& cheatHand = *p_cheatHand;
	auto& otherHand = (p_cheatHand == &m_playerHand) ? m_comHand : m_playerHand;

    auto size = p_cheatHand->size();
	m_deck.insert(m_deck.end(), p_cheatHand->begin(), p_cheatHand->end());
	p_cheatHand->clear();
	m_deck.insert(m_deck.end(), otherHand.begin(), otherHand.end());
	otherHand.clear();

	SortDeck(); // ���� ������ ����

	for (int i = 0; i < size; ++i) // ���� ī�带 ġƮ �÷��̾�� �й�
	{
        cheatHand.push_back(m_deck.back());
		m_deck.pop_back();
	}

	ShuffleDeck(); // �ٽ� ����

	for (int i = 0; i < size; ++i) // ������ ī�带 ���濡�� �й�
    {
        otherHand.push_back(m_deck.back());
        m_deck.pop_back();
    }
}

TurnResult CCheat::DrawCard()
{
    // 1. ���� ���� ġƮ �÷��̾��� ������ ���� Ȯ���մϴ�.
    std::vector<Card>& currentHand = m_isPlayerTurn ? m_playerHand : m_comHand;
    bool isCheatTurn = (p_cheatHand == &currentHand);
	if (isCheatTurn == false)
    {
        // ġƮ �÷��̾ �ƴ� ���, �⺻ DrawCard ������ ����մϴ�.
        return CGameLogic::DrawCard();
    }
    else
    {
		auto result = CGameLogic::DrawCard();

        auto size = p_cheatHand->size();

        m_deck.insert(m_deck.end(), p_cheatHand->begin(), p_cheatHand->end());
        p_cheatHand->clear();

        SortDeck();

        for (int i = 0; i < size; ++i)
        {
            p_cheatHand->push_back(m_deck.back());
            m_deck.pop_back();
        }

        ShuffleDeck();

		return result;
    }
}

void CCheat::SortDeck()
{
    std::sort(m_deck.begin(), m_deck.end(), [](const Card& a, const Card& b) {
        // ī�� ����� ����մϴ� (���ڰ� Ŭ���� ���� ī��).
        auto getCardValue = [](const Card& card) {
            if (card.suit == CardSuit::JOKER_RED) return 100;
            if (card.suit == CardSuit::JOKER_BLACK) return 90;
            if (card.rank == 1) return 80; // A
            if (card.rank == 2) return 70;
            if (card.rank == 3) return 60;
            //if (card.rank == 7) return 50;
            if (card.rank == 13) return 40; // K
            if (card.rank == 11) return 30; // J
            return card.rank; // ������ ���� ī��
            };

        // a ī�尡 b ī�庸�� �� ������ true�� ��ȯ (�������� ����)
        return getCardValue(a) < getCardValue(b);
        });
}

void CCheat::ShuffleDeck()
{
    if (m_deck.empty()) return;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_deck.begin(), m_deck.end(), g);
}