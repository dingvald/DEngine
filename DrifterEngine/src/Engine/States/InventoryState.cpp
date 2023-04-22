#include "pch.h"
#include "InventoryState.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Utility/ItemIDToEntityID.h"

drft::InventoryState::InventoryState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	
}

bool drft::InventoryState::handleEvent(const sf::Event& ev)
{
	_inventoryWindow.handleEvent(ev);

	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Escape)
		{
			requestStackPop();
			return false;
		}
		break;
	}

	return false;
}

bool drft::InventoryState::update(const float dt)
{
	_inventoryWindow.update(dt);

    return false;
}

void drft::InventoryState::render(sf::RenderTarget& target)
{
	_inventoryWindow.render(target);
}

void drft::InventoryState::onPush()
{
	determineSessionEntities();
	setupPanels();
}

void drft::InventoryState::onPop()
{
	shutdownSessionEntities();
}

void drft::InventoryState::setupPanels()
{
	const auto& VIEW = getContext().window.getView();

	_inventoryWindow.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,200)
			})
		.setChildrenAlignment(gui::ElementAlignment::CENTER)
		.insertChild("ItemList", gui::List());

	_inventoryWindow["ItemList"]
		.setSize({ VIEW.getSize().x / 3, 3 * (VIEW.getSize().y / 4) })
		.setPosition({ 0,0 })
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100),
			.outlineColor = sf::Color(255,255,255),
			.outlineThickness = 1.0f,
			.innerPadding = 32.0f,
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White,
			.textSize = 16
			})
		.setTextString(util::getEntityName({ getContext().registry, _sessionEntities.front() }) + "'s Inventory")
		.setTextOrigin(gui::ElementOrigin::BOTTOM_CENTER)
		.setTextPosition(gui::ElementTextPosition::TOP_CENTER)
		.setChildrenAlignment(gui::ElementAlignment::TOP_CENTER, {-32, 0});

	auto& container = getContext().registry.get<component::Container>(_sessionEntities.front());
	int count = 0;
	for (auto item : container.contents)
	{
		const auto itemEntity = util::ItemIDToEntityID(item, getContext().registry);
		const auto itemRender = getContext().registry.get<component::Render>(itemEntity);
		const auto& texture = getContext().textures.get("Sprites");
		const auto textureRect = util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), texture);

		_inventoryWindow["ItemList"]
			.insertChild(std::to_string(count), gui::Button());

		auto& itemText = _inventoryWindow["ItemList"][std::to_string(count)];
		itemText.setSize({ 48,16 })
			.setOrigin(gui::ElementOrigin::CENTER_LEFT)
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
			.setTextString(util::getEntityName({ getContext().registry, itemEntity }))
			.setTextPosition(gui::ElementTextPosition::CENTER_RIGHT)
			.setTextOrigin(gui::ElementOrigin::CENTER_LEFT);
		itemText.setChildrenAlignment(gui::ElementAlignment::CENTER, { -32, 0 });
		itemText.insertChild("Icon", gui::Icon(sf::Sprite(texture, textureRect)));

		auto& icon = _inventoryWindow["ItemList"][std::to_string(count)]["Icon"];
		icon.setSize({ 16.f, 16.f })
			.setStyle(gui::ElementState::Idle, {
				.fillColor = itemRender.color
				})
			.setOrigin(gui::ElementOrigin::CENTER);

		++count;
	}
}

void drft::InventoryState::determineSessionEntities()
{
	auto view = getContext().registry.view<component::action::OpenInventory>();
	for (auto entity : view)
	{
		_sessionEntities.push_back(entity);
	}
	assert(_sessionEntities.size() <= 2);
}

void drft::InventoryState::shutdownSessionEntities()
{
	for (auto entity : _sessionEntities)
	{
		getContext().registry.remove<component::action::OpenInventory>(entity);
	}
}
