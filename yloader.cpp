#include "yloader.h"

YLoader YLoader::loader; //global is bad, yes

void operator >> (const YAML::Node& node, EntityDef& entity){
    //node["anim_distance"]
    //node["bloodcolor"] 
    node["damage"] >> entity.damage;
    node["height"] >> entity.height;
    node["hp"] >> entity.hp;
    node["image"] >> entity.image;
    //node["meeting"]
    node["name"] >> entity.Name;
    node["speed"] >> entity.speed;
    node["width"] >> entity.width;    
    const YAML::Node &anim = node["animation"][0];
    for(YAML::Iterator i = anim.begin(); i != anim.end(); ++i){        
        const YAML::Node & key   = i.first();
        const YAML::Node & value = i.second();
        std::pair <int, int> values ( value[0], value[1] );
        entity.animation[key] = values;
    }
}

/** Load image file
    @return A pointer to the SDL_Surface surface.
**/
SDL_Surface* YLoader::LoadImage( std::string filename )
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load( filename.c_str() );
    
    if( loadedImage != NULL )
    {
        if( loadedImage->format->Amask ) {
            optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
        } else {
            optimizedImage = SDL_DisplayFormat( loadedImage );
        }
        SDL_FreeSurface( loadedImage );
    }

    return optimizedImage;
}

void YLoader::LoadConfig( std::string filename )
{
    try{
        std::ifstream fin( filename.c_str() );
        YAML::Parser parser(fin);
        YAML::Node doc;
        parser.GetNextDocument(doc);
        cout << "Doc node size is " << doc.size() << endl;
        //Add way to process map files.
        for( unsigned int i =0; i< doc.size(); i++ ){
            string tag = doc[i].GetTag();
            if( tag == "!EntityDef"){
                cout << "Doc node tag: " << doc[i].GetTag() << endl;
                EntityDef entity;
                doc[i] >> entity;                
            }else{
                cout << "Unprocessed tag: " << doc.GetTag() << endl;
            }
        }
    }catch( YAML::ParserException &e ){
            cout << "YAML Exception caught in " << filename << ": " << e.what() << endl;
    }
}
