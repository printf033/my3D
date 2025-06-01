#ifndef AABB_HPP
#define AABB_HPP

#include <glm/glm.hpp>

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 centre;
    glm::vec3 size;
    // where am I
    void *where;

    AABB(const glm::vec3 &min,
         const glm::vec3 &max,
         void *where = nullptr)
        : min(min),
          max(max),
          centre((min + max) * 0.5f),
          size(max - min),
          where(where) {}
    ~AABB() = default;
    AABB(const AABB &) = default;
    AABB &operator=(const AABB &) = default;
    AABB(AABB &&) = default;
    AABB &operator=(AABB &&) = default;
    inline bool intersects(const AABB &other) const
    {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
    inline bool contains(const AABB &other) const
    {
        return (min.x <= other.min.x && max.x >= other.max.x) &&
               (min.y <= other.min.y && max.y >= other.max.y) &&
               (min.z <= other.min.z && max.z >= other.max.z);
    }
    inline bool containsPoint(const glm::vec3 &point) const
    {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }
    inline AABB getGlobalAABB(const glm::vec3 &position) const
    {
        return AABB(min + position, max + position);
    }
    inline void print() const
    {
        std::cout << "\nmin x:" << min.x << " centre x:" << centre.x << " max x:" << max.x
                  << "\nmin y:" << min.y << " centre y:" << centre.y << " max y:" << max.y
                  << "\nmin z:" << min.z << " centre z:" << centre.z << " max z:" << max.z
                  << std::endl;
    }
};

#endif