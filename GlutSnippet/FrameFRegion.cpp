#include "FrameFRegion.h"
#include "CommonGL.h"

#include "SVGParser.h"
#include "SVGPattern.h"
#include "SVGElement.h"
#include "FRNode.h"
#include "FRStructure.h"

#include "Geo2D.h"

FrameFRegion::FrameFRegion() 
{
	this->pattern_ = 0;
	this->fregion_ = 0;
	this->theta_index_ = 0;
}

FrameFRegion::FrameFRegion(Frame * parent, int lx, int ly, int width, int height,string name)
	: Frame(parent,lx,ly,width,height,name) 
{
	/* parameters that can be changed */
	string svg_file_name		= "D:/misc/svgpath/newflowerbed3.svg";
	fregion_file_name_			= "D:/misc/fregion/drawing-1.txt";

	this->theta_index_			= 0;

	/* read pattern from svg file */
	SVGParser * parser = new SVGParser();
	parser->parse(svg_file_name.c_str());
	pattern_ = parser->toSVGPattern();
	pattern_->init();
	delete parser;
	pattern_->getBoundingBoxCenter(this->centre_[0],this->centre_[1]); 
	
	/* initial fundamental region */
	double theta1_angle			= 0;
	double theta2_angle			= 36.5;
	theta1_radian_ = _PI_ / 180 * theta1_angle;
	theta2_radian_ = _PI_ / 180 * theta2_angle;

	this->fregion_ = 0;
	this->ComputeFRSTree();	

	/* save the fregion */
	cout << "hahahhahahahah";
	this->SaveFR(fregion_file_name_.c_str());
}

FrameFRegion::~FrameFRegion() 
{
	if(this->pattern_ != 0) delete this->pattern_;
	if(this->fregion_ != 0) delete this->fregion_;
}

void FrameFRegion::drawScene() 
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
	gluOrtho2D(0,width,0,height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	writeText(GLUT_BITMAP_9_BY_15,
		0,20,
		"c/C: Change",
		255,255,255,width,height);

	/* draw pattern */
	if(pattern_ != 0) pattern_->drawPattern();

	/* draw cut lines */
	glColor3f(1,0,0);
	glBegin(GL_LINES);
		if(theta_index_ == 0)
			glColor3f(0,1,0);
		else
			glColor3f(1,0,0);
		glVertex2d(this->centre_[0],this->centre_[1]);
		glVertex2d(this->p0_[0],this->p0_[1]);
		
		if(theta_index_ == 1)
			glColor3f(0,1,0);
		else
			glColor3f(1,0,0);
		glVertex2d(this->centre_[0],this->centre_[1]);
		glVertex2d(this->p1_[0],this->p1_[1]);
	glEnd();
	glColor3f(1,1,1);

	/* draw the fregion */
	glPushMatrix();
	glTranslated(350,0,0);
	if(this->fregion_ != 0) this->fregion_->Draw();
	glPopMatrix();
}

void FrameFRegion::ComputeIntersectionBetweenLineLine(std::vector<v2d> & inter, const v2d & v0, const v2d & v1, const v2d & v2, const v2d & v3)	
{
	if(v0[0] == v1[0] && v0[1] == v1[1])
	{
		//cout<<"Wrong - FrameFRegion::ComputeIntersectionBetweenLineLine: v0 == v1"<<endl;
		return;
	}

	if(v2[0] == v3[0] && v2[1] == v3[1])
	{
		//cout<<"Wrong - FrameFRegion::ComputeIntersectionBetweenLineLine: v2 == v3"<<endl;
		return;
	}

	double x1,y1,x2,y2,x3,y3,x4,y4;

	x1 = v0[0];
	y1 = v0[1];

	x2 = v1[0];
	y2 = v1[1];

	x3 = v2[0];
	y3 = v2[1];

	x4 = v3[0];
	y4 = v3[1];

	double a = (x1*y2 - y1*x2) * (x3 - x4) - (x1 - x2) * (x3*y4 - y3*x4);
	double b = (x1*y2 - y1*x2) * (y3 - y4) - (y1 - y2) * (x3*y4 - y3*x4);
	double c = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

	if(c == 0) return;
	
	double x, y;
	
	x = a / c;
	y = b / c;
	
	if(IsPointOnLineSeg(_v2d_(x,y),v0,v1) && IsPointOnLineSeg(_v2d_(x,y),v2,v3))
		inter.push_back(_v2d_(x,y));
	/*if(Geo2D::IsPointOnLineseg(_v2d_(x,y),v0,v1,0.01) 
		&& 
		Geo2D::IsPointOnLineseg(_v2d_(x,y),v2,v3,0.01))
		inter.push_back(_v2d_(x,y));*/
}

