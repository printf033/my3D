#include <iostream>
#include <thread>
#include <chrono>
#define ENGINE_GL
#include "engine_gl.hpp"
#include "ground.hpp"

#include "logger.hpp"

int main()
{
    // mylog::Logger::setOutputFunc(mylog::AsyncHelper::outputFunc_async_file);
    // mylog::Logger::setFlushFunc(mylog::AsyncHelper::flushFunc_async_file);

    auto &engine = Engine::get();
    engine.inputFor3D();
    engine.addShader("static",
                     std::filesystem::current_path() / "../opengl/glsl/modl_vs",
                     std::filesystem::current_path() / "../opengl/glsl/modl_fs",
                     std::filesystem::current_path() / "../opengl/glsl/modl_gs");
    engine.addShader("dynamic",
                     std::filesystem::current_path() / "../opengl/glsl/anim_vs",
                     std::filesystem::current_path() / "../opengl/glsl/anim_fs",
                     std::filesystem::current_path() / "../opengl/glsl/anim_gs");
    Ground ground(Animator(Model(std::filesystem::current_path() / "../resources/terrains/boxes/boxes.fbx")));
    Engine::world = &ground;
    ground.addSoul("sphere",
                   Soul(Animator(Model(std::filesystem::current_path() / "../resources/objects/sphere/sphere.fbx"))));
    // ground.addSoul("cube",
    //                Soul(Animator(Model(std::filesystem::current_path() / "../resources/objects/cube/cube.fbx"))));
    // engine.addDeliver("spin", ground.getSoul("cube").myTransforms());
    auto &sphere = ground.getSoul("sphere");
    // auto &cube = ground.getSoul("cube");
    while (engine.isRunning())
    {
        engine.update();
        ////////////////////////////////////////
        engine.draw("static", ground);
        sphere.setView(engine.getGlobalMat());
        engine.draw("static", sphere, sphere.getGlobalMat());
        // cube.updateTransforms(Engine::deltaTime);
        // engine.draw("dynamic", "spin", cube);
        ////////////////////////////////////////
        engine.showNpoll();
    }
    return 0;
}