#include "Scene_Frogger.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include "Physics.h"

#include <fstream>
#include <iostream>
//
// Created by David Burchill on 2023-09-27.
//


Scene_Frogger::Scene_Frogger(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine)
{   
    init(levelPath);
}

void Scene_Frogger::init(const std::string& levelPath)
{
	loadLevel(levelPath);
	registerActions();

	_worldView = _game->window().getDefaultView();

	float baseY = _worldBounds.height - 60.f;
	float rowSpacing = 40.f;

	_spawnPoints.push({ "car", baseY });
	_spawnPoints.push({ "raceCarL", baseY - rowSpacing });
	_spawnPoints.push({ "tractor", baseY - 2 * rowSpacing });
	_spawnPoints.push({ "raceCarR", baseY - 3 * rowSpacing });
	_spawnPoints.push({ "truck", baseY - 4 * rowSpacing });

	_spawnPoints.push({ "3turtles", baseY - 6 * rowSpacing });
	_spawnPoints.push({ "tree1", baseY - 7 * rowSpacing });
	_spawnPoints.push({ "tree2", baseY - 8 * rowSpacing });
	_spawnPoints.push({ "2turtles", baseY - 9 * rowSpacing });
	_spawnPoints.push({ "tree1", baseY - 10 * rowSpacing });

	sf::Vector2f spawnPlayerPos{ _worldBounds.width / 2.f, _worldBounds.height - 20.f};
	spawnPlayer(spawnPlayerPos);
	spawnLillyPads();

	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(50);
}

void Scene_Frogger::loadLevel(const std::string& path)
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

void Scene_Frogger::spawnPlayer(sf::Vector2f pos)
{
	_player = _entityManager.addEntity("player");
	_player->addComponent<CTransform>(pos);

	auto bb = _player->addComponent<CAnimation>(Assets::getInstance().getAnimation("up")).animation.getBB();
	_player->addComponent<CBoundingBox>(bb);
	_player->addComponent<CInput>();

}

void Scene_Frogger::registerActions()
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

void Scene_Frogger::sDoAction(const Command& command)
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



void Scene_Frogger::sCollisions()
{
	checkCarsCollision();
	checkLilyCollision();
	checkIfDead(_player);
}


void Scene_Frogger::onEnd()
{
}


void Scene_Frogger::playerMovement()
{
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

}

void Scene_Frogger::annimatePlayer()
{
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
}

void Scene_Frogger::adjustPlayerPosition(sf::Time dt)
{
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
}

void Scene_Frogger::sSpawnNPCs()
{
	while (!_spawnPoints.empty())
	{
		spawnNPCs(3, _spawnPoints.top());
		_spawnPoints.pop();
	}	
}

void Scene_Frogger::checkCarsCollision()
{
	float baseY = _worldBounds.height - 60.f;
	float rowSpacing = 40.f;

	for (auto& car : _entityManager.getEntities("car")) {
		auto overlap = Physics::getOverlap(_player, car);
		auto pos = _player->getComponent<CTransform>().pos;

		if (pos.y <= baseY && pos.y > baseY - 6 * rowSpacing) {
			if (overlap.x > 0 && overlap.y > 0) {
				if (_player->getComponent<CState>().state != "dead") {
					_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("die");
					_player->getComponent<CTransform>().vel = { 0.f, 0.f };
					//_player->removeComponent<CBoundingBox>();			
					_player->addComponent<CState>().state = "dead";
					SoundPlayer::getInstance().play("death");
				}
			}
		}
		else if (pos.y < baseY - 5 * rowSpacing && pos.y >= baseY - 10 * rowSpacing) {
			bool inSafeArea = false;
			for (auto& safeCar : _entityManager.getEntities("car")) {
				auto safeOverlap = Physics::getOverlap(_player, safeCar);
				auto carVel = safeCar->getComponent<CTransform>().vel;

				if (safeOverlap.x > 0 && safeOverlap.y > 0){
					inSafeArea = true;
					_player->getComponent<CTransform>().vel = carVel * 2.0f;

					break;
				}
			}

			if (!inSafeArea) {
				if (_player->getComponent<CState>().state != "dead") {
					_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("die");
					_player->getComponent<CTransform>().vel = { 0.f, 0.f };
					//_player->removeComponent<CBoundingBox>();
					_player->addComponent<CState>().state = "dead";
					SoundPlayer::getInstance().play("death");
				}
			}			
		}
	}
}

void Scene_Frogger::checkLilyCollision()
{	
	auto landYPos = 60.f;
	auto landHeight = 60.f;
	auto yPos = _player->getComponent<CTransform>().pos.y;

	if (yPos >= landYPos && yPos <= landYPos + landHeight) {

		bool inSafeArea = false;
		for (auto& safeLilly : _entityManager.getEntities("lillyPad")) {
			auto safeOverlap = Physics::getOverlap(_player, safeLilly);

			if (safeOverlap.x > 0 && safeOverlap.y > 0) {
				inSafeArea = true;
				_player->getComponent<CTransform>().vel = { 0.f, 0.f };
				_player->getComponent<CTransform>().pos = safeLilly->getComponent<CTransform>().pos;
				_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("frogIcon");
				safeLilly->removeComponent<CBoundingBox>();

				spawnPlayer({ _worldBounds.width / 2.f, _worldBounds.height - 20.f });
				break;
			}
		}

		if (!inSafeArea) {
			if (_player->getComponent<CState>().state != "dead") {
				_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("die");
				_player->getComponent<CTransform>().vel = { 0.f, 0.f };
				//_player->removeComponent<CBoundingBox>();
				_player->addComponent<CState>().state = "dead";
				SoundPlayer::getInstance().play("death");
			}
		}
	}
}

