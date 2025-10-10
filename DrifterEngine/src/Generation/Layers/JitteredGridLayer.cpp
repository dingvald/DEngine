#include "pch.h"
#include "JitteredGridLayer.h"
#include <Random/Random.h>

static const float JITTER_FACTOR = 0.25;

using namespace drft;

GenerationState JitteredGridLayerChunk::generate(GenerationLevel)
{
	drft::rng::Random random{ getLocalSeed() };
	const sf::Vector3i centerPoint = _volume.center();
	const sf::Vector3i dimensions = _volume.dimensions();

	sf::Vector2i jitter;
	jitter.x = random.intInRange(dimensions.x * -JITTER_FACTOR, dimensions.x * JITTER_FACTOR);
	jitter.y = random.intInRange(dimensions.y * -JITTER_FACTOR, dimensions.y * JITTER_FACTOR);
	jitteredPoint = spatial::toXY(centerPoint) + jitter;

	return GenerationState::Complete;
}

std::vector<sf::Vector2i> JitteredGridLayer::getPointsInArea(sf::IntRect area, sf::Vector3i origin)
{
	std::vector<sf::Vector2i> result;
	forEachLoadedChunkInArea(area, origin.z, [&area, &result](JitteredGridLayerChunk& chunk)
		{
			if (!area.contains(chunk.jitteredPoint)) return;

			result.emplace_back(chunk.jitteredPoint);
		});
	return result;
}

sf::Vector3i drft::JitteredGridLayer::getChunkDimensions() const
{
	return { 8, 8, 8 };
}


