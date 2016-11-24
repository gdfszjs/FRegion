#ifndef FRAME_MEDIAL_AXIS_H_
#define FRAME_MEDIAL_AXIS_H_

#include "Frame.h"

class SVGPattern;
class SVGElement;

class FrameMedialAxis : public Frame 
{
public:
	FrameMedialAxis			();
	FrameMedialAxis			(Frame * parent, int lx, int ly, int width, int height,string name);
	~FrameMedialAxis		();

	virtual void drawScene	();
	//virtual void onMouseFunc(int button,int state,int x,int y);
	//virtual void onMotionFunc(int x,int y);


	/* Functions for computing medial axis */
	void NearestBetweenPointAndElement			(double & d, v2d & np, const v2d & sp, SVGElement* e);
	void DrawCircle								(const v2d & center, double r);
	bool IsPointOnMedialAxis					(const v2d & p, const std::vector<SVGElement*> & ee); 
	void FindMedialAxis							(std::vector<v2d> & ma, SVGPattern * pattern);
	
private:
	SVGPattern *				pattern_;
	v2d							centre_;
	std::vector<v2d>			medial_axis_;
};

#endif // FRAME_MEDIAL_AXIS_H_