#include "UnitSquareMapping.hpp"


#include <iostream>

UnitSquareMapping::UnitSquareMapping()
	: _data(512 * 424)
{ }

UnitSquareMapping::UnitSquareMapping(const Point2d &txt0, const Point2d &txt1, const Point2d &txt2, const Point2d &txt3)
	: _data(512 * 424)
{
    _p0=Point2d(txt0.x()*512,(1-txt0.y())*424);
    _p1=Point2d(txt1.x()*512,(1-txt1.y())*424);
    _p2=Point2d(txt2.x()*512,(1-txt2.y())*424);
    _p3=Point2d(txt3.x()*512,(1-txt3.y())*424);

    const double x0=_p1.x(), y0=_p1.y();
    const double x1=_p2.x(), y1=_p2.y();
    const double x2=_p3.x(), y2=_p3.y();
    const double x3=_p0.x(), y3=_p0.y();

    const double dx1=x1-x2, dx2=x3-x2, dx3=x0-x1+x2-x3;
    const double dy1=y1-y2, dy2=y3-y2, dy3=y0-y1+y2-y3;

    double a,b,c,d,e,f,g,h,i=1;


    if(fabs(dx3)<1e-8 && fabs(dy3)<1e-8)
    {
        a=x1-x0;
        b=x2-x1;
        c=x0;
        d=y1-y0;
        e=y2-y1;
        f=y0;
        g=0;
        h=0;
    }
    else
    {
        g=(dx3*dy2-dx2*dy3)/(dx1*dy2 - dx2*dy1);
        h=(dx1*dy3-dx3*dy1)/(dx1*dy2 - dx2*dy1);
        a=x1-x0+g*x1;
        b=x3-x0+h*x3;
        c=x0;
        d=y1-y0+g*y1;
        e=y3-y0+h*y3;
        f=y0;
    }

    _toParam(0,0)=e*i-f*h;
    _toParam(0,1)=c*h-b*i;
    _toParam(0,2)=b*f-c*e;

    _toParam(1,0)=f*g-d*i;
    _toParam(1,1)=a*i-c*g;
    _toParam(1,2)=c*d-a*f;

    _toParam(2,0)=d*h-e*g;
    _toParam(2,1)=b*g-a*h;
    _toParam(2,2)=a*e-b*d;

    _fromParam=_toParam.inverse();
}

UnitSquareMapping::~UnitSquareMapping()
{
}