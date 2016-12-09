#include "SVGElement.h"
#include "CommonGl.h"
#include "GLUTMain.h"
#include "FrameChecker.h"
#include <opencv2\imgproc\imgproc.hpp>
#include "Geo2D.h"

// ===================================================================================================
// SVGElement

SVGElement::SVGElement(const SVGElement & e)
{
	segs_.clear();
	for (size_t i = 0; i < e.segs_.size(); i++)
		segs_.push_back(e.segs_[i]->deepCopy());
	min_x_ = e.min_x_;
	max_x_ = e.max_x_;
	min_y_ = e.min_y_;
	max_y_ = e.max_y_;
	centroid_x_ = e.centroid_x_;
	centroid_y_ = e.centroid_y_;
	guid_string_ = e.guid_string_;
}

SVGElement::~SVGElement()
{
	for (size_t i = 0; i < segs_.size(); i++)
		delete segs_[i];
	segs_.clear();
}

void SVGElement::addSeg(ElementSeg * p)
{
	segs_.push_back(p);
}

void SVGElement::drawElement()
{
	for (size_t i = 0; i < segs_.size(); i++)
		segs_[i]->drawSeg();

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameChecker * checker = (FrameChecker*)(main_frame->getFrameByName("Bounding Box?"));
	if (checker != 0 && checker->getStatus())
	{
		glColor3f(0, 0, 1);
		glBegin(GL_LINE_LOOP);
		glVertex2d(min_x_, min_y_);
		glVertex2d(min_x_, max_y_);
		glVertex2d(max_x_, max_y_);
		glVertex2d(max_x_, min_y_);
		glEnd();
	}
}

void SVGElement::drawElementPoints()
{
	for (size_t i = 0; i < segs_.size(); i++)
		segs_[i]->drawSegPoints();

	glColor3f(0, 1, 0);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2d(centroid_x_, centroid_y_);
	glEnd();
}

void SVGElement::computeBoundingBox()
{
	min_x_ = 1e5;
	min_y_ = 1e5;
	max_x_ = -1e5;
	max_y_ = -1e5;

	for (size_t i = 0; i < segs_.size(); i++)
		segs_[i]->computeBoundingBox(min_x_, max_x_, min_y_, max_y_);
}

void SVGElement::getBoundingBox(double & minx, double & maxx, double & miny, double & maxy)
{
	minx = min_x_;
	maxx = max_x_;
	miny = min_y_;
	maxy = max_y_;
}

void SVGElement::translate(double x, double y)
{
	for (size_t i = 0; i < segs_.size(); i++)
		segs_[i]->translate(x, y);
}

void SVGElement::scale(double x, double y)
{
	for (size_t i = 0; i < segs_.size(); i++)
		segs_[i]->scale(x, y);
}

void SVGElement::rotate(double t)
{
	for (size_t i = 0; i < segs_.size(); i++)
		segs_[i]->rotate(t);
}

bool SVGElement::PointInElement(double x, double y)
{
	if (x >= min_x_ && x <= max_x_ &&
		y >= min_y_ && y <= max_y_)
		return true;
	else
		return false;
}

void SVGElement::collectPoints(std::vector<v2d> & pts)
{
	pts.clear();
	for (size_t i = 0; i < segs_.size(); i++)
		segs_[i]->collectPoints(pts);
}

void SVGElement::collectPoints(std::vector<v2d> & pts, double t_gap)
{
	pts.clear();
	for (size_t i = 0; i < segs_.size(); i++)
		segs_[i]->collectPoints(pts, t_gap);
}

void SVGElement::computeCentroid()
{
	/*std::vector<v2d> pts;
	collectPoints(pts,0.05);
	centroid_x_ = 0;
	centroid_y_ = 0;
	for(size_t i = 0;i<pts.size();i++)
	{
		centroid_x_ += pts[i][0];
		centroid_y_ += pts[i][1];
	}
	centroid_x_ /= pts.size();
	centroid_y_ /= pts.size();*/

	/* using OpenCV moments to compute centroid */
	std::vector<v2d> pts;

	this->collectPoints(pts, 0.01);

	if (pts.empty()) return;
	std::vector<cv::Point2f> pts2;
	for (size_t i = 0; i < pts.size(); i++)
	{
		cv::Point2f p;
		p.x = (float)(pts[i][0]);
		p.y = (float)(pts[i][1]);
		pts2.push_back(p);
	}

	cv::Moments mo = cv::moments(pts2, false);

	this->centroid_x_ = mo.m10 / mo.m00;
	this->centroid_y_ = mo.m01 / mo.m00;

	/* using the first point as anchor point */
	/*std::vector<v2d> pts;
	this->collectPoints(pts);
	this->centroid_x_ = pts[0][0];
	this->centroid_y_ = pts[0][1];*/
}

