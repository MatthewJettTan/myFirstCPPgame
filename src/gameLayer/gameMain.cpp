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



struct GameData
{
	GameMap gameMap;

	Camera2D camera;

	int creativeSelectedBlock = Block::dirt;

	Vector2 selectionStart = {};	// top-left
	Vector2 selectionEnd = {};		// bottom-right
	Structure copyStructure;

	char saveName[100] = {};

	PhysicalEntity player;
	EntityHolder entities;

}gameData;


#pragma region global setting
AssetManager assetManager;
bool showImgui = false;
#pragma endregion

void spawnSlime(Vector2 position)
{
	Slime slime;

	slime.physics.teleport(position);

	auto id = gameData.entities.idHolder.getEntityIdAndIncrement();

	gameData.entities.entities[id] = std::make_unique<Slime>(slime);
}



bool initGame()
{

	assetManager.loadAll();

	generateWorld(gameData.gameMap);

	// initialize member variables of camera
	gameData.camera.target = { 0, 0 };  // world-space center of view
	gameData.camera.rotation = 0.f;
	gameData.camera.zoom = 100.f;
	// initialize player
	gameData.player.teleport({ 20, 125 });
	gameData.player.transform.w = 0.9f;
	gameData.player.transform.h = 1.8f;
	// initialize slime
	spawnSlime({ 18, 110 });

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
	if (IsKeyDown(KEY_LEFT)) {gameData.player.transform.pos.x -= CAMERA_SPEED * deltaTime;}
	if (IsKeyDown(KEY_RIGHT)) { gameData.player.transform.pos.x += CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_UP)) { gameData.player.transform.pos.y -= CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_DOWN)) { gameData.player.transform.pos.y += CAMERA_SPEED * deltaTime; }

	if (IsKeyDown(KEY_SPACE)) { gameData.player.jump(10); }

#pragma endregion

#pragma region enetities

	// players
	gameData.player.applyGravity();
	gameData.player.updateForces(deltaTime);
	gameData.player.resolveConstrains(gameData.gameMap);
	gameData.camera.target = gameData.player.transform.pos;
	gameData.player.updateFinal();

	// update all NPC-entities
	std::ranlux24_base rng(std::random_device{}());

	EntityUpdateData updateData
	{
		rng,
		gameData.player.transform.pos
	};

	for (auto& e : gameData.entities.entities)
	{
		e.second->update(deltaTime, updateData);

		e.second->physics.applyGravity();

		e.second->physics.updateForces(deltaTime);
		e.second->physics.resolveConstrains(gameData.gameMap);
		e.second->physics.updateFinal();
	}

#pragma endregion

#pragma region mouse control
	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	// Boundary control
	if (gameData.creativeSelectedBlock < 0) { gameData.creativeSelectedBlock = 0; }
	if (gameData.creativeSelectedBlock >= Block::BLOCKS_COUNT) { gameData.creativeSelectedBlock = Block::BLOCKS_COUNT - 1; }

	if (showImgui == true)
	{
		if (IsKeyPressed(KEY_ONE)) { gameData.selectionStart = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyPressed(KEY_TWO)) { gameData.selectionEnd = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyPressed(KEY_THREE)) { gameData.copyStructure.pasteIntoMap(gameData.gameMap, Vector2{ (float)blockX, (float)blockY }); }
		if (IsKeyPressed(KEY_FOUR)) { spawnSlime(worldPos); }

		// ensure that start-position is smaller than end-position
		if (gameData.selectionStart.x > gameData.selectionEnd.x)
		{
			std::swap(gameData.selectionStart.x, gameData.selectionEnd.x);
		}
		if (gameData.selectionStart.y > gameData.selectionEnd.y)
		{
			std::swap(gameData.selectionStart.y, gameData.selectionEnd.y);
		}
	}

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

	// show the structure selection
	if (showImgui == true)
	{
		Rectangle rect;
		rect.x = gameData.selectionStart.x;
		rect.y = gameData.selectionStart.y;
		rect.width = gameData.selectionEnd.x - gameData.selectionStart.x;
		rect.height = gameData.selectionEnd.y - gameData.selectionStart.y;

		rect.width++;
		rect.height++;

		DrawRectangleLinesEx(rect, 0.1,		// Rectengle, LineThickness
			{ 20, 101, 250, 145 });			// Color

	}


	//// intersection test
	//Transform2D testFixedRec;
	//testFixedRec.pos = { 0.5, 110.5 };
	//testFixedRec.w = 1;
	//testFixedRec.h = 1;

	//Transform2D testMovingRec;
	//testMovingRec.pos = { (float)floor(worldPos.x), (float)floor(worldPos.y)};
	//testMovingRec.w = 1;
	//testMovingRec.h = 1;
	//// Just setting the correct parameters, but not obtaining the rectangle
	//
	//// transform-transform intersection test 
	//if (testFixedRec.intersectTransformFromTopLeft(testMovingRec))	// if mouse intersect with the transform2D "test"
	//{
	//	DrawRectangleLinesEx(testFixedRec.getAABB(), 0.1, GREEN);	// draw rectangle outline
	//	DrawRectangleLinesEx(testMovingRec.getAABBFromTopLeft(), 0.1, GREEN);
	//}
	//else
	//{
	//	DrawRectangleLinesEx(testFixedRec.getAABB(), 0.1, BLUE);
	//	DrawRectangleLinesEx(testMovingRec.getAABBFromTopLeft(), 0.1, RED);
	//}


	// draw the slime
	for (auto& e : gameData.entities.entities)
	{
		e.second->render(assetManager);
	}

	// draw the player
	DrawTexturePro(
		assetManager.player,
		{0, 0, (float)assetManager.player.width, (float)assetManager.player.height},
		getRectangleForEntity(gameData.player.transform, 1, 2), //dest
		{0, 0},// origin (top-left corner)
		0.0f, // rotation
		WHITE // tint
	);

	DrawRectangleLinesEx(gameData.player.transform.getAABB(), 0.1,
		{20, 101, 250, 120});

	EndMode2D();
#pragma endregion

#pragma region ImGui
	if (showImgui == true)
	{
		ImGui::Begin("Game control");

		ImGui::Text("VELOCITY Y: %f", gameData.player.velocity);

		ImGui::SliderFloat("Camera zoom:", &gameData.camera.zoom, 10, 150);
		ImGui::SliderFloat("Camera speed:", &CAMERA_SPEED, 5, 50);

		if (ImGui::Button("Copy"))
		{
			gameData.copyStructure.copyFromMap(gameData.gameMap,
				gameData.selectionStart, gameData.selectionEnd);
		}

		ImGui::InputText("File name", gameData.saveName, sizeof(gameData.saveName));

		if (ImGui::Button("Save to file"))
		{
			// obtain the file address
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			saveBlockDataToFile(gameData.copyStructure.structureData, gameData.copyStructure.w,
				gameData.copyStructure.h, path.c_str());
		}
		if (ImGui::Button("Load from file"))
		{
			// obtain the file address
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			loadBlockDataFromFile(gameData.copyStructure.structureData, gameData.copyStructure.w,
				gameData.copyStructure.h, path.c_str());
		}


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
