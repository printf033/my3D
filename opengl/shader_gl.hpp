#ifndef SHADER_GL_HPP
#define SHADER_GL_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <glad/glad.h>

class Shader
{
    GLuint ID_ = 0;

public:
    Shader(const std::filesystem::path &vertexPath,
           const std::filesystem::path &fragmentPath,
           const std::filesystem::path &geometryPath)
    {
        GLuint vertex = compileShader(vertexPath, GL_VERTEX_SHADER);
        GLuint fragment = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
        GLuint geometry = compileShader(geometryPath, GL_GEOMETRY_SHADER);
        ID_ = glCreateProgram();
        if (vertex != 0)
            glAttachShader(ID_, vertex);
        if (fragment != 0)
            glAttachShader(ID_, fragment);
        if (geometry != 0)
            glAttachShader(ID_, geometry);
        glLinkProgram(ID_);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(geometry);
        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(ID_, GL_LINK_STATUS, &success);
        if (0 == success)
        {
            glGetProgramInfoLog(ID_, 1024, NULL, infoLog);
            glDeleteProgram(ID_);
            throw std::runtime_error("glLinkProgram failed");
        }
    }
    ~Shader()
    {
        glDeleteProgram(ID_);
    }
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;
    Shader(Shader &&other) : ID_(other.ID_)
    {
        other.ID_ = 0;
    }
    Shader &operator=(Shader &&other)
    {
        ID_ = other.ID_;
        other.ID_ = 0;
        return *this;
    }
    inline GLuint getID() const { return ID_; }
    inline void use() const { glUseProgram(ID_); }
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID_, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID_, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID_, name.c_str()), value);
    }
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID_, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID_, name.c_str()), x, y);
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID_, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID_, name.c_str()), x, y, z);
    }
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID_, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID_, name.c_str()), x, y, z, w);
    }
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    GLuint compileShader(const std::filesystem::path &path, GLenum shaderType)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << path << std::endl;
            return 0;
        }
        auto size = std::filesystem::file_size(path);
        std::string code;
        code.resize(size, '\0');
        file.read(code.data(), size);
        GLuint shader = glCreateShader(shaderType);
        const char *shaderCode = code.c_str();
        glShaderSource(shader, 1, &shaderCode, NULL);
        glCompileShader(shader);
        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (0 == success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Failed to compile shader: " << infoLog << std::endl;
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }
};

#endif
