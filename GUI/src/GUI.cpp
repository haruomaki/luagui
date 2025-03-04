#include "GUI.hpp"
#include "World.hpp"

static const char *const DEFAULT_VSH = R"(
#version 330 core

in vec3 position;
in vec2 uv;
in vec4 color;
in mat4 instanceModelMatrix;
out vec2 vuv;
out vec4 vColor;
// uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void){
    gl_Position = projectionMatrix * viewMatrix * instanceModelMatrix * vec4(position, 1.0);
    vuv = uv;
    vColor = color;
}
)";

static const char *const DEFAULT_FSH = R"(
varying vec2 vuv;
uniform sampler2D texture;
uniform bool is_tex;
uniform vec4 baseColor;
varying vec4 vColor;

void main(void){
    if (is_tex) {
        gl_FragColor = texture2D(texture, vuv);
    } else {
        gl_FragColor = vColor * baseColor;
    }
}
)";

GUI::GUI()
    : resources(*this)
    , resume_condition([this] {
        return !this->windows.empty();
        // return !this->windows.empty() && this->epoch() < 5;
    }) {
    // デフォルトシェーダの設定
    GL::ProgramObject pg{GL::create_shader(GL_VERTEX_SHADER, DEFAULT_VSH),
                         GL::create_shader(GL_FRAGMENT_SHADER, DEFAULT_FSH)};
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
    this->worlds_.clear(); // リソースよりも前に解放する。
    this->resources.clear();
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
    // print("<<<更新>>> epoch=", epoch());
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
        const RGBA bg = window->background_color;
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        glDepthMask(GL_TRUE); // ONになっていないとデプスバッファがクリアできない
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
