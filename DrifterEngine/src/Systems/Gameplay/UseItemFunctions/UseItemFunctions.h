#pragma once

namespace drft::system::use
{
	using Parameters = std::unordered_map<std::string, float>;
	void heal(entt::registry& registry, entt::entity user, Parameters params);
}