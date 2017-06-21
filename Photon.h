//
// Created by 王颖 on 21/06/2017.
//

#ifndef PPM_PHOTON_H
#define PPM_PHOTON_H

#include "Color.h"
#include "Vec3.h"
#include "Object.h"
struct Photon {
    Vector3 _center , _direction;
    Color _power;
    int _KdPlane;
};

class NearestPhotons {
    Nearestphotons(){}
    ~Nearestphotons(){}

    Vector3 _center;
    int _maxPhotons , _foundPhotons;

    bool _heaped;
    double* _distanceSquare;
    Photon** _photons;

};

class Photonmap {
    int max_photons , stored_photons , emit_photons;
    Photon* photons;
    Vector3 box_min , box_max;

    void BalanceSegment( Photon* porg , int index , int st , int en );
    void MedianSplit( Photon* porg , int st , int en , int med , int axis );
    void LocatePhotons( Nearestphotons* np , int index ); //called by index = 1

public:
    Photonmap(int size);
    ~Photonmap();

    int GetStoredPhotons() { return stored_photons; }
    void SetEmitPhotons( int photons ) { emit_photons = photons; }

    Color GetIrradiance( Collider* collider , double max_dist , int n );
    double GetRadius2(Collider* collider, double max_dist, int n);
    void Store( Photon );
    void Balance();
};
#endif //PPM_PHOTON_H
