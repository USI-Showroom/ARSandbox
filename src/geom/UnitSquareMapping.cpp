#include "UnitSquareMapping.hpp"

UnitSquareMapping::UnitSquareMapping()
: _data(NULL)
{ }

UnitSquareMapping::UnitSquareMapping(const Point2d &txt0, const Point2d &txt1, const Point2d &txt2, const Point2d &txt3)
: _data(NULL)
{
    _p0=Point2d(txt0.x()*512,(1-txt0.y())*424);
    _p1=Point2d(txt1.x()*512,(1-txt1.y())*424);
    _p2=Point2d(txt2.x()*512,(1-txt2.y())*424);
    _p3=Point2d(txt3.x()*512,(1-txt3.y())*424);
}

UnitSquareMapping::~UnitSquareMapping()
{

}