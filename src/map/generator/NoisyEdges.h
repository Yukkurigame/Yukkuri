/*
 * NoisyEdges.h
 *
 *  Created on: 08.03.2013
 */

#ifndef NOISYEDGES_H_
#define NOISYEDGES_H_

#include "utils/list.h"
#include "basic_types.h"
#include "map/generator/graph/Center.h"
#include "map/generator/fifth-party/PMPRNG.h"
#include <vector>
#include <map>


class NoisyEdges
{
public:
	NoisyEdges( );
	~NoisyEdges( );

	static const float NOISY_LINE_TRADEOFF = 0.5;  // low: jagged vedge; high: jagged dedge

	std::map< int, list< s2f >* > path[2]; // edge index -> Vector.<Point>

	// Build noisy line paths for each of the Voronoi edges. There are
	// two noisy line paths for each edge, each covering half the
	// distance: path0 is from v0 to the midpoint and path1 is from v1
	// to the midpoint. When drawing the polygons, one or the other
	// must be drawn in reverse order.
	void buildNoisyEdges( std::vector< Center* >& centers, /*Lava lava,*/PM_PRNG* random );
	// Helper function: build a single noisy line in a quadrilateral A-B-C-D,
	// and store the output points in a Vector.
	static list< s2f >* buildNoisyLineSegments( PM_PRNG* random, const s2f* A,
			const s2f* B, const s2f* C, const s2f* D, float minLength, bool reverse = false );

	inline bool processed()
	{
		return _processed;
	}

private:
	bool _processed;
};

#endif /* NOISYEDGES_H_ */
