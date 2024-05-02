#pragma once
#include "ISpawningAlgorithm.h"

namespace drft::gen
{
	class SpawningAlgorithmRegistry
	{
	public:
		const ISpawningAlgorithm& get(const std::string& name) const;
		bool contains(const std::string& name) const;

		template<typename T>
		void registerAlgorithm(std::string name)
		{
			static_assert(std::is_base_of<ISpawningAlgorithm, T>::value, "Type must be derived from ISpawningAlgorithm");
			if (_spawningAlgorithms.contains(name)) throw std::exception("Already contains algorithm name");
			_spawningAlgorithms.emplace(std::move(name), std::make_unique<T>());
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<ISpawningAlgorithm>> _spawningAlgorithms;
	};
}


