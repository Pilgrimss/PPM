//
// Created by 王颖 on 21/06/2017.
//

#ifndef PPM_HITMAP_H
#define PPM_HITMAP_H

# include "Vec3.h"
# include "Color.h"
# include "Object.h"
#include "Photon.h"
class Hit{
public:
    Hit():_object(NULL),_pixel(-1),_kdPlane(-1),_r(0),_maxR(0),_photonNum(0),_addNum(0){
        _weight = Color(1,1,1);
    }
    void CalnIrradiance(Photon* photon){
        _addNum += 1;
        _color += photon->_power * _object->_GetMaterial()->_BRDF(-photon->_direction, _normal, -_direction);
    }

    Vec3 _center, _direction, _normal;
    Object* _object;
    int _pixel;
    Color _weight, _color;
    int _kdPlane;
    float _r, _maxR;
    float _photonNum, _addNum;
};
class HitMap {

public:
    // Construction & Destruction
    HitMap(int size):_maxHits(size),_storedHits(0){
        _hits = new Hit[size + 1];
        _minAngle = Vec3d(INF, INF, INF);
        _maxAngle = Vec3d(-INF, -INF, -INF);
    }
    ~HitMap() {
        delete[] _hits;
    }

    // Set & Get
    void SetReduction(double damp) { _damp = damp; }
    int GetStoredHitpoints() { return _storedHits; }
    Hit* GetHitpoints() { return _hits; }


    void _Store(Hit hit) {
        if (_storedHits >= _maxHits) return;
        _hits[++_storedHits] = hit;
        for (int i = 0; i < 3; i++) {
            _minAngle[i] = min(_minAngle[i], hit._center[i]);
            _maxAngle[i] = max(_maxAngle[i], hit._center[i]);
        }
    }
    void _MaintainHits() {
        for (int i = _storedHits; i >= 1; i--) {
            Hit* hit = &_hits[i];
            if (hit->_addNum < EPS) continue;
            double k = (hit->_photonNum + _damp * hit->_addNum) / (hit->_photonNum + hit->_addNum);
            hit->_r *= k;
            hit->_color *= k;
            hit->_photonNum += _damp * hit->_addNum;
            hit->_addNum = 0;
        }
       _MaintainHitsMaxR();
    }
    void _BalanceKd() {
        Hit* hit = new Hit[_storedHits + 1];

        for (int i = 0; i <= _storedHits; i++)
            hit[i] = _hits[i];

        _BalanceHit(hit, 1, 1, _storedHits);
        delete[] hit;

        _MaintainHitsMaxR();
    }
    void _InsertPhoton(Photon photon) {
        _CalPhoton(&photon, 1);
    }

private:
    int _maxHits, _storedHits;
    Hit* _hits;
    Vec3d _minAngle, _maxAngle;
    double _damp;

    void _BalanceHit(Hit* hit, int index, int start, int end) {
        if (start == end) {
            _hits[index] = hit[start];
            return;
        }

        int med = 1;
        while (4 * med <= end - start + 1)
            med <<= 1;
        if (3 * med <= end - start + 1)
            med = med * 2 + start - 1;
        else
            med = end + 1 - med;

        double max =  -INF;
        int axis = -1;
        for (int i = 0; i < 3; i++){
            double temp = _maxAngle[i] - _minAngle[i];
            if(temp > max) {
                axis = i;
                max = temp;
            }
        }

        _MedianSplit(hit, start, end, med, axis);
        _hits[index] = hit[med];
        _hits[index]._kdPlane = axis;

        if (start < med) {
            double split = _maxAngle[axis];
            _maxAngle[axis] = _hits[index]._center[axis];
            _BalanceHit(hit, index * 2, start, med - 1);
            _maxAngle[axis] = split;
        }

        if (end > med) {
            double split = _minAngle[axis];
            _minAngle[axis] = _hits[index]._center[axis];
            _BalanceHit(hit, index * 2 + 1, med + 1, end);
            _minAngle[axis] = split;
        }
    }

    void _MedianSplit(Hit* hit, int start, int end, int med, int axis) {

        int l = start, r = end;
        while (l < r) {
            double key = hit[r]._center[axis];
            int i = l - 1, j = r;
            for (; ; ) {
                while (hit[++i]._center[axis] < key);
                while (hit[--j]._center[axis] > key && j > l);
                if (i >= j) break;
                swap(hit[i], hit[j]);
            }

            swap(hit[i], hit[r]);
            if (i >= med) r = i - 1;
            if (i <= med) l = i + 1;
        }
    }
    void _MaintainHitsMaxR() {
        for (int i = _storedHits; i >= 1; i--) {
            Hit* hit = &_hits[i];
            hit->_maxR = hit->_r;
            if ((i << 1) <= _storedHits && hit->_maxR < _hits[i << 1]._maxR)
                hit->_maxR = _hits[i << 1]._maxR;
            if ((i << 1) + 1 <= _storedHits && hit->_maxR < _hits[(i << 1) + 1]._maxR)
                hit->_maxR = _hits[(i << 1) + 1]._maxR;
        }
    }
    void _CalPhoton(Photon* photon, int index) { //called by index = 1
        if (index > _storedHits) return;
        Hit* hit = &_hits[index];

        if (index * 2 <= _storedHits) {
            double distance = photon->_center[hit->_kdPlane] - hit->_center[hit->_kdPlane];
            if (distance < 0) {
                _CalPhoton(photon, index * 2);
                if (index * 2 + 1 <= _storedHits && distance * distance < _hits[index * 2 + 1]._maxR)
                    _CalPhoton(photon, index * 2 + 1);
            }
            else {
                _CalPhoton(photon, index * 2 + 1);
                if (distance * distance < _hits[index * 2]._maxR)
                   _CalPhoton(photon, index * 2);
            }
        }

        double distanceSquare = hit->_center._DistanceSquare(photon->_center);
        if (distanceSquare <= _hits[index]._r)
            _hits[index].CalnIrradiance(photon);
    }

};


#endif //PPM_HITMAP_H
