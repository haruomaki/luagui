#include <Property.hpp>
#include <base.hpp>

class ValueAndHistory {
    int value_ = 0;
    vector<int> set_history_ = {};

  public:
    [[nodiscard]] int getValue() const {
        return value_;
    }
    void setValue(int value) {
        value_ = value;
        set_history_.push_back(value);
    }
    void setValue(double value) {
        setValue(int(value * 10));
    }
    void setValue(initializer_list<int> list) {
        for (auto &&value : list) {
            setValue(value);
        }
    }

    void show() {
        cout << "current value: " << value_ << endl;
        cout << "history: ";
        for (auto &&record : set_history_) {
            cout << record << " ";
        }
        cout << endl;
    }

    PropertyGetSet<&ValueAndHistory::getValue, static_cast<void (ValueAndHistory::*)(int)>(&ValueAndHistory::setValue), static_cast<void (ValueAndHistory::*)(double)>(&ValueAndHistory::setValue), static_cast<void (ValueAndHistory::*)(initializer_list<int>)>(&ValueAndHistory::setValue)> value{this};
};

int main() {
    ValueAndHistory t;
    t.value.set(8);
    t.value.set(2.1);
    debug(t.value + 15);
    t.value = 7;
    debug(t.value.get());
    t.value += 5;
    (t.value += 8.8);
    // t.value + {3, 6, 4};

    string s = t.value.get() + "aaa";
    debug(s);

    t.show();
}
