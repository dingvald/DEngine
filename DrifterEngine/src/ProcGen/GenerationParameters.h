#pragma once
#include <unordered_map>
#include <variant>
#include <string>
#include "Spatial/AutoGrid.h"

namespace drft::gen
{
	using Parameter = std::variant<bool, float, std::string, std::vector<float>>;
	using GenerationParameters = std::unordered_map<std::string, Parameter>;
}
