#pragma once
#include"NeuralNetwork.h"
#include"Car.h"
#include"Road.h"
//all scenes should have access to same common staff like pointer to gl window, they are loaded in DrivingDarvin.cpp
class Resources {
public:
	Shader* imgShader;
	Shader* colorShader;
    Shader* carShader;
	Mesh carMesh;
	Keyboard keyboard;//user keyboard
	Texture carTexture;
	Texture roadTexture;
    Road road;//for now i asume each scene uses the same road
    GLFWwindow* window;
    float cameraSize;//for projection matrix
    bool showDebug = true;//for example rays
};
//scene template
class Scene {
public:
	Resources* res;//all scenes should have access to common resources loaded on start
    virtual void loadScene(Resources* r) { res = r; }
	virtual void update(float deltaTime){}
	virtual void dispose(){}
};
//ai driving
class AIScene:public Scene {
public:
    /*Simple scene where one can tech cars how to drive for more detail how to play with this scene see redme.txt*/
	vector<Car*> cars;
	vector<NeuralNetwork*> brains;
	const vector<int> levels = { 6 };//of neural network, all cars should have the same structure of brain
	int carNum = 30;;//cars for each generation
	int lastBestIndex = 0;//track best car to adjust camera to its pos
	int lastBest = -1;
    float learnRate=0.1f;//how big the random mutations of cars are after each generation
    float learnRateUp = 0.002f;
    bool init = false;//
    //move car to start and reset its brain
    void reset_car(int i) {
        cars[i]->alive = true;
        cars[i]->setPosDir(res->road.getStartPos(), res->road.getStartDir());
        brains[i]->randomize();
        cars[i]->lastPointOnPath = 0;
    }
	void loadScene(Resources* r) override {
        res = r;
        if (!init) {//load cars only once
            init = true;
            Utils::resetRandom();
            for (int i = 0; i < carNum; i++) {
                Car* car = new Car(nullptr);
                NeuralNetwork* nn = new NeuralNetwork(car->sensor.rayCount, 4, levels);
                AIControlls* controlls = new AIControlls(nn);
                car->setCarTexture(res->carTexture);
                car->setControlls(controlls);
                car->setPosDir(res->road.getStartPos(), res->road.getStartDir());
                cars.push_back(car);
                brains.push_back(nn);
            }
        }
        else {// reset cars if loading second time
            for (int i = 0; i < carNum; i++)
            {
                reset_car(i);
            }
            lastBest = -1;
            lastBestIndex = 0;
        }
		
	}
	void update(float deltaTime) override {
        for (auto c : cars) {
            if (c->alive)
                c->update(0.02f, &(res->road.collissionMesh));
        }
        //get index of best car in the array
        int currentBest = Car::getBestCar(cars);
        //is there a new best car?
        if (currentBest != lastBestIndex && cars[currentBest]->lastPointOnPath > cars[lastBestIndex]->lastPointOnPath+1) {
            lastBest = cars[currentBest]->lastPointOnPath;
            lastBestIndex = currentBest;
        }
        //set window title with respect to best car score
        vec2 p = cars[lastBestIndex]->position;
        glfwSetWindowTitle(res->window, ("Score : " + to_string(cars[lastBestIndex]->lastPointOnPath) + " LearnRate :"+to_string(learnRate)).c_str() );

        //adjust camera to center of best car
        mat4 projectionMat = ortho<float>(p.x - res->cameraSize, p.x + res->cameraSize, p.y - res->cameraSize, p.y + res->cameraSize);
        mat4 modelMat = translate(mat4(1.0f), vec3(0, 0, 0));
        
        //apply matrices
        res->imgShader->use();
        res->imgShader->setMat4("projection", projectionMat);
        res->imgShader->setMat4("model", modelMat);
        res->road.draw(res->imgShader);

        res->colorShader->use();
        res->colorShader->setMat4("projection", projectionMat);

        res->carShader->use();
        res->carShader->setMat4("projection", projectionMat);


        //draw cars
        for (int i = 0; i < cars.size(); i++) {
            if(i!=lastBestIndex)
            cars[i]->draw(res->carShader, res->colorShader, false);

        }
        //draw rays of best car
        cars[lastBestIndex]->draw(res->carShader, res->colorShader, res->showDebug);

        //check their collisions and draw them
        for (auto c : cars) {
            vector<CollisionData> data = CollisionSystem::checkCollision(c->collisionMesh, res->road.collissionMesh);
            if (data.size() != 0) {
                for (auto v : data) {
                    Primitives::drawCube(v.position, 0.06f, res->colorShader, vec3(1, 0, 0));
                }
                c->alive = false;
            }
            data.clear();
        }
        //grant them points by checking the collision with next segment of road
        for (auto c : cars) {
            pair<vec2, vec2> distSeg = res->road.getDistPoint(c->lastPointOnPath);
            vector<CollisionData> data2 = CollisionSystem::getSegmentMeshCollision(distSeg.first, distSeg.second, &c->collisionMesh);
            if (data2.size() > 0) {
                c->lastPointOnPath += 1;
            }
            data2.clear();
        }
        //reset world
        if (res->keyboard.getKeyInfo(GLFW_KEY_R).wasPressed) {
            int bb = Car::getBestCar(cars);
            NeuralNetwork* nn = brains[bb];
            for (int i = 0; i < carNum; i++)
            {
                cars[i]->alive = true;
                cars[i]->setPosDir(res->road.getStartPos(), res->road.getStartDir());
                if(i!=bb)
                    brains[i]->mutateFrom(nn, learnRate);
                cars[i]->lastPointOnPath = 0;
            }
            lastBest = -1;
            lastBestIndex = 0;
        }
        //clear world
        if (res->keyboard.getKeyInfo(GLFW_KEY_C).wasPressed) {
            for (int i = 0; i < carNum; i++)
            {
                reset_car(i);
            }
            lastBest = -1;
            lastBestIndex = 0;
        }
        //save best to file
        if (res->keyboard.getKeyInfo(GLFW_KEY_S).wasPressed) {
            brains[lastBestIndex]->saveToFile();
            cout << "Saving best car to Resources/Saves/car.nn" << endl;
        }
        //load brain from file
        if (res->keyboard.getKeyInfo(GLFW_KEY_L).wasPressed) {
            NeuralNetwork* nn = NeuralNetwork::loadFromFile();
            int bb = 0;
            for (int i = 0; i < carNum; i++)
            {
                cars[i]->alive = true;
                cars[i]->setPosDir(res->road.getStartPos(), res->road.getStartDir());
                if (i != bb) {
                    brains[i]->mutateFrom(nn, learnRate);
                }
                else {
                    brains[i]->mutateFrom(nn, 0);
                }
                cars[i]->lastPointOnPath = 0;
            }
            delete nn;
            lastBest = -1;
            lastBestIndex = 0;
        }
        //adjust learn rate
        if (res->keyboard.getKeyInfo(GLFW_KEY_RIGHT).isPressed) {
            learnRate += learnRateUp;
        }
        else if (res->keyboard.getKeyInfo(GLFW_KEY_LEFT).isPressed) {
            learnRate -= learnRateUp;
        }
	}
	void dispose() {
       
	}
}; 
//human driving WSAD
class DriveScene :public Scene {
 
public:
    Car* car;
    void loadScene(Resources* r) override {
        res = r;
        KeyboardControlls* controlls = new KeyboardControlls(&(res->keyboard));
        car = new Car(controlls,vec3(0,0,1));
        car->setCarTexture(res->carTexture);
        car->setPosDir(res->road.getStartPos(), res->road.getStartDir());
    }
    void update(float deltaTime) override {
       
         car->update(0.02f, &(res->road.collissionMesh));
        
        glfwSetWindowTitle(res->window, ("Score : " + to_string(car->lastPointOnPath)).c_str());
        vec2 p = car->position;
        mat4 projectionMat = ortho<float>(p.x - res->cameraSize, p.x + res->cameraSize, p.y - res->cameraSize, p.y + res->cameraSize);
        mat4 modelMat = translate(mat4(1.0f), vec3(0, 0, 0));

        //apply matrices
        res->imgShader->use();
        res->imgShader->setMat4("projection", projectionMat);
        res->imgShader->setMat4("model", modelMat);
        res->road.draw(res->imgShader);

        res->colorShader->use();
        res->colorShader->setMat4("projection", projectionMat);
        res->carShader->use();
        res->carShader->setMat4("projection", projectionMat);

        car->draw(res->carShader, res->colorShader, res->showDebug);
        vector<CollisionData> data = CollisionSystem::checkCollision(car->collisionMesh, res->road.collissionMesh);
        if (data.size() != 0) {
            for (auto v : data) {
                Primitives::drawCube(v.position, 0.06f, res->colorShader, vec3(1, 0, 0));
            }
        }
        data.clear();
        pair<vec2, vec2> distSeg = res->road.getDistPoint(car->lastPointOnPath);
        vector<CollisionData> data2 = CollisionSystem::getSegmentMeshCollision(distSeg.first, distSeg.second, &car->collisionMesh);
        if (data2.size() > 0) {
            car->lastPointOnPath += 1;
        }
        data2.clear();
    }
    void dispose() {
        delete car->controlls;
        delete car;
    }
};
//Human WSAD vs Ai
class RaceScene :public Scene {
public:
    Car* car;
    Car* aiCar;
    const vector<int> levels = { 6 };//same structure as in learning phase
    NeuralNetwork* nn;
    void loadScene(Resources* r) override {
        res = r;
        KeyboardControlls* controlls = new KeyboardControlls(&(res->keyboard));
        car = new Car(controlls,vec3(0,0,1));
        car->setCarTexture(res->carTexture);
        car->setPosDir(res->road.getStartPos(), res->road.getStartDir());

        nn = NeuralNetwork::loadFromFile();
        //nn = new NeuralNetwork(car->sensor.rayCount, 4, levels);
        AIControlls* aiControlls = new AIControlls(nn);

        aiCar = new Car(aiControlls);
        aiCar->setCarTexture(res->carTexture);
        aiCar->setPosDir(res->road.getStartPos(), res->road.getStartDir());
    }
    void checkCar(Car* car) {
        vector<CollisionData> data = CollisionSystem::checkCollision(car->collisionMesh, res->road.collissionMesh);
        if (data.size() != 0) {
            for (auto v : data) {
                Primitives::drawCube(v.position, 0.06f, res->colorShader, vec3(1, 0, 0));
            }
        }
        data.clear();
        pair<vec2, vec2> distSeg = res->road.getDistPoint(car->lastPointOnPath);
        vector<CollisionData> data2 = CollisionSystem::getSegmentMeshCollision(distSeg.first, distSeg.second, &car->collisionMesh);
        if (data2.size() > 0) {
            car->lastPointOnPath += 1;
        }
        data2.clear();
    }
    void update(float deltaTime) override {

        car->update(0.02f, &(res->road.collissionMesh));
        aiCar->update(0.02f, &(res->road.collissionMesh));

        glfwSetWindowTitle(res->window, ("Score : " + to_string(car->lastPointOnPath)).c_str());
        vec2 p = car->position;
        mat4 projectionMat = ortho<float>(p.x - res->cameraSize, p.x + res->cameraSize, p.y - res->cameraSize, p.y + res->cameraSize);
        mat4 modelMat = translate(mat4(1.0f), vec3(0, 0, 0));

        //apply matrices
        res->imgShader->use();
        res->imgShader->setMat4("projection", projectionMat);
        res->imgShader->setMat4("model", modelMat);
        res->road.draw(res->imgShader);

        res->colorShader->use();
        res->colorShader->setMat4("projection", projectionMat);
        res->carShader->use();
        res->carShader->setMat4("projection", projectionMat);

        car->draw(res->carShader, res->colorShader, res->showDebug);
        aiCar->draw(res->carShader, res->colorShader, res->showDebug);
        //colision etc.
        checkCar(aiCar);
        checkCar(car);
    }
    void dispose() {
        delete aiCar->controlls;
        delete aiCar;
        delete nn;
        delete car->controlls;
        delete car;
    }
};