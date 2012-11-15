/*
 * YOBA.h
 *
 *  Created on: 21.06.2012
 *
 *	Yukkuri role-playing system
 *
 */

#ifndef YOBA_H_
#define YOBA_H_

#include <math.h>
#include <stdlib.h>

enum character {
	// Base
	uBaseLevel = 1, uBaseAge, uBaseKills,
	// Attributes
	uAttrStrength, uAttrIntelligence, uAttrAgility, uAttrLuck,

	uCharIntLast
};

enum character_float {
	// Characteristics
	uChrSpeed = uCharIntLast + 1, uChrDamage, uChrDefence, uChrNutritive,
	// Parametes
	uParamHP, uParamExp, uParamFed,
	// State
	uStateHP, uStateExp, uStateFed,

	uCharLast
};


struct Attributes {
	int strength;
	int intelligence;
	int agility;
	int luck;
	Attributes( ) : strength(1), intelligence(1), agility(1), luck(100) {}
};

struct Characteristics {
	float speed;	// Max speed in m/s
	float damage;
	float defence;
	float nutritive;

	Characteristics() : speed(1.0), damage(1.0), defence(0), nutritive(1.0) {}
};

struct Skills {

};


struct Parameters
{
	float hp;
	float exp;
	float fed; // percents
	Parameters() : hp(1.0), exp(20.0), fed(100.0) {}

};

// For null references
namespace {
	static int NULL_INT = 0;
	static float NULL_FLOAT = 0.0;
}

struct CharBuild {

	int level;
	int age;
	int kills;

	Attributes attr;
	Characteristics chars;
	Skills skils;

	// Maximum
	Parameters params;
	// Current
	Parameters state;


	CharBuild() : level(1) {}

#define PARAM_SWITCH( pre, post ) 			\
	switch( name ){							\
		case uBaseLevel:					\
			pre level post;					\
			break;							\
		case uBaseAge:						\
			pre age post;					\
			break;							\
		case uBaseKills: 					\
			pre kills post;					\
			break;							\
		case uAttrStrength:					\
			pre attr.strength post;			\
			break;							\
		case uAttrIntelligence:				\
			pre attr.intelligence post;		\
			break;							\
		case uAttrAgility:					\
			pre attr.agility post;			\
			break;							\
		case uAttrLuck:						\
			pre attr.luck post;				\
			break;							\
		case uCharIntLast:					\
			break;							\
	}
#define PARAM_SWITCH_FLOAT( pre, post ) 	\
	switch( name ){							\
		case uChrSpeed:						\
			pre chars.speed post;			\
			break;							\
		case uChrDamage:					\
			pre chars.damage post;			\
			break;							\
		case uChrDefence:					\
			pre chars.defence post;			\
			break;							\
		case uChrNutritive:					\
			pre chars.nutritive post;		\
			break;							\
		case uParamHP:						\
			pre params.hp post;				\
			break;							\
		case uParamExp:						\
			pre params.exp post;			\
			break;							\
		case uParamFed:						\
			pre params.fed post;			\
			break;							\
		case uStateHP:						\
			pre state.hp post;				\
			break;							\
		case uStateExp:						\
			pre state.exp post;				\
			/* return in get */				\
			if( state.exp >= params.exp )	\
				levelUp();					\
			break;							\
		case uStateFed:						\
			pre state.fed post;				\
			break;							\
		case uCharLast:						\
			break;							\
	}


	inline int get( enum character name ){
		PARAM_SWITCH( return,  );
		return 0;
	}
	inline float get( enum character_float name ){
		PARAM_SWITCH_FLOAT( return,  );
		return 0.0;
	}
	inline float get( int name ){
		if( name < uCharIntLast )
			return (float)get( (enum character)name );
		return get( (enum character_float)name );
	}
	inline int* getPtr( enum character name ){
		PARAM_SWITCH( return &,  );
		return 0;
	}
	inline float* getPtr( enum character_float name ){
		PARAM_SWITCH_FLOAT( return &,  );
		return 0;
	}
	inline int& getRef( enum character name ){
		PARAM_SWITCH( return,  );
		return NULL_INT;
	}
	inline float& getRef( enum character_float name ){
		PARAM_SWITCH_FLOAT( return,  );
		return NULL_FLOAT;
	}

	inline void set( enum character name, float value ){
		PARAM_SWITCH(  , = (int)value );
	}
	inline void set( enum character_float name, float value ){
		PARAM_SWITCH_FLOAT(  , = value );
	}
	inline void set( int name, float val ){
		if( name < uCharIntLast )
			set( (enum character)name, val );
		else
			set( (enum character_float)name, val );
	}

#undef PARAM_SWITCH
#undef PARAM_SWITCH_FLOAT

	inline float getDamage() {
		float dmg = chars.damage / state.fed * 100;
		if( dmg < 0.4 )
			dmg = 0.4f;
		return dmg;
	}

	inline void grow(){
		++age;
		set( uStateExp,  state.exp + log(10 * level) );
	}

	inline void recieveDamage( float dmg ){
		state.hp -= dmg;
	}

	inline void tire( ){ tire( 0.2f * log( (float)level ) ); }
	inline void tire( float value ){
		if( state.fed > 1 )
			state.fed -= value;
	}

	inline void levelUp( int addlevel = 1 ){
		for( int i = 1; i<= addlevel; ++i){
			++level;
			if( params.hp == 0 ) params.hp = 1;
			state.hp = state.hp / params.hp;
			state.exp -= params.exp;
			params.hp += 10 * level;
			params.exp += abs(100 * log( level / 40.0f ));
			state.hp = params.hp * state.hp;
		}
	}
};


#endif /* YOBA_H_ */
