#include "Geo2D.h"

#include "SVGPattern.h"
#include "SVGElement.h"

#include "Deform2D\TriangleMesh.h"

extern "C"
{
#include "Triangle\triangle.h"
}

#include "FRNode.h"

#ifdef ANSI_DECLARATORS
extern "C" void triangulate(char *, struct triangulateio *, struct triangulateio *,
                 struct triangulateio *);
extern "C" void trifree(VOIDD *memptr);
#else /* not ANSI_DECLARATORS */
void triangulate();
void trifree();
#endif /* not ANSI_DECLARATORS */

void Geo2D::PointReflectionOverLine(v2d & v, const v2d & p, const v2d & v1, const v2d & v2)
{
	double x0,y0,A,B,C;
	
	x0 = p[0];
	y0 = p[1];

	LineParams(A,B,C,v1,v2);

	double e, f, g;
	
	e = A * x0 + B * y0 + C;

	if(e == 0)
	{
		v[0] = p[0];
		v[1] = p[1];
	}
	else
	{
		f = -e - C;
		g = A * y0 - B * x0;

		v[0] = (f*A - g*B) / (A*A + B*B);
		v[1] = (f*B + g*A) / (A*A + B*B);
	}
}

SVGPattern * Geo2D::PatternReflectionOverLine(SVGPattern * pattern, const v2d & v1, const v2d & v2)
{
	SVGPattern * np = new SVGPattern();

	for(size_t i = 0;i<pattern->elements_.size();i++)
	{
		SVGElement * es = pattern->elements_[i];
		SVGElement * et = new SVGElement();
		
		for(size_t j = 0;j<es->segs_.size();j++)
		{
			ElementSeg * segs = es->segs_[j];
			if(segs->SegType() == 0)
			{
				ElementSegLine * segt = new ElementSegLine();
				PointReflectionOverLine(segt->pts_[0],((ElementSegLine*)segs)->pts_[0],v1,v2);
				PointReflectionOverLine(segt->pts_[1],((ElementSegLine*)segs)->pts_[1],v1,v2);
				et->segs_.push_back(segt);
			}
			else
			{
				ElementSegBezier * segt = new ElementSegBezier();
				PointReflectionOverLine(segt->pts_[0],((ElementSegLine*)segs)->pts_[0],v1,v2);
				PointReflectionOverLine(segt->pts_[1],((ElementSegLine*)segs)->pts_[1],v1,v2);
				PointReflectionOverLine(segt->pts_[2],((ElementSegLine*)segs)->pts_[2],v1,v2);
				PointReflectionOverLine(segt->pts_[3],((ElementSegLine*)segs)->pts_[3],v1,v2);
				et->segs_.push_back(segt);
			}
		}
		
		np->elements_.push_back(et);
	}

	return np;
}

void Geo2D::LineParams(double & A, double & B, double & C, const v2d & v1, const v2d & v2)
{
	A = v2[1] - v1[1];
	B = v1[0] - v2[0];
	C = v1[0] * (-A) + v1[1] * (-B);
}

bool Geo2D::IsPointOnLineseg(const v2d & v, const v2d & v1, const v2d & v2, double precision)
{
	double A,B,C;
	LineParams(A,B,C,v1,v2);

	double d = (A * v[0] + B * v[1] + C) / sqrt(A * A + B * B);

	/*cout<<A<<"x + "<<B<<"y + "<<C<<endl;
	cout<<v1[0]<<","<<v1[1]<<endl;
	cout<<v2[0]<<","<<v2[1]<<endl;
	cout<<v[0]<<","<<v[1]<<endl;
	cout<<d<<endl;*/

	if(fabs(d) > precision) return false; // not on line
	else
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
}

void Geo2D::BezierParams(v4d & coex, v4d & coey, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	double x0,y0,x1,y1,x2,y2,x3,y3;
	
	x0 = p0[0];
	y0 = p0[1];

	x1 = p1[0];
	y1 = p1[1];

	x2 = p2[0];
	y2 = p2[1];

	x3 = p3[0];
	y3 = p3[1];

	double ax,bx,cx;
	double ay,by,cy;

	cx = 3 * (x1 - x0);
	bx = 3 * (x2 - x1) - cx;
	ax = x3 - x0 - cx - bx;

	cy = 3 * (y1 - y0);
	by = 3 * (y2 - y1) - cy;
	ay = y3 - y0 - cy - by;

	coex = _v4d_(ax,bx,cx,x0);
	coey = _v4d_(ay,by,cy,y0);
}

