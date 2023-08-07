#include "pch.h"
#include "WorldMapState.h"
#include "Components/Components.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "WorldMap/WorldMap.h"

static constexpr float FLASH_RATE = 1.0;
static constexpr std::string_view SAVE_DIRECTORY = ".\\data\\savegame\\";

drft::WorldMapState::WorldMapState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	const auto& VIEW = getContext().window.getView();

	_mapBackground.setSize(VIEW.getSize());
	_mapBackground.setPosition(VIEW.getCenter());
	_mapBackground.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,200)
		});

	_map.setTexture(getContext().textures.get("Sprites"));
	_mapNotes.noteSprites.setTexture(getContext().textures.get("Sprites"));
}

bool drft::WorldMapState::handleEvent(const sf::Event& ev)
{
	if (!_guiStack.isEmpty())
	{
		_guiStack.handleEvent(ev);
		return false;
	}
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad8 || ev.key.code == sf::Keyboard::Up)
		{
			moveCursor({ 0,-1 });
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad2 || ev.key.code == sf::Keyboard::Down)
		{
			moveCursor({ 0,1 });
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad6 || ev.key.code == sf::Keyboard::Right)
		{
			moveCursor({ 1,0 });
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad4 || ev.key.code == sf::Keyboard::Left)
		{
			moveCursor({ -1,0 });
			return false;
		}
		if (ev.key.code == sf::Keyboard::Space)
		{
			openOnSelectMenu();
			return false;
		}
		break;
	}

	return false;
}

bool drft::WorldMapState::update(const float dt)
{
	_guiStack.update(dt);

	pulseCurrentPositionTile(dt);
	pulseCursor(dt);
	pulseMapNotes(dt);

	return false;
}

void drft::WorldMapState::render(sf::RenderTarget& target)
{
	_mapBackground.render(target);
	target.draw(_map);
	target.draw(_currentPositionTile);
	target.draw(_cursor);
	if (_drawNotes)
	{
		target.draw(_mapNotes.noteSprites);
	}
	_guiStack.render(target);
}

void drft::WorldMapState::onPush()
{
	const auto& VIEW = getContext().window.getView();
	
	loadMapNotes();

	auto view = getContext().registry.view<component::Camera, component::Position>();
	for (auto [entity, camera, pos] : view.each())
	{
		_currentPosition = spatial::toChunkCoordinate(pos.position);
	}

	refreshMapSprites();

	_currentPositionTile.setSize({ spatial::TILE_WIDTH, spatial::TILE_HEIGHT });
	_currentPositionTile.setPosition(spatial::toWorldSpace(_currentPosition));
	_currentPositionTile.setFillColor(sf::Color::White);

	_cursor.setSize({ spatial::TILE_WIDTH, spatial::TILE_HEIGHT });
	_cursor.setPosition(spatial::toWorldSpace(_currentPosition));
	_cursor.setOutlineThickness(1.0f);
	_cursor.setOutlineColor(sf::Color::Yellow);
	_cursor.setFillColor(sf::Color(0, 0, 0, 0));
	_cursorPosition = _currentPosition;
}

void drft::WorldMapState::onPop()
{
	saveMapNotes();

	getContext().registry.clear<component::action::OpenWorldMap>();
}

void drft::WorldMapState::refreshMapSprites()
{
	_map.clear();
	_mapNotes.noteSprites.clear();

	const auto& VIEW = getContext().window.getView();
	
	const auto& worldMap = getContext().registry.ctx().get<const WorldMap&>();
	for (int y = 0; y < worldMap.getDimensions().y; ++y)
	{
		for (int x = 0; x < worldMap.getDimensions().x; ++x)
		{
			sf::Vector2f screenPosition = spatial::toWorldSpace(sf::Vector2i(x,y));
			_map.addSprite(static_cast<unsigned int>(util::Sprite::Square),
				sf::Color::Black, screenPosition);
			_map.addSprite(static_cast<unsigned int>(util::Sprite::Dot),
				sf::Color::Magenta, screenPosition);

			if (_mapNotes.notes.contains({x,y}))
			{
				_mapNotes.noteSprites.addSprite(static_cast<unsigned int>(_mapNotes.notes.at({x,y}).icon),
					_mapNotes.notes.at({x,y}).color, screenPosition);
			}
		}
	}
}

void drft::WorldMapState::addMapNote(sf::Vector2i position, util::Sprite sprite, sf::Color color)
{
	_mapNotes.notes[position] = { sprite, color };

	const auto& VIEW = getContext().window.getView();
	sf::Vector2f screenPosition = spatial::toWorldSpace(_cursorPosition);
	_mapNotes.noteSprites.addSprite(static_cast<unsigned int>(_mapNotes.notes.at(position).icon),
		_mapNotes.notes.at(position).color, screenPosition);
}

