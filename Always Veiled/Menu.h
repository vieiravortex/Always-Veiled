#pragma once
#include "Game.h"

namespace Menu {
	void initMenu(Game& game);
	void updateMenu(Game& game, sf::Time delta);
	void handleMenuClick(Game& game, sf::Vector2f mousePos);
}