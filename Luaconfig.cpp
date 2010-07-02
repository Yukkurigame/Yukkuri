/*
 * Luaconfig.cpp
 *
 *  Created on: 04.06.2010
 */

#include "Luaconfig.h"

LuaConfig LuaConfig::conf; //global is bad, yes

LuaStackChecker::LuaStackChecker( lua_State* L, const char* filename/* = ""*/, int line/* = 0*/ )
: luaState_(L),
  filename_(filename),
  line_(line)
{
  top_ = lua_gettop(L);
}

LuaStackChecker::~LuaStackChecker()
{
	int c;
	c = lua_gettop(luaState_) -  top_;
	if( c != 0 ){
		luaL_error(luaState_, "Lua stack corrupted! %d fields. File [%s] line[%d]", c , filename_, line_);
	}
}

LuaConfig::LuaConfig( )
{
	Lconf = lua_open();
    luaL_openlibs(Lconf);
    luaL_loadfile(Lconf, "data/scripts/configs.lua"); //TODO add SCRIPTPATH
    lua_pcall( Lconf, 0, LUA_MULTRET, 0 );
}

LuaConfig::~LuaConfig( )
{
	lua_close(Lconf);
}

bool LuaConfig::LoadAll( string type )
{
	debug(3, "Loading " + type + "\n");
	string dirname = "data/defs/"; //TODO: DEFSPATH
    DIR *dp;
    struct dirent *ep;
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
    	debug(3, "Bad directory.\n");
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
	LuaStackChecker sc(Lconf, __FILE__, __LINE__);
	int stat;
	//Load config file from yaml
	lua_getfield( Lconf, LUA_GLOBALSINDEX, "load" );
	lua_pushstring( Lconf, filename.c_str() );
	stat = lua_pcall( Lconf, 1, 0, 0 );
	if( stat )
			return false;
	return true;
}

//TODO: Create right execute function
bool LuaConfig::execFunction( std::string function, std::string param )
{
	LuaStackChecker sc(Lconf, __FILE__, __LINE__);
	int stat;
	lua_getfield( Lconf, LUA_GLOBALSINDEX, function.c_str() );
	//TODO: variable params
	lua_pushstring( Lconf, param.c_str() );
	stat = lua_pcall( Lconf, 1, 0, 0 );
	if( stat )
			return false;
	return true;
}

string LuaConfig::getRandom( string field, string config )
{
	LuaStackChecker sc( Lconf, __FILE__, __LINE__ );
	lua_getfield( Lconf, LUA_GLOBALSINDEX, "getOneFromSeveral" );
	lua_pushstring( Lconf, field.c_str() );
	lua_pushstring( Lconf, config.c_str() );
	lua_call( Lconf, 2, 1 );
	string ret = "";
	if( lua_isstring(Lconf, -1) ){
		ret = lua_tostring(Lconf, -1);
	}
	lua_pop( Lconf, 1 );
	return ret;
}

bool LuaConfig::getSubconfigs( string config, std::vector< string >& ret )
{
	LuaStackChecker sc( Lconf, __FILE__, __LINE__ );
	lua_getfield( Lconf, LUA_GLOBALSINDEX, "getSubconfigsList" );
	lua_pushstring( Lconf, config.c_str() );
	lua_call( Lconf, 1, 1 );
	bool res = getValue( Lconf, -1, ret );
	lua_pop( Lconf, 1 );
	return res;
}

template<> bool LuaConfig::getValue( lua_State* L, int index, bool& ret)
{
	if( lua_isboolean(L, index) ){
		ret = lua_toboolean(L, index) != 0;
		return true;
	}
	return false;
}

//FIXME: Но зачем
template<> bool LuaConfig::getValue( lua_State* L, int index, float& ret)
{
	if( lua_isnumber(L, index) ){
		ret = lua_tonumber(L, index);
		return true;
	}
	return false;
}

template<> bool LuaConfig::getValue( lua_State* L, int index, double& ret)
{
	if( lua_isnumber(L, index) ){
		ret = lua_tonumber(L, index);
		return true;
	}
	return false;
}

template<> bool LuaConfig::getValue( lua_State* L, int index, int& ret)
{
	if( lua_isnumber(L, index) ){
		ret = static_cast<int>(lua_tonumber(L, index));
		return true;
	}
	return false;
}

template<> bool LuaConfig::getValue( lua_State* L, int index, string& ret)
{
	if( lua_isstring(L, index) ){
		ret = lua_tostring(L, index);
		return true;
	}
	return false;
}