void SVGElement::getCentroid(double & x, double & y)
{
	x = centroid_x_;
	y = centroid_y_;
}

ostream & operator<<(ostream & o, SVGElement & e)
{
	// o<<e.guid_string_<<'\n';
	for (size_t i = 0; i < e.segs_.size(); i++)
		o << *(e.segs_[i]);
	o << '\n';
	return o;
}

istream & operator>>(istream & infile, SVGElement & e)
{
	do
	{
		char first_letter = ' ';
		infile.get(first_letter);
		if (first_letter == 'L')
		{
			ElementSegLine * esl = new ElementSegLine();
			infile >> (*esl);
			e.segs_.push_back(esl);
		}
		else if (first_letter == 'C')
		{
			ElementSegBezier * esb = new ElementSegBezier();
			infile >> (*esb);
			e.segs_.push_back(esb);
		}
		else
		{
			break;
		}
	} while (1);

	return infile;
}

bool SVGElement::ElementCompare(SVGElement * e)
{
	for (int i = 0; i < this->segs_.size(); i++)
	{
		if (typeid(this->segs_.at(i)) != typeid(e->segs_.at(i)))
		{
			return false;
		}
		if (!this->segs_.at(i)->ElementSegCompare(e->segs_.at(i)))
		{
			return false;
		}

	}
	return true;
}

double SVGElement::DistanceFromPoint(const v2d & pt)
{
	double mind = 1e10;

	for (size_t i = 0; i < segs_.size(); i++)
	{
		double d = segs_[i]->DistanceFromPoint(pt);
		if (d < mind)
			mind = d;
	}

	return mind;
}

// ===================================================================================================
// ElementSegLine

ElementSegLine::ElementSegLine(const v2d & p1, const v2d & p2)
{
	pts_[0] = p1;
	pts_[1] = p2;
}

ElementSegLine::ElementSegLine(const ElementSegLine & esl)
{
	for (int i = 0; i < 2; i++)
	{
		pts_[i][0] = esl.pts_[i][0];
		pts_[i][1] = esl.pts_[i][1];
	}
}

ElementSeg * ElementSegLine::deepCopy()
{
	return (new ElementSegLine(*this));
}

void ElementSegLine::drawSeg()
{
	glBegin(GL_LINES);
	glVertex2d(pts_[0][0], pts_[0][1]);
	glVertex2d(pts_[1][0], pts_[1][1]);
	glEnd();
}

void ElementSegLine::drawSegPoints()
{
	glColor3f(0, 1, 0);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex2d(pts_[0][0], pts_[0][1]);
	glVertex2d(pts_[1][0], pts_[1][1]);

	glEnd();
}

void ElementSegLine::computeBoundingBox(double & x1, double & x2, double & y1, double & y2)
{
	for (int i = 0; i < 2; i++)
	{
		if (pts_[i][0] < x1)
			x1 = pts_[i][0];
		if (pts_[i][0] > x2)
			x2 = pts_[i][0];
		if (pts_[i][1] < y1)
			y1 = pts_[i][1];
		if (pts_[i][1] > y2)
			y2 = pts_[i][1];
	}
}

void ElementSegLine::translate(double x, double y)
{
	for (int i = 0; i < 2; i++)
	{
		pts_[i][0] += x;
		pts_[i][1] += y;
	}
}

void ElementSegLine::scale(double x, double y)
{
	for (int i = 0; i < 2; i++)
	{
		pts_[i][0] *= x;
		pts_[i][1] *= y;
	}
}

void ElementSegLine::rotate(double t)
{
	for (int i = 0; i < 2; i++)
	{
		double x = pts_[i][0] * cos(t) - pts_[i][1] * sin(t);
		double y = pts_[i][0] * sin(t) + pts_[i][1] * cos(t);
		pts_[i][0] = x;
		pts_[i][1] = y;
	}
}

