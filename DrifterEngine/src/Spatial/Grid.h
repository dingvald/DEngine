#pragma once
#include <vector>
#include "SFML/Graphics/Rect.hpp"

namespace drft::spatial
{
	template<typename T>
	class Grid
	{
	public:
		Grid() = default;
		Grid(int width, int height);
		Grid(int width, int height, std::initializer_list<T> list);

		void resize(int width, int height);
		void fill(T val);
		void fill(std::function<T(int, int)> fillFunc);
		void fill(T val, int left, int top, int width, int height);

		bool contains(int x, int y) const;

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



