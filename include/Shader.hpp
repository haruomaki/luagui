#include <guibase.hpp>

GLuint createShader(GLenum shader_type, const string &source_code);

class ProgramObject {
    GLuint program_id_;

  public:
    ProgramObject(std::initializer_list<GLuint> shader_ids);
    [[nodiscard]] GLuint getPrgramId() const;
    void use() const;
};
