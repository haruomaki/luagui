#include <KeyCallback.hpp>
#include <SizeCallback.hpp>
#include <Update.hpp>
#include <Window.hpp>

using namespace std::chrono_literals;

Window::Window(int width, int height, const char *title)
    : gwin_(glfwCreateWindow(width, height, title, nullptr, nullptr)) {
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
        debug(window->size_callbacks_.size());
        for (auto &&size_callback : window->size_callbacks_) {
            (size_callback->size_callback)(width, height);
        }
    });

    // キーコールバック
    // INFO: キャラを一定速度で動かしたいなどの際は、1フレームごとにgetKeyメソッドを呼び出す方がいい
    glfwSetKeyCallback(gwin_, [](GLFWwindow *gwin, int key, int /*scancode*/, int action, int /*mods*/) {
        auto *window = static_cast<Window *>(glfwGetWindowUserPointer(gwin));
        for (auto *key_callback : window->key_callbacks_) {
            (*key_callback)(key, action);
        }
    });

    // setCamera({0, 0}, default_camera_zoom);
}

// Window::~Window() {
// print("Windowのデストラクタです");
// debug(this->size_callbacks_.size());
// this->size_callbacks_.erase(this->size_callbacks_.begin());
// debug(this->size_callbacks_.size());
// print("消しました");

// glfwSetWindowSizeCallback(gwin_, nullptr);
// glfwSetKeyCallback(gwin_, nullptr);
// }

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
