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
		BirdMan,
		Axe,
		Machete,
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
		Triangle,
		JungleTree,
		Roots,
		Raft,
		PlantFibre,
		Twine,
		ZolGibbon,
		Mosquito,
		Alligator,
		Snake,
		Brambles,
		Flower,
		Herb,
		Poultice,
		Firefly,
		OreDeposit,
		Ore,
		Skeleton,
		Torch,
		Barricade,
		Ash,
		InLiquidEffect,
		Butterfly,
		Diamond,
		StatusEffect,
		Ephemera,
		X,
		DungeonKey,
		Rat,
		Vulture,
		Cougar,
		Golem,
		Breastplate,
		Hood,
		TatteredVest,
		Legplates,
		Sandals,
		Bracers,
		Shield,
		ShortSword,
		DoorClosed,
		DoorOpen,
		Katana,
		ChestClosed,
		ChestOpen,
		HeartShrine
	};

	class SpriteIndexer
	{
	public:
		static sf::IntRect get(Sprite sprite, const sf::Texture& texture);
	};
}



