#include <GL/glew.h> // Include this before you include any other gl stuff
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Best Documentation: http://docs.gl

int main(void)
{
    ////////////////// Init OpenGL //////////////////
    /////////////////////////////////////////////////

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //// Changes OpenGL to 3.3 Core, which requires you to create a vao
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "OpenGL Practice", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);
    
    // Init glew, after the context
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW: glewInit() did not work!" << std::endl;
   
    // Output OpenGL info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;


    ///////////////// Graphics Data /////////////////
    /////////////////////////////////////////////////

    // Buffer data
    {
        float vertexBufferData[] =
        {
           -400.0f,-225.0f, 0.0f, 0.0f,
            400.0f,-225.0f, 1.0f, 0.0f,
            400.0f, 225.0f, 1.0f, 1.0f,
           -400.0f, 225.0f, 0.0f, 1.0f
        };

        unsigned int indexBufferData[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Each vertex is 2 floats
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        // Blending for transperency
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        //// Vertex Array and Buffer Objects ////
        VertexBuffer vbo(vertexBufferData, 4 * 4 * sizeof(float));
        VertexArray vao;

        vao.AddBuffer(vbo, layout);


        //// Index Buffer ////
        IndexBuffer ibo(indexBufferData, 6);

        // MVP
        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // "Camera"
        

        //// Shader ////
        Shader shader("res/shaders/BasicShader.Shader");
        shader.Bind();
        shader.SetUniform1i("u_Texture", 0);

        // Texture
        Texture texture("res/textures/hk.png");
        texture.Bind();


        // Undbind everything
        vbo.Unbind();
        vao.Unbind();
        ibo.Unbind();
        shader.Unbind();

        Renderer renderer;
        
        //// ImGui ////

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

        ////////////////// Main Loop ///////////////////
        
        // Variables
        glm::vec4 tintColor(0);
        glm::vec3 translation(0);
        glm::vec3 translation2(100, 100 ,0);

        while (!glfwWindowShouldClose(window))
        {
            // Clear
            renderer.Clear();

            // ImGui New Frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();



            //////// Draw Stuff Here ////////////////////////

            {
                ImGui::Begin("Debug");

                ImGui::SliderFloat3("Pic 1", &translation.x, 0.0f, 960.0f);
                ImGui::SliderFloat3("Pic 2", &translation2.x, 0.0f, 960.0f);
                ImGui::ColorEdit3("Tint", (float*)&tintColor);

                ImGui::End();
            }

            // Hollow Knight
            shader.Bind();
            shader.SetUniform4f("u_Color", tintColor);

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(vao, ibo, shader);
            }

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translation2);
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(vao, ibo, shader);
            }


            /////////////////////////////////////////////////

            // ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Swap Buffers
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();

        }
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}