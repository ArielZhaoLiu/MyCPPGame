﻿//
// Created by David Burchill on 2023-09-27.
//

#ifndef BREAKOUT_COMPONENTS_H
#define BREAKOUT_COMPONENTS_H


#include <memory>
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "Animation.h"
#include <bitset>
#include <functional>


struct Component
{
    bool		has{ false };
    Component() = default;
};


struct CAnimation : public Component {
    Animation   animation;
    bool hasExploded = false;

    CAnimation() = default;
    CAnimation(const Animation& a) : animation(a) {}

};

struct CSprite : public Component {
    sf::Sprite sprite;

    CSprite() = default;

    CSprite(const sf::Texture &t)
            : sprite(t) {
        centerOrigin(sprite);
    }

    CSprite(const sf::Texture &t, sf::IntRect r)
            : sprite(t, r) {
        centerOrigin(sprite);
    }
};


struct CTransform : public Component
{

    sf::Transformable  tfm;
    sf::Vector2f	pos			{ 0.f, 0.f };
    sf::Vector2f	prevPos		{ 0.f, 0.f };
    sf::Vector2f	vel			{ 0.f, 0.f };
    sf::Vector2f	scale		{ 1.f, 1.f };

    float           angVel{ 0 };
    float	        angle{ 0.f };

    CTransform() = default;
    CTransform(const sf::Vector2f& p) : pos(p)  {}
    CTransform(const sf::Vector2f& p, const sf::Vector2f& v)
            : pos(p), prevPos(p),  vel(v){}

};



struct CBoundingBox : public Component
{
    sf::Vector2f size{0.f, 0.f};
    sf::Vector2f halfSize{ 0.f, 0.f };

    CBoundingBox() = default;
    CBoundingBox(const sf::Vector2f& s) : size(s), halfSize(0.5f * s)
    {}
};

struct CState : public Component {
    std::string state{"none"};
	float relatePosX{ 0.f };

    CState() = default;
    CState(const std::string& s) : state(s){}

};


struct CInput : public Component
{
    //enum dirs {
    //    UP      = 1 << 0,
    //    DOWN    = 1 << 1,
    //    LEFT    = 1 << 2,
    //    RIGHT   = 1 << 3
    //};

    //unsigned char dir{0};

    bool up{ false };
    bool left{ false };
    bool right{ false };
    bool down{ false };

    CInput() = default;
};

struct CAutoPilot : public Component
{
    size_t currentLeg{0};
    sf::Time countdown{sf::Time::Zero};

    CAutoPilot() = default;

};

struct CSlowDownEffect : public Component
{
	float duration;
    float speedFactor;

	CSlowDownEffect() = default;
	CSlowDownEffect(float time, float factor) : duration(time), speedFactor(factor) {}

};

struct CMagnetEffect : public Component
{
	float duration;
	CMagnetEffect() = default;
	CMagnetEffect(float time) : duration(time){}
};

struct CScorePopup : public Component {
	std::string text;
	sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;

	bool isCentered{ false };
	float scale{ 0.5f };
	bool hasIcon{ false };

	CScorePopup() = default;

	CScorePopup(const std::string& t, sf::Vector2f v, float lt, float mlt)
		: text(t), velocity(v), lifetime(lt), maxLifetime(mlt) {
	}
    CScorePopup(const std::string& t, float lt, float mlt)
        : text(t), lifetime(lt), maxLifetime(mlt) {
    }
};

struct CStunned : public Component {
    float duration = 0.f;

    CStunned() = default;
    CStunned(float d) : duration(d) {}
};

struct CTextPopup : public Component {

    sf::RectangleShape shape;
    sf::Text sfText;
    std::string text;
    float lifetime;
    float maxLifetime;

	CTextPopup() = default;
	CTextPopup(const std::string& t, float lt, float mlt)
		: text(t), lifetime(lt), maxLifetime(mlt) {
	}
};



#endif //BREAKOUT_COMPONENTS_H
