#pragma once

#include <graphical_base.hpp>

class Window;

class GUI {
    std::vector<std::unique_ptr<Window>> windows_;
    bool looping_ = false;
    GLFWmonitor *monitor_ = nullptr; // 初期化時のプライマリモニターを保持
                                     // NOTE: モニターはプログラム初期化時のものをずっとメインとみなす。

    // INFO: Windowクラスへ移動したいが、ウィンドウ自身に今いるモニターを認識させる機能がなさそうなため、このままでいいかも。
    std::pair<float, float> dpi_; // ディスプレイのDPI。プログラムを通して、初期化時に算出した値をずっと使い回す。
                                  // 描画のたびにDPIを再計算してもいいが、パフォーマンス上の懸念が拭えず、マルチモニターや途中のDPI変更などの際は結局多少なりとも乱れてしまうだろうから、シンプルなこの方式が最良と判断。

    std::pair<float, float> px_meter_; // モニターの物理的サイズと画素密度に基づいて、1ピクセルが何メートル四方かを計算した値。
    std::pair<float, float> vpixel_;   // px_meter_にシステムの画面拡大率を乗算した値。
                                       // vpixelを使うと、HiDPIなどで画面のスケールを変えているときに対応できる。

  public:
    int tick = 0;

    GUI();
    ~GUI();

    // コピーコンストラクタ、コピー代入演算子を削除
    GUI(const GUI &) = delete;
    GUI &operator=(const GUI &) const = delete;

    // ムーブコンストラクタ、ムーブ代入演算子を削除
    GUI(GUI &&) = delete;
    GUI &operator=(GUI &&) const = delete;

    // ウィンドウを新規作成する。
    Window &create_window(int width, int height, const std::string &title);

    // 毎フレーム呼び出してウィンドウの更新＆イベント処理を行う。
    // 直接触らず、`mainloop()`関数から呼び出すことを推奨。
    void refresh_windows();

    // メインループに制御を移す。
    template <typename F = void (*)()>
    void mainloop(F &&custom_routine = [] {}) {
        if (looping_) {
            throw std::runtime_error("すでにメインループが始まっています");
        }
        looping_ = true;

        // 描画のループ
        while (!this->windows_.empty()) {
            tick++;

            // 受け取ったイベント（キーボードやマウス入力）を処理する
            // キー押下の瞬間などを捉えるために、ユーザ処理よりも前に置く
            glfwPollEvents();

            custom_routine(); // 削除されたウィンドウへのアクセスを避けるため、ウィンドウ処理よりも前に置く
            this->refresh_windows();
        }

        looping_ = false;
    }

    // ビデオモード（解像度・リフレッシュレート・色情報）を取得する。
    // 戻り値の参照は指定したモニターが切断されるか、ライブラリが終了するまで有効
    [[nodiscard]] const GLFWvidmode &video_mode() const {
        return *glfwGetVideoMode(monitor_);
    }

    // ディスプレイの物理サイズ（ミリメートル単位）を取得する。
    [[nodiscard]] std::pair<int, int> monitor_physical_size() const {
        int width_mm, height_mm;
        glfwGetMonitorPhysicalSize(monitor_, &width_mm, &height_mm);
        return {width_mm, height_mm};
    }

    // モニターのcontent scaleを取得する。TODO: glfwGetWindowContentScale()との違いは謎。
    [[nodiscard]] std::pair<float, float> monitor_content_scale() const {
        float xscale, yscale;
        glfwGetMonitorContentScale(monitor_, &xscale, &yscale);
        return {xscale, yscale};
    }

    // 計算済みのDPIを取得する。
    [[nodiscard]] std::pair<float, float> dpi() const { return dpi_; }
    // 計算済みのピクセル寸法を取得する。
    [[nodiscard]] std::pair<float, float> px_meter() const { return px_meter_; }
    // 計算済みのvpixel(px_meter✕画面拡大率)を取得する。
    [[nodiscard]] std::pair<float, float> vpixel() const { return vpixel_; }
};
