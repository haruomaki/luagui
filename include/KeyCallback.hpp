#include <guibase.hpp>

void masterKeyCallback(GLFWwindow *gwin, int key, int scancode, int action, int mods);

class KeyCallback {
    std::function<void(int, int)> key_callback_;

  public:
    virtual void keyCallback(int key, int action) = 0;
    KeyCallback();
    ~KeyCallback();
};
