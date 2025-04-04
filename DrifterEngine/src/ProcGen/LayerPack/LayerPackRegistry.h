#pragma once
#include <ProcGen/LayerPack/LayerPack.h>

class LayerPackRegistry
{
public:
	void loadLayerPacks(const std::filesystem::path& entityPackDirectory);

	const LayerPack& get(entt::id_type id) const;

private:
	std::unordered_map<entt::id_type, LayerPack> _packs;
};