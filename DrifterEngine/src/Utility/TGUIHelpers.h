#pragma once

namespace drft::GuiHelpers
{
	tgui::Texture createTGUITextureFromUV(const std::string& id, sf::IntRect uv);

	void setupNavigationGraph(tgui::PanelListBox::Ptr list);

	void tryFocusItem(tgui::PanelListBox::Ptr list, size_t index);

	tgui::Button::Ptr buttonizePanel(tgui::Panel::Ptr panel);

	tgui::Button::Ptr buttonizePanel(tgui::Panel::Ptr panel, tgui::Label::Ptr text);

	int getFocusedIndex(tgui::PanelListBox::Ptr list);

	std::string colorizedString(const std::string& str, const sf::Color& color);
}