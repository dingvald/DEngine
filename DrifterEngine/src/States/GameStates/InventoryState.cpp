#include "pch.h"
#include "InventoryState.h"

#include "Components/Components.h"
#include "Components/RenderComponent.h"
#include <Components/PaperdollLayoutComponent.h>
#include "Components/HealthComponent.h"
#include "Components/ContainerComponent.h"
#include "Components/InteractableComponent.h"
#include "Components/ItemComponent.h"
#include "Components/WearableComponent.h"
#include "Components/DescriptionComponent.h"
#include "Components/MaterialComponent.h"

#include <Events/ChangeMouseVisibilityEvent.h>

#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/ItemDatabase.h"
#include <Systems/Core/MouseVisualizationSystem.h>
#include <Utility/StandardLogger.h>
#include "Utility/StringManipulation.h"
#include "Utility/TextureAtlas.h"
#include <Utility/TGUIHelpers.h>

static const char* w_InventoryList			= "Inventory List";
static const char* w_EntryIcon				= "Icon";
static const char* w_EntryName				= "Name";

static const char* w_EquipmentGrid			= "Equipment Grid";
static const char* w_GridEntrySlotName		= "Slot Name";
static const char* w_GridEntryOverlay		= "Grid Overlay";

drft::InventoryState::InventoryState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	determineSessionEntities();

	auto inventory = tgui::PanelListBox::create();
	inventory->setSize(tgui::bindWidth(_guiGroup) * 0.25f, tgui::bindHeight(_guiGroup) * 0.5f);
	inventory->setOrigin(0.5f, 0.5f);
	inventory->setPosition("75%", "50%");
	inventory->setItemsHeight(32.f);
	inventory->onMousePress([this](tgui::Vector2f position) { onLeftMousePressInventoryWindow(position); });
	_guiGroup->add(inventory, w_InventoryList);

	auto inventoryTemplate = inventory->getPanelTemplate();
	setupInventoryEntryTemplate(inventoryTemplate);

	auto equipmentBackground = tgui::Panel::create();
	equipmentBackground->setOrigin(0.5f, 0.5f);
	equipmentBackground->setPosition("25%", "50%");
	equipmentBackground->setSize(tgui::bindWidth(_guiGroup) * 0.25f, tgui::bindHeight(_guiGroup) * 0.5f);

	auto equipment = tgui::Grid::create();
	equipment->setOrigin(0.5f, 0.5f);
	equipment->setPosition("25%", "50%");
	equipment->setTextSize(16);
	
	_guiGroup->add(equipmentBackground);
	_guiGroup->add(equipment, w_EquipmentGrid);

	refreshInventoryUI(inventory, false);

	setupPaperdollNodeTemplate();
	refreshEquipmentUI(equipment);
}

bool drft::InventoryState::handleEvent(const sf::Event& ev)
{
	if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keypressed->code == sf::Keyboard::Key::Escape)
		{
			if (_draggingItem.has_value())
			{
				_draggingItem->undo();
				_draggingItem.reset();
				refreshInventoryUI(_guiGroup->get<tgui::PanelListBox>(w_InventoryList), false);
				refreshEquipmentUI(_guiGroup->get<tgui::Grid>(w_EquipmentGrid));
			}
			else
			{
				requestStackPop();
			}		
			return true;
		}
	}
	if (const auto mousePressed = ev.getIf<sf::Event::MouseButtonPressed>())
	{
		if (mousePressed->button == sf::Mouse::Button::Left)
		{
			tgui::Vector2f mousePos = { static_cast<float>(mousePressed->position.x), static_cast<float>(mousePressed->position.y) };
			auto widget = _guiGroup->getWidgetAtPos(mousePos, false);
			if (!widget)
			{
				onLeftMousePressOutsideAllWindows();
				return true;
			}
		}
	}

	return false;
}

bool drft::InventoryState::update()
{
	if (_draggingItem.has_value())
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(getContext().window);
		_draggingItem->display->setPosition({ mousePosition.x, mousePosition.y });
		if (_draggingItem->isClickHandled())
		{
			_draggingItem.reset();
		}
	}
	return false;
}

void drft::InventoryState::determineSessionEntities()
{
	auto equipView = getContext().registry.view<component::action::OpenEquipment>();
	_sessionEntity = { getContext().registry, equipView.front() };
	_sessionEntity.remove<component::action::OpenEquipment>();

	_container.set(_sessionEntity);
	_body.set(_sessionEntity);

	if (!_container.isValid())
	{
		error_logger << "Error: trying to open inventory of an entity with no container component" << std::endl;
	}

	if (!_body.isValid())
	{
		error_logger << "Error: trying to open inventory of an entity with no body component" << std::endl;
	}
}

