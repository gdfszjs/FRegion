#ifndef GRID_4_CONNECTED_UNDIRECT_MRF_H_
#define	GRID_4_CONNECTED_UNDIRECT_MRF_H_

/* 
	2015.05.13	Caculate the best elements for the sructures of the manifold,
		such that the FRSTrees of adjacent structures have similar patterns
		
		For that, build a MRF for the 2D manifold (this is a temporary strategy)
		each pixel (or patch) of the manifold is a MRF node
		Each node which represents a structure can be assigned with existing elements, assigning
		the leaf FRSNode one by one
		For a MRF node, all possible element assignment can be listed
		We compute the similarity between the assigned elements, i.e., the compatibility 
		We aslo compute the local evidence for the assignment itself to the MRF structure
*/

#include "BpKernel.h"

typedef LibIV::Memory::Array::FastArray2D<BpNode*> GridMRF;

class Grid4ConnectedUndirectMRF
{
public:
	Grid4ConnectedUndirectMRF();
	~Grid4ConnectedUndirectMRF();
	

	//void Initialize(int manifold_x, int manifold_y, int grid_size, int frs_region_num, int example_num);
	
	// labels are the possible objects assigned to a MRF node
	// w and h are the width and height of the MRF
	void Initialize(int w, int h, int labels);
	void Free();

	void SetLocalEvidence();
	void SetLabelSimilarity();
	void ComputeBeliefForMRF();
	


	//void ShowSomething();
	
private:
	GridMRF											mrf_;
	LabelSimilarity									ls_;
	//LibIV::Memory::Array::FastArray2D<v2d>		MRF_node_pos_;


}; // end of BpElementFill

#endif // GRID_4_CONNECTED_UNDIRECT_MRF_H_