#pragma once

namespace drft::util
{
	enum class Sprite : unsigned int
	{
		None,
		Human,
		Dog,
		Dot,
		Square,
		Tree,
		Grass,
		Blood,
		Hit_1,
		Hit_2,
		Rock,
		Zombie,
		Axe,
		Sword,
		Material,
		Heart,
		Total
	};

	class SpriteIndexer
	{
	public:
		static sf::IntRect get(unsigned int index, const sf::Texture& texture);
	};
}