void Scene_Frogger::checkIfDead(sPtrEntt e)
{
	if (e->hasComponent<CState>() && e->getComponent<CState>().state == "dead")
	{
		auto anim = e->getComponent<CAnimation>().animation;
		if (anim.hasEnded()) {
			e->destroy();
			spawnPlayer({ _worldBounds.width / 2.f, _worldBounds.height - 20.f });
			e->getComponent<CState>().state = "alive";
			e->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("up");
		}
	}
}

void Scene_Frogger::spawnNPCs(int num, SpawnPoint sp)
{
	sf::Vector2f pos{ 0.f, sp.y };
	auto width = _worldBounds.width;  
	auto spacer = width / (num + 1);

	sf::Vector2f speed = { _config.enemySpeed, 0.f };

	if (sp.type == "truck" || sp.type == "tree2" || sp.type == "3turtles") {
		num = 2; 
		speed.x = -_config.enemySpeed; // other direction
	}
	else if (sp.type == "raceCarL" || sp.type == "tractor") {
		speed.x = (sp.type == "raceCarL") ? -_config.enemySpeed : _config.enemySpeed;
	}
	else if (sp.type == "car" || sp.type == "raceCarR") {
		speed.x = (sp.type == "car") ? _config.enemySpeed : -_config.enemySpeed;
	}
	else if (sp.type == "2turtles" ) {
		speed.x = _config.enemySpeed;
	}
	else if (sp.type == "tree1") {
		speed.x = (sp.type == "tree1") ? _config.enemySpeed : -_config.enemySpeed;
	}

	for (int i = 1; i <= num; i++) {
		pos.x = i * spacer;
		auto car = _entityManager.addEntity("car");

		auto& tfm = car->addComponent<CTransform>(pos, speed);

		if (speed.x < 0) {
			tfm.angle = 180.f;
		}						
		else
		{
			tfm.angle = 0;
		}
			
		auto bb = car->
			addComponent<CAnimation>(Assets::getInstance().getAnimation(sp.type)).animation.getBB();
		car->addComponent<CBoundingBox>(bb);
		car->addComponent<CAutoPilot>();


	}
}

void Scene_Frogger::spawnLillyPads()
{
	std::vector<sf::Vector2f> lillyPadPositions = {
	{35.f, 100.f}, {138.f, 100.f}, {240.f, 100.f}, {342.f, 100.f}, {444.f, 100.f}
	};

	for (const auto& pos : lillyPadPositions) {
		auto lilly = _entityManager.addEntity("lillyPad");
		lilly->addComponent<CTransform>(pos);
		auto bb = lilly->
			addComponent<CAnimation>(Assets::getInstance().getAnimation("lillyPad")).animation.getBB();

		lilly->addComponent<CBoundingBox>(bb);
	}
}

void Scene_Frogger::sAnimation(sf::Time dt)
{
	for (auto e : _entityManager.getEntities()) {
		// update all animations
		if (e->getComponent<CAnimation>().has) {
			auto& anim = e->getComponent<CAnimation>();
			anim.animation.update(dt);			
		}
	}
}

void Scene_Frogger::sMovement(sf::Time dt)
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


void Scene_Frogger::sUpdate(sf::Time dt)
{
	if (_isPaused)
		return;

	SoundPlayer::getInstance().removeStoppedSounds();
	_entityManager.update();

	sAnimation(dt);
	sMovement(dt);

	// update cars
	for (auto& car : _entityManager.getEntities("car")) {
		auto& tfm = car->getComponent<CTransform>();

		// check if car is out of bounds
		if (tfm.pos.x < 0) {
			// if car is out of left bounds, reset to right side
			tfm.pos.x = _worldBounds.width;
		}
		else if (tfm.pos.x > _worldBounds.width) {
			// if car is out of right bounds, reset to left side
			tfm.pos.x = 0;
		}

		auto anim = car->getComponent<CAnimation>().animation;
		int currentFrame = anim._currentFrame;

		//if (currentFrame == 2) { 
		//	if (car->hasComponent<CBoundingBox>()) {
		//		car->getComponent<CBoundingBox>().has = false;
		//	}
		//}
		//else {
		//	if (car->hasComponent<CBoundingBox>()) {
		//		car->getComponent<CBoundingBox>().has = true;
		//	}
		//}

		// move car
		tfm.pos += tfm.vel * dt.asSeconds();

		// update car animation
		//auto anim = car->getComponent<CAnimation>().animation;
		//if (anim.hasEnded()) {
		//	car->removeComponent<CBoundingBox>();
		//}

	}

	sCollisions();
	sSpawnNPCs();
	adjustPlayerPosition(dt);



}


void Scene_Frogger::update(sf::Time dt)
{
	sUpdate(dt);
}

void Scene_Frogger::sRender()
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
