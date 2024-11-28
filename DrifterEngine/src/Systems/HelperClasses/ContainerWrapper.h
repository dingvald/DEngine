#pragma once
#include <Components/ItemComponent.h>

struct ContainerComponent;

namespace drft
{
	enum class SortType
	{
		Alphabetic,
	};

	class ContainerWrapper
	{
	public:
		void setContainer(entt::handle entity);
		bool isValid() const;

		void sort(SortType sortType = SortType::Alphabetic);

		const std::vector<ItemComponent::ID>& getItems() const;

	private:
		ContainerComponent* _container;
		entt::registry* _registry;
	};
}