void drft::WorldMapState::moveCursor(sf::Vector2i direction)
{
	const auto& worldMap = getContext().registry.ctx().get<const WorldMap&>();
	sf::Vector2i intended = _cursorPosition + direction;
	if (intended.x < 0 || intended.y < 0 || intended.x >= worldMap.getDimensions().x || intended.y >= worldMap.getDimensions().y)
	{
		return;
	}
	
	_cursorPosition += direction;
	_cursor.move({static_cast<float>(direction.x * spatial::TILE_HEIGHT),
		static_cast<float>(direction.y * spatial::TILE_WIDTH)});
}

void drft::WorldMapState::openOnSelectMenu()
{
	const auto& VIEW = getContext().window.getView();
	auto& commandList = _guiStack.insert("CommandList", gui::List(true))
		.setPosition(_cursor.getPosition() + sf::Vector2f(32, 0))
		.setSize({ 64,128 })
		.setStyle(gui::ElementState::Focused, {
			.fillColor = sf::Color(0,0,0,255),
			.outlineColor = sf::Color(255,255,255,100),
			.outlineThickness = 1.f,
			.innerPadding = {4.f, 4.f},
			.childPadding = {2.f, 2.f}
			})
		.setOrigin(gui::ElementPosition::TOP_LEFT)
		.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);

	commandList.insert("Add Note", gui::Label())
		.setStyle(gui::ElementState::Idle, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White,
			.textSize = 16
			})
		.setStyle(gui::ElementState::Focused, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::Yellow,
			.textSize = 16
			})
		.setStyle(gui::ElementState::Unselectable, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color(100,100,100),
			.textSize = 16
			})
		.setStyle(gui::ElementState::FocusedUnselectable, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color(150,150,150),
			.textSize = 16
			})
		.setTextString("Add Note")
		.setTextOrigin(gui::ElementPosition::TOP_LEFT)
		.registerCallback(gui::ElementCallbackType::OnIsSelectable, [this]() -> bool {
			return !_mapNotes.notes.contains(_cursorPosition);
			})
		.registerCallback(gui::ElementCallbackType::OnSelect, [this]() -> bool {
				openIconSelection();
				return true;
			});

	commandList.insert("Remove Note", gui::Label())
		.setStyle(gui::ElementState::Idle, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White,
			.textSize = 16
			})
		.setStyle(gui::ElementState::Focused, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::Yellow,
			.textSize = 16
			})
		.setStyle(gui::ElementState::Unselectable, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color(100,100,100),
			.textSize = 16
			})
		.setStyle(gui::ElementState::FocusedUnselectable, {
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color(150,150,150),
			.textSize = 16
			})
		.setTextString("Remove Note")
		.setTextOrigin(gui::ElementPosition::TOP_LEFT)
		.registerCallback(gui::ElementCallbackType::OnIsSelectable, [this]() -> bool {
			return _mapNotes.notes.contains(_cursorPosition);
			})
		.registerCallback(gui::ElementCallbackType::OnSelect, [this]() -> bool {
				_mapNotes.notes.erase(_cursorPosition);
				refreshMapSprites();
				return true;
			});
}

void drft::WorldMapState::openIconSelection()
{
	constexpr int ICON_GRID_WIDTH = 4;
	constexpr int ICON_GRID_HEIGHT = 4;

	const auto& sprites = getContext().textures.get("Sprites");

	const auto& VIEW = getContext().window.getView();
	auto& iconGrid = _guiStack.insert("Icons", gui::Grid(ICON_GRID_WIDTH, ICON_GRID_HEIGHT))
		.setPosition(_cursor.getPosition() + sf::Vector2f(32, 0))
		.setOrigin(gui::ElementPosition::CENTER_LEFT)
		.setStyle(gui::ElementState::Focused, {
			.fillColor = sf::Color(0,0,0,220),
			.outlineColor = sf::Color(150,150,150,100),
			.outlineThickness = 1.f,
			.innerPadding = {8.f, 8.f},
			.childPadding = {2.f, 2.f},
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White
			})
		.setTextString("Select Icon")
		.setTextPosition(gui::ElementPosition::TOP_CENTER)
		.setTextOrigin(gui::ElementPosition::BOTTOM_CENTER)
		.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);

	for (int i = 0; i < IconSprites.size(); ++i)
	{
		sf::Sprite sprite;
		sprite.setTexture(sprites);
		sprite.setTextureRect(util::SpriteIndexer::get(IconSprites.at(i), sprites));
		sprite.setColor(sf::Color::White);

		iconGrid.insert(std::to_string(i), gui::SingleContainer())
			.setSize({ 16,16 })
			.setOrigin(gui::ElementPosition::TOP_LEFT)
			.setStyle(gui::ElementState::Idle, {
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color(150,150,150,100),
				.outlineThickness = 1.f
				})
			.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color(40,40,0,150),
				.outlineColor = sf::Color::Yellow,
				.outlineThickness = 1.f,
				})
			.setChildrenOrigin(gui::ElementPosition::CENTER)
			.registerCallback(gui::ElementCallbackType::OnSelect, [this, index = i]() -> bool {
				openColorSelection(IconSprites.at(index));
				})
			.insert("Icon", gui::Icon(sprite))
				.setSize({ 16,16 })
				.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
				.setStyle(gui::ElementState::Idle, {
						.fillColor = sf::Color::White
					})
				.setStyle(gui::ElementState::Focused, {
						.fillColor = sf::Color::White
					});
	}
}

