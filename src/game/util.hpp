#ifndef UTIL_HPP
#define UTIL_HPP

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

inline void clamp(double& value, const double minValue, const double maxValue)
{
    if (value < minValue) value = minValue;
    if (value > maxValue) value = maxValue;
}

}
#endif
