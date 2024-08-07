#pragma once
#include <ProcGen/GenerationLayer/GenerationState.h>

class AbstractChunk
{
public:
	virtual GenerationState generate() = 0;
	virtual void destroy() = 0;
	virtual bool isGenerated() const = 0;
};