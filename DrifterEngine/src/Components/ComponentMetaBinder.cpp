#include "pch.h"
#include "ComponentMetaBinder.h"

#include "AbilitiesComponent.h"
#include "ActorComponent.h"
#include "AIComponent.h"
#include "AnimationComponent.h"
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
#include "PlayerHasSeenComponent.h"
#include "PositionComponent.h"
#include "ProjectileComponent.h"
#include "PrototypeComponent.h"
#include "RenderComponent.h"
#include "SharpComponent.h"
#include "SkillsComponent.h"
#include "SolidMaterialComponent.h"
#include "SprintingComponent.h"
#include "SpriteControllerComponent.h"
#include "StairsComponent.h"
#include "StaminaComponent.h"
#include "SyncedAnimationComponent.h"
#include "TemporaryLightSourceComponent.h"
#include "TickingLifetimeComponent.h"
#include "UtilityAIComponent.h"
#include "VisionComponent.h"
#include "VisualEffectComponent.h"
#include "VolumeComponent.h"
#include "WearableComponent.h"
#include "WeightComponent.h"

void ComponentMetaBinder::bindComponents()
{
	AbilitiesComponent::bind(_ctx);
	ActorComponent::bind(_ctx);
	AIComponent::bind(_ctx);
	AnimationComponent::bind(_ctx);
	BaseMaterialComponent::bind(_ctx);
	BodyComponent::bind(_ctx);
	ChunkSourceTrackerComponent::bind(_ctx);
	ContainerComponent::bind(_ctx);
	ConsumableComponent::bind(_ctx);
	CraftableComponent::bind(_ctx);
	DateAndTimeTrackerComponent::bind(_ctx);
	DescriptionComponent::bind(_ctx);
	DominantSideComponent::bind(_ctx);
	FactionComponent::bind(_ctx);
	FlyingComponent::bind(_ctx);
	GasMaterialComponent::bind(_ctx);
	GlobalLightSourceComponent::bind(_ctx);
	HealingComponent::bind(_ctx);
	HealthComponent::bind(_ctx);
	HotbarComponent::bind(_ctx);
	InheritanceComponent::bind(_ctx);
	InLiquidComponent::bind(_ctx);
	ItemComponent::bind(_ctx);
	ItemIDTrackerComponent::bind(_ctx);
	LevelingComponent::bind(_ctx);
	LightBlockingComponent::bind(_ctx);
	LiquidMaterialComponent::bind(_ctx);
	LitComponent::bind(_ctx);
	LocalLightSourceComponent::bind(_ctx);
	MaterialCompositionComponent::bind(_ctx);
	OpenableComponent::bind(_ctx);
	PaperdollLayoutComponent::bind(_ctx);
	PhysicalBlockingComponent::bind(_ctx);
	PlayerInputComponent::bind(_ctx);
	PlayerHasSeenComponent::bind(_ctx);
	PositionComponent::bind(_ctx);
	ProjectileComponent::bind(_ctx);
	PrototypeComponent::bind(_ctx);
	RenderComponent::bind(_ctx);
	SharpComponent::bind(_ctx);
	SkillsComponent::bind(_ctx);
	SolidMaterialComponent::bind(_ctx);
	SprintingComponent::bind(_ctx);
	SpriteControllerComponent::bind(_ctx);
	StairsComponent::bind(_ctx);
	StaminaComponent::bind(_ctx);
	SyncedAnimationComponent::bind(_ctx);
	TemporaryLightSourceComponent::bind(_ctx);
	TickingLifetimeComponent::bind(_ctx);
	UtilityAIComponent::bind(_ctx);
	VisionComponent::bind(_ctx);
	VisualEffectComponent::bind(_ctx);
	VolumeComponent::bind(_ctx);
	WearableComponent::bind(_ctx);
	WeightComponent::bind(_ctx);
}

entt::meta_ctx& ComponentMetaBinder::cxt()
{
	return _ctx;
}
