#ifndef UNIT_H
#define UNIT_H

#include <math.h>
#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::endl;

#include "SDL/SDL_image.h"
//#include "ConfigManager.h"
#include "Luaconfig.h"
#include "ConfigTypes.h"

enum e_unitID { STATIC = 0, PLAYER, ENTITY, OBJECT};

class Unit
{
public:
    Unit();
    virtual ~Unit();    
    bool Create( );
    void setUnitType( enum e_unitID t_Unit );
    virtual void update(const int&) {};
    virtual void moveUnit(  signed int x, signed int y , const int& dt) {};
    double dist( Unit* );

    void setUnitPos( float x, float y ) { setUnitX( x ); setUnitY( y ); }    
    void setUnitX( float x );
    void setUnitY( float y );
    int getUnitAnim() { return (const int)m_iAn; }

    void setUnitAnim( int );
    string getUnitName() { return defs->Name; }

    float getUnitX() { return (const float)m_fX; }
    float getUnitY() { return (const float)m_fY; }
    float* getUnitpX() { return &m_fX; }
    float* getUnitpY() { return &m_fY; }
    e_unitID getUnitType() { return Type; }
    
    SDL_Surface* getUnitImage() {return m_Img;}
    void setUnitImage( SDL_Surface* );
    string getUnitImageName( ) { return defs->imageName;  }
    
    int getUnitWidth() { return defs->width; }
    int getUnitHeight() { return defs->height; }
    int getUnitImageCols() { return defs->imagecols; }
    int getUnitImageRows() { return defs->imagerows; }

    void setPlayer( bool pl ) { player = pl; }
    bool isPlayer( ) { return player; }

protected:
    float m_fX, m_fY;
    
private:
    int m_iAn;
    bool player;
    e_unitID Type;
    SDL_Surface* m_Img;
    EntityDefs* defs;
        
};

#endif //UNIT_H
