#ifndef UNIT_SQUARE_MAPPING_HPP_
#define UNIT_SQUARE_MAPPING_HPP_

#include <iostream>
#include <algorithm>
#include <vector>
#include "Point2.hpp"
#include "Matrix3.hpp"
#include "IKinectProcessor.hpp"

#define MINIMUM(a,b,c,d) std::min(a,std::min(b,std::min(c,d)))
#define MAXIMUM(a,b,c,d) std::max(a,std::max(b,std::max(c,d)))


class UnitSquareMapping{
private:
    Point2d _p0, _p1, _p2, _p3;
    Matrix3d _fromParam, _toParam;

    std::vector<UINT16> _data;

    double _minH, _maxH;

public:
    UnitSquareMapping();
    UnitSquareMapping(const Point2d &txt0, const Point2d &txt1, const Point2d &txt2, const Point2d &txt3);
    ~UnitSquareMapping();

    inline void setData(const UINT16 *data, const double minH, const double maxH)
    {
        _minH=minH;
        _maxH=maxH;

		for (int i = 0; i < 512 * 424; ++i)
			_data[i] = data[i];
    }

    inline bool isInside(const int x, const int y)
    {
        const Point2d p=toParameterization(x,y);

        return isInsideParam(p);
    }

    inline bool isInsideParam(const Point2d &p)
    {
        return p.x()>=0 && p.y()>=0 && p.x()<=1 && p.y()<=1;
    }

    inline void boundingBox(Point2d &minP, Point2d &maxP) const
    {
        minP.x()=MINIMUM(_p0.x(), _p1.x(), _p2.x(), _p3.x());
        minP.y()=MINIMUM(_p0.y(), _p1.y(), _p2.y(), _p3.y());

        maxP.x()=MAXIMUM(_p0.x(), _p1.x(), _p2.x(), _p3.x());
        maxP.y()=MAXIMUM(_p0.y(), _p1.y(), _p2.y(), _p3.y());
    }


    inline double getHeight( int x,  int y) const
    {
		if (x >= 512) x = 511;
		if (y >= 424) y = 423;

        assert(x>=0 && x<512);
        assert(y>=0 && y<424);
        //if(!_data) return 0;

        return (_data[x  + y*512]-_minH)/(_maxH-_minH);
    }

    inline double getHeight(const Point2d &p) const
    {
        return getHeight(p.x(),p.y());
    }

    inline double getHeight(const double x, const double y) const
    {
        const int x1=floor(x);
        const int x2=ceil(x);
        const double tx=x-x1;

        const int y1=floor(y);
        const int y2=ceil(y);
        const double ty=y-y1;

        return (1-tx) * ( (1-ty)*getHeight(x1,y1)+ty*getHeight(x1,y2) )+
        tx * ( (1-ty)*getHeight(x2,y1)+ty*getHeight(x2,y2) );
    }

    inline double getHeightFromParam(const Point2d p) const
    {
        return getHeightFromParam(p.x(),p.y());
    }

    inline double getHeightFromParam(const double u, const double v) const
    {
        assert(u>=0 && u<=1);
        assert(v>=0 && v<=1);
        //if(!_data) return 0;

        Point2d p=fromParameterization(u,v);
        return getHeight(p.x(),p.y());
    }

    inline Point2d fromParameterization(const Point2d &p) const
    {
        assert(p.x()>=0 && p.x()<=1);
        assert(p.y()>=0 && p.y()<=1);

        return _fromParam*p;
    }

    inline Point2d toParameterizationDir(const Point2d &dir) const
    {
        const Point3d tmp=_toParam*Point3d(dir);
        return Point2d(tmp.x(),tmp.y());
    }

    inline Point2d fromParameterization(const double u, const double v) const
    {
        return fromParameterization(Point2d(u,v));
    }

    inline Point2d toParameterization(const Point2d &p) const
    {
         return _toParam*p;
    }

    inline Point2d toParameterization(const int x, const int y) const
    {
        return toParameterization(Point2d(x,y));
    }

    inline Point2d paramGrad(const Point2d &p) const
    {
        const double u=p.x();
        const double v=p.y();

        double denom=_fromParam(2,0) * u + _fromParam(2,1) * v + _fromParam(2,2);
        denom*=denom;
        denom=1/denom;

        const double f1u =  (_fromParam(0,0) * _fromParam(2,1) * v + _fromParam(0,0) * _fromParam(2,2) - _fromParam(2,0) * _fromParam(0,1) * v - _fromParam(2,0) * _fromParam(0,2))*denom;
        const double f1v = -(-_fromParam(0,1) * _fromParam(2,0) * u - _fromParam(0,1) * _fromParam(2,2) + _fromParam(2,1) * _fromParam(0,0) * u + _fromParam(2,1) * _fromParam(0,2))*denom;
        const double f2u =  (_fromParam(1,0) * _fromParam(2,1) * v + _fromParam(1,0) * _fromParam(2,2) - _fromParam(2,0) * _fromParam(1,1) * v - _fromParam(2,0) * _fromParam(1,2))*denom;
        const double f2v = -(-_fromParam(1,1) * _fromParam(2,0) * u - _fromParam(1,1) * _fromParam(2,2) + _fromParam(2,1) * _fromParam(1,0) * u + _fromParam(2,1) * _fromParam(1,2))*denom;

        // std::cout<<f1u<<" "<<f1v<<"\n"<<f2u<<" "<<f2v<<std::endl;

        Point2d tmp=grad(fromParameterization(p));

        return Point2d(f1u*tmp.x()+f1v*tmp.y(),f2u*tmp.x()+f2v*tmp.y());
    }

    inline Point2d grad(const Point2d &p) const
    {
        return Point2d(
            (getHeight(p.x()-1,p.y())-getHeight(p.x()+1,p.y()))/2.0,
            (getHeight(p.x(),p.y()-1)-getHeight(p.x(),p.y()+1))/2.0);
    }

};
#endif

