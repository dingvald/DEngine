#include "pch.h"
#include "WorldMapState.h"
#include "Components/Components.h"
#include "Systems/Helpers/GetCurrentCamera.h"
#include "Spatial/Conversions.h"
#include "Spatial/Helpers.h"
#include "WorldMap/WorldMap.h"
#include "Biomes/Biome.h"

static constexpr float FLASH_RATE = 1.0;
static constexpr std::string_view SAVE_DIRECTORY = ".\\data\\savegame\\";

const std::vector<sf::Vector2i> IconUVs =
{
	{3, 7}, // House
	{4, 7}, // Star
	{5, 7}, // Dungeon
	{0, 7}, // Chest
};

using namespace entt::literals;

drft::WorldMapState::WorldMapState(StateStack& stack, StateContext& context)
	: State(stack, context)
{
	const auto& VIEW = getContext().window.getView();

	_mapBackground.setSize(VIEW.getSize());
	_mapBackground.setPosition(VIEW.getCenter());
	_mapBackground.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,200)
		});

	_map.setTexture(getContext().textures.getTexture());
	_mapNotes.noteSprites.setTexture(getContext().textures.getTexture());
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
		if (ev.key.code == sf::Keyboard::Numpad1)
		{
			moveCursor({ -1, 1 });
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad7)
		{
			moveCursor({ -1, -1 });
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad9)
		{
			moveCursor({ 1, -1 });
			return false;
		}
		if (ev.key.code == sf::Keyboard::Numpad3)
		{
			moveCursor({ 1, 1 });
			return false;
		}
		if (ev.key.code == sf::Keyboard::Space)
		{
			openOnSelectMenu();
			return false;
		}
		if (ev.key.code == sf::Keyboard::Subtract) {
			_scalingFactor += 0.1;
			_scalingFactor = std::min(_scalingFactor, 2.0f);
		}
		if (ev.key.code == sf::Keyboard::Add) {
			_scalingFactor -= 0.1;
			_scalingFactor = std::max(_scalingFactor, 0.5f);
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
	applyScaling();

	return false;
}

void drft::WorldMapState::render(sf::RenderTarget& target)
{
	const auto& worldMap = getContext().registry.ctx().get<const WorldMap&>();
	auto worldMapDimensions = spatial::toFloatSpace(worldMap.getDimensions());
	auto defaultViewSize = target.getView().getSize();
	sf::View view;

	view.setSize(target.getView().getSize());

	sf::Vector2f viewPosition = _cursor.getPosition();
	float edgeOffsetX = (worldMapDimensions.x - defaultViewSize.x) * 0.5f * _scalingFactor;
	float edgeOffsetY = (worldMapDimensions.y - defaultViewSize.y) * 0.5f * _scalingFactor;

	viewPosition.x = std::clamp(viewPosition.x, edgeOffsetX, worldMapDimensions.x - edgeOffsetX);
	viewPosition.y = std::clamp(viewPosition.y, edgeOffsetY, worldMapDimensions.y - edgeOffsetY);

	view.setCenter(viewPosition);
	view.zoom(_scalingFactor);
	
	target.setView(view);

	_mapBackground.render(target);
	target.draw(_map);
	target.draw(_currentPositionTile);
	target.draw(_cursor);
	if (_drawNotes)
	{
		target.draw(_mapNotes.noteSprites);
	}
	_guiStack.render(target);
	target.setView(target.getDefaultView());
}

