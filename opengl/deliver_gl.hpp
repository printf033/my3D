#ifndef DELIVER_GL_HPP
#define DELIVER_GL_HPP

#include <glad/glad.h>

class Deliver
{
    GLuint SSBO_ = 0;
    GLuint bindingIndex_ = 0;
    GLsizeiptr size_ = 0;
    const void *transforms_ = nullptr;

public:
    Deliver(const std::vector<glm::mat4> &transforms, GLuint bindingIndex)
        : size_(transforms.size() * sizeof(glm::mat4)),
          transforms_(transforms.data()),
          bindingIndex_(bindingIndex)
    {
        glGenBuffers(1, &SSBO_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     size_,
                     transforms_,
                     GL_DYNAMIC_DRAW);
    }
    ~Deliver()
    {
        glDeleteBuffers(1, &SSBO_);
    }
    Deliver(const Deliver &) = delete;
    Deliver &operator=(const Deliver &) = delete;
    Deliver(Deliver &&other)
        : SSBO_(other.SSBO_),
          bindingIndex_(other.bindingIndex_),
          size_(other.size_),
          transforms_(other.transforms_)
    {
        other.SSBO_ = 0;
        other.bindingIndex_ = 0;
        other.size_ = 0;
        other.transforms_ = nullptr;
    }
    Deliver &operator=(Deliver &&other)
    {
        SSBO_ = other.SSBO_;
        bindingIndex_ = other.bindingIndex_;
        size_ = other.size_;
        transforms_ = other.transforms_;
        other.SSBO_ = 0;
        other.bindingIndex_ = 0;
        other.size_ = 0;
        other.transforms_ = nullptr;
        return *this;
    }
    void deliverTransforms(GLuint ID) const
    {
        GLuint blockIndex = glGetProgramResourceIndex(ID, GL_SHADER_STORAGE_BLOCK, "BoneTrans");
        if (blockIndex == GL_INVALID_INDEX)
            std::cerr << "Shader storage block BoneMatrices not found!\n";
        else
            glShaderStorageBlockBinding(ID, blockIndex, bindingIndex_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex_, SSBO_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size_, transforms_);
    }
};

#endif
