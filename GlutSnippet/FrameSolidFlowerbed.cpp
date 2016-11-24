#include "FrameSolidFlowerbed.h"
#include "CommonGl.h"
#include "SVGPattern.h"
#include "FRStructure.h"
#include "SVGElement.h"
#include "FrameStructureBlend.h"
#include "Geo2D.h"
#include "TextureLoader.h"
#include <list>
#include "GLUTMain.h"
#include "FrameManifold.h"


#define VERTEX_DIM 8

FrameSolidFlowerbed::FrameSolidFlowerbed() {
}

FrameSolidFlowerbed::FrameSolidFlowerbed(Frame * parent, int lx, int ly, int width, int height,string name)
	: Frame(parent,lx,ly,width,height,name) 
{
	this->camera_pos_ = _v3d_(0,0,30);
	//this->camera_pos_ = _v3d_(0,0,500);
	//this->rotate_x_   = -65;
	this->rotate_x_   = 0;
	this->rotate_y_   = 0;
	this->list_id_    = 0;
}

FrameSolidFlowerbed::~FrameSolidFlowerbed() 
{
	if(this->list_id_ != 0)
		glDeleteLists(this->list_id_,1);
}

void _stdcall OnBeginCallback(unsigned int which)
{
	glBegin(which);
	//cout<<"glBegin("<<which<<")"<<endl;
}

void _stdcall OnEndCallback()
{
	glEnd();
	//cout<<"glEnd()"<<endl;
}

void _stdcall OnVertexCallback(void * vertex_data)
{
	double * data = (double*)(vertex_data);
	//cout<<"glColor3d("<<data[3]<<","<<data[4]<<","<<data[5]<<")"<<endl;
	glColor3dv(data+3);
	//cout<<"glVertex3d("<<data[0]<<","<<data[1]<<","<<data[2]<<")"<<endl;
	//glTexCoord2dv(data + 6);
	glVertex3dv(data);

}

static double	Combine_Vertices[1024][VERTEX_DIM];
static int		Combine_Vertex_Index = 0 ;		

void _stdcall OnCombineCallback(const GLdouble newVertex[3], const GLdouble *neighborVertex[4],
                            const GLfloat neighborWeight[4], GLdouble **outData)
{
	// copy new intersect vertex to local array
    // Because newVertex is temporal and cannot be hold by tessellator until next
    // vertex callback called, it must be copied to the safe place in the app.
    // Once gluTessEndPolygon() called, then you can safly deallocate the array.
    Combine_Vertices[Combine_Vertex_Index][0] = newVertex[0];
    Combine_Vertices[Combine_Vertex_Index][1] = newVertex[1];
    Combine_Vertices[Combine_Vertex_Index][2] = newVertex[2];

    // compute vertex color with given weights and colors of 4 neighbors
    // the neighborVertex[4] must hold required info, in this case, color.
    // neighborVertex was actually the third param of gluTessVertex() and is
    // passed into here to compute the color of the intersect vertex.
	if(neighborWeight != 0 && neighborVertex != 0)
	{
		if(neighborVertex[0] != 0)
		{
			for(int i = 3;i<VERTEX_DIM;i++)
				Combine_Vertices[Combine_Vertex_Index][i] = neighborWeight[0] * neighborVertex[0][i];	
		}

		if(neighborVertex[1] != 0)
		{
			for(int i = 3;i<VERTEX_DIM;i++)
				Combine_Vertices[Combine_Vertex_Index][i] += neighborWeight[1] * neighborVertex[1][i];	
		}

		if(neighborVertex[2] != 0)
		{
			for(int i = 3;i<VERTEX_DIM;i++)
				Combine_Vertices[Combine_Vertex_Index][i] += neighborWeight[2] * neighborVertex[2][i];	
		}

		if(neighborVertex[3] != 0)
		{
			for(int i = 3;i<VERTEX_DIM;i++)
				Combine_Vertices[Combine_Vertex_Index][i] += neighborWeight[3] * neighborVertex[3][i];	
		}
	}

    // return output data (vertex coords and others)
    *outData = Combine_Vertices[Combine_Vertex_Index];   // assign the address of new intersect vertex

    ++Combine_Vertex_Index;  // increase index for next vertex
	if(Combine_Vertex_Index >= 1024) 
		cout<<"Error: OnCombineCallback"<<endl;
	//cout<<Combine_Vertex_Index<<endl;
}

void _stdcall OnErrorCallback(unsigned int error_code)
{
	const GLubyte *errorStr;

    errorStr = gluErrorString(error_code);
    cerr << "[ERROR]: " << errorStr<<" code:"<<error_code << endl;
}

void FrameSolidFlowerbed::onMouseFunc(int button,int state,int x,int y)
{
	int lx,ly;
	this->globalPt2Local(lx,ly,x,y);

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		this->mouseOrgX = lx;
		this->mouseOrgY = ly;
		
		this->isLeftButtonDown = true;
	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		double dx = lx - this->mouseOrgX;
		double dy = ly - this->mouseOrgY;

		this->rotate_y_ += dx;
		this->rotate_x_ -= dy;

		this->isLeftButtonDown = false;
	}
}

