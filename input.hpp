#ifndef INPUT_HPP
#define INPUT_HPP

#define GLFW_INCLUDE_NONE  // Prevent GLFW from including OpenGL headers
#include <GLFW/glfw3.h>
#include "camera.hpp"

// Global input state
extern Camera camera;
extern float deltaTime;
extern float lastX;
extern float lastY;
extern bool firstMouse;

// Input function declarations
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#endif