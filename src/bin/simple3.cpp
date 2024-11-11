#include <NewTexture.hpp>
#include <furitype.hpp>
#include <sumigl.hpp>

// シェーダーのソースコード
static const char *const VERTEX_SHADER_SOURCE = R"(
#version 330 core
in vec2 position;

void main()
{
    gl_Position = vec4(position, 0, 1);
}
)";

static const char *const GEOMETRY_SHADER_SOURCE = R"(
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec2 bezierPos;

void main()
{
    // 赤、緑、青の色を定義
    vec2 colors[3] = vec2[3](
        vec2(0, 0),  // 赤
        vec2(0.5, 0),  // 緑
        vec2(1, 1)   // 青
    );

    for (int i = 0; i < 3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        bezierPos = colors[i];
        EmitVertex();
    }
    EndPrimitive();
}
)";

static const char *const FRAGMENT_SHADER_SOURCE = R"(
#version 330 core
uniform bool is_bezier;
in vec2 bezierPos; // ベジェ曲線を塗りつぶすための座標情報
out vec4 FragColor;

void main()
{
    if (!is_bezier || bezierPos.x * bezierPos.x <= bezierPos.y) {
        FragColor = vec4(0, 0, 0, 0);
    } else {
        discard;
    }
}
)";

struct GlyphOutline {
    std::vector<glm::vec2> bodies; // TRIANGLE_FANで描く用のフォントの本体部
    std::vector<glm::vec2> rounds; // TRIANGLESで描く用のベジェ曲線
};

