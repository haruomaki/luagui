#include <GUI.hpp>

void masterSizeCallback(GLFWwindow *gwin, int width, int height);

class SizeCallback {
    std::function<void(int, int)> size_callback_;

  public:
    virtual void sizeCallback(int width, int height) = 0;
    SizeCallback();
    ~SizeCallback();
};
