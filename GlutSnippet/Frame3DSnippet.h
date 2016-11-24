#pragma once

#include "Frame.h"


class Frame3DSnippet: public Frame {
public:
	Frame3DSnippet();
	
	Frame3DSnippet(Frame * parent, int lx, int ly, int width, int height,string name);
	
	~Frame3DSnippet();

	virtual void drawScene();
	//virtual void onMouseFunc(int button,int state,int x,int y);
	//virtual void onMotionFunc(int x,int y);
	
private:
};