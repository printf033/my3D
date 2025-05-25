#ifdef ENGINE_GL

#ifndef ENGINE_GL_HPP
#define ENGINE_GL_HPP

#include <iostream>
#include <stdexcept>
#include <bitset>
#include <thread>
#include <functional>
#include <unordered_map>
#include <string>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_gl.hpp"
#include "mesh_gl.hpp"
#include "mapping_gl.hpp"
#include "deliver_gl.hpp"
#include "model.hpp"
#include "animator.hpp"
#include "ground.hpp"

#define BACKGROUND_RED 0.7f
#define BACKGROUND_GREEN 0.7f
#define BACKGROUND_BLUE 0.0f
#define BACKGROUND_ALPHA 1.0f
#define EYE_X 0.0f
#define EYE_Y 0.0f
#define EYE_HEIGHT 1.5f
#define VIEW_FOV_VERTICAL 60.0f
#define VIEW_ASPECT_RATE (16.0f / 9.0f)
#define VIEW_NEAR_LIMIT 0.1f
#define VIEW_FAR_LIMIT 100.0f
#define MOVE_SENSITIVITY 0.2f
#define VIEW_SENSITIVITY 0.15f
#define ZOOM_SENSITIVITY 3.0f
#define KEY_SCAN_INTERVAL_us 100

// OpenGL 4.6 core
class Engine
{
    GLFWwindow *window_ = nullptr;
    std::jthread workThread_;
    std::unordered_map<std::string, Shader> shaders_;
    std::unordered_map<std::string, Deliver> delivers_;

