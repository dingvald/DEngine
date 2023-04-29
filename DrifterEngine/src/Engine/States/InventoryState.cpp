#include "pch.h"
#include "InventoryState.h"
#include "Components/Components.h"
#include "Utility/EntityHelpers.h"
#include "Utility/ItemIDToEntityID.h"
#include "Utility/SpriteIndexer.h"

static constexpr int INVENTORY_WIDTH = 5;
static constexpr int INVENTORY_HEIGHT = 6;

static constexpr int EQUIPMENT_WIDTH = 3;
static constexpr int EQUIPMENT_HEIGHT = 4;

static const std::vector<std::string_view> PAPER_DOLL =
{
	"None",		"Head",		"None",
	"Hands",	"Body",		"None",
	"HeldR",	"Legs",		"HeldL",
	"None",		"Feet",		"None"
};

static const std::map<std::string, drft::util::Sprite> PAPER_DOLL_SPRITES =
{
	{"Head", drft::util::Sprite::PaperDollHead},
	{"Hands", drft::util::Sprite::PaperDollHands},
	{"Body", drft::util::Sprite::PaperDollBody},
	{"HeldR", drft::util::Sprite::PaperDollHandR},
	{"HeldL", drft::util::Sprite::PaperDollHandL},
	{"Legs", drft::util::Sprite::PaperDollLegs},
	{"Feet", drft::util::Sprite::PaperDollFeet}
};


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

void drft::InventoryState::setupPanels()
{
	const auto& VIEW = getContext().window.getView();

	_inventoryBackground.setSize(VIEW.getSize());
	_inventoryBackground.setPosition(VIEW.getCenter());
	_inventoryBackground.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100)
			});
	
	setupInventoryGrid();
	setupEquipmentGrid();

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
			_flowControl["EquipmentGrid"].setVisibility(false);
		break;
		case SessionType::Equip:
			_flowControl.transferControlTo("EquipmentGrid");
			_flowControl["InventoryGrid"].setVisibility(false);
		break;
	}
}

void drft::InventoryState::setupInventoryGrid()
{
	const auto& VIEW = getContext().window.getView();

	auto& inventoryGrid = _flowControl.insert("InventoryGrid", gui::Grid(INVENTORY_WIDTH, INVENTORY_HEIGHT));
	inventoryGrid.setPosition(VIEW.getCenter());
	inventoryGrid.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,50),
		.outlineColor = sf::Color(150,150,150,100),
		.outlineThickness = 1.f,
		.innerPadding = {24.f, 24.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color(150,150,150)
		});
	inventoryGrid.setStyle(gui::ElementState::Focused, {
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
				const auto itemEntity = util::ItemIDToEntityID(item, getContext().registry);
				const auto& itemRender = getContext().registry.get<component::Render>(itemEntity);
				const auto& sprites = getContext().textures.get("Sprites");

				auto& container = inventoryGrid[count];
				sf::Sprite sprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), sprites) };
				container.insert("Icon", gui::Icon(sprite))
					.setSize({ 32,32 })
					.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
					.setStyle(gui::ElementState::Idle, {
							.fillColor = itemRender.color
						});
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
				.outlineThickness = 1.f,
				});
			container.setStyle(gui::ElementState::Active, {
				.fillColor = sf::Color(0,0,0,150),
				.outlineColor = sf::Color::Red,
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
						const auto itemEntity = util::ItemIDToEntityID(entityContainer.contents.at(index), getContext().registry);
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
						const auto itemEntity = util::ItemIDToEntityID(itemID, getContext().registry);

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
								[this, itemEntity]() -> bool
								{
									std::cout << "Using item " << util::getEntityName({ this->getContext().registry, itemEntity }) << std::endl;
									return true;
								});

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

									_flowControl.transferControlTo("EquipmentGrid");
									_flowControl["EquipmentGrid"].setPosition(VIEW.getCenter());
									_flowControl["EquipmentGrid"].setTextString("Equip " + util::getEntityName({ this->getContext().registry, itemEntity }) + " where?");
									_flowControl["InventoryGrid"].setVisibility(false);
									_flowControl["EquipmentGrid"].setVisibility(true);
									_inventoryBlob["ItemLabel"].setTextString("");
									_currentItemID = itemID;
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
										.setTextString("Weight:\n" + std::to_string(itemPhysical.weight) + "kg")
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
						const auto itemEntity = util::ItemIDToEntityID(entityContainer.contents.at(index), getContext().registry);
						auto itemName = util::getEntityName({ getContext().registry, itemEntity });
						_inventoryBlob["ItemLabel"].setTextString(std::move(itemName));
						_inventoryBlob["ItemLabel"].setPosition(container.getPosition() + sf::Vector2f(16.f, -2.f));
						return true;
					});
			}
			break;
			}
		}
	}
}

