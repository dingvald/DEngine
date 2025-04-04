#include "pch.h"
#include "LayerFactory.h"

#include <ProcGen/Layers/RandomLayer.h>
#include <ProcGen/Layers/PerlinNoiseLayer.h>
#include <ProcGen/Layers/LLoydRelaxedLayer.h>
#include <ProcGen/Layers/FillLayer.h>

void LayerFactory::bind()
{
	using namespace entt::literals;
	registerLayer<drft::RandomLayer>("random"_hs);
	registerLayer<drft::PerlinNoiseLayer>("perlin"_hs);
	registerLayer<drft::LloydRelaxedLayer>("relaxed_points"_hs);
	registerLayer<drft::FillLayer>("fill"_hs);
}

std::unique_ptr<drft::OnDemandLayer> LayerFactory::build(entt::id_type id, const rapidjson::Value& json) const
{
	if (!_factories.contains(id))
	{
		return nullptr;
	}
	auto layer = _factories.at(id)();
	layer->createFromJson(json);
	return std::move(layer);
}
