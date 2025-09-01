#pragma once

#include "GameLogic.h"

static int cheatHand = 1;

class CCheat : public CGameLogic
{
public:
	CCheat() : CGameLogic() {}
	~CCheat() {}

	void StartGame() override;
	TurnResult DrawCard() override;

private:
	void SetCheatPlayer();
	void SortDeck();
	void ShuffleDeck();

private:
	std::vector<Card>* p_cheatHand;
};