void FrameSolidFlowerbed::onMotionFunc(int x, int y)
{
	int lx,ly;
	this->globalPt2Local(lx,ly,x,y);

	if(this->isLeftButtonDown)
	{
		double dx = lx - this->mouseOrgX;
		double dy = ly - this->mouseOrgY;

		this->rotate_y_ += dx;
		this->rotate_x_ -= dy;
	
		this->mouseOrgX = lx;
		this->mouseOrgY = ly;
	}
}

void FrameSolidFlowerbed::drawScene()
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
	gluPerspective(45,(double)width / (double) height,0.1,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera_pos_[0],camera_pos_[1],camera_pos_[2], 0,0,0, 0,1,0);
	glRotated(this->rotate_x_,1,0,0);
	glRotated(this->rotate_y_,0,1,0);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	/*int num = (int)(360 / this->angle_) / 2;
	for(int i = 0;i<num;i++)
	{
		glPushMatrix();
		glRotated(this->angle_ * i * 2,0,0,1);
	
		glPushMatrix();
		glScaled(0.75,0.75,1);
		glTranslated(-this->center_[0],-this->center_[1],0);
		this->DrawSolidSector();
		glPopMatrix();
	
		glPushMatrix();
		glScaled(1,-1,1);
		glScaled(0.75,0.75,1);
		glTranslated(-this->center_[0],-this->center_[1],0);
		this->DrawSolidSector();
		glPopMatrix();

		glPopMatrix();
	}*/
	
	/*if (this->list_id_ != 0)
		glCallList(this->list_id_);*/

	glPushMatrix();
	glTranslated(-3,-3,0);
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
	glEnd();
	glPopMatrix();
	
	glDisable(GL_DEPTH_TEST);
}

void FrameSolidFlowerbed::ComputeAllReflectionLines(FRSTree * tree,  std::vector<v2d> & to)
{
	if(tree == 0) return;
	
	to.clear();

	double	angle	= tree->fr_angle();
	int		num		= (int)(360.0 / angle);

	to.push_back(_v2d_(560,260));

	for(int i = 1;i<num;i++)
	{
		
		double	this_angle	= (angle * i) / 180.0 * 3.14159;
		v2d		tmp			= _v2d_(to[0][0] - center_[0],to[0][1] - center_[1]);
		double	x			= tmp[0] * cos(this_angle) - tmp[1] * sin(this_angle);
		double	y			= tmp[0] * sin(this_angle) + tmp[1] * cos(this_angle);
	
		to.push_back(_v2d_(x+center_[0],y+center_[1]));
	}
}

struct SS
{
	double d;
	int	   i;
};

bool SSGreater(const SS & s1, const SS & s2)
{
	return s1.d >= s2.d;
}



/* 
	As the SVGElement is cut by a line, the remaining segs may not be end to end
	For example, A->B->C->A is cut by a line, B and C are removed, A is kept
	Two new points D and E are introduced 
	Now connections are A->D and E->A
	D is different from E	

	This function is to change (A->D, E->A) to (E->A, A->D)

	Such cases will not appear: (A->B, A->C) or (B->A, C->A)
*/ 

void FrameSolidFlowerbed::OrderSVGElementSegs(SVGElement * e, std::vector<ElementSeg *> & segs)
{
	std::vector<std::vector<ElementSeg *>> eses;
	std::vector<ElementSeg*> es;
	
	for(size_t i = 0;i<e->segs_.size();i++)
	{
		/*v2d ppp = e->segs_[i]->FirstVertex();
		v2d ppp2 = e->segs_[i]->LastVertex();
		cout<<ppp[0]<<","<<ppp[1]<<"   "<<ppp2[0]<<","<<ppp2[1]<<endl;*/

		if(es.empty())
		{
			es.push_back(e->segs_[i]);
		}
		else
		{
			v2d p  = es[es.size()-1]->LastVertex();
			v2d pp = e->segs_[i]->FirstVertex();

			if(p[0] != pp[0] || p[1] != pp[1])
			{
				eses.push_back(es);
				es.clear();
				es.push_back(e->segs_[i]);
			}
			else
			{
				es.push_back(e->segs_[i]);
			}
		}
	}

	if(!es.empty()) eses.push_back(es);


		

	// sort
	bool flag = true;
	while(flag)
	{
		flag = false;

		std::vector<std::vector<ElementSeg*>> eeee;
		for(size_t i = 0;i<eses.size();i++)
		{
			bool this_flag = false;
			for(size_t j = 0;j<eeee.size();j++)
			{
				v2d A0 = eeee[j][0]->FirstVertex();
				v2d B0 = eeee[j][eeee[j].size()-1]->LastVertex();

				v2d A1 = eses[i][0]->FirstVertex();
				v2d B1 = eses[i][eses[i].size()-1]->LastVertex();

				// only two cases to be considered: (A->B,B->C), (A->B, C->A)
				if(B0[0] == A1[0] && B0[1] == A1[1])
				{
					for(size_t k = 0;k<eses[i].size();k++)
						eeee[j].push_back(eses[i][k]);
					flag = true;
					this_flag = true;
				}
				else if(B1[0] == A0[0] && B1[1] == A0[1])
				{
					for(size_t k = 0;k<eses[i].size();k++)
						eeee[j].insert(eeee[j].begin() + k,eses[i][k]);
					flag = true;
					this_flag = true;
				}
			}

			if(!this_flag)
				eeee.push_back(eses[i]);
		}

		eses.clear();
		for(size_t i = 0;i<eeee.size();i++)
			eses.push_back(eeee[i]);
	}

	if(eses.size() > 1) 
		cout<<"FrameSolidFlowerbed::OrderSVGElementSegs - should select i that maximizes eses[i].size()"<<endl;

	for(size_t i = 0;i<eses[0].size();i++)
		segs.push_back(eses[0][i]);
}

