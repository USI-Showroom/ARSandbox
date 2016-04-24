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
	const int x = static_cast<int>(center.x());
	const int y = static_cast<int>(center.y());
	for(int i = -radius; i < radius; ++i) {
		unsigned int idx = y * _height + x + i;
		_b[idx] = amount;
	}
}

// http://stackoverflow.com/a/26843664
// 
QByteArray Simulation::getWaterField()
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	for (double x : _d) {
		stream << x;
	}
	return data;
}

double Simulation::b(int x, int y) const
{
	return _b.at( y * _height + x );
}

double Simulation::d(int x, int y) const
{
	return _d.at( y * _height + x );
}

double Simulation::s(int x, int y) const
{
	return _s.at( y * _height + x );
}

double Simulation::fl(int x, int y) const
{
	return _fl.at( y * _height + x );
}

double Simulation::fr(int x, int y) const
{
	return _fr.at( y * _height + x );
}

double Simulation::ft(int x, int y) const
{
	return _ft.at( y * _height + x );
}

double Simulation::fb(int x, int y) const
{
	return _fb.at( y * _height + x );
}

double Simulation::u(int x, int y) const
{
	return _u.at( y * _height + x );
}

double Simulation::v(int x, int y) const
{
	return _v.at( y * _height + x );
}
