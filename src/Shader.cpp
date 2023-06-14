#include <Shader.hpp>
#include <guibase.hpp>

// シェーダ https://gist.github.com/tasonco/73a7291769278320c76928b1bd963cba
GLuint createShader() {
    // バーテックスシェーダのコンパイル
    auto vsh_id = glCreateShader(GL_VERTEX_SHADER);
    auto vsh_string = loadString("shader/shader.vsh");
    const char *vsh_char = vsh_string.c_str();
    glShaderSource(vsh_id, 1, &vsh_char, nullptr);
    glCompileShader(vsh_id);

    // フラグメントシェーダのコンパイル
    auto fsh_id = glCreateShader(GL_FRAGMENT_SHADER);
    auto fsh_string = loadString("shader/shader.fsh");
    const char *fsh_char = fsh_string.c_str();
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
