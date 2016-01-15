#ifndef __POINT_2_H__
#define __POINT_2_H__

#include <cassert>
#include <cmath>
#include <ostream>

class Point2d {
public:
  Point2d() 
    : _x( 0 ), _y( 0 ) {}

  explicit Point2d( const double v )
    : _x( v ), _y( v ) {}

  Point2d( const double x , const double y ) 
    : _x( x ), _y( y ) {}

  inline double& x() { return _x; }
  inline const double x() const { return _x; }

  inline double& y() { return _y; }
  inline const double y() const { return _y; }

  inline double& operator [] ( const int i ) {
    assert( i < 2 );
    if ( i == 0 )
      return _x;
    return _y;
  }
  inline const double operator [] ( const int i ) const {
    assert( i < 2 );
    if ( i == 0 )
      return _x;
    return _y;
  }


  inline const bool operator == ( const Point2d& p ) const {
    if ( fabs(_x - p.x())<1e-8 && fabs(_y - p.y())<1e-8 )
      return true;
    return false;
  }


  inline const Point2d operator -() const {
    return Point2d( -_x, -_y );
  }


  inline const Point2d operator + ( const Point2d& p ) const {
    return Point2d( _x + p.x(), _y + p.y() );
  }

  
  inline Point2d& operator += ( const Point2d& p ) {
    _x += p.x(); _y += p.y();
    return *this;
  }

  inline const Point2d operator - ( const Point2d& p ) const {
    return Point2d( _x - p.x(), _y - p.y() );
  }

  inline Point2d& operator -= ( const Point2d& p ) {
    _x -= p.x(); _y -= p.y();
    return *this;
  }


  inline const Point2d operator / ( const double w ) const {
    return Point2d( _x / w, _y / w );
  }


  friend const Point2d operator / ( const double w, const Point2d& p ) {
    return p / w;
  }

  inline Point2d& operator /= ( const double w ) {
    _x /= w; _y /= w;
    return *this;
  }


  inline const Point2d operator * ( const double w ) const {
    return Point2d( _x * w, _y * w );
  }


  friend const Point2d operator * ( const double w, const Point2d& p ) {
    return p * w;
  }


  inline Point2d& operator *= ( const double w ) {
    _x *= w; _y *= w;
    return *this;
  }


  inline const double operator * ( const Point2d& p ) const {
    return ( _x * p.x() + _y * p.y() );
  }


  inline const double norm1() const {
    return fabs(_x) + fabs(_y);
  }


  inline const double norm() const {
    return sqrt( _x * _x + _y * _y );
  }


  inline const double squaredNorm() const {
    return _x * _x + _y * _y;
  }


  inline const Point2d normalized() const {
    return ( *this / norm() );
  }


  inline Point2d& normalize() {
    const double n = norm();
    _x /= n; _y /= n;
    return *this;
  }

  friend std::ostream& operator << ( std::ostream& s, const Point2d& p )  {    
    s  << p.x() << " " << p.y();
    return s;
  }


private:
  double _x;
  double _y;

};

#endif