#ifndef ANIMATOR_GL_HPP
#define ANIMATOR_GL_HPP

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "model.hpp"
#include "animation.hpp"

class Animator : public Model
{
    std::unordered_map<std::string, Animation> animations_;
    std::vector<glm::mat4> transforms_;
    Animation *curAnim_ = nullptr;
    double curTick_ = 0.0;

public:
    Animator(Model &&model)
        : Model(std::move(model))
    {
        readAnimations(getPath());
    }
    Animator(Model &&model,
             const std::string &animName,
             GLuint nextBindingPoint)
        : Model(std::move(model))
    {
        readAnimations(getPath());
        setCurAnimation(animName);
    }
    ~Animator()
    {
        curAnim_ = nullptr;
        animations_.clear();
        transforms_.clear();
    }
    void swap(Animator &other)
    {
        Model::swap(other);
        std::swap(animations_, other.animations_);
        std::swap(curAnim_, other.curAnim_);
        std::swap(curTick_, other.curTick_);
        std::swap(transforms_, other.transforms_);
    }
    Animator(const Animator &) = delete;
    Animator &operator=(const Animator &) = delete;
    Animator(Animator &&other)
        : Model(std::move(other)),
          animations_(std::move(other.animations_)),
          curAnim_(other.curAnim_),
          curTick_(other.curTick_),
          transforms_(std::move(other.transforms_))
    {
        other.curAnim_ = nullptr;
        other.curTick_ = 0.0;
    }
    Animator &operator=(Animator &&other)
    {
        if (this != &other)
            Animator(std::move(other)).swap(*this);
        return *this;
    }
    void setCurAnimation(const std::string &animName)
    {
        if (!animations_.contains(animName))
            std::cerr << "Animation not found: " << animName << std::endl;
        curAnim_ = &animations_.at(animName);
        std::clog << "Set animation: " << animName
                  << ", duration: " << curAnim_->getDuration()
                  << ", ticks/s: " << curAnim_->getTicksPerSecond() << std::endl;
    }
    void updateTransforms(double deltaTime)
    {
        assert(curAnim_ != nullptr);
        if (curTick_ < curAnim_->getDuration())
        {
            calculateTransform(getRootHierarchy(), glm::mat4(1.0f));
            curTick_ += deltaTime * curAnim_->getTicksPerSecond();
        }
    }
    inline const std::vector<glm::mat4> &myTransforms() const
    {
        return transforms_;
    }

private:
    void readAnimations(const std::filesystem::path &path)
    {
        Assimp::Importer importer;
        auto paiScene = importer.ReadFile(path,
                                          aiProcess_CalcTangentSpace |
                                              aiProcess_Triangulate |
                                              aiProcess_GenSmoothNormals |
                                              aiProcess_LimitBoneWeights |
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_ConvertToLeftHanded);
        assert(paiScene != nullptr &&
               !(paiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) &&
               paiScene->mRootNode != nullptr);
        animations_.reserve(paiScene->mNumAnimations);
        for (unsigned int i = 0; i < paiScene->mNumAnimations; ++i)
        {
            auto paiAnimation = paiScene->mAnimations[i];
            animations_.emplace(paiAnimation->mName.C_Str(), Animation(paiAnimation));
            ///////////////////////////////////////////////////////////////
            curAnim_ = &animations_.at(paiAnimation->mName.C_Str());
            std::clog << "Set animation: " << paiAnimation->mName.C_Str() // 报菜名
                      << ", duration: " << curAnim_->getDuration()
                      << ", ticks/s: " << curAnim_->getTicksPerSecond() << std::endl;
            ///////////////////////////////////////////////////////////////
        }
        transforms_.resize(getBonesLoaded().size(), glm::mat4(1.0f));
    }
    void calculateTransform(const Hierarchy *node, glm::mat4 parentTransform)
    {
        assert(node != nullptr);
        if (curAnim_ != nullptr && curAnim_->getKeyFrames().contains(node->name))
            parentTransform *= curAnim_->getKeyFrames().at(node->name).interpolate(curTick_);
        transforms_[node->id] = parentTransform * node->offset;
        for (const auto &child : node->children)
            calculateTransform(child, parentTransform);
    }
};
#endif