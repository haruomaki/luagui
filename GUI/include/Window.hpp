#pragma once

#include "FunctionSet.hpp"
#include "Resource.hpp"
#include "Shader.hpp"
#include <graphical_base.hpp>

class SizeCallback;
class World;
class GUI;
struct Material;
class Viewport;

// 一つのウィンドウを表すクラス
class Window {
    GLFWwindow *gwin_ = nullptr;
    std::set<std::unique_ptr<Resource>> window_objects_;
    FunctionSet<void(int, int)> size_callbacks_; // TODO: key_callbackに合わせてpublicにしてもいい
    std::vector<std::unique_ptr<World>> worlds_ = {};

    friend class SizeCallback;
    friend class KeyCallback;
    friend class Update;

    friend class GUI;
    void draw_routine();
    void update_routine();

    friend class World;
    void refresh_world_order();

  public:
    GUI &gui;
    std::optional<ProgramObject> default_shader;
    Material *default_material = nullptr;
    Viewport *default_viewport = nullptr;
    FunctionSet<void()> resource_updates;

    FunctionSet<void(int key, int action)> key_callbacks;

    Window(GUI &gui, int width, int height, const char *title);
    ~Window();

    // // コピーは禁止する
    // Window(const Window &) = default;
    // Window &operator=(const Window &) = default;

    // // ムーブコンストラクタはデフォルト、ムーブ代入は禁止
    // Window(Window &&) = default;
    // Window &operator=(Window &&) = default;

    [[nodiscard]] GLFWwindow *glfw() const;
    [[nodiscard]] pair<int, int> window_size() const;
    [[nodiscard]] pair<int, int> frame_buffer_size() const;
    [[nodiscard]] pair<float, float> window_content_scale() const;
    void close() const;

    // glfwGetKeyのラッパー。GLFW_PRESSのときtrue、GLFW_RELEASEのときfalse
    [[nodiscard]] bool key(int key) const;

    // glfwGetCursorPos()のラッパー
    [[nodiscard]] pair<double, double> cursor_pos() const;

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...> && // ArgsはTのコンストラクタの引数
                 std::is_convertible_v<T *, Resource *> // TはWindowObjectの派生クラス
    T &append_resource(Args &&...args) {
        // WindowObjectのコンストラクタを呼ぶ直前には必ずsetWindowStaticを呼び、直後nullptrにリセット
        Resource::set_window_static(this);
        // argsを引数として使って、ヒープ上にT型のオブジェクトを作成
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...); // NOTE: &&やforwardは必要かよく分からない
        Resource::set_window_static(nullptr);

        auto [it, inserted] = this->window_objects_.insert(std::move(ptr));
        if (!inserted) {
            throw std::runtime_error("make_childに失敗");
        }
        auto ptr2 = dynamic_cast<T *>(it->get());
        return *ptr2;
    }

    // 各種コールバックを設定する関数群
    // TODO: テンプレートを駆使して短く記述
    template <CallbackKind callback_kind>
    void set_callback();
    template <CallbackKind callback_kind>
    void set_callback(std::function<void(int, int)> &&callback);

    template <>
    void set_callback<CallbackKind::Size>(std::function<void(int, int)> &&callback) {
        this->size_callbacks_.set_function(std::move(callback));
    }

    // // コールバックを削除する関数群
    // template <CallbackKind callback_kind>
    // void unset_callback(std::function<void(int, int)> &&callback);

    // template <>
    // void unset_callback<Size>(std::function<void(int, int)> &&callback) {
    //     this->size_callbacks_.erase(callback);
    // }

    World &create_world();
};
