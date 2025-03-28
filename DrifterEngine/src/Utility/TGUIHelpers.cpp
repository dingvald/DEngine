#include "pch.h"
#include "TGUIHelpers.h"
#include <Utility/ColorToHexString.h>

namespace
{
	tgui::UIntRect toUIntRect(sf::IntRect rect)
	{
		return {
			static_cast<unsigned int>(rect.position.x),
			static_cast<unsigned int>(rect.position.y),
			static_cast<unsigned int>(rect.size.x),
			static_cast<unsigned int>(rect.size.y),
		};
	}
}


tgui::Texture drft::GuiHelpers::createTGUITextureFromUV(const std::string& id, sf::IntRect uv)
{
	return tgui::Texture{ id, toUIntRect(uv) };
}

void drft::GuiHelpers::setupNavigationGraph(tgui::PanelListBox::Ptr list)
{
	if (list->getItemCount() == 0) return;

	tgui::Panel::Ptr previous = list->getItemByIndex(list->getItemCount() - 1);
	for (auto&& panel : list->getItems())
	{
		panel->setNavigationUp(previous);
		previous->setNavigationDown(panel);
		previous = panel;
	}
}

void drft::GuiHelpers::tryFocusItem(tgui::PanelListBox::Ptr list, size_t index)
{
	if (auto item = list->getItemByIndex(index))
	{
		item->setFocused(true);
	}
}

tgui::Button::Ptr drft::GuiHelpers::buttonizePanel(tgui::Panel::Ptr panel)
{
	auto button = tgui::Button::create();
	panel->add(button, "buttonized_panel");
	button->setSize(tgui::bindSize(panel));
	button->moveToBack();

	return button;
}

tgui::Button::Ptr drft::GuiHelpers::buttonizePanel(tgui::Panel::Ptr panel, tgui::Label::Ptr text)
{
	auto button = tgui::Button::create();
	auto defaultTextColor = text->getRenderer()->getTextColor();
	panel->add(button, "buttonized_panel");
	panel->onFocus([button, text]() { 
		button->setFocused(true);
		auto textColor = button->getRenderer()->getTextColorFocused();
		text->getRenderer()->setTextColor(textColor);
		});
	panel->onUnfocus([button, text, defaultTextColor]() {
		button->setFocused(false); 
		text->getRenderer()->setTextColor(defaultTextColor);
		});
	button->setSize("100%, 100%");
	button->moveToBack();

	return button;
}

int drft::GuiHelpers::getFocusedIndex(tgui::PanelListBox::Ptr list)
{
	if (list->getItemCount() == 0) return -1;
	if (auto focused = list->getFocusedChild())
	{
		if (auto panel = focused->cast<tgui::Panel>())
		{
			return list->getIndexByItem(panel);
		}
	}
	return -1;
}

std::string drft::GuiHelpers::colorizedString(const std::string& str, const sf::Color& color)
{
	return std::format("<color={}>{}</color>", drft::util::colorToHexString(color), str);
}
