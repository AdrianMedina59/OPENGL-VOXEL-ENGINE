#include "setup.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Setup
GLFWwindow* initWindow(unsigned int width, unsigned int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    glViewport(0, 0, width, height);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    return window;
}


unsigned int createCubeVAO() {
    float cubeVertices[] = {
        // BACK face (index 0)
    //positions                //normals          //texture coords
  0.5f, -0.5f, -0.5f,        0.0f,0.0f,-1.0f,      1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,       0.0f,0.0f,-1.0f,      0.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,       0.0f,0.0f,-1.0f,      0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,       0.0f,0.0f,-1.0f,      0.0f, 0.0f,
  0.5f,  0.5f, -0.5f,        0.0f,0.0f,-1.0f,      1.0f, 0.0f,
  0.5f, -0.5f, -0.5f,        0.0f,0.0f,-1.0f,      1.0f, 1.0f,

  // FRONT face (index 1)
  -0.5f, -0.5f,  0.5f,      0.0f,0.0f,1.0f,        0.0f, 1.0f,
  0.5f, -0.5f,  0.5f,       0.0f,0.0f,1.0f,        1.0f, 1.0f,
  0.5f,  0.5f,  0.5f,       0.0f,0.0f,1.0f,        1.0f, 0.0f,
  0.5f,  0.5f,  0.5f,       0.0f,0.0f,1.0f,        1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,      0.0f,0.0f,1.0f,        0.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,      0.0f,0.0f,1.0f,        0.0f, 1.0f,

  // LEFT face (index 2)
  -0.5f, -0.5f, -0.5f,      -1.0f,0.0f,0.0f,       0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,      -1.0f,0.0f,0.0f,       1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,      -1.0f,0.0f,0.0f,       1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,      -1.0f,0.0f,0.0f,       1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,      -1.0f,0.0f,0.0f,       0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,      -1.0f,0.0f,0.0f,       0.0f, 1.0f,

  // RIGHT face (index 3)
  0.5f, -0.5f,  0.5f,        1.0f,0.0f,0.0f,       0.0f, 1.0f,
  0.5f, -0.5f, -0.5f,        1.0f,0.0f,0.0f,       1.0f, 1.0f,
  0.5f,  0.5f, -0.5f,        1.0f,0.0f,0.0f,       1.0f, 0.0f,
  0.5f,  0.5f, -0.5f,        1.0f,0.0f,0.0f,       1.0f, 0.0f,
  0.5f,  0.5f,  0.5f,        1.0f,0.0f,0.0f,       0.0f, 0.0f,
  0.5f, -0.5f,  0.5f,        1.0f,0.0f,0.0f,       0.0f, 1.0f,

  // BOTTOM face (index 4)
  -0.5f, -0.5f, -0.5f,     0.0f,-1.0f,0.0f,        0.0f, 1.0f,
  0.5f, -0.5f, -0.5f,      0.0f,-1.0f,0.0f,        1.0f, 1.0f,
  0.5f, -0.5f,  0.5f,      0.0f,-1.0f,0.0f,        1.0f, 0.0f,
  0.5f, -0.5f,  0.5f,      0.0f,-1.0f,0.0f,        1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,     0.0f,-1.0f,0.0f,        0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,     0.0f,-1.0f,0.0f,        0.0f, 1.0f,

  // TOP face (index 5)
  -0.5f,  0.5f,  0.5f,      0.0f,1.0f,0.0f,       0.0f, 0.0f,
  0.5f,  0.5f,  0.5f,       0.0f,1.0f,0.0f,       1.0f, 0.0f,
  0.5f,  0.5f, -0.5f,       0.0f,1.0f,0.0f,       1.0f, 1.0f,
  0.5f,  0.5f, -0.5f,       0.0f,1.0f,0.0f,       1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,      0.0f,1.0f,0.0f,       0.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,      0.0f,1.0f,0.0f,       0.0f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    //postion attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //normal attributes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture attributes
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

   

    return VAO;
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture at path: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}
