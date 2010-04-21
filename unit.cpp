#include "unit.h"

Unit::Unit()
{
    // Load up additional data
    m_iAn = 0;
    m_fdistance = 0;
    m_img = loader.LoadImage( "data/images/reimu.png" );    
    loader.LoadAnimation(11, 4, 79, 68);
    if( !m_img ) {
        cout << endl << "Could not load all images. Please ensure Res folder is populated" << endl;
        exit( 1 );
    }
}

Unit::~Unit()
{
}

void Unit::setUnitType( enum e_unitID t_Unit )
{
        Type = t_Unit;        

    return;
}

/** Move the unit according to the eight cardinal directions.
    @return Boolean value represents a Collision (true)
    @remark 
**/
void Unit::moveUnit( signed int x, signed int y, const int& dt )
{     
    if( x != 0 || y != 0 )
    {
        int l = sqrt( x * x + y * y );
        //    speed = abs(self.dfn.speed * self.fed * 2) #(self.dfn.speed * self.world.map.speed(self.x, self.y))
        float speed = 80.0f * ( dt / 1000.0f);
        if( speed < 0.05f ) 
            speed = 0.05f;
        float dx = speed * x / l;
        float dy = speed * y / l;        
        m_fdistance += sqrt( dx * dx + dy * dy );        
        int anim; //FUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
        if( dx < 0 && dy < 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 16; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx < 0 && dy > 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 8; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx > 0 && dy < 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 28; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx > 0 && dy > 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 20; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dy < 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 4; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dy > 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 0; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx < 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 12; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx > 0 ) //UUUUUUUUUUUUUUUUUUUUUU 
            anim = 24; //UUUUUUUUUUUUUUUUUUUUUU
        cout << dx << '|' << dy << '|' << anim << endl;
        setUnitAnim( anim +((const int)(m_fdistance / m_animdistance) % 4));
        m_fX += dx;
        m_fY += dy;
    }
}

void Unit::setUnitX( float x )
{
//    if( !m_iX )
        m_fX = x;

    return;
}

void Unit::setUnitY( float y )
{
//    if( !m_iY )
        m_fY = y;

    return;
}

SDL_Rect* Unit::getUnitAnim( )
{
    return loader.GetAnim(m_iAn);
}

void Unit::setUnitAnim( int num )
{
    m_iAn = num;
}