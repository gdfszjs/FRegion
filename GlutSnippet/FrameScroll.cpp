#include "FrameScroll.h"


FrameScroll::FrameScroll() : Frame()
{
	this->bar_size_ = 0;
}

FrameScroll::FrameScroll(Frame * parent, int lx, int ly, int width, int height,string name) :
	Frame(parent,lx,ly,width,height,name) 
{
	this->bar_size_ = 0;
}

FrameScroll::~FrameScroll()
{
}

void FrameScroll::drawScene()
{

}

void FrameScroll::onMouseFunc(int button,int state,int x,int y)
{

}

void FrameScroll::onMotionFunc(int x,int y)
{

}
