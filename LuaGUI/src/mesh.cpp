#include "mesh.hpp"
#include "Lunchbox/core/glTF.hpp"
#include "SumiGL/Window.hpp"
#include "SumiGL/constants.hpp"
#include "vec.hpp"
#include <GUI/GUI.hpp>
#include <GUI/Mesh.hpp>
#include <GUI/WorldObject.hpp>
#include <GUI/image.hpp>
#include <Lunchbox/Storage.hpp>
#include <SumiGL/Texture.hpp>
#include <utility>

using DrawModeMap = std::unordered_map<std::string, GLenum>;

static const DrawModeMap &draw_mode_map() {
    static const auto *m = new DrawModeMap{
        {"points", GL_POINTS},
        {"lines", GL_LINES},
        {"line_strip", GL_LINE_STRIP},
        {"line_loop", GL_LINE_LOOP},
        {"triangles", GL_TRIANGLES},
        {"triangle_strip", GL_TRIANGLE_STRIP},
        {"triangle_fan", GL_TRIANGLE_FAN},
        {"quads", GL_QUADS},
        {"quad_strip", GL_QUAD_STRIP},
        {"polygon", GL_POLYGON}};
    return *m;
}

static std::string mode_to_string(GLenum mode) {
    for (const auto &[s, m] : draw_mode_map()) {
        if (mode == m) return s;
    }
    warn("未知のドローモード番号です。");
    return "";
}

static GLenum string_to_mode(const std::string &s) {
    return draw_mode_map().at(s);
}

static Mesh *new_mesh(const sol::state &lua, const V3 &coords, const V2 &uvs) {
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
        "both", &Material::both,
        "line_width", &Material::line_width,
        "point_size", &Material::point_size,
        "from_image", [&lua](Image &img) -> Material * {
            GUI &gui = lua["__GUI"];
            auto texture = GL::create_texture(img.width, img.height, img.channels, img.pixels.get());
            Material &material = MaterialBuilder().texture(std::move(texture)).build(gui);
            return &material; },
        "load", [&lua](const std::string &file_path) -> Material * {
            GUI &gui = lua["__GUI"];
            lunchbox::Storage &storage = lua["__Storage"];
            auto model = storage.get_model(file_path);
            auto img = gltf::textures(model);
            auto texture = GL::create_texture(img.width, img.height, img.channels, img.pixels.get());
            Material &material = MaterialBuilder().texture(std::move(texture)).build(gui);
            return &material; },
        sol::base_classes, sol::bases<Resource>());

    lua.new_usertype<Mesh>(
        "Mesh",
        "new", sol::overload([&lua]() -> Mesh * { GUI &gui = lua["__GUI"]; return &gui.resources.append<Mesh>().get(); }, [&lua](const V3 &coords, const V2 &uvs) { return new_mesh(lua, coords, uvs); }),
        "indices", sol::property([](Mesh *m) { return VI(m->indices); }, [](Mesh *m, const VI &i) { m->indices = i; }),
        "coords", sol::property([](Mesh *m) { return V3(m->vertices.getCoords()); }, [](Mesh *m, const V3 &c) { m->vertices.setCoords(c); }),
        "colors", sol::property([](Mesh *m) { return CV(m->vertices.getColors()); }, [](Mesh *m, const CV &c) { m->vertices.setColors(c); }),
        "uvs", sol::property([](Mesh *m) { return V2(m->vertices.get_uvs()); }, [](Mesh *m, const V2 &u) { m->vertices.set_uvs(u); }),
        "use_index", &Mesh::use_index,
        "draw_mode", sol::property([](Mesh *m) { return mode_to_string(m->draw_mode); }, [](Mesh *m, const std::string &s) { m->draw_mode = string_to_mode(s); }),
        "load", [&lua](const std::string &file_path) -> Mesh * {
            GUI &gui = lua["__GUI"];
            lunchbox::Storage &storage = lua["__Storage"];
            auto model = storage.get_model(file_path);
            auto &mesh = gui.resources.append<Mesh>().get();
            mesh.vertices.setCoords(gltf::vertex_data(model));
            mesh.vertices.set_uvs(gltf::uv_data(model));
            mesh.indices = gltf::index_data(model);
            mesh.use_index = true;
            mesh.draw_mode = GL_TRIANGLES;
            return &mesh; },
        "from_image", [&lua](const std::string &file_path) -> Mesh * {
            GUI &gui = lua["__GUI"];
            lunchbox::Storage &storage = lua["__Storage"];
            auto image = storage.get_image(file_path);
            auto &mesh = gui.resources.append<Mesh>().get();
            const float w = float(image.width) * px_meter;
            const float h = float(image.height) * px_meter;
            mesh.vertices.setCoords({{-w, -h, 0}, {w, -h, 0}, {w, h, 0}, {-w, h, 0}});
            mesh.vertices.set_uvs({{0, 1}, {1, 1}, {1, 0}, {0, 0}});
            mesh.draw_mode = GL_TRIANGLE_FAN;
            return &mesh; },
        sol::base_classes, sol::bases<Resource>());

    lua["WorldObject"]["add_mesh_component"] = [](WorldObject &parent, Material *material, Mesh *mesh) -> MeshComponent * {
        return &parent.add_component<MeshComponent>(*mesh, material);
    };
}
