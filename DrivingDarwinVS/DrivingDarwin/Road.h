#pragma once
#include<glm.hpp>
#include"Path.h"
#include"Utils.h"
#include"Mesh.h"
#include"CollisionMesh.h"
/*
* This class is responsible for creating the road mesh from path and collison mesh
*/
class Road {
public:
	Mesh mesh;
	Path path;
	float roadSize;
	float spacing = 1.7f;//the spacing value between new mesh points on road
	float textureRepetition = 0.3f;//factor for tilling road texture
	CollisionMesh collissionMesh;
	vector<pair<vec2, vec2>> distMesh;//cars score points when coliding with next segment
	Road(float roadSize=2.0f){
		mesh = Mesh();
		this->roadSize = roadSize;
		
	};
	void createPath(string pathFile) {
		path = Path::LoadPath(pathFile);
	}
	void setRoadTexture(Texture tex) {
		mesh.setTexture(tex);
	}
	vec2 getStartPos() {
		return path.points[0];
	}
	vec2 getStartDir() {//there is always a path of a least 2 points
		vec2 dir = path.points[1] - path.points[0];
		dir = normalize(dir);
		return dir;
	}
	pair<vec2, vec2> getDistPoint(int index) {
		index = index % distMesh.size();
		return distMesh[index];
	}
	/*
	Creting mesh is actually pretty easy,
	Get even points on the path, for each point check what its left and right direction is
	and expand mesh in that directions
	*/
	void createMesh() {
		collissionMesh.clear();
		collissionMesh.createSubmesh(path.closedPath);//there are 2 separate boundries of road(left and right)
		collissionMesh.createSubmesh(path.closedPath);//is closedPath then there is a submesh with a loop
		vector<vec2> points = path.getEvenlySplitPoints(spacing, 60.0f);
		vector<Vertex> verticies;
		vector<unsigned int> triangles;
		float totalDist = 0;//for tilling the road

		for (int i = 0; i < points.size(); i++)
		{
			//dir is used to calculate left and right, basicialy it is mean of direction to next and minus direction to last point
			vec2 dir = { 0,0 };
			if (i != 0 || path.closedPath) {//for first point in open path there is no previous point
				vec2 prevPoint = points[(i-1+points.size())%points.size()];
				dir += Utils::normalize(points[i] - prevPoint);
				totalDist += Utils::dist(prevPoint, points[i]);
			}
			if (i != points.size() - 1 || path.closedPath) {//for last point in open path there is no next point
				vec2 nextPoint = points[(i + 1) % points.size()];
				dir += Utils::normalize(nextPoint - points[i]);
			}
			if ((i != 0 && i != points.size()) || path.closedPath) {
				dir = dir / 2.0f;//mean the result
			}
			vec2 right = { dir.y,-dir.x };
			vec2 left = {-dir.y,dir.x };
			//create new vertecies
			vec2 rightPoint = (points[i] + right * roadSize);
			vec2 leftPoint = (points[i] + left * roadSize);

			verticies.push_back(Vertex(rightPoint,vec2(1,totalDist*textureRepetition)));
			verticies.push_back(Vertex(leftPoint, vec2(0, totalDist*textureRepetition)));
			distMesh.push_back({ leftPoint,rightPoint });

			collissionMesh.add(0, rightPoint);//0th submesh for right boundry
			collissionMesh.add(1, leftPoint);//1st submesh for left boundry
			if (i != points.size() - 1) {
				unsigned int v = (unsigned int)i;
				//create triangles for the mesh
				triangles.insert(triangles.end(), {v * 2, v * 2 + 1, v * 2 + 2,v * 2 + 2, v * 2 + 1, v * 2 + 3});
			}else if (path.closedPath) {
				totalDist += Utils::dist(points[0], points[i]);
				unsigned int v = (unsigned int)i;
				//to avoid strange artifacts we need to add new points which overlap point 0 and 1
				verticies.push_back(Vertex(verticies[0].position, vec2(1, totalDist * textureRepetition)));
				verticies.push_back(Vertex(verticies[1].position, vec2(0, totalDist * textureRepetition)));
				triangles.insert(triangles.end(), { v * 2, v * 2 + 1, v * 2 + 2,v * 2 + 2, v * 2 + 1, v * 2 + 3 });
			}
		}
		mesh.updateMesh(verticies, triangles);
	}
	void draw(Shader* shader) {
		mesh.draw(shader, true);
	}
};