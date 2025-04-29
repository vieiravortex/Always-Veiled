#include "Renderer.h"
#include "Menu.h"
#include <iostream>
#include <sstream>

void Renderer::updateCamera(Game& game) {
	game.camera.setCenter(game.player.sprite.getPosition().x + 300.f, 300.f);
	game.camera.setSize(1024.f, 600.f);
	game.window.setView(game.camera);
}

void Renderer::updateWin(Game& game, sf::Time delta) {
	game.winMessageTimer += delta.asSeconds();
	if (game.winMessageTimer >= 2.f) {
		if (game.stationPhase == Game::StationPhase::MachinistCabin) {
			std::cout << "Win: Transitioning to Menu from MachinistCabin" << std::endl;
			Menu::initMenu(game);
		}
		else {
			std::cout << "Win: Transitioning to Choose Level from RiverLevel" << std::endl;
			Menu::initMenu(game);
			game.state = Game::GameState::Menu;
			game.menuPage = 1;
			game.winMessageTimer = 0.f;
			game.camera = game.window.getDefaultView();
			game.window.setView(game.camera);
		}
	}
}

void Renderer::updateGameOver(Game& game, sf::Time delta) {
	game.gameOverTimer += delta.asSeconds();
	if (game.gameOverTimer >= 2.f) {
		std::cout << "GameOver: Transitioning to Menu" << std::endl;
		game.resetGame();
	}
}

void Renderer::updateEntityAnimations(Game& game, sf::Time delta) {
	// Update player animation
	if (!game.player.animationFrames.empty()) {
		game.player.animationTimer += delta.asSeconds();
		if (game.player.animationTimer >= game.player.frameDuration) {
			game.player.currentFrame = (game.player.currentFrame + 1) % game.player.animationFrames.size();
			game.player.sprite.setTexture(*game.player.animationFrames[game.player.currentFrame]);
			game.player.animationTimer = 0.f;
		}
	}

	// Update goal animation
	if (!game.goal.animationFrames.empty()) {
		game.goal.animationTimer += delta.asSeconds();
		if (game.goal.animationTimer >= game.goal.frameDuration) {
			game.goal.currentFrame = (game.goal.currentFrame + 1) % game.goal.animationFrames.size();
			game.goal.sprite.setTexture(*game.goal.animationFrames[game.goal.currentFrame]);
			game.goal.animationTimer = 0.f;
		}
	}

	// Update enemies animations
	for (auto& enemy : game.enemies) {
		if (!enemy.animationFrames.empty()) {
			enemy.animationTimer += delta.asSeconds();
			if (enemy.animationTimer >= enemy.frameDuration) {
				enemy.currentFrame = (enemy.currentFrame + 1) % enemy.animationFrames.size();
				enemy.sprite.setTexture(*enemy.animationFrames[enemy.currentFrame]);
				enemy.animationTimer = 0.f;
			}
		}
	}

	// Update obstacles animations (if any)
	for (auto& obstacle : game.obstacles) {
		if (!obstacle.animationFrames.empty()) {
			obstacle.animationTimer += delta.asSeconds();
			if (obstacle.animationTimer >= obstacle.frameDuration) {
				obstacle.currentFrame = (obstacle.currentFrame + 1) % obstacle.animationFrames.size();
				obstacle.sprite.setTexture(*obstacle.animationFrames[obstacle.currentFrame]);
				obstacle.animationTimer = 0.f;
			}
		}
	}
}

