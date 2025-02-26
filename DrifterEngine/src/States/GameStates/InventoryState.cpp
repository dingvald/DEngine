#include "pch.h"
#include "InventoryState.h"

#include "Components/Components.h"
#include "Components/RenderComponent.h"
#include "Components/BodyComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ContainerComponent.h"
#include "Components/InteractableComponent.h"
#include "Components/ItemComponent.h"
#include "Components/WearableComponent.h"
#include "Components/DescriptionComponent.h"
#include "Components/MaterialComponent.h"

#include "Utility/EntityHelpers.h"
#include "Systems/Helpers/ItemDatabase.h"
#include <Utility/StandardLogger.h>
#include "Utility/StringManipulation.h"
#include "Utility/TextureAtlas.h"
#include <Utility/TGUIHelpers.h>

static const char* w_InventoryList			= "Inventory List";
static const char* w_EntryIcon				= "Icon";
static const char* w_EntryName				= "Name";

static const char* w_EquipmentList			= "Equipment List";
static const char* w_EquipmentListGrid		= "Equipment List Grid";
static const char* w_EntrySlotName			= "Slot Name";

drft::InventoryState::InventoryState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
	determineSessionEntities();

	auto inventory = tgui::PanelListBox::create();
	inventory->setSize(tgui::bindWidth(_guiGroup) * 0.25f, tgui::bindHeight(_guiGroup) * 0.5f);
	inventory->setOrigin(0.5f, 0.5f);
	inventory->setPosition("75%", "50%");
	inventory->setItemsHeight(32.f);
	_guiGroup->add(inventory, w_InventoryList);

	auto inventoryTemplate = inventory->getPanelTemplate();
	setupInventoryEntryTemplate(inventoryTemplate);

	auto equipment = tgui::PanelListBox::create();
	equipment->setSize(tgui::bindWidth(_guiGroup) * 0.25f, tgui::bindHeight(_guiGroup) * 0.5f);
	equipment->setOrigin(0.5f, 0.5f);
	equipment->setPosition("25%", "50%");
	equipment->setItemsHeight(32.f);
	_guiGroup->add(equipment, w_EquipmentList);

	auto equipmentTemplate = equipment->getPanelTemplate();
	setupEquipmentEntryTemplate(equipmentTemplate);

	refreshInventoryUI(inventory);
	refreshEquipmentUI(equipment);
}

bool drft::InventoryState::handleEvent(const sf::Event& ev)
{
	if (const auto keypressed = ev.getIf<sf::Event::KeyPressed>())
	{
		if (keypressed->code == sf::Keyboard::Key::Escape)
		{
			requestStackPop();
			return true;
		}
	}
	return false;

	return false;
}

void drft::InventoryState::determineSessionEntities()
{
	auto equipView = getContext().registry.view<component::action::OpenEquipment>();
	_sessionEntity = { getContext().registry, equipView.front() };
	_sessionEntity.remove<component::action::OpenEquipment>();
	_container.setContainer(_sessionEntity);
	if (!_container.isValid())
	{
		error_logger << "Error: trying to open inventory of an entity with no container component" << std::endl;
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

void drft::InventoryState::setupEquipmentEntryTemplate(tgui::Panel::Ptr templatePanel)
{
	auto slotName = tgui::Label::create();
	slotName->setVerticalAlignment(tgui::VerticalAlignment::Center);
	slotName->setTextSize(16);
	slotName->setPosition("0%, 0%");
	templatePanel->add(slotName, w_EntrySlotName);
	
	auto icon = tgui::Picture::create();
	icon->setSize(16, 24);
	icon->setPosition("40%", 0.f);
	templatePanel->add(icon, w_EntryIcon);

	auto itemName = tgui::Label::create();
	itemName->setVerticalAlignment(tgui::VerticalAlignment::Center);
	itemName->setTextSize(16);
	itemName->setPosition(tgui::bindRight(icon) + 4.f, 0.f);
	templatePanel->add(itemName, w_EntryName);
}

void drft::InventoryState::refreshInventoryUI(tgui::PanelListBox::Ptr list)
{
	if (!_container.isValid()) return;

	_container.sort();

	for (auto&& item_id : _container.getItems())
	{
		entt::entity item_entity = ItemDatabase::getEntityFromItemID(item_id);
		entt::const_handle item_handle = { getContext().registry, item_entity };
		auto panel = list->addItem();
		addItemToInventoryUI(item_handle, panel);
	}
}

void drft::InventoryState::refreshEquipmentUI(tgui::PanelListBox::Ptr equipment)
{
	if (auto body = _sessionEntity.try_get<BodyComponent>())
	{
		for (auto&& part : body->parts.getAllParts())
		{
			auto panel = equipment->addItem();
			addItemToEquipmentUI(*part, panel);
		}
	}
}

void drft::InventoryState::addItemToInventoryUI(entt::const_handle item, tgui::Panel::Ptr panel)
{
	auto name = util::getEntityName(item);
	auto render = util::getRenderData(item);
	auto rect = getContext().textures.getUV(render.texture, render.uvSize, render.uvCoords);
	tgui::Texture texture{ name, GuiHelpers::toUIntRect(rect) };
	texture.setColor(render.color);

	auto icon = panel->get<tgui::Picture>(w_EntryIcon);
	icon->getRenderer()->setTexture(texture);
	icon->setIgnoreMouseEvents(true);

	auto text = panel->get<tgui::Label>(w_EntryName);
	text->setText(name);
	text->setIgnoreMouseEvents(true);

	GuiHelpers::buttonizePanel(panel);
}

void drft::InventoryState::addItemToEquipmentUI(const BodyPart& part, tgui::Panel::Ptr panel)
{
	auto slotName = shortenPartName(part.getName());
	auto slotNameWidget = panel->get<tgui::Label>(w_EntrySlotName);
	slotNameWidget->setText(slotName);
	slotNameWidget->setIgnoreMouseEvents(true);

	GuiHelpers::buttonizePanel(panel);
}

std::string drft::InventoryState::shortenPartName(const std::string& fullPartName) const
{
	auto splitString = util::split(fullPartName, " ");
	std::string result;
	if (splitString.size() > 1)
	{
		result = util::getStringAcronym(splitString.front()) + " " + splitString.back();
	}
	else
	{
		result = fullPartName;
	}
	return result;
}
