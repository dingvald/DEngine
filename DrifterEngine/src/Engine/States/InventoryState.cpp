#include "pch.h"
#include "InventoryState.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/ItemDatabase.h"
#include "Utility/SpriteIndexer.h"

//TODO: width and height should depend on player's container size
static constexpr int INVENTORY_WIDTH = 5;
static constexpr int INVENTORY_HEIGHT = 6;

static constexpr int EQUIPMENT_WIDTH = 3;
static constexpr int EQUIPMENT_HEIGHT = 4;


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
	updateEquipmentDisplay();
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
	setupEquipmentDisplay();

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

	switch (_sessionType)
	{
		case SessionType::Inventory:
			_flowControl.transferControlTo("InventoryGrid");
		break;
		case SessionType::Equip:
			_flowControl.transferControlTo("EquipmentDisplay");
		break;
	}
}

void drft::InventoryState::setupInventoryDisplay()
{
	const auto& VIEW = getContext().window.getView();

	auto& inventoryGrid = _flowControl.insert("InventoryGrid", gui::Grid(INVENTORY_WIDTH, INVENTORY_HEIGHT));
	inventoryGrid.setPosition(VIEW.getCenter() + sf::Vector2f{160, 0});
	inventoryGrid.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,150),
		.outlineColor = sf::Color(150,150,150,100),
		.outlineThickness = 1.f,
		.innerPadding = {24.f, 24.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	inventoryGrid.setStyle(gui::ElementState::Focused, {
		.fillColor = sf::Color(0,0,0,150),
		.outlineColor = sf::Color(150,150,150,100),
		.outlineThickness = 1.f,
		.innerPadding = {24.f, 24.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	inventoryGrid.setTextString("Inventory");
	inventoryGrid.setTextPosition(gui::ElementPosition::TOP_CENTER);
	inventoryGrid.setTextOrigin(gui::ElementPosition::BOTTOM_CENTER);

	const auto& entityContainer = getContext().registry.get<component::Container>(_sessionEntities.front());
	inventoryGrid.registerCallback(gui::ElementCallbackType::OnUpdate,
		[this, &entityContainer, &inventoryGrid]() -> bool
		{
			for (int i = 0; i < INVENTORY_WIDTH * INVENTORY_HEIGHT - 1; ++i)
			{
				inventoryGrid[i].clear();
			}
			int count = 0;
			for (auto& item : entityContainer.contents)
			{
				auto& container = inventoryGrid[count];
				const auto itemEntity = ItemDatabase::getEntityFromItemID(item);
				addItemIcon(container, itemEntity, {32, 32});

				++count;
			}
			return true;
		});

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
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color(150,150,150,100),
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

			switch (_sessionType)
			{
			case SessionType::Inventory:
			{
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
						const auto itemEntity = ItemDatabase::getEntityFromItemID(itemID);

						auto& commandList = _inventoryStack.insert("CommandList", gui::List(true))
							.setSize({ 64,128 })
							.setStyle(gui::ElementState::Focused, {
								.fillColor = sf::Color(0,0,0,255),
								.outlineColor = sf::Color(255,255,255,100),
								.outlineThickness = 1.f,
								.innerPadding = {4.f, 4.f},
								.childPadding = {2.f, 2.f}
								})
							.setOrigin(gui::ElementPosition::TOP_LEFT);

						commandList.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);

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
							.setTextOrigin(gui::ElementPosition::TOP_LEFT)
							.registerCallback(gui::ElementCallbackType::OnSelect,
								[this, itemEntity, itemID]() -> bool
								{
									const auto& VIEW = getContext().window.getView();
									std::cout << "Equipping item " << util::getEntityName({ this->getContext().registry, itemEntity }) << std::endl;

									_flowControl.transferControlTo("EquipmentDisplay");
									_flowControl["EquipmentDisplay"].setPosition(VIEW.getCenter());
									_flowControl["EquipmentDisplay"].setTextString("Equip " + util::getEntityName({ this->getContext().registry, itemEntity }) + " where?");
									_flowControl["EquipmentDisplay"].setVisibility(false);
									_flowControl["EquipmentDisplay"].setVisibility(true);
									_inventoryBlob["ItemLabel"].setTextString("");
									_sessionContext.setCurrentItem(itemID);
									_inventoryStack.clear();
							
									return true;
								});

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

						_inventoryStack["CommandList"].setPosition(container.getPosition() + sf::Vector2f{ 36,-1 });

						return true;
					});
			}
			break;
			case SessionType::Equip:
			{
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
				container.registerCallback(gui::ElementCallbackType::OnIsSelectable,
					[col, row, this, &entityContainer, &container]() -> bool
					{
						const int index = col + INVENTORY_WIDTH * row;
						if (index >= entityContainer.contents.size())
						{
							return false;
						}
						if (_sessionContext.getCurrentSlot().compare("HeldR") == 0
							|| _sessionContext.getCurrentSlot().compare("HeldL") == 0)
						{
							return true;
						}
						const auto itemEntity = ItemDatabase::getEntityFromItemID(entityContainer.contents.at(index));
						if (auto wearable = getContext().registry.try_get<component::Wearable>(itemEntity))
						{
							if (wearable->slot == _sessionContext.getCurrentSlot())
							{
								return true;
							}
						}

						return false;
					});
				container.registerCallback(gui::ElementCallbackType::OnSelect,
					[col, row, this, &entityContainer, &container]() -> bool
					{
						const auto VIEW = getContext().window.getView();
						auto sessionEntity = this->_sessionEntities.front();
						const int index = col + INVENTORY_WIDTH * row;
						if (index >= entityContainer.contents.size()) return true;

						const auto itemID = entityContainer.contents.at(index);
						if (!container.isSelectable())
						{
							_inventoryStack.insert("Message", gui::PopupMessage())
								.setPosition(VIEW.getCenter())
								.setStyle(gui::ElementState::Focused, {
									.fillColor = sf::Color(0,0,0,255),
									.outlineColor = sf::Color(255,255,255,150),
									.outlineThickness = 1.f,
									.innerPadding = {2.f, 2.f},
									.font = &getContext().fonts.get("Terminus"),
									.textColor = sf::Color::White
									})
								.setTextString("Cannot equip the "
									+ util::getEntityName({ getContext().registry, ItemDatabase::getEntityFromItemID(itemID) }) + ".");
							return true;
						}
						getContext().registry.emplace_or_replace<component::action::Equip>(sessionEntity, itemID, _sessionContext.getCurrentSlot());

						_inventoryStack.insert("Message", gui::PopupMessage())
							.setPosition(VIEW.getCenter())
							.setStyle(gui::ElementState::Focused, {
								.fillColor = sf::Color(0,0,0,255),
								.outlineColor = sf::Color(255,255,255,150),
								.outlineThickness = 1.f,
								.innerPadding = {2.f, 2.f},
								.font = &getContext().fonts.get("Terminus"),
								.textColor = sf::Color::White
								})
							.setTextString(util::getEntityName({ getContext().registry, sessionEntity }) + " equipped the "
								+ util::getEntityName({ getContext().registry, ItemDatabase::getEntityFromItemID(itemID) }) + ".")
							.registerCallback(gui::ElementCallbackType::OnLeave,
								[this]() -> bool
								{
									_flowControl.transferControlTo("EquipmentDisplay");
									_flowControl["InventoryGrid"].setVisibility(false);
									_flowControl["EquipmentDisplay"].setVisibility(true);

									return true;
								});

						_sessionContext.reset();
						return true;
					});
			}
			break;
			}
		}
	}
}

