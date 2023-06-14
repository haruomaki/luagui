#include <Shader.hpp>

// シェーダ https://gist.github.com/tasonco/73a7291769278320c76928b1bd963cba
GLuint createShader() {
    // バーテックスシェーダのコンパイル
    cout << "crateShader内" << endl;
    GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);
    cout << "glCreateShader完了" << endl;
    string vertexShader = R"#(
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
    const char *vs = vertexShader.c_str();
    cout << "いくぜ！" << endl;
    glShaderSource(vShaderId, 1, &vs, NULL);
    cout << "source完了" << endl;
    glCompileShader(vShaderId);
    cout << "コンパイル完了" << endl;

    // フラグメントシェーダのコンパイル
    GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    string fragmentShader = R"#(
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
    const char *fs = fragmentShader.c_str();
    glShaderSource(fShaderId, 1, &fs, NULL);
    glCompileShader(fShaderId);

    // プログラムオブジェクトの作成
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vShaderId);
    glAttachShader(programId, fShaderId);

    // リンク
    glLinkProgram(programId);

    glUseProgram(programId);

    return programId;
}
