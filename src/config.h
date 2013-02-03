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
	struct {
		int windowWidth;
		int windowHeight;
		float maxFrameRate;
		float minFrameRate;
		int renderMethod;
	} video;
	float widgetsPosZ;
	int mapTileSize;
	std::string mapDefaultTile;
	int dayLength;
	int actionInterval;
	int maxSpawn;
	int maxEdibles;
	int playerDies;
	std::string default_image;
	struct {
		std::string configs;
		std::string scripts;
		std::string image;
		std::string normals;
		std::string fonts;
		std::string proto;
		std::string shaders;
		std::string meshes;
	} path;

	float _tileAngle;
};

bool loadMainConfig();


#endif /* CONFIG_H_ */
