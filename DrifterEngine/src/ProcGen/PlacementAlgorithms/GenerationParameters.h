#pragma once
#include <unordered_map>
#include <string>
#include "Spatial/AutoGrid.h"

namespace drft::gen
{
	using GenerationParameters = std::unordered_map<std::string, std::variant<float, std::string>>;
}