double Geo2D::DistanceBetweenPointLineseg(v2d & v, const v2d & p, const v2d & v1, const v2d & v2)
{
	v2d pf, pol;
	PointReflectionOverLine(pf,p,v1,v2);
	pol = _v2d_((p[0] + pf[0])/2, (p[1] + pf[1])/2);
	
	if(IsPointOnLineseg(pol,v1,v2))
	{
		v = pol;
		return dist_v2d(p,pol);
	}
	else
	{
		double d1 = dist_v2d(p,v1);
		double d2 = dist_v2d(p,v2);
		if(d1 < d2)
		{
			v = v1;
			return d1;
		}
		else
		{
			v = v2;
			return d2;
		}
	}
}

double Geo2D::DistanceBetweenPointTriangle(const v2d & p, const v2d & p0, const v2d & p1, const v2d & p2)
{
	v2d v;
	double d1 = Geo2D::DistanceBetweenPointLineseg(v,p,p0,p1);
	double d2 = Geo2D::DistanceBetweenPointLineseg(v,p,p0,p2);
	double d3 = Geo2D::DistanceBetweenPointLineseg(v,p,p1,p2);

	double m1 = d1 < d2 ? d1 : d2;
	double m2 = m1 < d3 ? m1 : d3;
	
	return m2;
}

double Geo2D::DistanceBetweenPointBezier(v2d & v, const v2d & p, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	v4d coex, coey;
	BezierParams(coex,coey,p0,p1,p2,p3);

	double mind = 1e10;

	for(double t = 0;t<=1;t+=0.01)
	{
		double x = coex[0] * t * t * t + coex[1] * t * t + coex[2] * t + coex[3];
		double y = coey[0] * t * t * t + coey[1] * t * t + coey[2] * t + coey[3];
		double d = dist_v2d(_v2d_(x,y),p);
		if(d < mind)
		{
			mind = d;
			v = _v2d_(x,y);
		}
	}
	
	return mind;
}

void Geo2D::InitTriangulateio(triangulateio ** pptio)
{
	(*pptio) = (triangulateio*)malloc(sizeof(triangulateio));

	struct triangulateio * tio = (*pptio);

	tio->pointlist = 0;                      
	tio->pointattributelist = 0;             
	tio->pointmarkerlist = 0;                
	tio->numberofpoints = 0;                 
	tio->numberofpointattributes = 0;        

	tio->trianglelist = 0;                   
	tio->triangleattributelist = 0;          
	tio->trianglearealist = 0;               
	tio->neighborlist = 0;                   
	tio->numberoftriangles = 0;              
	tio->numberofcorners = 0;                
	tio->numberoftriangleattributes = 0;     

	tio->segmentlist = 0;                    
	tio->segmentmarkerlist = 0;              
	tio->numberofsegments = 0;               

	tio->holelist = 0;                       
	tio->numberofholes = 0;                  

	tio->regionlist = 0;                     
	tio->numberofregions = 0;                

	tio->edgelist = 0;               
	tio->edgemarkerlist = 0;         
	tio->normlist = 0;               
	tio->numberofedges = 0;          
}

void Geo2D::FreeTriangulateio(triangulateio ** pptio, bool in)
{
	struct triangulateio * tio = (*pptio);
	if(tio->pointlist != 0) free(tio->pointlist);                     
	if(tio->pointattributelist != 0) free(tio->pointattributelist);
	if(tio->pointmarkerlist != 0) free(tio->pointmarkerlist);               
	
	if(tio->trianglelist != 0) free(tio->trianglelist);                  
	if(tio->triangleattributelist != 0) free(tio->triangleattributelist);          
	if(tio->trianglearealist != 0) free(tio->trianglearealist);               
	if(tio->neighborlist != 0) free(tio->neighborlist);                   
	
	if(tio->segmentlist != 0) free(tio->segmentlist);                    
	if(tio->segmentmarkerlist != 0) free(tio->segmentmarkerlist);             

	if(in) // only allocalte mem for "in" triangulateio
		if(tio->holelist != 0) 
			free(tio->holelist);                      

	if(in) // only allocalte mem for "in" triangulateio
		if(tio->regionlist != 0) 
			free(tio->regionlist);                     

	if(tio->edgelist != 0) free(tio->edgelist);               
	if(tio->edgemarkerlist != 0) free(tio->edgemarkerlist);        
	if(tio->normlist != 0) free(tio->normlist);              

	free(*pptio);
	(*pptio) = 0;
}

