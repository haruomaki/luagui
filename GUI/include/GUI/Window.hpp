#pragma once

#include "Resource.hpp"
#include <SumiGL/Context.hpp>
#include <SumiGL/Shader.hpp>
#include <SumiGL/buffered_container.hpp>

class World;
class GUI;
struct Material;
class Viewport;

// 一つのウィンドウを表すクラス
class Window : public GL::Window {
    GLFWwindow *gwin_ = nullptr; // GL::Windowのgwinと同じ
    using KeyArray = std::array<bool, 512>;
    KeyArray key_down_{}, key_up_{};
    std::pair<double, double> last_cursor_ = cursor_pos();
    std::set<std::unique_ptr<Resource>> resources_;
    std::vector<std::unique_ptr<World>> worlds_;

    friend class Update;

    void routine();
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
    BufferedSet<std::function<void()> *> resource_updates;
    std::vector<std::function<void()>> raw_worlds;

    // ウィンドウ内描画領域の大きさを表す変数。framebuffer_size()は直接取得する（故に重い）のに対し、ここには毎フレーム自動で取得されたものがキャッシュされている。
    std::pair<int, int> fbsize_cache = {0, 0};

    Window(GL::Context &gui, int width, int height, const char *title);
    ~Window();

    // コピーもムーブも禁止する
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    void close() const;

    // 前フレームからのカーソルの移動差分を得る。
    [[nodiscard]] std::pair<double, double> cursor_diff() const {
        auto [x, y] = cursor_pos();
        return {x - last_cursor_.first, y - last_cursor_.second};
    }

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

    World &create_world();
};
