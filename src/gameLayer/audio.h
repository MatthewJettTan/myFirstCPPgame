#pragma once

namespace Audio
{

	void init();

	void loadAllMusicAndSounds();

	void update();

	void playSound(int sound, float volume = 1);

	enum Sounds
	{
		noneSound = 0,
		placeBlock,
		breakBlock,

		SOUNDS_COUNT
	};

	enum Musics
	{
		noneMusic = 0,
		musicForest,
		musicDesert,
		musicSnow,
		musicCave,

		MUSIC_COUNT,
	};

}