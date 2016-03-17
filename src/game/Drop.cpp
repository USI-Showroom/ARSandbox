#include "Drop.hpp"

#ifndef GRAVITY
#define GRAVITY 9.8
#endif

Drop::Drop(const double minH, const double maxH)
: _position(Point2d(0.55, 0.65)), 
  _life(0), _maxLife(3),
  _minH(minH), _maxH(maxH),
  _mass(0.5), _speed(0.5), _angle(2.0), _dt(0.001)
{
    std::cout << "life: " << _life << std::endl;
}

void Drop::update(const UnitSquareMapping &mapping)
{
    newDirection(mapping);
    _position += _dt*_direction;
}

Drop::~Drop()
{ }

void Drop::newDirection(const UnitSquareMapping &mapping)
{
    Point2d grad=mapping.paramGrad(_position);
    const double h=mapping.getHeightFromParam(_position);



    if(grad.isZero())
    {
        --_life;
        return;
    }

    if(h>=_minH && h<=_maxH)
    {
        grad.rotated();

        if(_life<_maxLife)
            ++_life;

        if(grad*_direction<0)
            grad=-grad;
    }
    else
    { 
        --_life;
        if(h<_minH) //go up
            grad=-grad;

        _direction.normalize();
        grad.normalize();
        grad=_direction+0.2*grad;
    }

    // const double newAngle=atan2(grad.y(),grad.x());
    // if(_resurrected)
    // {
    //     _angle=newAngle;
    //     _resurrected=false;
    // }
    // else
    //     _angle=0.3*newAngle+0.7*_angle;

    _direction=Point2d(cos(_angle),sin(_angle))*_speed;
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
    //assert(_life == 0);
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
