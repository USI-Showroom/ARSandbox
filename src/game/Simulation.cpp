#include "Simulation.hpp"
#include <cmath>
#include <iostream>
#include "util.hpp"

double Simulation::_minW = 0.0;
double Simulation::_maxW = 0.0;
double Simulation::_minS = 0.0;
double Simulation::_maxS = 0.0;

Simulation::Simulation( int newWidth, int newHeight,
                        const UnitSquareMapping& mapping )
    : _width( newWidth ), _height( newHeight ),
      
      _terrain ( _width * _height, 0.0 ),
      _water   ( _width * _height, 0.0 ),
      _sediment( _width * _height, 0.0 ),
      
      _leftFlux  ( _width * _height, 0.0 ),
      _rightFlux ( _width * _height, 0.0 ), 
      _topFlux   ( _width * _height, 0.0 ),
      _bottomFlux( _width * _height, 0.0 ),
      
      _u ( _width * _height, 0.0 ),
      _v ( _width * _height, 0.0 ),
      _s1( _width * _height, 0.0 ),
      
      _mapping( mapping ), _grid( nullptr ), newWater(false) {}

Simulation::~Simulation() {}

// Accessor methods for arrays
//
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

    double outFlow, inFlow, tempFlow, dh, d1, d2, h1;
    outFlow = inFlow = tempFlow = dh = d1 = d2 = h1 = 0.0;

    double d1l, h1l, dhl, d1t, h1t, dht, d1r, h1r, dhr, d1b, h1b, dhb;

    // compute outFlows for inner cells
    // inner cells have all four neighbors defined
    for ( int y = 1; y < _height-1; ++y ) {
    	for ( int x = 1; x < _width-1; ++x ) {
        	
        	d1 = water(x,y);
        	h1 = _grid->getHeight(x,y);
        	
        	// left neighbor
        	d1l = water(x-1, y);
        	h1l = _grid->getHeight(x-1, y);
        	dhl = h1 + d1 - h1l - d1l;

        	// top neighbor
        	d1t = water(x, y-1);
        	h1t = _grid->getHeight(x, y-1);
        	dht = h1 + d1 - h1t - d1t;

        	// right neighbor
        	d1r = water(x+1, y);
        	h1r = _grid->getHeight(x+1, y);
        	dhr = h1 + d1 - h1r - d1r;

        	// bottom neighbor
        	d1b = water(x, y+1);
        	h1b = _grid->getHeight(x, y+1);
        	dhb = h1 + d1 - h1b - d1b;

        	// update fluxes
        	_leftFlux  [y * _width + x] = std::max(0.0, fluxFactor * dhl);
        	_topFlux   [y * _width + x] = std::max(0.0, fluxFactor * dht);
        	_rightFlux [y * _width + x] = std::max(0.0, fluxFactor * dhr);
        	_bottomFlux[y * _width + x] = std::max(0.0, fluxFactor * dhb);

        	// outFlow
        	outFlow += _leftFlux  [y * _width + x];
        	outFlow += _topFlux   [y * _width + x];
        	outFlow += _rightFlux [y * _width + x];
        	outFlow += _bottomFlux[y * _width + x];

			// compute scaling factor
			double z = std::fabs(outFlow) < 1e-8 ? 0 : (d1*lx*ly) / (outFlow * dt);
			double K = std::min(1.0, z);

			// scale the outflows
			_leftFlux  [y * _width + x] *= K;
			_topFlux   [y * _width + x] *= K;
			_rightFlux [y * _width + x] *= K;
			_bottomFlux[y * _width + x] *= K;
        }
    }


	for ( int y = 0; y < _width; y+=(_width-1) ) {
		for ( int x = 0; x < _width; x+=(_width-1) ) {

		    d1 = water(x,y);
		    h1 = _grid->getHeight(x,y);
		    
		    // left neighbor
        	d1l = x == 0 ? 0.0 : water(x-1, y);
        	h1l = x == 0 ? 0.0 : _grid->getHeight(x-1, y);
        	dhl = h1 + d1 - h1l - d1l;

        	// top neighbor
        	d1t = y == 0 ? 0.0 : water(x, y-1);
        	h1t = y == 0 ? 0.0 : _grid->getHeight(x, y-1);
        	dht = h1 + d1 - h1t - d1t;

        	// right neighbor
        	d1r = x == _width - 1 ? 0.0 : water(x+1, y);
        	h1r = x == _width - 1 ? 0.0 : _grid->getHeight(x+1, y);
        	dhr = h1 + d1 - h1r - d1r;

        	// bottom neighbor
        	d1b = y == _width - 1 ? 0.0 : water(x, y+1);
        	h1b = y == _width - 1 ? 0.0 : _grid->getHeight(x, y+1);
        	dhb = h1 + d1 - h1b - d1b;

			// update fluxes
			_leftFlux  [y * _width + x] = std::max(0.0, fluxFactor * dhl);
			_topFlux   [y * _width + x] = std::max(0.0, fluxFactor * dht);
			_rightFlux [y * _width + x] = std::max(0.0, fluxFactor * dhr);
			_bottomFlux[y * _width + x] = std::max(0.0, fluxFactor * dhb);

			// outFlow
        	outFlow += _leftFlux  [y * _width + x];
        	outFlow += _topFlux   [y * _width + x];
        	outFlow += _rightFlux [y * _width + x];
        	outFlow += _bottomFlux[y * _width + x];

			// compute scaling factor
			double z = std::fabs(outFlow) < 1e-8 ? 0 : (d1*lx*ly) / (outFlow * dt);
			double K = std::min(1.0, z);

			// scale the outflows
			_leftFlux  [y * _width + x] *= K;
			_topFlux   [y * _width + x] *= K;
			_rightFlux [y * _width + x] *= K;
			_bottomFlux[y * _width + x] *= K;
		}
	}

	// compute volume of water passing through cell(x,y)
    for ( int y = 0; y < _height; ++y ) {
		for ( int x = 0; x < _width; ++x ) {

        	inFlow = 0.0; outFlow = 0.0;
        	
        	inFlow += x == 0          ? 0.0 : rightFlux (x-1, y);
        	inFlow += y == 0          ? 0.0 : topFlux   (x, y-1);
        	inFlow += x == _width - 1 ? 0.0 : leftFlux  (x+1, y);
        	inFlow += y == _width - 1 ? 0.0 : bottomFlux(x, y+1);

        	outFlow += rightFlux(x,y);
        	outFlow += topFlux(x,y);
        	outFlow += leftFlux(x,y);
        	outFlow += bottomFlux(x,y);

        	double dv = dt * ( inFlow - outFlow );
        	
        	if (fabs(dv) < 1e-8) {
        		continue;
        	}
            
            d2 = dv / ( lx * ly );

            d1 = water(x,y);
            d2 += d1;

            // calculate average amount of water that passes through cell
            double dwx = (rightFlux(x-1, y) - leftFlux(x, y) + rightFlux(x, y) - leftFlux(x-1, y)) * 0.5;
            // TODO: possible bug
            double dwy = (topFlux(x, y+1) - bottomFlux(x, y) + bottomFlux(x, y) - topFlux(x, y+1)) * 0.5;

            double dbar = 0.5 * (d1 + d2);

            double uu = dwx / (dbar * lx);
            double vv = dwy / (dbar * ly);

            // update velocity field
            _u[y * _width + x] = uu;
            _v[y * _width + x] = vv;

    		
    		// simulate erosion
            
            // local velocity
            double uV = u(x,y);
            double vV = v(x,y);


            double angle = fabs(1.0 - _grid->getCellNormal(x, y).z());
            double C = Kc * sin(acos(angle)) * ( sqrt(uV*uV + vV*vV)  );

            double st = sediment(x,y);
            if ( C > st ) {
            	_terrain[y * _width + x] -= Ks * ( C - st );
                _s1[y * _width + x] += Ks * ( C - st );
            } else {
            	_terrain[y * _width + x] += Ks * ( C - st );
                _s1[y * _width + x] -= Kd * ( st - C );
            }

            // simulate evaporation
            d2 *= ( 1 - Ke * dt );

            _water[y * _width + x] = d2;
        }
    }

    for ( int y = 0; y < _height; ++y ) {
		for ( int x = 0; x < _width; ++x ) {

			// local velocity
            double uV = u(x,y);
            double vV = v(x,y);

            // where the flow comes from
            double sedimentFromX = static_cast<double>(x) * uV * dt;
            double sedimentFromY = static_cast<double>(y) * vV * dt;

           	int sx = floor(sedimentFromX);
           	int sy = floor(sedimentFromY);

            util::clamp(sx, 0, _sediment.size());
            util::clamp(sy, 0, _sediment.size());

            _sediment[sy * _width + sx] = _s1[x * _width + x];

            // update min and max terrain for current cell
            if ( terrain(x,y) < _minS ) {
                _minS = terrain(x,y);
            }
            if ( terrain(x,y) > _maxS ) {
                _maxS = terrain(x,y);
            }


            double waterCurrent = _water[y * _width +x];
            // update min and max for water
            if ( waterCurrent < _minW ) {
                _minW = waterCurrent;
            }
            if ( waterCurrent > _maxW ) {
                _maxW = waterCurrent;
            }

            if (newWater) {
	        	std::cout << "water heights:";
		        for ( int y = 0; y < _height; ++y ) {
					for ( int x = 0; x < _width; ++x ) {
						std::cout << water(x,y) << ", ";
					}
					std::cout << std::endl;
				}

				std::cout << "terrain heights:";
		        for ( int y = 0; y < _height; ++y ) {
					for ( int x = 0; x < _width; ++x ) {
						std::cout << terrain(x,y) << ", ";
					}
					std::cout << std::endl;
				}
			}
	        
        } // end x for loop
    } // end y for loop
    assert(true==false);
}

void Simulation::addWaterSource( const int cellIndex, const double amount ) {
    _water[cellIndex] += amount;
    newWater = true;
}

const double Simulation::getWaterAt( int x, int y ) {
    return _water.at( y * _height + x );
}

const double Simulation::getTerrainAt( int x, int y ) {
    return _terrain.at( y * _height + x );
}

const double Simulation::getSedimentAt( int x, int y ) {
    return _sediment.at( y * _height + x );
}

void Simulation::setGrid( Grid* newGrid ) {
	_grid = newGrid;
	// set cell size
    lx = ly = 0.25;
}

void Simulation::draw( QPainter& painter, const UnitSquareMapping& mapping ) {
    double waterHeight, terrainHeight, sedimentHeight;
    waterHeight = terrainHeight = sedimentHeight = 0.0;

    for ( int y = 0; y < _height; ++y ) {
    	for ( int x = 0; x < _width; ++x ) {
            
            terrainHeight = (terrain(x,y) - _minS) / (_maxS - _minS);
            waterHeight = (water(x,y) - _minW) / (_maxW - _minW);
            sedimentHeight = sediment(x,y);

            _grid->drawCell( painter, x, y, terrainHeight, waterHeight,
                             sedimentHeight );
        }
    }
}
