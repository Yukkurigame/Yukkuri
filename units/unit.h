#ifndef UNIT_H
#define UNIT_H

#include <math.h>
#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::endl;

#include "GraphicsTypes.h" //TODO: disable
#include "Luaconfig.h"
#include "ConfigTypes.h"

enum e_unitID { STATIC = 0, PLAYER, ENTITY, PLANT, OBJECT};

class Unit
{
public:
    Unit();
    virtual ~Unit();
    virtual void update( const int& ) {};
    virtual void grow() {};
    virtual void moveUnit( signed int x, signed int y, const int& dt ) {};
    virtual void moveUnit( short axis, signed int val ) {};

    double dist( Unit* );

    virtual bool Create( );
    void Delete( );
    bool isDeleted( ) { return Deleted; }
    virtual bool loadAnimation( ) { return true; }
    void setUnitType( enum e_unitID t_Unit );
    void setUnitPos( float x, float y ) { setUnitX( x ); setUnitY( y ); }
    void setUnitX( float x );
    void setUnitY( float y );
    int getUnitAnim() { return (const int)Anim; }

    void setUnitAnim( int );

    void setUnitName( string type );
    string getUnitName() { return UnitName; }

    float getUnitX() { return (const float)X; }
    float getUnitY() { return (const float)Y; }
    float* getUnitpX() { return &X; }
    float* getUnitpY() { return &Y; }
    string getUnitType() { return Type; }

    template<typename T>
    void getConfigValue( string name, T& ret ){
    	LuaConfig::Instance()->getValue( name, UnitName, Type, ret );
    }

    //как-то это не хорошо, почему картинка хранится в юните? структ какой придумать что ли.
    inline Sprite* getUnitImage() {return Image;}
    bool setUnitImage( Sprite* );
    string getUnitImageName( ) { return defs->imageName;  }


    int getUnitWidth() { return defs->width; }
    int getUnitHeight() { return defs->height; }
    int getUnitImageCols() { return defs->imagecols; }
    int getUnitImageRows() { return defs->imagerows; }

    virtual bool isEdible( ) { return false; }
    virtual void hit( float ) {};
    virtual float getNutritive( ) { return 0; }

protected:
    float X, Y;
    float Scale;

private:
    string UnitName;
    int Anim;
    string Type;
    Sprite* Image;
    EntityDefs* defs;
    bool Deleted;

};

#endif //UNIT_H
