#ifndef UNIT_H
#define UNIT_H

#include <math.h>
#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::endl;

#include "SDL/SDL_image.h"
//#include "Graphics.h"
//#include "yloader.h"

enum e_unitID { STATIC = 0, PLAYER, OBJECT, ENTITY};

class Unit
{
public:
   Unit();
   virtual ~Unit();
    void setUnitType( enum e_unitID t_Unit );
    //void setUnitDir( enum e_unitDir t_Dir ) { Direction = t_Dir; }
    void moveUnit(  signed int x, signed int y , const int& dt); //enum e_unitDir t_Dir );

    void setUnitPos( int x, int y ) { setUnitX( x ); setUnitY( y ); }    
    void setUnitX( float x );
    void setUnitY( float y );
    
    void setUnitAnim( int );
    int getUnitAnim() { return (const int)m_iAn; }

    string getUnitName() { return UnitName; }
    void setUnitName( string name );

    float getUnitX() { return (const float)m_fX; }
    float getUnitY() { return (const float)m_fY; }
    float* getUnitpX() { return &m_fX; }
    float* getUnitpY() { return &m_fY; }
    e_unitID getUnitType() { return Type; }
    
    SDL_Surface* getUnitImage() {return m_Img;}
    void setUnitImage( SDL_Surface* );

private:
    float m_fX, m_fY, m_fdistance; 
    int m_iAn;
    const static int m_animdistance = 20;
    string UnitName;
    e_unitID Type;
    SDL_Surface* m_Img;
        
};

#endif //UNIT_H