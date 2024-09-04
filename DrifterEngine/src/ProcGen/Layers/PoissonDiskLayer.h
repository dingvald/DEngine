#pragma once
#include <JSON/ICreateFromJson.h>
#include <ProcGen/LayeredProcGen/LayeredProcGen.h>

class PoissonDiskLayer;

class PoissonDiskLayerChunk : public GenerationChunk<PoissonDiskLayer, PoissonDiskLayerChunk>
{
public:
	using GenerationChunk::GenerationChunk;
	virtual GenerationState generate(int level) override;
	
private:
	virtual int numLevels() override { return 2; }
	GenerationState generateRandomPoints(sf::IntRect area);
	GenerationState relaxRandomPoints(sf::IntRect area);

public:
	std::vector<sf::Vector2i> randomPoints;
	std::unordered_set<sf::Vector2i> distributedPoints;
};

class PoissonDiskLayer : public GenerationLayer<PoissonDiskLayer, PoissonDiskLayerChunk>, public IGetValueAt, public ICreateFromJson
{
public:
	PoissonDiskLayer();

	double getValueAt(sf::Vector2i position) override;
	void createFromJson(const rapidjson::Value& json) override;

	float getDistributionDensity() const;

private:
	float _density = 0.5;
};