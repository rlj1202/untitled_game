#include <iostream>
#include <filesystem>

using namespace std;

int main() {
    filesystem::path file("./test2.js");
    std::error_code error;
    
    cout << filesystem::current_path() << endl;
    cout << filesystem::absolute(file) << endl;
    cout << filesystem::canonical(file, error) << endl;
    cout << error << endl;
    cout << filesystem::canonical("/bin") << endl;

    return 0;
}