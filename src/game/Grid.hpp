#ifndef GRID_HPP
#define GRID_HPP

#include "Point2.hpp"
#include "Point3.hpp"
#include "UnitSquareMapping.hpp"
#include <QPainter>

class Grid {
public:
    Grid( int newSize, const UnitSquareMapping* );
    ~Grid();

    const int& size();
    void draw( QPainter& painter );
    void drawCell( QPainter&, const int, const int, double, double,
                   double ) const;

    double getHeight( int x, int y ) const;
    int getCellIndex( const double, const double ) const;
    Point3d getCellNormal( int x, int y ) const;

private:
    int _size;

    const double _xStep;
    const double _yStep;

    const int _imgHeight;

    const UnitSquareMapping* _mapping;
};

#endif // GRID_HPP
