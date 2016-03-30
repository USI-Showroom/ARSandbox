#ifndef __RK4_HPP
#define __RK4_HPP

#include "Point2.hpp"

// Runge Kutta Integrator
double rk4(double(*f)(double, double), double dx, double x, double y)
{
	double	k1 = dx * f(x, y),
			k2 = dx * f(x + dx / 2, y + k1 / 2),
			k3 = dx * f(x + dx / 2, y + k2 / 2),
			k4 = dx * f(x + dx, y + k3);
	return y + (k1 + 2 * k2 + 2 * k3 + k4) / 6;
}

#endif