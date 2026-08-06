#include <raylib.h>
#include <imgui.h>
#include <asserts.h>
#include <raymath.h>
#include "gameMain.h"
#include "assetManager.h"
#include "gameMap.h"
#include "helpers.h"
#include "worldGenerator.h"



struct GameData
{
	GameMap gameMap;
	Camera2D camera;

	int creativeSelectedBlock = Block::dirt;

}gameData;

AssetManager assetManager;

bool showImgui = false;

bool initGame()
{

	assetManager.loadAll();

	generateWorld(gameData.gameMap);

	// initialize member variables of camera
	gameData.camera.target = { 0, 110 };  // world-space center of view
	gameData.camera.rotation = 0.f;
	gameData.camera.zoom = 50.f;


	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

	gameData.camera.offset = { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };

	ClearBackground({ 75, 75, 150, 255 });

	if (IsKeyPressed(KEY_P)) { showImgui = !showImgui; }

#pragma region camera movement

	static float CAMERA_SPEED = 10;
	if (IsKeyDown(KEY_LEFT)) {gameData.camera.target.x -= CAMERA_SPEED * deltaTime;}
	if (IsKeyDown(KEY_RIGHT)) { gameData.camera.target.x += CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_UP)) { gameData.camera.target.y -= CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_DOWN)) { gameData.camera.target.y += CAMERA_SPEED * deltaTime; }

#pragma endregion

#pragma region mouse control
	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	// Boundary control
	if (gameData.creativeSelectedBlock < 0) { gameData.creativeSelectedBlock = 0; }
	if (gameData.creativeSelectedBlock >= Block::BLOCKS_COUNT) { gameData.creativeSelectedBlock = Block::BLOCKS_COUNT - 1; }

	if (showImgui == false)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			auto b = gameData.gameMap.getBlockSafe(blockX, blockY);

			if (b)
			{
				*b = {};
			}
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
			auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
			if (b)
			{
				b->type = gameData.creativeSelectedBlock;
			}
		}
	}
#pragma endregion

#pragma region draw world
	BeginMode2D(gameData.camera);

	Vector2 topLeftView = GetScreenToWorld2D({ 0, 0 }, gameData.camera);
	Vector2 bottomRightView = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, gameData.camera);

	int startXView = (int)floorf(topLeftView.x - 1);
	int endXView = (int)ceilf(bottomRightView.x - 1);
	int startYView = (int)floorf(topLeftView.y - 1);
	int endYView = (int)ceilf(bottomRightView.y - 1);

	startXView = Clamp(startXView, 0, gameData.gameMap.w - 1);
	endXView = Clamp(endXView, 0, gameData.gameMap.w - 1);

	startYView = Clamp(startYView, 0, gameData.gameMap.h - 1);
	endYView = Clamp(endYView, 0, gameData.gameMap.h - 1);

	for (int y = startYView; y < endYView; y++)
	{
		for (int x = startXView; x < endXView; x++)
		{
			auto& b = gameData.gameMap.getBlockUnsafe(x, y);

			if (b.type != Block::air)
			{
				DrawTexturePro(
					assetManager.textures,
					getTextureAtlas(b.type, 0, 32, 32),	// source
					{(float)x, (float)y, 1, 1},	// dest
					{ 0, 0 },		// origin (top-left corner)
					0.0f,		// rotation
					WHITE		// tint
				);
			}
		}
	}

	// draw selected block
	DrawTexturePro(
		assetManager.frame,
		{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height },
		{ (float)blockX, (float)blockY, 1, 1 },
		{ 0, 0 },
		0.f,
		WHITE
	);

	EndMode2D();
#pragma endregion

#pragma region ImGui
	if (showImgui == true)
	{
		ImGui::Begin("Game control");

		ImGui::SliderFloat("Camera zoom:", &gameData.camera.zoom, 10, 150);
		ImGui::SliderFloat("Camera speed:", &CAMERA_SPEED, 5, 50);

		ImGui::Separator();

		for (int i = 0; i < Block::BLOCKS_COUNT; i++)
		{

			auto atlas = getTextureAtlas(i, 0, 32, 32);
			atlas.x /= assetManager.textures.width;			// Convert tile X pixel coordinate to normalized 0‑1 UV‑U coordinate
			atlas.width /= assetManager.textures.width;		// Convert tile pixel width into normalized UV width
			atlas.y /= assetManager.textures.height;
			atlas.height /= assetManager.textures.height;

			ImGui::PushID(i);

			ImTextureID tex = (ImTextureID)(intptr_t)assetManager.textures.id;	// specify the use of textures
			if (ImGui::ImageButton(tex,
				{ 35, 35 }, { atlas.x, atlas.y },
				{ atlas.x + atlas.width, atlas.y + atlas.height }))
			{
				gameData.creativeSelectedBlock = i;
			}

			ImGui::PopID();

			if (i % 10 != 0)	// 10 patterns in each row
			{
				ImGui::SameLine();
			}
		}

		ImGui::End();
	}
#pragma endregion

	DrawFPS(10, 10);

	return true;
}


void closeGame()
{
}
