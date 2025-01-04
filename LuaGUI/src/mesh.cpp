#include "mesh.hpp"
#include <GUI/GUI.hpp>
#include <GUI/Mesh.hpp>
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
        auto &mesh = gui.resources.append<Mesh>().get();

        const int segments = 12;
        std::vector<glm::vec3> coords(segments + 1);

        float radius = 0.03;
        for (int i = 0; i <= segments; ++i) {
            float theta = 2 * M_PIf * float(i) / float(segments); // 角度を計算
            float x = radius * cosf(theta);                       // x座標
            float y = radius * sinf(theta);                       // y座標
            coords[i] = glm::vec3(x, y, 0);
        }

        mesh.vertices.setCoords(coords);

        return &mesh;
    });

    lua.new_usertype<Material>("Material");
    lua.new_usertype<Mesh>("Mesh");

    lua["WorldObject"]["add_mesh_component"] = [&lua](WorldObject &parent, Material *material, Mesh *mesh) -> MeshComponent * {
        return &parent.child_component<MeshComponent>(*mesh, material);
    };
}
