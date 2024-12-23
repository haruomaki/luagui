#pragma once

#include <SumiGL/Context.hpp>
#include <SumiGL/Shader.hpp>
#include <SumiGL/buffered_container.hpp>

class World;
struct GUI;
struct Material;
class Viewport;
struct CameraInterface;

// 一つのウィンドウを表すクラス
class Window : public GL::Window {
    GLFWwindow *gwin_ = nullptr; // GL::Windowのgwinと同じ
    using KeyArray = std::array<bool, 512>;
    KeyArray key_down_{}, key_up_{};
    std::pair<double, double> last_cursor_ = cursor_pos();
    std::vector<std::unique_ptr<World>> worlds_;

    void routine();
    void draw_routine();
    void update_routine();
    void physics_routine();
    void post_process();

    friend class World;
    void refresh_world_order();

  public:
    GUI &gui;
    std::optional<GL::ProgramObject> default_shader;
    Material *default_material = nullptr;
    BufferedSet<std::function<void()> *> resource_updates;
    std::vector<std::function<void()>> raw_worlds;
    BufferedSet<CameraInterface *> cameras;

    // ウィンドウ内描画領域の大きさを表す変数。framebuffer_size()は直接取得する（故に重い）のに対し、ここには毎フレーム自動で取得されたものがキャッシュされている。
    std::pair<int, int> fbsize_cache = {0, 0};

    Window(GUI &gui, int width, int height, const char *title);
    ~Window();

    // コピーもムーブも禁止する
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    void close() const;

    // 前フレームからのカーソルの移動差分を得る。
    [[nodiscard]] std::pair<double, double> cursor_diff() const {
        auto [x, y] = cursor_pos();
        return {x - last_cursor_.first, y - last_cursor_.second};
    }

    // 現在フレームにおける、キー変更イベント（押した）の有無を表す
    [[nodiscard]] const KeyArray &key_down() const { return key_down_; }

    // 現在フレームにおける、キー変更イベント（離した）の有無を表す
    [[nodiscard]] const KeyArray &key_up() const { return key_up_; }

    World &create_world();
};
