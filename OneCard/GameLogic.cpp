// GameLogic.cpp
#include "pch.h"
#include "GameLogic.h"
#include "Resource.h"
#include <algorithm> // std::shuffle
#include <random>    // std::shuffle

CGameLogic::CGameLogic()
{
	m_isPlayerTurn = true; // 게임 시작 시 플레이어가 먼저 시작
}

CGameLogic::~CGameLogic()
{
    // ... 소멸자 ...
}

void CGameLogic::StartGame()
{
	// 함수 시작 시 모든 벡터를 깨끗하게 비웁니다.
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

    // 1. 모든 카드(52장)를 m_deck에 생성
	for (int i = 0; i < 4; ++i)
	{
		CardSuit suit = static_cast<CardSuit>(i);
		for (int rank = 1; rank <= 13; ++rank)
		{
			m_deck.push_back({ suit, rank, resourceIDs[i][rank - 1] });
		}
	}

    // 2. 덱 셔플
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_deck.begin(), m_deck.end(), g);

    // 3. 플레이어와 컴퓨터에게 카드 7장씩 분배
	m_playerHand.clear();
	m_comHand.clear();
	for (int i = 0; i < 7; ++i) {
		m_playerHand.push_back(m_deck.back());
		m_deck.pop_back();
		m_comHand.push_back(m_deck.back());
		m_deck.pop_back();
	}

    // 4. 첫 번째 공개된 카드 설정
	m_openPile.clear();
	m_openPile.push_back(m_deck.back());
	m_deck.pop_back();
	// 5. 플레이어가 먼저 시작
	m_isPlayerTurn = true;
}

bool CGameLogic::PlayCard(int playerCardIndex)
{
	// 1. 인덱스 유효성 검사
	if (playerCardIndex < 0 || playerCardIndex >= m_playerHand.size())
		return false;
	const Card& playedCard = m_playerHand[playerCardIndex];
	const Card& topCard = m_openPile.back();
	// 2. 카드가 유효한지 검사 (같은 무늬 또는 같은 숫자)
	if (playedCard.suit == topCard.suit || playedCard.rank == topCard.rank)
	{
		// 3. 카드 플레이
		m_openPile.push_back(playedCard);
		m_playerHand.erase(m_playerHand.begin() + playerCardIndex);
		m_isPlayerTurn = false; // 턴 종료
		return true;
	}
	return false; // 유효하지 않은 카드
}

void CGameLogic::DrawCard()
{
	if (m_deck.empty()) return; // 덱이 비었으면 아무것도 안 함
	if (!m_isPlayerTurn) return; // 플레이어 턴이 아니면 안 됨
	// 1. 덱에서 카드 한 장 뽑기
	m_playerHand.push_back(m_deck.back());
	m_deck.pop_back();
	m_isPlayerTurn = false; // 턴 종료
}

void CGameLogic::ComTurn()
{
	if (m_isPlayerTurn) return; // 컴퓨터 턴이 아님
	const Card& topCard = m_openPile.back();
	// 1. 낼 수 있는 카드 찾기
	auto it = std::find_if(m_comHand.begin(), m_comHand.end(), [&](const Card& card) {
		return card.suit == topCard.suit || card.rank == topCard.rank;
	});
	if (it != m_comHand.end())
	{
		// 2. 카드 플레이
		m_openPile.push_back(*it);
		m_comHand.erase(it);
	}
	else
	{
		// 3. 낼 카드가 없으면 뽑기
		if (!m_deck.empty())
		{
			m_comHand.push_back(m_deck.back());
			m_deck.pop_back();
		}
	}
	m_isPlayerTurn = true; // 플레이어 턴으로 변경
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
	// 실패 시 기본값 또는 에러를 나타내는 카드 반환
	return Card{ CardSuit::ERR, -1, -1 };
}