#pragma once
#include <JSON/ICreateFromJson.h>
#include <Generation/TaggedPositions.h>
#include <Generation/SlotPositionList.h>
#include <JSON/StringExpressions.h>

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
	void decorate(SlotPositionList& inOutSlotPositions, const TaggedPositions& taggedPositions, const GenerationContext& context) const;

protected:
	virtual SlotPositionList decorateImpl(const PositionList& taggedPositions, const GenerationContext& context) const = 0;
	virtual void createFromJsonImpl(const rapidjson::Value& json) = 0;
	size_t generateUniqueSeed(size_t fromSeed) const;

private:
	PositionList getMyPositions(const TaggedPositions& taggedPositions, size_t globalSeed) const;
	bool meetsCondition(drft::rng::Random& random) const;

private:
	drft::util::BooleanStringExpression _tagExpression;
	float _chance = 1.0f;
	int _priority = UNINITIALIZED_SLOT_PRIORITY;
};