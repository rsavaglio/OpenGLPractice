#include <GL/glew.h> // Include this before you include any other gl stuff
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Best Documentation: http://docs.gl

//// Error Checking ////

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) // while error is not 0
    {
        std::cout << "[OpenGL Error] (" << error << "): " <<
            function << " " <<
            file << ":" <<
            line << std::endl;
        return false;
    }
    return true;
}

//// Shader Functions ////

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
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();

    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // Error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
            << " shader!" << std::endl;
        std::cout << message << std::endl;

        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
   
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    // We can delete the intermediates now that 'program' contains the shaders
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}


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

    //// Vertex Array Objects ////
 
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    //// Vertex Buffer ////

    // Generate, bind, set data, set layout
    unsigned int vertexBufferID;
    GLCall(glGenBuffers(1, &vertexBufferID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), vertexBufferData, GL_STATIC_DRAW));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0)); // Links the currently bound vao and vertex buffer
    GLCall(glEnableVertexAttribArray(0));

    //// Index Buffer ////

    // Gen, bind, set data
    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indexBufferData, GL_STATIC_DRAW));

    //// Shaders ////
    ShaderProgramSoucre shaderSource = ParseShader("res/shaders/BasicShader.Shader");
    unsigned int shader = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
    GLCall(glUseProgram(shader));

    //// Uniforms ////
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);

    // Undbind everything
    GLCall(glUseProgram(0));
    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    

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
        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

        // Bind VAO
        GLCall(glBindVertexArray(vao));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        // Draw
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

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

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}