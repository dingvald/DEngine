#pragma once
#include <JSON/ICreateFromJson.h>
#include <Generation/Decorators/IDecorator.h>
#include <Generation/SlotPositionList.h>


class IFeature;
struct GenerationContext;

struct GeneratedFeature
{
	SlotPositionList slotPositions;
	sf::IntRect area;
	const IFeature* feature = nullptr;
};

using TagList = std::vector<entt::id_type>;

class IFeature : public ICreateFromJson
{
public:
	using Ptr = std::unique_ptr<IFeature>;
public:
	virtual void createFromJson(const rapidjson::Value& json) = 0;
	GeneratedFeature generate(sf::Vector3i position, const GenerationContext& context) const;
	void addDecorator(IDecorator::Ptr&& decorator);

protected:
	virtual TaggedPositions generateTags(sf::Vector3i position, const GenerationContext& context) const = 0;

private:
	GeneratedFeature decorate(TaggedPositions& taggedPositions, const GenerationContext& context) const;

private:
	std::vector<IDecorator::Ptr> _decorators;
};