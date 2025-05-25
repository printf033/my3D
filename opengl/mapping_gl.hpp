#ifndef MAPPING_GL_HPP
#define MAPPING_GL_HPP

#include <GLFW/glfw3.h>

// 两个mapping表要严格对照

enum class Mapping_key
{
    EXIT = GLFW_KEY_ESCAPE,
    CURSOR = GLFW_KEY_LEFT_ALT,
    FORWARD = GLFW_KEY_W,
    BACKWARD = GLFW_KEY_S,
    LEFT = GLFW_KEY_A,
    RIGHT = GLFW_KEY_D,
    UP = GLFW_KEY_SPACE,
    DOWN = GLFW_KEY_LEFT_CONTROL,
    FORWARD_1 = GLFW_KEY_UP,
    BACKWARD_1 = GLFW_KEY_DOWN,
    LEFT_1 = GLFW_KEY_LEFT,
    RIGHT_1 = GLFW_KEY_RIGHT,
    UP_1 = GLFW_KEY_KP_0,
    DOWN_1 = GLFW_KEY_RIGHT_CONTROL,
};

enum class Mapping_bitset
{
    EXIT,
    CURSOR,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    FORWARD_1,
    BACKWARD_1,
    LEFT_1,
    RIGHT_1,
    UP_1,
    DOWN_1,

    count
};

#endif
