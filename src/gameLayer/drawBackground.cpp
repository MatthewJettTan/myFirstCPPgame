#include "drawBackground.h"
#include "assetManager.h"
#include <raymath.h>
#include <algorithm>

void DrawBackground::draw(float deltaTime, AssetManager& assetManager, Camera2D camera, Vector2 mapSize)
{

	auto drawOneBackground = [&](int type, float parallax, float opacity)
		{

			Texture bg = assetManager.forestBG;

			switch (type)
			{
				case forest: bg = assetManager.forestBG; break;
				case desert: bg = assetManager.desertBG; break;
				case snow: bg = assetManager.snowBG; break;
				case cave: bg = assetManager.caveBG; break;
			}


			int screenW = GetScreenWidth();
			int screenH = GetScreenHeight();

			float aspectRatio = (float)bg.width / (float)bg.height;
			float bgScaleScreen = 2.0f;

			float base = std::max(screenW, screenH) * bgScaleScreen;

			// scale backgrouond so that it always covers the screen while keeping aspect ratio
			float bgW = base;
			float bgH = base;
			if (aspectRatio > 1.0f)
			{
				// bgW is wider, scale height accordingly
				bgH = bgW / aspectRatio;
			}
			else
			{
				// bgH is wider, scale width accordingly
				bgW = bgH * aspectRatio;
			}


			// compute camera ranges
			const float halfViewW = (screenW * 0.5f) / camera.zoom;
			const float halfViewH = (screenH * 0.5f) / camera.zoom;

			const float camMinX = halfViewW;
			const float camMaxX = mapSize.x - halfViewW;
			const float camMinY = halfViewH;
			const float camMaxY = mapSize.y = halfViewH;
			// i don't know the logic exactly

			Vector2 camPos = camera.target;
			camPos.x = Clamp(camPos.x, camMinX, camMaxX);
			camPos.y = Clamp(camPos.y, camMinY, camMaxY);

			const float camRangeX = std::max(0.f, (camMaxX - camMinX));
			const float camRangeY = std::max(0.f, (camMaxY - camMinY));
			const float normX = camRangeX > 0.f ? (camPos.x - camMinX) / camRangeX : 0.f;
			const float normY = camRangeY > 0.f ? (camPos.y - camMinY) / camRangeY : 0.f;

			// how far the bg can move inside the screen (in screen space)
			const float maxOffX = bgW - screenW;
			const float maxOffY = bgH - screenH;

			const float offX = -maxOffX * normX * parallax;
			const float offY = -maxOffY * normY * parallax;

			Rectangle src = { 0, 0, (float)bg.width, (float)bg.height };
			Rectangle dest = { offX, offY, bgW, bgH };

			DrawTexturePro(bg, src, dest, { 0.0 }, 0.0f, {255, 255, 255, (unsigned char)(255 * opacity)});



		};

	drawOneBackground(currentBackgroundType, 0.3, 1);

	// draw the transition on top
	transitionTime -= deltaTime;
	if (transitionTime > 0)		// during transition
	{
		float opacity = transitionTime;
		if (opacity > 1) { opacity = 1; }
		drawOneBackground(currentTransitionType, 0.3, opacity);
	}
}



void DrawBackground::setBackground(int background)
{
	if (background != currentBackgroundType)
	{
		if (transitionTime <= 0)
		{
			transitionTime = 1;
			currentTransitionType = currentBackgroundType;
			currentBackgroundType = background;
		}
		else
		{
			// we wait for the previous transition to finish first
		}
	}
}
