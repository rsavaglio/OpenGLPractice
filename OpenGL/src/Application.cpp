#include <GL/glew.h> // Include this before you include any other gl stuff
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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
    window = glfwCreateWindow(640, 480, "OpenGL Practice", NULL, NULL);
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
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f, 0.5f,
        };

        unsigned int indexBufferData[] = {
            0, 1, 2,
            2, 3, 0
        };

        //// Vertex Array and Buffer Objects ////
        VertexBuffer vbo(vertexBufferData, 4 * 2 * sizeof(float));
        VertexArray vao;

        // Each vertex is 2 floats
        VertexBufferLayout layout;
        layout.Push<float>(2);

        // Add buffer and layout to vbo
        vao.AddBuffer(vbo, layout);

        //// Index Buffer ////
        IndexBuffer ibo(indexBufferData, 6);

        //// Shader ////
        Shader shader("res/shaders/BasicShader.Shader");


        // Undbind everything
        vbo.Unbind();
        vao.Unbind();
        ibo.Unbind();
        shader.Unbind();


        ////////////////// Main Loop ///////////////////

        // Color Animator
        float r = 0.0f;
        float increment = 0.05f;

        while (!glfwWindowShouldClose(window))
        {
            // Clear
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            //////// Draw Stuff Here ////////////////////////

            // Bind Shader
            shader.Bind();
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

            // Bind VAO
            vao.Bind();
            ibo.Bind();

            // Draw
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

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