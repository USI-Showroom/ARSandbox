#include <cmath>
#include <iostream>
#include "Simulation.hpp"

#ifdef NO_KINECT
static const int scaling=3;
#else
static const int scaling=7;
#endif

Simulation::Simulation(int newWidth, int newHeight, const UnitSquareMapping& mapping)
	:	_width(newWidth),
		_height(newHeight),
		terrain(_width * _height, 0.0),
		water(_width * _height, 0.0),
		sediment(_width * _height, 0.0),
		leftFlux(_width * _height),
		rightFlux(_width * _height),
		topFlux(_width * _height),
		bottomFlux(_width * _height),
		_u(_width * _height),
		_v(_width * _height),
		_mapping(mapping),
		_grid(nullptr)
{}

Simulation::~Simulation()
{}

void Simulation::update(double dt)
{
	incrementWater(dt);
	updateWaterSurface(dt);
}

void Simulation::incrementWater(double dt)
{
	addWaterSource(Point2d(0.5, 0.5), 10, 0.20);
}

void Simulation::updateWaterSurface(double dt)
{
	if (_grid == nullptr) {
#ifdef DEBUG
		std::cout << "ERROR: no grid found for simulation" << std::endl;
#endif
		return;
	}

	// cross sectional area of the pipe
	const double A = 0.0025;
	// length of virtual pipe
	const double l = 0.15;
	// gravity
	const double g = 9.81;
	// flux factor
	double fluxFactor = g * A / l;
	// cell size
	double lx, ly;
	lx = ly = 1.0;

	for (int x = 0; x < _width; ++x) {
		for (int y = 0; y < _height; ++y) {

			double inFlow = 0.0;
			double outFlow = 0.0;
			double l = 0.0;
			int currentCell = y * _width  + x;

			// current water height
			const double d1 = water.at(currentCell);
			// current terrain height
			const double h1 = _grid->getHeight(x,y);
			double heightDiff = 0.0;

			// flow difference between left and right flux
			double dflowRightLeft = 0.0;
			// flow difference between top and bottom flux
			double dflowTopBottom = 0.0;

			// there is flow from left
			if (x > 0) {
				// height diff between current cell and left cell
				heightDiff = (h1+d1) - (_grid->getHeight(x - 1, y) + water.at(y * _width + x - 1));
				l = leftFlux[currentCell] + (fluxFactor * heightDiff);
				leftFlux[currentCell] = std::max(0.0, l);
				outFlow += leftFlux[currentCell];
				inFlow += rightFlux[y * _width + x - 1];
				dflowRightLeft = rightFlux[y * _width + x - 1];
			} else {
				leftFlux[currentCell] = 0.0;
			}

			// there is flow from right
			if (x < _width - 1) {
				heightDiff = (h1+d1) - (_grid->getHeight(x + 1, y) + water.at(y * _width + x + 1));
				l = rightFlux[currentCell] + (fluxFactor * heightDiff);
				rightFlux[currentCell] = std::max(0.0, l);
				outFlow += rightFlux[currentCell];
				inFlow += leftFlux[y * _width + x + 1];
				dflowRightLeft -= leftFlux[y * _width + x + 1];
			} else {
				rightFlux[currentCell] = 0.0;
			}

			// there is flow from above
			if (y > 0) {
				heightDiff = (h1+d1) - (_grid->getHeight(x, y - 1) + water.at((y - 1) * _width + x));
				l = topFlux[currentCell] + (fluxFactor * heightDiff);
				topFlux[currentCell] = std::max(0.0, l);
				outFlow += topFlux[currentCell];
				inFlow += bottomFlux[(y-1) * _width + x];
				dflowTopBottom = bottomFlux[(y-1) * _width + x];
			} else {
				topFlux[currentCell] = 0.0;
			}

			// there is flow from bottom
			if (y < _height - 1) {
				heightDiff = (h1+d1) - (_grid->getHeight(x, y + 1) + water.at((y + 1) * _width + x));
				l = bottomFlux[currentCell] + (fluxFactor * heightDiff);
				bottomFlux[currentCell] = std::max(0.0, l);
				outFlow += bottomFlux[currentCell];
				inFlow += topFlux[(y + 1) * _width + x];
				dflowTopBottom += topFlux[(y + 1) * _width + x];
			} else {
				bottomFlux[currentCell] = 0.0;
			}

			// // compute scaling factor
			// double z = (water[currentCell]*lx*ly) / (outFlow * dt);
			// double K = std::min(1.0, z);

			// // scale the flows
			// leftFlux[currentCell] *= K;
			// bottomFlux[currentCell] *= K;
			// topFlux[currentCell] *= K;
			// rightFlux[currentCell] *= K;

#ifdef DEBUG
			std::cout << "currentCell: " << currentCell << "\n"
			          << "inFlow: "      << inFlow << "\n"
			          << "outFlow: "     << outFlow<< "\n";
#endif
			// update water velocity field with net volume change for water
			double dv = dt * (inFlow - outFlow);
			double d2 = dv / (lx * ly);
			water[currentCell] = d1 + d2;

			// calculate average amount of water that passes through cell
			double dwx = dflowRightLeft - leftFlux[currentCell] + rightFlux[currentCell];
			double dwy = dflowTopBottom - topFlux[currentCell] + bottomFlux[currentCell];
			double avgWaterX = 0.5 * dwx;
			double avgWaterY = 0.5 * dwy;

			double avgWater = avgWaterX + avgWaterY;
			// update velocity fields
			if (avgWater == 0.0) {
                _u[currentCell] = _v.at(currentCell) = 0.0;
            } else {
            	_u[currentCell] = avgWaterX;
            	_v[currentCell] = avgWaterY;
            }
		}
	} // end for loop
#ifdef DEBUG
	std::cout << std::endl;
	std::cout << "Water heights at cells:" << std::endl;
	// print out the grid values
	for (int x = 0; x < _width; ++x) {
		for (int y = 0; y < _height; ++y) {
			std::cout << water[y * _width + x] << ",  ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif
}

void Simulation::addWaterSource(const Point2d &center, const int radius, const double amount)
{
	water[4] = 0.20;
}

const double Simulation::getWaterAt(int x, int y)
{
	return water.at(y * _height + x);
}

// http://stackoverflow.com/a/26843664
//
QByteArray Simulation::getWaterField()
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	for (size_t i = 0; i < water.size(); ++i) {
		stream << water.at(i);
	}
	return data;
}

void Simulation::setGrid(const Grid *newGrid)
{
	_grid = newGrid;
}

void Simulation::draw( QPainter& painter, const UnitSquareMapping& mapping ) {
    for ( int x = 0; x < _width; ++x ) {
        for ( int y = 0; y < _height; ++y ) {
            QPen pen( Qt::blue, 2 );
            painter.setPen( pen );

            QBrush brush( Qt::blue );
            painter.setBrush( brush );

            const double cellOpacity = water[y * _width + x];
            _grid->drawCell( painter, x, y, cellOpacity * 10.0 );
        }
    }
}
