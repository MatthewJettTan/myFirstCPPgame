#include "worldGenerator.h"
#include "randomStuff.h"

void generateWorld(GameMap& gameMap, int seed)
{
	const int w = 900;
	const int h = 500;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed);

	int keepDirectionTimeDirt = getRandomInt(rng, 5, 40);
	int directionDirt = getRandomInt(rng, -2, 2);

	int keepDirectionTimeStone = getRandomInt(rng, 5, 40);
	int directionStone = getRandomInt(rng, -2, 2);

	int dirtHeight = 70;
	int stoneHeight = 90;

	for (int x = 0; x < w; x++)
	{
		// code for dirt
		keepDirectionTimeDirt--;
		if (keepDirectionTimeDirt <= 0)
		{
			keepDirectionTimeDirt = getRandomInt(rng, 5, 40);
			directionDirt = getRandomInt(rng, -2, 2);
		}
		if (directionDirt == -1)
		{
			if (getRandomChance(rng, 0.5))
			{
				dirtHeight--;
			}
		}
		else if (directionDirt == -2)
		{
			if (getRandomChance(rng, 0.5))
			{
				dirtHeight--;
			}
			if (getRandomChance(rng, 0.5))
			{
				dirtHeight--;
			}
		}
		else if (directionDirt == 1)
		{
			if (getRandomChance(rng, 0.5))
			{
				dirtHeight++;
			}
		}
		else if (directionDirt == 2)
		{
			if (getRandomChance(rng, 0.5))
			{
				dirtHeight++;
			}
			if (getRandomChance(rng, 0.5))
			{
				dirtHeight++;
			}
		}
		if (dirtHeight < 50)
		{
			dirtHeight = 50;
		}
		if (dirtHeight > 90)
		{
			dirtHeight = 90;
		}

		// same code for stone
		keepDirectionTimeStone--;
		if (keepDirectionTimeStone <= 0)
		{
			keepDirectionTimeStone = getRandomInt(rng, 5, 40);
			directionStone = getRandomInt(rng, -2, 2);
		}
		if (directionStone == -1)
		{
			if (getRandomChance(rng, 0.5))
			{
				stoneHeight--;
			}
		}
		if (directionStone == -2)
		{
			if (getRandomChance(rng, 0.5))
			{
				stoneHeight--;
			}
			if (getRandomChance(rng, 0.5))
			{
				stoneHeight--;
			}
		}
		if (directionStone == 1)
		{
			if (getRandomChance(rng, 0.5))
			{
				stoneHeight++;
			}
		}
		if (directionStone == 2)
		{
			if (getRandomChance(rng, 0.5))
			{
				stoneHeight++;
			}
			if (getRandomChance(rng, 0.5))
			{
				stoneHeight++;
			}
		}
		if (stoneHeight < 60)
		{
			stoneHeight = 60;
		}

		if (stoneHeight > 120)
		{
			stoneHeight = 120;
		}

		// set the vertical blocks
		for (int y = 0; y < h; y++)
		{
			Block b;
			b.type = Block::air;

			if (y > dirtHeight)
			{
				b.type = Block::dirt;
			}

			if (y == dirtHeight)
			{
				b.type = Block::grassBlock;
			}

			if (y > stoneHeight)
			{
				b.type = Block::stone;
			}

			gameMap.getBlockUnsafe(x, y) = b;
		}
	}
}