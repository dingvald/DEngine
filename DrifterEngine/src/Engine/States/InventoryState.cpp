#include "pch.h"
#include "InventoryState.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Utility/ItemIDToEntityID.h"

static constexpr int INVENTORY_WIDTH = 5;
static constexpr int INVENTORY_HEIGHT = 6;

drft::InventoryState::InventoryState(StateStack& stack, StateContext& context)
    : State(stack, context)
{}

bool drft::InventoryState::handleEvent(const sf::Event& ev)
{
	if (!_inventoryBackground.handleEvent(ev)) return false;
	if (!_inventoryStack.handleEvent(ev)) return false;

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
	_inventoryBackground.update(dt);
	_inventoryStack.update(dt);

    return false;
}

void drft::InventoryState::render(sf::RenderTarget& target)
{
	_inventoryBackground.render(target);
	_inventoryStack.render(target);
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
	
#pragma region Background Setup

	_inventoryBackground.setSize(VIEW.getSize());
	_inventoryBackground.setPosition(VIEW.getCenter());
	_inventoryBackground.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100)
			});

#pragma endregion
	
#pragma region Grid Setup

	auto& inventoryGrid = _inventoryStack.insert("Grid", gui::Grid(INVENTORY_WIDTH, INVENTORY_HEIGHT));
	inventoryGrid = gui::Grid(INVENTORY_WIDTH, INVENTORY_HEIGHT);
	inventoryGrid.setPosition(VIEW.getCenter());
	inventoryGrid.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,100),
		.outlineColor = sf::Color(255,255,255,100),
		.outlineThickness = 1.f,
		.innerPadding = {24.f, 24.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	inventoryGrid.setTextString(util::getEntityName({ getContext().registry, _sessionEntities.front() }) + "'s Inventory");
	inventoryGrid.setTextPosition(gui::ElementPosition::TOP_CENTER);
	inventoryGrid.setTextOrigin(gui::ElementPosition::BOTTOM_CENTER);

	auto& entityContainer = getContext().registry.get<component::Container>(_sessionEntities.front());
	inventoryGrid.registerCallback(gui::ElementCallbackType::OnUpdate,
		[this, &entityContainer, &inventoryGrid]() -> bool
		{
			int count = 0;
			for (auto& item : entityContainer.contents)
			{
				const auto itemEntity = util::ItemIDToEntityID(item, getContext().registry);
				const auto& itemRender = getContext().registry.get<component::Render>(itemEntity);
				const auto& sprites = getContext().textures.get("Sprites");

				auto& container = inventoryGrid[count];
				sf::Sprite sprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), sprites) };
				container.clear();
				container.insert("Icon", gui::Icon(sprite))
					.setSize({ 32,32 })
					.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
					.setStyle(gui::ElementState::Idle, {
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
			std::string elementName = std::to_string(col) + "-" + std::to_string(row);
			auto& container = inventoryGrid.insert(std::move(elementName), gui::SingleContainer());

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
				.outlineThickness = 2.f,
				});
			container.setStyle(gui::ElementState::Active, {
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color::Red,
				.outlineThickness = 1.f
				});
			container.registerCallback(gui::ElementCallbackType::OnFocus,
				[col, row, this, &entityContainer, &container]() -> bool
				{
					const int index = col + INVENTORY_WIDTH * row;
					if (index >= entityContainer.contents.size())
					{
						_inventoryStack["ItemLabel"].setTextString("");
						_inventoryStack["ItemLabel"].setPosition(container.getPosition() + sf::Vector2f(16.f, -2.f));
						return false;
					}
					const auto itemEntity = util::ItemIDToEntityID(entityContainer.contents.at(index), getContext().registry);
					auto itemName = util::getEntityName({ getContext().registry, itemEntity });
					_inventoryStack["ItemLabel"].setTextString(std::move(itemName));
					_inventoryStack["ItemLabel"].setPosition(container.getPosition() + sf::Vector2f(16.f, -2.f));
					return true;
				});
			container.registerCallback(gui::ElementCallbackType::OnSelect,
				[col, row, this, &entityContainer, &container]() ->bool
				{
					const int index = col + INVENTORY_WIDTH * row;
					if (index >= entityContainer.contents.size())
					{
						return false;
					}
					const auto itemEntity = util::ItemIDToEntityID(entityContainer.contents.at(index), getContext().registry);
					
					_inventoryStack["CommandList"].setVisibility(true);
					_inventoryStack["CommandList"].setPosition(container.getPosition() + sf::Vector2f{36,-1});
					_inventoryStack["CommandList"].layoutChildren();

					_inventoryStack["CommandList"]["Use"].registerCallback(gui::ElementCallbackType::OnSelect,
						[this, itemEntity]() -> bool
						{
							std::cout << "Using item " << util::getEntityName({this->getContext().registry, itemEntity}) << std::endl;
						});
					_inventoryStack["CommandList"]["Swap"].registerCallback(gui::ElementCallbackType::OnSelect,
						[this, itemEntity]() -> bool
						{
							std::cout << "Swapping item " << util::getEntityName({ this->getContext().registry, itemEntity }) << std::endl;
						});
					_inventoryStack["CommandList"]["Drop"].registerCallback(gui::ElementCallbackType::OnSelect,
						[this, itemEntity]() -> bool
						{
							std::cout << "Dropping item " << util::getEntityName({ this->getContext().registry, itemEntity }) << std::endl;
						});
					_inventoryStack["CommandList"]["Equip"].registerCallback(gui::ElementCallbackType::OnSelect,
						[this, itemEntity]() -> bool
						{
							std::cout << "Equipping item " << util::getEntityName({ this->getContext().registry, itemEntity }) << std::endl;
						});
					_inventoryStack["CommandList"]["Info"].registerCallback(gui::ElementCallbackType::OnSelect,
						[this, itemEntity]() -> bool
						{
							std::cout << "Info for item " << util::getEntityName({ this->getContext().registry, itemEntity }) << std::endl;
						});

					return true;
				});
		}
	}

	inventoryGrid.layoutChildren();

