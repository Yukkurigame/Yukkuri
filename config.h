/*
 * config.h
 *
 *  Created on: 02.09.2010
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <cstring>

struct MainConfig
{
	int windowWidth;
	int windowHeight;
	float maxFrameRate;
	float minFrameRate;
	float widgetsPosZ;
	int mapTileSize;
	int mapDefaultTile;
	int dayLength;
	int actionInterval;
	int maxSpawn;
	int maxEdibles;
	int playerDies;
	int textureBorder;
	std::string imagePath;
	std::string defaultImage;
	std::string fontsPath;
	std::string scriptsPath;
	std::string configsPath;

	MainConfig(){
		memset( this, 0, sizeof(this) );
	}
	bool load();

};

#endif /* CONFIG_H_ */
