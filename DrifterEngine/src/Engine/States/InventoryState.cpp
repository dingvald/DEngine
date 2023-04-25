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
	_itemInfoWindow.handleEvent(ev);

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
	_itemInfoWindow.update(dt);

    return false;
}

void drft::InventoryState::render(sf::RenderTarget& target)
{
	_inventoryWindow.render(target);
	_itemInfoWindow.render(target);
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

	_inventoryWindow
		.setSize(VIEW.getSize())
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,150)
			})
		.setChildrenOrigin(gui::ElementAlignment::CENTER)
		.insertChild("ItemGrid", gui::Grid(6, 8))
		.insertChild("ItemInfo", gui::Window());

	_inventoryWindow["ItemGrid"]
		.setPosition({ 0,0 })
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100),
			.outlineColor = sf::Color(255,255,255),
			.outlineThickness = 1.0f,
			.innerPadding = 32.0f,
			.childPadding = 8.f,
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White,
			.textSize = 16
			})
		.setTextString(util::getEntityName({ getContext().registry, _sessionEntities.front() }) + "'s Inventory")
		.setTextOrigin(gui::ElementOrigin::BOTTOM_CENTER)
		.setTextPosition(gui::ElementTextPosition::TOP_CENTER)
		.setChildrenOrigin(gui::ElementAlignment::TOP_LEFT);

	

	auto& container = getContext().registry.get<component::Container>(_sessionEntities.front());
	int count = 0;
	for (auto item : container.contents)
	{
		const auto itemEntity = util::ItemIDToEntityID(item, getContext().registry);
		const auto itemRender = getContext().registry.get<component::Render>(itemEntity);
		const auto& texture = getContext().textures.get("Sprites");
		const auto textureRect = util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), texture);

		_inventoryWindow["ItemGrid"]
			.insertChild(std::to_string(count), gui::Window());

		auto& iconContainer = _inventoryWindow["ItemGrid"][std::to_string(count)];
		iconContainer.setSize({ 32.f,32.f })
			.setStyle(gui::ElementState::Idle, {
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color(50,50,50),
				.outlineThickness = 1.f,
				})
			.setStyle(gui::ElementState::Focused, {
						.fillColor = sf::Color(30, 30, 10, 150),
						.outlineColor = sf::Color::Yellow,
						.outlineThickness = 1.f,
					})
			.setStyle(gui::ElementState::Active, {
						.fillColor = sf::Color(10, 10, 10, 150),
						.outlineColor = sf::Color::Yellow,
						.outlineThickness = 1.f,
						})
			.setOrigin(gui::ElementOrigin::CENTER)
			.setChildrenOrigin(gui::ElementAlignment::CENTER)
			.insertChild("Icon", gui::Icon(sf::Sprite(texture, textureRect)))
			.registerCallback(gui::ElementCallbackType::OnFocus, [this, itemEntity]() -> bool
				{
					return true;
				});

			
		auto& icon = iconContainer["Icon"];
		icon.setSize({ 32.f, 32.f })
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
