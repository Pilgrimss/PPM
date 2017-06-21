//
// Created by 王颖 on 21/06/2017.
//

#ifndef PPM_OBJ_H
#define PPM_OBJ_H
# include <fstream>
# include <sstream>
#include <iostream>
# include <string>
# include <cstdlib>
# include <map>
# include "Object.h"
using namespace std;
class Obj {
public:
    Obj():_polyhedron(NULL),_vSize(0), _tSize(0), _nSize(0),_dSize(0), _mSize(0){}
    ~Obj() {}
    Polyhedron* _SetPolyhedron(Polyhedron* polyhedron) { _polyhedron = polyhedron; }
    void _ReadObj(string file){
        _ReadObjSize(file);
        ifstream fin(file.c_str());
        string order;

        int matID = -1;
        int vCnt = 0, vtCnt = 0, vnCnt = 0, fCnt = 0;
        while (getline(fin, order, '\n')) {
            stringstream fin2(order);
            string var;
            if (!(fin2 >> var)) continue;

            if (var == "mtllib") {
                string mtlFile;
                fin2 >> mtlFile;
                _ReadMtl(mtlFile);
            }
            if (var == "usemtl") {
                string matName;
                fin2 >> matName;
                matID = _materialMap[matName];
            }
            if (var == "v") {
                vCnt++;
               fin2 >> _vertex[vCnt];
            }
            if (var == "vt") {
                vtCnt++;
                fin2 >> _texture[vtCnt].second >> _texture[vtCnt].first;
            }
            if (var == "vn") {
                vnCnt++;
                fin2 >> _normal[vnCnt];
            }
            if (var == "f") {
                Triangle* delta = _delta[fCnt] = new Triangle;
                delta->SetParent(polyhedron);
                //tri->SetSample(polyhedron->GetSample());
                if (matID != -1)
                    delta->SetMaterial(_material[matID]);
                else
                    delta->SetMaterial(polyhedron->GetMaterial());
                string str;
                for (int i = 0; fin2 >> str; i++) {
                    int bufferLen = 0, buffer[3];
                    buffer[0] = buffer[1] = buffer[2] = -1;
                    for (int s = 0, t = 0; t < (int)str.length(); t++)
                        if (t + 1 >= (int)str.length() || str[t + 1] == '/') {
                            buffer[bufferLen++] = atoi(str.substr(s, t - s + 1).c_str());
                            s = t + 2;
                        }
                    int vertexID = i;
                    if (i >= 3) {
                        vertexID = 2;
                        delta = _delta[fCnt] = new Triangle;
                        delta = _delta[fCnt - 1];
                        delta->GetVertex(1) = delta->GetVertex(2);
                        delta->GetPos(1) = delta->GetPos(2);
                        delta->GetTextureVertex(1) = delta->GetTextureVertex(2);
                        delta->GetNormalVectorID(1) = delta->GetNormalVectorID(2);
                    }
                    if (buffer[0] > 0) {
                        delta->GetVertex(vertexID) = buffer[0];
                        Vec3d vertexPos = _vertex[buffer[0]];
                        vertexPos = vertexPos._Rotate(Vec3d(1, 0, 0), polyhedron->GetAngles().GetCoord(0));
                        vertexPos = vertexPos._Rotate(Vec3d(0, 1, 0), polyhedron->GetAngles().GetCoord(1));
                        vertexPos = vertexPos._Rotate(Vec3d(0, 0, 1), polyhedron->GetAngles().GetCoord(2));
                        vertexPos = polyhedron->GetO() + vertexPos * polyhedron->GetSize();
                        delta->GetPos(vertexID) = vertexPos;
                    }
                    if (buffer[1] > 0) {
                        delta->GetTextureVertex(vertexID) = buffer[1];
                    }
                    if (buffer[2] > 0) {
                        delta->GetNormalVectorID(vertexID) = buffer[2];
                    }
                    if (i >= 2) {
                        delta->PreTreatment();
                        fCnt++;
                    }
                }
            }
        }
        fin.close();

        _CalNormal();

        TriangleNode* root = polyhedron->GetTree()->GetRoot();
        root->size = fCnt;
        root->tris = new Triangle*[root->size];
        for (int i = 0; i < root->size; i++) {
            root->tris[i] = _delta[i];
            root->box.Update(_delta[i]);
        }
        polyhedron->GetTree()->BuildTree();

        polyhedron->SetVertexN(_normal);
        polyhedron->SetPixel(_texture);
        delete[] _vertex;
        delete[] _delta;
        delete[] _material;
    }

private:
    Polyhedron* _polyhedron;
    int _vSize, _tSize, _nSize, _dSize, _mSize; // size of vertexes, texture vertexes, normals, deltas, materials
    Vec3d* _vertex;
    pair<double, double>* _texture;
    Vec3d* _normal;
    Triangle** _delta;
    Material** _material;
    map<string, int> _materialMap;

