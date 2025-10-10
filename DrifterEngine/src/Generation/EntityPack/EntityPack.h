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
	void add(const EntityPack& other);
	std::optional<entt::id_type> selectEntity(entt::id_type slotId, drft::rng::Random& random) const;
	bool has(entt::id_type slotId) const;

private:
	using WeightedEntityList = std::unordered_map<entt::id_type, int>;
	std::unordered_map<entt::id_type, WeightedEntityList> _packs;
};