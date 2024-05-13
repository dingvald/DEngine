#pragma once

namespace drft
{
	namespace gen
	{
		struct GenerationContext;
	}
	class StructureBaseShape;
	class StructureInstance
	{
	public:
		StructureInstance(drft::LayoutPtr layout);
		void stamp(sf::Vector2i tileOrigin, const gen::GenerationContext& context, entt::registry& registry) const;

	private:
		sf::Vector2i _dimensions;
		drft::LayoutPtr _layout;
	};
}


