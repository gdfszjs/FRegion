#ifndef FR_NODE_H_
#define FR_NODE_H_

#include "Common.h"

class FRNode
{
public:
	FRNode							()						{ set_left_node(0); set_right_node(0); }

	virtual		~FRNode				()						{ }

	virtual void Draw				()						= 0;
	virtual void DrawControlPoints	()						= 0;
	
	virtual int  SelectControlPoint	(const v2d & pt) const	= 0;

	virtual void set_pts			(int idx, v2d pt)		= 0;
	virtual v2d  pts				(int idx)				= 0;
	void         set_left_node		(FRNode * node)			{ left_node_  = node; }
	void         set_right_node		(FRNode* node)			{ right_node_ = node; }
	FRNode *	 left_node			()						{ return left_node_; }
	FRNode *	 right_node			()						{ return right_node_; }
	virtual v2d  GetFirstEnd		()						= 0;
	virtual v2d  GetLastEnd			()						= 0;
	virtual void SetFirstEnd		(const v2d & pt)		= 0;
	virtual void SetLastEnd			(const v2d & pt)		= 0;

	virtual int  NodeType			()						= 0;

	virtual void SamplePoints		(std::vector<v2d> & pts, double dt)			= 0;
	virtual void ReverseSamplePoints(std::vector<v2d> & pts, double dt)			= 0;
	virtual void SamplePointsByGap	(std::vector<v2d> & pts, double gap, int grain_num)		= 0;
	

	virtual void translate			(double x, double y)						= 0;
	virtual void scale				(double x, double y)						= 0;
	virtual void rotate				(double t)									= 0;

	virtual double		Length				(double t)							= 0;
	virtual double		LengthTo_t			(double l)						    = 0;
	virtual void		tToPoint			(v2d & pt, double t)				= 0;
	virtual FRNode*		CopySelf			()									= 0;
	virtual FRNode*		CopyInverseSelf		()									= 0;

protected:
	FRNode * left_node_;
	FRNode * right_node_;

};

class FRNodeLine : public FRNode
{
public:
	FRNodeLine()  { }
	~FRNodeLine() { }
	
	virtual void	Draw							();	
	virtual void	DrawControlPoints				();
	virtual void	set_pts							(int idx, v2d pt);
	virtual v2d		pts								(int idx);
	virtual v2d		GetFirstEnd						()									{ return pts_[0]; }
	virtual v2d  	GetLastEnd						()									{ return pts_[1]; }
	virtual void 	SetFirstEnd						(const v2d & pt)					{ pts_[0] = pt; }
	virtual void 	SetLastEnd						(const v2d & pt)					{ pts_[1] = pt; }
	virtual int  	SelectControlPoint				(const v2d & pt) const;
	virtual int  	NodeType						()									{ return 0; }

	virtual void SamplePoints		(std::vector<v2d> & pts, double dt);
	virtual void ReverseSamplePoints(std::vector<v2d> & pts, double dt);
	virtual void SamplePointsByGap	(std::vector<v2d> & pts, double gap, int grain_num);
	
	virtual void translate			(double x, double y);
	virtual void scale				(double x, double y);
	virtual void rotate				(double t);

	virtual double Length			(double t);
	virtual double LengthTo_t		(double l);
	virtual void   tToPoint			(v2d & pt, double t);

	virtual	FRNode*		CopySelf		();
	virtual FRNode*		CopyInverseSelf	();
	
private:
	v2d pts_[2];
};

class FRNodeBezier : public FRNode
{
public:
	FRNodeBezier() { }
	~FRNodeBezier() { }

	virtual void	Draw							();
	virtual void	DrawControlPoints				();
	virtual void 	set_pts							(int idx, v2d pt);
	virtual v2d  	pts								(int idx);
	virtual v2d  	GetFirstEnd						()									{ return pts_[0]; }
	virtual v2d  	GetLastEnd						()									{ return pts_[3]; }
	virtual void 	SetFirstEnd						(const v2d & pt)					{ pts_[0] = pt; }
	virtual void 	SetLastEnd						(const v2d & pt)					{ pts_[3] = pt; }
	virtual int  	SelectControlPoint				(const v2d & pt) const;
	virtual int  	NodeType						()									{ return 1; }

	virtual void SamplePoints		(std::vector<v2d> & pts, double dt);
	virtual void ReverseSamplePoints(std::vector<v2d> & pts, double dt);
	virtual void SamplePointsByGap	(std::vector<v2d> & pts, double gap, int grain_num);

	virtual void translate			(double x, double y);
	virtual void scale				(double x, double y);
	virtual void rotate				(double t);

	virtual double Length			(double t);
	virtual double LengthTo_t		(double l);
	virtual void   tToPoint			(v2d & pt, double t);
	virtual FRNode*		CopySelf		();
	virtual FRNode*		CopyInverseSelf	();
private:
	v2d pts_[4];
};

#endif // FR_NODE_H_