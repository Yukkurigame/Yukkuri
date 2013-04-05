/*
 * RegionFile.cpp
 *
 *  Created on: 04.04.2013
 */

#include "map/RegionFile.h"
#include "debug.h"
#include <sys/stat.h>
#include <stdio.h>

RegionFile::ChunkBuffer::ChunkBuffer( RegionFile& parent, int x, int y ) :
		_file( parent ), _x( x ), _y( y )
{
}

RegionFile::RegionFile( const char* path )
{
	offsets = new int[1024];
	fileName = path;
	_lastModified = 0;

	Debug::debug( Debug::MAP, "Region load %s.\n", fileName );
	sizeDelta = 0;

	struct stat fileStat;
	int stat_status;
	if( ( stat_status = stat( path, &fileStat ) ) == 0 )
		_lastModified = fileStat.st_mtime;

	file = fopen( path, "rw" );
	if( !stat_status || fileStat.st_size < SECTOR_LENGTH ){
		/* we need to write the chunk offset table */
		fseek( file, 0L, SEEK_SET );
		for( int i = 0; i < 1024; ++i )
			fprintf( file, "%d", 0);
		sizeDelta += SECTOR_LENGTH;
	}

	fseek( file, 0L, SEEK_END );
	int seek_length = ftell( file );

	if( ( seek_length & 0xfff ) != 0 ){
		/* the file size is not a multiple of 4KB, grow it */
		for( int i = 0; i < ( seek_length & 0xfff ); ++i )
			fprintf( file, "%s", "\0");
	}

	fseek( file, 0L, SEEK_END );
	seek_length = ftell( file );

	/* set up the available sector map */
	int nSectors = seek_length / SECTOR_LENGTH;
	sectorFree.resize( nSectors );
	std::fill( sectorFree.begin(), sectorFree.end(), true );

	sectorFree[0] = false; // chunk offset table

	fseek( file, 0L, SEEK_SET );

	for( int i = 0; i < 1024; ++i ){
		unsigned int offset;
		fscanf( file, "%d", &offset );
		offsets[i] = offset;
		if( offset != 0 && ( offset >> 8 ) + ( offset & 0xFF ) <= sectorFree.size() ){
			for( int sectorNum = 0; sectorNum < ( offset & 0xFF ); ++sectorNum ){
				sectorFree[ (offset >> 8 ) + sectorNum ] = false;
			}
		}
	}

}

RegionFile::~RegionFile( )
{
	fclose(file);
}


int RegionFile::getSizeDelta( )
{
	int ret = sizeDelta;
	sizeDelta = 0;
	return ret;
}

int RegionFile::getChunkDataInputStream( char** data, int x, int y )
{
	if( outOfBounds( x, y ) ){
		Debug::debug( Debug::MAP, "Read [%d:%d] out of bounds.\n", x, y );
		return -1;
	}

	int offset = getOffset( x, y );
	if( offset == 0 )
		return -1;

	unsigned int sectorNumber = offset >> 8;
	unsigned int numSectors = offset & 0xFF;

	if( sectorNumber + numSectors > sectorFree.size() ){
		Debug::debug( Debug::MAP, "Read [%d:%d] invalid sector.\n", x, y );
		return -1;
	}

	unsigned int length;
	fseek( file, sectorNumber * SECTOR_LENGTH, SEEK_SET );
	fscanf( file, "%d", &length );

	if( length > SECTOR_LENGTH * numSectors ){
		Debug::debug( Debug::MAP,
			"Read [%d:%d] invalid length: %d > SECTOR_LENGTH * %d.\n", x, y,
			length, numSectors );
		return -1;
	}

	int version;
	fscanf( file, "%d", &version );

	if( version == 1 ){
		(*data) = new char[length - 1];
		fread( (*data), length - 1, 1, file );
		return length - 1;
	}

	Debug::debug( Debug::MAP, "Read [%d:%d] File version %d not supported.\n",
				x, y, version);
	return -1;
}


