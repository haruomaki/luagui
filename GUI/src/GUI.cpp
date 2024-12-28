#include "GUI.hpp"
#include "World.hpp"

GUI::GUI()
    : resources(*this)
    , resume_condition([this] {
        return !this->windows.empty();
    }) {
    // デフォルトシェーダの設定
    GL::ProgramObject pg{GL::create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/default.vsh")),
                         GL::create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/default.fsh"))};
    auto default_shader = this->resources.append<Shader>(std::move(pg));
    default_shader.get().name = "default_shader";

    // デフォルトマテリアルの設定
    auto &default_material = MaterialBuilder().build(*this);
    default_material.name = "default_material";
}

GUI::~GUI() {
    info("GUIのデストラクタ");
    cleanup(); // ワールド本体とGUI（やその子クラス）のメンバが相互依存していたとき、ワールドの方が先に消えることを明示する。
    info("GUIのデストラクタおわり");
}

void GUI::cleanup() {
    this->resources.clear();
    this->worlds_.clear();
}

World &GUI::create_world() {
    auto world = std::make_unique<World>(*this);
    worlds_.emplace_back(std::move(world));

    return *worlds_.back();
}

void GUI::default_routine1() {
    // --------------------
    // 更新
    // --------------------

    // リソースの更新処理 TODO: 場所はどこがいい？
    trace("[update] 《resource》->world");
    resource_updates.flush();
    for (const auto *update : this->resource_updates) {
        (*update)();
    }

    // 各ワールドの更新処理
    trace("[update] resource->《world》");
    for (const auto &world : this->worlds_) {
        world->master_update();
    }

    // 各ワールドの物理演算処理
    for (const auto &world : this->worlds_) {
        world->master_physics();
    }

    // --------------------
    // 描画
    // --------------------

    // prorityが高いものほど後に描画されるようにソートする
    std::map<Window *, std::set<std::pair<int, CameraInterface *>>> sorted_cameras;
    cameras.flush();
    cameras.foreach ([&](CameraInterface *camera) {
        if (camera->active && camera->window) sorted_cameras[camera->window].emplace(camera->priority, camera);
    });

    for (const auto &[window, cam] : sorted_cameras) {
        // OpenGLの描画関数のターゲットにするウィンドウを指定
        glfwMakeContextCurrent(window->gwin());

        // 画面の初期化
        constexpr RGBA bg_color{.r = 0.2f, .g = 0.2f, .b = 0.2f, .a = 1};
        glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (const auto &[priority, camera] : cam) {
            trace("[draw_routine] カメラ描画（優先度 ", priority, "）");
            glClear(GL_DEPTH_BUFFER_BIT);
            camera->render();
        }

        // 上記描画した図形を表画面のバッファにスワップする
        glfwSwapBuffers(window->gwin());
    }

    //--------------------
    // 後処理
    // -------------------

    for (std::unique_ptr<World> &world : this->worlds_) {
        // 各種フラッシュ TODO: 場所はここでいい？
        world->draws.flush();
        world->updates.flush();
        world->rigidbodies.flush();
    }

    // ウィンドウのキー入力などを毎フレーム監視する。
    // キーアップダウンを正確に処理するために、step()とpoll_events()をすぐ隣接させている。
    // ウィンドウの削除処理があるため、ルーチンのできるだけ最後に置いている。
    windows.flush();
    for (Window *window : windows) window->step();

    // 受け取ったイベント（キーボードやマウス入力）を処理する。
    // キー押下の瞬間などを捉えるために、ユーザ処理の直前（間に描画を挟まず）に置く。
    poll_events();
}
