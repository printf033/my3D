#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "aabb.hpp"

#define OCTREE_MAX_DEPTH 5
#define OCTREE_MAX_OBJECTS 4

class Octree
{
    struct OctreeNode : public AABB
    {
        std::vector<AABB> objects;
        OctreeNode *children[8] = {nullptr};
        int depth = 0;
        int maxDepth;
        int maxObjects;

        OctreeNode(AABB &&aabb,
                   int depth,
                   int maxDepth = OCTREE_MAX_DEPTH,
                   int maxObjects = OCTREE_MAX_OBJECTS)
            : AABB(std::move(aabb)),
              depth(depth),
              maxDepth(maxDepth),
              maxObjects(maxObjects) {}
        ~OctreeNode()
        {
            for (auto &child : children)
                delete child;
        }
        OctreeNode(const OctreeNode &) = delete;
        OctreeNode &operator=(const OctreeNode &) = delete;
        OctreeNode(OctreeNode &&other)
            : AABB(std::move(other)),
              objects(std::move(other.objects)),
              depth(other.depth),
              maxDepth(other.maxDepth),
              maxObjects(other.maxObjects)
        {
            for (int i = 0; i < 8; ++i)
            {
                children[i] = other.children[i];
                other.children[i] = nullptr;
            }
        }
        OctreeNode &operator=(OctreeNode &&other)
        {
            depth = other.depth;
            maxDepth = other.maxDepth;
            maxObjects = other.maxObjects;
            AABB::min = other.min;
            AABB::max = other.max;
            AABB::centre = other.centre;
            AABB::size = other.size;
            AABB::where = other.where;
            objects = std::move(other.objects);
            for (int i = 0; i < 8; ++i)
            {
                children[i] = other.children[i];
                other.children[i] = nullptr;
            }
            return *this;
        }
        void insert(const AABB &obj)
        {
            if (!intersects(obj))
                return;
            if (isLeaf())
            {
                objects.push_back(obj);
                if (objects.size() > maxObjects && depth < maxDepth)
                {
                    subdivide();
                    std::vector<AABB> remainingObjects;
                    for (const auto &existingObj : objects)
                    {
                        bool assigned = false;
                        for (auto &child : children)
                            if (child && child->contains(existingObj))
                            {
                                child->insert(existingObj);
                                assigned = true;
                                break;
                            }
                        if (!assigned)
                            remainingObjects.push_back(existingObj);
                    }
                    objects = std::move(remainingObjects);
                }
                return;
            }
            for (auto &child : children)
                if (child && child->contains(obj))
                {
                    child->insert(obj);
                    return;
                }
            objects.push_back(obj);
        }
        void query(const AABB &range, std::vector<AABB> &result) const
        {
            if (!intersects(range))
                return;
            for (const auto &obj : objects)
                if (obj.intersects(range))
                    result.push_back(obj);
            if (!isLeaf())
                for (const auto &child : children)
                    if (child)
                        child->query(range, result);
        }
        inline void print() const
        {
            std::cout << "\ndepth:" << depth << std::endl;
            AABB::print();
            std::cout << "\nobjects:" << std::endl;
            for (auto &obj : objects)
                obj.print();
            std::cout << "\nchildren:" << std::endl;
            for (auto &child : children)
                if (child)
                    child->print();
        }

    private:
        inline bool isLeaf() const { return nullptr == children[0]; }
        void subdivide()
        {
            for (int i = 0; i < 8; ++i)
            {
                glm::vec3 newMin = glm::vec3(
                    (i & 1) ? centre.x : min.x,
                    (i & 2) ? centre.y : min.y,
                    (i & 4) ? centre.z : min.z);
                glm::vec3 newMax = glm::vec3(
                    (i & 1) ? max.x : centre.x,
                    (i & 2) ? max.y : centre.y,
                    (i & 4) ? max.z : centre.z);
                children[i] = new OctreeNode(AABB(newMin, newMax), depth + 1, maxDepth, maxObjects);
            }
        }
    };
    OctreeNode *root_ = nullptr;

public:
    Octree(AABB &&aabb = AABB(glm::vec3(std::nanf("")), glm::vec3(std::nanf(""))))
    {
        if (std::isnan(aabb.min.x))
            return;
        root_ = new OctreeNode(std::move(aabb), 0);
    }
    ~Octree() { delete root_; }
    Octree(const Octree &) = delete;
    Octree &operator=(const Octree &) = delete;
    Octree(Octree &&other) : root_(other.root_) { other.root_ = nullptr; }
    Octree &operator=(Octree &&other)
    {
        root_ = other.root_;
        other.root_ = nullptr;
        return *this;
    }
    inline void insert(const AABB &obj)
    {
        assert(root_);
        root_->insert(obj);
    }
    std::vector<AABB> query(const AABB &range) const
    {
        assert(root_);
        std::vector<AABB> result;
        root_->query(range, result);
        return result;
    }
    inline const glm::vec3 &getMin() const
    {
        assert(root_);
        return root_->min;
    }
    inline const glm::vec3 &getMax() const
    {
        assert(root_);
        return root_->max;
    }
    inline const glm::vec3 &getCentre() const
    {
        assert(root_);
        return root_->centre;
    }
    inline const glm::vec3 &getSize() const
    {
        assert(root_);
        return root_->size;
    }
    inline AABB getGlobalAABB(const glm::vec3 &position = glm::vec3(0.0f)) const
    {
        assert(root_);
        return AABB(root_->min + position, root_->max + position, root_->where);
    }
    inline AABB getDeltaAABB(const glm::vec3 &position, const glm::vec3 &prePosition) const
    {
        assert(root_);
        AABB deltaAABB(root_->min + position, root_->max + position, root_->where);
        if (prePosition.x < 0.0f)
            deltaAABB.min.x += prePosition.x;
        else
            deltaAABB.max.x += prePosition.x;
        if (prePosition.y < 0.0f)
            deltaAABB.min.y += prePosition.y;
        else
            deltaAABB.max.y += prePosition.y;
        if (prePosition.z < 0.0f)
            deltaAABB.min.z += prePosition.z;
        else
            deltaAABB.max.z += prePosition.z;
        return deltaAABB;
    }
    inline void print() const
    {
        assert(root_);
        root_->print();
    }
};

#endif