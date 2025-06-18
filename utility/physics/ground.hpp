#ifndef GROUND_HPP
#define GROUND_HPP

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include "collider.hpp"

#define DECAY_RATE 0.9f // 为了简化，所有mesh的动量衰减系数都一样
#define FRICTION_RATE 0.9f

class Ground : public Animator
{
    std::unordered_map<std::string, Collider> colliders_;
    float precision_ = 0.1f;

public:
    Ground(Animator &&entity) : Animator(std::move(entity)) {}
    ~Ground() { colliders_.clear(); }
    Ground(const Ground &) = delete;
    Ground &operator=(const Ground &) = delete;
    Ground(Ground &&) = delete;
    Ground &operator=(Ground &&) = delete;
    inline void addCollider(const std::string &name, Collider &&collider) { colliders_.emplace(name, std::move(collider)); }
    inline Collider &getCollider(const std::string &name) { return colliders_.at(name); }
    void update(float deltaTime) // (s)
    {
        // 所有检测对象都有的力
        auto g = Ground::gravityAcc();
        for (auto &it : colliders_)
        {
            auto &collider = it.second;
            // 存在就会有的力
            collider.myOuterAcceleration() = g;
            // collider.myOuterAcceleration() += buoyancy(1.0f, 1.0f, g) / collider.getMass();
            // 运动就会有的力
            collider.myOuterAcceleration() += resistance(0.1f, collider.myVelocity()) / collider.getMass();
            // 碰撞就会有的力
            auto v0 = collider.myVelocity();
            auto v = v0 + (collider.myInnerAcceleration() + collider.myOuterAcceleration()) * deltaTime;
            glm::vec3 prePosition = (v0 + v) * deltaTime * 0.5f;
            AABB deltaAABB = collider.getOctree().getDeltaAABB(collider.myPosition(), prePosition);
            for (auto &aabb : getOctree().query(deltaAABB))
                collidingOffset(collider, aabb, deltaAABB);
            collider.myVelocity() += (collider.myInnerAcceleration() + collider.myOuterAcceleration()) * deltaTime;
            collider.myPosition() += collider.myVelocity() * deltaTime;
            collider.processDecay();
            collider.print(); ////////////////////////////////////////////////////////////////////////
        }
    }

private:
    // 返回所受地球重力加速度
    inline glm::vec3 gravityAcc() const
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
    void collidingOffset(Collider &collider, const AABB &aabb, const AABB &deltaAABB)
    {
        const Mesh &mesh = *reinterpret_cast<Mesh *>(aabb.where);
        for (auto &triangle : mesh.getOctree().query(deltaAABB))
        {
            auto p = reinterpret_cast<GLuint *>(triangle.where);
            auto &v0 = mesh.getVertices()[*p];
            auto &v1 = mesh.getVertices()[*(p + 1)];
            auto &v2 = mesh.getVertices()[*(p + 2)];
            auto edge1 = v1.position - v0.position;
            auto edge2 = v2.position - v0.position;
            auto normal = glm::normalize(glm::cross(edge1, edge2));
            auto iacc = glm::dot(normal, collider.myInnerAcceleration());
            auto oacc = glm::dot(normal, collider.myOuterAcceleration());
            auto vel = glm::dot(normal, collider.myVelocity());
            glm::vec3 v_perpendicular = collider.myVelocity() - vel * normal;
            glm::vec3 frictionDir = glm::vec3(0.0f);
            if (glm::length2(v_perpendicular) > 1e-6f)
                frictionDir = -glm::normalize(v_perpendicular);
            if (oacc < 0.0f)
            {
                collider.myOuterAcceleration() += glm::abs(oacc) * FRICTION_RATE * frictionDir;
                collider.myOuterAcceleration() -= oacc * normal;
            }
            if (iacc < 0.0f)
            {
                collider.myOuterAcceleration() += glm::abs(iacc) * FRICTION_RATE * frictionDir;
                collider.myInnerAcceleration() -= iacc * normal;
            }
            if (vel < 0.0f)
                collider.myVelocity() -= (1.0f + DECAY_RATE) * vel * normal;
        }
    }
    void collidingOffset(Collider_sphere &collider, const Mesh &mesh, const AABB &deltaAABB)
    {
        for (auto &aabb : mesh.getOctree().query(deltaAABB)) // capsule
        {
            auto p = reinterpret_cast<GLuint *>(aabb.where);
            auto &v0 = mesh.getVertices()[*p];
            auto &v1 = mesh.getVertices()[*(p + 1)];
            auto &v2 = mesh.getVertices()[*(p + 2)];
            auto edge1 = v1.position - v0.position;
            auto edge2 = v2.position - v0.position;
            auto normal = glm::normalize(glm::cross(edge1, edge2));
            glm::vec2 intersection;
            float distance = 0.0f;
        }
    }
};

#endif