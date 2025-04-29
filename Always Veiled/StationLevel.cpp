#include "StationLevel.h"
#include "Renderer.h"
#include <algorithm>
#include <iostream>

// Initialize static members
int StationLevel::rightmostEnemyIndex = -1;
float StationLevel::phaseTransitionTimer = 0.f;
bool StationLevel::pendingPhaseTransition = false;
Game::StationPhase StationLevel::nextPhase = Game::StationPhase::Platform;

void StationLevel::initStationLevel(Game& game) {
	game.state = Game::GameState::StationLevel;
	game.stationPhase = Game::StationPhase::Platform;
	game.levelProgress = 0.f;
	game.spawnTimer = 0.f;
	game.shootTimer = 0.f;
	game.bossActive = false;
	phaseTransitionTimer = 0.f;
	pendingPhaseTransition = false;

	// Player (Parvis) initialization
	game.player.sprite.setTexture(game.parvisStandTex);
	game.player.sprite.setTextureRect(sf::IntRect(0, 0, game.parvisStandTex.getSize().x, game.parvisStandTex.getSize().y));
	game.player.sprite.setScale(0.5f, 0.5f);
	game.player.sprite.setPosition(200.f, 450.f);
	game.player.velocity = sf::Vector2f(0.f, 0.f);
	game.player.health = 10;
	game.player.invincibilityTimer = 0.f;
	game.player.facingRight = true;
	game.player.retreatTimer = 0.f;
	game.player.animationFrames = { &game.parvisStandTex };
	game.player.currentFrame = 0;
	game.player.animationTimer = 0.f;
	game.player.frameDuration = 0.5f;

	// Goal (Lever) initialization
	game.goal.sprite.setTexture(game.leverTex);
	game.goal.sprite.setTextureRect(sf::IntRect(0, 0, game.leverTex.getSize().x, game.leverTex.getSize().y));
	game.goal.sprite.setPosition(-1000.f, -1000.f);
	game.goal.velocity = sf::Vector2f(0.f, 0.f);
	game.goal.animationFrames = { &game.leverTex };
	game.goal.currentFrame = 0;
	game.goal.animationTimer = 0.f;
	game.goal.frameDuration = 0.f;

	// Background
	game.background.setTexture(game.trainBgTex);
	game.background2.setTexture(game.trainBgTex);
	game.backgroundOffsetX = 0.f;

	game.enemies.clear();
	game.obstacles.clear();
	game.bullets.clear();
	game.bulletVelocities.clear();
	game.enemyBullets.clear();

	// Spawn gendarmes
	std::vector<sf::Vector2f> gridPositions;
	std::uniform_real_distribution<float> yDist(300.f, 500.f);
	std::uniform_real_distribution<float> shootTimerDist(0.f, 2.f);
	for (int x = 0; x < 6; ++x) {
		float posX = 512.f + x * 500.f;
		if (posX < game.levelLength - 100.f) {
			float posY = yDist(game.getRNG());
			gridPositions.emplace_back(posX, posY);
		}
	}
	std::shuffle(gridPositions.begin(), gridPositions.end(), game.getRNG());

	float maxX = 0.f;
	rightmostEnemyIndex = -1;
	for (int i = 0; i < 6 && !gridPositions.empty(); ++i) {
		Game::Entity gendarme;
		gendarme.sprite.setTexture(game.gendarmeStandTex);
		sf::Vector2f pos = gridPositions.back();
		gendarme.sprite.setPosition(pos);
		gendarme.velocity = sf::Vector2f(-200.f, 0.f);
		gendarme.health = 2;
		gendarme.shootTimer = shootTimerDist(game.getRNG());
		gendarme.invincibilityTimer = 0.f;
		gendarme.facingRight = false;
		gendarme.retreatTimer = 0.f;
		gendarme.animationFrames = { &game.gendarmeWalk1Tex, &game.gendarmeWalk2Tex, &game.gendarmeWalk3Tex };
		gendarme.currentFrame = 0;
		gendarme.animationTimer = 0.f;
		gendarme.frameDuration = 0.2f;
		if (!gendarme.facingRight) {
			gendarme.sprite.setScale(-1.f, 1.f);
			gendarme.sprite.setOrigin(gendarme.sprite.getLocalBounds().width, 0.f);
		}
		game.enemies.push_back(gendarme);
		if (pos.x > maxX) {
			maxX = pos.x;
			rightmostEnemyIndex = static_cast<int>(game.enemies.size()) - 1;
		}
		gridPositions.pop_back();
	}

	for (auto& enemy : game.enemies) {
		float enemyHeight = enemy.sprite.getGlobalBounds().height;
		if (enemy.sprite.getPosition().y > 600.f - enemyHeight)
			enemy.sprite.setPosition(enemy.sprite.getPosition().x, 600.f - enemyHeight);
		if (!enemy.facingRight) {
			enemy.sprite.setScale(-1.f, 1.f);
			enemy.sprite.setOrigin(enemy.sprite.getLocalBounds().width, 0.f);
		}
	}

	Renderer::updateCamera(game);
	std::cout << "Initialized StationLevel: Platform phase, levelLength=" << game.levelLength
		<< ", gendarmes spawned=" << game.enemies.size()
		<< ", rightmost enemy index=" << rightmostEnemyIndex << " at x="
		<< (rightmostEnemyIndex >= 0 ? game.enemies[rightmostEnemyIndex].sprite.getPosition().x : -1) << std::endl;
}

