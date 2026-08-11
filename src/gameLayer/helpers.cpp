#include "helpers.h"

Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY, bool flipX)
{
	float sizeX = cellSizePixelsX;
	if (flipX) { sizeX *= -1; }

	return shrinkUV(Rectangle{ (float)x * cellSizePixelsX, (float)y * cellSizePixelsY,
		(float)sizeX, (float)cellSizePixelsY });
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

Rectangle shrinkUV(Rectangle in)
{
	float shrink = 0.1;
	in.width -= shrink;
	in.height -= shrink;
	in.x += shrink / 2.f;
	in.y += shrink / 2.f;

	return in;
}