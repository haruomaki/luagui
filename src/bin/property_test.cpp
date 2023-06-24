#include <GUI.hpp>
#include <PropertyTest.hpp>
#include <iostream>

using namespace std;

class ValueAndHistory {
    int value_ = 0;
    vector<int> history_ = {};

  public:
    [[nodiscard]] string getHistory() {
        return toStr(history_);
    }

    void setValue(int value) {
        value_ = value;
        history_.push_back(value);
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
        cout << "history: ";
        for (auto &&record : history_) {
            cout << record << " ";
        }
        cout << endl;
    }

    PropertyGet<&ValueAndHistory::getHistory> history{this};
    PropertySet<&ValueAndHistory::setValue> value{this};
};

int main() {
    ValueAndHistory t;
    t.value = 7;
    (t.value = 5) = 3;
    // t.value = 3.14;
    // t.value = {3, 6, 4};

    t.show();

    string s = (t.history + "aaa") + ("bbb" + t.history);
    cout << s << endl;
}
