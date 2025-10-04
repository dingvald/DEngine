#pragma once
#include <JSON/ICreateFromJson.h>
#include <ProcGen/Structures/Instance/StructureInstanceLayer.h>
#include <ProcGen/PositionSelector/IPositionSelector.h>
#include <JSON/JsonValueStorage.h>
#include <ProcGen/Decorators/IDecorator.h>

struct GenerationContext;
struct GenerationFinalizationContext;

class StructureTemplateLayer : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	void finalize(const GenerationFinalizationContext& context);

	StructureInstanceLayer generate(sf::Vector2i origin, const GenerationContext& context) const;
	sf::IntRect getArea() const { return _area;}

private:
	struct PrefabPoolItem : ICreateFromJson
	{
		void createFromJson(const rapidjson::Value& json) override;
		void finalize(const GenerationFinalizationContext& context);

		entt::id_type id;
		IPositionSelector::Ptr positionSelector = nullptr;

	private:
		JsonValueStorage::Ptr _positionSelectorJson = nullptr;
	};
	struct DecoratorItem : ICreateFromJson
	{
		void createFromJson(const rapidjson::Value& json) override;
		void finalize(const GenerationFinalizationContext& context);
		void decorate(SlotPositionList& inOutSlotPositions, const TaggedPositions& taggedPositions, const GenerationContext& context) const;

	private:
		JsonValueStorage::Ptr _decoratorJson = nullptr;
		IDecorator::Ptr _decoratorPtr = nullptr;
	};

private:
	std::vector<PrefabPoolItem> _prefabPool;
	std::vector<DecoratorItem> _decorators;
	sf::IntRect _area;
};