void drft::InventoryState::setupEquipmentGrid()
{
	const auto& VIEW = getContext().window.getView();

	auto& equipmentGrid = _flowControl.insert("EquipmentGrid", gui::Grid(EQUIPMENT_WIDTH, EQUIPMENT_HEIGHT));
	equipmentGrid.setPosition(VIEW.getCenter());
	equipmentGrid.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,50),
		.outlineColor = sf::Color(150,150,150,100),
		.outlineThickness = 1.f,
		.innerPadding = {24.f, 24.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color(150,150,150)
		});
	equipmentGrid.setStyle(gui::ElementState::Focused, {
		.fillColor = sf::Color(0,0,0,100),
		.outlineColor = sf::Color(255,255,255,100),
		.outlineThickness = 1.f,
		.innerPadding = {24.f, 24.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	equipmentGrid.setTextString(util::getEntityName({ getContext().registry, _sessionEntities.front() }) + "'s Equipment");
	equipmentGrid.setTextPosition(gui::ElementPosition::TOP_CENTER);
	equipmentGrid.setTextOrigin(gui::ElementPosition::BOTTOM_CENTER);

	const auto& entityBody = getContext().registry.get<component::Body>(_sessionEntities.front());
	equipmentGrid.registerCallback(gui::ElementCallbackType::OnUpdate,
		[this, &entityBody, &equipmentGrid]() -> bool
		{
			for (auto& slotName : PAPER_DOLL)
			{
				if (!slotName.compare("None"))
				{
					continue;
				}
				equipmentGrid[slotName.data()].clear();

				const auto itemEntity = util::ItemIDToEntityID(entityBody.parts.at(slotName.data()), getContext().registry);
				if (itemEntity != entt::null)
				{
					const auto& itemRender = getContext().registry.get<component::Render>(itemEntity);
					const auto& sprites = getContext().textures.get("Sprites");

					sf::Sprite sprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), sprites) };
					equipmentGrid[slotName.data()].insert("Icon", gui::Icon(sprite))
						.setSize({ 32,32 })
						.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
						.setStyle(gui::ElementState::Idle, {
								.fillColor = itemRender.color
							});
				}
				else
				{
					const auto& sprites = getContext().textures.get("Sprites");
					sf::Sprite sprite = { sprites, util::SpriteIndexer::get(PAPER_DOLL_SPRITES.at(slotName.data()), sprites)};
					equipmentGrid[slotName.data()].insert("Icon", gui::Icon(sprite))
						.setSize({ 32,32 })
						.setOrigin(gui::ElementPosition::BOTTOM_RIGHT)
						.setStyle(gui::ElementState::Idle, {
								.fillColor = sf::Color(80,80,80,150)
							});
				}
			}

			return true;
		});

	const auto& entityContainer = getContext().registry.get<component::Container>(_sessionEntities.front());

	int dummyCount = 0;
	for (auto& slotName : PAPER_DOLL)
	{
		if (!slotName.compare("None"))
		{
			equipmentGrid.insert("Dummy" + std::to_string(dummyCount), gui::SingleContainer())
				.setSize({ 32,32 })
				.setOrigin(gui::ElementPosition::TOP_LEFT)
				.registerCallback(gui::ElementCallbackType::OnIsSelectable, []() -> bool
					{
						return false;
					});
			++dummyCount;
			continue;
		}

		auto& container = equipmentGrid.insert(slotName.data(), gui::SingleContainer());
		container.setSize({ 32,32 });
		container.setOrigin(gui::ElementPosition::TOP_LEFT);
		container.setChildrenOrigin(gui::ElementPosition::CENTER);
		container.setStyle(gui::ElementState::Unselectable, {
		.fillColor = sf::Color(0,0,0,150),
		.outlineColor = sf::Color(150,150,150,100),
		.outlineThickness = 1.f
			});
		container.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,150),
		.outlineColor = sf::Color(255,255,255,100),
		.outlineThickness = 1.f
			});
		container.setStyle(gui::ElementState::Focused, {
			.fillColor = sf::Color(40,40,0,150),
			.outlineColor = sf::Color::Yellow,
			.outlineThickness = 1.f,
			});

		auto itemInSlot = entityBody.parts.at(slotName.data());
		auto itemEntity = util::ItemIDToEntityID(itemInSlot, getContext().registry);

		switch (_sessionType)
		{
			case SessionType::Inventory:
				{
				container.registerCallback(gui::ElementCallbackType::OnIsSelectable,
					[this, slotName, &container]() -> bool
					{
						std::string slotNameString = slotName.data();

				if (slotNameString.compare("HeldR") == 0 || slotNameString.compare("HeldL") == 0)
				{
					return true;
				}
				return false;
					});
				container.registerCallback(gui::ElementCallbackType::OnFocus,
					[this, slotName, &container]() -> bool
					{
						_inventoryBlob["ItemLabel"].setTextString(slotName.data());
						_inventoryBlob["ItemLabel"].setPosition(container.getPosition() + sf::Vector2f(16.f, -2.f));

				return true;
					});
				container.registerCallback(gui::ElementCallbackType::OnSelect,
					[this, itemEntity, slotName, &container]() -> bool
					{
						auto sessionEntity = this->_sessionEntities.front();
						const auto VIEW = getContext().window.getView();
						getContext().registry.emplace_or_replace<component::action::Equip>(sessionEntity, _currentItemID, slotName.data());

						_inventoryStack.insert("Message", gui::Label())
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
								+ util::getEntityName({ getContext().registry, util::ItemIDToEntityID(_currentItemID, getContext().registry)}));

						_currentItemID = 0;
						return true;
					});
				}
				break;
			case SessionType::Equip:
				{
				container.registerCallback(gui::ElementCallbackType::OnFocus,
					[this, slotName, &container]() -> bool
					{
						_inventoryBlob["ItemLabel"].setTextString(slotName.data());
						_inventoryBlob["ItemLabel"].setPosition(container.getPosition() + sf::Vector2f(16.f, -2.f));

						return true;
					});
				container.registerCallback(gui::ElementCallbackType::OnSelect,
					[this, &container, itemEntity, itemInSlot, slotName]() -> bool
					{
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

						if (itemInSlot > 0)
						{
							commandList.insert("Remove", gui::Label())
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
								.setTextString("remove")
								.setTextOrigin(gui::ElementPosition::TOP_LEFT)
								.registerCallback(gui::ElementCallbackType::OnSelect,
									[this, slotName]() -> bool
									{
										auto sessionEntity = this->_sessionEntities.front();
										getContext().registry.emplace_or_replace<component::action::Unequip>(sessionEntity, slotName.data());
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
											.setTextString("Weight:\n" + std::to_string(itemPhysical.weight) + "kg")
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
						else
						{
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
									[this, slotName]() -> bool
									{
										const auto& VIEW = getContext().window.getView();

										_flowControl.transferControlTo("InventoryGrid");
										_flowControl["InventoryGrid"].setPosition(VIEW.getCenter());
										_flowControl["InventoryGrid"].setTextString("Equip what to " + std::string(slotName.data()));
										_flowControl["InventoryGrid"].setVisibility(true);
										_flowControl["EquipmentGrid"].setVisibility(false);
										_inventoryStack.clear();

										return true;
									});
						}

						_inventoryStack["CommandList"].setPosition(container.getPosition() + sf::Vector2f{ 36,-1 });

						return true;
					});
				}
			break;
		}
	}
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
