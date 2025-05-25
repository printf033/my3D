#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "animator.hpp"
#include "mapping_gl.hpp"

#define COLLIDER_FEET_X 0.0f
#define COLLIDER_FEET_Y 0.0f
#define COLLIDER_FEET_HEIGHT 0.0f
#define COLLIDER_VELOCITY 0.05f

class Collider : public Animator
{
    bool fallingFlag_ = true;
    bool movingFlag_ = false;
    Mapping_bitset whereHit_ = Mapping_bitset::count;
    float velocity_ = COLLIDER_VELOCITY;
    glm::vec3 feet_; // 默认模型的原点都是在脚上
    glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right_ = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 direction_ = glm::vec3(0.0f);
    glm::vec3 shell_;

public:
    Collider(Animator &&entity,
             float x = COLLIDER_FEET_X,
             float y = COLLIDER_FEET_Y,
             float height = COLLIDER_FEET_HEIGHT)
        : Animator(std::move(entity)),
          feet_(x, height, y),
          shell_(x, (getMax().y + getMin().y) * .5f, y) {}
    Collider(Animator &&entity,
             const glm::mat4 &globalMat)
        : Animator(std::move(entity)),
          feet_(glm::vec3(globalMat[3][0], globalMat[3][1], globalMat[3][2])),
          shell_(glm::vec3(globalMat[3][0], (getMax().y + getMin().y) * .5f, globalMat[3][2])) {}
    ~Collider() = default;
    Collider(const Collider &) = delete;
    Collider &operator=(const Collider &) = delete;
    Collider(Collider &&other)
        : Animator(std::move(other)),
          fallingFlag_(other.fallingFlag_),
          movingFlag_(other.movingFlag_),
          whereHit_(other.whereHit_),
          velocity_(other.velocity_),
          feet_(other.feet_),
          front_(other.front_),
          up_(other.up_),
          right_(other.right_),
          direction_(other.direction_),
          shell_(other.shell_)
    {
        other.fallingFlag_ = false;
        other.movingFlag_ = false;
        other.whereHit_ = Mapping_bitset::count;
        other.feet_ = glm::vec3(0.0f);
        other.front_ = glm::vec3(0.0f);
        other.up_ = glm::vec3(0.0f);
        other.right_ = glm::vec3(0.0f);
        other.direction_ = glm::vec3(0.0f);
        other.shell_ = glm::vec3(0.0f);
        other.velocity_ = 0.0f;
    }
    Collider &operator=(Collider &&other)
    {
        if (this != &other)
            Collider(std::move(other)).swap(*this);
        return *this;
    }
    inline bool getFallingFlag() const { return fallingFlag_; }
    inline bool getMovingFlag() const { return movingFlag_; }
    inline void clearFallingFlag() { fallingFlag_ = false; }
    inline void clearMovingFlag() { movingFlag_ = false; }
    inline glm::vec3 &myFeet() { return feet_; }
    inline glm::vec3 &myDirection() { return direction_; }
    inline float getVelocity() const { return velocity_; }
    inline glm::vec3 getShell() const { return shell_; }
    inline glm::mat4 getGlobalMat() const
    {
        return glm::mat4(
            glm::vec4(right_, 0.0f),
            glm::vec4(up_, 0.0f),
            glm::vec4(front_, 0.0f),
            glm::vec4(feet_.x, feet_.y, feet_.z, 1.0f));
    }
    void setViewMove(const glm::mat4 &globalMat)
    {
        front_.x = globalMat[0][2];
        front_.y = globalMat[1][2];
        front_.z = globalMat[2][2];
        up_.x = globalMat[0][1];
        up_.y = globalMat[1][1];
        up_.z = globalMat[2][1];
        right_.x = globalMat[0][0];
        right_.y = globalMat[1][0];
        right_.z = globalMat[2][0];
    }
    void setWhereHit(Mapping_bitset direction)
    {
        whereHit_ = direction;
    }
    void processPosMove(Mapping_bitset direction, float deltaTime)
    {
        if (direction == whereHit_)
            return;
        glm::vec3 change(.0f);
        switch (direction)
        {
        case Mapping_bitset::FORWARD_1:
            direction_ = glm::vec3(.0f);
            direction_.x = front_.x;
            direction_.z = front_.z;
            change = glm::normalize(direction_) * (velocity_ * deltaTime);
            feet_.x += change.x;
            feet_.z += change.z;
            shell_.x = feet_.x;
            shell_.z = feet_.z;
            change = glm::normalize(direction_) * std::max(std::max(getMax().x, getMax().z), std::max(std::abs(getMin().x), std::abs(getMin().z))); // 目前shell水平方向采用圆形（半径为最大的feet_到vertex距离），高效不精准
            shell_.x += change.x;
            shell_.z += change.z;
            break;
        case Mapping_bitset::BACKWARD_1:
            direction_ = glm::vec3(.0f);
            direction_.x = -front_.x;
            direction_.z = -front_.z;
            change = glm::normalize(direction_) * (velocity_ * deltaTime);
            feet_.x += change.x;
            feet_.z += change.z;
            shell_.x = feet_.x;
            shell_.z = feet_.z;
            change = glm::normalize(direction_) * std::max(std::max(getMax().x, getMax().z), std::max(std::abs(getMin().x), std::abs(getMin().z))); // 目前shell水平方向采用圆形（半径为最大的feet_到vertex距离），高效不精准
            shell_.x += change.x;
            shell_.z += change.z;
            break;
        case Mapping_bitset::LEFT_1:
            direction_ = glm::vec3(.0f);
            direction_.x = -right_.x;
            direction_.z = -right_.z;
            change = glm::normalize(direction_) * (velocity_ * deltaTime);
            feet_.x += change.x;
            feet_.z += change.z;
            shell_.x = feet_.x;
            shell_.z = feet_.z;
            change = glm::normalize(direction_) * std::max(std::max(getMax().x, getMax().z), std::max(std::abs(getMin().x), std::abs(getMin().z))); // 目前shell水平方向采用圆形（半径为最大的feet_到vertex距离），高效不精准
            shell_.x += change.x;
            shell_.z += change.z;
            break;
        case Mapping_bitset::RIGHT_1:
            direction_ = glm::vec3(.0f);
            direction_.x = right_.x;
            direction_.z = right_.z;
            change = glm::normalize(direction_) * (velocity_ * deltaTime);
            feet_.x += change.x;
            feet_.z += change.z;
            shell_.x = feet_.x;
            shell_.z = feet_.z;
            change = glm::normalize(direction_) * std::max(std::max(getMax().x, getMax().z), std::max(std::abs(getMin().x), std::abs(getMin().z))); // 目前shell水平方向采用圆形（半径为最大的feet_到vertex距离），高效不精准
            shell_.x += change.x;
            shell_.z += change.z;
            break;
        case Mapping_bitset::UP_1:
            direction_ = glm::vec3(.0f);
            direction_.y = 1.0f; /////
            change = glm::normalize(direction_) * (velocity_ * deltaTime);
            feet_.y += change.y;
            shell_.y = feet_.y + getMax().y; // 没有考虑模型旋转
            fallingFlag_ = true;
            break;
        case Mapping_bitset::DOWN_1:
            direction_ = glm::vec3(.0f);
            direction_.y = -1.0f; ////
            change = glm::normalize(direction_) * (velocity_ * deltaTime);
            feet_.y += change.y;
            shell_.y = feet_.y; // 没有考虑模型旋转
            fallingFlag_ = true;
            break;
        default:
            break;
        }
        movingFlag_ = true;
        ///////////////////////////////////////////////////////////
        std::clog << "feet x:" << feet_.x
                  << " y:" << feet_.y
                  << " z:" << feet_.z << std::endl;
        std::clog << "shell x:" << shell_.x
                  << " y:" << shell_.y
                  << " z:" << shell_.z << std::endl;
        std::clog << "direction x:" << direction_.x
                  << " y:" << direction_.y
                  << " z:" << direction_.z << std::endl;
        ///////////////////////////////////////////////////////////
    }

private:
    void swap(Collider &other)
    {
        Animator::swap(other);
        std::swap(fallingFlag_, other.fallingFlag_);
        std::swap(movingFlag_, other.movingFlag_);
        std::swap(whereHit_, other.whereHit_);
        std::swap(velocity_, other.velocity_);
        std::swap(feet_, other.feet_);
        std::swap(front_, other.front_);
        std::swap(up_, other.up_);
        std::swap(right_, other.right_);
        std::swap(direction_, other.direction_);
        std::swap(shell_, other.shell_);
    }
};

#endif