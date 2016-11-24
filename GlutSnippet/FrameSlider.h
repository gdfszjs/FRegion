#pragma once

#include "Frame.h"
#include "SliderClick.h"

template<class T> 
class FrameSlider : public Frame
{
public:
	FrameSlider();
	
	FrameSlider(Frame * parent, int lx, int ly, int width, int height,string name,string caption,
		int slider_color_r, int slider_color_g, int slider_color_b,
		T min_value, T max_value, T curr_value, SliderClick * psc = 0);
	
	~FrameSlider();

	virtual void drawScene();
	virtual void onMouseFunc(int button,int state,int x,int y);
	virtual void onMotionFunc(int x,int y);
	
	T getValue() { return curr_value_; }
	void SetMinValue(T val)  { this->min_value_ = val; }
	void SetMaxValue(T val)  { this->max_value_ = val; }
	void SetCurrValue(T val) { this->curr_value_ = val; this->curr_pos_x_ = value2pos(); }
	
protected:
	T pos2value()
	{
		if(sizeof(T) == 4)
			curr_value_ = (T)(curr_pos_x_ * (max_value_ - min_value_) / (width - 10) + min_value_ + 0.5);
		else 
			curr_value_ = (T)(curr_pos_x_ * (max_value_ - min_value_) / (width - 10) + min_value_);
		return curr_value_;
	}
	double value2pos()
	{
		if(max_value_ < min_value_)
			max_value_ = min_value_;
		if(max_value_ == min_value_)
			curr_pos_x_ = (width - 10) / 2;
		else 
			curr_pos_x_ = (double)(curr_value_-min_value_) * (double)(width - 10) / (double)(max_value_ - min_value_);
		return curr_pos_x_;
	}

private:
	int slider_color_r_;
	int slider_color_g_;
	int slider_color_b_;

	T min_value_;
	T max_value_;
	T curr_value_;
	double curr_pos_x_;

	string caption_;

	SliderClick * psc_;
};


template<class T>
FrameSlider<T>::FrameSlider(Frame * parent, int lx, int ly, int width, int height,string name, string caption,
	int slider_color_r, int slider_color_g, int slider_color_b,
	T min_value, T max_value, T curr_value, SliderClick * psc):
	Frame(parent,lx,ly,width,height,name) 
{
	caption_ = caption;

	min_value_ = min_value;
	max_value_ = max_value;
	if(min_value_ >= max_value_)
		max_value_ = min_value_;
	
	curr_value_ = curr_value;
	if(curr_value_ < min_value_ || curr_value_ > max_value_)
		curr_value_ = min_value_;

	curr_pos_x_ = value2pos();
	slider_color_r_ = slider_color_r;
	slider_color_g_ = slider_color_g;
	slider_color_b_ = slider_color_b;

	this->psc_ = psc;
}

template<class T>
FrameSlider<T>::FrameSlider() : Frame()
{
	slider_color_r_ = 0;
	slider_color_g_ = 0;
	slider_color_b_ = 255;
	min_value_ = 0;
	max_value_ = 0;
	curr_value_ = 0;
	curr_pos_x_ = 0;
	caption_ = "";
	this->psc_ = 0;
}

template<class T>
FrameSlider<T>::~FrameSlider()
{
	if(this->psc_ != 0) delete this->psc_;
}

template<class T>
void FrameSlider<T>::drawScene()
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

	// draw slider
	glColor3d(slider_color_r_/255.0,slider_color_g_/255.0,slider_color_b_/255.0);
	glBegin(GL_TRIANGLES);
	glVertex2i(5,5);
	glVertex2i(width - 5, 5);
	glVertex2i(width - 5, height - 5);
	glEnd();

	// draw text
	char buf[1024];
	sprintf_s(buf,1024,caption_.c_str(),curr_value_);

	writeText(GLUT_BITMAP_9_BY_15,
		5,height-13,
		buf,	
		0,0,0,width,height);

	// draw circle 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(208.0/255,72.0/255,65.0/255,0.8);
	double pos_x = 5 + curr_pos_x_;
	double pos_y = height / 2.0;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(pos_x,pos_y);
	for(int i = 0;i<360;i++)
	{
		double theta = 3.14159 * 2 / 360 * i;
		double x = pos_x + (height-2) / 2 * cos(theta);
		double y = pos_y + (height-2) / 2 * sin(theta);
		glVertex2d(x,y);
	}
	glEnd();
	glDisable(GL_BLEND);
}

template<class T>
void FrameSlider<T>::onMouseFunc(int button,int state,int x,int y)
{
	int lx,ly;
	globalPt2Local(lx,ly,x,y);
	
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			if(lx >= 5 && lx <= width - 5)
			{
				double r = (height - 2) / 2;
				double pos_x = 	5 + curr_pos_x_;
				double pos_y = height / 2;
				double dx = pos_x - lx;
				double dy = pos_y - ly;
				double d = sqrt(dx * dx + dy * dy);
				bool in_circle_flag = d < r ? true : false;		
				if(in_circle_flag)
				{
					isLeftButtonDown = true;
					curr_pos_x_ = lx-5;
					pos2value();
				
					//if(this->psc_) this->psc_->OnClick(this->getValue());
				}
			}
		}
		else if(state == GLUT_UP)
		{
			if(isLeftButtonDown)
			{
				if(lx >= 5 && lx <= width-5)
				{
					curr_pos_x_ = lx-5;
					pos2value();

					if(this->psc_) this->psc_->OnClick(this->getValue());
				}
				isLeftButtonDown = false;
			}
		}
	}
}

template<class T>
void FrameSlider<T>::onMotionFunc(int x,int y)
{
	int lx,ly;
	globalPt2Local(lx,ly,x,y);

	if(isLeftButtonDown)
	{
		if(lx >= 5 && lx <= width-5)
		{
			curr_pos_x_ = lx-5;
			pos2value();

			//if(this->psc_) this->psc_->OnClick(this->getValue());
		}
	}
}
