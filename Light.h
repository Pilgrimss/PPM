//
// Created by 王颖 on 21/06/2017.
//

#ifndef PPM_LIGHT_H
#define PPM_LIGHT_H

#include "Vec3.h"
#include "Color.h"
#include "Object.h"

class Light{
public:
    //Construction
    Light():_sample(rand()){}

    // Get & Set
    int _GetSample() { return _sample; }
    int _SetSample(int sample) {_sample = sample;}
    Color _GetColor() { return _color; }
    Color _SetColor(Color color) {_color = color;}
    Light* _GetNext() { return _next; }
    void _SetNext( Light* next ) { _next = next; }

    // Virtual API
    virtual Vec3d _GetCenter() = 0;
    virtual LightCollision _Collide( Vec3d origin , Vec3d direction ) = 0;
    virtual Color _GetIrradiance( Collision* collidion , Object* head , int shade, int* hash ) = 0;
    virtual Photon _EmitPhoton() = 0;

    // IO
    friend istream & operator >> (istream &is, const Light &v)
    {
        is >> v._color;
        return is;
    }
protected:
    int _sample;
    Color _color;
    Light* _next;
    Color _CalnIrradiance( Collision* collision , Vec3d ray , int* hash ) {
        Object* object =  collision->_GetObject();
        Color color = _color * object->_GetMaterial()->_BRDF(ray, collision->_GetNormal(),-collision->_GetDirection());
        color /= ray._ModuleSquare();

        if(! color._IsBlack() && hash != NULL)
            *hash = (*hash + _sample) % HASH_MOD;

        return color;
    }
};


class LightCollision {

public:
    LightCollision():_light(NULL),_crash(false) {}
    Light* _GetLight() { return _light; }
    Light* _SetLight(Light* light) { _light = light; }
    bool _GetCrash() {return _crash;}
    bool _SetCrash(bool crash) {_crash = crash;}
    double _GetDistance() {return _distance;}
    double _SetDistance(double distance) { _distance = distance;}
private:
    Light* _light;
    bool _crash;
    double _distance;
};

class AreaLight : public Light {

public:
    AreaLight() : Light() {}
    ~AreaLight() {}


    // virtual API

    Vec3d _GetCenter() { return _center; }

    LightCollision _Collide( Vec3d origin , Vec3d direction ) {
        LightCollision lightCollision;
        lightCollision._SetLight(this);

        direction._Normalize();
        Vec3d normal = (_xAxis._Cross(_yAxis));
        normal._Normalize();
        double cos = normal._Dot( direction);
        if ( fabs( cos ) < EPS ) return lightCollision;
        double distance = ( normal * _center._Dot( normal ) - origin )._Dot( normal ) / cos;
        if ( distance < EPS ) return lightCollision;

        Vec3d vec = ( origin + direction * distance ) - _center;
        if ( fabs( _xAxis._Dot( vec ) ) > _xAxis._ModuleSquare() ) return lightCollision;
        if ( fabs( _yAxis._Dot( vec ) ) > _yAxis._ModuleSquare() ) return lightCollision;

        lightCollision._SetCrash(true);
        lightCollision._SetDistance(distance);
        return lightCollision;
    }

    Color _GetIrradiance( Collision* collision , Object* head , int shade, int* hash ) {
        Object* object = collision->_GetObject();
        Color color;

        for ( int i = -2 ; i < 2 ; i++ )
            for ( int j = -2 ; j < 2 ; j++ )
                for ( int k = 0 ; k < shade ; k++ ) {
                    Vec3d vec = (_center + _xAxis * ( ( ran() + i ) / 2 ) + _yAxis * ( ( ran() + j ) / 2 )) - collision->_GetHit();
                    double distance = vec._ModuleSquare();

                    bool isShade = false;
                    for ( Object* temp = head ; temp != NULL ; temp = temp->_GetNext() ) {
                        Collision tempCollision = temp->_Collide(collision->_GetHit(), vec);
                        if ( tempCollision._GetCrash() && tempCollision._GetDistance() < distance ) {
                            isShade = true;
                            break;
                        }
                    }

                    if (!isShade) color += _CalnIrradiance(collision , vec , NULL );
                }

        color /= (16.0 * shade);
        return color;
    }

    Photon _EmitPhoton() {
        Photon photon;
        photon.power = _color / _color._Power();
        photon.pos = _center + _xAxis * ( ran() * 2 - 1 ) + _yAxis * ( ran() * 2 - 1 );
        photon.dir.AssRandomVector();
        return  photon;
    }

    // IO
    friend istream & operator >> (istream &is, const AreaLight &v)
    {
        is >> v._center >> v._xAxis >> v._yAxis;
        return is;
    }
private:
    Vec3d _center , _xAxis , _yAxis;
};


class PointLight : public Light {
    Vec3d _center;
public:
    PointLight() : Light() {}

    Vec3d _GetCenter() { return _center; }

    LightCollision _Collide( Vec3d origin , Vec3d direction ) {
        LightCollision lightCollision;
        lightCollision._SetLight(this);
        return lightCollision;
    }

    Color _GetIrradiance( Collision* collision , Object* head , int shade , int* hash ) {
        Object* object = collision->_GetObject();
        Vec3d vec = _center - collision->_GetHit();
        double distance = vec._Module();

        for (Object* temp = head ; temp != NULL ; temp = temp->_GetNext()) {
            Collision tempCollision = temp->_Collide(collision->_GetHit(), vec);
            if ( tempCollision._GetCrash() && tempCollision._GetDistance() < distance ) return Color();
        }

        return _CalnIrradiance( collision , vec , hash );
    }

    Photon EmitPhoton() {
        Photon photon;
        photon._power = _color / _color._Power();
        photon._pos = _center;
        photon._direction.AssRandomVector();
        return photon;
    }

    // IO
    friend istream & operator >> (istream &is, const PonitLight &v)
    {
        is >> v._color;
        return is;
    }
};
#endif //PPM_LIGHT_H
