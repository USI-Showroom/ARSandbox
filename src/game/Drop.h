#ifndef DROP_HPP
#define DROP_HPP

#include <QObject>
#include "Point2.hpp"
#include "UnitSquareMapping.hpp"

class Drop : QObject
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
    Drop(const double minH, const double maxH);
    virtual ~Drop();

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
    void newDirection(const UnitSquareMapping &mapping);
};

#endif
