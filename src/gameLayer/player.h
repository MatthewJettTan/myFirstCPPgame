#pragma once

#include "physics.h"
#include <raylib.h>
#include "entity.h"
#include "entityAnimation.h"
#include "items.h"

struct AssetManager;

struct Player : public Entity
{
	Player()
	{
		physics.transform.w = 0.8f;
		physics.transform.h = 1.6f;

		life = getMaxLife();
	}

	EntityAnimation animations;

	Vector2& getPosition()
	{
		return physics.transform.pos;
	}

	void render(AssetManager& assetManager) override;

	bool update(float deltaTime, EntityUpdateData entityUpdateData) override;

	int getEntityType() override { return EntityType_Player; }
	
	float getMaxLife() { return 10.f; }

	int armourHead = Item::partyHat;
	int armourChest = Item::goldChestPlate;
	int armourLegs = Item::iceBoots;
	int heldItem = Item::goldSword;
};