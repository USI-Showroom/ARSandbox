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
		_mapping(mapping)
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
	for (int x = 0; x < _width; ++x) {
		for (int y = 0; y < _height; ++y) {
			
			// computing indices of cells
			const int index = y * _height + x;
			const int top = (y-1) * _height + x;
			const int bottom = (y+1 * _height) + x;
			const int left = y * _height + x - 1;
			const int right = y * _height + x + 1;
			
			// cross sectional area of the pipe
			const double A = 0.25;
			
			// current water height
			const double d1 = water.at(index);
			
			// flux factor
			const double b =  9.81 * d1 / 0.15;

			// height difference between left, right, top and bottom neighbor
			const double lth = _mapping.getHeightFromParam(Point2d(x-1,y));
			const double rth = _mapping.getHeightFromParam(Point2d(x+1,y));
			const double tth = _mapping.getHeightFromParam(Point2d(x,y-1));
			const double bth = _mapping.getHeightFromParam(Point2d(x,y+1));
			
			// compute flow out
			leftFlux[index] = std::max(0.0, leftFlux[index] + dt * A * b);
			bottomFlux[index] = std::max(0.0, bottomFlux[index] + dt * A * b);
			topFlux[index] = std::max(0.0, topFlux[index] + dt * A * b);
			rightFlux[index] = std::max(0.0, rightFlux[index] + dt * A * b);
			double flowOut = leftFlux[index] + bottomFlux[index] + topFlux[index] + rightFlux[index];

			// smooth the flow out
			const double K = std::min(1.0, (d1 * _width * _height) / flowOut * dt);
			leftFlux[index] *= K;
			bottomFlux[index] *= K;
			topFlux[index] *= K;
			rightFlux[index] *= K;
			
			
			// compute flow in
			double flowIn = leftFlux[left] + bottomFlux[bottom] + topFlux[top] + rightFlux[right];
			
			// compute net volume change for water
			const double DV = dt * (flowIn - flowOut);
			water[index] = d1 + ( DV / (_width * _height) );

			// compute average flux velocity in the X and Y direction
			double avgWaterX = rightFlux[left] - leftFlux[index] + rightFlux[index] - leftFlux[right];
			avgWaterX *= 0.5;
			double avgWaterY = rightFlux[top] - leftFlux[index] + rightFlux[index] - leftFlux[bottom];
			avgWaterY *= 0.5;
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
