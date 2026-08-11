#include "ui.h"

Rectangle placeRectangleTopRightCorner(Rectangle r, float w)
{
	r.x = w - r.width;
	r.y = 0;
	return r;
}

Rectangle placeRectangleTopLeftCorner(Rectangle r, float w)
{
	r.x = 0;
	r.y = 0;
	return r;
}

Rectangle placeRectangleCenterTop(Rectangle r, float w)
{
	r.x = (w - r.width) / 2.0f;
	r.y = 0;
	return r;
}

Rectangle shrinkRectanglePercentage(Rectangle r, float percentageX, float percentageY)
{
	float shrinkX = r.width * percentageX;
	float shrinkY = r.height * percentageY;

	r.width -= shrinkX;
	r.height -= shrinkY;

	r.x += shrinkX / 2.f;
	r.y += shrinkY / 2.f;

	return r;
}

void UIEngine::updateAndRender()
{
	float w = GetScreenWidth();
	float h = GetScreenHeight();

	Rectangle oneButtonRectangle;
	oneButtonRectangle.width = w * 0.8f;
	oneButtonRectangle.height = h / (widgets.size() + 1);

	// to resure button don't occupy the max ammount of space available
	oneButtonRectangle.height = std::min(oneButtonRectangle.height, oneButtonRectangle.width / 8.f);


	oneButtonRectangle = placeRectangleCenterTop(oneButtonRectangle, w);
	oneButtonRectangle.y += oneButtonRectangle.height / 2.f;

	const int fontSize = (int)(oneButtonRectangle.height * 0.5f);

	for (auto& w : widgets)
	{
		Rectangle smallerRect = shrinkRectanglePercentage(oneButtonRectangle, 0.01, 0.1);

		//DrawRectangle(smallerRect.x, smallerRect.y, smallerRect.width, smallerRect.height,
		//	{ 90, 90, 110, 205 });

		auto drawText = [&](Rectangle smallerRect, float yOffset = 0)
			{
				int textWidth = MeasureText(w.text.c_str(), fontSize);
				int textHeight = fontSize;		// in Raylib, font height = fontSize for default font

				float textX = smallerRect.x + (smallerRect.width - textWidth) / 2.f;
				float textY = smallerRect.y + (smallerRect.height - textHeight) / 2.f;

				// to create a shadow effect
				Color shadowColor = { 0, 0, 0, 200 };
				DrawText(w.text.c_str(), textX - fontSize * 0.08f, textY + fontSize * 0.08 + yOffset, fontSize, shadowColor);
				DrawText(w.text.c_str(), textX, textY + yOffset, fontSize, WHITE);

			};

		w.isBeingClicked = false;
		w.isHovered = false;
		w.isReleased = false;

		if (CheckCollisionPointRec(GetMousePosition(), smallerRect))
		{
			w.isHovered = true;

			if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
			{
				w.isBeingClicked = true;
			}
			
			if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
			{
				w.isReleased = true;
			}
		}

		switch (w.type)
		{
		case button:
		{
			const float clickOffset = 0.05f;
			Color clickColor = { 120, 120, 135, 205 };
			Color defaultColor = { 90, 90, 110, 205 };

			if (w.isBeingClicked)
			{
				DrawRectangle(smallerRect.x, smallerRect.y + smallerRect.height * clickOffset,
					smallerRect.width, smallerRect.height,
					clickColor);
			}
			else
			{
				if (w.isHovered)
				{
					DrawRectangle(smallerRect.x, smallerRect.y, smallerRect.width, smallerRect.height,
						clickColor);
				}
				else
				{
					DrawRectangle(smallerRect.x, smallerRect.y, smallerRect.width, smallerRect.height,
						defaultColor);
				}
			}
			
			if (w.isBeingClicked)
			{
				drawText(smallerRect, smallerRect.height * clickOffset);
			}
			else
			{
				drawText(smallerRect);
			}

			break;
		}
		case title:
		{
			drawText(smallerRect);

			break;
		}
		}

		

		oneButtonRectangle.y += oneButtonRectangle.height;
	}

	// check if the widgets didn't change, if the widgets change
	// we want to ignore all inputs for one frame, so that people don't accidentally click an unwanted button
	bool disableInputThisFrame = false;

	// amount change
	if (widgets.size() != lastFrameWidgets.size())
	{
		disableInputThisFrame = true;
	}
	// type change
	else
	{
		for (int i = 0; i < widgets.size(); i++)
		{
			if (widgets[i].type != lastFrameWidgets[i].type)
			{
				disableInputThisFrame = true;
				break;
			}
		}
	}

	lastFrameWidgets = widgets;

	if (disableInputThisFrame)
	{
		for (auto& w : lastFrameWidgets)
		{
			w.isBeingClicked = false;
			w.isHovered = false;
			w.isReleased = false;
		}
	}

	widgets.clear();
}