void drft::InventoryState::setupInventoryEntryTemplate(tgui::Panel::Ptr templatePanel)
{
	auto icon = tgui::Picture::create();
	templatePanel->add(icon, w_EntryIcon);
	icon->setSize(16, 24);
	icon->setOrigin(0.f, 0.5f);
	icon->setPosition(4, "50%");

	auto text = tgui::Label::create();
	templatePanel->add(text, w_EntryName);
	text->setVerticalAlignment(tgui::VerticalAlignment::Center);
	text->setTextSize(16);
	text->setOrigin(0.f, 0.5f);
	text->setPosition(icon->getSize().x + 8, "50%");
}

void drft::InventoryState::setupPaperdollNodeTemplate()
{
	_paperdollNodeTemplate = tgui::Panel::create();
	_paperdollNodeTemplate->setSize(64, 96);
	_paperdollNodeTemplate->getRenderer()->setBorders({ 1, 1 });
	_paperdollNodeTemplate->getRenderer()->setBorderColor(tgui::Color{ 100,100,100,100 });

	auto slotName = tgui::Label::create();
	slotName->setTextSize(12);
	slotName->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
	slotName->setOrigin(0.5f, 0.f);
	slotName->setPosition("50%", "0%");
	slotName->getRenderer()->setTextColor(tgui::Color{ 150,150,150,150 });

	auto itemIcon = tgui::Picture::create();
	itemIcon->setOrigin(0.5f, 0.5f);
	itemIcon->setPosition("50%", "50%");
	itemIcon->setSize(tgui::bindSize(_paperdollNodeTemplate));

	auto overlay = tgui::Panel::create();
	overlay->setPosition(tgui::bindPosition(_paperdollNodeTemplate));
	overlay->setSize(tgui::bindSize(_paperdollNodeTemplate));
	overlay->setIgnoreMouseEvents(true);
	overlay->getRenderer()->setBackgroundColor(tgui::Color::Transparent);

	_paperdollNodeTemplate->add(itemIcon, w_EntryIcon);
	_paperdollNodeTemplate->add(slotName, w_GridEntrySlotName);
	_paperdollNodeTemplate->add(overlay, w_GridEntryOverlay);
}

void drft::InventoryState::refreshInventoryUI(tgui::PanelListBox::Ptr list, bool shouldSort)
{
	if (!_container.isValid()) return;

	const unsigned int scrollPosition = list->getVerticalScrollbar()->getValue();

	if (shouldSort) _container.sort();

	list->removeAllItems();

	for (size_t index = 0; index < _container.getItems().size(); index++)
	{
		auto item_id = _container.getItems().at(index);
		entt::entity item_entity = ItemDatabase::getEntityFromItemID(item_id);
		entt::const_handle item_handle = { getContext().registry, item_entity };
		auto panel = list->addItem();
		addItemToInventoryUI(index, item_handle, panel);
	}

	list->getVerticalScrollbar()->setValue(scrollPosition);
}

void drft::InventoryState::refreshEquipmentUI(tgui::Grid::Ptr equipment)
{
	equipment->removeAllWidgets();

	auto paperdoll = _sessionEntity.try_get<PaperdollLayoutComponent>();
	if (!paperdoll) return;

	const tgui::Padding gridCellPadding = { 12, 8 };

	for (int row = 0; row < paperdoll->dimensions.y; row++)
	{
		for (int col = 0; col < paperdoll->dimensions.x; col++)
		{
			if (paperdoll->layout.contains({ col, row }))
			{
				const std::string slotName = paperdoll->layout.at({ col, row });
				if (!_body.hasSlot(slotName)) return;

				auto panel = tgui::Panel::copy(_paperdollNodeTemplate);
				equipment->addWidget(panel, row, col, tgui::Grid::Alignment::Center, gridCellPadding);
				addItemToEquipmentUI(slotName, panel);
			}
			else
			{
				auto emptyPanel = tgui::Panel::create();
				emptyPanel->setSize(tgui::bindSize(_paperdollNodeTemplate));
				equipment->addWidget(emptyPanel, row, col, tgui::Grid::Alignment::Center, gridCellPadding);
			}
		}
	}
}

