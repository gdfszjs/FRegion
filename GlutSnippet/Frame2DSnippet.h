#ifndef FRAME_2D_SNIPPET_H_
#define FRAME_2D_SNIPPET_H_

#include "Frame.h"

class Frame2DSnippet : public Frame 
{
public:
	Frame2DSnippet		();
	
	Frame2DSnippet		(Frame * parent, int lx, int ly, int width, int height,string name);
	
	~Frame2DSnippet		();

	virtual void	drawScene			();
	//virtual void onMouseFunc(int button,int state,int x,int y);
	//virtual void onMotionFunc(int x,int y);
	
private:
};

#endif // FRAME_2D_SNIPPET_H_