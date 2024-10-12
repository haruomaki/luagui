#include <furitype.hpp>

// シェーダーのソースコード
static const char *const VERTEX_SHADER_SOURCE = R"(
#version 330 core
in vec2 position;

void main()
{
    gl_Position = vec4(position, 0, 1);
}
)";

static const char *const FRAGMENT_SHADER_SOURCE = R"(
#version 330 core
// in vec3 vertexColor; // ジオメトリシェーダから渡される色
// in vec2 bezierPos; // ベジェ曲線を塗りつぶすための座標情報
out vec4 FragColor;

void main()
{
    FragColor = vec4(0, 0, 0, 0);
    // if (bezierPos.x * bezierPos.x <= bezierPos.y) {
    //     FragColor = vec4(vertexColor, 1.0);
    // } else {
    //     discard;
    // }
}
)";

struct GlyphOutline {
    std::vector<glm::vec2> bodies; // TRIANGLE_FANで描く用のフォントの本体部
    std::vector<glm::vec2> rounds; // TRIANGLESで描く用のベジェ曲線
};

int main() {
    freetype::Context ft;
    auto *face = ft.load_font("assets/fonts/main.ttf");

    constexpr int max_charcode = 256;
    std::map<int, GlyphOutline> glyph_outlines;
    for (int charcode = 0; charcode < max_charcode; ++charcode) {
        FT_UInt glyph_index = FT_Get_Char_Index(face, charcode);
        if (glyph_index == 0) {
            // グリフが存在しない場合
            continue;
        }

        if (FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP) != 0) {
            // エラー処理
            continue;
        }

        FT_Outline outline = face->glyph->outline;
        // outlineを使って処理を行う

        // bodiesを生成
        auto add_body = [&](FT_Vector p) {
            glyph_outlines[charcode].bodies.emplace_back(p.x, p.y);
        };

        // roundsを生成
        FT_Vector pa, po, pb;
        auto add_round = [&]() {
            glyph_outlines[charcode].rounds.emplace_back(pa.x, pa.y);
            glyph_outlines[charcode].rounds.emplace_back(po.x, po.y);
            glyph_outlines[charcode].rounds.emplace_back(pb.x, pb.y);
        };

        FT_Vector origin = {0, 0};
        int start = 0;
        for (int ct = 0; ct < outline.n_contours; ct++) {
            // 同一contourで一周
            auto len = outline.contours[ct] + 1 - start;
            int tag_hist2 = -1, tag_hist1 = -1, tag = -1;

            add_body(origin);
            for (int i = 0; i < len + 1; i++) {
                FT_Vector p = outline.points[start + (i % len)];
                tag_hist2 = tag_hist1;
                tag_hist1 = tag;
                tag = outline.tags[start + (i % len)];

                if (tag == 1) {
                    add_body(p);
                    if (tag_hist2 == 1 && tag_hist1 == 0) {
                        pb = p;
                        add_round();
                    }
                    pa = p; // paには「最後に見つけた実点」が入る
                } else if (tag == 0) {
                    if (tag_hist2 == 1 && tag_hist1 == 0) {
                        FT_Vector mid = {(po.x + p.x) / 2, (po.y + p.y) / 2};
                        pb = mid;
                        add_round();
                        pa = mid;
                        tag_hist1 = 1;
                    }
                    po = p; // poには「最後に見つけた制御点」が入る
                } else {
                    throw std::runtime_error("1と0以外のタグには対応していません");
                }
            }
            start += len;
        }
    }

    // buffer[65].n_contours = 2;
    // buffer[65].n_points = 7;
    // buffer[65].contours[0] = 3;
    // buffer[65].contours[1] = 6;
    // buffer[65].points[0] = {0, 0, 1};
    // buffer[65].points[1] = {0, 1, 1};
    // buffer[65].points[2] = {1, 1, 1};
    // buffer[65].points[3] = {1, 0, 1};
    // buffer[65].points[4] = {0.2, 0.2, 1};
    // buffer[65].points[5] = {0.8, 0.2, 1};
    // buffer[65].points[6] = {0.5, 0.8, 1};
    debug(glyph_outlines[66].bodies);
    debug(glyph_outlines[66].rounds);

    GUI gui;
    Window &window = gui.create_window(500, 500, "魔法使いの書斎");

    // シェーダーのコンパイル
    GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
    GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);

    // シェーダープログラムのリンク
    ProgramObject shader = {
        vertex_shader,
        fragment_shader,
    };

    // 三角形の頂点データ
    // auto &points = glyph_outlines[int('A')].points;
    // std::vector<glm::vec2> vertices(points.size());
    // // for (auto p : points) {
    // //     vertices
    // // }
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    VertexArrayObject vao;
    VertexBufferObject vbo(sizeof(vertices), (float *)vertices, GL_STATIC_DRAW);

    vao.bind([&] {
        vbo.bind([&] {
            shader.set_attribute_float("position", 3, false, 3 * sizeof(float), nullptr);
        });
    });

    // ステンシルバッファの設定
    glEnable(GL_STENCIL_TEST);

    window.raw_worlds.emplace_back([&] {
        if (window.key(GLFW_KEY_Q)) window.close();

        // ステンシルバッファの初期化
        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);
        // ステンシルバッファの設定
        glStencilFunc(GL_ALWAYS, 0, ~0);
        glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        // 三角形の描画
        glDepthMask(GL_FALSE);
        shader.use();
        vao.bind([&] {
            glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
        });
        glDepthMask(GL_TRUE);

        // // ステンシル関数の設定
        glStencilFunc(GL_EQUAL, 0, ~0);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // 四角形の描画
        glUseProgram(0);
        glBegin(GL_QUADS);        // 四角形の描画開始
        glVertex2f(-0.9f, -0.9f); // 左下
        glVertex2f(0.9f, -0.9f);  // 右下
        glVertex2f(0.9f, 0.9f);   // 右上
        glVertex2f(-0.9f, 0.9f);  // 左上
        glEnd();                  // 四角形の描画終了

        glFlush(); // 描画命令の実行
    });

    gui.mainloop();
}
