#include "FRNode.h"
#include "CommonGl.h"
#include "Geo2D.h"

/* =================================================================================================== */
/* FRNodeLine */

void FRNodeLine::Draw()
{
	glBegin(GL_LINES);
	glVertex2d(pts_[0][0],pts_[0][1]);
	glVertex2d(pts_[1][0],pts_[1][1]);
	glEnd();

	bool test = false;
	if(test)
	{
		v2d pt;
		Geo2D::PointOnLine(pt,0.8,pts_[0],pts_[1]);
		glColor3f(0,1,0);
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex2d(pt[0],pt[1]);
		glEnd();
		glPointSize(1);
		glColor3f(1,1,1);
	}
}

void FRNodeLine::DrawControlPoints()
{
	glBegin(GL_POINTS);
	glVertex2d(pts_[0][0],pts_[0][1]);
	glVertex2d(pts_[1][0],pts_[1][1]);
	glEnd();
}

void FRNodeLine::set_pts(int idx, v2d pt)
{
	if(idx <0 || idx >= 2) return;
	
	pts_[idx] = _v2d_(pt[0],pt[1]);
}

v2d FRNodeLine::pts(int idx)
{
	if(idx < 0 || idx >= 2) return _v2d_(0,0);

	return pts_[idx];
}

int FRNodeLine::SelectControlPoint(const v2d & pt) const
{
	double r = 20;
	
	for(int i = 0;i<2;i++)
	{
		double d = dist_v2d(pt,pts_[i]);
		if(d < r) return i;
	}
	
	return -1;
}

void FRNodeLine::SamplePointsByGap(std::vector<v2d> & pts, double gap, int grain_num)
{
	int i = 0;
	double len = this->Length(1);
	while( i * gap < len)
	{
		double t = this->LengthTo_t(i*gap);
		v2d pt;
		this->tToPoint(pt,t);
		pts.push_back(pt);
		i++;
	}
}

void FRNodeLine::SamplePoints(std::vector<v2d> & pts, double dt)
{
	for(double t = 0;t<1;t+=dt)
	{
		double x = pts_[0][0] + t * (pts_[1][0] - pts_[0][0]);
		double y = pts_[0][1] + t * (pts_[1][1] - pts_[0][1]);
		pts.push_back(_v2d_(x,y));
	}
}

void FRNodeLine::ReverseSamplePoints(std::vector<v2d> & pts, double dt)
{
	for(double t = 1;t>1e-6;t-=dt)
	{
		double x = pts_[0][0] + t * (pts_[1][0] - pts_[0][0]);
		double y = pts_[0][1] + t * (pts_[1][1] - pts_[0][1]);
		pts.push_back(_v2d_(x,y));
	}
}



void FRNodeLine::translate(double x, double y)
{
	for(int i = 0;i<2;i++)
	{
		pts_[i][0] += x;
		pts_[i][1] += y;
	}
}

void FRNodeLine::scale(double x, double y)
{
	for(int i = 0;i<2;i++)
	{
		pts_[i][0] *= x;
		pts_[i][1] *= y;
	}
}

void FRNodeLine::rotate(double t)
{
	for(int i = 0;i<2;i++)
	{
		double x = pts_[i][0] * cos(t) - pts_[i][1] * sin(t);
		double y = pts_[i][0] * sin(t) + pts_[i][1] * cos(t);
		pts_[i][0] = x;
		pts_[i][1] = y;
	}
}

double FRNodeLine::Length(double t)
{
	double a = pts_[1][0] - pts_[0][0];
	double b = pts_[1][1] - pts_[0][1];

	double l = sqrt(a * a + b * b);
	
	return (t * l);
}

double FRNodeLine::LengthTo_t(double l)
{
	double tl = this->Length(1);
	return (l / tl);
}

void FRNodeLine::tToPoint(v2d & pt, double t)
{
	Geo2D::InterpLinear(pt,t,this->pts_[0],this->pts_[1]);
}

