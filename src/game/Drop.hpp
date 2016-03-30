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
    void setLife     (const int);
    void setPosition (const double, const double);

    bool           alive     ()  const;

    const Point2d& direction ()  const;
    const Point2d& position  ()  const;

private:
    Point2d _position, _direction, _velocity, _acceleration;

    int _life;

    double _minH, _maxH;
    double _mass;
    double _dt;

    void updatePosition   (const UnitSquareMapping &);
    void clampCoordinates (Point2d&, const double, const double);
};

#endif
