#pragma once
#include <ProcGen/Decorators/IDecorator.h>

class SimpleDecorator : public IDecorator
{
	void decorate(SlotPositionList& slotPositions, TaggedPositions& taggedPositions, const GenerationContext& context) const override;
	void createFromJsonImpl(const rapidjson::Value& json) override;

private:
	entt::id_type _slot;
};