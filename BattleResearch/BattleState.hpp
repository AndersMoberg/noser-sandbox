// BattleState.hpp
// Nolan Check
// Created 2/15/2012

#ifndef _BATTLESTATE_HPP
#define _BATTLESTATE_HPP

#include <memory>

class BattleState;
typedef std::shared_ptr<BattleState> BattleStatePtr;

class BattleState
{

public:

	static BattleStatePtr Create();

	void Run();

private:

	BattleState();

	void PlayerTurn();
	void MonsterTurn();

	void DealPlayerDamage(int hp);
	void DealMonsterDamage(int hp);

	enum State
	{
		PLAYER_TURN,
		MONSTER_TURN,
		QUIT
	};
	State m_state;

	int m_playerHp;
	int m_monsterHp;

};

#endif
