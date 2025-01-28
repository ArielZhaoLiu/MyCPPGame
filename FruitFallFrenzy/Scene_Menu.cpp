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
	MusicPlayer::getInstance().play("menuTheme");
	MusicPlayer::getInstance().setVolume(50);

    registerAction(sf::Keyboard::W,			"UP");
    registerAction(sf::Keyboard::Up,		"UP");
    registerAction(sf::Keyboard::S,			"DOWN");
    registerAction(sf::Keyboard::Down,	 	"DOWN");
	registerAction(sf::Keyboard::D,			"PLAY");
	registerAction(sf::Keyboard::Escape,	"QUIT");

	_title = "Fruit Fall Frenzy";
	_menuStrings.push_back("Level 1");
	_menuStrings.push_back("Level 2");
	_menuStrings.push_back("Level 3");

	_levelPaths.push_back("../assets/level1.txt");
	_levelPaths.push_back("../assets/level1.txt");
	_levelPaths.push_back("../assets/level1.txt");

	_menuText.setFont(Assets::getInstance().getFont("main"));

	const size_t CHAR_SIZE{ 64 };
	_menuText.setCharacterSize(CHAR_SIZE);

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

	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);

	static const sf::Color backgroundColor(100, 100, 255);

	sf::Text footer("UP: W    DOWN: S   PLAY:D    QUIT: ESC", 
		Assets::getInstance().getFont("main"), 20);
	footer.setFillColor(normalColor);
	footer.setPosition(100, 1050);

	_game->window().clear(backgroundColor);

	_menuText.setFillColor(normalColor);
	_menuText.setString(_title);
	_menuText.setPosition(100, 80);
	_game->window().draw(_menuText);

	for (size_t i{ 0 }; i < _menuStrings.size(); ++i)
	{
		_menuText.setFillColor((i == _menuIndex ? selectedColor : normalColor));
		_menuText.setPosition(100, 100 + (i+1) * 96);
		_menuText.setString(_menuStrings.at(i));
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
