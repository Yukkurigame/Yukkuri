#ifndef YUKKURI_H
#define YUKKURI_H

#include <vector>
using std::vector;

#include "engine.h"
//#include "yloader.h"
#include "unit.h"
#include "unitmanager.h"

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

protected:
    SDL_Surface* objects_sfc;
    //SDL_Surface* img;
private:
    UnitManager units;
    //gameState states;
};

#endif //YUKKURI_H