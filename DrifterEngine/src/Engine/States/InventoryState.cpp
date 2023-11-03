#include "pch.h"
#include "InventoryState.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/ItemDatabase.h"
#include "Utility/SpriteIndexer.h"
#include "Utility/GetStringAcronym.h"

//TODO: width and height should depend on player's container size
static constexpr int INVENTORY_WIDTH = 5;
static constexpr int INVENTORY_HEIGHT = 6;

static constexpr int HELD_ITEMS_WIDTH = 2;
static constexpr int HELD_ITEMS_HEIGHT = 1;

static constexpr int WORN_ITEMS_WIDTH = 4;
static constexpr int WORN_ITEMS_HEIGHT = 4;

// GUI Element Positions:

static constexpr int PANEL_HEIGHT_OFFSET = -128;

static constexpr int HELD_PANEL_WIDTH_OFFSET = 0;
static constexpr int HELD_PANEL_HEIGHT_OFFSET = PANEL_HEIGHT_OFFSET;

static constexpr int WORN_PANEL_WIDTH_OFFSET = HELD_PANEL_WIDTH_OFFSET;
static constexpr int WORN_PANEL_HEIGHT_OFFSET = HELD_PANEL_HEIGHT_OFFSET + 84;

static constexpr int INVENTORY_PANEL_WIDTH_OFFSET = -256;
static constexpr int INVENTORY_PANEL_HEIGHT_OFFSET = PANEL_HEIGHT_OFFSET;


drft::InventoryState::InventoryState(StateStack& stack, StateContext& context)
    : State(stack, context)
{}

bool drft::InventoryState::handleEvent(const sf::Event& ev)
{
	if (!_inventoryBackground.handleEvent(ev)) return false;
	if (!_inventoryStack.handleEvent(ev)) return false;
	if (!_flowControl.handleEvent(ev)) return false;
	if (!_inventoryBlob.handleEvent(ev)) return false;

	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (ev.key.code == sf::Keyboard::Tab)
		{
			_flowControl.cycleControl();
		}
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
	updateInventoryDisplay();
	updateHeldItemsDisplay();
	updateWornItemsDisplay();
	_inventoryStack.update(dt);
	_flowControl.update(dt);
	_inventoryBlob.update(dt);

    return false;
}

void drft::InventoryState::render(sf::RenderTarget& target)
{
	_inventoryBackground.render(target);
	_flowControl.render(target);
	_inventoryBlob.render(target);
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

void drft::InventoryState::addItemIcon(gui::Element& container, entt::entity item, sf::Vector2f iconSize)
{
	const auto& itemRender = getContext().registry.get<component::Render>(item);
	const auto& sprites = getContext().textures.get("Sprites");

	sf::Sprite sprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), sprites) };
	container.insert("Icon", gui::Icon(sprite))
		.setSize(iconSize)
		.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
		.setStyle(gui::ElementState::Idle, {
				.fillColor = itemRender.color
			})
		.setStyle(gui::ElementState::Focused, {
					.fillColor = itemRender.color
			})
		.setStyle(gui::ElementState::Unselectable, {
				.fillColor = sf::Color(30,30,30,200)
			})
		.setStyle(gui::ElementState::FocusedUnselectable, {
				.fillColor = sf::Color(60,60,60,200)
			});

	if (auto health = getContext().registry.try_get<component::Health>(item))
	{
		float scalingFactor = health->current / health->max;
		container.insert("Health", gui::Panel())
			.setSize({ iconSize.x, (iconSize.y - iconSize.y * scalingFactor) })
			.setLocalPosition({ 0, 16})
			.setOrigin(gui::ElementPosition::BOTTOM_CENTER)
			.setStyle(gui::ElementState::Idle, {
				.fillColor = sf::Color(255,0,0,60)
				})
			.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color(255,0,0,60)
				});
	}
}

