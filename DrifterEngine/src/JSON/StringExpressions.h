#pragma once
#include <Utility/ContainerHelpers.h>
#include <Utility/stdHashing.h>

using TokenValues = std::unordered_map<entt::id_type, bool>;
template <typename T>
using TokenValueLists = std::unordered_map<entt::id_type, std::vector<T>>;


namespace drft::util
{
	class BooleanStringExpression
	{
	public:
		BooleanStringExpression() = default;
		BooleanStringExpression(std::string expression);
		bool evaluate(const TokenValues& values) const;
		template <typename T>
		std::vector<T> mergeLists(const TokenValueLists<T>& lists) const;
		const std::unordered_set<entt::id_type>& getTokens() const;

	private:
		std::vector<entt::id_type> parseExpression(std::string expression) const;
		bool isOperation(char ch) const;
		bool isOperation(entt::id_type token) const;

	private:
		std::vector<entt::id_type> _parsedExpression;
		std::unordered_set<entt::id_type> _tokens;
	};

	namespace details
	{
		template<typename T>
		const std::vector<T>& getList(entt::id_type id, const TokenValueLists<T>& list1, const TokenValueLists<T>& list2)
		{
			static const std::vector<T> EmptyList = {};
			if (list1.contains(id))
			{
				return list1.at(id);
			}
			else if (list2.contains(id))
			{
				return list2.at(id);
			}
			return EmptyList;
		}
	}

	template<typename T>
	inline std::vector<T> BooleanStringExpression::mergeLists(const TokenValueLists<T>& lists) const
	{
		using namespace entt::literals;
		if (lists.empty()) return {};

		if (_parsedExpression.size() == 1)
		{
			entt::id_type onlyToken = *_parsedExpression.begin();
			if (!lists.contains(onlyToken)) return {};
			
			return lists.at(onlyToken);
		}

		TokenValueLists<T> workingLists;
		std::stack<entt::id_type> stack;
		unsigned int incrementer = 0;
		for (auto&& token : _parsedExpression)
		{
			if (!isOperation(token))
			{
				stack.push(token);
			}
			else
			{
				entt::id_type lhs = stack.top();
				stack.pop();
				entt::id_type rhs = stack.top();
				stack.pop();

				std::vector<T> leftList = details::getList(lhs, lists, workingLists);
				std::vector<T> rightList = details::getList(rhs, lists, workingLists);

				if (token == "&"_hs)
				{
					workingLists.emplace(incrementer, vector_intersection(leftList, rightList));
				}
				else if (token == "|"_hs)
				{
					workingLists.emplace(incrementer, vector_union(leftList, rightList));
				}

				stack.push(incrementer);
				incrementer++;
			}
		}

		return workingLists.at(stack.top());
	}
}