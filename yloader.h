#ifndef YUKKURILOADER_H
#define YUKKURILOADER_H

#include <cstdlib>
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;

#include "SDL/SDL_image.h"
#include "Graphics.h"
#include"yaml.h"
#include "ConfigTypes.h"

class YLoader
{
    public:
        static YLoader loader;
        SDL_Surface* LoadImage( std::string );
        
        //template< class T >
        //bool LoadConfig( std::string, vector< T>* );
        template< class T >
        bool LoadConfig( std::string filename, vector< T > *pvec ){
        	
        	try{
        		/*std::ifstream fin( filename.c_str() );
        		YAML::Parser parser(fin);
        		YAML::Node doc;
        		parser.GetNextDocument(doc);
        		cout << "Doc node size is " << doc.size() << endl;
        		 //Add way to process map files.
        		 for( unsigned int i =0; i< doc.size(); i++ ){
        		 	 //wtf??
        		 	 //string tag = doc[i].GetTag();
        		 	 //if( tag == "!EntityDef"){
        		 	 	 //cout << "Doc node tag: " << doc[i].GetTag() << endl;
        		 	 	 T node;
        		 	 	 doc[i] >> node;
        		 	 	 pvec->push_back(T);
        		 	 //}else{
        		 	 	 //cout << "Unprocessed tag: " << doc.GetTag() << endl;
        		 	  //}
        		 */
                //}
            }catch( YAML::ParserException &e ){
            	//cout << "YAML Exception caught in " << filename << ": " << e.what() << endl;
            	return false;
            }    
        	return true;    
        }

};

#endif //YUKKURILOADER_H