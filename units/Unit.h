#ifndef UNIT_H
#define UNIT_H

#include <string>

#include "Animation.h"
#include "Prototypes.h"
#include "3rdparty/CUDataUser.h"

class CUData;


enum unitType { utStatic = 0, utPlayer, utEntity, utPlant, utCorpse };

class Unit : public CUDataUser
{
public:
	Unit();
	virtual ~Unit();

	virtual bool Create( int id );

	inline unsigned char isDeleted()	{ return flags & 1; }
	inline void setDeleted()				{ flags |= 1; }
	inline void clearDeleted()			{ flags &= ~1; } // WHY?


	virtual void update( const int& );
	virtual void grow() {};
	virtual void die( ) { setDeleted(); };
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
	inline float* getUnitpX() { return &X; }
	inline float* getUnitpY() { return &Y; }
	inline float* getUnitpParameter( std::string name ){
		return &Parameters[name];
	}


	virtual bool isEdible( ) { return false; }
	virtual void hit( float ) {};

protected:
	virtual CUData* createUData();

	float X, Y, Z;
	int HP;
	std::map < std::string, float > Parameters;
	Animation Image;
	ActionManager Actions;
	std::string UnitName;
	enum unitType UnitType;
	std::string Type;

private:
	unsigned int UnitId;
	unsigned int flags;		// 1 - deleted
};

#endif //UNIT_H
