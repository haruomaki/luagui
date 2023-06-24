#include <GUI.hpp>
#include <PropertyTest.hpp>
#include <iostream>

using namespace std;

class ValueAndHistory {
    int value_ = 0;
    vector<int> get_history_ = {};
    vector<int> set_history_ = {};

  public:
    [[nodiscard]] int getValue() {
        get_history_.push_back(value_);
        return value_;
    }
    [[nodiscard]] string getSetHistory() {
        return toStr(set_history_);
    }

    void setValue(int value) {
        value_ = value;
        set_history_.push_back(value);
    }
    // void setValue(double value) {
    //     setValue(int(value));
    // }
    // void setValue(initializer_list<int> list) {
    //     for (auto &&value : list) {
    //         setValue(value);
    //     }
    // }

    void show() {
        cout << "current value: " << value_ << endl;
        cout << "get history: ";
        for (auto &&record : get_history_) {
            cout << record << " ";
        }
        cout << endl;
        cout << "set history: ";
        for (auto &&record : set_history_) {
            cout << record << " ";
        }
        cout << endl;
    }

    PropertyGet<&ValueAndHistory::getSetHistory> set_history{this};
    PropertyGetSet<&ValueAndHistory::getValue, &ValueAndHistory::setValue> value{this};
    // PropertySet<&ValueAndHistory::setValue> value_get_set{this};
};

int main() {
    ValueAndHistory t;
    t.value = 7;
    int x = (t.value = 5) = 3;
    t.value = t.value + t.value();
    // t.value = 3.14;
    // t.value = {3, 6, 4};

    t.show();

    string s = (t.set_history + "aaa") + ("bbb" + t.set_history);
    cout << s << endl;
}
