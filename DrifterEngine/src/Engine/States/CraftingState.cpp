#include "pch.h"
#include "CraftingState.h"
#include "Components/Components.h"
#include "Factory/EntityFactory.h"
#include "Utility/EntityHelpers.h"
#include "Utility/ItemIDToEntityID.h"


static constexpr float CRAFTING_WINDOW_WIDTH = 320.f;
static constexpr float CRAFTING_WINDOW_HEIGHT = 256.f;

drft::CraftingState::CraftingState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
}

bool drft::CraftingState::handleEvent(const sf::Event& ev)
{
	_craftingList.handleEvent(ev);

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

bool drft::CraftingState::update(const float dt)
{
	_craftingBackground.update(dt);
	_craftingWindow.update(dt);
	_craftingList.update(dt);

    return false;
}

void drft::CraftingState::render(sf::RenderTarget& target)
{
	_craftingBackground.render(target);
	_craftingWindow.render(target);
	_craftingList.render(target);
}

void drft::CraftingState::onPush()
{
	determineSessionEntities();

	const auto& VIEW = getContext().window.getView();

	_craftingBackground.setSize(VIEW.getSize());
	_craftingBackground.setPosition(VIEW.getCenter());
	_craftingBackground.setStyle(gui::ElementState::Idle, {
			.fillColor = sf::Color(0,0,0,100)
		});

	_craftingWindow.setSize({ CRAFTING_WINDOW_WIDTH, CRAFTING_WINDOW_HEIGHT });
	_craftingWindow.setPosition(VIEW.getCenter());
	_craftingWindow.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,100),
		.outlineColor = sf::Color(150,150,150,100),
		.outlineThickness = 1.f,
		.innerPadding = {24.f, 24.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	_craftingWindow.setTextString("Crafting");
	_craftingWindow.setTextPosition(gui::ElementPosition::TOP_CENTER);
	_craftingWindow.setTextOrigin(gui::ElementPosition::BOTTOM_CENTER);


	setupCraftingList();
}

void drft::CraftingState::onPop()
{
	shutdownSessionEntities();
}

void drft::CraftingState::determineSessionEntities()
{
	auto craftingView = getContext().registry.view<component::action::OpenCrafting>();
	_sessionEntity = craftingView.front();
}

void drft::CraftingState::shutdownSessionEntities()
{
	getContext().registry.remove<component::action::OpenCrafting>(_sessionEntity);
}

void drft::CraftingState::setupCraftingList()
{
	const auto& VIEW = getContext().window.getView();
	_craftingList.setPosition(VIEW.getCenter() - sf::Vector2f{128,0});
	_craftingList.setSize({ 64, CRAFTING_WINDOW_HEIGHT});
	_craftingList.setStyle(gui::ElementState::Idle, {
		.fillColor = sf::Color(0,0,0,50),
		.innerPadding = {24.f, 24.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color(150,150,150)
		});
	_craftingList.setStyle(gui::ElementState::Focused, {
		.fillColor = sf::Color(0,0,0,100),
		.innerPadding = {24.f, 24.f},
		.childPadding = {8.f, 8.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	_craftingList.setState(gui::ElementState::Focused);
	_craftingList.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);

	const auto craftableItems = getContext().registry.try_get<component::MyCraftableItems>(_sessionEntity);
	const auto& prototypeReg = getContext().registry.ctx().get<const EntityFactory&>().prototypes();

	if (craftableItems)
	{
		const auto& sprites = getContext().textures.get("Sprites");
		int count = 0;
		for (auto&& craftable : craftableItems->craftables)
		{
			std::string str = std::to_string(count);
			_craftingList.insert(str.data(), gui::DualContainer())
				.setStyle(gui::ElementState::Idle, {
						.childPadding = {20, 0}
					})
				.setStyle(gui::ElementState::Focused, {
						.childPadding = {20, 0}
					});

			const auto& itemRender = prototypeReg.get<component::Render>(craftable);
			sf::Sprite sprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), sprites) };
			_craftingList[str.data()].insert("Icon", gui::Icon(sprite))
				.setSize({ 32, 32 })
				.setStyle(gui::ElementState::Idle, {
					.fillColor = itemRender.color
					})
				.setStyle(gui::ElementState::Focused, {
					.fillColor = itemRender.color
					});

			_craftingList[str.data()].insert("Label", gui::Label())
				.setSize({ 32, 32 })
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
				.setTextString(util::getEntityName({ prototypeReg, craftable }))
				.setTextOrigin(gui::ElementPosition::CENTER_LEFT);

			++count;
		}
		for (auto&& craftable : craftableItems->partialCraftables)
		{
			std::string str = std::to_string(count);
			_craftingList.insert(str.data(), gui::DualContainer())
				.setStyle(gui::ElementState::Idle, {
						.childPadding = {20, 0}
					})
				.setStyle(gui::ElementState::Focused, {
						.childPadding = {20, 0}
					});

			const auto& itemRender = prototypeReg.get<component::Render>(craftable);
			sf::Sprite sprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), sprites) };
			_craftingList[str.data()].insert("Icon", gui::Icon(sprite))
				.setSize({ 32, 32 })
				.setStyle(gui::ElementState::Idle, {
					.fillColor = itemRender.color
					})
				.setStyle(gui::ElementState::Focused, {
					.fillColor = itemRender.color
					});

			_craftingList[str.data()].insert("Label", gui::Label())
				.setSize({ 32, 32 })
				.setStyle(gui::ElementState::Idle, {
					.font = &getContext().fonts.get("Terminus"),
					.textColor = sf::Color(80,80,80),
					.textSize = 16
					})
				.setStyle(gui::ElementState::Focused, {
					.font = &getContext().fonts.get("Terminus"),
					.textColor = sf::Color(120,120,120),
					.textSize = 16
					})
				.setTextString(util::getEntityName({ prototypeReg, craftable }))
				.setTextOrigin(gui::ElementPosition::CENTER_LEFT);

			++count;
		}
	}
}

