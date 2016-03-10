#ifndef WATERDROP_HPP
#define WATERDROP_HPP

#include <QObject>
#include "Point2.hpp"
#include "UnitSquareMapping.hpp"

class WaterDrop : QObject
{
    Q_OBJECT
private:
    Point2d _position, _direction;

    int _life, _maxLife;
    int _decisionTicks;
    int _soundTicks;

    double _minH, _maxH;
    double _speed;
    double _angle;

    bool _resurrected;

public:
    WaterDrop(const double minH, const double maxH);
    virtual ~WaterDrop();

    void update(const UnitSquareMapping &mapping);

    inline const Point2d &position() const { return _position; }
    inline bool alive() const { return _life > 0; }

    inline void setPosition(const double newX, const double newY)
    {
        _position.x() = newX;
        _position.y() = newY;
    }

    inline void setLife(const int newLife)
    {
        _life = newLife;
    }

    inline const Point2d &direction() const { return _direction; }

private:
    void resurrect(const UnitSquareMapping &mapping);
    void newDirection(const UnitSquareMapping &mapping);
};

#endif
