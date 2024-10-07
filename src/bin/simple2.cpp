// なぜかヘッダファイルが無くても動く！？

// シェーダーのソースコード
static const char *const VERTEX_SHADER_SOURCE = R"(
#version 330 core
in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

static const char *const GEOMETRY_SHADER_SOURCE = R"(
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 vertexColor; // フラグメントシェーダに渡す色
out vec2 bezierPos; // 2次ベジェ曲線の座標情報

void main()
{
    vec4 center = gl_in[0].gl_Position;

    // 三角形の頂点を定義
    float scale = 2;
    vec4 offset1 = vec4(0.1, 0.0, 0.0, 0.0) * scale;
    vec4 offset2 = vec4(-0.05, 0.0866, 0.0, 0.0) * scale;
    vec4 offset3 = vec4(-0.05, -0.0866, 0.0, 0.0) * scale;
    
    // 各頂点の色を設定
    vec3 color1 = vec3(1.0, 0.0, 0.0); // 赤
    vec3 color2 = vec3(0.0, 1.0, 0.0); // 緑
    vec3 color3 = vec3(0.0, 0.0, 1.0); // 青

    gl_Position = center + offset1;
    vertexColor = color1;
    bezierPos = vec2(0, 0);
    EmitVertex();

    gl_Position = center + offset2;
    vertexColor = color2;
    bezierPos = vec2(0.5, 0);
    EmitVertex();

    gl_Position = center + offset3;
    vertexColor = color3;
    bezierPos = vec2(1, 1);
    EmitVertex();

    EndPrimitive();
}
)";

static const char *const FRAGMENT_SHADER_SOURCE = R"(
#version 330 core
in vec3 vertexColor; // ジオメトリシェーダから渡される色
in vec2 bezierPos; // ベジェ曲線を塗りつぶすための座標情報
out vec4 FragColor;

void main()
{
    if (bezierPos.x * bezierPos.x <= bezierPos.y) {
        FragColor = vec4(vertexColor, 1.0);
    } else {
        discard;
    }
}
)";

int main() {
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

    // 三角形の頂点データ
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    VertexArrayObject vao;
    VertexBufferObject vbo(sizeof(vertices), (float *)vertices, GL_STATIC_DRAW);

    vao.bind([&] {
        vbo.bind([&] {
            shader.set_attribute_float("aPos", 3, false, 3 * sizeof(float), nullptr);
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
