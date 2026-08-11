#include "assetManager.h"

void AssetManager::loadAll()
{
	dirt = LoadTexture(RESOURCES_PATH "dirt.png");

	textures = LoadTexture(RESOURCES_PATH "textures.png");

	frame = LoadTexture(RESOURCES_PATH "frame.png");

	player = LoadTexture(RESOURCES_PATH "player.png");

	slime = LoadTexture(RESOURCES_PATH "slime.png");

	items = LoadTexture(RESOURCES_PATH "items.png");

	forestBG = LoadTexture(RESOURCES_PATH "forestBG.png");
	desertBG = LoadTexture(RESOURCES_PATH "desertBG.png");
	snowBG = LoadTexture(RESOURCES_PATH "snowBG.png");
	caveBG = LoadTexture(RESOURCES_PATH "caveBG.png");

	playerFeet = LoadTexture(RESOURCES_PATH "body/player_feet.png");
	playerBack = LoadTexture(RESOURCES_PATH "body/player_back.png");
	playerFront = LoadTexture(RESOURCES_PATH "body/player_front.png");
	playerHead = LoadTexture(RESOURCES_PATH "body/player_head.png");

	hearts = LoadTexture(RESOURCES_PATH "hearts.png");

	
}