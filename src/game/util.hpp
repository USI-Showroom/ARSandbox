#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include "Point2.hpp"
#include "Point3.hpp"
#include "UnitSquareMapping.hpp"
#include <QColor>

// Some utility functions
// @author matteo.muscella@usi.ch

namespace util
{

inline void clamp(Point2d& p, const double minValue, const double maxValue)
{
    if (p.x() < minValue) p.x() = minValue;
    if (p.y() < minValue) p.y() = minValue;

    if (p.x() > maxValue) p.x() = maxValue;
    if (p.y() > maxValue) p.y() = maxValue;
}

inline void clamp(Point3d& p, const double minValue, const double maxValue)
{
    if (p.x() < minValue) p.x() = minValue;
    if (p.y() < minValue) p.y() = minValue;
    if (p.z() < minValue) p.z() = minValue;

    if (p.x() > maxValue) p.x() = maxValue;
    if (p.y() > maxValue) p.y() = maxValue;
    if (p.z() > maxValue) p.z() = maxValue;
}

template <typename T>
inline void clamp(T& val, const T& a, const T& b)
{
    if (val < a) val = a;
    if (val > b) val = b;
}

inline void clamp(double& value, const double minValue, const double maxValue)
{
    if (value < minValue) value = minValue;
    if (value > maxValue) value = maxValue;
}

inline Point2d scale(Point2d& point, const int imgHeight, const int scaling, const UnitSquareMapping& mapping)
{
    Point2d p = mapping.fromParameterization(point);
    p.y() = imgHeight - p.y();

    if (p.y() == imgHeight) {
        --p.y();
    }
    
    assert(p.y() >= 0);
    assert(p.y() < imgHeight);

    p *= scaling;
    return p;
}

}
#endif
