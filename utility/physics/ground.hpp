#ifndef GROUND_HPP
#define GROUND_HPP

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include "soul.hpp"

#define DECAY_RATE 0.9f // 为了简化，所有mesh的动量衰减系数都一样

class Ground : public Animator
{
    std::unordered_map<std::string, Soul> souls_;
    float precision_ = 0.1f;

public:
    Ground(Animator &&entity) : Animator(std::move(entity)) {}
    ~Ground()
    {
        souls_.clear();
    }
    Ground(const Ground &) = delete;
    Ground &operator=(const Ground &) = delete;
    Ground(Ground &&) = delete;
    Ground &operator=(Ground &&) = delete;
    inline void addSoul(const std::string &name, Soul &&soul) { souls_.emplace(name, std::move(soul)); }
    inline Soul &getSoul(const std::string &name) { return souls_.at(name); }
    void update(float deltaTime) // (s)
    {
        // 所有检测对象都有的力
        auto gravity = Ground::gravityAcc();
        for (auto &it : souls_)
        {
            // 简化，质量都为单位1
            auto &soul = it.second;
            // 存在就会有的力
            soul.myOuterAcceleration() = gravity;
            // soul.myOuterAcceleration() += buoyancy(rho, V, glm::length(gravity), gravity) / mass;
            // 运动就会有的力
            // soul.myOuterAcceleration() += resistance(C, soul.myVelocity()) / mass;
            // 碰撞就会有的力
            glm::vec3 preDeltaPosition = (soul.myInnerAcceleration() + soul.myOuterAcceleration()) * deltaTime * deltaTime;
            AABB deltaAABB = soul.getOctree().getGlobalAABB(soul.myPosition());
            if (preDeltaPosition.x < 0.0f)
                deltaAABB.min.x += preDeltaPosition.x;
            else
                deltaAABB.max.x += preDeltaPosition.x;
            if (preDeltaPosition.y < 0.0f)
                deltaAABB.min.y += preDeltaPosition.y;
            else
                deltaAABB.max.y += preDeltaPosition.y;
            if (preDeltaPosition.z < 0.0f)
                deltaAABB.min.z += preDeltaPosition.z;
            else
                deltaAABB.max.z += preDeltaPosition.z;
            for (auto &aabb : getOctree().query(deltaAABB))
                collidingOffset(*((Mesh *)aabb.where), soul);
            soul.myVelocity() += (soul.myInnerAcceleration() + soul.myOuterAcceleration()) * deltaTime;
            soul.myPosition() += soul.myVelocity() * deltaTime;
            soul.processDecay();
            soul.print(); ////////////////////////////////////////////////////////////////////////
        }
    }

private:
    // 返回所受地球重力加速度
    inline glm::vec3
    gravityAcc() const
    {
        return glm::vec3(.0f, -9.8f, .0f);
    }
    // 返回所受浮力
    glm::vec3 buoyancy(float rho, float V, const glm::vec3 &g)
    {
        float gMag = glm::length(g);
        if (gMag == 0.0f || V == 0.0f || rho == 0.0f)
            return glm::vec3(0.0f);
        float buoyantMag = rho * V * gMag; // rho * V * g
        return -glm::normalize(g) * buoyantMag;
    }
    // 返回所受阻力
    glm::vec3 resistance(float C, const glm::vec3 &v)
    {
        float speed = glm::length(v);
        if (speed == 0.0f || C == 0.0f)
            return glm::vec3(0.0f);
        float resistanceMag = C * speed; // 简化
        if (speed > 3.0f)
            resistanceMag *= speed;
        return -glm::normalize(v) * resistanceMag;
    }
    void collidingOffset(const Mesh &mesh, Soul &soul)
    {
        soul.myOuterAcceleration() = glm::vec3(0.0f);
        soul.myVelocity() = -soul.myVelocity();
    }
    void collidingOffset(const Mesh &mesh, Soul &soul, const AABB &deltaAABB)
    {
       
    }
};

#endif