
#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"
#include "GameEngine.h"


struct LevelConfig {
	float       playerSpeed{ 200.f };
	float       enemySpeed{ 10.f };
	float       missileSpeed{ 150.f };
	float		spawnInterval{ 2.f };
	float		friendsSpeed{ 10.f };
};

class Scene_FruitFall : public Scene {

	sPtrEntt                            _player{ nullptr };
	sf::View                            _worldView;  // camera
	sf::FloatRect                       _worldBounds;
	bool                                _drawTextures{ true };
	bool                                _drawAABB{ false };
	LevelConfig                         _config;

	//systems
	void 				    sAnimation(sf::Time dt);
	void                    sMovement(sf::Time dt);
	void                    sCollisions();
	void                    sUpdate(sf::Time dt);
	void					onEnd() override;

	// helper functions
	void	                registerActions();
	void                    spawnPlayer(sf::Vector2f pos);
	void                    playerMovement();
	void                    annimatePlayer();
	void                    adjustPlayerPosition(sf::Time dt);
	void                    init(const std::string& path);
	void                    loadLevel(const std::string& path);


public:
	 Scene_FruitFall(GameEngine* gameEngine, const std::string& levelPath);

	 
	 void		update(sf::Time dt) override;
	 void		sDoAction(const Command& action) override;
	 void		sRender() override;
};
