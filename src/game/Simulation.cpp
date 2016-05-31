#include "Simulation.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <random>
#include "util.hpp"


double Simulation::_minW = 0.0;
double Simulation::_maxW = 0.0;
double Simulation::_minS = 0.0;
double Simulation::_maxS = 0.0;

std::mt19937 rnd;

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

      _mapping( mapping ), _grid( nullptr ),
      _newWater(false), _isRaining(false),

      // init constants
      A(0.00005), l(1.0), g(9.81), lx(1.0), ly(1.0),
      Kc(25.0), Ks(0.0001*12*10), Kd(0.0001*12*10), Ke(0.00011*0.5)

      {}

Simulation::~Simulation() {}

// Accessor methods for arrays
//
const double Simulation::water(int x, int y) {
	int idx = y * _width + x;
	if (idx >= 0 && idx < _water.size()) {
		return _water.at(idx);
	} else {
		return 0.0;
	}
}

const double Simulation::sediment(int x, int y) {
	int idx = y * _width + x;
	if (idx >= 0 && idx < _sediment.size()) {
		return _sediment[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::terrain(int x, int y) {
	int idx = y * _width + x;
	if (idx >= 0 && idx < _terrain.size()) {
		return _terrain[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::rightFlux(int x, int y) {
	int idx = y * _width + x;
	if (idx >= 0 && idx < _rightFlux.size()) {
		return _rightFlux[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::bottomFlux(int x, int y) {
	int idx = y * _width + x;
	if (idx >= 0 && idx < _bottomFlux.size()) {
		return _bottomFlux[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::topFlux(int x, int y) {
	int idx = y * _width + x;
	if (idx >= 0 && idx < _topFlux.size()) {
		return _topFlux[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::leftFlux(int x, int y) {
	int idx = y * _width + x;
	if (idx >= 0 && idx < _leftFlux.size()) {
		return _leftFlux[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::u(int x, int y) {
	int idx = y * _width + x;
	if (idx >= 0 && idx < _u.size()) {
		return _u[idx];
	} else {
		return 0.0;
	}
}

const double Simulation::v(int x, int y) {
	int idx = y * _width + x;
	if (idx >= 0 && idx < _v.size()) {
		return _v[idx];
	} else {
		return 0.0;
	}
}

void Simulation::toggleRain()
{
    _isRaining = _isRaining ? false : true;
    if (_isRaining) {
        std::cout << "rain on";
    } else {
        std::cout << "rain off";
    }
}

void Simulation::rain(double dt)
{
    int a = 1;
    int b = _width - 2;
    std::uniform_int_distribution<ushort> rndInt(a, b);

    double amount = 1.0/16.0;
    for ( uint i = 0; i < 100; i++ ) {
        uint x = rndInt(rnd);
        uint y = rndInt(rnd);

        _water[ (y-1) * _width + x-1] += amount;
        _water[ (y-1) * _width + x  ] += amount;
        _water[ (y-1) * _width + x+1] += amount;
        _water[  y    * _width + x-1] += amount;
        _water[  y    * _width + x  ] += amount;
        _water[  y    * _width + x+1] += amount;
        _water[ (y+1) * _width + x-1] += amount;
        _water[ (y+1) * _width + x  ] += amount;
        _water[ (y+1) * _width + x+1] += amount;
    }
}

void Simulation::update( double dt ) {
	if ( _grid == nullptr ) {
#ifdef DEBUG
        std::cout << "ERROR: no grid found for simulation" << std::endl;
#endif
        return;
    }

    // if (_isRaining) rain(dt);



    updateWaterSurface(dt);
}

void Simulation::updateWaterSurface( double dt ) {
    if(!_mapping.initialized()) return;

    // flux factor
    double fluxFactor = g * A / l * dt;


    d1  = d2  = b1  = 0.0;
    d1l = b1l = dhl = 0.0;
    d1t = b1t = dht = 0.0;
    d1r = b1r = dhr = 0.0;
    d1b = b1b = dhb = 0.0;

    // compute outFlows for cells
    // boundaries are taken into consideration
    // because accessors return 0 if cell(x, y) are outside of grid
    for ( std::set<int>::iterator idx = active_cells.begin();
          idx != active_cells.end();
          idx ++ )
    {
        int x = *idx / _height;
        int y = *idx % _width;

        outFlow = inFlow = 0.0;

    	d1 = water(x,y);
    	b1 = _grid->getHeight(x,y) + terrain(x,y); //_terrain[y * _width + x];

    	// left neighbor
    	d1l = water(x-1, y);
    	b1l = _grid->getHeight(x-1, y) + terrain(x-1,y); //_terrain[y * _width + x -1];
    	dhl = b1 + d1 - b1l - d1l;

    	// top neighbor
    	d1t = water(x, y-1);
    	b1t = _grid->getHeight(x, y-1) + terrain(x,y-1); //_terrain[(y - 1) * _width + x];
    	dht = b1 + d1 - b1t - d1t;

    	// right neighbor
    	d1r = water(x+1, y);
    	b1r = _grid->getHeight(x+1, y) + terrain(x+1,y); //_terrain[y * _width + x + 1];
    	dhr = b1 + d1 - b1r - d1r;

    	// bottom neighbor
    	d1b = water(x, y+1);
    	b1b = _grid->getHeight(x, y+1) + terrain(x,y+1); //_terrain[(y+1) * _width + x];
    	dhb = b1 + d1 - b1b - d1b;

        if (x == 0)
            dhl = 0.0;

        if (y == 0)
            dht = 0.0;

        if (x == _width -1)
            dhr = 0.0;

        if (y == _height -1)
            dhb = 0.0;


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

#ifdef DEBUG
        // if ( dhl != dhl || dht != dht || dhr != dhr || dhb != dhb ) {
            std::cout << "\ndh: " << x<<" "<<y<<
                         "\ndhl: " << _leftFlux  [y * _width + x] <<
                         "\ndht: " << _topFlux   [y * _width + x] <<
                         "\ndhr: " << _rightFlux [y * _width + x] <<
                         "\ndhb: " << _bottomFlux[y * _width + x] <<
            std::endl;
        // }
#endif
    }

	// compute volume of water passing through cell(x,y)
    for ( auto idx = active_cells.begin(); idx != active_cells.end(); idx ++ )
    {
        int x = *idx / _height;
        int y = *idx % _width;
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

    	if (fabs(dv) < 1e-12) {
    		continue;
    	}

        d2 = dv / ( lx * ly );

        d1 = water(x,y);
        d2 += d1;

        double dbar = 0.5 * (d1 + d2);

        // to avoid velocity get to 0
        if (fabs(dbar) < 1e-8) {
            dbar = 1.0;
        }

        // calculate average amount of water that passes through cell
        double dwx = (rightFlux(x-1, y) - leftFlux(x, y) + rightFlux(x, y) - leftFlux(x+1, y)) * 0.5;
        // TODO: possible bug
        double dwy = (topFlux(x, y-1) - bottomFlux(x, y) + topFlux(x, y) - bottomFlux(x, y+1)) * 0.5;

        // std::cout << "\ndwx: " << dwx << ", dwy: " << dwy << std::endl;

        double uu = dwx / (dbar * lx);
        double vv = dwy / (dbar * ly);

        if (fabs(dbar) < 1e-8) {
            uu = 0.0;
            vv = 0.0;
        }

        // update velocity field
        _u[y * _width + x] = uu;
        _v[y * _width + x] = vv;


		// simulate erosion
        double nh = _grid->getCellNormal(x, y).z();
        double angle = max(0.01, fabs(1.0 - nh));
        double sq = sqrt(uu*uu + vv*vv);
        double C = Kc * angle * sq * (std::min(water(x,y),0.01)/0.01) ;

        double st = sediment(x,y);

        if ( C > st ) {
        	_terrain[y * _width + x] -= Ks * ( C - st );
            _s1[y * _width + x] = sediment(x,y) + Ks * ( C - st );
        } else {
        	_terrain[y * _width + x] += Kd * ( st - C );
            _s1[y * _width + x] = sediment(x,y) - Kd * ( st - C );
        }

#ifdef DEBUG
        assert(_terrain[y * _width + x] == _terrain[y * _width + x]);

		std::cout
            <<"\n"<< x<<" "<<y
    		<< "\nangle: "<< angle << ", "
    		<< "\nterrain: " << _terrain[y * _width + x] << ", "
    		<< "\nst: " << st << ", "
    		<< "\nC: " << C << ", "
    		<< "\nuu: " << uu << ", "
    		<< "\nvv: " << vv << ", "
    		<< "\nsqrt: " << sq
            << "\ninFlow: " << inFlow
            << "\noutFlow:" << outFlow
            << "\ndv:" << dv
            << "\ndw x: " << dwx
            << "\ndw y: " << dwy
            << "\ndbar: " << dbar
    	   << std::endl;
#endif
        // simulate evaporation
        d2 *= ( 1 - Ke * dt );

        _water[y * _width + x] = d2;
    }

    Simulation::_minW = water(0,0);
    Simulation::_maxW = water(0,0);
    Simulation::_minS = terrain(0,0);
    Simulation::_maxS = terrain(0,0);

    // transports sediment from cell x,y to target cell by
    // computing final position as velocity * time
    for ( auto idx = active_cells.begin(); idx != active_cells.end(); idx ++ )
    {
        int x = *idx / _height;
        int y = *idx % _width;

        // local velocity
        double uV = u(x,y);
        double vV = v(x,y);

        // where the flow comes from
        double sedimentFromX = static_cast<double>(x) - uV * dt;
        double sedimentFromY = static_cast<double>(y) - vV * dt;

        // integer coordinates
        int x0 = floor(sedimentFromX);
        int y0 = floor(sedimentFromY);
        int x1 = x0+1;
        int y1 = y0+1;

        // interpolation factors
        float fX = sedimentFromX - x0;
        float fY = sedimentFromY - y0;

        // clamp to grid borders
        x0 = glm::clamp(x0,0,_width-1);
        x1 = glm::clamp(x1,0,_width-1);
        y0 = glm::clamp(y0,0,_height-1);
        y1 = glm::clamp(y1,0,_height-1);

        float newSediment = glm::mix(
            glm::mix(sediment(x0,y0),sediment(x1,y0),fX),
            glm::mix(sediment(x0,y1),sediment(x1,y1),fX),
            fY);
        _s1[y * _width + x] = newSediment;

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
    } // end idx for loop

    // update sediment
    for ( auto idx = active_cells.begin(); idx != active_cells.end(); idx ++ )
    {
        int x = *idx / _height;
        int y = *idx % _width;
        _sediment[y * _width + x] = _s1.at(y * _width + x);
    } // end idx for loop

#ifdef DEBUG
    std::cout << "\nwater heights:" << std::endl;
    for ( int y = 0; y < _height; ++y ) {
        for ( int x = 0; x < _width; ++x ) {
            std::cout << water(x,y) << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << "\nterrain heights:" << std::endl;
    for ( int y = 0; y < _height; ++y ) {
        for ( int x = 0; x < _width; ++x ) {
            std::cout << terrain(x,y) << ", ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    //exit(0);
#endif
}

std::vector<int> Simulation::getNeighbors(const int index) {
    std::vector<int> indices;

    if (index < 0 || index >= _width * _width) {
        return std::vector<int> ();
    }

    if (index == 0) {
        indices.push_back(index + 1);
        indices.push_back(index + _width);
        indices.push_back(index + _width - 1);

    }  else if (index > 0 && index < _width) {
        indices.push_back(index + 1);
        indices.push_back(index + _width + 1);
        indices.push_back(index + _width);
        indices.push_back(index + _width - 1);
        indices.push_back(index - 1);

    } else if ((index % _width) == _width - 1) {
        indices.push_back(index - 1);
        indices.push_back(index - _width - 1);
        indices.push_back(index - _width);
        indices.push_back(index + _width);
        indices.push_back(index + _width - 1);

    } else if (index == _width * _width - 1) {
        indices.push_back(index - 1);
        indices.push_back(index - _width - 1);
        indices.push_back(index - _width);

    } else if (index > _width * _width - _width) {
        indices.push_back(index - 1);
        indices.push_back(index - _width - 1);
        indices.push_back(index - _width);
        indices.push_back(index - _width + 1);
        indices.push_back(index + 1);

    } else if ((index % _width) == 0) {
        indices.push_back(index - _width);
        indices.push_back(index - _width + 1);
        indices.push_back(index + 1);
        indices.push_back(index + _width + 1);
        indices.push_back(index + _width);

    } else if (index > _width && index < _width * (_width - 1) - 1) {
        indices.push_back(index - 1);
        indices.push_back(index - 1 - _width);
        indices.push_back(index - _width);
        indices.push_back(index - _width + 1);
        indices.push_back(index + 1);
        indices.push_back(index + _width + 1);
        indices.push_back(index + _width - 1);
    }
    return indices;
}

void Simulation::addWaterSource( const int cellIndex, const double amount ) {
    std::vector<int> neighbors = getNeighbors(cellIndex);
    for (size_t i = 0; i < neighbors.size(); i++) {
        if (i%2 == 0) {
            _water[cellIndex] = amount / 1.5;
        } else {
            _water[cellIndex] = amount;
        }
    }
    std::cout << "added water at " << neighbors.size() << " cells"  << std::endl;
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
}

void Simulation::draw( QPainter& painter ) {
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
