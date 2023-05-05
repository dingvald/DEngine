#pragma once
#include <vector>

namespace drft::spatial
{
	template<typename T>
	class Grid
	{
	public:
		Grid() = default;
		Grid(int width, int height);

		void resize(int width, int height);
		void fill(T val);

		T& at(int x, int y);
		const T& at(int x, int y) const;

		int width() const;
		int height() const;

		void clear();

	private:
		std::vector<T> _cells;
		int _width;
		int _height;
	};

} // namespace drft

#include "Grid.inl"



