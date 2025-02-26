#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Lunchbox/core/third-party/tiny_gltf.h>
#include <iostream>

bool load_model(tinygltf::Model &model, const std::string &filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cout << "Error: " << err << std::endl;
    }
    return res;
}

int main() {
    tinygltf::Model model;
    if (load_model(model, "assets/models/cube.glb")) {
        std::cout << "Model loaded successfully!" << std::endl;
    } else {
        std::cout << "Failed to load model." << std::endl;
    }
    return 0;
}