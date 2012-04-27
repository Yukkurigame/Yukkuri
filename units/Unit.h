#ifndef UNIT_H
#define UNIT_H

#include <string>

#include "Animation.h"


enum unitType { STATIC = 0, PLAYER, ENTITY, PLANT, CORPSE, OBJECT};

class Unit
{
public:
	Unit();
	virtual ~Unit();

	virtual bool Create( int id );
	void Delete( );
	inline bool isDeleted( ) { return Deleted; }

	virtual void update( const int& );
	virtual void grow() {};
	virtual void moveUnit( signed int x, signed int y, const int& dt ) {};
	virtual void moveUnit( short axis, signed int val ) {};

	float dist( Unit* );

	bool setUnitName( std::string type );
	void setUnitType( enum unitType type );
	virtual void setUnitPos( float x, float y );
	inline void setUnitX( float x ) { setUnitPos(x, Y); }
	inline void setUnitY( float y ) { setUnitPos(X, y); }
	void setUnitSize( float size );
	void setUnitParameter( std::string name, float value );
	void increaseUnitParameter( std::string name, float value = 1 );


	inline unsigned int getUnitId( ) { return UnitId; }

	std::string getUnitName() { return UnitName; }
	std::string getUnitType() { return Type; }
	enum unitType geteUnitType() { return UnitType; }
	inline float getUnitX() { return (const float)X; }
	inline float getUnitY() { return (const float)Y; }
	inline float getUnitSize() { return Image.getSize(); }
	inline float getUnitParameter( std::string name ) { return Parameters[name]; }
	float* getUnitpX() { return &X; }
	float* getUnitpY() { return &Y; }
	float* getUnitpParameter( std::string name );

	virtual bool isEdible( ) { return false; }
	virtual void hit( float ) {};

protected:
	float X, Y, Z;
	std::map < std::string, float > Parameters;
	Animation Image;
	std::string UnitName;
	enum unitType UnitType;
	std::string Type;

private:
	unsigned int UnitId;
	bool Deleted;
};

#endif //UNIT_H
