#ifndef FRAME_FUNCTION_PANEL_H_
#define FRAME_FUNCTION_PANEL_H_

#include "Frame.h"

class FrameFunctionPanel : public Frame 
{
public:
	FrameFunctionPanel();
	
	FrameFunctionPanel(Frame * parent, int lx, int ly, int width, int height,string name);
	
	~FrameFunctionPanel();

	//virtual void drawScene();
	//virtual void onMouseFunc(int button,int state,int x,int y);
	//virtual void onMotionFunc(int x,int y);
};

#endif // FRAME_FUNCTION_PANEL_H_