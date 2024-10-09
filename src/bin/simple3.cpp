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
    
    // 基準点を最初に発行
    Point p0 = glyph.points[0];
    gl_Position = center + vec4(p0.x/2000, p0.y/2000, 0.0, 1.0);
    EmitVertex();
    
    // 制御点の数に基づいて頂点を生成
    for (int i = 1; i < 15; ++i) {
        Point p = glyph.points[i];
        gl_Position = center + vec4(p.x / 2000, p.y / 2000, 0.0, 1.0);
        vertexColor = vec3(1.0, 0.3, 0.0); // 色は適宜設定
        EmitVertex();
        
        // 基準点を再度発行
        gl_Position = center + vec4(p0.x/2000, p0.y/2000, 0.0, 1.0);
        EmitVertex();
    }
    EndPrimitive();
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

    std::array<GlyphOutline, 256> buffer = {};
    for (FT_ULong charcode = 0; charcode <= 255; ++charcode) {
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

        // contoursのコピー
        std::copy(outline.contours, outline.contours + outline.n_contours, buffer[charcode].contours);

        // pointsのコピー
        for (int i = 0; i < outline.n_points; ++i) {
            buffer[charcode].points[i] = {
                .x = static_cast<float>(outline.points[i].x),
                .y = static_cast<float>(outline.points[i].y),
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
    ShaderStorageBufferObject glyph_outline_ssbo(sizeof(GlyphOutline) * 256, buffer.data(), GL_STATIC_DRAW);
    glyph_outline_ssbo.bind_base(0);

    // 三角形の頂点データ
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    int codes[] = {65, 65, 65};

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
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    window.raw_worlds.emplace_back([&] {
        if (window.key(GLFW_KEY_Q)) window.close();

        // ステンシルバッファの設定
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        // 三角形の描画
        shader.use();
        vao.bind([&] {
            glDrawArrays(GL_POINTS, 0, 3);
        });

        // ステンシルバッファの設定を変更
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
    });

    gui.mainloop();
}
