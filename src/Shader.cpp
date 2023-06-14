#include <Shader.hpp>

// シェーダ https://gist.github.com/tasonco/73a7291769278320c76928b1bd963cba
GLuint createShader() {
    // バーテックスシェーダのコンパイル
    GLuint vsh_id = glCreateShader(GL_VERTEX_SHADER);
    string vertex_shader = R"#(
    attribute vec3 position;
    attribute vec2 uv;
    attribute vec4 color;
    varying vec2 vuv;
    varying vec4 vColor;
    void main(void){
        gl_Position = vec4(position, 1.0);
        vuv = uv;
        vColor = color;
    }
    )#";
    const char *vsh_char = vertex_shader.c_str();
    glShaderSource(vsh_id, 1, &vsh_char, nullptr);
    glCompileShader(vsh_id);

    // フラグメントシェーダのコンパイル
    GLuint fsh_id = glCreateShader(GL_FRAGMENT_SHADER);
    string fragment_shader = R"#(
    varying vec2 vuv;
    uniform sampler2D texture;
    uniform bool is_tex;
    varying vec4 vColor;
    void main(void){
        if (is_tex) {
            gl_FragColor = texture2D(texture, vuv);
        } else {
            gl_FragColor = vColor;
        }
    }
    )#";
    const char *fsh_char = fragment_shader.c_str();
    glShaderSource(fsh_id, 1, &fsh_char, nullptr);
    glCompileShader(fsh_id);

    // プログラムオブジェクトの作成
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vsh_id);
    glAttachShader(program_id, fsh_id);

    // リンク
    glLinkProgram(program_id);

    glUseProgram(program_id);

    return program_id;
}
