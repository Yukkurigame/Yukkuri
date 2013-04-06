/*
 * RegionFile.h
 *
 *  Created on: 04.04.2013
 */

#ifndef REGIONFILE_H_
#define REGIONFILE_H_

/*
 * Region File Format
 * Based on McRegion ideas.
 *
 * blah-blah
 *
Concept: The minimum unit of storage on hard drives is 4KB. 90% of Minecraft
chunks are smaller than 4KB. 99% are smaller than 8KB. Write a simple
container to store chunks in single files in runs of 4KB sectors.

Each region file represents a 32x32 group of chunks. The conversion from
chunk number to region number is floor(coord / 32): a chunk at (30, -3)
would be in region (0, -1), and one at (70, -30) would be at (3, -1).
Region files are named "r.x.z.data", where x and z are the region coordinates.

A region file begins with a 4KB header that describes where chunks are stored
in the file. A 4-byte big-endian integer represents sector offsets and sector
counts. The chunk offset for a chunk (x, z) begins at byte 4*(x+z*32) in the
file. The bottom byte of the chunk offset indicates the number of sectors the
chunk takes up, and the top 3 bytes represent the sector number of the chunk.
Given a chunk offset o, the chunk data begins at byte 4096*(o/256) and takes up
at most 4096*(o%256) bytes. A chunk cannot exceed 1MB in size. If a chunk
offset is 0, the corresponding chunk is not stored in the region file.

Chunk data begins with a 4-byte big-endian integer representing the chunk data
length in bytes, not counting the length field. The length must be smaller than
4096 times the number of sectors. The next byte is a version field, to allow
backwards-compatible updates to how chunks are encoded.

A version of 1 represents a gzipped NBT file. The gzipped data is the chunk
length - 1.

A version of 2 represents a deflated (zlib compressed) NBT file. The deflated
data is the chunk length - 1.
 *
 */

#define CHUNK_HEADER_SIZE 8
#define CHUNK_VERSION 1
#define SECTOR_LENGTH 4096
#define SECTOR_SIZE (SECTOR_LENGTH-1)

#include <stdio.h>
#include <vector>


class RegionFile
{
public:
	class ChunkBuffer
	{
	public:
		ChunkBuffer( RegionFile& parent, int x, int y );
		void write( char* data, int length ){
			_file.write( _x, _y, data, length );
		}
	private:
		RegionFile& _file;
		int _x;
		int _y;
	};

	RegionFile( const char* );
	~RegionFile( );

	inline long lastModified() {
		return _lastModified;
	}

	int getSizeDelta( );
	int getChunkDataInputStream( char** data, int x, int y );
	ChunkBuffer* getChunkDataOutputStream( int x, int y );


protected:
	void write( int x, int y, char* data, int length );

private:
	void write( int sectorNumber, char* data, int length );
	bool outOfBounds( int x, int y );
	int getOffset( int x, int y );
	void setOffset( int x, int y, int offset );

	const char* fileName;
	FILE* file;
	int* offsets;
	std::vector<bool> sectorFree;
	int sizeDelta;
	long _lastModified;

	static char emptySector[SECTOR_LENGTH];
};

#endif /* REGIONFILE_H_ */
