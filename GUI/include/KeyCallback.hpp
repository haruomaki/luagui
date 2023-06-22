#include <Window.hpp>

class KeyCallback {
    Window &window_;
    std::function<void(int, int)> key_callback_;

  public:
    virtual void keyCallback(int key, int action) = 0;
    KeyCallback(Window &window);
    ~KeyCallback();
};
