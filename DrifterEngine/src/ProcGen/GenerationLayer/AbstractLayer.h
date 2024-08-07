#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <ProcGen/GenerationLayer/GenerationState.h>
#include <ProcGen/GenerationLayer/GenerationContext.h>

class AbstractLayer
{
public:
	virtual GenerationState generate(GenerationContext&& context) = 0;
};