#include <cmath>
#include "Simulation.hpp"

Simulation::Simulation(int newWidth, int newHeight)
	:	_width(newWidth),
		_height(newHeight),
		_b(_width * _height),
		_d(_width * _height, 0.0),
		_s(_width * _height),
		_fl(_width * _height),
		_fr(_width * _height),
		_ft(_width * _height),
		_fb(_width * _height),
		_u(_width * _height),
		_v(_width * _height)
{}

Simulation::~Simulation()
{}

void Simulation::update(double dt)
{
	incrementWater(dt);
	simulateFlow(dt);
	computeOutflowFlux(dt);
	// updateWaterSurface(dt);
	// updateVelocityField(dt);
	// simulateErosion(dt);
	// transportSediment(dt);
}

void Simulation::incrementWater(double dt)
{

}

void Simulation::simulateFlow(double dt)
{

}

void Simulation::computeOutflowFlux(double dt)
{

}

void Simulation::addWaterSource(const Point2d &center, const int radius, const double amount)
{
	const int x = round(center.x());
	const int y = round(center.y());
	for(int i = -radius; i < radius; ++i) {
		unsigned int idx = y * _height + x * i;
		_d[idx] = amount;
	}
}

// http://stackoverflow.com/a/26843664
// 
QByteArray Simulation::getWaterField()
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	for (size_t i = 0; i < _d.size(); ++i) {
		stream << _d.at(i);
	}
	return data;
}
