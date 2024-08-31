#include "pch.h"
#include "JitteredGridLayer.h"
#include <Random/Random.h>

static const float JITTER_FACTOR = 0.25;

GenerationState JitteredGridLayerChunk::generate(int level)
{
	drft::rng::Random random{ getLocalSeed() };
	const sf::Vector2i centerPoint = { _bounds.left + _bounds.width / 2, _bounds.top + _bounds.height / 2 };
	sf::Vector2i jitter;
	jitter.x = random.intInRange(_bounds.width * -JITTER_FACTOR, _bounds.width * JITTER_FACTOR);
	jitter.y = random.intInRange(_bounds.height * -JITTER_FACTOR, _bounds.height * JITTER_FACTOR);;
	jitteredPoint = centerPoint + jitter;

	return GenerationState::Complete;
}

JitteredGridLayer::JitteredGridLayer()
	: GenerationLayer({8, 8})
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


