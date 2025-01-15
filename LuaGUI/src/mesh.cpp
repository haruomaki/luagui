#include "mesh.hpp"
#include <GUI/GUI.hpp>
#include <GUI/Mesh.hpp>
#include <GUI/WorldObject.hpp>
#include <GUI/image.hpp>
#include <Lunchbox/Storage.hpp>
#include <SumiGL/Texture.hpp>
#include <utility>

static Mesh *new_mesh(const sol::state &lua, std::vector<std::vector<float>> &&coords, std::vector<std::vector<float>> &&uvs) {
    GUI &gui = lua["__GUI"];
    auto &mesh = gui.resources.append<Mesh>().get();

    std::vector<glm::vec3> coords_v(coords.size());
    for (size_t i = 0; i < coords.size(); i++) {
        coords_v[i][0] = coords[i].at(0);
        coords_v[i][1] = coords[i].at(1);
        coords_v[i][2] = coords[i].at(2);
    }
    mesh.vertices.setCoords(coords_v);

    std::vector<glm::vec2> uvs_v(uvs.size());
    for (size_t i = 0; i < uvs.size(); i++) {
        uvs_v[i][0] = uvs[i].at(0);
        uvs_v[i][1] = uvs[i].at(1);
    }
    mesh.vertices.set_uvs(uvs_v);

    mesh.draw_mode = GL_TRIANGLE_FAN;

    return &mesh;
}

void register_mesh(sol::state &lua) {
    lua.new_usertype<Image>(
        "Image",
        "width", &Image::width,
        "height", &Image::height,
        "channels", &Image::channels,
        "load", [&lua](const std::string &file_path) -> Image * {
            GUI &gui = lua["__GUI"];
            lunchbox::Storage &storage = lua["__Storage"];
            auto img_data = storage.get_image(file_path);
            return &gui.resources.append<Image>(std::move(img_data)).get();
        },
        sol::base_classes, sol::bases<Resource>());

    lua.new_usertype<Material>(
        "Material",
        "write_depth", &Material::write_depth,
        "from_image", [&lua](Image &img) -> Material * {
            GUI &gui = lua["__GUI"];
            auto texture = GL::create_texture(img.width, img.height, img.channels, img.pixels.get());
            Material &material = MaterialBuilder().texture(std::move(texture)).build(gui);
            return &material;
        },
        sol::base_classes, sol::bases<Resource>());

    lua.new_usertype<Mesh>(
        "Mesh",
        "new", [&lua]() { return new_mesh(lua, {}, {}); },
        "new", [&lua](std::vector<std::vector<float>> coords, std::vector<std::vector<float>> uvs) { return new_mesh(lua, std::move(coords), std::move(uvs)); },
        sol::base_classes, sol::bases<Resource>());

    lua["WorldObject"]["add_mesh_component"] = [](WorldObject &parent, Material *material, Mesh *mesh) -> MeshComponent * {
        return &parent.add_component<MeshComponent>(*mesh, material);
    };
}
