//
// Created by 王颖 on 20/06/2017.
//

#ifndef PPM_TEXTURE_H
#define PPM_TEXTURE_H

#include "Color.h"
#include "Image.h"
class Material{
public:
    Material():_color(),_absorb(), _refl(0),_refr(0),_diff(0),_spec(0),_ior(0),_monteAngle(0),_texture(NULL),_bumpMap(NULL){};
    //Set & Get
    Image* _GetTexture(){return _texture;}
    Image* _SetTexture(Image* texture){_texture = texture;}
    Image* _SetBumpMap(Image* bumpMap){_bumpMap = bumpMap;}
    Color _GetColor(){return _color;}
    Color _SetColor(Color color){_color = color;}
    Color _GetAbsorb(){return _absorb;}
    Color _SetAbsorb(Color absorb){_absorb = absorb;}
    double _SetDiff(double diff){_diff = diff;}
    double _SetRefl(double refl){_refl = refl;}
    double _SetRefr(double refr){_refr = refr;}
    double _SetSpec(double spec){_spec = spec;}
    double _SetIor(double ior){_ior = ior;}

    double _BRDF(Vec3d ray, Vec3d normal, Vec3d view) {
        double brdf = 0;
        ray._Normalize();
        view._Normalize();

        if (_diff > EPS && ray._Dot(normal) > EPS)
            brdf += _diff * ray._Dot(normal);
        if (_spec > EPS && ray._Dot(-view.Reflect(normal)) > EPS)
            brdf += _spec * pow(ray._Dot(-view.Reflect(normal)), 50);
        return brdf;
    }

    //IO stream
    friend istream & operator >> (istream &is, const Material &v)
    {
        return is;
    }
private:
    Color _color, _absorb;
    double _refl, _refr, _diff, _spec;// radio of reflection, refraction, diffuse, specular
    double _ior;// index of refraction
    double _monteAngle;
    Image* _texture;
    Image* _bumpMap;
};
#endif //PPM_TEXTURE_H
