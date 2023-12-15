#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <thread>
#include <chrono>
#include<vector>
#include<glm.hpp>
#include <gtc/matrix_transform.hpp>
#include"Shader.h"
#include"Road.h"
#include"Controlls.h"
#include"Car.h"
#include"CollisionSystem.h"
#include"Primitives.h"
#include"NeuralNetwork.h"
#include"Scene.h"
#include <iostream>
using namespace glm;
using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)//adjust screen to window if resized
{
    glViewport(0, 0, width, height);
}
GLFWwindow* createWindow() {//create window and chceck for errors
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Score : 0", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    glViewport(0, 0, 800, 800);
    return window;
}

Scene* scenes[3];
int currentSceneIndex = 1;
Scene* currentScene;
//helper function to load scenes
void loadScene(Resources* r) {
    cout << "Loading scene " << currentSceneIndex << endl;
    currentScene->dispose();//tidy last scene
    currentScene = scenes[currentSceneIndex - 1];
    currentScene->loadScene(r);
}
int main()
{
    auto window = createWindow();
    if (window == nullptr) { return -1; }
    //Setup resource which can be used by every scene, for now i asume each scene usues the same road
    Resources res;
    res.window = window;
    //load shaders, see them in Resources/Shaders
    res.imgShader =new Shader("Resources/Shaders/main.vs", "Resources/Shaders/main.fs");
    res.colorShader =new  Shader("Resources/Shaders/main.vs", "Resources/Shaders/notex.fs");
    res.carShader = new  Shader("Resources/Shaders/main.vs", "Resources/Shaders/car.fs");


    //Shape of a car is a rectangle
    vector<Vertex> vertices = { Vertex(vec3(0,0,0),vec2(0,0)),
        Vertex(vec3(0.5,0,0),vec2(1,0)),Vertex(vec3(.5,.5,0),vec2(1,1)),Vertex(vec3(0,.5,0),vec2(0,1)) };
    vector<unsigned int> triangles = { 0,1,2,0,2,3 };
    res.roadTexture = Texture::LoadTexture("Resources/Textures/road.png");
    res.carTexture= Texture::LoadTexture("Resources/Textures/car.png");

    //prepare road object
    res.road=Road(1.5f);
    res.road.createPath("Resources/Paths/bezier.path");
    res.road.setRoadTexture(res.roadTexture);
    res.road.createMesh();

    //some common keys
    res.keyboard.enableKey(GLFW_KEY_W);
    res.keyboard.enableKey(GLFW_KEY_S);//save cars brain(Scene 2)
    res.keyboard.enableKey(GLFW_KEY_D);
    res.keyboard.enableKey(GLFW_KEY_A);
    res.keyboard.enableKey(GLFW_KEY_R);//reset world and give new generation best brain(scene 2)
    res.keyboard.enableKey(GLFW_KEY_C);//reset world and give new generation random brain(scene 2)
    res.keyboard.enableKey(GLFW_KEY_1);//to scene 1
    res.keyboard.enableKey(GLFW_KEY_2);//to scene 2
    res.keyboard.enableKey(GLFW_KEY_3);//to scene 3
    res.keyboard.enableKey(GLFW_KEY_H);//Toogle draw debug
    res.keyboard.enableKey(GLFW_KEY_L);//Load last brain (scene 2)
    res.keyboard.enableKey(GLFW_KEY_LEFT);//For adjusting the learn rate
    res.keyboard.enableKey(GLFW_KEY_RIGHT);//For adjusting the learn rate

    res.cameraSize = 3.0f;
   
    //prepare scenes
    scenes[1] = new AIScene();
    scenes[0] = new DriveScene();
    scenes[2] = new RaceScene();
    currentScene=scenes[currentSceneIndex-1];
    currentScene->loadScene(&res);
    float deltaTime = 0.02;//fixed delta
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glClearColor(0.1, 0.9, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        //change scene
        if (res.keyboard.getKeyInfo(GLFW_KEY_1).wasPressed && currentSceneIndex != 1) {
            currentSceneIndex = 1;
            loadScene(&res);
        }else if (res.keyboard.getKeyInfo(GLFW_KEY_2).wasPressed && currentSceneIndex != 2) {
            currentSceneIndex = 2;
            loadScene(&res);
        }else if (res.keyboard.getKeyInfo(GLFW_KEY_3).wasPressed && currentSceneIndex != 3) {
            currentSceneIndex = 3;
            loadScene(&res);
        }
        if (res.keyboard.getKeyInfo(GLFW_KEY_H).wasPressed) {
            res.showDebug = !res.showDebug;//show debug for example rays coming out of a car
        }
        currentScene->update(deltaTime);

        //loop should bee 25 ms then, so roughly 50 fps
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        auto e = glGetError();
        if (e != GL_NO_ERROR) {
            cout << e << endl;
        }
        glfwPollEvents();//check for keyboard events
        res.keyboard.update(window);
       
    }
    //should do some clean up with gl staff but for now there is no use to it
    glfwTerminate();
}