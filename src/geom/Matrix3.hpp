#ifndef MATRIX3_H__
#define MATRIX3_H__

#include "Point2.hpp"
#include "Point3.hpp"

class Matrix3d {
public:
    Matrix3d()
    {
        for(int r=0; r<3; r++) {
            for(int c=0; c<3; c++) {
                if (r==c) {
                    _v[r][c] = 1.0;
                }
                else {
                    _v[r][c] = 0.0;
                }
            }
        }
    }

    double &operator() (const int i, const int j)
    {
        return _v[i][j];
    }

    const double operator() (int i, int j) const
    {
        return _v[i][j];
    }

    Matrix3d inverse() const
    {
        const Matrix3d &m=*this;
        Matrix3d result;

        const double det=m.determinant();

        result(0, 0) = (m(1, 1)*m(2, 2) - m(1, 2)*m(2, 1))/det;
        result(0, 1) = (m(0, 2)*m(2, 1) - m(0, 1)*m(2, 2))/det;
        result(0, 2) = (m(0, 1)*m(1, 2) - m(0, 2)*m(1, 1))/det;
        result(1, 0) = (m(1, 2)*m(2, 0) - m(1, 0)*m(2, 2))/det;
        result(1, 1) = (m(0, 0)*m(2, 2) - m(0, 2)*m(2, 0))/det;
        result(1, 2) = (m(0, 2)*m(1, 0) - m(0, 0)*m(1, 2))/det;
        result(2, 0) = (m(1, 0)*m(2, 1) - m(1, 1)*m(2, 0))/det;
        result(2, 1) = (m(0, 1)*m(2, 0) - m(0, 0)*m(2, 1))/det;
        result(2, 2) = (m(0, 0)*m(1, 1) - m(0, 1)*m(1, 0))/det;

        return result;
    }

    inline double determinant() const
    {
        const Matrix3d &m=*this;

        return m(0, 0)*m(1, 1)*m(2, 2) + 
                m(0, 1)*m(1, 2)*m(2, 0) + 
                m(0, 2)*m(1, 0)*m(2, 1) - 
                m(0, 0)*m(1, 2)*m(2, 1) - 
                m(0, 1)*m(1, 0)*m(2, 2) - 
                m(0, 2)*m(1, 1)*m(2, 0); 
    }



// const Matrix3d operator * ( const Matrix3d &m ) const {
//     Matrix3d M;

//     for (int r=0; r<3; r++) 
//         M(r,r) = 0;


//     for (int r=0; r<3; r++){
//         for (int c=0; c<3; c++){
//             for (int i=0; i<3; i++) {
//                 M(r,c)    += _v[r][i]*m(i,c);
//             }
//         }
//     }
//     return M;
// }


// const bool operator == ( Matrix3d &m ) const {
//     for (int r=0; r<3; r++){
//         for (int c=0; c<3; c++){
//             if (_v[r][c] != m(r,c))
//                 return false;
//         }
//     }

//     return true;
// }


    Point3d operator * ( const Point3d &p ) const {
        const double x = _v[0][0]*p.x() + _v[0][1]*p.y() + _v[0][2]*p.z();
        const double y = _v[1][0]*p.x() + _v[1][1]*p.y() + _v[1][2]*p.z();
        const double z = _v[2][0]*p.x() + _v[2][1]*p.y() + _v[2][2]*p.z();

        return Point3d( x, y, z );
    }

    Point2d operator * ( const Point2d &p ) const {
        const double x = _v[0][0]*p.x() + _v[0][1]*p.y() + _v[0][2];
        const double y = _v[1][0]*p.x() + _v[1][1]*p.y() + _v[1][2];
        const double z = _v[2][0]*p.x() + _v[2][1]*p.y() + _v[2][2];

        return Point2d( x/z, y/z );
    }

    friend std::ostream &operator << ( std::ostream &s, const Matrix3d &m )  {
        for(int r=0; r<3; r++) {
            for(int c=0; c<3; c++) {
                s << m(r,c) <<"\t";
            }
            s<<"\n";
        }

        return s;
    }

private:
    double _v[3][3];
};


#endif
