#ifndef FRAME_F_REGION_H_
#define FRAME_F_REGION_H_

#include "Frame.h"

class SVGPattern;
class SVGElement;
class ElementSeg;
class FRSTree;

class FrameFRegion : public Frame 
{
public:
	FrameFRegion();
	
	FrameFRegion(Frame * parent, int lx, int ly, int width, int height,string name);
	
	~FrameFRegion();

	virtual void drawScene();
	virtual void		onMouseFunc										(int button,int state,int x,int y);
	//virtual void onMotionFunc(int x,int y);
	virtual void		onKeyboardFunc									(unsigned char key, int x, int y);

	
	bool				IsPointOnLineSeg								(const v2d & v, const v2d & v1, const v2d & v2);	
	void				ComputeIntersectionBetweenLineLine				(std::vector<v2d> & inter, const v2d & v0, const v2d & v1, const v2d & v2, const v2d & v3);	
	void				ComputeIntersectionBetweenLineCubicBezier		(std::vector<v2d> & inters, const v2d & v0, const v2d & v1, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	
	SVGElement *		SplitElementByLine			(const SVGElement & e, const v2d & v0, const v2d & v1);
	
	bool				IsPointLeftLine				(const v2d & v, const v2d & v0, const v2d & v1);
	bool				IsPointRightLine			(const v2d & v, const v2d & v0, const v2d & v1);
	bool				IsSegInFR					(ElementSeg & s);
	
	SVGPattern *		ComputeFR					();
	
	void				RotateFR					(double theta,double cx, double cy);
	void				ResizeFRAngle				(double new_angle, double theta1, double theta2);
	void				SaveFR						(const char * filename);

	void				ComputeFRSTree				();

private:
	SVGPattern *				pattern_;
	string						fregion_file_name_;

	/*								p0_, theta2_
								-
							-
						-	
					-
			centre_---------------------p1_, theta1_*/
	v2d							centre_;
	v2d							p0_;
	v2d							p1_;
	double						theta2_radian_;
	double						theta1_radian_;

	FRSTree *					fregion_;
	
	/* User Interaction */
	int							theta_index_;
	
};

#endif // FRAME_F_REGION_H_