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
		PaperDollHead,
		PaperDollBody,
		PaperDollHandR,
		PaperDollHandL,
		PaperDollLegs,
		PaperDollFeet,
		PaperDollHands,
		Boulder,
		Sand,
		Water,
		Cactus,
		Triangle
	};

	class SpriteIndexer
	{
	public:
		static sf::IntRect get(Sprite sprite, const sf::Texture& texture);
	};
}



