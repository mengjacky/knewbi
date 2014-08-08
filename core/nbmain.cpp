#include <stdio.h>
#include "nbcore.h"
int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "lack lua main file!!\n");
        return 1;
    }
    KnewbiCore *core = KnewbiCore::Instance();

    core->Setup(argv[1]);
    return 0;
}
