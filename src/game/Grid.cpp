#include "Grid.hpp"
#include <iostream>
#include <cassert>

#ifdef NO_KINECT
static const int scaling=3;
#else
static const int scaling=7;
#endif

Grid::Grid(int newSize)
	:	_size(newSize)
{

}

Grid::~Grid()
{

}

int& Grid::size()
{
	return _size;
}

void Grid::draw(QPainter &painter)
{
	const int xStep = 512 / _size;
	const int yStep = 424 / _size;

	for (int i = 0; i < _size; ++i) {
		for (int j = 0; j < _size; ++j) {

			Point2d p1(i*xStep, j*yStep);
			Point2d p2(xStep, yStep);

            assert(p1.y() >= 0);
            assert(p1.y() < 424);

            p1*=scaling;
            p2*=scaling;

			painter.drawRect(p1.x(), p1.y(), p2.x(), p2.y());
#ifdef DEBUG
			std::cout << "Rect @(" << i << ", " << j << ") " << std::endl;
#endif
		}
	}
}