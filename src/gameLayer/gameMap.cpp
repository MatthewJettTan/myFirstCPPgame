#include "gameMap.h"
#include <asserts.h>

void GameMap::create(int w, int h)
{
	*this = {};		// reset all the data
	mapData.resize(w * h);		// set the size of map

	this->w = w;
	this->h = h;

	for (auto& e : mapData) { e = {}; }		// clear all block data and set all blocks as unsafe
}

Block& GameMap::getBlockUnsafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() ==
		w * h, "Map data not initialized");

	permaAssertCommentDevelopement(x >= 0 && y >= 0 && x < w && y < h,
		"getBlockUnsafe out of bounds error");

	return mapData[x + y * w];		// formula for getting right block in 2 dimensional vector
}

Block* GameMap::getBlockSafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() ==
		w * h, "Map data not initialized");

	if (x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }

	return &mapData[x + y * w];
}