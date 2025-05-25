#ifndef MESH_GL_HPP
#define MESH_GL_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    int boneIDs[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    GLuint id;
    std::string type;
    std::string path;
};

class Mesh
{
    std::vector<Vertex> vertices_;
    std::vector<GLuint> indices_;
    std::vector<Texture> textures_;
    GLuint VAO_, VBO_, EBO_;
    // AABB attributes
    glm::vec3 min_;
    glm::vec3 max_;
    std::string name_; // debug

public:
    Mesh(const std::vector<Vertex> &vertices,
         const std::vector<unsigned int> &indices,
         const std::vector<Texture> &textures,
         const glm::vec3 &min,
         const glm::vec3 &max,
         const std::string &name)
        : vertices_(vertices),
          indices_(indices),
          textures_(textures),
          VAO_(0),
          VBO_(0),
          EBO_(0),
          min_(min),
          max_(max),
          name_(name)
    {
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);
        glGenBuffers(1, &EBO_);
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(4);
        glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, boneIDs));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, weights));
        glEnableVertexAttribArray(6);
        glBindVertexArray(0);
    }
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&other)
        : vertices_(std::move(other.vertices_)),
          indices_(std::move(other.indices_)),
          textures_(std::move(other.textures_)),
          VAO_(other.VAO_),
          VBO_(other.VBO_),
          EBO_(other.EBO_),
          min_(other.min_),
          max_(other.max_),
          name_(std::move(other.name_))
    {
        other.VAO_ = 0;
        other.VBO_ = 0;
        other.EBO_ = 0;
        other.min_ = glm::vec3(0.0f);
        other.max_ = glm::vec3(0.0f);
    }
    Mesh &operator=(Mesh &&other)
    {
        if (this != &other)
            Mesh(std::move(other)).swap(*this);
        return *this;
    }
    ~Mesh()
    {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
        glDeleteBuffers(1, &EBO_);
        vertices_.clear();
        indices_.clear();
        textures_.clear();
    }
    void draw(GLuint ID) const
    {
        for (std::size_t i = 0; i < textures_.size(); ++i) // glsl有错误//////////////////////////////////////////
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glUniform1i(glGetUniformLocation(ID, textures_[i].type.c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures_[i].id);
        }
        glBindVertexArray(VAO_);
        glDrawElements(GL_TRIANGLES, static_cast<GLuint>(indices_.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }
    inline const std::vector<Vertex> &getVertices() const { return vertices_; }
    inline const std::vector<GLuint> &getIndices() const { return indices_; }
    inline const std::vector<Texture> &getTextures() const { return textures_; }
    inline const glm::vec3 &getMin() const { return min_; }
    inline const glm::vec3 &getMax() const { return max_; }
    inline const std::string &getName() const { return name_; }

private:
    void swap(Mesh &other)
    {
        std::swap(vertices_, other.vertices_);
        std::swap(indices_, other.indices_);
        std::swap(textures_, other.textures_);
        std::swap(VAO_, other.VAO_);
        std::swap(VBO_, other.VBO_);
        std::swap(EBO_, other.EBO_);
        std::swap(min_, other.min_);
        std::swap(max_, other.max_);
        std::swap(name_, other.name_);
    }
};

#endif