void ElementSegLine::collectPoints(std::vector<v2d> & pts)
{
	pts.push_back(pts_[0]);
	pts.push_back(pts_[1]);
}

void ElementSegLine::collectPoints(std::vector<v2d> & pts, double t_gap)
{
	for (double t = 0; t < 1.0; t += t_gap)
	{
		v2d pt;
		Geo2D::InterpLinear(pt, t, pts_[0], pts_[1]);

		pts.push_back(pt);
	}

}

void ElementSegLine::write(ostream & o)
{
	o << 'L';
	for (int i = 0; i < 2; i++)
		o << ' ' << pts_[i][0] << ' ' << pts_[i][1];
	o << '\n';
}

void ElementSegLine::read(istream & infile)
{
	string line;
	getline(infile, line);
	(istringstream)(line) >> pts_[0][0] >> pts_[0][1] >> pts_[1][0] >> pts_[1][1];
}

double ElementSegLine::DistanceFromPoint(const v2d & pt)
{
	v2d tmp;
	double d = Geo2D::DistanceBetweenPointLineseg(tmp, pt, pts_[0], pts_[1]);
	return d;
}

void ElementSegLine::getsamplepoint()
{
	//直线上取样的100个点
	int numberOfPoints = 100;
	double s_x = pts_[0][0];
	double s_y = pts_[0][1];
	double e_x = pts_[1][0];
	double e_y = pts_[1][1];
	double r_x = 0;
	double r_y = 0;
	if (s_x == e_x)
	{
		for (int i = 0; i < numberOfPoints; i++)
		{
			r_x = s_x;
			r_y = s_y + (s_y - e_y) * (i * 1.0 / numberOfPoints);
			samplepoints.push_back(_v2d_(r_x, r_y));
		}
	}
	else if (s_y == e_y)
	{
		for (int i = 0; i < numberOfPoints; i++)
		{
			r_x = s_x + (s_x - e_x) * (i * 1.0 / numberOfPoints);
			r_y = s_y;
			samplepoints.push_back(_v2d_(r_x, r_y));
		}
	}
	else
	{
		for (int i = 0; i < numberOfPoints; i++)
		{
			double x_range = s_x - s_y;
			double r_x = s_x + (s_x - e_x) * (i * 1.0 / numberOfPoints) * i;
			double r_y = (e_y - s_y) / (e_x - s_x) * (r_x - s_x) + s_y;
			samplepoints.push_back(_v2d_(r_x,r_y));
		}
	}
}

bool ElementSegLine::ElementSegCompare(ElementSegLine * es)
{
	//compare the two elementseg
	if (this->pts_[0] == es->pts_[0] && this->pts_[1] == es->pts_[1])
	{
		return true;
	}

	return false;
}


// ===================================================================================================
// ElementSegBezier
ElementSegBezier::ElementSegBezier(const v2d & p1, const v2d & p2, const v2d & p3, const v2d & p4)
{
	pts_[0] = p1;
	pts_[1] = p2;
	pts_[2] = p3;
	pts_[3] = p4;
}

ElementSegBezier::ElementSegBezier(const ElementSegBezier & esb)
{
	for (int i = 0; i < 4; i++)
	{
		pts_[i][0] = esb.pts_[i][0];
		pts_[i][1] = esb.pts_[i][1];
	}
}

ElementSeg * ElementSegBezier::deepCopy()
{
	return (new ElementSegBezier(*this));
}

void ElementSegBezier::drawSeg()
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

	glMap1d(GL_MAP1_VERTEX_3, 0, 1, 3, 4, &ctrlpoints[0][0]);
	glMapGrid1f(10, 0, 1);
	glEnable(GL_MAP1_VERTEX_3);
	// glColor3f(1,1,1);
	glEvalMesh1(GL_LINE, 0, 10);
}

void ElementSegBezier::drawSegPoints()
{
	glColor3f(0, 1, 0);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex2d(pts_[0][0], pts_[0][1]);
	glVertex2d(pts_[3][0], pts_[3][1]);
	glEnd();
}
void ElementSegBezier::computeBoundingBox(double & x1, double & x2, double & y1, double & y2)
{
	for (int i = 0; i < 4; i++)
	{
		if (pts_[i][0] < x1)
			x1 = pts_[i][0];
		if (pts_[i][0] > x2)
			x2 = pts_[i][0];
		if (pts_[i][1] < y1)
			y1 = pts_[i][1];
		if (pts_[i][1] > y2)
			y2 = pts_[i][1];
	}
}

