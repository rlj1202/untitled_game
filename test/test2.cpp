#include <iostream>
#include <filesystem>

using namespace std;

class A {
public:
    A(int number) : number(number) {
        cout << "constructed : " << number << endl;
    }
    A(const A& o) {
        cout << "copied" << endl;
    }
    A(A&& o) {
        cout << "moved" << endl;
    }
    ~A() {
        cout << "destroyed" << endl;
    }

    int number = 0;
};

class Test {
public:
    Test(std::initializer_list<A> list) {
        cout << "using initializer list" << endl;
        for (auto& a : list) {
            cout << a.number << endl;
        }
    }
};

int main() {
    filesystem::path res("/workspaces/codespace/untitled_game/res");
    filesystem::path file0("textures/minecraft_atlas");
    filesystem::path file1("textures/gui");
    std::error_code error;
    
    filesystem::path full_path0 = res / file0;
    filesystem::path full_path1 = res / file1;

    cout << res << endl;
    cout << full_path0 << endl;
    cout << full_path1 << endl;
    cout << filesystem::exists(full_path0) << endl;
    cout << filesystem::exists(full_path1) << endl;
    cout << filesystem::is_regular_file(full_path0) << endl;
    cout << filesystem::is_regular_file(full_path1) << endl;
    cout << filesystem::is_directory(full_path0) << endl;
    cout << filesystem::is_directory(full_path1) << endl;


    return 0;
}