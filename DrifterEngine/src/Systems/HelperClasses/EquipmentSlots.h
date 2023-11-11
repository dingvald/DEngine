#pragma once

enum class EquipmentLayer
{
	Held,
	Base,
	Outer,
	OverAll
};

enum class EquipmentSlot
{
	Head,
	Torso,
	Arm,
	Hand,
	Legs,
	Feet,

	Total
};

static inline const std::unordered_map<std::string, EquipmentSlot> String2EquipmentSlot =
{
	{"Head", EquipmentSlot::Head},
	{"Torso", EquipmentSlot::Torso},
	{"Arm", EquipmentSlot::Arm},
	{"Hand", EquipmentSlot::Hand},
	{"Legs", EquipmentSlot::Legs},
	{"Feet", EquipmentSlot::Feet}
};

static inline const std::unordered_map<EquipmentSlot, std::string> EquipmentSlot2String =
{
	{EquipmentSlot::Head, "Head"},
	{EquipmentSlot::Torso, "Torso"},
	{EquipmentSlot::Arm, "Arm" },
	{EquipmentSlot::Hand, "Hand" },
	{EquipmentSlot::Legs, "Legs" },
	{EquipmentSlot::Feet, "Feet" }
};

std::vector<EquipmentSlot> convertStringsToSlots(const std::vector<std::string>& slots);