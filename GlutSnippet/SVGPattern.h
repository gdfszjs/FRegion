#ifndef SVG_PATTERN_H_
#define SVG_PATTERN_H_

#include "Common.h"

class SVGElement;

class SVGPattern
{
public:
	SVGPattern();
	~SVGPattern();

	void drawPattern();
	void addElement(SVGElement * p) { elements_.push_back(p); }
	void computeBoundingBox();
	void init();
	void translate(double x, double y);
	void scale(double x, double y);
	void elementsContainPoint(std::vector<SVGElement*> & selected_elements, double x, double y);
	SVGElement * elementContainPoint(double x, double y);
	void getBoundingBoxCenter(double & x, double & y);
	void getBoundingBoxWH(double & w, double & h);

	/* save and load patterns */
	void Save(char * filename);
	void Save(ofstream & outfile);
	void Load(char * filename);
	void Load(ifstream & infile);

	bool comparePattern(SVGPattern *e);

	std::vector<SVGElement*> elements_;
private:
	double min_x_;
	double max_x_;
	double min_y_;
	double max_y_;
}; // SVGPattern
#endif // SVG_PATTERN_H_