void drft::InventoryState::addItemToInventoryUI(size_t index, entt::const_handle item, tgui::Panel::Ptr panel)
{
	auto name = util::getEntityName(item);
	auto render = util::getRenderData(item);
	auto rect = getContext().textures.getUV(render.texture, render.uvSize, render.uvCoords);
	auto texture = GuiHelpers::createTGUITextureFromUV(name, rect);
	texture.setColor(render.color);

	auto icon = panel->get<tgui::Picture>(w_EntryIcon);
	icon->getRenderer()->setTexture(texture);
	icon->setIgnoreMouseEvents(true);

	auto text = panel->get<tgui::Label>(w_EntryName);
	text->setText(name);
	text->setIgnoreMouseEvents(true);
	
	auto button = GuiHelpers::buttonizePanel(panel);
	button->onMousePress([this, index, item]() { onLeftMousePressInventoryItem(index, item); });
}

void drft::InventoryState::addItemToEquipmentUI(const std::string& slotName, tgui::Panel::Ptr layout)
{
	auto label = layout->get<tgui::Label>(w_GridEntrySlotName);
	label->setText(shortenPartName(slotName));
	auto slot = _body.getSlot(slotName);
	if (!slot) return;

	auto overlay = layout->get<tgui::Panel>(w_GridEntryOverlay);
	if (_draggingItem.has_value() && !_draggingItem->isClickHandled() && _body.canEquip(slotName, _draggingItem->getItem().entity()))
	{
		overlay->getRenderer()->setBackgroundColor(tgui::Color{ 0, 0, 255, 100 });
	}
	else
	{
		overlay->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
	}

	if (slot->item != ItemComponent::NONE)
	{
		entt::entity item_entity = ItemDatabase::getEntityFromItemID(slot->item);
		entt::const_handle item_handle = { getContext().registry, item_entity };

		auto render = util::getRenderData(item_handle);
		auto rect = getContext().textures.getUV(render.texture, render.uvSize, render.uvCoords);
		auto texture = GuiHelpers::createTGUITextureFromUV(slotName, rect);
		texture.setColor(render.color);
		auto icon = layout->get<tgui::Picture>(w_EntryIcon);
		icon->getRenderer()->setTexture(texture);

		layout->onMousePress([this, slotName, item_handle]() { onLeftMousePressEquipmentItem(slotName, item_handle); });
	}
	else
	{
		layout->onMousePress([this, slotName]() { onLeftMousePressEquipmentItem(slotName, entt::const_handle{}); });
	}
}

std::string drft::InventoryState::shortenPartName(const std::string& fullPartName) const
{
	auto splitString = util::split(fullPartName, "_");
	std::string result;
	if (splitString.size() > 1)
	{
		result = util::getStringAcronym(util::capitalize(splitString.front())) + "." + util::capitalize(splitString.back());
	}
	else
	{
		result = util::capitalize(fullPartName);
	}
	return result;
}

void drft::InventoryState::onLeftMousePressInventoryItem(size_t index, entt::const_handle item)
{
	if (_draggingItem.has_value())
	{
		if (_draggingItem->isClickHandled()) return;

		_container.addBefore(_draggingItem.value().getItem().entity(), index);
		_draggingItem->setClickHandled();

		refreshInventoryUI(_guiGroup->get<tgui::PanelListBox>(w_InventoryList), false);
		refreshEquipmentUI(_guiGroup->get<tgui::Grid>(w_EquipmentGrid));
	}
	else
	{
		_container.remove(item.entity());
		_draggingItem.emplace(DraggingFromInventoryContext{ .item = item, .container = &_container, .index = index }, _guiGroup);
		refreshInventoryUI(_guiGroup->get<tgui::PanelListBox>(w_InventoryList), false);
		refreshEquipmentUI(_guiGroup->get<tgui::Grid>(w_EquipmentGrid));
	}
}

void drft::InventoryState::onLeftMousePressEquipmentItem(const std::string& slotName, entt::const_handle item)
{
	if (_draggingItem.has_value())
	{
		if (_draggingItem->isClickHandled()) return;

		if (!_body.canEquip(slotName, item.entity())) return;

		_body.equip(slotName, _draggingItem.value().getItem().entity());
		_draggingItem.reset();
		if (item)
		{
			_draggingItem.emplace(DraggingFromInventoryContext{ .item = item, .container = &_container, .index = _container.getItems().size() }, _guiGroup);
		}

		refreshInventoryUI(_guiGroup->get<tgui::PanelListBox>(w_InventoryList), false);
		refreshEquipmentUI(_guiGroup->get<tgui::Grid>(w_EquipmentGrid));
	}
	else if (item)
	{
		_body.unequip(slotName);
		_draggingItem.emplace(DraggingFromEquipmentContext{ .item = item, .body = &_body, .slot = slotName }, _guiGroup);
		refreshInventoryUI(_guiGroup->get<tgui::PanelListBox>(w_InventoryList), false);
		refreshEquipmentUI(_guiGroup->get<tgui::Grid>(w_EquipmentGrid));
	}
}

