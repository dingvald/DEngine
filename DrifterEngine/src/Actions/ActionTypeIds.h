#pragma once

enum class ActionID
{
	None,

	Wait,
	MoveNorth,
	MoveSouth,
	MoveWest,
	MoveEast,
	MoveNorthWest,
	MoveSouthWest,
	MoveNorthEast,
	MoveSouthEast,

	MeleeAttackNorth,
	MeleeAttackSouth,
	MeleeAttackWest,
	MeleeAttackEast,
	MeleeAttackNorthWest,
	MeleeAttackSouthWest,
	MeleeAttackNorthEast,
	MeleeAttackSouthEast,

	Drop,
	PickUp,

	ContextualInteract,
	InteractNorth,
	InteractSouth,
	InteractWest,
	InteractEast,
	InteractNorthWest,
	InteractSouthWest,
	InteractNorthEast,
	InteractSouthEast,
};

