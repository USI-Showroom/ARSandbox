#ifndef GRID_HPP
#define GRID_HPP

#include "Point2.hpp"
#include "Point3.hpp"
#include <QPainter>
#include "UnitSquareMapping.hpp"

class Grid
{
public:
	Grid(int newSize, const UnitSquareMapping *newMapping);
	~Grid();

	const int& size();
	void draw(QPainter &painter);
	double getHeight(int x, int y) const;
	void drawCell( QPainter&, const int, const int, double, double, double ) const;
	int getCellIndex(const double, const double);
	Point3d getCellNormal( int x, int y ) const;

private:
	int _size;

	const double xStep;
	const double yStep;

	const int imgHeight;

	const UnitSquareMapping *mapping;
};

#endif // GRID_HPP
