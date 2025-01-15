#include "mesh.hpp"
#include "vec.hpp"
#include <GUI/GUI.hpp>
#include <GUI/Mesh.hpp>
#include <GUI/WorldObject.hpp>
#include <GUI/image.hpp>
#include <Lunchbox/Storage.hpp>
#include <SumiGL/Texture.hpp>
#include <utility>

static Mesh *new_mesh(const sol::state &lua, V3 &&coords, V2 &&uvs) {
    GUI &gui = lua["__GUI"];
    auto &mesh = gui.resources.append<Mesh>().get();

    mesh.vertices.setCoords(coords);
    mesh.vertices.set_uvs(uvs);

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
        "new", [&lua]() -> Material * {GUI &gui = lua["__GUI"]; return &MaterialBuilder().build(gui); },
        "write_depth", &Material::write_depth,
        "from_image", [&lua](Image &img) -> Material * {
            GUI &gui = lua["__GUI"];
            auto texture = GL::create_texture(img.width, img.height, img.channels, img.pixels.get());
            Material &material = MaterialBuilder().texture(std::move(texture)).build(gui);
            return &material; },
        sol::base_classes, sol::bases<Resource>());

    lua.new_usertype<Mesh>(
        "Mesh",
        "new", sol::overload([&lua]() -> Mesh * { GUI &gui = lua["__GUI"]; return &gui.resources.append<Mesh>().get(); }, [&lua](V3 coords, V2 uvs) { return new_mesh(lua, std::move(coords), std::move(uvs)); }),
        "coords", sol::property([](Mesh *m) { return m->vertices.getCoords(); }, [](Mesh *m, const V3 &c) { m->vertices.setCoords(c); }),
        "colors", sol::property([](Mesh *m) { return m->vertices.getColors(); }, [](Mesh *m, const CV &c) { m->vertices.setColors(c); }),
        "uvs", sol::property([](Mesh *m) { return m->vertices.get_uvs(); }, [](Mesh *m, const V2 &u) { m->vertices.set_uvs(u); }),
        sol::base_classes, sol::bases<Resource>());

    lua["WorldObject"]["add_mesh_component"] = [](WorldObject &parent, Material *material, Mesh *mesh) -> MeshComponent * {
        return &parent.add_component<MeshComponent>(*mesh, material);
    };
}
