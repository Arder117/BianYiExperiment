#include "function.h"

int main() {
    CFG cfg;
    string filename = "D:/Projects/CLionProjects/Bianyi/experiment5/test";
    readCFG(&cfg, filename);
    printCFG(cfg);

    calculateFirst(cfg);
    printFirst();

    calculateFollow(cfg);
    printFollow();

    calculateSelect(cfg);
    printSelect();

    return 0;
}