void ElementSegBezier::translate(double x, double y)
{
	for (int i = 0; i < 4; i++)
	{
		pts_[i][0] += x;
		pts_[i][1] += y;
	}
}

void ElementSegBezier::scale(double x, double y)
{
	for (int i = 0; i < 4; i++)
	{
		pts_[i][0] *= x;
		pts_[i][1] *= y;
	}
}

void ElementSegBezier::rotate(double t)
{
	for (int i = 0; i < 4; i++)
	{
		double x = pts_[i][0] * cos(t) - pts_[i][1] * sin(t);
		double y = pts_[i][0] * sin(t) + pts_[i][1] * cos(t);
		pts_[i][0] = x;
		pts_[i][1] = y;
	}
}

void ElementSegBezier::collectPoints(std::vector<v2d> & pts)
{
	pts.push_back(pts_[0]);
	pts.push_back(pts_[3]);
}

void ElementSegBezier::collectPoints(std::vector<v2d> & pts, double t_gap)
{
	for (double t = 0; t < 1.0; t += t_gap)
	{
		double t1 = 1 - t;
		double x = t1 * t1 * t1 * pts_[0][0] +
			3 * t1 * t1 * t  * pts_[1][0] +
			3 * t1 * t  * t  * pts_[2][0] +
			t  * t  * t  * pts_[3][0];
		double y = t1 * t1 * t1 * pts_[0][1] +
			3 * t1 * t1 * t  * pts_[1][1] +
			3 * t1 * t  * t  * pts_[2][1] +
			t  * t  * t  * pts_[3][1];

		pts.push_back(_v2d_(x, y));
	}
}

void ElementSegBezier::write(ostream & o)
{
	o << 'C';
	for (int i = 0; i < 4; i++)
		o << ' ' << pts_[i][0] << ' ' << pts_[i][1];
	o << '\n';
}

void ElementSegBezier::read(istream & infile)
{
	string line;
	getline(infile, line);
	(istringstream)(line) >> pts_[0][0] >> pts_[0][1] >> pts_[1][0] >> pts_[1][1] >> pts_[2][0] >> pts_[2][1] >> pts_[3][0] >> pts_[3][1];
}

double ElementSegBezier::DistanceFromPoint(const v2d & pt)
{
	v2d tmp;
	double d = Geo2D::DistanceBetweenPointBezier(tmp, pt, pts_[0], pts_[1], pts_[2], pts_[3]);
	return d;
}

void ElementSegBezier::getsamplepoint()
{
	int numberOfPoints = 100;
	float   dt;
	int     i;
	dt = 1.0 / (numberOfPoints - 1);

	for (i = 0; i < numberOfPoints; i++)
	{
		float t = i*dt;
		float ax, bx, cx;
		float ay, by, cy;
		float tSquared, tCubed;
		double x;
		double y;

		/*計算多項式係數*/

		cx = 3.0 * (pts_[1][0] - pts_[0][0]);
		bx = 3.0 * (pts_[2][0] - pts_[1][0]) - cx;
		ax = pts_[3][0] - pts_[0][0] - cx - bx;

		cy = 3.0 * (pts_[1][1] - pts_[0][1]);
		by = 3.0 * (pts_[2][1] - pts_[1][1]) - cy;
		ay = pts_[3][1] - pts_[0][1] - cy - by;

		/*計算位於參數值t的曲線點*/

		tSquared = t * t;
		tCubed = tSquared * t;

		x = (ax * tCubed) + (bx * tSquared) + (cx * t) + pts_[0][0];
		y = (ay * tCubed) + (by * tSquared) + (cy * t) + pts_[0][1];
		//cout << x << " " << y << endl;
		this->samplepoints.push_back(_v2d_(x, y));
	}
}

bool ElementSegBezier::ElementSegCompare(ElementSegBezier * es)
{
	//compare the two elementseg
	if (this->pts_[0] == es->pts_[0] && this->pts_[1] == es->pts_[1] && this->pts_[2] == es->pts_[2] && this->pts_[3] == es->pts_[3])
	{
		return true;
	}

	return false;
}