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

std::vector<std::string> drft::util::stringSplit(const std::string& string, const std::string& delimiters)
{
    std::vector<std::string> res;
    size_t pos_start = 0, pos_end, delim_len = delimiters.length();
    std::string token;

    while ((pos_end = string.find(delimiters, pos_start)) != std::string::npos) {
        token = string.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.emplace_back(token);
    }

    res.emplace_back(string.substr(pos_start));
    return res;
}

