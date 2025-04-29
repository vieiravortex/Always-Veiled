#include "RiverLevel.h"
#include "Renderer.h"
#include <algorithm>
#include <iostream>

void RiverLevel::initRiverLevel(Game& game) {
	game.state = Game::GameState::RiverLevel;
	game.levelProgress = 0.f;
	game.spawnTimer = 0.f;
	game.shootTimer = 0.f;
	game.bossActive = false;
	game.goalBlinkTimer = 0.f;

	// Player (Anla) initialization
	game.player.sprite.setTexture(game.anlaStand1Tex);
	game.player.sprite.setTextureRect(sf::IntRect(0, 0, game.anlaStand1Tex.getSize().x, game.anlaStand1Tex.getSize().y));
	// Ensure player sprite scale is reset to 1.0f for RiverLevel
	game.player.sprite.setScale(1.f, 1.f);
	game.player.sprite.setPosition(200.f, 450.f);
	game.player.velocity = sf::Vector2f(0.f, 0.f);
	game.player.health = 10;
	game.player.invincibilityTimer = 0.f;
	game.player.facingRight = true;
	game.player.animationFrames = { &game.anlaStand1Tex, &game.anlaStand2Tex }; // Default standing animation
	game.player.currentFrame = 0;
	game.player.animationTimer = 0.f;
	game.player.frameDuration = 0.5f; // Slower for standing

	// Goal (Girl drowning) initialization
	game.goal.sprite.setTexture(game.girlDrowning1Tex);
	game.goal.sprite.setTextureRect(sf::IntRect(0, 0, game.girlDrowning1Tex.getSize().x, game.girlDrowning1Tex.getSize().y));
	float goalWidth = game.goal.sprite.getGlobalBounds().width;
	game.goal.sprite.setPosition(game.levelLength - goalWidth, 450.f);
	game.goal.velocity = sf::Vector2f(0.f, 0.f);
	game.goal.animationFrames = { &game.girlDrowning1Tex, &game.girlDrowning2Tex };
	game.goal.currentFrame = 0;
	game.goal.animationTimer = 0.f;
	game.goal.frameDuration = 0.5f; // Drowning animation

	// Background
	game.background.setTexture(game.riverBgTex);
	game.background2.setTexture(game.riverBgTex);
	game.backgroundOffsetX = 0.f;

	game.enemies.clear();
	game.obstacles.clear();
	game.bullets.clear();
	game.enemyBullets.clear();

	// Spawn obstacles and enemies
	std::vector<sf::Vector2f> gridPositions;
	std::uniform_real_distribution<float> yDist(300.f, 500.f);
	for (int x = 0; x < 20; ++x) {
		float posX = 512.f + x * 200.f;
		if (posX < game.levelLength - goalWidth - 100.f) {
			float posY = yDist(game.getRNG());
			gridPositions.emplace_back(posX, posY);
		}
	}
	std::shuffle(gridPositions.begin(), gridPositions.end(), game.getRNG());
	std::cout << "Grid positions available: " << gridPositions.size() << std::endl;

	std::uniform_int_distribution<int> typeDist(0, 2);
	int rockCount = 0, fishCount = 0, boatCount = 0;

	for (size_t i = 0; i < gridPositions.size(); ++i) {
		int type = typeDist(game.getRNG());
		sf::Vector2f pos = gridPositions[i];

		if (type == 0) {
			Game::Entity rock;
			rock.sprite.setTexture(game.rockTex);
			rock.sprite.setPosition(pos);
			rock.velocity = sf::Vector2f(0.f, 0.f);
			rock.animationFrames = { &game.rockTex }; // No animation for rocks
			rock.currentFrame = 0;
			rock.animationTimer = 0.f;
			rock.frameDuration = 0.f;
			game.obstacles.push_back(rock);
			rockCount++;
			std::cout << "Spawned rock at x=" << pos.x << ", y=" << pos.y << std::endl;
		}
		else if (type == 1) {
			Game::Entity fish;
			fish.sprite.setTexture(game.fishSwim1Tex);
			fish.sprite.setPosition(pos);
			fish.velocity = sf::Vector2f(0.f, game.getRNG()() % 2 == 0 ? 150.f : -150.f);
			fish.animationFrames = { &game.fishSwim1Tex, &game.fishSwim2Tex };
			fish.currentFrame = 0;
			fish.animationTimer = 0.f;
			fish.frameDuration = 0.3f; // Faster for fish
			game.enemies.push_back(fish);
			fishCount++;
			std::cout << "Spawned fish at x=" << pos.x << ", y=" << pos.y << std::endl;
		}
		else {
			Game::Entity boat;
			boat.sprite.setTexture(game.boatmanSwim1Tex);
			boat.sprite.setPosition(pos);
			boat.velocity = sf::Vector2f(-100.f, 0.f);
			boat.animationFrames = { &game.boatmanSwim1Tex, &game.boatmanSwim2Tex, &game.boatmanSwim3Tex };
			boat.currentFrame = 0;
			boat.animationTimer = 0.f;
			boat.frameDuration = 0.2f;
			game.enemies.push_back(boat);
			boatCount++;
			std::cout << "Spawned boatman at x=" << pos.x << ", y=" << pos.y << std::endl;
		}
	}

	// Ensure at least one of each type
	if (rockCount == 0 && !gridPositions.empty()) {
		size_t replaceIdx = game.getRNG()() % (fishCount + boatCount);
		if (replaceIdx < fishCount) {
			game.obstacles.push_back(game.enemies[replaceIdx]);
			game.obstacles.back().sprite.setTexture(game.rockTex);
			game.obstacles.back().velocity = sf::Vector2f(0.f, 0.f);
			game.obstacles.back().animationFrames = { &game.rockTex };
			game.enemies.erase(game.enemies.begin() + replaceIdx);
			fishCount--;
			rockCount++;
		}
		else {
			size_t boatIdx = replaceIdx - fishCount;
			game.obstacles.push_back(game.enemies[fishCount + boatIdx]);
			game.obstacles.back().sprite.setTexture(game.rockTex);
			game.obstacles.back().velocity = sf::Vector2f(0.f, 0.f);
			game.obstacles.back().animationFrames = { &game.rockTex };
			game.enemies.erase(game.enemies.begin() + fishCount + boatIdx);
			boatCount--;
			rockCount++;
		}
	}
	if (fishCount == 0 && !gridPositions.empty()) {
		size_t replaceIdx = game.getRNG()() % (rockCount + boatCount);
		if (replaceIdx < rockCount) {
			game.enemies.push_back(game.obstacles[replaceIdx]);
			game.enemies.back().sprite.setTexture(game.fishSwim1Tex);
			game.enemies.back().velocity = sf::Vector2f(0.f, game.getRNG()() % 2 == 0 ? 150.f : -150.f);
			game.enemies.back().animationFrames = { &game.fishSwim1Tex, &game.fishSwim2Tex };
			game.enemies.back().frameDuration = 0.3f;
			game.obstacles.erase(game.obstacles.begin() + replaceIdx);
			rockCount--;
			fishCount++;
		}
		else {
			size_t boatIdx = replaceIdx - rockCount;
			game.enemies.push_back(game.enemies[fishCount + boatIdx]);
			game.enemies.back().sprite.setTexture(game.fishSwim1Tex);
			game.enemies.back().velocity = sf::Vector2f(0.f, game.getRNG()() % 2 == 0 ? 150.f : -150.f);
			game.enemies.back().animationFrames = { &game.fishSwim1Tex, &game.fishSwim2Tex };
			game.enemies.back().frameDuration = 0.3f;
			game.enemies.erase(game.enemies.begin() + fishCount + boatIdx);
			boatCount--;
			fishCount++;
		}
	}
	if (boatCount == 0 && !gridPositions.empty()) {
		size_t replaceIdx = game.getRNG()() % (rockCount + fishCount);
		if (replaceIdx < rockCount) {
			game.enemies.push_back(game.obstacles[replaceIdx]);
			game.enemies.back().sprite.setTexture(game.boatmanSwim1Tex);
			game.enemies.back().velocity = sf::Vector2f(-100.f, 0.f);
			game.enemies.back().animationFrames = { &game.boatmanSwim1Tex, &game.boatmanSwim2Tex, &game.boatmanSwim3Tex };
			game.enemies.back().frameDuration = 0.2f;
			game.obstacles.erase(game.obstacles.begin() + replaceIdx);
			rockCount--;
			boatCount++;
		}
		else {
			game.enemies.push_back(game.enemies[replaceIdx - rockCount]);
			game.enemies.back().sprite.setTexture(game.boatmanSwim1Tex);
			game.enemies.back().velocity = sf::Vector2f(-100.f, 0.f);
			game.enemies.back().animationFrames = { &game.boatmanSwim1Tex, &game.boatmanSwim2Tex, &game.boatmanSwim3Tex };
			game.enemies.back().frameDuration = 0.2f;
			game.enemies.erase(game.enemies.begin() + (replaceIdx - rockCount));
			fishCount--;
			boatCount++;
		}
	}

	// Adjust entity positions
	for (auto& entity : game.enemies) {
		float entityHeight = entity.sprite.getGlobalBounds().height;
		if (entity.sprite.getPosition().y > 600.f - entityHeight)
			entity.sprite.setPosition(entity.sprite.getPosition().x, 600.f - entityHeight);
	}
	for (auto& obstacle : game.obstacles) {
		float obstacleHeight = obstacle.sprite.getGlobalBounds().height;
		if (obstacle.sprite.getPosition().y > 600.f - obstacleHeight)
			obstacle.sprite.setPosition(obstacle.sprite.getPosition().x, 600.f - obstacleHeight);
	}

	Renderer::updateCamera(game);
	std::cout << "Initialized RiverLevel: levelLength=" << game.levelLength << ", goal at x=" << (game.levelLength - goalWidth)
		<< ", rocks spawned=" << rockCount << ", fish spawned=" << fishCount << ", boatmen spawned=" << boatCount << std::endl;
}

