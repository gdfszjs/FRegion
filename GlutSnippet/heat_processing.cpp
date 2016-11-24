#include "heat_processing.h"

void HeatProcessing::ComputeImageRegionOfPoint(IplImage * img, std::vector<v2i> & region, const v2i & pos)
{
	if(pos[0] < 0 || pos[0] >= img->width || pos[1] < 0 || pos[1] >= img->height) return;

	int r,g,b;
	LibIV::CppHelpers::Global::imgData(r,g,b,img,pos[0],pos[1]);

	// breadth first 
	std::vector<v2i> sstak;

	LibIV::Memory::Array::FastArray2D<int> mask;
	mask.set(img->width,img->height);
	mask.fill(0);
	
	sstak.push_back(pos);
	mask.at(pos[1],pos[0]) = 1;

	int pointer1 = 0;
	while(pointer1 < (int)(sstak.size()))
	{
		v2i v = sstak[pointer1];

		int rr,gg,bb;
		LibIV::CppHelpers::Global::imgData(rr,gg,bb,img,v[0],v[1]);

		if(r == rr && g == gg && b == bb)
		{
			region.push_back(v);

			if(v[0] >= 1 && mask.at(v[1],v[0]-1) != 1)
			{
				sstak.push_back(_v2i_(v[0]-1,v[1]));
				mask.at(v[1],v[0]-1) = 1;
			}
			if(v[0] + 1 < img->width && mask.at(v[1],v[0]+1) != 1)
			{
				sstak.push_back(_v2i_(v[0]+1,v[1]));
				mask.at(v[1],v[0]+1) = 1;
			}
			if(v[1] >= 1 && mask.at(v[1]-1,v[0]) != 1)
			{
				sstak.push_back(_v2i_(v[0],v[1]-1));
				mask.at(v[1]-1,v[0]) = 1;
			}
			if(v[1] + 1 < img->height && mask.at(v[1]+1,v[0]) != 1)
			{
				sstak.push_back(_v2i_(v[0],v[1]+1));
				mask.at(v[1] + 1, v[0]) = 1;
			}
			if(v[0] >= 1 && v[1] >= 1 && mask.at(v[1]-1,v[0]-1) != 1)
			{
				sstak.push_back(_v2i_(v[0]-1,v[1]-1));
				mask.at(v[1]-1,v[0]-1) = 1;
			}
			if(v[0] >= 1 && v[1] + 1 < img->height && mask.at(v[1]+1,v[0]-1) != 1)
			{
				sstak.push_back(_v2i_(v[0]-1,v[1]+1));
				mask.at(v[1]+1,v[0]-1) = 1;
			}
			if(v[0] + 1 < img->width && v[1] >= 1 && mask.at(v[1]-1,v[0]+1) != 1)
			{
				sstak.push_back(_v2i_(v[0]+1,v[1]-1));
				mask.at(v[1]-1,v[0]+1) = 1;
			}
			if(v[0] + 1 < img->width && v[1] + 1 < img->height && mask.at(v[1]+1,v[0]+1) != 1)
			{
				sstak.push_back(_v2i_(v[0]+1,v[1]+1));
				mask.at(v[1]+1,v[0]+1) = 1;
			}
		}
		
		pointer1++;
	}
}

void HeatProcessing::ComputeImageRegionBoundaryInner(IplImage * img, const std::vector<v2i> & region, std::vector<v2i> & boundary, std::vector<v3i> & color, std::vector<v2i> & inner)
{
	int r,g,b;
	v2i v = region[0];
	LibIV::CppHelpers::Global::imgData(r,g,b,img,v[0],v[1]);

	for(size_t i = 0;i<region.size();i++)
	{
		v = region[i];

		std::vector<v2i> neighbors;
		neighbors.push_back(_v2i_(v[0]-1,v[1]-1));
		neighbors.push_back(_v2i_(v[0]  ,v[1]-1));
		neighbors.push_back(_v2i_(v[0]+1,v[1]-1));

		neighbors.push_back(_v2i_(v[0]-1,v[1]));
		neighbors.push_back(_v2i_(v[0]  ,v[1]));
		neighbors.push_back(_v2i_(v[0]+1,v[1]));

		neighbors.push_back(_v2i_(v[0]-1,v[1]+1));
		neighbors.push_back(_v2i_(v[0]  ,v[1]+1));
		neighbors.push_back(_v2i_(v[0]+1,v[1]+1));

		bool flag_boundary = false;
		for(size_t j = 0;j<neighbors.size();j++)
		{
			v2i vv = neighbors[j];
			if(vv[0] >= 0 && vv[0] < img->width && vv[1] >= 0 && vv[1] < img->height)
			{
				int rr,gg,bb;
				LibIV::CppHelpers::Global::imgData(rr,gg,bb,img,vv[0],vv[1]);

				if(rr != r || gg != g || bb != b)
				{
					boundary.push_back(v);
					color.push_back(_v3i_(rr,gg,bb));
					flag_boundary = true;
					break;
				}
			}
		}

		if(!flag_boundary)
			inner.push_back(v);
	}
}

double HeatProcessing::ComputeDistanceBetweenPointAndPointSet(const v2i & v, const std::vector<v2i> & v_set, v2i & nearest_v)
{
	double min_dist = 1e20;

	for(size_t i = 0;i<v_set.size();i++)
	{
		v2i v0 = v_set[i];

		double x = v[0] - v0[0];
		double y = v[1] - v0[1];

		double dd = sqrt(x*x + y*y);

		if(dd < min_dist)
		{
			min_dist = dd;
			nearest_v = v0;
		}
	}
	
	return min_dist;
}

bool HeatProcessing::FindFirstDifferentColorNeighbor(IplImage * img, const v2i & v, v2i & neighbor)
{
	bool flag_inner_boundary = false;

	int r,g,b;
	LibIV::CppHelpers::Global::imgData(r,g,b,img,v[0],v[1]);
	
	std::vector<v2i> neighbors;

	neighbors.push_back(_v2i_(v[0]-1, v[1]-1));
	neighbors.push_back(_v2i_(v[0]-0, v[1]-1));
	neighbors.push_back(_v2i_(v[0]+1, v[1]-1));
	
	neighbors.push_back(_v2i_(v[0]-1, v[1]-0));
	neighbors.push_back(_v2i_(v[0]-0, v[1]-0));
	neighbors.push_back(_v2i_(v[0]+1, v[1]-0));

	neighbors.push_back(_v2i_(v[0]-1, v[1]+1));
	neighbors.push_back(_v2i_(v[0]-0, v[1]+1));
	neighbors.push_back(_v2i_(v[0]+1, v[1]+1));

	for(size_t i = 0;i<neighbors.size();i++)
	{
		v2i p = neighbors[i];

		if(p[0] >= 0 && p[0] < img->width &&
			p[1] >= 0 && p[1] < img->height)
		{
			int rr,gg,bb;
			LibIV::CppHelpers::Global::imgData(rr,gg,bb,img,p[0],p[1]);

			if(rr!=r || gg!=g || bb!=b)
			{
				neighbor = p;
				flag_inner_boundary = true;
				break;
			}
		}
	}
	return flag_inner_boundary;
}
