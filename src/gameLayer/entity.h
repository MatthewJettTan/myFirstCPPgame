#pragma once
#include <unordered_map>
#include "physics.h"

struct AssetManager;

enum EntityType
{
	EntityType_Player = 0,
	EntityType_Slime,


};



struct EntityUpdateData
{
	std::ranlux24_base& rng;

	Vector2 playerPosition = {};
};



struct Entity
{
	PhysicalEntity physics;

	Vector2& getPosition()
	{
		return physics.transform.pos;
	}

	void teleport(Vector2 pos)
	{
		physics.teleport(pos);
	}

	virtual void render(AssetManager& assetManager) = 0;

	virtual void update(float deltaTime, EntityUpdateData entityUpdateData) = 0;

	virtual int getEntityType() = 0;
};