void drft::WorldMapState::openColorSelection(util::Sprite sprite)
{
	const auto& VIEW = getContext().window.getView();
	_guiStack.insert("Colors", gui::List(true))
		.setPosition(_cursor.getPosition() + sf::Vector2f(32, 0))
		.setOrigin(gui::ElementPosition::CENTER_LEFT)
		.setStyle(gui::ElementState::Focused, {
			.fillColor = sf::Color(0,0,0,255),
			.outlineColor = sf::Color(255,255,255,100),
			.outlineThickness = 1.f,
			.innerPadding = {4.f, 4.f},
			.childPadding = {2.f, 2.f},
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White
			})
		.setTextString("Select Color")
		.setTextPosition(gui::ElementPosition::TOP_CENTER)
		.setTextOrigin(gui::ElementPosition::BOTTOM_CENTER)
		.setChildrenOrigin(gui::ElementPosition::TOP_CENTER, {-4, 8});

	for (auto& [colorName, color] : IconColors)
	{
		_guiStack["Colors"].insert(std::string{ colorName }, gui::Label())
			.setStyle(gui::ElementState::Idle, {
				.font = &getContext().fonts.get("Terminus"),
				.textColor = color,
				.textSize = 16
				})
			.setStyle(gui::ElementState::Focused, {
				.outlineColor = sf::Color::Yellow,
				.outlineThickness = 1.f,
				.font = &getContext().fonts.get("Terminus"),
				.textColor = color,
				.textSize = 16
				})
			.setTextString(std::string{ colorName })
			.setTextOrigin(gui::ElementPosition::CENTER)
			.registerCallback(gui::ElementCallbackType::OnSelect, [this, color, sprite]() -> bool {
				addMapNote(_cursorPosition, sprite, color);
				_guiStack.clear();
				return true;
				});
	}
}

void drft::WorldMapState::pulseCurrentPositionTile(float dt)
{
	const float twoPI = 2.0f * 3.14152f;
	const float f = 0.5f;
	static float elapsed = 0.0f;
	elapsed += dt;
	if (elapsed > 10.0)
	{
		elapsed = 0.0;
	}
	float alpha = 255 * ((std::sinf(twoPI * f * elapsed) + 1.f) / 2.f);
	auto col = _currentPositionTile.getFillColor();
	_currentPositionTile.setFillColor(sf::Color(col.r, col.g, col.b, static_cast<sf::Uint8>(alpha)));
}

void drft::WorldMapState::pulseCursor(float dt)
{
	static float elapsed = 0.0f;
	elapsed += dt;
	if (elapsed > 0.5f)
	{
		_cursor.setOutlineThickness(-1.f * _cursor.getOutlineThickness());
		elapsed = 0.0;
	}

}

void drft::WorldMapState::pulseMapNotes(float dt)
{
	static float elapsed = 0.0f;
	elapsed += dt;
	if (elapsed > 0.75f)
	{
		_drawNotes = !_drawNotes;
		elapsed = 0.0;
	}
}

void drft::WorldMapState::saveMapNotes()
{
	std::ofstream file(std::string(SAVE_DIRECTORY.data()) + "mapnotes.json");
	cereal::JSONOutputArchive archive(file);

	archive(_mapNotes);
}

void drft::WorldMapState::loadMapNotes()
{
	std::string fullPath = std::string(SAVE_DIRECTORY.data()) + "mapnotes.json";
	if (std::filesystem::exists(fullPath))
	{
		std::ifstream file(fullPath);
		cereal::JSONInputArchive archive(file);

		archive(_mapNotes);
	}
}
