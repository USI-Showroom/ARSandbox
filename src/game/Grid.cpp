#include "Grid.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <QString>

#include "util.hpp"

#ifdef NO_KINECT
static const int scaling=3;
#else
static const int scaling=7;
#endif

Grid::Grid(int newSize, const UnitSquareMapping *newMapping)
    :   _size(newSize),
        xStep(1.0 / _size), yStep(1.0 / _size),
        imgHeight(424),
        mapping(newMapping)
{

}

Grid::~Grid()
{

}

const int& Grid::size()
{
    return _size;
}

double Grid::getHeight(int x, int y) const
{
#ifdef DEBUG
    assert(x < _size);
    assert(y < _size);
#endif
    
    if (x < _size || y < _size)
        return 0;
    
    const int sw = 3;
    const int sh = 3;
    const int npoints = sw*sh;
    double perc = 1e-2;

    std::vector<double> samples;
    samples.push_back(10.0);
    samples.push_back(40.0);
    samples.push_back(90.0);

    double avg = 0;
    double h, a, b;
    for (int i = 0; i < npoints; ++i) {

        a = samples.at(i/sw);
        b = samples.at(i%sh);

        Point2d p( a * xStep * perc * (x + 1.0), b * yStep * perc * (y + 1.0));
        h = mapping->getHeightFromParam(p);

        avg += h;
    }

    avg /= npoints;
    return avg;
}

// center, boundaries
// z -> getHeightFromParam
Point3d Grid::getCellNormal( int x, int y ) const
{
    double xx = static_cast<double>(x) / _size;
    double yy = static_cast<double>(y) / _size;

    const Point2d a1 = Point2d(xx, yy);
    const Point2d a2 = a1 + Point2d(0.05, 0.0);
    const Point2d a3 = a2 + Point2d(0.0,-0.05);

    const double h1 = mapping->getHeightFromParam(a1);
    const double h2 = mapping->getHeightFromParam(a2);
    const double h3 = mapping->getHeightFromParam(a3);

    const Point3d p1 = Point3d(a1.x(), a1.y(), h1);
    const Point3d p2 = Point3d(a2.x(), a2.y(), h2);
    const Point3d p3 = Point3d(a3.x(), a3.y(), h3);

    Point3d n = ( p2 - p1 ) ^ ( p3 - p1 );
    n.normalize();

    return n;
}

int Grid::getCellIndex(const double x, const double y)
{
    const int xx = floor(x / xStep);
    const int yy = floor(y / yStep);
#ifdef DEBUG
    std::cout << "xx: " << xx << ", " << "yy: " << yy << std::endl;
#endif
    return yy * _size + xx;
}

void Grid::draw(QPainter &painter)
{
    for (int i = 0; i < _size; ++i) {
        for (int j = 0; j < _size; ++j) {

            Point2d p1(i*xStep, j*yStep);
            Point2d p2(xStep, yStep);

            // draw gridlines
            for (int nx = 0; nx < 10; ++nx) {
                for (int ny = 0; ny < 10; ++ny) {
                    Point2d p_ = p1 + Point2d(nx / 9.0 * xStep, ny / 9.0 * yStep);
                    Point2d p = mapping->fromParameterization(p_);
                    p.y() = imgHeight - p.y();

                    if (p.y() == imgHeight)
                        --p.y();
                    assert(p.y() >= 0);
                    assert(p.y() < imgHeight);
                    p*=scaling;
                    QPen pen(Qt::gray, 2);
                    painter.setPen(pen);

                    QBrush brush(Qt::gray);
                    painter.setBrush(brush);

                    painter.drawEllipse(QPointF(p.x(), p.y()), 1, 1);
                }
            }
        }
    }
}

void Grid::drawCell( QPainter& painter, const int x, const int y, double terrain, double water, double sediment ) const {

    Point2d p1( x * xStep, y * yStep );

    int r = static_cast<int>(std::min(255.0, std::max(0.0, terrain*255.0)));
    int g = static_cast<int>(std::min(255.0, std::max(0.0, sediment*255.0)));
    int b = static_cast<int>(std::min(255.0, std::max(0.0, water*255.0)));

    // std::cout << "Color: " << r << ", " << g << ", " << b << std::endl;
    QColor color(r,g,b,255);

    int nItems = 12;
    double nPoints = static_cast<double>(nItems);
    for ( int nx = 1; nx < nItems; ++nx ) {
        for ( int ny = 1; ny < nItems; ++ny ) {
            
            Point2d p_ = p1 + Point2d( nx / nPoints * xStep, ny / nPoints * yStep );
            Point2d p = util::scale( p_, imgHeight, scaling, *mapping );

            // painter.setRenderHint(QPainter::Antialiasing, true);

            QPen pen( color, 1 );
            painter.setPen( pen );

            QBrush brush( color );
            painter.setBrush( brush );

            painter.drawRect( p.x()-3, p.y()-3, 6, 6);



        }
    }
}
