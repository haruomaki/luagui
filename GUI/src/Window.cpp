#include <KeyCallback.hpp>
#include <SizeCallback.hpp>
#include <Update.hpp>
#include <Window.hpp>

Window::Window(int width, int height) {
    // ライブラリglfw の初期化
    if (glfwInit() == 0) {
        throw;
    }

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    // ウィンドウを作成
    // window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    gwin_ = glfwCreateWindow(width, height, "Hello World", nullptr, nullptr);
    if (gwin_ == nullptr) {
        glfwTerminate();
        throw;
    }

    // 作成したウィンドウを，OpenGLの描画関数のターゲットにする
    glfwMakeContextCurrent(gwin_);
    glfwSwapInterval(1);

    // GLEWの初期化
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLEW");
    }

    debug(glGetString(GL_VERSION));
    debug(glGetString(GL_VENDOR));
    debug(glGetString(GL_RENDERER));

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
        for (auto *size_callback : window->size_callbacks_) {
            (*size_callback)(width, height);
        }
    });

    // キーコールバック
    glfwSetKeyCallback(gwin_, [](GLFWwindow *gwin, int key, int /*scancode*/, int action, int /*mods*/) {
        auto *window = static_cast<Window *>(glfwGetWindowUserPointer(gwin));
        for (auto *key_callback : window->key_callbacks_) {
            (*key_callback)(key, action);
        }
    });

    // setCamera({0, 0}, default_camera_zoom);
}

Window::~Window() { glfwTerminate(); }

GLFWwindow *Window::getGLFW() const {
    return this->gwin_;
}

pair<int, int> Window::getWindowSize() const {
    int width, height;
    glfwGetWindowSize(gwin_, &width, &height);
    return {width, height};
}

pair<int, int> Window::getFrameBufferSize() const {
    int width, height;
    glfwGetFramebufferSize(gwin_, &width, &height);
    return {width, height};
}

pair<float, float> Window::getWindowContentScale() const {
    float xscale, yscale;
    glfwGetWindowContentScale(gwin_, &xscale, &yscale);
    return {xscale, yscale};
}

void Window::close() const {
    glfwSetWindowShouldClose(gwin_, GL_TRUE);
}

bool Window::getKey(int key) const {
    return glfwGetKey(this->gwin_, key) == GLFW_PRESS;
}

void Window::mainloop(const std::function<void()> &callback) {
    if (looping_) {
        throw std::runtime_error("すでにメインループが始まっています");
    }
    looping_ = true;

    // 描画のループ
    while (glfwWindowShouldClose(gwin_) == 0) {
        // // WorldObjectの更新 TODO: 一フレームごとに更新 vs setPosition()ごとに更新（重いかも）
        // world_object_root_.refreshAbsolutePosition();

        // 更新処理
        tick++;
        for (auto *update : this->updates_) {
            (*update)();
        }

        // 画面の初期化
        constexpr RGBA bg_color{0.2f, 0.2f, 0.2f, 1};
        glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ユーザの描画関数
        // glDisable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        callback();
        // glEnable(GL_DEPTH_TEST);

        // 上記描画した図形を表画面のバッファにスワップする
        glfwSwapBuffers(gwin_);

        // 受け取ったイベント（キーボードやマウス入力）を処理する
        glfwPollEvents();
    }

    looping_ = false;
}
