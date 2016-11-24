#ifndef HEAT_PROCESSING_H_
#define HEAT_PROCESSING_H_

#include "CommonCV.h"
#include "Common.h"

class HeatProcessing
{
public:
	static void		ComputeImageRegionOfPoint				(IplImage * img, std::vector<v2i> & region, const v2i & pos);
	static void		ComputeImageRegionBoundaryInner			(IplImage * img, const std::vector<v2i> & region, std::vector<v2i> & boundary, std::vector<v3i> & color, std::vector<v2i> & inner);
	static double	ComputeDistanceBetweenPointAndPointSet	(const v2i & v, const std::vector<v2i> & v_set, v2i & nearest_v);
	static bool		FindFirstDifferentColorNeighbor			(IplImage * img, const v2i & v, v2i & neighbor);
};

#endif	// end of HEAT_PROCESSING_H_