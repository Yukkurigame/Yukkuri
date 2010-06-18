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
#include "Luaconfig.h"

class Config
{
    public:
	    Config( );
        static Config conf;
        //void LoadAll();
        bool LoadEntities( );
        bool Load( string );
        bool LoadAll( string );
        string getString( string name );
        double getNumber( string name );
        //bool LoadType( string filetype );
        ConfigSet* FindEntity( string );
    private:
        LuaConfig* Luaconf;
        std::map < string, ConfigSet > Entities; //Как-то это не хорошо, думаю надо делать new в Load и хранить указатели. 
};

#endif //YCONFIGMANAGER_H
