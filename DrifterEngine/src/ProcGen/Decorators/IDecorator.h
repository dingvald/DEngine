#pragma once
#include <JSON/ICreateFromJson.h>
#include <ProcGen/TaggedPositions.h>
#include <ProcGen/SlotPositionList.h>

struct GenerationContext;

namespace drft::rng
{
	class Random;
}

class IDecorator : public ICreateFromJson
{
public:
	using Ptr = std::unique_ptr<IDecorator>;
public:
	void createFromJson(const rapidjson::Value& json) override;
	virtual void decorate(SlotPositionList& slotPositions, TaggedPositions& taggedPositions, const GenerationContext& context) const = 0;

protected:
	virtual void createFromJsonImpl(const rapidjson::Value& json) = 0;
	const PositionList& getMyPositions(const TaggedPositions& taggedPositions) const;
	bool meetsCondition(drft::rng::Random& random) const;

private:
	entt::id_type _tag;
	float _chance;
};