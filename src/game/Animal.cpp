#include "Animal.hpp"
#include <time.h> 
#include <stdlib.h>
#include <iostream>

// Animal::Animal()
// :_life(0), _minH(0), _maxH(0), _resurrected(false), _sound()
// { }

Animal::Animal(const double minH, const double maxH)
:_life(0), _minH(minH), _maxH(maxH), _resurrected(false)//, _sound()
{ }

void Animal::think(const UnitSquareMapping &mapping)
{
	if (!alive())
		resurrect(mapping);


	if (alive())
	{
		newDirection(mapping);
		--_decisionTicks;




		if (_sound.state()==QMediaPlayer::StoppedState)
		{
			--_soundTicks;
			if (_soundTicks <= 0){
				const double r = randRange(0, 5);

				_soundTicks = 30;
				if (r > 4.5){
					_sound.play();
				}
			}
		}
	}
}

Animal::~Animal()
{
    _sound.stop();
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


        if(grad.isZero()) 
        {
            --_life;
            return;
        }

        if(h>=_minH && h<=_maxH)
        {
            grad.rotated();

            if(_life<_maxLife)
                ++_life;

            if(grad*_direction<0)
                grad=-grad;
        }
        else
        { 
            --_life;
            if(h<_minH) //go up
                grad=-grad;

            _direction.normalize();
            grad.normalize();
            grad=_direction+0.2*grad;
        }

        const double newAngle=atan2(grad.y(),grad.x());
        if(_resurrected)
        {
            _angle=newAngle;
            _resurrected=false;
        }
        else
            _angle=0.3*newAngle+0.7*_angle;

        _direction=Point2d(cos(_angle),sin(_angle))*_speed;
        
        _decisionTicks=1;
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
            QString s=QString::number(round(randRange(1,6)));
            _sound.setMedia(QUrl("qrc:/sounds/cow"+s));
            _sound.setVolume(randRange(20,60));
            _soundTicks=0;

            _maxLife=randRange(15,30);
            _life=_maxLife;
            _position.x()=u;
            _position.y()=v;
            _decisionTicks=0;
            _speed=randRange(1,3);
            _resurrected=true;
            break;
        }
    }
}