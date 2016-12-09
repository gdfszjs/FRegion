#ifndef GEO_2D_H_
#define GEO_2D_H_

#include "Common.h"

class	SVGPattern;
struct	triangulateio;
class	FRNode;

namespace rmsmesh
{
	class TriangleMesh;
}

class Geo2D
{
public:
	/* general */
	static void			InterpLinear				(v2d & v, double t, const v2d & v0, const v2d & v1);

	/* reflection over line */
	static void			PointReflectionOverLine		(v2d & v, const v2d & p, const v2d & v1, const v2d & v2);
	static SVGPattern * PatternReflectionOverLine	(SVGPattern * pattern, const v2d & v1, const v2d & v2);
	
	/* line */
	static void			LineParams					(double & A, double & B, double & C, const v2d & v1, const v2d & v2);
	static void			PointOnLine					(v2d & pt, double t, const v2d & p0, const v2d & p1);

	/* point and line		*/
	static bool			IsPointOnLineseg			(const v2d & v, const v2d & v1, const v2d & v2, double precision = 0.0000001);
	static double		DistanceBetweenPointLineseg (v2d & v, const v2d & p, const v2d & v1, const v2d & v2);
	static double		DistanceBetweenPointTriangle(const v2d & p, const v2d & p0, const v2d & p1, const v2d & p2);
	static double		tOfPointOnLine				(const v2d & v, const v2d & v0, const v2d & v1);

	/* bezier */
	static void			BezierParams					(v4d & coex, v4d & coey, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static double		DistanceBetweenPointBezier		(v2d & v, const v2d & p, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static void			SplitBezier						(std::vector<v2d> & b1, std::vector<v2d> & b2, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static void			SplitBezier						(std::vector<std::vector<v2d>> & beziers, const std::vector<v2d> & pts, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static double		tOfPointOnBezier				(const v2d & v, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static void			CubicRoots						(std::vector<double> & roots, const v4d & bezier_coeff);
	static void			PointOnBezier					(v2d & pt, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static void			PointOnBezier					(v2d & pt, double t, const v2d & p0, const v2d & p1, const v2d & p2);
	static void			PointOnCubicBezier_deCasteljau	(v2d & pt, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static void			PointOnQuadratic_deCasteljau	(v2d & pt, double t, const v2d & p0, const v2d & p1, const v2d & p2);
	static void			GetPointProjection				(double & t, const v2d & p, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static double		RefineProjection_				(double t, double dist, double precision, const v2d & p, const  v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);

	/* tangents and normals of Bezier */
	static void			DerivativeOnCubicBezier			(v2d & dt, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static void			SecondDerivativeOnCubicBezier	(v2d & dt, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static void			TangentOnCubicBeizer			(v2d & ta, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static void			NormalOnCubicBezier				(v2d & no, double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);

	/* the art length of bezier curve */
	/* refer to http://pomax.github.io/bezierinfo/#arclength */
	static double		GetCubicBeizerArcLength			(double t, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);
	static double		CubicBeizerArcLengthTo_t		(double l, const v2d & p0, const v2d & p1, const v2d & p2, const v2d & p3);

	/* 2D triangle mesh 
		triangulateio is the basic structure of Triangle. The tasks are:
		1. what's the form of your input?
		2. change your input to get its corresponding triangulateio
		3. what's the form of your output?
		4. get your output from triangulateio
	*/
	static void					InitTriangulateio			(triangulateio ** pptio);
	static void					FreeTriangulateio			(triangulateio ** pptio, bool in = false);
	
	static triangulateio *		InputToTriangulateio		(const std::vector<v2d> & input_points, const std::vector<v2i> & input_segments);
	static triangulateio *		InputToTriangulateioWithHole(const std::vector<v2d> & input_points, const std::vector<v2i> & input_segments, const std::vector<v2d> & input_holes);
	static triangulateio *		ComputeMeshByTriangle		(triangulateio * tio);
	static void					TriangulateioToFile			(triangulateio * tio,string filename);
	static void					TriangulateioToOutput		(triangulateio * tio, rmsmesh::TriangleMesh & mesh);

	/* point and polygon */
	static bool					PointInPolygon				(const v2d & pt, const std::vector<FRNode*> & poly);
};

#endif // GEO_2D_H_