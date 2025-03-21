#pragma once
#include <Components/ItemComponent.h>
#include <Components/Wrappers/ComponentWrapper.h>

struct ContainerComponent;

namespace drft
{
	enum class SortType
	{
		Alphabetic,
	};

	class ContainerWrapper : public ComponentWrapper<ContainerComponent>
	{
	public:
		void sort(SortType sortType = SortType::Alphabetic);
		bool remove(entt::entity item);
		bool add(entt::entity item);
		bool addBefore(entt::entity itemToAdd, size_t index);
		bool addAfter(entt::entity itemToAdd, size_t index);
		const std::vector<ItemComponent::ID>& getItems() const;
	};
}