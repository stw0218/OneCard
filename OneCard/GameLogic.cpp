// GameLogic.cpp
#include "pch.h"
#include "GameLogic.h"
#include "Resource.h"
#include <algorithm> // std::shuffle
#include <random>    // std::shuffle

CGameLogic::CGameLogic()
{
	m_isPlayerTurn = true; // 게임 시작 시 플레이어가 먼저 시작
	m_attackStack = 0;
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

	// 조커 2장 추가
	m_deck.push_back({ CardSuit::JOKER_BLACK, 20, IDB_JOKER_BLACK });
	m_deck.push_back({ CardSuit::JOKER_RED, 21, IDB_JOKER_RED });

	m_attackStack = 0;
	m_forcedSuit.reset();
    m_attackCard.reset();

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

    // 4. 첫 번째 공개된 카드 설정 (특수 카드가 아닐 때까지 반복)
    m_openPile.clear();
    while (true)
    {
        Card firstCard = m_deck.back();
        m_deck.pop_back();

        // A(1), 2, 7, 조커(20, 21)가 아니면 덱에 추가하고 루프 탈출
        if (firstCard.rank != 1 && firstCard.rank != 2 && firstCard.rank != 7 &&
            firstCard.rank != 20 && firstCard.rank != 21)
        {
            m_openPile.push_back(firstCard);
            break;
        }
        else
        {
            // 특수 카드이면 덱의 맨 아래에 다시 넣고 섞음 (또는 그냥 버림)
            m_deck.insert(m_deck.begin(), firstCard);
        }
    }

	// 5. 플레이어가 먼저 시작
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
    if (m_attackStack > 0) // 공격받는 상황
    {
        // optional에 값이 있는지, 즉 공격 카드가 설정되어 있는지 확인
        if (m_attackCard.has_value())
        {
            CardSuit attackSuit = m_attackCard.value().suit;

            // 1-1. 레드 조커의 공격은 막을 수 없음 (어떤 카드도 낼 수 없음)
            if (attackSuit == CardSuit::JOKER_RED) {
                isValidPlay = false;
            }
            // 1-2. 블랙 조커의 공격은 다른 조커로만 방어 가능
            else if (attackSuit == CardSuit::JOKER_BLACK) {
                if (isPlayedCardJoker) isValidPlay = true;
            }
            // 1-3. 일반 카드(A, 2)의 공격
            else {
                // 3(방어), 2/A(반격), 조커(반격) 카드로 방어 가능
                if (playedCard.rank == 3 || playedCard.rank == 1 || playedCard.rank == 2 || isPlayedCardJoker) {
                    isValidPlay = true;
                }
            }
        }
    }
    else // 일반 상황
    {
        // 조커는 언제나 가능, 또는 강제된 모양, 또는 기본 규칙(같은 모양/숫자)
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

    // --- 3. 카드 이동 ---
    m_openPile.push_back(playedCard);
    currentHand.erase(currentHand.begin() + cardIndex); // currentHand에서 카드 제거
    m_forcedSuit.reset();

    // --- 4. 특수 효과 및 턴 처리 ---
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
        // 7을 낸 사람이 플레이어일 때만 모양 선택이 필요합니다.
        // 컴퓨터는 스스로 모양을 선택해야 합니다 (AI 로직 추가 필요).
        // 일단 이 함수를 호출했다는 것은 모양이 이미 결정되었다고 가정합니다.
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

    // 턴을 넘길지 결정
    if (!keepTurn) {
        // 7을 낸 플레이어는 모양 선택을 위해 턴을 유지해야 함
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

    // 1. 공격 스택 처리
    if (m_attackStack > 0)
    {
        for (int i = 0; i < m_attackStack; ++i) {
            if (m_deck.empty()) {
                ReshuffleDeck(); // 덱이 비었으면 리셔플
                // 리셔플 후에도 덱이 비어있다면 (섞을 카드가 없었다면) 중단
                if (m_deck.empty()) break;
            }
            currentHand.push_back(m_deck.back());
            m_deck.pop_back();
        }
        m_attackStack = 0;
        m_attackCard.reset();
    }
    // 2. 일반 상황 처리
    else
    {
        if (m_deck.empty()) {
            ReshuffleDeck(); // 덱이 비었으면 리셔플
        }
        // 리셔플 후에도 덱이 비어있으면 카드를 뽑을 수 없으므로 턴만 넘김
        if (m_deck.empty()) {
            m_isPlayerTurn = !m_isPlayerTurn;
            return CheckGameOver();
        }
        currentHand.push_back(m_deck.back());
        m_deck.pop_back();
    }

    // 3. 카드를 뽑고 나면 턴을 다음 사람에게 넘깁니다.
    m_isPlayerTurn = !m_isPlayerTurn;
    return CheckGameOver();
}

// 컴퓨터 턴 로직 (AI)
TurnResult CGameLogic::ComTurn()
{
    if (m_isPlayerTurn) return TurnResult::INVALID_PLAY; // 컴퓨터 턴이 아니면 즉시 반환

    const Card topCard = GetOpenCard();
    int bestCardIndex = -1;
    bool keepTurn = false; // J, K를 냈을 때 턴을 유지할지 결정하는 플래그

    // --- 1. AI: 어떤 카드를 낼지 결정 ---
    // 1-1. 공격받는 상황일 때
    if (m_attackStack > 0)
    {
        // 3(방어) 카드를 최우선으로 찾음
        for (int i = 0; i < m_comHand.size(); ++i) {
            if (m_comHand[i].rank == 3) {
                bestCardIndex = i;
                break;
            }
        }
        // 방어 카드가 없으면, 반격할 수 있는 공격 카드를 찾음
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
    // 1-2. 일반 상황일 때
    else
    {
        for (int i = 0; i < m_comHand.size(); ++i)
        {
            const Card& comCard = m_comHand[i];
            bool isJoker = comCard.suit == CardSuit::JOKER_BLACK || comCard.suit == CardSuit::JOKER_RED;

            // 낼 수 있는 카드인지 확인
            if (isJoker ||
                (m_forcedSuit.has_value() && comCard.suit == m_forcedSuit.value()) ||
                (!m_forcedSuit.has_value() && (comCard.suit == topCard.suit || comCard.rank == topCard.rank)))
            {
                bestCardIndex = i;
                break; // 가장 먼저 찾은 카드를 냄 (AI 단순화)
            }
        }
    }

    // 2. 낼 카드가 있는 경우
    if (bestCardIndex != -1)
    {
        return PlayCard(bestCardIndex);
    }
    // 3. 낼 카드가 없는 경우
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
	// 실패 시 기본값 또는 에러를 나타내는 카드 반환
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

// 7을 냈을 때 Dlg가 호출해주는 함수
void CGameLogic::SetForcedSuit(CardSuit suit)
{
	m_forcedSuit = suit;
	m_isPlayerTurn = false; // 이제 턴을 넘김
}

int CGameLogic::GetAttackStack() const
{
	return m_attackStack;
}

void CGameLogic::ReshuffleDeck()
{
    // 1. 공개된 카드 더미에 섞을 카드가 있는지 확인 (1장 이하면 섞을 수 없음)
    if (m_openPile.size() <= 1)
    {
        return; // 리셔플할 카드가 없음
    }

    // 2. 맨 위 카드를 따로 보관합니다.
    Card topCard = m_openPile.back();
    m_openPile.pop_back();

    // 3. 나머지 카드들을 m_openPile에서 m_deck으로 모두 옮깁니다.
    //    m_deck의 끝에 m_openPile의 시작부터 끝까지의 모든 요소를 삽입합니다.
    m_deck.insert(m_deck.end(), m_openPile.begin(), m_openPile.end());

    // 4. 이제 m_openPile은 비웁니다.
    m_openPile.clear();

    // 5. 보관했던 맨 위 카드를 다시 m_openPile에 넣습니다.
    m_openPile.push_back(topCard);

    // 6. 새로 채워진 덱을 다시 섞습니다.
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_deck.begin(), m_deck.end(), g);

    // TRACE 매크로는 디버그 모드일 때 '출력' 창에 메시지를 보여줘서
    // 리셔플이 언제 일어났는지 확인하기 좋습니다.
    TRACE("--- Deck has been reshuffled. ---\n");
}