#include "pch.h"
#include "JitteredGridLayer.h"
#include <Random/Random.h>

static const float JITTER_FACTOR = 0.25;

GenerationState JitteredGridLayerChunk::generate()
{
	drft::rng::Random random{ getLocalSeed() };
	const sf::Vector2i centerPoint = { bounds().left + bounds().width / 2, bounds().top + bounds().height / 2 };
	sf::Vector2i jitter;
	jitter.x = random.intInRange(bounds().width * -JITTER_FACTOR, bounds().width * JITTER_FACTOR);
	jitter.y = random.intInRange(bounds().height * -JITTER_FACTOR, bounds().height * JITTER_FACTOR);;
	jitteredPoint = centerPoint + jitter;

	return GenerationState::Complete;
}

JitteredGridLayer::JitteredGridLayer()
	: GenerationLayer({32, 32})
{}

std::vector<sf::Vector2i> JitteredGridLayer::getPointsInBounds(sf::IntRect area)
{
	std::vector<sf::Vector2i> result;
	forEachLoadedChunkInArea(area, [&area, &result](JitteredGridLayerChunk& chunk)
		{
			if (area.contains(chunk.jitteredPoint))
			{
				result.emplace_back(chunk.jitteredPoint);
			}
		});
	return result;
}


