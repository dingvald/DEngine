#pragma once
#include <ProcGen/GenerationLayer/GenerationLayer.h>
#include <ProcGen/GenerationLayer/GenerationChunk.h>

class EntityLayer;

class EntityLayerChunk : public GenerationChunk<EntityLayer, EntityLayerChunk>
{


};

class EntityLayer : public GenerationLayer<EntityLayer, EntityLayerChunk>
{
public:
	EntityLayer();

private:

};