bool FrameFRegion::IsPointOnLineSeg(const v2d & v, const v2d & v1, const v2d & v2)
{
	double x1,y1,x2,y2;

	x1 = v1[0] - v[0];
	y1 = v1[1] - v[1];

	x2 = v2[0] - v[0];
	y2 = v2[1] - v[1];

	double a = x1 * x2 + y1 * y2;
	
	if(a <= 0) return true;
	else 
		return false;	
}

void FrameFRegion::ComputeIntersectionBetweenLineCubicBezier(std::vector<v2d> & inters, 
	const v2d & v0, const v2d & v1, 
	const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	double A,B,C; // parameters of line
	
	A = v1[1] - v0[1];
	B = v0[0] - v1[0];
	C = v0[0] * (-A) + v0[1] * (-B);

	v4d bx, by;
	
	Geo2D::BezierParams(bx,by,p0,p1,p2,p3);
	
	v4d cc;
	cc[0] = A * bx[0] + B * by[0];
	cc[1] = A * bx[1] + B * by[1];
	cc[2] = A * bx[2] + B * by[2];
	cc[3] = A * bx[3] + B * by[3] + C;

	std::vector<double> roots;
	Geo2D::CubicRoots(roots,cc);

	inters.clear();
	
	for(size_t i = 0;i<roots.size();i++)
	{
		double t = roots[i];

		double x = bx[0] * t * t * t + bx[1] * t * t + bx[2] * t + bx[3];
		double y = by[0] * t * t * t + by[1] * t * t + by[2] * t + by[3];

		/*if(this->IsPointOnLineSeg(_v2d_(x,y),v0,v1))
			inters.push_back(_v2d_(x,y));*/
		if(Geo2D::IsPointOnLineseg(_v2d_(x,y),v0,v1,1))
		{
			inters.push_back(_v2d_(x,y));
		}
	}
}

SVGElement * FrameFRegion::SplitElementByLine(const SVGElement & e, const v2d & v0, const v2d & v1)
{
	SVGElement * ans = new SVGElement();

	for(size_t i = 0;i<e.segs_.size();i++)
	{
		if(e.segs_[i]->SegType() == 0) // line
		{
			std::vector<v2d> inter; 
			this->ComputeIntersectionBetweenLineLine(inter,v0,v1,
				((ElementSegLine*)(e.segs_[i]))->pts_[0],
				((ElementSegLine*)(e.segs_[i]))->pts_[1]);

			if(inter.empty()) // push back the source line
			{
				ElementSegLine * line = new ElementSegLine( 
					((ElementSegLine*)(e.segs_[i]))->pts_[0],
					((ElementSegLine*)(e.segs_[i]))->pts_[1]);
				ans->segs_.push_back(line);
			}
			else // the source line is divided into two new lines
			{
				ElementSegLine * line1 = new ElementSegLine(
					((ElementSegLine*)(e.segs_[i]))->pts_[0],
					inter[0]);

				ElementSegLine * line2 = new ElementSegLine(
					inter[0],
					((ElementSegLine*)(e.segs_[i]))->pts_[1]);

				ans->segs_.push_back(line1);
				ans->segs_.push_back(line2);
			}
		}
		else // bezier
		{
			std::vector<v2d> inters;
			this->ComputeIntersectionBetweenLineCubicBezier(inters,v0,v1,
				((ElementSegBezier*)(e.segs_[i]))->pts_[0],
				((ElementSegBezier*)(e.segs_[i]))->pts_[1],
				((ElementSegBezier*)(e.segs_[i]))->pts_[2],
				((ElementSegBezier*)(e.segs_[i]))->pts_[3]);
			
			if(inters.empty()) // push back the source bezier 
			{
				ElementSegBezier * bezier = new ElementSegBezier( 
					((ElementSegBezier*)(e.segs_[i]))->pts_[0],
					((ElementSegBezier*)(e.segs_[i]))->pts_[1],
					((ElementSegBezier*)(e.segs_[i]))->pts_[2],
					((ElementSegBezier*)(e.segs_[i]))->pts_[3]);

				ans->segs_.push_back(bezier);
			}
			else // the source is divided to multiple beziers
			{
				std::vector<std::vector<v2d>> beziers;
				Geo2D::SplitBezier(beziers,inters,
					((ElementSegBezier*)(e.segs_[i]))->pts_[0],
					((ElementSegBezier*)(e.segs_[i]))->pts_[1],
					((ElementSegBezier*)(e.segs_[i]))->pts_[2],
					((ElementSegBezier*)(e.segs_[i]))->pts_[3]);

				for(size_t j = 0;j<beziers.size();j++)
				{
					ElementSegBezier * bj = new ElementSegBezier(beziers[j][0],beziers[j][1],beziers[j][2],beziers[j][3]);
					ans->segs_.push_back(bj);
				}
			}
		}
	}

	return ans;
}

