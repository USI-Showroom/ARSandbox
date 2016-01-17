#include "Animal.hpp"
#include <time.h> 
#include <stdlib.h>
#include <iostream>

Animal::Animal(const double minH, const double maxH)
:_life(0), _minH(minH), _maxH(maxH)
{
    srand (time(NULL));
}

void Animal::update(const UnitSquareMapping &mapping)
{
    static const double dt=0.002;
    if(!alive())
    {
        resurrect(mapping);
    }
    else
    {
        move(mapping);
        --_decisionTicks;

        _position+=dt*_direction;
        //fixme warning with the borders....
    }
}

void Animal::move(const UnitSquareMapping &mapping)
{
    if(_decisionTicks<=0)
    {
        Point2d grad=mapping.paramGrad(_position);
        const double h=mapping.getHeightFromParam(_position);

        if(h>=_minH && h<=_maxH)
        {
            grad.rotated();

            if(!_direction.isZero() && grad*_direction<0)
                grad=-grad;
        }
        else if(h>_maxH) //go down
            grad=-grad;
        //else //the grad points to go up

        grad.normalize();

        _direction=grad;
        _decisionTicks=3;
    }
}

void Animal::resurrect(const UnitSquareMapping &mapping)
{
    static const int nTrials=5;
    for(int i=0;i<nTrials;++i)
    {
        const double u=(double(rand())/RAND_MAX)*0.9+0.05;
        const double v=(double(rand())/RAND_MAX)*0.9+0.05;
        const double h=mapping.getHeightFromParam(u,v);

        if(h>=_minH && h<=_maxH)
        {
            _life=100;
            _position.x()=u;
            _position.y()=v;
            _decisionTicks=0;
            break;
        }
    }
}