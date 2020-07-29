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
            200.0f, 200.0f, 0.0f, 0.0f,
            500.0f, 200.0f, 1.0f, 0.0f,
            500.0f, 500.0f, 1.0f, 1.0f,
            200.0f, 500.0f, 0.0f, 1.0f
        };

        unsigned int indexBufferData[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Blending for transperency
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        //// Vertex Array and Buffer Objects ////
        VertexBuffer vbo(vertexBufferData, 4 * 4 * sizeof(float));
        VertexArray vao;

        // Each vertex is 2 floats
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        // Add buffer and layout to vbo
        vao.AddBuffer(vbo, layout);

        //// Index Buffer ////
        IndexBuffer ibo(indexBufferData, 6);

        // Projection Matrix
        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-400, 0, 0)); // "Camera"
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
        glm::mat4 mvp = proj * view * model; // Backwards operations because column major memory layout of OpenGL

        //// Shader ////
        Shader shader("res/shaders/BasicShader.Shader");
        shader.Bind();
        shader.SetUniform1i("u_Texture", 0);
        shader.SetUniformMat4f("u_MVP", mvp);

        // Texture
        Texture texture("res/textures/hk.png");
        texture.Bind();


        // Undbind everything
        vbo.Unbind();
        vao.Unbind();
        ibo.Unbind();
        shader.Unbind();

        Renderer renderer;

        ////////////////// Main Loop ///////////////////

        // Color Animator
        float r = 0.0f;
        float increment = 0.05f;

        while (!glfwWindowShouldClose(window))
        {
            // Clear
            renderer.Clear();

            //////// Draw Stuff Here ////////////////////////

            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            renderer.Draw(vao, ibo, shader);

            // Animate Color
            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            /////////////////////////////////////////////////

            // Swap Buffers
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();

        }
    }

    glfwTerminate();
    return 0;
}