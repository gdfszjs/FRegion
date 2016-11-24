#include "FrameChecker.h"
#include "CommonGl.h"

#include "CheckerClick.h"

FrameChecker::FrameChecker(Frame * parent, int lx, int ly, int width, int height,string name,
	int checker_color_r, int checker_color_g, int checker_color_b, CheckerClick * pcc) :
	Frame(parent,lx,ly,width,height,name) 
{
	checker_color_r_ = checker_color_r;
	checker_color_g_ = checker_color_g;
	checker_color_b_ = checker_color_b;
	status_ = false;
	square_width_ = width < height ? width : height;
	offset_ = square_width_ * 0.15;
	square_width_ -= offset_ * 2;
	c1_x_ = offset_;
	c1_y_ = c1_x_;
	c2_x_ = offset_ + square_width_;
	c2_y_ = c2_x_;
	
	pcc_ = pcc;
}

FrameChecker::FrameChecker() : Frame()
{
	checker_color_r_ = 0;
	checker_color_g_ = 0;
	checker_color_b_ = 255;
	status_ = false;	
	c1_x_ = 0;
	c1_y_ = 0;
	c2_x_ = 0;
	c2_y_ = 0;

	pcc_ = 0;
}

FrameChecker::~FrameChecker()
{
	if(pcc_ != 0) delete pcc_;
}

void FrameChecker::drawScene()
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
	gluOrtho2D(0,width,0,height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLineWidth(1);

	// draw square
	glColor3d(checker_color_r_/255.0,checker_color_g_/255.0,checker_color_b_/255.0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(c1_x_,c1_y_);
	glVertex2d(c1_x_,c2_y_);
	glVertex2d(c2_x_,c2_y_);
	glVertex2d(c2_x_,c1_y_);
	glEnd();

	// draw text
	writeText(GLUT_BITMAP_9_BY_15,
		(int)(c2_x_ + 5),(int)(height / 2 - 5),
		name.c_str(),	
		0,0,0,width,height);

	if(status_) 
	{
		glBegin(GL_LINE_STRIP);
		glVertex2d(c1_x_ + offset_, c1_y_ + square_width_ / 2);
		glVertex2d(c1_x_ + square_width_ / 2, c1_y_ + offset_);
		glVertex2d(c1_x_ + square_width_ - offset_, c1_y_ + square_width_ - offset_);
		glEnd();
	}
}

void FrameChecker::onMouseFunc(int button,int state,int x,int y)
{
	int lx, ly;
	globalPt2Local(lx,ly,x,y);
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			if(lx >= c1_x_ && 
				lx <= c2_x_ &&
				ly >= c1_y_ &&
				ly <= c2_y_)
			{
				status_ = !status_;
				this->OnStatusChange(status_);
			}
		}
	}
}

void FrameChecker::OnStatusChange(bool status)
{
	if(pcc_)
		pcc_->OnClick(status);
}

