#include "Frame3DSnippet.h"
#include "CommonGl.h"

Frame3DSnippet::Frame3DSnippet() {
}

Frame3DSnippet::Frame3DSnippet(Frame * parent, int lx, int ly, int width, int height,string name)
	: Frame(parent,lx,ly,width,height,name) {
}

Frame3DSnippet::~Frame3DSnippet() {
}

void Frame3DSnippet::drawScene()
{
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
	gluPerspective(45,(double)width / (double) height,0.1,100);

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(10,10,10,
		0,0,0,
		0,1,0);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_LINES);
		glColor3d(0,0,1);
		glVertex3d(0,0,0);
		glVertex3d(0,1.5,0);
		glColor3d(0,1,0);
		glVertex3d(0,0,0);
		glVertex3d(1.5,0,0);
		glColor3d(1,0,0);
		glVertex3d(0,0,0);
		glVertex3d(0,0,1.5);

		/*glColor3d(0.6,0.6,0.6);
		glVertex3d(0,0,0);
		glVertex3d(0,-1.5,0);
		glVertex3d(0,0,0);
		glVertex3d(-1.5,0,0);
		glVertex3d(0,0,0);
		glVertex3d(0,0,-1.5);*/
	glEnd();
}