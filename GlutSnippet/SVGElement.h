#ifndef SVG_ELEMENT_H_
#define SVG_ELEMENT_H_
#include <LibIV\libiv.h>
#include "Common.h"

class ElementSeg;

class SVGElement
{
public:
	SVGElement								() { }
	~SVGElement								();
	SVGElement								(const SVGElement & e);
		
	void 		addSeg						(ElementSeg * p);
	
	// drawing ...
	void 		drawElement					();
	void 		drawElementPoints			();

	// properties ...
	void 		computeBoundingBox			();	
	void 		getBoundingBox				(double & minx, double & maxx, double & miny, double & maxy);
	void 		computeCentroid				();
	void		getCentroid					(double & x, double & y);

	void		set_guid_string				(string s)	{ guid_string_ = s; }
	string		guid_string					()			{ return guid_string_; }

	// transforming ... 
	void		translate					(double x, double y);
	void 		scale						(double x, double y);
	void 		rotate						(double t);

	// judging ....
	bool		PointInElement				(double x, double y);
	double		DistanceFromPoint			(const v2d & pt);

	// statistics ... 
	void				collectPoints(std::vector<v2d> & pts);
	virtual void		collectPoints(std::vector<v2d> & pts, double t_gap); // using t_gap to extract points on bezier curve

	// overridding operators ...
	friend ostream &	operator<<(ostream & o, SVGElement & e);
	friend istream &	operator>>(istream & infile, SVGElement & e);

	//compare the seg element with itself
	bool		ElementCompare				(SVGElement * e);

	std::vector<ElementSeg*> segs_;
private:
	double min_x_;
	double max_x_;
	double min_y_;
	double max_y_;
	double centroid_x_;
	double centroid_y_;

	string guid_string_;
}; // SVGElement


class ElementSeg
{
public:
	ElementSeg() {}
	~ElementSeg() {}
	ElementSeg(const ElementSeg & es) {}

	virtual void drawSeg() {}
	virtual void drawSegPoints() {}
	virtual void translate(double x, double y) {}
	virtual void scale(double x, double y) {}
	virtual void rotate(double t) {}
	virtual void computeBoundingBox(double & x1, double & x2, double & y1, double & y2) {}
	virtual void collectPoints(std::vector<v2d> & pts) {} 
	virtual void collectPoints(std::vector<v2d> & pts, double t_gap) {}
	virtual ElementSeg * deepCopy() {return NULL;}
	virtual int  SegType() { return 0; }
	virtual double DistanceFromPoint(const v2d & pt) = 0;

	friend ostream & operator<<(ostream & o, ElementSeg & es) { es.write(o); return o;}
	virtual void write(ostream & o) {}
	friend istream & operator>>(istream & infile, ElementSeg & es) { es.read(infile); return infile;}
	virtual void read(istream & infile) {}

	virtual v2d	FirstVertex	() = 0;
	virtual v2d LastVertex  () = 0;

	virtual bool ElementSegCompare(ElementSeg * es) { return false; }
private:
}; // ElementSeg


class ElementSegLine : public ElementSeg
{
public:
	ElementSegLine() {}
	ElementSegLine(const v2d & p1, const v2d & p2);
	ElementSegLine(const ElementSegLine & esl);
	~ElementSegLine() {}

	virtual void drawSeg();
	virtual void drawSegPoints();
	virtual void translate(double x, double y);
	virtual void scale(double x, double y);
	virtual void rotate(double t);
	virtual void computeBoundingBox(double & x1, double & x2, double & y1, double & y2);
	virtual void collectPoints(std::vector<v2d> & pts);
	virtual void collectPoints(std::vector<v2d> & pts, double t_gap);
	virtual ElementSeg * deepCopy();
	virtual void write(ostream & o);
	virtual void read(istream & infile);
	virtual double DistanceFromPoint(const v2d & pt);
	virtual v2d FirstVertex() { return pts_[0]; };
	virtual v2d LastVertex () { return pts_[1]; };

	virtual bool ElementSegCompare(ElementSegLine * es);
//private:
	v2d pts_[2]; 
}; // ElementSegLine

class ElementSegBezier : public ElementSeg
{
public:
	ElementSegBezier() {}
	ElementSegBezier(const v2d & p1, const v2d & p2, const v2d & p3, const v2d & p4);
	ElementSegBezier(const ElementSegBezier & esb);
	~ElementSegBezier() {}

	virtual void drawSeg();
	virtual void drawSegPoints();
	virtual void translate(double x, double y);
	virtual void scale(double x, double y);
	virtual void rotate(double t);
	virtual void computeBoundingBox(double & x1, double & x2, double & y1, double & y2);
	virtual void collectPoints(std::vector<v2d> & pts);
	virtual void collectPoints(std::vector<v2d> & pts, double t_gap);
	virtual ElementSeg * deepCopy();
	virtual void write(ostream & o);
	virtual void read(istream & infile);
	virtual int  SegType() { return 1; }
	virtual double DistanceFromPoint(const v2d & pt);
	virtual v2d FirstVertex() { return pts_[0]; }
	virtual v2d LastVertex () { return pts_[3]; }

	virtual bool ElementSegCompare(ElementSegBezier * es);
//private:
	v2d pts_[4];
}; // ElementSegBezier

#endif // SVG_ELEMENT_H_