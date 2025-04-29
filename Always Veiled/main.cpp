#include <SFML/Graphics.hpp>
#include "Game.h"
#include <iostream>

int main()
{
	sf::RenderWindow window(sf::VideoMode(1024, 600), "Always Veiled");

	sf::Image icon;
	if (!icon.loadFromFile("icon.png")) {
		std::cerr << "Failed to load app icon." << std::endl;
		return 1;
	}
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	Game game(window);
	game.run();
	return 0;
}