void drft::InventoryState::setupPanels()
{
	const auto& VIEW = getContext().window.getView();

	_inventoryBackground.setSize(VIEW.getSize());
	_inventoryBackground.setPosition(VIEW.getCenter());
	_inventoryBackground.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100)
			});
	
	setupInventoryDisplay();
	setupHeldItemsDisplay();
	setupWornItemsDisplay();

	_inventoryBlob.insert("ItemLabel", gui::Label());
	_inventoryBlob["ItemLabel"]
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

	_flowControl.transferControlTo("InventoryGrid");
}

void drft::InventoryState::setupInventoryDisplay()
{
	const auto& VIEW = getContext().window.getView();

	auto& inventoryGrid = _flowControl.insert("InventoryGrid", gui::Grid(INVENTORY_WIDTH, INVENTORY_HEIGHT));
	inventoryGrid.setPosition(VIEW.getCenter() + sf::Vector2f{INVENTORY_PANEL_WIDTH_OFFSET, INVENTORY_PANEL_HEIGHT_OFFSET });
	inventoryGrid.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,200),
		.outlineColor = sf::Color(80,80,80,100),
		.outlineThickness = 1.f,
		.innerPadding = {16.f, 16.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	inventoryGrid.setStyle(gui::ElementState::Focused, {
		.fillColor = sf::Color(0,0,0,150),
		.outlineColor = sf::Color(150,150,150,100),
		.outlineThickness = 1.f,
		.innerPadding = {16.f, 16.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	inventoryGrid.setOrigin(gui::ElementPosition::TOP_LEFT);
	inventoryGrid.setTextString("Inventory:");
	inventoryGrid.setTextPosition(gui::ElementPosition::TOP_LEFT);
	inventoryGrid.setTextOrigin(gui::ElementPosition::BOTTOM_LEFT);

	const auto& entityContainer = getContext().registry.get<component::Container>(_sessionEntities.front());
	for (int row = 0; row < INVENTORY_HEIGHT; ++row)
	{
		for (int col = 0; col < INVENTORY_WIDTH; ++col)
		{
			std::string elementName = std::to_string(col) + "-" + std::to_string(row);
			auto& container = inventoryGrid.insert(std::move(elementName), gui::DualContainer());

			container.setSize({ 32, 32 });
			container.setOrigin(gui::ElementPosition::TOP_LEFT);
			container.setChildrenOrigin(gui::ElementPosition::CENTER);
			container.setStyle(gui::ElementState::Idle, {
				.fillColor = sf::Color(0,0,0,200),
				.outlineColor = sf::Color(80,80,80,100),
				.outlineThickness = 1.f
				});
			container.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color(40,40,0,150),
				.outlineColor = sf::Color::Yellow,
				.outlineThickness = 1.f,
				});
			container.setStyle(gui::ElementState::Active, {
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color::Red,
				.outlineThickness = 1.f
				});
			container.setStyle(gui::ElementState::Unselectable, {
				.fillColor = sf::Color(0,0,0,80),
				.outlineColor = sf::Color(150,150,150,80),
				.outlineThickness = 1.f
				});
			container.setStyle(gui::ElementState::FocusedUnselectable, {
				.fillColor = sf::Color(0,0,0,120),
				.outlineColor = sf::Color(150,150,150,120),
				.outlineThickness = 1.f
				});

			container.registerCallback(gui::ElementCallbackType::OnFocus,
				[col, row, this, &entityContainer, &container]() -> bool
				{
					const int index = col + INVENTORY_WIDTH * row;
					if (index >= entityContainer.contents.size())
					{
						_inventoryBlob["ItemLabel"].setTextString("");
						_inventoryBlob["ItemLabel"].setPosition(container.getPosition() + sf::Vector2f(16.f, -2.f));
						return false;
					}
					const auto itemEntity = ItemDatabase::getEntityFromItemID(entityContainer.contents.at(index));
					auto itemName = util::getEntityName({ getContext().registry, itemEntity });
					_inventoryBlob["ItemLabel"].setTextString(std::move(itemName));
					_inventoryBlob["ItemLabel"].setPosition(container.getPosition() + sf::Vector2f(16.f, -2.f));
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
					const auto itemID = entityContainer.contents.at(index);
					const auto commandListPosition = container.getPosition() + sf::Vector2f{ 36,-1 };
					createItemCommandList(CommandListType::Inventory, commandListPosition, itemID);

					return true;
				});
		}
	}
}

void drft::InventoryState::setupWornItemsDisplay()
{
	const auto& VIEW = getContext().window.getView();

	auto& wornItemsDisplay = _flowControl.insert("WornItemsDisplay", gui::Grid(WORN_ITEMS_WIDTH, WORN_ITEMS_HEIGHT));
	wornItemsDisplay.setPosition(VIEW.getCenter() + sf::Vector2f{WORN_PANEL_WIDTH_OFFSET, WORN_PANEL_HEIGHT_OFFSET });
	wornItemsDisplay.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,200),
		.outlineColor = sf::Color(80,80,80,100),
		.outlineThickness = 1.f,
		.innerPadding = {16.f, 16.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	wornItemsDisplay.setStyle(gui::ElementState::Focused, {
		.fillColor = sf::Color(0,0,0,150),
		.outlineColor = sf::Color(150,150,150,100),
		.outlineThickness = 1.f,
		.innerPadding = {16.f, 16.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	wornItemsDisplay.setOrigin(gui::ElementPosition::TOP_LEFT);
	wornItemsDisplay.setTextString("Worn:");
	wornItemsDisplay.setTextPosition(gui::ElementPosition::TOP_LEFT);
	wornItemsDisplay.setTextOrigin(gui::ElementPosition::BOTTOM_LEFT);
}

void drft::InventoryState::setupHeldItemsDisplay()
{
	const auto& VIEW = getContext().window.getView();

	auto& heldItemsDisplay = _flowControl.insert("HeldItemsDisplay", gui::Grid(HELD_ITEMS_WIDTH, HELD_ITEMS_HEIGHT));
	heldItemsDisplay.setPosition(VIEW.getCenter() + sf::Vector2f{ HELD_PANEL_WIDTH_OFFSET, HELD_PANEL_HEIGHT_OFFSET });
	heldItemsDisplay.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,200),
		.outlineColor = sf::Color(80,80,80,100),
		.outlineThickness = 1.f,
		.innerPadding = {16.f, 16.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	heldItemsDisplay.setStyle(gui::ElementState::Focused, {
		.fillColor = sf::Color(0,0,0,150),
		.outlineColor = sf::Color(150,150,150,100),
		.outlineThickness = 1.f,
		.innerPadding = {16.f, 16.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	heldItemsDisplay.setOrigin(gui::ElementPosition::TOP_LEFT);
	heldItemsDisplay.setTextString("Held:");
	heldItemsDisplay.setTextPosition(gui::ElementPosition::TOP_LEFT);
	heldItemsDisplay.setTextOrigin(gui::ElementPosition::BOTTOM_LEFT);
}

void drft::InventoryState::updateInventoryDisplay()
{
	auto& inventoryGrid = _flowControl["InventoryGrid"];
	for (int i = 0; i < INVENTORY_WIDTH * INVENTORY_HEIGHT - 1; ++i)
	{
		inventoryGrid[i].clear();
	}

	const auto& entityContainer = getContext().registry.get<component::Container>(_sessionEntities.front());
	int count = 0;
	for (auto& item : entityContainer.contents)
	{
		auto& container = inventoryGrid[count];
		const auto itemEntity = ItemDatabase::getEntityFromItemID(item);
		addItemIcon(container, itemEntity, { 32, 32 });

		++count;
	}
}

void drft::InventoryState::updateWornItemsDisplay()
{
	auto& wornItemsDisplay = _flowControl["WornItemsDisplay"];
	wornItemsDisplay.clear();
	if (auto body = getContext().registry.try_get<component::Body>(_sessionEntities.front()))
	{
		auto wornItems = body->parts.getAllWornEquipped();
		for (auto& [partName, item] : wornItems)
		{
			auto& container = wornItemsDisplay.insert(std::to_string(item), gui::DualContainer());
			container.setSize({ 32, 32 });
			container.setOrigin(gui::ElementPosition::TOP_LEFT);
			container.setChildrenOrigin(gui::ElementPosition::CENTER);
			container.setStyle(gui::ElementState::Idle, {
				.fillColor = sf::Color(0,0,0,200),
				.outlineColor = sf::Color(80,80,80,100),
				.outlineThickness = 1.f
				});
			container.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color(40,40,0,150),
				.outlineColor = sf::Color::Yellow,
				.outlineThickness = 1.f,
				});
			container.setStyle(gui::ElementState::Active, {
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color::Red,
				.outlineThickness = 1.f
				});
			container.setStyle(gui::ElementState::Unselectable, {
				.fillColor = sf::Color(0,0,0,80),
				.outlineColor = sf::Color(150,150,150,80),
				.outlineThickness = 1.f
				});
			container.setStyle(gui::ElementState::FocusedUnselectable, {
				.fillColor = sf::Color(0,0,0,120),
				.outlineColor = sf::Color(150,150,150,120),
				.outlineThickness = 1.f
				});
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			addItemIcon(container, itemEntity, { 32, 32 });
			container.insert("Slot Abbrev", gui::Label())
				.setLocalPosition({ -8, -8 })
				.setStyle(gui::ElementState::Idle, {
					.font = &getContext().fonts.get("Terminus"),
					.textColor = sf::Color(255,255,255,200)
					})
				.setStyle(gui::ElementState::Focused, {
					.font = &getContext().fonts.get("Terminus"),
					.textColor = sf::Color(255,255,255,50)
					})
				.setTextString(util::getStringAcronym(partName));
			container.registerCallback(gui::ElementCallbackType::OnFocus,
				[this, &container, item]() -> bool
				{
					const auto itemEntity = ItemDatabase::getEntityFromItemID(item);
					auto itemName = util::getEntityName({ getContext().registry, itemEntity });
					_inventoryBlob["ItemLabel"].setTextString(std::move(itemName));
					_inventoryBlob["ItemLabel"].setPosition(container.getPosition() + sf::Vector2f(16.f, -2.f));
					return true;
				});
			container.registerCallback(gui::ElementCallbackType::OnSelect, 
				[this, item, &container]() -> bool
				{
					const auto commandListPosition = container.getPosition() + sf::Vector2f{ 36,-1 };
					createItemCommandList(CommandListType::Worn, commandListPosition, item);

					return true;
				});
		}
	}
	if (wornItemsDisplay == _flowControl.getActive() && wornItemsDisplay.isEmpty())
	{
		_flowControl.cycleControl();
	}
}

void drft::InventoryState::updateHeldItemsDisplay()
{
	auto& heldItemsDisplay = _flowControl["HeldItemsDisplay"];
	heldItemsDisplay.clear();
	if (auto body = getContext().registry.try_get<component::Body>(_sessionEntities.front()))
	{
		auto heldItems = body->parts.getAllHeldEquipped();
		for (auto& [partName, item] : heldItems)
		{
			auto& container = heldItemsDisplay.insert(std::to_string(item), gui::DualContainer());
			container.setSize({ 32, 32 });
			container.setOrigin(gui::ElementPosition::TOP_LEFT);
			container.setChildrenOrigin(gui::ElementPosition::CENTER);
			container.setStyle(gui::ElementState::Idle, {
				.fillColor = sf::Color(0,0,0,200),
				.outlineColor = sf::Color(80,80,80,100),
				.outlineThickness = 1.f
				});
			container.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color(40,40,0,150),
				.outlineColor = sf::Color::Yellow,
				.outlineThickness = 1.f,
				.font = &getContext().fonts.get("Terminus"),
				.textColor = sf::Color::White
				});
			container.setStyle(gui::ElementState::Active, {
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color::Red,
				.outlineThickness = 1.f,
				.font = &getContext().fonts.get("Terminus"),
				.textColor = sf::Color::White
				});
			container.setStyle(gui::ElementState::Unselectable, {
				.fillColor = sf::Color(0,0,0,80),
				.outlineColor = sf::Color(150,150,150,80),
				.outlineThickness = 1.f,
				.font = &getContext().fonts.get("Terminus"),
				.textColor = sf::Color::White
				});
			container.setStyle(gui::ElementState::FocusedUnselectable, {
				.fillColor = sf::Color(0,0,0,120),
				.outlineColor = sf::Color(150,150,150,120),
				.outlineThickness = 1.f,
				.font = &getContext().fonts.get("Terminus"),
				.textColor = sf::Color::White
				});
			auto itemEntity = ItemDatabase::getEntityFromItemID(item);
			addItemIcon(container, itemEntity, { 32, 32 });
			container.insert("Slot Abbrev", gui::Label())
				.setLocalPosition({ -8, -8 })
				.setStyle(gui::ElementState::Idle, {
					.font = &getContext().fonts.get("Terminus"),
					.textColor = sf::Color(255,255,255,200)
					})
				.setStyle(gui::ElementState::Focused, {
					.font = &getContext().fonts.get("Terminus"),
					.textColor = sf::Color(255,255,255,50)
					})
				.setTextString(util::getStringAcronym(partName));
			container.registerCallback(gui::ElementCallbackType::OnFocus,
				[this, &container, item]() -> bool
				{
					const auto itemEntity = ItemDatabase::getEntityFromItemID(item);
					auto itemName = util::getEntityName({ getContext().registry, itemEntity });
					_inventoryBlob["ItemLabel"].setTextString(std::move(itemName));
					_inventoryBlob["ItemLabel"].setPosition(container.getPosition() + sf::Vector2f(16.f, -2.f));
					return true;
				});
			container.registerCallback(gui::ElementCallbackType::OnSelect, 
				[this, item, &container]() -> bool
				{
					const auto commandListPosition = container.getPosition() + sf::Vector2f{ 36,-1 };
					createItemCommandList(CommandListType::Held, commandListPosition, item);

					return true;
				});
		}
	}
	if (heldItemsDisplay == _flowControl.getActive() && heldItemsDisplay.isEmpty())
	{
		_flowControl.cycleControl();
	}
}

void drft::InventoryState::determineSessionEntities()
{
	auto equipView = getContext().registry.view<component::action::OpenEquipment>();
	for (auto entity : equipView)
	{
		_sessionEntities.push_back(entity);
	}
	assert(_sessionEntities.size() <= 2);
}

void drft::InventoryState::shutdownSessionEntities()
{
	for (auto entity : _sessionEntities)
	{
		getContext().registry.remove<component::action::OpenEquipment>(entity);
	}
}

void drft::InventoryState::createItemCommandList(CommandListType type, sf::Vector2f position, unsigned long itemID)
{
	const auto itemEntity = ItemDatabase::getEntityFromItemID(itemID);

	auto& commandList = _inventoryStack.insert("CommandList", gui::List(true))
		.setSize({ 64,128 })
		.setPosition(position)
		.setStyle(gui::ElementState::Focused, {
			.fillColor = sf::Color(0,0,0,255),
			.outlineColor = sf::Color(255,255,255,100),
			.outlineThickness = 1.f,
			.innerPadding = {4.f, 4.f},
			.childPadding = {2.f, 2.f}
			})
		.setOrigin(gui::ElementPosition::TOP_LEFT)
		.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);

	switch (type)
	{
	case CommandListType::Inventory:
		{
			if (auto usable = getContext().registry.try_get<component::Usable>(itemEntity))
			{
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
					.setTextOrigin(gui::ElementPosition::TOP_LEFT)
					.registerCallback(gui::ElementCallbackType::OnSelect,
						[this, itemEntity, itemID]() -> bool
						{
							requestStackPop();
				getContext().registry.emplace_or_replace<component::action::Use>(_sessionEntities.front(), itemEntity, itemID);

				return true;
						});
			}

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
				.setTextOrigin(gui::ElementPosition::TOP_LEFT)
				.registerCallback(gui::ElementCallbackType::OnSelect,
					[this, itemEntity]() -> bool
					{
						std::cout << "Swapping item " << util::getEntityName({ this->getContext().registry, itemEntity }) << std::endl;
			return true;
					});

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
				.setTextOrigin(gui::ElementPosition::TOP_LEFT)
				.registerCallback(gui::ElementCallbackType::OnSelect,
					[this, itemEntity, itemID]() -> bool
					{
						auto sessionEntity = this->_sessionEntities.front();

			if (getContext().registry.all_of<component::action::Drop>(sessionEntity))
			{
				getContext().registry.patch<component::action::Drop>(sessionEntity,
					[itemID](component::action::Drop& drop)
					{
						drop.toDrop.push_back(itemID);
					});
			}
			else
			{
				getContext().registry.emplace<component::action::Drop>(sessionEntity, std::vector<component::Item::ID>{itemID});
			}

			_inventoryStack.clear();

			return true;
					});

			auto& equipCommand = commandList.insert("Equip", gui::Label());
			equipCommand.setStyle(gui::ElementState::Idle, {
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
				.setTextOrigin(gui::ElementPosition::TOP_LEFT)
				.registerCallback(gui::ElementCallbackType::OnSelect,
					[this, itemID, &equipCommand]() -> bool
					{
						auto commandBounds = equipCommand.getGlobalBounds();
						sf::Vector2f position = { commandBounds.left + commandBounds.width + 8, equipCommand.getPosition().y};
						tryEquipItem(itemID, position);
						return true;
					});
		}
	break;
	default:
		{
		commandList.insert("Unequip", gui::Label())
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
			.setTextString("unequip")
			.setTextOrigin(gui::ElementPosition::TOP_LEFT)
			.registerCallback(gui::ElementCallbackType::OnSelect,
				[this, itemID]() -> bool
				{
					tryUnequipItem(itemID);
					return true;
				});
		}
	break;	
	}
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
		.setTextOrigin(gui::ElementPosition::TOP_LEFT)
		.registerCallback(gui::ElementCallbackType::OnSelect,
			[this, itemEntity]() -> bool
			{
				auto& itemInfo = getContext().registry.get<component::Info>(itemEntity);
	auto& itemPhysical = getContext().registry.get<component::Physical>(itemEntity);
	_inventoryStack.insert("ItemInfoList", gui::List(false));
	_inventoryStack["ItemInfoList"]
		.setPosition(getContext().window.getView().getCenter())
		.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color::Black,
				.outlineColor = sf::Color(100,100,100,255),
				.outlineThickness = 2.f,
				.innerPadding = {4,4},
				.childPadding = {0, 16}
			})
		.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);
	_inventoryStack["ItemInfoList"].insert("Name", gui::Label())
		.setStyle(gui::ElementState::Idle, {
				.font = &getContext().fonts.get("Terminus"),
				.textSize = 16
			})
		.setTextString("Name:\n" + itemInfo.name)
		.setTextOrigin(gui::ElementPosition::TOP_LEFT);
	_inventoryStack["ItemInfoList"].insert("Description", gui::Label())
		.setStyle(gui::ElementState::Idle, {
			.font = &getContext().fonts.get("Terminus"),
			.textSize = 16
			})
		.setTextString("Description:\n" + itemInfo.description)
		.setTextOrigin(gui::ElementPosition::TOP_LEFT);
	_inventoryStack["ItemInfoList"].insert("Weight", gui::Label())
		.setStyle(gui::ElementState::Idle, {
			.font = &getContext().fonts.get("Terminus"),
			.textSize = 16
			})
		.setTextString("Weight:\n" + std::format("{:.1f}", itemPhysical.weight) + "kg")
		.setTextOrigin(gui::ElementPosition::TOP_LEFT);

	std::stringstream ss;
	ss << "[";
	for (int i = 0; i < itemPhysical.materials.size(); ++i)
	{
		if (itemPhysical.materials[i].compare("") == 0) continue;

		ss << itemPhysical.materials[i];
		if (i < itemPhysical.materials.size() - 1)
		{
			ss << ", ";
		}
	}
	ss << "]";

	_inventoryStack["ItemInfoList"].insert("Materials", gui::Label())
		.setStyle(gui::ElementState::Idle, {
				.font = &getContext().fonts.get("Terminus"),
				.textSize = 16
			})
		.setTextString("Materials:\n" + ss.str())
		.setTextOrigin(gui::ElementPosition::TOP_LEFT);

	return true;
			});
}

void drft::InventoryState::tryEquipItem(unsigned long itemID, sf::Vector2f position)
{
	if (auto body = getContext().registry.try_get<component::Body>(_sessionEntities.front()))
	{
		const auto& VIEW = getContext().window.getView();
		const auto itemEntity = ItemDatabase::getEntityFromItemID(itemID);
		const auto playersName = util::getEntityName({ getContext().registry, _sessionEntities.front() });
		const auto itemName = util::getEntityName({ getContext().registry, itemEntity });

		auto& commandList = _inventoryStack.insert("Equip Where?", gui::List(true))
			.setSize({ 64,128 })
			.setPosition(position)
			.setStyle(gui::ElementState::Focused, {
				.fillColor = sf::Color(0,0,0,255),
				.outlineColor = sf::Color(255,255,255,100),
				.outlineThickness = 1.f,
				.innerPadding = {4.f, 4.f},
				.childPadding = {2.f, 2.f},
				.font = &getContext().fonts.get("Terminus"),
				.textColor = sf::Color::White,
				.textSize = 16
				})
			.setOrigin(gui::ElementPosition::TOP_LEFT)
			.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);

		if (auto wearable = getContext().registry.try_get<component::Wearable>(itemEntity))
		{
			auto slots = body->parts.getSlotPartsForItem(wearable->slots);
			for (auto& slot : slots)
			{
				commandList.insert(std::string{ slot }, gui::Label())
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
					.setTextString(std::string{ slot })
					.setTextOrigin(gui::ElementPosition::TOP_LEFT)
					.registerCallback(gui::ElementCallbackType::OnSelect,
						[this, itemEntity, itemID, wearable, slot]() -> bool
						{
							std::string slotName = slot;
							component::action::Equip equipAction{ .toEquip = itemID, .partName = slotName, .layer = static_cast<EquipmentLayer>(wearable->layer) };
							getContext().registry.emplace_or_replace<component::action::Equip>(_sessionEntities.front(), equipAction);
							_inventoryStack.clear();
							return true;
						});
			}
		}
		else
		{
			auto hands = body->parts.search(PartType::Hand);
			for (auto hand : hands)
			{
				commandList.insert(std::string{hand->name}, gui::Label())
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
					.setTextString(std::string{ hand->name })
					.setTextOrigin(gui::ElementPosition::TOP_LEFT)
					.registerCallback(gui::ElementCallbackType::OnSelect,
						[this, itemEntity, itemID, hand]() -> bool
						{
							std::string slotName = hand->name;
							component::action::Equip equipAction{ .toEquip = itemID, .partName = slotName, .layer = EquipmentLayer::Held };
							getContext().registry.emplace_or_replace<component::action::Equip>(_sessionEntities.front(), equipAction);
							_inventoryStack.clear();
							return true;
						});
			}
		}
	}
}

void drft::InventoryState::tryUnequipItem(unsigned long itemID)
{
	if (auto body = getContext().registry.try_get<component::Body>(_sessionEntities.front()))
	{
		if (auto container = getContext().registry.try_get<component::Container>(_sessionEntities.front()))
		{
			if (container->contents.size() < container->capacity)
			{
				getContext().registry.emplace_or_replace<component::action::Unequip>(_sessionEntities.front(), itemID);
			}
		}
	}
	_inventoryStack.clear();
}