void FrameSolidFlowerbed::ComputeFRElementPoints(FRSTree * tree, std::vector<std::vector<v2d>> & pts)
{
	if(tree == 0) return;

	std::vector<SS> dist;
	
	// Compute the distance of the element to the origin of the pattern
	for(size_t i = 0;i<tree->pattern_->elements_.size();i++)
	{
		tree->pattern_->elements_[i]->computeCentroid();
		double x, y;
		tree->pattern_->elements_[i]->getCentroid(x,y);
		
		double x1 = x - 260;
		double y1 = y - 260;
		double d  = sqrt(x1 * x1 + y1 * y1);

		SS s;
		s.d = d;
		s.i = (int)(i);

		dist.push_back(s);
	}

	// Sort the elements by their distance to the pattern origin
	std::sort(dist.begin(),dist.end(),SSGreater);

	for(size_t i = 0;i<dist.size();i++)
	{
		int idx = dist[i].i;
		
		SVGElement * ele = tree->pattern_->elements_[idx];

		std::vector<ElementSeg*> segs;
		this->OrderSVGElementSegs(ele,segs);
		/*for(size_t j = 0;j<ele->segs_.size();j++)
			segs.push_back(ele->segs_[j]);*/

		if(segs.empty()) continue;
		
		/* all the points on the element */
		std::vector<v2d> pp;
		for(size_t j = 0;j<segs.size();j++)
		{
			std::vector<v2d> ppp;
			segs[j]->collectPoints(ppp,0.1);
			for(size_t k = 0;k<ppp.size();k++)
				pp.push_back(ppp[k]);
		}

		pts.push_back(pp);
	}
}

int FrameSolidFlowerbed::PointOnWhichLine(const v2d & pt, const v2d & p0, const v2d & p1, const v2d & p2)
{
	bool b1,b2;
	b1 = Geo2D::IsPointOnLineseg(pt,p0,p1,3);
	b2 = Geo2D::IsPointOnLineseg(pt,p0,p2,3);

	if(b1) return 1;
	else if(b2) return 2;
	else return -1;
}

void FrameSolidFlowerbed::Case12(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp)
{
	size_t num = pts.size();

	std::vector<v2d> p;

	for(size_t i = 0;i<pts.size();i++)
		p.push_back(pts[i]);

	for(size_t i = 1;i<this->to_.size();i++)
	{
		int j0 = (int)(p.size() - num);
		int j1 = (int)(p.size());

		for(int j = j1-1;j>=j0;j--)
		{
			v2d rf;
			Geo2D::PointReflectionOverLine(rf,p[j],this->center_,this->to_[i]);
			p.push_back(rf);
		}
	}

	pp.push_back(p);
}

void FrameSolidFlowerbed::Case21(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp)
{
	std::vector<v2d> rpts;
	if(pts.empty()) return;

	for(int i = (int)(pts.size() - 1);i>=0;i--)
		rpts.push_back(pts[i]);

	this->Case12(rpts,pp);
}

void FrameSolidFlowerbed::Case11(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp)
{
	std::vector<v2d> p;
	for(size_t i = 0;i<pts.size();i++)
		p.push_back(pts[i]);

	for(int i = (int)(p.size()-1); i>=0; i--)
	{
		v2d rf;
		Geo2D::PointReflectionOverLine(rf,p[i],this->center_,this->to_[0]);
		p.push_back(rf);
	}

	pp.push_back(p);

	for(size_t i = 1;i<this->to_.size();i+=2)
	{
		p.clear();
		for(size_t j = 0;j<pp[pp.size() - 1].size();j++)
		{
			v2d rf;
			Geo2D::PointReflectionOverLine(rf,pp[pp.size()-1][j],this->center_,this->to_[i]);
			p.push_back(rf);
		}
		pp.push_back(p);
	}
}