void StationLevel::initMachinistCabin(Game& game) {
	game.state = Game::GameState::StationLevel;
	game.stationPhase = Game::StationPhase::MachinistCabin;
	game.bossActive = true;
	game.enemies.clear();
	phaseTransitionTimer = 0.f;
	pendingPhaseTransition = false;

	// Player (Parvis) initialization
	game.player.sprite.setTexture(game.parvisStandTex);
	game.player.sprite.setTextureRect(sf::IntRect(0, 0, game.parvisStandTex.getSize().x, game.parvisStandTex.getSize().y));
	game.player.sprite.setScale(1.f, 1.f);
	game.player.sprite.setPosition(200.f, 450.f - 64.f);
	game.player.velocity = sf::Vector2f(0.f, 0.f);
	game.player.health = 10;
	game.player.invincibilityTimer = 0.5f;
	game.player.facingRight = true;
	game.player.retreatTimer = 0.f;
	game.player.animationFrames = { &game.parvisStandTex };
	game.player.currentFrame = 0;
	game.player.animationTimer = 0.f;
	game.player.frameDuration = 0.5f;

	// Goal (Lever) initialization
	game.goal.sprite.setTexture(game.leverTex);
	game.goal.sprite.setTextureRect(sf::IntRect(0, 0, game.leverTex.getSize().x, game.leverTex.getSize().y));
	game.goal.sprite.setPosition(900.f, 280.f);
	game.goal.velocity = sf::Vector2f(0.f, 0.f);
	game.goal.animationFrames = { &game.leverTex };
	game.goal.currentFrame = 0;
	game.goal.animationTimer = 0.f;
	game.goal.frameDuration = 0.f;

	// Boss (Motorman) initialization
	Game::Entity boss;
	boss.sprite.setTexture(game.motormanStandTex);
	boss.sprite.setTextureRect(sf::IntRect(0, 0, game.motormanStandTex.getSize().x, game.motormanStandTex.getSize().y));
	boss.sprite.setScale(1.f, 1.f);
	boss.sprite.setPosition(824.f - 64.f, 450.f - 64.f);
	boss.velocity = sf::Vector2f(0.f, 0.f);
	boss.health = 6;
	boss.shootTimer = 0.f;
	boss.invincibilityTimer = 0.f;
	boss.facingRight = false;
	boss.retreatTimer = 0.f;
	boss.animationFrames = { &game.motormanStandTex, &game.motormanPushTex };
	boss.currentFrame = 0;
	boss.animationTimer = 0.f;
	boss.frameDuration = 0.5f;
	if (!boss.facingRight) {
		boss.sprite.setScale(-1.f, 1.f);
		boss.sprite.setOrigin(boss.sprite.getLocalBounds().width, 0.f);
	}
	game.enemies.push_back(boss);

	// Background
	game.background.setTexture(game.driverscabinBgTex);
	game.background2.setTexture(game.driverscabinBgTex);
	game.backgroundOffsetX = 0.f;

	game.camera.setCenter(512.f, 300.f);
	game.camera.setSize(1024.f, 600.f);
	game.window.setView(game.camera);
	std::cout << "Initialized MachinistCabin: Fixed arena with lever at x=900, player and boss scaled to 256x256" << std::endl;
}

