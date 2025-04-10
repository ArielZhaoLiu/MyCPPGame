#include "Scene_Menu.h"
#include "Scene_FruitFall.h"
#include "MusicPlayer.h"
#include <memory>
#include "SoundPlayer.h"

void Scene_Menu::onEnd()
{
	_game->window().close();
}

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}



void Scene_Menu:: init()
{
	MusicPlayer::getInstance().play("menuTheme2");
	MusicPlayer::getInstance().setVolume(20);

    registerAction(sf::Keyboard::W,			"UP");
    registerAction(sf::Keyboard::Up,		"UP");
    registerAction(sf::Keyboard::S,			"DOWN");
    registerAction(sf::Keyboard::Down,	 	"DOWN");
	registerAction(sf::Keyboard::D,			"PLAY");
	registerAction(sf::Keyboard::Escape,	"QUIT");
	registerAction(sf::Keyboard::B,			"BACK");

	_menuStrings.push_back("START GAME");
	_menuStrings.push_back("HINTS");
	_menuStrings.push_back("ACKNOWLEDGEMENTS");
	_menuStrings.push_back("QUIT");

	_levelPaths.push_back("../assets/level1.txt");
	_levelPaths.push_back("");
	_levelPaths.push_back("");
	_levelPaths.push_back("");

	_menuText.setFont(Assets::getInstance().getFont("Frijole"));

	const size_t CHAR_SIZE{ 28 };
	_menuText.setCharacterSize(CHAR_SIZE);

	if (_backgroundTexture.loadFromFile("../assets/Textures/menu_bg.jpg")) {
		_backgroundSprite.setTexture(_backgroundTexture);
	}
	else {
		std::cerr << "Failed to load background image!" << std::endl;
	}

	if (_acknowledgementTexture.loadFromFile("../assets/Textures/acknoledgement.png")) {
		_acknowledgementSprite.setTexture(_acknowledgementTexture);
	}
	else {
		std::cerr << "Failed to load acknowledgement image!" << std::endl;
	}

	if (_frontTreeTexture.loadFromFile("../assets/Textures/menu_tree_front.png")) {
		_frontTreeSprite.setTexture(_frontTreeTexture);
	}
	else {
		std::cerr << "Failed to load background image!" << std::endl;
	}

	// for clouds
	std::string cloudPaths[3] = {
	"../assets/Textures/cloud1.png",
	"../assets/Textures/cloud2.png",
	"../assets/Textures/cloud3.png"
	};

	for (int i = 0; i < 6; ++i) {
		int texIndex = i / 2;  // 2 clouds each line using the same texture.

		if (_cloudTextures[texIndex].loadFromFile(cloudPaths[texIndex])) {
			_clouds[i].setTexture(_cloudTextures[texIndex]);


			// initial spawn position
			float startX = static_cast<float>(900 * i);
			float startY = 150.f + texIndex * 180.f;
			//float startY = 800.f + (i * 600.f);  
			_clouds[i].setPosition(startX, startY);
			_cloudBasePos[i] = _clouds[i].getPosition();
			
			_cloudSpeeds[i] = sf::Vector2f(40.f + texIndex * 20.f, 0.f);
			//_cloudSpeeds[i] = sf::Vector2f(40.f + (i % 3) * 30.f, 0.f); 
		}
		else {
			std::cerr << "Failed to load cloud image: " << cloudPaths[texIndex] << std::endl;
		}
	}

	spawnAngelAndDevil();

}

void Scene_Menu::update(sf::Time dt)
{
	float windowWidth = static_cast<float>(_game->window().getSize().x);

	for (int i = 0; i < 6; ++i) {
		sf::Vector2f pos = _clouds[i].getPosition();
		pos += _cloudSpeeds[i] * dt.asSeconds();

		if (pos.x > windowWidth + _clouds[i].getGlobalBounds().width) {
			pos.x = -_clouds[i].getGlobalBounds().width;
		}

		int rowIndex = i / 2;
		float floatOffset = std::sin(_cloudFloatTime * (0.5f + rowIndex * 0.2f)) * 5.f;
		_clouds[i].setPosition(pos.x, _cloudBasePos[i].y + floatOffset);

	}

	_cloudFloatTime += dt.asSeconds();
	_entityManager.update();

	for (auto e : _entityManager.getEntities()) {
		// update all animations
		if (e->getComponent<CAnimation>().has) {
			auto& anim = e->getComponent<CAnimation>();
			anim.animation.update(dt);
		}
	}
}


