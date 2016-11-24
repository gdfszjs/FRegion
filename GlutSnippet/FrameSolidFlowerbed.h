#pragma once

#include "Frame.h"

class FRSTree;
class SVGElement;
class GLUtesselator;
class ElementSeg;

struct SideRect
{
	v3d p[4];
};

class FrameSolidFlowerbed: public Frame 
{
public:
	FrameSolidFlowerbed						();
	
	FrameSolidFlowerbed						(Frame * parent, int lx, int ly, int width, int height,string name);
	
	~FrameSolidFlowerbed					();

	virtual void drawScene					();
	virtual void onMouseFunc(int button,int state,int x,int y);
	virtual void onMotionFunc(int x,int y);

	// A patch for the SVGElement: segs may be unordered
	void OrderSVGElementSegs				(SVGElement * e, std::vector<ElementSeg*> & segs);
	// 2015.05.22	pathch for SVGElement: if end point is near enough to FRSTree shell, stitch them
	void StitchElementAndFRSTreeShell		(std::vector<v2d> & pts, v2d p0, v2d p1);
	void StitchElementAndFRSTreeShell		(std::vector<v2d> & pts, int index, v2d p0, v2d p1, v2d p2);
	
	// 2015.05.08	after a long period of time, start reading the code from here
	void ComputeOneCompleteElement			(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & complete, int & is_closed);
	void ComputeCompleteElementForFlowerbed	(const std::vector<std::vector<v2d>> & pts, std::vector<std::vector<std::vector<v2d>>> & complete, std::vector<int> & is_closed);
	
	void ComputeAllReflectionLines			(FRSTree * tree, std::vector<v2d> & to);
	void ComputeFRElementPoints				(FRSTree * tree, std::vector<std::vector<v2d>> & pts);
	void Case12								(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp);	
	void Case21								(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp);
	void Case11								(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp);
	void Case22								(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp);
	void CaseOther							(const std::vector<v2d> & pts, std::vector<std::vector<v2d>> & pp);
	int  PointOnWhichLine					(const v2d & pt, const v2d & p0, const v2d & p1, const v2d & p2);

	/* translate a 2D shape to 3D model */
	void Line2SideRect						(SideRect & sr, const v2d & v0, const v2d & v1, double height); 
	void Shape2Model						(const std::vector<v2d> & shape, std::vector<SideRect> & sides, std::vector<v3d> & top, std::vector<v2d> & top_tex_coords, double height);
	void DrawModelTop						(GLUtesselator * tess, const std::vector<v3d> & top, const std::vector<v2d> & top_tex_coords, double color[3],int texture_id);
	void DrawSideRect						(const SideRect & sr, GLUtesselator * tess);
	void DrawModelFromShape					(GLUtesselator * tess, const std::vector<v2d> & shape, double height, bool is_closed, double top_color[3],int texture_id);
	void DrawSolidFlowerbed					();

	// 2015.05.22	new drawing functions
	void DrawSolidElement					(GLUtesselator * tess, const std::vector<v2d> & pts, double height, double color[3]);
	void DrawSolidSector					();

	void RefreshPattern						(FRSTree * tree);
	void RefreshPatternJustOneSector		(FRSTree * tree);

	void DrawPanorama						();
	
	IplImage * ShowShapeInImage				(const std::vector<v2d> & shape);
	
private:
	v2d											center_;
	double										angle_;
	std::vector<v2d>							to_;
	
	std::vector<std::vector<std::vector<v2d>>>	complete_;
	std::vector<int>							is_closed_;	
	std::vector<std::vector<v2d>>				fr_;

	v3d											camera_pos_;
	double										rotate_x_;
	double										rotate_y_;

	unsigned int								list_id_; 
};