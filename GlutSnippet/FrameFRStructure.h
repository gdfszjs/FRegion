#ifndef FRAME_FR_STRUCTURE_H_
#define FRAME_FR_STRUCTURE_H_

#include "Frame.h"

class FRNode;
class SVGElement;
class FRSTree;
class FRSNode;

class FrameFRStructure : public Frame 
{
public:
	FrameFRStructure								();
	
	FrameFRStructure								(Frame * parent, int lx, int ly, int width, int height,string name);
	
	~FrameFRStructure			   					();

	virtual void	drawScene	   					();
	virtual void	onMouseFunc	 					(int button,int state,int x,int y);
	virtual void	onMotionFunc					(int x,int y);
	virtual void	onSpecialKeyboardFunc			(int key,int x,int y);
	
	void			LoadFR		  					(const char * filename);
	void			SaveFRSTree						();

	void			NodeStickWhenAdd				(const std::vector<FRNode*> & all_nodes, FRNode * node);
	void			NodeStickWhenEdit				(const std::vector<FRNode*> & all_nodes, FRNode * node, int idx);
	double			FindNearestPointOnNodeToPoint	(v2d & near_pt, FRNode * & near_node, const std::vector<FRNode*> & nodes, const v2d & pt);
	void			AddNode							(FRNode * node);
	bool			ComputeControlPtIndexByMousePos	(int & idx, const FRNode & node, const v2d & pt);
	
	void			SetCurrentNodeAndElements		();

private:
	/* fr/FR stands for Fundamental Region 
		the purpose of this class is to:
		read a fr pattern, and create a FR structure tree based on the pattern */

	FRSTree *									frs_tree_;
	
	std::vector<FRNode*>						current_nodes_;
	std::vector<SVGElement*>					current_elements_;

	/* selection */
	std::vector<FRNode*>						selected_fr_nodes_;	
	std::vector<SVGElement*>					selected_elements_;

	/* add a line node */
	std::vector<v2d>							line_control_points_;	

	/* add a bezier node */
	std::vector<v2d>							bezier_control_points_;

	/* when edit node, we must remember the index of edited control point */
	int											edit_control_point_idx_;

	string										fregion_file_name_;	
	string										frstree_file_name_;
};

#endif // FRAME_FR_STRUCTURE_H_