void Renderer::drawScene(Game& game) {
	game.window.clear(sf::Color::Black);
	std::cout << "Drawing state: ";
	switch (game.state) {
	case Game::GameState::Menu: std::cout << "Menu"; break;
	case Game::GameState::Story: std::cout << "Story"; break;
	case Game::GameState::RiverLevel: std::cout << "RiverLevel"; break;
	case Game::GameState::StationLevel: std::cout << "StationLevel"; break;
	case Game::GameState::GameOver: std::cout << "GameOver"; break;
	case Game::GameState::Win: std::cout << "Win"; break;
	case Game::GameState::Paused: std::cout << "Paused"; break;
	case Game::GameState::QuitQuiz: std::cout << "QuitQuiz"; break;
	}
	std::cout << std::endl;

	if (game.state == Game::GameState::Story) {
		sf::VertexArray gradient(sf::Quads, 4);
		gradient[0].position = sf::Vector2f(0.f, 0.f);
		gradient[1].position = sf::Vector2f(1024.f, 0.f);
		gradient[2].position = sf::Vector2f(1024.f, 600.f);
		gradient[3].position = sf::Vector2f(0.f, 600.f);
		gradient[0].color = sf::Color::Black;
		gradient[1].color = sf::Color::Black;
		gradient[2].color = sf::Color(139, 69, 19);
		gradient[3].color = sf::Color(139, 69, 19);
		game.window.draw(gradient);

		sf::RectangleShape overlay(sf::Vector2f(610.f, 255.f));
		overlay.setFillColor(sf::Color(0, 0, 0, 200));
		overlay.setPosition(196.f, 169.f);
		game.window.draw(overlay);

		sf::Text title;
		title.setFont(game.fontRoman);
		title.setString(game.storyTitle);
		title.setCharacterSize(48);
		title.setFillColor(sf::Color::Red);
		title.setPosition(512.f - title.getGlobalBounds().width / 2.f, 100.f);
		game.window.draw(title);

		sf::Text story;
		story.setFont(game.fontLight);
		story.setString(game.storyText);
		story.setCharacterSize(24);
		story.setFillColor(sf::Color::White);
		story.setStyle(sf::Text::Regular);
		story.setPosition(212.f, 180.f);
		std::string wrappedText;
		std::string currentLine;
		std::istringstream iss(game.storyText);
		std::string word;
		while (iss >> word) {
			sf::Text temp;
			temp.setFont(game.fontLight);
			temp.setCharacterSize(24);
			temp.setString(currentLine + (currentLine.empty() ? "" : " ") + word);
			if (temp.getGlobalBounds().width > 580.f) {
				wrappedText += currentLine + "\n";
				currentLine = word;
			}
			else {
				currentLine += (currentLine.empty() ? "" : " ") + word;
			}
		}
		if (!currentLine.empty()) {
			wrappedText += currentLine;
		}
		story.setString(wrappedText);
		game.window.draw(story);

		sf::Text continueText;
		continueText.setFont(game.fontRoman);
		continueText.setString("Press any key or click to continue");
		continueText.setCharacterSize(30);
		continueText.setFillColor(sf::Color::Green);
		continueText.setPosition(512.f - continueText.getGlobalBounds().width / 2.f, 455.f);
		game.window.draw(continueText);

		sf::Text backText;
		backText.setFont(game.fontRoman);
		backText.setString("< Back");
		backText.setCharacterSize(32);
		backText.setFillColor(sf::Color::Blue);
		backText.setPosition(512.f - backText.getGlobalBounds().width / 2.f, 520.f);
		game.window.draw(backText);
	}
	else if (game.state == Game::GameState::QuitQuiz) {
		// Draw gradient background
		sf::VertexArray gradient(sf::Quads, 4);
		gradient[0].position = sf::Vector2f(0.f, 0.f);
		gradient[1].position = sf::Vector2f(1024.f, 0.f);
		gradient[2].position = sf::Vector2f(1024.f, 600.f);
		gradient[3].position = sf::Vector2f(0.f, 600.f);
		gradient[0].color = sf::Color::Black;
		gradient[1].color = sf::Color::Black;
		gradient[2].color = sf::Color(139, 69, 19);
		gradient[3].color = sf::Color(139, 69, 19);
		game.window.draw(gradient);

		// Prepare quiz text with wrapping
		std::string wrappedText;
		std::string currentLine;
		std::istringstream iss(game.quitQuizText);
		std::string word;
		int numLines = 1; // Start with 1 line
		while (iss >> word) {
			sf::Text temp;
			temp.setFont(game.fontLight);
			temp.setCharacterSize(24);
			temp.setString(currentLine + (currentLine.empty() ? "" : " ") + word);
			if (temp.getGlobalBounds().width > 580.f) {
				wrappedText += currentLine + "\n";
				currentLine = word;
				numLines++;
			}
			else {
				currentLine += (currentLine.empty() ? "" : " ") + word;
			}
		}
		if (!currentLine.empty()) {
			wrappedText += currentLine;
		}

		// Draw semi-transparent overlay with dynamic size
		float overlayHeight = 20.f + 30.f * numLines; // Adjusted base height to 20
		sf::RectangleShape overlay(sf::Vector2f(610.f, overlayHeight));
		overlay.setFillColor(sf::Color(0, 0, 0, 200));
		overlay.setPosition(207.f, 200.f - 10.f); // Centered overlay at x=207
		game.window.draw(overlay);

		// Draw quiz text, centering each line
		std::istringstream lines(wrappedText);
		std::string line;
		float yPos = 200.f;
		while (std::getline(lines, line)) {
			sf::Text lineText;
			lineText.setFont(game.fontLight);
			lineText.setString(line);
			lineText.setCharacterSize(24);
			lineText.setFillColor(sf::Color::Yellow);
			lineText.setStyle(sf::Text::Regular);
			float lineWidth = lineText.getGlobalBounds().width;
			lineText.setPosition(512.f - lineWidth / 2.f, yPos);
			game.window.draw(lineText);
			yPos += 30.f; // Move to next line
		}

		// Draw buttons or final message based on quiz step
		if (game.quitQuizStep < 3) {
			// Draw Yes/No buttons
			sf::Text yesText;
			yesText.setFont(game.fontRoman);
			yesText.setString("Yes");
			yesText.setCharacterSize(32);
			yesText.setFillColor(sf::Color::Green);
			yesText.setPosition(450.f - yesText.getGlobalBounds().width, 400.f);
			game.window.draw(yesText);

			sf::Text noText;
			noText.setFont(game.fontRoman);
			noText.setString("No");
			noText.setCharacterSize(32);
			noText.setFillColor(sf::Color::Red);
			noText.setPosition(650.f - noText.getGlobalBounds().width, 400.f);
			game.window.draw(noText);
		}
		else {
			// Draw "I have the honor" text
			sf::Text honorText;
			honorText.setFont(game.fontRoman);
			honorText.setString("I have the honor");
			honorText.setCharacterSize(32);
			honorText.setFillColor(sf::Color::Green);
			honorText.setPosition(512.f - honorText.getGlobalBounds().width / 2.f, 400.f);
			game.window.draw(honorText);
		}
	}
	else if (game.state != Game::GameState::Menu) {
		if (game.stationPhase == Game::StationPhase::MachinistCabin) {
			game.background.setPosition(game.camera.getCenter().x - 512.f, 0.f);
			game.window.draw(game.background);
		}
		else {
			float bgX1 = game.camera.getCenter().x - 512.f + game.backgroundOffsetX;
			float bgX2 = bgX1 + 1024.f;
			game.background.setPosition(bgX1, 0.f);
			game.background2.setPosition(bgX2, 0.f);
			game.window.draw(game.background);
			game.window.draw(game.background2);
		}

		game.window.draw(game.player.sprite);
		if (game.state == Game::GameState::RiverLevel || game.stationPhase == Game::StationPhase::MachinistCabin)
			game.window.draw(game.goal.sprite);
		for (const auto& enemy : game.enemies)
			game.window.draw(enemy.sprite);
		for (const auto& obstacle : game.obstacles)
			game.window.draw(obstacle.sprite);
		for (const auto& bullet : game.bullets)
			game.window.draw(bullet);
		for (const auto& bullet : game.enemyBullets)
			game.window.draw(bullet);
	}

	if (game.state == Game::GameState::Menu) {
		sf::VertexArray gradient(sf::Quads, 4);
		gradient[0].position = sf::Vector2f(0.f, 0.f);
		gradient[1].position = sf::Vector2f(1024.f, 0.f);
		gradient[2].position = sf::Vector2f(1024.f, 600.f);
		gradient[3].position = sf::Vector2f(0.f, 600.f);
		gradient[0].color = sf::Color::Black;
		gradient[1].color = sf::Color::Black;
		gradient[2].color = sf::Color(139, 69, 19);
		gradient[3].color = sf::Color(139, 69, 19);
		game.window.draw(gradient);

		sf::Text text;
		if (game.menuPage == 0) {
			text.setFont(game.fontCondensed);
			text.setString("ALWAYS VEILED");
			text.setCharacterSize(48);
			text.setFillColor(sf::Color::White);
			text.setPosition(512.f - text.getGlobalBounds().width / 2.f, 100.f);
			game.window.draw(text);

			text.setFont(game.fontRoman);
			text.setString("Start Game");
			text.setCharacterSize(32);
			text.setFillColor(sf::Color::Green);
			text.setPosition(512.f - text.getGlobalBounds().width / 2.f, 200.f);
			game.window.draw(text);

			text.setString("Quit Game");
			text.setCharacterSize(32);
			text.setFillColor(sf::Color::Red);
			text.setPosition(512.f - text.getGlobalBounds().width / 2.f, 300.f);
			game.window.draw(text);

			text.setFont(game.fontLight);
			text.setString("The game is developed by Vieira Vortex");
			text.setCharacterSize(12);
			text.setFillColor(sf::Color::White);
			text.setPosition(1019.f - text.getGlobalBounds().width, 580.f);
			game.window.draw(text);

			text.setString("v18.05.1982");
			text.setCharacterSize(12);
			text.setFillColor(sf::Color::White);
			text.setPosition(5.f, 580.f);
			game.window.draw(text);
		}
		else if (game.menuPage == 1) {
			text.setFont(game.fontBlack);
			text.setString("What do you prefer?");
			text.setCharacterSize(32);
			text.setFillColor(sf::Color::Green);
			text.setPosition(512.f - text.getGlobalBounds().width / 2.f, 100.f);
			game.window.draw(text);

			text.setFont(game.fontRoman);
			text.setString("Save the Girl");
			text.setCharacterSize(32);
			text.setFillColor(sf::Color::Yellow);
			text.setPosition(512.f - text.getGlobalBounds().width / 2.f, 200.f);
			game.window.draw(text);

			text.setString("Hijack a Train");
			text.setCharacterSize(32);
			text.setFillColor(sf::Color(255, 165, 0));
			text.setPosition(512.f - text.getGlobalBounds().width / 2.f, 300.f);
			game.window.draw(text);

			text.setString("< Back");
			text.setCharacterSize(32);
			text.setFillColor(sf::Color::Blue);
			text.setPosition(512.f - text.getGlobalBounds().width / 2.f, 400.f);
			game.window.draw(text);
		}
		std::cout << "Menu drawn: page=" << game.menuPage << std::endl;
	}
	else if (game.state == Game::GameState::RiverLevel || game.state == Game::GameState::StationLevel) {
		sf::Text text;
		text.setFont(game.fontCondensed);
		text.setString("HP: " + std::to_string(game.player.health));
		text.setCharacterSize(50);
		text.setFillColor(sf::Color::Red);
		text.setPosition(game.camera.getCenter().x - 500.f, 10.f);
		game.window.draw(text);
	}
	else if (game.state == Game::GameState::Win) {
		sf::RectangleShape overlay(sf::Vector2f(1024.f, 600.f));
		overlay.setFillColor(sf::Color(0, 0, 0, 128));
		overlay.setPosition(game.camera.getCenter().x - 512.f, 0.f);
		game.window.draw(overlay);

		sf::Text text;
		text.setFont(game.fontBlack);
		if (game.stationPhase == Game::StationPhase::MachinistCabin) {
			text.setString("You hijacked a train!");
			text.setCharacterSize(48);
			text.setFillColor(sf::Color::White);
			text.setPosition(game.camera.getCenter().x - text.getGlobalBounds().width / 2.f, 200.f);
			game.window.draw(text);
		}
		else {
			text.setString("You saved her!");
			text.setCharacterSize(48);
			text.setFillColor(sf::Color::White);
			text.setPosition(game.camera.getCenter().x - text.getGlobalBounds().width / 2.f, 200.f);
			game.window.draw(text);
		}
	}
	else if (game.state == Game::GameState::GameOver) {
		sf::RectangleShape overlay(sf::Vector2f(1024.f, 600.f));
		overlay.setFillColor(sf::Color(0, 0, 0, 128));
		overlay.setPosition(game.camera.getCenter().x - 512.f, 0.f);
		game.window.draw(overlay);

		sf::Text text;
		text.setFont(game.fontBlack);
		text.setString("Try again!");
		text.setCharacterSize(48);
		text.setFillColor(sf::Color::White);
		text.setPosition(game.camera.getCenter().x - text.getGlobalBounds().width / 2.f, 200.f);
		game.window.draw(text);
	}
	else if (game.state == Game::GameState::Paused) {
		sf::RectangleShape overlay(sf::Vector2f(1024.f, 600.f));
		overlay.setFillColor(sf::Color(0, 0, 0, 128));
		overlay.setPosition(game.camera.getCenter().x - 512.f, 0.f);
		game.window.draw(overlay);

		sf::Text text;
		text.setFont(game.fontBlack);
		text.setString("Pause");
		text.setCharacterSize(48);
		text.setFillColor(sf::Color::White);
		float pausedX = game.camera.getCenter().x - text.getGlobalBounds().width / 2.f;
		text.setPosition(pausedX, 100.f);
		game.window.draw(text);
		std::cout << "Drawing Paused at x=" << pausedX << ", y=100" << std::endl;

		sf::RectangleShape buttonBg;
		buttonBg.setFillColor(sf::Color(50, 50, 50, 200));
		buttonBg.setOutlineThickness(2.f);
		buttonBg.setOutlineColor(sf::Color::White);

		text.setFont(game.fontRoman);
		text.setString("Continue the Game");
		text.setCharacterSize(32);
		text.setFillColor(sf::Color::Green);
		float continueWidth = text.getGlobalBounds().width;
		buttonBg.setSize(sf::Vector2f(continueWidth + 20.f, 50.f));
		buttonBg.setPosition(game.camera.getCenter().x - (continueWidth + 20.f) / 2.f, 200.f);
		game.window.draw(buttonBg);
		float continueX = game.camera.getCenter().x - continueWidth / 2.f;
		text.setPosition(continueX, 210.f);
		game.window.draw(text);
		std::cout << "Drawing Continue at x=" << continueX << ", y=210" << std::endl;

		text.setString("Return to Main Menu");
		text.setFillColor(sf::Color::Red);
		float exitWidth = text.getGlobalBounds().width;
		buttonBg.setSize(sf::Vector2f(exitWidth + 20.f, 50.f));
		buttonBg.setPosition(game.camera.getCenter().x - (exitWidth + 20.f) / 2.f, 300.f);
		game.window.draw(buttonBg);
		float exitX = game.camera.getCenter().x - exitWidth / 2.f;
		text.setPosition(exitX, 310.f);
		game.window.draw(text);
		std::cout << "Drawing Exit to Menu at x=" << exitX << ", y=310" << std::endl;
	}

	game.window.display();
}