bool FrameFRegion::IsPointLeftLine(const v2d & v, const v2d & v0, const v2d & v1)
{
	double A,B,C;
	
	Geo2D::LineParams(A,B,C,v0,v1);

	double d = A * v[0] + B * v[1] + C;
	
	return (d<1);
}

bool FrameFRegion::IsPointRightLine(const v2d & v, const v2d & v0, const v2d & v1)
{
	double A,B,C;
	Geo2D::LineParams(A,B,C,v0,v1);
	double d = A * v[0] + B * v[1] + C;
	return (d>-1);
}

bool FrameFRegion::IsSegInFR(ElementSeg & s)
{
	if(s.SegType() == 0) 
	{
		v2d p0,p1;
		p0 = ((ElementSegLine*)(&s))->pts_[0];
		p1 = ((ElementSegLine*)(&s))->pts_[1];

		if(this->IsPointLeftLine(p0,this->centre_,this->p1_) &&
			this->IsPointLeftLine(p1,this->centre_,this->p1_) &&
			this->IsPointRightLine(p0,this->centre_,this->p0_) &&
			this->IsPointRightLine(p1,this->centre_,this->p0_))
				return true;
			else
				return false;
	}
	else
	{
		std::vector<v2d> pts;
		((ElementSegBezier*)(&s))->collectPoints(pts,0.1);

		for(size_t i = 0;i<pts.size();i++)
		{
			if(!this->IsPointLeftLine(pts[i],this->centre_,this->p1_) ||
				!this->IsPointRightLine(pts[i],this->centre_,this->p0_))
			{
				return false;
			}
		}
		return true;
	}
	
	
}

SVGPattern* FrameFRegion::ComputeFR()
{
	if(this->pattern_ == 0) return 0;

	SVGPattern * ans = new SVGPattern();
	std::vector<SVGElement*> ime;

	for(size_t i = 0;i<pattern_->elements_.size();i++)
	{
		SVGElement * ie1 = this->SplitElementByLine(*(pattern_->elements_[i]),this->centre_,this->p1_);
		SVGElement * ie2 = this->SplitElementByLine(*ie1,this->centre_,this->p0_);
		ime.push_back(ie2);
		delete ie1;
	}

	for(size_t i = 0;i<ime.size();i++)
	{
		SVGElement * e = new SVGElement();
		for(size_t j = 0;j<ime[i]->segs_.size();j++)
		{
			/*if(i==1)
			{
				if(j==3)
					if(IsSegInFR(*(ime[i]->segs_[j])))
					{
				if(ime[i]->segs_[j]->SegType() == 0)
					e->addSeg(new ElementSegLine(*((ElementSegLine*)(ime[i]->segs_[j]))));
				else
					e->addSeg(new ElementSegBezier(*((ElementSegBezier*)(ime[i]->segs_[j]))));
					}
			}
			else*/
				if(IsSegInFR(*(ime[i]->segs_[j])))
			{
				if(ime[i]->segs_[j]->SegType() == 0)
					e->addSeg(new ElementSegLine(*((ElementSegLine*)(ime[i]->segs_[j]))));
				else
					e->addSeg(new ElementSegBezier(*((ElementSegBezier*)(ime[i]->segs_[j]))));
			}
		}
		if(e->segs_.empty()) delete e;
		else
			ans->elements_.push_back(e);
	}

	for(size_t i = 0;i<ime.size();i++)
		delete ime[i];

	return ans;
}

