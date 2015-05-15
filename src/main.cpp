#include <glad/glad.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#include <OpenCL/cl_gl.h>
#include <OpenGL/OpenGL.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#elif defined __linux__
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <CL/cl_platform.h>
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include <tuple>
#include <algorithm>

#include "yaml-cpp/yaml.h"
#include "cl.hpp"
#include "Tracer.hpp"
#include "Scene.hpp"
#include "Drawer.hpp"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

void glfw_error(int error, const char* description)
{
    std::cerr << "Error " << error << ": " << description << std::endl;
}


void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void window_resize_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

std::tuple<cl::Context, cl::Device, cl::CommandQueue> init_cl(const std::string& device_name, GLFWwindow* window) 
{
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);

    for (auto & platform : all_platforms) {
        std::cout << platform.getInfo<CL_PLATFORM_NAME>()
                  << std::endl;
    }

    cl::Platform default_platform = all_platforms[0];
    std::cout << "Using platform: "
              << default_platform.getInfo<CL_PLATFORM_NAME>()
              << std::endl;

    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

    cl::Device device;
    for (auto & dev : all_devices) {
        const std::string name = dev.getInfo<CL_DEVICE_NAME>();
        std::cout << name
                  << std::endl;
        if (name.find(device_name) != std::string::npos) {
            device = dev;
        }
    }

    std::cout << "Using device: "
              << device.getInfo<CL_DEVICE_NAME>()
              << std::endl;

    std::cout << "Extensions: "
              << device.getInfo<CL_DEVICE_EXTENSIONS>()
              << std::endl;

    cl_context_properties properties[] {
#ifdef __APPLE__
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
        (cl_context_properties) CGLGetShareGroup(CGLGetCurrentContext()),
#elif defined __linux__
        CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetGLXContext(window),
        CL_GLX_DISPLAY_KHR, (cl_context_properties)glfwGetX11Display(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)(all_platforms[0])(),
#endif
        0
    };

#ifdef __APPLE__
    cl::Context context (device, properties, clLogMessagesToStdoutAPPLE);
#elif defined __linux__
    cl::Context context (device, properties, &contextCallback);
#endif
    cl::CommandQueue queue = cl::CommandQueue(context, device);

    return std::make_tuple(context, device, queue);
}

GLFWwindow* init_gl(int width, int height)
{
    glfwSetErrorCallback(glfw_error);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "tracer", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    gladLoadGL();

    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_resize_callback);
    window_resize_callback(window, width, height);
    return window;
}

int main()
{
    YAML::Node config = YAML::LoadFile("../config.yaml");

    int width = config["width"].as<int>();
    int height = config["height"].as<int>();

    auto window = init_gl(width, height);

    std::string device_name = config["compute_device"].as<std::string>();

    cl::Context context;
    cl::Device device;
    cl::CommandQueue queue;
    std::tie(context, device, queue) = init_cl(device_name, window);
    Tracer tracer(context, device, queue);

    Drawer drawer(width, height);

    auto scene_file = config["scene"].as<std::string>();

    auto scene = Scene::load("../scenes/" + scene_file, context, device, queue);
    tracer.load_kernels();
    tracer.set_scene(scene);
    tracer.set_texture(drawer.texture(), width, height);

    while (!glfwWindowShouldClose(window)) {
        scene.update();
        tracer.trace();
        drawer.display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
