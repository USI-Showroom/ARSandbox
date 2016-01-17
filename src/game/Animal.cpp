#include "Animal.hpp"
#include <time.h> 
#include <stdlib.h>
#include <iostream>

Animal::Animal(const double minH, const double maxH)
:_life(0), _minH(minH), _maxH(maxH)
{ }

void Animal::think(const UnitSquareMapping &mapping)
{
    if(!alive())
    {
        resurrect(mapping);
    }
    else
    {
        newDirection(mapping);
        --_decisionTicks;

        
        
    }
}

void Animal::update()
{
    move();
}

void Animal::move()
{
    if(!alive()) return;

    static const double dt=0.001;

    _position+=dt*_direction;

    if(_position.isNan())
    {
        _life=0;
    }

    if(_position.x()<0.05) {
        --_life;
        _position.x()=0.05;
    }
    if(_position.y()<0.05) {
        --_life;
        _position.y()=0.05;
    }

    if(_position.x()>0.95) {
        --_life;
        _position.x()=0.95;
    }
    if(_position.y()>0.95){
        --_life;
        _position.y()=0.95;  
    } 
}

void Animal::newDirection(const UnitSquareMapping &mapping)
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
        else if(h>_maxH){ //go down
            grad=-grad;
            --_life;
        }
        else //the grad points to go up
        {
            --_life;
        }

        grad.normalize();
        grad+=Point2d(randRange(-0.05,0.05),randRange(-0.05,0.05));
        grad.normalize();

        _direction=grad;
        _decisionTicks=3;
    }
}

double Animal::randRange(const double min, const double max)
{
    return (double(rand())/RAND_MAX)*(max-min)+min;
}

void Animal::resurrect(const UnitSquareMapping &mapping)
{
    static const int nTrials=5;
    for(int i=0;i<nTrials;++i)
    {
        const double u=randRange(0.05,0.95);
        const double v=randRange(0.05,0.95);
        const double h=mapping.getHeightFromParam(u,v);

        if(h>=_minH && h<=_maxH)
        {
            _life=10;
            _position.x()=u;
            _position.y()=v;
            _decisionTicks=0;
            break;
        }
    }
}