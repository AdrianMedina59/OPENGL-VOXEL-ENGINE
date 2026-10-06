#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ctime>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "camera.hpp"
#include "Block.hpp"
#include "Chunk.hpp"
#include "setup.hpp"
#include "input.hpp"
#include "World.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include "stb_image.h"
#include <unordered_map>


// Window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Global time state
float lastFrame = 0.0f;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

struct BlockHit {
    glm::ivec3 position; // block position
    int face;            // face index hit (0-5)
    bool hit;
};


int main() {

    stbi_set_flip_vertically_on_load(false);

    GLFWwindow* window = initWindow(SCR_WIDTH, SCR_HEIGHT);
    if (!window) return -1;
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader shader("shaders/vertex_shader.txt", "shaders/fragment_shader.txt");
    Shader shaderLight("shaders/Light_vertex_shader.txt", "shaders/Light_fragment_shader.txt");

    unsigned int VAO = createCubeVAO();
    unsigned int LightVAO = createCubeVAO();
    unsigned int textureAtlas = loadTexture("textures/blocks.png");


    glActiveTexture(GL_TEXTURE0); // activate texture unit 0
    int width, height;
    glBindTexture(GL_TEXTURE_2D, textureAtlas);// bind the texture atlas
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    std::cout << "Texture size: " << width << "x" << height << std::endl;

    
    int seed = static_cast<int>(time(0));
    World world(seed);
    
    

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    

    glm::vec3 lightPos(0.0f, 2.0f, 2.0f);

    glm::vec3 lightColor(1.0f);
  
 

    float fpsTimer = 0.0f;
    int fpsFrames = 0;
    // Main render loop
    stbi_set_flip_vertically_on_load(true);

    camera.Position = glm::vec3(8.0f, 33.0f, 20.0f);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Optional: Dark theme
    ImGui::StyleColorsDark();

    // Initialize ImGui for GLFW and OpenGL3
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    float fogStart = 0.0f;
    float fogEnd = 20.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


       

        lightPos.x = 32.0f;
        lightPos.y = 32.0f;

        processInput(window);

        glClearColor(0.529f, 0.808f, 0.980f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureAtlas);

       

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        shader.use();
       
        shader.setInt("atlas", 0); // set the uniform to texture unit 0
        

       
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", lightPos);
        shader.setMat4("projection", glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f));
        shader.setMat4("view", camera.GetViewMatrix());
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("material.ambient", 1.0f, 0.5f, 0.5f);
        shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);
        shader.setVec3("light.ambient",ambientColor);
        shader.setVec3("light.diffuse", ambientColor); 
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("fogStart",fogStart);
        shader.setFloat("fogEnd", fogEnd);

       
      

      

        // Create rotation model matrix around lightPos
        float angle = currentFrame;
        float radius = 2.0f;
       
        world.Render(shader,camera.Position);
        world.Update(camera.Position);

     
      


        Block LightBlock(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.02f), LIGHT);
        shaderLight.use();
        shaderLight.setMat4("projection", glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f));
        shaderLight.setMat4("view", camera.GetViewMatrix());
        

        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, lightPos);
        model2 = glm::scale(model2, glm::vec3(0.2f));
        LightBlock.SetModelMatrix(model2);
        LightBlock.Draw(shaderLight,LightVAO);
           
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // [Your ImGui editing UI code here]
        ImGui::SetNextWindowSize(ImVec2(200, 200));
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::Begin("World Settings");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Position: %.1f, %.1f, %.1f", camera.Position.x, camera.Position.y, camera.Position.z);
        // e.g. slider to tweak render distance
        ImGui::SliderInt("Render Distance", &world.renderDistance, 1, 32);
        ImGui::SliderFloat("Fog Start", &fogStart, 0.0f, 100.0f);
        ImGui::SliderFloat("Fog End", &fogEnd, 0.0f, 100.0f);

        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
  
  
        
        glDeleteVertexArrays(1, &VAO);
        glfwTerminate();
        return 0;
    
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
