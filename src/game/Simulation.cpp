#include <cmath>
#include <iostream>
#include "Simulation.hpp"

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
	// updateVelocityField(dt);
	// simulateErosion(dt);
	// transportSediment(dt);
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
	const double A = 0.25;
	// length of virtual pipe
	const double l = 0.15;
	// gravity
	const double g = 9.81;
	// flux factor
	double b = g * A / l;

	for (int x = 0; x < _width; ++x) {
		for (int y = 0; y < _height; ++y) {

			// computing indices of cells
			const int currentCell = y * _width + x;
			const int top = (y-1) * _width + x;
			const int bottom = (y+1 * _width) + x;
			const int left = y * _width + x - 1;
			const int right = y * _width + x + 1;
			
			// current water height
			const double d1 = water.at(currentCell);
			// current terrain height
			const double h1 = _grid->getHeight(x,y);

			// height (water height + terrain height) difference between
			// current cell and left, right, top and bottom cells
			const double lth = h1 - _grid->getHeight();
			const double rth = h1 - _grid->getHeight();
			const double tth = h1 - _grid->getHeight();
			const double bth = h1 - _grid->getHeight();
			
			// compute flow out
			leftFlux[currentCell]   = std::max(0.0, leftFlux[currentCell]   + dt * A * b * lth);
			rightFlux[currentCell]  = std::max(0.0, rightFlux[currentCell]  + dt * A * b * rth);
			topFlux[currentCell]    = std::max(0.0, topFlux[currentCell]    + dt * A * b * tth);
			bottomFlux[currentCell] = std::max(0.0, bottomFlux[currentCell] + dt * A * b * bth);
			
			double flowOut = leftFlux[currentCell] + bottomFlux[currentCell] + topFlux[currentCell] + rightFlux[currentCell];

			// smooth the flow out
			const double K = std::min(1.0, (d1 * _width * _height) / flowOut * dt);
			leftFlux[currentCell] *= K;
			bottomFlux[currentCell] *= K;
			topFlux[currentCell] *= K;
			rightFlux[currentCell] *= K;
			
			
			// compute flow in
			double flowIn = leftFlux[left] + bottomFlux[bottom] + topFlux[top] + rightFlux[right];
			
			// compute net volume change for water
			const double DV = dt * (flowIn - flowOut);
			water[currentCell] = d1 + ( DV / (_width * _height) );

			// compute average flux velocity in the X and Y direction
			double avgWaterX = rightFlux[left] - leftFlux[currentCell] + rightFlux[currentCell] - leftFlux[right];
			avgWaterX *= 0.5;
			double avgWaterY = rightFlux[top] - leftFlux[currentCell] + rightFlux[currentCell] - leftFlux[bottom];
			avgWaterY *= 0.5;

			double avgWater = avgWaterX + avgWaterY;

			// update velocity fields
			if (avgWater == 0.0) {
                _u.at(currentCell) = _v.at(currentCell) = 0.0;
            } else {
            	_u.at(currentCell) = avgWaterX;
            	_v.at(currentCell) = avgWaterY;
            }
		}
	}
}

void Simulation::addWaterSource(const Point2d &center, const int radius, const double amount)
{
	_temporaryWaterAmount = amount;
	const int x = round(center.x());
	const int y = round(center.y());
	for(int i = -radius; i < radius; ++i) {
		unsigned int index = y * _height + x * i;
		water[index] = amount;
	}
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
