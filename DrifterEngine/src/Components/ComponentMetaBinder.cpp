#include "pch.h"
#include "ComponentMetaBinder.h"

#include "ActorComponent.h"
#include "AIComponent.h"
#include "AnimationComponent.h"
#include "AttackerComponent.h"
#include "BaseMaterialComponent.h"
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
#include "GasMaterialComponent.h"
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
#include "LiquidMaterialComponent.h"
#include "LitComponent.h"
#include "LocalLightSourceComponent.h"
#include "MaterialCompositionComponent.h"
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
#include "SolidMaterialComponent.h"
#include "SprintingComponent.h"
#include "SpriteControllerComponent.h"
#include "StaminaComponent.h"
#include "SyncedAnimationComponent.h"
#include "TemporaryLightSourceComponent.h"
#include "TickingLifetimeComponent.h"
#include "VisionComponent.h"
#include "VisualEffectComponent.h"
#include "VolumeComponent.h"
#include "WearableComponent.h"
#include "WeightComponent.h"

void ComponentMetaBinder::bindComponents()
{
	ActorComponent::bind();
	AIComponent::bind();
	AnimationComponent::bind();
	AttackerComponent::bind();
	BaseMaterialComponent::bind();
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
	GasMaterialComponent::bind();
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
	LiquidMaterialComponent::bind();
	LitComponent::bind();
	LocalLightSourceComponent::bind();
	MaterialCompositionComponent::bind();
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
	SolidMaterialComponent::bind();
	SprintingComponent::bind();
	SpriteControllerComponent::bind();
	StaminaComponent::bind();
	SyncedAnimationComponent::bind();
	TemporaryLightSourceComponent::bind();
	TickingLifetimeComponent::bind();
	VisionComponent::bind();
	VisualEffectComponent::bind();
	VolumeComponent::bind();
	WearableComponent::bind();
	WeightComponent::bind();
}
