#pragma once
#include <Engine/States/State.h>
#include "GUI/GUIElement.h"
#include "Utility/SpriteBatch.h"
#include "Utility/SpriteIndexer.h"
#include "Utility/stdHashing.h"
#include "Spatial/WorldMapPosition.h"



namespace drft
{
	struct MapNote
	{
		size_t index;
		sf::Color color = sf::Color::Magenta;
	};

	const std::unordered_map<std::string, sf::Color> IconColors =
	{
		{"RED", sf::Color::Red},
		{"BLUE", sf::Color::Blue},
		{"GREEN", sf::Color::Green},
		{"YELLOW", sf::Color::Yellow},
		{"ORANGE", sf::Color(255,165,0)},
		{"CYAN", sf::Color::Cyan},
		{"MAGENTA", sf::Color::Magenta},
		{"GREY", sf::Color(125,125,125)}
	};

	struct MapNotes
	{
		std::unordered_map<drft::WorldMapPosition, MapNote> notes;
		SpriteBatch noteSprites;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(notes);
		}
	};

	class WorldMapState : public State
	{
	public:
		WorldMapState(StateStack& stack, StateContext& context);

		bool handleEvent(const sf::Event& ev) override;
		bool update(const float dt) override;
		void render(sf::RenderTarget& target) override;
		void onPush() override;
		void onPop() override;

	private:
		void refreshMapSprites();
		void addMapNote(drft::WorldMapPosition position, size_t iconIndex, sf::Color color);
		void moveCursor(sf::Vector2i direction);
		void openOnSelectMenu();
		void openIconSelection();
		void openColorSelection(size_t iconIndex);
		void pulseCurrentPositionTile(float dt);
		void pulseCursor(float dt);
		void pulseMapNotes(float dt);
		void applyScaling();

		void saveMapNotes();
		void loadMapNotes();

	private:
		std::vector<sf::Vector2i> _surroundings;
		WorldMapPosition _currentPosition;
		WorldMapPosition _cursorPosition;
		gui::Panel _mapBackground;
		gui::Stack _guiStack;
		sf::RectangleShape _currentPositionTile;
		sf::RectangleShape _cursor;
		SpriteBatch _map;
		MapNotes _mapNotes;
		float _scalingFactor = 0.5f;
		bool _drawNotes = true;
		bool _mapNotesDirty = false;
	};
}

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, sf::Vector2i& vec2)
	{
		archive(vec2.x, vec2.y);
	}

	template<class Archive>
	void serialize(Archive& archive, drft::MapNote& mapNote)
	{
		archive(mapNote.index, mapNote.color.r, mapNote.color.g, mapNote.color.b);
	}
}

