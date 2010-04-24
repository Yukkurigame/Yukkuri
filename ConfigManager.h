#ifndef YCONFIGMANAGER_H
#define YCONFIGMANAGER_H

#include <string>
using std::string;
#include <map>
#include <vector>


struct EntityDef {
    string Name;
    string image;
    std::map < string, std::pair< int, int> > animation;
    int height;
    int width;
    float hp;
    int speed;     
    float damage;
    //int bloodcolor[3];
    //int meeting;
};
/*
class Config
{
    public:
        //void LoadAll();
        //bool LoadConfig( string filename );
        //bool LoadType( string filetype );
        
    private:            
}
*/
#endif //YCONFIGMANAGER_H