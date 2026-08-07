#include "helpers.h"

Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY)
{
	return Rectangle{ (float)x * cellSizePixelsX, (float)y * cellSizePixelsY,
		(float)cellSizePixelsX, (float)cellSizePixelsY };
}

Rectangle getRectangleForEntity(Transform2D transform, float textureW, float textureH)
{
	Transform2D result = transform;
	result.w = textureW;
	result.h = textureH;

	// move the sprite so that the bottom of the sprite matches the bottom of the collider
	result.pos.y += (transform.h - result.h) / 2;

	return result.getAABB();
}