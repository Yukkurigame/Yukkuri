#include "ConfigManager.h"

Config Config::conf; //global is bad, yes

bool Config::LoadEntities()
{
    cout << "Load entities";
    DIR *dp;
    struct dirent *ep;
    vector < string > files;
    dp = opendir ("data/defs/");
    if( dp != NULL ){
        while ( (ep = readdir( dp ) ) != NULL) {
            string fname = string(ep->d_name);
            if(fname.substr(fname.find_last_of(".") + 1) == "entity")
                files.push_back(fname);
        }
        closedir(dp);        
    }else{
        cout << "[FAIL]" << endl;
        cout << "bad directory" << endl;         
        return false;
    }
    int success = 0;
    vector<ConfigSet> pvec;
    for (vector<string>::iterator it=files.begin(); it < files.end(); it++){         
        if( YLoader::loader.LoadConfig( "data/defs/" + *it, &pvec ) )
            success++;
    }
    for (vector<ConfigSet>::iterator it=pvec.begin(); it < pvec.end(); it++){
    	string name = (*it).get("name");
    	Entities[name] = (*it);
    }
    cout << " done." << endl;
    cout << "Loaded " << success << " from " << files.size() << " config files." << endl;
    return true;
}



ConfigSet* Config::FindEntity( string name )
{
    map < string, ConfigSet>::iterator iter = Entities.find( name );
    if( iter != Entities.end() )
        return &iter->second;
    return NULL;
}

