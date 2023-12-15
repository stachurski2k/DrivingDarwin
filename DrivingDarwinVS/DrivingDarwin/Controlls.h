#pragma once
#include<map>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Utils.h"
#include"CollisionSystem.h"
#include<string>
using namespace std;

//for each frame key can have 3 states
struct KeyInfo {
	bool wasPressed = false;
	bool isPressed=false;
	bool wasReleased = false;
	int keyValue = -1;
};
/*
Controlls are responsible for listening to input of registered keys(in keys map)
*/
class Keyboard {
public:
	map<int, KeyInfo> keys;
	KeyInfo getKeyInfo(int k) {
		if (keys.find(k) != keys.end()) {
			return keys[k];
		}
		return{ 0,0,0,-1 };
	}
	//register key
	void enableKey(int key) {
		if (keys.find(key) == keys.end())
			keys.insert({ key, { 0,0,0,key } });
	}
	void disableKey(int key) {
		if (keys.find(key) != keys.end())
			keys.erase(key);
	}
	//should be called every frame, checks for new input
	void update(GLFWwindow* window) {
		for (auto k : keys) {
			bool pressed = glfwGetKey(window, k.first);
			if (!keys[k.first].isPressed && pressed) {
				keys[k.first].wasPressed = true;
			}
			else {
				keys[k.first].wasPressed = false;
			}
			if (keys[k.first].isPressed && !pressed) {
				keys[k.first].wasReleased = true;
			}
			else {
				keys[k.first].wasReleased = false;
			}
			keys[k.first].isPressed = pressed;
		}
	}
};
//car controlls can be either human or ai
class CarControlls{
public:
	vector<CollisionData>* raysData;
	boolx4 virtual getMovementDirections() { return{ 1,0,0,0 }; };
};
//make use of keyboard for car input
class KeyboardControlls:public CarControlls {//human controlls
public:
	Keyboard* keyboard;
	KeyboardControlls(Keyboard* k) {
		keyboard = k;
		k->enableKey(GLFW_KEY_W);
		k->enableKey(GLFW_KEY_S);
		k->enableKey(GLFW_KEY_D);
		k->enableKey(GLFW_KEY_A);

	}
	boolx4 virtual getMovementDirections() override{
		return{ keyboard->getKeyInfo(GLFW_KEY_W).isPressed,keyboard->getKeyInfo(GLFW_KEY_S).isPressed,
			keyboard->getKeyInfo(GLFW_KEY_D).isPressed,keyboard->getKeyInfo(GLFW_KEY_A).isPressed };
	}
};
