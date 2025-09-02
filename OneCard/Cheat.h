#pragma once

#include "GameLogic.h"

class CCheat : public CGameLogic
{
public:
	CCheat() : CGameLogic(), p_cheatHand(nullptr) {}
	~CCheat() {}

	void StartGame() override;
	TurnResult DrawCard() override;

	const char cheatCode[32] = "show me the money";

private:
	void SetCheatPlayer();
	void SortDeck();
	void ShuffleDeck();

private:
	std::vector<Card>* p_cheatHand;
};