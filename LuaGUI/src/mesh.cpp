#include "mesh.hpp"
#include <GUI/GUI.hpp>
#include <GUI/Mesh.hpp>
#include <GUI/utility.hpp>
#include <Lunchbox/Storage.hpp>
#include <SumiGL/Texture.hpp>

void register_mesh(sol::state &lua) {
    lua.set_function("new_material", [&lua]() -> Material * {
        GUI &gui = lua["__GUI"];
        lunchbox::Storage &storage = lua["__Storage"];
        auto img = storage.get_image("assets/images/ピンクレンガ.png");
        auto cat_texture = GL::create_texture(img.width, img.height, img.channels, img.pixels.get());
        Material &cat_material = MaterialBuilder().texture(cat_texture.get()).build(gui);
        return &cat_material;
    });

    lua.set_function("new_mesh", [&lua]() -> Mesh * {
        GUI &gui = lua["__GUI"];

        auto &gon = new_mesh(gui, GL_TRIANGLE_FAN, {{0, 0, 0}, {0, 0.01, 0}, {0.01, 0.01, 0}, {0.01, 0, 0}},
                             {
                                 {0.9, 0.3, 0, 1},
                                 {0.1, 0.2, 0.7, 0.3},
                                 {0.9, 0.2, 0.7, 0.3},
                                 {0.3, 0.7, 0.5f, 0.5},
                             },
                             {{1, 0}, {0, 0}, {0, 1}, {1, 1}});
        return &gon;
    });

    lua.new_usertype<Material>("Material");
    lua.new_usertype<Mesh>("Mesh");

    lua["WorldObject"]["add_mesh_component"] = [&lua](WorldObject *obj, Material *material, Mesh *mesh) -> MeshComponent * {
        return &obj->add_component<MeshComponent>(*mesh, material);
    };
}
