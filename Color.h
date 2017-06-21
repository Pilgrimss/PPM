//
// Created by 王颖 on 20/06/2017.
//

#ifndef PPM_COLOR_H
#define PPM_COLOR_H

#include <cmath>
#include "Vec3.h"

class Color {
public:
    //Construction & Destruction
    Color():_r(0),_g(0),_b(0){}
    Color(double r, double g, double b): _r(r), _g(g), _b(b){}
    ~Color(){};

    // Operators
     Color operator + ( const Color& v) const {
         return Color( _r + v._r , _g + v._g , _b + v._b );
     }
     Color operator - ( const Color& v) const {
         return Color( _r - v._r , _g - v._g , _b - v._b );
     }
     Color operator * ( const Color& v) const {
         return Color( _r * v._r , _g * v._g , _b * v._b );
     }
     Color operator * ( const double& k) const {
         return Color( _r * k , _g * k , _b * k );
     }
     Color operator / ( const double& k) const {
         return Color( _r / k , _g / k , _b / k );
     }
     Color& operator += (const Color& v) {
         *this = *this + v;
         return *this;
     }
     Color& operator -= (const Color& v) {
         *this = *this - v;
         return *this;
    }
     Color& operator *= (const Color& v) {
         *this = *this * v;
         return *this;
     }
     Color& operator *= (const double& k) {
         *this = *this * k;
         return *this;
     }
     Color& operator /= (const double& k) {
         *this = *this / k;
         return *this;
     }

    // Parameters

    Color Confine() {
        return Color( min( _r , 1.0 ) , min( _g , 1.0 ) , min( _b , 1.0f ) );
    }

    Color _Exp() const {
        return Color(exp(_r),exp(_g),exp(_b));
    }

    double _Power() const {
        return (_r + _g + _b) /3;
    }

    double _RgbMax() const{
        return max(_r,max(_g,_b));
    }

    double _IsBlack() const{
        return fabs( _r ) < EPS && fabs( _g ) < EPS && fabs( _b ) < EPS;
    }

    // IO stream


    friend istream & operator >> (istream &is, const Color &v)
    {
        is >> v._r  >> v._g >> v._b;
        return is;
    }

    friend ostream & operator << (ostream &os, const Color &v)
    {
        os << "[" << v._r << " " << v._g << " " << v._b << "]";
        return os;
    }
    double _r,_g,_b;
};


#endif //PPM_COLOR_H
