#pragma once
#include <raylib.h>

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

	void loadAll();
};