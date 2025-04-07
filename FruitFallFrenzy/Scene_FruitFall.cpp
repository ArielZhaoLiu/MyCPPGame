#include "Scene_FruitFall.h"
#include "MusicPlayer.h"
#include "SoundPlayer.h"
#include "Physics.h"

#include <fstream>
#include <iostream>
#include <random>


namespace {
	std::random_device rd;
	std::mt19937 rng(rd());
}

Scene_FruitFall::Scene_FruitFall(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine)
{   
    init(levelPath);
}

void Scene_FruitFall::init(const std::string& levelPath)
{
	loadLevel(levelPath);
	registerActions();
	loadHighestScore();

	_worldView = _game->window().getDefaultView();

	sf::Vector2f spawnPlayerPos{ _worldBounds.width / 2.f, _worldBounds.height - 320.f};
	sf::Vector2f spawnFruitPos{ _worldBounds.width / 2.f, 0.f };

	spawnPlayer(spawnPlayerPos);
	spawnFruit();
	spawnIcons();

	MusicPlayer::getInstance().play("gameTheme");
	MusicPlayer::getInstance().setVolume(10);


	// for clouds
	std::string cloudPaths[3] = {
	"../assets/Textures/cloud1.png",
	"../assets/Textures/cloud2.png",
	"../assets/Textures/cloud3.png"
	};

	for (int i = 0; i < 6; ++i) {
		int texIndex = i / 2;  // 2 clouds each line using the same texture.

		if (_config._cloudTextures[texIndex].loadFromFile(cloudPaths[texIndex])) {
			_config._clouds[i].setTexture(_config._cloudTextures[texIndex]);


			// initial spawn position
			float startX = static_cast<float>(900 * i);
			float startY = 150.f + texIndex * 180.f;
			//float startY = 800.f + (i * 600.f);  
			_config._clouds[i].setPosition(startX, startY);
			_config._clouds[i].setScale(0.8f, 0.8f);
			_config._clouds[i].setColor(sf::Color(255, 255, 255, 200));

			_config._cloudBasePos[i] = _config._clouds[i].getPosition();

			_config._cloudSpeeds[i] = sf::Vector2f(20.f + texIndex * 10.f, 0.f);
			//_cloudSpeeds[i] = sf::Vector2f(40.f + (i % 3) * 30.f, 0.f); 
		}
		else {
			std::cerr << "Failed to load cloud image: " << cloudPaths[texIndex] << std::endl;
		}
	}
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
		else if (token == "GameFrontBg") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = _entityManager.addEntity("gameFrontBg");

			auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;

			sprite.setOrigin(0.f, 0.f);
			sprite.setPosition(pos);
		}
		else if (token == "WinBkg") {
			std::string name;
			sf::Vector2f pos;
			config >> name >> pos.x >> pos.y;
			auto e = _entityManager.addEntity("winbkg");

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

void Scene_FruitFall::saveHighestScore()
{
	std::ofstream file("highscore.txt");
	if (file.is_open()) {
		file << _config.highestScore;
		file.close();
	}
	else {
		std::cout << "Unable to open file for writing: highscore.txt\n";
	}
}

void Scene_FruitFall::loadHighestScore()
{
	std::ifstream file("highscore.txt");
	if (file.is_open()) {
		file >> _config.highestScore;
		file.close();
	}
	else {
		std::cout << "Unable to open file for reading: highscore.txt\n";
	}
}

void Scene_FruitFall::createScorePopup(sf::Vector2f pos, const std::string& text)
{
	auto popup = _entityManager.addEntity("scorePopup");
	popup->addComponent<CTransform>(pos);
	popup->addComponent<CScorePopup>();

	auto& popupComp = popup->getComponent<CScorePopup>();
	popupComp.text = text;
	popupComp.velocity = sf::Vector2f(0.f, -50.f);
	popupComp.lifetime = 1.f;
	popupComp.maxLifetime = 1.f;
}

void Scene_FruitFall::spawnPlayer(sf::Vector2f pos)
{
	// player logic
	_player = _entityManager.addEntity("player");
	_player->addComponent<CTransform>(pos);

	auto bb = Assets::getInstance().getAnimation("stand_front").getBB();

	bb.x = 0.4 * bb.x;
	bb.y = 0.1 * bb.y;

	_player->addComponent<CBoundingBox>(bb);
	_player->addComponent<CInput>();

	// visual: player front
	_playerFront = _entityManager.addEntity("playerFront");
	_playerFront->addComponent<CTransform>(pos);
	_playerFront->addComponent<CAnimation>(Assets::getInstance().getAnimation("stand_front"));

	// visual: player back
	_playerBack = _entityManager.addEntity("playerBack");
	_playerBack->addComponent<CTransform>(pos);
	_playerBack->addComponent<CAnimation>(Assets::getInstance().getAnimation("stand_back"));
}


void Scene_FruitFall::spawnFruit()
{
	auto fruitTypes = _config.fruitTypes;

	// spawn random fruit type
	std::uniform_int_distribution<int> dist(0, fruitTypes.size() - 1);
	auto fruitType = fruitTypes[dist(rng)];

	// spawn random position
	auto pos = sf::Vector2f{ 0.f, 0.f };
	std::uniform_int_distribution<int> distX(180, static_cast<int>(_worldBounds.width - 200));
	pos.x = static_cast<float>(distX(rng));


	auto vel = _config.fruitSpeed * uVecBearing(90);
	auto e = _entityManager.addEntity("fruit");
	e->addComponent<CTransform>(pos, vel);
	e->addComponent<CAnimation>(Assets::getInstance().getAnimation(fruitType));
	auto bb = e->getComponent<CAnimation>().animation.getBB();
	float scale = 0.8f;
	bb.x *= scale;
	bb.y *= scale;
	e->addComponent<CBoundingBox>(bb);
}

void Scene_FruitFall::spawnIcons()
{
	auto timerEntity = _entityManager.addEntity("timer");
	timerEntity->addComponent<CTransform>(sf::Vector2f{ 90.f, 70.f });
	timerEntity->addComponent<CAnimation>(Assets::getInstance().getAnimation("timer"));


	auto hintEntity = _entityManager.addEntity("hint");
	hintEntity->addComponent<CTransform>(sf::Vector2f{ 90.f, 540.f });
	hintEntity->addComponent<CAnimation>(Assets::getInstance().getAnimation("hint"));

	auto restartEntity = _entityManager.addEntity("restart");
	restartEntity->addComponent<CTransform>(sf::Vector2f{ 90.f, 645.f });
	restartEntity->addComponent<CAnimation>(Assets::getInstance().getAnimation("restart"));

	auto pauseEntity = _entityManager.addEntity("pause");
	pauseEntity->addComponent<CTransform>(sf::Vector2f{ 90.f, 742.f });
	pauseEntity->addComponent<CAnimation>(Assets::getInstance().getAnimation("pause"));

	auto escEntity = _entityManager.addEntity("esc");
	escEntity->addComponent<CTransform>(sf::Vector2f{ 90.f, 852.f });
	escEntity->addComponent<CAnimation>(Assets::getInstance().getAnimation("esc"));

}

void Scene_FruitFall::spawnBombs()
{
	auto bomb = _config.bombTypes[0];

	// spawn random position
	auto pos = sf::Vector2f{ 0.f, 0.f };
	std::uniform_int_distribution<int> distX(50, static_cast<int>(_worldBounds.width - 100));
	pos.x = static_cast<float>(distX(rng));

	auto vel = _config.fruitSpeed * uVecBearing(90);
	auto e = _entityManager.addEntity("bomb");
	e->addComponent<CTransform>(pos, vel);
	e->addComponent<CAnimation>(Assets::getInstance().getAnimation(bomb));
	auto bb = e->getComponent<CAnimation>().animation.getBB();
	float scale = 0.8f;
	bb.x *= scale;
	bb.y *= scale;
	e->addComponent<CBoundingBox>(bb);
}

void Scene_FruitFall::spawnPowerUps()
{
	auto powerUpTypes = _config.powerupTypes;

	// spawn random power up
	std::uniform_int_distribution<int> dist(0, powerUpTypes.size() - 1);
	auto powerUpType = powerUpTypes[dist(rng)];

	// spawn random position
	auto pos = sf::Vector2f{ 0.f, 0.f };
	std::uniform_int_distribution<int> distX(50, static_cast<int>(_worldBounds.width - 100));
	pos.x = static_cast<float>(distX(rng));


	auto vel = _config.fruitSpeed * uVecBearing(90);
	auto e = _entityManager.addEntity("powerUp");
	e->addComponent<CTransform>(pos, vel);
	e->addComponent<CAnimation>(Assets::getInstance().getAnimation(powerUpType));
	auto bb = e->getComponent<CAnimation>().animation.getBB();
	float scale = 0.8f;
	bb.x *= scale;
	bb.y *= scale;
	e->addComponent<CBoundingBox>(bb);
}

void Scene_FruitFall::registerActions()
{
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	//registerAction(sf::Keyboard::W, "UP");
	//registerAction(sf::Keyboard::Up, "UP");
	//registerAction(sf::Keyboard::S, "DOWN");
	//registerAction(sf::Keyboard::Down, "DOWN");

	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::E, "EXIT");
}

