#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "animator.hpp"
#include "mapping_gl.hpp"

#define SOUL_POSITION_X 0.0f
#define SOUL_POSITION_Y 0.0f
#define SOUL_POSITION_H 0.0f
#define SOUL_INNER_ACCELERATION_INCREMENT 1.0f
#define SOUL_INNER_ACCELERATION_REDUCTION_RATE 0.99f
#define SOUL_MAX_INNER_ACCELERATION 15.0f

struct physicalProperties
{
    float mass = 1.0f;
};

// Cuboid
class Collider : public Animator
{
    glm::vec3 position_;
    glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right_ = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 velocity_ = glm::vec3(0.0f);
    glm::vec3 innerAcceleration_ = glm::vec3(0.0f);
    glm::vec3 outerAcceleration_ = glm::vec3(0.0f);
    physicalProperties physicalProperties_;

public:
    Collider(Animator &&entity,
             float x = SOUL_POSITION_X,
             float y = SOUL_POSITION_Y,
             float height = SOUL_POSITION_H,
             const physicalProperties &physclPrprts = physicalProperties())
        : Animator(std::move(entity)),
          position_(x, height, y),
          physicalProperties_(physclPrprts) {}
    Collider(Animator &&entity,
             const glm::mat4 &globalMat,
             const physicalProperties &physclPrprts = physicalProperties())
        : Animator(std::move(entity)),
          position_(glm::vec3(globalMat[3][0], globalMat[3][1], globalMat[3][2])),
          physicalProperties_(physclPrprts) {}
    ~Collider() = default;
    void swap(Collider &other)
    {
        Animator::swap(other);
        std::swap(position_, other.position_);
        std::swap(front_, other.front_);
        std::swap(up_, other.up_);
        std::swap(right_, other.right_);
        std::swap(velocity_, other.velocity_);
        std::swap(innerAcceleration_, other.innerAcceleration_);
        std::swap(outerAcceleration_, other.outerAcceleration_);
    }
    Collider(const Collider &) = delete;
    Collider &operator=(const Collider &) = delete;
    Collider(Collider &&other)
        : Animator(std::move(other)),
          position_(other.position_),
          front_(other.front_),
          up_(other.up_),
          right_(other.right_),
          velocity_(other.velocity_),
          innerAcceleration_(other.innerAcceleration_),
          outerAcceleration_(other.outerAcceleration_),
          physicalProperties_(other.physicalProperties_)
    {
        other.position_ = glm::vec3(0.0f);
        other.front_ = glm::vec3(0.0f);
        other.up_ = glm::vec3(0.0f);
        other.right_ = glm::vec3(0.0f);
        other.velocity_ = glm::vec3(0.0f);
        other.innerAcceleration_ = glm::vec3(0.0f);
        other.outerAcceleration_ = glm::vec3(0.0f);
        other.physicalProperties_ = physicalProperties();
    }
    Collider &operator=(Collider &&other)
    {
        if (this != &other)
            Collider(std::move(other)).swap(*this);
        return *this;
    }
    inline glm::mat4 getGlobalMat() const
    {
        return glm::mat4(
            glm::vec4(right_, 0.0f),
            glm::vec4(up_, 0.0f),
            glm::vec4(front_, 0.0f),
            glm::vec4(position_.x, position_.y, position_.z, 1.0f));
    }
    inline glm::vec3 &myPosition() { return position_; }
    inline glm::vec3 &myVelocity() { return velocity_; }
    inline glm::vec3 &myOuterAcceleration() { return outerAcceleration_; }
    inline glm::vec3 &myInnerAcceleration() { return innerAcceleration_; }
    inline float getMass() { return physicalProperties_.mass; }
    inline void print() const
    {
        std::cout << "\n位置\nx:" << position_.x
                  << " y:" << position_.y
                  << " z:" << position_.z
                  << "\n速度:" << glm::length(velocity_)
                  << "\nx:" << velocity_.x
                  << " y:" << velocity_.y
                  << " z:" << velocity_.z
                  << "\n内加速度:" << glm::length(innerAcceleration_)
                  << "\nx:" << innerAcceleration_.x
                  << " y:" << innerAcceleration_.y
                  << " z:" << innerAcceleration_.z
                  << "\n外加速度:" << glm::length(outerAcceleration_)
                  << "\nx:" << outerAcceleration_.x
                  << " y:" << outerAcceleration_.y
                  << " z:" << outerAcceleration_.z
                  << "\n\n";
    }
    void setView(const glm::mat4 &globalMat)
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
    void processBoost(Mapping_bitset direction)
    {
        glm::vec3 moveDir(0.0f);
        switch (direction)
        {
        case Mapping_bitset::FORWARD_1:
            moveDir += glm::vec3(front_.x, 0.0f, front_.z);
            break;
        case Mapping_bitset::BACKWARD_1:
            moveDir -= glm::vec3(front_.x, 0.0f, front_.z);
            break;
        case Mapping_bitset::LEFT_1:
            moveDir -= glm::vec3(right_.x, 0.0f, right_.z);
            break;
        case Mapping_bitset::RIGHT_1:
            moveDir += glm::vec3(right_.x, 0.0f, right_.z);
            break;
        case Mapping_bitset::UP_1:
            moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
            break;
        case Mapping_bitset::DOWN_1:
            moveDir += glm::vec3(0.0f, -1.0f, 0.0f);
            break;
        default:
            break;
        }
        if (glm::length(moveDir) > 0.0f)
        {
            innerAcceleration_ += glm::normalize(moveDir) * SOUL_INNER_ACCELERATION_INCREMENT;
            if (glm::length(innerAcceleration_) > SOUL_MAX_INNER_ACCELERATION)
                innerAcceleration_ = glm::normalize(innerAcceleration_) * SOUL_MAX_INNER_ACCELERATION;
        }
    }
    void processDecay()
    {
        if (glm::length(innerAcceleration_) > 0.0f)
            innerAcceleration_ *= SOUL_INNER_ACCELERATION_REDUCTION_RATE;
    }
};

// sphere
class Collider_sphere : public Collider
{
    float radius_;

public:
    Collider_sphere(Collider &&entity,
                    float radius)
        : Collider(std::move(entity)),
          radius_(radius) {}
    ~Collider_sphere() = default;
    void swap(Collider_sphere &other)
    {
        Collider::swap(other);
        std::swap(radius_, other.radius_);
    }
    Collider_sphere(const Collider_sphere &) = delete;
    Collider_sphere &operator=(const Collider_sphere &) = delete;
    Collider_sphere(Collider_sphere &&other)
        : Collider(std::move(other)),
          radius_(other.radius_)
    {
        other.radius_ = 0.0f;
    }
    Collider_sphere &operator=(Collider_sphere &&other)
    {
        if (this != &other)
            Collider_sphere(std::move(other)).swap(*this);
        return *this;
    }
    inline float getRadius() const { return radius_; }
    inline const glm::vec3 &getCentre() const { return getOctree().getCentre(); }
};

#endif