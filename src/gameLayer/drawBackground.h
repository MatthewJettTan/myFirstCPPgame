#pragma once
#include "assetManager.h"

struct AssetManager;

struct DrawBackground
{

	enum BackGround
	{
		none = 0,
		forest,
		desert,
		snow,
		cave,
	};

	int currentBackgroundType = forest;		// new bg
	int currentTransitionType = none;		// old bg

	float transitionTime = 0;


	void draw(float deltaTime, AssetManager& assetManager, Camera2D camera, Vector2 mapSize);

	void setBackground(int background);
};