void Scene_FruitFall::sDoAction(const Command& command)
{
	
	// On Key Press
	if (command.type() == "START") {
		if (command.name() == "PAUSE") { setPaused(!_isPaused); }
		else if (
			command.name() == "EXIT") {
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
		/*else if (
			command.name() == "UP") {
			_player->getComponent<CInput>().up = true;
		}
		else if (command.name() == "DOWN") {
			_player->getComponent<CInput>().down = true;
		}*/
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
		/*else if (
			command.name() == "UP") {
			_player->getComponent<CInput>().up = false;
		}
		else if (command.name() == "DOWN") {
			_player->getComponent<CInput>().down = false;
		}*/
	}
	
}



void Scene_FruitFall::sCollisions()
{
	checkFruitsCollision();
	checkBombsCollision();
	checkPowerUpsCollision();
}


void Scene_FruitFall::onEnd()
{// save highest score
	if (_config.currentScore > _config.highestScore) {
		_config.highestScore = _config.currentScore;
		saveHighestScore();
	}
	//MusicPlayer::getInstance().play("gameOver");
	MusicPlayer::getInstance().setVolume(10);

	// remove all entities
	for (auto e : _entityManager.getEntities()) {
		e->destroy();
	}
}


void Scene_FruitFall::playerMovement()
{
	/*
	// no movement if player is dead
	if (_player->hasComponent<CState>() && _player->getComponent<CState>().state == "dead")
		return;
	*/

	// player movement
	auto& pInput = _player->getComponent<CInput>();

	auto& transform = _player->getComponent<CTransform>();
	auto& transformFront = _playerFront->getComponent<CTransform>();
	auto& transformBack = _playerBack->getComponent<CTransform>();


	sf::Vector2f pv{ 0.f, 0.f };
		
	if (pInput.left && !pInput.right)
	{
		transform.pos.x -=40;
		transformFront.pos.x -= 40;
		transformBack.pos.x -= 40;


		if (_playerFront && _playerFront->hasComponent<CAnimation>()) {
			_playerFront->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("left_front");
		}
		if (_playerBack && _playerBack->hasComponent<CAnimation>()) {
			_playerBack->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("left_back");
		}
		
		SoundPlayer::getInstance().play("move");
		pInput.left = false;
	}
		
	else if (pInput.right && !pInput.left)
	{
		transform.pos.x +=40; 
		transformFront.pos.x += 40;
		transformBack.pos.x += 40;

		if (_playerFront && _playerFront->hasComponent<CAnimation>()) {
			_playerFront->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("left_front");
			_playerFront->getComponent<CAnimation>().animation._sprite.setScale(-1.f, 1.f);
		}
		if (_playerBack && _playerBack->hasComponent<CAnimation>()) {
			_playerBack->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("left_back");
			_playerBack->getComponent<CAnimation>().animation._sprite.setScale(-1.f, 1.f);
		}

	
		SoundPlayer::getInstance().play("move");
		pInput.right = false;

	}
	/*else
	{
		_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("stand_front");

	}*/
	
	// normalize
	_player->getComponent<CTransform>().vel = normalize(pv);
	_playerFront->getComponent<CTransform>().vel = normalize(pv);
	_playerBack->getComponent<CTransform>().vel = normalize(pv);


	annimatePlayer();
	

}

void Scene_FruitFall::annimatePlayer()
{
	/*
	if (_player->getComponent<CState>().state == "dead")
		return;
		*/

	//auto& pInput = _player->getComponent<CInput>();
	//if (pInput.up)
	//	_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("basket");
	//else if (pInput.down)
	//	_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("basket");
	//else if (pInput.left)
	//	_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("left");
	//else if (pInput.right)
	//	_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("right");
		
}

void Scene_FruitFall::adjustPlayerPosition(sf::Time dt)
{
	/*
	// don't ajust position if dead
	if (_player->getComponent<CState>().state == "dead")
		return;
		*/

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
	player_pos.x = std::max(player_pos.x, left + halfSize.x + 220);
	player_pos.x = std::min(player_pos.x, right - halfSize.x - 240);
	player_pos.y = std::max(player_pos.y, top + halfSize.y);
	player_pos.y = std::min(player_pos.y, bot - halfSize.y);

	// adjust front and back sprites
	auto& playerTfmFront = _playerFront->getComponent<CTransform>().pos = player_pos;
	auto& playerTfmBack = _playerBack->getComponent<CTransform>().pos = player_pos;

}

void Scene_FruitFall::adjustFruitPosition(sf::Time dt)
{
	for (auto e : _entityManager.getEntities("fruit")) {

		auto& tfm = e->getComponent<CTransform>();
		auto& playerPos = _player->getComponent<CTransform>().pos;
		auto fruitSize = e->getComponent<CBoundingBox>();
		auto& stateComp = e->getComponent<CState>(); 

		if (stateComp.state == "caught") {
			
			// First time caught, save relative position to player
			if (stateComp.relatePosX == 0.0f) {
				stateComp.relatePosX = tfm.pos.x - playerPos.x; 
				_config.caughtFruits.push_back(e);
			}

			tfm.pos.x = playerPos.x + stateComp.relatePosX;
			tfm.pos.y = playerPos.y + 65 - fruitSize.size.y / 2;
			//tfm.vel.y = 0.f;

			if (_config.caughtFruits.size() > 5) {
				_config.caughtFruits.front()->destroy();
				_config.caughtFruits.erase(_config.caughtFruits.begin());
			}
		}

		// if fruit is out of bounds, remove it
		if (tfm.pos.y > _worldBounds.height) {
			e->destroy();
		}
		// if fruit is out of bounds, remove it
		if (tfm.pos.x < 0 || tfm.pos.x > _worldBounds.width) {
			e->destroy();
		}
	}
}

void Scene_FruitFall::checkFruitsCollision()
{

	for (auto e : _entityManager.getEntities("fruit")) {

		// Deault, fruit is falling
		if (!e->hasComponent<CState>()) {
			e->addComponent<CState>().state = "falling";
		}

		auto& fruitState = e->getComponent<CState>().state;

		auto overlap = Physics::getOverlap(_player, e);
		if (fruitState != "caught" && overlap.x > 0 && overlap.y > 0) {
            fruitState = "caught";
			SoundPlayer::getInstance().play("catch2");

			if (e->getComponent<CAnimation>().animation.getName() == "apple")
			{
				_config.currentScore += 2;
				_config.fruitCollected["apple"]++;

				createScorePopup(e->getComponent<CTransform>().pos, "+2");
			}
			else if (e->getComponent<CAnimation>().animation.getName() == "banana")
			{
				_config.currentScore += 2;
				_config.fruitCollected["banana"]++;

				createScorePopup(e->getComponent<CTransform>().pos, "+2");

			}
			else if (e->getComponent<CAnimation>().animation.getName() == "cherry")
			{
				_config.currentScore += 6;
				_config.fruitCollected["cherry"]++;

				createScorePopup(e->getComponent<CTransform>().pos, "+6");

			}
			else if (e->getComponent<CAnimation>().animation.getName() == "mango")
			{
				_config.currentScore += 4;
				_config.fruitCollected["mango"]++;

				createScorePopup(e->getComponent<CTransform>().pos, "+4");
			}
			else if (e->getComponent<CAnimation>().animation.getName() == "strawbury")
			{
				_config.currentScore += 6;
				_config.fruitCollected["strawbury"]++;

				createScorePopup(e->getComponent<CTransform>().pos, "+6");
			}
			else if (e->getComponent<CAnimation>().animation.getName() == "watermelon")
			{
				_config.currentScore += 20;
				_config.fruitCollected["watermelon"]++;

				createScorePopup(e->getComponent<CTransform>().pos, "+20");
			}
		}
	}

}

void Scene_FruitFall::checkBombsCollision()
{
	for (auto e : _entityManager.getEntities("bomb")) {
		auto overlap = Physics::getOverlap(_player, e);
		if (overlap.x > 0 && overlap.y > 0) {

			// check first time explosion
			if (!e->hasComponent<CAnimation>() || !e->getComponent<CAnimation>().hasExploded) {
				_config.currentScore -= 40;
				_config.countdownTime -= 30;

				SoundPlayer::getInstance().play("explosion");
				e->addComponent<CAnimation>(Assets::getInstance().getAnimation("explosion"));
				_player->getComponent<CAnimation>().animation = Assets::getInstance().getAnimation("sadface");

				e->getComponent<CAnimation>().hasExploded = true;

			}			
			//e->destroy();
		}
	}
}

void Scene_FruitFall::checkPowerUpsCollision()
{
	for (auto e : _entityManager.getEntities("powerUp")) {
		auto overlap = Physics::getOverlap(_player, e);
		if (overlap.x > 0 && overlap.y > 0) {

			if (e->getComponent<CAnimation>().animation.getName() == "time")
			{
				_config.countdownTime += 5;

				e->destroy();
			}
			else if (e->getComponent<CAnimation>().animation.getName() == "magnet")
			{
				if (!_player->hasComponent<CMagnetEffect>()) {
					_player->addComponent<CMagnetEffect>(5.f);
				}

				if (_config.magnetEntity == nullptr) {
					_config.magnetEntity = e;
					_config.magnetTimer = 6.f;
				}

				SoundPlayer::getInstance().play("magnet");
				e->destroy();

			}
			else if (e->getComponent<CAnimation>().animation.getName() == "slowdown")
			{
				if (!_player->hasComponent<CSlowDownEffect>()) {
					_player->addComponent<CSlowDownEffect>(5.f, 300.f);
					_config.fruitSpeed -= 300;
				}		

				if (_config.slowdownEntity == nullptr) {
					_config.slowdownEntity = e;
					_config.slowdownTimer = 5.f;
				}

				e->destroy();
			
			}
			else if (e->getComponent<CAnimation>().animation.getName() == "pineapple")
			{
				// to do
			}

			
		}
	}
}

void Scene_FruitFall::updateSlowdownEffect(sf::Time dt)
{
	if (_player->hasComponent<CSlowDownEffect>()) {
		auto& effect = _player->getComponent<CSlowDownEffect>();
		effect.duration -= dt.asSeconds();
		if (effect.duration <= 0.f) {
			_config.fruitSpeed += effect.speedFactor;
			_player->removeComponent<CSlowDownEffect>();
		}
	}

	// slowdown effect attached to player
	if (_config.slowdownEntity) {
		auto slowdownPos = _player->getComponent<CTransform>().pos;
		float offsetX = -80.f;
		float offsetY = -30.f;

		_config.slowdownEntity->getComponent<CTransform>().pos = slowdownPos + sf::Vector2f{ offsetX, offsetY };

		_config.slowdownTimer -= dt.asSeconds();
		if (_config.slowdownTimer <= 0.f) {
			_config.slowdownEntity->destroy();
			_config.slowdownEntity = nullptr;
		}
	}
}

void Scene_FruitFall::updateMagnetEffect(sf::Time dt)
{
	if (_player->hasComponent<CMagnetEffect>()) {
		auto& effect = _player->getComponent<CMagnetEffect>();

		for (auto e : _entityManager.getEntities("fruit")) {
			auto& tfm = e->getComponent<CTransform>();
			auto& playerTfm = _player->getComponent<CTransform>();
			auto dir = playerTfm.pos - tfm.pos;
			auto vel = normalize(dir) * 360.f;
			tfm.vel = vel;
		}

		effect.duration -= dt.asSeconds();
		if (effect.duration <= 0.f) {
			_player->removeComponent<CMagnetEffect>();
		}
	}

	// magnet effect attached to player
	if (_config.magnetEntity) {
		auto magnetPos = _player->getComponent<CTransform>().pos;

		float offsetX = 140.f;
		float offsetY = -30.f;

		_config.magnetEntity->getComponent<CTransform>().pos = magnetPos + sf::Vector2f{ offsetX, offsetY };


		_config.magnetTimer -= dt.asSeconds();
		if (_config.magnetTimer <= 0.f) {
			_config.magnetEntity->destroy();
			_config.magnetEntity = nullptr;
		}
	}
}

void Scene_FruitFall::sAnimation(sf::Time dt)
{

	for (auto e : _entityManager.getEntities()) {
		// update all animations
		if (e->getComponent<CAnimation>().has) {
			auto& anim = e->getComponent<CAnimation>();
			anim.animation.update(dt);

			if (anim.animation.getName() == "explosion" && anim.animation.hasEnded()) { // for explosion
				SoundPlayer::getInstance().play("sayNo");
				e->destroy();
				
			}
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

	if (_player && _playerBack && _playerFront) {
		auto pos = _player->getComponent<CTransform>().pos;
		_playerBack->getComponent<CTransform>().pos = pos;
		_playerFront->getComponent<CTransform>().pos = pos;
	}
	
	auto e = _entityManager.getEntities();  // check how many entities are there
	std::cout << e.size() << "\n";

	_entityManager.update();
	SoundPlayer::getInstance().removeStoppedSounds();

	_config.spawnFruitTimer += dt.asSeconds();
	_config.spawnBombTimer += dt.asSeconds();
	_config.spawnPowerUpTimer += dt.asSeconds();


	_config.gameTime += dt.asSeconds(); // game total time
	_config.countdownTime -= dt.asSeconds(); // countdown time decrease



	// game phase

	if (_config.gameTime >= 90 && _config.slowdownEntity == nullptr) {
		_config.fruitSpeed = 300.f;
		_config.spawnFruitInterval = 0.1f;
		_config.spawnPowerUpInterval = 2.f;
		_config.spawnBombsInterval = 0.2f;
	}

	if (_config.gameTime >= 85 && _config.slowdownEntity == nullptr) {
		_config.fruitSpeed = 400.f;
		_config.spawnFruitInterval = 0.1f;
		_config.spawnPowerUpInterval = 2.f;
		_config.spawnBombsInterval = 0.2f;
	}

	if (_config.gameTime >= 80 && _config.slowdownEntity == nullptr) {
		_config.fruitSpeed = 600.f;
		_config.spawnFruitInterval = 0.1f;
		_config.spawnPowerUpInterval = 2.f;
		_config.spawnBombsInterval = 0.2f;

	}

	if (_config.gameTime >= 60 && _config.slowdownEntity == nullptr) {
		_config.fruitSpeed = 500.f;
		_config.spawnFruitInterval = 0.2f;
		_config.spawnPowerUpInterval = 2.f;
		_config.spawnBombsInterval = 0.5f;

	}

	if (_config.gameTime >= 40 && _config.slowdownEntity == nullptr) {
		_config.fruitSpeed = 500.f;
		_config.spawnFruitInterval = 0.3f;
		_config.spawnPowerUpInterval = 2.f;
		_config.spawnBombsInterval = 1.f;

	}

	if (_config.gameTime >= 30 && _config.slowdownEntity == nullptr) {
		_config.fruitSpeed = 400.f;
		_config.spawnFruitInterval = 0.4f;
		_config.spawnPowerUpInterval = 3.f;
		
	}

	if (_config.gameTime >= 15 && _config.slowdownEntity == nullptr) {
		_config.fruitSpeed = 350.f;
		_config.spawnFruitInterval = 0.5f;
		_config.spawnPowerUpInterval = 4.f;
		_config.spawnBombsInterval = 2.3f;
	}

	if (_config.gameTime >= 13 && _config.spawnPowerUpTimer >= _config.spawnPowerUpInterval) {
		spawnPowerUps();
		_config.spawnPowerUpTimer = 0.f;
	}

	if (_config.gameTime >= 10) {
		_config.fruitSpeed = 300.f;
	}

	if (_config.gameTime >= 5 && _config.spawnBombTimer >= _config.spawnBombsInterval) {
		spawnBombs();
		_config.spawnBombTimer = 0.f;
	}

	if (_config.spawnFruitTimer >= _config.spawnFruitInterval) {
		spawnFruit();
		_config.spawnFruitTimer = 0.f;
	}

	// Clouds
	float windowWidth = static_cast<float>(_game->window().getSize().x);

	for (int i = 0; i < 6; ++i) {
		sf::Vector2f pos = _config._clouds[i].getPosition();
		pos += _config._cloudSpeeds[i] * dt.asSeconds();

		if (pos.x > windowWidth + _config._clouds[i].getGlobalBounds().width) {
			pos.x = -_config._clouds[i].getGlobalBounds().width;
		}

		int rowIndex = i / 2;
		float floatOffset = std::sin(_config._cloudFloatTime * (0.5f + rowIndex * 0.2f)) * 5.f;
		_config._clouds[i].setPosition(pos.x, _config._cloudBasePos[i].y + floatOffset);

	}

	_config._cloudFloatTime += dt.asSeconds();


	// For popup scores
	for (auto e : _entityManager.getEntities("scorePopup")) {
		auto& tfm = e->getComponent<CTransform>();
		auto& popup = e->getComponent<CScorePopup>();

		tfm.pos += popup.velocity * dt.asSeconds();

		popup.lifetime -= dt.asSeconds();
		if (popup.lifetime <= 0.f) {
			e->destroy();
		}
	}
	
	// Game Over
	if (_config.countdownTime <= 0.f) {
		_isGameOver = true;
		_config.countdownTime = 0;
	}
	
	_entityManager.update();
	sAnimation(dt);
	sMovement(dt);
	sCollisions();
	updateSlowdownEffect(dt);
	updateMagnetEffect(dt);
	adjustPlayerPosition(dt);
	adjustFruitPosition(dt);

	if (_isGameOver) {
		updateGameOver(dt);
		return;
	}

}

void Scene_FruitFall::updateGameOver(sf::Time dt)
{
	_showGameOverScreen = true;
	_gameOverTimer += dt.asSeconds();

	// fruits float up
	auto vel = _config.fruitSpeed * uVecBearing(-90);
	for (auto e : _entityManager.getEntities("fruit"))
	{
		e->getComponent<CTransform>().vel = vel;
	}

	_overlayAlpha += dt.asSeconds() * 100;
	if (_overlayAlpha > 255) _overlayAlpha = 255;

	if (_gameOverTimer >= 2.f) {
		_victoryAlpha += dt.asSeconds() * 200;
		if (_victoryAlpha > 255) _victoryAlpha = 255;
	}

	// highest score
	if (_config.currentScore > _config.highestScore) {
		_config.highestScore = _config.currentScore;
		saveHighestScore();
	}

	// remove all entities 
	if (_gameOverTimer >= 5.f) {
		for (auto e : _entityManager.getEntities()) {
			e->destroy();
		}
	}
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

	// Draw Clouds
	for (int i = 0; i < 5; ++i) {
		_game->window().draw(_config._clouds[i]);
	}


	// Draw Front background
	sf::Sprite frontBg;
	frontBg.setTexture(Assets::getInstance().getTexture("GameFrontBackground"));
	_game->window().draw(frontBg);

	// Draw Back basket
	if (_playerBack && _playerBack->getComponent<CAnimation>().has) {
		auto& anim = _playerBack->getComponent<CAnimation>().animation;
		auto& tfm = _playerBack->getComponent<CTransform>();
		anim.getSprite().setPosition(tfm.pos);
		_game->window().draw(anim.getSprite());
	}

	// Draw Entities except basket and powerups
	for (auto e : _entityManager.getEntities()) {
		if (e == _player || e == _playerFront || e == _playerBack ) continue;

		if (_config.magnetEntity || _config.slowdownEntity) {
			if (e == _config.magnetEntity || e == _config.slowdownEntity) {
				continue;
			}
		}


		if (e->getComponent<CAnimation>().has) {
			auto& anim = e->getComponent<CAnimation>().animation;
			auto& tfm = e->getComponent<CTransform>();
			anim.getSprite().setPosition(tfm.pos);
			_game->window().draw(anim.getSprite());
		}

		// Draw bounding box

		if (_drawAABB && e->hasComponent<CBoundingBox>()) {
			auto& bb = e->getComponent<CBoundingBox>();
			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f{ bb.size.x, bb.size.y });
			centerOrigin(rect);
			rect.setPosition(e->getComponent<CTransform>().pos);
			rect.setFillColor(sf::Color{ 0, 0, 0, 0 });
			rect.setOutlineColor(sf::Color{ 0, 255, 0 });
			rect.setOutlineThickness(2.f);
			_game->window().draw(rect);
		}

		// draw timer
		static sf::Text text("01:00", Assets::getInstance().getFont("Arcade"), 50);
		text.setString(std::to_string((int)_config.countdownTime));
		centerOrigin(text);
		text.setFillColor(sf::Color::Black);
		text.setPosition(90.f, 90.f);
		_game->window().draw(text);


		sf::Text currentScore("Current Score: " + std::to_string(_config.currentScore), Assets::getInstance().getFont("score"), 30);
		//centerOrigin(currentScore);
		currentScore.setPosition(20.f, 180.f);
		currentScore.setFillColor(sf::Color::Black);

		sf::Text bestScore("Highest Score: " + std::to_string(_config.highestScore), Assets::getInstance().getFont("score"), 30);
		//centerOrigin(text);
		bestScore.setPosition(20.f, 250.f);
		bestScore.setFillColor(sf::Color::Black);

		_game->window().draw(currentScore);
		_game->window().draw(bestScore);

	}

	// Draw Front basket
	if (_playerFront && _playerFront->getComponent<CAnimation>().has) {
		auto& anim = _playerFront->getComponent<CAnimation>().animation;
		auto& tfm = _playerFront->getComponent<CTransform>();
		anim.getSprite().setPosition(tfm.pos);
		_game->window().draw(anim.getSprite());
	}

	// Draw PowerUps (magnet, slowdown attached in front of basket)
	if (_config.magnetEntity || _config.slowdownEntity) {
		for (auto e : _entityManager.getEntities()) {
			if (e == _config.magnetEntity || e == _config.slowdownEntity) {
				if (e->getComponent<CAnimation>().has) {
					auto& anim = e->getComponent<CAnimation>().animation;
					auto& tfm = e->getComponent<CTransform>();
					anim.getSprite().setPosition(tfm.pos);
					_game->window().draw(anim.getSprite());
				}
			}
		}
	}

	// For popup scores
	for (auto& e : _entityManager.getEntities("scorePopup")) {
		auto& popup = e->getComponent<CScorePopup>();
		auto& tfm = e->getComponent<CTransform>();

		float alpha = popup.lifetime / popup.maxLifetime * 255.f;
		sf::Text text(popup.text, Assets::getInstance().getFont("score"), 50);
		text.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(alpha)));
		text.setPosition(tfm.pos.x, tfm.pos.y - 100.f);

		_game->window().draw(text);

	}


	if (_showGameOverScreen) {
		sf::RectangleShape overlay(sf::Vector2f(_worldBounds.width, _worldBounds.height));
		overlay.setFillColor(sf::Color(255, 240, 200, _overlayAlpha));
		_game->window().draw(overlay);

		sf::Sprite endScreen;
		endScreen.setTexture(Assets::getInstance().getTexture("WinBackground"));
		endScreen.setColor(sf::Color(255, 255, 255, _victoryAlpha));
		_game->window().draw(endScreen);

		if (_victoryAlpha >= 255) {
			auto font = Assets::getInstance().getFont("score");
			auto textColor = sf::Color(22, 22, 22);

			sf::Text currentScore(std::to_string(_config.currentScore), font, 34);
			currentScore.setPosition(306, 236);
			currentScore.setFillColor(textColor);
			_game->window().draw(currentScore);


			sf::Text bestScore(std::to_string(_config.highestScore), font, 34);
			bestScore.setPosition(350, 310);
			bestScore.setFillColor(textColor);
			_game->window().draw(bestScore);


			std::vector<std::string> fruitNames = { "apple", "banana", "watermelon", "strawbury", "mango", "cherry" };

			int startX = 206;  // First row
			int startY = 500;  // 
			int spacingX = 180;
			int spacingY = 124;  // spacing between the 2 rows

			for (size_t i = 0; i < fruitNames.size(); ++i) {
				int col = i % 3;
				int row = i / 3;

				sf::Text fruitText(std::to_string(_config.fruitCollected[fruitNames[i]]), font, 34);
				fruitText.setPosition(startX + col * spacingX, startY + row * spacingY);
				fruitText.setFillColor(textColor);
				_game->window().draw(fruitText);
			}
		}
	}
}
