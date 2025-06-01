// #ifndef SPACE_HPP
// #define SPACE_HPP

// #include <glm/glm.hpp>
// #include <glm/gtx/intersect.hpp>
// #include "soul.hpp"

// class Space : public Animator
// {
//     std::unordered_map<std::string, Soul> souls_;
//     float precision = 0.01f;

// public:
//     Space(Animator &&entity) : Animator(std::move(entity)) {}
//     ~Space() { souls_.clear(); }
//     Space(const Space &) = delete;
//     Space &operator=(const Space &) = delete;
//     Space(Space &&) = delete;
//     Space &operator=(Space &&) = delete;
//     inline void addSoul(const std::string &name, Soul &&soul) { souls_.emplace(name, std::move(soul)); }
//     inline Soul &getSoul(const std::string &name) { return souls_.at(name); }
//     void update(float deltaTime)
//     {
//         for (auto &it : souls_)
//         {
//             auto &soul = it.second;
//             if (glm::length(soul.getVelocity()) > precision)
//                 LOG_INFO << "speed: " << glm::length(soul.getVelocity());
//             // 处理与其他soul的碰撞
//             /////////////////////////////////////////////////
//             // octree later...
//             //////////////////////////////////////////////////////////
//             for (auto &mesh : getMeshes())
//             {
//                 if (isColliding(mesh, soul))
//                     continue;
//                 else
//                 {
//                     LOG_DEBUG << "collide: " << mesh.getName();////////////
//                     supportingOffset(mesh, soul);
//                 }
//             }
//         }
//     }

// private:
//     // AABB culling
//     bool noColliding(const Mesh &mesh, const Soul &soul)
//     {
//         return soul.getMax().x + soul.getPosition().x < mesh.getMin().x ||
//                soul.getMin().x + soul.getPosition().x > mesh.getMax().x ||
//                soul.getMax().y + soul.getPosition().y < mesh.getMin().y ||
//                soul.getMin().y + soul.getPosition().y > mesh.getMax().y ||
//                soul.getMax().z + soul.getPosition().z < mesh.getMin().z ||
//                soul.getMin().z + soul.getPosition().z > mesh.getMax().z;
//     }
//     // 返回m所受引力加速度
//     glm::vec3 attraction(float G, float M, float m, float r, const glm::vec3 &direction_m2M)
//     {
//         if (M == 0.0f || m == 0.0f || glm::length(direction_m2M) == 0.0f || r == 0.0f)
//             return glm::vec3(0.0f);
//         float accelerationMagnitude = G * M / (r * r); // a = G * M / r^2
//         return glm::normalize(direction_m2M) * accelerationMagnitude;
//     }
//     // 支持力补偿
//     void supportingOffset(const Mesh &mesh, Soul &soul)
//     {
//         for (int i = 0; i < mesh.getIndices().size(); i += 3)
//         {
//             auto &v0 = mesh.getVertices()[mesh.getIndices()[i]];
//             auto &v1 = mesh.getVertices()[mesh.getIndices()[i + 1]];
//             auto &v2 = mesh.getVertices()[mesh.getIndices()[i + 2]];
//             auto edge1 = v1.position - v0.position;
//             auto edge2 = v2.position - v0.position;
//             auto normal = glm::normalize(glm::cross(edge1, edge2));
//             glm::vec2 intersection;
//             float distance = .0f;
//             if (glm::intersectRayTriangle(soul.getPosition(),
//                                           -normal,
//                                           v0.position,
//                                           v1.position,
//                                           v2.position,
//                                           intersection,
//                                           distance) &&
//                 (distance <= precision && distance >= -precision))
//             {
//                 float projection = glm::dot(soul.myAcceleration(), -normal);
//                 if (projection > 0.0f)
//                 {
//                     soul.myAcceleration() += normal * projection;
//                     // 弹性碰撞在这里实现
//                     ////////////////////////////////////////////
//                 }
//             }
//         }
//     }
//     // 返回所受摩擦力
//     glm::vec3 friction(float u, float supportingMagnitude, const glm::vec3 &velocity)
//     {
//         if (glm::length(velocity) == 0.0f || u == 0.0f)
//             return glm::vec3(0.0f);
//         float frictionMagnitude = u * supportingMagnitude;
//         return -glm::normalize(velocity) * frictionMagnitude;
//     }
//     // 返回所受阻力
//     glm::vec3 resistance(float C, const glm::vec3 &velocity)
//     {
//         if (glm::length(velocity) == 0.0f || C == 0.0f)
//             return glm::vec3(0.0f);
//         float speed = glm::length(velocity);
//         float resistanceMagnitude = C * speed * speed;
//         return -glm::normalize(velocity) * resistanceMagnitude;
//     }
//     // 返回所受浮力
//     glm::vec3 buoyancy(float rho, float V, float g, const glm::vec3 &gravityDirection)
//     {
//         if (V == 0.0f || rho == 0.0f)
//             return glm::vec3(0.0f);
//         float buoyantAcceleration = rho * V * g; // a = (rho * V * g)
//         return -glm::normalize(gravityDirection) * buoyantAcceleration;
//     }
// };

// #endif