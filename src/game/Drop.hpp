#ifndef DROP_HPP
#define DROP_HPP

#include <QObject>
#include "Point2.hpp"
#include "UnitSquareMapping.hpp"

class Drop : QObject
{
    Q_OBJECT
public:
    Drop(const double, const double);
    virtual ~Drop();

    void update(const UnitSquareMapping &);
    void setLife(const int);
    void setPosition(const double, const double);

    const Point2d &direction() const;
    const Point2d &position()  const;
    bool alive() const;

private:
    Point2d _position, _direction;

    int _life, _maxLife;

    double _minH, _maxH;
    double _mass;
    double _speed;
    double _angle;
    double _dt;

    void newDirection(const UnitSquareMapping &);
};

#endif
