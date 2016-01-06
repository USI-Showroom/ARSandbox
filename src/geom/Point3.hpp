#ifndef POINT3_H
#define POINT3_H


class Point3d {

public:
  Point3d() 
  : _x( 0.0 ), _y( 0.0 ), _z( 0.0 ) 
  {}

  explicit Point3d( double v )
  : _x( v ), _y( v ), _z( v ) 
  {}

  Point3d( double x, double y, double z ) 
  : _x( x ), _y( y ), _z( z )
  {}

  inline double& x() { return _x; }
  inline double x() const { return _x; }
  
  inline double& y() { return _y; }
  inline double y() const { return _y; }

  inline double& z() { return _z; }
  inline double z() const { return _z; }

  inline void setCoords( const double x, const double y, const double z ) {
    _x = x; _y = y; _z = z;
  }

  
  inline double& operator [] ( const int i ) {
    assert( i < 3 );
    if ( i == 0 )
      return _x;
    if ( i == 1 )
      return _y;

    return _z;
  }

  inline double operator [] ( const int i ) const {
    assert( i < 3 );
    if ( i == 0 )
      return _x;
    if ( i == 1 )
      return _y;
    
    return _z;
  }

  inline bool operator == ( const Point3d& p ) const {
    if ( _x == p.x() && _y == p.y() && _z == p.z() )
      return true;
    return false;
  }

  inline const Point3d operator - () const {
    return Point3d( -_x, -_y, -_z );
  }

  inline const Point3d operator + ( const Point3d& p ) const {
    return Point3d( _x + p.x(), _y + p.y(), _z + p.z() );
  }

  inline void operator += ( const Point3d& p ) {
    _x += p.x(); _y += p.y(); _z += p.z();
  }

  inline const Point3d operator - ( const Point3d& p ) const {
    return Point3d( _x - p.x(), _y - p.y(), _z - p.z() );
  }

  inline void operator -= ( const Point3d& p ) {
    _x -= p.x(); _y -= p.y(); _z -= p.z();
  }

  inline const Point3d operator / ( const double w ) const {
    return Point3d( _x / w, _y / w, _z / w );
  }

  friend const Point3d operator / ( const double w, const Point3d& p ) {
    return p / w;
  }

  inline void operator /= ( const double w ) {
    _x /= w; _y /= w; _z /= w;
  }

  inline const Point3d operator * ( const double w ) const {
    return Point3d( _x * w, _y * w, _z * w );
  }

  friend const Point3d operator * ( const double w, const Point3d& p ) {
    return p * w;
  }

  inline void operator *= ( const double w ) {
    _x *= w; _y *= w; _z *= w;
  }

  inline const Point3d operator ^ ( const Point3d& p ) const {
    return Point3d( ( _y * p.z() ) - ( p.y() * _z ),
      ( _z * p.x() ) - ( p.z() * _x ), ( _x * p.y() ) - ( p.x() * _y ) );
  }

  inline double operator * ( const Point3d& p ) const {
    return ( _x * p.x() + _y * p.y() + _z * p.z() );
  }

  inline double norm() const {
    return sqrt( _x * _x + _y * _y + _z * _z );
  }

  inline double squaredNorm() const {
    return _x * _x + _y * _y + _z * _z;
  }

  inline const Point3d normalized() const {
    return ( *this / norm() );
  }

  inline void normalize() {
    const double n = norm();
    _x /= n; _y /= n; _z /= n;
  }

  inline double getAngle( const Point3d& p ) const {
    return ( atan2( ( *this ^ p ).norm(), ( *this * p ) ) );
  }

  friend std::ostream& operator << ( std::ostream& s, const Point3d& p )  {    
    s  << p.x() << "," << p.y()<< "," <<p.z() << std::endl;
    return s;
  }

private:
  double _x;
  double _y;
  double _z;

};

#endif
