/*
 * config.h
 *
 *  Created on: 02.09.2010
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>


struct MainConfig
{
	int windowWidth;
	int windowHeight;
	float maxFrameRate;
	float minFrameRate;
	float widgetsPosZ;
	int mapTileSize;
	std::string mapDefaultTile;
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
	std::string protoPath;
	std::string shadersPath;

	float _tileAngle;

	MainConfig();

	bool load();

};

#endif /* CONFIG_H_ */
