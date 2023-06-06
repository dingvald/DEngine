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
		void init() override;

		static Relationship resolveRelationship(const std::string& faction1, const std::string& faction2);
		static void modifyCloseness(const std::string& faction1, const std::string& faction2, Closeness deltaValue);

	private:
		static Closeness getCloseness(const std::string& faction1, const std::string& faction2);
		
		using FactionTable = std::unordered_map<std::string, std::unordered_map<std::string, int>>;
		static FactionTable _factionTable;
	};


}
