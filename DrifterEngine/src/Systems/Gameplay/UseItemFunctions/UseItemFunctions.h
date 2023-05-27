#pragma once

namespace drft::system::use
{
	using Parameters = std::unordered_map<std::string, std::string>;
	void heal(entt::registry& registry, entt::entity user, Parameters params);
	void read(entt::registry& registry, entt::entity user, Parameters params);
}