#pragma once
#include <unordered_map>
#include "physics.h"
#include <random>

struct AssetManager;
struct EntityHolder;

enum EntityType
{
	EntityType_Player = 0,
	EntityType_Slime,
	EntityType_DroppedItem,


};



struct EntityUpdateData
{
	std::ranlux24_base& rng;
	Vector2 playerPosition = {};

	EntityHolder& entityHolder;		// used for accessing other entities

	std::uint64_t ownId = 0;		// the if of current entity
};



struct Entity
{
	PhysicalEntity physics;
	float life = 1;

	Vector2& getPosition()
	{
		return physics.transform.pos;
	}

	void teleport(Vector2 pos)
	{
		physics.teleport(pos);
	}

	virtual void render(AssetManager& assetManager) = 0;

	virtual bool update(float deltaTime, EntityUpdateData entityUpdateData) = 0;

	virtual int getEntityType() = 0;

	virtual float getMaxLife() = 0;
};