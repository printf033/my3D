#ifndef GROUND_HPP
#define GROUND_HPP

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include "collider.hpp"

// 默认第一个Mesh为地面！
#define GRAVITY_ACCELERATION 9.8f

class Ground : public Animator
{
    std::unordered_map<std::string, Collider> colliders_;
    float half_g_ = GRAVITY_ACCELERATION * 0.5f;
    float sumTime_s_ = 0.0f;

public:
    Ground(Animator &&entity)
        : Animator(std::move(entity)) {}
    ~Ground()
    {
        colliders_.clear();
    }
    Ground(const Ground &) = delete;
    Ground &operator=(const Ground &) = delete;
    Ground(Ground &&) = delete;
    Ground &operator=(Ground &&) = delete;
    void addCollider(const std::string &name, Collider &&collider)
    {
        colliders_.emplace(name, std::move(collider));
    }
    Collider &getCollider(const std::string &name)
    {
        return colliders_.at(name);
    }
    void detectNcorrect(float deltaTime_us)
    {
        float o = 1e-6f;
        auto &groundMesh = getMeshes()[0]; // 默认第一个Mesh为地面！！！
        for (auto &it : colliders_)
        {
            auto &collider = it.second;
            // octree later...
            //////////////////////////////////////////////////////////
            for (auto &mesh : getMeshes())
            {
                float precision = 0.001f;
                if (&mesh == &groundMesh) // 重力模拟
                {
                    if (collider.getFallingFlag())
                        for (int i = 0; i < groundMesh.getIndices().size(); i += 3)
                        {
                            auto &v0 = groundMesh.getVertices()[groundMesh.getIndices()[i]];
                            auto &v1 = groundMesh.getVertices()[groundMesh.getIndices()[i + 1]];
                            auto &v2 = groundMesh.getVertices()[groundMesh.getIndices()[i + 2]];
                            glm::vec2 intersection;
                            float distance = .0f;
                            if (glm::intersectRayTriangle(collider.myFeet(),
                                                          glm::vec3(0.0f, -1.0f, 0.0f), // 默认重力竖直向下！！！
                                                          v0.position,
                                                          v1.position,
                                                          v2.position,
                                                          intersection,
                                                          distance) &&
                                (distance > o || distance < -o))
                            { // d = 1/2 * g * ( t^2 - t0^2 )
                                if (distance < -o)
                                    collider.myFeet().y -= distance;
                                float t0_2 = sumTime_s_ * sumTime_s_;
                                sumTime_s_ += deltaTime_us * 1e-6;
                                float t_2 = sumTime_s_ * sumTime_s_;
                                float d = half_g_ * (t_2 - t0_2);
                                collider.myFeet().y -= d;
                                collider.myDirection() = glm::normalize(collider.myDirection());
                                collider.myDirection() *= collider.getVelocity();
                                collider.myDirection().y -= sumTime_s_ * GRAVITY_ACCELERATION;
                                if (collider.myFeet().y < precision && collider.myFeet().y > -precision)
                                {
                                    sumTime_s_ = 0.0f;
                                    collider.clearFallingFlag();
                                }
                                break;
                            }
                        }
                }
                else // 处理与地面中其他Mesh的碰撞
                {
                    float precision = 0.01f;
                    if (collider.getMovingFlag())
                        if ((collider.getMax().x + collider.myFeet().x < mesh.getMin().x) ||
                            (collider.getMin().x + collider.myFeet().x > mesh.getMax().x) ||
                            (collider.getMax().y + collider.myFeet().y < mesh.getMin().y) ||
                            (collider.getMin().y + collider.myFeet().y > mesh.getMax().y) ||
                            (collider.getMax().z + collider.myFeet().z < mesh.getMin().z) ||
                            (collider.getMin().z + collider.myFeet().z > mesh.getMax().z))
                        {
                            collider.setWhereHit(Mapping_bitset::count);
                            continue; // AABB culling
                        }
                        else
                        {
                            // std::clog << "collide: " << mesh.getName() << std::endl;
                            auto direction = collider.myDirection();
                            if (direction.x > o)
                            {
                                collider.myFeet().x += -precision;
                                collider.setWhereHit(Mapping_bitset::RIGHT_1);
                            }
                            if (direction.x < -o)
                            {
                                collider.myFeet().x += precision;
                                collider.setWhereHit(Mapping_bitset::LEFT_1);
                            }
                            if (direction.y > o)
                            {
                                collider.myFeet().y += -precision;
                                collider.setWhereHit(Mapping_bitset::UP_1);
                            }
                            if (direction.y < -o)
                            {
                                collider.myFeet().y += precision;
                                collider.setWhereHit(Mapping_bitset::DOWN_1);
                                collider.clearFallingFlag();
                            }
                            if (direction.z > o)
                            {
                                collider.myFeet().z += -precision;
                                collider.setWhereHit(Mapping_bitset::BACKWARD_1);
                            }
                            if (direction.z < -o)
                            {
                                collider.myFeet().z += precision;
                                collider.setWhereHit(Mapping_bitset::FORWARD_1);
                            }
                            // 弹性碰撞以后再实现
                            //////////////////////////////////////////////////////////////////
                        }
                }
            }
            // 处理与其他Collider的碰撞
            /////////////////////////////////////////////////
        }
    }
};

#endif