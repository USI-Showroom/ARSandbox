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

    void update      (const UnitSquareMapping &);
    void setAlive    ();
    void setPosition (const double, const double);

    bool alive() const;

    const Point2d& direction () const;
    const Point2d& position  () const;

private:
    Point2d _position, _direction, _velocity;
    Point3d _acceleration;

    int _life;

    double _minH, _maxH;
    double _mass, _friction;
    double _dt;

    void updatePosition(const UnitSquareMapping &);
};

#endif
