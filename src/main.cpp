#include <cppgui.hpp>

void hello() {
    cout << "こんにちはです！" << endl;
}

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

    // GLEWの初期化
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW");
    }

    glfwSetWindowUserPointer(gwin_, this);

    // ウィンドウサイズコールバック関数を登録する
    glfwSetWindowSizeCallback(gwin_, [](GLFWwindow *gwin, int width, int height) {
        // OpenGLのビューポートを再設定する
        glViewport(0, 0, width, height);
        // ここで描画処理などを行う
        Window *window = static_cast<Window *>(glfwGetWindowUserPointer(gwin));
        // window->setCamera({200, 200}, 1);
        window->setCameraCorner({0, 0}, 1);
    });

    glfwSetKeyCallback(gwin_, masterKeyCallback);

    // camera_ = {0, (double)width, 0, (double)height};
    setCameraCorner({0, 0}, zoom_);
}

Window::~Window() { glfwTerminate(); }

pair<int, int> Window::getWindowSize() {
    int width, height;
    glfwGetWindowSize(gwin_, &width, &height);
    return pair<int, int>(width, height);
}

pair<int, int> Window::getFrameBufferSize() {
    int width, height;
    glfwGetFramebufferSize(gwin_, &width, &height);
    return pair<int, int>(width, height);
}

pair<float, float> Window::getWindowContentScale() {
    float xscale, yscale;
    glfwGetWindowContentScale(gwin_, &xscale, &yscale);
    return pair<float, float>(xscale, yscale);
}

void Window::mainloop(std::function<void()> f) {
    if (looping_) {
        throw runtime_error("すでにメインループが始まっています");
    }
    looping_ = true;

    // 描画のループ
    while (glfwWindowShouldClose(gwin_) == 0) {
        // 更新処理
        tick_++;
        for (auto &&poly : polys_) {
            poly->update();
        }

        if (tick_ % 60 == 0) {
            cout << "み" << endl;
        }

        // 画面を塗りつぶす
        glClear(GL_COLOR_BUFFER_BIT);

        // // カメラの設定
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(camera_.left_, camera_.right_, camera_.bottom_, camera_.top_, -1.0, 1.0);

        // 描画準備
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // 登録された図形の描画
        for (auto &&poly : polys_) {
            poly->draw();
        }

        // ユーザの描画関数
        f();

        // 上記描画した図形を表画面のバッファにスワップする
        glfwSwapBuffers(gwin_);

        // 受け取ったイベント（キーボードやマウス入力）を処理する
        glfwPollEvents();
    }

    looping_ = false;
}

void Window::setCamera(Point<float> pos, float zoom) {
    auto fb = getFrameBufferSize();
    auto width = fb.first;
    auto height = fb.second;
    camera_ = {pos.x_ - width / 2, pos.x_ + width / 2, pos.y_ - height / 2, pos.y_ + height / 2};
}

void Window::setCameraCorner(Point<float> pos, float zoom) {
    auto fb = getFrameBufferSize();
    auto width = fb.first;
    auto height = fb.second;
    camera_ = {pos.x_, pos.x_ + width, pos.y_, pos.y_ + height};
}

// Windowの要素にアクセスするためここで定義
Polygon::Polygon(Window &window, vector<Point<float>> points, GLenum usage)
    : vertices_(points)
    , window_(window)
    , polys_(window.polys_) {
    // ポリゴンリストに追加
    polys_.insert(this);

    // 頂点バッファオブジェクト（VBO）の生成とデータの転送
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_.size() * 2, vertices_.data(), usage); // WARNING: vertices_のアライメントによっては動作しない
}
