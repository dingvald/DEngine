#pragma once
#include <JSON/ICreateFromJson.h>

namespace drft::rng
{
	class Random;
}

class EntityPack : public ICreateFromJson
{
public:
	void createFromJson(const rapidjson::Value& json) override;
	void add(EntityPack other);
	std::optional<std::string> selectEntity(entt::id_type slotId, drft::rng::Random& random);

private:
	using WeightedEntityList = std::unordered_map<std::string, int>;
	std::unordered_map<entt::id_type, WeightedEntityList> _packs;

	
};