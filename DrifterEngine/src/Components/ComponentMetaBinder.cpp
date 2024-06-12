#include "pch.h"
#include "ComponentMetaBinder.h"

#include "ActorComponent.h"
#include "AIComponent.h"
#include "AnimationComponent.h"
#include "AttackerComponent.h"
#include "BodyComponent.h"
#include "ConsumableComponent.h"
#include "ContainerComponent.h"
#include "CraftableComponent.h"
#include "DescriptionComponent.h"
#include "FactionComponent.h"
#include "FlyingComponent.h"
#include "GlobalLightSourceComponent.h"
#include "HealingComponent.h"
#include "HealthComponent.h"
#include "InheritanceComponent.h"
#include "InLiquidComponent.h"
#include "ItemComponent.h"
#include "LevelingComponent.h"
#include "LightBlockingComponent.h"
#include "LiquidComponent.h"
#include "LitComponent.h"
#include "LocalLightSourceComponent.h"
#include "MaterialComponent.h"
#include "OpenableComponent.h"
#include "PlayerComponent.h"
#include "PositionComponent.h"
#include "ProjectileComponent.h"
#include "PrototypeComponent.h"
#include "RenderComponent.h"
#include "SharpComponent.h"
#include "SprintingComponent.h"
#include "SpriteControllerComponent.h"
#include "StaminaComponent.h"
#include "SyncedAnimationComponent.h"
#include "TemporaryLightSourceComponent.h"
#include "TickingLifetimeComponent.h"
#include "VisualEffectComponent.h"
#include "WearableComponent.h"

void ComponentMetaBinder::bindComponents()
{
	ActorComponent::bind();
	AIComponent::bind();
	AnimationComponent::bind();
	AttackerComponent::bind();
	BodyComponent::bind();
	ContainerComponent::bind();
	ConsumableComponent::bind();
	CraftableComponent::bind();
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
	OpenableComponent::bind();
	PlayerComponent::bind();
	PositionComponent::bind();
	ProjectileComponent::bind();
	PrototypeComponent::bind();
	RenderComponent::bind();
	SharpComponent::bind();
	SprintingComponent::bind();
	SpriteControllerComponent::bind();
	StaminaComponent::bind();
	SyncedAnimationComponent::bind();
	TemporaryLightSourceComponent::bind();
	TickingLifetimeComponent::bind();
	VisualEffectComponent::bind();
	WearableComponent::bind();
}
