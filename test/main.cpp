#include <cstdio>

#include <pugixml.hpp>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

using namespace std;

int main() {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("../../res/block_types.xml");
    if (!result) {
        printf("Failed to load xml file\n");
        return -1;
    }
    
    for (pugi::xml_node block_type : doc.child("Config").child("BlockTypes").children("BlockType")) {
        printf("block type name : %s\n", block_type.child("Name").child_value());
    }

    printf("test...\n");

    return 0;
}