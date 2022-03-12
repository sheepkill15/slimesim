#ifndef VERTEX_FRAGMENT_SHADER_H
#define VERTEX_FRAGMENT_SHADER_H

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class VertFragShader
{
public:
    VertFragShader(const char *vertFile, const char* fragFile)
    {
        ifstream vertexShader(vertFile);
        std::string str((std::istreambuf_iterator<char>(vertexShader)),
                        std::istreambuf_iterator<char>());
        const char *vsData = str.c_str();
        unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsData, nullptr);
        glCompileShader(vs);
        {
            int success;
            char infoLog[512];
            glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vs, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                          << infoLog << std::endl;
            }
        }
        ifstream fragShader(fragFile);
        std::string fragmentShaderSource((std::istreambuf_iterator<char>(fragShader)),
                                         std::istreambuf_iterator<char>());
        const char *fsData = fragmentShaderSource.c_str();
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fsData, NULL);
        glCompileShader(fragmentShader);

        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vs);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        program = shaderProgram;
        
        glDeleteShader(vs);
        glDeleteShader(fragmentShader);
    }

    GLuint program;
};

#endif