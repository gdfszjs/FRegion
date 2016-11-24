#include "GLUTMain.h"
#include "CommonGl.h"
#include "Frame.h"

Frame::Frame()
{
	parentFrame = 0;
	childFrames.clear();

	x = y = width = height = 0;

	isLeftButtonDown = isRightButtonDown = isMiddleButtonDown = false;
	mouseOx = mouseOy = mouseTranx = mouseTrany = mouseOrgX = mouseOrgY = 0;

	br = bb = bg = alpha = 0;
	name = "Frame";
}

Frame::Frame(Frame * parent, int lx, int ly, int width, int height,string name)
{
	parentFrame = parent;
	childFrames.clear();

	x = lx;
	y = ly;
	this->width = width;
	this->height = height;

	isLeftButtonDown = isRightButtonDown = isMiddleButtonDown = false;
	mouseOx = mouseOy = mouseTranx = mouseTrany = mouseOrgX = mouseOrgY = 0;

	br = bb = bg = alpha = 0;

	this->name = name;
}

Frame::~Frame()
{
	for(size_t i = 0; i<childFrames.size();i++)
		if(childFrames[i])
			delete childFrames[i];
	childFrames.clear();
}

void Frame::drawScene()
{
	int gx, gy;
	_xyInGlobal(gx,gy);

	glEnable(GL_SCISSOR_TEST);
	glScissor(gx,gy,width,height);
	glClearColor(br/255.0f,bg/255.0f,bb/255.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	glViewport(gx,gy,width,height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(gx,width,gy,height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	/*if(GLUTMain::Instance()->getFocus())
		writeText(GLUT_BITMAP_9_BY_15,0,0,
		GLUTMain::Instance()->getFocus()->getName().c_str(),
		255,255,255,
		GLUTMain::Instance()->WinX(),
		GLUTMain::Instance()->WinY());*/
	
	for(size_t i = 0;i<childFrames.size();i++)
		if(childFrames[i])
			childFrames[i]->drawScene();
}

void Frame::onMouseWheelFunc(int wheel, int direction, int x, int y)
{

}

void Frame::onMouseFunc(int button,int state,int x,int y)
{
}

void Frame::onMotionFunc(int x,int y)
{

}

void Frame::onPassiveMotionFunc(int x, int y)
{

}

void Frame::setFocusFrame(int gx, int gy)
{
	if(isPointInFrame(gx,gy))
	{
		GLUTMain::Instance()->setFocus(this);

		for(size_t i = 0;i<childFrames.size();i++)
			if(childFrames[i]) childFrames[i]->setFocusFrame(gx,gy);
	}
}

Frame * Frame::getFocusFrame(int gx, int gy)
{
	if(!isPointInFrame(gx,gy))
		return NULL;

	Frame * ans = NULL;
	for(size_t i = 0;i<childFrames.size(); i++)
	{
		ans = childFrames[i]->getFocusFrame(gx,gy);
		if(ans != NULL) break;
	}
	
	if(ans != NULL) return ans;
	else return this;
}