static GlyphOutline parse_glyph(const FT_Outline &outline) {
    GlyphOutline ret;

    // bodiesを生成
    auto add_body = [&](FT_Vector p) {
        ret.bodies.emplace_back(float(p.x) / 2000, float(p.y) / 2000);
    };

    // roundsを生成
    FT_Vector pa, po, pb;
    auto add_round = [&]() {
        ret.rounds.emplace_back(float(pa.x) / 2000, float(pa.y) / 2000);
        ret.rounds.emplace_back(float(po.x) / 2000, float(po.y) / 2000);
        ret.rounds.emplace_back(float(pb.x) / 2000, float(pb.y) / 2000);
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
                    add_body(mid);
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
    return ret;
}

// TODO: vaoにサイズ情報を付加
GL::Texture render_path(const GL::ProgramObject &shader, const GL::VertexArray &body_vao, size_t body_size, const GL::VertexArray &round_vao, size_t round_size) {
    // ステンシルバッファの初期化
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    // ステンシルバッファの設定
    glStencilFunc(GL_ALWAYS, 0, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    // フォント本体の描画
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDepthMask(GL_FALSE);
    shader.use();
    shader.set_uniform("is_bezier", 0);
    body_vao.bind([&] {
        glDrawArrays(GL_TRIANGLE_FAN, 0, int(body_size));
    });

    // 輪郭の丸みの描画
    shader.set_uniform("is_bezier", 1);
    round_vao.bind([&] {
        glDrawArrays(GL_TRIANGLES, 0, int(round_size));
    });
    glDepthMask(GL_TRUE);

    // ステンシル関数の設定
    glStencilFunc(GL_EQUAL, 0, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // 四角形の描画
    // glUseProgram(0);
    // glBegin(GL_QUADS);        // 四角形の描画開始
    // glVertex2f(-0.9f, -0.9f); // 左下
    // glVertex2f(0.9f, -0.9f);  // 右下
    // glVertex2f(0.9f, 0.9f);   // 右上
    // glVertex2f(-0.9f, 0.9f);  // 左上
    // glEnd();                  // 四角形の描画終了

    // glFlush(); // 描画命令の実行

    // ステンシルバッファの内容をテクスチャにコピー
    int width = 500, height = 500;
    std::vector<GLubyte> stencil_data(width * height);
    glReadPixels(0, 0, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, stencil_data.data());

    GL::Texture stencil_tex;
    stencil_tex.bind([&] {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, stencil_data.data());
    });
    return stencil_tex;
}

int main() {
    debug(GL::hi_sumigl());
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

        // outlineを使って処理を行う
        FT_Outline outline = face->glyph->outline;
        glyph_outlines[charcode] = parse_glyph(outline);
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
    debug(glyph_outlines[65].bodies);
    debug(glyph_outlines[65].rounds);

    GUI gui;
    Window &window = gui.create_window(500, 500, "魔法使いの書斎");

    // シェーダーのコンパイル
    GLuint vertex_shader = GL::create_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
    GLuint geometry_shader = GL::create_shader(GL_GEOMETRY_SHADER, GEOMETRY_SHADER_SOURCE);
    GLuint fragment_shader = GL::create_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);

    // シェーダープログラムのリンク
    GL::ProgramObject shader = {
        vertex_shader,
        geometry_shader,
        fragment_shader,
    };

    GL::ProgramObject font_ssaa_shader = {
        GL::create_shader(GL_VERTEX_SHADER, load_string("assets/shaders/fontSSAA.vsh")),
        GL::create_shader(GL_FRAGMENT_SHADER, load_string("assets/shaders/fontSSAA.fsh")),
    };

    // 三角形の頂点データ
    // auto &points = glyph_outlines[int('A')].points;
    // std::vector<glm::vec2> vertices(points.size());
    // // for (auto p : points) {
    // //     vertices
    // // }
    std::vector<glm::vec2> &vertices = glyph_outlines[80].bodies;
    std::vector<glm::vec2> &roundv = glyph_outlines[80].rounds;
    std::vector<glm::vec2> quad_vec = {{-0.9, -0.9}, {-0.9, 0.9}, {0.9, 0.9}, {0.9, -0.9}};

    GL::VertexArray vao, vao_roundv, quad_vao;
    GL::VertexBuffer vbo(vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);
    GL::VertexBuffer vbo_roundv(roundv.size() * sizeof(glm::vec2), roundv.data(), GL_STATIC_DRAW);
    GL::VertexBuffer quad_vbo(quad_vec.size() * sizeof(glm::vec2), quad_vec.data(), GL_STATIC_DRAW);

    vao.bind([&] {
        vbo.bind([&] {
            shader.set_attribute_float("position", 2, false, 0, nullptr);
        });
    });
    vao_roundv.bind([&] {
        vbo_roundv.bind([&] {
            shader.set_attribute_float("position", 2, false, 0, nullptr);
        });
    });
    quad_vao.bind([&]() {
        quad_vbo.bind([&]() {
            font_ssaa_shader.set_attribute_float("position", 2, false, 0, nullptr);
        });
    });

    // GLuint fbo;
    // glGenFramebuffers(1, &fbo);
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // // カラーバッファをテクスチャとしてアタッチ（RGBA形式のテクスチャ）
    // GLuint color_tex;
    // int width = 500, height = 500;
    // glGenTextures(1, &color_tex);
    // glBindTexture(GL_TEXTURE_2D, color_tex);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);

    // // オフセット例 (RGBに対応する横方向のサンプル位置と垂直方向のサンプル)
    // glm::vec2 offsets[6] = {
    //     glm::vec2(-0.33f, -0.5f), // Red channel, first vertical sample
    //     glm::vec2(0.0f, -0.5f),   // Green channel, first vertical sample
    //     glm::vec2(0.33f, -0.5f),  // Blue channel, first vertical sample
    //     glm::vec2(-0.33f, 0.5f),  // Red channel, second vertical sample
    //     glm::vec2(0.0f, 0.5f),    // Green channel, second vertical sample
    //     glm::vec2(0.33f, 0.5f)    // Blue channel, second vertical sample
    // };

    // ステンシルバッファの設定
    glEnable(GL_STENCIL_TEST);

    window.raw_worlds.emplace_back([&] {
        if (window.key(GLFW_KEY_Q)) window.close();

        render_path(shader, vao, vertices.size(), vao_roundv, roundv.size());
        render_path(shader, vao, vertices.size(), vao_roundv, roundv.size());
        render_path(shader, vao, vertices.size(), vao_roundv, roundv.size());
        render_path(shader, vao, vertices.size(), vao_roundv, roundv.size());
        render_path(shader, vao, vertices.size(), vao_roundv, roundv.size());
        render_path(shader, vao, vertices.size(), vao_roundv, roundv.size());
        render_path(shader, vao, vertices.size(), vao_roundv, roundv.size());

        font_ssaa_shader.use();
        quad_vao.bind([&] {
            glDrawArrays(GL_QUADS, 0, 4);
        });

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });

    gui.mainloop();
}
