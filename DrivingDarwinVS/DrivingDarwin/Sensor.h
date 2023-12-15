#pragma once
#ifndef SENSOR

#define SENSOR
#include<glm.hpp>
#define _USE_MATH_DEFINES
#include<math.h>
#include"Primitives.h"
#include"Utils.h"
#include"CollisionSystem.h"
/*
Sensor class will cast rays and check whether they intersect with any object
*/
class Sensor {
public:
	int rayCount = 9;
	float maxAngle = M_PI_2;
	float rayLength = 2.5f;
	vector<pair<vec2, vec2>> rays;//AB
	vector<CollisionData> collisions;//one for each ray
	Sensor() {};
	Sensor(int rc, float maxA, float rl) {
		rayCount = rc;
		maxAngle = maxA;
		rayLength = rl;
		for (int i = 0; i < rayCount; i++) {
			collisions.push_back({vec2(0,0),0,false});
		}
	}
	void updateRays(float carAngle, vec2 carPos, CollisionMesh* environment) {
		rays.clear();
		//create rays in direction of car
		for (int i = 0; i < rayCount; i++)
		{
			//similar direction calculation to car movement
			float angle = Utils::lerp(-maxAngle, maxAngle, rayCount == 1 ? 1 : ((float)i / (rayCount - 1))) + carAngle;
			vec2 A = carPos;
			vec2 B = vec2(carPos.x + cos(angle) * rayLength, carPos.y + sin(angle) * rayLength);
			rays.push_back({ A,B });
		}
		collisions.clear();
		//now check collision of those rays with environmnet(road)
		for (auto r : rays)
		{
			vector<CollisionData> data = CollisionSystem::getSegmentMeshCollision(r.first, r.second, environment);
			float best = 1.1;
			int bestI = -1;
			for (int i = 0; i < data.size(); i++)
			{
				//get the closest intersection point
				if (data[i].offset < best && data[i].actualCollision) {
					best = data[i].offset;
					bestI = i;
				}
			}
			if (bestI != -1) {
				collisions.push_back(data[bestI]);
			}
			else {
				collisions.push_back({ vec2(0,0),1,0 });//no collision so maximum offset
			}
			data.clear();
		}
	}
	void draw(Shader* shader) {
		for (auto r : rays) {
			Primitives::drawLine(r.first, r.second, 0.01f, shader);
		}
		//draw also collision points if any
		for (auto c : collisions) {
			if (c.actualCollision) {
				Primitives::drawCube(c.position, 0.05f, shader, vec3(1, 0, 0));
			}
		}
	}
};
#endif 