void drft::InventoryState::setupEquipmentDisplay()
{
	const auto& VIEW = getContext().window.getView();
	auto& equipmentDisplay = _flowControl.insert("EquipmentDisplay", gui::List(true));
	equipmentDisplay.setPosition(VIEW.getCenter() + sf::Vector2f{ -160, 0 });
	equipmentDisplay.setStyle(gui::ElementState::Idle, {
					.innerPadding = {24.f, 24.f},
					.childPadding = {0.f, 24.f},
					.font = &getContext().fonts.get("Terminus"),
					.textColor = sf::Color::White
		});
	equipmentDisplay.setStyle(gui::ElementState::Focused, {
			.innerPadding = {24.f, 24.f},
			.childPadding = {0.f, 24.f},
			.font = &getContext().fonts.get("Terminus"),
			.textColor = sf::Color::White
			});
	equipmentDisplay.setTextString("Equipped");
	equipmentDisplay.setTextPosition(gui::ElementPosition::TOP_CENTER);
	equipmentDisplay.setTextOrigin(gui::ElementPosition::BOTTOM_CENTER);
	equipmentDisplay.setChildrenOrigin(gui::ElementPosition::TOP_LEFT, {-80, 0});

	if (auto body = getContext().registry.try_get<component::Body>(_sessionEntities.front()))
	{
		for (auto part : body->parts.flatten())
		{
			auto& row = equipmentDisplay.insert(std::string(part->name), gui::DualContainer())
				.setOrigin(gui::ElementPosition::CENTER_LEFT)
				.setTextOrigin(gui::ElementPosition::CENTER)
				.setStyle(gui::ElementState::Idle, {
					.outlineColor = sf::Color(255,255,255,150),
					.outlineThickness = 0.f,
					.innerPadding = {2.f, 2.f}
					})
				.setStyle(gui::ElementState::Focused, {
					.outlineColor = sf::Color(255,255,255,150),
					.outlineThickness = 0.f,
					.innerPadding = {2.f, 2.f}
					});

			row.insert("Name", gui::Label())
				.setOrigin(gui::ElementPosition::CENTER_LEFT)
				.setTextOrigin(gui::ElementPosition::CENTER)
				.setStyle(gui::ElementState::Idle, {
					.outlineColor = sf::Color(255,255,255,150),
					.outlineThickness = 0.f,
					.innerPadding = {2.f, 2.f},
					.font = &getContext().fonts.get("Terminus"),
					.textColor = sf::Color::White
					})
				.setStyle(gui::ElementState::Focused, {
					.outlineColor = sf::Color(255,255,255,150),
					.outlineThickness = 0.f,
					.innerPadding = {2.f, 2.f},
					.font = &getContext().fonts.get("Terminus"),
					.textColor = sf::Color::Yellow
					})
				.setTextString(std::string(part->name));

			row.insert("Equipped", gui::MultiContainer())
				.setSize({ 16, 16 })
				.setLocalPosition({112, 0})
				.setStyle(gui::ElementState::Idle, {
					.outlineColor = sf::Color(255,255,255,150),
					.outlineThickness = 0.f,
					.innerPadding = {2.f, 2.f}
					})
				.setStyle(gui::ElementState::Focused, {
					.outlineColor = sf::Color(255,255,255,150),
					.outlineThickness = 0.f,
					.innerPadding = {2.f, 2.f}
					});
		}
	}
	equipmentDisplay.update(0.f);
}

