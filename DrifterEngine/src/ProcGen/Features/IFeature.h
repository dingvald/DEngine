#pragma once
#include <JSON/ICreateFromJson.h>
#include <ProcGen/Decorators/IDecorator.h>
#include <ProcGen/SlotPositionList.h>


class IFeature;
struct GenerationContext;

struct FeatureGenerationResult
{
	sf::Vector2i origin;
	SlotPositionList slotPositions;
	const IFeature* feature = nullptr;
};

using TagList = std::vector<entt::id_type>;

class IFeature : public ICreateFromJson
{
public:
	using Ptr = std::unique_ptr<IFeature>;
public:
	virtual void createFromJson(const rapidjson::Value& json) = 0;
	FeatureGenerationResult generate(sf::Vector2i position, const GenerationContext& context) const;
	void addDecorator(IDecorator::Ptr&& decorator);

protected:
	virtual TaggedPositions doGenerate(const GenerationContext& context) const = 0;

private:
	FeatureGenerationResult decorate(TaggedPositions& taggedPositions, const GenerationContext& context) const;

private:
	std::vector<IDecorator::Ptr> _decorators;
};