void RiverLevel::updateRiverLevel(Game& game, sf::Time delta) {
	if (game.player.invincibilityTimer > 0.f) {
		game.player.invincibilityTimer -= delta.asSeconds();
		int blinkPhase = static_cast<int>(game.player.invincibilityTimer / 0.1f) % 2;
		game.player.sprite.setColor(sf::Color(255, 255, 255, blinkPhase == 0 ? 255 : 128));
		if (game.player.invincibilityTimer < 0.f) {
			game.player.invincibilityTimer = 0.f;
			game.player.sprite.setColor(sf::Color::White);
		}
	}

	game.player.velocity = sf::Vector2f(0.f, 0.f);
	float speed = 200.f;
	bool isMoving = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		speed *= 1.5f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		game.player.velocity.x = speed;
		isMoving = true;
		if (!game.player.facingRight) {
			game.player.facingRight = true;
			game.player.sprite.setScale(1.f, 1.f);
			game.player.sprite.setOrigin(0.f, 0.f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && game.player.sprite.getPosition().x > 200.f) {
		game.player.velocity.x = -speed;
		isMoving = true;
		if (game.player.facingRight) {
			game.player.facingRight = false;
			game.player.sprite.setScale(-1.f, 1.f);
			game.player.sprite.setOrigin(game.player.sprite.getLocalBounds().width, 0.f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		game.player.velocity.y = -speed;
		isMoving = true;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		game.player.velocity.y = speed;
		isMoving = true;
	}

	// Update player animation based on movement
	if (isMoving) {
		if (game.player.animationFrames != std::vector<sf::Texture*>{&game.anlaSwim1Tex, &game.anlaSwim2Tex, &game.anlaSwim3Tex}) {
			game.player.animationFrames = { &game.anlaSwim1Tex, &game.anlaSwim2Tex, &game.anlaSwim3Tex };
			game.player.currentFrame = 0;
			game.player.animationTimer = 0.f;
			game.player.frameDuration = 0.2f;
			game.player.sprite.setTexture(*game.player.animationFrames[0]);
		}
	}
	else {
		if (game.player.animationFrames != std::vector<sf::Texture*>{&game.anlaStand1Tex, &game.anlaStand2Tex}) {
			game.player.animationFrames = { &game.anlaStand1Tex, &game.anlaStand2Tex };
			game.player.currentFrame = 0;
			game.player.animationTimer = 0.f;
			game.player.frameDuration = 0.5f;
			game.player.sprite.setTexture(*game.player.animationFrames[0]);
		}
	}

	game.player.sprite.move(game.player.velocity * delta.asSeconds());

	float playerHeight = game.player.sprite.getGlobalBounds().height;
	float playerWidth = game.player.sprite.getGlobalBounds().width;
	if (game.player.sprite.getPosition().y < 300.f)
		game.player.sprite.setPosition(game.player.sprite.getPosition().x, 300.f);
	if (game.player.sprite.getPosition().y > 600.f - playerHeight)
		game.player.sprite.setPosition(game.player.sprite.getPosition().x, 600.f - playerHeight);
	if (game.player.sprite.getPosition().x < 200.f)
		game.player.sprite.setPosition(200.f, game.player.sprite.getPosition().y);
	if (game.player.sprite.getPosition().x > game.levelLength - playerWidth)
		game.player.sprite.setPosition(game.levelLength - playerWidth, game.player.sprite.getPosition().y);

	if (game.player.sprite.getPosition().x < game.levelLength - playerWidth) {
		game.backgroundOffsetX -= game.player.velocity.x * delta.asSeconds();
		if (game.backgroundOffsetX <= -1024.f)
			game.backgroundOffsetX += 1024.f;
		if (game.backgroundOffsetX > 0.f)
			game.backgroundOffsetX -= 1024.f;
	}

	Renderer::updateCamera(game);

	for (auto& enemy : game.enemies) {
		float enemyHeight = enemy.sprite.getGlobalBounds().height;
		if (enemy.sprite.getTexture() == &game.fishSwim1Tex &&
			(enemy.sprite.getPosition().y <= 300.f || enemy.sprite.getPosition().y >= 600.f - enemyHeight)) {
			enemy.velocity.y = -enemy.velocity.y;
		}
		enemy.sprite.move(enemy.velocity * delta.asSeconds());

		if (enemy.sprite.getPosition().y < 300.f)
			enemy.sprite.setPosition(enemy.sprite.getPosition().x, 300.f);
		if (enemy.sprite.getPosition().y > 600.f - enemyHeight)
			enemy.sprite.setPosition(enemy.sprite.getPosition().x, 600.f - enemyHeight);

		if (game.player.invincibilityTimer <= 0.f && enemy.sprite.getGlobalBounds().intersects(game.player.sprite.getGlobalBounds())) {
			game.player.health--;
			game.player.invincibilityTimer = 0.5f;
			if (game.player.health <= 0) {
				game.state = Game::GameState::GameOver;
				game.gameOverTimer = 0.f;
				return;
			}
		}
		if (enemy.sprite.getPosition().x < game.player.sprite.getPosition().x - 1124.f)
			enemy.active = false;
	}

	for (auto& obstacle : game.obstacles) {
		float obstacleHeight = obstacle.sprite.getGlobalBounds().height;
		if (obstacle.sprite.getPosition().y < 300.f)
			obstacle.sprite.setPosition(obstacle.sprite.getPosition().x, 300.f);
		if (obstacle.sprite.getPosition().y > 600.f - obstacleHeight)
			obstacle.sprite.setPosition(obstacle.sprite.getPosition().x, 600.f - obstacleHeight);

		if (game.player.invincibilityTimer <= 0.f && obstacle.sprite.getGlobalBounds().intersects(game.player.sprite.getGlobalBounds())) {
			game.player.health--;
			game.player.invincibilityTimer = 0.5f;
			if (game.player.health <= 0) {
				game.state = Game::GameState::GameOver;
				game.gameOverTimer = 0.f;
				return;
			}
		}
		if (obstacle.sprite.getPosition().x < game.player.sprite.getPosition().x - 1124.f)
			obstacle.active = false;
	}

	if (game.player.sprite.getGlobalBounds().intersects(game.goal.sprite.getGlobalBounds())) {
		game.goalBlinkTimer += delta.asSeconds();
		int blinkPhase = static_cast<int>(game.goalBlinkTimer / 0.1f) % 2;
		game.goal.sprite.setColor(sf::Color(255, 255, 255, blinkPhase == 0 ? 255 : 128));
		if (game.goalBlinkTimer >= 0.5f) {
			game.state = Game::GameState::Win;
			game.winMessageTimer = 0.f;
			game.goal.sprite.setColor(sf::Color::White);
			game.goalBlinkTimer = 0.f;
		}
	}

	game.enemies.erase(std::remove_if(game.enemies.begin(), game.enemies.end(),
		[](const Game::Entity& e) { return !e.active; }), game.enemies.end());
	game.obstacles.erase(std::remove_if(game.obstacles.begin(), game.obstacles.end(),
		[](const Game::Entity& e) { return !e.active; }), game.obstacles.end());

	game.levelProgress = game.player.sprite.getPosition().x / game.levelLength;

	// Update animations
	Renderer::updateEntityAnimations(game, delta);
}