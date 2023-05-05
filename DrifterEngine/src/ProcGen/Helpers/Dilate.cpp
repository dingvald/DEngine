#include "pch.h"
#include "Dilate.h"

drft::spatial::Grid<int> drft::gen::dilate(const spatial::Grid<int>& subject, const spatial::Grid<int>& kernel, int dilatedValue)
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
					if (subject.at(col, row) == dilatedValue)
					{
						int offsetx = kx - 1;
						int offsety = ky - 1;
						if (row + offsety < 0 || row + offsety >= subject.height()
							|| col + offsetx < 0 || col + offsetx >= subject.width())
						{
							continue;
						}
						result.at(col + offsetx, row + offsety) = dilatedValue;
					}
				}
			}
		}
	}

	return result;
}
