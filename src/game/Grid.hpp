#ifndef GRID_HPP
#define GRID_HPP

#include "Point2.hpp"
#include <QPainter>
#include "UnitSquareMapping.hpp"

class Cell
{
	void draw(QPainter &painter);
};

class Grid
{
public:
	Grid(int newSize, const UnitSquareMapping *newMapping);
	~Grid();

	int& size();
	void draw(QPainter &painter);
	double getHeight(int x, int y) const;

private:
	int _size;

	const double xStep;
	const double yStep;

	const UnitSquareMapping *mapping;
};

#endif // GRID_HPP
