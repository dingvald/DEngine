#include "pch.h"
#include "ComponentMetaBinder.h"

#include "ActorComponent.h"
#include "AIComponent.h"
#include "AnimationComponent.h"
#include "AttackerComponent.h"
#include "BodyComponent.h"
#include "ChunkSourceTrackerComponent.h"
#include "ConsumableComponent.h"
#include "ContainerComponent.h"
#include "CraftableComponent.h"
#include "DateAndTimeTrackerComponent.h"
#include "DescriptionComponent.h"
#include "DominantSideComponent.h"
#include "FactionComponent.h"
#include "FlyingComponent.h"
#include "GlobalLightSourceComponent.h"
#include "HealingComponent.h"
#include "HealthComponent.h"
#include "HotbarComponent.h"
#include "InheritanceComponent.h"
#include "InLiquidComponent.h"
#include "ItemComponent.h"
#include "ItemIDTrackerComponent.h"
#include "LevelingComponent.h"
#include "LightBlockingComponent.h"
#include "LiquidComponent.h"
#include "LitComponent.h"
#include "LocalLightSourceComponent.h"
#include "MaterialComponent.h"
#include "OpenableComponent.h"
#include "PaperdollLayoutComponent.h"
#include "PhysicalBlockingComponent.h"
#include "PlayerInputComponent.h"
#include "PositionComponent.h"
#include "ProjectileComponent.h"
#include "PrototypeComponent.h"
#include "RenderComponent.h"
#include "SharpComponent.h"
#include "SkillsComponent.h"
#include "SprintingComponent.h"
#include "SpriteControllerComponent.h"
#include "StaminaComponent.h"
#include "SyncedAnimationComponent.h"
#include "TemporaryLightSourceComponent.h"
#include "TickingLifetimeComponent.h"
#include "VisionComponent.h"
#include "VisualEffectComponent.h"
#include "WearableComponent.h"

void ComponentMetaBinder::bindComponents()
{
	ActorComponent::bind();
	AIComponent::bind();
	AnimationComponent::bind();
	AttackerComponent::bind();
	BodyComponent::bind();
	ChunkSourceTrackerComponent::bind();
	ContainerComponent::bind();
	ConsumableComponent::bind();
	CraftableComponent::bind();
	DateAndTimeTrackerComponent::bind();
	DescriptionComponent::bind();
	DominantSideComponent::bind();
	FactionComponent::bind();
	FlyingComponent::bind();
	GlobalLightSourceComponent::bind();
	HealingComponent::bind();
	HealthComponent::bind();
	HotbarComponent::bind();
	InheritanceComponent::bind();
	InLiquidComponent::bind();
	ItemComponent::bind();
	ItemIDTrackerComponent::bind();
	LevelingComponent::bind();
	LightBlockingComponent::bind();
	LiquidComponent::bind();
	LitComponent::bind();
	LocalLightSourceComponent::bind();
	MaterialComponent::bind();
	OpenableComponent::bind();
	PaperdollLayoutComponent::bind();
	PhysicalBlockingComponent::bind();
	PlayerInputComponent::bind();
	PositionComponent::bind();
	ProjectileComponent::bind();
	PrototypeComponent::bind();
	RenderComponent::bind();
	SharpComponent::bind();
	SkillsComponent::bind();
	SprintingComponent::bind();
	SpriteControllerComponent::bind();
	StaminaComponent::bind();
	SyncedAnimationComponent::bind();
	TemporaryLightSourceComponent::bind();
	TickingLifetimeComponent::bind();
	VisionComponent::bind();
	VisualEffectComponent::bind();
	WearableComponent::bind();
}
