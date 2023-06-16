#include <GUI.hpp>

GLuint createShader(GLenum shader_type, const string &source_code);

enum StorageQualifier {
    Attribute,
    Uniform,
};

class ProgramObject {
    GLuint program_id_;

  public:
    ProgramObject() = default; // FIXME: glCreateProgram()をしないコンストラクタはナンセンス
    ProgramObject(std::initializer_list<GLuint> shader_ids);
    [[nodiscard]] GLuint getPrgramId() const;
    void use() const;

    template <StorageQualifier q>
    GLint getLocation(const string &name);

    void setVertexAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
};

class VertexArrayObject {
    GLuint array_;

  public:
    VertexArrayObject() = default;

    static VertexArrayObject gen() {
        VertexArrayObject vao;
        glGenVertexArrays(1, &vao.array_); // VAOの生成
        return vao;
    }

    inline void bind(const function<void()> &proc_in_bind) const {
        glBindVertexArray(array_); // VAOをバインド
        proc_in_bind();
        glBindVertexArray(0);
    }
};
