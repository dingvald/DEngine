#include "pch.h"
#include "Erode.h"

drft::spatial::Grid<int> drft::gen::erode(const spatial::Grid<int>& subject, const spatial::Grid<int>& kernel, int erodedValue)
{
	spatial::Grid<int> result{ subject.width(), subject.height() };

	for (int row = 0; row < subject.height(); ++row)
	{
		for (int col = 0; col < subject.width(); ++col)
		{
			result.at(col, row) = subject.at(col, row);
			// iterate kernel
			for (int ky = 0; ky < kernel.height(); ++ky)
			{
				for (int kx = 0; kx < kernel.width(); ++kx)
				{
					if (kernel.at(kx, ky) == 0) continue;

					int offsetx = kx - 1;
					int offsety = ky - 1;
					if (row + offsety < 0 || row + offsety >= subject.height()
						|| col + offsetx < 0 || col + offsetx >= subject.width())
					{
						result.at(col, row) = erodedValue;
						goto EndKernel;
					}
					if (subject.at(col + offsetx, row + offsety) == erodedValue)
					{
						result.at(col, row) = erodedValue;
						goto EndKernel;
					}
				}
			}
		EndKernel: {}
		}
	}

	return result;
}
