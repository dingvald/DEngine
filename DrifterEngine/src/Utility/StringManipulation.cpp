#include "pch.h"
#include "StringManipulation.h"

static const std::string drft::util::EMPTY_STRING = {};

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

std::string drft::util::capitalizeAll(const std::string& str)
{
    if (str.empty()) return {};

    std::string result = str;
    bool capitalizeNext = true;

    for (size_t i = 0; i < result.size(); ++i)
    {
        if (std::isspace(static_cast<unsigned char>(result[i])))
        {
            capitalizeNext = true;
        }
        else if (capitalizeNext)
        {
            result[i] = std::toupper(static_cast<unsigned char>(result[i]));
            capitalizeNext = false;
        }
    }

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

std::optional<std::string> drft::util::getStringBetween(const std::string& str, const std::string& substr1, const std::string& substr2)
{
    size_t pos1 = str.find(substr1);
    if (pos1 == std::string::npos) {
        return std::nullopt;
    }
    size_t start_pos = pos1 + substr1.length();
    size_t pos2 = str.find(substr2, start_pos);
    if (pos2 == std::string::npos) {
        return std::nullopt;
    }
    return str.substr(start_pos, pos2 - start_pos);
}

std::string drft::util::removeUnderscores(const std::string& jsonString)
{
    std::string result = jsonString;
    std::replace(result.begin(), result.end(), '_', ' ');
    return result;
}

