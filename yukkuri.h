#ifndef YUKKURI_H
#define YUKKURI_H

#include <vector>
using std::vector;

#include "engine.h"
#include "unitmanager.h"
//#include "map.h"
#include "Interface.h"
#include "Camera.h"
//#include "SDL_getenv.h"
#include "daytime.h"

class Yukkuri: public CEngine
{
public:
    void AdditionalInit ();
    void Think        ( const int& iElapsedTime );
    void Render        (  );
 
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


private:
    UnitManager units;
    DayTime daytime;
};

#endif //YUKKURI_H
