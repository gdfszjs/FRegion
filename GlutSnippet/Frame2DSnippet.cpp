#include "Frame2DSnippet.h"
#include "CommonGL.h"

Frame2DSnippet::Frame2DSnippet() 
{
}

Frame2DSnippet::Frame2DSnippet(Frame * parent, int lx, int ly, int width, int height,string name)
	: Frame(parent,lx,ly,width,height,name) 
{
}

Frame2DSnippet::~Frame2DSnippet() 
{

}

void Frame2DSnippet::drawScene() {
	int gx,gy;
	_xyInGlobal(gx,gy);

	glEnable(GL_SCISSOR_TEST);
	glScissor(gx,gy,width,height);
	glClearColor(br/255.0f,bg/255.0f,bb/255.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	
	glViewport(gx,gy,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,width,0,height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex2d(0,0);
		glVertex2d(500,500);
	glEnd();
}