void Scene_Menu::sRender()
{
	 
	sf::View view = _game->window().getView();
	view.setCenter(_game->window().getSize().x / 2.f, _game->window().getSize().y / 2.f);
	_game->window().setView(view);

	static const sf::Color selectedColor(205, 133, 63);
	static const sf::Color normalColor(30, 30, 30);

	static const sf::Color backgroundColor(100, 100, 255);

	sf::Text footer("UP: W    DOWN: S   PLAY:D    QUIT: ESC", 
		Assets::getInstance().getFont("main"), 20);
	footer.setFillColor(normalColor);
	footer.setPosition(100, 1050);

	_game->window().draw(_backgroundSprite); // draw bg

	// ☁️ Clouds
	for (int i = 0; i < 5; ++i) {
		_game->window().draw(_clouds[i]);
	}

	_game->window().draw(_frontTreeSprite); // draw front tree

	_menuText.setFillColor(normalColor);
	_menuText.setString(_title);
	_menuText.setPosition(100, 80);
	_game->window().draw(_menuText);

	// Draw menu acknowledgements
	if (_showAcknowledgements) {
		_acknowledgementSprite.setPosition(_game->window().getSize().x / 2.f -516.f, _game->window().getSize().y / 2.f - 500.f);
		sf::RectangleShape overlay(sf::Vector2f(_game->windowSize().x, _game->windowSize().y));
		overlay.setFillColor(sf::Color(0, 0, 0, 150));
		_game->window().draw(overlay);
		_game->window().draw(_acknowledgementSprite);
	}
	else {
		for (size_t i{ 0 }; i < _menuStrings.size(); ++i)
		{
			_menuText.setString(_menuStrings.at(i)); //  getLocalBounds() get correct width
			sf::FloatRect textBounds = _menuText.getLocalBounds();

			_menuText.setOrigin(textBounds.width / 2.f, textBounds.height / 2.f); // 
			_menuText.setPosition(_game->window().getSize().x / 2.f, 590 + (i + 1) * 70); // allign center

			_menuText.setFillColor((i == _menuIndex) ? selectedColor : normalColor);
			_menuText.setScale((i == _menuIndex) ? sf::Vector2f(1.2f, 1.2f) : sf::Vector2f(1.0f, 1.0f));

			_game->window().draw(_menuText);
		}
	}

	

	_game->window().draw(footer);
	//m_game->window().display();


	// for draw angel and devil
	for (auto e : _entityManager.getEntities()) {

		if (e->getComponent<CAnimation>().has) {
			auto& anim = e->getComponent<CAnimation>().animation;
			auto& tfm = e->getComponent<CTransform>();
			anim.getSprite().setPosition(tfm.pos);
			_game->window().draw(anim.getSprite());
		}
	}

}


void Scene_Menu::sDoAction(const Command& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			_menuIndex = (_menuIndex + _menuStrings.size() - 1) % _menuStrings.size();
			SoundPlayer::getInstance().play("plop", 15);

		} 
		else if (action.name() == "DOWN")
		{
			_menuIndex = (_menuIndex + 1) % _menuStrings.size();
			SoundPlayer::getInstance().play("plop", 20);
		}
		else if (action.name() == "PLAY")
		{
			std::string selectedItem = _menuStrings[_menuIndex];
			if (selectedItem == "ACKNOWLEDGEMENTS"){
				_acknowledgementTexture.loadFromFile("assets/images/acknoledgement.png");
				_acknowledgementSprite.setTexture(_acknowledgementTexture);
				_showAcknowledgements = true;
			}
			else
			{
				_game->changeScene("PLAY", std::make_shared<Scene_FruitFall>(_game, _levelPaths[_menuIndex]));
			}
			
		}
		else if (action.name() == "BACK")
		{
			if (_showAcknowledgements)
			{
				_showAcknowledgements = false;
			}
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}

}

void Scene_Menu::spawnAngelAndDevil()
{
	auto angel = _entityManager.addEntity("angel");
	auto& animAngel = angel->addComponent<CAnimation>(Assets::getInstance().getAnimation("angel")).animation;
	auto& tfmAngel = angel->addComponent<CTransform>(sf::Vector2f(1243.f, 837.f));
	


	auto devil = _entityManager.addEntity("devil");
	auto& animDevil = devil->addComponent<CAnimation>(Assets::getInstance().getAnimation("devil")).animation;
	auto& tfmDevil = devil->addComponent<CTransform>(sf::Vector2f(243.f, 846.f));


}
