#pragma once
#include <unordered_map>
#include <string>

namespace drft::gen
{
	struct GenerationParameters
	{
		std::unordered_map<std::string, float> params;
	};
}
