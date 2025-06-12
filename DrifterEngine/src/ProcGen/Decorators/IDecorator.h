#pragma once
#include <JSON/ICreateFromJson.h>
#include <ProcGen/TaggedPositions.h>
#include <ProcGen/SlotPositionList.h>
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
	virtual void decorate(SlotPositionList& inOutSlotPositions, TaggedPositions& inOutTaggedPositions, const GenerationContext& context) const = 0;

protected:
	virtual void createFromJsonImpl(const rapidjson::Value& json) = 0;
	PositionList getMyPositions(const TaggedPositions& taggedPositions) const;
	bool meetsCondition(drft::rng::Random& random) const;
	template<typename T>
	size_t generateUniqueSeed(size_t fromGlobalSeed) const
	{
		size_t seed = fromGlobalSeed;
		entt::id_type typeHash = entt::type_index<T>::value();
		hash_combine(seed, typeHash);
		return seed;
	}

private:
	drft::util::BooleanStringExpression _tagExpression;
	float _chance;
};