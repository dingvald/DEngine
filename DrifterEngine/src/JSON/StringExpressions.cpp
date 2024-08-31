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
    std::stack<char> operator_stack;
    std::vector<entt::id_type> output;
    bool error_found = false;

    for (size_t i = 0; i < expression.size(); i++)
    {
        const char token = expression[i];
        if (isOperation(token))
        {
            if (token == '(')
            {
                operator_stack.push(token);
            }
            else if (token == ')')
            {
                while (!operator_stack.empty() && operator_stack.top() != '(')
                {
                    output.emplace_back(HashedOperationsMap.at(operator_stack.top()));
                    operator_stack.pop();
                }
                if (operator_stack.empty())
                {
                    error_found = true;
                    break;
                }
                if (operator_stack.top() != '(') error_found = true;
                operator_stack.pop();
                while (!operator_stack.empty())
                {
                    output.emplace_back(HashedOperationsMap.at(operator_stack.top()));
                    operator_stack.pop();
                }
            }
            else
            {
                while (!operator_stack.empty() && isOperation(operator_stack.top()) && operator_stack.top() != '(')
                {
                    output.emplace_back(HashedOperationsMap.at(operator_stack.top()));
                    operator_stack.pop();
                }
                operator_stack.push(token);
            }  
        }
        else
        {
            size_t indexOfFirstSpecialChar = expression.find_first_of(BooleanOperations, i);
            output.emplace_back(entt::hashed_string{ expression.substr(i, indexOfFirstSpecialChar - i).c_str()});
            i = indexOfFirstSpecialChar - 1;
        }
    }
    while (!operator_stack.empty())
    {
        if (operator_stack.top() == '(' || operator_stack.top() == ')') error_found = true;

        output.emplace_back(HashedOperationsMap.at(operator_stack.top()));
        operator_stack.pop();
    }

    if (error_found)
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


