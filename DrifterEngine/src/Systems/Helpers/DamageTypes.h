#pragma once

namespace Damage
{
	using TypeMap = std::unordered_map<std::string, int>;

	void mergeOrAdd(TypeMap& main, const TypeMap& other);
}