#pragma endregion
	
#pragma region Item Label Setup

	_inventoryStack.insert("ItemLabel", gui::Label());
	_inventoryStack["ItemLabel"]
		.setOrigin(gui::ElementPosition::BOTTOM_CENTER)
		.setTextOrigin(gui::ElementPosition::CENTER)
		.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,255),
		.outlineColor = sf::Color(255,255,255,150),
		.outlineThickness = 0.f,
		.innerPadding = {2.f, 2.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
			});

#pragma endregion

#pragma region Command List Setup

	auto& commandList = _inventoryStack.insert("CommandList", gui::List(true));
	commandList
		.setSize({ 64,128 })
		.setPosition(VIEW.getCenter())
		.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,255),
			.outlineColor = sf::Color(255,255,255,100),
			.outlineThickness = 1.f,
			.innerPadding = {4.f, 4.f},
			.childPadding = {2.f, 2.f}
			})
		.setOrigin(gui::ElementPosition::TOP_LEFT)
		.setVisibility(false);
	commandList.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);

	commandList.insert("Use", gui::Label())
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
		.setTextString("use")
		.setTextOrigin(gui::ElementPosition::TOP_LEFT);

	commandList.insert("Swap", gui::Label())
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
		.setTextString("swap")
		.setTextOrigin(gui::ElementPosition::TOP_LEFT);

	commandList.insert("Drop", gui::Label())
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
		.setTextString("drop")
		.setTextOrigin(gui::ElementPosition::TOP_LEFT);

	commandList.insert("Equip", gui::Label())
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
		.setTextString("equip")
		.setTextOrigin(gui::ElementPosition::TOP_LEFT);

	commandList.insert("Info", gui::Label())
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
		.setTextString("info")
		.setTextOrigin(gui::ElementPosition::TOP_LEFT);

	commandList.layoutChildren();

#pragma endregion

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
