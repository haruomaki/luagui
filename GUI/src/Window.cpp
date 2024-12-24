#include "Window.hpp"
#include "GUI.hpp"
#include "World.hpp"
#include <SumiGL/Context.hpp>

using namespace std::chrono_literals;

Window::Window(GUI &gui, int width, int height, const char *title)
    : GL::Window(gui.ctx(), width, height, title, [this] { this->routine(); })
    , gwin_(gwin)
    , gui(gui) {

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
}

Window::~Window() {
    this->worlds_.clear(); // key_callbacksが消える前にKeyCallbackObjectが消えないといけない

    print("Windowのデストラクタです");
    // debug(this->size_callbacks_.size());
    // this->size_callbacks_.erase(this->size_callbacks_.begin());
    // debug(this->size_callbacks_.size());
    // print("消しました");

    // glfwSetWindowSizeCallback(gwin_, nullptr);
    // glfwSetKeyCallback(gwin_, nullptr);
}

void Window::close() const {
    glfwSetWindowShouldClose(gwin_, GL_TRUE);
}

World &Window::create_world() {
    auto world = std::make_unique<World>(*this);
    this->worlds_.push_back(std::move(world));

    return *this->worlds_.back();
}

void Window::routine() {
    if (glfwWindowShouldClose(gwin)) {
        this->destroy();
        return;
    }

    // 更新処理。physicsとupdateは順不同？
    trace("[mainloop] p1 《physics》->update->draw->post");
    this->physics_routine();
    trace("[mainloop] p2 physics->《update》->draw->post");
    this->update_routine();

    // 更新処理ののち描画。
    trace("[mainloop] p3 physics->update->《draw》->post");
    this->draw_routine();
    trace("[mainloop] p4 physics->update->draw->《post》");
    this->post_process();
    trace("[mainloop] p5");
}

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

    // カスタムの描画ルーチンを実行
    for (const auto &world : this->raw_worlds) {
        glClear(GL_DEPTH_BUFFER_BIT);
        world();
    }

    // 上記描画した図形を表画面のバッファにスワップする
    glfwSwapBuffers(this->gwin_);
}

void Window::update_routine() {
    // // WorldObjectの更新 TODO: 一フレームごとに更新 vs setPosition()ごとに更新（重いかも）
    // world_object_root_.refreshAbsolutePosition();

    // フレームバッファサイズを更新
    fbsize_cache = framebuffer_size();

    // 各ワールドの更新処理
    trace("[update] resource->《world》");
    for (const auto &world : this->worlds_) {
        world->master_update();
    }
}

void Window::physics_routine() {
    // 各ワールドの物理演算処理
    for (const auto &world : this->worlds_) {
        world->master_physics();
    }
}

void Window::post_process() {
    // 今フレームのキーイベント発生状況を0にリセットする
    key_down_.fill(false);
    key_up_.fill(false);

    // カーソル位置を更新
    last_cursor_ = cursor_pos();

    // rigidbody_components_（物理演算結果を物体の位置に反映するために管理）をフラッシュしておく。
    // 次フレームに同一アドレスの別コンポーネントが作られたときに、競合しないようにするため。
    for (std::unique_ptr<World> &world : this->worlds_) {
        world->rigidbody_components.flush();

        // 各種フラッシュ TODO: 場所はここでいい？
        world->draws.flush();
        world->updates.flush();
        world->rigidbodies.flush();
    }
}
