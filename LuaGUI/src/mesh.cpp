#include "mesh.hpp"
#include <GUI/GUI.hpp>
#include <GUI/Mesh.hpp>
#include <GUI/image.hpp>
#include <Lunchbox/Storage.hpp>
#include <SumiGL/Texture.hpp>

void register_mesh(sol::state &lua) {
    lua.new_usertype<Image>(
        "Image",
        sol::base_classes, sol::bases<Resource>());

    lua.set_function("load_image", [&lua](const std::string &file_path) -> Image * {
        GUI &gui = lua["__GUI"];
        lunchbox::Storage &storage = lua["__Storage"];
        auto img_data = storage.get_image(file_path);
        return &gui.resources.append<Image>(std::move(img_data)).get();
    });

    lua.set_function("new_material", [&lua](Image &img) -> Material * {
        GUI &gui = lua["__GUI"];
        auto texture = GL::create_texture(img.width, img.height, img.channels, img.pixels.get());
        Material &material = MaterialBuilder().texture(std::move(texture)).build(gui);
        return &material;
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
        mesh.draw_mode = GL_TRIANGLE_FAN;

        return &mesh;
    });

    lua.new_usertype<Material>("Material");
    lua.new_usertype<Mesh>("Mesh");

    lua["WorldObject"]["add_mesh_component"] = [&lua](WorldObject &parent, Material *material, Mesh *mesh) -> MeshComponent * {
        return &parent.child_component<MeshComponent>(*mesh, material);
    };
}
