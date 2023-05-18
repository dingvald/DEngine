#pragma once

namespace drft::util
{
	class SmoothTransition
	{
	public:
		SmoothTransition(float startVal, float endVal, float startOfRange, float endOfRange);
		float compute(float currentRange) const;

	private:
		const float _startVal = 0;
		const float _endVal = 0;
		const float _startOfRange = 0;
		const float _endOfRange = 0;
	};

	class SmoothColorTransition
	{
	public:
		SmoothColorTransition(sf::Color start, sf::Color end, float startOfRange, float endOfRange);
		sf::Color compute(float currentRange) const;

	private:
		SmoothTransition _r;
		SmoothTransition _g;
		SmoothTransition _b;
	};
}