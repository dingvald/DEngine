#pragma once

enum class CurrentActorState
{
	Pending,
	InProgress,
	Complete
};

struct CurrentActorComponent
{
	CurrentActorState state = CurrentActorState::Pending;
	int pointsSpent = 0;
	int ticks = 0;
};