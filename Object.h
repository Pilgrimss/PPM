//
// Created by 王颖 on 20/06/2017.
//
/*
 * sample means?

 */
#ifndef PPM_OBJECT_H
#define PPM_OBJECT_H

#include "Material.h"

class Collision {
public:
    // Construction & Destruction
    Collision():_object(NULL),_crash(false),_conNormal(false){}

    // Get & Set
    Object* _GetObject(){return _object;}
    Object* _SetObject(Object* object){_object = object;}
    double _GetDistance(){return _distance;}
    double _SetDistance(double distance){_distance = distance;}
    bool _GetCrash(){return _crash;}
    bool _SetCrash(bool crash){_crash = crash;}
    bool _GetConNoamal(){return _conNormal;}
    bool _SetConNormal(bool conNormal){_conNormal = conNormal;}
    Vec3d _GetHit(){return _hit;}
    Vec3d _SetHit(Vec3d hit){_hit = hit;}
    Vec3d _GetNormal(){return _normal;}
    Vec3d _SetNormal(Vec3d normal){_normal = normal;}
    Vec3d _GetDirection(){return _direction;}
    Vec3d _SetDirection(Vec3d direction){_direction = direction;}

private:
    Object * _object;
    double _distance;
    bool _crash, _conNormal;//conNormal: consistent with the direction of normal
    Vec3d _hit, _normal, _direction;
};
class Object {
public:

    //Construction & Destruction
    Object():_material(),_next(NULL){}
    Object(const Object& object){
        *this = object;
        _material = object._material;
    }
    ~Object(){delete _material;}

    // Get & Set
    Material* _GetMaterial() { return _material; }
    Material* _SetMaterial(Material* material) { _material = material; }
    Object* _GetNext() { return _next; }
    Object* _SetNext( Object* next ) { _next = next; }

    // Virtual API
    virtual void PreProcess() {}
    virtual Collision _Collide( Vec3d ray_0 , Vec3d ray_V )= 0;
    virtual Color _GetTexture(Vec3d C) = 0;

    // IO stream
    friend istream & operator >> (istream &is, const Object &v)
    {
        is >> *v._material;
        return is;
    }
private:
    int _sample;
    Material* _material;
    Object* _next;
};

class Plane: public Object {
public:
    // Construction & Destruction
    Plane():Object(){}

    // Virtual API
    Collision _Collide( Vec3d origin , Vec3d direction ) {
        Collision collision;
        direction._Normalize();
        _normal._Normalize();
        double cos = direction._Dot(_normal);
        if(fabs(cos) < EPS) return collision;
        double distance = (_normal * _r - origin)._Dot(_normal)/cos;
        if(distance < EPS) return collision;

        collision._SetCrash(true);
        collision._SetDirection(direction);
        collision._SetObject(this);
        collision._SetDistance(distance);
        collision._SetConNormal((cos < 0));
        collision._SetHit(origin + direction * distance);
        collision._SetNormal((cos < 0)? _normal : - _normal);
        return collision;
    }
    Color _GetTexture(Vec3d point){
        double u = point._Dot(_xAxis) / _xAxis._ModuleSquare() + 0.5;
        double v = point._Dot(_yAxis) /_yAxis._ModuleSquare() + 0.5;
        return _GetMaterial()->_GetTexture()->GetSmoothColor(u,v);
    }
    //IO
    friend istream & operator >> (istream &is, const Plane &v)
    {
        return is;
    }
private:
    Vec3d _normal, _xAxis, _yAxis;
    double _r;
};

class Rectangle : public Object {
public:

    Rectangle():Object(){}

    Collision Collide( Vec3d origin , Vec3d direction ) {
        Collision collision;
        direction._Normalize();
        _normal._Normalize();
        double cos = direction._Dot(_normal);
        if(fabs(cos) < EPS) return collision;
        double distance = (_normal * _r - origin)._Dot(_normal)/cos;
        if(distance < EPS) return collision;

        double u = collision._GetHit()._Dot( _xAxis ) / _xAxis._ModuleSquare();
        double v = collision._GetHit()._Dot( _yAxis ) / _yAxis._ModuleSquare();
        collision._SetCrash((0 <= u && u <= 1 && 0 <= v && v <= 1));
        collision._SetDirection(direction);
        collision._SetObject(this);
        collision._SetDistance(distance);
        collision._SetConNormal((cos < 0));
        collision._SetHit(origin + direction * distance);
        collision._SetNormal((cos < 0)? _normal : - _normal);
        return collision;
    }

    Color _GetTexture(Vec3d point){
        double u = point._Dot(_xAxis) / _xAxis._ModuleSquare() ;
        double v = point._Dot(_yAxis) /_yAxis._ModuleSquare();
        return _GetMaterial()->_GetTexture()->GetSmoothColor(u,v);
    }

    // IO
    friend istream & operator >> (istream &is, const Rectangle &v)
    {
        return is;
    }

private:
    Vec3d _normal, _xAxis, _yAxis;
    double _r;
};

class Sphere : public Object {
public:
    Sphere():Object(){
        _xAxis = Vec3d( 0 , 0 , 1 );
        _yAxis = Vec3d( 0 , 1 , 0 );
    }

    Collision Collide( Vec3d origin , Vec3d direction ) {
        Collision collision;
        direction._Normalize();
        Vec3d vec = origin - _center;
        double b = - vec._Dot( direction );
        double det = _r * _r - (vec._ModuleSquare() - b * b);

        if ( det < EPS ) return collision;
        det = sqrt( det );
        double x1 = b - det  , x2 = b + det;
        if ( x2 < EPS ) return collision;
        Vec3d normal = ( collision._GetHit() - origin);
        normal._Normalize();
        collision._SetNormal((x1 > EPS) ? normal : -normal);
        collision._SetConNormal((x1 > EPS));
        collision._SetDistance( x1 > EPS ) ? x1 : x2);

        collision._SetCrash(true);
        collision._SetDirection(direction);
        collision._SetObject(this);
        collision._SetHit(origin + direction * collision._GetDistance();

        return collision;
    }
    Color GetTexture(Vec3d point) {
        Vec3d vec = point - _center;
        vec._Normalize();
        double a = acos( - vec._Dot( _xAxis ) );
        double b = acos( std::min( std::max( vec._Dot( _yAxis ) / sin( a ) , -1.0 ) , 1.0 ) );
        double u = a / PI , v = b / 2 / PI;
        if ( vec._Dot(_yAxis._Cross(_xAxis)) < 0 ) v = 1 - v;
        return _GetMaterial()->_GetTexture()->GetSmoothColor(u,v);
    }

    // IO
    friend istream & operator >> (istream &is, const Sphere &v)
    {
        return is;
    }
private:
    Vec3d _center , _xAxis , _yAxis;
    double _r;
};


#endif //PPM_OBJECT_H
