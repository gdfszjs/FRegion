#ifndef FRAME_STRUCTURE_BLEND_H_
#define FRAME_STRUCTURE_BLEND_H_

#include "Frame.h"
#include <vector>

class	SVGPattern;
class	SVGElement;
class	FRNode;
struct	triangulateio;
class	FRSTree;
class	FRSNode;


namespace rmsmesh
{
	class TriangleMesh;
}

class FrameStructureBlend : public Frame 
{
public:
	FrameStructureBlend								();
	FrameStructureBlend								(Frame * parent, int lx, int ly, int width, int height,string name);
	~FrameStructureBlend							();

	virtual void	drawScene						();
	//virtual void	onMouseFunc						(int button,int state,int x,int y);
	//virtual void	onMotionFunc					(int x,int y);
	virtual void	onSpecialKeyboardFunc			(int key,int x,int y);

	FRSTree*		LoadFRSTree						(char * frs_file_name);

	// this function is crucial, as it saves all the high-dimensional data
	void			ExtractHighDimData				();
	
	void			CompleteFRSTree					();
	
	// 2015.05.19	functions for comparing FRSTree and FRSNode
	void			ResizeFRSNodeBoundaryPointsByAngle		(double new_angle, double old_angle, v2d centre, const std::vector<v2d> & src_points, std::vector<v2d> & dst_points);	
	void			SamplePointsOfFRSNodeBoundary			(FRSTree * tree, FRSNode * node, std::vector<v2d> & points, int num);
	void			SamplePointsOfFRSNodeBoundaryBySeg		(FRSTree * tree, FRSNode * node, std::vector<v2d> & points);
	double			DistanceBetweenTwoPointSets				(const std::vector<v2d> & pts1, const std::vector<v2d> & pts2);
	
	// 2015.05.19	functions for deforming FRSTree
	void			DeformFRSNode					(FRSTree * src_tree, FRSNode * src_node, FRSTree * dst_tree, FRSNode * dst_node, std::vector<SVGElement*> & eles);
	void			DeformElements					(std::vector<SVGElement*> & deformed_elements, const std::vector<SVGElement*> & src_elements, const rmsmesh::TriangleMesh * src_mesh, const rmsmesh::TriangleMesh * dst_mesh);
	v2d				DeformPoint						(const v2d & v, const rmsmesh::TriangleMesh * src_mesh, const rmsmesh::TriangleMesh * dst_mesh);

	// 2015.05.19	functions for showing, drawing and testing
	void			DrawRmsTriangleMesh 			(const rmsmesh::TriangleMesh & mesh);
	void			ShowRmsTriangleMeshInImage		(const rmsmesh::TriangleMesh & mesh,char * filename);
	void			ShowPointsInImage				(const std::vector<v2d> & pts1, const std::vector<v2d> & pts2);
	void			ShowFRSNodeInImage				(char * filename, FRSTree * tree, int frs_node_code, bool flag_show_ele = false);

	void			BuildFRSTreeforfiletest					(ofstream  & outfile);//to test the vector and the number of the structure;just for test!
	vector<int>		BuildFRSTreeforseletindex				();
	void			BuildFRSTree							();
	void			BuildFRSTreeTest						();
	void			BuildTreeByCode							();

	
	FRSTree*		BuildTreeStructureForManifoldPosition	(int x, int y);
	void			ComputeMostSimilarCandidatesForTree		(FRSTree * tree, std::vector<int> & corre);
	void			DeformCandidates						(FRSTree * tree,const std::vector<int> & corre); 
	FRSTree*		BuildFRSTreeShell						(double shell_angle, double shell_radius, v2d shell_centre);
	void			SubdivideFRSTreeShell					(FRSTree * tree, std::vector<double> hddata);

	// 2015.05.17	compute heat map for the element filling
	//	the pixels of the manifold have the same heat if they are filled by the same elements
	//	heat is important: used for computing blending alpha of manifold position
	void			ComputeHeatMapForElementFilling			();
	v3i				Candidate2Color							(const std::vector<int> & cand);

	void			LoadManifoldSimilarCode					();
	void			LoadAlpha								();

	
	void			BlendFRSTree							();
	SVGElement*		BlendTwoElements						(const SVGElement * ee ,const SVGElement * ff, double alpha);
	SVGElement*		ChangeElementLineToBezier				(const SVGElement * e);

	
	void			RefreshData								();

	
// =====================================================================================================

	/* FRS examples */
	std::vector<FRSTree*>					all_trees_;
	int										tree_index_;
	std::vector<FRSNode*>					tree_frsnodes_;
	std::vector<std::vector<v2d>>			tree_frsnode_boundary_points_;
	std::vector<v2i>						tree_frsnode_codes_; // [0] is the index of tree, and [1] is the code of frsnode

	LibIV::Memory::Array::FastArray2D<v4i>		manifold_similar_codes_;
	LibIV::Memory::Array::FastArray2D<double>	alpha_;	
		
	/* a generated FRS */	
	FRSTree *								gen_frs_tree_;

	// 2015.05.19	code of FRSNode
	int										arr[20];

};

#endif // FRAME_STRUCTURE_BLEND_H_