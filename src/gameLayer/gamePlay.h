#pragma once
#include <gameMap.h>
#include <raylib.h>
#include <entityHolder.h>
#include <drawBackground.h>
#include <player.h>
#include <unordered_map>
#include <structure.h>
#include <assetManager.h>

struct GamePlay
{
	GameMap gameMap;
	Camera2D camera;
	DrawBackground background;

	int creativeSelectedBlock = Block::dirt;

	Vector2 selectionStart = {};	// top-left
	Vector2 selectionEnd = {};		// bottom-right
	Structure copyStructure;

	char saveName[100] = {};

	Player player;
	EntityHolder entities;

	bool insideInventory = false;

	bool showImgui = false;

	// Functions

	void spawnSlime(Vector2 position);

	void spawnDroppedItem(Vector2 position, int type);

	Rectangle getInventoryRectangle(float w, float h);

	bool init();

	bool update(AssetManager& assetManager);
};