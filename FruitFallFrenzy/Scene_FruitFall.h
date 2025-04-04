
#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"
#include "GameEngine.h"


struct LevelConfig {
	float					fruitSpeed{ 200.f };
	float					spawnFruitTimer = 0.f;
	float					spawnBombTimer = 0.f;
	float					spawnPowerUpTimer = 0.f;
	float					spawnFruitInterval{ 1.f };		// time between fruit spawns
	float					spawnBombsInterval{ 3.f };		// time between bombs spawns
	float					spawnPowerUpInterval{ 5.f };	// time between power ups spawns
	float					gameTime = 0.f;					// game total time
	float					countdownTime{ 100.f };			// 60s countdown
	int						currentScore{ 0 };
	int						highestScore{ 0 };

	std::shared_ptr<Entity>	magnetEntity = nullptr ;		// magnet power up entity pointer
	float					magnetTimer = 0.0f;				// Magnet effect timer

	std::shared_ptr<Entity>	slowdownEntity = nullptr;		// slowdown power up entity pointer
	float					slowdownTimer = 0.0f;			// slowdown effect timer


	std::vector<std::string> fruitTypes = { "mango", "apple", "banana", "watermelon", "cherry", "strawbury"};
	std::vector<std::string> bombTypes = { "bomb" };
	std::vector<std::string> powerupTypes = { "slowdown", "magnet", "pineapple", "time"};

	sf::Texture				_cloudTextures[6];
	sf::Sprite				_clouds[6];
	sf::Vector2f			_cloudSpeeds[6];
	sf::Vector2f			_cloudBasePos[6]; // save initial position for floating effect

	float					_cloudFloatTime = 0.f;

	std::vector<std::shared_ptr<Entity>>			caughtFruits;
	std::map<std::string, int>						fruitCollected;


};


class Scene_FruitFall : public Scene {

	sPtrEntt                            _player{ nullptr };
	sPtrEntt                            _playerFront{ nullptr };
	sPtrEntt                            _playerBack{ nullptr };

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
	void                    updateMagnetEffect(sf::Time dt);
	void                    init(const std::string& path);
	void                    loadLevel(const std::string& path);

	void					saveHighestScore();
	void					loadHighestScore();

public:
	 Scene_FruitFall(GameEngine* gameEngine, const std::string& levelPath);

	 
	 void		update(sf::Time dt) override;
	 void		sDoAction(const Command& action) override;
	 void		sRender() override;
};
