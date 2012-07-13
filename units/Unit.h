#ifndef UNIT_H
#define UNIT_H

#include <string>

#include "Animation.h"
#include "Prototypes.h"
#include "YOBA.h"
#include "3rdparty/CUDataUser.h"

class CUData;


enum unitType { utStatic = 0, utPlayer, utEntity, utPlant, utCorpse };
enum unitFlag { ufDeleted = 1, ufEdible = 2, ufMoving = 4, ufLast };

class Unit : public CUDataUser
{
public:
	Unit();
	virtual ~Unit();

	virtual bool Create( int id, std::string proto );

	inline unsigned char isDeleted()	{ return flags & 1; }
	inline void setDeleted()				{ flags |= 1; }
	inline void clearDeleted()			{ flags &= ~1; } // WHY?
	inline unsigned char isEdible()	{ return flags & 2; }
	inline void setEdible()				{ flags |= 2; }
	inline void clearEdible()			{ flags &= ~2; }

	void update( const int& );

	virtual bool update( const Frame& f );
	virtual void die( ) { setDeleted(); };
	virtual void moveUnit( signed int x, signed int y, const int& dt ) {};
	virtual void moveUnit( short axis, signed int val ) {};
	virtual void grow() { Char.grow(); }


	float dist( Unit* );

	bool setUnitName( std::string type );
	void setUnitType( enum unitType type );
	virtual void setUnitPos( float x, float y );
	inline void setUnitX( float x ) { setUnitPos(x, Y); }
	inline void setUnitY( float y ) { setUnitPos(X, y); }
	inline void setUnitSize( float size ) { Image.setSize(size); }
	inline void setUnitParameter( enum character param, float value ){ Char.set( param, value ); }
	inline void setUnitParameter( enum character_float param, float value ){ Char.set( param, value ); }

	inline int getUnitParameter( enum character param ){ 	return Char.get( param ); }
	inline int* getUnitpParameter( enum character param ){ return Char.getPtr( param ); }
	inline float getUnitParameter( enum character_float param ){ return Char.get( param ); }
	inline float* getUnitpParameter( enum character_float param ){ return Char.getPtr( param ); }


#define GET_PARAM( type, name, value ) \
	inline type getUnit##name() { return value; }

	GET_PARAM( unsigned int, Id, UnitId )

	GET_PARAM( std::string, Name, UnitName )
	GET_PARAM( std::string, Type, Type )
	GET_PARAM( enum unitType, eType, UnitType )
	GET_PARAM( float, X, (const float)X )
	GET_PARAM( float, Y, (const float)Y )
	GET_PARAM( float, Size, Image.getSize() )
	GET_PARAM( float*, pX, &X )
	GET_PARAM( float*, pY, &Y )
#undef GET_PARAM

	virtual void hit( float ) {};

protected:
	virtual CUData* createUData();

	float X, Y, Z;
	CharBuild Char;
	Animation Image;
	int DT;
	ActionManager Actions;
	std::string UnitName;
	enum unitType UnitType;
	std::string Type;

	unsigned int flags;		// 1 - deleted
							// 2 - isEdible
							// 4 - isMoving

private:
	unsigned int UnitId;

};

#endif //UNIT_H
