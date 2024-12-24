#include "Window.hpp"
#include "GUI.hpp"
#include "World.hpp"
#include <SumiGL/Context.hpp>

using namespace std::chrono_literals;

Window::Window(GUI &gui, int width, int height, const char *title)
    : GL::Window(gui, width, height, title)
    , gwin_(gwin)
    , gui(gui) {

    this->GL::Window::routine = [this] { this->routine(); };

    // ブレンド（透明処理）の設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 深度テストを有効化
    glEnable(GL_DEPTH_TEST);

    // 押した/離した瞬間を記録するためのキーコールバック。key_down()/keu_up()に必要
    glfwSetKeyCallback(gwin_, [](GLFWwindow *gwin, int key, int scancode, int action, int mods) {
        auto *window = static_cast<Window *>(GL::Window::user_pointer(gwin));
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

    // FIXME: current_windowが切り替わるのは、現状ウィンドウが作成されたときだけ
    gui.current_window = this;
}

Window::~Window() {
    print("Windowのデストラクタです");
    gui.windows.request_erase(this);
}

void Window::close() const {
    glfwSetWindowShouldClose(gwin_, GL_TRUE);
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

void Window::draw_routine() {
    // OpenGLの描画関数のターゲットにするウィンドウを指定
    glfwMakeContextCurrent(this->gwin_);

    // 画面の初期化
    constexpr RGBA bg_color{0.2f, 0.2f, 0.2f, 1};
    glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // prorityが高いものほど後に描画されるようにソートする
    std::set<std::pair<int, CameraInterface *>> sorted_cameras;
    cameras.flush();
    cameras.foreach ([&](CameraInterface *camera) {
        if (camera->active) sorted_cameras.emplace(camera->priority, camera);
    });
    for (const auto &[priority, camera] : sorted_cameras) {
        trace("[draw_routine] カメラ描画（優先度 ", priority, "）");
        glClear(GL_DEPTH_BUFFER_BIT);
        camera->render();
    }

    // 上記描画した図形を表画面のバッファにスワップする
    glfwSwapBuffers(this->gwin_);
}

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
