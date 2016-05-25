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
	void setGrid(Grid *);
	
	const double getWaterAt(int x, int y);
	const double getTerrainAt(int x, int y);
	const double getSedimentAt(int x, int y);

	void draw(QPainter &, const UnitSquareMapping &);

	// ranges for shader
	static double _minW, _maxW, _minS, _maxS;

	const double water(int x, int y);
	const double sediment(int x, int y);
	const double terrain(int x, int y);
	const double rightFlux(int x, int y);
	const double bottomFlux(int x, int y);
	const double topFlux(int x, int y);
	const double leftFlux(int x, int y);
	const double u(int x, int y);
	const double v(int x, int y);

private:
	int _width, _height;

	vector<double> _terrain;
	vector<double> _water;
	vector<double> _sediment;
	vector<double> _leftFlux, _rightFlux, _topFlux, _bottomFlux;
	vector<double> _u, _v, _s1;

	const UnitSquareMapping _mapping;
	Grid *_grid;

	bool newWater;

	// simulation constants
	// cross sectional area of the pipe
    constexpr static double A = 8.0;
    // length of virtual pipe
    constexpr static double l = 5.05;
    // gravity
    constexpr static double g = 12.81;
    // cell size
    double lx, ly;
    // sediment capacity constant
    constexpr static double Kc = 0.0075;
    // dissolving constant
    constexpr static double Ks = 0.002;
    // deposition constant
    constexpr static double Kd = 0.0025;
    // evaporation constant
    constexpr static double Ke = 0.00005;
};

#endif // SIMULATION_HPP_INCLUDED
