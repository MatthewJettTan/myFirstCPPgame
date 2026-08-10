#include "items.h"
#include "assetManager.h"
#include "helpers.h"

Texture getTextureForItemType(int itemType, AssetManager& assetManager)
{
	if (itemType < Block::BLOCKS_COUNT)
	{
		// is a block
		return assetManager.textures;
	}
	else
	{
		// is a item
		return assetManager.items;
	}
}

Rectangle getTextureCoordinatesForItemType(int itemType)
{
	if (itemType < Block::BLOCKS_COUNT)
	{
		// is a block
		return getTextureAtlas(itemType, 4, 32, 32);
	}
	else
	{
		// is a item
		return getTextureAtlas(itemType - Block::BLOCKS_COUNT, 0, 32, 32);
	}
}