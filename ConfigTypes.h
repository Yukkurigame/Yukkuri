#ifndef YCONFIGTYPES_H
#define YCONFIGTYPES_H

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
    int imagecols;
    int imagerows;
    //int bloodcolor[3];
    //int meeting;
};

#endif //YCONFIGTYPES_H