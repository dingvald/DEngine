#pragma once

enum class MouseContextualActionState
{
	Move,
	Attack,
	Interact
};

struct MouseCursorComponent
{
	MouseContextualActionState actionState = MouseContextualActionState::Move;
};