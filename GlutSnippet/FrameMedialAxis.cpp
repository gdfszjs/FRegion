#include "FrameMedialAxis.h"
#include "CommonGL.h"

#include "SVGParser.h"
#include "SVGPattern.h"
#include "SVGElement.h"

FrameMedialAxis::FrameMedialAxis() 
{
}

FrameMedialAxis::FrameMedialAxis(Frame * parent, int lx, int ly, int width, int height,string name)
	: Frame(parent,lx,ly,width,height,name) 
{
	string svg_file_name = "D:/drawing-1.svg";
	/* read pattern from svg file */
	SVGParser * parser = new SVGParser();
	parser->parse(svg_file_name.c_str());
	pattern_ = parser->toSVGPattern();
	pattern_->init();
	delete parser;
	pattern_->getBoundingBoxCenter(this->centre_[0],this->centre_[1]); 

	this->FindMedialAxis(this->medial_axis_,this->pattern_);
}

FrameMedialAxis::~FrameMedialAxis() 
{
	if(this->pattern_ != 0) delete this->pattern_;
}

void FrameMedialAxis::drawScene() {
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

	/* draw pattern */
	if(pattern_ != 0) pattern_->drawPattern();


	/*if(pattern_ != 0)
	{

		v2d p = _v2d_(200,200);
		SVGElement * e = pattern_->elements_[4];

		double d;
		v2d pp;

		this->NearestBetweenPointAndElement(d,pp,p,e);

		DrawCircle(p,d);
	}*/

	glColor3f(1,0,0);
	glPointSize(2);
	glBegin(GL_POINTS);
		
		for(size_t i = 0;i<this->medial_axis_.size();i++)
		{
			glVertex2d(this->medial_axis_[i][0],this->medial_axis_[i][1]);
		}
	glEnd();
	glPointSize(1);
	glColor3f(1,1,1);

	/*v2d aa = _v2d_(260,260), bb = _v2d_(540,540);

	glColor3f(1,0,0);
	glBegin(GL_LINE_LOOP);
		glVertex2d(aa[0],aa[1]);
		glVertex2d(aa[0],bb[1]);
		glVertex2d(bb[0],bb[1]);
		glVertex2d(bb[0],aa[1]);
	glEnd();
	glColor3f(1,1,1);*/
}

void FrameMedialAxis::NearestBetweenPointAndElement(double & d, v2d & np, const v2d & sp, SVGElement* e)
{
	std::vector<v2d> pts;
	e->collectPoints(pts,0.01);

	d = 1e10;

	for(size_t i = 0;i<pts.size();i++)
	{
		v2d p = pts[i];

		double x = p[0] - sp[0];
		double y = p[1] - sp[1];
		double dd = sqrt(x*x+y*y);

		if(dd < d)
		{
			d = dd;
			np[0] = p[0];
			np[1] = p[1];
		}
	}
}

void FrameMedialAxis::DrawCircle(const v2d & center, double r)
{
	glBegin(GL_LINE_LOOP);
		for(int i = 0;i<360;i++)
		{
			double t = i * 3.14159/180;
			double x = center[0] + r * cos(t);
			double y = center[1] + r * sin(t);
			glVertex2d(x,y);
		}
	glEnd();
}

bool FrameMedialAxis::IsPointOnMedialAxis(const v2d & p, const std::vector<SVGElement*> & ee)
{
	if(ee.size() < 2) return false;

	std::vector<double> dist;

	for(size_t i = 0;i<ee.size();i++)
	{
		SVGElement * e = ee[i];

		double d;
		v2d	np;

		this->NearestBetweenPointAndElement(d,np,p,e);
		dist.push_back(d);	
	}

	std::sort(dist.begin(),dist.end(),std::less<double>());

	double aa = dist[0];
	double bb = dist[1];

	if(fabs(aa-bb) < 1) return true;
	else return false;
}

void FrameMedialAxis::FindMedialAxis(std::vector<v2d> & pts, SVGPattern * pattern)
{
	pts.clear();

	if(pattern == 0) return;

	pattern->computeBoundingBox();

	double cx,cy;
	double w,h;
	pattern->getBoundingBoxCenter(cx,cy);
	pattern->getBoundingBoxWH(w,h);

	for(int i = 260;i<540;i++)
	{
		cout<<i<<endl;
		for(int j = 260;j<540;j++)
		{
			if(this->IsPointOnMedialAxis(_v2d_(j,i),pattern->elements_))
			{
				//cout<<"OK"<<endl;
				pts.push_back(_v2d_(j,i));
			}
		}
	}
}