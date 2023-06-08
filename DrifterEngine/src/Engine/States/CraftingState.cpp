#include "pch.h"
#include "CraftingState.h"
#include "Components/Components.h"
#include "Factory/EntityFactory.h"
#include "Utility/EntityHelpers.h"
#include "Utility/ItemIDToEntityID.h"


static constexpr float CRAFTING_WINDOW_WIDTH = 352.f;
static constexpr float CRAFTING_WINDOW_HEIGHT = 256.f;

drft::CraftingState::CraftingState(StateStack& stack, StateContext& context)
    : State(stack, context)
{
}

bool drft::CraftingState::handleEvent(const sf::Event& ev)
{
	if (!_popupStack.handleEvent(ev)) return false;
	if (!_craftingList.handleEvent(ev)) return false;

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
	if (_requiresRefresh)
	{
		refreshCraftingList();
	}

	if (!_popupStack.update(dt)) return false;
	if (!_craftingBackground.update(dt)) return false;
	if (!_craftingWindow.update(dt)) return false;
	if (!_craftingList.update(dt)) return false;
	if (!_requiresList.update(dt)) return false;

    return false;
}

void drft::CraftingState::render(sf::RenderTarget& target)
{
	_craftingBackground.render(target);
	_craftingWindow.render(target);
	_craftingList.render(target);
	_requiresList.render(target);
	_popupStack.render(target);
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
		.fillColor = sf::Color(0,0,0,150),
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
	_craftingList.setPosition(VIEW.getCenter() - sf::Vector2f{146,0});
	_craftingList.setSize({ 64, CRAFTING_WINDOW_HEIGHT});
	_craftingList.setStyle(gui::ElementState::Idle, {
		.innerPadding = {16.f, 16.f},
		.childPadding = {0.f, 24.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color(150,150,150)
		});
	_craftingList.setStyle(gui::ElementState::Focused, {
		.innerPadding = {16.f, 16.f},
		.childPadding = {0.f, 24.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	_craftingList.setState(gui::ElementState::Focused);
	_craftingList.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);

	_requiresList.setPosition(VIEW.getCenter());
	_requiresList.setOrigin(gui::ElementPosition::CENTER_LEFT);
	_requiresList.setSize({ (CRAFTING_WINDOW_WIDTH / 2), CRAFTING_WINDOW_HEIGHT });
	_requiresList.setStyle(gui::ElementState::Idle, {
		.innerPadding = {16.f, 16.f},
		.childPadding = {0.f, 24.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color(150,150,150)
		});
	_requiresList.setStyle(gui::ElementState::Focused, {
		.outlineColor = sf::Color(100,100,100,100),
		.outlineThickness = 1.f,
		.innerPadding = {16.f, 16.f},
		.childPadding = {0.f, 24.f},
		.font = &getContext().fonts.get("Terminus"),
		.textColor = sf::Color::White
		});
	_requiresList.setState(gui::ElementState::Focused);
	_requiresList.setChildrenOrigin(gui::ElementPosition::TOP_LEFT);

	refreshCraftingList();
}

void drft::CraftingState::refreshCraftingList()
{
	_craftingList.clear();
	_requiresList.clear();

	const auto craftableItems = getContext().registry.try_get<component::MyCraftableItems>(_sessionEntity);
	const auto& factory = getContext().registry.ctx().get<const EntityFactory&>();
	const auto& prototypeReg = factory.prototypes();

	if (craftableItems && (!craftableItems->craftables.empty() || !craftableItems->partialCraftables.empty()))
	{
		// Determine which materials the session entity has
		std::unordered_map<std::string, int> materialCount;
		const auto& container = getContext().registry.get<component::Container>(_sessionEntity);
		for (auto itemID : container.contents)
		{
			auto itemEntity = util::ItemIDToEntityID(itemID, getContext().registry);
			++materialCount[util::getEntityName({ getContext().registry, itemEntity })];
		}

		const auto& sprites = getContext().textures.get("Sprites");
		int count = 0;
		int matCount = 0;
		const int numCraftables = craftableItems->craftables.size();

		auto craftables = std::vector<entt::entity>(craftableItems->craftables);
		craftables.insert(craftables.end(), craftableItems->partialCraftables.begin(), craftableItems->partialCraftables.end());

		const sf::Color craftableIdle = sf::Color::White;
		const sf::Color craftableFocused = sf::Color::Yellow;
		const sf::Color uncraftableIdle = sf::Color(80, 80, 80);
		const sf::Color uncraftableFocused = sf::Color(120, 120, 120);

		// Insert craftable items into list
		for (auto craftable : craftables)
		{
			const auto craftableName = util::getEntityName({ prototypeReg, craftable });
			const auto& recipe = prototypeReg.get<component::Craftable>(factory.get(craftableName)).recipe;

			std::string str = std::to_string(count);
			_craftingList.insert(str.data(), gui::DualContainer())
				.setStyle(gui::ElementState::Idle, {
						.childPadding = {12, 0}
					})
				.setStyle(gui::ElementState::Focused, {
						.childPadding = {12, 0}
					})
				.setStyle(gui::ElementState::Unselectable, {
						.childPadding = {12, 0}
					})
				.setStyle(gui::ElementState::FocusedUnselectable, {
						.childPadding = {12, 0}
					})
				.registerCallback(gui::ElementCallbackType::OnIsSelectable,
					[count, numCraftables]() {
						return count < numCraftables;
					})
				.registerCallback(gui::ElementCallbackType::OnSelect,
					[this, craftableName, &recipe]() -> bool
					{
						component::action::Craft toCraft;
						toCraft.itemName = craftableName;
						for (auto [matName, amount] : recipe)
						{
							toCraft.recipe[matName] = amount;
						}
						getContext().registry.emplace_or_replace<component::action::Craft>(_sessionEntity, toCraft);
						_requiresRefresh = true;
						_popupStack.insert("Message", gui::PopupMessage())
							.setPosition(getContext().window.getView().getCenter())
							.setStyle(gui::ElementState::Focused, {
								.fillColor = sf::Color(0,0,0,255),
								.outlineColor = sf::Color(255,255,255,150),
								.outlineThickness = 1.f,
								.innerPadding = {2.f, 2.f},
								.font = &getContext().fonts.get("Terminus"),
								.textColor = sf::Color::White
								})
							.setTextString(util::getEntityName({ getContext().registry, _sessionEntity }) + " crafted the "
								+ craftableName + ".");

						return true;
					});

					const auto& itemRender = prototypeReg.get<component::Render>(craftable);
					sf::Sprite sprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(itemRender.sprite), sprites) };
					_craftingList[str.data()].insert("Icon", gui::Icon(sprite))
						.setSize({ 16, 16 })
						.setStyle(gui::ElementState::Idle, {
							.fillColor = itemRender.color
							})
						.setStyle(gui::ElementState::Focused, {
							.fillColor = itemRender.color
							})
						.setStyle(gui::ElementState::Unselectable, {
							.fillColor = itemRender.color
							})
						.setStyle(gui::ElementState::FocusedUnselectable, {
							.fillColor = itemRender.color
							});


					_craftingList[str.data()].insert("Label", gui::Label())
						.setStyle(gui::ElementState::Idle, {
							.font = &getContext().fonts.get("Terminus"),
							.textColor = craftableIdle,
							.textSize = 16
							})
						.setStyle(gui::ElementState::Focused, {
							.font = &getContext().fonts.get("Terminus"),
							.textColor = craftableFocused,
							.textSize = 16
							})
						.setStyle(gui::ElementState::Unselectable, {
							.font = &getContext().fonts.get("Terminus"),
							.textColor = uncraftableIdle,
							.textSize = 16
							})
						.setStyle(gui::ElementState::FocusedUnselectable, {
							.font = &getContext().fonts.get("Terminus"),
							.textColor = uncraftableFocused,
							.textSize = 16
							})
						.setTextString(craftableName.data())
						.setTextOrigin(gui::ElementPosition::CENTER_LEFT);

					_requiresList.insert(str.data(), gui::MultiContainer())
						.setStyle(gui::ElementState::Idle, {
								.childPadding = {48, 0}
							})
						.setStyle(gui::ElementState::Focused, {
								.childPadding = {48, 0}
							});

					for (auto [matName, amount] : recipe)
					{
						const auto& matRender = prototypeReg.get<component::Render>(factory.get(matName));
						sf::Sprite matSprite = { sprites, util::SpriteIndexer::get(static_cast<util::Sprite>(matRender.sprite), sprites) };
						std::string matstr = std::to_string(matCount);
						sf::Color numberColor = sf::Color::White;
						if (materialCount[matName] < static_cast<int>(amount))
						{
							numberColor = sf::Color(80, 80, 80);
						}
						_requiresList[str.data()].insert(matstr.data(), gui::Icon(matSprite))
							.setSize({ 16, 16 })
							.setStyle(gui::ElementState::Idle, {
								.fillColor = matRender.color,
								.font = &getContext().fonts.get("Terminus"),
								.textColor = numberColor,
								.textSize = 16
								})
							.setStyle(gui::ElementState::Focused, {
								.fillColor = matRender.color,
								.font = &getContext().fonts.get("Terminus"),
								.textColor = numberColor,
								.textSize = 16
								})
							.setTextString(std::format("{}/{}", materialCount[matName], amount))
							.setTextOrigin(gui::ElementPosition::CENTER_LEFT)
							.setTextPosition(gui::ElementPosition::CENTER_RIGHT)
							.setState(gui::ElementState::Idle);

						++matCount;
					}

					++count;
		}
	}
	else
	{
		_craftingList.insert("Nothing", gui::Label())
			.setStyle(gui::ElementState::Idle, {
				.font = &getContext().fonts.get("Terminus"),
				.textColor = sf::Color(80,80,80),
				.textSize = 16
				})
			.setStyle(gui::ElementState::Focused, {
				.font = &getContext().fonts.get("Terminus"),
				.textColor = sf::Color(80,80,80),
				.textSize = 16
				})
			.setStyle(gui::ElementState::Unselectable, {
				.font = &getContext().fonts.get("Terminus"),
				.textColor = sf::Color(80,80,80),
				.textSize = 16
				})
			.setStyle(gui::ElementState::FocusedUnselectable, {
				.font = &getContext().fonts.get("Terminus"),
				.textColor = sf::Color(80,80,80),
				.textSize = 16
				})
			.setTextString("No Materials.")
			.setTextOrigin(gui::ElementPosition::CENTER_LEFT);
	}

	_requiresRefresh = false;
}

