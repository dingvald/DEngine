#include "pch.h"
#include "InventoryState.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Utility/ItemIDToEntityID.h"

static constexpr int INVENTORY_WIDTH = 6;
static constexpr int INVENTORY_HEIGHT = 8;

drft::InventoryState::InventoryState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	
}

bool drft::InventoryState::handleEvent(const sf::Event& ev)
{
	_inventoryPanel.handleEvent(ev);
	_inventoryGrid.handleEvent(ev);

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
	_inventoryPanel.update(dt);
	_inventoryGrid.update(dt);

    return false;
}

void drft::InventoryState::render(sf::RenderTarget& target)
{
	_inventoryPanel.render(target);
	_inventoryGrid.render(target);
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

	_inventoryPanel.setSize(VIEW.getSize());
	_inventoryPanel.setPosition(VIEW.getCenter());
	_inventoryPanel.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100)
			});

	_inventoryGrid = gui::Grid(6, 8);
	_inventoryGrid.setPosition(VIEW.getCenter());
	_inventoryGrid.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,150),
		.outlineColor = sf::Color(255,255,255,150),
		.outlineThickness = 1.f,
		.innerPadding = 16.f,
		.childPadding = 8.f,
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	_inventoryGrid.setTextString(util::getEntityName({ getContext().registry, _sessionEntities.front() }) + "'s Inventory");
	_inventoryGrid.setTextPosition(gui::ElementPosition::TOP_CENTER);
	_inventoryGrid.setTextOrigin(gui::ElementPosition::BOTTOM_CENTER);

	auto& entityContainer = getContext().registry.get<component::Container>(_sessionEntities.front());
	_inventoryGrid.registerCallback(gui::ElementCallbackType::OnUpdate, [this, &entityContainer]() -> bool
		{
			int count = 0;
			for (auto& item : entityContainer.contents)
			{
				auto itemEntity = util::ItemIDToEntityID(item, getContext().registry);
				auto& itemRender = getContext().registry.get<component::Render>(itemEntity);
				auto& sprites = getContext().textures.get("Sprites");
				auto& container = dynamic_cast<gui::Container&>(this->_inventoryGrid[count]);
				sf::Sprite sprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), sprites) };
				container.remove();
				container.insert(gui::Icon(sprite) )
					.setSize({ 32,32 })
					.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
					.setStyle(gui::ElementState::Idle,
						{
							.fillColor = itemRender.color
						});
				container.layoutChildren();
				++count;
			}
			return true;
		});

	for (int row = 0; row < INVENTORY_HEIGHT; ++row)
	{
		for (int col = 0; col < INVENTORY_WIDTH; ++col)
		{
			auto& container = _inventoryGrid.insert(gui::SingleContainer());

			container.setSize({ 32, 32 });
			container.setOrigin(gui::ElementPosition::TOP_LEFT);
			container.setChildrenOrigin(gui::ElementPosition::CENTER);
			container.setStyle(gui::ElementState::Idle, {
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color(255,255,255,100),
				.outlineThickness = 1.f
				});
			container.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color(40,40,0,150),
				.outlineColor = sf::Color::Yellow,
				.outlineThickness = 1.f
				});
			container.setStyle(gui::ElementState::Active, {
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color::Red,
				.outlineThickness = 1.f
				});
		}
	}

	_inventoryGrid.layoutChildren();
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
