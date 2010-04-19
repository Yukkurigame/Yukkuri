#ifndef UNIT_H
#define UNIT_H

enum e_unitDir { NORTH = 0, EAST, NORWE, NOREA, SOUTH, WEST, SOUEA, SOUWE, STATIC };
enum e_unitID { WATER = 0, ISL, PRT, WRECK, PLY, CMP };

class Unit
{
public:
	void setUnitType( enum e_unitID t_Unit );
	void setUnitDir( enum e_unitDir t_Dir ) { Direction = t_Dir; }
	void moveUnit(  signed int x, signed int y ); //enum e_unitDir t_Dir );

	void setUnitPos( int x, int y ) { setUnitX( x ); setUnitY( y ); }
	void setUnitX( int x );
	void setUnitY( int y );

	int getUnitX() { return (const int)m_iX; }
	int getUnitY() { return (const int)m_iY; }
	
	e_unitID getUnitType() { return Type; }
	int getUnitDir() { return (const int)Direction; }

private:
	int m_iX, m_iY;
	e_unitDir Direction;
	e_unitID Type;
};

#endif //UNIT_H