#include "worldGenerator.h"
#include "randomStuff.h"

void generateWorld(GameMap& gameMap, int seed)
{
	const int w = 900;
	const int h = 500;

	gameMap.create(w, h);

	int stoneSize = 70;
	int dirtSize = 90;

	std::ranlux24_base rng(seed);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			Block b;

			if (y < h - (dirtSize + stoneSize))
			{
				// air
			}
			else if (y == h - (dirtSize + stoneSize))
			{
				b.type = Block::grassBlock;
			}
			else if (y < h - stoneSize)
			{
				b.type = Block::dirt;
			}
			else
			{
				b.type = Block::stone;

				if (getRandomChance(rng, 0.1))
				{
					b.type = Block::gold;
				}
			}

			gameMap.getBlockUnsafe(x, y) = b;
		}
	}
}