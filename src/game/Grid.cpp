#include "Grid.hpp"
#include <iostream>
#include <cassert>

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

int& Grid::size()
{
    return _size;
}

double Grid::getHeight(int x, int y) const
{
    assert(x < _size);
    assert(y < _size);

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

void Grid::drawCell( QPainter& painter, const int x, const int y, const double opacity ) const {

    Point2d p1( x * xStep, y * yStep );

    double rescaleOpacity = ( opacity + 0.0 ) / 20.0;

    int r = std::min( 255.0, 255 * rescaleOpacity );
    int b = r;
    int g = r;
    if ( rescaleOpacity < 0.0 || rescaleOpacity > 1.0 ) {
        r = 0.0;
    } else if ( rescaleOpacity > 1.0 ) {
        g = 0.0;
    }

    QColor color( r, g, b );

#ifdef DEBUG
    std::cout << "cell color: " << r << ", " << g << ", " << b << std::endl;
#endif

    int nItems = 20;
    for ( int nx = 1; nx < nItems; ++nx ) {
        for ( int ny = 1; ny < nItems; ++ny ) {
            Point2d p_ = p1 + Point2d( nx / static_cast<double>(nItems) * xStep, ny / static_cast<double>(nItems) * yStep );
            Point2d p = util::scale( p_, imgHeight, scaling, *mapping );

            QPen pen( color, 1 );
            painter.setPen( pen );

            QBrush brush( color );
            painter.setBrush( brush );

            painter.drawEllipse( QPointF( p.x(), p.y() ), 1, 1 );
        }
    }
}
