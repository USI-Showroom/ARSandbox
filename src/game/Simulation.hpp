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
	void computeOutflowFlux(double dt);

	void addWaterSource(const Point2d &center, const int radius, const double amount);

	const double getWaterAt(int x, int y);

	QByteArray getWaterField();

private:
	int _width, _height;

	vector<double> _b;
	vector<double> _d;
	vector<double> _s;
	vector<double> _fl, _fr, _ft, _fb;
	vector<double> _u, _v;

	double _temporaryWaterAmount;

	const UnitSquareMapping _mapping;
};

#endif // SIMULATION_HPP_INCLUDED
