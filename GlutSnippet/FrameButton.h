#ifndef FRAME_BUTTON_H_ 
#define FRAME_BUTTON_H_

#include "Frame.h"

class ButtonClick;

class FrameButton : public Frame
{
public:
	FrameButton();
	
	FrameButton(Frame * parent, int lx, int ly, int width, int height,string name, ButtonClick * pbc);
	
	~FrameButton();

	virtual void drawScene();
	virtual void onMouseFunc(int button,int state,int x,int y);
	virtual void onMotionFunc(int x,int y);

	virtual void onLoseFocus();
	
private:
	ButtonClick * pbc;
};

#endif // FRAME_BUTTON_h_