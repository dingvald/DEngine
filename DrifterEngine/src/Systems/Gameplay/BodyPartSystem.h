#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class BodyPartSystem : public System
	{
	public:
		void init() override;
		void update(const float dt) override;

	private:
		int calculateDamageFromEquipped(unsigned long itemID);
		std::string& determinePartHit(std::unordered_map<std::string, unsigned long>& parts);
	};

}