triangulateio * Geo2D::InputToTriangulateio(const std::vector<v2d> & input_points, const std::vector<v2i> & input_segments)
{
	struct triangulateio * ans;
	Geo2D::InitTriangulateio(&ans);
	
	ans->numberofpoints = (int)(input_points.size());

	if(ans->numberofpoints == 0) return ans;
	
	ans->pointlist = (REAL *) malloc(ans->numberofpoints * 2 * sizeof(REAL));

	for(int i = 0;i<ans->numberofpoints;i++)
	{
		ans->pointlist[i*2]	 = input_points[i][0];
		ans->pointlist[i*2+1]= input_points[i][1];
	}

	ans->numberofsegments = (int)(input_segments.size());

	ans->segmentlist = (int *)malloc(ans->numberofsegments * 2 * sizeof(int));

	for(int i = 0;i<ans->numberofsegments;i++)
	{
		ans->segmentlist[i*2]   = input_segments[i][0];
		ans->segmentlist[i*2+1] = input_segments[i][1];
	}

	return ans;
}

triangulateio * Geo2D::ComputeMeshByTriangle(triangulateio * tio)
{
	struct triangulateio * ans, * vorout;
	Geo2D::InitTriangulateio(&ans);
	Geo2D::InitTriangulateio(&vorout);

	triangulate("zpqa500Q",tio,ans,vorout);

	Geo2D::FreeTriangulateio(&vorout);

	return ans;
}

void Geo2D::TriangulateioToOutput(triangulateio * tio, rmsmesh::TriangleMesh & mesh)
{
	for(int i = 0;i<tio->numberofpoints;i++)
	{
		Wml::Vector3f vert((float)(tio->pointlist[i*2]), (float)(tio->pointlist[i*2+1]),0);	
		mesh.AppendVertexData(vert);
	}

	for(int i = 0;i<tio->numberoftriangles;i++)
	{
		unsigned int tri[3] = { tio->trianglelist[i * 3] , tio->trianglelist[i * 3 + 1], tio->trianglelist[i * 3 + 2] };
		mesh.AppendTriangleData(tri);
	}
}

void Geo2D::InterpLinear(v2d & v, double t, const v2d & v0, const v2d & v1)
{
	v[0] = v0[0] + t * (v1[0] - v0[0]);
	v[1] = v0[1] + t * (v1[1] - v0[1]);
}

void Geo2D::SplitBezier(std::vector<v2d> & b1, std::vector<v2d> & b2, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	v2d p4,p5,p6,p7,p8,p9;
	
	Geo2D::InterpLinear(p4,t,p0,p1);
	Geo2D::InterpLinear(p5,t,p1,p2);
	Geo2D::InterpLinear(p6,t,p2,p3);
	Geo2D::InterpLinear(p7,t,p4,p5);
	Geo2D::InterpLinear(p8,t,p5,p6);
	Geo2D::InterpLinear(p9,t,p7,p8);

	b1.push_back(p0);
	b1.push_back(p4);
	b1.push_back(p7);
	b1.push_back(p9);

	b2.push_back(p9);
	b2.push_back(p8);
	b2.push_back(p6);
	b2.push_back(p3);
}

void Geo2D::SplitBezier(std::vector<std::vector<v2d>> & beziers, const std::vector<v2d> & pts, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	if(pts.empty()) 
	{
		std::vector<v2d> b;
		b.push_back(p0);
		b.push_back(p1);
		b.push_back(p2);
		b.push_back(p3);
		beziers.push_back(b);
		return;
	}

	double t;
	std::vector<v2d> b1,b2;

	t = Geo2D::tOfPointOnBezier(pts[0],p0,p1,p2,p3);
	Geo2D::SplitBezier(b1,b2,t,p0,p1,p2,p3);
	
	beziers.push_back(b1);
	
	std::vector<v2d> pts_remain;
	for(size_t i = 1;i<pts.size();i++)
		pts_remain.push_back(pts[i]);

	Geo2D::SplitBezier(beziers,pts_remain,b2[0],b2[1],b2[2],b2[3]);
}

