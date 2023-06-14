#include <cppgui.hpp>

void hello() {
    cout << "こんにちはです！" << endl;
}

// 画像 https://nn-hokuson.hatenablog.com/entry/2017/02/24/171230
GLuint loadTexture(const string &filename) {
    // テクスチャIDの生成
    GLuint tex_id;
    glGenTextures(1, &tex_id);

    // ファイルの読み込み
    std::ifstream fstr(filename, std::ios::binary);
    const size_t file_size = static_cast<size_t>(fstr.seekg(0, ifstream::end).tellg());
    fstr.seekg(0, ifstream::beg);
    char *texture_buffer = new char[file_size];
    fstr.read(texture_buffer, file_size);

    // テクスチャをGPUに転送
    const int width = 256, height = 256;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_buffer);

    // テクスチャの設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // テクスチャのアンバインド
    delete[] texture_buffer;
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex_id;
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
    glfwSwapInterval(1);

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
        auto *window = static_cast<Window *>(glfwGetWindowUserPointer(gwin));
        // window->setCamera({200, 200}, 1);
        window->setCameraCorner({0, 0}, 1);
    });

    glfwSetKeyCallback(gwin_, masterKeyCallback);

    // camera_ = {0, (double)width, 0, (double)height};
    setCameraCorner({0, 0}, zoom_);

    program_id_ = createShader();
}

Window::~Window() { glfwTerminate(); }

pair<int, int> Window::getWindowSize() {
    int width, height;
    glfwGetWindowSize(gwin_, &width, &height);
    return {width, height};
}

pair<int, int> Window::getFrameBufferSize() {
    int width, height;
    glfwGetFramebufferSize(gwin_, &width, &height);
    return {width, height};
}

pair<float, float> Window::getWindowContentScale() {
    float xscale, yscale;
    glfwGetWindowContentScale(gwin_, &xscale, &yscale);
    return {xscale, yscale};
}

void Window::mainloop(const std::function<void()> &callback) {
    if (looping_) {
        throw runtime_error("すでにメインループが始まっています");
    }
    looping_ = true;

    // 描画のループ
    while (glfwWindowShouldClose(gwin_) == 0) {
        // // WorldObjectの更新 TODO: 一フレームごとに更新 vs setPosition()ごとに更新（重いかも）
        // world_object_root_.refreshAbsolutePosition();

        // 更新処理
        tick_++;
        masterUpdate();

        constexpr int interval = 60;
        if (tick_ % interval == 0) {
            cout << "み" << endl;
        }

        // 画面の初期化
        constexpr RGBA bg_color{0.2, 0.2, 0.2, 1};
        glClearColor(bg_color.r_, bg_color.g_, bg_color.b_, bg_color.a_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);

        // カメラの設定
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(camera_.left_, camera_.right_, camera_.bottom_, camera_.top_, -1.0, 1.0);

        // 描画準備
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // 登録された図形の描画
        masterDraw();

        // ユーザの描画関数
        callback();

        // 上記描画した図形を表画面のバッファにスワップする
        glfwSwapBuffers(gwin_);

        // 受け取ったイベント（キーボードやマウス入力）を処理する
        glfwPollEvents();
    }

    looping_ = false;
}

void Window::setCamera(Point<float> pos, float /*zoom*/) {
    auto framebuf = getFrameBufferSize();
    auto width = framebuf.first;
    auto height = framebuf.second;
    camera_ = {pos.x_ - width / 2, pos.x_ + width / 2, pos.y_ - height / 2, pos.y_ + height / 2};
}

void Window::setCameraCorner(Point<float> pos, float /*zoom*/) {
    auto framebuf = getFrameBufferSize();
    auto width = framebuf.first;
    auto height = framebuf.second;
    camera_ = {pos.x_, pos.x_ + width, pos.y_, pos.y_ + height};
}
