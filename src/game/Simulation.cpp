#include "Simulation.hpp"
#include <cmath>
#include <iostream>
#include "util.hpp"
#include <glm/glm.hpp>

double Simulation::_minW = 0.0;
double Simulation::_maxW = 0.0;
double Simulation::_minS = 0.0;
double Simulation::_maxS = 0.0;

Simulation::Simulation( int newWidth, int newHeight )
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
      _s1( _width * _height, 0.0 )

      {}

Simulation::~Simulation() {}

void Simulation::addWaterSource( const int cellIndex, const double amount ) {
    _water[cellIndex] += amount;
}

void Simulation::update( double dt, const UnitSquareMapping &mapping, const Grid &grid ) {
    if(!mapping.initialized()) return;

    flowSimulation(dt, grid);
    updateWaterSurface(dt, grid);
    erosionDeposition(dt, grid);
    sedimentTransport(dt);
    //evaporation(dt);
}

// Simulation steps
//
void Simulation::flowSimulation(const double dt, const Grid &_grid)
{
    d1  = d2  = b1  = 0.0;
    d1l = b1l = dhl = 0.0;
    d1t = b1t = dht = 0.0;
    d1r = b1r = dhr = 0.0;
    d1b = b1b = dhb = 0.0;

    double fluxFactor = dt * g * A / l;
    
    // compute outFlows for cells
    // boundaries are taken into consideration
    // because accessors return 0 if cell(x, y) are outside of grid
    for ( int y = 0; y < _height; ++y ) {
        for ( int x = 0; x < _width; ++x ) {
            outFlow = inFlow = 0.0;         

            d1 = water(x,y);
            b1 = _grid.getHeight(x,y) + terrain(x,y);
            
            // left neighbor
            d1l = water(x-1, y);
            b1l = _grid.getHeight(x-1, y) + terrain(x-1,y);
            dhl = b1 + d1 - b1l - d1l;

            // top neighbor
            d1t = water(x, y-1);
            b1t = _grid.getHeight(x, y-1) + terrain(x,y-1);
            dht = b1 + d1 - b1t - d1t;

            // right neighbor
            d1r = water(x+1, y);
            b1r = _grid.getHeight(x+1, y) + terrain(x+1,y);
            dhr = b1 + d1 - b1r - d1r;

            // bottom neighbor
            d1b = water(x, y+1);
            b1b = _grid.getHeight(x, y+1) + terrain(x,y+1);
            dhb = b1 + d1 - b1b - d1b;

            if (x == 0) dhl = 0.0;

            if (y == 0) dht = 0.0;

            if (x == _width -1) dhr = 0.0;

            if (y == _height -1) dhb = 0.0;


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
            std::cout
                << "\ndh: " << x << ", " << y
                << "\ndhl: " << _leftFlux  [y * _width + x]
                << "\ndht: " << _topFlux   [y * _width + x]
                << "\ndhr: " << _rightFlux [y * _width + x]
                << "\ndhb: " << _bottomFlux[y * _width + x]
                << std::endl;
            // }
#endif
        }
    }
}


void Simulation::updateWaterSurface( double dt, const Grid &_grid )
{
    double dbar, dv, dwx, dwy, uu, vv;
    dbar = dv = dwx = dwy = uu = vv = 0.0;
    
    // compute volume of water passing through cell(x,y)
    for ( int y = 0; y < _height; ++y ) {
        for ( int x = 0; x < _width; ++x ) {

            inFlow = 0.0; outFlow = 0.0;
            
            inFlow += rightFlux(x-1, y);
            inFlow += topFlux(x, y-1);
            inFlow += leftFlux(x+1, y);
            inFlow += bottomFlux(x, y+1);

            outFlow += rightFlux(x,y);
            outFlow += topFlux(x,y);
            outFlow += leftFlux(x,y);
            outFlow += bottomFlux(x,y);

            dv = dt * ( inFlow - outFlow );
            
            if (fabs(dv) < 1e-12) {
                continue;
            }
            
            d2 = dv / ( lx * ly );

            d1 = water(x,y);
            d2 += d1;

            dbar = 0.5 * (d1 + d2);

            // to avoid velocity get to 0
            if (fabs(dbar) < 1e-8) {
                dbar = 1.0;
            }

            // calculate average amount of water that passes through cell
            dwx = (rightFlux(x-1, y) - leftFlux(x, y) + rightFlux(x, y) - leftFlux(x+1, y)) * 0.5;
            // TODO: possible bug
            dwy = (topFlux(x, y-1) - bottomFlux(x, y) + topFlux(x, y) - bottomFlux(x, y+1)) * 0.5;

            // std::cout << "\ndwx: " << dwx << ", dwy: " << dwy << std::endl;

            uu = dwx / (dbar * lx);
            vv = dwy / (dbar * ly);

            if (fabs(dbar) < 1e-8) {
                uu = 0.0;
                vv = 0.0;
            }

            // update velocity field
            _u[y * _width + x] = uu;
            _v[y * _width + x] = vv;

#ifdef DEBUG
            std::cout
            << "\nWater Update step:"
            << "\ncell: " << x << " " << y
            << "\ninFlow: " << inFlow
            << "\noutFlow:" << outFlow
            << "\ndv:" << dv
            << "\ndw x: " << dwx
            << "\ndw y: " << dwy
            << "\ndbar: " << dbar
            << "\n";
#endif
        }
    }

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
    
    // exit(0);
#endif        
}

