
#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"
#include "GameEngine.h"


struct LevelConfig {
	float       fruitSpeed{ 200.f };
	float		spawnFruitTimer = 0.f;
	float		spawnBombTimer = 0.f;
	float		spawnPowerUpTimer = 0.f;
	float		spawnFruitInterval{ 1.f }; // time between fruit spawns
	float		spawnBombsInterval{ 3.f }; // time between bombs spawns
	float		spawnPowerUpInterval{ 5.f }; // time between power ups spawns
	//int			gamePhase = 1;        // （1 = only fruits, 2 = adding bombs, 3 = adding power ups, 4 = more speed）
	float		gameTime = 0.f;     // game total time
	float		countdownTime{ 30.f }; // 60s countdown
	int			currentScore{ 0 };
	int			highestScore{ 0 };

	std::vector<std::string> fruitTypes = { "mango", "apple", "banana", "watermelon", "cherry", "strawbury"};
	std::vector<std::string> bombTypes = { "bomb" };
	std::vector<std::string> powerupTypes = { "slowdown", "magnet", "pineapple", "time"};

};


class Scene_FruitFall : public Scene {

	sPtrEntt                            _player{ nullptr };
	sf::View                            _worldView;  // camera
	sf::FloatRect                       _worldBounds;
	bool                                _drawTextures{ true };
	bool                                _drawAABB{ false };
	LevelConfig                         _config;

	bool 								_showGameOverScreen{ false };
	float								_gameOverTimer{0.f};
	float								_overlayAlpha{ 0.f };
	float								_victoryAlpha{ 0.f };

	//systems
	void 				    sAnimation(sf::Time dt);
	void                    sMovement(sf::Time dt);
	void                    sCollisions();
	void                    sUpdate(sf::Time dt);
	void					onEnd() override;

	// helper functions
	void	                registerActions();
	void                    spawnPlayer(sf::Vector2f pos);
	//void                    spawnFruit(sf::Vector2f pos);
	void                    spawnFruit();
	void                    spawnIcons();
	void                    spawnBombs();
	void                    spawnPowerUps();
	void                    playerMovement();
	void                    annimatePlayer();
	void                    adjustPlayerPosition(sf::Time dt);
	void                    adjustFruitPosition(sf::Time dt);
	void                    checkFruitsCollision();
	void                    checkBombsCollision();
	void                    checkPowerUpsCollision();
	void                    updateSlowdownEffect(sf::Time dt);
	void                    init(const std::string& path);
	void                    loadLevel(const std::string& path);


public:
	 Scene_FruitFall(GameEngine* gameEngine, const std::string& levelPath);

	 
	 void		update(sf::Time dt) override;
	 void		sDoAction(const Command& action) override;
	 void		sRender() override;
};
