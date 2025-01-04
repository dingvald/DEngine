#pragma once
#include <variant>
#include <vector>
#include <EnTT/entt.hpp>

template<typename EntityType>
class IUtilityBlackboard
{
public:
	using Data = std::variant<std::monostate, bool, float, int, EntityType>;
	using DataList = std::vector<Data>;
	using DataName = std::uint32_t;

	virtual Data get(DataName name) const = 0;
	virtual const DataList& getList(DataName name) const = 0;
};