void drft::InventoryState::updateInventoryDisplay()
{
}

void drft::InventoryState::updateEquipmentDisplay()
{
	auto& equipmentDisplay = _flowControl["EquipmentDisplay"];
	if (auto body = getContext().registry.try_get<component::Body>(_sessionEntities.front()))
	{
		for (auto part : body->parts.flatten())
		{
			auto& equippedContainer = equipmentDisplay[std::string(part->name)]["Equipped"];
			equippedContainer.clear();
			auto equippedItems = part->getEquipped();
			for (auto item : equippedItems)
			{
				auto& container = equippedContainer.insert("Container", gui::SingleContainer())
					.setSize({16,16})
					.setStyle(gui::ElementState::Idle, {
						.fillColor = sf::Color(0,0,0,100),
						.outlineColor = sf::Color(150,150,150,100),
						.outlineThickness = 1.f,
						.innerPadding = {8.f, 8.f}
						})
					.setStyle(gui::ElementState::Focused, {
						.fillColor = sf::Color(0,0,0,50),
						.outlineColor = sf::Color::Yellow,
						.outlineThickness = 1.f,
						.innerPadding = {8.f, 8.f}
						});
				auto itemEntity = ItemDatabase::getEntityFromItemID(item);
				addItemIcon(container, itemEntity, {16, 16});
			}
		}
	}
	equipmentDisplay.update(0.f);
}

void drft::InventoryState::determineSessionEntities()
{
	auto inventoryView = getContext().registry.view<component::action::OpenInventory>();
	for (auto entity : inventoryView)
	{
		_sessionEntities.push_back(entity);
		_sessionType = SessionType::Inventory;
	}

	auto equipView = getContext().registry.view<component::action::OpenEquipment>();
	for (auto entity : equipView)
	{
		_sessionEntities.push_back(entity);
		_sessionType = SessionType::Equip;
	}
	assert(_sessionEntities.size() <= 2);
}

void drft::InventoryState::shutdownSessionEntities()
{
	for (auto entity : _sessionEntities)
	{
		getContext().registry.remove<component::action::OpenInventory>(entity);
		getContext().registry.remove<component::action::OpenEquipment>(entity);
	}
}
