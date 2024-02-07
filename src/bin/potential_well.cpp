#include <master.hpp>
#include <utility.hpp>

class WaveSimulator : public WorldObject {
  public:
    // シミュレーションのパラメータ
    const int grid_size = 100;          // グリッドのサイズ
    const double dx = 0.1;              // グリッド間の距離
    const double dt = 0.01;             // 時間ステップ
    const double c = 1.0;               // 波の速度
    const double damping_factor = 0.99; // 減衰係数

    // グリッドの初期化
    std::vector<double> grid;
    std::vector<double> prev_grid;

    WaveSimulator(const ProgramObject &shader)
        : grid(grid_size, 0)
        , prev_grid(grid_size, 0) {
        initialize();
        auto &line = this->append_child<DynamicArray>(shader);
        this->get_world().timer.task(0.05, [this, &line] {
            this->step();
            line.vertices.xs = linspace(-300, 300, grid_size);
            line.vertices.ys = map(grid, [](auto x) { return float(x) * 300; });
            line.vertices.colors = std::vector<RGBA>(grid_size, {1, 1, 1, 1});
        });
    }

    // 初期条件の設定
    void initialize() {
        // 初期の波形を設定
        for (int i = grid_size / 4; i < 3 * grid_size / 4; ++i) {
            grid[i] = 1.0; // 波の初期振幅
        }
    }

    // シミュレーションの更新
    void step() {
        for (int i = 1; i < grid_size - 1; ++i) {
            // オイラー法による更新
            double acceleration = (c * c) * (grid[i + 1] + grid[i - 1] - 2 * grid[i]) / (dx * dx);
            prev_grid[i] = grid[i];
            grid[i] += acceleration * dt;
        }

        // 境界条件
        grid[0] = grid[1];
        grid[grid_size - 1] = grid[grid_size - 2];

        // 減衰
        for (int i = 0; i < grid_size; ++i) {
            grid[i] *= damping_factor;
        }
    }
};

// int main() {
//     initialize();

//     // シミュレーションの実行
//     for (int t = 0; t < 1000; ++t) {
//         update();
//     }

//     // シミュレーション結果の出力
//     for (int i = 0; i < grid_size; ++i) {
//         std::cout << grid[i] << " ";
//     }
//     std::cout << std::endl;

//     return 0;
// }
int main() {
    GUI gui;
    Window &window = gui.create_window(600, 500, "井戸型ポテンシャル");
    auto &viewport = window.make_child<MaximumViewport>();
    World &main_world = window.create_world();

    // バーテックスシェーダのコンパイル
    auto vsh_string = load_string("assets/shaders/shader.vsh");
    auto vsh_id = create_shader(GL_VERTEX_SHADER, vsh_string);

    // フラグメントシェーダのコンパイル
    auto fsh_string = load_string("assets/shaders/shader.fsh");
    auto fsh_id = create_shader(GL_FRAGMENT_SHADER, fsh_string);

    auto main_shader = ProgramObject{vsh_id, fsh_id};

    auto &camera = main_world.append_child<MobileNormalCamera>(viewport);
    camera.position = {0, 0, 1000};
    camera.rotate = ANGLE_Y(M_PIf);
    camera.scale = 1;
    camera.set_active();

    Font migmix_font;
    auto &sample_text = main_world.append_child<Text>(migmix_font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});
    auto &credit_text = main_world.append_child<Text>(migmix_font, "(C) LearnOpenGL.com", RGBA{0.3, 0.7, 0.9, 0.4});

    sample_text.set_position({-200, 50, 200});
    credit_text.set_position({200, 400, 1});

    main_world.append_child<WaveSimulator>(main_shader);

    gui.mainloop();
}
