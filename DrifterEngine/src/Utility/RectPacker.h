#pragma once
#include "stb/stb_rect_pack.h"

using PackingRect = stbrp_rect;

class RectPacker
{
public:
	RectPacker(int width, int height);
	bool pack(std::vector<PackingRect>& inoutRects);
	sf::Vector2i getActualSize();

private:
	int _width;
	int _height;

	int _maxWidth;
	int _maxHeight;
};