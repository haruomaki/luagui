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

void print_vertex_data(const tinygltf::Model &model) {
    for (const auto &mesh : model.meshes) {
        for (const auto &primitive : mesh.primitives) {
            const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView &buffer_view = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer &buffer = model.buffers[buffer_view.buffer];

            const auto *positions = reinterpret_cast<const float *>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
            for (size_t i = 0; i < accessor.count; ++i) {
                std::cout << "Vertex " << i << ": ("
                          << positions[i * 3 + 0] << ", "
                          << positions[i * 3 + 1] << ", "
                          << positions[i * 3 + 2] << ")" << std::endl;
            }
        }
    }
}

void print_uv_data(const tinygltf::Model &model) {
    for (const auto &mesh : model.meshes) {
        for (const auto &primitive : mesh.primitives) {
            const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
            const tinygltf::BufferView &buffer_view = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer &buffer = model.buffers[buffer_view.buffer];

            const auto *uvs = reinterpret_cast<const float *>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
            for (size_t i = 0; i < accessor.count; ++i) {
                std::cout << "UV " << i << ": ("
                          << uvs[i * 2 + 0] << ", "
                          << uvs[i * 2 + 1] << ")" << std::endl;
            }
        }
    }
}

void print_index_data(const tinygltf::Model &model) {
    for (const auto &mesh : model.meshes) {
        for (const auto &primitive : mesh.primitives) {
            const tinygltf::Accessor &accessor = model.accessors[primitive.indices];
            const tinygltf::BufferView &buffer_view = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer &buffer = model.buffers[buffer_view.buffer];

            const auto *indices = reinterpret_cast<const unsigned short *>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);
            for (size_t i = 0; i < accessor.count; ++i) {
                std::cout << "Index " << i << ": " << indices[i] << std::endl;
            }
        }
    }
}

int main() {
    tinygltf::Model model;
    if (load_model(model, "assets/models/cube.glb")) {
        std::cout << "Model loaded successfully!" << std::endl;
    } else {
        std::cout << "Failed to load model." << std::endl;
    }

    print_vertex_data(model);
    print_uv_data(model);
    print_index_data(model);
}