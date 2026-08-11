#include "player.h"
#include "assetManager.h"
#include "helpers.h"

void Player::render(AssetManager& assetManager)
{
	auto aabb = physics.transform.getAABB();

	// this is the old version
	//DrawTexturePro(
	//	assetManager.player,
	//	{0, 0, (float)assetManager.player.width, (float)assetManager.player.height},
	//	aabb,
	//	{ 0,0 },
	//	0.0f,
	//	WHITE
	//);

	auto textureUV = getTextureAtlas(animations.positionX, animations.positionY, 32, 64,
		animations.movingLeft);

	DrawTexturePro(
		assetManager.playerBack,
		textureUV,
		aabb,
		{ 0,0 },
		0.0f,
		WHITE
	);

	DrawTexturePro(
		assetManager.playerFeet,
		textureUV,
		aabb,
		{ 0,0 },
		0.0f,
		WHITE
	);

	DrawTexturePro(
		assetManager.playerHead,
		textureUV,
		aabb,
		{ 0,0 },
		0.0f,
		WHITE
	);

	DrawTexturePro(
		assetManager.playerFront,
		textureUV,
		aabb,
		{ 0,0 },
		0.0f,
		WHITE
	);





}

bool Player::update(float deltaTime, EntityUpdateData entityUpdateData)
{

	return true;
}