#pragma once
#include <raymath.h>

struct Settings
{

	float musicVolume = 0.75f;
	float masterVolume = 0.75f;
	float soundsVolume = 0.75f;

	void sanitize()
	{
		musicVolume = Clamp(musicVolume, 0.f, 1.f);
		masterVolume = Clamp(masterVolume, 0.f, 1.f);
		soundsVolume = Clamp(soundsVolume, 0.f, 1.f);
	}

};

Settings& getSettings();