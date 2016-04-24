#ifndef SIMULATION_HPP_INCLUDED
#define SIMULATION_HPP_INCLUDED

#include <vector>
#include <QByteArray>
#include <QDataStream>
#include "Point2.hpp"

using namespace std;

class Simulation
{
public:
	Simulation(int newWidth, int newHeight);
	~Simulation();

	void update(double dt);
	void incrementWater(double dt);
	void simulateFlow(double dt);
	void computeOutflowFlux(double dt);

	void addWaterSource(const Point2d &center, const int radius, const double amount);

	QByteArray getWaterField();

private:
	int _width, _height;

	vector<double> _b;
	vector<double> _d;
	vector<double> _s;
	vector<double> _fl, _fr, _ft, _fb;
	vector<double> _u, _v;

	double b(int x, int y) const;
	double d(int x, int y) const;
	double s(int x, int y) const;
	
	double fl(int x, int y) const;
	double fr(int x, int y) const;
	double ft(int x, int y) const;
	double fb(int x, int y) const;
	
	double u(int x, int y) const;
	double v(int x, int y) const;
};

#endif // SIMULATION_HPP_INCLUDED
