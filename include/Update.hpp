#include <GUI.hpp>

void masterUpdate();

class Update {
    std::function<void()> update_;

  public:
    virtual void update() = 0;
    Update();
    ~Update();
};
