#include "GLUTMain.h"
#include "FrameButton.h"
#include "ButtonClick.h"

#include "CommonGl.h"
#include "Common.h"

FrameButton::FrameButton() : Frame()
{
	pbc = 0;
}

FrameButton::FrameButton(Frame * parent, int lx, int ly, int width, int height,string name, ButtonClick * pbc) : 
	Frame(parent,lx,ly,width,height,name) 
{
	br = BUTTONUPR;
	bg = BUTTONUPG;
	bb = BUTTONUPB;
	this->pbc = pbc;
}

FrameButton::~FrameButton()
{
	// cout<<"Destructor - FrameButton - "<<name<<endl;
	if(pbc) delete pbc;
}
void FrameButton::drawScene()
{
	int gx, gy;
	_xyInGlobal(gx,gy);

	glEnable(GL_SCISSOR_TEST);
	glScissor(gx,gy,width,height);
	glClearColor(br/255.0f,bg/255.0f,bb/255.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	
	glViewport(gx,gy,width,height);

	writeText(GLUT_BITMAP_9_BY_15,
		0,20,
		name.c_str(),
		255,255,255,width,height);
}

void FrameButton::onMouseFunc(int button,int state,int x,int y)
{
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_UP)
		{
			br = BUTTONUPR;
			bg = BUTTONUPG;
			bb = BUTTONUPB;

			if(pbc) pbc->onClick();
		}
		else if(state == GLUT_DOWN)
		{
			br = BUTTONDOWNR;
			bg = BUTTONDOWNG;
			bb = BUTTONDOWNB;
		}
	}
}

void FrameButton::onMotionFunc(int x,int y)
{
	br = BUTTONDOWNR;
	bg = BUTTONDOWNG;
	bb = BUTTONDOWNB;
}

void FrameButton::onLoseFocus()
{
	br = BUTTONUPR;
	bg = BUTTONUPG;
	bb = BUTTONUPB;
}