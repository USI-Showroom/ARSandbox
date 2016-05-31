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
    void rain(double dt);

    std::vector<int> getNeighbors(const int index);

	const double getWaterAt(int x, int y);
	const double getTerrainAt(int x, int y);
	const double getSedimentAt(int x, int y);

	void draw(QPainter &);

	// ranges for shader
	static double _minW, _maxW, _minS, _maxS;

    void toggleRain();

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

	const UnitSquareMapping &_mapping;
	Grid *_grid;

	bool _newWater, _isRaining;

	// simulation constants
	// cross sectional area of the pipe
    const double A;
    // length of virtual pipe
    const double l;
    // gravity
    const double g;
    // cell size
    const double lx, ly;
    // sediment capacity constant
    const double Kc;
    // dissolving constant
    const double Ks;
    // deposition constant
    const double Kd;
    // evaporation constant
    const double Ke;

    double outFlow, inFlow;

    double d1, d2, b1;
    double d1l, b1l, dhl;
    double d1t, b1t, dht;
    double d1r, b1r, dhr;
    double d1b, b1b, dhb;
};

#endif // SIMULATION_HPP_INCLUDED
