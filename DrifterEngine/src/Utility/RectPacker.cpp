#include "pch.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "RectPacker.h"

RectPacker::RectPacker(int width, int height)
	: _width(width)
	, _height(height)
	, _maxWidth(0)
	, _maxHeight(0)
{}

bool RectPacker::pack(std::vector<PackingRect>& inoutRects)
{
	if (inoutRects.empty()) return false;

	stbrp_context context = {};
	std::vector<stbrp_node> nodes;
	nodes.resize(inoutRects.size());
	stbrp_init_target(&context, _width, _height, nodes.data(), nodes.size());
	const int result = stbrp_pack_rects(&context, inoutRects.data(), inoutRects.size());

	for (const auto& rect : inoutRects)
	{
		_maxWidth = std::max(_maxWidth, rect.w + rect.x);
		_maxHeight = std::max(_maxHeight, rect.h + rect.y);
	}

	return result;
}

sf::Vector2i RectPacker::getActualSize()
{
	return {_maxWidth, _maxHeight};
}
