#include "Game.h"
#include "Menu.h"
#include "RiverLevel.h"
#include "StationLevel.h"
#include "Renderer.h"
#include <iostream>

Game::Game(sf::RenderWindow& win)
	: window(win), state(GameState::Menu), stationPhase(StationPhase::Platform),
	levelProgress(0.f), spawnTimer(0.f), shootTimer(0.f), bossActive(false),
	levelLength(3072.f), menuPage(0), winMessageTimer(0.f), gameOverTimer(0.f),
	backgroundOffsetX(0.f), storyTimer(0.f), storyKeyPressed(false),
	quitQuizStep(0), quitQuizKeyPressed(false) {
	camera = window.getDefaultView();
	seed = generateRandomSeed();
	std::seed_seq seq(seed.begin(), seed.end());
	rng.seed(seq);
	loadResources();
	Menu::initMenu(*this);
}

void Game::run() {
	sf::Clock clock;
	while (window.isOpen()) {
		sf::Time delta = clock.restart();
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				if (state == GameState::RiverLevel || state == GameState::StationLevel)
					state = GameState::Paused;
				else if (state == GameState::Paused)
					state = (stationPhase == StationPhase::MachinistCabin) ? GameState::StationLevel : GameState::RiverLevel;
			}
			if (state == GameState::Menu) {
				if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					Menu::handleMenuClick(*this, mousePos);
				}
			}
			else if (state == GameState::Paused) {
				if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (mousePos.x >= camera.getCenter().x - 100 && mousePos.x <= camera.getCenter().x + 100 &&
						mousePos.y >= 200 && mousePos.y <= 250) {
						state = (stationPhase == StationPhase::MachinistCabin) ? GameState::StationLevel : GameState::RiverLevel;
					}
					if (mousePos.x >= camera.getCenter().x - 100 && mousePos.x <= camera.getCenter().x + 100 &&
						mousePos.y >= 300 && mousePos.y <= 350) {
						resetGame();
					}
				}
			}
			else if (state == GameState::Story) {
				if (event.type == sf::Event::KeyPressed ||
					(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)) {
					sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (event.type == sf::Event::MouseButtonPressed &&
						mousePos.y >= 500 && mousePos.y <= 550 &&
						mousePos.x >= 400 && mousePos.x <= 600) {
						state = GameState::Menu;
						menuPage = 1;
						storyKeyPressed = false;
						storyTimer = 0.f;
						std::cout << "Back clicked in Story, returning to menuPage=1" << std::endl;
					}
					else {
						storyKeyPressed = true;
					}
				}
			}
			else if (state == GameState::QuitQuiz) {
				if (event.type == sf::Event::KeyPressed ||
					(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)) {
					quitQuizKeyPressed = true;
				}
			}
		}

		switch (state) {
		case GameState::Menu:
			Menu::updateMenu(*this, delta);
			break;
		case GameState::Story:
			storyTimer += delta.asSeconds();
			if (storyKeyPressed) {
				state = nextLevelState;
				if (state == GameState::RiverLevel) {
					RiverLevel::initRiverLevel(*this);
				}
				else if (state == GameState::StationLevel) {
					StationLevel::initStationLevel(*this);
				}
				storyKeyPressed = false;
				storyTimer = 0.f;
			}
			break;
		case GameState::RiverLevel:
			RiverLevel::updateRiverLevel(*this, delta);
			break;
		case GameState::StationLevel:
			StationLevel::updateStationLevel(*this, delta);
			break;
		case GameState::GameOver:
			Renderer::updateGameOver(*this, delta);
			break;
		case GameState::Win:
			Renderer::updateWin(*this, delta);
			break;
		case GameState::Paused:
			break;
		case GameState::QuitQuiz:
			if (quitQuizKeyPressed) {
				quitQuizStep++;
				quitQuizKeyPressed = false;
				if (quitQuizStep >= 4) {
					window.close(); // Exit game after final quiz step
				}
				else {
					// Set quiz text for the current step
					switch (quitQuizStep) {
					case 0:
						quitQuizText = "Lepidoptera, Pieridae. It calls us.";
						break;
					case 1:
						quitQuizText = "It's about time, mister. They will still study it, they will think about us and argue about us. Of course, various opinions will arise, but in the majority of cases, future young people, I hope, will treat us with sympathy. Do you agree?";
						break;
					case 2:
						quitQuizText = "Let's not hide the fact that there were mistakes. Who can argue with that? You don't argue with that, do you?";
						break;
					case 3:
						quitQuizText = "So, making noise, taking risks, jumping out of windows, you must agree, is reckless, but we simply had no choice.";
						break;
					}
				}
			}
			break;
		}
		Renderer::drawScene(*this);
	}
}

