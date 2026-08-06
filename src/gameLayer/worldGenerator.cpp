#include "worldGenerator.h"
#include "randomStuff.h"
#include <FastNoiseSIMD.h>
#include <raylib.h>
#include "structure.h"
#include "saveMap.h"

void generateWorld(GameMap& gameMap, int seed)
{
	const int w = 900;
	const int h = 500;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed++);

	int desertStart = getRandomInt(rng, 10, w - 210);
	int desertEnd = desertStart + 100 + getRandomInt(rng, 0, 100);
	if (desertEnd > w) { desertEnd = w; }

	Structure treeStructure;
	loadBlockDataFromFile(treeStructure.structureData, treeStructure.w, treeStructure.h, RESOURCES_PATH "structures/tree.bin");

	// create pointer to NoiseGenerator
	std::unique_ptr<FastNoiseSIMD> dirtNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> stoneNoiseGenerator(FastNoiseSIMD::NewFastNoiseSIMD());

	// set up seeds for NoiseGenerator
	dirtNoiseGenerator->SetSeed(seed++);
	stoneNoiseGenerator->SetSeed(seed++);

	// Set up configuation
	dirtNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	dirtNoiseGenerator->SetFractalOctaves(1);
	dirtNoiseGenerator->SetFrequency(0.02);

	stoneNoiseGenerator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	stoneNoiseGenerator->SetFractalOctaves(4);
	stoneNoiseGenerator->SetFrequency(0.01);

	// generate noises
	// Allocate two aligned float‑array buffers for noise values.
	float* dirtNoise = FastNoiseSIMD::GetEmptySet(w);
	float* stoneNoise = FastNoiseSIMD::GetEmptySet(w);
	// Compute terrain‑height noise and write results into buffers via SIMD acceleration.
	dirtNoiseGenerator->FillNoiseSet(dirtNoise, 0.2f, 0, 0, w, 1, 1);
	stoneNoiseGenerator->FillNoiseSet(stoneNoise, 0.2f, 0, 0, w, 1, 1);

	// convert from [-1,1] to [0,1]
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoise[i] + 1) / 2;
		stoneNoise[i] = (stoneNoise[i] + 1) / 2;

		//dirtNoise[i] = std::pow(dirtNoise[i], 1);		// gentle/steeper dirt curve
		//stoneNoise[i] = std::pow(stoneNoise[i], 2);		// gentle/steeper stone curve
	}
	
	// offset means the thickness from stoneHeight to dirtHeight
	// simply speaking, the thickness of dirt
	int dirtOffsetStart = -5;
	int dirtOffsetEnd = 35;
	// the thickness of stones
	int stoneHeightStart = 80;
	int stoneHeightEnd = 170;

	for (int x = 0; x < w; x++)
	{

		bool inDesert = (x >= desertStart && x <= desertEnd);

		int dirtType = Block::dirt;
		int grassType = Block::grassBlock;
		int stoneType = Block::stone;

		if (inDesert == true)
		{
			dirtType = Block::sand;
			grassType = Block::sand;
			stoneType = Block::sandStone;
		}

		int stoneHeight = stoneHeightStart + (stoneHeightEnd - stoneHeightStart) * stoneNoise[x]; // the curve of stones
		int dirtOffset = dirtOffsetStart + (dirtOffsetEnd - dirtOffsetStart) * dirtNoise[x];		// the offset
		int dirtHeight = stoneHeight - dirtOffset;		// the curve of dirt

		for (int y = 0; y < h; y++)
		{
			Block b;
			b.type = Block::air;

			if (y > dirtHeight)
			{
				b.type = dirtType;
			}
			if (y == dirtHeight)
			{
				b.type = grassType;
			}
			if (y > stoneHeight)
			{
				b.type = stoneType;
			}

			if (inDesert)
			{
				int desertMidPos = (desertEnd + desertStart) / 2;
				int desertHalfWidth = (desertEnd - desertStart) / 2;
				int distanceFromDesertMid = std::abs(x - desertMidPos);

				// give a value from 0 at edge to 1 at center
				float desertDepthPercent = 1 - distanceFromDesertMid / float(desertHalfWidth);

				int desertStoneStart = 10 + stoneHeight;
				int desertStoneDepth = 20 + stoneHeight;	// how deep the triangle goes

				int triangleStoneY = desertStoneStart + desertDepthPercent * desertStoneDepth;

				// apply stone if below the triangle
				if (y > triangleStoneY)
				{
					b.type = Block::stone;
				}


			}

			gameMap.getBlockUnsafe(x, y) = b;
		}
	}

	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
	FastNoiseSIMD::FreeNoiseSet(stoneNoise);

#pragma region fill trees
	for (int x = 0; x < w; x++)
	{
		if (getRandomChance(rng, 0.04))
		{
			for (int y = 0; y < h; y++)
			{
				auto type = gameMap.getBlockUnsafe(x, y).type;

				if (type == Block::air)
				{
					continue;
				}
				if (type == Block::grassBlock)
				{
					Vector2 spawnPos{ (float)x, (float)y };
					spawnPos.x -= 1;	// hard_code
					spawnPos.y -= treeStructure.h;

					if (spawnPos.y >= 0)
					{
						treeStructure.pasteIntoMap(gameMap, spawnPos);
					}
					x += 3;		// increase distance between trees to avoid overlap

					break;
				}
				else
				{
					continue;
				}

			}
		}
	}
	
#pragma endregion
}