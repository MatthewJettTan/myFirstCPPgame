#include "gamePlay.h"
#include "entities/droppedItem.h"
#include "ui.h"
#include "audio.h"
#include "settings.h"
#include <worldGenerator.h>
#include <helpers.h>
#include <imgui.h>
#include <saveMap.h>

void GamePlay::spawnSlime(Vector2 position)
{
	Slime slime;

	slime.physics.teleport(position);

	auto id = entities.idHolder.getEntityIdAndIncrement();

	entities.entities[id] = std::make_unique<Slime>(slime);
}

void GamePlay::spawnDroppedItem(Vector2 position, int type)
{
	DroppedItem droppedItem;

	droppedItem.teleport(position);
	droppedItem.itemType = type;

	auto id = entities.idHolder.getEntityIdAndIncrement();

	entities.entities[id] = std::make_unique<DroppedItem>(droppedItem);
}

Rectangle GamePlay::getInventoryRectangle(float w, float h)
{
	Rectangle inventoryRectangle;

	float size = std::min(w, h) * 0.9f;

	inventoryRectangle.height = h * 0.30f;
	inventoryRectangle.width = inventoryRectangle.height * 3;

	// don't let the inventory become bigger than the screen width
	float maxWidth = w * 0.9;
	if (inventoryRectangle.width > maxWidth)
	{
		float scaleFactor = maxWidth / inventoryRectangle.width;
		inventoryRectangle.height *= scaleFactor;
		inventoryRectangle.width *= scaleFactor;
	}


	inventoryRectangle = placeRectangleTopLeftCorner(inventoryRectangle, w);

	inventoryRectangle.x += w * 0.01f;
	inventoryRectangle.y += h * 0.01f;

	return inventoryRectangle;
}

bool GamePlay::init()
{
	
	generateWorld(gameMap);

	// initialize member variables of camera
	camera.target = { 0, 0 };  // world-space center of view
	camera.rotation = 0.f;
	camera.zoom = 100.f;
	// initialize player
	player.physics.teleport({ 20, 125 });
	player.physics.transform.w = 0.9f;
	player.physics.transform.h = 1.8f;
	// initialize slime
	spawnSlime({ 18, 110 });


	return true;
}

