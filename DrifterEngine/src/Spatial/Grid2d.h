#pragma once
#include <vector>
#include <functional>

namespace drft::spatial
{
	template<typename T>
	class Grid2d
	{
	public:
		Grid2d() = default;
		Grid2d(int width, int height);
		Grid2d(int width, int height, std::initializer_list<T> list);

		void resize(int width, int height);
		void fill(T val);
		void fill(std::function<T(int, int)> fillFunc);
		void fill(T val, int left, int top, int width, int height);

		bool contains(int x, int y) const;

		T& at(int x, int y);
		const T& at(int x, int y) const;

		// Iterates through the grid and performs the given function
		// The function provides the x and y positions and the value
		void forEach(std::function<void(int, int, T)> func) const;

		int width() const;
		int height() const;

		void clear();

	private:
		std::vector<T> _cells;
		int _width;
		int _height;
	};

} // namespace drft

#include "Grid2d.inl"



