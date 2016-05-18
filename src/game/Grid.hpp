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
	void drawCell( QPainter&, const int, const int, double, double, double ) const;
	int getCellIndex(const double, const double);

private:
	int _size;

	const double xStep;
	const double yStep;

	const int imgHeight;

	const UnitSquareMapping *mapping;
};

#endif // GRID_HPP