RegionFile::ChunkBuffer* RegionFile::getChunkDataOutputStream( int x, int y )
{
	if( outOfBounds( x, y ) )
		return NULL;
	return new ChunkBuffer( *this, x, y );
}


/* write a chunk at (x,y) with length bytes of data to disk */
void RegionFile::write( int x, int y, char* data, int length )
{

	int offset = getOffset( x, y );
	int sectorNumber = offset >> 8;
	int sectorsAllocated = offset & 0xFF;
	int sectorsNeeded = ( length + CHUNK_HEADER_SIZE ) / SECTOR_LENGTH + 1;

	if( sectorsNeeded >= 256 ) // maximum chunk size is 1MB
		return;

	if( sectorNumber != 0 && sectorsAllocated == sectorsNeeded ){
		/* we can simply overwrite the old sectors */
		Debug::debug( Debug::MAP, "Save region %s [%d:%d] %d B = rewrite", fileName, x, y,
				length );
		write( sectorNumber, data, length );
	}else{
		/* we need to allocate new sectors */

		/* mark the sectors previously used for this chunk as free */
		for( int i = 0; i < sectorsAllocated; ++i )
			sectorFree.at( sectorNumber + i ) = true;

		/* scan for a free space large enough to store this chunk */
		int runStart = 0;
		while( !sectorFree[runStart] ){
			++runStart;
			if( runStart >= (int)sectorFree.size() ){
				runStart = -1;
				break;
			}
		}

		int runLength = 0;
		if( runStart != -1 ){
			for( unsigned int i = runStart; i < sectorFree.size(); ++i ){
				if( runLength != 0 ){
					if( sectorFree.at( i ) )
						runLength++;
					else
						runLength = 0;
				}else if( sectorFree.at( i ) ){
					runStart = i;
					runLength = 1;
				}
				if( runLength >= sectorsNeeded )
					break;
			}
		}

		if( runLength >= sectorsNeeded ){
			/* we found a free space large enough */
			Debug::debug( Debug::MAP, "Save region %s [%d:%d] %d B = reuse", fileName, x,
					y, length );
			sectorNumber = runStart;
			setOffset( x, y, ( sectorNumber << 8 ) | sectorsNeeded );
			for( int i = 0; i < sectorsNeeded; ++i )
				sectorFree[sectorNumber + i] = false;
			write( sectorNumber, data, length );
		}else{
			/* no free space large enough found -- we need to grow the file */
			Debug::debug( Debug::MAP, "Save region %s [%d:%d] %d B = grow", fileName, x,
					y, length );
			fseek( file, 0L, SEEK_END );
			sectorNumber = sectorFree.size();
			for( int i = 0; i < sectorsNeeded; ++i ){
				fwrite( emptySector, SECTOR_LENGTH, 1, file );
				sectorFree.push_back( false );
			}
			sizeDelta += SECTOR_LENGTH * sectorsNeeded;

			write( sectorNumber, data, length );
			setOffset( x, y, ( sectorNumber << 8 ) | sectorsNeeded );
		}
	}
}



/* write a chunk data to the region file at specified sector number */
void RegionFile::write(int sectorNumber, char* data, int length)
{
	fseek( file, sectorNumber * SECTOR_LENGTH, SEEK_SET );
	fprintf( file, "%d", length + 1 ); // chunk length
	fprintf( file, "%d", CHUNK_VERSION ); // chunk version number
	fwrite( data, length, 1, file ); // chunk data
}


/* is this an invalid chunk coordinate? */
bool RegionFile::outOfBounds( int x, int y )
{
	return x < 0 || x >= 32 || y < 0 || y >= 32;
}


int RegionFile::getOffset( int x, int y )
{
	return offsets[x + y * 32];
}


void RegionFile::setOffset( int x, int y, int offset )
{
	offsets[x + y * 32] = offset;
	fseek( file, (x + y * 32) * 4, SEEK_SET );
	fprintf( file, "%d", offset);
}
