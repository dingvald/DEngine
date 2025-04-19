#pragma once
#include <ProcGen/Decorators/IDecorator.h>
#include <Utility/Math.h>

class EntityDecorator : public IDecorator
{
	void decorate(SlotPositionList& slotPositions, TaggedPositions& taggedPositions, const GenerationContext& context) const override;
	void createFromJsonImpl(const rapidjson::Value& json) override;

	void generateCluster(sf::Vector3i origin, SlotPositionList& slotPositions, drft::rng::Random& random) const;

private:
	struct SlotNumberChance
	{
		entt::id_type slot;
		int number;
		float chance;
	};
	drft::math::Range<int> _radius;
	std::vector<SlotNumberChance> _slots;
};
