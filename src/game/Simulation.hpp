#ifndef SIMULATION_HPP_INCLUDED
#define SIMULATION_HPP_INCLUDED

#include <vector>
#include "Point2.hpp"
#include "UnitSquareMapping.hpp"
#include "Grid.hpp"

using namespace std;

class Simulation
{
public:
	Simulation(int newWidth, int newHeight, const UnitSquareMapping &);
	~Simulation();

	void update(double dt);
	void incrementWater(double dt);
	void updateWaterSurface(double dt);
	void addWaterSource(const int cellIndex, const double amount);
	void setGrid(const Grid *);
	
	const double getWaterAt(int x, int y);
	const double getTerrainAt(int x, int y);
	const double getSedimentAt(int x, int y);

	void draw(QPainter &, const UnitSquareMapping &);

	// ranges for shader
	static double _minW, _maxW, _minS, _maxS;

private:
	int _width, _height;

	vector<double> terrain;
	vector<double> water;
	vector<double> sediment;
	vector<double> leftFlux, rightFlux, topFlux, bottomFlux;
	vector<double> _u, _v;

	const UnitSquareMapping _mapping;
	const Grid *_grid;
};

#endif // SIMULATION_HPP_INCLUDED
