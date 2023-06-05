#pragma once

namespace drft::util
{
	class Statistics
	{
	public:
		Statistics(unsigned int maxNumSamples);

		void sample(double val);
		double average() const;
		double max() const;
		double min() const;


	private:
		void reset();
	private:
		unsigned int _count = 0;
		unsigned int _maxNumSamples;
		double _average = 0.0;
		double _min = std::numeric_limits<double>::max();
		double _max = std::numeric_limits<double>::min();
	};
}