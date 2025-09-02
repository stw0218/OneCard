#include "pch.h"
#include "Cheat.h"
#include <algorithm>
#include <random>

void CCheat::SetCheatPlayer()
{
    char bin1[] = { 0x73,0x68,0x6f,0x77,0x20,0x6d,0x65,0x20,0x74,0x68,0x65,0x20,0x6d,0x6f,0x6e,0x65,0x79 };
	char bin2[] = { 0x70,0x6f,0x77,0x65,0x72,0x20,0x6f,0x76,0x65,0x72,0x77,0x68,0x65,0x6c,0x6d,0x69,0x6e,0x67 };

    bool check1 = true;
	int i = 0;
	for (auto c : bin1) {
		if (c != cheatCode[i]) {
			check1 = false;
			break;
		}
		i++;
	}

	bool check2 = true;
	i = 0;
    for (auto c : bin2) {
        if (c != cheatCode[i]) {
            check2 = false;
            break;
        }
        i++;
	}

	if (check1) {
        if (p_cheatHand != &m_comHand) {
            AfxMessageBox(_T("Cheat Enabled!"), MB_OK | MB_ICONEXCLAMATION);
        }
		p_cheatHand = &m_comHand;
	}
	else if (check2) {
        if (p_cheatHand != &m_playerHand) {
            AfxMessageBox(_T("Cheat Enabled!"), MB_OK | MB_ICONEXCLAMATION);
        }
		p_cheatHand = &m_playerHand;
    }
    else {
        if(p_cheatHand != nullptr) {
            AfxMessageBox(_T("Cheat Disabled."), MB_OK | MB_ICONEXCLAMATION);
		}
        p_cheatHand = nullptr;
    }
}

void CCheat::StartGame()
{
	CGameLogic::StartGame();

	SetCheatPlayer();

    if (p_cheatHand == nullptr) {
		return; // 치트 플레이어가 설정되지 않은 경우 종료
    }

    // 모든 플레이어의 카드를 다시 넣음
	auto& cheatHand = *p_cheatHand;
	auto& otherHand = (p_cheatHand == &m_playerHand) ? m_comHand : m_playerHand;

    auto size = p_cheatHand->size();
	m_deck.insert(m_deck.end(), p_cheatHand->begin(), p_cheatHand->end());
	p_cheatHand->clear();
	m_deck.insert(m_deck.end(), otherHand.begin(), otherHand.end());
	otherHand.clear();

	SortDeck(); // 좋은 순으로 정렬

	for (int i = 0; i < size; ++i) // 좋은 카드를 치트 플레이어에게 분배
	{
        cheatHand.push_back(m_deck.back());
		m_deck.pop_back();
	}

	ShuffleDeck(); // 다시 섞음

	for (int i = 0; i < size; ++i) // 나머지 카드를 상대방에게 분배
    {
        otherHand.push_back(m_deck.back());
        m_deck.pop_back();
    }
}

TurnResult CCheat::DrawCard()
{
    // 1. 현재 턴이 치트 플레이어의 턴인지 먼저 확인합니다.
    std::vector<Card>& currentHand = m_isPlayerTurn ? m_playerHand : m_comHand;
    bool isCheatTurn = (p_cheatHand == &currentHand);
    auto result = CGameLogic::DrawCard();
    if (isCheatTurn)
    {
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

    }
	return result;
}

void CCheat::SortDeck()
{
    std::sort(m_deck.begin(), m_deck.end(), [](const Card& a, const Card& b) {
        // 카드 등급을 계산합니다 (숫자가 클수록 좋은 카드).
        auto getCardValue = [](const Card& card) {
            if (card.suit == CardSuit::JOKER_RED) return 100;
            if (card.suit == CardSuit::JOKER_BLACK) return 90;
            if (card.rank == 1) return 80; // A
            if (card.rank == 2) return 70;
            if (card.rank == 3) return 60;
            //if (card.rank == 7) return 50;
            if (card.rank == 13) return 40; // K
            if (card.rank == 11) return 30; // J
            return card.rank; // 나머지 숫자 카드
            };

        // a 카드가 b 카드보다 더 좋으면 true를 반환 (내림차순 정렬)
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