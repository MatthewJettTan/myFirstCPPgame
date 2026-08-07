#pragma once

#include "physics.h"
#include <raylib.h>
#include <random>
#include "entity.h"

struct AssetManager;

struct DroppedItem: public Entity
{
	DroppedItem()
	{
		physics.transform.w = 0.8f;
		physics.transform.h = 0.8f;
	}

	int itemType = 0;	// refer to "blocks.h"
	int itemCounter = 1;

	void render(AssetManager& assetManager) override;

	bool update(float deltaTime, EntityUpdateData entityUpdateData) override;

	int getEntityType() override { return EntityType_DroppedItem; }

	float getMaxLife() { return 1.f; }
};