#ifndef FRAME_CHECKER_H_
#define FRAME_CHECKER_H_

#include "Frame.h"

class CheckerClick;

class FrameChecker : public Frame
{
public:
	FrameChecker();
	
	FrameChecker(Frame * parent, int lx, int ly, int width, int height,string name,
		int checker_color_r, int checker_color_g, int checker_color_b, CheckerClick * pcc = 0);
	
	~FrameChecker();

	virtual void drawScene();
	virtual void onMouseFunc(int button,int state,int x,int y);

	void OnStatusChange(bool status);
	void UnCheck() { status_ = false; OnStatusChange(status_); }
	
	bool getStatus() { return status_; }
private:
	int checker_color_r_;
	int checker_color_g_;
	int checker_color_b_;

	bool status_;	
	double c1_x_, c1_y_, c2_x_, c2_y_;	
	double square_width_;
	double offset_;

	CheckerClick * pcc_;
};


#endif // FRAME_CHECKER_H_