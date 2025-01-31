#include "Scene_FruitFall.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include "Physics.h"

#include <fstream>
#include <iostream>
//
// Created by David Burchill on 2023-09-27.
//


Scene_FruitFall::Scene_FruitFall(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine)
{   
    init(levelPath);
}

void Scene_FruitFall::init(const std::string& levelPath)
{
	loadLevel(levelPath);
	registerActions();

	_worldView = _game->window().getDefaultView();

	sf::Vector2f spawnPlayerPos{ _worldBounds.width / 2.f, _worldBounds.height - 344.f};
	spawnPlayer(spawnPlayerPos);

	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(50);
}

void Scene_FruitFall::loadLevel(const std::string& path)
{
	std::ifstream config(path);
	if (config.fail()) {
		std::cerr << "Open file " << path << " failed\n";
		config.close();
		exit(1);
	}

	std::string token{ "" };
	config >> token;
	while (!config.eof()) {
		if (token == "Bkg") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = _entityManager.addEntity("bkg");

			// for background, no textureRect its just the whole texture
			// and no center origin, position by top left corner
			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
			//e->addComponent<CTransform>(); // default ctor 0,0 pos 0,0 vel
			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token == "World") {
			config >> _worldBounds.width >> _worldBounds.height;
		}


		if (config.fail()) {
			config.clear(); // clear error on stream
			std::cout << "*** Error reading config file\n";
		}

		config >> token;
	}

	config.close();
}

void Scene_FruitFall::spawnPlayer(sf::Vector2f pos)
{
	_player = _entityManager.addEntity("player");
	_player->addComponent<CTransform>(pos);

	auto bb = _player->addComponent<CAnimation>(Assets::getInstance().getAnimation("basket")).animation.getBB();
	_player->addComponent<CBoundingBox>(bb);
	_player->addComponent<CInput>();

}

void Scene_FruitFall::registerActions()
{
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");

	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::Q, "QUIT");
}

void Scene_FruitFall::sDoAction(const Command& command)
{
	
	// On Key Press
	if (command.type() == "START") {
		if (command.name() == "PAUSE") { setPaused(!_isPaused); }
		else if (
			command.name() == "QUIT") {
			_game->quitLevel();
		}
		else if (
			command.name() == "BACK") {
			_game->backLevel();
		}

		else if (
			command.name() == "TOGGLE_TEXTURE") {
			_drawTextures = !_drawTextures;
		}
		else if (
			command.name() == "TOGGLE_COLLISION") {
			_drawAABB = !_drawAABB;
		}


		// Player control
		else if (command.name() == "LEFT") { _player->getComponent<CInput>().left = true; }
		else if (
			command.name() == "RIGHT") {
			_player->getComponent<CInput>().right = true;
		}
		else if (
			command.name() == "UP") {
			_player->getComponent<CInput>().up = true;
		}
		else if (command.name() == "DOWN") {
			_player->getComponent<CInput>().down = true;
		}
		else if (command.name() == "TOGGLE") {
			_player->getComponent<CInput>().down = true;
		}

	}
	// on Key Release
	else if (command.type() == "END") {
		// Player control
		if (command.name() == "LEFT") { _player->getComponent<CInput>().left = false; }
		else if (
			command.name() == "RIGHT") {
			_player->getComponent<CInput>().right = false;
		}
		else if (
			command.name() == "UP") {
			_player->getComponent<CInput>().up = false;
		}
		else if (command.name() == "DOWN") {
			_player->getComponent<CInput>().down = false;
		}
	}
	
}



void Scene_FruitFall::sCollisions()
{

}


void Scene_FruitFall::onEnd()
{
}


void Scene_FruitFall::playerMovement()
{
	/*
	// no movement if player is dead
	if (_player->hasComponent<CState>() && _player->getComponent<CState>().state == "dead")
		return;

	// player movement
	auto& pInput = _player->getComponent<CInput>();
	auto& transform = _player->getComponent<CTransform>();

	sf::Vector2f pv{ 0.f, 0.f };

	if (pInput.up && !pInput.down)
	{
		transform.pos.y -= 40;
		_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("up");
		SoundPlayer::getInstance().play("hop");
		pInput.up = false;
	}
		
	else if (pInput.left && !pInput.right)
	{
		transform.pos.x -= 40;
		_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("left");
		SoundPlayer::getInstance().play("hop");
		pInput.left = false;
	}
		
	else if (pInput.right && !pInput.left)
	{
		transform.pos.x += 40; 
		_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("right");
		SoundPlayer::getInstance().play("hop");
		pInput.right = false;
	}
		
	else if (pInput.down && !pInput.up) {
		transform.pos.y += 40;
		_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("down");
		SoundPlayer::getInstance().play("hop");
		pInput.down = false;
	}
	
	// normalize
	_player->getComponent<CTransform>().vel = normalize(pv);

	annimatePlayer();
	*/

}

