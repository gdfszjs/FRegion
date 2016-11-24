#ifndef FRAME_MANIFOLD_H_
#define FRAME_MANIFOLD_H_

#include "Frame.h"
#include "CommonGl.h"
#include <LibIV\libiv.h>

class FRSTree;

class FrameManifold : public Frame 
{
public:
	FrameManifold		();
	
	FrameManifold		(Frame * parent, int lx, int ly, int width, int height,string name, 
		string manifold_filename, string var_filename, string anchor_point_filename);
	
	~FrameManifold		();

	virtual void			drawScene						();
	virtual void			onMouseFunc						(int button,int state,int x,int y);
	virtual void			onMotionFunc					(int x,int y);
	virtual void			onSpecialKeyboardFunc			(int key, int x, int y);

	void					LoadManifold					(string manifold_filename, string var_filename, string anchor_point_filename);
	std::vector<double>		GetManifoldValue				();
	std::vector<double>		GetManifoldValue				(int x, int y);
	void					DrawFR2Circle					(FRSTree * tree, bool draw_structure = true, bool draw_ele = true, bool draw_reflection = true);

	void					ComputeBoundaryAndNeighbor		();

	void					GetOrderedFRSTrees				();
	void					GetOrderedIndex					();
	
	int						dim_x()							{return dim_x_;}
	int						dim_y()							{return dim_y_;}
	void					set_manifold_x(int x)			{manifold_x_ = x;}
	void					set_manifold_y(int y)			{manifold_y_ = y;}
	int						manifold_x()					{return manifold_x_;}
	int						manifold_y()					{return manifold_y_;}			
	double					variance(int x, int y)			{return variance_.at(y,x);}
	v2i						nearest_neighbor()				{return nearest_neighbor_;}
	
private:
	std::vector<std::vector<double>>			gplvm_manifold_;		// begining modifying at this point
	std::vector<v2d>							gplvm_anchor_pts_;
	IplImage *									variance_fig_;			// show the variance
	LibIV::Memory::Array::TensorDb				variance_;
	
	int											dim_x_;					// the resolution of manifold
	int											dim_y_;

	int											offset_x_;
	int											offset_y_;

	int											manifold_x_;			// current cursor position on the manifold
	int											manifold_y_;

	// 2015.05.21	interactions on heat map 
	//			user indicate point, find the boundary
	std::vector<v2i>							boundary_;
	v2i											nearest_neighbor_;

	std::vector<std::vector<FRSTree*>> center_FRSTree;
	std::vector<std::vector<std::vector<int>>> all_select_index;
	std::vector<std::vector<v2i>> center_position_FRSTree;
	std::vector<std::vector<std::vector<int>>> correc_index_order;
	int											draw_flag = 0;				//if flag == 1,means to draw the selected point;if flag == 0,means not to draw the selected point;
};

#endif // FRAME_MANIFOLD_H_