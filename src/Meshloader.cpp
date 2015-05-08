#include "Meshloader.hpp"

#include <boost/iostreams/device/mapped_file.hpp>
#include <algorithm>

#include "iqm.h"

std::ostream& operator<<(std::ostream& strm, const Vertex& v)
{
    return strm << "("
                << v.v.s[0] << ", "
                << v.v.s[1] << ", "
                << v.v.s[2] << ")";

}

std::ostream& operator<<(std::ostream& strm, const Triangle& t)
{
    return strm << "("
                << t.i.s[0] << ", "
                << t.i.s[1] << ", "
                << t.i.s[2] << ")";
}

Mesh load_mesh(std::string filename)
{
    using namespace boost::iostreams;

    mapped_file_source mesh_file(filename);

    const iqmheader* ih = reinterpret_cast<const iqmheader*>(mesh_file.data());

    const iqmvertexarray* iva = reinterpret_cast<const iqmvertexarray*>(mesh_file.data() + ih->ofs_vertexarrays);

    Mesh mesh;
    mesh.positions.reserve(ih->num_vertexes);
    mesh.triangles.reserve(ih->num_triangles);

    const vt* vertices = reinterpret_cast<const vt*>(mesh_file.data() + iva->offset);
    for(unsigned int i = 0; i < ih->num_vertexes; i++) {
        auto v = vertices[i];
        mesh.positions.emplace_back(v.a);
    }

    std::cout << "vertices: " << std::endl;
    for (auto & p : mesh.positions) {
        std::cout << p << std::endl;
    }

    const id* triangles = reinterpret_cast<const id*>(mesh_file.data() + ih->ofs_triangles);
    for(unsigned int i = 0; i < ih->num_triangles; i++) {
        auto t = triangles[i];
        mesh.triangles.emplace_back(t.a);
    }

    std::cout << std::endl << "indices: " << std::endl;
    for (auto & t : mesh.triangles) {
        std::cout << t << std::endl;
    }
    std::cout << std::endl;
    return mesh;
}

