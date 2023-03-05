#include <pch.h>
#include "Snapshot.h"
#include "Reflection.h"

namespace snapshot {

	void Snapshot::save(OutputArchive archive, entt::const_handle h)
	{
		archive(cereal::make_nvp("e_count", 1UL));
		saveHandle(archive, h);
	}

	void Snapshot::save(OutputArchive archive, std::vector<entt::entity>& entities, entt::registry const& reg)
	{
		auto sz = entities.size();
		archive(cereal::make_nvp("e_count", sz));
		for (auto& e : entities)
		{
			auto h = entt::const_handle{ reg, e };
			saveHandle(archive, h);
		}
	}

	void Snapshot::save(OutputArchive archive, entt::registry const& reg)
	{
		auto sz = reg.size();

		archive(cereal::make_nvp("e_count", sz));

		for (auto it = reg.data(), last = it + sz; it != last; ++it) {
			auto h = entt::const_handle{ reg, *it };
			saveHandle(archive, h);
		}
	}

	void Snapshot::saveHandle(OutputArchive& archive, entt::const_handle h)
	{
		auto e = h.entity();

		auto e_serial = detail::SerializeHandleEntity{ .e = e, .components = std::vector<Handle>{} };

		for (auto [id, storage] : h.storage())
		{
			auto refl_comp = ComponentReflection{ storage.type() };
			auto serializable = entt::resolve(storage.type()).prop(SERIALIZABLE_TAG);
			if (refl_comp && serializable)
			{
				e_serial.components.push_back(Handle{ refl_comp.get(h) });
			}
		}

		auto label = std::to_string((size_t)e);
		archive(cereal::make_nvp(label, e_serial));
	}

	void SnapshotLoader::load(InputArchive archive, entt::handle h)
	{
		{
			auto sz = 0UL;
			archive(sz);
		}

		loadHandle(archive, h);
	}

	void SnapshotLoader::load(InputArchive archive, entt::registry& reg)
	{
		auto sz = 0UL;
		archive(sz);

		for (auto i = 0UL; i < sz; ++i) {
			loadHandle(archive, reg);
		}
	}

	void SnapshotLoader::loadHandle(InputArchive archive, entt::registry& reg)
	{
		auto serial_e = detail::SerializeEntity{};
		archive(serial_e);

		auto h = entt::handle{ reg, reg.create(serial_e.e) };

		for (auto& comp : serial_e.components) {
			auto comp_name = comp.reflection().name().data();
			comp.componentReflection().emplace(h, *comp);

		}
	}

	void SnapshotLoader::loadHandle(InputArchive archive, entt::handle h)
	{
		auto serial_e = detail::SerializeEntity{};
		archive(serial_e);

		for (auto& comp : serial_e.components) {
			auto comp_name = comp.reflection().name().data();
			comp.componentReflection().emplace(h, *comp);
		}
	}

} // namespace snapshot