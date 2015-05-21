#include "Scene.hpp"
#include "Meshloader.hpp"

#include "GLFW/glfw3.h"
#include "yaml-cpp/yaml.h"

#include "lodepng.h"

#include <cmath>

Scene::Scene(cl::Context context, cl::Device device, cl::CommandQueue queue)
    : context(context)
    , device(device)
    , queue(queue)
{
}

void Scene::update()
{
    float x = fmod((glfwGetTime() * 0.7), 2 * M_PI);
    //spheres[0].center.s[0] = 0.f - std::sin(x - M_PI) * 30;
    //spheres[0].center.s[1] = 0.0f - std::sin(x - M_PI / 2) * 30;
    //spheres[1].center.s[0] = 0.f - std::sin(x - M_PI) * 40;
    ////cl::copy(spheres.begin(), spheres.end(), spheresBuffer);
    //queue.enqueueWriteBuffer(spheresBuffer, CL_TRUE, 0,
    //                         sizeof(Sphere) * spheres.size(), spheres.data());

    lights[0].location.x = std::cos(x - M_PI / 2) * 25;
    //lights[0].location.y = std::sin(x - M_PI) * 22;
    lights[0].location.z = -90.f - std::cos(x - M_PI) * 25;
    //cl::copy(lights.begin(), lights.end(), lightsBuffer);
    queue.enqueueWriteBuffer(lightsBuffer, CL_TRUE, 0,
                             sizeof(Light) * lights.size(), lights.data());
}

Scene Scene::load(const std::string & filename, cl::Context context, cl::Device device, cl::CommandQueue queue)
{
    YAML::Node scene_file = YAML::LoadFile(filename);

    Scene scene(context, device, queue);
    scene.lights = scene_file["lights"].as<std::vector<Light>>();

    for(auto n : scene_file["materials"]) {
        Material mat;
        auto diffuse = load_texture("../textures/" + n["diffuse"].as<std::string>());
        scene.texture_array.insert(scene.texture_array.end(),
                                   diffuse.begin(),
                                   diffuse.end());
        mat.diffuse = scene.materials.size();
        mat.fresnel0 = n["fresnel0"].as<float>();
        mat.roughness = n["roughness"].as<float>();
        scene.materials.push_back(mat);
    }

    for(auto n : scene_file["meshes"]) {
        Mesh mesh = load_mesh(n["file"].as<std::string>());
        CLMesh clmesh;
        clmesh.num_triangles = mesh.indices.size();
        clmesh.material = n["material"].as<cl_int>();
        clmesh.position = n["position"].as<cl_float3>();
        clmesh.scale = n["scale"].as<cl_float3>();
        clmesh.orientation = n["orientation"].as<cl_float4>();
        clmesh.base_vertex = scene.vertices.size();
        clmesh.base_triangle = scene.indices.size();

        BVHNode bvhnode;
        bvhnode.bounds = mesh.bounds;
        bvhnode.mesh = scene.clmeshes.size();
        bvhnode.scale = clmesh.scale;
        bvhnode.position = clmesh.position;

        scene.vertices.insert(scene.vertices.end(), 
                              mesh.vertices.begin(), 
                              mesh.vertices.end());
        scene.vertexAttributes.insert(scene.vertexAttributes.end(),
                                      mesh.vertexAttributes.begin(),
                                      mesh.vertexAttributes.end());
        scene.indices.insert(scene.indices.end(), 
                               mesh.indices.begin(), 
                               mesh.indices.end());
        scene.clmeshes.push_back(clmesh);
        scene.bvh.push_back(bvhnode);
    }

    scene.lightsBuffer = cl::Buffer(context, scene.lights.begin(), 
                                    scene.lights.end(), true);
    scene.materialsBuffer = cl::Buffer(context, scene.materials.begin(), 
                                       scene.materials.end(), true);
    auto format = cl::ImageFormat(CL_RGBA, CL_UNSIGNED_INT8);
    scene.texturesBuffer = cl::Image2DArray(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                            format, (size_t)scene_file["materials"].size(),
                                            512, 512, 0, 0, scene.texture_array.data());

    scene.vertexBuffer = cl::Buffer(context, scene.vertices.begin(),
                                    scene.vertices.end(), true);
    scene.vertexAttributesBuffer = cl::Buffer(context, scene.vertexAttributes.begin(),
                                              scene.vertexAttributes.end(), true);
    scene.indicesBuffer = cl::Buffer(context, scene.indices.begin(),
                                     scene.indices.end(), true);
    scene.meshesBuffer = cl::Buffer(context, scene.clmeshes.begin(),
                                    scene.clmeshes.end(), true);
    scene.bvhBuffer = cl::Buffer(context, scene.bvh.begin(),
                                 scene.bvh.end(), true);
    return scene;
}

std::vector<unsigned char> load_texture(const std::string & filename)
{
    std::vector<unsigned char> pixels;
    unsigned int width, height;
    lodepng::decode(pixels, width, height, filename.c_str());
    return pixels;
}

namespace YAML {
    template<>
    struct convert<cl_float4> {
        static bool decode(const Node& node, cl_float4& f4) {
            if (node.size() == 3) {
                f4.x = node[0].as<cl_float>();
                f4.y = node[1].as<cl_float>();
                f4.z = node[2].as<cl_float>();
                return true;
            } else if (node.size() == 4) {
                f4.x = node[0].as<cl_float>();
                f4.y = node[1].as<cl_float>();
                f4.z = node[2].as<cl_float>();
                f4.w = node[3].as<cl_float>();
                return true;
            } else {
                return false;
            }
        }
    };

    template<>
    struct convert<Light> {
        static bool decode(const Node& node, Light& light) {
            if (node.size() != 3) {
                return false;
            }

            light.color = node["color"].as<cl_float3>();
            light.location = node["location"].as<cl_float3>();
            light.radius = node["radius"].as<cl_float>();
            return true;
        }
    };
}
