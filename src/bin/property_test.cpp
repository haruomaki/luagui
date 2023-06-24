#include <GUI.hpp>
#include <PropertyTest.hpp>
#include <iostream>

using namespace std;

void dummySet(int value) {
    debug(value);
}

class AnotherClass {
    int value_ = 0;

  public:
    void setValue(int value) {
        value_ = value;
        debug(value);
    }
};

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
    void setValueDouble(double value) {
        setValue(int(value * 10));
    }
    void setValueList(initializer_list<int> list) {
        for (auto &&value : list) {
            setValue(value);
        }
    }

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
    PropertyGetSet<&ValueAndHistory::getValue, &ValueAndHistory::setValue, &ValueAndHistory::setValueDouble> value{this};
    PropertySet<&ValueAndHistory::setValue, &ValueAndHistory::setValueDouble, &ValueAndHistory::setValueList> value_setonly{this};
};

int main() {
    ValueAndHistory t;
    t.value = 7;
    int x = (t.value = 5) = 3;
    t.value = t.value + t.value();
    t.value = 2.71;

    t.value_setonly = 3.14;
    (t.value_setonly = x * 3) = {3, 6, 4};

    t.show();

    string s = (t.set_history + "aaa") + ("bbb" + t.set_history);
    cout << s << endl;
}

// class kihon {
//   public:
//     void show(int data1) {
//         cout << "data1 = " << data1 << '\n';
//     }
// };

// /********** 派生クラス **********/
// class hasei : public kihon {

//   public:
//     using kihon::show;
//     void show(int data1, int data2) {
//         cout << "data1 = " << data1 << '\n';
//         cout << "data2 = " << data2 << '\n';
//     }
// };

// int main() {

//     hasei b;
//     b.show(10);     // 基本クラスのshow関数が呼ばれる
//     b.show(10, 20); // 派生クラスのshow関数が呼ばれる
// }
