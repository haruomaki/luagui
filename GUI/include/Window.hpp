#pragma once

#include "SizeCallback.hpp"
#include <graphical_base.hpp>

class SizeCallback;

using FunctionId = int;

template <typename Func>
class FunctionSet {
  public:
    std::map<FunctionId, std::function<Func>> functions_;
    FunctionId function_id_counter_ = 0;
    FunctionId set_function(std::function<Func> &&func) {
        this->functions_[this->function_id_counter_] = std::move(func);
        return function_id_counter_++;
    }
};

// 一つのウィンドウを表すクラス
class Window {
    GLFWwindow *gwin_ = nullptr;
    std::set<std::unique_ptr<WindowObject>> window_objects_;
    FunctionSet<void(int, int)> size_callbacks_;
    std::set<std::function<void(int, int)> *> key_callbacks_;
    std::set<std::function<void()> *> updates_;

    friend class GUI;
    friend class SizeCallback;
    friend class KeyCallback;
    friend class Update;

  public:
    Window(int width, int height, const char *title);
    // ~Window();

    // // コピーは禁止する
    // Window(const Window &) = default;
    // Window &operator=(const Window &) = default;

    // // ムーブコンストラクタはデフォルト、ムーブ代入は禁止
    // Window(Window &&) = default;
    // Window &operator=(Window &&) = default;

    [[nodiscard]] GLFWwindow *get_glfw() const;
    [[nodiscard]] pair<int, int> get_window_size() const;
    [[nodiscard]] pair<int, int> get_frame_buffer_size() const;
    [[nodiscard]] pair<float, float> get_window_content_scale() const;
    void close() const;

    // glfwGetKeyのラッパー。GLFW_PRESSのときtrue、GLFW_RELEASEのときfalse
    [[nodiscard]] bool get_key(int key) const;

    template <typename T, typename... Args>
        requires std::is_constructible_v<T, Args...> &&     // ArgsはTのコンストラクタの引数
                 std::is_convertible_v<T *, WindowObject *> // TはWindowObjectの派生クラス
    T &make_child(Args &&...args) {
        // WindowObjectのコンストラクタを呼ぶ直前には必ずsetWindowStaticを呼び、直後nullptrにリセット
        WindowObject::set_window_static(this);
        // argsを引数として使って、ヒープ上にT型のオブジェクトを作成
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...); // NOTE: &&やforwardは必要かよく分からない
        WindowObject::set_window_static(nullptr);

        auto [it, inserted] = this->window_objects_.insert(std::move(ptr));
        if (!inserted) {
            std::runtime_error("make_childに失敗");
        }
        auto ptr2 = static_cast<T *>(it->get());
        return *ptr2;
    }

    // 各種コールバックを設定する関数群
    // TODO: テンプレートを駆使して短く記述
    template <CallbackKind callback_kind>
    void set_callback();
    template <CallbackKind callback_kind>
    void set_callback(std::function<void(int, int)> &&callback);

    template <>
    void set_callback<Size>(std::function<void(int, int)> &&callback) {
        this->size_callbacks_.set_function(std::move(callback));
    }

    // // コールバックを削除する関数群
    // template <CallbackKind callback_kind>
    // void unset_callback(std::function<void(int, int)> &&callback);

    // template <>
    // void unset_callback<Size>(std::function<void(int, int)> &&callback) {
    //     this->size_callbacks_.erase(callback);
    // }
};
