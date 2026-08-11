#pragma once
#include <raylib.h>
#include <unordered_map>

struct AssetManager
{
	Texture2D dirt = {};
	Texture2D textures = {};
	Texture2D frame = {};
	Texture2D player = {};
	Texture2D slime = {};
	Texture2D items = {};

	Texture2D forestBG = {};
	Texture2D desertBG = {};
	Texture2D snowBG = {};
	Texture2D caveBG = {};

	Texture2D playerFeet = {};
	Texture2D playerBack = {};
	Texture2D playerFront = {};
	Texture2D playerHead = {};

	Texture2D hearts = {};

	void loadAll();

};