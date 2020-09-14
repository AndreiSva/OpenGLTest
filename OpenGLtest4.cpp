#ifdef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#define GLEW_STATIC

#include <streambuf>
#include <fstream>
#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static std::string GetShaderFile(const char* path) {
    std::ifstream filereader(path);
    std::string shader((std::istreambuf_iterator<char>(filereader)), std::istreambuf_iterator<char>());
    filereader.close();
    return shader;
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char* errMessage = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, errMessage);

        std::cout << "OpenGL shader error" << std::endl;
        std::cout << errMessage << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertShader, const std::string& fragShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "OpenGL error" << std::endl;
    }

    float pos[] = {
        -0.5f, -0.5f,
         0.5,  -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), pos, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    unsigned int indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    unsigned int shader = CreateShader(GetShaderFile("res/shaders/triangle/triangle.vert"), GetShaderFile("res/shaders/triangle/triangle.frag"));
    glUseProgram(shader);

    int location = glGetUniformLocation(shader, "u_Color");
    int colorIndex = 0;
    float color[] = {
         1.0f, 0.0f, 0.0f
    };
    glUniform4f(location, color[0], color[1], color[2], 0.0f);
    float value = (float)1 / 255;
    bool substract = false;


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform4f(location, color[0], color[1], color[2], 0.0f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        if (colorIndex >= 1 && color[1] <= 0) {

            if (color[2] <= 0) {
                colorIndex = 0;

                glfwSwapBuffers(window);

                glfwPollEvents();

                continue;
            }

            color[2] -= value;
            color[0] += value;

            glfwSwapBuffers(window);

            glfwPollEvents();

            continue;
        }

        if (!substract) {
            color[colorIndex + 1] += value;
        }
        else {
            if (color[2] >= 1) {

                if (color[1] <= 0) {
                    colorIndex++;

                    glfwSwapBuffers(window);

                    glfwPollEvents();

                    continue;
                }

                color[1] -= value;

                if (color[1] <= 0) {
                    substract = false;
                }
            }
            else {
                color[colorIndex - 1] -= value;


                if (color[colorIndex - 1] <= 0) {
                    substract = false;
                }
            }
        }

        if (colorIndex == 1 && color[2] >= 1) {
            substract = true;
        }
        else if (color[colorIndex + 1] >= color[colorIndex] && !substract) {
            substract = true;
            colorIndex++;
        }

        /*if (colorIndex == 1 && color[0] >= 1 && color[1] <= 0) {
            colorIndex = 0;
        }*/

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}