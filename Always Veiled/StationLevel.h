#ifndef STATION_LEVEL_H
#define STATION_LEVEL_H

#include "Game.h"

class StationLevel {
public:
	static void initStationLevel(Game& game);
	static void initMachinistCabin(Game& game);
	static void updateStationLevel(Game& game, sf::Time delta);
private:
	static int rightmostEnemyIndex; // Index of the rightmost enemy
	static float phaseTransitionTimer; // Timer for phase transition delay
	static bool pendingPhaseTransition; // Flag for pending transition
	static Game::StationPhase nextPhase; // Next phase to transition to
};

#endif // STATION_LEVEL_H