void FrameFRegion::RotateFR(double theta,double cx,double cy)
{
	for(size_t i = 0;i<this->fregion_->pattern_->elements_.size();i++)
	{
		this->fregion_->pattern_->elements_[i]->translate(-cx,-cy);
		this->fregion_->pattern_->elements_[i]->rotate(theta);
		this->fregion_->pattern_->elements_[i]->translate(cx,cy);
	}

	for(size_t i = 0;i<this->fregion_->all_frnodes_.size();i++)
	{
		this->fregion_->all_frnodes_[i]->translate(-cx,-cy);
		this->fregion_->all_frnodes_[i]->rotate(theta);
		this->fregion_->all_frnodes_[i]->translate(cx,cy);
	}
}

void FrameFRegion::SaveFR(const char * filename)
{	
	ofstream outfile(filename);
	if(this->fregion_ != 0)	this->fregion_->Save(outfile);
}

void FrameFRegion::ResizeFRAngle(double new_angle, double theta1, double theta2)
{
	double old_angle = theta2 - theta1;
	double por		 = new_angle / old_angle;

	if(this->fregion_ == 0) return;

	/* get the centre */
	v2d centre = this->fregion_->all_frnodes_[0]->GetFirstEnd();

	for(size_t i = 0;i<this->fregion_->pattern_->elements_.size();i++)
	{
		for(size_t j = 0;j<this->fregion_->pattern_->elements_[i]->segs_.size();j++)
		{
			if(this->fregion_->pattern_->elements_[i]->segs_[j]->SegType() == 0)
			{
				ElementSegLine * seg_line = (ElementSegLine*)(this->fregion_->pattern_->elements_[i]->segs_[j]);
				for(int k = 0;k<2;k++)
				{
					v2d pt = seg_line->pts_[k];
					v2d ve = _v2d_(pt[0] - centre[0], pt[1] - centre[1]);
					
					double stheta  = atan2(ve[1],ve[0]);
					double dtheta  = stheta * por;
					double sradius = sqrt(ve[0] * ve[0] + ve[1] * ve[1]);
					
					seg_line->pts_[k][0] = centre[0] + sradius * cos(dtheta);
					seg_line->pts_[k][1] = centre[1] + sradius * sin(dtheta);
				}
			}
			else
			{
				ElementSegBezier * seg_bezier = (ElementSegBezier*)(this->fregion_->pattern_->elements_[i]->segs_[j]);
				for(int k = 0;k<4;k++)
				{
					v2d pt = seg_bezier->pts_[k];
					v2d ve = _v2d_(pt[0] - centre[0], pt[1] - centre[1]);
					
					double stheta  = atan2(ve[1],ve[0]);
					double dtheta  = stheta * por;
					double sradius = sqrt(ve[0] * ve[0] + ve[1] * ve[1]);
					
					seg_bezier->pts_[k][0] = centre[0] + sradius * cos(dtheta);
					seg_bezier->pts_[k][1] = centre[1] + sradius * sin(dtheta);
				}
			}
		}
	}
		
	for(size_t i = 0;i<this->fregion_->all_frnodes_.size();i++)
	{
		if(this->fregion_->all_frnodes_[i]->NodeType() == 0)
		{
			FRNodeLine * node_line = (FRNodeLine*)(this->fregion_->all_frnodes_[i]);
			for(int k = 0;k<2;k++)
			{
				v2d pt = node_line->pts(k);
				v2d ve = _v2d_(pt[0] - centre[0], pt[1] - centre[1]);
				
				double stheta  = atan2(ve[1],ve[0]);
				double dtheta  = stheta * por;
				double sradius = sqrt(ve[0] * ve[0] + ve[1] * ve[1]);
			
				v2d npt;
				npt[0] = centre[0] + sradius * cos(dtheta);
				npt[1] = centre[1] + sradius * sin(dtheta);
				node_line->set_pts(k,npt);
			}
		}
		else
		{
			FRNodeBezier * node_bezier = (FRNodeBezier*)(this->fregion_->all_frnodes_[i]);
			for(int k = 0;k<4;k++)
			{
				v2d pt = node_bezier->pts(k);
				v2d ve = _v2d_(pt[0] - centre[0], pt[1] - centre[1]);
				
				double stheta  = atan2(ve[1],ve[0]);
				double dtheta  = stheta * por;
				double sradius = sqrt(ve[0] * ve[0] + ve[1] * ve[1]);
			
				v2d npt;
				npt[0] = centre[0] + sradius * cos(dtheta);
				npt[1] = centre[1] + sradius * sin(dtheta);
				node_bezier->set_pts(k,npt);
			}
		}
	}
}

