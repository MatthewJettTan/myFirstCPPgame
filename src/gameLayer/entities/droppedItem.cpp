#include "droppedItem.h"
#include "assetManager.h"
#include "helpers.h"
#include "entityHolder.h"
#include "items.h"

void DroppedItem::render(AssetManager& assetManager)
{
	auto aabb = getRectangleForEntity(physics.transform, 1, 1);

	Texture2D texture = getTextureForItemType(itemType, assetManager);
	Rectangle rectangle = getTextureCoordinatesForItemType(itemType);

	DrawTexturePro(
		texture,
		rectangle,
		aabb,
		{ 0,0 },
		0.0f,
		WHITE
	);
}

bool DroppedItem::update(float deltaTime, EntityUpdateData entityUpdateData)
{
	for (auto& e : entityUpdateData.entityHolder.entities)
	{
		if (e.first != entityUpdateData.ownId)
		{
			if (e.second->getEntityType() == EntityType::EntityType_DroppedItem)
			{
				DroppedItem* other = reinterpret_cast<DroppedItem*>(e.second.get());

				if (itemType == other->itemType)
				{
					if (Vector2Distance(getPosition(), other->getPosition()) < 0.7)
					{
						other->itemCounter += itemCounter;
						return 0;
					}
				}
			}
		}
	}
	return true;
}