    Engine()
        : workThread_([this](std::stop_token st) { // 检查按键
              auto checkKey = [&](Mapping_bitset key, bool isPress)
              {
                  switch (key)
                  {
                  case Mapping_bitset::EXIT:
                      if (isPress)
                          glfwSetWindowShouldClose(window_, true);
                      else
                          ;
                      break;
                  case Mapping_bitset::CURSOR:
                      if (isPress)
                          glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                      else
                          glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                      break;
                  case Mapping_bitset::FORWARD:
                      if (isPress)
                          processPosMove(key);
                      else
                          ;
                      break;
                  case Mapping_bitset::BACKWARD:
                      if (isPress)
                          processPosMove(key);
                      else
                          ;
                      break;
                  case Mapping_bitset::LEFT:
                      if (isPress)
                          processPosMove(key);
                      else
                          ;
                      break;
                  case Mapping_bitset::RIGHT:
                      if (isPress)
                          processPosMove(key);
                      else
                          ;
                      break;
                  case Mapping_bitset::UP:
                      if (isPress)
                          processPosMove(key);
                      else
                          ;
                      break;
                  case Mapping_bitset::DOWN:
                      if (isPress)
                          processPosMove(key);
                      else
                          ;
                      break;
                  case Mapping_bitset::FORWARD_1:
                      if (isPress)
                          if (nullptr != world)
                              reinterpret_cast<Ground *>(world)->getCollider("sphere").processPosMove(key, deltaTime);
                      break;
                  case Mapping_bitset::BACKWARD_1:
                      if (isPress)
                          if (nullptr != world)
                              reinterpret_cast<Ground *>(world)->getCollider("sphere").processPosMove(key, deltaTime);
                      break;
                  case Mapping_bitset::LEFT_1:
                      if (isPress)
                          if (nullptr != world)
                              reinterpret_cast<Ground *>(world)->getCollider("sphere").processPosMove(key, deltaTime);
                      break;
                  case Mapping_bitset::RIGHT_1:
                      if (isPress)
                          if (nullptr != world)
                              reinterpret_cast<Ground *>(world)->getCollider("sphere").processPosMove(key, deltaTime);
                      break;
                  case Mapping_bitset::UP_1:
                      if (isPress)
                          if (nullptr != world)
                              reinterpret_cast<Ground *>(world)->getCollider("sphere").processPosMove(key, deltaTime);
                      break;
                  case Mapping_bitset::DOWN_1:
                      if (isPress)
                          if (nullptr != world)
                              reinterpret_cast<Ground *>(world)->getCollider("sphere").processPosMove(key, deltaTime);
                      break;
                  default:
                      break;
                  }
              };
              while (!st.stop_requested())
              {
                  auto lastScanTime = std::chrono::high_resolution_clock::now();
                  std::this_thread::sleep_for(std::chrono::microseconds(KEY_SCAN_INTERVAL_us));
                  if (nullptr == window_)
                      continue;
                  for (int i = 0; i < keyMapping.size(); ++i)
                      checkKey((Mapping_bitset)i, keyMapping.test(i));
                  if (nullptr != world)
                      reinterpret_cast<Ground *>(world)->detectNcorrect(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - lastScanTime).count());
              }
          })
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        window_ = glfwCreateWindow(mode->width, mode->height, "my3D", monitor, nullptr);
        if (nullptr == window_)
            throw std::runtime_error("glfwCreateWindow failed");
        glfwMakeContextCurrent(window_);
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window_, int width, int height)
                                       { glViewport(0, 0, width, height); });
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwDestroyWindow(window_);
            glfwTerminate();
            throw std::runtime_error("gladLoadGLLoader failed");
        }
    }
    ~Engine()
    {
        workThread_.request_stop();
        if (workThread_.joinable())
            workThread_.join();
        delivers_.clear();
        shaders_.clear();
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;
    Engine(Engine &&) = delete;
    Engine &operator=(Engine &&) = delete;

public:
    static float moveSensitivity;
    static float viewSensitivity;
    static float zoomSensitivity;
    static float cursorX;
    static float cursorY;
    static float yaw;
    static float pitch;
    static float fovy;
    static float aspect;
    static float nearLimit;
    static float farLimit;
    static void *world;
    static double lastTime;
    static double deltaTime;
    static glm::vec3 front;
    static glm::vec3 up;
    static glm::vec3 right;
    static glm::vec3 eye;
    static glm::vec3 worldUp;
    static std::bitset<(int)Mapping_bitset::count> keyMapping;

    static Engine &get()
    {
        static Engine instance;
        return instance;
    }
    bool isRunning() const
    {
        return !glfwWindowShouldClose(window_);
    }
    void inputFor3D()
    {
        glfwSetCursorPosCallback(window_, [](GLFWwindow *window_, double xpos, double ypos) { // 鼠标移动
            float xoffset = (float)xpos - cursorX;
            float yoffset = cursorY - (float)ypos;
            cursorX = (float)xpos;
            cursorY = (float)ypos;
            yaw += xoffset * viewSensitivity;
            pitch += yoffset * viewSensitivity;
            pitch = glm::clamp(pitch, -89.9f, 89.9f);
            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            Engine::front = glm::normalize(front);
            right = glm::normalize(glm::cross(Engine::front, worldUp));
            up = glm::normalize(glm::cross(right, Engine::front));
        });
        glfwSetMouseButtonCallback(window_, [](GLFWwindow *window_, int button, int action, int mods) { // 鼠标点击
            std::clog << "mouse: " << button << ", " << action << ", " << mods << std::endl;
        });
        glfwSetScrollCallback(window_, [](GLFWwindow *window_, double xoffset, double yoffset) { // 鼠标滚轮
            fovy -= (float)yoffset * zoomSensitivity;
            fovy = glm::clamp(fovy, 10.0f, 120.0f);
        });
        glfwSetKeyCallback(window_, [](GLFWwindow *window_, int key, int scancode, int action, int mods) { // 按键输入
            switch ((Mapping_key)key)
            {
            case Mapping_key::EXIT:
                keyMapping.set((int)Mapping_bitset::EXIT, action != GLFW_RELEASE);
                break;
            case Mapping_key::CURSOR:
                keyMapping.set((int)Mapping_bitset::CURSOR, action != GLFW_RELEASE);
                break;
            case Mapping_key::FORWARD:
                keyMapping.set((int)Mapping_bitset::FORWARD, action != GLFW_RELEASE);
                break;
            case Mapping_key::BACKWARD:
                keyMapping.set((int)Mapping_bitset::BACKWARD, action != GLFW_RELEASE);
                break;
            case Mapping_key::LEFT:
                keyMapping.set((int)Mapping_bitset::LEFT, action != GLFW_RELEASE);
                break;
            case Mapping_key::RIGHT:
                keyMapping.set((int)Mapping_bitset::RIGHT, action != GLFW_RELEASE);
                break;
            case Mapping_key::UP:
                keyMapping.set((int)Mapping_bitset::UP, action != GLFW_RELEASE);
                break;
            case Mapping_key::DOWN:
                keyMapping.set((int)Mapping_bitset::DOWN, action != GLFW_RELEASE);
                break;
            case Mapping_key::FORWARD_1:
                keyMapping.set((int)Mapping_bitset::FORWARD_1, action != GLFW_RELEASE);
                break;
            case Mapping_key::BACKWARD_1:
                keyMapping.set((int)Mapping_bitset::BACKWARD_1, action != GLFW_RELEASE);
                break;
            case Mapping_key::LEFT_1:
                keyMapping.set((int)Mapping_bitset::LEFT_1, action != GLFW_RELEASE);
                break;
            case Mapping_key::RIGHT_1:
                keyMapping.set((int)Mapping_bitset::RIGHT_1, action != GLFW_RELEASE);
                break;
            case Mapping_key::UP_1:
                keyMapping.set((int)Mapping_bitset::UP_1, action != GLFW_RELEASE);
                break;
            case Mapping_key::DOWN_1:
                keyMapping.set((int)Mapping_bitset::DOWN_1, action != GLFW_RELEASE);
                break;
            default:
                break;
            }
        });
    }
    // 不要调用
    void inputFor2D()
    {
        glfwSetCursorPosCallback(window_, [](GLFWwindow *window_, double xpos, double ypos)
                                 { std::clog << "cursor: " << xpos << ", " << ypos << std::endl; });
        glfwSetMouseButtonCallback(window_, [](GLFWwindow *window_, int button, int action, int mods)
                                   { std::clog << "mouse: " << button << ", " << action << ", " << mods << std::endl; });
        glfwSetScrollCallback(window_, [](GLFWwindow *window_, double xoffset, double yoffset)
                              { std::clog << "scroll: " << xoffset << ", " << yoffset << std::endl; });
        glfwSetKeyCallback(window_, [](GLFWwindow *window_, int key, int scancode, int action, int mods)
                           { std::clog << "key: " << key << ", " << scancode << ", " << action << ", " << mods << std::endl; });
        glfwSetCharCallback(window_, [](GLFWwindow *window_, unsigned int codepoint)
                            { std::clog << "char: " << codepoint << std::endl; });
        glfwSetWindowCloseCallback(window_, [](GLFWwindow *window_)
                                   { std::clog << "close" << std::endl; });
    }
    void addShader(const std::string &name,
                   const std::filesystem::path &vertexPath,
                   const std::filesystem::path &fragmentPath,
                   const std::filesystem::path &geometryPath)
    {
        shaders_.emplace(name, Shader(vertexPath, fragmentPath, geometryPath));
    }
    void addDeliver(const std::string &name,
                    const std::vector<glm::mat4> &transforms)
    {
        delivers_.emplace(name, Deliver(transforms, delivers_.size())); // 由于绑定点的缘故，delivers_不可以随便删除
    }
    void update() const
    {
        double curTime = glfwGetTime();
        deltaTime = curTime - lastTime;
        lastTime = curTime;
        glClearColor(BACKGROUND_RED, BACKGROUND_GREEN, BACKGROUND_BLUE, BACKGROUND_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);
        // glFrontFace(GL_CCW);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    void draw(const std::string &shaderName,
              const Mesh &mesh,
              const glm::mat4 &globalMat = glm::mat4(1.0f)) const
    {
        auto &shader = shaders_.at(shaderName);
        shader.use();
        shader.setMat4("model", globalMat);
        shader.setMat4("view", glm::lookAt(eye, eye + front, up));
        shader.setMat4("projection", glm::perspective(glm::radians(fovy), aspect, nearLimit, farLimit));
        mesh.draw(shader.getID());
    }
    void draw(const std::string &shaderName,
              const Model &model,
              const glm::mat4 &globalMat = glm::mat4(1.0f)) const
    {
        auto &shader = shaders_.at(shaderName);
        auto ID = shader.getID();
        shader.use();
        shader.setMat4("model", globalMat);
        shader.setMat4("view", glm::lookAt(eye, eye + front, up));
        shader.setMat4("projection", glm::perspective(glm::radians(fovy), aspect, nearLimit, farLimit));
        for (auto &mesh : model.getMeshes())
            mesh.draw(ID);
    }
    void draw(const std::string &shaderName,
              const std::string &deliverName,
              const Animator &animator,
              const glm::mat4 &globalMat = glm::mat4(1.0f)) const
    {
        auto &shader = shaders_.at(shaderName);
        auto &deliver = delivers_.at(deliverName);
        auto ID = shader.getID();
        shader.use();
        shader.setMat4("model", globalMat);
        shader.setMat4("view", glm::lookAt(eye, eye + front, up));
        shader.setMat4("projection", glm::perspective(glm::radians(fovy), aspect, nearLimit, farLimit));
        deliver.deliverTransforms(ID);
        for (auto &mesh : animator.getMeshes())
            mesh.draw(ID);
    }
    void showNpoll() const
    {
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
    inline glm::mat4 getGlobalMat() const
    {
        return glm::mat4(
            glm::vec4(right, 0.0f),
            glm::vec4(up, 0.0f),
            glm::vec4(front, 0.0f),
            glm::vec4(eye.x, eye.y, eye.z, 1.0f));
    }

private:
    void processPosMove_subduct(Mapping_bitset direction)
    {
        float rate = moveSensitivity * deltaTime;
        switch (direction)
        {
        case Mapping_bitset::FORWARD:
            eye += front * rate;
            break;
        case Mapping_bitset::BACKWARD:
            eye -= front * rate;
            break;
        case Mapping_bitset::LEFT:
            eye -= right * rate;
            break;
        case Mapping_bitset::RIGHT:
            eye += right * rate;
            break;
        case Mapping_bitset::UP:
            eye += up * rate;
            break;
        case Mapping_bitset::DOWN:
            eye -= up * rate;
            break;
        default:
            break;
        }
    }
    void processPosMove(Mapping_bitset direction)
    {
        float rate = moveSensitivity * deltaTime;
        glm::vec3 final(0.0f, 0.0f, 0.0f);
        switch (direction)
        {
        case Mapping_bitset::FORWARD:
            final.x = front.x;
            final.z = front.z;
            eye += final * rate;
            break;
        case Mapping_bitset::BACKWARD:
            final.x = front.x;
            final.z = front.z;
            eye -= final * rate;
            break;
        case Mapping_bitset::LEFT:
            final.x = right.x;
            final.z = right.z;
            eye -= final * rate;
            break;
        case Mapping_bitset::RIGHT:
            final.x = right.x;
            final.z = right.z;
            eye += final * rate;
            break;
        case Mapping_bitset::UP:
            final.y = 1.0f;
            eye += final * rate;
            break;
        case Mapping_bitset::DOWN:
            final.y = 1.0f;
            eye -= final * rate;
            break;
        default:
            break;
        }
    }
};
float Engine::moveSensitivity = MOVE_SENSITIVITY;
float Engine::viewSensitivity = VIEW_SENSITIVITY;
float Engine::zoomSensitivity = ZOOM_SENSITIVITY;
float Engine::cursorX = 0.0f;
float Engine::cursorY = 0.0f;
float Engine::yaw = -90.0f;
float Engine::pitch = 0.0f;
float Engine::fovy = VIEW_FOV_VERTICAL;
float Engine::aspect = VIEW_ASPECT_RATE;
float Engine::nearLimit = VIEW_NEAR_LIMIT;
float Engine::farLimit = VIEW_FAR_LIMIT;
void *Engine::world = nullptr;
double Engine::lastTime = 0.0;
double Engine::deltaTime = 0.0;
glm::vec3 Engine::front(0.0f, 0.0f, -1.0f);
glm::vec3 Engine::up(0.0f, 1.0f, 0.0f);
glm::vec3 Engine::right(1.0f, 0.0f, 0.0f);
glm::vec3 Engine::eye(EYE_X, EYE_HEIGHT, EYE_Y);
glm::vec3 Engine::worldUp(0.0f, 1.0f, 0.0f);
std::bitset<(int)Mapping_bitset::count> Engine::keyMapping{};
#endif

#endif
