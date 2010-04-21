#ifndef UNIT_H
#define UNIT_H

#include <math.h>
#include <iostream>
using std::cout;
using std::endl;

#include "SDL/SDL_image.h"
#include "yloader.h"

enum e_unitDir { NORTH = 0, EAST, NORWE, NOREA, SOUTH, WEST, SOUEA, SOUWE, STATIC };
enum e_unitID { WATER = 0, ISL, PRT, WRECK, PLY, CMP };

class Unit
{
public:
   Unit();
   virtual ~Unit();
    void setUnitType( enum e_unitID t_Unit );
    void setUnitDir( enum e_unitDir t_Dir ) { Direction = t_Dir; }
    void moveUnit(  signed int x, signed int y , const int& dt); //enum e_unitDir t_Dir );

    void setUnitPos( int x, int y ) { setUnitX( x ); setUnitY( y ); }    
    void setUnitX( float x );
    void setUnitY( float y );
    
    void setUnitAnim( int );
    SDL_Rect* getUnitAnim();

    int getUnitX() { return (const float)m_fX; }
    int getUnitY() { return (const float)m_fY; }
    e_unitID getUnitType() { return Type; }
    int getUnitDir() { return (const int)Direction; }
    
    SDL_Surface* getImage() {return m_img;}

private:
    YLoader loader;
    float m_fX, m_fY, m_fdistance; 
    int m_iAn;
    const static int m_animdistance = 20;
    e_unitDir Direction;
    e_unitID Type;
    SDL_Surface* m_img;    
        
};

#endif //UNIT_H