void FrameSolidFlowerbed::Case22(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp)
{
	std::vector<v2d> p;
	for(size_t i = 0;i<pts.size();i++)
		p.push_back(pts[i]);

	for(int i = (int)(p.size()-1); i>=0; i--)
	{
		v2d rf;
		Geo2D::PointReflectionOverLine(rf,p[i],this->center_,this->to_[1]);
		p.push_back(rf);
	}

	pp.push_back(p);

	for(size_t i = 2;i<this->to_.size();i+=2)
	{
		p.clear();
		for(size_t j = 0;j<pp[pp.size() - 1].size();j++)
		{
			v2d rf;
			Geo2D::PointReflectionOverLine(rf,pp[pp.size()-1][j],this->center_,this->to_[i]);
			p.push_back(rf);
		}
		pp.push_back(p);
	}
}
	
void FrameSolidFlowerbed::CaseOther(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp)
{
	std::vector<v2d> p;
	for(size_t i = 0;i<pts.size();i++)
		p.push_back(pts[i]);
	pp.push_back(p);

	for(size_t i = 1;i<this->to_.size();i++)
	{
		p.clear();
		for(size_t j = 0;j<pp[pp.size() - 1].size();j++)
		{
			v2d rf;
			Geo2D::PointReflectionOverLine(rf,pp[pp.size() - 1][j],this->center_,this->to_[i]);
			p.push_back(rf);
		}
		pp.push_back(p);
	}
}

void FrameSolidFlowerbed::ComputeOneCompleteElement(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & complete, int & is_closed)
{
	is_closed = -1;

	if(pts.empty()) return;

	v2d p0,p1;
	p0 = pts[0];
	p1 = pts[pts.size() - 1];
	
	int a0,a1;
	a0 = this->PointOnWhichLine(p0,this->center_,this->to_[0],this->to_[1]);
	a1 = this->PointOnWhichLine(p1,this->center_,this->to_[0],this->to_[1]);

	/*static int index = 0;
	char filename[1024];
	sprintf_s(filename,1024,"D:/test/test%d.png",index);
	IplImage * img = cvCreateImage(cvSize(1000,1000),IPL_DEPTH_8U,3);
	cvZero(img);

	cvLine(img,cvPoint(center_[0],center_[1]),cvPoint(to_[0][0],to_[0][1]),cvScalar(0,0,255));
	cvLine(img,cvPoint(center_[0],center_[1]),cvPoint(to_[1][0],to_[1][1]),cvScalar(0,0,255));
	
	cvCircle(img,cvPoint(p0[0],p0[1]),5,cvScalar(0,255,0));
	cvCircle(img,cvPoint(p1[0],p1[1]),5,cvScalar(255,0,0));

	cvSaveImage(filename,img);

	index++;
	cvReleaseImage(&img);*/

	if(a0 == 1 && a1 == 2)
	{
		this->Case12(pts,complete);
		//cout<<"Case12"<<endl;
		is_closed = 1;
	}
	else if(a0 == 2 && a1 == 1)
	{
		this->Case21(pts,complete);
		//cout<<"Case21"<<endl;
		is_closed = 1;
	}
	else if(a0 == 1 && a1 == 1)
	{
		this->Case11(pts,complete);
		//cout<<"Case11"<<endl;
		is_closed = 1;
	}
	else if(a0 == 2 && a1 == 2)
	{
		this->Case22(pts,complete);
		//cout<<"Case22"<<endl;
		is_closed = 1;
	}
	else
	{
		double x = p0[0] - p1[0];
		double y = p0[1] - p1[1];

		double r = sqrt(x * x + y * y);
		
		if(r > 2)
			is_closed = -1;
		else 
			is_closed = 1;
		this->CaseOther(pts,complete);
		//cout<<"CaseOther"<<endl;
	}
}

void FrameSolidFlowerbed::ComputeCompleteElementForFlowerbed(const std::vector<std::vector<v2d>> & pts, std::vector<std::vector<std::vector<v2d>>> & complete, std::vector<int> & is_closed)
{
	for(size_t i = 0;i<pts.size();i++)
	{
		std::vector<std::vector<v2d>> pp;
		int c;
		this->ComputeOneCompleteElement(pts[i],pp,c);
		complete.push_back(pp);
		is_closed.push_back(c);
	}
}

void FrameSolidFlowerbed::RefreshPattern(FRSTree * tree)
{
	//cout<<"RefreshPattern"<<endl;
	this->ComputeAllReflectionLines(tree,this->to_);

	this->fr_.clear();
	this->complete_.clear();
	this->is_closed_.clear();

	// The solid flowerbed is rendered using points
	// The SVGElement which is composed of lines should be converted to points
	this->ComputeFRElementPoints(tree,this->fr_);
	this->ComputeCompleteElementForFlowerbed(this->fr_,this->complete_,this->is_closed_);

	/*if(!this->complete_.empty())
	{
		for(size_t i = 0;i<this->complete_.size();i++)
		{
			char filename[1024];
			sprintf_s(filename,1024,"D:/test%d.png",i);
			IplImage * img = this->ShowShapeInImage(this->complete_[i][0]);
			cvSaveImage(filename,img);
			cvReleaseImage(&img);
		}
	}*/
}

