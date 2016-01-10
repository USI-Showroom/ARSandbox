#include "Triangulator.hpp"

#include <fstream>
#include <sstream>

Triangulator::Triangulator()
{}


Triangulator::~Triangulator()
{}

void Triangulator::assignValues(triangulateio &in) const
{
	const int pointsSize=_points.size();

	in.numberofpoints = pointsSize;
	in.numberofpointattributes = 0;

	in.pointlist = (double *) malloc(in.numberofpoints * 2 * sizeof(double));
	in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));

	in.numberofsegments = 0;
	in.segmentlist = (int *) malloc(in.numberofsegments * 2 * sizeof(int));
	in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));


	for(int i = 0;i< pointsSize; ++i)
	{
		in.pointlist[2*i]=_points[i].x();
		in.pointlist[2*i+1]=_points[i].y();

		in.pointmarkerlist[i] = 0;
	}


	in.numberofholes = 0;
	in.numberofregions = 0;
}

void Triangulator::triangulateValues(triangulateio &mid, triangulateio &out)
{
	out.pointlist = (double *) NULL;
	out.pointmarkerlist = (int *) NULL;

	out.trianglelist = (int *) NULL;
	out.triangleattributelist = (double *) NULL;

	out.segmentlist = (int *) NULL;
	out.segmentmarkerlist = (int *) NULL;

	out.edgelist=(int *) NULL;
	out.edgemarkerlist = (int *) NULL;

	std::stringstream buf;
	buf.precision(100);
	buf.setf(std::ios::fixed, std::ios::floatfield);


	buf<<"Q";			
	buf<< "ez";
	buf<<"YY";
	buf<<"S0";


	char* str = new char[buf.str().size() + 1];
	strcpy(str, buf.str().c_str());

// std::cout<<str<<std::endl;
	trianglelib::triangulate(str, &mid, &out, (triangulateio *) NULL);

	delete[] str;
}




void Triangulator::saveTriangulation(triangulateio &triangulation, const std::string &path)
{
	std::ofstream file;
	file.open(path);

	for (int i = 0; i < triangulation.numberofpoints; ++i) { 
		const double x = triangulation.pointlist[i * 2 ];
		const double y = triangulation.pointlist[i * 2 + 1];
		const double h = _heigths[i];

		file<<"v "<<x<<" "<<y<<" "<<h<<"\n";
	}

	file<<"\n";

	for(int i=0;i<triangulation.numberoftriangles;++i)
	{
		const int i1 = triangulation.trianglelist[i * 3];
		const int i2 = triangulation.trianglelist[i * 3+1];
		const int i3 = triangulation.trianglelist[i * 3+2];

		file<<"f "<<i1<<" "<<i2<<" "<<i3<<"\n";
	}

	file.close();
}

void Triangulator::freeIn(triangulateio &in)
{
	free(in.pointlist);
	free(in.pointmarkerlist);

	free(in.segmentlist);
	free(in.segmentmarkerlist);
}

void Triangulator::freeOut(triangulateio &out)
{
	free(out.pointlist);
	free(out.pointmarkerlist);

	free(out.trianglelist);
	free(out.triangleattributelist);

	free(out.segmentlist);
	free(out.segmentmarkerlist);

	free(out.edgelist);
	free(out.edgemarkerlist);

}