double Geo2D::tOfPointOnBezier(const v2d & v, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	/*v4d bx,by;
	Geo2D::BezierParams(bx,by,p0,p1,p2,p3);
	
	v4d coeff = bx;
	coeff[3] -= v[0];

	std::vector<double> roots;
	Geo2D::CubicRoots(roots,coeff);

	for(size_t i = 0;i<roots.size();i++)
	{
		double t = roots[i];
		double x = bx[0] * t * t * t + bx[1] * t * t + bx[2] * t + bx[3];
		double y = by[0] * t * t * t + by[1] * t * t + by[2] * t + by[3];

		if(fabs(x - v[0]) < 0.5 && fabs(y - v[1]) < 0.5)
			return t;
	}

	return 0;*/

	double t;
	Geo2D::GetPointProjection(t,v,p0,p1,p2,p3);
	return t;
}

void Geo2D::CubicRoots(std::vector<double> & roots, const v4d & bezier_coeff)
{
	double a = bezier_coeff[0];
	double b = bezier_coeff[1];
	double c = bezier_coeff[2];
	double d = bezier_coeff[3];
	
	double A = b / a;
	double B = c / a;
	double C = d / a;

	double Q, R, D, S, T, Im;
	double t0,t1,t2;

	Q = (3*B - A * A) / 9;
	R = (9 * A * B - 27 * C - 2 * A * A * A) / 54;
	D = Q * Q * Q + R * R;

	if(D >= 0)
	{
		double RpD = R + sqrt(D);
		double RsD = R - sqrt(D);
		
		if(RpD >= 0)
			S = pow(RpD,1.0/3);
		else
			S = -pow(-RpD,1.0/3);

		if(RsD >= 0)
			T = pow(RsD,1.0/3);
		else
			T = -pow(-RsD,1.0/3);

		t0 = -A / 3 + (S + T);
		t1 = -A / 3 - (S + T) / 2;
		t2 = -A / 3 - (S + T) / 2;

		Im = fabs(sqrt(3.0) * (S-T) / 2);

		if(Im != 0)
		{
			t1 = -1;
			t2 = -1;
		}
	}
	else
	{
		double th = acos(R / sqrt(-Q * Q * Q));

		t0 = 2 * sqrt(-Q) * cos(th/3) - A / 3;
		t1 = 2 * sqrt(-Q) * cos((th+2*_PI_)/3) - A / 3;
		t2 = 2 * sqrt(-Q) * cos((th+4*_PI_)/3) - A / 3;
	}

	roots.clear();
	if(t0 >= 0 && t0 <= 1)
		roots.push_back(t0);

	if(t1 >= 0 && t1 <= 1)
		roots.push_back(t1);

	if(t2 >= 0 && t2 <= 1)
		roots.push_back(t2);

	std::sort(roots.begin(),roots.end());
}

double Geo2D::tOfPointOnLine(const v2d & v, const v2d & v0, const v2d & v1)
{
	if(v0[0] == v1[0] && v0[1]  == v1[1]) return 0;
	if(v0[0] == v1[0])
		return (v[1] - v0[1]) / (v1[1] - v0[1]);
	else
		return (v[0] - v0[0]) / (v1[0] - v0[0]);
}

void Geo2D::PointOnLine(v2d & pt, double t, const v2d & p0, const v2d & p1)
{
	Geo2D::InterpLinear(pt,t,p0,p1);
}

void Geo2D::PointOnBezier(v2d & pt, double t, const v2d & p0, const v2d & p1, const v2d & p2)
{
	Geo2D::PointOnQuadratic_deCasteljau(pt,t,p0,p1,p2);
}

void Geo2D::PointOnBezier(v2d & pt, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	/*v4d coex, coey;
	Geo2D::BezierParams(coex,coey,p0,p1,p2,p3);

	pt[0] = coex[0] * t * t * t + coex[1] * t * t + coex[2] * t + coex[3];
	pt[1] = coey[0] * t * t * t + coey[1] * t * t + coey[2] * t + coey[3];*/
	Geo2D::PointOnCubicBezier_deCasteljau(pt,t,p0,p1,p2,p3);
}

