#include <GL/glew.h> // Include this before you include any other gl stuff
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSoucre
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSoucre ParseShader(const std::string& filepath)
{
    // Open file
    std::ifstream stream(filepath);

    // Define Type
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    ShaderType type = ShaderType::NONE;

    // Line and string stream variables for parsing
    std::string line;
    std::stringstream ss[2];

    // Parse file, if the line contains "#shader type" change the type
    // Otherwise add the line to the correct string stream
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            // Using the enum type as an index instead of branching
            ss[(int)type] << line << '\n';
        }
    }

    // Return the struct using the string stream array
    return { ss[0].str(), ss[1].str() };

}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
            << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
   
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // We can delete the intermediates now that 'program' contains the shaders
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

/*

Best Documentation: http://docs.gl

*/

int main(void)
{
    ////////////////// Init OpenGL //////////////////
    /////////////////////////////////////////////////

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "OpenGL Practice", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    // Init glew, after the context
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW: glewInit() did not work!" << std::endl;
   
    // Output OpenGL info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    

    ////////////////// Buffer Data //////////////////
    /////////////////////////////////////////////////

    float triangleVertices[6] = 
    {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };
    
    // Generate
    unsigned int bufferID;
    glGenBuffers(1, &bufferID);

    // Bind
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);

    // Set Data 
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), triangleVertices, GL_STATIC_DRAW);
    
    // Set Layout
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0); // Index of attribute, numb of vars in the attribute, type, normalize, size of vertex, (const void)* to attribute in vertex
    glEnableVertexAttribArray(0);

    // Shaders
    ShaderProgramSoucre shaderSource = ParseShader("res/shaders/BasicShader.Shader");
    unsigned int shader = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
    glUseProgram(shader);

    // Undbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ////////////////// Main Loop ////////////////////
    /////////////////////////////////////////////////
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Clear
        glClear(GL_COLOR_BUFFER_BIT);

        //////// Draw Stuff Here /////////////////////////////////////////

        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        //////////////////////////////////////////////////////////////////

        // Swap Buffers
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}