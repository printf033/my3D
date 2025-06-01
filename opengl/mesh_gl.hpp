#ifndef MESH_GL_HPP
#define MESH_GL_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "octree.hpp"

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
    // base data
    std::vector<Vertex> vertices_;
    std::vector<GLuint> indices_;
    std::vector<Texture> textures_;
    // AABB attributes
    Octree octree_;
    // shade attributes
    GLuint VAO_;
    GLuint VBO_;
    GLuint EBO_;

    // debug
    std::string name_; ////////////////////////////////////////////////////////del

public:
    Mesh(std::vector<Vertex> &&vertices,
         std::vector<unsigned int> &&indices,
         std::vector<Texture> &&textures,
         Octree &&octree,
         const std::string &name)
        : vertices_(std::move(vertices)),
          indices_(std::move(indices)),
          textures_(std::move(textures)),
          octree_(std::move(octree)),
          VAO_(0),
          VBO_(0),
          EBO_(0),
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
          octree_(std::move(other.octree_)),
          VAO_(other.VAO_),
          VBO_(other.VBO_),
          EBO_(other.EBO_),
          name_(std::move(other.name_))
    {
        other.VAO_ = 0;
        other.VBO_ = 0;
        other.EBO_ = 0;
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
    inline const Octree &getOctree() const { return octree_; }
    inline const std::string &getName() const { return name_; }

private:
    void swap(Mesh &other)
    {
        std::swap(vertices_, other.vertices_);
        std::swap(indices_, other.indices_);
        std::swap(textures_, other.textures_);
        std::swap(octree_, other.octree_);
        std::swap(VAO_, other.VAO_);
        std::swap(VBO_, other.VBO_);
        std::swap(EBO_, other.EBO_);
        std::swap(name_, other.name_);
    }
};

#endif
