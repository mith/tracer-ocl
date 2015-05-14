#include "Meshloader.hpp"

#include <boost/iostreams/device/mapped_file.hpp>
#include <algorithm>

#include "iqm.h"

std::ostream& operator<<(std::ostream& strm, const Vertex& v)
{
    return strm << "("
                << v.position.x << ", "
                << v.position.y << ", "
                << v.position.z << ")";

}

std::ostream& operator<<(std::ostream& strm, const Indice& t)
{
    return strm << "("
                << t.i.x << ", "
                << t.i.y << ", "
                << t.i.z << ")";
}

Mesh load_mesh(std::string filename)
{
    using namespace boost::iostreams;

    mapped_file_source mesh_file("../meshes/" + filename);

    const iqmheader* ih = reinterpret_cast<const iqmheader*>(mesh_file.data());

    const iqmvertexarray* vaptr = reinterpret_cast<const iqmvertexarray*>(mesh_file.data() + ih->ofs_vertexarrays);

    const iqmvertexarray* posva = vaptr;
    const iqmvertexarray* uvva = vaptr + 1;
    const iqmvertexarray* normalva = vaptr + 2;

    Mesh mesh;
    mesh.vertices.reserve(ih->num_vertexes);
    mesh.indices.reserve(ih->num_triangles);

    std::array<float, 3> min {{0.0f, 0.0f, 0.0f}};
    std::array<float, 3> max {{0.0f, 0.0f, 0.0f}};

    auto positions = reinterpret_cast<const std::array<float, 3>*>(mesh_file.data() + posva->offset);
    auto normals = reinterpret_cast<const std::array<float, 3>*>(mesh_file.data() + normalva->offset);
    for(unsigned int i = 0; i < ih->num_vertexes; i++) {
        auto pos = positions[i];
        auto nor = normals[i];


        for (int j = 0; j < 3; j++) {
            min[j] = std::min(pos[j], min[j]);
            max[j] = std::max(pos[j], max[j]);
        }

        mesh.vertices.emplace_back(pos, nor);
    }

    auto triangles = reinterpret_cast<const std::array<unsigned int, 3>*>(mesh_file.data() + ih->ofs_triangles);
    for(unsigned int i = 0; i < ih->num_triangles; i++) {
        auto t = triangles[i];
        mesh.indices.emplace_back(t);
    }
    
    mesh.bounds.min = {{min[0], min[1], min[2]}};
    mesh.bounds.max = {{max[0], max[1], max[2]}};

    return mesh;
}

