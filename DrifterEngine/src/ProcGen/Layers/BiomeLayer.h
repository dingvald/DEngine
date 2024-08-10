#pragma once
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class BiomeLayer;

namespace details
{
	class BiomeLayerChunk : public GenerationChunk<BiomeLayer, BiomeLayerChunk>
	{
	public:
		using GenerationChunk::GenerationChunk;

		virtual GenerationState generate() override;
		virtual void destroy() override;
		std::string message = "EMPTY";
	};
}


class BiomeLayer : public GenerationLayer<BiomeLayer, details::BiomeLayerChunk>
{
public:
	BiomeLayer();
	std::string getMessagesInArea(sf::IntRect area);
};