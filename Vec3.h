//
// Created by 王颖 on 20/06/2017.
//

#ifndef PPM_VEC3_H
#define PPM_VEC3_H

#include <iostream>
#include <cmath>
#define INF 1e8
#define EPS 1e-8
#define PI 3.1415926535897932384626
#define ran() (double(rand() * (RAND_MAX + 1) + rand()) / ((RAND_MAX + 1) * (RAND_MAX + 1) - 1))
using namespace std;
template <typename T>
class Vec3 {
public:

    //Constructions & destruction
    Vec3() : _x(T(0)), _y(T(0)), _z(T(0)) {}
    Vec3(T xx) : _x(xx), _y(xx), _z(xx) {}
    Vec3(T xx, T yy, T zz) : _x(xx), _y(yy), _z(zz) {}
    ~Vec3(){}

    //Operators

    const T& operator [] (uint8_t i) const {
        return (&_x)[i];
    }

    T& operator [] (uint8_t i) { return (&_x)[i];
    }

    Vec3<T> operator + (const Vec3<T> &v) const{
        return Vec3<T>(_x + v._x, _y + _y, _z + v._z);
    }

    Vec3<T>& operator += (const Vec3<T> &v) {
        _x += v._x, _y += v._y, _z += v._z;
        return *this;
    }

    Vec3<T> operator - (const Vec3<T> &v) const{
        return Vec3<T>(_x - v._x, _y  - v._y, _z - v._z);
    }

    Vec3<T>& operator -= (const Vec3<T> &v) {
        _x -= v._x, _y -= v._y, _z -= v._z;
        return *this;
    }

    Vec3<T> operator * (const T &f) const {
        return Vec3<T>(_x * f, _y * f, _z * f);
    }

    Vec3<T> operator *= (const T &f) const {
        _x *= f, _y *= f, _z *= f;
        return *this;
    }

    Vec3<T> operator * (const Vec3<T> &v) const {
        return Vec3<T>(_x * v._x, _y * v._y, _z * v._z);
    }

    Vec3<T>& operator *= (const Vec3<T> &v) {
        _x *= v._x, _y *= v._y, _z *= v._z;
        return *this;
    }

    Vec3<T> operator / (const T &f) const {
        return Vec3<T>(_x / f, _y / f, _z / f);
    }

    Vec3<T>& operator /= (const Vec3<T> &v) {
        _x /= v._x, _y /= v._y, _z /= v._z;
        return *this;
    }
    Vec3<T> operator - () const {
        return Vec3<T>(-_x, -_y, -_z);
    }

    T _Dot(const Vec3<T> &v) const{
        return _x * v._x + _y * v._y + _z * v._z;
    }

    Vec3<T> _Cross(const Vec3<T> &v) const {
        return Vec3<T>(_y * v._z - _z * v._y , _z * v._x - _x * v._z , _x * v._y - _y * v._x );
    }

    // Module & Distance

    T _ModuleSquare() const {
        return _x * _x + _y * _y + _z * _z;
    }

    T _Module() const {
        return sqrt(_ModuleSquare());
    }

    T _DistanceSquare(const Vec3<T> &v) const {
        return (v - *this)._ModuleSquare();
    }

    T _Distance(const Vec3<T> &v) const {
        return (v - *this)._Module();
    }

    //Normalize
    Vec3<T> _Normalize()
    {
        double module = _Module();
        if (module < EPS) return Vec3<T>(0, 0, 1);
        *this /= module;
        return *this;
    }

    Vec3<T> _UnitVec() const{
        double module = _Module();
        if (module < EPS) return Vec3<T>(0, 0, 1);
        return *this / module;
    }

    Vec3<T> _RandomVec(){
        do {
            _x = 2 * ran() - 1;
            _y = 2 * ran() - 1;
            _z = 2 * ran() - 1;
        } while ( _ModuleSquare() > 1 || _ModuleSquare() < EPS );
        _Normalize();
    }

    bool IsZeroVec() {
        return fabs( _x ) < EPS && fabs( _y ) < EPS && fabs( _z ) < EPS;
    }

    // Get a  Vertical Vector

    Vec3<T> _VertialVec() const {
        Vec3 vertical = _Cross(Vec3( 0 , 0 , 1 ));
        if (vertical.IsZeroVec())
            vertical = Vec3( 1 , 0 , 0 );
        else vertical._Normalize();
        return vertical;
    }

    // Rotate

    Vec3<T> _Rotate( Vec3<T> axis , T theta ) {
        Vec3<T> rotate;
        T cost = cos( theta );
        T sint = sin( theta );
        rotate._x += _x * ( axis._x * axis._x + ( 1 - axis._x * axis._x ) * cost );
        rotate._x += _y * ( axis._x * axis._y * ( 1 - cost ) - axis._z * sint );
        rotate._x += _z * ( axis._x * axis._z * ( 1 - cost ) + axis._y * sint );
        rotate._y += _x * ( axis._y * axis._x * ( 1 - cost ) + axis._z * sint );
        rotate._y += _y * ( axis._y * axis._y + ( 1 - axis._y * axis._y ) * cost );
        rotate._y += _z * ( axis._y * axis._z * ( 1 - cost ) - axis._x * sint );
        rotate._z += _x * ( axis._z * axis._x * ( 1 - cost ) - axis._y * sint );
        rotate._z += _y * ( axis._z * axis._y * ( 1 - cost ) + axis._x * sint );
        rotate._z += _z * ( axis._z * axis._z + ( 1 - axis._z * axis._z ) * cost );
        return rotate;
    }

    // Optics Properties

    Vec3<T>& Reflect(const Vec3<T>& normal) const{
        Vec3 ray = _UnitVec();
        Vec3<T> refl = ray - normal * 2 * ray._Dot(normal);
        return refl;
    }

    Vec3<T>& Refract(const Vec3<T>& normal, T& ior) const{// ior: index of refraction
        Vec3<T> ray = _UnitVec(), refr = ray * ior;
        T cosI = - normal._Dot(ray), cosTSquare = 1 - ( ior * ior ) * ( 1 - cosI * cosI );
        if ( cosTSquare > EPS )
            refr += normal * ( ior * cosI - (T)sqrt(cosTSquare));
        else
            refr = ray.Reflect(normal);
        return refr;
    }

    Vec3<T> Diffuse() {
        Vec3<T> vec = _VertialVec();
        double height = acos(sqrt(ran()));
        double angle = ran() * 2 * PI;
        return _Rotate(vec,height)._Rotate(*this,angle);
    }

    // IO stream
    friend istream & operator >> (istream &is, const Vec3<T> &v)
    {
        is >> v._x  >> v._y >> v._z;
        return is;
    }
    friend ostream & operator << (ostream &os, const Vec3<T> &v)
    {
        os << "[" << v._x << " " << v._y << " " << v._z << "]";
        return os;
    }

private:
    T _x, _y, _z;
};



typedef Vec3<double> Vec3d;


#endif //PPM_VEC3_H