void Geo2D::PointOnQuadratic_deCasteljau(v2d & pt, double t, const v2d & p0, const v2d & p1, const v2d & p2)
{
	v2d p10,p11;
	Geo2D::InterpLinear(p10,t,p0,p1);
	Geo2D::InterpLinear(p11,t,p1,p2);
	Geo2D::InterpLinear(pt,t,p10,p11);
}
void Geo2D::PointOnCubicBezier_deCasteljau(v2d & pt, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	v2d p10,p11,p12,p20,p21;
	Geo2D::InterpLinear(p10,t,p0,p1);
	Geo2D::InterpLinear(p11,t,p1,p2);
	Geo2D::InterpLinear(p12,t,p2,p3);
	Geo2D::InterpLinear(p20,t,p10,p11);
	Geo2D::InterpLinear(p21,t,p11,p12);
	Geo2D::InterpLinear(pt,t,p20,p21);	
}

bool Geo2D::PointInPolygon(const v2d & pt, const std::vector<FRNode*> & poly)
{
	std::vector<v2d> pts;
	
	for(size_t i = 0;i<poly.size();i++)
	{
		if(poly[i]->NodeType() == 0)
		{
			FRNodeLine * node = (FRNodeLine*)(poly[i]);
			pts.push_back(node->GetFirstEnd());
			pts.push_back(node->GetLastEnd());
		}
		else
		{
			FRNodeBezier * node = (FRNodeBezier*)(poly[i]);
			v4d coex, coey;
			Geo2D::BezierParams(coex,coey,node->pts(0),node->pts(1),node->pts(2),node->pts(3));

			for(double t = 0;t<1;t+=0.1)
			{
				double x = coex[0] * t * t * t + coex[1] * t * t + coex[2] * t + coex[3];
				double y = coey[0] * t * t * t + coey[1] * t * t + coey[2] * t + coey[3];
				pts.push_back(_v2d_(x,y));
			}
		}
	}

	pts.push_back(pts[0]);

	double angle = 0;

	for(size_t i = 0;i<pts.size()-1;i++)
	{
		double x1 = pts[i]	[0]	- pt[0];
		double y1 = pts[i]	[1]	- pt[1];
		double x2 = pts[i+1][0] - pt[0];
		double y2 = pts[i+1][1] - pt[1];

		double t1,t2,t3;
		
		t1 = atan2(y1,x1);
		t2 = atan2(y2,x2);
		t3 = t2 - t1;
	
		while(t3 > 3.14159)
			t3 -= 2 * 3.14159;
		while(t3 < -3.14159)
			t3 += 2 * 3.14159;
		
		angle += t3;
	}

	if(fabs(angle) < 3.14159)
		return false;
	else
		return true;
}

void Geo2D::DerivativeOnCubicBezier(v2d & dt, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	v2d A,B,C;
	A = (p1 - p0) * 3;
	B = (p2 - p1) * 3;
	C = (p3 - p2) * 3;
	
	Geo2D::PointOnBezier(dt,t,A,B,C);
}

void Geo2D::SecondDerivativeOnCubicBezier(v2d & dt, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	v2d A,B,C;

	A = (p1 - p0) * 3;
	B = (p2 - p1) * 3;
	C = (p3 - p2) * 3;

	v2d AA,BB;

	AA = (B - A) * 2;
	BB = (C - B) * 2;

	Geo2D::InterpLinear(dt,t,AA,BB);
}

void Geo2D::TangentOnCubicBeizer(v2d & ta, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	v2d dt;
	Geo2D::DerivativeOnCubicBezier(dt,t,p0,p1,p2,p3);
	double l = sqrt(dt[0] * dt[0] + dt[1] * dt[1]);
	ta = _v2d_(dt[0]/l,dt[1]/l);
}

void Geo2D::NormalOnCubicBezier(v2d & no, double t,  const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	v2d dt;
	Geo2D::TangentOnCubicBeizer(dt,t,p0,p1,p2,p3);
	no = _v2d_(-dt[1],dt[0]);
}