void drft::WorldMapState::onPush()
{
	const auto& VIEW = getContext().window.getView();
	
	loadMapNotes();

	auto cameraInfo = system::getCurrentCamera(getContext().registry);
	_currentPosition.fromTileSpace(cameraInfo.position);

	refreshMapSprites();

	_currentPositionTile.setSize({ spatial::TILE_WIDTH, spatial::TILE_HEIGHT });
	_currentPositionTile.setPosition(spatial::toFloatSpace(sf::Vector2i{ _currentPosition.x, _currentPosition.y }));
	_currentPositionTile.setFillColor(sf::Color::White);

	_cursor.setSize({ spatial::TILE_WIDTH, spatial::TILE_HEIGHT });
	_cursor.setPosition(spatial::toFloatSpace(sf::Vector2i{ _currentPosition.x, _currentPosition.y }));
	_cursor.setOutlineThickness(1.0f);
	_cursor.setOutlineColor(sf::Color::Yellow);
	_cursor.setFillColor(sf::Color(0, 0, 0, 0));
	_cursorPosition = _currentPosition;
}

void drft::WorldMapState::onPop()
{
	if (_mapNotesDirty)
	{
		saveMapNotes();
	}
	getContext().registry.clear<component::action::OpenWorldMap>();
}

void drft::WorldMapState::refreshMapSprites()
{
	_map.clear();
	_mapNotes.noteSprites.clear();

	const auto& VIEW = getContext().window.getView();
	const auto& textureAtlas = getContext().textures;
	const auto& worldMap = getContext().registry.ctx().get<const WorldMap&>();

	const sf::IntRect squareUV = textureAtlas.getUV("simple_tileset"_hs, { 16, 16 }, { 4, 0 });

	const auto dimensions = worldMap.getDimensions();

	for (int y = 0; y < dimensions.y; ++y)
	{
		for (int x = 0; x < dimensions.x; ++x)
		{
			const auto worldMapPosition = WorldMapPosition{ x, y };
			const Biome* biome = worldMap.getBiome(worldMapPosition);
			if (!biome) continue;

			const auto& icon = biome->getIcon();
			sf::Vector2f screenPosition = spatial::toFloatSpace({x, y});

			_map.addSprite(squareUV, sf::Color::Black, screenPosition);
			const auto uv = getContext().textures.getUV(icon.texture, icon.uvSize, icon.uvCoords);
			_map.addSprite(uv, icon.color, screenPosition);

			if (_mapNotes.notes.contains({x,y}))
			{
				sf::Vector2i localUV = IconUVs.at(_mapNotes.notes.at({ x, y }).index);
				sf::IntRect iconUV = textureAtlas.getUV("simple_tileset"_hs, { 16, 16 }, localUV);
				_mapNotes.noteSprites.addSprite(iconUV, _mapNotes.notes.at({x,y}).color, screenPosition);
			}
		}
	}
}

void drft::WorldMapState::addMapNote(drft::WorldMapPosition position, size_t iconIndex, sf::Color color)
{
	const auto& textureAtlas = getContext().textures;
	_mapNotes.notes[position] = { iconIndex, color };

	const auto& VIEW = getContext().window.getView();
	sf::Vector2f screenPosition = _currentPosition.toFloatSpace();
	sf::IntRect uv = textureAtlas.getUV("simple_tileset"_hs, { 16, 16 }, IconUVs.at(iconIndex));
	_mapNotes.noteSprites.addSprite(uv, color, screenPosition);
	_mapNotesDirty = true;
}

void drft::WorldMapState::moveCursor(sf::Vector2i direction)
{
	const auto& worldMap = getContext().registry.ctx().get<const WorldMap&>();
	const auto& VIEW = getContext().window.getView();
	sf::Vector2i intended = sf::Vector2i{ _cursorPosition.x, _cursorPosition.y } + direction;
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

	const auto& textureAtlas = getContext().textures;

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

	for (size_t i = 0; i < IconUVs.size(); ++i)
	{
		sf::Sprite sprite = textureAtlas.getSprite("simple_tileset"_hs, { 16, 16 }, IconUVs[i]);
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
				openColorSelection(index);
				return true;
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

void drft::WorldMapState::openColorSelection(size_t index)
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
			.registerCallback(gui::ElementCallbackType::OnSelect, [this, color, index]() -> bool {
				addMapNote(_cursorPosition, index, color);
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

void drft::WorldMapState::applyScaling()
{

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
