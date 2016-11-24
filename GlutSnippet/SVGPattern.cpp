#include "SVGPattern.h"
#include "SVGElement.h"
#include "CommonGl.h"
#include "GLUTMain.h"
#include "FrameChecker.h"

SVGPattern::SVGPattern()
{
}

SVGPattern::~SVGPattern()
{
	for(size_t i = 0;i<elements_.size();i++)
		delete elements_[i];
	elements_.clear();
}

void SVGPattern::drawPattern()
{
	for(size_t i = 0;i<elements_.size();i++)
		elements_[i]->drawElement();

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameChecker * checker = (FrameChecker*)(main_frame->getFrameByName("Bounding Box?"));
	if(checker != 0 && checker->getStatus())
	{
		glColor3f(1,0,0);
		glBegin(GL_LINE_LOOP);
		glVertex2d(min_x_,min_y_);
		glVertex2d(min_x_,max_y_);
		glVertex2d(max_x_,max_y_);
		glVertex2d(max_x_,min_y_);
		glEnd();
	}
	
	/*glColor3f(1,0,0);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2d((min_x_ + max_x_)/2,(min_y_ + max_y_)/2);
	glEnd();*/
}

void SVGPattern::computeBoundingBox()
{
	min_x_ = 1e5;
	min_y_ = 1e5;
	max_x_ = -1e5;
	max_y_ = -1e5;

	for(size_t i = 0;i<elements_.size();i++)
	{
		double x1,x2,y1,y2;
		elements_[i]->computeBoundingBox();
		elements_[i]->getBoundingBox(x1,x2,y1,y2);
		if(x1 < min_x_)
			min_x_ = x1;
		if(x2 > max_x_)
			max_x_ = x2;
		if(y1 < min_y_)
			min_y_ = y1;
		if(y2 > max_y_)
			max_y_ = y2;
	}
}

void SVGPattern::translate(double x, double y)
{
	for(size_t i = 0;i<elements_.size();i++)
		elements_[i]->translate(x,y);
}

void SVGPattern::scale(double x, double y)
{
	for(size_t i = 0;i<elements_.size();i++)
		elements_[i]->scale(x,y);
}

void SVGPattern::init()
{
	computeBoundingBox();	
	double width = max_x_ - min_x_;
	double height = max_y_ - min_y_;
	double cx = (min_x_ + max_x_) / 2;
	double cy = (min_y_ + max_y_) / 2;
	translate(- cx, - cy); 

	double ss = 500 / width;		// radius: 500
	scale(ss,ss);

	translate(260,260);				// centre: (260,260)

	// computing properties ...
	computeBoundingBox();
	for(size_t i = 0; i<elements_.size();i++)
		elements_[i]->computeCentroid();
}

void SVGPattern::elementsContainPoint(std::vector<SVGElement*> & selected_elements, double x, double y)
{
	for(size_t i = 0;i<elements_.size();i++)
		if(elements_[i]->PointInElement(x,y))
			selected_elements.push_back(elements_[i]);
}

SVGElement* SVGPattern::elementContainPoint(double x, double y)
{
	std::vector<SVGElement*> selected_elements;
	elementsContainPoint(selected_elements,x,y);
	if(selected_elements.empty())
		return NULL;
	else if(selected_elements.size() == 1)
		return selected_elements[0];
	else
	{
		SVGElement * ans = NULL;
		double distance = 1e10;
		for(size_t i = 0;i<selected_elements.size();i++)
		{
			double cx,cy;
			selected_elements[i]->getCentroid(cx,cy);
			double tmp1 = x - cx;
			double tmp2 = y - cy;
			double tmp3 = tmp1 * tmp1 + tmp2 * tmp2;
			if(tmp3 < distance)
			{
				distance = tmp3;
				ans = selected_elements[i];
			}
		}
		return ans;
	}
}

void SVGPattern::getBoundingBoxCenter(double & x, double & y)
{
	x = (min_x_ + max_x_)/2;
	y = (min_y_ + max_y_)/2;
}

void SVGPattern::getBoundingBoxWH(double & w, double & h)
{
	w = (max_x_ - min_x_)/2;
	h = (max_y_ - min_y_)/2;
}

void SVGPattern::Save(char * filename)
{
	ofstream outfile(filename);
	Save(outfile);
}

void SVGPattern::Save(ofstream & outfile)
{
	outfile<<"Pattern\n";
	for(size_t i = 0;i<elements_.size();i++)
	{
		outfile<<"Element\n";
		outfile<<(*(elements_[i]));
	}
	outfile<<"End\n";
}

void SVGPattern::Load(char * filename)
{
	ifstream infile(filename);
	Load(infile);
}

void SVGPattern::Load(ifstream & infile)
{
	string line;
	while(getline(infile,line) && line == "Element")
	{
		SVGElement * e = new SVGElement();
		infile>>(*e);
		elements_.push_back(e);
	}
}

bool SVGPattern::comparePattern(SVGPattern *e)
{
	for (int i = 0; i < this->elements_.size(); i++)
	{
		if (!this->elements_.at(i)->ElementCompare(e->elements_.at(i))) return false;
	}
	return true;
}