void FrameSolidFlowerbed::DrawSolidFlowerbed()
{
	//glBegin(GL_LINES);
	//glVertex3d(center_[0],center_[1],0);
	//glVertex3d(to_[1][0],to_[1][1],0);
	//glEnd();

	GLUtesselator * tess = gluNewTess();
	gluTessCallback(tess,GLU_TESS_BEGIN,(void (_stdcall*)(void))OnBeginCallback);
	gluTessCallback(tess,GLU_TESS_END,(void (_stdcall*)(void))OnEndCallback);
	gluTessCallback(tess,GLU_TESS_VERTEX,(void (_stdcall*)(void))OnVertexCallback);
	gluTessCallback(tess,GLU_TESS_COMBINE,(void (_stdcall*)(void))OnCombineCallback);
	gluTessCallback(tess,GLU_TESS_ERROR,(void (_stdcall*)(void))OnErrorCallback);

	//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NEGATIVE);

	double	height				= 0.1;
	//double	top_color[3]		= { 0.1,0.1,0.1};

	/*double top_color[6][3] = { {60.0/255,100.0/255,30.0/255},
		{220.0/255,220.0/255,30.0/255},
		{210.0/255,220.0/255,220.0/255},
		{120.0/255,70.0/255,150.0/255},
		{160.0/255,220.0/255,100.0/255},
		{187.0/255,28.0/255,35.0/255}
	};*/

/*	double top_color[7][3] = { 
		{48.0/255,32.0/255,94.0/255},
		{0.0/255,91.0/255,162.0/255},
		{5.0/255,108.0/255,55.0/255},
		{3.0/255,156.0/255,74.0/255},
		{253.0/255,231.0/255,0.0/255},
		{228.0/255,127.0/255,37.0/255},
		{238.0/255,39.0/255,36.0/255}
	};
*/


	double top_color[7][3] = { 
		{34.0/255,198.0/255,200.0/255},
		{135.0/255,97.0/255,154.0/255},
		{80.0/255,129.0/255,185.0/255},
		{185.0/255,55.0/255,21.0/255},
		{132.0/255,218.0/255,69.0/255},
		{226.0/255,20.0/255,146.0/255},
		{243.0/255,212.0/255,69.0/255}
	};

/*	double top_color[7][3] = { 
		{0.0/255,0.0/255,0.0/255},
		{20.0/255,20.0/255,20.0/255},
		{60.0/255,60.0/255,60.0/255},
		{100.0/255,100.0/255,100.0/255},
		{140.0/255,140.0/255,140.0/255},
		{180.0/255,180.0/255,180.0/255},
		{220.0/255,220.0/255,220.0/255}
	};
*/

	int	texture_id	= 0;
	int color_id	= 0;
	for(size_t i = 0;i<this->complete_.size();i++)
	//for(int i = this->complete_.size()-1;i>=0;i--)
	{
		for(size_t j = 0;j<this->complete_[i].size();j++)
		{
			this->DrawModelFromShape(tess,this->complete_[i][j],height,this->is_closed_[i] == 1,top_color[color_id],texture_id);
		}
		height += 0.1;
		
		if(this->is_closed_[i] == 1)
		{
			texture_id++;
			if(texture_id > 7) texture_id = 0;
			color_id++;
			if(color_id > 6) color_id = 0; 
		}
	}
	gluDeleteTess(tess);
}


//void FrameSolidFlowerbed::RefreshPattern(FRSTree * tree)
//{
//	//cout<<"RefreshPattern"<<endl;
//	this->ComputeAllReflectionLines(tree,this->to_);
//
//	this->fr_.clear();
//	this->complete_.clear();
//	this->is_closed_.clear();
//
//	this->ComputeFRElementPoints(tree,this->fr_);
//	this->ComputeCompleteElementForFlowerbed(this->fr_,this->complete_,this->is_closed_);
//
//	//if(!this->complete_.empty())
//	//{
//	//	IplImage * img = this->ShowShapeInImage(this->complete_[0][0]);
//	//	cvSaveImage("D:/test.png",img);
//	//	cvReleaseImage(&img);
//	//}
//	
//	if(this->list_id_ != 0)
//		glDeleteLists(this->list_id_,1);
//	this->list_id_ = glGenLists(1);
//
//	glNewList(this->list_id_,GL_COMPILE);
//
//	//glBegin(GL_LINES);
//	//glVertex3d(center_[0],center_[1],0);
//	//glVertex3d(to_[1][0],to_[1][1],0);
//	//glEnd();
//
//	GLUtesselator * tess = gluNewTess();
//	gluTessCallback(tess,GLU_TESS_BEGIN,(void (_stdcall*)(void))OnBeginCallback);
//	gluTessCallback(tess,GLU_TESS_END,(void (_stdcall*)(void))OnEndCallback);
//	gluTessCallback(tess,GLU_TESS_VERTEX,(void (_stdcall*)(void))OnVertexCallback);
//	gluTessCallback(tess,GLU_TESS_COMBINE,(void (_stdcall*)(void))OnCombineCallback);
//	gluTessCallback(tess,GLU_TESS_ERROR,(void (_stdcall*)(void))OnErrorCallback);
//
//	//gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NEGATIVE);
//
//	double	height				= 0.1;
//	double	top_color[3]		= { 0.1,0.1,0.1};
//	int		texture_id			= 0;
//	for(size_t i = 0;i<this->complete_.size();i++)
//	{
//		for(size_t j = 0;j<this->complete_[i].size();j++)
//		{
//			this->DrawModelFromShape(tess,this->complete_[i][j],height,this->is_closed_[i] == 1,top_color,texture_id);
//		}
//		height += 0.1;
//		
//		if(this->is_closed_[i] == 1)
//		{
//			texture_id++;
//			if(texture_id > 7) texture_id = 0;
//			top_color[0] += 0.1;
//			top_color[1] += 0.1;
//			top_color[2] += 0.1;
//		}
//	}
//	
//
//	gluDeleteTess(tess);
//
//	glEndList();
//}


