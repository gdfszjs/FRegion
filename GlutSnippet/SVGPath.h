#ifndef SVG_PATH_H_
#define SVG_PATH_H_
#include "Common.h"
#include "LibIV\libiv.h"
#include "CommonGl.h"

class SVGPathCommand;
class SVGElement;
class ElementSeg;

class SVGPath
{
public:
	SVGPath();
	~SVGPath();
	
	void addCommand(SVGPathCommand * c) { commands_.push_back(c); } 
	size_t commandCount() { return commands_.size(); } 
	SVGPathCommand * getCommand(size_t idx);

	SVGElement * pathToElement();
private:
	std::vector<SVGPathCommand*> commands_;
};

class SVGPathCommand
{
public:
	SVGPathCommand(){}
	~SVGPathCommand(){}

	virtual ElementSeg* commandToSeg(double & curr_x, double & curr_y) = 0;
private:
};

class SVGPathCommandM : public SVGPathCommand
{
public:
	SVGPathCommandM(){}
	SVGPathCommandM(v2d v) { move_to_pos_ = v; }
	~SVGPathCommandM(){}

	virtual ElementSeg* commandToSeg(double & curr_x, double & curr_y);

	v2d move_to_pos_;
};

class SVGPathCommandL : public SVGPathCommand
{
public:
	SVGPathCommandL(){}
	SVGPathCommandL(v2d v) { line_to_pos_ = v; }
	~SVGPathCommandL(){}

	virtual ElementSeg* commandToSeg(double & curr_x, double & curr_y);
private:
	v2d line_to_pos_;
};

class SVGPathCommandC : public SVGPathCommand
{
public:
	SVGPathCommandC(){}
	SVGPathCommandC(v6d v) { bezier_curve_ = v; }
	~SVGPathCommandC(){}

	virtual ElementSeg* commandToSeg(double & curr_x, double & curr_y);
private:
	v6d bezier_curve_;
};

#endif // SVG_PATH_H_