#pragma once
#include "ISpawningPredicate.h"

namespace drft::gen
{
	class SpawningPredicateRegistry
	{
	public:
		const ISpawningPredicate& get(const std::string& name) const;
		bool contains(const std::string& name) const;

		template<typename T>
		void registerAlgorithm(std::string name)
		{
			static_assert(std::is_base_of<ISpawningPredicate, T>::value, "Type must be derived from ISpawningPredicate");
			if (_spawningPredicates.contains(name)) throw std::exception("Already contains algorithm name");
			_spawningPredicates.emplace(std::move(name), std::make_unique<T>());
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<ISpawningPredicate>> _spawningPredicates;
	};
}


