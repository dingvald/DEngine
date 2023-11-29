#pragma once

namespace drft::goap
{
	enum class SensorType
	{
		Internal,
		Touch,
		Visual,
		Auditory,
		Olfactory,
		Sixth
	};

	static std::unordered_map<SensorType, int> SensorMemory =
	{
		{SensorType::Internal, 0},
		{SensorType::Touch, 1},
		{SensorType::Visual, 5},
		{SensorType::Auditory, 3},
		{SensorType::Olfactory, 5},
		{SensorType::Sixth, 10}
	};
}