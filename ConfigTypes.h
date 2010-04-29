#ifndef YCONFIGTYPES_H
#define YCONFIGTYPES_H

#include <string>
using std::string;
#include <map>
#include <vector>

class ConfigSet
{
    public:
        void set(std::string, std::string);
        string get( std::string );
        
    private:
        std::map <string, string> Strings;
};

#endif //YCONFIGTYPES_H