#pragma once

using TokenValues = std::unordered_map<entt::id_type, bool>;

namespace drft::util
{
	class BooleanStringExpression
	{
	public:
		BooleanStringExpression() = default;
		BooleanStringExpression(std::string expression);
		bool evaluate(const TokenValues& values) const;

	private:
		std::vector<entt::id_type> parseExpression(std::string expression);
		bool isOperation(char ch) const;

	private:
		std::vector<entt::id_type> _parsedExpression;
	};
}