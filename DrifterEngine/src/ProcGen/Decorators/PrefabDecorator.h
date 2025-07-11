#pragma once
#include <ProcGen/Decorators/IDecorator.h>


class PrefabDecorator : public IDecorator
{
	SlotPositionList decorateImpl(const PositionList& taggedPositions, const GenerationContext& context) const override;
	void createFromJsonImpl(const rapidjson::Value& json) override;

private:
	entt::id_type _prefabId;
};

