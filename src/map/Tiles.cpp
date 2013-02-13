/*
 * Tiles.cpp
 *
 *  Created on: 27.05.2012
 *
 */

#include "map/Tiles.h"
#include "map/Region.h"
#include "map/Map.h"

namespace {

	unsigned int TilesCount = 0;

}


void MapTile::create( signed int x, signed int y )
{
	TileID = TilesCount;
	TilesCount++;

	Real.x = pos.x = x;
	Real.y = pos.y = y;

	Type = Region::getTile( x, y );

	Map::fromMapCoordinates( Real );

	//sprite = NULL;
	//sprite = RenderManager::Instance()->CreateGLSprite( RealX - ( conf.mapTileSize >> 1 ),
							//RealY - ( conf.mapTileSize >> 2 ), 0,
							//conf.mapTileSize, conf.mapTileSize,
							//Type->texture, Type->picture );

	//FIXME: я не уверен, что это правильно, но выглядит нормально. Может внезапно вылезти боком.
	//Image.x = RealX - ( conf.mapTileSize >> 1 );
	//Image.y = RealY - ( conf.mapTileSize >> 2 );
}

