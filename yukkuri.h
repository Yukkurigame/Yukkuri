#ifndef YUKKURI_H
#define YUKKURI_H

#include <vector>
using std::vector;

#include "engine.h"
#include "unitmanager.h"
//#include "map.h"
#include "Camera.h"

#define pixel_Loc(A)    ( (A * 64) + 12 )

class Yukkuri: public CEngine
{
public:
    void AdditionalInit ();
    void Think        ( const int& iElapsedTime );
    void Render        ( SDL_Surface* pDestSurface );
 
    void KeyUp          (const int& iKeyEnum);
    void KeyDown        (const int& iKeyEnum);
 
    void MouseMoved     (const int& iButton, 
                 const int& iX, 
                 const int& iY, 
                 const int& iRelX, 
                     const int& iRelY);
 
    void MouseButtonUp  (const int& iButton, 
                 const int& iX, 
                 const int& iY, 
                 const int& iRelX, 
                     const int& iRelY);
 
    void MouseButtonDown(const int& iButton, 
                 const int& iX, 
                 const int& iY, 
                 const int& iRelX, 
                     const int& iRelY);
 
    void WindowInactive();
    void WindowActive();
 
    void End();    

    void drawMap( SDL_Surface* Surf_Display, int MapX, int MapY );

protected:
    SDL_Surface* objects_sfc;
private:
    UnitManager units;
    //Map map;
};

#endif //YUKKURI_H