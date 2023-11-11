#pragma once

namespace drft::util
{
	std::string getStringAcronym(const std::string& string);
	std::vector<std::string> stringSplit(const std::string& string, const std::string& delimiters);
}