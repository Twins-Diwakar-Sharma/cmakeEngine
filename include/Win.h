#ifndef KHIDKI
#define KHIDKI

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <tuple>
#include "Projection.h"

#ifndef DISPLAY_SERVER
#define DISPLAY_SERVER "win"
#endif

class Win
{
private:
    GLFWwindow* win;
    static int width, height;
    const char* title="GLFWTEST";
    void static resize(GLFWwindow* w, int width, int height);
    float senstivity = 0.05f;

public:
    Win();
    ~Win();
    bool close();
    void terminate();
    void swap();
    float getAspect();
    void handleKey(float& t, float& s, float val);
    void handleMouse(float& rx, float& ry);
    void pollEvents();

    void handleHold(bool& hold);
    void handleTerrain(bool& updateTerrain);
    void handleWireframe(bool& wireframe);

    std::tuple<int,int> getDimensions(); // return width, height
};

#endif
