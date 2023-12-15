#pragma once
#include<glm.hpp>
#include <cstdlib>
#include <ctime>

using namespace glm;
/*
Usefull functions for avoiding repetition
*/
struct vec2x4 {
	vec2 v1, v2, v3, v4;
};
struct boolx4 {
	bool v1, v2,  v3,  v4;
};
class Utils {
public:
	static float lerp(float a, float b,float t) {
		return a + (b - a) * t;
	}
	static vec2 lerp(vec2 v1, vec2 v2, float t) {
		return  v1 + (v2 - v1) * t;
	}
	static vec2 lerpSqr(vec2 v1, vec2 v2,vec2 v3, float t) {
		vec2 a = lerp(v1, v2, t);
		vec2 b = lerp(v2, v3, t);
		return lerp(a, b, t);
	}
	static vec2 lerpCubic(vec2 v1, vec2 v2, vec2 v3, vec2 v4, float t) {
		vec2 a = lerpSqr(v1, v2, v3, t);
		vec2 b = lerpSqr(v2, v3, v4, t);
		return lerp(a, b, t);
	}
	static vec2 lerpCubic(vec2x4 s, float t) {
		return lerpCubic(s.v1, s.v2, s.v3, s.v4, t);
	}
	static float dist(vec2 a, vec2 b) {
		vec2 c = a - b;
		return sqrt(c.x * c.x + c.y * c.y);
	}
	static vec2 normalize(vec2 v) {
		float mag = sqrt(v.x * v.x + v.y * v.y);
		return v / mag;
	}
	static float mag(vec2 a) {
		return sqrt(a.x * a.x + a.y * a.y);
	}
	static  float angle(vec2 a, vec2 b) {
		float mag1 = mag(a);
		float mag2 = mag(b);
		return acos((a.x * b.x + a.y * b.y) / (mag1 * mag2));
	}
	static void resetRandom() {
		srand((unsigned int)time(NULL));
	}
	static float randomFloat01() {
		return  float(rand()) / float((RAND_MAX));
	}
};
