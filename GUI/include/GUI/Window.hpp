#pragma once

#include "FunctionSet.hpp"
#include "Resource.hpp"
#include <SumiGL/Context.hpp>
#include <SumiGL/Shader.hpp>
#include <SumiGL/buffered_container.hpp>

class SizeCallback;
class World;
class GUI;
struct Material;
class Viewport;

// 一つのウィンドウを表すクラス
class Window : public GL::Window {
    GLFWwindow *gwin_ = nullptr; // FIXME: GL::Windowのgwinに置き換えたい
    using KeyArray = std::array<bool, 512>;
    KeyArray key_down_{}, key_up_{};
    std::set<std::unique_ptr<Resource>> resources_;
    FunctionSet<void(int, int)> size_callbacks_; // TODO: key_callbackに合わせてpublicにしてもいい
    std::vector<std::unique_ptr<World>> worlds_;

    friend class SizeCallback;
    friend class KeyCallback;
    friend class Update;

    void draw_routine();
    void update_routine();
    void physics_routine();
    void post_process();

    friend class World;
    void refresh_world_order();

  public:
    GL::Context &gui;
    std::optional<GL::ProgramObject> default_shader;
    Material *default_material = nullptr;
    Viewport *default_viewport = nullptr;
    BufferedSet<std::function<void()> *> resource_updates;
    std::vector<std::function<void()>> raw_worlds;

    FunctionSet<void(int key, int action)> key_callbacks;

    Window(GL::Context &gui, int width, int height, const char *title);
    ~Window();

    // // コピーは禁止する
    // Window(const Window &) = default;
    // Window &operator=(const Window &) = default;

    // // ムーブコンストラクタはデフォルト、ムーブ代入は禁止
    // Window(Window &&) = default;
    // Window &operator=(Window &&) = default;

    void routines();

    [[nodiscard]] GLFWwindow *glfw() const;
    [[nodiscard]] pair<int, int> window_size() const;
    [[nodiscard]] pair<int, int> frame_buffer_size() const;
    [[nodiscard]] pair<float, float> window_content_scale() const;
    [[nodiscard]] int refresh_rate() const;
    void close() const;

    // glfwGetKeyのラッパー。GLFW_PRESSのときtrue、GLFW_RELEASEのときfalse
    [[nodiscard]] bool key(int key) const;

    // glfwGetCursorPos()のラッパー
    [[nodiscard]] pair<double, double> cursor_pos() const;

    // 現在フレームにおける、キー変更イベント（押した）の有無を表す
    [[nodiscard]] const KeyArray &key_down() const { return key_down_; }

    // 現在フレームにおける、キー変更イベント（離した）の有無を表す
    [[nodiscard]] const KeyArray &key_up() const { return key_up_; }

    template <typename T, typename... Args>
        requires std::constructible_from<T, Args...> && // ArgsはTのコンストラクタの引数
                 std::convertible_to<T *, Resource *>   // TはResourceの派生クラス
    T &append_resource(Args &&...args) {
        // WindowObjectのコンストラクタを呼ぶ直前には必ずsetWindowStaticを呼び、直後nullptrにリセット
        Resource::set_window_static(this);
        // argsを引数として使って、ヒープ上にT型のオブジェクトを作成
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...); // NOTE: &&やforwardは必要かよく分からない
        Resource::set_window_static(nullptr);

        auto [it, inserted] = this->resources_.insert(std::move(ptr));
        if (!inserted) {
            throw std::runtime_error("make_childに失敗");
        }
        auto ptr2 = dynamic_cast<T *>(it->get());
        return *ptr2;
    }

    template <typename T = Resource>
        requires std::convertible_to<T *, Resource *> // TはResourceの派生クラス
    T *find_resource(const std::string &name) {
        // NOTE: 虱潰しに検索するので簡潔だが非効率
        // 名前とリソースの対応を表すmapを新たに作りたいが、リソースの追加・削除によるバグが怖い
        for (const auto &rc : this->resources_) {
            if (rc->name_ == name) return dynamic_cast<T *>(rc.get());
        }
        return nullptr;
    }

    // 各種コールバックを設定する関数。
    template <CallbackKind callback_kind>
    void set_callback(std::function<void(int, int)> &&callback) {
        if constexpr (callback_kind == CallbackKind::Size) {
            this->size_callbacks_.request_set_function(std::move(callback));
        } else {
            static_assert(false, "CallbackKindが不正です");
        }
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
