#include <iostream>
#include <filesystem>

using namespace std;

int main() {
    filesystem::path file("./test2.js");
    std::error_code error;
    
    cout << filesystem::current_path() << endl;
    cout << file << endl;
    cout << filesystem::absolute(file) << endl;
    cout << filesystem::canonical(file, error) << endl;
    cout << error << endl;
    cout << filesystem::canonical("/bin") << endl;

    {
        filesystem::path dir("/workspaces/codespace/untitled_game/build.webgl");
        filesystem::path file("./untitled_game.data");

        cout << file << endl;
        
        cout << dir / file << endl;
        cout << filesystem::canonical(dir / file, error) << endl;
        cout << error << endl;

        cout << dir / "/workspaces/" << endl;
    }

    return 0;
}