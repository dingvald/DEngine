#include "pch.h"
#include "GetStringAcronym.h"

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
