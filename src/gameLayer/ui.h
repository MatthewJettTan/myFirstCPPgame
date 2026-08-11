#pragma once
#include <raylib.h>
#include <string>
#include <vector>

Rectangle placeRectangleTopRightCorner(Rectangle r, float w);

Rectangle placeRectangleTopLeftCorner(Rectangle r, float w);

Rectangle shrinkRectanglePercentage(Rectangle r, float percentageX, float percentageY);

Rectangle placeRectangleCenterTop(Rectangle r, float w);

struct UIEngine
{

	enum Type
	{
		none,
		title,
		button,
	};

	struct Widget
	{
		std::string text = "";
		int type = 0;

		bool isHovered = 0;
		bool isBeingClicked = 0;
		bool isReleased = 0;

	};

	std::vector<Widget> widgets;

	std::vector<Widget> lastFrameWidgets;

	bool addButton(std::string text)
	{
		Widget w;
		w.type = button;
		w.text = text;

		widgets.push_back(w);

		if (lastFrameWidgets.size() < widgets.size())
		{
			return false;
		}
		if (lastFrameWidgets[widgets.size() - 1].type != button)
		{
			return false;
		}

		return lastFrameWidgets[widgets.size() - 1].isReleased;
	}

	void addTitle(std::string text)
	{
		Widget w;
		w.type = title;
		w.text = text;

		widgets.push_back(w);
	}

	void updateAndRender();

};