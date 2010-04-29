#ifndef YCONFIGMANAGER_H
#define YCONFIGMANAGER_H

#include <sys/types.h>
#include <dirent.h>
#include <string>
using std::string;
#include <map>
#include <vector>
#include "ConfigTypes.h"
#include "yloader.h"

class Config
{
    public:
        static Config conf;
        //void LoadAll();
        bool LoadEntities( );
        //bool LoadType( string filetype );
        ConfigSet* FindEntity( string );
    private:
        std::map < string, ConfigSet > Entities;
};

#endif //YCONFIGMANAGER_H