bool GamePlay::update(AssetManager& assetManager)
{
	Audio::update();

	float deltaTime = GetFrameTime();
	if (deltaTime > 1.f / 5) { deltaTime = 1 / 5.f; }

	camera.offset = { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };

	ClearBackground({ 75, 75, 150, 255 });

	if (IsKeyPressed(KEY_P)) { showImgui = !showImgui; }

#pragma region camera movement

	static float CAMERA_SPEED = 10;
	static bool creativeMode = false;
	{
		bool moving = 0;
		bool falling = 0;


		if (IsKeyDown(KEY_A))
		{
			player.physics.transform.pos.x -= CAMERA_SPEED * deltaTime;
			moving = true;
			player.animations.movingLeft = true;
		}
		if (IsKeyDown(KEY_D))
		{
			player.physics.transform.pos.x += CAMERA_SPEED * deltaTime;
			moving = true;
			player.animations.movingLeft = false;
		}
		if (creativeMode == true)
		{
			if (IsKeyDown(KEY_W))
			{
				player.physics.transform.pos.y -= CAMERA_SPEED * deltaTime;
				moving = true;
			}
			if (IsKeyDown(KEY_S))
			{
				player.physics.transform.pos.y += CAMERA_SPEED * deltaTime;
				moving = true;
			}
		}
		if (IsKeyDown(KEY_SPACE))
		{
			player.physics.jump(10.0);
		}

		if (player.physics.downTouch)
		{
			falling = 0;
		}
		else
		{
			falling = 1;
		}

		if (falling)
		{
			player.animations.setAnimation(2);
		}
		else if (moving)
		{
			player.animations.setAnimation(1);
		}
		else {
			player.animations.setAnimation(0);
		}
		player.animations.update(deltaTime, 0.08, 7);
	}
#pragma endregion

#pragma region enetities

	auto updateEntityPhysics = [&](auto& entity, bool applyGravity = true)
		{
			if (applyGravity) { entity.physics.applyGravity(); }

			entity.physics.updateForces(deltaTime);
			entity.physics.resolveConstrains(gameMap);
			entity.physics.updateFinal();
		};

	// players
	updateEntityPhysics(player, !creativeMode);

	camera.target = player.physics.transform.pos;

	// update all NPC-entities
	std::ranlux24_base rng(std::random_device{}());

	for (auto it = entities.entities.begin(); it != entities.entities.end();)
	{
		EntityUpdateData updateData
		{
			rng,
			player.physics.transform.pos,
			entities,
			it->first
		};

		bool shouldKill = false;

		if (!it->second->update(deltaTime, updateData)
			|| it->second->life <= 0)
		{
			shouldKill = true;
		}
		if (shouldKill)
		{
			// erase returns the next valid iterator
			it = entities.entities.erase(it);
		}
		else
		{
			//physics
			updateEntityPhysics(*(it->second));

			it++;
		}
	}

#pragma endregion

#pragma region mouse control

	// check if mouse is inside the inventoryMenu
	bool insideInventoryMenu = false;
	Rectangle inventoryRectangle = getInventoryRectangle(GetScreenWidth(), GetScreenHeight());
	if (insideInventory &&
		CheckCollisionPointRec(GetMousePosition(), inventoryRectangle))
	{
		insideInventoryMenu = true;
	}


	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	// Boundary control
	if (creativeSelectedBlock < 0) { creativeSelectedBlock = 0; }
	if (creativeSelectedBlock >= Block::BLOCKS_COUNT) { creativeSelectedBlock = Block::BLOCKS_COUNT - 1; }

	if (showImgui == true)
	{
		if (IsKeyPressed(KEY_ONE)) { selectionStart = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyPressed(KEY_TWO)) { selectionEnd = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyPressed(KEY_THREE)) { copyStructure.pasteIntoMap(gameMap, Vector2{ (float)blockX, (float)blockY }); }
		if (IsKeyPressed(KEY_FOUR)) { spawnSlime(worldPos); }

		// ensure that start-position is smaller than end-position
		if (selectionStart.x > selectionEnd.x)
		{
			std::swap(selectionStart.x, selectionEnd.x);
		}
		if (selectionStart.y > selectionEnd.y)
		{
			std::swap(selectionStart.y, selectionEnd.y);
		}
	}

	if (showImgui == false)
	{
		if (!insideInventoryMenu)
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{
				auto b = gameMap.getBlockSafe(blockX, blockY);

				if (b)
				{
					if (b->type)
					{
						spawnDroppedItem({ (float)blockX + 0.5f, (float)blockY + 0.5f }, b->type);
					}

					*b = {};
				}
			}

		if (!insideInventoryMenu)
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
			{
				auto b = gameMap.getBlockSafe(blockX, blockY);
				if (b)
				{
					b->type = creativeSelectedBlock;
				}
			}

		if (IsKeyPressed(KEY_TAB))
		{
			insideInventory = !insideInventory;
		}
	}
#pragma endregion

#pragma region draw world

	// draw background
	{
		int backgroundType = DrawBackground::forest;

		if (player.getPosition().x > gameMap.desertStart
			&& player.getPosition().x < gameMap.desertEnd
			)
		{
			backgroundType = DrawBackground::desert;
		}
		if (player.getPosition().y > 130)
		{
			backgroundType = DrawBackground::cave;
		}

		background.setBackground(backgroundType);

		background.draw(deltaTime, assetManager, camera,
			{ (float)gameMap.w, (float)gameMap.h });
	}


	BeginMode2D(camera);

	Vector2 topLeftView = GetScreenToWorld2D({ 0, 0 }, camera);
	Vector2 bottomRightView = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, camera);

	int startXView = (int)floorf(topLeftView.x - 1);
	int endXView = (int)ceilf(bottomRightView.x + 1);
	int startYView = (int)floorf(topLeftView.y - 1);
	int endYView = (int)ceilf(bottomRightView.y + 1);

	startXView = Clamp(startXView, 0, gameMap.w - 1);
	endXView = Clamp(endXView, 0, gameMap.w - 1);

	startYView = Clamp(startYView, 0, gameMap.h - 1);
	endYView = Clamp(endYView, 0, gameMap.h - 1);

	for (int y = startYView; y < endYView; y++)
	{
		for (int x = startXView; x < endXView; x++)
		{
			auto& b = gameMap.getBlockUnsafe(x, y);

			if (b.type != Block::air)
			{
				DrawTexturePro(
					assetManager.textures,
					getTextureAtlas(b.type, 0, 32, 32),	// source
					{ (float)x, (float)y, 1, 1 },	// dest
					{ 0, 0 },		// origin (top-left corner)
					0.0f,		// rotation
					WHITE		// tint
				);
			}
		}
	}

	// draw selected block
	if (!insideInventoryMenu)
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
		rect.x = selectionStart.x;
		rect.y = selectionStart.y;
		rect.width = selectionEnd.x - selectionStart.x;
		rect.height = selectionEnd.y - selectionStart.y;

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
	for (auto& e : entities.entities)
	{
		e.second->render(assetManager);
	}

	// draw the player
	player.render(assetManager);

	DrawRectangleLinesEx(player.physics.transform.getAABB(), 0.1,
		{ 20, 101, 250, 120 });

	EndMode2D();
