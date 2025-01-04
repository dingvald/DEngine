#pragma once
#include <AI/Utility/IUtilityInputProvider.h>

class AIDataClearingHouse : public IUtilityInputProvider<entt::entity>
{
public:
	using InputProviderFunction = std::function<float(entt::const_handle, entt::const_handle)>;
	AIDataClearingHouse(entt::registry& registry);

	float getInput(UtilityInputID inputID, entt::entity actor, entt::entity target) const override;

	void registerInput(UtilityInputID id, InputProviderFunction func);

private:
	entt::registry& _registry;
	std::unordered_map<UtilityInputID, InputProviderFunction> _inputFunctions;
};