#include "pch.h"
#include "ComponentMetaBinder.h"

#include "Components/ActorComponent.h"
#include "Components/AIComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/AttackerComponent.h"
#include "Components/BodyComponent.h"
#include "Components/ConsumableComponent.h"
#include "Components/ContainerComponent.h"
#include "Components/DescriptionComponent.h"
#include "Components/FactionComponent.h"
#include "Components/FlyingComponent.h"
#include "Components/GlobalLightSourceComponent.h"
#include "Components/HealingComponent.h"
#include "Components/HealthComponent.h"
#include "Components/InheritanceComponent.h"
#include "Components/InLiquidComponent.h"
#include "Components/ItemComponent.h"
#include "Components/LevelingComponent.h"
#include "Components/LightBlockingComponent.h"
#include "Components/LiquidComponent.h"
#include "Components/LitComponent.h"
#include "Components/LocalLightSourceComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/MyCraftableItemsComponent.h"
#include "Components/OpenableComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/PositionComponent.h"
#include "Components/ProjectileComponent.h"
#include "Components/PrototypeComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SharpComponent.h"
#include "Components/SprintingComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/TemporaryLightSourceComponent.h"
#include "Components/TickingLifetimeComponent.h"
#include "Components/VisualEffectComponent.h"
#include "Components/WearableComponent.h"

void ComponentMetaBinder::bindComponents()
{
	ActorComponent::bind();
	AIComponent::bind();
	AnimationComponent::bind();
	AttackerComponent::bind();
	BodyComponent::bind();
	ContainerComponent::bind();
	ContainerComponent::bind();
	DescriptionComponent::bind();
	FactionComponent::bind();
	FlyingComponent::bind();
	GlobalLightSourceComponent::bind();
	HealingComponent::bind();
	HealthComponent::bind();
	InheritanceComponent::bind();
	InLiquidComponent::bind();
	ItemComponent::bind();
	LevelingComponent::bind();
	LightBlockingComponent::bind();
	LiquidComponent::bind();
	LitComponent::bind();
	LocalLightSourceComponent::bind();
	MaterialComponent::bind();
	MyCraftableItemsComponent::bind();
	OpenableComponent::bind();
	PlayerComponent::bind();
	PositionComponent::bind();
	ProjectileComponent::bind();
	PrototypeComponent::bind();
	RenderComponent::bind();
	SharpComponent::bind();
	SprintingComponent::bind();
	StaminaComponent::bind();
	TemporaryLightSourceComponent::bind();
	TickingLifetimeComponent::bind();
	VisualEffectComponent::bind();
	WearableComponent::bind();
}
