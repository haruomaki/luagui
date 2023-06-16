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

struct VertexArrayObject {
    GLuint array_;

    static inline VertexArrayObject gen() {
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

template <GLenum TARGET>
struct BufferObject {
    GLuint buffer_;

    // glGenBuffers() + glBufferData()
    static inline BufferObject gen(GLsizeiptr size, const void *data, GLenum usage) {
        BufferObject xbo;
        glGenBuffers(1, &xbo.buffer_);
        glBindBuffer(TARGET, xbo.buffer_);
        glBufferData(TARGET, size, data, usage);
        glBindBuffer(TARGET, 0);
        return xbo;
    }

    inline void bind(const function<void()> &proc_in_bind) const {
        glBindBuffer(TARGET, buffer_);
        proc_in_bind();
        glBindBuffer(TARGET, 0);
    }
};

using VertexBufferObject = BufferObject<GL_ARRAY_BUFFER>;
