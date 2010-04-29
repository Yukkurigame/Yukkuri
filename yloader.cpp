#include "yloader.h"

YLoader YLoader::loader; //global is bad, yes

void operator >> (const YAML::Node& node, ConfigSet& conf){
    //node["anim_distance"]
    //node["bloodcolor"]
    for(YAML::Iterator i = node.begin(); i != node.end(); ++i){        
        const YAML::Node & key   = i.first();
        const YAML::Node & value = i.second();
        YAML::CONTENT_TYPE type = value.GetType();
        string out, k;
        key >> k;        
        switch (type){
            case YAML::CT_SCALAR:
                value >> out;
                conf.set(k, out);
                break;
        }
    }
    //node["damage"] >> conf;    
    /*node["damage"] >> conf.damage;
    node["height"] >> conf.height;
    node["hp"] >> conf.hp;
    node["image"] >> conf.image;
    //node["meeting"]
    node["name"] >> conf.Name;
    node["speed"] >> conf.speed;
    node["width"] >> conf.width;
    node["image_cols"] >> conf.imagecols;
    node["image_rows"] >> conf.imagerows;
    const YAML::Node &anim = node["animation"][0];
    for(YAML::Iterator i = anim.begin(); i != anim.end(); ++i){        
        const YAML::Node & key   = i.first();
        const YAML::Node & value = i.second();
        std::pair <int, int> values ( value[0], value[1] );
        conf.animation[key] = values;
    }
    */
}

/** Load image file
    @return A pointer to the SDL_Surface surface.
**/
SDL_Surface* YLoader::LoadImage( std::string filename )
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load( filename.c_str() );
    
    if( loadedImage != NULL ){
        if( loadedImage->format->Amask ) {
            optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
        } else {
            optimizedImage = SDL_DisplayFormat( loadedImage );
        }
        SDL_FreeSurface( loadedImage );
    }else{
        cout << "Could not load " << filename << endl;
        return NULL;
    }
    return optimizedImage;
}

bool YLoader::LoadConfig( std::string filename, vector< ConfigSet > *pvec )
{
   try{   	    
        std::ifstream fin( filename.c_str() );
        YAML::Parser parser(fin);
        YAML::Node doc;
        parser.GetNextDocument(doc);
        //cout << "Doc node size is " << doc.size() << endl;
        //Add way to process map files.
        for( unsigned int i =0; i< doc.size(); i++ ){
            //wtf??
            //string tag = doc[i].GetTag();
            //if( tag == "!confDef"){
                //cout << "Doc node tag: " << doc[i].GetTag() << endl;
                ConfigSet node;
                doc[i] >> node;
                pvec->push_back(node);
            //}else{
                //cout << "Unprocessed tag: " << doc.GetTag() << endl;
            //}
        }
    }catch( YAML::ParserException &e ){
            cout << "YAML Exception caught in " << filename << ": " << e.what() << endl;
            return false;
    }        
    return true;    
}