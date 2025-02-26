#include <Lunchbox/core/glTF.hpp>
#include <iostream>

int main() {
    tinygltf::Model model;
    if (gltf::load_model(model, "assets/models/cube.glb")) {
        std::cout << "Model loaded successfully!" << std::endl;
    } else {
        std::cout << "Failed to load model." << std::endl;
    }

    for (size_t i = 0; auto v : gltf::vertex_data(model)) {
        std::cout << "Vertexv " << i++ << ": " << v.x << " "
                  << v.y << " "
                  << v.z << std::endl;
    }
    gltf::uv_data(model);
    gltf::index_data(model);
}