void StationLevel::updateStationLevel(Game& game, sf::Time delta) {
	const std::size_t survivingEnemies = game.enemies.size();
	std::vector<float> possibleY = { 300.f, 350.f, 400.f, 450.f };

	if (game.player.invincibilityTimer > 0.f) {
		game.player.invincibilityTimer -= delta.asSeconds();
		int blinkPhase = static_cast<int>(game.player.invincibilityTimer / 0.1f) % 2;
		game.player.sprite.setColor(sf::Color(255, 255, 255, blinkPhase == 0 ? 255 : 128));
		if (game.player.invincibilityTimer < 0.f) {
			game.player.invincibilityTimer = 0.f;
			game.player.sprite.setColor(sf::Color::White);
		}
	}

	float playerHeight = game.player.sprite.getGlobalBounds().height;
	float playerWidth = game.player.sprite.getGlobalBounds().width;

	bool isShooting = false;
	bool isMoving = false;
	game.player.velocity = sf::Vector2f(0.f, 0.f);
	float speed = 200.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		speed *= 1.5f;
	if (game.shootTimer < 0.5f)
		speed = 0.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		game.player.velocity.x = speed;
		isMoving = true;
		if (!game.player.facingRight) {
			game.player.facingRight = true;
			float scaleX = (game.stationPhase == Game::StationPhase::MachinistCabin) ? 1.f : 0.5f;
			game.player.sprite.setScale(scaleX, scaleX);
			game.player.sprite.setOrigin(0.f, 0.f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && game.player.sprite.getPosition().x > 200.f) {
		game.player.velocity.x = -speed;
		isMoving = true;
		if (game.player.facingRight) {
			game.player.facingRight = false;
			float scaleX = (game.stationPhase == Game::StationPhase::MachinistCabin) ? -1.f : -0.5f;
			game.player.sprite.setScale(scaleX, (game.stationPhase == Game::StationPhase::MachinistCabin) ? 1.f : 0.5f);
			game.player.sprite.setOrigin(game.player.sprite.getLocalBounds().width, 0.f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && game.shootTimer >= 0.5f) {
		game.player.velocity.y = -speed;
		isMoving = true;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && game.shootTimer >= 0.5f) {
		game.player.velocity.y = speed;
		isMoving = true;
	}

	game.player.sprite.move(game.player.velocity * delta.asSeconds());

	if (game.player.sprite.getPosition().y < 300.f)
		game.player.sprite.setPosition(game.player.sprite.getPosition().x, 300.f);
	if (game.player.sprite.getPosition().y > 600.f - playerHeight)
		game.player.sprite.setPosition(game.player.sprite.getPosition().x, 600.f - playerHeight);

	if (game.stationPhase == Game::StationPhase::MachinistCabin) {
		if (game.player.sprite.getPosition().x < 0.f)
			game.player.sprite.setPosition(0.f, game.player.sprite.getPosition().y);
		if (game.player.sprite.getPosition().x > 1024.f - playerWidth)
			game.player.sprite.setPosition(1024.f - playerWidth, game.player.sprite.getPosition().y);
	}
	else {
		if (game.player.sprite.getPosition().x < 200.f)
			game.player.sprite.setPosition(200.f, game.player.sprite.getPosition().y);
		if (game.player.sprite.getPosition().x > game.levelLength - playerWidth)
			game.player.sprite.setPosition(game.levelLength - playerWidth, game.player.sprite.getPosition().y);
	}

	if (game.stationPhase != Game::StationPhase::MachinistCabin && game.player.sprite.getPosition().x < game.levelLength - playerWidth) {
		game.backgroundOffsetX -= game.player.velocity.x * delta.asSeconds();
		if (game.backgroundOffsetX <= -1024.f)
			game.backgroundOffsetX += 1024.f;
		if (game.backgroundOffsetX > 0.f)
			game.backgroundOffsetX -= 1024.f;
	}

	if (game.stationPhase != Game::StationPhase::MachinistCabin)
		Renderer::updateCamera(game);

	game.shootTimer += delta.asSeconds();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && game.shootTimer >= 0.5f) {
		isShooting = true;
		sf::Sprite bullet;
		float bulletScale = (game.stationPhase == Game::StationPhase::MachinistCabin) ? 1.f : 0.5f;
		float bulletX = game.player.facingRight ? game.player.sprite.getPosition().x + (game.stationPhase == Game::StationPhase::MachinistCabin ? 200.f : 100.f) : game.player.sprite.getPosition().x - 32.f;
		float bulletY = game.player.sprite.getPosition().y + (game.stationPhase == Game::StationPhase::MachinistCabin ? 56.f : 28.f);
		bullet.setPosition(bulletX, bulletY);

		sf::Vector2f bulletVelocity(0.f, 0.f);
		bool isRight = game.player.facingRight;

		if (game.stationPhase == Game::StationPhase::MachinistCabin) {
			bullet.setTexture(game.bulletNormalTex);
			bulletVelocity = sf::Vector2f(isRight ? 500.f : -500.f, 0.f);
			bullet.setScale(isRight ? bulletScale : -bulletScale, bulletScale);
			game.player.animationFrames = { &game.parvisShootNormalTex };
			game.player.frameDuration = 0.5f;
			if (!isRight) {
				bullet.setOrigin(bullet.getLocalBounds().width, 0.f);
			}
		}
		else {
			bool isUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
			bool isDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

			if (isRight) {
				if (isUp && !isDown) {
					bullet.setTexture(game.bulletUpTex);
					bulletVelocity = sf::Vector2f(500.f, -500.f);
					game.player.animationFrames = { &game.parvisShootUpTex };
				}
				else if (isDown && !isUp) {
					bullet.setTexture(game.bulletDownTex);
					bulletVelocity = sf::Vector2f(500.f, 500.f);
					game.player.animationFrames = { &game.parvisShootDownTex };
				}
				else {
					bullet.setTexture(game.bulletNormalTex);
					bulletVelocity = sf::Vector2f(500.f, 0.f);
					game.player.animationFrames = { &game.parvisShootNormalTex };
				}
				bullet.setScale(bulletScale, bulletScale);
			}
			else {
				if (isUp && !isDown) {
					bullet.setTexture(game.bulletUpTex);
					bulletVelocity = sf::Vector2f(-500.f, -500.f);
					game.player.animationFrames = { &game.parvisShootUpTex };
				}
				else if (isDown && !isUp) {
					bullet.setTexture(game.bulletDownTex);
					bulletVelocity = sf::Vector2f(-500.f, 500.f);
					game.player.animationFrames = { &game.parvisShootDownTex };
				}
				else {
					bullet.setTexture(game.bulletNormalTex);
					bulletVelocity = sf::Vector2f(-500.f, 0.f);
					game.player.animationFrames = { &game.parvisShootNormalTex };
				}
				bullet.setScale(-bulletScale, bulletScale);
				bullet.setOrigin(bullet.getLocalBounds().width, 0.f);
			}
			game.player.frameDuration = 0.5f;
		}

		bullet.setPosition(bulletX, bulletY);

		bool immediateHit = false;
		if (game.stationPhase == Game::StationPhase::MachinistCabin) {
			for (auto& enemy : game.enemies) {
				if (bullet.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
					enemy.health--;
					enemy.invincibilityTimer = 0.5f;
					if (enemy.health <= 0)
						enemy.active = false;
					immediateHit = true;
					break;
				}
			}
		}

		if (!immediateHit) {
			game.bullets.push_back(bullet);
			game.bulletVelocities.push_back(bulletVelocity);
		}

		game.shootTimer = 0.f;
		game.player.currentFrame = 0;
		game.player.animationTimer = 0.f;
		game.player.sprite.setTexture(*game.player.animationFrames[0]);
	}

	// Update player animation only if not shooting
	if (!isShooting && game.shootTimer >= 0.5f) {
		if (isMoving) {
			if (game.player.animationFrames != std::vector<sf::Texture*>{&game.parvisWalk1Tex, &game.parvisWalk2Tex, &game.parvisWalk3Tex}) {
				game.player.animationFrames = game.player.animationFrames = { &game.parvisWalk1Tex, &game.parvisWalk2Tex, &game.parvisWalk3Tex };
				game.player.currentFrame = 0;
				game.player.animationTimer = 0.f;
				game.player.frameDuration = 0.2f;
				game.player.sprite.setTexture(*game.player.animationFrames[0]);
			}
		}
		else {
			if (game.player.animationFrames != std::vector<sf::Texture*>{&game.parvisStandTex}) {
				game.player.animationFrames = { &game.parvisStandTex };
				game.player.currentFrame = 0;
				game.player.animationTimer = 0.f;
				game.player.frameDuration = 0.5f;
				game.player.sprite.setTexture(*game.player.animationFrames[0]);
			}
		}
	}

	// Update bullets
	for (size_t i = 0; i < game.bullets.size();) {
		if (i < game.bulletVelocities.size()) {
			game.bullets[i].move(game.bulletVelocities[i].x * delta.asSeconds(), game.bulletVelocities[i].y * delta.asSeconds());
		}
		else {
			float bulletSpeed = game.bullets[i].getScale().x > 0 ? 500.f : -500.f;
			game.bullets[i].move(bulletSpeed * delta.asSeconds(), 0.f);
		}

		bool remove = false;
		if (game.bullets[i].getPosition().x > game.player.sprite.getPosition().x + 1124.f ||
			game.bullets[i].getPosition().x < game.player.sprite.getPosition().x - 1124.f ||
			game.bullets[i].getPosition().y < 0.f || game.bullets[i].getPosition().y > 600.f)
			remove = true;

		for (auto& enemy : game.enemies) {
			if (game.bullets[i].getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
				enemy.health--;
				enemy.invincibilityTimer = 0.5f;
				if (enemy.health <= 0)
					enemy.active = false;
				remove = true;
				break;
			}
		}

		if (remove) {
			game.bullets.erase(game.bullets.begin() + i);
			if (i < game.bulletVelocities.size()) {
				game.bulletVelocities.erase(game.bulletVelocities.begin() + i);
			}
		}
		else {
			++i;
		}
	}

	for (size_t i = 0; i < game.enemies.size(); ++i) {
		auto& enemy = game.enemies[i];
		float enemyHeight = enemy.sprite.getGlobalBounds().height;
		bool isLeftOfPlayer = enemy.sprite.getPosition().x < game.player.sprite.getPosition().x;

		if (enemy.invincibilityTimer > 0.f) {
			enemy.invincibilityTimer -= delta.asSeconds();
			int blinkPhase = static_cast<int>(enemy.invincibilityTimer / 0.1f) % 2;
			enemy.sprite.setColor(sf::Color(255, 255, 255, blinkPhase == 0 ? 255 : 128));
			if (enemy.invincibilityTimer <= 0.f) {
				enemy.invincibilityTimer = 0.f;
				enemy.sprite.setColor(sf::Color::White);
			}
		}

		if (enemy.retreatTimer > 0.f) {
			enemy.retreatTimer -= delta.asSeconds();
			if (enemy.retreatTimer <= 0.f) {
				enemy.retreatTimer = 0.f;
			}
		}

		if (game.stationPhase == Game::StationPhase::MachinistCabin) {
			if (enemy.retreatTimer > 0.f) {
				enemy.velocity.x = isLeftOfPlayer ? -200.f : 200.f;
			}
			else {
				enemy.velocity.x = isLeftOfPlayer ? 200.f : -200.f;
			}

			if (enemy.velocity.x > 0 && !enemy.facingRight) {
				enemy.facingRight = true;
				enemy.sprite.setScale(1.f, 1.f);
				enemy.sprite.setOrigin(0.f, 0.f);
			}
			else if (enemy.velocity.x < 0 && enemy.facingRight) {
				enemy.facingRight = false;
				enemy.sprite.setScale(-1.f, 1.f);
				enemy.sprite.setOrigin(enemy.sprite.getLocalBounds().width, 0.f);
			}

			float playerY = game.player.sprite.getPosition().y;
			auto closestY = std::min_element(possibleY.begin(), possibleY.end(),
				[playerY](float a, float b) { return std::abs(a - playerY) < std::abs(b - playerY); });
			float targetY = *closestY;
			float currentY = enemy.sprite.getPosition().y;
			if (std::abs(currentY - targetY) > 1.f) {
				enemy.velocity.y = (targetY > currentY) ? 200.f : -200.f;
			}
			else {
				enemy.velocity.y = 0.f;
				enemy.sprite.setPosition(enemy.sprite.getPosition().x, targetY);
			}

			enemy.sprite.move(enemy.velocity * delta.asSeconds());

			if (enemy.sprite.getPosition().x < 0.f) {
				enemy.sprite.setPosition(0.f, game.player.sprite.getPosition().y);
				enemy.velocity.x = 200.f;
				enemy.facingRight = true;
				enemy.sprite.setScale(1.f, 1.f);
				enemy.sprite.setOrigin(0.f, 0.f);
			}
			if (enemy.sprite.getPosition().x > 1024.f - enemy.sprite.getGlobalBounds().width) {
				enemy.sprite.setPosition(1024.f - enemy.sprite.getGlobalBounds().width, enemy.sprite.getPosition().y);
				enemy.velocity.x = -200.f;
				enemy.facingRight = false;
				enemy.sprite.setScale(-1.f, 1.f);
				enemy.sprite.setOrigin(enemy.sprite.getLocalBounds().width, 0.f);
			}

			if (enemy.sprite.getPosition().y < 300.f) {
				enemy.sprite.setPosition(enemy.sprite.getPosition().x, 300.f);
				enemy.velocity.y = 0.f;
			}
			if (enemy.sprite.getPosition().y > 600.f - enemyHeight) {
				enemy.sprite.setPosition(enemy.sprite.getPosition().x, 600.f - enemyHeight);
				enemy.velocity.y = 0.f;
			}

			if (enemy.sprite.getGlobalBounds().intersects(game.player.sprite.getGlobalBounds())) {
				if (game.player.invincibilityTimer <= 0.f) {
					game.player.health--;
					game.player.invincibilityTimer = 0.5f;
					if (enemy.retreatTimer <= 0.f) {
						enemy.retreatTimer = 0.5f;
						enemy.animationFrames = { &game.motormanStandTex, &game.motormanPushTex };
						enemy.currentFrame = 0;
						enemy.animationTimer = 0.f;
					}
					if (game.player.health <= 0) {
						game.player.invincibilityTimer = 1.f;
						return;
					}
				}
			}
			else {
				if (enemy.animationFrames != std::vector<sf::Texture*>{&game.motormanWalk1Tex, &game.motormanWalk2Tex, &game.motormanWalk3Tex}) {
					enemy.animationFrames = { &game.motormanWalk1Tex, &game.motormanWalk2Tex, &game.motormanWalk3Tex };
					enemy.currentFrame = 0;
					enemy.animationTimer = 0.f;
					enemy.frameDuration = 0.2f;
					enemy.sprite.setTexture(*enemy.animationFrames[0]);
				}
			}
		}
		else {
			if (enemy.retreatTimer > 0.f) {
				enemy.velocity.x = isLeftOfPlayer ? -200.f : 200.f;
			}
			else {
				enemy.velocity.x = isLeftOfPlayer ? 200.f : -200.f;
			}

			if (enemy.velocity.x > 0 && !enemy.facingRight) {
				enemy.facingRight = true;
				enemy.sprite.setScale(1.f, 1.f);
				enemy.sprite.setOrigin(0.f, 0.f);
			}
			else if (enemy.velocity.x < 0 && enemy.facingRight) {
				enemy.facingRight = false;
				enemy.sprite.setScale(-1.f, 1.f);
				enemy.sprite.setOrigin(enemy.sprite.getLocalBounds().width, 0.f);
			}

			bool hasObstacle = false;
			bool playerCollision = false;
			bool enemyCollision = false;
			sf::FloatRect nextBounds = enemy.sprite.getGlobalBounds();
			nextBounds.left += enemy.velocity.x * delta.asSeconds();
			sf::FloatRect currentBounds = enemy.sprite.getGlobalBounds();

			for (size_t j = 0; j < game.enemies.size(); ++j) {
				if (i != j && currentBounds.intersects(game.enemies[j].sprite.getGlobalBounds())) {
					hasObstacle = true;
					enemyCollision = true;
					if (game.enemies[j].retreatTimer <= 0.f) {
						game.enemies[j].retreatTimer = 0.5f;
					}
					if (enemy.retreatTimer <= 0.f) {
						enemy.retreatTimer = 0.5f;
					}
					break;
				}
			}

			for (size_t j = 0; j < game.enemies.size(); ++j) {
				if (i != j && nextBounds.intersects(game.enemies[j].sprite.getGlobalBounds())) {
					hasObstacle = true;
					enemyCollision = true;
					break;
				}
			}
			if (nextBounds.intersects(game.player.sprite.getGlobalBounds())) {
				hasObstacle = true;
				playerCollision = true;
				if (enemy.retreatTimer <= 0.f) {
					enemy.retreatTimer = 1.f;
				}
			}

			if (hasObstacle) {
				std::vector<float> availableY = possibleY;
				std::vector<int> yCounts(possibleY.size(), 0);
				for (size_t j = 0; j < game.enemies.size(); ++j) {
					if (i != j) {
						float otherY = game.enemies[j].sprite.getPosition().y;
						for (size_t k = 0; k < possibleY.size(); ++k) {
							if (std::abs(otherY - possibleY[k]) < 1.f) {
								yCounts[k]++;
							}
						}
					}
				}
				float playerY = game.player.sprite.getPosition().y;
				if (!playerCollision) {
					availableY.erase(std::remove_if(availableY.begin(), availableY.end(),
						[playerY](float y) { return std::abs(y - playerY) < 50.f; }), availableY.end());
				}

				if (!availableY.empty()) {
					float currentY = enemy.sprite.getPosition().y;
					size_t minCountIndex = 0;
					int minCount = yCounts[0];
					float closestY = possibleY[0];
					float minDist = std::abs(possibleY[0] - currentY);
					for (size_t k = 0; k < possibleY.size(); ++k) {
						float y = possibleY[k];
						if (std::find(availableY.begin(), availableY.end(), y) != availableY.end()) {
							if (yCounts[k] < minCount || (yCounts[k] == minCount && std::abs(y - currentY) < minDist)) {
								minCount = yCounts[k];
								minCountIndex = k;
								closestY = y;
								minDist = std::abs(y - currentY);
							}
						}
					}
					enemy.sprite.setPosition(enemy.sprite.getPosition().x, closestY);
				}
				else if (playerCollision || enemyCollision) {
					size_t minCountIndex = std::min_element(yCounts.begin(), yCounts.end()) - yCounts.begin();
					enemy.sprite.setPosition(enemy.sprite.getPosition().x, possibleY[minCountIndex]);
				}
			}

			bool enemyShooting = false;
			if (enemy.shootTimer < 0.5f) {
				enemy.velocity = sf::Vector2f(0.f, 0.f);
				enemy.animationFrames = { &game.gendarmeStandTex, &game.gendarmeShootTex };
				enemy.frameDuration = 0.5f;
			}
			else {
				enemy.velocity.y = 0.f;
				enemy.animationFrames = { &game.gendarmeWalk1Tex, &game.gendarmeWalk2Tex, &game.gendarmeWalk3Tex };
				enemy.frameDuration = 0.2f;
			}

			enemy.sprite.move(enemy.velocity * delta.asSeconds());

			if (enemy.sprite.getPosition().y < 300.f)
				enemy.sprite.setPosition(enemy.sprite.getPosition().x, 300.f);
			if (enemy.sprite.getPosition().y > 600.f - enemyHeight)
				enemy.sprite.setPosition(enemy.sprite.getPosition().x, 600.f - enemyHeight);

			if (game.stationPhase == Game::StationPhase::Platform || game.stationPhase == Game::StationPhase::TrainRoof) {
				enemy.shootTimer += delta.asSeconds();
				bool isVisible = enemy.sprite.getPosition().x >= game.player.sprite.getPosition().x - 512.f &&
					enemy.sprite.getPosition().x <= game.player.sprite.getPosition().x + 512.f;
				if (isVisible && enemy.shootTimer >= 2.f) {
					bool shootRight = game.player.sprite.getPosition().x > enemy.sprite.getPosition().x;
					enemyShooting = true;
					sf::Sprite bullet;
					bullet.setTexture(game.bulletNormalTex);
					float bulletScale = (game.stationPhase == Game::StationPhase::MachinistCabin) ? 1.f : 0.5f;
					float bulletX = shootRight ? enemy.sprite.getPosition().x + (game.stationPhase == Game::StationPhase::MachinistCabin ? 256.f : 32.f) : enemy.sprite.getPosition().x - (game.stationPhase == Game::StationPhase::MachinistCabin ? 256.f : 32.f);
					bullet.setPosition(bulletX, enemy.sprite.getPosition().y + (game.stationPhase == Game::StationPhase::MachinistCabin ? 32.f : 32.f));
					if (shootRight) {
						bullet.setScale(bulletScale, bulletScale);
					}
					else {
						bullet.setScale(-bulletScale, bulletScale);
						bullet.setOrigin(bullet.getLocalBounds().width, 0.f);
					}
					game.enemyBullets.push_back(bullet);
					enemy.shootTimer = 0.f;
					enemy.animationFrames = { &game.gendarmeStandTex, &game.gendarmeShootTex };
					enemy.currentFrame = 0;
					enemy.animationTimer = 0.f;
					enemy.frameDuration = 0.5f;
					enemy.sprite.setTexture(*enemy.animationFrames[0]);
				}
			}

			if (game.player.invincibilityTimer <= 0.f && enemy.sprite.getGlobalBounds().intersects(game.player.sprite.getGlobalBounds())) {
				game.player.health--;
				game.player.invincibilityTimer = 0.5f;
				if (game.player.health <= 0) {
					game.player.invincibilityTimer = 1.f;
					return;
				}
			}
			if (enemy.sprite.getPosition().x < game.player.sprite.getPosition().x - 1124.f && !isLeftOfPlayer)
				enemy.active = false;
		}
	}

	for (auto it = game.enemyBullets.begin(); it != game.enemyBullets.end();) {
		float bulletSpeed = it->getScale().x > 0 ? 500.f : -500.f;
		it->move(bulletSpeed * delta.asSeconds(), 0.f);
		bool remove = false;
		if (it->getPosition().x < game.player.sprite.getPosition().x - 1124.f || it->getPosition().x > game.player.sprite.getPosition().x + 1124.f)
			remove = true;

		if (game.player.invincibilityTimer <= 0.f && it->getGlobalBounds().intersects(game.player.sprite.getGlobalBounds())) {
			game.player.health--;
			game.player.invincibilityTimer = 0.5f;
			remove = true;
			if (game.player.health <= 0) {
				game.player.invincibilityTimer = 1.f;
				return;
			}
		}

		if (remove)
			it = game.enemyBullets.erase(it);
		else
			++it;
	}

	if (game.stationPhase != Game::StationPhase::MachinistCabin && !pendingPhaseTransition) {
		bool transitionTriggered = false;
		std::string transitionReason;
		if (rightmostEnemyIndex >= 0 && rightmostEnemyIndex < game.enemies.size() && !game.enemies[rightmostEnemyIndex].active) {
			transitionTriggered = true;
			transitionReason = "Rightmost enemy killed";
		}
		else if (game.enemies.empty()) {
			transitionTriggered = true;
			transitionReason = "All enemies killed";
		}
		if (transitionTriggered) {
			pendingPhaseTransition = true;
			phaseTransitionTimer = 3.f;
			nextPhase = (game.stationPhase == Game::StationPhase::Platform) ? Game::StationPhase::TrainRoof : Game::StationPhase::MachinistCabin;
			std::cout << transitionReason << ", initiating "
				<< (nextPhase == Game::StationPhase::TrainRoof ? "TrainRoof" : "MachinistCabin")
				<< " transition in 3 seconds" << std::endl;
		}
	}

	if (pendingPhaseTransition) {
		phaseTransitionTimer -= delta.asSeconds();
		if (phaseTransitionTimer <= 0.f) {
			if (nextPhase == Game::StationPhase::TrainRoof) {
				game.stationPhase = Game::StationPhase::TrainRoof;
				game.player.health = 10;
				game.player.invincibilityTimer = 0.5f;
				game.player.sprite.setScale(0.5f, 0.5f);
				game.background.setTexture(game.carriagesBgTex);
				game.background2.setTexture(game.carriagesBgTex);
				game.backgroundOffsetX = 0.f;
				game.goal.sprite.setPosition(-1000.f, -1000.f);
				game.player.sprite.setPosition(200.f, game.player.sprite.getPosition().y);
				game.levelProgress = 0.f;

				game.enemies.clear();
				std::vector<sf::Vector2f> gridPositions;
				std::uniform_real_distribution<float> yDist(300.f, 500.f);
				std::uniform_real_distribution<float> shootTimerDist(0.f, 2.f);
				for (int x = 0; x < 6; ++x) {
					float posX = 512.f + x * 500.f;
					if (posX < game.levelLength - 100.f) {
						float posY = yDist(game.getRNG());
						gridPositions.emplace_back(posX, posY);
					}
				}
				std::shuffle(gridPositions.begin(), gridPositions.end(), game.getRNG());

				float maxX = 0.f;
				rightmostEnemyIndex = -1;
				for (int i = 0; i < 6 && !gridPositions.empty(); ++i) {
					Game::Entity gendarme;
					gendarme.sprite.setTexture(game.gendarmeStandTex);
					sf::Vector2f pos = gridPositions.back();
					gendarme.sprite.setPosition(pos);
					gendarme.velocity = sf::Vector2f(-200.f, 0.f);
					gendarme.health = 2;
					gendarme.shootTimer = shootTimerDist(game.getRNG());
					gendarme.invincibilityTimer = 0.f;
					gendarme.facingRight = false;
					gendarme.retreatTimer = 0.f;
					gendarme.animationFrames = { &game.gendarmeWalk1Tex, &game.gendarmeWalk2Tex, &game.gendarmeWalk3Tex };
					gendarme.currentFrame = 0;
					gendarme.animationTimer = 0.f;
					gendarme.frameDuration = 0.2f;
					if (!gendarme.facingRight) {
						gendarme.sprite.setScale(-1.f, 1.f);
						gendarme.sprite.setOrigin(gendarme.sprite.getLocalBounds().width, 0.f);
					}
					game.enemies.push_back(gendarme);
					if (pos.x > maxX) {
						maxX = pos.x;
						rightmostEnemyIndex = static_cast<int>(game.enemies.size()) - 1;
					}
					gridPositions.pop_back();
				}

				for (auto& enemy : game.enemies) {
					float enemyHeight = enemy.sprite.getGlobalBounds().height;
					if (enemy.sprite.getPosition().y > 600.f - enemyHeight)
						enemy.sprite.setPosition(enemy.sprite.getPosition().x, 600.f - enemyHeight);
				}

				pendingPhaseTransition = false;
				phaseTransitionTimer = 0.f;
				std::cout << "Transitioned to TrainRoof phase, rightmost enemy index=" << rightmostEnemyIndex
					<< " at x=" << (rightmostEnemyIndex >= 0 ? game.enemies[rightmostEnemyIndex].sprite.getPosition().x : -1)
					<< ", gendarmes spawned=" << game.enemies.size() << std::endl;
			}
			else if (nextPhase == Game::StationPhase::MachinistCabin) {
				initMachinistCabin(game);
				pendingPhaseTransition = false;
				phaseTransitionTimer = 0.f;
				return;
			}
		}
	}

	if (game.stationPhase == Game::StationPhase::MachinistCabin && game.enemies.empty() &&
		game.player.sprite.getGlobalBounds().intersects(game.goal.sprite.getGlobalBounds()) &&
		std::abs(game.player.sprite.getPosition().x - (1024.f - playerWidth)) < 1.f) {
		game.state = Game::GameState::Win;
		game.winMessageTimer = 0.f;
	}

	if (game.player.health <= 0 && game.player.invincibilityTimer > 0.f) {
		return;
	}

	game.enemies.erase(std::remove_if(game.enemies.begin(), game.enemies.end(),
		[](const Game::Entity& e) { return !e.active; }), game.enemies.end());

	if (game.player.health <= 0 && game.player.invincibilityTimer <= 0.f) {
		game.state = Game::GameState::GameOver;
		game.gameOverTimer = 0.f;
	}

	if (game.stationPhase != Game::StationPhase::MachinistCabin)
		game.levelProgress = game.player.sprite.getPosition().x / game.levelLength;

	// Update animations
	Renderer::updateEntityAnimations(game, delta);
}