#include "Grid.hpp"
#include <iostream>
#include <cassert>

#ifdef NO_KINECT
static const int scaling=3;
#else
static const int scaling=7;
#endif

Grid::Grid(int newSize, const UnitSquareMapping *newMapping)
	:	_size(newSize),
		xStep(1.0 / _size), yStep(1.0 / _size),
		imgHeight(424),
		mapping(newMapping)
{

}

Grid::~Grid()
{

}

int& Grid::size()
{
	return _size;
}

double Grid::getHeight(int x, int y) const
{
	assert(x < _size);
	assert(y < _size);

	const int sw = 3;
	const int sh = 3;
	const int npoints = sw*sh;
	double perc = 1e-2;

	std::vector<double> samples;
	samples.push_back(10.0);
	samples.push_back(40.0);
	samples.push_back(90.0);

	double avg = 0;
	double h, a, b;
	for (int i = 0; i < npoints; ++i) {

		a = samples.at(i/sw);
		b = samples.at(i%sh);

		Point2d p( a * xStep * perc * (x + 1.0), b * yStep * perc * (y + 1.0));
		h = mapping->getHeightFromParam(p);

		avg += h;
	}

	avg /= npoints;
	return avg;
}

void Grid::draw(QPainter &painter)
{
	for (int i = 0; i < _size; ++i) {
		for (int j = 0; j < _size; ++j) {

			Point2d p1(i*xStep, j*yStep);
			Point2d p2(xStep, yStep);

			for (int nx = 0; nx < 10; ++nx) {
				for (int ny = 0; ny < 10; ++ny) {
					Point2d p_ = p1 + Point2d(nx/9.0 * xStep, ny / 9.0 * yStep);
					Point2d p = mapping->fromParameterization(p_);
					p.y() = imgHeight - p.y();

					if (p.y() == imgHeight)
						--p.y();
					assert(p.y() >= 0);
					assert(p.y() < imgHeight);
					p*=scaling;
					QPen pen(Qt::blue, 2);
					painter.setPen(pen);

					QBrush brush(Qt::blue);
					painter.setBrush(brush);

					painter.drawEllipse(QPointF(p.x(), p.y()), 5, 5);
				}
			}
		}
	}
}