void Scene_FruitFall::annimatePlayer()
{
	/*
	if (_player->getComponent<CState>().state == "dead")
		return;

	auto& pInput = _player->getComponent<CInput>();
	if (pInput.up)
		_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("up");
	else if (pInput.down)
		_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("down");
	else if (pInput.left)
		_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("left");
	else if (pInput.right)
		_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("right");

		*/
}

void Scene_FruitFall::adjustPlayerPosition(sf::Time dt)
{
	/*
	// don't ajust position if dead
	if (_player->getComponent<CState>().state == "dead")
		return;

	auto& playerTfm = _player->getComponent<CTransform>();
	playerTfm.pos += playerTfm.vel * dt.asSeconds();

	auto center = _worldView.getCenter();
	sf::Vector2f viewHalfSize = _game->windowSize() / 2.f;

	auto left = center.x - viewHalfSize.x;
	auto right = center.x + viewHalfSize.x;
	auto top = center.y - viewHalfSize.y;
	auto bot = center.y + viewHalfSize.y;

	auto& player_pos = _player->getComponent<CTransform>().pos;
	auto halfSize = _player->getComponent<CBoundingBox>().halfSize;

	// keep player in bounds
	player_pos.x = std::max(player_pos.x, left + halfSize.x);
	player_pos.x = std::min(player_pos.x, right - halfSize.x);
	player_pos.y = std::max(player_pos.y, top + halfSize.y);
	player_pos.y = std::min(player_pos.y, bot - halfSize.y);
	*/
}

void Scene_FruitFall::sAnimation(sf::Time dt)
{

	for (auto e : _entityManager.getEntities()) {
		// update all animations
		if (e->getComponent<CAnimation>().has) {
			auto& anim = e->getComponent<CAnimation>();
			anim.animation.update(dt);			
		}
	}
}

void Scene_FruitFall::sMovement(sf::Time dt)
{
	playerMovement();


	// move all the objects that have a transform component
	for (auto e : _entityManager.getEntities()) {
		if (e->hasComponent<CTransform>()) {
			auto& tfm = e->getComponent<CTransform>();
			tfm.pos += tfm.vel * dt.asSeconds();
		}
	}
}


void Scene_FruitFall::sUpdate(sf::Time dt)
{
	if (_isPaused)
		return;

	SoundPlayer::getInstance().removeStoppedSounds();
	_entityManager.update();

	sAnimation(dt);
	sMovement(dt);

	sCollisions();
	adjustPlayerPosition(dt);



}


void Scene_FruitFall::update(sf::Time dt)
{
	sUpdate(dt);
}

void Scene_FruitFall::sRender()
{
	// Draw Background first
	for (auto e : _entityManager.getEntities("bkg")) {
		if (e->getComponent<CSprite>().has) {
			auto& sprite = e->getComponent<CSprite>().sprite;
			_game->window().draw(sprite);
		}
	}
	
	// Draw Entities except frog
	for (auto e : _entityManager.getEntities()) {
		if (e == _player) continue;

		if (e->getComponent<CAnimation>().has) {
			auto& anim = e->getComponent<CAnimation>().animation;
			auto& tfm = e->getComponent<CTransform>();
			anim.getSprite().setPosition(tfm.pos);
			_game->window().draw(anim.getSprite());
		}

		if (_drawAABB && e->hasComponent<CBoundingBox>()) {
			auto& bb = e->getComponent<CBoundingBox>();
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f{ bb.size.x, bb.size.y });
			centerOrigin(rect);
			rect.setPosition(e->getComponent<CTransform>().pos);
			rect.setFillColor(sf::Color{0, 0, 0, 0});
			rect.setOutlineColor(sf::Color{ 0, 255, 0});
			rect.setOutlineThickness(2.f);
			_game->window().draw(rect);
		}
	}
	

	// Draw Frog
	if (_player->getComponent<CAnimation>().has) {
		auto& anim = _player->getComponent<CAnimation>().animation;
		auto& tfm = _player->getComponent<CTransform>();
		anim.getSprite().setPosition(tfm.pos);
		_game->window().draw(anim.getSprite());
	}
}
