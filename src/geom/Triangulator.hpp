#ifndef TRIANGULATOR_H__
#define TRIANGULATOR_H__

#include <vector>
#include "Point2.hpp"
#include "triangle/triangle.hpp"


class Triangulator {
private:
	typedef struct trianglelib::triangulateio triangulateio;

	std::vector<Point2d> _points;
	std::vector<double> _heigths;

public:
	Triangulator();
	virtual ~Triangulator();

	inline void addPoint(const Point2d &p, const double h)
	{
		_points.push_back(p);
		_heigths.push_back(h);
	}

	inline void save(const std::string &path) 
	{
		triangulateio out;
		triangulateio in;

		assignValues(in);
		triangulateValues(in, out);

		freeIn(in);

		saveTriangulation(out, path);
		freeOut(out);
	}



private:
	void assignValues(triangulateio &in) const;

	void triangulateValues(triangulateio &mid, triangulateio &out);
	void saveTriangulation(triangulateio &triangulation, const std::string &path);

	void freeIn(triangulateio &in);
	void freeOut(triangulateio &out);
};



#endif // TRIANGULATOR_H__
