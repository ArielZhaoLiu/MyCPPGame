#include "Scene_Menu.h"
#include "Scene_FruitFall.h"
#include "MusicPlayer.h"
#include <memory>

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

	_menuStrings.push_back("START GAME");
	_menuStrings.push_back("HINTS");
	_menuStrings.push_back("SETTINGS");
	_menuStrings.push_back("QUIT");

	_levelPaths.push_back("../assets/level1.txt");
	_levelPaths.push_back("../assets/level1.txt");
	_levelPaths.push_back("../assets/level1.txt");

	_menuText.setFont(Assets::getInstance().getFont("Frijole"));

	const size_t CHAR_SIZE{ 28 };
	_menuText.setCharacterSize(CHAR_SIZE);

	if (_backgroundTexture.loadFromFile("../assets/Textures/menu_bg.jpg")) {
		_backgroundSprite.setTexture(_backgroundTexture);
	}
	else {
		std::cerr << "Failed to load background image!" << std::endl;
	}

}

void Scene_Menu::update(sf::Time dt)
{
	_entityManager.update();
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

	_game->window().clear(backgroundColor);

	_game->window().draw(_backgroundSprite);

	_menuText.setFillColor(normalColor);
	_menuText.setString(_title);
	_menuText.setPosition(100, 80);
	_game->window().draw(_menuText);

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

	_game->window().draw(footer);
	//m_game->window().display();

}


void Scene_Menu::sDoAction(const Command& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			_menuIndex = (_menuIndex + _menuStrings.size() - 1) % _menuStrings.size();
		} 
		else if (action.name() == "DOWN")
		{
			_menuIndex = (_menuIndex + 1) % _menuStrings.size();
		}
		else if (action.name() == "PLAY")
		{
			_game->changeScene("PLAY", std::make_shared<Scene_FruitFall>(_game, _levelPaths[_menuIndex]));
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}

}
