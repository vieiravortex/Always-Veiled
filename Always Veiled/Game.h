#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <string>
#include <vector>

class Game {
public:
	enum class GameState { Menu, Story, RiverLevel, StationLevel, GameOver, Win, Paused, QuitQuiz };
	enum class StationPhase { Platform, TrainRoof, MachinistCabin };

	struct Entity {
		sf::Sprite sprite;
		sf::Vector2f velocity;
		int health = 1;
		float shootTimer = 0.f;
		float invincibilityTimer = 0.f;
		bool active = true;
		bool facingRight = true;
		float retreatTimer = 0.f;
		// Animation fields
		std::vector<sf::Texture*> animationFrames; // List of texture frames for animation
		int currentFrame = 0; // Current animation frame index
		float animationTimer = 0.f; // Timer for frame switching
		float frameDuration = 0.2f; // Duration per frame (adjustable)
	};

	Game(sf::RenderWindow& window);
	void run();
	void resetGame();
	std::mt19937& getRNG() { return rng; }

	sf::RenderWindow& window;
	GameState state;
	StationPhase stationPhase;
	sf::View camera;
	Entity player, goal;
	std::vector<Entity> enemies, obstacles;
	std::vector<sf::Sprite> bullets, enemyBullets;
	std::vector<sf::Vector2f> bulletVelocities;
	sf::Sprite background, background2;
	float levelProgress, spawnTimer, shootTimer, levelLength;
	bool bossActive;
	float backgroundOffsetX;
	int menuPage;
	float winMessageTimer, gameOverTimer;
	float goalBlinkTimer;

	sf::Font fontLight;
	sf::Font fontBlack;
	sf::Font fontRoman;
	sf::Font fontCondensed; // New font for "Always Veiled" text

	// River level textures
	sf::Texture anlaStand1Tex, anlaStand2Tex, anlaSwim1Tex, anlaSwim2Tex, anlaSwim3Tex;
	sf::Texture boatmanSwim1Tex, boatmanSwim2Tex, boatmanSwim3Tex;
	sf::Texture fishSwim1Tex, fishSwim2Tex;
	sf::Texture girlDrowning1Tex, girlDrowning2Tex;
	sf::Texture riverBgTex, rockTex;

	// Train level textures
	sf::Texture parvisStandTex, parvisWalk1Tex, parvisWalk2Tex, parvisWalk3Tex;
	sf::Texture parvisShootNormalTex, parvisShootUpTex, parvisShootDownTex;
	sf::Texture gendarmeStandTex, gendarmeShootTex, gendarmeWalk1Tex, gendarmeWalk2Tex, gendarmeWalk3Tex;
	sf::Texture motormanStandTex, motormanPushTex, motormanWalk1Tex, motormanWalk2Tex, motormanWalk3Tex;
	sf::Texture leverTex, trainBgTex, carriagesBgTex, driverscabinBgTex;
	sf::Texture bulletNormalTex, bulletUpTex, bulletDownTex;

	// Retained for Menu.cpp
	sf::Texture machinistTex;

	// Story display variables
	std::string storyTitle;
	std::string storyText;
	float storyTimer;
	bool storyKeyPressed;
	GameState nextLevelState;

	// Quit quiz variables
	int quitQuizStep; // Current step of the quit quiz (0 to 3)
	bool quitQuizKeyPressed; // Flag to detect input for advancing quiz
	std::string quitQuizText; // Current quiz text

private:
	std::mt19937 rng;
	std::string seed;

	void loadResources();
	void setSeed(const std::string& newSeed);
	std::string generateRandomSeed();
};