#ifndef GRID_HPP
#define GRID_HPP

#include "Point2.hpp"
#include <QPainter>
#include "UnitSquareMapping.hpp"

class Grid
{
public:
	Grid(int newSize);
	~Grid();

	int& size();
	void draw(QPainter &painter);

private:
	int _size;
	int _cellSize;
};

#endif // GRID_HPP
