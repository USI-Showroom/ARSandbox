#ifndef SIMULATION_HPP_INCLUDED
#define SIMULATION_HPP_INCLUDED

#include <vector>
#include <QByteArray>
#include <QDataStream>
#include "Point2.hpp"
#include "UnitSquareMapping.hpp"

using namespace std;

class Simulation
{
public:
	Simulation(int newWidth, int newHeight, const UnitSquareMapping &);
	~Simulation();

	void update(double dt);
	void incrementWater(double dt);
	void updateWaterSurface(double dt);
	void addWaterSource(const Point2d &center, const int radius, const double amount);
	const double getWaterAt(int x, int y);

	QByteArray getWaterField();

private:
	int _width, _height;

	vector<double> terrain;
	vector<double> water;
	vector<double> sediment;
	vector<double> leftFlux, rightFlux, topFlux, bottomFlux;
	vector<double> _u, _v;

	double _temporaryWaterAmount;

	const UnitSquareMapping _mapping;
};

#endif // SIMULATION_HPP_INCLUDED
