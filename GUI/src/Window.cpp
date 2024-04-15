#include "Window.hpp"
#include "Material.hpp"
#include "Viewport.hpp"
#include "World.hpp"

#include <algorithm>

using namespace std::chrono_literals;

Window::Window(GUI &gui, int width, int height, const char *title)
    : gwin_(glfwCreateWindow(width, height, title, nullptr, nullptr))
    , gui(gui) {
    // ウィンドウを作成
    if (gwin_ == nullptr) {
        throw std::runtime_error("ウィンドウの作成に失敗しました");
    }
    // GLFWwindow *gwin = glfwCreateWindow(width, height, title, nullptr, nullptr);
    // if (gwin == nullptr) {
    //     throw std::runtime_error("ウィンドウの作成に失敗しました");
    // }
    // this->gwin_ = gwin;

    // 作成したウィンドウを，OpenGLの描画関数のターゲットにする
    glfwMakeContextCurrent(gwin_);
    glfwSwapInterval(1);

    // GLEWの初期化
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw std::runtime_error("GLEWの初期化に失敗しました");
    }

    dump(glGetString(GL_VERSION));
    dump(glGetString(GL_VENDOR));
    dump(glGetString(GL_RENDERER));

    // ブレンド（透明処理）の設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 深度テストを有効化
    glEnable(GL_DEPTH_TEST);

    // ユーザー領域にこのクラスのインスタンスを設定
    glfwSetWindowUserPointer(gwin_, this);

    // ウィンドウサイズコールバック関数を登録する
    glfwSetWindowSizeCallback(gwin_, [](GLFWwindow *gwin, int width, int height) {
        // ここで描画処理などを行う
        auto *window = static_cast<Window *>(glfwGetWindowUserPointer(gwin));
        // window->setCamera({0, 0}, default_camera_zoom);
        // debug(window->getViewMatrix());

        // 登録されたコールバック関数たちを実行
        window->size_callbacks_.safe_foreach([&](auto size_callback) {
            size_callback(width, height);
        });
    });

    // キーコールバック
    // INFO: キャラを一定速度で動かしたいなどの際は、1フレームごとにgetKeyメソッドを呼び出す方がいい
    glfwSetKeyCallback(gwin_, [](GLFWwindow *gwin, int key, int /*scancode*/, int action, int /*mods*/) {
        auto *window = static_cast<Window *>(glfwGetWindowUserPointer(gwin));
        window->key_callbacks.safe_foreach([&](auto key_callback) {
            key_callback(key, action);
        });
    });

    // デフォルトシェーダの設定
    this->default_shader.emplace({create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/default.vsh")),
                                  create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/default.fsh"))});

    // デフォルトマテリアルの設定
    this->default_material = &MaterialBuilder().build(*this);

    // デフォルトビューポートの設定
    this->default_viewport = &this->append_resource<MaximumViewport>();
}

Window::~Window() {
    this->worlds_.clear();         // key_callbacksが消える前にKeyCallbackObjectが消えないといけない
    this->window_objects_.clear(); // resource_updatesが消える前にResourceUpdateのデストラクタを呼ぶ

    // print("Windowのデストラクタです");
    // debug(this->size_callbacks_.size());
    // this->size_callbacks_.erase(this->size_callbacks_.begin());
    // debug(this->size_callbacks_.size());
    // print("消しました");

    // glfwSetWindowSizeCallback(gwin_, nullptr);
    // glfwSetKeyCallback(gwin_, nullptr);
}

GLFWwindow *Window::glfw() const {
    return this->gwin_;
}

pair<int, int> Window::window_size() const {
    int width, height;
    glfwGetWindowSize(gwin_, &width, &height);
    return {width, height};
}

pair<int, int> Window::frame_buffer_size() const {
    int width, height;
    glfwGetFramebufferSize(gwin_, &width, &height);
    return {width, height};
}

pair<float, float> Window::window_content_scale() const {
    float xscale, yscale;
    glfwGetWindowContentScale(gwin_, &xscale, &yscale);
    return {xscale, yscale};
}

void Window::close() const {
    glfwSetWindowShouldClose(gwin_, GL_TRUE);
}

bool Window::key(int key) const {
    return glfwGetKey(this->gwin_, key) == GLFW_PRESS;
}

pair<double, double> Window::cursor_pos() const {
    double x, y;
    glfwGetCursorPos(this->gwin_, &x, &y);
    return {x, y};
}

World &Window::create_world() {
    // draw_priority_の最大値
    int max_priority = std::numeric_limits<int>::min();
    for (const auto &world : worlds_) {
        max_priority = std::max(max_priority, world->draw_priority_);
    }

    // 新規ワールドの描画優先度は、max_priorityより少しだけ大きい10の倍数とする
    int one_level_higher = 0;
    if (max_priority >= 0) {
        one_level_higher = max_priority - (max_priority % 10) + 10;
    }

    auto world = std::make_unique<World>(*this, one_level_higher);
    this->worlds_.push_back(std::move(world));

    return *this->worlds_.back();
}

void Window::draw_routine() {
    // OpenGLの描画関数のターゲットにするウィンドウを指定
    glfwMakeContextCurrent(this->gwin_);

    // 画面の初期化
    constexpr RGBA bg_color{0.2f, 0.2f, 0.2f, 1};
    glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 優先度が非負である各ワールドを順に描画
    for (const auto &world : this->worlds_) {
        if (world->draw_priority_ >= 0) {
            glClear(GL_DEPTH_BUFFER_BIT);
            world->master_draw();
        }
    }

    // 上記描画した図形を表画面のバッファにスワップする
    glfwSwapBuffers(this->gwin_);
}

void Window::update_routine() {
    // // WorldObjectの更新 TODO: 一フレームごとに更新 vs setPosition()ごとに更新（重いかも）
    // world_object_root_.refreshAbsolutePosition();

    // リソースの更新処理
    this->resource_updates.safe_foreach([](const auto update) {
        update();
    });

    // 各ワールドの更新処理
    for (const auto &world : this->worlds_) {
        world->master_update();
    }
}

// World::draw_priority_に基づき、worlds_を昇順に並べ替える
void Window::refresh_world_order() {
    std::sort(this->worlds_.begin(), this->worlds_.end(),
              [](const auto &world1, const auto &world2) {
                  return world1->draw_priority_ < world2->draw_priority_;
              });
}
