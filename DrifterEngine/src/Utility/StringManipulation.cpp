#include "pch.h"
#include "StringManipulation.h"

std::string drft::util::getStringAcronym(const std::string& string)
{
    std::string result;
    if (string.empty()) return result;

    result.push_back(string[0]);
    for (int i = 0; i < string.size(); ++i)
    {
        if (string[i] == ' ' && ((i+1) < string.size()))
        {
            result.push_back(string[i + 1]);
        }
    }

    return result;
}

std::vector<std::string> drft::util::split(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> res;
    size_t pos_start = 0;
    size_t pos_end = 0;
    std::string token;

    while ((pos_end = str.find_first_of(delimiters, pos_start)) != std::string::npos) {
        if (pos_end != pos_start)
        {
            res.emplace_back(str.substr(pos_start, pos_end - pos_start));
        }
        pos_start = pos_end + 1;
    }

    if (pos_start != str.length()) {
        res.push_back(str.substr(pos_start));
    }
    return res;
}

std::string drft::util::removeWhitespace(const std::string& str)
{
    std::string copy = str;
    copy.erase(std::remove_if(copy.begin(), copy.end(), ::isspace), copy.end());
    return std::move(copy);
}

std::vector<std::string> drft::util::removeWhitespace(const std::vector<std::string>& strings)
{
    std::vector<std::string> result;

    for (auto&& str : strings)
    {
        result.emplace_back(removeWhitespace(str));
    }

    return result;
}

std::string drft::util::capitalize(const std::string& str)
{
    if (str.size() == 0) return {};

    std::string result = str;
    result[0] = std::toupper(result[0]);

    return result;
}

bool drft::util::stringContains(const std::string& str, const std::string& subStr)
{
    if (str.find(subStr) != std::string::npos)
    {
        return true;
    }
    return false;
}

