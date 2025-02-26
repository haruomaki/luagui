#include <Lunchbox/Storage.hpp>
#include <iostream>

int main() {
    lunchbox::Storage storage;
    auto model = storage.get_model("models/cube.glb");

    for (size_t i = 0; auto v : gltf::vertex_data(model)) {
        std::cout << "Vertexv " << i++ << ": " << v.x << " "
                  << v.y << " "
                  << v.z << std::endl;
    }
    gltf::uv_data(model);
    gltf::index_data(model);
}