#include <iostream>
#include "VLangtonAnt.h"
#include "VLangtonParalela.h"
#include "MatrizContigua.h"

int main(int argc, char** argv) {
    VLangtonParalela x(50, "DDEE", argc, argv);
    x.exec();

    //VLangtonAnt x;
    //x.exec();

    //MatrizContigua<int> m(2,2);
    return 0;
}
