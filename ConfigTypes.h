#ifndef YCONFIGTYPES_H
#define YCONFIGTYPES_H

#include <string>
using std::string;
//#include <map>
//#include <vector>

/*class ConfigSet
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
*/

struct EntityDefs
{
	string Name;
	string imageName;
	int width;
	int height;
	int imagecols;
	int imagerows;
};

//TODO: ororoshenkiroro
struct AnimationDefs
{
	int down[2];
	int leftdown[2];
	int left[2];
	int leftup[2];
	int up[2];
	int rightup[2];
	int right[2];
    int rightdown[2];
};

#endif //YCONFIGTYPES_H
