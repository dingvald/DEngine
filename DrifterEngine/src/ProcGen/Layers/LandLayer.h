#pragma once

#include <ProcGen/LayeredProcGen/LayeredProcGen.h>
#include <JSON/ICreateFromJson.h>

class LandLayer;

class LandLayerChunk : public GenerationChunk<LandLayer, LandLayerChunk>
{
public:
	using GenerationChunk::GenerationChunk;
	virtual GenerationState generate() override;

	std::vector<sf::Vector2i> landPoints;
	std::vector<sf::Vector2i> oceanPoints;
};

class LandLayer : public GenerationLayer<LandLayer, LandLayerChunk>, public ICreateFromJson
{
public:
	LandLayer();

	void createFromJson(const rapidjson::Value& json) override;

	bool isLand(sf::Vector2i point);

	entt::id_type getInputLayerID() const;
	float getLandThreshold() const;

private:
	entt::id_type _inputLayerID = {};
	float _landThreshold = 0.f;
};
