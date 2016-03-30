#ifndef __RK4_HPP
#define __RK4_HPP

#include "Point2.hpp"

// Runge Kutta Integrator
// http://gafferongames.com/game-physics/integration-basics/
// Adapted to my ASR project
//
struct State
{
	Point2d x;
	Point2d v;
};

struct Derivative
{
    Point2d dx;      // dx/dt = velocity
    double dv;       // dv/dt = acceleration
};

// Derivative evaluate( const State &initial,
//                      double t,
//                      double dt,
//                      const Derivative &d )
// {
//     State state;
//     state.x = initial.x + d.dx*dt;
//     state.v = Point2d(initial.v.x() + d.dv*dt, initial.v.y() + d.dv*dt);

//     Derivative output;
//     output.dx = state.v;
//     output.dv = acceleration(state);
//     return output;
// }

// // interpolation between previous state and next state
// double acceleration( const State &state )
// {
//     const double k = 10;
//     const double b = 1;
//     return -k * state.x - b*state.v;
// }

// void integrate( State &state, 
//                 double t, 
//                 double dt )
// {
//     Derivative a,b,c,d;

//     a = evaluate( state, t, 0.0, Derivative() );
//     b = evaluate( state, t, dt*0.5, a );
//     c = evaluate( state, t, dt*0.5, b );
//     d = evaluate( state, t, dt, c );

//     double dxdt = 1.0 / 6.0 * 
//         ( a.dx + 2.0*(b.dx + c.dx) + d.dx );
    
//     double dvdt = 1.0 / 6.0 * 
//         ( a.dv + 2.0*(b.dv + c.dv) + d.dv );

//     state.x = state.x + dxdt * dt;
//     state.v = state.v + dvdt * dt;
// }
#endif