void Simulation::erosionDeposition(const double dt, const Grid &_grid)
{
    for ( int y = 0; y < _height; ++y ) {
        for ( int x = 0; x < _width; ++x ) {

            // local velocity
            double uu = u(x,y);
            double vv = v(x,y);
            
            // simulate erosion
            double nh = _grid.getCellNormal(x, y).z();
            double angle = max(0.01, fabs(1.0 - nh));
            double sq = sqrt(uu*uu + vv*vv);
            double C = Kc * angle * sq * (std::min(water(x,y),0.01)/0.01);

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
                << "\nErosion step:"
                << "\ncell: " << x << ", " << y
                << "\nangle: "<< angle << ", "
                << "\nterrain: " << _terrain[y * _width + x] << ", "
                << "\nsediment: " << st << ", "
                << "\nC: " << C << ", "
                << "\nuu: " << uu << ", "
                << "\nvv: " << vv << ", "
                << "\nsqrt: " << sq
                << std::endl;
#endif
        } // end x for loop
    } // end y for loop
}

void Simulation::sedimentTransport(const double dt)
{
    Simulation::_minW = water(0,0);
    Simulation::_maxW = water(0,0);
    Simulation::_minS = terrain(0,0);
    Simulation::_maxS = terrain(0,0);

    for ( int y = 0; y < _height; ++y ) {
        for ( int x = 0; x < _width; ++x ) {

            // local velocity
            double uV = u(x,y);
            double vV = v(x,y);

            // where the flow comes from
            double sedimentFromX = static_cast<double>(x) - uV * dt;
            double sedimentFromY = static_cast<double>(y) - vV * dt;

            int x0 = floor(sedimentFromX);
            int y0 = floor(sedimentFromY);
            int x1 = x0+1;
            int y1 = y0+1;

            float fX = sedimentFromX - x0;
            float fY = sedimentFromY - y0;

            x0 = glm::clamp(x0,0,int(_width - 1));
            x1 = glm::clamp(x1,0,int(_width - 1));
            y0 = glm::clamp(y0,0,int(_height- 1));
            y1 = glm::clamp(y1,0,int(_height- 1));

            float newVal =
            glm::mix(
                glm::mix(sediment(y0,x0),sediment(y0,x1),fX),
                glm::mix(sediment(y1,x0),sediment(y1,x1),fX),
                fY);

            _s1[y * _width + x] = newVal;

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
        } // end x for loop
    } // end y for loop

    // update sediment
    for ( int y = 0; y < _height; ++y ) {
        for ( int x = 0; x < _width; ++x ) {
            _sediment[y * _width + x] = _s1[y * _width + x];
        }
    }
}

void Simulation::evaporation( double dt ) {
    for ( int y = 0; y < _height; ++y ) {
        for ( int x = 0; x < _width; ++x ) {
            // simulate evaporation
            d2 *= ( 1 - Ke * dt );
            _water[y * _width + x] = std::max(d2, 0.0);
            if (water(x,y) < 1e-2) {
                _water[y * _width + x] = 0.0;
            }
        }
    }
}

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

void Simulation::draw( QPainter& painter, const UnitSquareMapping& mapping, const Grid &grid ) {
    double waterHeight, terrainHeight, sedimentHeight;
    waterHeight = terrainHeight = sedimentHeight = 0.0;

    for ( int y = 0; y < _height; ++y ) {
        for ( int x = 0; x < _width; ++x ) {
            
            terrainHeight = (terrain(x,y) - _minS) / (_maxS - _minS);
            waterHeight = (water(x,y) - _minW) / (_maxW - _minW);
            sedimentHeight = sediment(x,y);

            grid.drawCell( painter, x, y, terrainHeight, waterHeight, sedimentHeight );
        }
    }
}
