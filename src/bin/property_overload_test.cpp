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
        setValue(int(value));
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
    t.value = 7;
    t.value += 5;
    t.value = 3.14;
    t.value = {3, 6, 4};

    t.value += false;

    t.show();
}
