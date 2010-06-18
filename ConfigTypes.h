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
        void set(std::string, std::map <string, std::pair <int, int> > );
        void set(std::string, std::vector < int > );
        string get( std::string );
        
    private:
        std::map <string, string> Strings;
        std::map <string, std::map <string, std::pair <int, int> > > Pairs;
        std::map <string, std::vector < int > > Vectors;
};

#endif //YCONFIGTYPES_H