/* 2D shape to 3D model */

void FrameSolidFlowerbed::Line2SideRect(SideRect & sr, const v2d & v0, const v2d & v1, double height)
{
	sr.p[0] = _v3d_(v0[0],v0[1],0);
	sr.p[1] = _v3d_(v1[0],v1[1],0);
	sr.p[2] = _v3d_(v1[0],v1[1],height);
	sr.p[3] = _v3d_(v0[0],v0[1],height);
}

void FrameSolidFlowerbed::DrawSideRect(const SideRect & sr, GLUtesselator * tess)
{
	double color[3] = {0.2,0.2,0.2};
	double vd[4][VERTEX_DIM];
	for(int i = 0;i<4;i++)
	{
		vd[i][0] = sr.p[i][0];
		vd[i][1] = sr.p[i][1];
		vd[i][2] = sr.p[i][2];
		vd[i][3] = color[0];
		vd[i][4] = color[1];
		vd[i][5] = color[2];
		vd[i][6] = 0;
		vd[i][7] = 0;
	}

	Combine_Vertex_Index = 0;
	gluTessBeginPolygon(tess,0);
		gluTessBeginContour(tess);
			for(int i = 0;i<4;i++)
				gluTessVertex(tess,vd[i],vd[i]);
		gluTessEndContour(tess);
	gluTessEndPolygon(tess);
}

void FrameSolidFlowerbed::DrawModelTop(GLUtesselator * tess, const std::vector<v3d> & top, const std::vector<v2d> & top_tex_coords, double color[3],int texture_id)
{
	if(top.size() < 3) return;
	
	double * vd = new double[top.size() * VERTEX_DIM];
	for(size_t i = 0;i<top.size();i++)
	{
		vd[i * VERTEX_DIM]		= top[i][0];
		vd[i * VERTEX_DIM + 1]	= top[i][1];
		vd[i * VERTEX_DIM + 2]	= top[i][2];
		vd[i * VERTEX_DIM + 3]	= color[0];
		vd[i * VERTEX_DIM + 4]	= color[1];
		vd[i * VERTEX_DIM + 5]	= color[2];
		vd[i * VERTEX_DIM + 6]	= top_tex_coords[i][0];
		vd[i * VERTEX_DIM + 7]	= top_tex_coords[i][1];
	}

	//glBindTexture(GL_TEXTURE_2D,TextureLoader::Instance()->getTextureId(texture_id));
	//glEnable(GL_TEXTURE_2D);

	Combine_Vertex_Index = 0;
	gluTessBeginPolygon(tess,0);
		gluTessBeginContour(tess);
			for(size_t i = 0;i<top.size();i++)
				gluTessVertex(tess,vd + i * VERTEX_DIM,vd + i * VERTEX_DIM);
		gluTessEndContour(tess);
	gluTessEndPolygon(tess);

	glDisable(GL_TEXTURE_2D);

	delete [] vd;
}

void FrameSolidFlowerbed::Shape2Model(const std::vector<v2d> & shape, std::vector<SideRect> & sides, std::vector<v3d> & top, std::vector<v2d> & top_tex_coords, double height)
{
	if(shape.size() < 2) return;

	top.push_back(_v3d_(shape[0][0],shape[0][1],height));

	for(size_t i = 1;i<shape.size();i++)
	{
		v2d v0 = shape[i-1];
		v2d v1 = shape[i];

		SideRect sr;

		this->Line2SideRect(sr,v0,v1,height);

		sides.push_back(sr);
		top.push_back(_v3d_(v1[0],v1[1],height));
	}

	double minx = 100000, miny = 100000, maxx = -100000, maxy = -100000;
	for(size_t i = 0;i<top.size();i++)
	{
		v3d pt = top[i];
		if(pt[0] < minx) minx = pt[0];
		if(pt[0] > maxx) maxx = pt[0];
		if(pt[1] < miny) miny = pt[1];
		if(pt[1] > maxy) maxy = pt[1];
	}

	double xx = maxx-minx;
	double yy = maxy-miny;

	for(size_t i =0;i<top.size();i++)
	{
		v3d pt = top[i];
		v2d coor;

		coor[0] = (pt[0] - minx) / xx;
		coor[1] = (pt[1] - miny) / yy;

		top_tex_coords.push_back(coor);
	}
}