#pragma endregion

#pragma region ui

	{
		float w = GetScreenWidth();
		float h = GetScreenHeight();

		Rectangle heartRectangle;

		heartRectangle.height = h * 0.05f;
		heartRectangle.width = heartRectangle.height * 5;

		heartRectangle = placeRectangleTopRightCorner(heartRectangle, w);

		// test the drawing area
		//DrawRectangle(heartRectangle.x, heartRectangle.y, heartRectangle.width, heartRectangle.height, 
		//	{ 230, 41, 55, 155 });

		// draw the heart
		for (int i = 0; i < 5; i++)
		{
			Rectangle oneHeartRectangle = heartRectangle;
			oneHeartRectangle.width = oneHeartRectangle.height;
			oneHeartRectangle.x += oneHeartRectangle.width * i;

			DrawTexturePro(
				assetManager.hearts,
				getTextureAtlas(0, 0, assetManager.hearts.width / 3, assetManager.hearts.height),
				oneHeartRectangle,
				{ 0, 0 },
				0.f,
				WHITE
			);
		}

		// draw bagUI
		if (insideInventory)
		{

			// draw inventoryRectangle
			Rectangle inventoryRectangle = getInventoryRectangle(w, h);
			DrawRectangle(inventoryRectangle.x, inventoryRectangle.y, inventoryRectangle.width, inventoryRectangle.height,
				{ 100, 100, 100, 100 });
			inventoryRectangle = shrinkRectanglePercentage(inventoryRectangle, 0.01, 0.01);

			// draw the blocks in the bagUI
			Rectangle oneCellRectangle;
			oneCellRectangle.height = inventoryRectangle.height / 3;
			oneCellRectangle.width = oneCellRectangle.height;
			oneCellRectangle.x = inventoryRectangle.x;
			oneCellRectangle.y = inventoryRectangle.y;

			for (int i = 0; i < 9; i++)
				for (int j = 0; j < 3; j++)
				{

					Rectangle r = oneCellRectangle;
					r.x += i * oneCellRectangle.width;
					r.y += j * oneCellRectangle.height;

					r = shrinkRectanglePercentage(r, 0.1, 0.1);

					if (CheckCollisionPointRec(GetMousePosition(), r))
					{
						DrawTexturePro(
							assetManager.frame,
							{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height, },
							r,
							{ 0, 0 },
							0.f,
							{ 220, 250, 220, 250 }
						);
					}
					else
					{
						DrawTexturePro(
							assetManager.frame,
							{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height, },
							r,
							{ 0, 0 },
							0.f,
							{ 180, 180, 200, 240 }
						);
					}
				}
		}
	}

#pragma endregion

#pragma region ImGui
	if (showImgui == true)
	{
		ImGui::Begin("Game control");

		ImGui::Text("VELOCITY Y: %f", player.physics.velocity);

		ImGui::SliderFloat("Camera zoom:", &camera.zoom, 10, 150);
		ImGui::SliderFloat("Camera speed:", &CAMERA_SPEED, 5, 50);
		ImGui::Checkbox("CreativeMode:", &creativeMode);

		if (ImGui::Button("Hurt a slime"))
		{
			for (auto& e : entities.entities)
			{
				if (e.second->getEntityType() == EntityType_Slime)
				{
					e.second->life -= 3;
					break;
				}
			}
		}

		if (ImGui::Button("Copy"))
		{
			copyStructure.copyFromMap(gameMap,
				selectionStart, selectionEnd);
		}

		ImGui::InputText("File name", saveName, sizeof(saveName));

		if (ImGui::Button("Save to file"))
		{
			// obtain the file address
			std::string path = RESOURCES_PATH "structures/";
			path += saveName;
			path += ".bin";

			saveBlockDataToFile(copyStructure.structureData,copyStructure.w,
				copyStructure.h, path.c_str());
		}
		if (ImGui::Button("Load from file"))
		{
			// obtain the file address
			std::string path = RESOURCES_PATH "structures/";
			path += saveName;
			path += ".bin";

			loadBlockDataFromFile(copyStructure.structureData, copyStructure.w,
				copyStructure.h, path.c_str());
		}

		ImGui::Separator();

		ImGui::SliderFloat("master volume", &(getSettings().masterVolume), 0, 1);
		ImGui::SliderFloat("sound volume", &(getSettings().soundsVolume), 0, 1);

		if (ImGui::Button("Play Sound"))
		{
			Audio::playSound(Audio::placeBlock);
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
				creativeSelectedBlock = i;
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
