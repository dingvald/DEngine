#pragma once
#include <string>
#include <map>
#include <memory>

template <typename Resource, typename Identifier>
class ResourceHolder
{
public:
	void load(Identifier id, const std::filesystem::path& filename);
	template <typename Parameter>
	void load(Identifier id, const std::filesystem::path&, const Parameter& param);
	Resource& get(Identifier id);
	const Resource& get(Identifier id) const;

private:
	std::map<Identifier, std::unique_ptr<Resource> > _resourceMap;
};

#include "ResourceHolder.inl"
