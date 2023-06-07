#include <base.hpp>

void hello();
int x;

#include <base.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <set>

inline namespace gui {

static set<function<void(int, int)> *> global_key_callbacks;

class Window;

template <typename T>
struct Rect {
    T left_, right_, bottom_, top_;
};

class Polygon {
    vector<Point<GLfloat>> vertices_;
    GLuint vbo_;
    Window &window_;
    set<Polygon *> &polys_;

  public:
    RGB color_;
    function<void(int, int)> key_callback_ = nullptr;

    Polygon(Window &window, vector<Point<float>> points, GLenum usage);

    ~Polygon() {
        polys_.erase(this);
        // key_callbacks_.erase(this->keyCallback);
    }

    void draw() const {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glColor3ub(color_.r_, color_.b_, color_.g_);
        glDrawArrays(GL_POLYGON, 0, vertices_.size());

        glDisableVertexAttribArray(0); // TODO: 不要？
    }

    void update() {
        for (auto &vert : vertices_) {
            const float velocity = 0.3f;
            vert.y_ += velocity;
        }

        // VRAM上のVBOを更新
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertices_.size() * 2, vertices_.data());
    }
};

class Element {
  public:
    Point<float> pos_;
    set<Element> childs_;
    function<void(int, int)> key_callback_;

    Element() {
        key_callback_ = bind(&Element::keyCallback, this, placeholders::_1, placeholders::_2);
        global_key_callbacks.insert(&key_callback_);
    }
    ~Element() {
        global_key_callbacks.erase(&key_callback_);
    }

    virtual void keyCallback(int /*unused*/, int /*unused*/) {}
};

class Window {
    GLFWwindow *gwin_;
    Rect<double> camera_;
    float zoom_ = 1;
    bool looping_ = false;
    set<Polygon *> polys_;
    set<function<void(int, int)> *> key_callbacks_;

  public:
    Window(int width, int height) {
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

        glfwSetKeyCallback(gwin_, [](GLFWwindow *gwin, int key, int scancode, int action, int mods) {
            // Window *window = static_cast<Window *>(glfwGetWindowUserPointer(gwin));
            for (auto key_callback : global_key_callbacks) {
                (*key_callback)(key, action);
            }
        });

        // camera_ = {0, (double)width, 0, (double)height};
        setCameraCorner({0, 0}, zoom_);
    }

    ~Window() { glfwTerminate(); }

    pair<int, int> getWindowSize() {
        int width, height;
        glfwGetWindowSize(gwin_, &width, &height);
        return pair<int, int>(width, height);
    }

    pair<int, int> getFrameBufferSize() {
        int width, height;
        glfwGetFramebufferSize(gwin_, &width, &height);
        return pair<int, int>(width, height);
    }

    pair<float, float> getWindowContentScale() {
        float xscale, yscale;
        glfwGetWindowContentScale(gwin_, &xscale, &yscale);
        return pair<float, float>(xscale, yscale);
    }

    void mainloop(std::function<void()> f) {
        if (looping_) {
            throw runtime_error("すでにメインループが始まっています");
        }
        looping_ = true;

        // 描画のループ
        while (glfwWindowShouldClose(gwin_) == 0) {
            // 更新処理
            for (auto &&poly : polys_) {
                poly->update();
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

    void setCamera(Point<float> pos, float zoom) {
        auto fb = getFrameBufferSize();
        auto width = fb.first;
        auto height = fb.second;
        camera_ = {pos.x_ - width / 2, pos.x_ + width / 2, pos.y_ - height / 2, pos.y_ + height / 2};
    }

    void setCameraCorner(Point<float> pos, float zoom) {
        auto fb = getFrameBufferSize();
        auto width = fb.first;
        auto height = fb.second;
        camera_ = {pos.x_, pos.x_ + width, pos.y_, pos.y_ + height};
    }

    friend class Polygon;
};

// Windowの要素にアクセスするためここで定義
Polygon::Polygon(Window &window, vector<Point<float>> points, GLenum usage = GL_DYNAMIC_DRAW)
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

} // namespace gui
