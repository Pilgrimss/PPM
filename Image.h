//
// Created by 王颖 on 20/06/2017.
//

#ifndef PPM_IMAGE_H
#define PPM_IMAGE_H

#include "Color.h"
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

struct BITMAPFILEHEADER {
    dword bfSize;
    word bfReserved1;
    word bfReserved2;
    dword bfOffBits;
};

struct BITMAPINFOHEADER {
    dword biSize;
    long biWidth;
    long biHeight;
    word biPlanes;
    word biBitCount;
    dword biCompression;
    dword biSizeImage;
    long biXPelsPerMeter;
    long biYPelsPerMeter;
    dword biClrUsed;
    dword biClrImportant;
};

struct RGBQUAD {
    byte rgbBlue;
    byte rgbGreen;
    byte rgbRed;
    byte rgbReserved;
};

struct IMAGEDATA {
    byte red;
    byte green;
    byte blue;
    Color GetColor() {
        return Color( red , green , blue ) / 255;
    }
};

class Image {
public:
    //Construction & Destruction
    Image(int H = 0 , int W = 0 ) {
        _Init(H,W);
    }
    ~Image() {
        for ( int i = 0 ; i < _strInfo.biHeight ; i++ )
            delete[] _image[i];

        delete[] _image;
    }
    // Get & Set
    Color GetColor( int i , int j ) {
        return Color( _image[i][j].red , _image[i][j].green , _image[i][j].blue ) / 256;
    }

    void SetColor( int i , int j , Color col) {
        _image[i][j].red = ( byte ) ( col._r * 255 );
        _image[i][j].green = ( byte ) ( col._g * 255 );
        _image[i][j].blue = ( byte ) ( col._b * 255 );
    }

    Color GetSmoothColor( double u , double v ) {
        double U = ( u + EPS - floor( u + EPS ) ) * _strInfo.biHeight;
        double V = ( v + EPS - floor( v + EPS ) ) * _strInfo.biWidth;
        int U1 = ( int ) floor( U + EPS ) , U2 = U1 + 1;
        int V1 = ( int ) floor( V + EPS ) , V2 = V1 + 1;
        double rat_U = U2 - U;
        double rat_V = V2 - V;
        if ( U1 < 0 ) U1 = (int)_strInfo.biHeight - 1; if ( U2 == _strInfo.biHeight ) U2 = 0;
        if ( V1 < 0 ) V1 = (int)_strInfo.biWidth - 1; if ( V2 == _strInfo.biWidth ) V2 = 0;
        Color ret;
        ret = ret + _image[U1][V1].GetColor() * rat_U * rat_V;
        ret = ret + _image[U1][V2].GetColor() * rat_U * ( 1 - rat_V );
        ret = ret + _image[U2][V1].GetColor() * ( 1 - rat_U ) * rat_V;
        ret = ret + _image[U2][V2].GetColor() * ( 1 - rat_U ) * ( 1 - rat_V );
        return ret;
    }
    // IO
    void _Input( std::string file ) {
        _Release();

        FILE *fpi = fopen( file.c_str() , "rb" );
        word bfType;
        fread( &bfType , 1 , sizeof( word ) , fpi );
        fread( &_strHead , 1 , sizeof( BITMAPFILEHEADER ) , fpi );
        fread( &_strInfo , 1 , sizeof( BITMAPINFOHEADER ) , fpi );

        RGBQUAD Pla;
        for ( int i = 0 ; i < ( int ) _strInfo.biClrUsed ; i++ ) {
            fread( ( char * ) & ( Pla.rgbBlue ) , 1 , sizeof( byte ) , fpi );
            fread( ( char * ) & ( Pla.rgbGreen ) , 1 , sizeof( byte ) , fpi );
            fread( ( char * ) & ( Pla.rgbRed ) , 1 , sizeof( byte ) , fpi );
        }

        _Init( _strInfo.biHeight , _strInfo.biWidth );

        int pitch = _strInfo.biWidth % 4;
        for(int i = 0 ; i < _strInfo.biHeight ; i++ ) {
            for(int j = 0 ; j < _strInfo.biWidth ; j++ ) {
                fread( &_image[i][j].blue , 1 , sizeof( byte ) , fpi );
                fread( &_image[i][j].green , 1 , sizeof( byte ) , fpi );
                fread( &_image[i][j].red , 1 , sizeof( byte ) , fpi );
            }
            byte buffer = 0;
            for (int j = 0; j < pitch; j++)
                fread( &buffer , 1 , sizeof( byte ) , fpi );
        }

        fclose( fpi );
    }

    void _Output( std::string file ) {
        FILE *fpw = fopen( file.c_str() , "wb" );

        word bfType = 0x4d42;
        fwrite( &bfType , 1 , sizeof( word ) , fpw );
        fwrite( &_strHead , 1 , sizeof( BITMAPFILEHEADER ) , fpw );
        fwrite( &_strInfo , 1 , sizeof( BITMAPINFOHEADER ) , fpw );

        int pitch = _strInfo.biWidth % 4;
        for ( int i = 0 ; i < _strInfo.biHeight ; i++ ) {
            for ( int j = 0 ; j < _strInfo.biWidth ; j++ ) {
                fwrite( &_image[i][j].blue , 1 , sizeof( byte ) , fpw );
                fwrite( &_image[i][j].green , 1 , sizeof( byte ) , fpw );
                fwrite( &_image[i][j].red , 1 , sizeof( byte ) , fpw );
            }
            byte buffer = 0;
            for (int j = 0; j < pitch; j++)
                fwrite( &buffer , 1 , sizeof( byte ) , fpw );
        }

        fclose( fpw );
    }



private:
    BITMAPFILEHEADER _strHead;
    BITMAPINFOHEADER _strInfo;
    bool _imageCreated;
    IMAGEDATA** _image;

    void _Release() {
        for ( int i = 0 ; i < _strInfo.biHeight ; i++ )
            delete[] _image[i];

        delete[] _image;
    }
    void _Init(int H, int W){
        _strHead.bfReserved1 = 0;
        _strHead.bfReserved2 = 0;
        _strHead.bfOffBits = 54;

        _strInfo.biSize = 40;
        _strInfo.biPlanes = 1;
        _strInfo.biHeight = H;
        _strInfo.biWidth = W;
        _strInfo.biBitCount = 24;
        _strInfo.biCompression = 0;
        _strInfo.biSizeImage = H * W * 3;
        _strInfo.biXPelsPerMeter = 0;
        _strInfo.biYPelsPerMeter = 0;
        _strInfo.biClrUsed = 0;
        _strInfo.biClrImportant = 0;

        _strHead.bfSize = _strInfo.biSizeImage + _strInfo.biBitCount;

        _image = new IMAGEDATA*[H];
        for ( int i = 0 ; i < H ; i++ )
            _image[i] = new IMAGEDATA[W];
    }
};



#endif //PPM_IMAGE_H