FRNode * FRNodeLine::CopySelf()
{
	FRNodeLine * l = new FRNodeLine();
	l->pts_[0] = this->pts_[0];
	l->pts_[1] = this->pts_[1];
	return l;
}

FRNode * FRNodeLine::CopyInverseSelf()
{
	FRNodeLine * l = new FRNodeLine();
	l->pts_[0] = this->pts_[1];
	l->pts_[1] = this->pts_[0];
	return l;
}



/* =================================================================================================== */
/* FRNodeBezier */

void FRNodeBezier::Draw()
{
	GLdouble ctrlpoints[4][3];
	ctrlpoints[0][0] = pts_[0][0];
	ctrlpoints[0][1] = pts_[0][1];
	ctrlpoints[0][2] = 0;

	ctrlpoints[1][0] = pts_[1][0];
	ctrlpoints[1][1] = pts_[1][1];
	ctrlpoints[1][2] = 0;

	ctrlpoints[2][0] = pts_[2][0];
	ctrlpoints[2][1] = pts_[2][1];
	ctrlpoints[2][2] = 0;

	ctrlpoints[3][0] = pts_[3][0];
	ctrlpoints[3][1] = pts_[3][1];
	ctrlpoints[3][2] = 0;

	glMap1d(GL_MAP1_VERTEX_3,0,1,3,4,&ctrlpoints[0][0]);
	glMapGrid1f(10,0,1);
	glEnable(GL_MAP1_VERTEX_3);
	glEvalMesh1(GL_LINE,0,10);

	bool test = false;
	if(test)
	{
		v2d pt,ta,no;
		Geo2D::PointOnBezier(pt,0.8,pts_[0],pts_[1],pts_[2],pts_[3]);
		Geo2D::TangentOnCubicBeizer(ta,0.8,pts_[0],pts_[1],pts_[2],pts_[3]);
		Geo2D::NormalOnCubicBezier(no,0.8,pts_[0],pts_[1],pts_[2],pts_[3]);

		glColor3f(0,1,0);

		glPointSize(1);
		glBegin(GL_POINTS);
		glVertex2d(pt[0],pt[1]);
		glEnd();
		glPointSize(1);

		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2d(pt[0],pt[1]);
		glVertex2d(pt[0] + ta[0] * 100,pt[1] + ta[1] * 100);
		glEnd();
		glLineWidth(1);

		glBegin(GL_LINES);
		glVertex2d(pt[0],pt[1]);
		glVertex2d(pt[0] + no[0] * 100,pt[1] + no[1] * 100);
		glEnd();

		glColor3f(1,1,1);
	}
}

void FRNodeBezier::set_pts(int idx, v2d pt)
{
	if(idx < 0 || idx >= 4) return;
	pts_[idx] = _v2d_(pt[0],pt[1]);
}

v2d FRNodeBezier::pts(int idx)
{
	if(idx < 0 || idx >= 4) return _v2d_(0,0);
	
	return pts_[idx];
}

void FRNodeBezier::DrawControlPoints()
{
	glBegin(GL_POINTS);
	glVertex2d(pts_[0][0],pts_[0][1]);
	glVertex2d(pts_[1][0],pts_[1][1]);
	glVertex2d(pts_[2][0],pts_[2][1]);
	glVertex2d(pts_[3][0],pts_[3][1]);
	glEnd();
}

int FRNodeBezier::SelectControlPoint(const v2d & pt) const
{
	double r = 20;
	
	for(int i = 0;i<4;i++)
	{
		double d = dist_v2d(pt,pts_[i]);
		if(d < r) return i;
	}
	
	return -1;
}

