#pragma once
#include "Structures/StructureComponent.h"
#include "Utility/Math.h"

namespace drft
{
	class SquareComponent : public StructureComponent
	{
	public:
		virtual void createFromJSON(const rapidjson::Value& json) override;
		virtual void apply(std::unordered_map<entt::entity, PositionList>& layout) const override;

	private:
		sf::Vector2i _origin;
		math::Range<int> _width;
		math::Range<int> _height;
		float _outlineIntegrity = 1.0f;
		float _fillIntegrity = 1.0f;
		std::vector<std::pair<std::string, int>> _outlineEntityWeights;
		std::vector<std::pair<std::string, int>> _fillEntityWeights;
	};
}


