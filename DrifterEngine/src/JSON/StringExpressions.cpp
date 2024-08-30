#include "pch.h"
#include "StringExpressions.h"
#include <Utility/StringManipulation.h>
#include <Utility/StandardErrorLogger.h>

using namespace entt::literals;

const std::unordered_map<entt::id_type, std::function<bool(bool, bool)>> BooleanExpressions
{
    {"&"_hs, [](bool lhs, bool rhs)->bool {return lhs && rhs; }},
    {"|"_hs, [](bool lhs, bool rhs)->bool {return lhs || rhs; }}
};

const auto StringEnd = std::string::npos;

const std::string_view BooleanOperations = "&|()";

const std::unordered_set<entt::id_type> HashedOperations =
{
    "&"_hs,
    "|"_hs,
    "!"_hs
};
const std::unordered_map<char, entt::id_type> HashedOperationsMap =
{
    {'&', "&"_hs},
    {'|', "|"_hs},
    {'!', "!"_hs}
};

drft::util::BooleanStringExpression::BooleanStringExpression(std::string expression)
{
    _parsedExpression = parseExpression(util::removeWhitespace(expression));
}

bool drft::util::BooleanStringExpression::evaluate(const TokenValues & values) const
{
    if (_parsedExpression.empty() && values.size() == 1)
    {
        return values.begin()->second;
    }
    // Validate input
    for (auto&& id : _parsedExpression)
    {
        if (HashedOperations.contains(id)) continue;

        if (!values.contains(id))
        {
            error_logger << "Error while evaluating expression: unknown ID of 1 or more values." << std::endl;
            return false;
        }
    }

    std::stack<bool> stack;
    for (auto&& token : _parsedExpression)
    {
        if (!HashedOperations.contains(token))
        {
            stack.push(values.at(token));
        }
        else
        {
            bool lhs = stack.top();
            stack.pop();
            bool rhs = stack.top();
            stack.pop();
            stack.push(BooleanExpressions.at(token)(lhs, rhs));
        }
    }

    return stack.top();
}

std::vector<entt::id_type> drft::util::BooleanStringExpression::parseExpression(std::string expression)
{
    std::stack<char> stack;
    std::vector<entt::id_type> output;
    bool errorFound = false;

    for (size_t i = 0; i < expression.size(); i++)
    {
        const char token = expression[i];
        if (isOperation(token))
        {
            if (token == '(')
            {
                stack.push(token);
            }
            else if (token == ')')
            {
                if (stack.empty())
                {
                    errorFound = true;
                    break;
                }
                while (stack.top() != '(')
                {
                    if (stack.empty()) 
                    {
                        errorFound = true; 
                        break;
                    }
                    output.emplace_back(HashedOperationsMap.at(stack.top()));
                    stack.pop();
                }
                if (stack.top() != '(') errorFound = true;
                stack.pop();
                while (!stack.empty())
                {
                    output.emplace_back(HashedOperationsMap.at(stack.top()));
                    stack.pop();
                }
            }
            else
            {
                while (!stack.empty() && isOperation(stack.top()) && stack.top() != '(')
                {
                    output.emplace_back(HashedOperationsMap.at(stack.top()));
                    stack.pop();
                }
                stack.push(token);
            }  
        }
        else
        {
            size_t indexOfFirstSpecialChar = expression.find_first_of(BooleanOperations, i);
            output.emplace_back(entt::hashed_string{ expression.substr(i, indexOfFirstSpecialChar - i).c_str()});
            i = indexOfFirstSpecialChar - 1;
        }
    }
    while (!stack.empty())
    {
        if (stack.top() == '(' || stack.top() == ')') errorFound = true;

        output.emplace_back(HashedOperationsMap.at(stack.top()));
        stack.pop();
    }

    if (errorFound)
    {
        error_logger << "Error when parsing malformed expression: " << expression << std::endl;
        return {};
    }
    return output;
}

bool drft::util::BooleanStringExpression::isOperation(char ch) const
{
    return BooleanOperations.find_first_of(ch) != StringEnd;
}