void FRNodeBezier::SamplePointsByGap(std::vector<v2d> & pts, double gap, int grain_num)
{
	v2d p0 = pts_[0];
	v2d p1 = pts_[1];
	v2d p2 = pts_[2];
	v2d p3 = pts_[3];

	int cnt = grain_num;

	std::vector<double> ll; 
	for(int i = 0;i<cnt;i++)	
	{
		double t = (double)(i) / (double)(cnt);
		ll.push_back(Geo2D::GetCubicBeizerArcLength(t,p0,p1,p2,p3));
	}

	int j = 0;
	for(int i = 0;i<cnt-1;i++)
	{
		if(j * gap >= ll[i] && j * gap < ll[i+1])
		{
			v2d pt;
			Geo2D::PointOnCubicBezier_deCasteljau(pt,(double)i/(double)cnt,p0,p1,p2,p3);
			pts.push_back(pt);
			j++;
		}
	}
}

void FRNodeBezier::SamplePoints(std::vector<v2d> & pts, double dt)
{
	v2d p0 = pts_[0];
	v2d p1 = pts_[1];
	v2d p2 = pts_[2];
	v2d p3 = pts_[3];
	
	v4d coex,coey;
	Geo2D::BezierParams(coex,coey,p0,p1,p2,p3);
	
	for(double j = 0;j<1;j+=dt)
	{
		double x = coex[0] * j * j * j + coex[1] * j * j + coex[2] * j + coex[3];
		double y = coey[0] * j * j * j + coey[1] * j * j + coey[2] * j + coey[3];

		pts.push_back(_v2d_(x,y));
	}
}

void FRNodeBezier::ReverseSamplePoints(std::vector<v2d> & pts, double dt)
{
	v2d p0 = pts_[0];
	v2d p1 = pts_[1];
	v2d p2 = pts_[2];
	v2d p3 = pts_[3];

	v4d coex,coey;
	Geo2D::BezierParams(coex,coey,p0,p1,p2,p3);
	
	for(double j = 1;j>1e-6;j-=dt)
	{
		double x = coex[0] * j * j * j + coex[1] * j * j + coex[2] * j + coex[3];
		double y = coey[0] * j * j * j + coey[1] * j * j + coey[2] * j + coey[3];

		pts.push_back(_v2d_(x,y));
	}
}

void FRNodeBezier::translate(double x, double y)
{
	for(int i = 0;i<4;i++)
	{
		pts_[i][0] += x;
		pts_[i][1] += y;
	}
}

void FRNodeBezier::scale(double x, double y)
{
	for(int i = 0;i<4;i++)
	{
		pts_[i][0] *= x;
		pts_[i][1] *= y;
	}
}

void FRNodeBezier::rotate(double t)
{
	for(int i = 0;i<4;i++)
	{
		double x = pts_[i][0] * cos(t) - pts_[i][1] * sin(t);
		double y = pts_[i][0] * sin(t) + pts_[i][1] * cos(t);
		pts_[i][0] = x;
		pts_[i][1] = y;
	}
}

double FRNodeBezier::Length(double t)
{
	return Geo2D::GetCubicBeizerArcLength(t,pts_[0],pts_[1],pts_[2],pts_[3]);
}

double FRNodeBezier::LengthTo_t(double l)
{
	return Geo2D::CubicBeizerArcLengthTo_t(l,pts_[0],pts_[1],pts_[2],pts_[3]);
}

void FRNodeBezier::tToPoint(v2d & pt,double t)
{
	Geo2D::PointOnBezier(pt,t,this->pts_[0],this->pts_[1],this->pts_[2],this->pts_[3]);
}

FRNode*	FRNodeBezier::CopySelf()
{
	FRNodeBezier * b = new FRNodeBezier();
	b->pts_[0] = this->pts_[0];
	b->pts_[1] = this->pts_[1];
	b->pts_[2] = this->pts_[2];
	b->pts_[3] = this->pts_[3];
	return b;
}

FRNode * FRNodeBezier::CopyInverseSelf()
{
	FRNodeBezier * b = new FRNodeBezier();
	b->pts_[0] = this->pts_[3];
	b->pts_[1] = this->pts_[2];
	b->pts_[2] = this->pts_[1];
	b->pts_[3] = this->pts_[0];
	return b;
}