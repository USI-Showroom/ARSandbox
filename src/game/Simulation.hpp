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
	Simulation(int newWidth, int newHeight);
	~Simulation();

	void update(double dt, const UnitSquareMapping &mapping);
	void updateWaterSurface(double dt);
	void addWaterSource(const int cellIndex, const double amount);
    void flowSimulation(const double dt);
    void erosionDeposition(const double dt);
    void sedimentTransport(const double dt);
    void evaporation(const double dt);
	
	void setGrid(Grid *);
	
	void draw(QPainter &, const UnitSquareMapping &);

	// ranges for shader
	static double _minW, _maxW, _minS, _maxS;

private:
	int _width, _height;

	vector<double> _terrain;
	vector<double> _water;
	vector<double> _sediment;
	vector<double> _leftFlux, _rightFlux, _topFlux, _bottomFlux;
	vector<double> _u, _v, _s1;

	Grid *_grid;

	bool newWater;

	// simulation constants
	// cross sectional area of the pipe
    constexpr static double A = 0.00005;
    // length of virtual pipe
    constexpr static double l = 1.0;
    // gravity
    constexpr static double g = 9.81;
    // cell size
    double lx, ly;
    // sediment capacity constant
    constexpr static double Kc = 25.0;
    // dissolving constant
    constexpr static double Ks = 0.0001*12*10;
    // deposition constant
    constexpr static double Kd = 0.0001*12*10;
    // evaporation constant
    constexpr static double Ke = 0.00011*0.5;

    double outFlow, inFlow;
    
    double d1, d2, b1;
    double d1l, b1l, dhl;
    double d1t, b1t, dht;
    double d1r, b1r, dhr;
    double d1b, b1b, dhb;

    const double water(int x, int y);
	const double sediment(int x, int y);
	const double terrain(int x, int y);
	const double rightFlux(int x, int y);
	const double bottomFlux(int x, int y);
	const double topFlux(int x, int y);
	const double leftFlux(int x, int y);
	const double u(int x, int y);
	const double v(int x, int y);
};

#endif // SIMULATION_HPP_INCLUDED
