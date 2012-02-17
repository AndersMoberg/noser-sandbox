// BattleState.cpp
// Nolan Check
// Created 2/15/2012

#include "BattleState.hpp"

#include <iostream>

BattleState::BattleState()
	: m_state(PLAYER_TURN),
	m_playerHp(100),
	m_monsterHp(100)
{ }

BattleStatePtr BattleState::Create()
{
	BattleStatePtr p(new BattleState);

	return p;
}

void BattleState::Run()
{
	bool done = false;
	while (!done)
	{
		switch (m_state)
		{
		case PLAYER_TURN:
			PlayerTurn();
			break;
		case MONSTER_TURN:
			MonsterTurn();
			break;
		case QUIT:
			done = true;
			break;
		}

		// Move to next turn
		switch (m_state)
		{
		case PLAYER_TURN:
			m_state = MONSTER_TURN;
			break;
		case MONSTER_TURN:
			m_state = PLAYER_TURN;
			break;
		}
	}
}

void BattleState::PlayerTurn()
{
	std::cout << "Choose action:" << std::endl
		<< "1) Attack" << std::endl
		<< "q) Quit" << std::endl
		<< '>';

	char choice;
	std::cin >> choice;
	switch (choice)
	{
	case '1':
		std::cout << "You attack! 25 hp damage." << std::endl;
		DealMonsterDamage(25);
		break;
	case 'Q':
	case 'q':
		m_state = QUIT;
		break;
	}
}

void BattleState::MonsterTurn()
{
	std::cout << "Monster attacks! 10 hp damage." << std::endl;
	DealPlayerDamage(10);
}

void BattleState::DealPlayerDamage(int hp)
{
	m_playerHp -= hp;
	if (m_playerHp <= 0)
	{
		std::cout << "You were defeated." << std::endl;
		m_state = QUIT;
	}
}

void BattleState::DealMonsterDamage(int hp)
{
	m_monsterHp -= hp;
	if (m_monsterHp <= 0)
	{
		std::cout << "You defeated the monster!" << std::endl;
		m_state = QUIT;
	}
}
