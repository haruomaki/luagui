#include <GUI.hpp>

template <typename SelfType>
struct Base {
    SelfType *getThis() {
        return static_cast<SelfType *>(this);
    }
};

struct Derived1 : Base<Derived1> {
    int v = 0;

    void show() {
        debug(v);
    }
};

struct Derived2 : Base<Derived1> {
    int v = 0;

    void show() const {
        debug(v * 100);
    }
};

int main() {
    Derived1 d1;
    Derived2 d2;

    auto *p = d1.getThis();
    p->v = 100;
    p->show();

    auto *p2 = d2.getThis();
    p2->v = 364;
    p2->show();
}
