#pragma once
#include"CollisionMesh.h"
#include<vector>
#include"Utils.h"
/*
Interface for checking mesh collision, not efficient for now :(
*/
struct CollisionData {
	vec2 position;
	float offset;
	bool actualCollision;
};
class CollisionSystem {
public:
	//https://www.youtube.com/watch?v=5FkOO1Wwb8w&t=0s
	//segment intersection formula ^^^^
	static CollisionData getSegmentIntersection(vec2 A, vec2 B, vec2 C, vec2 D) {//ab segment cd segment
		float tTop = (D.x - C.x) * (A.y - C.y) - (D.y - C.y) * (A.x - C.x);
		float uTop = (C.y - A.y) * (A.x - B.x) - (C.x - A.x) * (A.y - B.y);
		float bottom = (D.y - C.y) * (B.x - A.x) - (D.x - C.x) * (B.y - A.y);
		if (bottom != 0) {
			float t = tTop / bottom;
			float u = uTop / bottom;
			if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
				return {Utils::lerp(A,B,t),t,1};
			}
		}
		return{ vec2(0,0),1,0 };
	}
	//check segment ab against whole mesh m
	static vector<CollisionData> getSegmentMeshCollision(vec2 A,vec2 B,CollisionMesh* m) {
		vector<CollisionData> c;
		for (auto s : m->meshes) {
			for (int i = 0; i < s.vertices.size() - 1; i++) {
				CollisionData d = getSegmentIntersection(A, B, s.vertices[i], s.vertices[i + 1]);
				if(d.actualCollision)
					c.push_back(d);
			}
			if (s.loop) {
				CollisionData d = getSegmentIntersection(A, B, s.vertices[s.vertices.size()-1], s.vertices[0]);
				if(d.actualCollision)
					c.push_back(d);

			}
		}
		return c;
	}
	static vector<CollisionData> checkCollision(CollisionMesh& m1, CollisionMesh& m2) {
		//just check every segment of m1 with m2, very ineficient
		vector<CollisionData> collisions;
		for (auto s1:m1.meshes)
		{
			for (int i = 0; i < s1.vertices.size()-1; i++) {
				auto c = getSegmentMeshCollision(s1.vertices[i], s1.vertices[i+1], &m2);
				collisions.insert(collisions.end(), c.begin(), c.end());
				c.clear();
			}
			if (s1.loop) {
				auto c = getSegmentMeshCollision(s1.vertices[s1.vertices.size() - 1], s1.vertices[0],&m2);
				collisions.insert(collisions.end(), c.begin(), c.end());
				c.clear();
			}
		}
		return collisions;
	}
};