void drft::InventoryState::onLeftMousePressInventoryWindow(tgui::Vector2f position)
{
	if (!_draggingItem.has_value()) return;
	if (_draggingItem->isClickHandled()) return;

	auto inventory = _guiGroup->get<tgui::PanelListBox>(w_InventoryList);
	auto hit = inventory->getWidgetAtPos(position, true);

	if (hit) return;

	_container.add(_draggingItem->getItem().entity());
	_draggingItem->setClickHandled();

	refreshInventoryUI(_guiGroup->get<tgui::PanelListBox>(w_InventoryList), false);
	refreshEquipmentUI(_guiGroup->get<tgui::Grid>(w_EquipmentGrid));
}

void drft::InventoryState::onLeftMousePressOutsideAllWindows()
{
	std::cout << "Pressed" << std::endl;
	if (!_draggingItem.has_value()) return;

	if (_sessionEntity.all_of<component::action::Drop>())
	{
		_sessionEntity.patch<component::action::Drop>([this](component::action::Drop& drop)
			{
				drop.toDrop.push_back(ItemDatabase::getItemIDFromEntity(_draggingItem->getItem()));
			});
	}
	else
	{
		std::vector<ItemComponent::ID> toDrop;
		toDrop.push_back(ItemDatabase::getItemIDFromEntity(_draggingItem->getItem()));
		_sessionEntity.emplace<component::action::Drop>(std::move(toDrop));
	}


	_draggingItem->setClickHandled();

	refreshInventoryUI(_guiGroup->get<tgui::PanelListBox>(w_InventoryList), false);
	refreshEquipmentUI(_guiGroup->get<tgui::Grid>(w_EquipmentGrid));
}

drft::InventoryState::DraggingItem::DraggingItem(DraggingContext ctx, tgui::Group::Ptr gui)
	: context(ctx)
{
	if (std::holds_alternative<std::monostate>(context)) return;

	entt::const_handle item = getItem();

	display = tgui::Group::create();
	display->setSize({ 32, 48 });
	gui->add(display);

	auto background = tgui::Panel::create();
	background->setSize(tgui::bindSize(display));
	background->getRenderer()->setBackgroundColor(tgui::Color{ 0, 0, 0, 200 });
	background->setEnabled(false);
	display->add(background);

	auto icon = tgui::Picture::create();
	auto name = util::getEntityName(item);
	auto render = util::getRenderData(item);
	auto rect = item.registry()->ctx().get<TextureAtlas>().getUV(render.texture, render.uvSize, render.uvCoords);
	auto texture = GuiHelpers::createTGUITextureFromUV(name, rect);
	texture.setColor(render.color);

	icon->getRenderer()->setTexture(texture);
	icon->setSize(tgui::bindSize(display));
	icon->setEnabled(false);
	display->add(icon);

	display->setEnabled(false);
}

drft::InventoryState::DraggingItem::~DraggingItem()
{
	if (!display) return;

	if (auto parent = display->getParent())
	{
		parent->remove(display);
	}
}


entt::const_handle drft::InventoryState::DraggingItem::getItem() const
{
	if (std::holds_alternative<std::monostate>(context)) return entt::const_handle{};

	if (auto fromInventory = std::get_if<DraggingFromInventoryContext>(&context))
	{
		return fromInventory->item;
	}
	else if (auto fromEquipment = std::get_if<DraggingFromEquipmentContext>(&context))
	{
		return fromEquipment->item;
	}
	return entt::const_handle{};
}

void drft::InventoryState::DraggingItem::undo()
{
	if (std::holds_alternative<std::monostate>(context)) return;

	if (auto fromInventory = std::get_if<DraggingFromInventoryContext>(&context))
	{
		fromInventory->container->addBefore(fromInventory->item, fromInventory->index);
	}
	else if (auto fromEquipment = std::get_if<DraggingFromEquipmentContext>(&context))
	{
		fromEquipment->body->equip(fromEquipment->slot, fromEquipment->item.entity());
	}
}

void drft::InventoryState::DraggingItem::setClickHandled()
{
	_isClickHandled = true;
}

bool drft::InventoryState::DraggingItem::isClickHandled() const
{
	return _isClickHandled;
}
