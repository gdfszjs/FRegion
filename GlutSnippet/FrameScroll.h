#ifndef FRAME_SCROLL_H_
#define FRAME_SCROLL_H_

#include "Frame.h"

class FrameScroll : public Frame
{
public:
	FrameScroll();
	
	FrameScroll(Frame * parent, int lx, int ly, int width, int height,string name);
	
	~FrameScroll();

	virtual void drawScene();
	virtual void onMouseFunc(int button,int state,int x,int y);
	virtual void onMotionFunc(int x,int y);

private:
	int bar_size_; 
	
};



#endif // FRAME_SCROLL_H_