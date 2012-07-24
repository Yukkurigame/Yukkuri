#ifndef UNIT_H
#define UNIT_H

#include <string>

#include "graphics/Animation.h"
#include "units/ActionTimer.h"
#include "units/Prototypes.h"
#include "units/YOBA.h"
#include "units/physics/physics.h"
#include "3rdparty/CUDataUser.h"

class CUData;


enum unitType { utStatic = 0, utPlayer, utEntity, utPlant, utCorpse, utLast };
enum unitFlag { ufDeleted = 1, ufEdible = 2, ufMoving = 4, ufLast };

// Wrapper for velocity_func
void call_velocity_func(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);


class Unit : public CUDataUser
{
public:
	Unit();
	virtual ~Unit();

	virtual bool Create( int id, std::string proto );

	void setShape( cpShape* s ){
		physShape = cpSpaceAddShape( Phys::space, s );
		physShape->collision_type = 1 << UnitType;
	}

	inline unsigned char isDeleted()	{ return flags & 1; }
	inline void setDeleted()				{ flags |= 1; }
	inline void clearDeleted()			{ flags &= ~1; } // WHY?
	inline unsigned char isEdible()	{ return flags & 2; }
	inline void setEdible()				{ flags |= 2; }
	inline void clearEdible()			{ flags &= ~2; }

	void update( const int& );

	virtual bool update( const Frame& f );
	virtual void updatePhysics( );
	virtual void velocityFunc( ) { };
	virtual void die( ) { setDeleted(); };
	virtual void moveUnit( signed int x, signed int y, const int& dt ) {};
	virtual void moveUnit( short axis, signed int val ) {};
	virtual void grow() { Char.grow(); }


	float dist( Unit* );

	bool setUnitName( std::string type );
	void setUnitType( enum unitType type );
	virtual void setUnitPos( float x, float y );
	void setUnitSize( float size );
	inline void setUnitX( float x ) { setUnitPos( x, (float)physBody->p.y ); }
	inline void setUnitY( float y ) { setUnitPos( (float)physBody->p.x, y ); }
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
	GET_PARAM( std::string, TypeName, Type )
	GET_PARAM( enum unitType, Type, UnitType )
	GET_PARAM( const float, X, (const float)physBody->p.x )
	GET_PARAM( const float, Y, (const float)physBody->p.y )
	GET_PARAM( float, Size, Image.getSize() )
	GET_PARAM( double*, pX, &(physBody->p.x) )
	GET_PARAM( double*, pY, &(physBody->p.y) )
#undef GET_PARAM

	virtual void hit( float ) {};

	// Function bindings
	int color( lua_State* L );

protected:
	virtual CUData* createUData();

	ActionTimer* actionTimers;

	cpBody* physBody;
	cpShape* physShape;

	float Z;
	PhysObject phys;
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
