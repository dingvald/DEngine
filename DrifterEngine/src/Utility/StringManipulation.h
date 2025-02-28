#pragma once

namespace drft::util
{
	std::string getStringAcronym(const std::string& string);
	std::vector<std::string> split(const std::string& string, const std::string& delimiters);
	std::string removeWhitespace(const std::string& str);
	std::vector<std::string> removeWhitespace(const std::vector<std::string>& strings);

	std::string capitalize(const std::string& str);
}