void FrameSolidFlowerbed::DrawModelFromShape(GLUtesselator * tess, const std::vector<v2d> & shape, double height, bool is_closed, double top_color[3],int texture_id)
{
	// If the shape is not closed, directly exit the rendering
	if(!is_closed) return;

	std::vector<SideRect> sides;
	std::vector<v3d>	  top;
	std::vector<v2d>	  top_tex_coords;
	this->Shape2Model(shape,sides,top,top_tex_coords,height);

	for(size_t i = 0;i<sides.size();i++)
		this->DrawSideRect(sides[i],tess);
	
	this->DrawModelTop(tess,top,top_tex_coords,top_color,texture_id);
}

IplImage * FrameSolidFlowerbed::ShowShapeInImage(const std::vector<v2d> & shape)
{
	/*double minx = 10000, miny = 10000, maxx = -10000, maxy = -10000;

	for(size_t i = 0;i<shape.size();i++)
	{
		v2d pt = shape[i];
		if(pt[0] < minx) minx = pt[0];
		if(pt[0] > maxx) maxx = pt[0];
		if(pt[1] < miny) miny = pt[1];
		if(pt[1] > maxy) maxy = pt[1];
	}*/

	//int width = (int)(maxx - minx + 10);
	//int height = (int)(maxy - miny + 10);

	int width = 800;
	int height = 800;

	IplImage * img = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);

	cvZero(img);

	for(size_t i = 1;i<shape.size();i++)
	{
		v2d p0 = shape[i-1];
		v2d p1 = shape[i];

		//p0 = p0 - _v2d_(minx,miny);
		//p1 = p1 - _v2d_(minx,miny);

		cvLine(img,cvPoint((int)(p0[0]),(int)(p0[1])),
			cvPoint((int)(p1[0]),(int)(p1[1])),cvScalar(255,0,0));
	}

	return img;
}

void FrameSolidFlowerbed::DrawPanorama()
{
	int coords[15][2] = { 
		{221,88} ,
		{152,194},
		{194,270},
		{292,227},
		{214,322},
		{223,414},
		{259,489},
		{190,475},
		{51,470},
		{59,382},
		{146,356},
		{80,256},
		{36,158},
		{100,70},
		{2,7}
	};

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameManifold * frame_mani = (FrameManifold*)(main_frame->getFrameByName("Manifold"));
	FrameStructureBlend * frame_blend = (FrameStructureBlend*)(main_frame->getFrameByName("FrameView"));

	if(frame_mani == 0) return;
	if(frame_blend == 0) return;

	std::vector<v2d> pts;

	for(int i = 0;i<14;i++)
	{
		v2d p0,p1;
		p0 = _v2d_(coords[i][0],500 - coords[i][1] + 1);
		p1 = _v2d_(coords[i+1][0],500 - coords[i+1][1] + 1);

		for(int j = 0;j<3;j++)
		{
			double t = (double)j / 3;
			v2d tp;

			Geo2D::InterpLinear(tp,t,p0,p1);
			pts.push_back(tp);
		}

	}

	std::vector<v2d> ppts;
	for(int i = 0;i<20;i++)
	{
		for(size_t j = 0;j<pts.size();j++)
		{
			ppts.push_back(pts[j]);	
		}
	}

	for(int i = 0;i<(int)(ppts.size());i++)
	{
		frame_mani->set_manifold_x((int)(ppts[i][0]));
		frame_mani->set_manifold_y((int)(ppts[i][1]));

		frame_blend->BuildFRSTree();
		this->RefreshPattern(frame_blend->gen_frs_tree_);


		glPushMatrix();

		double ii = i / 30;
		double jj = i % 30 - 15;


		glTranslated(jj * 500,ii * 500,0);

		this->DrawSolidFlowerbed();

		glPopMatrix();
	}
}

void FrameSolidFlowerbed::RefreshPatternJustOneSector(FRSTree * tree)
{
	this->fr_.clear();
	angle_ = tree->fr_angle();
	center_ = _v2d_(260,260);
	

	// The solid flowerbed is rendered using points
	// The SVGElement which is composed of lines should be converted to points
	this->ComputeFRElementPoints(tree,this->fr_);


	v2d pp;
	pp[0] = 260 + 280 * cos(angle_ / 180 * 3.14159);
	pp[1] = 260 + 280 * sin(angle_ / 180 * 3.14159);
	
	for(size_t i = 0;i<this->fr_.size();i++)
	{
		this->StitchElementAndFRSTreeShell(fr_[i],0				 ,_v2d_(260,260),_v2d_(540,260),pp);
		this->StitchElementAndFRSTreeShell(fr_[i],fr_[i].size()-1,_v2d_(260,260),_v2d_(540,260),pp);
	}

	/*for(size_t i = 0;i<fr_.size();i++)
	{
		char filename[1024];
		sprintf_s(filename,1024,"F:/kk%d.png",i);
		
		IplImage * img = this->ShowShapeInImage(fr_[i]);
		cvSaveImage(filename,img);
		cvReleaseImage(&img);

	}*/
}