void FrameFRegion::onKeyboardFunc(unsigned char key, int x, int y)
{
	if(key == 'C' || key == 'c')
		this->theta_index_ = (this->theta_index_ + 1) % 2;
}

void FrameFRegion::onMouseFunc(int button, int state, int x, int y)
{
	int lx,ly;
	this->globalPt2Local(lx,ly,x,y);
	
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		double llx		= lx - this->centre_[0];
		double lly		= ly - this->centre_[1];
		double theta	= atan2(lly,llx);
		
		if(this->theta_index_ == 0)
			this->theta2_radian_ = theta;
		else
			this->theta1_radian_ = theta;
		
		this->ComputeFRSTree();
		this->SaveFR(this->fregion_file_name_.c_str());	
	}
}

void FrameFRegion::ComputeFRSTree()
{
	double radius				= 280; 

	double theta1_angle			= 180 * this->theta1_radian_ / _PI_;
	double theta2_angle			= 180 * this->theta2_radian_ / _PI_;

	double resize_angle			= FRSTree::BestAngle(theta2_angle - theta1_angle);

	if(this->fregion_ != 0) delete this->fregion_;
	
	/* compute fregion */
	this->fregion_ = new FRSTree();
	this->fregion_->frs_root_node_ = new FRSNode();
	this->fregion_->current_node_ = this->fregion_->frs_root_node_;
	
	this->p0_[0] = this->centre_[0] + radius * cos(theta2_radian_);
	this->p0_[1] = this->centre_[1] + radius * sin(theta2_radian_);

	this->p1_[0] = this->centre_[0] + radius * cos(theta1_radian_);
	this->p1_[1] = this->centre_[1] + radius * sin(theta1_radian_);
	
	/* Get elements */
	this->fregion_->pattern_ = this->ComputeFR();
	for(size_t i = 0;i<this->fregion_->pattern_->elements_.size();i++)
		this->fregion_->frs_root_node_->element_indices_.push_back((int)i);
	
	/* Get boundary */	
	FRNodeLine * l0 = new FRNodeLine();
	FRNodeLine * l1 = new FRNodeLine();
	FRNodeLine * l2 = new FRNodeLine();

	l0->set_pts(0,this->centre_);
	l0->set_pts(1,this->p0_);
	
	l1->set_pts(0,this->p0_);
	l1->set_pts(1,this->p1_);

	l2->set_pts(0,this->p1_);
	l2->set_pts(1,this->centre_);

	l0->set_left_node(l2);
	l0->set_right_node(l1);

	l1->set_left_node(l0);
	l1->set_right_node(l2);

	l2->set_left_node(l1);
	l2->set_right_node(l0);

	this->fregion_->all_frnodes_.push_back(l0);
	this->fregion_->all_frnodes_.push_back(l1);
	this->fregion_->all_frnodes_.push_back(l2);

	this->fregion_->frs_root_node_->frnode_indices_.push_back(0);
	this->fregion_->frs_root_node_->frnode_indices_.push_back(1);
	this->fregion_->frs_root_node_->frnode_indices_.push_back(2);

	this->fregion_->set_fr_angle(resize_angle);

	/* transform the fregion: rotate and resize the sector */	
	this->RotateFR(-theta1_radian_,this->centre_[0],this->centre_[1]);
	this->ResizeFRAngle(resize_angle,theta1_angle,theta2_angle);
}
