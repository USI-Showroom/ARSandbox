#include "Drop.hpp"
#include "RK4.hpp"


#ifndef GRAVITY
#define GRAVITY 9.8
#endif

Drop::Drop(const double minH, const double maxH)
: _position(Point2d(0.55, 0.65)), 
  _life(0),
  _minH(minH), _maxH(maxH),
  _mass(0.5), _speed(0.0), _angle(0.0), _dt(0.001)
{ }

void Drop::update(const UnitSquareMapping &mapping)
{
    updatePosition(mapping);
}

Drop::~Drop()
{ }

void Drop::updatePosition(const UnitSquareMapping &mapping)
{
    Point2d      gradient = mapping.paramGrad(_position);
    const double height   = mapping.getHeightFromParam(_position);

    if(gradient.isZero())
    {
        --_life;
        return;
    }

    Point2d newDirection = gradient.normalize();

    // euler method
    double acceleration = height * GRAVITY / _mass;
    _speed += acceleration * _dt;
    _position += ( newDirection * _speed * _dt ) / 2.0;

    // runge kutta

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

void Drop::setLife(const int newLife)
{
    _life = newLife;
}

void Drop::setPosition(const double newX, const double newY)
{
    _position.x() = newX;
    _position.y() = newY;
}
