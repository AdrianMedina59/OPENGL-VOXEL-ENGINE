#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "input.hpp"
#include "imgui/imgui.h"
// Define the global variables
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float deltaTime = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
bool firstMouse = true;
bool cursor_Unlocked;
static bool ePressedLastFrame = false;
static bool guiMode = false;

void processInput(GLFWwindow* window)
{
    static bool wireframe = false;
    static bool slashPressedLastFrame = false;

    ImGuiIO& io = ImGui::GetIO();

    // Prevent movement keys when ImGui wants keyboard
    if (!guiMode || !io.WantCaptureKeyboard) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    bool ePressedNow = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
    if (ePressedNow && !ePressedLastFrame)
    {
        guiMode = !guiMode;
        glfwSetInputMode(window, GLFW_CURSOR,
            guiMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
    ePressedLastFrame = ePressedNow;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        cursor_Unlocked = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool slashPressedNow = glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS;
    if (slashPressedNow && !slashPressedLastFrame)
    {
        wireframe = !wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    }
    slashPressedLastFrame = slashPressedNow;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (guiMode) return;  // ignore all mouse motion if GUI is active

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
    
}