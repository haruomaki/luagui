#include <Lunchbox/core/glTF.hpp>

int main() {
    tinygltf::Model model;
    if (lunchbox::core::load_model(model, "assets/models/cube.glb")) {
        std::cout << "Model loaded successfully!" << std::endl;
    } else {
        std::cout << "Failed to load model." << std::endl;
    }

    lunchbox::core::print_vertex_data(model);
    lunchbox::core::print_uv_data(model);
    lunchbox::core::print_index_data(model);
}