double Geo2D::GetCubicBeizerArcLength(double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	// Legendre-Gauss abscissae (xi values, defined at i=n as the roots of the nth order Legendre polynomial Pn(x))
	double Tvalues[] = {-0.0765265211334973337546404093988382110047,0.0765265211334973337546404093988382110047,-0.2277858511416450780804961953685746247430,0.2277858511416450780804961953685746247430,-0.3737060887154195606725481770249272373957,0.3737060887154195606725481770249272373957,-0.5108670019508270980043640509552509984254,0.5108670019508270980043640509552509984254,-0.6360536807265150254528366962262859367433,0.6360536807265150254528366962262859367433,-0.7463319064601507926143050703556415903107,0.7463319064601507926143050703556415903107,-0.8391169718222188233945290617015206853296,0.8391169718222188233945290617015206853296,-0.9122344282513259058677524412032981130491,0.9122344282513259058677524412032981130491,-0.9639719272779137912676661311972772219120,0.9639719272779137912676661311972772219120,-0.9931285991850949247861223884713202782226,0.9931285991850949247861223884713202782226};
	// Legendre-Gauss weights (wi values, defined by a function linked to in the Bezier primer article)
	double Cvalues[] = {0.1527533871307258506980843319550975934919,0.1527533871307258506980843319550975934919,0.1491729864726037467878287370019694366926,0.1491729864726037467878287370019694366926,0.1420961093183820513292983250671649330345,0.1420961093183820513292983250671649330345,0.1316886384491766268984944997481631349161,0.1316886384491766268984944997481631349161,0.1181945319615184173123773777113822870050,0.1181945319615184173123773777113822870050,0.1019301198172404350367501354803498761666,0.1019301198172404350367501354803498761666,0.0832767415767047487247581432220462061001,0.0832767415767047487247581432220462061001,0.0626720483341090635695065351870416063516,0.0626720483341090635695065351870416063516,0.0406014298003869413310399522749321098790,0.0406014298003869413310399522749321098790,0.0176140071391521183118619623518528163621,0.0176140071391521183118619623518528163621};
	
	double z   = t / 2;
	double sum = 0;

	for(int i = 0;i<20;i++)
	{
		double corrected_z  = z * Tvalues[i] + z;

		v2d dt;
		Geo2D::DerivativeOnCubicBezier(dt,corrected_z,p0,p1,p2,p3);

		double B			= sqrt(dt[0] * dt[0] + dt[1] * dt[1]);
		sum				   += Cvalues[i] * B; 
	}
	return z * sum;
}

double Geo2D::CubicBeizerArcLengthTo_t(double l, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	std::vector<double> ll; 
	for(int i = 0;i<1000;i++)
	{
		double t = i / 1000.0;
		ll.push_back(Geo2D::GetCubicBeizerArcLength(t,p0,p1,p2,p3));
	}
	
	double ans = 0.5;

	for(int i = 0;i<999;i++)
	{
		double a = ll[i];
		double b = ll[i+1];
		if(l >= a && l < b)
		{
			ans = i / 1000.0;
			break;
		}
	}	

	return ans;	
}

void Geo2D::GetPointProjection(double & t, const v2d & p, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	double mindist = 1e20;

	for(int i = 0;i<200;i++)
	{
		double ii = (double)i / 199.0;
		v2d tp;
		Geo2D::PointOnCubicBezier_deCasteljau(tp,ii,p0,p1,p2,p3);
		
		double xx = p[0] - tp[0];
		double yy = p[1] - tp[1];
		
		double dist = (xx * xx + yy * yy);
		if(dist < mindist)
		{
			mindist = dist;
			t = ii;
		}
	}

	t = Geo2D::RefineProjection_(t,mindist,1.0/(1.01*200),p,p0,p1,p2,p3);
}

double Geo2D::RefineProjection_(double t, double dist, double precision, const v2d & p, const  v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3)
{
	if(precision < 0.00001) return t;

	double prev_t = t - precision,
		   next_t = t + precision;
	
	v2d prev_pt,next_pt;
	
	Geo2D::PointOnCubicBezier_deCasteljau(prev_pt,prev_t,p0,p1,p2,p3);
	Geo2D::PointOnCubicBezier_deCasteljau(next_pt,next_t,p0,p1,p2,p3);

	double prev_x = prev_pt[0] - p[0],
		   prev_y = prev_pt[1] - p[1],
		   next_x = next_pt[0] - p[0],
		   next_y = next_pt[1] - p[1];
	
	double prev_dist = (prev_x * prev_x + prev_y * prev_y),
		   next_dist = (next_x * next_x + next_y * next_y);
	
	
	if(prev_t >=0 && prev_dist < dist) { return Geo2D::RefineProjection_(prev_t,prev_dist,precision,p,p0,p1,p2,p3); }
	if(next_t <=1 && next_dist < dist) { return Geo2D::RefineProjection_(next_t,next_dist,precision,p,p0,p1,p2,p3); }

	return Geo2D::RefineProjection_(t,dist,precision/2,p,p0,p1,p2,p3);
}