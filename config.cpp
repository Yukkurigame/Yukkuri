/*
 * config.cpp
 *
 *  Created on: 02.09.2010
 */

#include "config.h"
#include "LuaConfig.h"
#include <cstring>

MainConfig conf;

MainConfig::MainConfig()
{
		memset( this, 0, sizeof(this) );
}

bool MainConfig::load( )
{
	LuaConfig* lc = new LuaConfig;

	std::string id;
	std::string config;
	id = "config_general";
	config = "config";
	lc->getValue( "windows_width" , id, config, windowWidth );
	lc->getValue( "windows_height" , id, config, windowHeight );
	lc->getValue( "maximum_frame_rate" , id, config, maxFrameRate );
	lc->getValue( "minimum_frame_rate" , id, config, minFrameRate );
	lc->getValue( "texture_border" , id, config, textureBorder );

	lc->getValue( "images_path" , id, config, imagePath );
	lc->getValue( "default_image_name" , id, config, defaultImage );
	lc->getValue( "fonts_path" , id, config, fontsPath );
	lc->getValue( "scripts_path" , id, config, scriptsPath );
	lc->getValue( "configs_path" , id, config, configsPath );
	lc->getValue( "proto_path" , id, config, protoPath );

	lc->getValue( "widgets_z" , id, config, widgetsPosZ );

	lc->getValue( "map_tile_size" , id, config, mapTileSize );
	lc->getValue( "map_default_tile" , id, config, mapDefaultTile );
	lc->getValue( "day_length" , id, config, dayLength );
	lc->getValue( "action_interval" , id, config, actionInterval );
	lc->getValue( "max_spawn" , id, config, maxSpawn );
	lc->getValue( "max_edibles" , id, config, maxEdibles );

	lc->getValue( "player_dies" , id, config, playerDies );

	delete lc;

	return true;
};