    void _ReadMtlSize(string file) {
        ifstream fin(file.c_str());
        string order;

        while (getline(fin, order, '\n')) {
            stringstream fin2(order);
            string var;
            if (!(fin2 >> var)) continue;
            if (var == "newmtl")
                _mSize++;
        }
        fin.close();

        _material = new Material*[_mSize + 1];
    }
    void _ReadObjSize(string file) {
        ifstream fin(file.c_str());
        string order;

        while (getline(fin, order, '\n')) {
            stringstream fin2(order);
            string var;
            if (!(fin2 >> var)) continue;
            if (var == "mtllib") {
                string mtlFile;
                fin2 >> mtlFile;
                _ReadMtlSize(mtlFile);
            }
            if (var == "v")
                _vSize++;
            if (var == "vt")
                _tSize++;
            if (var == "_normal")
                _nSize++;
            if (var == "f") {
                int vertexCnt = 0;
                string var;
                while (fin2 >> var)
                    vertexCnt++;
                _dSize += max(0, vertexCnt - 2);
            }
        }
        fin.close();

        _vertex = new Vec3d[_vSize + 1];
        _texture = new pair<double, double>[_tSize + 1];
        if (_nSize == 0)
            _normal = new Vec3d[_vSize + 1];
        else
            _normal = new Vec3d[_nSize + 1];
        _delta = new Triangle*[_dSize];
    }

    void _ReadMtl(string file) {
        ifstream fin(file.c_str());
        string order;

        int _materialCnt = 0;
        while (getline(fin, order, '\n')) {
            stringstream fin2(order);
            string var;
            if (!(fin2 >> var)) continue;

            if (var == "newmtl") {
                string _materialName;
                fin2 >> _materialName;
                _materialMap[_materialName] = ++_materialCnt;
                _material[_materialCnt] = new Material;
            }
            if (var == "Ka") {

            }
            if (var == "Kd") {
                Color color;
                fin >> color;
                double diff = _material[_materialCnt]->_GetColor()._RgbMax();
                _material[_materialCnt]->_SetDiff(diff);
                _material[_materialCnt]->_SetColor(color/diff);
            }
            if (var == "Ks") {
                double refl;
                fin2 >> refl;
                _material[_materialCnt]->_SetRefl(refl);
            }
            if (var == "Tf") {
                Color absorb;
                fin >> absorb;
                _material[_materialCnt]->_SetAbsorb(absorb);
                if (absorb._Power() < 1 - EPS) {
                    _material[_materialCnt]->_SetRefr(1);
                    _material[_materialCnt]->_SetDiff(0);
                    _material[_materialCnt]->_SetSpec(0);
                    _material[_materialCnt]->_SetRefl(0);
                }
            }
            if (var == "Ni") {
                double ior;
                fin2 >> ior;
                _material[_materialCnt]->_SetIor(ior);
            }
            if (var == "map_Kd") {
                Image *texture;
                string bmpFile;
                fin2 >> bmpFile;
                texture->_Input(bmpFile);
                _material[_materialCnt]->_SetTexture(texture);
            }
            if (var == "map_bump") {
                Image* bumpMap;
                string bmpFile;
                fin2 >> bmpFile;
                bumpMap->_Input(bmpFile);
                _material[_materialCnt]->_SetBumpMap(bumpMap);
            }
        }
        fin.close();
    }
    void _CalNormal() {
        if (_nSize > 0) {
            for (int i = 1; i <= _nSize; i++) {
                _normal[i] = _normal[i]._Rotate(Vec3d(1, 0, 0), polyhedron->GetAngles().GetCoord(0));
                _normal[i] = _normal[i]._Rotate(Vec3d(0, 1, 0), polyhedron->GetAngles().GetCoord(1));
                _normal[i] = _normal[i]._Rotate(Vec3d(0, 0, 1), polyhedron->GetAngles().GetCoord(2));
            }
        }
    }
};
#endif //PPM_OBJ_H