void Game::loadResources() {
	// Load fonts
	if (!fontLight.loadFromFile("Fonts/Univers 45 Light Regular.otf"))
		std::cerr << "Error: Failed to load font Fonts/Univers 45 Light Regular.otf" << std::endl;
	if (!fontBlack.loadFromFile("Fonts/Univers 75 Black Regular.otf"))
		std::cerr << "Error: Failed to load font Fonts/Univers 75 Black Regular.otf" << std::endl;
	if (!fontRoman.loadFromFile("Fonts/Univers 55 Roman.otf"))
		std::cerr << "Error: Failed to load font Fonts/Univers 55 Roman.otf" << std::endl;
	if (!fontCondensed.loadFromFile("Fonts/Futura Condensed Extra Bold.otf"))
		std::cerr << "Error: Failed to load font Fonts/Futura Condensed Extra Bold.otf" << std::endl;

	// River level textures
	if (!anlaStand1Tex.loadFromFile("Sprites/River/anla_stand1.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/anla_stand1.png" << std::endl;
	if (!anlaStand2Tex.loadFromFile("Sprites/River/anla_stand2.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/anla_stand2.png" << std::endl;
	if (!anlaSwim1Tex.loadFromFile("Sprites/River/anla_swim1.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/anla_swim1.png" << std::endl;
	if (!anlaSwim2Tex.loadFromFile("Sprites/River/anla_swim2.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/anla_swim2.png" << std::endl;
	if (!anlaSwim3Tex.loadFromFile("Sprites/River/anla_swim3.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/anla_swim3.png" << std::endl;
	if (!boatmanSwim1Tex.loadFromFile("Sprites/River/boatman_swim1.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/boatman_swim1.png" << std::endl;
	if (!boatmanSwim2Tex.loadFromFile("Sprites/River/boatman_swim2.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/boatman_swim2.png" << std::endl;
	if (!boatmanSwim3Tex.loadFromFile("Sprites/River/boatman_swim3.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/boatman_swim3.png" << std::endl;
	if (!fishSwim1Tex.loadFromFile("Sprites/River/fish_swim1.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/fish_swim1.png" << std::endl;
	if (!fishSwim2Tex.loadFromFile("Sprites/River/fish_swim2.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/fish_swim2.png" << std::endl;
	if (!girlDrowning1Tex.loadFromFile("Sprites/River/girl_drowning1.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/girl_drowning1.png" << std::endl;
	if (!girlDrowning2Tex.loadFromFile("Sprites/River/girl_drowning2.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/girl_drowning2.png" << std::endl;
	if (!riverBgTex.loadFromFile("Sprites/River/river.jpg"))
		std::cerr << "Error: Failed to load texture Sprites/River/river.jpg" << std::endl;
	if (!rockTex.loadFromFile("Sprites/River/rock.png"))
		std::cerr << "Error: Failed to load texture Sprites/River/rock.png" << std::endl;
	else
		std::cout << "Loaded texture Sprites/River/rock.png successfully" << std::endl;

	// Train level textures
	if (!parvisStandTex.loadFromFile("Sprites/Train/parvis_stand.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/parvis_stand.png" << std::endl;
	if (!parvisWalk1Tex.loadFromFile("Sprites/Train/parvis_walk1.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/parvis_walk1.png" << std::endl;
	if (!parvisWalk2Tex.loadFromFile("Sprites/Train/parvis_walk2.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/parvis_walk2.png" << std::endl;
	if (!parvisWalk3Tex.loadFromFile("Sprites/Train/parvis_walk3.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/parvis_walk3.png" << std::endl;
	if (!parvisShootNormalTex.loadFromFile("Sprites/Train/parvis_shoot_normal.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/parvis_shoot_normal.png" << std::endl;
	if (!parvisShootUpTex.loadFromFile("Sprites/Train/parvis_shoot_up.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/parvis_shoot_up.png" << std::endl;
	if (!parvisShootDownTex.loadFromFile("Sprites/Train/parvis_shoot_down.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/parvis_shoot_down.png" << std::endl;
	if (!gendarmeStandTex.loadFromFile("Sprites/Train/gendarme_stand.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/gendarme_stand.png" << std::endl;
	if (!gendarmeShootTex.loadFromFile("Sprites/Train/gendarme_shoot.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/gendarme_shoot.png" << std::endl;
	if (!gendarmeWalk1Tex.loadFromFile("Sprites/Train/gendarme_walk1.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/gendarme_walk1.png" << std::endl;
	if (!gendarmeWalk2Tex.loadFromFile("Sprites/Train/gendarme_walk2.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/gendarme_walk2.png" << std::endl;
	if (!gendarmeWalk3Tex.loadFromFile("Sprites/Train/gendarme_walk3.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/gendarme_walk3.png" << std::endl;
	if (!motormanStandTex.loadFromFile("Sprites/Train/motorman_stand.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/motorman_stand.png" << std::endl;
	if (!motormanPushTex.loadFromFile("Sprites/Train/motorman_push.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/motorman_push.png" << std::endl;
	if (!motormanWalk1Tex.loadFromFile("Sprites/Train/motorman_walk1.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/motorman_walk1.png" << std::endl;
	if (!motormanWalk2Tex.loadFromFile("Sprites/Train/motorman_walk2.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/motorman_walk2.png" << std::endl;
	if (!motormanWalk3Tex.loadFromFile("Sprites/Train/motorman_walk3.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/motorman_walk3.png" << std::endl;
	if (!leverTex.loadFromFile("Sprites/Train/lever.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/lever.png" << std::endl;
	if (!trainBgTex.loadFromFile("Sprites/Train/train.jpg"))
		std::cerr << "Error: Failed to load texture Sprites/Train/train.jpg" << std::endl;
	if (!carriagesBgTex.loadFromFile("Sprites/Train/carriages.jpg"))
		std::cerr << "Error: Failed to load texture Sprites/Train/carriages.jpg" << std::endl;
	if (!driverscabinBgTex.loadFromFile("Sprites/Train/driverscabin.jpg"))
		std::cerr << "Error: Failed to load texture Sprites/Train/driverscabin.jpg" << std::endl;
	if (!bulletNormalTex.loadFromFile("Sprites/Train/bullet_normal.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/bullet_normal.png" << std::endl;
	if (!bulletUpTex.loadFromFile("Sprites/Train/bullet_up.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/bullet_up.png" << std::endl;
	if (!bulletDownTex.loadFromFile("Sprites/Train/bullet_down.png"))
		std::cerr << "Error: Failed to load texture Sprites/Train/bullet_down.png" << std::endl;

	// Retained for Menu.cpp
	if (!machinistTex.loadFromFile("Sprites/machinist.png"))
		std::cerr << "Error: Failed to load texture Sprites/machinist.png" << std::endl;

	background.setScale(1024.f / riverBgTex.getSize().x, 600.f / riverBgTex.getSize().y);
	background2.setScale(1024.f / riverBgTex.getSize().x, 600.f / riverBgTex.getSize().y);
}

void Game::resetGame() {
	seed = generateRandomSeed();
	std::seed_seq seq(seed.begin(), seed.end());
	rng.seed(seq);
	Menu::initMenu(*this);
}

void Game::setSeed(const std::string& newSeed) {
	seed = newSeed;
	std::seed_seq seq(newSeed.begin(), newSeed.end());
	rng.seed(seq);
}

std::string Game::generateRandomSeed() {
	std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::uniform_int_distribution<int> dist(0, static_cast<int>(chars.size()) - 1);
	std::string result;
	for (int i = 0; i < 8; ++i)
		result += chars[dist(rng)];
	return result;
}