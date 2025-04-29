#pragma once
#include "Game.h"

namespace Renderer {
	void updateCamera(Game& game);
	void drawScene(Game& game);
	void updateWin(Game& game, sf::Time delta);
	void updateGameOver(Game& game, sf::Time delta);
	void updateEntityAnimations(Game& game, sf::Time delta);
}