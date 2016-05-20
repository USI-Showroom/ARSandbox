#include "Simulation.hpp"
#include <cmath>
#include <iostream>

double Simulation::_minW = 0.0;
double Simulation::_maxW = 0.0;
double Simulation::_minS = 0.0;
double Simulation::_maxS = 0.0;

Simulation::Simulation( int newWidth, int newHeight,
                        const UnitSquareMapping& mapping )
    : _width( newWidth ), _height( newHeight ),
      terrain( _width * _height, 0.0 ), water( _width * _height, 0.0 ),
      sediment( _width * _height, 0.0 ), leftFlux( _width * _height ),
      rightFlux( _width * _height ), topFlux( _width * _height ),
      bottomFlux( _width * _height ), _u( _width * _height ),
      _v( _width * _height ), _mapping( mapping ), _grid( nullptr ) {}

Simulation::~Simulation() {}

void Simulation::update( double dt ) {
    incrementWater( dt );
    updateWaterSurface( dt );
}

void Simulation::incrementWater( double dt ) {}

void Simulation::updateWaterSurface( double dt ) {
    if ( _grid == nullptr ) {
#ifdef DEBUG
        std::cout << "ERROR: no grid found for simulation" << std::endl;
#endif
        return;
    }

    // cross sectional area of the pipe
    const double A = 0.85;
    // length of virtual pipe
    const double l = 0.45;
    // gravity
    const double g = 9.81;
    // flux factor
    double fluxFactor = g * A / l;
    // cell size
    double lx, ly;
    lx = ly = 1.0;

    for ( int x = 0; x < _width; ++x ) {
        for ( int y = 0; y < _height; ++y ) {

            double inFlow = 0.0;
            double outFlow = 0.0;
            double l = 0.0;
            int currentCell = y * _width + x;

            // current water height
            const double d1 = water.at( currentCell );
            // current terrain height
            const double h1 = _grid->getHeight( x, y );
            double heightDiff = 0.0;

            // flow difference between left and right flux
            double dflowRightLeft = 0.0;
            // flow difference between top and bottom flux
            double dflowTopBottom = 0.0;

            // there is flow from left
            if ( x > 0 ) {
                // height diff between current cell and left cell
                double waterLeft = water.at( y * _width + x - 1 );
                heightDiff = d1 - waterLeft;
                l = leftFlux[currentCell] + ( fluxFactor * heightDiff );
                leftFlux[currentCell] = std::max( 0.0, l );
                outFlow += leftFlux[currentCell];
                inFlow += rightFlux[y * _width + x - 1];
                dflowRightLeft = rightFlux[y * _width + x - 1];

                if ( waterLeft < _minW ) {
                    _minW = waterLeft;
                }
                if ( waterLeft > _maxW ) {
                    _maxW = waterLeft;
                }
            } else {
                leftFlux[currentCell] = 0.0;
            }

            // there is flow from right
            if ( x < _width - 1 ) {
                double waterRight = water.at( y * _width + x + 1 );
                heightDiff = d1 - waterRight;
                l = rightFlux[currentCell] + ( fluxFactor * heightDiff );
                rightFlux[currentCell] = std::max( 0.0, l );
                outFlow += rightFlux[currentCell];
                inFlow += leftFlux[y * _width + x + 1];
                dflowRightLeft -= leftFlux[y * _width + x + 1];

                if ( waterRight < _minW ) {
                    _minW = waterRight;
                }
                if ( waterRight > _maxW ) {
                    _maxW = waterRight;
                }

            } else {
                rightFlux[currentCell] = 0.0;
            }

            // there is flow from above
            if ( y > 0 ) {
                double waterUp = water.at( ( y - 1 ) * _width + x );
                heightDiff = d1 - waterUp;
                l = topFlux[currentCell] + ( fluxFactor * heightDiff );
                topFlux[currentCell] = std::max( 0.0, l );
                outFlow += topFlux[currentCell];
                inFlow += bottomFlux[( y - 1 ) * _width + x];
                dflowTopBottom = bottomFlux[( y - 1 ) * _width + x];

                if ( waterUp < _minW ) {
                    _minW = waterUp;
                }
                if ( waterUp > _maxW ) {
                    _maxW = waterUp;
                }
            } else {
                topFlux[currentCell] = 0.0;
            }

            // there is flow from bottom
            if ( y < _height - 1 ) {
                double waterBottom = water.at( ( y + 1 ) * _width + x );
                heightDiff = d1 - waterBottom;
                l = bottomFlux[currentCell] + ( fluxFactor * heightDiff );
                bottomFlux[currentCell] = std::max( 0.0, l );
                outFlow += bottomFlux[currentCell];
                inFlow += topFlux[( y + 1 ) * _width + x];
                dflowTopBottom += topFlux[( y + 1 ) * _width + x];

                if ( waterBottom < _minW ) {
                    _minW = waterBottom;
                }
                if ( waterBottom > _maxW ) {
                    _maxW = waterBottom;
                }
            } else {
                bottomFlux[currentCell] = 0.0;
            }

// // compute scaling factor
// double z = (water[currentCell]*lx*ly) / (outFlow * dt);
// double K = std::min(1.0, z);

// // scale the flows
// leftFlux[currentCell] *= K;
// bottomFlux[currentCell] *= K;
// topFlux[currentCell] *= K;
// rightFlux[currentCell] *= K;

#ifdef DEBUG
            std::cout << "currentCell: " << currentCell << "\n"
                      << "inFlow: " << inFlow << "\n"
                      << "outFlow: " << outFlow << "\n";
#endif
            // update water velocity field with net volume change for water
            double dv = dt * ( inFlow - outFlow );
            double d2 = dv / ( lx * ly );
            water[currentCell] = d1 + d2;

            // calculate average amount of water that passes through cell
            double dwx =
                dflowRightLeft - leftFlux[currentCell] + rightFlux[currentCell];
            double dwy =
                dflowTopBottom - topFlux[currentCell] + bottomFlux[currentCell];
            double avgWaterX = 0.5 * dwx;
            double avgWaterY = 0.5 * dwy;

            double avgWater = avgWaterX + avgWaterY;
            // update velocity fields
            if ( avgWater == 0.0 ) {
                _u[currentCell] = _v.at( currentCell ) = 0.0;
            } else {
                _u[currentCell] = avgWaterX;
                _v[currentCell] = avgWaterY;
            }

            // simulate erosion
            // sediment capacity constant
            const double Kc = 25.0;
            // dissolving constant
            const double Ks = 0.001;
            // deposition constant
            const double Kd = 0.001;

            // local velocity
            double uV = _u[currentCell];
            double vV = _v[currentCell];

            double angle = 0.5;
            double C = Kc * angle * ( uV + vV );

            double st = sediment[currentCell];
            if ( C > st ) {
                sediment[currentCell] += Ks * ( C - st );
            } else {
                sediment[currentCell] += Kd * ( st - C );
            }

            // update min and max sediment for current cell
            if ( sediment[currentCell] < _minS ) {
                _minS = sediment[currentCell];
            }
            if ( sediment[currentCell] > _maxS ) {
                _maxS = sediment[currentCell];
            }

            // simulate evaporation
            const double Ke = 0.0004;
            water[currentCell] *= ( 1 - Ke * dt );

            // update min and max for water
            double waterCurrent = water[currentCell];
            if ( waterCurrent < _minW ) {
                _minW = waterCurrent;
            }
            if ( waterCurrent > _maxW ) {
                _maxW = waterCurrent;
            }
        }
    } // end for loop
#ifdef DEBUG
    std::cout << std::endl;
    std::cout << "Water heights at cells:" << std::endl;
    // print out the grid values
    for ( int x = 0; x < _width; ++x ) {
        for ( int y = 0; y < _height; ++y ) {
            std::cout << water[y * _width + x] << ",  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
#endif
}

void Simulation::addWaterSource( const int cellIndex, const double amount ) {
    water[cellIndex] = amount;
    _minW = amount;
    _maxW = amount;
    _minS = amount;
    _maxS = amount;
}

const double Simulation::getWaterAt( int x, int y ) {
    return water.at( y * _height + x );
}

const double Simulation::getTerrainAt( int x, int y ) {
    return terrain.at( y * _height + x );
}

const double Simulation::getSedimentAt( int x, int y ) {
    return sediment.at( y * _height + x );
}

void Simulation::setGrid( const Grid* newGrid ) { _grid = newGrid; }

void Simulation::draw( QPainter& painter, const UnitSquareMapping& mapping ) {
    int currentCell = 0;
    double waterHeight, terrainHeight, sedimentHeight;
    waterHeight = terrainHeight = sedimentHeight = 0.0;

    for ( int x = 0; x < _width; ++x ) {
        for ( int y = 0; y < _height; ++y ) {
            currentCell = y * _width + x;

            terrainHeight = terrain[currentCell];
            waterHeight = water[currentCell];
            sedimentHeight = sediment[currentCell];

            _grid->drawCell( painter, x, y, terrainHeight, waterHeight,
                             sedimentHeight );
        }
    }
}
