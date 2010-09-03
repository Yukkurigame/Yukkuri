/*
 * Luaconfig.cpp
 *
 *  Created on: 04.06.2010
 */

#include "Luaconfig.h"
#include "config.h"

LuaConfig* LuaConfig::conf = 0;

LuaConfig::LuaConfig( )
{
	OpenFile( "data/scripts/configs.lua" );
}

LuaConfig::~LuaConfig( )
{

}

bool LuaConfig::LoadAll( string type )
{
	extern MainConfig conf;

	string dirname = ( conf.configsPath != "" ) ? conf.configsPath : "data/defs/";
    DIR *dp;
    struct dirent *ep;

    debug(3, "Loading " + type + "\n");
    dp = opendir (dirname.c_str());
    int success = 0;
    int files = 0;
    if( dp != NULL ){
        while ( (ep = readdir( dp ) ) != NULL) {
            string fname = string(ep->d_name);
            if(fname.substr(fname.find_last_of(".") + 1) == type){
            	files++;
                if ( OpenConfig( dirname + fname ) )
                	success++;
            }
        }
        closedir(dp);
    }else{
    	debug(3, "\tFAIL\n");
    	debug(3, "Bad directory." + dirname + "\n");
        return false;
    }
    //pdbg(3, "Done.\n");
    //FIXME: debug print
    char dbg[38];
    sprintf(dbg, "Loaded %d from %d config files.\n", success, files);
    debug(3, dbg);
	return true;
}

bool LuaConfig::OpenConfig( std::string filename )
{
	const int prmsz = 1;
	int ret;
	const char* prm[prmsz] = { filename.c_str() };
	//bool res =
	execFunction("load", prm, prmsz, ret);
	//FIXME: ever true it's bad
	return true;
}

//TODO: Как-то оно не так делает.
string LuaConfig::getRandom( string field, string config )
{
	string ret;
	const int prmsz = 2;
	const char* prm[prmsz] = { field.c_str(), config.c_str() };
	//bool res = execFunction("getOneFromSeveral", prm, prmsz, ret);
	execFunction("getOneFromSeveral", prm, prmsz, ret);
	return ret;
}

bool LuaConfig::getSubconfigs( string config, std::vector< string >& ret )
{
	const int prmsz = 1;
	const char* prm[prmsz] = { config.c_str() };
	bool res = execFunction("getSubconfigsList", prm, prmsz, ret);
	return res;
}
