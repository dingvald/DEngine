#pragma once
#include <JSON/ICreateFromJson.h>
#include <ProcGen/Structures/Instance/StructureInstanceLayer.h>
#include <ProcGen/PositionSelector/IPositionSelector.h>
#include <JSON/JsonValueStorage.h>

struct GenerationContext;
struct GenerationFinalizationContext;

struct PrefabPoolItem : ICreateFromJson
{
	void createFromJson(const rapidjson::Value& json) override;
	void finalize(const GenerationFinalizationContext& context);

	entt::id_type id;
	IPositionSelector::Ptr positionSelector = nullptr;

private:
	JsonValueStorage::Ptr _positionSelectorJson = nullptr;
};

class StructureTemplateLayer : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	void finalize(const GenerationFinalizationContext& context);

	StructureInstanceLayer generate(const GenerationContext& context) const;
	sf::IntRect getArea() const { return _area;}

private:
	std::vector<PrefabPoolItem> _prefabPool;
	sf::IntRect _area;
};