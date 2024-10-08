#include <FreeType.hpp>
#include <furitype.hpp>

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
#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in int c[];
out vec3 vertexColor; // フラグメントシェーダに渡す色
// out vec2 bezierPos; // 2次ベジェ曲線の座標情報

layout(std430, binding = 0) buffer ControlPointBuffer {
    float controlPoints[];
};

struct GlyphInfo {
    int offset;
    int count;
};

layout(std430, binding = 1) buffer GlyphInfoBuffer {
    GlyphInfo glyphInfos[];
};

void main()
{
    vec4 center = gl_in[0].gl_Position;

    int kind = c[0];
    int offset = glyphInfos[kind].offset;
    int count = glyphInfos[kind].count;

    // 制御点の数に基づいて頂点を生成
    for (int i = 0; i < count; ++i) {
        gl_Position = center + vec4(controlPoints[offset + i * 2], controlPoints[offset + i * 2 + 1], 0.0, 0.0);
        vertexColor = vec3(1.0, 0.3, 0.0); // 色は適宜設定
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

// 各文字のオフセットと制御点の数
struct GlyphInfo {
    int offset;
    int count;
};

int main() {
    hello_furitype();
    FreeTypeContext ft_context;
    auto *face = ft_context.load_font("assets/fonts/main.ttf");
    debug(face->num_faces);

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

    // 制御点データ
    float control_points[] = {
        // 0
        0, 0,
        0, 0.1,
        0.1, 0,
        // 1
        0.1, 0.1,
        0.2, 0.2,
        0, 0.2,
        0.1, 0.4};

    GlyphInfo glyph_infos[] = {
        {0, 3},
        {6, 4},
    };

    // SSBOにフォントデータを送る
    ShaderStorageBufferObject control_points_ssbo(sizeof(float) * 14, (float *)control_points, GL_STATIC_DRAW);
    control_points_ssbo.bind_base(0);
    ShaderStorageBufferObject glyph_info_ssbo(sizeof(GlyphInfo) * 2, (GlyphInfo *)glyph_infos, GL_STATIC_DRAW);
    glyph_info_ssbo.bind_base(1);

    // 三角形の頂点データ
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    int codes[] = {0, 0, 1};

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