void FrameSolidFlowerbed::DrawSolidElement(GLUtesselator * tess, const std::vector<v2d> & pts, double height, double color[3])
{
	v3d sector_origin = _v3d_(260,260,height);
	std::vector<v3d> pts_3d;

	// draw element with origin
	pts_3d.push_back(sector_origin);	

	for(size_t i = 0;i<pts.size();i++)
		pts_3d.push_back(_v3d_(pts[i][0],pts[i][1],height));
	
	
	double * vd = new double[pts_3d.size() * VERTEX_DIM];
	for(size_t i = 0;i<pts_3d.size();i++)
	{
		vd[i * VERTEX_DIM]		= pts_3d[i][0];
		vd[i * VERTEX_DIM + 1]	= pts_3d[i][1];
		vd[i * VERTEX_DIM + 2]	= pts_3d[i][2];
		vd[i * VERTEX_DIM + 3]	= color[0];
		vd[i * VERTEX_DIM + 4]	= color[1];
		vd[i * VERTEX_DIM + 5]	= color[2];
		vd[i * VERTEX_DIM + 6]	= 0;
		vd[i * VERTEX_DIM + 7]	= 0;
	}

	//glBindTexture(GL_TEXTURE_2D,TextureLoader::Instance()->getTextureId(texture_id));
	//glEnable(GL_TEXTURE_2D);

	Combine_Vertex_Index = 0;
	gluTessBeginPolygon(tess,0);
		gluTessBeginContour(tess);
			for(size_t i = 0;i<pts_3d.size();i++)
				gluTessVertex(tess,vd + i * VERTEX_DIM,vd + i * VERTEX_DIM);
		gluTessEndContour(tess);
	gluTessEndPolygon(tess);

	//glDisable(GL_TEXTURE_2D);

	delete [] vd;
}

void FrameSolidFlowerbed::DrawSolidSector()
{
	GLUtesselator * tess = gluNewTess();
	gluTessCallback(tess,GLU_TESS_BEGIN,(void (_stdcall*)(void))OnBeginCallback);
	gluTessCallback(tess,GLU_TESS_END,(void (_stdcall*)(void))OnEndCallback);
	gluTessCallback(tess,GLU_TESS_VERTEX,(void (_stdcall*)(void))OnVertexCallback);
	gluTessCallback(tess,GLU_TESS_COMBINE,(void (_stdcall*)(void))OnCombineCallback);
	gluTessCallback(tess,GLU_TESS_ERROR,(void (_stdcall*)(void))OnErrorCallback);

	double color[7][3] = { 
		{34.0/255,198.0/255,200.0/255},
		{135.0/255,97.0/255,154.0/255},
		{80.0/255,129.0/255,185.0/255},
		{185.0/255,55.0/255,21.0/255},
		{132.0/255,218.0/255,69.0/255},
		{226.0/255,20.0/255,146.0/255},
		{243.0/255,212.0/255,69.0/255}
	};

	double	height		= 0.1;

	int		color_id	= 0;

	for(size_t i = 0;i<this->fr_.size();i++)
	{
		this->DrawSolidElement(tess,fr_[i],height,color[color_id]);
		height += 0.1;
		
		color_id++;
		if(color_id > 6) color_id = 0; 
	}

	gluDeleteTess(tess);
}

void FrameSolidFlowerbed::StitchElementAndFRSTreeShell(std::vector<v2d> & pts, v2d p0, v2d p1)
{
	v2d p = pts[0];
	v2d nearest_v; 
	double dist = Geo2D::DistanceBetweenPointLineseg(nearest_v,p,p0,p1);
	//cout<<"("<<p[0]<<","<<p[1]<<"),"<<dist<<",";
	if(dist < 10) 
		pts[0] = nearest_v;
	
	p = pts[pts.size()-1];
	dist = Geo2D::DistanceBetweenPointLineseg(nearest_v,p,p0,p1);
	//cout<<"("<<p[0]<<","<<p[1]<<"),"<<dist<<endl;
	if(dist < 10)
		pts[pts.size()-1] = nearest_v;
}

void FrameSolidFlowerbed::StitchElementAndFRSTreeShell(std::vector<v2d> & pts, int index, v2d p0, v2d p1, v2d p2)
{
	v2d p = pts[index];
	v2d nearest_v1; 
	v2d nearest_v2;
	double dist1 = Geo2D::DistanceBetweenPointLineseg(nearest_v1,p,p0,p1);
	double dist2 = Geo2D::DistanceBetweenPointLineseg(nearest_v2,p,p0,p2);
	
	if(dist1 < dist2 && dist1 < 20)
		pts[index] = nearest_v1;
	if(dist2 < dist1 && dist2 < 20)
		pts[index] = nearest_v2;
}