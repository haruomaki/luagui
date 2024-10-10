#include <furitype.hpp>

constexpr int MAX_POINTS = 1024;
constexpr int MAX_CONTOURS = 64;

// シェーダーのソースコード
static const char *const VERTEX_SHADER_SOURCE = R"(
#version 330 core
in vec3 aPos;
in int codepoint;
out int c;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    c = codepoint;
}
)";

static const char *const GEOMETRY_SHADER_SOURCE = R"(
#version 440 core

const int MAX_POINTS = 1024;
const int MAX_CONTOURS = 64;

layout (points) in;
layout (triangle_strip, max_vertices = 128) out;

in int c[];
out vec3 vertexColor; // フラグメントシェーダに渡す色
// out vec2 bezierPos; // 2次ベジェ曲線の座標情報

struct Point {
    float x, y;  // 座標
    uint tag;    // タグ情報
};

struct GlyphOutline {
    int glyph_id;      // グリフID。-1なら無効
    uint numContours;  // 輪郭の数
    uint numPoints;    // ポイントの数
    uint contours[MAX_CONTOURS]; // 輪郭の終点情報
    Point points[MAX_POINTS];
};

layout(std430, binding = 0) buffer GlyphBuffer {
    GlyphOutline glyphs[];
};

void main()
{
    vec4 center = gl_in[0].gl_Position;

    int kind = c[0];
    GlyphOutline glyph = glyphs[kind];
    
    uint start, end = -1;
    for (int ct = 0; ct < glyph.numContours; ct++) {
        uint tmp = end;
        end = glyph.contours[ct];
        start = tmp + 1;

        // 基準点を最初に発行
        Point p0 = glyph.points[start];
    
        // 制御点の数に基づいて頂点を生成
        for (uint i = start + 1; i <= end; i++) {
            Point p = glyph.points[i];
            if (p.tag == 1) {
                // 基準点を再度発行
                gl_Position = center + vec4(p0.x, p0.y, 0.0, 1.0);
                EmitVertex();
                
                gl_Position = center + vec4(p.x, p.y, 0.0, 1.0);
                vertexColor = vec3(1.0, 0.3, 0.0); // 色は適宜設定
                EmitVertex();
            }
        }

        EndPrimitive();
    }
}
)";

static const char *const FRAGMENT_SHADER_SOURCE = R"(
#version 330 core
in vec3 vertexColor; // ジオメトリシェーダから渡される色
// in vec2 bezierPos; // ベジェ曲線を塗りつぶすための座標情報
out vec4 FragColor;

void main()
{
    // if (bezierPos.x * bezierPos.x <= bezierPos.y) {
        FragColor = vec4(vertexColor, 1.0);
    // } else {
    //     discard;
    // }
}
)";

struct GlyphOutline {
    int glyph_id;                        // グリフID。-1なら無効
    unsigned int n_contours;             // 輪郭の数
    unsigned int n_points;               // ポイントの数
    unsigned int contours[MAX_CONTOURS]; // 輪郭の終点情報
    struct {
        float x, y;       // 座標
        unsigned int tag; // タグ情報
    } points[MAX_POINTS];
};

int main() {
    freetype::Context ft;
    auto *face = ft.load_font("assets/fonts/main.ttf");

    constexpr int max_charcode = 256;
    std::array<GlyphOutline, max_charcode> buffer = {};
    for (FT_ULong charcode = 0; charcode < max_charcode; ++charcode) {
        FT_UInt glyph_index = FT_Get_Char_Index(face, charcode);
        if (glyph_index == 0) {
            // グリフが存在しない場合
            buffer[charcode].glyph_id = -1;
            continue;
        }

        if (FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP) != 0) {
            // エラー処理
            buffer[charcode].glyph_id = -1;
            continue;
        }

        FT_Outline outline = face->glyph->outline;
        // outlineを使って処理を行う

        std::cout << charcode << ": ";
        for (int ct = 0; ct < outline.n_contours; ct++) {
            std::cout << outline.contours[ct] << " ";
        }
        std::cout << "\n";

        // contoursのコピー
        std::copy(outline.contours, outline.contours + outline.n_contours, buffer[charcode].contours);

        // pointsのコピー
        for (int i = 0; i < outline.n_points; ++i) {
            buffer[charcode].points[i] = {
                .x = static_cast<float>(outline.points[i].x) / 2000,
                .y = static_cast<float>(outline.points[i].y) / 2000,
                .tag = static_cast<uint8_t>(outline.tags[i])};
        }

        buffer[charcode].glyph_id = static_cast<int>(charcode);
        buffer[charcode].n_contours = outline.n_contours;
        buffer[charcode].n_points = uint(outline.n_points);
    }

    GUI gui;
    Window &window = gui.create_window(500, 500, "魔法使いの書斎");

    // シェーダーのコンパイル
    GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
    GLuint geometry_shader = create_shader(GL_GEOMETRY_SHADER, GEOMETRY_SHADER_SOURCE);
    GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);

    // シェーダープログラムのリンク
    ProgramObject shader = {
        vertex_shader,
        geometry_shader,
        fragment_shader,
    };

    // SSBOにフォントデータを送る
    ShaderStorageBufferObject glyph_outline_ssbo(sizeof(buffer), buffer.data(), GL_STATIC_DRAW);
    glyph_outline_ssbo.bind_base(0);

    // 三角形の頂点データ
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    int codes[] = {65, 66, 67};
    // int codes[] = {68, 69, 70};
    // int codes[] = {71, 72, 73};
    // int codes[] = {74, 75, 76};
    // int codes[] = {77, 78, 79};
    // int codes[] = {80, 81, 82};

    VertexArrayObject vao;
    VertexBufferObject vbo(sizeof(vertices), (float *)vertices, GL_STATIC_DRAW);
    VertexBufferObject vbo_codes(sizeof(codes), (int *)codes, GL_STATIC_DRAW);

    vao.bind([&] {
        vbo.bind([&] {
            shader.set_attribute_float("aPos", 3, false, 3 * sizeof(float), nullptr);
        });
        vbo_codes.bind([&] {
            shader.set_attribute_int("codepoint", 1, sizeof(int), nullptr);
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
            glDrawArrays(GL_POINTS, 0, 3);
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
