#pragma once
#include <Components/Wrappers/ComponentWrapper.h>
#include <Components/BodyComponent.h>

namespace drft
{
	class BodyWrapper : public ComponentWrapper<BodyComponent>
	{
	public:
		bool canEquip(const std::string& slot, entt::entity item) const;
		bool equip(const std::string& slot, entt::entity item);
		bool unequip(const std::string& slot);
		entt::entity swap(const std::string& slot, entt::entity item);

		bool hasSlot(const std::string& slot) const;
		const BodyPart::Slot* getSlot(const std::string& slot) const;

		entt::const_handle getItemInDominantHand() const;
	};
}