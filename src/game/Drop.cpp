#include "Drop.hpp"
#include "RK4.hpp"


#ifndef GRAVITY
#define GRAVITY 9.8
#endif

Drop::Drop(const double minH, const double maxH)
    : 
    _position(Point2d(0.55, 0.65)),
    _direction(0.0), _velocity(0.0), _acceleration(0.0),
    _life(0),
    _minH(minH), _maxH(maxH),
    _mass(0.5), _dt(0.001)
{ }

void Drop::update(const UnitSquareMapping &mapping)
{
    updatePosition(mapping);
}

Drop::~Drop()
{ }


void Drop::updatePosition(const UnitSquareMapping &mapping)
{
    clampCoordinates(_position, 0.05, 0.95);

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
    _velocity += acceleration * _dt;
    _position += newDirection * _velocity * _dt;
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

/**
 * @brief      Clamp x and y coordinates to minValue and maxValue
 *
 * @param      position  A Point2d with x and y coordinates
 * @param[in]  minValue  lower bound on the coordinates (double)
 * @param[in]  maxValue  upper boudn on the coordinates (double)
 */
void Drop::clampCoordinates(Point2d& position,
                            const double minValue,
                            const double maxValue)
{
    if (position.x() < minValue) position.x() = minValue;
    if (position.y() < minValue) position.y() = minValue;

    if (position.x() > maxValue) position.x() = maxValue;
    if (position.y() > maxValue) position.y() = maxValue;
}
