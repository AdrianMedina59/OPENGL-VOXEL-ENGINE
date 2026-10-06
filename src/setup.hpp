#ifndef SETUP_HPP
#define SETUP_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* initWindow(unsigned int width, unsigned int height);
unsigned int createCubeVAO();
unsigned int loadTexture(const char* path);

#endif