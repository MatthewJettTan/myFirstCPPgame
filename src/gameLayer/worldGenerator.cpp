#include "worldGenerator.h"
#include "randomStuff.h"
#include <FastNoiseSIMD.h>

void generateWorld(GameMap& gameMap, int seed)
{
	const int w = 900;
	const int h = 500;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed++);

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

		int stoneHeight = stoneHeightStart + (stoneHeightEnd - stoneHeightStart) * stoneNoise[x]; // the curve of stones
		int dirtOffset = dirtOffsetStart + (dirtOffsetEnd - dirtOffsetStart) * dirtNoise[x];		// the offset
		int dirtHeight = stoneHeight - dirtOffset;		// the curve of dirt

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

	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
	FastNoiseSIMD::FreeNoiseSet(stoneNoise);
}