#include "Window.hpp"
#include "Camera.hpp"
#include "GUI.hpp"
#include <SumiGL/Context.hpp>

using namespace std::chrono_literals;

Window::Window(GUI &gui, int width, int height, const std::string &title)
    : GL::Window(gui, width, height, title)
    , gui_(&gui) {

    this->GL::Window::routine = [this] { this->routine(); };

    // ユーザー領域にこのインスタンスを設定。
    GL::set_user_pointer(gwin(), this);

    // ブレンド（透明処理）の設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 深度テストを有効化
    glEnable(GL_DEPTH_TEST);

    // 押した/離した瞬間を記録するためのキーコールバック。key_down()/keu_up()に必要
    glfwSetKeyCallback(gwin(), [](GLFWwindow *gwin, int key, int scancode, int action, int mods) {
        auto *window = GL::get_user_pointer<Window>(gwin);
        try {
            if (action == GLFW_PRESS) {
                // debug(key, scancode, mods);
                window->key_down_.at(key) = true;
            } else if (action == GLFW_RELEASE) {
                window->key_up_.at(key) = true;
            }
        } catch (const std::out_of_range & /*e*/) {
            // Fnキーや無変換キーなどはキーコードが無い
            info("例外;未知のキーコードです。key=", key, ", scancode=", scancode, ", mods=", mods);
        }
    });

    gui.windows.request_set(this);
}

// FIXME: ムーブ可能にする。
// つまり、デストラクタで終了処理ではなく、destroy()関数で処理する。
// nullウィンドウならデストラクタでは何もしない。
Window::~Window() {
    print("Windowのデストラクタです");
    gui().windows.request_erase(this);

    // カメラとの接続を切る（NOTE: 越権行為かも）
    gui().cameras.flush();
    for (auto *camera : gui().cameras) {
        if (camera->window == this) camera->window = nullptr;
    }

    destroy();
}

Window::Window(Window &&other) noexcept { *this = std::move(other); }
Window &Window::operator=(Window &&other) noexcept {
    GL::Window::operator=(std::move(other));
    gui_ = other.gui_;
    other.gui_ = nullptr; // ムーブ元のインスタンスを無効化
    key_down_ = other.key_down_;
    key_up_ = other.key_up_;
    last_cursor_ = other.last_cursor_;
    return *this;
}

void Window::close() {
    glfwSetWindowShouldClose(gwin(), GLFW_TRUE);
}

// void Window::routine() {
//     if (glfwWindowShouldClose(gwin)) {
//         this->destroy();
//         return;
//     }

//     // 更新処理。physicsとupdateは順不同？
//     trace("[mainloop] p1 《physics》->update->draw->post");
//     // this->physics_routine();
//     trace("[mainloop] p2 physics->《update》->draw->post");
//     this->update_routine();

//     // 更新処理ののち描画。
//     trace("[mainloop] p3 physics->update->《draw》->post");
//     this->draw_routine();
//     trace("[mainloop] p4 physics->update->draw->《post》");
//     this->post_process();
//     trace("[mainloop] p5");
// }

void Window::update_routine() {
    // // WorldObjectの更新 TODO: 一フレームごとに更新 vs setPosition()ごとに更新（重いかも）
    // world_object_root_.refreshAbsolutePosition();

    // フレームバッファサイズを更新
    fbsize_cache = framebuffer_size();
}

void Window::post_process() {
    // 今フレームのキーイベント発生状況を0にリセットする
    key_down_.fill(false);
    key_up_.fill(false);

    // カーソル位置を更新
    last_cursor_ = cursor_pos();
}
