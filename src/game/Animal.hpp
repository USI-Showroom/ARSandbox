#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include "Point2.hpp"
#include "UnitSquareMapping.hpp"

class Animal
{
private:
    Point2d _position, _direction;
    int _life;
    int _decisionTicks;

    double _minH, _maxH;
    double _speed;
    double _angle, _globalAngle;
    bool _resurrected;

public:
    Animal(const double minH, const double maxH);

    void think(const UnitSquareMapping &mapping);
    void update();

    inline const Point2d &position() const { return _position; }
    inline bool alive() const { return _life>0; }

    inline double globalAngle() const { return _globalAngle; }

private:
    void resurrect(const UnitSquareMapping &mapping);
    void newDirection(const UnitSquareMapping &mapping);
    void move();

    double randRange(const double min, const double max);
};

#endif //ANIMAL_HPP
