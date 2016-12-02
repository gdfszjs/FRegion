#ifndef FR_STRUCTURE_H_
#define FR_STRUCTURE_H_

#include "Common.h"
class FRSTree;

class FRSNode
{
public:
	FRSNode();
	~FRSNode();

	double				TotalBoundaryLength(FRSTree * tree);
	double				EndpointGlobalProportion(FRSTree * tree, int n, double t);
	double				FirstEndpointGlobalProportion(FRSTree * tree);
	double				SecondEndpointGlobalProportion(FRSTree * tree);
	void				Compute_n_and_t_ByGlobalProportion(FRSTree * tree, double p, int & n, double & t);
	void				codeFRSNode(FRSTree * tree, int treenum, int partnum);

	void				BinaryTreeTo1DArray(int * arr, int index);

	void				SampleNPoints(std::vector<v2d> & pts, int N, FRSTree * tree);
	void				SampleNPoints2(std::vector<v2d> & pts, int N, FRSTree * tree);

	std::vector<int>	frnode_indices_;  // the frnodes are stored in frstree, these are just indices
	std::vector<int>	element_indices_; // the elements are stored in other place, these are just indices
	int					partition_frnode_index_;
	int					n1_;			  // the curve (or line) index of the boundary
	double				t1_;			  // the position on the curve (or line)
	int					n2_;
	double				t2_;
	FRSNode *			left_child_;
	FRSNode *			right_child_;
	FRSNode *			parrent_;
};

class SVGPattern;
class FRNode;
class SVGElement;

class FRSTree
{
public:
	FRSTree();
	~FRSTree();

	void			GetCurrentFRNodes				(std::vector<FRNode*> & nodes);
	void			GetCurrentElements				(std::vector<SVGElement*> & eles);

	void			GetFRNodesFromFRSNode			(const FRSNode * node, std::vector<FRNode*> & nodes);
	void			GetElementsFromFRSNode			(const FRSNode * node, std::vector<SVGElement*> & eles);

	int				IndexOfNode						(FRNode * node);
	int				IndexOfElement					(SVGElement * element);

	void			DrawFRSNode						(const FRSNode * node, bool decompose, bool draw_ele, double hoffset = 100);
	void			Draw							(bool decompose = true, bool draw_ele = true);

	void			SaveFRSNode						(ofstream & outfile, const FRSNode * node);
	void			Save							(ofstream & outfile);
	void			LoadFRSNode						(ifstream & infile, FRSNode * node);
	void			Load							(ifstream & infile);

	bool			IsElementInFRSNode				(FRSNode * frs_node, SVGElement * ele);
	void			SplitFRNode						(FRNode *& n1, FRNode *& n2, double & t, FRNode * node, const v2d & pt);
	void			SplitFRNode						(FRNode *& n1, FRNode *& n2, FRNode *& n3, double & t1, double & t2, FRNode * node, const v2d & p0, const v2d & p1);
	void			SplitFRSNodeOnIdenticalFRNode	(FRSNode * frs_node, FRNode * node, int idx);
	void			SplitFRSNodeOnDifferentFRNode	(FRSNode * frs_node, FRNode * node, int left_idx, int right_idx);
	void			SplitFRSNode					(FRSNode * frs_node, FRNode * node);
	void			SplitFRSNode					(FRSNode * frs_node, v6d curve);

	/* ExtractHighDimensionalData:
		every partition curve is a 6-dimensional data 
		but they belong to different hierarchical level
		By tranversing the tree structure, each partition 
		curve is stored into a txt file according to its
		places in the whole structure */
	void			ExtractHighDimensionalData					(FRSNode * node, string str, int tree_no);
	/* write all the partition curves into a line */
	void			ExtractHighDimensionalData					(FRSNode * node, string filepath);
	void			ExtractHighDimensionalDataBreadthFirst		(string filepath);
	void			ExtractHighDimensionalDataBreadthFirst		(std::vector<double> & hhdata);

	/* get all leaf nodes and string ids */
	void			GetLeafNodesAndStringIds					(FRSNode * frs_node, string cur_str_id, std::vector<FRSNode *> & leaf_nodes, std::vector<string> & str_ids);
	void			GetLeafNodesAndArrayIndices					(std::vector<FRSNode *> & leaf_nodes, std::vector<int> & indices);
	FRSNode*		GetLeafNodeByIndex							(int code);

	/* get frs_node by name */
	void			GetFRSNodeByName							(FRSNode * frs_node, string cur_name, string name, FRSNode *& ans);

	void			BinaryTreeTo1DArray							(int * arr);
	FRSNode*		FindParentFRSNodeByArrayIndex				(int n);
	FRSNode*		FindFRSNodeByArrayIndex						(int n);
	
	void			set_fr_angle								(double a)	{ fr_angle_ = a; }
	double			fr_angle									()			{ return fr_angle_; }

	bool			CompareWithFRSTree							(FRSTree * e);

	SVGPattern *			pattern_;
	std::vector<FRNode*>	all_frnodes_;		// frnode is line or beizer
	FRSNode *				frs_root_node_;
	double					fr_angle_;

	FRSNode *				current_node_;

	std::vector<int>					correc_indec;		//save the most possible candidate of all the FRSNode
	std::vector<std::vector<int>>		com_index_order;	//save all the possible candidate


	// static function members
	static double			BestAngle(double angle);
};
#endif // FR_STRUCTURE_H_