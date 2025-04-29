#include "Menu.h"
#include "RiverLevel.h"
#include "StationLevel.h"
#include <iostream>

void Menu::initMenu(Game& game) {
	game.state = Game::GameState::Menu;
	game.menuPage = 0;
	game.player.sprite.setPosition(0, 0);
	game.player.health = 10;
	game.player.velocity = sf::Vector2f(0.f, 0.f);
	game.player.invincibilityTimer = 0.f;
	game.enemies.clear();
	game.obstacles.clear();
	game.bullets.clear();
	game.enemyBullets.clear();
	game.winMessageTimer = 0.f;
	game.gameOverTimer = 0.f;
	game.backgroundOffsetX = 0.f;
	game.levelProgress = 0.f;
	game.spawnTimer = 0.f;
	game.shootTimer = 0.f;
	game.bossActive = false;
	game.stationPhase = Game::StationPhase::Platform;
	game.camera = game.window.getDefaultView();
	game.window.setView(game.camera);
	std::cout << "Initialized Menu state, view reset" << std::endl;
}

void Menu::updateMenu(Game& game, sf::Time delta) {
	// Placeholder for future menu animations if needed
}

void Menu::handleMenuClick(Game& game, sf::Vector2f mousePos) {
	if (game.state == Game::GameState::Menu) {
		if (game.menuPage == 0) {
			if (mousePos.x >= 400 && mousePos.x <= 600 && mousePos.y >= 200 && mousePos.y <= 250) {
				game.menuPage = 1;
				std::cout << "Play clicked, menuPage=1" << std::endl;
			}
			if (mousePos.x >= 400 && mousePos.x <= 600 && mousePos.y >= 300 && mousePos.y <= 350) {
				// Start quit quiz instead of closing window
				game.state = Game::GameState::QuitQuiz;
				game.quitQuizStep = 0;
				game.quitQuizKeyPressed = false;
				game.quitQuizText = "Lepidoptera, Pieridae. It calls us.";
				std::cout << "Quit Game clicked, starting QuitQuiz" << std::endl;
			}
		}
		else if (game.menuPage == 1) {
			if (mousePos.x >= 400 && mousePos.x <= 600 && mousePos.y >= 200 && mousePos.y <= 250) {
				// Show RiverLevel story before starting
				game.state = Game::GameState::Story;
				game.nextLevelState = Game::GameState::RiverLevel;
				game.storyTitle = "The Good Mother";
				game.storyText = "During the festival on May 18, 14-year-old Anla was enjoying a peaceful boat ride on the river when he saw a charming 8-year-old girl, the daughter of one of the festival participants, fall from the bank into the water. Ignoring the indifference of other boatmen, sharp underwater rocks and piranhas hiding in the river, Anla bravely rushed to the aid of the girl to return the little one to her mother.";
				game.storyTimer = 0.f;
				game.storyKeyPressed = false;
				std::cout << "River Level story selected" << std::endl;
			}
			if (game.machinistTex.getSize().x != 0 && mousePos.x >= 400 && mousePos.x <= 600 && mousePos.y >= 300 && mousePos.y <= 350) {
				// Show StationLevel story before starting
				game.state = Game::GameState::Story;
				game.nextLevelState = Game::GameState::StationLevel;
				game.storyTitle = "I Have the Honor";
				game.storyText = "In a turbulent time of reaction and persecution, 27-year-old Parvis accidentally leaves a book on a park bench, which causes the gendarmes to arrest his free-thinking friend Walton. Parvis gives his word of honor that he will free his unjustly convicted friend, and, without wasting time, decides to hijack a train with prisoners on which the gendarmes are going to take Walton to a political prison.";
				game.storyTimer = 0.f;
				game.storyKeyPressed = false;
				std::cout << "Train Level story selected" << std::endl;
			}
			if (mousePos.x >= 400 && mousePos.x <= 600 && mousePos.y >= 400 && mousePos.y <= 450) {
				game.menuPage = 0;
				std::cout << "Back clicked, menuPage=0" << std::endl;
			}
		}
	}
}