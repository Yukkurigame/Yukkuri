#ifndef UNIT_H
#define UNIT_H

#include <string>
using std::string;

#include "GraphicsTypes.h"
#include "Luaconfig.h"
#include "ConfigTypes.h"

enum unitType { STATIC = 0, PLAYER, ENTITY, PLANT, CORPSE, OBJECT};

class Unit
{
public:
	Unit();
	virtual ~Unit();
	virtual void update( const int& );
	virtual void grow() {};
	virtual void moveUnit( signed int x, signed int y, const int& dt ) {};
	virtual void moveUnit( short axis, signed int val ) {};

	float dist( Unit* );

	virtual bool Create( int id );
	void Delete( );
	bool isDeleted( ) { return Deleted; }
	virtual bool loadAnimation( ) { return true; }
	void setUnitType( enum unitType type );
	void setUnitPos( float x, float y ) { setUnitX( x ); setUnitY( y ); }
	void setUnitX( float x );
	void setUnitY( float y );
	void setUnitSize( float size );
	void setUnitParameter( string name, float value );
	void increaseUnitParameter( string name, float value = 1 );

	void setUnitAnim( int );
	int getUnitAnim() { return (const int)Anim; }

	unsigned int getUnitId( ) { return UnitId; }

	bool setUnitName( string type );
	string getUnitName() { return UnitName; }

	float getUnitX() { return (const float)X; }
	float getUnitY() { return (const float)Y; }
	float getUnitSize() { return Scale; }
	float getUnitParameter( string name ) { return Parameters[name]; }
	float* getUnitpX() { return &X; }
	float* getUnitpY() { return &Y; }
	float* getUnitpParameter( string name );
	string getUnitType() { return Type; }
	enum unitType geteUnitType() { return UnitType; }

	template<typename T>
	void getConfigValue( string name, T& ret ){
		//FIXME: FUUUU
		LuaConfig* cfg = new LuaConfig;
		cfg->getValue( name, UnitName, Type, ret );
		delete cfg;
	}

	inline Sprite* getUnitImage() {return Image;}
	bool setUnitImage( Sprite* );
	string getUnitImageName( ) { return defs->imageName; }


	int getUnitWidth() { return defs->width; }
	int getUnitHeight() { return defs->height; }
	int getUnitImageCols() { return defs->imagecols; }
	int getUnitImageRows() { return defs->imagerows; }

	virtual bool isEdible( ) { return false; }
	virtual void hit( float ) {};

protected:
	float X, Y, Z;
	std::map < string, float > Parameters;

private:
	unsigned int UnitId;
	string UnitName;
	int Anim;
	string Type;
	enum unitType UnitType;
	Sprite* Image;
	float Scale;
	EntityDefs* defs;
	bool Deleted;

};

#endif //UNIT_H
