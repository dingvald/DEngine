#pragma once
#include "Systems/System.h"

namespace drft::system
{
	enum class Relationship
	{
		Friendly,
		Neutral,
		Hostile
	};
	using Closeness = int;

	class FactionSystem : public System
	{
	public:
		using System::System;

		void init() override;

		[[nodiscard]] static Relationship resolveRelationship(entt::const_handle entity1, entt::const_handle entity2);
		static void modifyCloseness(const std::string& faction1, const std::string& faction2, Closeness deltaValue);

		// TODO: Save/Load faction table

	private:
		static Closeness getCloseness(const std::string& faction1, const std::string& faction2);
		
		using FactionTable = std::unordered_map<std::string, std::unordered_map<std::string, int>>;
		static FactionTable _factionTable;
	};


}
