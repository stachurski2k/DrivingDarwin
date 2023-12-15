#pragma once
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include"Controlls.h"
#include"Mesh.h";
#include"CollisionMesh.h"
#include"Sensor.h"
#include"Primitives.h"
#include"Utils.h"
using namespace glm;
/*
Car class is responsible for creating car mesh, using sensors and controlling movement of the car
*/
class Car {
public:
	float maxSpeed = 10.0f;
	float currentSpeed = 0.0f;
	float angle = 0;//angle of car, according to positive x axis
	vec2 position;
	float width,height;
	float acceleration=5.7f;
	float friction = 2.7f;
	float rotationSpeed = 4.1f;
	vec3 carColor;
	CarControlls* controlls;// controlls should not belong to car and be handled outside!
	CollisionMesh collisionMesh;
	Mesh carMesh;
	mat4 model;//world matrix(local to world)
	Sensor sensor;
	int lastPointOnPath = 0;//for checking if car is actually driving forward
	bool alive = true;//after hiting the edge of a road car should no be able to move
	static int getBestCar(vector<Car*> cars ) {
		int i = 0;
		int b = -1;
		for (int x = 0; x < cars.size(); x++) {
			if (cars[x]->lastPointOnPath > b) {
				b = cars[x]->lastPointOnPath;
				i = x;
			}
		}
		return i;
	}
	Car(CarControlls* ctr, vec3 carColor=vec3(1,0,0), float maxS = 10.3f, float width = 1.0f, float height = 0.6f, int rayCount = 7, float rayAngle = M_PI_2, float rayLength = 2.5f) {
		controlls = ctr;
		maxSpeed = maxS;
		this->width = width;
		this->height = height;
		createMesh();
		collisionMesh.createSubmesh(true);
		sensor = Sensor(rayCount, rayAngle, rayLength);
		this->carColor = carColor;
	}
	void setControlls(CarControlls* ctr) {
		controlls = ctr;
	}
	void setCarTexture(Texture tex) {
		carMesh.setTexture(tex);
	}
	void setPosDir(vec2 pos, vec2 dir) {
		this->position = pos;
		this->angle = Utils::angle(dir, vec2(1, 0));
	}
	void update(float deltaTime,CollisionMesh* environment) {
		controlls->raysData = &sensor.collisions;
		boolx4 c = controlls->getMovementDirections();
		//forward backward movement
		if (c.v1) {
			currentSpeed += acceleration * deltaTime;
		}
		if (c.v2) {
			currentSpeed -= acceleration * deltaTime;
		}
		//clamp speed
		if (currentSpeed > maxSpeed) {
			currentSpeed = maxSpeed;
		}
		if (currentSpeed < -maxSpeed / 2) {
			currentSpeed = -maxSpeed / 2;
		}
		//car should slow down when no force is aplied
		currentSpeed += ((currentSpeed > 0) ?- friction :friction)*deltaTime;
		if (abs(currentSpeed) <= friction*deltaTime) {//avoid movement coused by friction
			currentSpeed = 0;
		}
		if (currentSpeed != 0) {
			float flip = (currentSpeed > 0) ? 1 : -1;
			//for backward movement
			if (c.v3) {
				angle -= rotationSpeed * deltaTime*flip;
			}
			if (c.v4) {
				angle += rotationSpeed * deltaTime * flip;
			}
		}
		//update position based on angle, angle is from top direction
		position = { position.x + cos(angle) * currentSpeed*deltaTime,position.y + sin(angle) * currentSpeed*deltaTime };

		//create world matrix, apply rotation by angle
		model=mat4(1.0f);
		model = translate(model ,vec3(position.x, position.y, 0));
		model = rotate(model, angle, vec3(0, 0, 1));

		//create collision vertices
		collisionMesh.clear(0);
		for (Vertex v : carMesh.vertices) {
			vec4 p = vec4(v.position.x, v.position.y, v.position.z, 1);//mul with 4x4 mat requires vec4
			p = model * p;
			collisionMesh.add(0, vec2(p.x, p.y));
		}
		sensor.updateRays(angle, position,environment);
	}
	void draw(Shader* shader,Shader* noTex,bool drawSensor) {//asuming shader is alredy in use as many cars can be drawn(maybe instancing in future?)
		shader->use();
		shader->setMat4("model", model);
		shader->setVec3("color", carColor);
		carMesh.draw(shader, true);
		if(drawSensor)
			sensor.draw(noTex);
	}
	vec2 getForward() {
		return{ cos(angle)  , sin(angle) };
	}
	~Car() {
		carMesh.clear();
		sensor.rays.clear();
		collisionMesh.clear();
		sensor.collisions.clear();
	}
private:
	void createMesh() {
		//car should be a rectangle
		carMesh = Mesh();
		vector<Vertex> v;
		vector<unsigned int> t;
		//simple rectangle
		v.push_back(Vertex( vec2( - width / 2, -height / 2 ), vec2(0,0) ));
		v.push_back(Vertex(vec2(width / 2, -height / 2), vec2(1, 0)));
		v.push_back(Vertex(vec2(width / 2, height / 2), vec2(1, 1)));
		v.push_back(Vertex(vec2(-width / 2, height / 2), vec2(0, 1)));
		t.insert(t.end(), { 0u,1u,2u,0u,2u,3u });
		carMesh.updateMesh(v, t);
		v.clear(); t.clear();
	}
};