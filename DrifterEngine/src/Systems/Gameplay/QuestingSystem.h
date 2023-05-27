#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class QuestingSystem : public System
	{
	public:
		void init() override;
		void onStart(bool isNewGame) override;
		void fixedUpdate() override;
		void save(cereal::JSONOutputArchive& oarchive);
		void load(cereal::JSONInputArchive& iarchive);

	private:
		void setNewGoalPosition();
		void buildGoalEntity();

	private:
		bool _isGoalBuilt = false;
		sf::Vector2i _goalPosition;
	};
}


