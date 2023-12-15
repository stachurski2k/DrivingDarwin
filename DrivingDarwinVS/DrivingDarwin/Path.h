#pragma once
#include<fstream>
#include<glm.hpp>
#include<string>
#include<sstream>
#include<vector>
#include<cstring>
#include"Utils.h"
using namespace std;
using namespace glm;

/*
Path is responsible for loading and storing bezier paths
( could implement inheritance here but as i wont use other types of paths lets keep it simple)
as well as calulating points on this path given spacing value
*/
class Path {
public:
	bool closedPath;// path can be opened or closed
	vector<vec2> points;
	static Path LoadPath(string filePath) {
		Path p;
		ifstream fs(filePath);
		stringstream data;
		data << fs.rdbuf();
		string datastr;
		datastr = data.str();
		const char* realdata = datastr.c_str();

		//Points are stored as vec3 of 4b floats, 8b of data are for closedPath and autoSet(something usefull for my editor toll)
		int dataCount = datastr.size() / 4-3;
		int x;
		memcpy(&x, realdata+4, 4);
		p.closedPath = !(x == 0);

		//block copy data and construct vector of vec2
		float* pointData=new float[dataCount];
		memcpy(pointData, realdata + 12, dataCount * 4);
		for (int i = 0; i < dataCount; i+=2)
		{
			p.points.push_back(vec2(pointData[i], pointData[i + 1]));
		}
		delete[] pointData;
		fs.close();
		return p;
	}
	int segmentCount() { return points.size() / 3; }//segment consists of 4 points, however it shares one of its points with next
	int loopi(int i) {//make sure to go from points.size-1 to 0 when path is closed
		return (i + points.size()) % points.size();
	}
	vec2x4 getPointsInSegment(int i) {//in bezier curve 4 points are used for one segment
		return { points[i * 3],points[i * 3 + 1],points[i * 3 + 2],points[loopi(i * 3 + 3)] };
	}
	/*
	For constructing the mesh points in points vector are not enough.
	For this implementation i will go through every segment and bezier lerp between its 4 points,
	and create new points with correct spacing between them
	*/
	vector<vec2> getEvenlySplitPoints(float spacing, float resolution = 10) {
		vector<vec2> ep;
		ep.push_back(points[0]);// start with point 0
		vec2 lastp = points[0];
		float distSinceLastEvenPoint = 0;//distance since last point with correct spacing
		for (int i = 0; i < segmentCount(); i++)
		{
			float t = 0;//lerp value
			vec2x4 segment = getPointsInSegment(i);
			float len = Utils::dist(segment.v1, segment.v4) +
				0.5f * (Utils::dist(segment.v1, segment.v2) + Utils::dist(segment.v2, segment.v3) + Utils::dist(segment.v3, segment.v4));
			//The formula for calculating len is an aproximation of the real length of bezier curve

			while (t <= 1) {
				//increment t every step by len to create points on bezier curve
				t += 1/(len*resolution);

				vec2 p = Utils::lerpCubic(segment, t);//get lerp point on bezier curve
				distSinceLastEvenPoint += Utils::dist(p, lastp);//check how much distance since last correct point

				while (distSinceLastEvenPoint >= spacing) {
					//while points between curent lerp point and last correct point can be created do that

					float overshoot = distSinceLastEvenPoint - spacing;//check how far from last correct point
					vec2 direction = Utils::normalize(lastp - p);
					vec2 np=p+ direction *overshoot;//corect lerp point by overshoot distance
					ep.push_back(np);

					//prepare for rest of points
					distSinceLastEvenPoint = overshoot;
					lastp = np;
				}
				lastp = p;
			}
		}
		//if there is closed path then there is very litle chance that last even point was last bezier point
		if (!closedPath) {
			ep.push_back(points[points.size() - 1]);
		}
		return ep;
	}
};