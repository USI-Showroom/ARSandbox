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
      _terrain( _width * _height, 0.0 ), _water( _width * _height, 0.0 ),
      _sediment( _width * _height, 0.0 ), _leftFlux( _width * _height ),
      _rightFlux( _width * _height ), _topFlux( _width * _height ),
      _bottomFlux( _width * _height ), _u( _width * _height ),
      _v( _width * _height ),
      _mapping( mapping ), _grid( nullptr ) {}

Simulation::~Simulation() {}

const double Simulation::water(int x, int y) {
	int idx = y * _width + x;
	if (idx > 0 && idx < _water.size()) {
		return _water.at(idx);
	} else {
		return 0.0;
	}
}

const double Simulation::sediment(int x, int y) {
	int idx = y * _width + x;
	if (idx > 0 && idx < _sediment.size()) {
		return _sediment[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::terrain(int x, int y) {
	int idx = y * _width + x;
	if (idx > 0 && idx < _terrain.size()) {
		return _terrain[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::rightFlux(int x, int y) {
	int idx = y * _width + x;
	if (idx > 0 && idx < _rightFlux.size()) {
		return _rightFlux[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::bottomFlux(int x, int y) {
	int idx = y * _width + x;
	if (idx > 0 && idx < _bottomFlux.size()) {
		return _bottomFlux[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::topFlux(int x, int y) {
	int idx = y * _width + x;
	if (idx > 0 && idx < _topFlux.size()) {
		return _topFlux[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::leftFlux(int x, int y) {
	int idx = y * _width + x;
	if (idx > 0 && idx < _leftFlux.size()) {
		return _leftFlux[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::u(int x, int y) {
	int idx = y * _width + x;
	if (idx > 0 && idx < _u.size()) {
		return _u[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::v(int x, int y) {
	int idx = y * _width + x;
	if (idx > 0 && idx < _v.size()) {
		return _v[idx];
	} else {
		return 0.0;
	}
}

void Simulation::update( double dt ) {
	if ( _grid == nullptr ) {
#ifdef DEBUG
        std::cout << "ERROR: no grid found for simulation" << std::endl;
#endif
        return;
    }
    
    updateWaterSurface( dt );
}

void Simulation::updateWaterSurface( double dt ) {
    // flux factor
    double fluxFactor = g * A / l * dt;
    // cell size
    lx = ly = 1.0 / (_grid->size());

    double outFlow, inFlow, tempFlow;

 //    for ( int x = 0; x < _width; ++x ) {
 //        for ( int y = 0; y < _height; ++y ) {

 //            double outFlow = 0.0;
 //            double l = 0.0;
 //            int currentCell = y * _width + x;

 //            // current water height
 //            d1 = water.at( currentCell );
 //            // current terrain height
 //            const double h1 = _grid->getHeight( x, y );
 //            double heightDiff = 0.0;

 //            // flow difference between left and right flux
 //            double dflowRightLeft = 0.0;
 //            // flow difference between top and bottom flux
 //            double dflowTopBottom = 0.0;

 //            // there is flow from left
 //            if ( x > 0 ) {
 //                // height diff between current cell and left cell
 //                double waterLeft = water.at( y * _width + x - 1 );
 //                double heightLeft = _grid->getHeight( x - 1, y);
 //                heightDiff = h1 + d1 - waterLeft - heightLeft;
                
 //                l = leftFlux[currentCell] + ( fluxFactor * heightDiff );
                
 //                leftFlux[currentCell] = std::max( 0.0, l );
                
 //                outFlow += leftFlux[currentCell];
 //                inFlow += rightFlux[y * _width + x - 1];
                
 //                dflowRightLeft = rightFlux[y * _width + x - 1];

 //                if ( waterLeft < _minW ) {
 //                    _minW = waterLeft;
 //                }
 //                if ( waterLeft > _maxW ) {
 //                    _maxW = waterLeft;
 //                }
 //            } else {
 //                leftFlux[currentCell] = 0.0;
 //            }

 //            // there is flow from right
 //            if ( x < _width - 1 ) {
 //                double waterRight = water.at( y * _width + x + 1 );
 //                heightDiff = d1 - waterRight;
 //                l = rightFlux[currentCell] + ( fluxFactor * heightDiff );
 //                rightFlux[currentCell] = std::max( 0.0, l );
 //                outFlow += rightFlux[currentCell];
 //                inFlow += leftFlux[y * _width + x + 1];
 //                dflowRightLeft -= leftFlux[y * _width + x + 1];

 //                if ( waterRight < _minW ) {
 //                    _minW = waterRight;
 //                }
 //                if ( waterRight > _maxW ) {
 //                    _maxW = waterRight;
 //                }

 //            } else {
 //                rightFlux[currentCell] = 0.0;
 //            }

 //            // there is flow from above
 //            if ( y > 0 ) {
 //                double waterUp = water.at( ( y - 1 ) * _width + x );
 //                heightDiff = d1 - waterUp;
 //                l = topFlux[currentCell] + ( fluxFactor * heightDiff );
 //                topFlux[currentCell] = std::max( 0.0, l );
 //                outFlow += topFlux[currentCell];
 //                inFlow += bottomFlux[( y - 1 ) * _width + x];
 //                dflowTopBottom = bottomFlux[( y - 1 ) * _width + x];

 //                if ( waterUp < _minW ) {
 //                    _minW = waterUp;
 //                }
 //                if ( waterUp > _maxW ) {
 //                    _maxW = waterUp;
 //                }
 //            } else {
 //                topFlux[currentCell] = 0.0;
 //            }

 //            // there is flow from bottom
 //            if ( y < _height - 1 ) {
 //                double waterBottom = water.at( ( y + 1 ) * _width + x );
 //                heightDiff = d1 - waterBottom;
 //                l = bottomFlux[currentCell] + ( fluxFactor * heightDiff );
 //                bottomFlux[currentCell] = std::max( 0.0, l );
 //                outFlow += bottomFlux[currentCell];
 //                inFlow += topFlux[( y + 1 ) * _width + x];
 //                dflowTopBottom += topFlux[( y + 1 ) * _width + x];

 //                if ( waterBottom < _minW ) {
 //                    _minW = waterBottom;
 //                }
 //                if ( waterBottom > _maxW ) {
 //                    _maxW = waterBottom;
 //                }
 //            } else {
 //                bottomFlux[currentCell] = 0.0;
 //            }

	// 		// compute scaling factor
	// 		double z = std::fabs(outFlow) < 1e-8 ? 0 : t(water[currentCell]*lx*ly) / (outFlow * dt);
	// 		double K = std::min(1.0, z);

	// 		// scale the flows
	// 		leftFlux[currentCell] *= K;
	// 		bottomFlux[currentCell] *= K;
	// 		topFlux[currentCell] *= K;
	// 		rightFlux[currentCell] *= K;
	// 	}
	// }

	for ( int x = 0; x < _width; ++x ) {
        for ( int y = 0; y < _height; ++y ) {

        	inFlow = 0.0; outFlow = 0.0;
        	inFlow += rightFlux(x-1, y);
        	inFlow += topFlux(x, y-1);
        	inFlow += leftFlux(x+1, y);
        	inFlow += bottomFlux(x, y+1);

        	outFlow += rightFlux(x,y);
        	outFlow += topFlux(x,y);
        	outFlow += leftFlux(x,y);
        	outFlow += bottomFlux(x,y);

        	double dv = dt * ( inFlow - outFlow );
            d2 = dv / ( lx * ly );

            d1 = water(x,y);
            d2 += d1;

            // calculate average amount of water that passes through cell
            double dwx = rightFlux(x - 1, y) - leftFlux(x, y) + rightFlux(x, y) - leftFlux(x + 1, y);
            // TODO: possible bug
            double dwy = topFlux(x, y - 1) - bottomFlux(x, y) + bottomFlux(x, y) - topFlux(x, y + 1);
            double avgWaterX = 0.5 * dwx;
            double avgWaterY = 0.5 * dwy;

            double dbar = 0.5 * (d1 + d2);

            double uu = dwx / (dbar * lx);
            double vv = dwy / (dbar * ly);


            // simulate erosion
            // sediment capacity constant
            const double Kc = 15.0;
            // dissolving constant
            const double Ks = 0.001;
            // deposition constant
            const double Kd = 0.001;
            // evaporation constant
            const double Ke = 0.0004;

            // local velocity
            double uV = u(x,y);
            double vV = v(x,y);


            double angle = fabs(1.0 - _grid->getCellNormal(x, y).z());
            double C = Kc * angle * ( sqrt(uV*uV + vV*vV)  );

            double s1;
            double st = sediment(x,y);
            if ( C > st ) {
                s1 += Ks * ( C - st );
            } else {
                s1 -= Kd * ( st - C );
            }

            sediment[currentCell] = 

            // update min and max sediment for current cell
            if ( sediment(x,y) < _minS ) {
                _minS = sediment(x,y);
            }
            if ( sediment(x,y) > _maxS ) {
                _maxS = sediment(x,y);
            }

            // simulate evaporation
            d2 *= ( 1 - Ke * dt );

            water[currentCell] = d2;

            // update min and max for water
            double waterCurrent = water(x,y);
            if ( waterCurrent < _minW ) {
                _minW = waterCurrent;
            }
            if ( waterCurrent > _maxW ) {
                _maxW = waterCurrent;
            }
        }
    }
}

void Simulation::addWaterSource( const int cellIndex, const double amount ) {
    // water[cellIndex] = amount;
    // _minW = amount;
    // _maxW = amount;
    // _minS = amount;
    // _maxS = amount;
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
