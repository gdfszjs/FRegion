#include "SVGPath.h"
#include "SVGElement.h"

// ===================================================================================================
// SVGPath

SVGPath::SVGPath()
{
}

SVGPath::~SVGPath()
{
	for(size_t i = 0;i<commands_.size();i++)
		delete commands_[i];
	commands_.clear();
}

SVGPathCommand * SVGPath::getCommand(size_t idx)
{
	SVGPathCommand * ans;
	if(idx < commands_.size())
		ans = commands_[idx];
	return ans;
}

SVGElement * SVGPath::pathToElement()
{
	SVGElement * p = new SVGElement();
	double curr_x = 0, curr_y = 0;
	for(size_t i = 0; i<commands_.size();i++)
	{
		ElementSeg * ps = commands_[i]->commandToSeg(curr_x, curr_y);
		if(ps!=NULL)
			p->addSeg(ps);
	}
	return p;
}

// ===================================================================================================
// SVGPathCommandM
ElementSeg * SVGPathCommandM::commandToSeg(double & curr_x, double & curr_y)
{
	curr_x += move_to_pos_[0];
	curr_y += move_to_pos_[1];
		
	return NULL; // no line, must check this
	/*if(curr_x == 0 && curr_y == 0) // the first move
	{
		curr_x += move_to_pos_[0];
		curr_y += move_to_pos_[1];
		
		return NULL; // no line, must check this
	}
	else // return a line
	{
		if(this->move_to_pos_[0] == 0 && this->move_to_pos_[1] == 0) // no sense
			return NULL;
		v2d p1,p2;
		p1 = _v2d_(curr_x,curr_y);
		curr_x += move_to_pos_[0];
		curr_y += move_to_pos_[1];
		p2 = _v2d_(curr_x,curr_y);
		ElementSegLine * ps = new ElementSegLine(p1,p2);
		return ps;
	}*/
}

// ===================================================================================================
// SVGPathCommandL
ElementSeg * SVGPathCommandL::commandToSeg(double & curr_x, double & curr_y)
{
	if(line_to_pos_[0] == 0 && line_to_pos_[1] == 0)
		return NULL;
	v2d p1,p2;
	p1 = _v2d_(curr_x,curr_y);
	curr_x = line_to_pos_[0];
	curr_y = line_to_pos_[1];
	p2 = _v2d_(curr_x,curr_y);
	ElementSegLine * ps = new ElementSegLine(p1,p2);
	return ps;
}

// ===================================================================================================
// SVGPathCommandC
ElementSeg * SVGPathCommandC::commandToSeg(double & curr_x, double & curr_y)
{
	v2d p1,p2,p3,p4;
	p1 = _v2d_(curr_x,curr_y);
	p2 = _v2d_(curr_x = bezier_curve_[0], curr_y = bezier_curve_[1]);
	p3 = _v2d_(curr_x = bezier_curve_[2], curr_y = bezier_curve_[3]);
	p4 = _v2d_(curr_x = bezier_curve_[4], curr_y = bezier_curve_[5]);
	//curr_x += bezier_curve_[4];
	//curr_y += bezier_curve_[5];
	ElementSegBezier * ps = new ElementSegBezier(p1,p2,p3,p4);
	return ps;
}