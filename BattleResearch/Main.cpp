// Main.cpp
// Nolan Check
// Created 2/15/2012

#include <cstdlib>

#include "BattleState.hpp"

int main()
{
	BattleStatePtr battle = BattleState::Create();
	battle->Run();

	system("pause");
	return EXIT_SUCCESS;
}
