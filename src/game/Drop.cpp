#include "Drop.hpp"
#include "Point3.hpp"
#include "util.hpp"

#ifndef GRAVITY
#define GRAVITY 9.81
#endif

Drop::Drop(const double minH, const double maxH)
    : 
    _position(Point2d(0.55, 0.65)),
    _direction(0.0), _velocity(0.0), _acceleration(0.0),
    _life(0),
    _minH(minH), _maxH(maxH),
    _mass(1.0), _friction(1.2),
    _dt(0.001)
{ }

void Drop::update(const UnitSquareMapping &mapping)
{
    updatePosition(mapping);
    erodeTerrain(mapping);
    evaporate(mapping);
}

Drop::~Drop()
{ }


void Drop::updatePosition(const UnitSquareMapping &mapping)
{
    util::clamp(_position, 0.05, 0.95);

    Point2d      gradient = mapping.paramGrad(_position);
                 height   = mapping.getHeightFromParam(_position);

    static const Point3d ag = Point3d(0.0, 0.0, -GRAVITY) / _mass;

    const Point2d a1 = _position;
    const Point2d a2 = _position + Point2d(0.1,0.0);
    const Point2d a3 = _position + Point2d(0.0,0.1);

    const double  h1 = height;
    const double  h2 = mapping.getHeightFromParam(a2);
    const double  h3 = mapping.getHeightFromParam(a3);

    const Point3d p1 = Point3d(a1.x(), a1.y(), h1);
    const Point3d p2 = Point3d(a2.x(), a2.y(), h2);
    const Point3d p3 = Point3d(a3.x(), a3.y(), h3);

    Point3d n = ( p2 - p1 ) ^ ( p3 - p1 );
    n.normalize();

    const Point3d c = (ag * n) * n;

    _acceleration += (ag - c) * 10.0 - _friction * _acceleration;

    if(gradient.norm()>1e-3)
        _velocity     += _acceleration.norm() * gradient.normalized() * _dt;

    _position     += _velocity * _dt * 0.5;
}

void Drop::erodeTerrain(const UnitSquareMapping &mapping)
{
    const double Kc = 25.0;  // sediment capacity constant
    const double Ks = 0.001; // dissolving constant
    const double Kd = 0.001; // deposition constant
    const double st = 0.5;   // suspended sediment amount

    Point2d gradient = mapping.paramGrad(_position);
    const double angle = gradient * _position;

    // sediment transport capacity
    const double C = Kc * angle * _velocity.norm();

    if(C > st) {
        // erode
        
    } else {
        // deposit
        
    }
}

void Drop::evaporate(const UnitSquareMapping &mapping)
{
    const float Ke = 0.0003; // evaporation constant

    // update water height or amount
    double wh = 0.25 * (1 - Ke * _dt);
}

const Point2d& Drop::position() const
{
    return _position;
}

const Point2d& Drop::direction() const
{
    return _direction;
}

bool Drop::alive() const
{
    return _life > 0;
}

void Drop::setAlive()
{
    _life = 1;
}

void Drop::setPosition(const double newX, const double newY)
{
    _position.x() = newX;
    _position.y() = newY;
}
