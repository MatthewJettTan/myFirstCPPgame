#include <raylib.h>
#include <imgui.h>
#include <asserts.h>
#include <raymath.h>
#include "gameMain.h"
#include "structure.h"
#include "assetManager.h"
#include "gameMap.h"
#include "helpers.h"
#include "worldGenerator.h"
#include <string>
#include "saveMap.h"
#include "physics.h"
#include "entities/slime.h"
#include "entityHolder.h"
#include "entity.h"
#include "entities/droppedItem.h"
#include "player.h"
#include "items.h"
#include "audio.h"
#include "settings.h"
#include "drawBackground.h"
#include "ui.h"
#include "gamePlay.h"

AssetManager assetManager;
GamePlay gameplay;
UIEngine mainMenuButtons;
DrawBackground backgroundForMainMenu;
bool gameplayRunning = false;


bool initGame()
{
	Audio::init();
	assetManager.loadAll();

	gameplay.init();

	return true;
}



bool updateGame()
{
	Audio::update();
	//return gameplay.update(assetManager);

	ClearBackground({ 0, 0, 0, 255 });

	// draw moving background
	static const float camera_speed = 20;
	float deltaTime = GetFrameTime();

	static float x = 200;
	static float y = 500;

	x += deltaTime * camera_speed;
	y += deltaTime * camera_speed;
	
	
	// draw buttons
	if (!gameplayRunning)
	{
		Camera2D c = {};
		c.offset = { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };
		c.target = Vector2{ x, y };
		c.zoom = 10;
		backgroundForMainMenu.draw(GetFrameTime(), assetManager, c, { 1000, 1000 });

		mainMenuButtons.addTitle("Cancas Adventures");

		if (mainMenuButtons.addButton("Start game"))
		{
			gameplayRunning = true;
			gameplay = {};		// make sure we reset the gameplay
			gameplay.init();
		}

		mainMenuButtons.addButton("Settings");

		if (mainMenuButtons.addButton("Exit"))
		{
			return false;
		}

		mainMenuButtons.updateAndRender();

		return true;
	}
	else
	{
		return gameplay.update(assetManager);
	}

	return true;
}


void closeGame()
{
}
