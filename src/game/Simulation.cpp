#include <cmath>
#include "Simulation.hpp"

Simulation::Simulation(int newWidth, int newHeight, const UnitSquareMapping& mapping)
	:	_width(newWidth),
		_height(newHeight),
		_b(_width * _height, 0.0),
		_d(_width * _height, 0.0),
		_s(_width * _height, 0.0),
		_fl(_width * _height),
		_fr(_width * _height),
		_ft(_width * _height),
		_fb(_width * _height),
		_u(_width * _height),
		_v(_width * _height),
		_mapping(mapping)
{}

Simulation::~Simulation()
{}

void Simulation::update(double dt)
{
	incrementWater(dt);
	computeOutflowFlux(dt);
	updateWaterSurface(dt);
	// updateVelocityField(dt);
	// simulateErosion(dt);
	// transportSediment(dt);
}

void Simulation::incrementWater(double dt)
{
	addWaterSource(Point2d(0.5, 0.5), 10, 0.20);
}

void Simulation::computeOutflowFlux(double dt)
{
	for (int x = 0; x < _width; ++x) {
		for (int y = 0; y < _height; ++y) {
			const int index = y * _height + x;
			
			const double A = 0.25;
			const double dh = _mapping.getHeightFromParam(Point2d(x,y));
			const double b =  9.81 * dh / 0.15;
			
			_fl[index] = std::max(0.0, _fl[index] + dt * A * b);
			_fb[index] = std::max(0.0, _fb[index] + dt * A * b);
			_ft[index] = std::max(0.0, _ft[index] + dt * A * b);
			_fr[index] = std::max(0.0, _fr[index] + dt * A * b);
		}
	}
}

void Simulation::updateWaterSurface(dt)
{
	for (int x = 0; x < _width; ++x) {
		for (int y = 0; y < _height; ++y) {

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
		_d[index] = amount;
	}
}

const double Simulation::getWaterAt(int x, int y)
{
	return _d.at(y * _height + x);
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
