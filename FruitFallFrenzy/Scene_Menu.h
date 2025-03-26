#pragma once

#include "Scene.h"

class Scene_Menu : public Scene
{
private:
	std::vector<std::string>	_menuStrings;
	sf::Text					_menuText;
	std::vector<std::string>	_levelPaths;
	int							_menuIndex{0};
	std::string					_title;

	sf::Texture _backgroundTexture;
	sf::Sprite _backgroundSprite;

	sf::Texture _frontTreeTexture;
	sf::Sprite _frontTreeSprite;

	sf::Texture _cloudTextures[7];
	sf::Sprite _clouds[7];
	sf::Vector2f _cloudSpeeds[6];
	sf::Vector2f _cloudBasePos[6]; // 存储基础位置用于上下浮动

	float _cloudFloatTime = 0.f;

	void init();
	void onEnd() override;

public:

	Scene_Menu(GameEngine* gameEngine);

	void update(sf::Time dt) override;

	void sRender() override;
	void sDoAction(const Command& action) override;
	

};

